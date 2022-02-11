#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
    //std::string name; 
    float utilization = 0.0;
    std::vector<long> prev_utiliz;
    void UpdateUtilization();
};

#endif