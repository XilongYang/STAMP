// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)
#ifndef STAMP_SOCKET_H
#define STAMP_SOCKET_H

#include "bytes.h"

#include <netinet/in.h>
#include <stdexcept>

namespace stamp {
    // 对UDP socket进行简单封装，以管理生命周期。
    // 不支持拷贝与移动操作。
    class UdpSocket {
    public:
        UdpSocket();
        ~UdpSocket();
        UdpSocket(const UdpSocket&) = delete;
        UdpSocket(UdpSocket&&) = delete;
        int get() const;
    private:
        int socket_;
    };

    // socket相关异常类

    // 查询地址信息失败
    class addrinfo_error : public std::runtime_error {
    public:
        explicit addrinfo_error(const std::string &s) : std::runtime_error(s) {}
    };

    // 发送失败
    class send_error : public std::runtime_error {
    public:
        explicit send_error(const std::string &s) : std::runtime_error(s) {}
    };

    // 绑定失败
    class bind_error : public std::runtime_error {
    public:
        explicit bind_error(const std::string &s) : std::runtime_error(s) {}
    };

    // 接收超时
    class recv_timeout : public std::runtime_error {
    public:
        explicit recv_timeout(const std::string &s) : std::runtime_error(s) {}
    };

    // 接收错误
    class recv_error : public std::runtime_error {
    public:
        explicit recv_error(const std::string &s) : std::runtime_error(s) {}
    };

    // 试图从字符串中解析出地址，失败时抛出异常。
    sockaddr_in parse_address(const char* address);

    // 使用send_socket向指定地址发送data, 失败时抛出异常。
    void send_packet(sockaddr_in address, const Bytes& data, const UdpSocket &send_socket);

    // 将socket与本机指定端口绑定。
    void bind(const UdpSocket &bind_socket, uint16_t port);

    // 使用recv_socket接受port端口收到的包，
    // 使用参数timeout设置超时等待时间，单位为秒，如果timeout为0则不会超时，
    // 使用参数address保存源地址,
    // 使用参数ttl保存接收到的packet的ip头部分的ttl。
    Bytes receive_packet(const UdpSocket &recv_socket, uint32_t timeout = 0
                         , sockaddr_in *address = nullptr, uint8_t *ttl = nullptr);
}


#endif //STAMP_SOCKET_H
