//============================================================================
// Name        : LockFreeQueue.cpp
// Author      : Yu,Kai
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "./LockFreeQueue.h"
using namespace std;

int main() {
  cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

  const int repeat_time = 10000;

  int number = 10000;
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
    else cout<<"test case with number:"<<number<<" passed"<<endl;
  }

  return 0;
}
