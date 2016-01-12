GTEST_ROOT= /opt/gtest-1.7.0

AUTOMAKE_OPTIONS = foreign subdir-objects
TBLIB_ROOT = /opt/tbl
AM_CPPFLAGS = -std=c++11  -I/opt/tair_rdb_bin/include \
	-I${TBLIB_ROOT}/include/tbnet -I${TBLIB_ROOT}/include/tbsys \
	-I$(GTEST_ROOT) -I$(GTEST_ROOT)/include \
	-I/usr/include/jsoncpp




noinst_PROGRAMS=ad_map_test user_map_test user_tag_update get_user_id
ad_map_test_SOURCES=ad_map_test.cpp $(GTEST_ROOT)/src/gtest-all.cc  ../ad_map.cpp ../user_map.cpp ../cron_timing.cpp
ad_map_test_LDADD=-lfcgi -luriparser -ljsoncpp -lpthread -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a\
	$(GTEST_ROOT)/lib/.libs/libgtest_main.a -lpqxx -lpq

user_map_test_SOURCES=user_map_test.cpp $(GTEST_ROOT)/src/gtest-all.cc  ../user_map.cpp 
user_map_test_LDADD=-lfcgi -luriparser -ljsoncpp -lpthread -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a\
	$(GTEST_ROOT)/lib/.libs/libgtest_main.a -lpqxx -lpq

user_tag_update_SOURCES=user_tag_update.cpp ../user_map.cpp
user_tag_update_LDADD=-lz -lpthread -lrt -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a -lboost_program_options -ljsoncpp -lpthread -lpqxx -lpq

get_user_id_SOURCES=get_user_id.cpp ../user_map.cpp
get_user_id_LDADD=-lz -lpthread -lrt -L/opt/tair_rdb_bin/lib -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a -lboost_program_options -ljsoncpp -lpthread -lpqxx -lpq
