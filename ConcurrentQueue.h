//
//  ConcurrentQueue.h
//  AysncIO
//
//  Created by Yanjiu Huang on 4/8/14.
//  Copyright (c) 2014 Yanjiu Huang. All rights reserved.
//

#ifndef __AysncIO__ConcurrentQueue__
#define __AysncIO__ConcurrentQueue__

#include <iostream>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <chrono>


using namespace std;

template<typename T>
class ConcurrentQueue{
public:
    
    ConcurrentQueue(int capacity);
    ~ConcurrentQueue();
    
    void pop(T& data);
    void push(const T& data);
    
    const T& front();
    
    void wait_to_put(T& e);
    const T& wait_to_take();
    
    bool wait_to_put_with_timeout(T& e, long timeout);
    bool wait_to_take_with_timeout(T& e, long timeout);
    
    bool isEmpty();
    int size();
    
private:
    
    queue<T>*           m_internal_queue;
    
    mutex               m_put_mutex;
    mutex               m_take_mutex;
    condition_variable  m_put_cond;
    condition_variable  m_take_cond;
    
    atomic_int          m_count;
    int                 m_capacity;
    
};

template<typename T>
ConcurrentQueue<T>::ConcurrentQueue(int capacity)
:m_capacity(capacity){
    
    m_internal_queue = new queue<T>;
    atomic_init(&m_count, 0);
    
}

template<typename T>
ConcurrentQueue<T>::~ConcurrentQueue(){
    if(m_internal_queue){
        delete m_internal_queue;
    }
}

template<typename T>
bool ConcurrentQueue<T>::wait_to_put_with_timeout(T &e, long timeout){
    
    std::unique_lock<mutex> lock(m_put_mutex);
    auto milli_timeout = chrono::milliseconds(timeout);
    
    while(atomic_load(&m_count) >= m_capacity){
        
        auto start = chrono::high_resolution_clock::now();
        if (milli_timeout.count() > 0 && m_put_cond.wait_for(lock, milli_timeout) == cv_status::timeout){
            return false;
        }
        auto finish = chrono::high_resolution_clock::now();
        
        milli_timeout = chrono::duration_cast<chrono::milliseconds>(finish - start);
    }
    
    m_internal_queue->push(e);
    atomic_fetch_add(&m_count, 1);
    
    if(m_count < m_capacity){
        m_put_cond.notify_one();
    }
    
    m_take_cond.notify_one();
    
    
    return true;
}

template<typename T>
bool ConcurrentQueue<T>::wait_to_take_with_timeout(T& e, long timeout){
    
    std::unique_lock<mutex> lock(m_take_mutex);
    auto milli_timeout = chrono::milliseconds(timeout);
    while(atomic_load(&m_count) <= 0){
        auto start = chrono::high_resolution_clock::now();
        if (milli_timeout.count() > 0 && m_take_cond.wait_for(lock, milli_timeout) == cv_status::timeout){
            return false;
        }
        auto finish = chrono::high_resolution_clock::now();
        milli_timeout = chrono::duration_cast<chrono::milliseconds>(finish - start);
    }
    
    e = m_internal_queue->front();
    m_internal_queue->pop();
    atomic_fetch_sub(&m_count, 1);
    
    if(m_count > 0){
        m_take_cond.notify_one();
    }
    
    m_put_cond.notify_one();
    return true;
}

template<typename T>
void ConcurrentQueue<T>::pop(T& data){
    
}

template<typename T>
void ConcurrentQueue<T>::push(const T& data){
    
}

template<typename T>
void ConcurrentQueue<T>::wait_to_put(T& data){
    
    std::unique_lock<mutex> lock(m_put_mutex);
    while (atomic_load(&m_count) >= m_capacity) {
        m_put_cond.wait(lock);
    }
    
    m_internal_queue->push(data);
    atomic_fetch_add(&m_count, 1);
    if(atomic_load(&m_count) < m_capacity){
        m_put_cond.notify_one();
    }
    
    m_take_cond.notify_one();
    
}

template<typename T>
const T& ConcurrentQueue<T>::wait_to_take(){
    
    std::unique_lock<mutex> lock(m_take_mutex);
    
    while (atomic_load(&m_count) == 0) {
        m_take_cond.wait(lock);
    }
    
    const T& t = m_internal_queue->front();
    m_internal_queue->pop();
    atomic_fetch_sub(&m_count, 1);
    if(atomic_load(&m_count) >= 1){
        m_take_cond.notify_one();
    }
    
    m_put_cond.notify_one();
    
    return t;
}

template<typename T>
int ConcurrentQueue<T>::size(){
    return atomic_load(&m_count);
}

template<typename T>
bool ConcurrentQueue<T>::isEmpty(){
    return atomic_load(&m_count) == 0;
}


template<typename T>
class Test{
public:
    Test(){}
    ~Test(){}
    void echo();
};

template<typename T>
void Test<T>::echo(){
    cout << "Hello World!" << endl;
}

template <typename T>
class BlockingQueue
{
public:
    
    T wait_to_take()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        auto item = queue_.front();
        queue_.pop();
        return item;
    }
    
    void wait_to_take(T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        item = queue_.front();
        queue_.pop();
    }
    
    void wait_to_put(const T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
        //mlock.unlock();
        cond_.notify_one();
    }
    
    void wait_to_put(T&& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(std::move(item));
        //mlock.unlock();
        cond_.notify_one();
    }
    
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif /* defined(__AysncIO__ConcurrentQueue__) */
