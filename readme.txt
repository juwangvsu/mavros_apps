-------------1/23/2022----------------------------------a

rebuild for ub18 ros-melodic
fix CMakeLists.txt by add melodic path to link to the libmavros.so

-------------5/28/2019------------------------------------

	this package is copied based on a strip down of mavros/mavros to have
	offb_node only.

	future mavros apps can also be here.

	the reason for the new pkg is to get ride of the ros search
	path confusion if we build turtlebot/src/mavros/mavros.
	if built, its search path will override that in /opt/ros/...

	the sympotum is rosrun mavros mavros_node will compliant fail to
	locate, since the strip-down mavros does not build mavros_node.

	hence we have to put a CATKIN_IGNORE in mavros/mavros, and
	use this package to host offb_node.

	this noded is called at mavroscall.sh @ ~/turtlebot/src/rqt_mypkg/scripts
	to run uav in offboard mode and circle around.

	to build this package,
		catkin_make --pkg mavros_apps

