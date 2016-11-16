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
 * FixSizeLockFreeQueue.h
 *
 *  Created on: Nov 14, 2016
 *      Author: yukai
 *		 Email: yukai2014@gmail.com
 * 
 * Description:
 *
 */

#ifndef LockFreeQueue_FIXSIZELOCKFREEQUEUE_H_
#define LockFreeQueue_FIXSIZELOCKFREEQUEUE_H_


#include <iostream>
#include <memory>
#include <atomic>
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
class FixSizeLockFreeQueue {
  const static int64_t DEFAULT_INIT_SIZE = 128;
 public:
  FixSizeLockFreeQueue():FixSizeLockFreeQueue(DEFAULT_INIT_SIZE){}
  FixSizeLockFreeQueue(int64_t init_size)
  :capacity_(-1), front_offset_(0),end_offset_(0),  buffer_(nullptr){
    std::cout<<"initialize size is:"<<init_size<<std::endl;
    assert(init_size > 0 && init_size < 10000000);
    Init(init_size);
    assert(capacity_ > 0 && "invalid capacity of initialized queue");
  }
  ~FixSizeLockFreeQueue(){
    buffer_.reset();
  }

  inline bool Push(const T& t) {
    while(true) {
      int64_t front = front_offset_;
      if (front - end_offset_ + 1 > capacity_ ) return false;
      if (front_offset_.compare_exchange_weak(front, front+1)){
        buffer_[front%capacity_] = t;
        return true;
      } else {
        continue;
      }
    }
  }

  bool Pop(T& t){
    while (true) {
      int64_t end = end_offset_;
      if (end >= front_offset_) return false;
      if (end_offset_.compare_exchange_weak(end, end + 1)) {
        t = std::move(buffer_[end%capacity_]);
        return true;
      } else {
        continue;
      }
    }
  }
  inline bool Empty() { return 0 == Size(); }
  inline int64_t Size() { return front_offset_-end_offset_;}

 private:
  void Init(int64_t size){
    T* buf = static_cast<T*>(calloc(size, sizeof(T)));
    buffer_= unique_ptr<T[]>(buf);
    capacity_ = size;
  }

 private:
  int64_t capacity_;
 public:
  atomic<int64_t> front_offset_;   // the offset of the next data to store
  atomic<int64_t> end_offset_;     // the offset of the last stored data
 private:
  unique_ptr<T[]> buffer_;
};



#endif /* LockFreeQueue_FIXSIZELOCKFREEQUEUE_H_ */
