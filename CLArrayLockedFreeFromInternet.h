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
 * CLArrayLockedFreeFromInternet.h
 *
 *  Created on: Nov 19, 2016
 *      Author: yukai
 *		 Email: yukai2014@gmail.com
 * 
 * Description:
 *
 */

#ifndef LockFreeQueue_CLARRAYLOCKEDFREEFROMINTERNET_H_
#define LockFreeQueue_CLARRAYLOCKEDFREEFROMINTERNET_H_


#include <iostream>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>


#define CAS(ptr , oldvalue , newvalue) __sync_bool_compare_and_swap(ptr , oldvalue , newvalue)
#define FULL false
#define EMPTY false
#define DEFAULTSIZE 128

template<typename T , uint32_t arraysize = DEFAULTSIZE>
class CLArrayLockedFree
{
public:

    CLArrayLockedFree();
    bool push(T);
    bool pop(T& t);

private:

    T m_Queue[arraysize];
    uint32_t front_offset_;
    uint32_t end_offset_;
    uint32_t written_offset_;

    inline uint32_t countToIndex(uint32_t);
};

template<typename T , uint32_t arraysize>
CLArrayLockedFree<T , arraysize>::CLArrayLockedFree()
{
    front_offset_ = 0;
    end_offset_ = 0;
    written_offset_ = 0;
}


template<typename T , uint32_t arraysize>
inline uint32_t
CLArrayLockedFree<T , arraysize>::countToIndex(uint32_t count)
{
    return (count%arraysize);
}

template<typename T , uint32_t arraysize>
bool
CLArrayLockedFree<T , arraysize>::push(T element)
{
    uint32_t front;
    uint32_t end;

    do
    {
        end = end_offset_;
        front = front_offset_;


        if(countToIndex(front + 1) == countToIndex(end))
            return FULL;

        if(!CAS(&front_offset_ , front , front + 1))
            continue;

        m_Queue[countToIndex(front)] = element;
        break;

    }while(1);

    while(!CAS(&written_offset_ , front , front + 1))
    {
        sched_yield();
    }

    return true;
}

template<typename T , uint32_t arraysize>
bool
CLArrayLockedFree<T , arraysize>::pop(T& t)
{
    uint32_t end;

    do
    {
        end = end_offset_;

        if(countToIndex(end) == countToIndex(written_offset_))
            return false;

        t = m_Queue[countToIndex(end)];

        if(!CAS(&end_offset_ , end , end + 1))
            continue;

        return true;

    }while(1);
}

#endif /* LockFreeQueue_CLARRAYLOCKEDFREEFROMINTERNET_H_ */
