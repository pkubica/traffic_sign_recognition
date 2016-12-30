# traffic_sign_recognition
Traffic sign recognition based on Dlib and OpenCV project
School project (POV)

Learning and detecting traffic sign in images and video.

![Recognition](https://raw.githubusercontent.com/pkubica/traffic_sign_recognition/master/results/recognize-01.png)

### Dependency
- Dlib 19.2 (need compile and copy to resources/dlib-19.2 see README.md)
- OpenCV 3.1.0 (need compile and copy to resources/opencv-310 see README.md) 
- libjpeg
- libpng
- zlib

### Use
After build this project in VS2015 you can find all necessary files in build folder

Folder structure:
- build 
  - signs - folder with images of signs
  - datasets - contains xml documents, input floder for trainer
  - descriptor - contains descriptors, output folder for trainer
  - tools - contains imglab.exe for creating datasets
  
trainer run parameters
- without parameters - learn all datasets in folder datasets
- one of these: "stop", "crossing", "give-way", "oneway-exit" , "main", "parking" - learn only specified dataset. see ./global/config.h

run example:
```
./traffic_sign_trainer
./traffic_sign_trainer stop
./traffic_sign_trainer main
```

detector run parameters
- "-i image.bmp" detect signs in image.bmp
- "-v video.mp4" detect signs in video.mp4

run example:
```
./traffic_sign_detector -i image.bmp
./traffic_sign_detector -v video.mp4
```
  


