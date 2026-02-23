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
        // 对空间进行扩容
        void EnsureEnoughSize(size_t len)
        {
            if (len <= WriterAbleSize())
            {
                return; // 不用扩容
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

        // 对读写指针进行向后偏移
        // 移动写位置的指针
        void MoveWriter(size_t len)
        {
            assert(len <= WriterAbleSize());
            _writer_index += len;
        }

    public:
        Buffer() : _buffer(DEFAULT_BUFFER_SIZE), _writer_index(0), _reader_index(0)
        {
        }

        // 向缓冲区中写入数据
        void Push(const char *data, size_t len)
        {
            // 缓冲区剩余空间不足 -- 1.扩容 2.阻塞返回false
            // 固定大小直接返回
            // if (len > WriterAbleSize())
            // {
            //     return;
            // }
            // 扩容
            EnsureEnoughSize(len);
            // 1.将数据拷贝进缓冲区
            std::copy(data, data + len, &_buffer[_writer_index]);
            // 2.将写入位置向后偏移
            MoveWriter(len);
        }
        // 获取可读数据起始地址
        const char *Begin()
        {
            return &_buffer[_reader_index];
        }
        // 可写数据长度
        size_t WriterAbleSize()
        {
            // 对于扩容思路,不存在可写空间大小,因为总是可写
            // 仅针对固定大小缓冲区
            return _buffer.size() - _writer_index;
        }
        // 可读数据的长度
        size_t ReadAbleSize()
        {
            return _writer_index - _reader_index;
        }

        // 对读写指针进行向后偏移
        // 移动读位置的指针
        void MoveReader(size_t len)
        {
            assert(len <= ReadAbleSize());
            _reader_index += len;
        }

        // 重置读写位置,初始化缓冲区
        void Reset()
        {
            _reader_index = 0;
            _writer_index = 0;
        }

        // 对buffer进行交换
        void Swap(Buffer &buffer)
        {
            _buffer.swap(buffer._buffer);
            std::swap(_reader_index, buffer._reader_index);
            std::swap(_writer_index, buffer._writer_index);
        }

        // 判断缓冲区是否为空
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
