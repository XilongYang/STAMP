// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)
#ifndef STAMP_TIMESTAMP_H
#define STAMP_TIMESTAMP_H

#include "bytes.h"

namespace stamp {
    enum TIME_STAMP {NTP, PTP};
    // 获取当前时间的对应格式时间戳
    // 支持NTP和PTP两种格式
    Bytes get_timestamp(TIME_STAMP type = NTP);
}

#endif //STAMP_TIMESTAMP_H
