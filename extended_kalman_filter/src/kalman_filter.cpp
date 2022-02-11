#include "kalman_filter.h"
#include <iostream>
using Eigen::MatrixXd;
using Eigen::VectorXd;

// Please note that the Eigen library does not initialize 
// VectorXd or MatrixXd objects with zeros upon creation.

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
    * predict the state
  */
  x_ = F_*x_;
  P_ = F_*P_*F_.transpose() + Q_;

}

void KalmanFilter::Update(const VectorXd &z) {
  /**
   * update the state by using Kalman Filter equations
  */
  VectorXd y = z - H_*x_;
  MatrixXd S = H_*P_*H_.transpose() + R_;
  MatrixXd K = P_*H_.transpose()*S.inverse();

  x_ = x_ + K*y;
  MatrixXd I = MatrixXd::Identity(x_.size(),x_.size());
  P_ = (I - K*H_)*P_;

}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
    * update the state by using Extended Kalman Filter equations
  */
  //Get predicted position and velocity values from the prediction step
  float px = x_[0];
  float py = x_[1];
  float vx = x_[2];
  float vy = x_[3];
  float p_sqrt = sqrtf(px*px+py*py);

  //calculate the h(x')
  VectorXd h_x(3);
  h_x << p_sqrt, atan2f(py,px),(px*vx +py*vy)/p_sqrt;

  VectorXd y = z - h_x;
  //adjust the phi value in such away that it is in the range of -pi to pi
  //float phi= y(1);
  while (y(1)<-1*M_PI || y(1)>M_PI){    
    if(y(1)>M_PI){ 
      y(1) -= 2*M_PI;
    }else{
      y(1) += 2*M_PI;
    }
  }
  //y(1) = phi;

  MatrixXd S = H_*P_*H_.transpose() + R_;
  MatrixXd K = P_*H_.transpose()*S.inverse();

  //state and covariance update 
  MatrixXd I = MatrixXd::Identity(x_.size(),x_.size());
  x_ = x_ + K*y;
  P_ = (I - K*H_)*P_;
}
