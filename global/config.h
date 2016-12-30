#pragma once
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/svm_threaded.h>

#define SVM_C 1 // 1e-12 to 1e12
#define UPSAMPLING 0 // 0 to 8 for learning 
#define D_UPSAMPLING 0 // detection upsampling only for images
#define VERBOSE true
#define THREADS 4 // 1 to 1000
#define FILTER 0 // 0 to 2
#define EPS 0.01 //1e-5, 1e4
#define NORM 0 // 1e-12 to 1e12
#define TARGET_SIZE 3200

typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<6>> scan_fhog_pyramid_pd6;
typedef dlib::structural_object_detection_trainer<scan_fhog_pyramid_pd6> sod_trainer_pd6;
typedef dlib::object_detector<scan_fhog_pyramid_pd6> detector_pd6;

std::string datasets_dir = "datasets\\";
std::string detectors_dir = "detectors\\";
std::vector<std::string> datasets = { "stop", "crossing", "give-way", "oneway-exit" , "main", "parking" };
std::map<std::string, dlib::rgb_pixel> colors = { 
	{"red",   dlib::rgb_pixel(255, 0, 0)}, 
	{"green", dlib::rgb_pixel(0, 255, 0)}, 
	{"blue",  dlib::rgb_pixel(0, 0, 255)} 
};
std::map<std::string, dlib::rgb_pixel> signs_color = {
	{"stop", colors["red"]},
	{"crossing", colors["blue"]},
	{"give-way", colors["red"]},
	{"oneway-exit", colors["red"]},
	{"main", colors["green"]},
	{"parking", colors["blue"]}
};