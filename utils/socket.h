// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)
#ifndef STAMP_SOCKET_H
#define STAMP_SOCKET_H

#include "bytes.h"
#include "timestamp.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <functional>

namespace stamp {
    // 将返回Bytes的可调用对象作为包生成器类型。
    using SendPacketGenerator = std::function<Bytes()>;
    using RecvPacketGenerator = std::function<Bytes(uint8_t ttl, const Bytes &receive_packet
                                                    , const Timestamp &receive_timestamp)>;

    // 向指定地址发送func生成的包类型，并从该地址接收到返回包。
    Bytes send_packet(const char* address, uint16_t port
                      , const SendPacketGenerator &packet_generator);

    // 接受port端口收到的包，并使用func生成一个包返回。
    Bytes receive_packet(uint16_t port, const RecvPacketGenerator &packet_generator);
}


#endif //STAMP_SOCKET_H
