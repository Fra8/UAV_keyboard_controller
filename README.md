# UAV_keyboard_controller

**Authors:** Erika Cocca, Carola Motolese, Alberto Rivolta 

## Prerequisites

### ROS 
We use [ROS](http://www.ros.org/). 

### RotorS 
We use [RotorS](https://github.com/ethz-asl/rotors_simulator/) for UAV gazebo simulation. Download and install instructions can be found at: https://github.com/ethz-asl/rotors_simulator/ .

## Running keyboard controller node
To launch the system with default parameters type:
```
rosrun crm_ros controllore3
```

## Usage
Reading from keyboard
	---------------------------
	Use 'WS' to forward/back
	Use 'AD' to left/right
	Use 'QE' to up/down
	Use 'ZX' to yaw counterclockwise/clockwise 
