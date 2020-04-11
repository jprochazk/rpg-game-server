
#include "ByteBuffer.h"
#include "endianness.h"

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

ByteBuffer::~ByteBuffer() {}

template<typename T, size_t ALLOC_SIZE>
void ByteBuffer::Append(T val)
{
    static_assert(ALLOC_SIZE >= sizeof(T), "ByteBuffer::Append(Allocated size must be greater than or equal to size of T)");

    NetworkEndian(val); // network byte-order
    size_t newSize = cursor_ + ALLOC_SIZE;
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
    HostEndian(*val); // host byte-order
    return true;
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


ByteBuffer::operator std::vector<uint8_t>() const
{
    return storage_;
}

