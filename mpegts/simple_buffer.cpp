#include "simple_buffer.h"
#include <assert.h>
#include <iterator>

static void swap_endian(char* out, size_t n)
{
    for (size_t i = 0; i < n / 2; ++i)
        std::swap(out[i], out[n - i - 1]);
}

SimpleBuffer::SimpleBuffer(int32_t size, int8_t value)
    : mData(size, value)
    , mPos(0)
{
}

SimpleBuffer::~SimpleBuffer()
{
}

void SimpleBuffer::write1Byte(int8_t val)
{
    char *p = (char *)&val;
    swap_endian(p, sizeof(val));
    write_raw(p, sizeof(val));
}

void SimpleBuffer::write2Bytes(int16_t val)
{
    char *p = (char *)&val;
    swap_endian(p, sizeof(val));
    write_raw(p, sizeof(val));
}

void SimpleBuffer::write3Bytes(int32_t val)
{
    char *p = (char *)&val;
    swap_endian(p, sizeof(val));
    write_raw(p + 1, 3);
}

void SimpleBuffer::write4Bytes(int32_t val)
{
    char *p = (char *)&val;
    swap_endian(p, sizeof(val));
    write_raw(p, sizeof(val));
}

void SimpleBuffer::write8Bytes(int64_t val)
{
    char *p = (char *)&val;
    swap_endian(p, sizeof(val));
    write_raw(p, sizeof(val));
}

void SimpleBuffer::write_raw(const char* p, size_t n)
{
    mData.resize(mData.size() + n);
    auto out = mData.data() + mData.size() - n;
    std::copy(p, p + n, out);
}

void SimpleBuffer::append(const uint8_t* bytes, int size)
{
    if (!bytes || size <= 0) {
#ifdef DEBUG
        std::cout << " append error " << std::endl;
#endif
        return;
    }

    mData.insert(mData.end(), bytes, bytes + size);
}

int8_t SimpleBuffer::read1Byte()
{
    int8_t val = 0;
    assert(require(sizeof(val)));
    read_to_host((char*)&val, sizeof(val));
    return val;
}

int16_t SimpleBuffer::read2Bytes()
{
    int16_t val = 0;
    assert(require(sizeof(val)));
    read_to_host((char*)&val, sizeof(val));
    return val;
}

int32_t SimpleBuffer::read3Bytes()
{
    assert(require(3));
    int32_t val = 0;
    char *p = (char *)&val;
    read_raw(p + 1, 3);
    swap_endian(p, 4);
    return val;
}

int32_t SimpleBuffer::read4Bytes()
{
    int32_t val = 0;
    assert(require(sizeof(val)));
    read_to_host((char*)&val, sizeof(val));
    return val;
}

int64_t SimpleBuffer::read8Bytes()
{
    int64_t val = 0;
    assert(require(sizeof(val)));
    read_to_host((char*)&val, sizeof(val));
    return val;
}

std::string SimpleBuffer::readString(int len)
{
    assert(require(len));
    std::string val;
    val.resize(len);
    read_raw(val.data(), len);
    return val;
}

bool SimpleBuffer::read_to_host(char* out, size_t n)
{
    if (read_raw(out, n))
    {
        swap_endian(out, n);
        return true;
    }
    else
    {
        return false;
    }
}

bool SimpleBuffer::read_raw(char* out, size_t n)
{
    if (size() - pos() >= n)
    {
        std::copy(mData.data() + mPos, mData.data() + mPos + n, out);
        mPos += n;
        return true;
    }
    else
    {
        return false;
    }
}

void SimpleBuffer::skip(int size)
{
    mPos += size;
}

bool SimpleBuffer::require(int required_size)
{
    assert(required_size >= 0);

    return required_size <= mData.size() - mPos;
}

bool SimpleBuffer::empty()
{
    return mPos >= mData.size();
}

int SimpleBuffer::size()
{
    return mData.size();
}

int SimpleBuffer::pos()
{
    return mPos;
}

uint8_t* SimpleBuffer::data()
{
    return (size() == 0) ? nullptr : &mData[0];
}

void SimpleBuffer::clear()
{
    mPos = 0;
    mData.clear();
}

void SimpleBuffer::setData(int pos, const uint8_t* data, int len)
{
    if (!data) {
#ifdef DEBUG
        std::cout << " setData error data ptr == nullpts " << std::endl;
#endif
        return;
    }

    if (pos + len > size()) {
#ifdef DEBUG
        std::cout << " setData error data out of bounds " << std::endl;
#endif
        return;
    }

    for (int i = 0; i < len; i++) {
        mData[pos + i] = data[i];
    }
}

