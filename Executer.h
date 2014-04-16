//
//  Executer.h
//  AysncIO
//
//  Created by Yanjiu Huang on 4/8/14.
//  Copyright (c) 2014 Yanjiu Huang. All rights reserved.
//

#ifndef __AysncIO__Executer__
#define __AysncIO__Executer__

#include <iostream>
#include <thread>
#include <future>
#include <atomic>
#include <queue>

#include "ConcurrentQueue.h"
#include "ExecuterTasks.h"

using namespace std;

#define EXECUTER_STATE_NOT_START    0
#define EXECUTER_STATE_STARTED      1
#define EXECUTER_STATE_SHUTTINGDOWN 2
#define EXECUTER_STATE_SHUTDOWN     3

typedef void (*task_handle_t)(void*);

class Task{

public:
    
    Task(){}
    ~Task(){}
    
    virtual void run();
    virtual void setHandle(task_handle_t t);
    virtual void setAttachment(void* obj);
    
    // should take care the copy and move behaviour
    
private:
    task_handle_t   m_handle;
    void*           m_attachment;
    
};



class Executer{

public:
    Executer();
    virtual ~Executer();
    virtual void execute(Task& task);
    virtual void shutdown();
    virtual void start();
    virtual void join();
    
    virtual void scheduleTask(ScheduledTask& task);
    virtual void schdueldTask(task_handle_t handle, long delayNanos, bool isRepeated);
    
    // add scheduled task
    // add thread-safe queue
    
protected:
    virtual void init();
    virtual void doRun();
    virtual void addTask(Task& t);
    
private:
    
    atomic_int                      m_state;
    ConcurrentQueue<Task>*          m_task_queue; // later to add delayed task queue
    priority_queue<ScheduledTask>*  m_delayed_queue;
    thread*                         m_thread;
    
    
    
};

#endif /* defined(__AysncIO__Executer__) */
