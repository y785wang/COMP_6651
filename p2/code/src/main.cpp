#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;

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
    for (int k = 0; k < rows*cols/30; ++k) {
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
}




bool isForegroundGrey(float grey, float foreGrey, float backGrey) {
    float foreGreyDiff;
    if (foreGrey > grey) {
        foreGreyDiff = foreGrey - grey;
    } else {
        foreGreyDiff = grey - foreGrey;
    }
    
    float backGreyDiff;
    if (backGrey > grey) {
        backGreyDiff = backGrey - grey;
    } else {
        backGreyDiff = grey - backGrey;
    }

    return foreGreyDiff < backGreyDiff;
}



void findCut(float foregroundGreyValue, float backgroundGreyValue, Mat& grey, Mat& out_image, float greyValue) {
    
    for (int i = 0; i < grey.rows; ++i) {
        for (int j = 0; j < grey.cols; ++j) {
            Vec3b pixel;
            pixel[0] = 0;
            pixel[1] = 0;
            pixel[2] = 0;
            
            if (isForegroundGrey(grey.at<float>(i, j), foregroundGreyValue, backgroundGreyValue)) {
                pixel[2] = 255; // red, foreground
                out_image.at<Vec3b>(i, j) = pixel;
            } else {
                pixel[0] = 255; // blue, background
                out_image.at<Vec3b>(i, j) = pixel;
            }
        }
    }
}




void graphCut(vector<vector<int> >& points, Mat& grey, Mat& out_image) {
    
    float foregroundGreyValue = 0;
    float backgroundGreyValue = 0;
    
    for (int i = 0; i < 4; ++i) {
        int x = points[i][0];
        int y = points[i][1];
        float greyValue = grey.at<float>(y, x);
        foregroundGreyValue += greyValue;
    }
    foregroundGreyValue /= 4;
    
    for (int i = 4; i < 8; ++i) {
        int x = points[i][0];
        int y = points[i][1];
        float greyValue = grey.at<float>(y, x);
        backgroundGreyValue += greyValue;
    }
    backgroundGreyValue /= 4;
    
    int x = points[0][0];
    int y = points[0][1];
    float greyValue = grey.at<float>(y, x);
    
    for (int j = 0; j < grey.rows; ++j) {
        findCut(foregroundGreyValue, backgroundGreyValue, grey, out_image, greyValue);
    }
}




int main( int argc, char** argv )
{
    if(argc!=4){
        cout<<"Usage: ../seg input_image initialization_file output_mask"<<endl;
        return -1;
    }
    
    // Load the input image
    // the image should be a 3 channel image by default but we will double check that in teh seam_carving
    Mat in_image;
    in_image = imread(argv[1]/*, CV_LOAD_IMAGE_COLOR*/);
    if(!in_image.data)
    {
        cout<<"Could not load input image!!!"<<endl;
        return -1;
    }
    if(in_image.channels()!=3){
        cout<<"Image does not have 3 channels!!! "<<in_image.depth()<<endl;
        return -1;
    }
    
    // initial the output image
    Mat out_image = in_image.clone();
    
    // load config file, store the initil pixels
    ifstream f(argv[2]);
    if(!f){
        cout<<"Could not load initial mask file!!!"<<endl;
        return -1;
    }
    int width = in_image.cols;
    int height = in_image.rows;
    int n;
    f>>n;
    vector<vector<int> > points;
    for (int i = 0; i < 8; ++i) {
        vector<int> pair;
        points.push_back(pair);
    }
    for(int i=0;i<n;++i){
        int x, y, t;
        f>>x>>y>>t;
        points[i].push_back(x);
        points[i].push_back(y);

        if(x<0 || x>=width || y<0 || y>=height){
            cout<<"Invalid pixel mask!"<<endl;
            return -1;
        }
    }
    
    // generate grey value for image
    Mat grey = Mat(in_image.rows, in_image.cols, CV_32FC1);
    for (int i = 0; i < in_image.rows; ++i) {
        for (int j = 0; j < in_image.cols; ++j) {
            Vec3b pixel = in_image.at<Vec3b>(i, j);
            grey.at<float>(i, j) = calculate_grey(pixel[0], pixel[1], pixel[2]);
        }
    }
    
    // generate energy(gradient) of the image
    Mat energy = Mat(height, width, CV_32FC1);
    calculate_energy(grey, energy, height, width);
    
    // initialize the two-directrion flow capacity matrix
    Mat flowCapacity = Mat(in_image.rows, in_image.cols, CV_32FC1);
    
    // initialize the two-direction flow residual matrix
    Mat flowResidual = Mat(in_image.rows, in_image.cols, CV_32FC1);
    
    graphCut(points, grey, out_image);

    
    // write it on disk
    imwrite( argv[3], out_image);
    
    // also display them both
    
    namedWindow( "Original image", WINDOW_AUTOSIZE );
    namedWindow( "Show Marked Pixels", WINDOW_AUTOSIZE );
    imshow( "Original image", in_image );
    imshow( "Show Marked Pixels", out_image );
    waitKey(0);
    return 0;
}
