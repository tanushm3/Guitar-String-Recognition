# Guitar String Recognition #

Welcome. This project uses OpenCV to find the guitar strings from an image of a guitar.

## Setup ##

After downloading this project, you should change the `IMG_SRC` constant on line 16 of `main.cpp`. To test with different images, just change the number in the filename. It is easiest to use the absolute path rather than the relative one.

## Building ##

The project has been built using Xcode and the clang compiler. However, it should be possible to build using g++. 

First of all, you need to find the library and include paths where OpenCV is located on your machine.

When building with g++, use these paths and include the following libraries:
`libopencv_core`
`libopencv_highgui`
`libopencv_imgproc`

If this fails, include any other OpenCV libraries that are needed.

## Running ##

Run the executable produced by the compiler. The program will show the edge detected image and the original image with lines overlayed on the strings.
