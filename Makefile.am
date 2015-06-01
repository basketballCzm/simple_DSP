
AUTOMAKE_OPTIONS = foreign
AM_CPPFLAGS = -std=c++11  -I/opt/tair_rdb_bin/include \
	-I${TBLIB_ROOT}/include/tbnet -I${TBLIB_ROOT}/include/tbsys 

bin_PROGRAMS=user_op ad_op


user_op_SOURCES=user_op.cpp  user_map.cpp
user_op_LDADD=-lfcgi -luriparser -lc++ -ljsoncpp -ljsoncpp -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a

ad_op_SOURCES=ad_op.cpp  ad_map.cpp user_map.cpp cron_timing.cpp
ad_op_LDADD=-lfcgi -luriparser -lc++ -ljsoncpp -ljsoncpp -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a

noinst_PROGRAMS=robot_commander ap_signal_receiver #ap_signal_dock ap_signal_sender 

robot_commander_SOURCES=robot_commander.cpp user_map.cpp
robot_commander_LDADD=-lc++ -ljsoncpp  -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a

ap_signal_receiver_SOURCES=ap_signal_receiver.cpp user_map.cpp
ap_signal_receiver_LDADD= -lrdkafka -lrdkafka++ -lz -lpthread -lrt -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a -ljsoncpp

#ap_signal_dock_SOURCES=ap_signal_dock.cpp 
#ap_signal_dock_LDADD=-lc++ -lzmq

#ap_signal_sender_SOURCES=ap_signal_sender.cpp 
#ap_signal_sender_LDADD=-lc++ -lzmq


configdir = $(prefix)/bin
config_DATA =  config.ini

SUBDIRS= test
