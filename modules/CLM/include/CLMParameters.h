/* Copyright (c) 2015 USC, IRIS, Computer vision Lab */
///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012, Tadas Baltrusaitis, all rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//     * The software is provided under the terms of this licence stricly for
//       academic, non-commercial, not-for-profit purposes.
//     * Redistributions of source code must retain the above copyright notice, 
//       this list of conditions (licence) and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright 
//       notice, this list of conditions (licence) and the following disclaimer 
//       in the documentation and/or other materials provided with the 
//       distribution.
//     * The name of the author may not be used to endorse or promote products 
//       derived from this software without specific prior written permission.
//     * As this software depends on other libraries, the user must adhere to 
//       and keep in place any licencing terms of those libraries.
//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite one of the following works:
//
//       Tadas Baltrusaitis, Peter Robinson, and Louis-Philippe Morency. 3D
//       Constrained Local Model for Rigid and Non-Rigid Facial Tracking.
//       IEEE Conference on Computer Vision and Pattern Recognition (CVPR), 2012.    
//
//       Tadas Baltrusaitis, Peter Robinson, and Louis-Philippe Morency. 
//       Constrained Local Neural Fields for robust facial landmark detection in the wild.
//       in IEEE Int. Conference on Computer Vision Workshops, 300 Faces in-the-Wild Challenge, 2013.    
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
// EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

//  Parameters of the CLM-Z and CLM trackers
//
//  Tadas Baltrusaitis
//  01/05/2012

#ifndef __CLM_PARAM_H
#define __CLM_PARAM_H

//#include <cv.h>
//#include <highgui.h>

#include <opencv2/opencv.hpp>

#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

namespace CLMTracker
{

struct CLMParameters
{

	// A number of RLMS or NU-RLMS iterations
	int num_optimisation_iteration;
	
	// Should pose be limited to 180 degrees frontal
	bool limit_pose;
	
	// Should face validation be done
	bool validate_detections;

	// Landmark detection validator boundary for correct detection, the regressor output -1 (perfect alignment) 1 (bad alignment), 
	double validation_boundary;

	// Used when tracking is going well
	vector<int> window_sizes_small;

	// Used when initialising or tracking fails
	vector<int> window_sizes_init;
	
	// Used for the current frame
	vector<int> window_sizes_current;

	string model_location;
	
	// this is used for the smooting of response maps (KDE sigma)
	double sigma;

	double reg_factor;	// weight put to regularisation
	double weight_factor; // factor for weighted least squares

	// should multiple views be considered during reinit
	bool multi_view;
	
	// How often should face detection be used to attempt reinitialisation, every n frames (set to negative not to reinit)
	int reinit_video_every;

	string face_detector_location;

	// Should the results be visualised and reported to console
	bool quiet_mode;

	CLMParameters()
	{
		// initialise the default values
		init();
	}

	// possible parameters are -clm 'file' which specifies the default location of main clm root
	CLMParameters(vector<string> &arguments)
	{
		// initialise the default values
	    init(); 

		bool* valid = new bool[arguments.size()];
		for(size_t i = 0; i < arguments.size(); ++i)
		{
			valid[i] = true;

			if (arguments[i].compare("-mloc") == 0) 
			{                    
				string model_loc = arguments[i + 1];
				model_location = model_loc;
				valid[i] = false;
				valid[i+1] = false;
				i++;

			}
			if (arguments[i].compare("-fdcloc") == 0) 
			{                    
				string fdc_loc = arguments[i + 1];
				face_detector_location = fdc_loc;
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			if (arguments[i].compare("-clm_sigma") == 0) 
			{                    
				sigma = atof(arguments[i + 1].c_str());
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if(arguments[i].compare("-w_reg") == 0)
			{
				weight_factor = atof(arguments[i + 1].c_str());
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if(arguments[i].compare("-reg") == 0)
			{
				reg_factor = atof(arguments[i + 1].c_str());
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if (arguments[i].compare("-multi_view") == 0) 
			{                    
				multi_view = (bool)(atoi(arguments[i + 1].c_str()) != 0);
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if(arguments[i].compare("-validate_detections") == 0)
			{
				validate_detections = (bool)(atoi(arguments[i + 1].c_str()) != 0);
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if(arguments[i].compare("-n_iter") == 0)
			{
				num_optimisation_iteration = atoi(arguments[i + 1].c_str());
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if (arguments[i].compare("-q") == 0) 
			{                    

				quiet_mode = true;

				valid[i] = false;
			}
			else if (arguments[i].compare("-clmwild") == 0) 
			{                    
				// For in the wild fitting these parameters are suitable
				window_sizes_init = vector<int>(3);
				window_sizes_init[0] = 15; window_sizes_init[1] = 15; window_sizes_init[2] = 15;		

				sigma = 2;
				reg_factor = 25;
				weight_factor = 5;
				num_optimisation_iteration = 10;

				valid[i] = false;
			}
			else if (arguments[i].compare("-help") == 0)
			{
				cout << "CLM parameters are defined as follows: -mloc <location of model file> -pdm_loc <override pdm location> -w_reg <weight term for patch rel.> -reg <prior regularisation> -clm_sigma <float sigma term> -fcheck <should face checking be done 0/1> -n_iter <num EM iterations> -clwild (for in the wild images) -q (quiet mode)" << endl; // Inform the user of how to use the program				
			}
		}

		for(int i=arguments.size()-1; i >= 0; --i)
		{
			if(!valid[i])
			{
				arguments.erase(arguments.begin()+i);
			}
		}

	}

	private:
		void init()
		{

			// number of iterations that will be performed at each clm scale
			num_optimisation_iteration = 5;
			
			// using an external face checker based on SVM
			validate_detections = true;

			window_sizes_small = vector<int>(2);
			window_sizes_init = vector<int>(3);

			// For fast tracking
			window_sizes_small[0] = 9;
			window_sizes_small[1] = 7;

			// Just for initialisation
			window_sizes_init.at(0) = 11;
			window_sizes_init.at(1) = 9;
			window_sizes_init.at(2) = 7;

			// For first frame use the initialisation
			window_sizes_current = window_sizes_init;

			model_location = "model/main_ccnf_general.txt";

			sigma = 1.5;
			reg_factor = 25;
			weight_factor = 0; // By default do not use NU-RLMS, as weight factor is dependent on the database and needs to be estimated

			validation_boundary = -0.4;

			limit_pose = true;
			multi_view = true;

			reinit_video_every = 4;
						
			// Face detection
			#if OS_UNIX
				face_detector_location = "classifiers/haarcascade_frontalface_alt.xml";
			#else
				face_detector_location = "classifiers/haarcascade_frontalface_alt.xml";
			#endif

			quiet_mode = false;
		}
};

}

#endif // __CLM_PARAM_H
