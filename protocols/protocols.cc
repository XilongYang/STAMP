// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "protocols.h"
#include "hmac.h"
#include <cstring>

namespace stamp {
    // 常量定义
    bool auth_mode = true;

    namespace {
        // Error Estimate
        // S = 0, Z = (NTP) ? 0 : 1;
        // Scale = 0, Multiplier = 1;
        uint16_t get_error_estimate() {
            uint16_t error_estimate = 1;
            if (timestamp_type == PTP) {
                error_estimate |= 1 << 14;
            }
            return error_estimate;
        }

        // HMAC-SHA256加密算法，计算src前96个字符的散列值，并将这个值的前16Byte写入dest中。
        void hmac_sha256(byte_t* dest, byte_t* src, const Bytes &key) {
            auto hmac_bytes = hmac(Bytes(96, src, 96)
                    , key, sha256);
            memcpy(dest, hmac_bytes.get(), 16);
        }
    }

    UnauthSenderPacket::UnauthSenderPacket(uint32_t seq_num) {
        sequence_number = hnswitch(seq_num);
        timestamp = hnswitch(get_timestamp());
        error_estimate = hnswitch(get_error_estimate());
    }

    AuthSenderPacket::AuthSenderPacket(uint32_t seq_num, const Bytes &hmac_key) {
        sequence_number = hnswitch(seq_num);
        timestamp = hnswitch(get_timestamp());
        error_estimate = hnswitch(get_error_estimate());
        hmac_sha256(HMAC, reinterpret_cast<byte_t*>(this), hmac_key);
    }

    UnauthReflectorPacket::UnauthReflectorPacket(uint32_t seq_num, Timestamp receive_ts, uint8_t ttl,
                                                 const Bytes &receive_packet) {
        sequence_number = hnswitch(seq_num);
        timestamp = hnswitch(get_timestamp());
        error_estimate = hnswitch(get_error_estimate());
        receive_timestamp = receive_ts;

        UnauthSenderPacket received = *reinterpret_cast<const UnauthSenderPacket*>
                (receive_packet.get());
        sender_sequence_number = received.sequence_number;
        sender_timestamp = received.timestamp;
        sender_error_estimate = received.error_estimate;
        sender_ttl = ttl;
    }

    AuthReflectorPacket::AuthReflectorPacket(uint32_t seq_num, Timestamp receive_ts, uint8_t ttl,
                                             const Bytes &receive_packet, const Bytes &hmac_key) {
        sequence_number = hnswitch(seq_num);
        timestamp = hnswitch(get_timestamp());
        error_estimate = hnswitch(get_error_estimate());
        receive_timestamp = receive_ts;

        AuthSenderPacket received = *reinterpret_cast<const AuthSenderPacket*>
                (receive_packet.get());
        sender_sequence_number = received.sequence_number;
        sender_timestamp = received.timestamp;
        sender_error_estimate = received.error_estimate;
        sender_ttl = ttl;
        hmac_sha256(HMAC, reinterpret_cast<byte_t*>(this), hmac_key);
    }
}
