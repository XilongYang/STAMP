// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "hmac.h"

namespace stamp {
    // SHA256算法需要的辅助函数与常量
    namespace {
        // 循环右移
        template<typename T>
        inline T CROR(const T &input, size_t bits) {
            size_t size = sizeof(T) * 8;
            return ((input >> bits) | (input << (size - bits)));
        }

        // SHA256算法的8个Hash初值，由自然数中的前8个质数的平方根小数部分取前32bit得来。
        const uint32_t h_init[8] =
                {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a
                        ,0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

        // SHA256算法的64个常数，由自然数中的前64个质数的立方根小数部分取前32bit得来。
        const uint32_t k[64] =
                {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b
                        ,0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01
                        ,0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7
                        ,0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc
                        ,0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152
                        ,0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147
                        ,0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc
                        ,0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85
                        ,0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819
                        ,0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08
                        ,0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f
                        ,0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208
                        ,0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

        // 填充待加密的数据，以符合SHA256算法的需求。规则如下：
        // STEP1: 对数据包末尾进行bit填充，使得其大小(bit)对512取模后等于448。
        //        填充部分第一位是1，其后全为0。
        //        注意1:即使数据大小对512取模已经等于448了，仍要进行填充，此时填充512bit。
        //        注意2:为什么是448？因为下一步还要填充64bit的值，448+64=512。
        // STEP2: 填充一个64bit的数字，表示数据填充前大小。
        Bytes pre_process(const Bytes &bytes) {
            // 512 / 8
            constexpr uint32_t unit_size = 64;
            // 448 / 8
            constexpr uint32_t target_size = 56;
            // 第一个字节，值为10000000
            constexpr unsigned char first_byte = 0x80;

            // 计算STEP1需要填充的字节数
            size_t need_size = target_size - (bytes.size() % unit_size);
            if (need_size < 0) need_size += unit_size;

            // 生成STEP2需要的大小(bit)，转换为大端序
            auto origin_size = hnswitch<uint64_t>(bytes.size() * 8);

            // 生成待填充的数据块
            Bytes result(bytes.size() + need_size + sizeof(uint64_t)
                    , bytes.get(), bytes.size());

            // 填充STEP1
            auto append_bytes = get_bytes(first_byte);
            result.modify(bytes.size(), append_bytes.size(), append_bytes);

            // 填充STEP2
            auto origin_bytes = get_bytes(origin_size);
            result.modify(bytes.size() + need_size, origin_bytes.size(), origin_bytes);

            return result;
        }

        // SHA256算法中用到的函数
        inline uint32_t EP0(uint32_t x) {
            return CROR(x, 2) ^ CROR(x, 13) ^ CROR(x, 22);
        }

        inline uint32_t EP1(uint32_t x) {
            return CROR(x, 6) ^ CROR(x, 11) ^ CROR(x, 25);
        }

        inline uint32_t CH(uint32_t x, uint32_t y, uint32_t z) {
            return ((x & y) ^ ((~x) & z));
        }

        inline uint32_t MAJ(uint32_t x, uint32_t y, uint32_t z) {
            return ((x & y) ^ (x & z) ^ (y & z));
        }
    }

    // SHA256算法实现
    Bytes sha256(const Bytes& message) {
        auto result = pre_process(message);

        // 初始化hash值H。
        uint32_t H[8];
        for (size_t i = 0; i < 8; ++i) {
            H[i] = h_init[i];
        }

        // 将result分割为512bit的chunk，并依次对每一个chunk进行处理。
        auto chunk_num = result.size() / 64;
        for (size_t i = 0; i < chunk_num; ++i) {
            auto begin = i * 64;
            // 将chunk分割成16个32bit的块作为初始值, 并迭代生成总共64个32bit的块。
            uint32_t w[64];
            for (size_t j = 0; j < 16; ++j) {
                uint32_t value =*reinterpret_cast<const uint32_t*>
                                (result.get() + begin + j * 4);
                // 由于强制转换的关系，需要调整字节序
                w[j] = hnswitch(value);
            }
            for (size_t j = 16; j < 64; ++j) {
                auto s0 = CROR(w[j - 15], 7)
                        ^ CROR(w[j - 15], 18)
                        ^ (w[j - 15] >> 3);
                auto s1 = CROR(w[j - 2], 17)
                        ^ CROR(w[j - 2], 19)
                        ^ (w[j - 2] >> 10);
                w[j] = w[j - 16] + s0 + w[j - 7] + s1;
            }
            // 初始代hash值h
            uint32_t h[8];
            for (size_t j = 0; j < 8; ++j) {
                h[j] = H[j];
            }
            // 根据64个块与k值计算hash值
            for (size_t j = 0; j < 64; ++j) {
                auto t1 = h[7] + EP1(h[4]) + CH(h[4], h[5], h[6]) + k[j] + w[j];
                auto t2 = EP0(h[0]) + MAJ(h[0], h[1], h[2]);
                for (size_t l = 7; l > 0; --l) {
                    h[l] = h[l - 1];
                    if (l == 4) {
                        h[l] += t1;
                    }
                }
                h[0] = t1 + t2;
            }
            // 为下一个chunk的计算更新hash值
            for (size_t j = 0; j < 8; ++j) {
                H[j] += h[j];
            }
        }
        result = Bytes(32);
        size_t cur_pos = 0;
        for (auto v : H) {
            // 同样由于制转换，需要调整字节序。
            cur_pos = result.modify(cur_pos, sizeof(uint32_t), get_bytes(hnswitch(v)));
        }
        return result;
    }

    // HMAC算法需要的辅助函数与常量
    namespace {
        constexpr uint8_t ipad = 0x36;
        constexpr uint8_t opad = 0x5C;

        // 规定加密单元大小，512bit
        constexpr size_t unit_size = 64;

        // 对密钥规整化处理，即小于512bit的补齐到512bit.
        inline Bytes key_padding(const Bytes &key_raw) {
            if (key_raw.size() > unit_size) {
                return sha256(key_raw);
            }
            Bytes result(unit_size);
            result.modify(0, key_raw.size(), key_raw);
            return result;
        }

        // 返回key与pad的循环序列进行xor运算的结果。
        inline Bytes key_xor(const Bytes &key, const uint8_t pad) {
            Bytes result(key);
            auto begin = reinterpret_cast<uint8_t*>(result.get());
            for (size_t i = 0; i < result.size(); ++i) {
                begin[i] ^= pad;
            }
            return result;
        }

        inline Bytes combine(const Bytes &pad_key, const Bytes &message) {
            Bytes result(pad_key.size() + message.size());
            size_t cur_pos = 0;
            cur_pos = result.modify(cur_pos, pad_key.size(), pad_key);
            result.modify(cur_pos, message.size(), message);
            return result;
        }
    }
    //HMAC算法实现
    Bytes hmac(const Bytes& message, const Bytes& key_raw, const std::function<Bytes(const Bytes&)>&func) {
        const auto key = key_padding(key_raw);
        const auto ipad_key = key_xor(key, ipad);
        const auto opad_key = key_xor(key, opad);

        auto result = combine(ipad_key, message);
        result = func(result);
        result = combine(opad_key, result);
        result = func(result);
        return result;
    }
}

