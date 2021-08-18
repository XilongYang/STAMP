// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)
#ifndef STAMP_PROTOCOLS_H
#define STAMP_PROTOCOLS_H

#include "bytes.h"
#include "timestamp.h"

namespace stamp {
    extern bool auth_mode;

    struct UnauthSenderPacket {
        UnauthSenderPacket(uint32_t seq_num);
        uint32_t sequence_number = 0;
        Timestamp timestamp;
        uint16_t error_estimate = 0;
        uint8_t MBZ[30] = {0};
    };

    struct AuthSenderPacket{
        AuthSenderPacket(uint32_t seq_num, const Bytes& hmac_key);
        uint32_t sequence_number = 0;
        uint8_t MBZ1[12] = {0};
        Timestamp timestamp;
        uint16_t error_estimate = 0;
        uint8_t MBZ[70] = {0};
        uint8_t HMAC[16] = {0};
    };

    struct UnauthReflectorPacket{
        UnauthReflectorPacket(uint32_t seq_num, Timestamp receive_ts, uint8_t ttl
                              , const Bytes& receive_packet);
        uint32_t sequence_number = 0;
        Timestamp timestamp;
        uint16_t  error_estimate = 0;
        uint8_t MBZ1[2] = {0};
        Timestamp receive_timestamp;
        uint32_t sender_sequence_number = 0;
        Timestamp sender_timestamp;
        uint16_t  sender_error_estimate = 0;
        uint8_t  MBZ2[2] = {0};
        uint8_t  sender_ttl = 0;
        uint8_t  MBZ3[3] = {0};
    };

    struct AuthReflectorPacket{
        AuthReflectorPacket(uint32_t seq_num, Timestamp receive_ts, uint8_t ttl
                            , const Bytes& receive_packet, const Bytes& hmac_key);
        uint32_t sequence_number = 0;
        uint8_t MBZ1[12] = {0};
        Timestamp timestamp;
        uint16_t  error_estimate = 0;
        uint8_t MBZ2[6] = {0};
        Timestamp receive_timestamp;
        uint8_t MBZ3[8] = {0};
        uint32_t sender_sequence_number = 0;
        uint8_t MBZ4[12] = {0};
        Timestamp sender_timestamp;
        uint16_t  sender_error_estimate = 0;
        uint8_t  MBZ5[6] = {0};
        uint8_t  sender_ttl = 0;
        uint8_t  MBZ6[15] = {0};
        uint8_t  HMAC[16] = {0};
    };
}

#endif //STAMP_PROTOCOLS_H
