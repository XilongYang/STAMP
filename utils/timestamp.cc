// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "timestamp.h"
#include "sys/time.h"
#include "unistd.h"

#include <ctime>

namespace stamp {
    // NTP时间由1970年起始，而linux时间由1900年起始
    // 故需补偿从1900到1970的时间差值2208988800(0x83aa7e80)s.
    constexpr uint32_t OFFSET_1970 = 0x83aa7e80;

    // NTP时间对小数部分的转换所需常数，值为2^32.
    constexpr uint32_t FRAC = 4294967296;

    Bytes get_timestamp(TIME_STAMP type) {
        timespec tn;
        clock_gettime(CLOCK_REALTIME, &tn);
        if (type == NTP) {
            // NTP整数秒进行偏差补正
            tn.tv_sec -= OFFSET_1970;
            // NTP小数秒进行转换
            double tmp = static_cast<double>(tn.tv_nsec) * 1e-9;
            tn.tv_nsec = tmp * FRAC;
        }
        // 进行端序转换, 生成Bytes返回
        tn.tv_sec = hnswitch<uint32_t>(tn.tv_sec);
        tn.tv_nsec = hnswitch<uint32_t>(tn.tv_nsec);
        Bytes timestamp(8);
        auto pos = timestamp.modify(0, sizeof(uint32_t), get_bytes(tn.tv_sec));
        timestamp.modify(pos, sizeof(uint32_t), get_bytes(tn.tv_nsec));
        return timestamp;
    }
}
