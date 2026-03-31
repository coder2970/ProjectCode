// 实现异步日志缓冲区
#pragma once
#include "util.hpp"
#include <vector>
#include <cassert>

namespace ns_log
{
#define DEFAULT_BUFFER_SIZE (1 * 1024 * 1024)
#define THRESHOLD_BUFFER_SIZE (8 * 1024 * 1024)
#define INCREMENT_BUFFER_SIZE (1 * 1024 * 1024)
    class Buffer
    {
    private:
        void EnsureEnoughSize(size_t len)
        {
            if (len <= WriterAbleSize())
            {
                return;
            }
            size_t new_size = 0;
            if (_buffer.size() < THRESHOLD_BUFFER_SIZE)
            {
                new_size = _buffer.size() * 2 + len; // 小于阈值翻倍增长
            }
            else
            {
                new_size = _buffer.size() + INCREMENT_BUFFER_SIZE + len; // 大于阈值线性增长
            }
            _buffer.resize(new_size);
        }

        void MoveWriter(size_t len)
        {
            assert(len <= WriterAbleSize());
            _writer_index += len;
        }

    public:
        Buffer() : _buffer(DEFAULT_BUFFER_SIZE), _writer_index(0), _reader_index(0)
        {
        }

        void Push(const char *data, size_t len)
        {
            // 缓冲区剩余空间不足 -- 1.扩容 2.阻塞返回false
            // 扩容
            EnsureEnoughSize(len);
            std::copy(data, data + len, &_buffer[_writer_index]);
            MoveWriter(len);
        }
        // 获取可读数据起始地址
        const char *Begin()
        {
            return &_buffer[_reader_index];
        }

        size_t WriterAbleSize()
        {
            return _buffer.size() - _writer_index;
        }

        size_t ReadAbleSize()
        {
            return _writer_index - _reader_index;
        }

        void MoveReader(size_t len)
        {
            assert(len <= ReadAbleSize());
            _reader_index += len;
        }

        void Reset()
        {
            _reader_index = 0;
            _writer_index = 0;
        }

        void Swap(Buffer &buffer)
        {
            _buffer.swap(buffer._buffer);
            std::swap(_reader_index, buffer._reader_index);
            std::swap(_writer_index, buffer._writer_index);
        }

        bool Empty()
        {
            return _reader_index == _writer_index;
        }

    private:
        std::vector<char> _buffer; // 通过vector管理缓冲区中的数据(格式化后的消息)
        size_t _reader_index;      // 当前可读数据的指针
        size_t _writer_index;       // 当前可写数据的指针
    };
}
