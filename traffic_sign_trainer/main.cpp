#include <dlib/svm_threaded.h>
#include <dlib/string.h>
#include <dlib/data_io.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <iostream>

#include "../global/config.h"

using namespace std;

void get_window_size(
	const std::vector<std::vector<dlib::rectangle>>& boxes,
	unsigned long& width,
	unsigned long& height,
	const unsigned long target_size = TARGET_SIZE
	)
{
	dlib::running_stats<double> avg_width, avg_height;
	for (unsigned long i = 0; i < boxes.size(); ++i)
	{
		for (unsigned long j = 0; j < boxes[i].size(); ++j)
		{
			avg_width.add(boxes[i][j].width());
			avg_height.add(boxes[i][j].height());
		}
	}

	double size = avg_width.mean()*avg_height.mean();
	double scale = std::sqrt(target_size / size);

	width = (unsigned long) (avg_width.mean()*scale + 0.5);
	height = (unsigned long) (avg_height.mean()*scale + 0.5);

	if (width == 0)
		width = 1;
	if (height == 0)
		height = 1;
}

void get_datasets(dlib::array<dlib::array2d<unsigned char>> &images, std::vector<std::vector<dlib::rectangle>> &boxes, string &input_file)
{
	dlib::load_image_dataset(images, boxes, input_file);
	cout << "Loaded " << images.size() << " images for training." << endl;
	for (unsigned long i = 0; i < UPSAMPLING; ++i) {
		dlib::upsample_image_dataset<dlib::pyramid_down<2>>(images, boxes);
		
	}
	if (UPSAMPLING > 0)
	{
		cout << "Upsampled images with factor " << UPSAMPLING << "." << endl;
	}
}
	

void init_trainer(sod_trainer_pd6 &trainer)
{
	trainer.set_num_threads(THREADS);
	cout << "Number of working threads: " << THREADS << endl;
	trainer.set_c(SVM_C);
	cout << "Start value of SVM-C: " << SVM_C << endl;
	if (VERBOSE)
	{
		trainer.be_verbose();
		cout << "Verbose mode was set." << endl;
	}
   
	trainer.set_epsilon(EPS);
	cout << "EPS: " << EPS << endl;
}

void init_detector(detector_pd6 &detector)
{
	if (FILTER > 0.0) {
		int num_filters_before = num_separable_filters(detector);
		detector = threshold_filter_singular_values(detector, FILTER);
		cout << "Treshold filter was applied with factor: " << FILTER << endl;
	}
}

void train_detector(std::vector<string> &datasets)
{
	for (unsigned int i = 0; i < datasets.size(); ++i)
	{
		dlib::array<dlib::array2d<unsigned char>> images;
		std::vector<std::vector<dlib::rectangle>> boxes;

		cout << "-- DATASET [" + datasets[i] + "]--" << endl;
		cout << "Loading dataset from " << datasets_dir + datasets[i] + "-dataset.xml" << endl;
		get_datasets(images, boxes, datasets_dir + datasets[i] + "-dataset.xml");

		cout << endl;
		cout << "-- SCANNER --" << endl;
		scan_fhog_pyramid_pd6 scanner;
		if (NORM > 0.0)
		{
			scanner.set_nuclear_norm_regularization_strength(NORM);
			cout << "Scanner normalization factor: " << NORM << endl;
		}

		unsigned long height = 0;
		unsigned long width = 0;
		get_window_size(boxes, width, height);
		scanner.set_detection_window_size(width, height);
		cout << "Windows detection size: " << width << "x" << height << endl;

		cout << endl;
		cout << "-- TRAINER --" << endl;
		sod_trainer_pd6 trainer(scanner);
		init_trainer(trainer);

		cout << endl;
		cout << "-- DETECTOR --" << endl;
		detector_pd6 detector = trainer.train(images, boxes);
		init_detector(detector);

		cout << endl;
		cout << "-- DETECTOR TEST --" << endl;
		cout << test_object_detection_function(detector, images, boxes) << endl;

		cout << endl;
		cout << "Saving detector to file: " << detectors_dir + datasets[i] + "-detector.svm" << endl;
		dlib::serialize(detectors_dir + datasets[i] + "-detector.svm") << detector;

		cout << "Saving visual interpretation of detector to image: " << detectors_dir + datasets[i] + "-detector.bmp" << endl;
		dlib::save_bmp(dlib::draw_fhog(detector), detectors_dir + datasets[i] + "-detector.bmp");
	}
	cout << "DONE!" << endl;
}

int main(int argc, char *argv[])
{
	try {
		if (argc == 2 && string(argv[1]) == "-h")
		{
			return EXIT_SUCCESS;
		}
		else if (argc == 2)
		{
			bool check = false;
			for (unsigned int i = 0; i < datasets.size(); ++i)
			{
				if (datasets[i] == string(argv[1]))
				{
					check = true;
					break;
				}
			}

			if (check == true)
			{
				std::vector<string> dataset = { string(argv[1]) };
				train_detector(dataset);
				return EXIT_SUCCESS;
			}
			else
			{
				cerr << "Wrong dataset, use -h for help" << endl;
				return EXIT_FAILURE;
			}
		}
		else if (argc == 1)
		{
			train_detector(datasets);
			return EXIT_SUCCESS;
		}
		else
		{
			cerr << "Bad parametres, please check -h for help" << endl;
			return EXIT_FAILURE;
		}

		
	}
	catch (exception& e) {
		cout << "\nexception thrown!" << endl;
		cout << e.what() << endl;
		return EXIT_FAILURE;
	}
}