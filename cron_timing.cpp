#include "cron_timing.h"
bool check_cron_timing(time_t time,cron_timing *time_range)
{
    struct tm *timeinfo =localtime (& time);
    if (bit_test(time_range->minute, timeinfo->tm_min)
            && bit_test(time_range->hour, timeinfo->tm_hour)
            && bit_test(time_range->month, timeinfo->tm_mon)
            && ((time_range->flags & YEAR_STAR)?true:(time_range->year==timeinfo->tm_year) )
            && ( ((time_range->flags & DOM_STAR) || (time_range->flags & DOW_STAR))
                ? (bit_test(time_range->dow,timeinfo->tm_wday) && bit_test(time_range->dom,timeinfo->tm_mday))
                : (bit_test(time_range->dow,timeinfo->tm_wday) || bit_test(time_range->dom,timeinfo->tm_mday))
               )   
       ) { 
        return true;
    }
    else
    {
        return false;
    }
}
