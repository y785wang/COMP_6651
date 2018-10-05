#ifndef SEAMCARVINGCOMP665156
#define SEAMCARVINGCOMP665156

#include <opencv2/opencv.hpp>

// the function you need to implement - by defaiult it calls seam_carving_trivial
bool seam_carving(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);

bool seam_carving_trivial(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);

bool reduce_horizontal_seam_trivial(cv::Mat& in_image, cv::Mat& out_image);

bool reduce_vertical_seam_trivial(cv::Mat& in_image, cv::Mat& out_image);

bool seam_carving_nontrivial(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);


bool reduce_vertical_seam_nontrivial(cv::Mat& in_image, cv::Mat& out_image, cv::Mat& grey);
bool reduce_horizontal_seam_nontrivial(cv::Mat& in_image, cv::Mat& out_image, cv::Mat& grey);


bool reduce_vertical_seam_nontrivial(cv::Mat& in_image, cv::Mat& out_image, cv::Mat& grey, cv::Mat& energy);
bool reduce_vertical_seam_nontrivial(cv::Mat& in_image, cv::Mat& out_image, cv::Mat& grey, cv::Mat& energy, cv::Mat& cu_energy);

#endif

