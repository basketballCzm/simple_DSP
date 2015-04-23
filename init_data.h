#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "config.h"
#include <atomic>
#include <syslog.h>
#include <new>
#include <sys/stat.h>
#include <errno.h>

template<typename T,typename T_item>
void init_shared_data(const char * filename)
{
    struct stat buffer;   
    if(stat (filename, &buffer) == 0)
    {
        //file exist
        return;
    }
    else 
    {
        int fd=open(filename,O_CREAT|O_EXCL|O_WRONLY,00777);
        if(fd==-1)
        {
            syslog(LOG_INFO, "init_shared_data create file  errno=%d",errno);
            return;
        }
        else
        {
            lseek(fd,sizeof(T)-sizeof(T_item),SEEK_SET);
            write(fd,"",1);
            syslog(LOG_INFO, "init_shared_data create file %s",filename);
            close(fd);
        }
    }
}

template<typename T,typename T_item>
T *load_shared_data(const char * filename)
{
    syslog(LOG_INFO, "ad_stat enter init_shared_data");
    init_shared_data<T,T_item>(filename);
    T* g_data=NULL;
    int fd;
    fd=open(filename,O_RDWR,00777); 
    
    if(fd<0)
    {
        syslog(LOG_ERR, "fail to open ad_data_file : %s ,errno=%d",filename,errno);
        return NULL;     
    }

    T data_head;
    ssize_t read_size = read(fd,(void *)&data_head,sizeof(T));
    if(read_size<sizeof(T)-sizeof(T_item))
    {
        syslog(LOG_ERR, "invalid ad_data_file:%s ,read size (%d) < sizeof(T)(%d) - sizeof(T_item)(%d) " 
                ,filename,read_size, sizeof(T), sizeof(T_item));
        return NULL;
    }

    ssize_t buffer_size; 
    int list_size;
    //TODO need processes mutex to update the size of item list
    syslog(LOG_INFO, "ad_stat data_head.size = %d",data_head.size);
    syslog(LOG_INFO, "ad_stat data_head.number= %d", *( (int *)&data_head.number) );
    
    if( data_head.size <= 0 )
    {    
        int number=data_head.number;
        if(number<1000)
            number=1000;

        list_size=number*2;
        buffer_size=sizeof(T)+(list_size-1)*sizeof(T_item);
        syslog(LOG_INFO, "ad data size is %ld",buffer_size);
        lseek(fd,buffer_size,SEEK_SET);
        write(fd,"",1);

        g_data = (T *) mmap( NULL,buffer_size ,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0 ); 
        g_data->size=list_size;
        syslog(LOG_INFO, "ad_stat call msync");
        msync(g_data,sizeof(T),MS_ASYNC);
    }
    else
    {
        g_data = (T *) mmap( NULL,buffer_size ,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0 ); 
    }

    atomic_init (&g_data->number,*( (int *)&data_head.number));
    //void *p_number = (void *)&data_head.number;
    //new (p_number) std::atomic<int>;
    close(fd);
    return g_data;
}


