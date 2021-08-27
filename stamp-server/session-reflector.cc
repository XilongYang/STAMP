// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "session-reflector.h"
#include "timestamp.h"
#include "protocols.h"

#include <unistd.h>

SessionReflector::SessionReflector(uint16_t port, bool state_mode, bool auth_mode)
: port_(port), state_mode_(state_mode), seq_number_(0) {
    set_auth_mode(auth_mode);
}

void SessionReflector::start_session() {
    stamp::bind(socket_, port_);
    while(seq_number_ < ~(static_cast<uint32_t>(0))) {
        sockaddr_in source_addr;
        uint8_t ttl;
        auto receive_byte = stamp::receive_packet(socket_, 0, &source_addr, &ttl);
        receive_byte.show();
        auto recv_timestamp = stamp::hnswitch(stamp::get_timestamp());
        stamp::Bytes data;
        if (state_mode_) {
            if (auth_mode_) {
                data = stamp::get_bytes(stamp::AuthReflectorPacket(seq_number_
                        , recv_timestamp, ttl, receive_byte, key_));
            } else {
                data = stamp::get_bytes(stamp::UnauthReflectorPacket(seq_number_
                        , recv_timestamp, ttl, receive_byte));
            }
            ++seq_number_;
        } else {
            uint32_t recv_seq = stamp::hnswitch(*reinterpret_cast<const uint32_t*>
                    (receive_byte.get()));
            if (auth_mode_) {
                data = stamp::get_bytes(stamp::AuthReflectorPacket(recv_seq
                        , recv_timestamp, ttl, receive_byte, key_));
            } else {
                data = stamp::get_bytes(stamp::UnauthReflectorPacket(recv_seq
                        , recv_timestamp, ttl, receive_byte));
            }
        }
        stamp::send_packet(source_addr, data, socket_);
    }
    throw std::overflow_error("Reflector overflow");
}

bool SessionReflector::auth_mode() const {
    return auth_mode_;
}

void SessionReflector::set_auth_mode(bool auth_mode) {
    auth_mode_ = auth_mode;
    if (auth_mode_) {
        char *passwd = getpass("Enter password to authenticate packet: ");
        size_t n = 0;
        while (passwd[n++]);
        --n;
        key_ = stamp::Bytes(passwd, n);
        key_.show(0);
    }
}
