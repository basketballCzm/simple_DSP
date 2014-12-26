#include <stdio.h>
#include <syslog.h>
#include "init_data.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "config.h"
#include "ad_model.h"

extern AdData* g_p_ad_data;

int init_ad_data()
{
    int fd;
    fd=open(AD_DATA_FILE,O_CREAT|O_RDWR,00777); 
    if(fd<0)
    {
        syslog(LOG_ERR, "fail to open ad_data_file : %s",AD_DATA_FILE);
        return -1;     
    }

    AdData ad_data_head;
    ssize_t read_size = read(fd,(void *)&ad_data_head,sizeof(AdData));
    if(read_size<sizeof(AdData))
    {
        syslog(LOG_ERR, "invalid ad_data_file:%s ,read size (%d) < sizeof(AdData)(%d) " ,AD_DATA_FILE,read_size, sizeof(AdData));
        return -1;
    }

    ssize_t ad_buff_size =ad_data_head.size*2;
    syslog(LOG_INFO, "ad data size is %ld",ad_data_head.size);
    lseek(fd,ad_buff_size,SEEK_SET);
    write(fd,"",1);
    g_p_ad_data = (AdData *) mmap( NULL,ad_buff_size ,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0 ); 
    close(fd);
    return 0;
}

int init_data()
{
    init_ad_data();
}
