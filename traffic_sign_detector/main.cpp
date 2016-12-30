#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/image_transforms.h>
#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../global/config.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace dlib;

class TrafficSignDetector
{
public:
	TrafficSignDetector(std::string name, std::string path, dlib::rgb_pixel color) :
		name(name), path(path), color(color) {};

	std::string name;
	std::string path;
	dlib::rgb_pixel color;
};

void print_help()
{
	cout << "Run examples:" << endl;
	cout << "-h print help" << endl;
	cout << "./detect -i image.bmp" << endl;
	cout << "./detect -v video.avi" << endl;
}

int main(int argc, char *argv[]) 
{
	try 
	{
		if (argc == 2 && string(argv[1]) == "-h")
		{
			print_help();
		}
		else if (argc != 3)
		{
			cerr << "Please specify image for detection or video. See help -h" << endl;
		}

		// Load SVM detectors
		std::vector<TrafficSignDetector> signs;
		std::vector<detector_pd6> detectors;

		for (unsigned int i = 0; datasets.size() > i; ++i)
		{
			detector_pd6 detector;
			string path = detectors_dir + datasets[i] + "-detector.svm";
			deserialize(path) >> detector;
			signs.push_back(TrafficSignDetector(datasets[i], path, signs_color[datasets[i]]));
			detectors.push_back(detector);
		}

		if (string(argv[1]) == "-i")
		{
			array2d<unsigned char> image;
			load_image(image, argv[2]);
			
			for (unsigned long i = 0; i < D_UPSAMPLING; ++i)
			{
				pyramid_up(image);
			}

			image_window window;
			std::vector<rect_detection> rects;

			evaluate_detectors(detectors, image, rects);

			window.clear_overlay();
			window.set_image(image);

			for (unsigned long j = 0; j < rects.size(); ++j) {
				window.add_overlay(rects[j].rect, signs[rects[j].weight_index].color, signs[rects[j].weight_index].name);
			}

			cout << "Press any key to exit..." << endl;
			cin.get();
		}
		else if (string(argv[1]) == "-v")
		{
			
			cv::VideoCapture vid;
			string videofile = string(argv[2]);
			cout << "Opening videofile: \"" + videofile + "\"" << endl;
			vid.open(videofile);

			if (!vid.isOpened())
			{
				cout << "Video isn't opened" << endl;
				return EXIT_FAILURE;
			}


			cout << "Video loaded" << endl;

			cv::Mat img, stop, hsv;

			//set start
			int cnt = 0;

			vid.set(CV_CAP_PROP_POS_FRAMES, cnt);

			cout << "Video set time" << endl;

			image_window window;

			while (true)
			{
				vid >> img;

				if (img.empty())
				{
					cout << "video end" << endl;
					break;
				}
					
				cnt++;
				cout << cnt << endl;
				
				/*
				// Used for testing with gopro 3 with 1080p from car.
				// Signs are on the right side so video can be
				// cropped to use only part of video for detection 
				// for speedup recognition

				int roiX = img.cols / 2;
				int roiY = img.rows / 4;
				int roiWidth = img.cols / 3;
				int roiHeight = img.rows / 2;
				cv::Rect myROI(roiX, roiY, roiWidth, roiHeight);
				img = img(myROI);
				//cv::resize(img, img, cv::Size(img.cols / 2, img.rows / 2));
				*/

				cv_image<bgr_pixel> cimg(img);

				std::vector<rect_detection> rects;

				evaluate_detectors(detectors, cimg, rects);

				window.clear_overlay();
				window.set_image(cimg);

				for (unsigned long j = 0; j < rects.size(); ++j) {
					window.add_overlay(rects[j].rect, signs[rects[j].weight_index].color, signs[rects[j].weight_index].name);
				}

			}
		}
		else
		{
			cerr << "Wrong parameters. See help -h" << endl;
		}
	}
	catch (exception& e) {
		cout << "\nexception thrown!" << endl;
		cout << e.what() << endl;
	}
}