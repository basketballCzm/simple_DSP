
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS=adstat ad_register ad_request
adstat_SOURCES=adstat.cpp init_data.cpp 
adstat_LDADD=-lfcgi -luriparser

ad_register_SOURCES=ad_register.cpp init_data.cpp
ad_register_LDADD=-lfcgi -luriparser

ad_request_SOURCES=ad_request.cpp init_data.cpp
ad_request_LDADD=-lfcgi -luriparser
