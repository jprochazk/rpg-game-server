
#pragma once
#ifndef SERVER_COMMON_BYTE_BUFFER_H
#define SERVER_COMMON_BYTE_BUFFER_H

#include "SharedDefines.h"
#include "endianness.h"

class ByteBuffer {
public:
    ByteBuffer(size_t reserve);
    ByteBuffer(std::vector<uint8_t>&& storage);
    ByteBuffer(const ByteBuffer& other);
    ByteBuffer(ByteBuffer&& other);
    ByteBuffer& operator=(const ByteBuffer& other);
    ByteBuffer& operator=(ByteBuffer&& other);
    ~ByteBuffer() = default;
    
    template<typename T>
    void Write(T val);

    template<typename T>
    bool Read(T* val);

    void Reserve(size_t size);
    size_t Size() const noexcept;
    size_t Capacity() const noexcept;
    const uint8_t* Data() const noexcept;
    std::vector<uint8_t> GetBuffer() const;
private:

    std::vector<uint8_t> storage_;
    size_t cursor_;
}; // class ByteBuffer

#endif // SERVER_COMMON_BYTE_BUFFER_H