#include "sc.h"
#include "math.h"
#include <algorithm>
#include <vector>
#include <string>

using namespace cv;
using namespace std;




/******************************************

 calculate_grey:

 calculate the grey value by applying the
 the grey function for each pixel

 1) int: r value
 2) int: g value
 3) int: b value

 *****************************************/
float calculate_grey(int r, int g, int b) {
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}




/******************************************************************

 calculate_energy:

 calcualt the energy by using the gradient (central difference)
 method for each pixel

 1) Mat: grey matrix
 2) Mat: energy matrix
 3) int: # of image rows
 4) int: # of image columns

 *****************************************************************/
void calculate_energy(Mat& grey, Mat& energy, int rows, int cols) {
    for (int i = 1; i < rows-1; ++i) { // set inner area energy
        for (int j = 1; j < cols; ++j) {
            float temp_x = grey.at<float>(i, j-1) - grey.at<float>(i, j+1);
            float temp_y = grey.at<float>(i-1, j) - grey.at<float>(i+1, j);
            energy.at<float>(i, j) = sqrt(temp_x * temp_x + temp_y * temp_y);
        }
    }
    for (int j = 1; j < cols-1; ++j) { // set first row (without two corners) energy
        float temp_x = grey.at<float>(0, j-1) - grey.at<float>(0, j+1);
        float temp_y = grey.at<float>(1, j);
        energy.at<float>(0, j) = sqrt(temp_x * temp_x + temp_y * temp_y);
    }
    for (int j = 1; j < cols-1; ++j) { // set last row (without two corners) energy
        float temp_x = grey.at<float>(rows-1, j-1) - grey.at<float>(rows-1, j+1);
        float temp_y = grey.at<float>(rows-2, j);
        energy.at<float>(rows-1, j) = sqrt(temp_x * temp_x + temp_y * temp_y);
    }
    for (int i = 1; i < rows-1; ++i) { // set first column (without two corners) energy
        float temp_x = grey.at<float>(i, 1);
        float temp_y = grey.at<float>(i-1, 0) - grey.at<float>(i+1, 0);
        energy.at<float>(i, 0) = sqrt(temp_x * temp_x + temp_y * temp_y);
    }
    for (int i = 1; i < rows-1; ++i) { // set last column (without two corners) energy
        float temp_x = grey.at<float>(i, cols-2);
        float temp_y = grey.at<float>(i-1, cols-1) - grey.at<float>(i+1, cols-1);
        energy.at<float>(i, cols-1) = sqrt(temp_x * temp_x + temp_y * temp_y);
    }
    energy.at<float>(0, 0) = sqrt(grey.at<float>(0, 1) * grey.at<float>(1, 0)); // set left-top corner
    energy.at<float>(0, cols-1) = sqrt(grey.at<float>(0, cols-2) * grey.at<float>(1, cols-1)); // set right-top corner
    energy.at<float>(rows-1, 0) = sqrt(grey.at<float>(rows-2, 0) * grey.at<float>(rows-1, 1)); // set left-bot corner
    energy.at<float>(rows-1, cols-1) = sqrt(grey.at<float>(rows-1, cols-2) * grey.at<float>(rows-2, cols-1)); // set left-top corner
}




/********************************************************************************************

 calculate_cu_energy:

 calcualte cumulate energy either vertically or horizontally

 1) Mat: energy matrix
 2) mat: cumulative energy matrix
 3) int: # of image rows
 4) int: # of image columns
 5) string: direction, eigher "VERTICAL" or "HORIZONTAL

 *******************************************************************************************/
void calculate_cu_energy(Mat& energy, Mat& cu_energy, int rows, int cols, string direction) {
    if (direction.compare("VERTICAL") == 0) {
        for (int j = 0; j < cols; ++j) {
            cu_energy.at<float>(0, j) = energy.at<float>(0, j);
        }
        for (int i = 1; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (j == 0) {
                    cu_energy.at<float>(i, j) = energy.at<float>(i, j) + min(cu_energy.at<float>(i-1, 0), cu_energy.at<float>(i-1, 1));
                } else if (j == cols-1) {
                    cu_energy.at<float>(i, j) = energy.at<float>(i, j) + min(cu_energy.at<float>(i-1, cols-1), cu_energy.at<float>(i-1, cols-2));
                } else {
                    cu_energy.at<float>(i, j) = energy.at<float>(i, j) +
                    min(cu_energy.at<float>(i-1, j-1), min(cu_energy.at<float>(i-1, j), cu_energy.at<float>(i-1, j+1)));
                }
            }
        }
    } else if (direction.compare("HORIZONTAL") == 0) {
        for (int i = 0; i < rows; ++i) {
            cu_energy.at<float>(i, 0) = energy.at<float>(i, 0);
        }
        for (int j = 1; j < cols; ++j) {
            for (int i = 0; i < rows; ++i) {
                if (i == 0) {
                    cu_energy.at<float>(i, j) = energy.at<float>(i, j) + min(cu_energy.at<float>(0, j-1), cu_energy.at<float>(1, j-1));
                } else if (i == rows-1) {
                    cu_energy.at<float>(i, j) = energy.at<float>(i, j) + min(cu_energy.at<float>(rows-1, j-1), cu_energy.at<float>(rows-2, j-1));
                } else {
                    cu_energy.at<float>(i, j) = energy.at<float>(i, j) +
                    min(cu_energy.at<float>(i-1, j-1), min(cu_energy.at<float>(i, j-1), cu_energy.at<float>(i+1, j-1)));
                }
            }
        }
    }
}


/********************************************************************************

 seam_carving:

 1) Mat: input mage matrix
 2) int: new width
 3) int: new height
 4) Mat: output image marix

 *******************************************************************************/
bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image) {

    // some sanity checks
    // Check 1 -> new_width <= in_image.cols
    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;
    }
    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;
    }

//    return seam_carving_trivial(in_image, new_width, new_height, out_image);
    return seam_carving_nontrivial(in_image, new_width, new_height, out_image);
}




/*******************************************************************************************
 
 seam_carving_nontrivial:

 1) Mat: input mage matrix
 2) int: new width
 3) int: new height
 4) Mat: output image marix
 
 ******************************************************************************************/
bool seam_carving_nontrivial(Mat& in_image, int new_width, int new_height, Mat& out_image) {
    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();
    Mat grey = Mat(iimage.rows, iimage.cols, CV_32FC1);
    
    int rows = iimage.rows;
    int cols = iimage.cols;
    
    // to grey
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Vec3b pixel = in_image.at<Vec3b>(i, j);
            grey.at<float>(i, j) = calculate_grey(pixel[0], pixel[1], pixel[2]);
        }
    }
    
    // determine if a row or column shoule be removed
    while(iimage.rows != new_height || iimage.cols != new_width) {
        if(iimage.rows > new_height){ // remove row
            reduce_horizontal_seam_nontrivial(iimage, oimage, grey);
            iimage = oimage.clone();
        }
        if(iimage.cols > new_width) { // remove column
            reduce_vertical_seam_nontrivial(iimage, oimage, grey);
            iimage = oimage.clone();
        }
    }
    
    out_image = oimage.clone();
    return true;
}




/*********************************************************************************
 
 reduce_horizontal_seam:

 remove an horiziontal seam 

 1) Mat: input image matrix
 2) Mat: output image matrix
 3) Mat: grey matrix
 
 ********************************************************************************/
bool reduce_horizontal_seam_nontrivial(Mat& in_image, Mat& out_image, Mat& grey) {
    int rows = in_image.rows;
    int cols = in_image.cols;
    out_image = Mat(rows-1, cols, CV_8UC3);
    Mat energy = Mat(rows, cols, CV_32FC1);
    Mat cu_energy = Mat(rows, cols, CV_32FC1);
    
    calculate_energy(grey, energy, rows, cols);
    
    calculate_cu_energy(energy, cu_energy, rows, cols, "HORIZONTAL");
    
    // find a min energy pixle for the beginnign of a min seam, then remove the pixel, copy image
    int min_index = 0;
    float min_energy = cu_energy.at<float>(0, cols-1);
    for (int i = 1; i < rows; ++i) { // fine a min
        if (cu_energy.at<float>(i, cols-1) < min_energy) {
            min_energy = cu_energy.at<float>(i, cols-1);
            min_index = i;
        }
    }
    for (int i = 0; i < rows; ++i) { // remove the min pixel in last column
        if (i < min_index) {
            out_image.at<Vec3b>(i, cols-1) = in_image.at<Vec3b>(i, cols-1);
        } else if (i > min_index) {
            out_image.at<Vec3b>(i-1, cols-1) = in_image.at<Vec3b>(i, cols-1);
            grey.at<float>(i-1, cols-1) = grey.at<float>(i, cols-1);
        }
    }
    
    // find the rest min energy pixel on the min seam, remove them, copy image
    for (int j = cols-2; j >= 0; --j) { // fina a min
        if (min_index == 0) { // left most case
            if (cu_energy.at<float>(0, j) > cu_energy.at<float>(1, j)) {
                min_index = 1;
            }
        } else if (min_index == rows-1) { // right most case
            if (cu_energy.at<float>(rows-2, j) < cu_energy.at<float>(rows-1, j)) {
                min_index = rows-2;
            }
        } else { // middle case
            if (cu_energy.at<float>(min_index-1, j) <= min(cu_energy.at<float>(min_index, j), cu_energy.at<float>(min_index+1, j))) {
                min_index -= 1;
            } else if (cu_energy.at<float>(min_index, j) > cu_energy.at<float>(min_index+1, j)) {
                min_index += 1;
            }
        }
        for (int i = 0; i < rows; ++i) { // remove the min pixel
            if (i < min_index) {
                out_image.at<Vec3b>(i, j) = in_image.at<Vec3b>(i, j);
            } else if (i > min_index) {
                out_image.at<Vec3b>(i-1, j) = in_image.at<Vec3b>(i, j);
                grey.at<float>(i-1, j) = grey.at<float>(i, j);
            }
        }
    }
    
    return true;
}




/*******************************************************************************
 
 reduce_vertical_seam:

 remove an vertical seam 

 1) Mat: input image matrix
 2) Mat: output image matrix
 3) Mat: grey matrix
 
 ******************************************************************************/
bool reduce_vertical_seam_nontrivial(Mat& in_image, Mat& out_image, Mat& grey) {
    int rows = in_image.rows;
    int cols = in_image.cols;
    out_image = Mat(rows, cols-1, CV_8UC3);
    Mat energy = Mat(rows, cols, CV_32FC1);
    Mat cu_energy = Mat(rows, cols, CV_32FC1);
    
    calculate_energy(grey, energy, rows, cols);
    
    calculate_cu_energy(energy, cu_energy, rows, cols, "VERTICAL");
    
    // find a min energy pixle for the beginnign of a min seam, then remove the pixel, copy image
    int min_index = 0;
    float min_energy = cu_energy.at<float>(rows-1, 0);
    for (int j = 1; j < cols; ++j) { // fine a min
        if (cu_energy.at<float>(rows-1, j) < min_energy) {
            min_energy = cu_energy.at<float>(rows-1, j);
            min_index = j;
        }
    }
    for (int j = 0; j < cols; ++j) { // remove the min pixel in last row
        if (j < min_index) {
            out_image.at<Vec3b>(rows-1, j) = in_image.at<Vec3b>(rows-1, j);
        } else if (j > min_index) {
            out_image.at<Vec3b>(rows-1, j-1) = in_image.at<Vec3b>(rows-1, j);
            grey.at<float>(rows-1, j-1) = grey.at<float>(rows-1, j);
        }
    }

    // find the rest min energy pixel on the min seam, remove them, copy image
    for (int i = rows-2; i >= 0; --i) { // fina a min
        if (min_index == 0) { // left most case
            if (cu_energy.at<float>(i, 0) > cu_energy.at<float>(i, 1)) {
                min_index = 1;
            }
        } else if (min_index == cols-1) { // right most case
            if (cu_energy.at<float>(i, cols-2) < cu_energy.at<float>(i, cols-1)) {
                min_index = cols-2;
            }
        } else { // middle case
            if (cu_energy.at<float>(i, min_index-1) <= min(cu_energy.at<float>(i, min_index), cu_energy.at<float>(i, min_index+1))) {
                min_index -= 1;
            } else if (cu_energy.at<float>(i, min_index) > cu_energy.at<float>(i, min_index+1)) {
                min_index += 1;
            }
        }
        for (int j = 0; j < cols; ++j) { // remove the min pixel
            if (j < min_index) {
                out_image.at<Vec3b>(i, j) = in_image.at<Vec3b>(i, j);
            } else if (j > min_index) {
                out_image.at<Vec3b>(i, j-1) = in_image.at<Vec3b>(i, j);
                grey.at<float>(i, j-1) = grey.at<float>(i, j);
            }
        }
    }
    
    return true;
}




//// seam carves by removing trivial seams
//bool seam_carving_trivial(Mat& in_image, int new_width, int new_height, Mat& out_image){
//
//    Mat iimage = in_image.clone();
//    Mat oimage = in_image.clone();
//    while(iimage.rows!=new_height || iimage.cols!=new_width){
//        // horizontal seam if needed
//        if(iimage.rows>new_height){
//            reduce_horizontal_seam_trivial(iimage, oimage);
//            iimage = oimage.clone();
//        }
//
//        if(iimage.cols>new_width){
//            reduce_vertical_seam_trivial(iimage, oimage);
//            iimage = oimage.clone();
//        }
//    }
//
//    out_image = oimage.clone();
//    return true;
//}
//
//// horizontl trivial seam is a seam through the center of the image
//bool reduce_horizontal_seam_trivial(Mat& in_image, Mat& out_image){
//
//    // retrieve the dimensions of the new image
//    int rows = in_image.rows-1;
//    int cols = in_image.cols;
//
//    // create an image slighly smaller
//    out_image = Mat(rows, cols, CV_8UC3);
//
//    //populate the image
//    int middle = in_image.rows / 2;
//
//    for(int i=0;i<=middle;++i)
//        for(int j=0;j<cols;++j){
//            Vec3b pixel = in_image.at<Vec3b>(i, j);
//
//            /* at operator is r/w
//            pixel[0] = 255;
//            pixel[1] =255;
//            pixel[2]=255;
//            */
//
//
//
//            out_image.at<Vec3b>(i,j) = pixel;
//        }
//
//    for(int i=middle+1;i<rows;++i)
//        for(int j=0;j<cols;++j){
//            Vec3b pixel = in_image.at<Vec3b>(i+1, j);
//
//            /* at operator is r/w
//             pixel[0] --> red
//             pixel[1] --> green
//             pixel[2] --> blue
//             */
//
//
//            out_image.at<Vec3b>(i,j) = pixel;
//        }
//
//    return true;
//}
//
//// vertical trivial seam is a seam through the center of the image
//bool reduce_vertical_seam_trivial(Mat& in_image, Mat& out_image){
//    // retrieve the dimensions of the new image
//    int rows = in_image.rows;
//    int cols = in_image.cols-1;
//
//    // create an image slighly smaller
//    out_image = Mat(rows, cols, CV_8UC3);
//
//    //populate the image
//    int middle = in_image.cols / 2;
//
//    for(int i=0;i<rows;++i)
//        for(int j=0;j<=middle;++j){
//            Vec3b pixel = in_image.at<Vec3b>(i, j);
//
//            /* at operator is r/w
//             pixel[0] --> red
//             pixel[1] --> green
//             pixel[2] --> blue
//             */
//
//
//            out_image.at<Vec3b>(i,j) = pixel;
//        }
//
//    for(int i=0;i<rows;++i)
//        for(int j=middle+1;j<cols;++j){
//            Vec3b pixel = in_image.at<Vec3b>(i, j+1);
//
//            /* at operator is r/w
//             pixel[0] --> red
//             pixel[1] --> green
//             pixel[2] --> blue
//             */
//
//
//            out_image.at<Vec3b>(i,j) = pixel;
//        }
//
//    return true;
//}

