
#pragma once
#ifndef SERVER_COMMON_BYTE_BUFFER_H
#define SERVER_COMMON_BYTE_BUFFER_H

#include "SharedDefines.h"

class ByteBuffer {
public:
    ByteBuffer(size_t reserve);
    ByteBuffer(std::vector<uint8_t>&& storage);
    ByteBuffer(const ByteBuffer& other);
    ByteBuffer(ByteBuffer&& other);
    ByteBuffer& operator=(const ByteBuffer& other);
    ByteBuffer& operator=(ByteBuffer&& other);
    ~ByteBuffer();
    
    template<typename T, size_t SIZE = sizeof(T) <= 4 ? sizeof(T) * 2 : sizeof(T)>
    void Append(T val);

    template<typename T>
    bool Read(T* val);

    size_t Size() const noexcept;
    size_t Capacity() const noexcept;
    const uint8_t* Data() const noexcept;

    // Conversion to std::vector of bytes
    operator std::vector<uint8_t>() const;
private:

    std::vector<uint8_t> storage_;
    size_t cursor_;
}; // class ByteBuffer

#endif // SERVER_COMMON_BYTE_BUFFER_H