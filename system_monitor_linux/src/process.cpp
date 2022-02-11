#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// DONE: Return this process's ID
int Process::Pid() { return this->pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const{ 
    return this->cpu_utilization_; 
}

// DONE: Return the command that generated this process
string Process::Command() { return this->command_; }

// DONE: Return this process's memory utilization
string Process::Ram() { return this->ram_; }

// DONE: Return the user (name) that generated this process
string Process::User() { return this->user_; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return this->uptime_; }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process &a) const {
    if(a.CpuUtilization()<this->CpuUtilization())
        return true;
    return false; 
}

//Constructor
Process::Process():
    pid_(0),user_(""),command_(""),ram_(""),uptime_(0),cpu_utilization_(0.0)
{}
//Mutator methods
void Process::User(std::string user){
    this->user_ = user;
}
void Process::Command(std::string cmd){
    this->command_ = cmd;
}
void Process::CpuUtilization(float util){
    this->cpu_utilization_ = util;
}
void Process::Ram(std::string ram){
    this->ram_ = ram;
}
void Process::UpTime(long int up_time){
    this->uptime_ = up_time;
}
void Process::Pid(int pid){
    this->pid_ = pid;
}