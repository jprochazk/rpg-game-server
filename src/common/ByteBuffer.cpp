
#include "ByteBuffer.h"

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

template<typename T>
void ByteBuffer::Write(T val)
{
    NetworkEndian(val); // ensure network byte-order
    size_t newSize = cursor_ + sizeof(T);
    if(storage_.capacity() < newSize) {
        storage_.reserve(newSize);
    }
    std::memcpy(&storage_[cursor_], &val, sizeof(T));
    cursor_ = newSize;
}

template<typename T>
bool ByteBuffer::Read(T* val)
{
    size_t newCursor_ = cursor_ + sizeof(T);
    if (newCursor_ > storage_.size()) {
        return false;
    }
    std::memcpy(val, &storage_[cursor_], sizeof(T));
    cursor_ = newCursor_;
    HostEndian(*val); // ensure host byte-order
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

std::vector<uint8_t> ByteBuffer::GetBuffer() const
{
    return storage_;
}

