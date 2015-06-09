GTEST_ROOT= /opt/gtest-1.7.0

AUTOMAKE_OPTIONS = foreign
AM_CPPFLAGS = -std=c++11  -I/opt/tair_rdb_bin/include \
	-I${TBLIB_ROOT}/include/tbnet -I${TBLIB_ROOT}/include/tbsys \
	-I$(GTEST_ROOT) -I$(GTEST_ROOT)/include



noinst_PROGRAMS=ad_map_test user_map_test
ad_map_test_SOURCES=ad_map_test.cpp $(GTEST_ROOT)/src/gtest-all.cc  ../ad_map.cpp ../user_map.cpp ../cron_timing.cpp
ad_map_test_LDADD=-lfcgi -luriparser -lc++ -ljsoncpp -ljsoncpp -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a\
	$(GTEST_ROOT)/lib/.libs/libgtest_main.a

user_map_test_SOURCES=user_map_test.cpp $(GTEST_ROOT)/src/gtest-all.cc  ../user_map.cpp 
user_map_test_LDADD=-lfcgi -luriparser -lc++ -ljsoncpp -ljsoncpp -ltairclientapi $(TBLIB_ROOT)/lib/libtbnet.a $(TBLIB_ROOT)/lib/libtbsys.a\
	$(GTEST_ROOT)/lib/.libs/libgtest_main.a
