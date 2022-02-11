#include <string>

#include "format.h"
using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    std::string formated = "";
    int hh = seconds/3600;
    int mm = (seconds - hh*3600)/60;
    int ss = (seconds - hh*3600 - mm*60);
    formated += std::to_string(hh)+":";
    
    // when ss or mm is <10 (single digit value), it needs to be adjusted to
    // 2 digits (ss or mm) by prepending 0.
    (mm<10)? formated += "0"+std::to_string(mm)+":":
             formated += std::to_string(mm)+":";

    (ss<10)? formated += "0"+std::to_string(ss):
             formated += std::to_string(ss);
    
    return formated; 
}