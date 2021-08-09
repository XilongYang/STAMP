// Copyright (c) 2021 Xilong Yang
// This code is licensed under MIT license (see LICENSE.txt for details)

#ifndef BYTES_H
#define BYTES_H

#include <cstdint>
#include <cstddef>

namespace stamp {
    typedef unsigned char byte_t;
    using std::size_t;

    class Bytes {
     public:
        // 初始化一个size位的Bytes，
        // 可以在首部填充字节数组data的前data_size位，
        // 不进行越界检查，可能会导致非法访问。
        // 未填充部分的值为0。
        explicit Bytes(size_t size = 0
        , const byte_t* data = nullptr, size_t data_size = 0);

        // 使用一个C风格字符数组的前data_size位初始化一个大小为data_size的Bytes。
        // data_size默认为0，此时Bytes的大小等于以data开头的C风格字符串的大小。
        explicit Bytes(const char* data, size_t data_size = 0);

        ~Bytes() noexcept;
        Bytes(const Bytes&);
        Bytes(Bytes&&) noexcept;
        Bytes& operator=(const Bytes&);
        Bytes& operator=(Bytes&&) noexcept;

        // 删除所有数据，size_置0，data_置空
        void clear();
        
        // 返回数据大小
        size_t size() const;
        
        // 用byte中以byte_pos开始的数据填充以pos开始长度为length的byte
        // 未指定byte_pos时，其值为0
        // 并不做越界检查，若byte剩余长度小于填充区间，则发生错误
        // 返回填充区间最后一个元素的后一个位置
        size_t modify(size_t pos, size_t length
        , const Bytes& byte, size_t byte_pos = 0);

        // 获取内部指针
        byte_t *get();
        const byte_t *get() const;

        // 打印内部字节，每n个字节换一行, n的默认值为4
        void show(size_t n = 4) const;
     private:
        size_t  size_;
        byte_t* data_;

        void copy(const Bytes& bytes);
        void move(Bytes&& bytes);
    };

    // 获取一个类型的字节表示。
    template <typename T>
    Bytes get_bytes(const T &input) {
        constexpr size_t size = sizeof(T);
        auto data = reinterpret_cast<const char*>(&input);
        return Bytes(data, size);
    }

    // 将一个值的字节序在本地序和网络序之间转换。
    // 即在一个小端机器上，字节序将被颠倒（因为函数总是期望收到一个小端序值而输出一个大端序值）。
    // 而在大端机器上并不转换值的字节序。
    template<typename T>
    T hnswitch(const T &input) {
        T output(0);
        constexpr size_t size = sizeof(T);
        auto data = reinterpret_cast<uint8_t*>(&output);

        for (size_t i = 0; i < size; ++i) {
            data[i] = input >> ((size - i  - 1) * 8);
        }
        return output;
    }
}

#endif