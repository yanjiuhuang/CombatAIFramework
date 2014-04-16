//
//  ExecuterTasks.h
//  AysncIO
//
//  Created by Yanjiu Huang on 4/9/14.
//  Copyright (c) 2014 Yanjiu Huang. All rights reserved.
//

#ifndef __AysncIO__ExecuterTasks__
#define __AysncIO__ExecuterTasks__

#include <iostream>
#include <chrono>

#include "Executer.h"

using namespace std;
using namespace std::chrono;

class ScheduledTask : public Task{
    
public:
    ScheduledTask();
    ScheduledTask(task_handle_t handle, long delayedNanos, bool is_repeated = false);
    ~ScheduledTask();
    
    /**
     *  if target's next execution time is after itself's execution, return true.
     *
     */
    bool operator>(const ScheduledTask& target);
    
    virtual void run();
    bool isRepeatable(){ return m_is_repeated; }
    
private:
    
    void updateExecutionTime();
    time_point<high_resolution_clock>   getNexExecutionTime() const;
    
    long                                m_delaynanos;
    time_point<high_resolution_clock>   m_next_execute_nanos;
    time_point<high_resolution_clock>   m_start_time;
    bool                                m_is_repeated;
    
};

#endif /* defined(__AysncIO__ExecuterTasks__) */
