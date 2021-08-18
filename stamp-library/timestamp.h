// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)
#ifndef STAMP_TIMESTAMP_H
#define STAMP_TIMESTAMP_H

#include "bytes.h"

#include <iostream>

namespace stamp {
    // 对TIME_STAMP格式进行设定
    enum TIME_STAMP_TYPE {NTP, PTP};
    extern TIME_STAMP_TYPE timestamp_type;

    // 时间戳结构
    struct Timestamp {
        uint32_t sec = 0;
        uint32_t nsec = 0;
    };


    // 获取当前时间的对应格式时间戳
    Timestamp get_timestamp();

    // 计算两个时间戳之间的时间差, 返回纳秒数。
    int64_t operator-(const Timestamp &begin, const Timestamp &end);

    template<>
    Timestamp hnswitch(const Timestamp& ts);
}

#endif //STAMP_TIMESTAMP_H
