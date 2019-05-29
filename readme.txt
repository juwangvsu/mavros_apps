-------------5/28/2019------------------------------------
	the mavros code here is old. so don't build and
	use the binary code here. the mavros installed
	under /opt/ros/... should be used. this mean that
	make sure the rospath and pythonpath are correct
	when calling mavros cmd etc.
	
	only launch file are usful.

	in particular, the local workspace should not contain
	executable stuff from this folder. if they exist
	and cause interference to mavros binary under /opt/ros,
	remove devel, build and make sure ignore file is put here
	and rebuild other turtlebot pkgs.

		also uav2cam.ui adding a few button to call rosservice directly for testing
			add sitl checkbox to launch individual sitl instance.

		also add  CATKIN_IGNORE on all ~/turtlebot/src/mavros/mavros subpkgs
			and rm devel, build, and catkin_make
			so mavsys will use the python library @ /opt/ros/...
		also add CATKIN_IGNORE on ~/turtlebot/src/turtlebot_apps/ to build
