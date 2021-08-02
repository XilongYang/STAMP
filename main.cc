// Copyright (c) 2021 Xilong Yang
// This code is licensed under MIT license (see LICENSE.txt for details)

#include <iostream>
#include "hmac.h"

int main() {
    stamp::Bytes msg("Hello");
    stamp::Bytes key("abc");
    auto hm = stamp::hmac(msg, key, stamp::sha256);
    hm.show(4);
    return 0;
}