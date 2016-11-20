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
  const static int64_t DEFAULT_INIT_SIZE = 128;
 public:
  FixSizeLockFreeQueue():FixSizeLockFreeQueue(DEFAULT_INIT_SIZE){}
  FixSizeLockFreeQueue(int64_t init_size)
  :capacity_(init_size), front_offset_(0),end_offset_(0), written_offset_(0), read_offset_(0), buffer_(nullptr){
//    std::cout<<"initialize size is:"<<init_size<<std::endl;
    assert(capacity_ > 0 && "invalid capacity of initialized queue");
    T* buf = static_cast<T*>(malloc(capacity_ * sizeof(T)));
    memset(buf, -1, sizeof(T) * capacity_);
    buffer_= unique_ptr<T[]>(buf);

    memset(ids, 0, sizeof(ids));
    memset((bool*)bids, 0, sizeof(bids));
    for (int i = 0; i < sizeof(bids); ++i) {
      if (bids[i]) {
        assert(false);
      }
    }
  }
  ~FixSizeLockFreeQueue(){
    buffer_.reset();
  }

  // return false if queue is full, otherwise true
  inline bool Push(const T& t) {
    while(true) {
      int64_t front = front_offset_;
      if (front - end_offset_ + 1 > capacity_ ) return false;
      if (CAS(front_offset_,front, front+1)){
        buffer_[front%capacity_] = t;

        while(!CAS(written_offset_,front, front + 1)) {
          sched_yield();
        }
        return true;
      }
    }
  }

  // return false if queue is empty, otherwise true with moving data into t
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
    }
  }
  inline bool Empty() { return 0 == Size(); }
  inline int64_t Size() { return front_offset_-end_offset_;}

 private:
  const int64_t capacity_;
 public:
  int64_t front_offset_;   // the offset of the next data to store
  int64_t end_offset_;     // the offset of the next stored data to read
  int64_t written_offset_;  // the offset of last data stored
  int64_t read_offset_;
 private:
  unique_ptr<T[]> buffer_;

  // FOR DEBUG
 public:
  int ids[100000];
  bool nothing[100000];
  volatile bool bids[100000];
};
/*
template<>
inline bool FixSizeLockFreeQueue<int>::Push(const int& t) {
  int64_t front;
  do {
    front = front_offset_;

    if (front - end_offset_ + 1 > capacity_ )
      return false;

//    if (front - read_offset_ + 1 > capacity_) continue;

  } while (!CAS(front_offset_,front, front+1));

  buffer_[front%capacity_] = t;
  bids[front%capacity_] = true;
  ids[front%capacity_] = t;

  while(!CAS(written_offset_,front, front + 1)) {
    sched_yield();
  }
  return true;
}
*/

/*
template<>
inline bool FixSizeLockFreeQueue<int>::Push(const int& t) {
  int64_t front, end;
  while(true) {
    front = front_offset_;
    end = end_offset_;
    if (front - end + 1 > capacity_ ) return false;
    if (front_offset_.compare_exchange_weak(front, front+1)){
      buffer_[front%capacity_] = t;

      {// FOR DEBUG
        if (bids[front]) {
          std::cout<<pthread_self()<<", end:"<<end_offset_<<", cached front:"<<front<<", front:"<<front_offset_
                           <<", written:"<<written_offset_<<std::endl;
          assert(false&&"must not be initialized");
        }
        bids[front] = true;
        ids[front] = t;
      }

      while(!written_offset_.compare_exchange_weak(front, front + 1)){
//        sched_yield();
      }
      return true;
    }
  }
}*/
/*

template<>
bool FixSizeLockFreeQueue<int>::Pop(int& t){
  while (true) {
    int64_t end = end_offset_;
    if (end >= front_offset_) {
//      std::cout<<"Pop return false. cached end_offset_ is:"<<end
//                <<", front:"<<front_offset_<<std::endl;
      return false;
    }
    if (end >= written_offset_) continue;


     * must get data before CAS operation,
     * otherwise can't guarantee the data to read is rewritten by new push()

    t = buffer_[end%capacity_];

    if (CAS(end_offset_, end, end + 1)) {

//      bids[end%capacity_] = false;
//      ids[end%capacity_] = -3;


       * need not indicate the offset of been read data, if get data before CAS(end_offset_)

//      while(!CAS(read_offset_, end, end+1)){
//        sched_yield();
//      }

      // FOR DEBUG
      if (unlikely(-2 == buffer_[end%capacity_])) {
        std::cout<<"got data whose value is -2, while ids["<<end%capacity_<<"]:"
                 <<ids[end%capacity_]<<std::endl;
        std::cout<<"bids["<<end%capacity_<<"]:"<<bids[end%capacity_]<<std::endl;
        std::cout<<pthread_self()<<", end:"<<end<<", front:"<<front_offset_
                 <<", written:"<<written_offset_<<", capacity:"<<capacity_<<std::endl;
//        assert(false);
      }


//      buffer_[end%capacity_] = -2;
      return true;
    } else {
      continue;
    }
  }
}
*/


#endif /* LockFreeQueue_FIXSIZELOCKFREEQUEUE_H_ */
