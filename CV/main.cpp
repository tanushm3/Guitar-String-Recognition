//
//  main.cpp
//  CV
//
//  Created by Simon Crequer on 25/02/16.
//  Copyright © 2016 Simon Crequer. All rights reserved.
//

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define IMG_SRC "/Users/simon/Documents/Xcode/CV/CV/Images/IMG_0010.jpg"

using namespace cv;

Mat src, img;

int main()
{
    src = imread(IMG_SRC, CV_LOAD_IMAGE_COLOR);
    src.copyTo(img);

    Mat dst, cdst;
    Canny(src, dst, 50, 200, 3);
    GaussianBlur(dst, dst, Size(1,3), 1.0);
    cvtColor(dst, cdst, CV_GRAY2BGR);
    
    std::vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI/180, 250, 50, 50 );
    float angles[lines.size()]; // gradients of lines
    std::vector<Vec4i> drawn_lines;
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        float dy = l[3] - l[1];
        float dx = l[2] - l[0];
        float theta = cvFastArctan(dy, dx);
        
        // make sure it's not too close to any previously drawn lines
        bool should_continue = false;
        for (size_t j = 0; j < drawn_lines.size(); j++) {
            Vec4i d = drawn_lines[j];
            if (abs(d[1] - l[1]) < 8 || abs(d[3] - l[3]) < 8) {
                should_continue = true;
                break;
            }
            
            if (d[0] < l[0] && d[2] < l[2]) {
                
                float x_proportion = (l[0] - d[0]) / (d[2] - d[0]);
                float x1 = d[0] + x_proportion * (d[2] - d[0]);
                float y1 = d[1] + x_proportion * (d[3] - d[1]);
                
                if (sqrt(pow(x1-l[0], 2) + pow(y1-l[1], 2)) < 10) {
                    //should_continue = true;
                }
                
            } else if (d[0] > l[0] && d[2] > l[2]) {
                
            }
        }
        
        if (l[0] > src.cols/8 || l[2] < src.cols/6) {
            should_continue = true;
        }
        
        if (should_continue) {
            continue;
        }
        
        //theta *= 180/CV_PI;
        //std::cout << theta << std::endl;
        
        if (theta > 345 && theta < 360) {
            //line( src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
            drawn_lines.insert(drawn_lines.begin(), l);
            angles[drawn_lines.size() - 1] = theta;
            //std::cout << theta << std::endl;
        }
    }
    std::cout << drawn_lines.size() << std::endl;
    
    float grads[drawn_lines.size()];
    // Extend the lines to go right across the image
    for (int i = 0; i < drawn_lines.size(); i++) {
        Vec4i l = drawn_lines[i];
        float grad = ((float)(l[3] - l[1]))/((float)(l[2]-l[0]));
        grads[i] = grad;
        //std::cout << l[0] << " " << l[1] << " " << l[2] << " " << l[3] << "    " << grad << std::endl;
        drawn_lines[i][0] = 0;
        //drawn_lines[i][1] = l[1] - (grad * l[0] / (float)(l[2] - l[0]));
        drawn_lines[i][1] = l[1] - (grad * l[0]);
        drawn_lines[i][2] = src.cols;
        //drawn_lines[i][3] = l[3] + (grad * (src.cols - l[2]) / (float)(l[2] - l[0]));
        drawn_lines[i][3] = l[3] + (grad * (src.cols - l[2]));
        
        l = drawn_lines[i];
    }
    
    float sum = 0.0;
    for (int i = 0; i < drawn_lines.size(); i++) {
        sum += grads[i];
    }
    float avg_grad = sum/drawn_lines.size();
    
    // Get rid of unwanted lines
    for (int i = 0; drawn_lines.size() > 6 && i < drawn_lines.size(); i++) {
        for (int j = 0; j < drawn_lines.size(); j++) {
            if (i == j) {
                continue;
            } else {
                if (abs(drawn_lines[i][1] - drawn_lines[j][1]) < 10) {
                    std::cout << "Hello!" << std::endl;
                    float i_diff = abs((drawn_lines[i][3]-drawn_lines[i][1]) / (drawn_lines[i][2]-drawn_lines[i][0]) - avg_grad);
                    float j_diff = abs((drawn_lines[j][3]-drawn_lines[j][1]) / (drawn_lines[j][2]-drawn_lines[j][0]) - avg_grad);
                    if (i_diff > j_diff) {
                        drawn_lines.erase(drawn_lines.begin() + i);
                    } else {
                        drawn_lines.erase(drawn_lines.begin() + j);
                    }
                }
            }
        }
    }
    
    // If there are still more than 6 lines, get rid of any with the biggest angle difference
    while (drawn_lines.size() > 6) {
        int largest_index = 0;
        Vec4i l = drawn_lines[0];
        float largest = ((float)l[3]-l[1])/(l[2]-l[0]);
        
        for (int i = 1; i < drawn_lines.size(); i++) {
            l = drawn_lines[i];
            float grad = ((float)l[3]-l[1])/(l[2]-l[0]);
            if (grad < largest) {
                largest = grad;
                largest_index = i;
            }
        }
        
        drawn_lines.erase(drawn_lines.begin() + largest_index);
    }
    
    float new_grads[6];
    for (int i = 0; i < 6; i++) {
        Vec4i l = drawn_lines[i];
        new_grads[i] = ((float)l[3]-l[1])/(l[2]-l[0]);
        std::cout << new_grads[i] << std::endl;
    }
    std::sort(new_grads, new_grads + 6);
    // Find median gradient
    float median = ((float)new_grads[2] + new_grads[3]) / 2.0;
    std::cout << median << std::endl;
    
    std::cout << drawn_lines.size() << std::endl;
    for (int i = 0; i < drawn_lines.size(); i++) {
        Vec4i l = drawn_lines[i];
        float grad = ((float)l[3]-l[1])/(l[2]-l[0]);
        
        // Adjust to median gradient
        float n = 5.0;
        float px = src.cols/n;
        float py = l[1] + grad * px;
        
        l[0] = 0;
        l[1] = py - median * px;
        l[2] = src.cols;
        l[3] = py + median * (src.cols - px);
        
        line( src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
    }

    resize(src, src, Size(src.cols/2, src.rows/2));
          resize(cdst, cdst, Size(cdst.cols/2, cdst.rows/2));
    imshow("lines", src);
    imshow("edges", cdst);
    std::cout << src.cols << std::endl;
    
    waitKey();
     
    return 0;
}