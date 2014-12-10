#include <iostream>
#include <fcgi_stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>

#include <sys/mman.h>
#include <fcntl.h>

#include "init_data.h"
#include "ad_model.h"

using namespace std;

AdData* g_p_ad_data=NULL;

int main()
{
    
    openlog("adstat", LOG_PID|LOG_CONS, LOG_LOCAL0 );
    
    init_data();
    
    while(FCGI_Accept() >= 0)
    {

        printf( "Content-type:text/html\r\n\r\n" );
        printf( "<p> Hello FastCGI !  </ p>" );
        printf( "<br /> Request number = [%d]", ++g_p_ad_data->number );
        printf( "<br /> Process ID: %d ", getpid() );
        syslog(LOG_INFO, "adstat load success ..."); 
    }   

    return 0;
}
