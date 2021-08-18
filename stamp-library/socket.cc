// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "socket.h"
#include "timestamp.h"

#include <unistd.h>
#include <netdb.h>
#include <stdexcept>
#include <string>

#ifdef __linux__
static constexpr unsigned recv_type = IP_TTL;
#else
static constexpr unsigned recv_type = IP_RECVTTL;
#endif

namespace stamp{
    namespace {
        // 对socket进行简单的封装，以管理生命周期。
        class UdpSocket {
        public:
            UdpSocket() : socket_(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {}
            ~UdpSocket() {
                close(socket_);
            }
            UdpSocket(const UdpSocket&) = delete;
            UdpSocket(UdpSocket&&) = delete;
            int get() const {
                return socket_;
            }
        private:
            int socket_;
        };

        // 试图从字符串中解析出地址，失败时抛出异常
        sockaddr_in parse_address(const char* address) {
            addrinfo hint{};
            hint.ai_flags = AI_CANONNAME;

            int err;
            addrinfo *ailist;
            if ((err = getaddrinfo(address, nullptr, &hint, &ailist)) != 0) {
                throw std::runtime_error(gai_strerror(err));
            }

            for (addrinfo *aip = ailist; aip != nullptr; aip = aip->ai_next) {
                if (aip->ai_family == AF_INET) {
                    sockaddr_in *sinp = (sockaddr_in*)aip->ai_addr;
                    return *sinp;
                }
            }
            throw std::runtime_error("No such address.");
        }
    }

    Bytes send_packet(const char* address, uint16_t port
                      , const SendPacketGenerator &packet_generator) {
        UdpSocket send_socket;

        sockaddr_in addr = parse_address(address);
        addr.sin_port = hnswitch(port);

        Bytes send_packet = packet_generator();
        if (-1 == sendto(send_socket.get(), send_packet.get(), send_packet.size()
                         , 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))) {
            throw std::runtime_error("Error while send message.");
        }

        uint8_t buf[1500];
        size_t  recv_size;
        if (-1 == (recv_size = recvfrom(send_socket.get(), buf, 1500, 0, nullptr, nullptr))) {
            throw std::runtime_error("Error while receive return message.");
        }
        alarm(10);

        return Bytes(recv_size, buf, recv_size);
    }

    Bytes receive_packet(uint16_t port, const RecvPacketGenerator &packet_generator) {
        UdpSocket recv_socket;
        int yes = 1;
        setsockopt(recv_socket.get(), IPPROTO_IP, IP_RECVTTL, &yes, sizeof(yes));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = hnswitch(INADDR_ANY);
        addr.sin_port = hnswitch(port);

        if (-1 == bind(recv_socket.get(), reinterpret_cast<sockaddr*>(&addr)
                       , sizeof(addr))) {
            throw std::runtime_error("Error while binding address");
        }

        uint8_t buf[1500];
        iovec iov[1] = {{buf, sizeof(buf)}};

        uint8_t ctrlDataBuf[CMSG_SPACE(sizeof(uint8_t))];

        sockaddr_storage src_addr{};

        msghdr hdr = {
                .msg_name = &src_addr,
                .msg_namelen = sizeof(src_addr),
                .msg_iov = iov,
                .msg_iovlen = 1,
                .msg_control = ctrlDataBuf,
                .msg_controllen = sizeof(ctrlDataBuf)
        };

        size_t receive_size;
        if (-1 == (receive_size = recvmsg(recv_socket.get(), &hdr, 0))) {
            throw std::runtime_error("Error while receiving message.");
        }

        auto receive_timestamp = hnswitch(get_timestamp());

        uint8_t ttl = -1;
        cmsghdr *cmsg = CMSG_FIRSTHDR(&hdr);
        for (; cmsg; CMSG_NXTHDR(&hdr, cmsg)) {
            if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == recv_type) {
                ttl = *reinterpret_cast<uint8_t *>(CMSG_DATA(cmsg));
                break;
            }
        }

        Bytes received_packet(receive_size, buf, receive_size);

        sockaddr from = *reinterpret_cast<sockaddr*>(&src_addr);

        Bytes return_packet = packet_generator(ttl, received_packet
                                               , receive_timestamp);
        if (-1 == (sendto(recv_socket.get(), return_packet.get()
                          , return_packet.size(), 0, &from, sizeof(from)))) {
            throw std::runtime_error("Error while sending return packet.");
        }

        return received_packet;
    }
}
