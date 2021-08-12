// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "socket.h"
#include "protocols.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage stamp-client ipv4-addr port" << endl;
        return -1;
    }

    auto re = stamp::send_packet(argv[1], std::stoul(argv[2])
                                 , []() {
        return stamp::get_bytes(stamp::UnauthSenderPacket(0));
    });

    re.show();
    return 0;
}