
#include "processor.h"
#include "linux_parser.h"

#include <string>
#include <vector>
#include <iostream>

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    this->UpdateUtilization();
    return this->utilization; 
}

void Processor::UpdateUtilization(){    
    std::vector<std::string> usage = LinuxParser::CpuUtilization();
    // get each time from the utilization & convert it to long from string
    long int user = std::stol(usage[0]) ;
    long int nice = std::stol(usage[1]);
    long int syst = std::stol(usage[2]);
    long int idle = std::stol(usage[3]);
    long int iowait = std::stol(usage[4]);
    long int irq = std::stol(usage[5]);
    long int softirq = std::stol(usage[6]);
    long int steal = std::stol(usage[7]);
    long int guest = std::stol(usage[8]);
    long int guest_nice = std::stol(usage[9]);
    //check the previous utilization (may be a second ago?)
    if(this->prev_utiliz.size()== 0){
        // prev vector is not initialized, 
        // intialize it here instead of using a constructor
        for(size_t i=0;i<usage.size();++i){
            this->prev_utiliz.push_back(0);
        }        
    }
    // The calcuation below is based on the following StackOverflow post.
    // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux?noredirect=1&lq=1
    user -= guest;
    nice -= guest_nice;

    long int curr_idle_time = idle + iowait;
    long int curr_nonidle_time = user + nice + syst + irq + softirq + steal;
    long int curr_total_time = curr_idle_time + curr_nonidle_time;

    long int prev_idle_time = this->prev_utiliz[3] + this->prev_utiliz[4];    
    long int prev_nonidle_time = this->prev_utiliz[0] + this->prev_utiliz[2] + 
                                 this->prev_utiliz[5] + this->prev_utiliz[6] + 
                                 this->prev_utiliz[7];                                 
    long int prev_total_time = prev_idle_time + prev_nonidle_time;

    long int total_diff = curr_total_time - prev_total_time;
    long int idle_diff = curr_idle_time - prev_idle_time;
    
    float cpu_usage = (total_diff - idle_diff)*1.0/total_diff;    
    
    //save the current stat to prev_utiliz property
    for(size_t i=0;i<usage.size();++i){
        this->prev_utiliz[i] = std::stol(usage[i]);
    }
    this->utilization = cpu_usage;
}