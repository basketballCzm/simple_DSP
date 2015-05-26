#include <ctime>
#include "bitstring.h"

#define FIRST_MINUTE    0
#define LAST_MINUTE 59
#define MINUTE_COUNT    (LAST_MINUTE - FIRST_MINUTE + 1)

#define FIRST_HOUR  0
#define LAST_HOUR   23
#define HOUR_COUNT  (LAST_HOUR - FIRST_HOUR + 1)

#define FIRST_DOM   1
#define LAST_DOM    31
#define DOM_COUNT   (LAST_DOM - FIRST_DOM + 1)

#define FIRST_MONTH 1
#define LAST_MONTH  12
#define MONTH_COUNT (LAST_MONTH - FIRST_MONTH + 1)

/* note on DOW: 0 and 7 are both Sunday, for compatibility reasons. */
#define FIRST_DOW   0
#define LAST_DOW    7
#define DOW_COUNT   (LAST_DOW - FIRST_DOW + 1)

typedef struct _cron_timing {
    bitstr_t        bit_decl(minute, MINUTE_COUNT);
    bitstr_t        bit_decl(hour,   HOUR_COUNT);
    bitstr_t        bit_decl(dom,    DOM_COUNT);
    bitstr_t        bit_decl(month,  MONTH_COUNT);
    bitstr_t        bit_decl(dow,    DOW_COUNT);
    int             year;
    int             flags;
#define DOM_STAR        0x01
#define DOW_STAR        0x02
#define WHEN_REBOOT     0x04
#define MIN_STAR        0x08
#define HR_STAR         0x10
#define YEAR_STAR       0x20
} cron_timing;


bool check_cron_timing(time_t time,cron_timing *time_range);
