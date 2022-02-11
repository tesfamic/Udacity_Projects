#include "ukf.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/**
 * Initializes Unscented Kalman filter
 */
UKF::UKF() {
  // if this is false, laser measurements will be ignored (except during init)
  use_laser_ = true;

  // if this is false, radar measurements will be ignored (except during init)
  use_radar_ = true;

  // initial state vector
  x_ = VectorXd(5);

  // initial covariance matrix
  P_ = MatrixXd(5, 5);
  
  // Process noise standard deviation longitudinal acceleration in m/s^2
  // ***** Can be tuned *****
  std_a_ = 0.5;

  // Process noise standard deviation yaw acceleration in rad/s^2
  // ***** Can be tuned *****
  std_yawdd_ = 0.5;

  // Laser measurement noise standard deviation position1 in m
  std_laspx_ = 0.15;

  // Laser measurement noise standard deviation position2 in m
  std_laspy_ = 0.15;

  // Radar measurement noise standard deviation radius in m
  std_radr_ = 0.3;

  // Radar measurement noise standard deviation angle in rad
  std_radphi_ = 0.03;

  // Radar measurement noise standard deviation radius change in m/s
  std_radrd_ = 0.3;

  /**TODO:Complete the initialization. See ukf.h for other member properties.
          Hint: one or more values initialized above might be wildly off...
  */
  ///* initially set to false, set to true in first call of ProcessMeasurement
  is_initialized_ = false;
  
  ///* State dimension
  n_x_ = 5;
  
  ///* Augmented state dimension
  n_aug_ = 7;
  
  ///* Sigma point spreading parameter
  lambda_ = 3 - n_aug_;
  
  ///* predicted sigma points matrix
  Xsig_pred_ = MatrixXd(n_x_, 2 * n_aug_ + 1);
  
  // Vector for weights
  weights_ = VectorXd(2*n_aug_+1);
  
  ///* time when the state is true, in us
  time_us_ = 0;  
  
}

UKF::~UKF() {
  //NIS_radar_.close();
  //NIS_lidar_.close();
}

/**
 * @param {MeasurementPackage} meas_package The latest measurement data of
 * either radar or laser.
 */
void UKF::ProcessMeasurement(MeasurementPackage meas_package) {

  if (!is_initialized_) {
    x_ << 1, 1, 1, 1, 1;
    if (meas_package.sensor_type_ == MeasurementPackage::RADAR) {
      double rho = meas_package.raw_measurements_(0);
      double phi = meas_package.raw_measurements_(1);
      //double rhodot = meas_package.raw_measurements_(2);
      x_(0) = rho*cos(phi);
      x_(1) = rho*sin(phi);
    }
    else if (meas_package.sensor_type_ == MeasurementPackage::LASER) {
      // Initialize state.
      x_(0) = meas_package.raw_measurements_[0];
      x_(1) = meas_package.raw_measurements_[1];
    }
    // initial guesses
    P_ << 0.01, 0,   0,   0,   0,
          0,   0.01, 0,   0,   0,
          0,   0,    0.1, 0,   0,
          0,   0,    0,   0.1, 0,
          0,   0,    0,   0,   0.1;

    //Initialize weights
    weights_(0) = lambda_/(lambda_ + n_aug_);
    for(int i=1;i<Xsig_pred_.cols();++i){
        weights_(i) = 0.5/(lambda_ + n_aug_);
    }

    // done initializing, no need to predict or update    
    time_us_ = meas_package.timestamp_;
    is_initialized_ = true;
    return;    
  }
  
  // Calculate delta_t, store current time for future
  double delta_t = (meas_package.timestamp_ - time_us_)/1000000.0;
  time_us_ = meas_package.timestamp_;

  //Call prediction function
  Prediction(delta_t);

  // Measurement updates
  if (meas_package.sensor_type_ == MeasurementPackage::RADAR && use_radar_) {
    UpdateRadar(meas_package);
  } else if(meas_package.sensor_type_ == MeasurementPackage::LASER && use_laser_) {
    UpdateLidar(meas_package);
  }  
}

/**
 * Predicts sigma points, the state, and the state covariance matrix.
 * @param {double} delta_t the change in time (in seconds) between the last
 * measurement and this one.
 */
void UKF::Prediction(double delta_t) {
  //cout<<"\n Prediction step started..."<<endl;
  //*************************************************************
  // 1. Generate sigma points and augument matrices ()
  //*******************************************************************
  //create augmented mean vector
  VectorXd x_aug = VectorXd(n_aug_);
  x_aug.head(5) = x_;
  x_aug(5) = 0;
  x_aug(6) = 0;
  //create augmented covariance matrix
  MatrixXd Q = MatrixXd(2,2);
  Q << std_a_*std_a_, 0,
        0, std_yawdd_*std_yawdd_;

  //create augmented state covariance
  MatrixXd P_aug = MatrixXd(n_aug_, n_aug_);
  P_aug.fill(0.0);
  P_aug.topLeftCorner(5, 5) = P_;
  P_aug.bottomRightCorner(2, 2) = Q;

  //calculate the square root of the P_aug
  MatrixXd P_aug_sqrt = P_aug.llt().matrixL();
  P_aug_sqrt = sqrt(lambda_+n_aug_)*P_aug_sqrt;

  //create sigma point matrix
  MatrixXd Xsig_aug = MatrixXd(n_aug_, 2 * n_aug_ + 1);
  Xsig_aug.col(0) = x_aug;
  for(int i = 0; i < n_aug_; i++) {
    Xsig_aug.col(i+1)        = x_aug + P_aug_sqrt.col(i);
    Xsig_aug.col(i+1+n_aug_) = x_aug - P_aug_sqrt.col(i);
  }
  //*************************************************
  // 2. Predict the sigma points
  //************************************************ 
  //predict sigma points
  VectorXd xs = VectorXd(5);
  VectorXd term1 = VectorXd(5);
  VectorXd noise_term = VectorXd(5);
  //double px = 0.0;
  //double py = 0.0;
  double v = 0.0;
  double yaw = 0.0;
  double yaw_r = 0.0;
  double nu_a = 0.0;
  double nu_yaw = 0.0;
  for(int i = 0; i < 2 * n_aug_ + 1; i++) {
    xs = (Xsig_aug.col(i)).head(n_x_);
    //px = Xsig_aug(0,i);
    //py = Xsig_aug(1,i);
    v = Xsig_aug(2,i);
    yaw = Xsig_aug(3,i);
    yaw_r = Xsig_aug(4,i);
    nu_a = Xsig_aug(5,i);
    nu_yaw = Xsig_aug(6,i);
    
    if(yaw_r < 0.0001) {// If yaw rate ~ 0
      term1 <<v*cos(yaw)*delta_t,
              v*sin(yaw)*delta_t,
              0,
              yaw_r*delta_t,
              0;      
    }else{ // if yaw rate is non zero
      double ratio = v/yaw_r;
      term1 <<(sin(yaw + yaw_r*delta_t) - sin(yaw))*ratio,
              (-cos(yaw + yaw_r*delta_t) + cos(yaw))*ratio,
              0,
              yaw_r * delta_t,
              0;
    }
    noise_term <<0.5*delta_t*delta_t*cos(yaw)*nu_a,
                 0.5*delta_t*delta_t*sin(yaw)*nu_a,
                 delta_t*nu_a,
                 0.5*delta_t*delta_t*nu_yaw,
                 delta_t*nu_yaw;
    //Adding the three terms
    Xsig_pred_.col(i) = xs + term1 + noise_term;
  }
  //*************************************************
  //3. Predict mean and covariance
  //***********************************************
  //create vector for predicted state
  VectorXd x_pred = VectorXd(n_x_); 
  x_pred.fill(0.0); 

  //create covariance matrix for prediction
  MatrixXd P_pred = MatrixXd(n_x_, n_x_);
  P_pred.fill(0.0);

  //predict state mean
  for(int i = 0; i < 2 * n_aug_ + 1; i++) {     
    x_pred += weights_(i) * Xsig_pred_.col(i);
  }
  //predict state covariance matrix
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {    
    VectorXd x_diff = Xsig_pred_.col(i) - x_pred;

    //normalize yaw angle
    while(x_diff(3) > M_PI) { x_diff(3) -= 2.0 * M_PI; }
    while(x_diff(3) < -M_PI) { x_diff(3) += 2.0 * M_PI; }
    P_pred += weights_(i) * x_diff * x_diff.transpose();
  }  
  x_ = x_pred;
  P_ = P_pred;
  //cout<<"\n Prediction completed."<<endl;
}

/**
 * Updates the state and the state covariance matrix using a laser measurement.
 * @param {MeasurementPackage} meas_package
 */
void UKF::UpdateLidar(MeasurementPackage meas_package) {
  //cout<<"\n Update step - Lidar: "<<endl;
  //measurement vector dimension
  int n_z = 2;

  //Get the measurement values
  VectorXd z = meas_package.raw_measurements_;

  //create matrix for sigma points in measurement space
  MatrixXd Zsig = MatrixXd(n_z, 2 * n_aug_ + 1);
  Zsig.fill(0.0);

  //mean predicted measurement
  VectorXd z_pred = VectorXd(n_z);
  z_pred.fill(0.0);
  
  //Measurement noise covariance
  MatrixXd R = MatrixXd(2,2);
  R << std_laspx_*std_laspx_, 0,
       0, std_laspy_*std_laspy_;

  //measurement covariance matrix S
  MatrixXd S = MatrixXd(n_z,n_z);
  S.fill(0.0);

  for (int i = 0; i < 2 * n_aug_ + 1; i++) {
    //double px = Xsig_pred(0,i);
    //double py = Xsig_pred(1,i);    
    Zsig.col(i) << Xsig_pred_(0,i), 
                   Xsig_pred_(1,i);
    
    //calculate mean predicted measurement
    z_pred += weights_(i) * Zsig.col(i);
  }  
  //calculate measurement covariance matrix S
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {
    VectorXd z_diff = Zsig.col(i) - z_pred;
    S += weights_(i) * z_diff * z_diff.transpose();
  }
  S += R;

  //**************************************************
  // Update mean and covariance 
  //-----------------------------------------
  //create matrix for cross correlation Tc
  MatrixXd Tc = MatrixXd(n_x_, n_z);
  Tc.fill(0.0);
  
  //calculate cross correlation matrix
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {
    VectorXd x_diff = Xsig_pred_.col(i) - x_;
    
    //normalize angle to keep it in range [-pi, pi]
    while(x_diff(3) > M_PI) { x_diff(3) -= 2.0 * M_PI; } 
    while(x_diff(3) < -M_PI) { x_diff(3) += 2.0 * M_PI; }
    
    VectorXd z_diff = Zsig.col(i) - z_pred;

    Tc += weights_(i) * x_diff * z_diff.transpose();
  }  
  // residual
  VectorXd z_diff = z - z_pred;

  //calculate Kalman gain K;
  MatrixXd K = Tc * S.inverse(); 

  //update state mean and covariance matrix
  x_ += K*z_diff;
  P_ -= K*S*K.transpose();

  //calculate NIS and save it to file 
  // Note: path to save the values should be changed based on 
  //       your platform;
  ofstream NIS_lidar_;
  NIS_lidar_.open("/home/a3/nis_lid.txt",ios::out|ios::app);
  double lidar_nis_value = 0.0;
  if(NIS_lidar_.is_open()){
    lidar_nis_value = z_diff.transpose() * S.inverse() * z_diff;
    string nis_lid = std::to_string(lidar_nis_value);
    NIS_lidar_<<nis_lid<<endl;
  }
  NIS_lidar_.close();  
  //cout<<"\n Lidar update completed."<<endl;
}

/**
 * Updates the state and the state covariance matrix using a radar measurement.
 * @param {MeasurementPackage} meas_package
 */
void UKF::UpdateRadar(MeasurementPackage meas_package) {
  //cout<<"Update step -Radar:"<<endl;
  //measurement vector dimension
  int n_z = 3;

  //get measurement values
  VectorXd z = meas_package.raw_measurements_;

  //create matrix for sigma points in measurement space
  MatrixXd Zsig = MatrixXd(n_z, 2 * n_aug_ + 1);
  Zsig.fill(0.0);

  //mean predicted measurement
  VectorXd z_pred = VectorXd(n_z);
  z_pred.fill(0.0);

  //measurement noise covariance
  MatrixXd R = MatrixXd(3,3);
  R << std_radr_*std_radr_, 0,  0,
       0, std_radphi_*std_radphi_, 0,
       0, 0,  std_radrd_*std_radrd_;
  //measurement covariance matrix S
  MatrixXd S = MatrixXd(n_z,n_z);
  S.fill(0.0);

  for (int i = 0; i < 2 * n_aug_ + 1; i++) {
    double px = Xsig_pred_(0,i);
    double py = Xsig_pred_(1,i);
    double v = Xsig_pred_(2,i);
    double yaw = Xsig_pred_(3,i);
    
    //convert rectangular to polar coordinate
    double rho = sqrt(px*px+py*py);
    double phi = atan2(py,px);
    double rho_d = (px*cos(yaw)*v +py*sin(yaw)*v) / rho;
    
    Zsig.col(i) << rho, phi, rho_d;
    
    //calculate mean predicted measurement
    z_pred += weights_(i) * Zsig.col(i);
  }
  
  //calculate measurement covariance matrix S
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {  
    VectorXd z_diff = Zsig.col(i) - z_pred;
    //normalize angle (yaw)
    while(z_diff(1) > M_PI) { z_diff(1) -= 2.0 * M_PI; }
    while(z_diff(1) < - M_PI) { z_diff(1) += 2.0 * M_PI; }

    S += weights_(i) * z_diff * z_diff.transpose();
  }
  S += R;
  //**************************************************
  // Update mean and covariance 
  //-----------------------------------------
  //create matrix for cross correlation Tc
  MatrixXd Tc = MatrixXd(n_x_, n_z);
  Tc.fill(0.0);
  
  //calculate cross correlation matrix
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {
    VectorXd x_diff = Xsig_pred_.col(i) - x_;
    VectorXd z_diff = Zsig.col(i) - z_pred;

    //normalize angle (yaw)
    while(x_diff(3) > M_PI) { x_diff(3) -= 2.0 * M_PI; }
    while(x_diff(3) < -M_PI) { x_diff(3) += 2.0 * M_PI; }

    while(z_diff(1) > M_PI) { z_diff(1) -= 2.0 * M_PI; }
    while(z_diff(1) < -M_PI) { z_diff(1) += 2.0 * M_PI; }
    Tc += weights_(i) * x_diff * z_diff.transpose();    
  }
  
  // residual
  VectorXd z_diff = z - z_pred;
  
  //normalize angles
  while(z_diff(1) > M_PI) { z_diff(1) -= 2.0 * M_PI; }
  while(z_diff(1) < -M_PI) { z_diff(1) += 2.0 * M_PI; }

  //calculate Kalman gain K;
  MatrixXd K = Tc * S.inverse();
  
  //update state mean and covariance matrix
  x_ += K*z_diff;
  P_ -= K*S*K.transpose();
    
  //calculate NIS and save it to file 
  // Note: path to save the values should be changed based on 
  //       your platform;
  ofstream NIS_radar_;
  NIS_radar_.open("/home/a3/nis_radar.txt",ios::out |ios::app);
  double radar_nis_value = 0.0;
  if(NIS_radar_.is_open()){
    radar_nis_value = z_diff.transpose() * S.inverse() * z_diff;
    string nis_rad = std::to_string(radar_nis_value);
    NIS_radar_<<nis_rad<<endl;
  }
  NIS_radar_.close();
  //cout<<"Radar update done."<<endl;
}

