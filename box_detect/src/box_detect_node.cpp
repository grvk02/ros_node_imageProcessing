#include<ros/ros.h>
#include<std_msgs/String.h>
#include <opencv2/opencv.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

using namespace cv;

ros::Publisher pub;
image_transport::Publisher image_pub_;

void center(const sensor_msgs::ImageConstPtr& msg){
cv_bridge::CvImagePtr cv_ptr;
cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
Mat src =cv_ptr->image;
Mat img;
cvtColor(src, img, COLOR_BGR2GRAY); 

threshold(img,img,60,255,THRESH_BINARY_INV );
threshold(img,img,60,255,THRESH_BINARY_INV );
erode(img,img, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
dilate( img, img, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
dilate( img, img, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
erode(img, img, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

Moments m=moments(img,true);
sensor_msgs::ImagePtr msgd = cv_bridge::CvImage(std_msgs::Header(), "mono8", img).toImageMsg();
image_pub_.publish(msgd);
std_msgs::String msgs;
msgs.data="Cordinates of center is  "+std::to_string(m.m10/m.m00)+","+std::to_string(m.m01/m.m00);
pub.publish(msgs);
}

int main(int argc,char** argv){
ros::init(argc,argv,"box_detect_node");
ros::NodeHandle nh;
image_transport::ImageTransport it(nh);
image_transport::Subscriber image_sub;
image_pub_ = it.advertise("/image_converter/output_video", 1);
image_sub=it.subscribe("/magnus/camera/image_raw",1,center);
pub=nh.advertise<std_msgs::String>("coordinates",1);
ros:: Rate loopRate(30);
while(ros::ok()){
ros::spinOnce();
loopRate.sleep();
}

return 0;
}

