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
    while (true) {
        auto re = stamp::receive_packet(std::stoi(argv[1]), [](uint32_t ttl, const stamp::Bytes& receive_packet
                , const stamp::Timestamp &receive_timestamp){
            return stamp::get_bytes(stamp::UnauthReflectorPacket(0, receive_timestamp, ttl, receive_packet));
        });
        re.show();
    }
    return 0;
}
