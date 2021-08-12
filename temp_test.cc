// Copyright (c) 2021 Xilong Yang
// This code is licensed under MIT license (see LICENSE.txt for details)

#include <iostream>
#include "protocols.h"

using std::cout;
using std::endl;

int main() {
    stamp::Bytes key("123", 3);
    stamp::Timestamp ts = stamp::hnswitch(stamp::get_timestamp());

    auto usp = stamp::UnauthSenderPacket(0);
    stamp::get_bytes(usp).show();

    auto urp = stamp::UnauthReflectorPacket(0, ts, 64
                                            , stamp::get_bytes(usp));
    stamp::get_bytes(urp).show();

    auto asp = stamp::AuthSenderPacket(0, key);
    stamp::get_bytes(asp).show();

    auto arp = stamp::AuthReflectorPacket(0, ts, 64
                                          , stamp::get_bytes(asp), key);
    stamp::get_bytes(arp).show();

    return 0;
}