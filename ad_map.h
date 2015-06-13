#include "ad_model.h"  
#include <json/json.h>
#include <limits.h>
#include <limits>

#define HIGHEST_N_ADS 1
#define NONE_INT_DATA INT_MIN
#define NEW_AD_MIN_SHOW_COUNT 100
#define NEW_AD_CTR 0.05



//user_id is mac address
namespace ad_map
{
    //kafka_offset default -1 meanes none
    void ad_map_init();
    int ad_add();
    int ad_remove();
    int ad_update();
    int ad_query();
    void ad_list_all();
	int ad_request(Json::Value &ret, const unsigned long long user_id, const int space_id, const int mall_id=0);
	int ad_click();
}

