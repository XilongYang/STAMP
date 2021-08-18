// Copyright (c) 2021 Xilong Yang
// This code is licensed under MIT license (see LICENSE.txt for details) 

#ifndef HMAC_H
#define HMAC_H

#include "bytes.h"
#include <functional>

namespace stamp {
    // 进行sha256加密，返回加密结果。
    Bytes sha256(const Bytes &message);

    // 进行HMAC加密，返回加密结果。
    // message: 待加密数据。
    // key_raw: 密钥
    // func:    一个接受const Bytes&返回Bytes的可调用对象。
    Bytes hmac(const Bytes &message, const Bytes &key_raw, const std::function<Bytes(const Bytes&)> &func);
}

#endif