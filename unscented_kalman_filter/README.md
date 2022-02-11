# Unscented Kalman Filter Project
Self-Driving Car Engineer Nanodegree Program

## Objective

> To utilize an Unscented Kalman Filter to estimate the state of a moving object of interest with noisy Lidar and Radar measurements.  

## Implementation results

If you have access to the Term 2 of CarND, you can find the rubrics [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/c3eb3583-17b2-4d83-abf7-d852ae1b9fff/concepts/f437b8b0-f2d8-43b0-9662-72ac4e4029c1). The result is briefly shown below
> Comparing the Unscented Kalman filter results with the ground truth data, the RMSE is calculated and the result is as follows.
  The result shows that the fused data using the UKF provides better position and velocity estimation than only Radar or only Lidar measurements.
>
 |                | RMSE_px| RMSE_py|RMSE_vx |RMSE_vy |
 |:--------------:|:------:|:------:|:------:|:------:|
 | Radar only     | 0.1560 | 0.1864 | 0.2316 | 0.2743 |
 | Lidar only     | 0.0962 | 0.0961 | 0.2478 | 0.2331 |
 | Radar & Lidar  | 0.0845 | 0.0799 | 0.3585 | 0.2451 |
        


[//]: # (Image References)
[NIS_radar]: ./images/nis_radar.png
[NIS_lidar]: ./images/nis_lidar.png

## Consistency check
   > Normalized Innovation Squared (NIS) result shown below indicates that the UKF estimation is well in the required NIS range in majority of the measurement data (between 5% - 95%), i.e. the implemented UKF is <b>consistent</b>.
   
   ![alt_text][NIS_radar] ![alt_text][NIS_lidar]


## Requirements to run the project

Tips for setting up your environment can be found [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d).

This project involves the Term 2 Simulator which can be downloaded [here](https://github.com/udacity/self-driving-car-sim/releases)

Note that the programs that need to be written to accomplish the project are src/ukf.cpp, src/ukf.h, tools.cpp, and tools.h
Here is the main protcol that main.cpp uses for uWebSocketIO in communicating with the simulator.

INPUT: values provided by the simulator to the c++ program

       ["sensor_measurement"] => the measurment that the simulator observed (either lidar or radar)

OUTPUT: values provided by the c++ program to the simulator

    ["estimate_x"] <= kalman filter estimated position x
    ["estimate_y"] <= kalman filter estimated position y
    ["rmse_x"]
    ["rmse_y"]
    ["rmse_vx"]
    ["rmse_vy"]

---

## Other Important Dependencies
* cmake >= 3.5
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./UnscentedKF` Previous versions use i/o from text files.  The current state uses i/o
from the simulator.


