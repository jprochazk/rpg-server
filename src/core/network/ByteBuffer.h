
#pragma once
#ifndef SERVER_COMMON_BYTE_BUFFER_H
#define SERVER_COMMON_BYTE_BUFFER_H

#include "pch.h"
#include "core/common/ByteConvert.h"

class ByteBuffer {
public:
    ByteBuffer();
    ByteBuffer(size_t reserve);
    ByteBuffer(std::vector<uint8_t>&& storage);
    ByteBuffer(const ByteBuffer& other);
    ByteBuffer(ByteBuffer&& other);
    ByteBuffer& operator=(const ByteBuffer& other);
    ByteBuffer& operator=(ByteBuffer&& other);
    ~ByteBuffer() = default;

    ByteBuffer& operator<<(uint8_t val);
    ByteBuffer& operator<<(uint16_t val);
    ByteBuffer& operator<<(uint32_t val);
    ByteBuffer& operator<<(int8_t val);
    ByteBuffer& operator<<(int16_t val);
    ByteBuffer& operator<<(int32_t val);
    ByteBuffer& operator<<(float val);
    ByteBuffer& operator<<(std::string const& val);
    
    ByteBuffer& operator>>(std::optional<uint8_t>& val);
    ByteBuffer& operator>>(std::optional<uint16_t>& val);
    ByteBuffer& operator>>(std::optional<uint32_t>& val);
    ByteBuffer& operator>>(std::optional<int8_t>& val);
    ByteBuffer& operator>>(std::optional<int16_t>& val);
    ByteBuffer& operator>>(std::optional<int32_t>& val);
    ByteBuffer& operator>>(std::optional<float>& val);
    ByteBuffer& operator>>(std::optional<std::string>& val);

    bool SetCursor(size_t pos);

    void Reserve(size_t size);
    size_t Size() const noexcept;
    size_t Capacity() const noexcept;
    uint8_t* Data() noexcept;
    std::vector<uint8_t> Contents() const;
private:
    template<typename T>
    void Write(T val);

    template<typename T>
    std::optional<T> Read();

    std::vector<uint8_t> storage_;
    size_t cursor_;
}; // class ByteBuffer

template<typename T>
void ByteBuffer::Write(T val)
{
    // we can only accept ints/floats of size < 4 bytes
    static_assert(
        std::is_fundamental<T>::value && 
        sizeof(T) <= 4, 
        "Write only accepts fundamental types of size 4 bytes and lower"
    );
    // ensure that storage has sufficient capacity
    if(storage_.capacity() < cursor_ + sizeof(T)) {
        storage_.reserve(cursor_ + sizeof(T));
    }
    // ensure that storage has pre-initialized values
    if(storage_.size() < cursor_ + sizeof(T)) {
        storage_.resize(cursor_ + sizeof(T));
    }
    EndianConvert(val); // ensure network byte-order
    std::memcpy(&storage_[cursor_], (uint8_t*)&val, sizeof(T));
    cursor_ += sizeof(T);
}


template<typename T>
std::optional<T> ByteBuffer::Read()
{
    // we can only accept ints/floats of size < 4 bytes
    // this is due to javascript only being able to handle arraybuffer 
    static_assert(
        std::is_copy_assignable<T>::value &&
        std::is_fundamental<T>::value && 
        sizeof(T) <= 4, 
        "Read only accepts fundamental types of size 4 bytes and lower"
    );

    if (cursor_ + sizeof(T) > storage_.size()) {
        // if there isn't enough data at current position 
        // in storage to make one entire T, return std::nullopt
        return {};
    }

    T val; // init the value
    // copy to it from storage
    std::memcpy(&val, &storage_[cursor_], sizeof(T));
    EndianConvert(val); // ensure host byte-order
    cursor_ += sizeof(T); // increment cursor
    return {val}; // return the value
}

#endif // SERVER_COMMON_BYTE_BUFFER_H