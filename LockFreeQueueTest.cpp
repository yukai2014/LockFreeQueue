//============================================================================
// Name        : LockFreeQueue.cpp
// Author      : Yu,Kai
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <thread>
#include <string.h>
#include "./LockFreeQueue.h"
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

int main() {
  cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

  int repeat_time = 1000;
  int number = 100;
  {
    bool success = true;
    LockFreeQueue<int> lf_queue;
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    for (int i = 0; i < number; ++i) {
      int res = lf_queue.Pop();
      if (i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    if (!success) cerr<<"test case 1 failed"<<endl;
    else {
      cout<<GREEN<<"test case 1 with number:"<<number<<" passed"<<BLACK<<endl;
      assert(false);
    }
  }
  cout<<"===================================="<<endl<<endl;

  number = 10000;
  {
    bool success = true;
    LockFreeQueue<int> lf_queue;
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    for (int i = 0; i < number; ++i) {
      int res = lf_queue.Pop();
      if (i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    if (!success) cerr<<"test case 2 failed"<<endl;
    else cout<<GREEN<<"test case 2 with number:"<<number<<" passed"<<BLACK<<endl;
  }
  cout<<"===================================="<<endl<<endl;

  number = 10000;
  {
    bool success = true;
    LockFreeQueue<int> lf_queue;
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    for (int i = 0; i < number; ++i) {
      int res = lf_queue.Pop();
      if (i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }

    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    for (int i = 0; i < number; ++i) {
      int res = lf_queue.Pop();
      if (i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    if (!success) cerr<<"test case 3 failed"<<endl;
    else cout<<GREEN<<"test case 3 with number:"<<number<<" passed"<<BLACK<<endl;
  }
  cout<<"===================================="<<endl<<endl;

  number = 10000;
  {
    bool success = true;
    LockFreeQueue<int> lf_queue(10000);
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    for (int i = 0; i < number; ++i) {
      int res = lf_queue.Pop();
      if (i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    if (!success) cerr<<"test case 4 failed"<<endl;
    else cout<<GREEN<<"test case 4 with number:"<<number<<" passed"<<BLACK<<endl;
  }
  cout<<"===================================="<<endl<<endl;

  number = 10000;
  {
    bool success = true;
    LockFreeQueue<int> lf_queue(9999);
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    for (int i = 0; i < number; ++i) {
      int res = lf_queue.Pop();
      if (i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    if (!success) cerr<<"test case 5 failed"<<endl;
    else cout<<GREEN<<"test case 5 with number:"<<number<<" passed"<<BLACK<<endl;
  }
  cout<<"===================================="<<endl<<endl;

  number = 10000;
  {
    bool success = true;
    LockFreeQueue<int> lf_queue(200, [](int size)->int {return size*2.5;});
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(i);
    }

    for (int i = 0; i < number; ++i) {
      int res = lf_queue.Pop();
      if (i != res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<res<<endl;
        success = false;
      }
    }
    if (!success) cerr<<"test case 6 with 2.5 rate expand function failed"<<endl;
    else cout<<GREEN<<"test case 6 with 2.5 rate expand function passed"<<BLACK<<endl;
  }
  cout<<"===================================="<<endl<<endl;

  number = 10000;
  {
    bool success = true;
    LockFreeQueue<int*> lf_queue(2500, [](int size)->int {return size*2;});
    for(int i = 0; i < number; ++i) {
      lf_queue.Push(new int(i));
    }

    for (int i = 0; i < number; ++i) {
      int* res = lf_queue.Pop();
      if (i != *res){
        cout<<"inconsistent value! expected value:"<<i<<", got value:"<<*res<<endl;
        success = false;
      }
    }
    if (!success) cerr<<"test case 7 with queue storing pointer to int type failed"<<endl;
    else cout<<GREEN<<"test case 7 with number:"<<number<<" passed"<<BLACK<<endl;
  }
  cout<<"===================================="<<endl<<endl;

  number = 100;
  { // failed in this case
    bool success = true;

    int thread_num = 5;
    int init_cap = thread_num * number;

    for (int t = 0; t < repeat_time; ++t) {
      LockFreeQueue<int> lf_queue(init_cap);
      std::thread **td = static_cast<thread**>(malloc(sizeof(thread*) * thread_num));
      for (int th_num = 0; th_num< thread_num; ++th_num){
        td[th_num]= new thread([&lf_queue, number](){
          for(int i = 0; i < number; ++i) {
            lf_queue.Push(i);
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
      while(!lf_queue.Empty()) {    // maybe two threads observe that queue has just one data left, so consider it's not empty and pop data. So this program Boom~ crashes!
        int res = lf_queue.Pop();
//        cout<<res<<", ";
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
        cout<<RED<<"test case 7:multi-thread input failed in "<<t<<" time"<<BLACK<<endl;
        break;
      }
      //    else cout<<GREEN"test case 7: multi-thread input passed"<<BLACK<<endl;
    }
    if (success) cout<<GREEN"test case 7: multi-thread input passed "<<repeat_time<<" time "<<BLACK<<endl;

  }
  cout<<"===================================="<<endl<<endl;


  return 0;
}
