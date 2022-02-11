#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() {    
    return cpu_;
}

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    //get the process ids 
    auto pids = LinuxParser::Pids();
    // clear the processes_ vector before adding new processes
    this->processes_.clear();

    for(auto pid: pids){
        Process p;
        p.Pid(pid);
        p.User(LinuxParser::User(pid));
        p.Ram(LinuxParser::Ram(pid));
        p.Command(LinuxParser::Command(pid));
        p.UpTime(LinuxParser::UpTime(pid));
        p.CpuUtilization(LinuxParser::CpuUtilization(pid));
        this->processes_.push_back(p);
    }
    //sort based on cpu usage
    std::sort( this->processes_.begin(),
               this->processes_.end(),
               [](Process &p1, Process &p2){  return p1<p2;  }
             );
    return processes_; 
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() {
    return LinuxParser::Kernel();
}

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization(); 
}

// DONE: Return the operating system name
std::string System::OperatingSystem() {     
    return LinuxParser::OperatingSystem(); 
}

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses(); 
}

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { 
    return LinuxParser::TotalProcesses(); 
}

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { 
    return LinuxParser::UpTime(); 
}