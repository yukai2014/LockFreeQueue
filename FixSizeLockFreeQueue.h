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
  :capacity_(init_size), front_offset_(0),end_offset_(0),  buffer_(nullptr){
//    std::cout<<"initialize size is:"<<init_size<<std::endl;
    assert(capacity_ > 0);
    T* buf = static_cast<T*>(malloc(capacity_ * sizeof(T)));
    memset(buf, -1, sizeof(T) * capacity_);
    buffer_= unique_ptr<T[]>(buf);
    assert(capacity_ > 0 && "invalid capacity of initialized queue");

    memset(ids, 0, sizeof(ids));
    memset(bids, 0, sizeof(bids));
  }
  ~FixSizeLockFreeQueue(){
    buffer_.reset();
  }

  // return false if queue is full, otherwise true
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

  // return false if queue is empty, otherwise true with moving data into t
  bool Pop(T& t){
    while (true) {
      int64_t end = end_offset_;
      if (end >= front_offset_) return false;
      if (end_offset_.compare_exchange_weak(end, end + 1)) {
        t = std::move(buffer_[end%capacity_]);
//        memset(&buffer_[end%capacity_], -1, sizeof(T));
        return true;
      } else {
        continue;
      }
    }
  }
  inline bool Empty() { return 0 == Size(); }
  inline int64_t Size() { return front_offset_-end_offset_;}

 private:
  const int64_t capacity_;
 public:
  atomic<int64_t> front_offset_;   // the offset of the next data to store
  atomic<int64_t> end_offset_;     // the offset of the last stored data
 private:
  unique_ptr<T[]> buffer_;

  // FOR DEBUG
 public:
  int ids[100000];
  bool bids[100000];
};

template<>
inline bool FixSizeLockFreeQueue<int>::Push(const int& t) {
  while(true) {
    int64_t front = front_offset_;
    if (front - end_offset_ + 1 > capacity_ ) return false;
    if (front_offset_.compare_exchange_weak(front, front+1)){
      buffer_[front%capacity_] = t;

      /*// FOR DEBUG
      assert(buffer_[front%capacity_] != -1);
      if (bids[front%capacity_]) assert(false&&"must not be initialized");
      bids[front%capacity_] = true;
      if (ids[front%capacity_] == 0) {
        ids[front%capacity_]= t;
      } else {
        std::cout<<"ids["<<front<<"] has value:"<<ids[front%capacity_]<<std::endl;
      }*/

      return true;
    } else {
      continue;
    }
  }
}

template<>
bool FixSizeLockFreeQueue<int>::Pop(int& t){
  while (true) {
    int64_t end = end_offset_;
    if (end >= front_offset_) return false;
    if (end_offset_.compare_exchange_weak(end, end + 1)) {
      t = buffer_[end%capacity_];

      /*// FOR DEBUG
      if (10000 == t) {
        std::cout<<"got data whose value is 10000, while ids["<<end%capacity_<<"]:"<<ids[end%capacity_]<<std::endl;
        std::cout<<"bids["<<end%capacity_<<"]:"<<bids[end%capacity_]<<std::endl;
        std::cout<<pthread_self()<<", end:"<<end<<std::endl;
//        assert(false);
      }*/
//      buffer_[end%capacity_] = -1;
      return true;
    } else {
      continue;
    }
  }
}


#endif /* LockFreeQueue_FIXSIZELOCKFREEQUEUE_H_ */
