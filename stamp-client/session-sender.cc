// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "session-sender.h"
#include "socket.h"
#include "protocols.h"

#include <unistd.h>

SessionSender::SessionSender(uint16_t port, uint32_t count, uint32_t timeout, bool auth_mode)
: port_(port), count_(count), timeout_(timeout){
    set_auth_mode(auth_mode);
}

TestResult SessionSender::start_session(const char *addr) {
    auto address = stamp::parse_address(addr);
    address.sin_port = stamp::hnswitch<uint16_t>(port_);

    TestResult result;
    for (size_t i = 0; i < count_; ++i) {
        stamp::Bytes data;
        if (auth_mode_) {
            data = stamp::get_bytes(stamp::AuthSenderPacket(i, key_));
        } else {
            data = stamp::get_bytes(stamp::UnauthSenderPacket(i));
        }

        stamp::send_packet(address, data, socket_);

        // 丢包数 = 原丢包率 * 原总包数
        auto packet_lose_num = result.packet_lose * result.time.size();

        try {
            auto recv_data = stamp::receive_packet(socket_, timeout_);
            recv_data.show();
            result.time.push_back(time_diff(recv_data));
        } catch (stamp::recv_timeout &e) {
            result.time.push_back(0);
            ++packet_lose_num;
        }
        result.packet_lose = packet_lose_num / result.time.size();
    }
    return result;
}

bool SessionSender::auth_mode() const {
    return auth_mode_;
}

void SessionSender::set_auth_mode(bool option) {
    auth_mode_ = option;
    if (auth_mode_) {
        char *passwd = getpass("Enter password to authenticate packet: ");
        size_t n = 0;
        while (passwd[n++]);
        --n;
        key_ = stamp::Bytes(passwd, n);
        key_.show(0);
    }
}

uint64_t SessionSender::time_diff(const stamp::Bytes &receive_data) {
    if (auth_mode_) {
        stamp::AuthReflectorPacket packet =
                *reinterpret_cast<const stamp::AuthReflectorPacket*>(receive_data.get());
        return stamp::hnswitch(packet.receive_timestamp)
             - stamp::hnswitch(packet.sender_timestamp);
    }
    stamp::UnauthReflectorPacket packet =
            *reinterpret_cast<const stamp::UnauthReflectorPacket*>(receive_data.get());
    return stamp::hnswitch(packet.receive_timestamp)
         - stamp::hnswitch(packet.sender_timestamp);
}
