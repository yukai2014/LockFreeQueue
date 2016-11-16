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
 * FixSizeLockFreeQueueTest.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: yukai
 *		 Email: yukai2014@gmail.com
 * 
 * Description:
 *
 */

#include <iostream>
#include <thread>
#include <string.h>
#include "./FixSizeLockFreeQueue.h"
using namespace std;

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

int case_id = 0;
#define OUTPUT_CASE_RESULT(success, test_name) \
    do{\
      if (!success) { \
        cout<<RED<<"test case "<<case_id << ": "<<test_name<<" failed"<<BLACK<<endl; \
        assert(false); \
      } \
      else { \
        cout<<GREEN<<"test case "<<case_id<<": "<<test_name<<" passed"<<BLACK<<endl; \
      } \
      ++case_id; \
      cout<<"===================================="<<endl<<endl; \
    } while(0);


int main(){

  int repeat_time = 1000;
  int number = 100;
  {
    bool success = true;
    FixSizeLockFreeQueue<int> lf_queue(128);
    if (!lf_queue.Empty() || lf_queue.Size() != 0) success = false;
    OUTPUT_CASE_RESULT(success, "get_size_of_empty_queue");
  }

  number = 100;
  {
    bool success = true;
    FixSizeLockFreeQueue<int> lf_queue(128);
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    if (lf_queue.Empty() || lf_queue.Size() != number) success = false;
    OUTPUT_CASE_RESULT(success, "get_queue_size_after_pushing_data");
  }

  number= 100;
  {
    bool success = true;
    FixSizeLockFreeQueue<int> lf_queue(101);
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    int res = -1;
    for (int i = 0; i < number; ++i) {
      if (!lf_queue.Pop(res) && i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    OUTPUT_CASE_RESULT(success, "push_less_data_than_init_size");
  }

  number = 121;
  {
    bool success = true;
    int init_size = 120;
    FixSizeLockFreeQueue<int> lf_queue(init_size);
    for(int i = 0; i < init_size; ++i) {
      lf_queue.Push(i);
    }

    if (lf_queue.Push(init_size+1)) assert(false && "should return false");

    int res= -1;
    for (int i = 0; i < init_size; ++i) {
      if (!lf_queue.Pop(res) && i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    OUTPUT_CASE_RESULT(success, "push_more_data_than_init_size");
  }

  number = 260;
  {
    bool success = true;
    FixSizeLockFreeQueue<int> lf_queue(260);
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    int res= -1;
    for (int i = 0; i < number; ++i) {
      if (!lf_queue.Pop(res) && i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    OUTPUT_CASE_RESULT(success, "push_same_size_of_data_with_init_size");
  }

  number = 100;
  {
    bool success = true;
    FixSizeLockFreeQueue<int> lf_queue;
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    int res= -1;
    for (int i = 0; i < number; ++i) {
      if (!lf_queue.Pop(res) && i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }

    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    for (int i = 0; i < number; ++i) {
      if (!lf_queue.Pop(res) && i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    OUTPUT_CASE_RESULT(success, "push_and_pop_less_data_than_init_size_twice");
  }

  number = 2500;
  {
    bool success = true;
    FixSizeLockFreeQueue<int*> lf_queue(2500);
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(new int(i));
    }

    int *res= nullptr;
    for (int i = 0; i < number; ++i) {
      if (!lf_queue.Pop(res) && i != (*res)){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<*res<<endl;
        success = false;
      }
    }
    OUTPUT_CASE_RESULT(success, "queue storing pointer point to int type");
  }

  number = 1000;
  {
    bool success = true;

    int thread_num = 5;
    int init_cap = thread_num * number;

    for (int t = 0; t < repeat_time; ++t) {
      FixSizeLockFreeQueue<int> lf_queue(init_cap);
      std::thread **td = static_cast<thread**>(malloc(sizeof(thread*) * thread_num));
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td[th_num]= new thread([&lf_queue, number](){
          for(int i = 0; i < number; ++i) {
            if (!lf_queue.Push(i)) assert(false && "meeting wrong when pushing data");
          };
        });
      }

      for (int th_num = 0; th_num< thread_num; ++th_num){
        (*td[th_num]).join();
      }
      int* count = new int[number];
      int before_value = -1, after_value = before_value + thread_num;

      memset(count , before_value, sizeof(int) * number);
      cout<<"size of queue is:"<<lf_queue.Size()<<endl;
      cout<<"front offset is: "<<lf_queue.front_offset_<<", end offset is: "<<lf_queue.end_offset_<<endl;
      assert(lf_queue.Size() == thread_num* number && "wrong size after pushing data");
      int res = -1;
      while(lf_queue.Pop(res)) {
        if (res >= number) {
          cout<<"error value"<<endl;
          success = false;
          break;
        } else {
          count[res]++;
        }
      }
      cout <<endl;
      cout<<"front offset is: "<<lf_queue.front_offset_<<", end offset is: "<<lf_queue.end_offset_<<endl;
      for (int i = 0; i < number; ++i) {
        if(count[i] != after_value) {
          cout<<"lost some data or duplicate some data, the "<<i<<"'s count is:"<<count[i]<<endl;
          success = false;
          //        break;
        }
      }

      if (!success) {
        cerr<<"test case "<<case_id << ": "<<"multi-thread_pushing"<<" failed"<<endl;
        break;
      }
      //    else cout<<GREEN"test case 7: multi-thread input passed"<<BLACK<<endl;
    }
    if (success) cout<<GREEN"test case "<<case_id<<": multi-thread input passed "<<repeat_time<<" time "<<BLACK<<endl;
  }
  case_id++;
  cout<<"===================================="<<endl<<endl;

  number = 1000;
  {
    bool success = true;

    int thread_num = 5;
    int init_cap = thread_num * number;

    for (int t = 0; t < repeat_time; ++t) {
      FixSizeLockFreeQueue<int> lf_queue(init_cap);
      for(int i = 0; i < init_cap; ++i) {
        if (!lf_queue.Push(i%number)) assert(false && "meeting wrong when pushing data");
      };
      assert(lf_queue.Size() == thread_num* number && "wrong size after pushing data");

      int before_value = -1, after_value = before_value + thread_num;
      int* count = new int[number];
      memset(count , before_value, sizeof(int) * number);

      std::thread **td = static_cast<thread**>(malloc(sizeof(thread*) * thread_num));
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td[th_num]= new thread([&lf_queue, &count, init_cap](){
          int res = -1;
          while(lf_queue.Pop(res)) {
            if (res >= init_cap) {
              cout<<"error value"<<endl;
              assert(false);
            } else {
              __sync_fetch_and_add(&count[res], 1);
            }
          }
        });
      }

      for (int th_num = 0; th_num< thread_num; ++th_num) (*td[th_num]).join();

      for (int i = 0; i < number; ++i) {
        if(count[i] != after_value) {
          cout<<"lost some data or duplicate some data, the "<<i<<"'s count is:"<<count[i]<<endl;
          success = false;
        }
      }

      if (!success) {
        cerr<<"test case "<<case_id << ": "<<"multi-thread_poping"<<" failed"<<endl;
        break;
      }
    }
    if (success) cout<<GREEN"test case "<<case_id << ": multi-thread output passed "<<repeat_time<<" time "<<BLACK<<endl;
  }
  case_id++;
  cout<<"===================================="<<endl<<endl;

  return 0;
}


