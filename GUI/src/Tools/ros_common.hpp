#pragma once

#include <opencv2/core.hpp>
#include <sensor_msgs/CameraInfo.h>
#include <image_geometry/pinhole_camera_model.h>
//#include <Core/FrameData.h>

class ImageCropTarget {
public:
  ImageCropTarget() = default;

  ImageCropTarget(const sensor_msgs::CameraInfo::ConstPtr &camera_info, const cv::Size &target_dimensions);

  void map_target(std::pair<cv::Mat, cv::Mat> &rgbd);

private:
  image_geometry::PinholeCameraModel camera_model;
  cv::Rect crop_roi;
  cv::Size target_dimensions;
};
