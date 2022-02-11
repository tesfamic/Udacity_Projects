/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: Tiffany Huang
 *------------------------------
 *  Code modified (as part of the project) by
 *       Tesfamichael Getahun
 */

#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h> 
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

#include "particle_filter.h"

using namespace std;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
	// TODO: Set the number of particles. Initialize all particles to first position (based on estimates of 
	//   x, y, theta and their uncertainties from GPS) and all weights to 1. 

	num_particles = 200;

	weights.reserve(num_particles);
	particles.reserve(num_particles);
	associated_obs.reserve(num_particles);
	//initialize weights
	default_random_engine gen;

	normal_distribution<double> dist_x(x, std[0]);
	normal_distribution<double> dist_y(y, std[1]);
	normal_distribution<double> dist_theta(theta, std[2]);

	for(unsigned int i=0;i<num_particles;++i){
		particles[i].id = i;
		particles[i].x = dist_x(gen);
		particles[i].y = dist_y(gen);
		particles[i].theta = dist_theta(gen);
		particles[i].weight = 1.0;
	} 
	is_initialized = true;
}

void ParticleFilter::prediction(double delta_t, double std_pos[], double velocity, double yaw_rate) {
	// TODO: Add measurements to each particle and add random Gaussian noise.
	// NOTE: When adding noise you may find std::normal_distribution and std::default_random_engine useful.
	//  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
	//  http://www.cplusplus.com/reference/random/default_random_engine/

    default_random_engine gen;  
	normal_distribution<double> dist_x(0, std_pos[0]);
	normal_distribution<double> dist_y(0, std_pos[1]);
	normal_distribution<double> dist_theta(0, std_pos[2]);

	//to avoid repeatitive calculation
	double yawrate_dt = yaw_rate*delta_t;
	double vel_over_yawr = velocity/yaw_rate;	
	double vel_dt = velocity*delta_t;
	double theta = 0.0;

	for(unsigned int i=0;i<num_particles;++i){
		theta = particles[i].theta; 
        //check if yaw_rate != 0
        // which one is faster: taking abs(yaw_rate) or using 'logical or'
		if(yaw_rate > 0.0001 || yaw_rate<-0.0001){ 
			particles[i].x     += vel_over_yawr*(sin(theta + yawrate_dt) - sin(theta));
			particles[i].y     += vel_over_yawr*(cos(theta) - cos(theta + yawrate_dt));
			particles[i].theta += yawrate_dt;		
		}else{					
			particles[i].x     += vel_dt*cos(theta);
			particles[i].y     += vel_dt*sin(theta);
			//particles[i].theta += dist_theta(gen);		
		}
		//add random zero mean gaussian noise
		particles[i].x     += dist_x(gen);
		particles[i].y     += dist_y(gen);
		particles[i].theta += dist_theta(gen);
	}
}

void ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted, std::vector<LandmarkObs>& observations) {
	// TODO: Find the predicted measurement that is closest to each observed measurement and assign the 
	//   observed measurement to this particular landmark.
	// NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
	//   implement this method and use it as a helper during the updateWeights phase.
	//*********NOTE***********************************
    // Here, instead of putting the id of closest landmark to the observaton id, I created another vector
    // called associated_obs to collect the closest landmark(id,x,y) for each observation. Because of this, there is no need
    // to search the predicted landmark in the weight update calculation => faster execution.
    // Disadvantage: it takes a little bit of memory.
    //************************************************
	associated_obs.clear();
	associated_obs.reserve(observations.size());
	unsigned int idx=0;
	double dist_pts = 0.0; //distance between points
	for (unsigned int i = 0; i<observations.size(); i++){
		double min_distance = 10000.0;
		//int closest_landmark_id = 0;
		auto obs = observations[i];
		for (unsigned int j = 0; j < predicted.size(); j++) {
			auto pred = predicted[j];
			dist_pts = dist(obs.x,obs.y, pred.x, pred.y);
			if (dist_pts < min_distance){
				min_distance = dist_pts;
				//closest_landmark_id = predicted[j].id;
				idx = j;
			}
		}
		//observations[i].id = closest_landmark_id;
		associated_obs.push_back(predicted[idx]);
	}
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
		const std::vector<LandmarkObs> &observations, const Map &map_landmarks) {
	// TODO: Update the weights of each particle using a mult-variate Gaussian distribution. You can read
		//   more about this distribution here: https://en.wikipedia.org/wiki/Multivariate_normal_distribution
		// NOTE: The observations are given in the VEHICLE'S coordinate system. Your particles are located
		//   according to the MAP'S coordinate system. You will need to transform between the two systems.
		//   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
		//   The following is a good resource for the theory:
		//   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
		//   and the following is a good resource for the actual equation to implement (look at equation 
		//   3.33
		//   http://planning.cs.uiuc.edu/node99.html	
	//--------------------------------------------------
	weights.clear(); //clear the weights vector for update
	for (unsigned int n=0; n<particles.size();n++){
		auto particle = particles[n];
		///coordinate transformation from car to map
		vector<LandmarkObs> transformed_obs;
		for (unsigned int j=0; j<observations.size(); ++j){
			auto observe = observations[j];
			LandmarkObs temp;
			temp.id = observe.id;
			temp.x = observe.x * cos(particle.theta) - observe.y * sin(particle.theta) + particle.x;
			temp.y = observe.x * sin(particle.theta) + observe.y * cos(particle.theta) + particle.y;			
			transformed_obs.push_back(temp);
		}
		//region of interest selection. Ignors some of the landmarks 
		vector<LandmarkObs> predicted;
		for (unsigned int j = 0; j < map_landmarks.landmark_list.size(); ++j) {
			auto landmark = map_landmarks.landmark_list[j];
			double dist_particle_landmark = dist(particle.x, particle.y, landmark.x_f, landmark.y_f );
			
			if (dist_particle_landmark < sensor_range){
				predicted.push_back(LandmarkObs{landmark.id_i,landmark.x_f,landmark.y_f});
			}
		}
		//associate the predicted landmarks with observation landmarks
        //Note: the data association creates another vector called 'associated_obs'
		dataAssociation(predicted, transformed_obs);

		//calculate the weight of a particle
		double std_x = std_landmark[0];
		double std_y = std_landmark[1];
		double x, y, mu_x, mu_y;
		double coeff = 1.0/(2*M_PI*std_x*std_y);  //the coefficient of the multivariate Gaussian pdf
		double prob_tot = 1.0;		
		for(unsigned int j=0;j<transformed_obs.size();++j){
			x = transformed_obs[j].x;
			y = transformed_obs[j].y;
			mu_x = associated_obs[j].x;  //corresponding closest landmark
			mu_y = associated_obs[j].y;
            double exponent = pow((x-mu_x)/std_x,2) + pow((y-mu_y)/std_y,2);
			prob_tot *= coeff*exp(-0.5*exponent);          
		}
		weights.push_back(prob_tot);  
		particles[n].weight = prob_tot; //update the particle weight
      	
		/*
			// for(unsigned int j=0;j<transformed_obs.size();++j){
			// 	x = transformed_obs[j].x;
			// 	y = transformed_obs[j].y;

			// 	int id_associated = transformed_obs[j].id;
			// 	//get the predicted associated values based on id
			// 	for(unsigned int k=0;k<predicted.size();++k){
			// 		if(predicted[k].id == id_associated){
			// 			mu_x = predicted[k].x;
			// 			mu_y = predicted[k].y;
			// 		}
			// 	}
			// 	double exponent = pow(((x-mu_x)/std_x),2) + pow(((y-mu_y)/std_y),2);
			//  prob_tot *= coeff* exp(-0.5*exponent);			
			// }
			// weights.push_back(prob_tot); 
			// particles[n].weight = prob_tot;
		*/		
	}
}

void ParticleFilter::resample() {
	// TODO: Resample particles with replacement with probability proportional to their weight. 
	// NOTE: You may find std::discrete_distribution helpful here.
	//   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution

  	//**** Resampling stage - direct translation of the python code in the course material ****//
      /*
      // double max_weight = *max_element(weights.begin(),weights.end());

       // //two uniform distributions are needed: for index and for beta
      // std::default_random_engine gen;
      // std::uniform_int_distribution<int> u_int_gen(0, num_particles-1);
      // std::uniform_real_distribution<double> u_beta_gen(0, max_weight);

      // std::vector<Particle> p3;
      // unsigned int index = u_int_gen(gen);
      // double beta = 0.0;
      // for(unsigned int i=0;i<num_particles;++i){
      // 	beta += u_beta_gen(gen)*2.0;
      // 	while(beta > weights[index]){
      // 		beta -= weights[index];
      // 		index = (index + 1)%num_particles;
      // 	}
      // 	p3.push_back(particles[index]);
      // }
      // particles = p3;
	  */
     ///***************************************************//
    //***** Resampling using std::discrete_distribution function
    std::default_random_engine gen;
	// weights distribution
	std::discrete_distribution<int> weights_distr(weights.begin(), weights.end());	
  	std::vector<Particle> p3;
	
	for(unsigned int i=0;i<num_particles;++i){
        unsigned int idx = weights_distr(gen);
		auto particle = particles[idx];
		p3.push_back(particle);
	}
	particles = p3;
}

Particle ParticleFilter::SetAssociations(Particle& particle, const std::vector<int>& associations, 
                                     const std::vector<double>& sense_x, const std::vector<double>& sense_y)
{
    //particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
    // associations: The landmark id that goes along with each listed association
    // sense_x: the associations x mapping already converted to world coordinates
    // sense_y: the associations y mapping already converted to world coordinates

    particle.associations= associations;
    particle.sense_x = sense_x;
    particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best)
{
	vector<int> v = best.associations;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseX(Particle best)
{
	vector<double> v = best.sense_x;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseY(Particle best)
{
	vector<double> v = best.sense_y;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
