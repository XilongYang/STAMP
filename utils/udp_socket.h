// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)
#ifndef STAMP_UDP_SOCKET_H
#define STAMP_UDP_SOCKET_H

#include "bytes.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace stamp {
    class UdpSocket {
     public:
        UdpSocket();
        ~UdpSocket();
        UdpSocket(const UdpSocket&) = delete;
        UdpSocket(UdpSocket&&) = delete;
        void sendto(const Bytes& bytes, const char* address, uint16_t port);
        Bytes recv(uint8_t &ttl, sockaddr &from);
     private:
        int socket_ = 0;

        sockaddr get_addr(const char* address, uint16_t port);
    };
}


#endif //STAMP_UDP_SOCKET_H
