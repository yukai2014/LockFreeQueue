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
 * guard.h
 *
 *  Created on: Nov 9, 2016
 *      Author: yukai
 *		 Email: yukai2014@gmail.com
 * 
 * Description:
 *
 */

#ifndef LockFreeQueue_GUARD_H_
#define LockFreeQueue_GUARD_H_

#include <mutex>
#include <pthread.h>


class SpinLock{
 public:
  SpinLock(){
    pthread_spin_init(&mutex_, 0);
  }
  ~SpinLock(){
    pthread_spin_destroy(&mutex_);
  }
  void lock() {
    pthread_spin_lock(&mutex_);
  }
  void unlock() {
    pthread_spin_unlock(&mutex_);
  }
 private:
  pthread_spinlock_t mutex_;
};

template <typename T>
class LockGuard{
 public:
  LockGuard(T& mutex):mutex_(mutex){
    mutex_.lock();
  }
  ~LockGuard(){
    mutex_.unlock();
  }
 private:
  LockGuard(const LockGuard& other);
  void operator=(const LockGuard& other);
 private:
  T& mutex_;
};

using MutexLockGuard = LockGuard<std::mutex>;
using SpinLockGuard = LockGuard<SpinLock>;

#endif /* LockFreeQueue_GUARD_H_ */
