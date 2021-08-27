// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)
#ifndef STAMP_SESSION_REFLECTOR_H
#define STAMP_SESSION_REFLECTOR_H

#include "socket.h"
#include "bytes.h"

class SessionReflector {
public:
    enum {STAMP_PORT = 862};
    explicit SessionReflector(uint16_t port, bool state_mode = false, bool auth_mode = false);

    void start_session();

    bool auth_mode() const;
    void set_auth_mode(bool auth_mode);

    uint16_t port_;
    bool state_mode_;
private:
    bool auth_mode_;
    stamp::UdpSocket socket_;
    stamp::Bytes key_;

    uint32_t seq_number_;
};

#endif //STAMP_SESSION_REFLECTOR_H
