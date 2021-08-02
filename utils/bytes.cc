// Copyright (c) 2021 Xilong Yang
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "bytes.h"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace stamp{

    Bytes::Bytes(size_t size, const byte_t* data, size_t data_size)
    : size_(size), data_(new byte_t[size_]) {
        if (size == 0) return;
        data_ = new byte_t[size];
        memset(data_, 0, size_);

        if (data == nullptr) return;
        assert(data_size <= size);
        memcpy(data_, data, data_size);
    }

    Bytes::Bytes(const char *data, size_t data_size) : size_(data_size), data_(new byte_t(size_)) {
        if (data_size == 0) {
            int i = 0;
            while (data[i]) ++i;
            size_ = i;
            data_ = new byte_t(size_);
        }
        memcpy(data_, data, size_);
    }

    Bytes::~Bytes() noexcept {
        clear();
    }

    Bytes::Bytes(const Bytes& bytes) : size_(0), data_(nullptr) {
        copy(bytes);
    }

    Bytes::Bytes(Bytes&& bytes) noexcept : size_(0), data_(nullptr) {
        move(std::move(bytes));
    }

    Bytes& Bytes::operator=(const Bytes &bytes) {
        copy(bytes);
        return *this;
    }

    Bytes& Bytes::operator=(Bytes &&bytes) noexcept {
        move(std::move(bytes));
        return *this;
    }

    void Bytes::clear() {
        if (data_ == nullptr) return;
        size_ = 0;
        delete [] data_;
        data_ = nullptr;
    }

    size_t Bytes::size() const {
        return size_;
    }

    size_t Bytes::modify(size_t pos, size_t length
    , const Bytes& bytes, size_t bytes_pos) {
        assert(size_ - pos >= length);
        assert(bytes.size_ - bytes_pos >= length);

        byte_t* cur = bytes.data_ + bytes_pos;
        for (int i = 0; i < length; ++i) {
            data_[pos] = *cur;
            ++cur;
            ++pos;
        }
        return pos;
    }

    byte_t *Bytes::get() {
        return data_;
    }

    const byte_t *Bytes::get() const {
        return data_;
    }

    void Bytes::show(size_t n) const {
        for (size_t i = 0; i < size_; ++i) {
            std::cout << std::hex << std::setw(2)
            << std::setfill('0') << static_cast<uint32_t>(data_[i]) << ' ';
            if ((i + 1) % n == 0) {
                std::cout << std::endl;
            }
        }
    }

    void Bytes::copy(const Bytes &bytes) {
        if (this == &bytes) return;
        clear();
        size_ = bytes.size_;
        data_ = new byte_t[size_];
        for (int i = 0; i < size_; ++i) {
            data_[i] = bytes.data_[i];
        }
    }

    void Bytes::move(Bytes &&bytes) {
        if (this == &bytes) return;
        clear();
        size_ = bytes.size_;
        bytes.size_ = 0;
        data_ = bytes.data_;
        bytes.data_ = nullptr;
    }
}