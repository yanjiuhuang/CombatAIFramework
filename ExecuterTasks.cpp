//
//  ExecuterTasks.cpp
//  AysncIO
//
//  Created by Yanjiu Huang on 4/9/14.
//  Copyright (c) 2014 Yanjiu Huang. All rights reserved.
//

#include "ExecuterTasks.h"

ScheduledTask::ScheduledTask(){
    m_is_repeated = false;
}

ScheduledTask::ScheduledTask(task_handle_t handle, long delayedNanos, bool is_repeated)
:m_delaynanos(delayedNanos), m_is_repeated(is_repeated){
    
    setHandle(handle);
    m_start_time = chrono::high_resolution_clock::now();
    chrono::nanoseconds delayOffset(delayedNanos);
    m_next_execute_nanos = m_start_time + delayOffset;
    
}

bool ScheduledTask::operator>(const ScheduledTask& t){
    return this->getNexExecutionTime() > t.getNexExecutionTime();
}

time_point<high_resolution_clock> ScheduledTask::getNexExecutionTime() const{
    return m_next_execute_nanos;
}

void ScheduledTask::run(){
    
    Task::run();
    
    if(m_is_repeated){
        updateExecutionTime();
    }
    
}



