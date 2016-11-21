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
#include <queue>
#include <string.h>
#include <thread>
#include <unistd.h>
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
int failed_case_num = 0;

#define PRINT_RESULT(test_name) PrintTestResult(__LINE__, success, test_name)
#define PRINT_RESULT_R(test_name, repeat_time) PrintTestResult(__LINE__, success, test_name, repeat_time);

inline void PrintTestResult(int line_no, bool success, const char* const test_name, int repeat_time = -1){
  if (!success) {
    cout<<RED<<"LINE: "<<line_no<<". test case "<<case_id << ": "<<test_name<<" failed"<<BLACK<<endl;
    failed_case_num++;
    assert(false);
  }
  else {
    if (repeat_time == -1)
      cout<<GREEN<<"LINE: "<<line_no<<". test case "<<case_id<<": "<<test_name<<" passed"<<BLACK<<endl;
    else
      cout<<GREEN<<"LINE: "<<line_no<<". test case "<<case_id<<": "<<test_name<<" passed, repeated "<<repeat_time<<" times"<<BLACK<<endl;
  }
  ++case_id;
  cout<<"===================================="<<endl<<endl;
}

int main(){

  int repeat_time = 1000;
  int number = 10000;

  {
    bool success = true;
    FixSizeLockFreeQueue<int> lf_queue(128);
    if (!lf_queue.Empty() || lf_queue.Size() != 0) success = false;
    PRINT_RESULT("get_size_of_empty_queue");
  }

  number = 100;
  {
    bool success = true;
    FixSizeLockFreeQueue<int> lf_queue(128);
    for(int i = 0; i < number; ++i) {
      if (!lf_queue.Push(i)) {}
    }

    if (lf_queue.Empty() || lf_queue.Size() != number) success = false;
    PRINT_RESULT( "get_queue_size_after_pushing_data");
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
    PRINT_RESULT( "push_less_data_than_init_size");
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
    PRINT_RESULT("push_more_data_than_init_size");
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
    PRINT_RESULT("push_same_size_of_data_with_init_size");
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
    PRINT_RESULT("push_and_pop_less_data_than_init_size_twice");
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
      if (lf_queue.Pop(res)){
        if ( i != (*res)){
          cout<<"inconsistent value! expected value:"<<i<<", got value:"<<*res<<endl;
          success = false;
        }
      } else {
        cout<<"can't pop data"<<endl;
        success = false;
      }
      delete res;
      res = nullptr;
    }
    PRINT_RESULT("queue storing pointer point to int type");
  }

  number = 1000;
  {
    bool success = true;

    int thread_num = 5;
    int init_cap = thread_num * number;
    int before_value = -1, after_value = before_value + thread_num;
    int* count = new int[number];

    for (int t = 0; t < repeat_time; ++t) {
      memset(count , before_value, sizeof(int) * number);
      FixSizeLockFreeQueue<int> lf_queue(init_cap);
      std::thread **td = new thread*[thread_num];
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td[th_num]= new thread([&lf_queue, number](){
          for(int i = 0; i < number; ++i) {
            if (!lf_queue.Push(i)) assert(false && "meeting wrong when pushing data");
          };
        });
      }

      for (int th_num = 0; th_num< thread_num; ++th_num){
        (*td[th_num]).join();
        delete td[th_num];
      }
      delete[] td;
      //      cout<<"size of queue is:"<<lf_queue.Size()<<endl;
      //      cout<<"front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<", written offset is: "<<lf_queue.written_off()<<endl;
      assert(lf_queue.Size() == thread_num * number && "wrong size after pushing data");
      int res = -1;
      while(lf_queue.Pop(res)) {
        if (res >= number || res < 0) {
          cout<<"error value"<<endl;
          success = false;
          break;
        } else {
          count[res]++;
        }
      }
      for (int i = 0; i < number; ++i) {
        if(count[i] != after_value) {
          cout<<"lost some data or duplicate some data, the "<<i<<"'s count is:"<<count[i]<<", expected value is:"<<after_value<<endl;
          success = false;
        }
      }

      if (!success){
        cout<<"after poping, front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<", written offset is: "<<lf_queue.written_off()<<endl;
        break;
      }
    }
    delete[] count;
    PRINT_RESULT_R("multi-thread input_then_pop_all", repeat_time);
  }

  number = 1000;
  { // case 8
    bool success = true;

    int thread_num = 5;
    int init_cap = thread_num * number;
    int before_value = -1, after_value = before_value + thread_num;
    int* count = new int[number];


    for (int t = 0; t < repeat_time; ++t) {
      memset(count , before_value, sizeof(int) * number);
      FixSizeLockFreeQueue<int> lf_queue(init_cap);
      for(int i = 0; i < init_cap; ++i) {
        if (!lf_queue.Push(i%number)) assert(false && "meeting wrong when pushing data");
      }
      assert(lf_queue.Size() == thread_num* number && "wrong size after pushing data");

      std::thread **td = new thread*[thread_num];
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td[th_num]= new thread([&lf_queue, &count, number](){
          int res = -1;
          while(lf_queue.Pop(res)) {
            if (res >= number || res < 0) {
              cout<<"error value"<<endl;
              assert(false);
            } else {
              __sync_fetch_and_add(&count[res], 1);
            }
          }
        });
      }

      for (int th_num = 0; th_num< thread_num; ++th_num) {(*td[th_num]).join(); delete td[th_num]; }
      delete[] td;
      if (!lf_queue.Empty())
        cout<<"front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<", written offset is: "<<lf_queue.written_off()<<endl;
      assert(lf_queue.Empty());

      for (int i = 0; i < number; ++i) {
        if(count[i] != after_value) {
          cout<<"lost some data or duplicate some data, the "<<i<<"'s count is:"<<count[i]<<", expected value is:"<<after_value<<endl;
          success = false;
        }
      }
      if (!success) {
        cout<<"front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<endl;
        break;
      }
    }
    delete[] count;
    PRINT_RESULT_R("input_all_then_multi-thread_output", repeat_time);
  }

  number = 1000;
  {   // case 9
    bool success = true;

    int input_thread_num = 5;
    int output_thread_num = 1;
    int init_cap = input_thread_num * number;
    int before_value = -1, after_value = before_value + input_thread_num, popped_num = 0;
    int* count = new int[number+1];

    for (int t = 0; t < repeat_time; ++t) {
      memset(count , before_value, sizeof(int) * number);
      FixSizeLockFreeQueue<int> lf_queue(init_cap);

      std::thread **td1 = new thread*[input_thread_num];
      for (int th_num = 0; th_num< input_thread_num; ++th_num){
        td1[th_num]= new thread([&lf_queue, number](){
          for(int i = 0; i < number; ++i) {
            if (!lf_queue.Push(i)) assert(false && "meeting wrong when pushing data");
          };
        });
      }
      usleep(1);   // make all producer thread starts as earlier than consumer threads as possible
      std::thread **td2 = new thread*[output_thread_num];
      for (int th_num = 0; th_num< output_thread_num; ++th_num){
        td2[th_num]= new thread([&lf_queue, &count,&popped_num, number, input_thread_num](){
          while(popped_num < number*input_thread_num){
            int res = -1;
            while(lf_queue.Pop(res)) {
              if (res >= number || res < 0) {
                cout<<"error value:"<<res<<endl;
                //              assert(false);
              } else {
                __sync_fetch_and_add(&count[res], 1);
                __sync_fetch_and_add(&popped_num, 1);
              }
            }
          }
        });
      }
      for (int th_num = 0; th_num< input_thread_num; ++th_num){ (*td1[th_num]).join(); delete td1[th_num]; }
      for (int th_num = 0; th_num< output_thread_num; ++th_num) { (*td2[th_num]).join(); delete td2[th_num]; }
      delete[] td1;
      delete[] td2;
      //      std::cout<<"checking bids..."<<std::endl;
      //      for (int i = 0; i < init_cap; ++i){
      //        if (lf_queue.bids[i] != true){
      //          std::cout<<"front_offset lost "<<i<<std::endl;
      //          assert(false);
      //        }
      //      }

      //      if (!lf_queue.Empty()) {
      //        cout<<"front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<", written offset is: "<<lf_queue.written_off()<<endl;
      //        cout<<"need to collect the rest of data"<<endl;
      //      }

      { // collect the rest of data in case that consumer threads finished earlier than producer threads
        int res = -1;
        while(lf_queue.Pop(res)) {
          if (res >= number || res < 0) {
            cout<<"error value"<<endl;
            assert(false);
          } else {
            __sync_fetch_and_add(&count[res], 1);
            //            if (res == 0) cout<<lf_queue.end()<<endl;
          }
        }
      }
      //      cout<<"----------------------------"<<endl;
      assert(lf_queue.Empty());

      for (int i = 0; i < number; ++i) {
        if(count[i] != after_value) {
          cout<<"lost some data or duplicate some data, the "<<i<<"'s count is:"<<count[i]<<", expected value is:"<<after_value<<endl;
          success = false;
        }
      }

      if (!success){
        cout<<"front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<endl;
        break;
      }
    }
    delete[] count;
    PRINT_RESULT_R("multi-thread_pushing_and_poping_into_big_enough_queue", repeat_time);
  }

  number = 1000;
  {   // case 10
    bool success = true;

    int thread_num = 5;
    int init_cap = thread_num * number / 2;
    int before_value = -1, after_value = before_value + thread_num, popped_num;
    int* count = new int[number];
    for (int t = 0; t < repeat_time; ++t) {
      popped_num = 0;
      memset(count , before_value, sizeof(int) * number);
      FixSizeLockFreeQueue<int> lf_queue(init_cap);

      std::thread **td1 = new thread*[thread_num];
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td1[th_num]= new thread([&lf_queue, number](){
          for(int i = 0; i < number; ++i) {
            while (!lf_queue.Push(i)){
              sched_yield();
            }
          };
        });
      }
      std::thread **td2 = new thread*[thread_num];
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td2[th_num]= new thread([&lf_queue, &count, &popped_num, thread_num, number](){
          while(popped_num < number*thread_num){
            int res = -1;
            while (lf_queue.Pop(res)) {
              if (res >= number || res < 0) {
                cout<<"error value:"<<res<<endl;
                //              assert(false);
              } else {
                __sync_fetch_and_add(&count[res], 1);
                __sync_fetch_and_add(&popped_num, 1);
              }
            }
            sched_yield();
          }
        });
      }

      for (int th_num = 0; th_num< thread_num; ++th_num) { (*td1[th_num]).join(); delete td1[th_num]; }
      for (int th_num = 0; th_num< thread_num; ++th_num) { (*td2[th_num]).join(); delete td2[th_num]; }
      delete[] td1;
      delete[] td2;
      assert(lf_queue.Empty());

      for (int i = 0; i < number; ++i) {
        if(count[i] != after_value) {
          cout<<"lost some data or duplicate some data, the "<<i<<"'s count is:"<<count[i]<<", expected value is:"<<after_value<<endl;
          success = false;
        }
      }

      if (!success) {
        cout<<"ERROR: front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<endl;
        break;
      }
    }
    delete[] count;
    PRINT_RESULT_R("multi-thread_loop_pushing_and_poping_into_smaller_queue", repeat_time);
  }

  {
    // case 10
     bool success = true;

     int thread_num = 2;
     int init_cap = thread_num * number / 2;
     int before_value = -1, after_value = before_value + thread_num, popped_num;
     int* count = new int[number];
     for (int t = 0; t < repeat_time; ++t) {
       popped_num = 0;
       memset(count , before_value, sizeof(int) * number);
       FixSizeLockFreeQueue<int> lf_queue(init_cap);

       std::thread **td1 = new thread*[thread_num];
       for (int th_num = 0; th_num< thread_num; ++th_num){
         td1[th_num]= new thread([&lf_queue, number](){
           for(int i = 0; i < number; ++i) {
             while (!lf_queue.Push(i)){
               sched_yield();
             }
           };
         });
       }
       std::thread **td2 = new thread*[thread_num];
       for (int th_num = 0; th_num< thread_num; ++th_num){
         td2[th_num]= new thread([&lf_queue, &count, &popped_num, thread_num, number](){
           while(popped_num < number*thread_num){
             int res = -1;
             while (lf_queue.Pop(res)) {
               if (res >= number || res < 0) {
                 cout<<"error value:"<<res<<endl;
                 //              assert(false);
               } else {
                 __sync_fetch_and_add(&count[res], 1);
                 __sync_fetch_and_add(&popped_num, 1);
               }
             }
             sched_yield();
           }
         });
       }

       for (int th_num = 0; th_num< thread_num; ++th_num) { (*td1[th_num]).join(); delete td1[th_num]; }
       for (int th_num = 0; th_num< thread_num; ++th_num) { (*td2[th_num]).join(); delete td2[th_num]; }
       delete[] td1;
       delete[] td2;
       assert(lf_queue.Empty());

       for (int i = 0; i < number; ++i) {
         if(count[i] != after_value) {
           cout<<"lost some data or duplicate some data, the "<<i<<"'s count is:"<<count[i]<<", expected value is:"<<after_value<<endl;
           success = false;
         }
       }

       if (!success) {
         cout<<"ERROR: front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<endl;
         break;
       }
     }
     delete[] count;
     PRINT_RESULT_R("multi-thread_loop_pushing_and_poping_into_smaller_queue_when_producer_less_than_cores", repeat_time);

  }

  {
    // case 10
    bool success = true;

    int thread_num = 5;
    int init_cap = thread_num * number / 2;
    int before_value = -1, after_value = before_value + thread_num, popped_num = 0;
    int* count = new int[number];

    for (int t = 0; t < repeat_time; ++t) {
      popped_num = 0;
      memset(count , before_value, sizeof(int) * number);
      FixSizeLockFreeQueue<long> lf_queue(init_cap);

      std::thread **td1 = new thread*[thread_num];
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td1[th_num]= new thread([&lf_queue, number](){
          for(int i = 0; i < number; ++i) {
            while (!lf_queue.Push(i)){
              sched_yield();
            }
          };
        });
      }
      std::thread **td2 = new thread*[thread_num];
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td2[th_num]= new thread([&lf_queue, &count, &popped_num, thread_num, number](){
          while(popped_num < number*thread_num){
            long res = -1;
            while (lf_queue.Pop(res)) {
              if (res >= number || res < 0) {
                cout<<"error value:"<<res<<endl;
                //              assert(false);
              } else {
                __sync_fetch_and_add(&count[res], 1);
                __sync_fetch_and_add(&popped_num, 1);
              }
            }
            sched_yield();
          }
        });
      }

      for (int th_num = 0; th_num< thread_num; ++th_num) { (*td1[th_num]).join(); delete td1[th_num]; }
      for (int th_num = 0; th_num< thread_num; ++th_num) { (*td2[th_num]).join(); delete td2[th_num]; }
      delete[] td1;
      delete[] td2;
      assert(lf_queue.Empty());

      for (int i = 0; i < number; ++i) {
        if(count[i] != after_value) {
          cout<<"lost some data or duplicate some data, the "<<i<<"'s count is:"<<count[i]<<", expected value is:"<<after_value<<endl;
          success = false;
        }
      }

      if (!success) {
        cout<<"ERROR: front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<endl;
        break;
      }
    }
    delete[] count;
    PRINT_RESULT_R("multi-thread_loop_pushing_and_poping_into_smaller_queue_with_long_type", repeat_time);
  }

  repeat_time = 1000;
  number = 10000;
  {
    bool success = true;

    int thread_num = 5;
    int init_cap = thread_num * number / 2;


    int before_value = -1, after_value = before_value + thread_num, popped_num = 0;
    int* count = new int[number];

    for (int t = 0; t < repeat_time; ++t) {
      popped_num = 0;
      memset(count , before_value, sizeof(int) * number);
      FixSizeLockFreeQueue<int*> lf_queue(init_cap);

      std::thread **td1 = new thread*[thread_num];
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td1[th_num]= new thread([&lf_queue, number](){
          for(int i = 0; i < number; ++i) {
//            int* p = new int(i);
            while (!lf_queue.Push(new int(i))){
              sched_yield();
            }
          };
        });
      }
      std::thread **td2 = new thread*[thread_num];
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td2[th_num]= new thread([&lf_queue, &count, &popped_num, thread_num, number](){
          while(popped_num < number*thread_num){
            int* res = nullptr;
            while (lf_queue.Pop(res)) {
              if (*res >= number || *res < 0) {
                cout<<"error value:"<<*res<<endl;
                //              assert(false);
              } else {
                __sync_fetch_and_add(&count[*res], 1);
                __sync_fetch_and_add(&popped_num, 1);
              }
              delete res;
              res = nullptr;
            }
            sched_yield();
          }
        });
      }

      for (int th_num = 0; th_num< thread_num; ++th_num) { (*td1[th_num]).join(); delete td1[th_num]; }
      for (int th_num = 0; th_num< thread_num; ++th_num) { (*td2[th_num]).join(); delete td2[th_num]; }
      delete[] td1;
      delete[] td2;
      assert(lf_queue.Empty());

      for (int i = 0; i < number; ++i) {
        if(count[i] != after_value) {
          cout<<"lost some data or duplicate some data, the "<<i<<"'s count is:"<<count[i]<<", expected value is:"<<after_value<<endl;
          success = false;
        }
      }

      if (!success) {
        cout<<"ERROR: front offset is: "<<lf_queue.front()<<", end offset is: "<<lf_queue.end()<<endl;
        break;
      }
    }
    delete[] count;
    PRINT_RESULT_R("multi-thread_loop_pushing_and_poping_into_smaller_queue_with_int_pointer_type", repeat_time);

  }

  cout<<"tested "<<case_id<<" cases"<<endl;
  if (failed_case_num == 0) {
    cout<<GREEN<<"ALL TEST CASES PASSED"<<BLACK<<endl;
  } else {
    cout<<RED<<failed_case_num<<" cases failed !!!!"<<BLACK<<endl;
  }


/*  std::queue<int*> q;
  for (int i = 0; i < repeat_time; ++i) {
     q.push(new int(i));
  }

  while (!q.empty()) {
    int* res = q.front();
    q.pop();
    delete res;
  }*/
  return 0;
}


