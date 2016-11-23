/*
 * Copyright [2012-2015] YuKai
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * LockFreeQueue.h
 *
 *  Created on: Nov 7, 2016
 *      Author: yukai
 *		 Email: yukai2014@gmail.com
 * 
 * Description:
 *
 */

#ifndef LockFreeQueue_LOCKFREEQUEUE_H_
#define LockFreeQueue_LOCKFREEQUEUE_H_

#include <queue>
using std::queue;

#include <iostream>
#include <memory>
#include <atomic>
#include <mutex>
#include <string>
#include <assert.h>
#include <malloc.h>
#include "./CompilerFeature.h"
#include "./Guard.h"

using std::string;
using std::atomic;
using std::make_shared;
using std::shared_ptr;
using std::unique_ptr;

/*****************************************

queue like this:
   end -> ================= -> front

*****************************************/
template<typename T>
class LockFreeQueue {
  const static int64_t DEFAULT_INIT_SIZE = 128;
 public:
  LockFreeQueue():LockFreeQueue(DEFAULT_INIT_SIZE){}
  LockFreeQueue(int64_t init_size,
                std::function<int64_t(int64_t)> expand_func = [](int64_t k){return k*2;})
  :capacity_(-1), front_offset_(0),end_offset_(0),  buffer_(nullptr), expand_func_(expand_func){
    std::cout<<"initialize size is:"<<init_size<<std::endl;
    assert(init_size > 0 && init_size < 10000000);
    Init(init_size);
    assert(capacity_ > 0 && "invalid capacity of initialized queue");
  }
  ~LockFreeQueue(){
    buffer_.reset();
  }

  void Push(const T& t) {
    if (unlikely(front_offset_ - end_offset_ +1 > capacity_ )) {
      MutexLockGuard guard(mutex_);
      if (front_offset_ - end_offset_ + 1 > capacity_) {
        int cap = GetNextCap();
        T* buff = static_cast<T*>(malloc(cap * sizeof(T)));
        memset(buff, -1, cap * sizeof(T));
        auto buf = unique_ptr<T[]>(buff);
        buffer_ = Migrate(std::move(buf));
        buffer_[front_offset_++] = t;
        capacity_ = cap;
        return;
      }
    }
    // ERROR!!!
    // two thread will execute this statement concurrently, when seeing the front_offset_ both satisfy " front_offset_ - end_offset_ + 1 = capacity_".
    // then the front_offset_ will be exceed the safe range, which leads to move the latter data after the last data to the first place
    // so the solution is make this queue fixed size.
    int last_front = __sync_fetch_and_add(&front_offset_, 1);
    buffer_[last_front%capacity_] = t;
  }
  const T& Pop(){
    int last_end = __sync_fetch_and_add(&end_offset_, 1);
    return buffer_[last_end%capacity_];
  }
  inline bool Empty() { return 0 == Size(); }
  inline int64_t Size() { return front_offset_-end_offset_;}

 private:
  void Init(int64_t size){
    T* buf = static_cast<T*>(calloc(size, sizeof(T)));
    buffer_= unique_ptr<T[]>(buf);
    capacity_ = size;
  }

  int64_t GetNextCap() {
    int64_t cap = expand_func_(capacity_);
    assert(cap > 0);
    return cap;
  }

  // migrate all data from old array buffer to new buffer,
  // reset end_offset_ and front_offset_
  unique_ptr<T[]> Migrate( unique_ptr<T[]>&& new_buf){
    int old_end_offset = end_offset_;
    for (int i = 0; i < front_offset_-end_offset_; ++i)
      new_buf[i]= std::move(buffer_[old_end_offset + i ]);

    end_offset_ = 0;
    front_offset_ = front_offset_ - old_end_offset;
    return std::move(new_buf);
  }

 private:
  int64_t capacity_;
 public:
  volatile int64_t front_offset_;   // the offset of the next data to store
  volatile int64_t end_offset_;     // the offset of the last stored data
 private:
  unique_ptr<T[]> buffer_;
  std::function<int64_t(int64_t)> expand_func_;

  std::mutex mutex_;
};


#endif /* LockFreeQueue_LOCKFREEQUEUE_H_ */
