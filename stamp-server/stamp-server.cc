// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "socket.h"
#include "protocols.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: stamp-server [port]" << endl;
        return -1;
    }
    stamp::UdpSocket sock;
    stamp::bind(sock, 20223);
    for(;;) {
        sockaddr_in addr;
        uint8_t ttl;
        auto recv = stamp::receive_packet(sock, &addr, &ttl);
        recv.show();
        auto recv_timestamp = stamp::hnswitch(stamp::get_timestamp());
        auto data = stamp::get_bytes(stamp::UnauthReflectorPacket(0, recv_timestamp, ttl, recv));
        stamp::send_packet(addr, data, sock);
    }
    return 0;
}
