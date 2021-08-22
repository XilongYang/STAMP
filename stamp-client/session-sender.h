// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)
#ifndef STAMP_SESSIONSENDER_H
#define STAMP_SESSIONSENDER_H

#include "bytes.h"
#include "socket.h"

#include <cstdint>
#include <utility>
#include <vector>

struct TestResult {
    std::vector<uint64_t> time;
    double   packet_lose = 0;
};

class SessionSender {
public:
    enum {STAMP_PORT = 862};
    explicit SessionSender(uint16_t port = STAMP_PORT, uint32_t count = 1
            , uint32_t timeout = 5, bool auth_mode = false);

    TestResult start_session(const char* addr);

    bool auth_mode() const;
    void set_auth_mode(bool option);

    uint16_t port_;
    uint32_t count_;
    uint32_t timeout_;
private:
    bool     auth_mode_;
    stamp::UdpSocket socket_;
    stamp::Bytes key_;

    uint64_t time_diff(const stamp::Bytes& data);
};

#endif //STAMP_SESSIONSENDER_H
