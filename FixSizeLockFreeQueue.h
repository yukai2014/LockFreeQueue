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


#include <assert.h>
#include <atomic>
#include <iostream>
#include <malloc.h>
#include <memory>
#include <string>
#include <thread>

#include "./CompilerFeature.h"
#include "./Guard.h"

using std::atomic;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unique_ptr;

#define CAS(ptr , oldvalue , newvalue) __sync_bool_compare_and_swap(&ptr , oldvalue , newvalue)

/*****************************************

queue like this:
   end -> ================= -> front

 *****************************************/

template<typename T>
class FixSizeLockFreeQueue {
 public:
  const static int64_t DEFAULT_INIT_SIZE = 128;

 public:
  FixSizeLockFreeQueue(int64_t init_size = DEFAULT_INIT_SIZE)
 :capacity_(init_size), front_offset_(0),end_offset_(0), written_offset_(0), buffer_(nullptr){

    assert(capacity_ > 0 && "invalid capacity of initialized queue");
    T* buf = new T[capacity_];
    memset(buf, -1, sizeof(T) * capacity_);
    buffer_= unique_ptr<T[]>(buf);
  }

  ~FixSizeLockFreeQueue(){
    buffer_.reset();
  }

  /* return false if queue is full, otherwise true */
  inline bool Push(const T& t) {
    while(true) {
      int64_t front = front_offset_;
      if (front - end_offset_ + 1 > capacity_ ) return false;
      if (CAS(front_offset_,front, front+1)){
        buffer_[front%capacity_] = t;

        while(!CAS(written_offset_,front, front + 1)) {
          sched_yield();    // great performance promotion when producers are more the physical cores
        }
        return true;
      }
    }
  }


  /* return false if queue is empty, otherwise true with moving data into t */
  bool Pop(T& t){
    while (true) {
      int64_t end = end_offset_;
      if (end >= front_offset_) return false;
      if (end >= written_offset_) {sched_yield(); continue;};

      /*
       * must get data before CAS operation,
       * otherwise can't guarantee the data to read is rewritten by new push()
       */
      t = buffer_[end%capacity_];
      if (CAS(end_offset_, end, end + 1)) {
        return true;
      }
      sched_yield();
    }
  }

  inline bool Empty() { return 0 == Size(); }
  inline int64_t Size() { return front_offset_-end_offset_;}
  inline const int64_t front() const {return front_offset_; }
  inline const int64_t end() const {return end_offset_;}
  inline const int64_t written_off() const { return written_offset_;}

 protected:
  const int64_t capacity_;
  int64_t front_offset_;   // the offset of the next data to store
  int64_t end_offset_;     // the offset of the next stored data to read
  int64_t written_offset_;  // the offset of last data stored
  unique_ptr<T[]> buffer_;

};

template<typename T>
class SingleProducerQueue : public FixSizeLockFreeQueue<T> {
  using LFQueue = FixSizeLockFreeQueue<T>;
  /* return false if queue is full, otherwise true */
 public:
  SingleProducerQueue(int64_t init_size = LFQueue::DEFAULT_INIT_SIZE) : FixSizeLockFreeQueue<T>(init_size){}

  inline bool Push(const T& t) {
    if (LFQueue::front_offset_ - LFQueue::end_offset_ + 1 > LFQueue::capacity_ ) return false;

    int front = __sync_fetch_and_add(&(LFQueue::front_offset_), 1);
    LFQueue::buffer_[front%LFQueue::capacity_] = t;
    return true;
  }

  /* return false if queue is empty, otherwise true with moving data into t */
  bool Pop(T& t){
    while (true) {
      int64_t end = LFQueue::end_offset_;
      if (end >= LFQueue::front_offset_) return false;
      /*
       * must get data before CAS operation,
       * otherwise can't guarantee the data to read is rewritten by new push()
       */
      t = LFQueue::buffer_[end%LFQueue::capacity_];
      if (CAS((LFQueue::end_offset_), end, end + 1)) {
        return true;
      }
      sched_yield();
    }
  }
};

#endif /* LockFreeQueue_FIXSIZELOCKFREEQUEUE_H_ */
