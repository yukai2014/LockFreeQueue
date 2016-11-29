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
 * FixSizeLockFreeQueuePerfTest.cpp
 *
 *  Created on: Nov 21, 2016
 *      Author: yukai
 *		 Email: yukai2014@gmail.com
 * 
 * Description:
 *
 */


#include <iostream>
#include <thread>
#include <string.h>
#include <unistd.h>
#include <mutex>
#include <queue>

#include "./FixSizeLockFreeQueue.h"
#include "./Guard.h"
#include "./Timer.h"

using std::cout;
using std::endl;
using std::queue;
using std::thread;

const long operation_time = 100*1000;
const int skip_time = 5;
const int repeat_time = 100;

// #define MUTEX

#ifdef MUTEX
std::mutex queue_mutex;
using MyLockGurad = MutexLockGuard;
#else
SpinLock queue_mutex;
using MyLockGurad = SpinLockGuard;
#endif

double LockFreeQueuePerfTest(const int capacity, const long operations,const  int producers, const int consumers) {
  int popped_num= 0;
  FixSizeLockFreeQueue<int> lf_queue(capacity);

  double current = Timer::GetCurrentMs(), res;

  std::thread **td1 = new thread*[producers];
  for (int th_num = 0; th_num< producers; ++th_num){
    td1[th_num]= new thread([&lf_queue, operations](){
      for(int i = 0; i < operations; ++i) {
        while (!lf_queue.Push(i)){
          sched_yield();
        }
      };
    });
  }
  std::thread **td2 = new thread*[consumers];
  for (int th_num = 0; th_num< consumers; ++th_num){
    td2[th_num]= new thread([&lf_queue, &popped_num, producers, operations](){
      int res = -1;
      while(popped_num < operations*producers){
        while (lf_queue.Pop(res)) {
          __sync_fetch_and_add(&popped_num, 1);

        }
        sched_yield();
      }
    });
  }
  for (int th_num = 0; th_num< producers; ++th_num) { (*td1[th_num]).join(); delete td1[th_num]; }
  for (int th_num = 0; th_num< consumers; ++th_num) { (*td2[th_num]).join(); delete td2[th_num]; }

  res = Timer::GetElapsedTime(current);

  delete[] td1;
  delete[] td2;
  return res;
}

double SingleProducerQueuePerfTest(const int capacity, const long operations,const  int producers, const int consumers) {
  int popped_num= 0;
  SingleProducerQueue<int> lf_queue(capacity);

  double current = Timer::GetCurrentMs(), res;

  std::thread **td1 = new thread*[producers];
  for (int th_num = 0; th_num< producers; ++th_num){
    td1[th_num]= new thread([&lf_queue, operations](){
      for(int i = 0; i < operations; ++i) {
        while (!lf_queue.Push(i)){
          usleep(1);
        }
      };
    });
  }
  std::thread **td2 = new thread*[consumers];
  for (int th_num = 0; th_num< consumers; ++th_num){
    td2[th_num]= new thread([&lf_queue, &popped_num, producers, operations](){
      int res = -1;
      while(popped_num < operations*producers){
        while (lf_queue.Pop(res)) {
          __sync_fetch_and_add(&popped_num, 1);

        }
        usleep(1);
      }
    });
  }
  for (int th_num = 0; th_num< producers; ++th_num) { (*td1[th_num]).join(); delete td1[th_num]; }
  for (int th_num = 0; th_num< consumers; ++th_num) { (*td2[th_num]).join(); delete td2[th_num]; }

  res = Timer::GetElapsedTime(current);

  delete[] td1;
  delete[] td2;
  return res;
}

double QueuePerfTest(const int capacity, const long operations,const  int producers, const int consumers) {
  int popped_num= 0;
  queue<int> queue;

  double current = Timer::GetCurrentMs(), res;
  std::thread **td1 = new thread*[producers];
  for (int th_num = 0; th_num< producers; ++th_num){
    td1[th_num]= new thread([&queue, operations](){
      for(int i = 0; i < operations; ++i) {
        MyLockGurad guard(queue_mutex);
        queue.push(i);
      };
    });
  }
  std::thread **td2 = new thread*[consumers];
  for (int th_num = 0; th_num< consumers; ++th_num){
    td2[th_num]= new thread([&queue, &popped_num, producers, operations](){
      int res;
      while(popped_num < operations*producers){
        if (!queue.empty()) {
          MyLockGurad guard(queue_mutex);
          if (!queue.empty()) {
            res = queue.front();
            queue.pop();
            popped_num++;
          }
        } else {
          sched_yield();
        }
      }
    });
  }
  for (int th_num = 0; th_num< producers; ++th_num) { (*td1[th_num]).join(); delete td1[th_num]; }
  for (int th_num = 0; th_num< consumers; ++th_num) { (*td2[th_num]).join(); delete td2[th_num]; }

  res = Timer::GetElapsedTime(current);

  delete[] td1;
  delete[] td2;
  return res;
}


double OnlyNewDeletePerfTest(const int capacity, const long operations,const  int producers, const int consumers) {
  int popped_num= 0, pushed_num = 0;
  int** queue = new int* [operations * producers];
  memset(queue, 0, operations * producers);

  double current = Timer::GetCurrentMs(), res;
  std::thread **td1 = new thread*[producers];
  for (int th_num = 0; th_num< producers; ++th_num){
    td1[th_num]= new thread([&queue, &pushed_num, operations](){
      for(int i = 0; i < operations; ++i) {
        int index = __sync_fetch_and_add(&pushed_num, 1);
        queue[index] = new int(i);
      };
    });
  }
  std::thread **td2 = new thread*[consumers];
  for (int th_num = 0; th_num< consumers; ++th_num){
    td2[th_num]= new thread([&queue, &popped_num, &pushed_num, producers, operations](){
      while(popped_num < operations*producers - 10){
        if (popped_num < pushed_num-10 ){
          int index = __sync_fetch_and_add(&popped_num, 1);
          if (queue[index]) {
            delete queue[index];
            queue[index] = nullptr;
          }
        } else {
          sched_yield();
        }
      }
    });
  }
  for (int th_num = 0; th_num< producers; ++th_num) { (*td1[th_num]).join(); delete td1[th_num]; }
  for (int th_num = 0; th_num< consumers; ++th_num) { (*td2[th_num]).join(); delete td2[th_num]; }

  res = Timer::GetElapsedTime(current);

  delete[] queue;
  delete[] td1;
  delete[] td2;
  return res;
}


void PerfTest(int capacity, long operations, int producers, int consumers) {

  cout<<"with params: capacity-"<<capacity<<", operation time-"<<operations<<", producers num-"<<producers<<", consumer num-"<<consumers<<endl;
  double   sum = 0.0;
  for (int i = 0; i < skip_time; ++i) {
    QueuePerfTest(capacity, operations, producers, consumers);
  }
  for (int i = 0; i < repeat_time; ++i) {
    sum += QueuePerfTest(capacity, operations, producers, consumers);
  }
  cout<<"STLQueueWithLock: "<<sum<<endl;

  sum = 0.0;
  for (int i = 0; i < skip_time; ++i) {
    LockFreeQueuePerfTest(capacity, operations, producers, consumers);
  }
  for (int i = 0; i < repeat_time; ++i) {
    sum += LockFreeQueuePerfTest(capacity, operations, producers, consumers);
  }
  cout<<"LockFreeQueue: "<<sum<<endl;

  if (producers == 1) {
    sum = 0.0;
    for (int i = 0; i < skip_time; ++i) {
      SingleProducerQueuePerfTest(capacity, operations, producers, consumers);
    }
    for (int i = 0; i < repeat_time; ++i) {
      sum += SingleProducerQueuePerfTest(capacity, operations, producers, consumers);
    }
    cout<<"SingleProducerQueue: "<<sum<<endl;
  }


  sum = 0.0;
  for (int i = 0; i < skip_time; ++i) {
    OnlyNewDeletePerfTest(capacity, operations, producers, consumers);
  }
  for (int i = 0; i < repeat_time; ++i) {
    sum += OnlyNewDeletePerfTest(capacity, operations, producers, consumers);
  }
  cout<<"only new and delete: "<<sum<<endl;
  cout<<"-----------------------------------------------------"<<endl;

}


int main() {
  PerfTest(10000, operation_time, 1, 1);
  PerfTest(10000, operation_time, 1, 2);
  PerfTest(10000, operation_time, 1, 3);
  PerfTest(10000, operation_time, 1, 4);
  PerfTest(10000, operation_time, 1, 5);
  PerfTest(10000, operation_time, 1, 6);
  PerfTest(10000, operation_time, 1, 7);
  PerfTest(10000, operation_time, 1, 8);
  PerfTest(10000, operation_time, 1, 9);
  PerfTest(10000, operation_time, 1, 10);

  PerfTest(10000, operation_time, 2, 1);
  PerfTest(10000, operation_time, 2, 2);
  PerfTest(10000, operation_time, 2, 3);
  PerfTest(10000, operation_time, 2, 4);
  PerfTest(10000, operation_time, 2, 5);
  PerfTest(10000, operation_time, 2, 6);
  PerfTest(10000, operation_time, 2, 7);
  PerfTest(10000, operation_time, 2, 8);

  PerfTest(10000, operation_time, 3, 1);
  PerfTest(10000, operation_time, 3, 2);
  PerfTest(10000, operation_time, 3, 3);
  PerfTest(10000, operation_time, 3, 4);
  PerfTest(10000, operation_time, 3, 5);
  PerfTest(10000, operation_time, 3, 6);

  PerfTest(10000, operation_time, 4, 1);
  PerfTest(10000, operation_time, 4, 2);
  PerfTest(10000, operation_time, 4, 3);
  PerfTest(10000, operation_time, 4, 4);
  PerfTest(10000, operation_time, 4, 5);
  PerfTest(10000, operation_time, 4, 6);

  PerfTest(10000, operation_time, 5, 1);
  PerfTest(10000, operation_time, 5, 2);
  PerfTest(10000, operation_time, 5, 3);
  PerfTest(10000, operation_time, 5, 4);
  PerfTest(10000, operation_time, 5, 5);
  PerfTest(10000, operation_time, 5, 6);

  PerfTest(10000, operation_time, 6, 1);
  PerfTest(10000, operation_time, 6, 2);
  PerfTest(10000, operation_time, 6, 4);
  PerfTest(10000, operation_time, 6, 6);
  return 0;
}

