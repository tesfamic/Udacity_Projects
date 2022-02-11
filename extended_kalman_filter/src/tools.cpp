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

  return rmse;
}
 /**
    * Calculates a Jacobian for measurement update for RADAR measurement.
  */
MatrixXd Tools::CalculateJacobian(const VectorXd& x_state) {
 
  MatrixXd Hj = MatrixXd::Zero(3,4);
  
  float px = x_state(0);
  float py = x_state(1);
  float vx = x_state(2);
  float vy = x_state(3);

  //To avoid division by zero in the Jacobian calculation,
  //check if the position is zero or not. However,px and py are floating point data =>
  //their values may not be exactly zero(or 0.000).
  // Return zero matrix for Hj
  if(px<=0.00001 && py<=0.00001){
    std::cout<<"Position px,py are zero.Zero Hj returned"<<std::endl;
    return Hj;
  }

  //To avoid repeatitive square root calculation for the same data, calculate once
  // and use it when needed.
  float p_sqr = px*px + py*py;  // (px^2 + py^2)
  float p_sqrt = sqrt(p_sqr);   // (px^2 + py^2)^0.5
  float p_sqr32 = p_sqrt*p_sqr; // (px^2 + py^2)^1.5
  float px_by_p_sqrt = px/p_sqrt;   // px/(px^2 + py^2)^0.5
  float py_by_p_sqrt = py/p_sqrt;   // py/(px^2 + py^2)^0.5

  Hj<< px_by_p_sqrt,             py_by_p_sqrt,             0,            0, //px/p_sqrt, py/p_sqrt,0, 0,
      -py/p_sqr,                 px/p_sqr,                 0,            0,
       py*(vx*py-vy*px)/p_sqr32, px*(vy*px-vx*py)/p_sqr32, px_by_p_sqrt, py_by_p_sqrt;

  return Hj;
}
