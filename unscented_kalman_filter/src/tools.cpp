#include <iostream>
#include "tools.h"

using Eigen::VectorXd;
using Eigen::MatrixXd;
using std::vector;

Tools::Tools() {}

Tools::~Tools() {}

VectorXd Tools::CalculateRMSE(const vector<VectorXd> &estimations,
                              const vector<VectorXd> &ground_truth) {
  /**
    * Calculate the RMSE here.
  */
  //std::cout<<"Calc RMSE called. "<<std::endl;
  VectorXd rmse(4),total_error(4);
  rmse<<0,0,0,0;
  total_error<<0,0,0,0;

  if((estimations.size() != ground_truth.size()) ||
     estimations.size()==0){
       std::cout<<"Invalid estimation data/ground truth data."<<std::endl;
       return rmse;
  }
  //otherwise, collect errors for each estimation
  for(unsigned int i=0;i<estimations.size();++i){
    VectorXd error = estimations[i] - ground_truth[i];
    //calculate the error square
    error = error.array().square();
    //Collect the errors
    total_error += error;
  }
  //calculate the mean of the total error
  total_error /= estimations.size();

  //calculate rmse
  rmse = total_error.array().sqrt();
  //std::cout<<"RMSE calculation ended: "<<rmse<<std::endl;
  return rmse;
}