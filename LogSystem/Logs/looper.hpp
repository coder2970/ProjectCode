#pragma once
// 实现异步工作器
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "buffer.hpp"

namespace ns_log
{
    using Functor = std::function<void(Buffer &)>;
    enum class AsyncType
    {
        ASYNC_SAFE,  // 安全状态 - 缓冲区满则阻塞
        ASYNC_UNSAFE // 不考虑资源耗尽问题 - 无限扩容
    };
    class AsyncLooper
    {
    private:
        // 线程入口函数 -
        // 对消费缓冲区中的数据进行处理,处理完毕后初始化缓冲区,交换缓冲区
        void ThreadEntry()
        {
            while (true)
            {
                {
                    // 1.判断生产缓冲区有无数据, 有则交换,无则阻塞
                    std::unique_lock<std::mutex> lock(_mutex);
                    if (_stop && _product_buffer.Empty())
                        break;
                    _cond_consume.wait(lock,
                                       [&]()
                                       { return _stop || !_product_buffer.Empty(); });

                    _consume_buffer.Swap(_product_buffer);

                    // 2.唤醒生产者
                    if(_loop_type == AsyncType::ASYNC_SAFE)
                        _cond_product.notify_all();
                }
                // 3.唤醒后,对消费缓冲区进行数据处理
                _callback(_consume_buffer);
                // 4.初始化消费缓冲区
                _consume_buffer.Reset();
            }
        }

    public:
        using ptr = std::shared_ptr<AsyncLooper>;
        AsyncLooper(const Functor &cb, AsyncType loop_type = AsyncType::ASYNC_SAFE)
            : _loop_type(loop_type), _stop(false), _thread(std::thread(&AsyncLooper::ThreadEntry, this)),
              _callback(cb) {}
        ~AsyncLooper() { Stop(); }
        // 停止异步工作器
        void Stop()
        {
            _stop = true;               
            _cond_consume.notify_all(); 
            _thread.join();            
        }
        // 添加数据到缓冲区
        void Push(const char *data, size_t len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_loop_type == AsyncType::ASYNC_SAFE)
                _cond_product.wait(
                    lock, [&]()
                    { return _product_buffer.WriterAbleSize() >= len; });
            _product_buffer.Push(data, len);
            _cond_consume.notify_one();
        }

    private:
        AsyncType _loop_type;
        std::atomic<bool> _stop;               // 工作器停止标志
        Buffer _product_buffer;                // 生产缓冲区
        Buffer _consume_buffer;                // 消费缓冲区
        std::mutex _mutex;                     // 线程安全的锁
        std::condition_variable _cond_product; // 生产者条件变量
        std::condition_variable _cond_consume; // 消费者条件变量
        std::thread _thread;                   // 异步工作器对应的线程
        Functor _callback;                     // 对缓冲区数据进行处理的回调函数,由异步工作器使用者来传入
    };
}