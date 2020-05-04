
#include "pch.h"
#include "ByteBuffer.h"

ByteBuffer::ByteBuffer()
    :storage_(), cursor_(0)
{

}

ByteBuffer::ByteBuffer(size_t reserve)
    : storage_(reserve), cursor_(0)
{
}

ByteBuffer::ByteBuffer(std::vector<uint8_t>&& storage)
    : storage_(std::move(storage)), cursor_(0)
{}

ByteBuffer::ByteBuffer(const ByteBuffer& other)
    : storage_(other.storage_), cursor_(other.cursor_)
{}

ByteBuffer::ByteBuffer(ByteBuffer&& other)
    : storage_(std::move(other.storage_)), cursor_(std::move(other.cursor_))
{}

ByteBuffer& ByteBuffer::operator=(const ByteBuffer& other) 
{
    if(&other == this) return *this;
    this->storage_ = other.storage_;
    this->cursor_ = other.cursor_;
    return *this;
}

ByteBuffer& ByteBuffer::operator=(ByteBuffer&& other)
{
    if(&other == this) return *this;
    this->storage_ = std::move(other.storage_);
    this->cursor_ = std::move(other.cursor_);
    return *this;
}

ByteBuffer& ByteBuffer::operator<<(uint8_t val) {
    Write(val);
    return *this;
}
ByteBuffer& ByteBuffer::operator<<(uint16_t val) {
    Write(val);
    return *this;
}
ByteBuffer& ByteBuffer::operator<<(uint32_t val) {
    Write(val);
    return *this;
}
ByteBuffer& ByteBuffer::operator<<(int8_t val) {
    Write(val);
    return *this;
}
ByteBuffer& ByteBuffer::operator<<(int16_t val) {
    Write(val);
    return *this;
}
ByteBuffer& ByteBuffer::operator<<(int32_t val) {
    Write(val);
    return *this;
}
ByteBuffer& ByteBuffer::operator<<(float val) {
    Write(val);
    return *this;
}
ByteBuffer& ByteBuffer::operator<<(const std::string& val) {
    // writing strings longer than 255 is a programmer error, so it's safe to assert
    assert(val.length() < 256);
    auto strlen = static_cast<uint8_t>(val.length());
    Write(strlen); // write strlen
    for(auto& c : val) {
        // write each character
        Write((uint8_t)c);
    }
    return *this;
}

ByteBuffer& ByteBuffer::operator>>(std::optional<uint8_t>& val) {
    val = Read<uint8_t>();
    return *this;
}
ByteBuffer& ByteBuffer::operator>>(std::optional<uint16_t>& val) {
    val = Read<uint16_t>();
    return *this;
}
ByteBuffer& ByteBuffer::operator>>(std::optional<uint32_t>& val) {
    val = Read<uint32_t>();
    return *this;
}
ByteBuffer& ByteBuffer::operator>>(std::optional<int8_t>& val) {
    val = Read<int8_t>();
    return *this;
}
ByteBuffer& ByteBuffer::operator>>(std::optional<int16_t>& val) {
    val = Read<int16_t>();
    return *this;
}
ByteBuffer& ByteBuffer::operator>>(std::optional<int32_t>& val) {
    val = Read<int32_t>();
    return *this;
}
ByteBuffer& ByteBuffer::operator>>(std::optional<float>& val) {
    val = Read<float>();
    return *this;
}
ByteBuffer& ByteBuffer::operator>>(std::optional<std::string>& val) {
    val.reset(); // ensure optional is std::nullopt
    auto strlen = Read<uint8_t>(); // first byte must be strlen!
    if(!strlen.has_value() || strlen.value() == 0) {
        return *this; // return if no value was found, or the string is empty
    }
    
    val = std::string{}; // initialize the string to avoid segfault
    val->reserve(strlen.value()); // pre-allocate string length
    for(uint8_t i = 0; i < strlen; i++) {
        // TODO possibly read entire string at once? (memcopy more than one value)
        // would require a separate read function for arrays or a size_t parameter.
        std::optional<uint8_t> c = Read<uint8_t>(); // read one character
        if(!c.has_value()) {
            // if we fail to read the whole string, don't continue
            val.reset();
            return *this;
        }
        val->push_back(*c);
    }
    return *this;
}

bool ByteBuffer::SetCursor(size_t pos)
{
    if(pos > storage_.size()) {
        return false;
    }
    cursor_ = pos;
    return true;
}

void ByteBuffer::Reserve(size_t size)
{
    storage_.reserve(size);
}

size_t ByteBuffer::Size() const noexcept
{
    return storage_.size();
}

size_t ByteBuffer::Capacity() const noexcept
{
    return storage_.max_size();
}

const uint8_t* ByteBuffer::Data() const noexcept
{
    return storage_.data();
}

std::vector<uint8_t> ByteBuffer::Contents() const
{
    return storage_;
}

