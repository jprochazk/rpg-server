
#include "network/byte_buffer.h"

#include "common/assert.h"

namespace network {

byte_buffer::byte_buffer()
    :storage_(), cursor_(0)
{

}

byte_buffer::byte_buffer(size_t reserve)
    : storage_(reserve), cursor_(0)
{
}

byte_buffer::byte_buffer(std::vector<uint8_t>&& storage)
    : storage_(std::move(storage)), cursor_(0)
{}

byte_buffer::byte_buffer(const byte_buffer& other)
    : storage_(other.storage_), cursor_(other.cursor_)
{}

byte_buffer::byte_buffer(byte_buffer&& other) noexcept
    : storage_(std::move(other.storage_)), cursor_(std::move(other.cursor_))
{}

byte_buffer& byte_buffer::operator=(const byte_buffer& other)
{
    if (&other == this) return *this;
    this->storage_ = other.storage_;
    this->cursor_ = other.cursor_;
    return *this;
}

byte_buffer& byte_buffer::operator=(byte_buffer&& other) noexcept
{
    if (&other == this) return *this;
    this->storage_ = std::move(other.storage_);
    this->cursor_ = std::move(other.cursor_);
    return *this;
}

byte_buffer& byte_buffer::operator<<(uint8_t val) {
    write(val);
    return *this;
}
byte_buffer& byte_buffer::operator<<(uint16_t val) {
    write(val);
    return *this;
}
byte_buffer& byte_buffer::operator<<(uint32_t val) {
    write(val);
    return *this;
}
byte_buffer& byte_buffer::operator<<(int8_t val) {
    write(val);
    return *this;
}
byte_buffer& byte_buffer::operator<<(int16_t val) {
    write(val);
    return *this;
}
byte_buffer& byte_buffer::operator<<(int32_t val) {
    write(val);
    return *this;
}
byte_buffer& byte_buffer::operator<<(float val) {
    write(val);
    return *this;
}
byte_buffer& byte_buffer::operator<<(std::vector<uint8_t> const& val) {
    append(val.data(), val.size());
    return *this;
}
byte_buffer& byte_buffer::operator<<(const std::string& val) {
    // writing strings longer than 255 is a programmer error, so it's safe to assert
    DEBUG_ASSERT(val.length() < static_cast<size_t>(UINT8_MAX), "Attempted to write string longer than UINT8_MAX size!");
    auto strlen = static_cast<uint8_t>(val.length());
    write(strlen); // write the string length
    append(val.data(), val.size()); // copy the string
    return *this;
}

byte_buffer& byte_buffer::operator>>(std::optional<uint8_t>& val) {
    val = read<uint8_t>();
    return *this;
}
byte_buffer& byte_buffer::operator>>(std::optional<uint16_t>& val) {
    val = read<uint16_t>();
    return *this;
}
byte_buffer& byte_buffer::operator>>(std::optional<uint32_t>& val) {
    val = read<uint32_t>();
    return *this;
}
byte_buffer& byte_buffer::operator>>(std::optional<int8_t>& val) {
    val = read<int8_t>();
    return *this;
}
byte_buffer& byte_buffer::operator>>(std::optional<int16_t>& val) {
    val = read<int16_t>();
    return *this;
}
byte_buffer& byte_buffer::operator>>(std::optional<int32_t>& val) {
    val = read<int32_t>();
    return *this;
}
byte_buffer& byte_buffer::operator>>(std::optional<float>& val) {
    val = read<float>();
    return *this;
}
byte_buffer& byte_buffer::operator>>(std::optional<std::string>& val) {
    val.reset(); // ensure optional is null
    auto strlen = read<uint8_t>(); // first byte must be strlen!
    if (!strlen.has_value() || strlen.value() == 0) {
        return *this; // return if no value was found, or the string is empty
    }

    auto vec = read<char>(static_cast<size_t>(strlen.value()));
    if (!vec) {
        val.reset();
        return *this;
    }
    val = std::optional{ std::string(vec->begin(), vec->end()) };
    return *this;
}

bool byte_buffer::set_cursor(size_t pos)
{
    if (pos > storage_.size()) {
        return false;
    }
    cursor_ = pos;
    return true;
}

void byte_buffer::reserve(size_t size)
{
    storage_.reserve(size);
}

void byte_buffer::resize(size_t size, uint8_t val)
{
    storage_.resize(size, val);
}

size_t byte_buffer::size() const noexcept
{
    return storage_.size();
}

size_t byte_buffer::capacity() const noexcept
{
    return storage_.max_size();
}

uint8_t* byte_buffer::data() noexcept
{
    return storage_.data();
}

const uint8_t* byte_buffer::data() const noexcept {
    return storage_.data();
}

std::vector<uint8_t> byte_buffer::contents() const
{
    return storage_;
}

bool byte_buffer::operator==(const byte_buffer& other) {
    return storage_ == other.storage_;
}

} // namespace network
