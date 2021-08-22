// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "socket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <csignal>

// 表示TTL的常量, linux使用的是IP_TTL, 而其它Unix使用的是IP_RECVTTL。
#ifdef __linux__
static constexpr unsigned recv_type = IP_TTL;
#else
static constexpr unsigned recv_type = IP_RECVTTL;
#endif

namespace stamp{
    UdpSocket::UdpSocket() : socket_(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {}

    UdpSocket::~UdpSocket() {
        close(socket_);
    }

    int UdpSocket::get() const {
        return socket_;
    }

    // 试图从字符串中解析出地址，失败时抛出异常
    sockaddr_in parse_address(const char* address) {
        addrinfo hint{};
        hint.ai_flags = AI_CANONNAME;

        int err;
        addrinfo *ailist;
        if ((err = getaddrinfo(address, nullptr, &hint, &ailist)) != 0) {
            throw addrinfo_error(gai_strerror(err));
        }

        for (addrinfo *aip = ailist; aip != nullptr; aip = aip->ai_next) {
            if (aip->ai_family == AF_INET) {
                auto *sinp = reinterpret_cast<sockaddr_in*>(aip->ai_addr);
                return *sinp;
            }
        }
        throw addrinfo_error("No such address.");
    }

    void send_packet(sockaddr_in address, const Bytes& data, const UdpSocket &send_socket) {
        if (-1 == sendto(send_socket.get(), data.get(), data.size(), 0
                         , reinterpret_cast<sockaddr*>(&address), sizeof(address))) {
            throw send_error("Error while send message.");
        }
    }

    void bind(const UdpSocket &bind_socket, uint16_t port) {
        // 生成一个表示本机所有设备的指定端口的地址。
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = hnswitch(INADDR_ANY);
        addr.sin_port = hnswitch(port);

        // 将socket与指定地址绑定。
        if (-1 == bind(bind_socket.get(), reinterpret_cast<sockaddr*>(&addr)
                , sizeof(addr))) {
            throw bind_error("Error while binding address");
        }
    }

    namespace {
        // 处理计时器信号，抛出异常
        void timeout_signal(int) {
            throw recv_timeout("Timeout");
        }
    }

    Bytes receive_packet(const UdpSocket &recv_socket, uint32_t timeout
                         ,sockaddr_in* address, uint8_t *ttl) {
        // 打开接收ttl选项。
        int yes = 1;
        setsockopt(recv_socket.get(), IPPROTO_IP, IP_RECVTTL, &yes, sizeof(yes));

        // 使用iov接收数据。
        uint8_t buf[1500];
        iovec iov[1] = {{buf, sizeof(buf)}};

        // 使用ctrl_data_buf接收TTL数据。
        uint8_t ctrl_data_buf[CMSG_SPACE(sizeof(uint8_t))];

        // 使用src_addr接收源地址信息。
        sockaddr_storage src_addr{};

        // 将上面三个变量保存在msghdr结构中。
        msghdr hdr = {
                .msg_name = &src_addr,
                .msg_namelen = sizeof(src_addr),
                .msg_iov = iov,
                .msg_iovlen = 1,
                .msg_control = ctrl_data_buf,
                .msg_controllen = sizeof(ctrl_data_buf)
        };

        // 设置超时信号捕捉
        signal(SIGALRM, timeout_signal);
        alarm(timeout);

        // 使用recvmsg接收数据，并保存在hdr中。
        size_t receive_size;
        if (-1 == (receive_size = recvmsg(recv_socket.get(), &hdr, 0))) {
            // 清空计时器
            alarm(0);
            throw recv_error("Error while receiving message.");
        }

        // 清空计时器
        alarm(0);

        // 保存源地址信息
        if (address != nullptr) {
            *address = *reinterpret_cast<sockaddr_in*>(&src_addr);
        }

        // 保存TTL信息
        if (ttl != nullptr) {
            *ttl = -1;
            cmsghdr *cmsg = CMSG_FIRSTHDR(&hdr);
            for (; cmsg; CMSG_NXTHDR(&hdr, cmsg)) {
                if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == recv_type) {
                    *ttl = *reinterpret_cast<uint8_t *>(CMSG_DATA(cmsg));
                    break;
                }
            }
        }

        return Bytes(receive_size, buf, receive_size);
    }
}
