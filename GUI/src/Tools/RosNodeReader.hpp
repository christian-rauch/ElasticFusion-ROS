#pragma once
#include "LogReader.h"
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <sensor_msgs/CompressedImage.h>
#include <ros/ros.h>
#include <image_transport/subscriber_filter.h>
#include <message_filters/sync_policies/approximate_time.h>
#include "ros_common.hpp"
//#include <Utils/GroundTruthOdometryInterface.hpp>
#include <Eigen/Geometry>
#include <tf2_ros/transform_listener.h>


class RosNodeReader : public LogReader/*, public GroundTruthOdometryInterface*/ {
public:
  RosNodeReader(const uint32_t synchroniser_queue_size,
                const bool flipColors = false, const cv::Size &target_dimensions = {},
                const std::string frame_gt_camera = {});

  ~RosNodeReader();

  void on_rgbd(const sensor_msgs::ImageConstPtr& msg_colour, const sensor_msgs::ImageConstPtr& msg_depth);

  void getNext() override;

  int getNumFrames() override;

  bool hasMore() override;

  bool rewound() override;

  void rewind() override;

  void getBack() override;

  void fastForward(int frame) override;

  const std::string getFile() override;

  void setAuto(bool value) override;

//  FrameData getFrameData() override;

//  Eigen::Matrix4f getIncrementalTransformation(uint64_t timestamp) override;

private:
  typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> ApproximateTimePolicy;

  // node
  std::unique_ptr<ros::NodeHandle> n;
  std::unique_ptr<image_transport::ImageTransport> it;
  std::unique_ptr<ros::AsyncSpinner> spinner;

  // ground truth camera poses in root frame
  std::string frame_gt_root;
  std::string frame_gt_camera;
  tf2::BufferCore tf_buffer;
  std::unique_ptr<tf2_ros::TransformListener> tf_listener;
  Eigen::Isometry3d ref_pose;

  // topics
  image_transport::SubscriberFilter sub_colour;
  image_transport::SubscriberFilter sub_depth;
  std::unique_ptr<message_filters::Synchronizer<ApproximateTimePolicy>> sync;

  // scale&crop to target dimensions
  ImageCropTarget image_crop_target;

  std::mutex mutex;
//  FrameData data;
  // colour-depth-pair
  std::pair<cv::Mat, cv::Mat> rgbd;
};
