AUTOMAKE_OPTIONS = foreign 
TBLIB_ROOT = /usr/local
AM_CPPFLAGS = -std=c++11  -I/opt/tair_rdb_bin/include \
	-I${TBLIB_ROOT}/include/tbnet -I${TBLIB_ROOT}/include/tbsys \
	-I/usr/include/jsoncpp

bin_PROGRAMS=user_op ad_op ap_signal_receiver user_tag_receiver user_phone_receiver


user_op_SOURCES=user_op.cpp  user_map.cpp
user_op_LDADD=-lfcgi -luriparser -ljsoncpp -lpthread -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a -lpqxx -lpq

ad_op_SOURCES=ad_op.cpp  ad_map.cpp user_map.cpp cron_timing.cpp
ad_op_LDADD=-lfcgi -luriparser -ljsoncpp -lpthread -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a -lpqxx -lpq

noinst_PROGRAMS=robot_commander #ap_signal_dock ap_signal_sender 

robot_commander_SOURCES=robot_commander.cpp user_map.cpp
robot_commander_LDADD=-ljsoncpp -lpthread -L/home/caster/tair_rdb_bin/lib -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a -lpqxx -lpq

ap_signal_receiver_SOURCES=ap_signal_receiver.cpp user_map.cpp
ap_signal_receiver_LDADD= -lz -lpthread -lrdkafka++ -lrdkafka -lrt  -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a -ljsoncpp -lpthread -lpqxx -lpq

user_tag_receiver_SOURCES=user_tag_receiver.cpp user_map.cpp
user_tag_receiver_LDADD= -lz -lpthread  -lrdkafka++ -lrdkafka -lrt  -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a -ljsoncpp -lpthread -lpqxx -lpq

user_phone_receiver_SOURCES=user_phone_receiver.cpp user_map.cpp
user_phone_receiver_LDADD= -lz -lpthread  -lrdkafka++ -lrdkafka -lrt  -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a -ljsoncpp -lpthread -lpqxx -lpq

#ap_signal_dock_SOURCES=ap_signal_dock.cpp 
#ap_signal_dock_LDADD=-lc++ -lzmq

#ap_signal_sender_SOURCES=ap_signal_sender.cpp 
#ap_signal_sender_LDADD=-lc++ -lzmq


configdir = $(prefix)/etc
config_DATA =  config.ini  
rootdir=$(prefix)
dist_root_SCRIPTS =  restart_fastcgi.sh restart_receiver.sh

SUBDIRS= test
