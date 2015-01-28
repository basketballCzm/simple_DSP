
AUTOMAKE_OPTIONS = foreign
AM_CPPFLAGS = -std=c++11 -stdlib=libc++ -Weverything -Wno-c++98-compat \
	-I/data/caster_zeng/package/libcxx/build/include/c++/v1

bin_PROGRAMS=adstat ad_register  ad_request user_op
adstat_SOURCES=adstat.cpp 
adstat_LDADD=-lfcgi -luriparser -lc++ 

ad_register_SOURCES=ad_register.cpp 
ad_register_LDADD=-lfcgi -luriparser -lc++ 

ad_request_SOURCES=ad_request.cpp 
ad_request_LDADD=-lfcgi -luriparser -lc++

user_op_SOURCES=user_op.cpp  user_map.cpp
user_op_LDADD=-lfcgi -luriparser -lc++ -ljsoncpp
