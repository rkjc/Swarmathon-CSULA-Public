#include <ros/ros.h>
#include <grid_map_ros/grid_map_ros.hpp>
#include <grid_map_msgs/GridMap.h>
#include <sensor_msgs/Range.h>
#include <cmath>
#include <sstream>
#include <std_msgs/String.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <Eigen/Dense>
#include <nav_msgs/Odometry.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <unistd.h>

using namespace std;

float heartbeat_publish_interval = 2;
const float CELLDIVISION = 0.05;
const float WALL = 1;
const float FOG = -0.5;
const float ROVER = 0.5;
const float DISCOVER = 0.0;

/*----------------MAKE SURE TO TURN FALSE WHEN YOU ARE NOT RUNNING THE SIMULATION----------------*/
/*->->->->->->->->->*/	bool SIMMODE = true;	/*<-<-<-<-<-<-<-<-<-<-<-<-<-<-*/
/*----------------MAKE SURE TO TURN FALSE WHEN YOU ARE NOT RUNNING THE SIMULATION----------------*/

//Publisher
ros::Publisher gridswarmPublisher;
//ros::Publisher mainGridPublisher;
ros::Publisher test;
ros::Publisher heartbeatPublisher;
//Subscriber
ros::Subscriber sonarLeftSubscriber,sonarLeftSubscriber1;
ros::Subscriber sonarCenterSubscriber,sonarCenterSubscriber1;
ros::Subscriber sonarRightSubscriber,sonarRightSubscriber1;
ros::Subscriber odometrySubscriber,odometrySubscriber1;
ros::Subscriber roverNameSubscriber;

//Timer
ros::Timer publish_heartbeat_timer;

std::string publishedName;
//Global
  const double pi = std::acos(-1);
  const int namesArrSize=6;
  string namesArr[namesArrSize] = {"achilles","aeneas","test","test","test","test"};//"ajax""aeneas"
  int arrCount = 0;
  int handleCounter = 0;
  float sleft[namesArrSize];
  float scenter[namesArrSize];
  float sright[namesArrSize];
  float orntn[namesArrSize];
  float xpos[namesArrSize];
  float ypos[namesArrSize];
  char host[128];
  bool firstgo = true;
using namespace std;
using namespace grid_map;
using namespace Eigen;

void publishHeartBeatTimerEventHandler(const ros::TimerEvent& event);

  void odometryHandler(const nav_msgs::Odometry::ConstPtr& message);
  void sonarHandlerLeft(const sensor_msgs::Range::ConstPtr& sonarLeft);
  void sonarHandlerCenter(const sensor_msgs::Range::ConstPtr& sonarCenter);
  void sonarHandlerRight(const sensor_msgs::Range::ConstPtr& sonarRight);
  void odometryHandler1(const nav_msgs::Odometry::ConstPtr& message);
  void sonarHandlerLeft1(const sensor_msgs::Range::ConstPtr& sonarLeft);
  void sonarHandlerCenter1(const sensor_msgs::Range::ConstPtr& sonarCenter);
  void sonarHandlerRight1(const sensor_msgs::Range::ConstPtr& sonarRight);

void roverNameHandler(const std_msgs::String& message);


int main(int argc, char **argv){
  gethostname(host, sizeof (host));
  string hostname(host);
  
 if (argc >= 2) {
   publishedName = argv[1];
   cout << "Welcome to the world of tomorrow " << publishedName
        << "!  GridMapSwarm turnDirectionule started." << endl;
 } else {
   publishedName = hostname;
   cout << "No Name Selected. Default is: " << publishedName << endl;
 }
  
  // NoSignalHandler so we can catch SIGINT ourselves and shutdown the node
  ros::init(argc, argv, (hostname + "_GRIDSWARM"), ros::init_options::NoSigintHandler);
  ros::NodeHandle gNH;
//PUBLISH
  heartbeatPublisher = gNH.advertise<std_msgs::String>((publishedName + "/gridSwarm/heartbeat"), 1,true);
  publish_heartbeat_timer = gNH.createTimer(ros::Duration(heartbeat_publish_interval),publishHeartBeatTimerEventHandler);
//  gridswarmPublisher = gNH.advertise<grid_map_msgs::GridMap>(publishedName + "/grid_map", 1);

//SUBSCRIBER
//  roverNameSubscriber = gNH.subscribe(("/roverNames"), 1, roverNameHandler);

  if (publishedName != namesArr[0]){
	cout << publishedName << " not first listed. Ending Grid-Map" <<endl;
	return 0;
  }

  cout << publishedName << " Was Listed first listed. Starting Grid-Map" <<endl;
  gridswarmPublisher = gNH.advertise<grid_map_msgs::GridMap>("/grid_map", 1);

  for(int i = 0; i < namesArrSize; i++){
	cout << "namesArr[" << i << "] =" << namesArr[i] <<":Start Loop"<<endl;
	if (namesArr[i] != "test"){
		arrCount = i;
		handleCounter = i;
		publishedName = namesArr[i];
		cout << "Entered Subscriber loop: "<< publishedName <<"["<<arrCount<<"]"<< " + HC:"<< handleCounter << endl;
		if (i == 0){
			odometrySubscriber = gNH.subscribe((publishedName + "/odom/filtered"), 10, odometryHandler);
			sonarLeftSubscriber = gNH.subscribe((publishedName + "/sonarLeft"), 10, sonarHandlerLeft);
			sonarCenterSubscriber = gNH.subscribe((publishedName + "/sonarCenter"), 10, sonarHandlerCenter);
			sonarRightSubscriber = gNH.subscribe((publishedName + "/sonarRight"), 10, sonarHandlerRight);
		}else if(i == 1){
			odometrySubscriber1 = gNH.subscribe((publishedName + "/odom/filtered"), 10, odometryHandler1);
			sonarLeftSubscriber1 = gNH.subscribe((publishedName + "/sonarLeft"), 10, sonarHandlerLeft1);
			sonarCenterSubscriber1 = gNH.subscribe((publishedName + "/sonarCenter"), 10, sonarHandlerCenter1);
			sonarRightSubscriber1 = gNH.subscribe((publishedName + "/sonarRight"), 10, sonarHandlerRight1);
		}
	//	message_filters::Subscriber<sensor_msgs::Range> sonarLeftSubscriber(gNH, (publishedName + "/sonarLeft"), 10);
	//	message_filters::Subscriber<sensor_msgs::Range> sonarCenterSubscriber(gNH, (publishedName + "/sonarCenter"), 10);
	//	message_filters::Subscriber<sensor_msgs::Range> sonarRightSubscriber(gNH, (publishedName + "/sonarRight"), 10);
	//	typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Range, sensor_msgs::Range, sensor_msgs::Range> sonarSyncPolicy;
	//	message_filters::Synchronizer<sonarSyncPolicy> sonarSync(sonarSyncPolicy(10), sonarLeftSubscriber, sonarCenterSubscriber, sonarRightSubscriber);
	//	sonarSync.registerCallback(boost::bind(&sonarHandler, _1, _2, _3));
	}//END OF IF STATEMENT
  }//END OF FOR LOOP
  cout << " + Exit Subscriber loop -"<< "- arrCount:"<<arrCount<< " + HC:"<< handleCounter << endl;
  


  ros::Rate rate(40.0);
  // Create grid Rover Specific Map.
  GridMap map({"elevation"});
  map.setFrameId("map");
  map.setGeometry(Length(15.5, 15.5), CELLDIVISION);
  ROS_INFO("Created map with size %f x %f m (%i x %i cells).",
    map.getLength().x(), map.getLength().y(),
    map.getSize()(0), map.getSize()(1));  

  while (ros::ok()) {
	cout << "Enter ROS OK"<<endl;
	// Add data to Rover Specific Grid Map.
	ros::Time time = ros::Time::now();
	for (GridMapIterator it(map); !it.isPastEnd(); ++it) {
		Position position;
		map.getPosition(*it, position);
		if (map.at("elevation", *it) == FOG || map.at("elevation", *it) == ROVER || firstgo == true){
			map.at("elevation", *it) = FOG;
		}
		//ORIGIN
//	Vector2d o(0,0);
//	map.atPosition("elevation", o) = 0;
//		cout << "Entering Grid-Map Data";
		for(int count = arrCount; count >= 0; count--){
//			cout << "!!Entered Grid-Map Data: "<< count<< endl;
			//ROVER
			float qx = xpos[count];
			float qy = ypos[count];
			Vector2d q(qx,qy);
			if (map.isInside(q)){
				map.atPosition("elevation", q) = ROVER;
			}
			//CAMERA 0.3m
			for (float length = CELLDIVISION; length <= 0.3;){
				for(float width = -0.15; width <= 0.15;){
					float Cax = (cos(orntn[count]) * length) + (xpos[count] + (sin(orntn[count]) * width));
					float Cay = (sin(orntn[count]) * length) + (ypos[count] + (cos(orntn[count]) * width));
					Vector2d cam(Cax,Cay);
					if (map.isInside(cam)){
						map.atPosition("elevation", cam) = DISCOVER;
					}
					width += CELLDIVISION;
				}
				length += CELLDIVISION;
			}
			//CENTER
			//THEORY: MAKE SURE PING HITS MULTIPLE TIMES IN A SECOND BEFORE SAVING.
			if (scenter[count] <= 2.8){
				float cx = (cos(orntn[count]) * scenter[count]) + xpos[count];
				float cy = (sin(orntn[count]) * scenter[count]) + ypos[count];
				Vector2d c(cx,cy);
				if (map.isInside(c)){
					map.atPosition("elevation", c) = WALL;
				}
			}
			//LEFT
			if (sleft[count] <= 2.8){
				float lx = (cos((pi/6)+orntn[count]) * sleft[count]) + xpos[count];
				float ly = (sin((pi/6)+orntn[count]) * sleft[count]) + ypos[count];
				Vector2d l(lx,ly);
				if (map.isInside(l)){
					map.atPosition("elevation", l) = WALL;
				}
			}
			//RIGHT
			if (sright[count] <= 2.8){
				float rx = (cos(-1*(pi/6)+orntn[count]) * sright[count]) + xpos[count];
				float ry = (sin(-1*(pi/6)+orntn[count]) * sright[count]) + ypos[count];
				Vector2d r(rx,ry);
				if (map.isInside(r)){
					map.atPosition("elevation", r) = WALL;
				}
			}
		}
	}//END OF ITERATOR
	firstgo = false;

	// Publish grid map.
	map.setTimestamp(time.toNSec());
	grid_map_msgs::GridMap message;
	//PUBLISH Rover Specific Grid Map
	GridMapRosConverter::toMessage(map, message);
	gridswarmPublisher.publish(message);
	ROS_INFO_THROTTLE(1.0, "Grid map (timestamp %f) published.", message.info.header.stamp.toSec());
	
	// Wait for next cycle.
	ros::spinOnce();
	rate.sleep();
  }//END OF ROS OK

return 0;
}

void publishHeartBeatTimerEventHandler(const ros::TimerEvent&){
	std_msgs::String msg;
	msg.data = "";
	heartbeatPublisher.publish(msg);
}
void sonarHandlerLeft(const sensor_msgs::Range::ConstPtr& sonarLeft) {
	float simoffsetLeft = 0;
	if(SIMMODE == true){
		simoffsetLeft = ((sonarLeft->range)/cos(pi/6)) - (sonarLeft->range); 
	}
	sleft[0]  = ((float(int(10 * sonarLeft->range)))/10) + simoffsetLeft;
}

void sonarHandlerLeft1(const sensor_msgs::Range::ConstPtr& sonarLeft) {
	float simoffsetLeft = 0;
	if(SIMMODE == true){
		simoffsetLeft = ((sonarLeft->range)/cos(pi/6)) - (sonarLeft->range); 
	}
	sleft[1]  = ((float(int(10 * sonarLeft->range)))/10) + simoffsetLeft;
}

void sonarHandlerCenter(const sensor_msgs::Range::ConstPtr& sonarCenter) {
	scenter[0]= ((float(int(10 * sonarCenter->range)))/10);
}
void sonarHandlerCenter1(const sensor_msgs::Range::ConstPtr& sonarCenter) {
	scenter[1]= ((float(int(10 * sonarCenter->range)))/10);
}

void sonarHandlerRight(const sensor_msgs::Range::ConstPtr& sonarRight) {
	float simoffsetRight = 0;
	if(SIMMODE == true){
		simoffsetRight = ((sonarRight->range)/cos(pi/6)) - (sonarRight->range); 
	}
	sright[0] = ((float(int(10 * sonarRight->range)))/10) + simoffsetRight;
}
void sonarHandlerRight1(const sensor_msgs::Range::ConstPtr& sonarRight) {
	float simoffsetRight = 0;
	if(SIMMODE == true){
		simoffsetRight = ((sonarRight->range)/cos(pi/6)) - (sonarRight->range); 
	}
	sright[1] = ((float(int(10 * sonarRight->range)))/10) + simoffsetRight;
}

void odometryHandler(const nav_msgs::Odometry::ConstPtr& message) {
	string rover = message->header.frame_id;
	rover = rover.substr(0,rover.find("/"));
	cout << rover << handleCounter << " entered odom Subscriver0" << endl;
	tf::Quaternion q(message->pose.pose.orientation.x, message->pose.pose.orientation.y, message->pose.pose.orientation.z, message->pose.pose.orientation.w);
	tf::Matrix3x3 m(q);
	double roll, pitch, yaw;
	m.getRPY(roll, pitch, yaw);
	orntn[0] = yaw;
	xpos[0] = message->pose.pose.position.x;
	ypos[0] = message->pose.pose.position.y;
}
void odometryHandler1(const nav_msgs::Odometry::ConstPtr& message) {
	string rover = message->header.frame_id;
	rover = rover.substr(0,rover.find("/"));
	cout << rover << handleCounter << " entered odom Subscriver1" << endl;
	tf::Quaternion q(message->pose.pose.orientation.x, message->pose.pose.orientation.y, message->pose.pose.orientation.z, message->pose.pose.orientation.w);
	tf::Matrix3x3 m(q);
	double roll, pitch, yaw;
	m.getRPY(roll, pitch, yaw);
	orntn[1] = yaw;
	xpos[1] = message->pose.pose.position.x;
	ypos[1] = message->pose.pose.position.y;
}

void roverNameHandler(const std_msgs::String& message){
	for(int i=0;i<namesArrSize; i++){
		if(namesArr[i].compare("test") == 0){
			namesArr[i] = message.data;
			i=7;
		}
		else if(namesArr[i].compare(message.data)==0){
			i=7;
		}
	}
	for(int i=0; i<namesArrSize; i++){ 
	//	cout << "namesArr[" << i << "] =" << namesArr[i]<< endl;
		// allRoversPublisher.publish(message);
	}
}
