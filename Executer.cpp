//
//  Executer.cpp
//  AysncIO
//
//  Created by Yanjiu Huang on 4/8/14.
//  Copyright (c) 2014 Yanjiu Huang. All rights reserved.
//

#include <unistd.h>

#include "Executer.h"

Executer::Executer(){
    init();
}

Executer::~Executer(){
    if(m_task_queue){
        delete m_task_queue;
    }
    
    if(m_thread){
        delete m_thread;
    }
}

void Executer::init(){
    m_task_queue = new ConcurrentQueue<Task>(256);
    std::atomic_init(&m_state, EXECUTER_STATE_NOT_START);
    m_delayed_queue = new priority_queue<ScheduledTask>;
    
}

void Executer::execute(Task& task){
    
    int oldState = atomic_load(&m_state);
    if(oldState >= EXECUTER_STATE_SHUTTINGDOWN){
        // start the thread again
        start();
    }
    
    if(atomic_load(&m_state) != EXECUTER_STATE_STARTED){
        cerr << "Can't start the Executor!" << endl;
        return;
    }
    
    addTask(task);
}

void Executer::shutdown(){
    atomic_store(&m_state, EXECUTER_STATE_SHUTTINGDOWN);
}

void Executer::start(){
    atomic_store(&m_state, EXECUTER_STATE_STARTED);
    if(m_thread){
        delete m_thread;
    }
    m_thread = new thread(&Executer::doRun, this);
}



void Executer::doRun(){
    
    if(atomic_load(&m_state) != EXECUTER_STATE_STARTED){
        return;
    }
    
    for(;;){
        if(m_task_queue->isEmpty()){
            // just sleep
            usleep(1000);
            continue;
        }
        
        
        Task t = m_task_queue->wait_to_take();
        t.run();
        
        if(atomic_load(&m_state) == EXECUTER_STATE_SHUTTINGDOWN){
            if(m_task_queue->isEmpty()){
                atomic_store(&m_state, EXECUTER_STATE_SHUTDOWN);
                return;
            }
        }
        
    }
}

void Executer::join(){
    if (m_thread && m_thread->joinable()) {
        //cout << "Try to join the executer here!" << endl;
        m_thread->join();
    }
}

void Executer::addTask(Task& t){
    if(atomic_load(&m_state) == EXECUTER_STATE_STARTED && m_task_queue){
        m_task_queue->wait_to_put(t);
    }
}

void Task::run(){
    if(m_handle){
        m_handle(m_attachment);
    }
}

void Task::setAttachment(void *obj){
    m_attachment = obj;
}

void Task::setHandle(task_handle_t handle){
    m_handle = handle;
}


