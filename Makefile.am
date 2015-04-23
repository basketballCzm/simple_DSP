
AUTOMAKE_OPTIONS = foreign
AM_CPPFLAGS = -std=c++11 -stdlib=libc++ -Weverything -Wno-c++98-compat -I/usr/local/include/c++/v1 -fcolor-diagnostics

bin_PROGRAMS=adstat ad_register  ad_request user_op
adstat_SOURCES=adstat.cpp 
adstat_LDADD=-lfcgi -luriparser -lc++ 

ad_register_SOURCES=ad_register.cpp 
ad_register_LDADD=-lfcgi -luriparser -lc++ 

ad_request_SOURCES=ad_request.cpp user_map.cpp 
ad_request_LDADD=-lfcgi -luriparser -lc++ -ljsoncpp

user_op_SOURCES=user_op.cpp  user_map.cpp
user_op_LDADD=-lfcgi -luriparser -lc++ -ljsoncpp

noinst_PROGRAMS=robot_commander #ap_signal_dock ap_signal_sender 

robot_commander_SOURCES=robot_commander.cpp user_map.cpp
robot_commander_LDADD=-lc++ -ljsoncpp

#ap_signal_dock_SOURCES=ap_signal_dock.cpp 
#ap_signal_dock_LDADD=-lc++ -lzmq

#ap_signal_sender_SOURCES=ap_signal_sender.cpp 
#ap_signal_sender_LDADD=-lc++ -lzmq
