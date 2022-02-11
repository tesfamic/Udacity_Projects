#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version,kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float mem_utilized = 0.0;
  long total_mem, free_mem;
  std::string total_mem_label; //MemTotal:
  std::string free_mem_label; //MemFree:
  int line_counter = 0;
  std::string line;
  std::ifstream mem_file(kProcDirectory+kMeminfoFilename);
  if (mem_file.is_open()) {
    //the first two lines are enough to roughly estimate mem. usage
    while (std::getline(mem_file, line) && line_counter<2) {
      std::istringstream linestream(line);
      (line_counter==0)?
        linestream>>total_mem_label>>total_mem:
        linestream>>free_mem_label>>free_mem;
      
      line_counter++;
    }
  }
  mem_file.close();
  //calculate used mememory
  mem_utilized = (1.0*(total_mem - free_mem))/total_mem;
  return mem_utilized; 
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {   
  long uptime = 0;
  std::string line;
  std::ifstream uptime_file(kProcDirectory+kUptimeFilename);
  if(uptime_file.is_open()){
    std::getline(uptime_file,line); //a single line
    std::istringstream line_stream(line);
    line_stream>>uptime;//up_time;
  }
  uptime_file.close();

  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// DONE: Read and return CPU utilization
// Returns the total cpu utilization - nor for individual cpus
vector<string> LinuxParser::CpuUtilization() {   
  std::string user, nice, syst, idle, iowait;
  std::string irq, softirq, steal, guest, guest_nice;
  std::string cpu_lbl;
  std::string line;
  std::ifstream cpu_file(kProcDirectory+kStatFilename);
  if(cpu_file.is_open()){
    std::getline(cpu_file,line); //get the first line
    std::istringstream line_stream(line);
    line_stream>>cpu_lbl>>user>>nice>>syst>>idle>>iowait>>irq>>
                 softirq>>steal>>guest>>guest_nice;
  }
  cpu_file.close();

  return {user,nice,syst,idle,iowait,irq,softirq,steal,guest,guest_nice}; 
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int total_processes = 0;
  std::string line;
  std::ifstream proc_file(kProcDirectory+kStatFilename);
  if(proc_file.is_open()){
    while(std::getline(proc_file,line)){
      std::istringstream line_stream(line);
      std::string txt_label ="";
      line_stream>>txt_label>>total_processes;
      if(txt_label =="processes"){  break; }
    }
  }
  proc_file.close();

  return total_processes; 
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int running_processes = 0;
  std::string line;
  std::ifstream proc_file(kProcDirectory+kStatFilename);
  if(proc_file.is_open()){
    while(std::getline(proc_file,line)){
      std::istringstream line_stream(line);
      std::string txt_label ="";
      line_stream>>txt_label>>running_processes;
      if(txt_label =="procs_running"){  break; }
    }
  }
  proc_file.close();
  
  return running_processes; 
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string cmd;
  std::ifstream cmd_file(kProcDirectory+std::to_string(pid)+"/"+kCmdlineFilename);
  if(cmd_file.is_open()){
    std::getline(cmd_file,cmd);
  }
  cmd_file.close();
  return cmd; 
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::string ram_val="0", txt_label="";
  std::string line;
  std::ifstream ram_file(kProcDirectory+std::to_string(pid)+"/"+kStatusFilename);
  if(ram_file.is_open()){
    while(std::getline(ram_file,line)){
      std::istringstream line_stream(line);      
      line_stream>>txt_label>>ram_val;
      if(txt_label == "VmSize:"){ break;  }
    }
  }
  ram_file.close();

  int ram_in_mb = std::stoi(ram_val)/1024; //convert to MB (truncated)
  ram_val = std::to_string(ram_in_mb);
  return ram_val; 
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string uid, txt_label;
  std::string line;
  std::ifstream uid_file(kProcDirectory+std::to_string(pid)+"/"+kStatusFilename);
  if(uid_file.is_open()){
    while(std::getline(uid_file,line)){
      std::istringstream line_stream(line);      
      line_stream>>txt_label>>uid;
      if(txt_label == "Uid:"){  break; }
    }
  }
  uid_file.close();
  return uid; 
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::string uid = LinuxParser::Uid(pid);
  std::string line;
  std::string user, x, u_id;
  std::ifstream user_file(kPasswordPath);
  if(user_file.is_open()){
    while(std::getline(user_file,line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream line_stream(line);
      line_stream>>user>>x>>u_id;
      if(u_id == uid){ break; }
    }
  }
  user_file.close();
  return user; 
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  std::string up_time;
  std::string line;
  std::ifstream uptime_file(kProcDirectory+std::to_string(pid)+"/"+kStatFilename);
  if(uptime_file.is_open()){
    std::getline(uptime_file,line);
    std::istringstream line_stream(line);
    int count = 0;
    while(!line_stream.eof()){
      line_stream>>up_time;
      // the 21'st value in the line is the uptime value. 
      if(count == 21) break;      
      count ++;
    }
  }
  uptime_file.close();
  long up_time_val = std::stol(up_time)/sysconf(_SC_CLK_TCK);

  return up_time_val; 
}

//Utilization calculated based on this refernce
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float LinuxParser::CpuUtilization(int pid){
  long utime,stime,cutime,cstime,starttime;
  std::string time_str;
  std::string line;
  std::ifstream cpu_file(kProcDirectory+std::to_string(pid)+"/"+kStatFilename);
  if(cpu_file.is_open()){
    std::getline(cpu_file,line);
    std::istringstream line_stream(line);
    int count = 0;
    while(!line_stream.eof()){
      line_stream>>time_str;
      if(count == 13) { utime = std::stol(time_str); }
      if(count == 14) { stime = std::stol(time_str); }
      if(count == 15) { cutime = std::stol(time_str); }
      if(count == 16) { cstime = std::stol(time_str); }
      if(count == 21) { starttime = std::stol(time_str); 
         break;  
      }     
      count ++;
    }
  }
  cpu_file.close();

  long total_time = utime + stime + cutime + cstime; //time in ticks
  long uptime = LinuxParser::UpTime(); //time in seconds

  long time_duration = uptime - (starttime/sysconf(_SC_CLK_TCK));
  total_time = total_time/sysconf(_SC_CLK_TCK); //to seconds
  float process_util = total_time*1.0/time_duration;
  return process_util;
}