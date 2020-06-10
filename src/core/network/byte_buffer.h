#pragma once

#include "common/endian.h"
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>

namespace network {

class byte_buffer {
public:
    byte_buffer();
    byte_buffer(size_t);
    byte_buffer(std::vector<uint8_t>&&);
    byte_buffer(const byte_buffer&);
    byte_buffer(byte_buffer&&) noexcept;
    byte_buffer& operator=(const byte_buffer&);
    byte_buffer& operator=(byte_buffer&&) noexcept;
    ~byte_buffer() = default;

    byte_buffer& operator<<(uint8_t);
    byte_buffer& operator<<(uint16_t);
    byte_buffer& operator<<(uint32_t);
    byte_buffer& operator<<(int8_t);
    byte_buffer& operator<<(int16_t);
    byte_buffer& operator<<(int32_t);
    byte_buffer& operator<<(float);
    byte_buffer& operator<<(std::vector<uint8_t> const&);
    byte_buffer& operator<<(std::string const&);
    byte_buffer& operator<<(byte_buffer const&);

    byte_buffer& operator>>(std::optional<uint8_t>&);
    byte_buffer& operator>>(std::optional<uint16_t>&);
    byte_buffer& operator>>(std::optional<uint32_t>&);
    byte_buffer& operator>>(std::optional<int8_t>&);
    byte_buffer& operator>>(std::optional<int16_t>&);
    byte_buffer& operator>>(std::optional<int32_t>&);
    byte_buffer& operator>>(std::optional<float>&);
    byte_buffer& operator>>(std::optional<std::string>&);

    bool set_cursor(size_t pos);

    void reserve(size_t);
    void resize(size_t, uint8_t val = 0);
    size_t size() const noexcept;
    size_t capacity() const noexcept;
    uint8_t* data() noexcept;
    const uint8_t* data() const noexcept;
    std::vector<uint8_t> contents() const;
private:
    template<typename T>
    void write(T const&);

    template<typename T>
    void append(T const*, size_t size);

    template<typename T>
    std::optional<T> read();

    template<typename T>
    std::optional<std::vector<T>> read(size_t size);

    std::vector<uint8_t> storage_;
    size_t cursor_;
}; // class byte_buffer

template<typename T>
void byte_buffer::write(T const& val)
{
    // we can only accept ints/floats of size < 4 bytes
    static_assert(
        std::is_fundamental<T>::value && sizeof(T) <= 4,
        "ByteBuffer::Write only accepts fundamental types of size 4 bytes and lower"
        );
    // ensure that storage has sufficient capacity
    if (storage_.capacity() < cursor_ + sizeof(T)) {
        storage_.reserve(cursor_ + sizeof(T));
    }
    // ensure that storage has pre-initialized values
    if (storage_.size() < cursor_ + sizeof(T)) {
        storage_.resize(cursor_ + sizeof(T));
    }
    common::endian_convert(val); // ensure network byte-order
    std::memcpy(&storage_[cursor_], (uint8_t*)&val, sizeof(T));
    cursor_ += sizeof(T);
}


template<typename T>
void byte_buffer::append(T const* data, size_t size) {
    static_assert(
        std::is_fundamental<T>::value && sizeof(T) <= 4,
        "ByteBuffer::Append only accepts fundamental types of size 4 bytes and lower"
        );
    if (size == 0) {
        return;
    }

    // copy all the data first, so as to not mess with the original data
    std::vector<T> cv(data, data + size);
    // EndianConvert all the values
    for (auto& val : cv) {
        common::endian_convert(val);
    }

    size = cv.size() * sizeof(T);
    if (storage_.capacity() < cursor_ + size) {
        storage_.reserve(cursor_ + size);
    }
    if (storage_.size() < cursor_ + size) {
        storage_.resize(cursor_ + size);
    }

    std::memcpy(&storage_[cursor_], (void*)cv.data(), size);

    cursor_ += size;
}

template<typename T>
std::optional<T> byte_buffer::read()
{
    // we can only accept ints/floats of size < 4 bytes due to javascript limitations
    static_assert(
        std::is_copy_assignable<T>::value &&
        std::is_fundamental<T>::value &&
        sizeof(T) <= 4,
        "ByteBuffer::Read only accepts fundamental types of size 4 bytes and lower"
        );

    if (cursor_ + sizeof(T) > storage_.size()) {
        // if there isn't enough data at current position 
        // in storage to make one entire T, return std::nullopt
        return {};
    }

    T val; // init the value
    // copy to it from storage
    std::memcpy(&val, &storage_[cursor_], sizeof(T));
    common::endian_convert(val); // ensure host byte-order
    cursor_ += sizeof(T); // increment cursor
    return { val }; // return the value
}


template<typename T>
std::optional<std::vector<T>> byte_buffer::read(size_t size) {
    static_assert(
        std::is_fundamental<T>::value &&
        sizeof(T) <= 4,
        "ByteBuffer::Read only accepts fundamental types of size 4 bytes and lower"
        );

    if (cursor_ + (sizeof(T) * size) > storage_.size()) {
        return {};
    }

    // prepare the output vector
    std::vector<T> storage(size);
    std::memcpy(storage.data(), &storage_[cursor_], sizeof(T) * size);
    for (auto& val : storage) {
        common::endian_convert(val);
    }
    cursor_ += sizeof(T) * size;
    return { storage };
}

} // namespace network

bool operator==(const network::byte_buffer& lhs, const network::byte_buffer& rhs);
