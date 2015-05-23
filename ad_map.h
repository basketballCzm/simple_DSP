#include "ad_model.h"  
#include <json/json.h>
#include <limits.h>
#include <limits>

//user_id is mac address
namespace ad_map
{
    inline void ad_map_init(int ns=0);
    //kafka_offset default -1 meanes none
    int ad_add();
    int ad_remove();
    int ad_update();
    int ad_query();
    void ad_list_all();
	int ad_request();
	int ad_click();
}

