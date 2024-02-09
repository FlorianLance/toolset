

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/


#pragma once

// std
#include <queue>
#include <mutex>

namespace tool{

template<class T>
class SafeQueue{

    //    std::condition_variable consumer_, producer_;
    std::mutex mutex_;
    std::queue<T> queue_;

    using unique_lock = std::unique_lock<std::mutex>;

public:
    template<class U>
    auto push_back(U&& item) -> void {

        std::unique_lock lock(mutex_);
        //        while(MaxQueueSize == queue_.size()){
        //            pop_front();
        ////            producer_.wait(lock);
        //        }
        queue_.push(std::forward<U>(item));
        if(queue_.size() > 20){
            queue_.pop();
        }
        //        consumer_.notify_one();
    }

    auto pop_front() -> std::optional<T> {
        std::unique_lock lock(mutex_);
        //        if(!mutex_.try_lock()){
        //            return std::nullopt;
        //        }
        if(queue_.empty()){
            return std::nullopt;
        }
        //        while(queue_.empty()){
        //            consumer_.wait(lock);
        //        }
        //        auto full = MaxQueueSize == queue_.size();
        auto item = queue_.front();
        queue_.pop();
        //        if(full){
        //            producer_.notify_all();
        //        }
        return item;
    }

    auto clean() -> void{
        std::unique_lock lock(mutex_);
        queue_ = {};
    }
};
}

// template<typename T, typename Container = std::queue<T>>
// class Queue
// {
// public:
//     Queue() = default;
//     ~Queue() = default;
//     //禁止拷贝和移动，编译器会自动delete
//     Queue(const Queue&) = delete;
//     Queue(Queue&&) = delete;
//     Queue& operator=(const Queue&) = delete;
//     Queue& operator=(Queue&&) = delete;
//     void push(const T& val)
//     {
//         emplace(val);
//     }
//     void push(T&& val)
//     {
//         emplace(std::move(val));
//     }
//     size_t size()const
//     {
//         std::unique_lock<std::mutex> lk{ mtx_ };
//         return q_.size();
//     }
//     template<typename...Args>
//     void emplace(Args&&...args)
//     {
//         std::unique_lock<std::mutex> lk{ mtx_ };
//         q_.push(std::forward<Args>(args)...);
//         cv_.notify_one();
//     }
//     T pop()//阻塞
//     {
//         std::unique_lock<std::mutex> lk{ mtx_ };
//         cv_.wait(lk, [this] {return !q_.empty(); });//如果队列不为空就继续执行，否则阻塞
//         assert(!q_.empty());
//         T ret{ std::move_if_noexcept(q_.front()) };
//         q_.pop();
//         return ret;
//     }
//     bool empty()const
//     {
//         std::unique_lock<std::mutex> lk{ mtx_ };
//         return q_.empty();
//     }
//     void clear()
//     {
//         std::unique_lock<std::mutex> lk{ mtx_ };
//         std::queue<T> empty;
//         std::swap(q_, empty);
//     }
// private:
//     Container q_;
//     mutable std::mutex mtx_;
//     std::condition_variable cv_;
// };
