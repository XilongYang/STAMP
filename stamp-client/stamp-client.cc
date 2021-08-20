// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "socket.h"
#include "protocols.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::stoi;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage stamp-client ipv4-addr port" << endl;
        return -1;
    }

    auto addr = stamp::parse_address(argv[1]);
    addr.sin_port = stamp::hnswitch<uint16_t>(20223);

    char buf[INET_ADDRSTRLEN];
    auto sock = stamp::UdpSocket();
    auto data = stamp::get_bytes(stamp::UnauthSenderPacket(0));

    stamp::send_packet(addr, data, sock);

    auto recv_data = stamp::receive_packet(sock);
    recv_data.show();
    return 0;
}