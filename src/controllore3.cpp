
/** @file controllore3.cpp
 * \brief Keyboard controller for changing the pose of a Firefly hexacopter (in a simulated evironment) 
 * Usage: WASD keys to move forward, backward, left and right, QE to move up and down, XZ to rotate.
 */

#include "ros/ros.h"
#include "geometry_msgs/PoseStamped.h"

#include <termios.h>
#include <signal.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define KEYCODE_A 0x61
#define KEYCODE_D 0x64
#define KEYCODE_S 0x73
#define KEYCODE_W 0x77 
#define KEYCODE_Q 0x71
#define KEYCODE_E 0x65
#define KEYCODE_Z 0x7a
#define KEYCODE_X 0x78

#define N 8
#define SIN 0
#define COS 1

/** Matrix with (sin, cos) theta/2 for every theta in 360 deg : 45 deg */
float rotations[N][2] = {
		{0, 1},
		{0.383, 0.924},
		{0.707, 0.707},
		{0.924, 0.383},
		{1, 0},
		{-0.924, 0.383},
		{-0.707, 0.707},
		{-0.383, 0.924}
	};

/** Rotations matrix index */
int r = 0; 

/** Desired pose publisher */
ros::Publisher goal_pub;
/** Message to be filled with the desired pose  */
geometry_msgs::PoseStamped cmd;


int kfd = 0;
struct termios cooked, raw;


void quit(int sig) {
	tcsetattr(kfd, TCSANOW, &cooked);
	exit(0);
}

int main(int argc, char **argv) {
	// default position
	cmd.header.frame_id = "base_link";
	cmd.pose.position.x = 0; 
	cmd.pose.position.y = 0; 
	cmd.pose.position.z = 0;   
	cmd.pose.orientation.w = 1; 
	cmd.pose.orientation.x = 0; 
	cmd.pose.orientation.y = 0; 
	cmd.pose.orientation.z = 0;

	ros::init(argc, argv, "controllore3");
	ros::NodeHandle n;
	goal_pub = n.advertise<geometry_msgs::PoseStamped>("/firefly/command/pose", 1000);
	ros::Rate loop_rate(50);

	// keyboard loop
	char c;
	bool dirty = false;
	
	// get the console in raw mode
	tcgetattr(kfd, &cooked);
	memcpy(&raw, &cooked, sizeof(struct termios));
	raw.c_lflag &=~ (ICANON | ECHO);

	// setting a new line, then end of file
	raw.c_cc[VEOL] = 1;
	raw.c_cc[VEOF] = 2;
	tcsetattr(kfd, TCSANOW, &raw);

	// print usage info
	puts("Reading from keyboard");
	puts("---------------------------");
	puts("Use 'WS' to forward/back");
	puts("Use 'AD' to left/right");
	puts("Use 'QE' to up/down");
	puts("Use 'ZX' to yaw counterclockwise/clockwise");

 	signal(SIGINT, quit);

	while (ros::ok()) {
		//  get the next event from the keyboard
		if(read(kfd, &c, 1) < 0) {
			perror("Error");
			exit(-1);
		}

		// keyboard mapping
		switch(c) { 	 
			case KEYCODE_W:
				cmd.pose.position.x = cmd.pose.position.x + 0.1;
				dirty = true;
				break;
			case KEYCODE_S:
				cmd.pose.position.x = cmd.pose.position.x - 0.1;
				dirty = true;
				break;
			case KEYCODE_A:
				cmd.pose.position.y = cmd.pose.position.y + 0.1;
				dirty = true;
				break;
			case KEYCODE_D:
				cmd.pose.position.y = cmd.pose.position.y - 0.1;
				dirty = true;
				break;
			case KEYCODE_Q:
				cmd.pose.position.z = cmd.pose.position.z + 0.1;
				dirty = true;
				break;
			case KEYCODE_E:
				cmd.pose.position.z = cmd.pose.position.z - 0.1;
				dirty = true;
				break;
			case KEYCODE_Z: // make a 45 deg counter-clockwise rotation
				r = (r + 1) % N;
				cmd.pose.orientation.z = rotations[r][SIN];
				cmd.pose.orientation.w = rotations[r][COS];
				dirty = true;
				break;
			case KEYCODE_X: // make a 45 deg clockwise rotation
				r = (r + N - 1) % N;
				cmd.pose.orientation.z = rotations[r][SIN];
				cmd.pose.orientation.w = rotations[r][COS];
				dirty = true;
				break;
		}
		
		if(dirty == true) {
			goal_pub.publish(cmd);
		}

		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
