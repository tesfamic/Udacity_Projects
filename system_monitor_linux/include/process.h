#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process();
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization() const;                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process &a) const;  // TODO: See src/process.cpp

  //setter methods
  void User(std::string user);
  void Command(std::string cmd);
  void CpuUtilization(float util);
  void Ram(std::string ram);
  void UpTime(long int up_time);
  void Pid(int pid);
  // TODO: Declare any necessary private members
 private:
    int pid_; //id of the process
    std::string user_;
    std::string command_;
    std::string ram_;
    long int uptime_;
    float cpu_utilization_;
};

#endif