// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "udp_socket.h"

#include <unistd.h>
#include <stdexcept>

#ifdef __linux__
static constexpr unsigned recv_type = IP_RECVTTL;
#elifdef __unix__
static constexpr unsigned recv_type = IP_TTL;
#endif

namespace stamp{
    UdpSocket::UdpSocket()
    : socket_(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {}

    UdpSocket::~UdpSocket() {
        close(socket_);
    }

    // 重构成两个函数
    void UdpSocket::sendto(const Bytes &bytes, const char *address, uint16_t port) {
        sockaddr addr = get_addr(address, port);

        if (-1 == ::sendto(socket_, bytes.get(), bytes.size()
                           , 0, &addr, sizeof(addr))) {
            throw std::runtime_error("Error while send message.");
        }
    }

    Bytes UdpSocket::recv(uint8_t &ttl, sockaddr &from) {
        int yes = 1;
        setsockopt(socket_, IPPROTO_IP, IP_RECVTTL, &yes, sizeof(yes));

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(862);
        if (-1 == bind(socket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))) {
            throw std::runtime_error("Error while bind address.");
        }

        uint8_t buf[1500];
        iovec iov[1] = {{buf, sizeof(buf)}};

        uint8_t ctrlDataBuf[CMSG_SPACE(sizeof(uint8_t))];

        sockaddr_storage src_addr;

        msghdr hdr = {
             .msg_name = &src_addr,
             .msg_namelen = sizeof(src_addr),
             .msg_iov = iov,
             .msg_iovlen = 1,
             .msg_control = ctrlDataBuf,
             .msg_controllen = sizeof(ctrlDataBuf)
        };

        size_t receive_size;
        if ((receive_size = recvmsg(socket_, &hdr, 0)) == -1) {
            throw std::runtime_error("Error while receiving message.");
        }

        cmsghdr *cmsg = CMSG_FIRSTHDR(&hdr);
        for (; cmsg; CMSG_NXTHDR(&hdr, cmsg)) {
            if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == recv_type) {
                ttl = *reinterpret_cast<uint8_t *>(CMSG_DATA(cmsg));
                break;
            }
        }

        from = *reinterpret_cast<sockaddr*>(&src_addr);

        return Bytes(receive_size, buf, receive_size);
    }

    sockaddr UdpSocket::get_addr(const char *address, uint16_t port) {
        uint32_t addr;
        inet_pton(AF_INET, address, reinterpret_cast<void*>(&addr));

        sockaddr_in socket_addr;
        socket_addr.sin_family = AF_INET;
        socket_addr.sin_addr = {addr};
        socket_addr.sin_port = hnswitch(port);

        return *reinterpret_cast<sockaddr*>(&socket_addr);
    }
}
