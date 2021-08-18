// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "timestamp.h"
#include "sys/time.h"

#include <ctime>

namespace stamp {
    TIME_STAMP_TYPE timestamp_type = NTP;
    // NTP时间由1970年起始，而linux时间由1900年起始
    // 故需补偿从1900到1970的时间差值2208988800(0x83aa7e80)s.
    constexpr uint32_t OFFSET_1970 = 0x83aa7e80;

    // NTP时间对小数部分的转换所需常数，值为2^32
    // NTP小数部分算法为: ns / 1e9 * FRAC,
    // 即使用小数部分数值/小数部分最大值=小数部分(单位：秒）来存储小数。
    constexpr uint64_t FRAC = 4294967296;

    Timestamp get_timestamp() {
        timespec tn;
        clock_gettime(CLOCK_REALTIME, &tn);
        if (timestamp_type == NTP) {
            // NTP整数秒进行偏差补正
            tn.tv_sec -= OFFSET_1970;
            // NTP小数秒进行转换
            double tmp = static_cast<double>(tn.tv_nsec) * 1e-9;
            tn.tv_nsec = tmp * FRAC;
        }
        Timestamp ts;
        ts.sec = tn.tv_sec;
        ts.nsec = tn.tv_nsec;
        return ts;
    }

    int64_t operator-(const Timestamp &end, const Timestamp &begin) {
        int64_t s = static_cast<int64_t>(end.sec) - static_cast<int64_t>(begin.sec);
        int64_t ns = static_cast<int64_t>(end.nsec) - static_cast<int64_t>(begin.nsec);
        if (timestamp_type == NTP) {
            double p = static_cast<double>(ns) / FRAC;
            ns = p * 1e9;
        }
        return s * 1e9 + ns;
    }

    template<>
    Timestamp hnswitch(const Timestamp& ts) {
        return {hnswitch(ts.sec), hnswitch(ts.nsec)};
    }
}
