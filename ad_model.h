#ifndef __AD_MODEL_H_2015_1_15__
#define __AD_MODEL_H_2015_1_15__
#include <atomic>

union GeoInfo
{
    struct 
    {
        float longitude; //x-axis
        float latitude;  //y-axis
        int level;  //mall level
    };
    struct 
    {
        float x; //x-axis
        float y;  //y-axis
        int z;  //mall level
    };
};

enum AdType
{
    NONE = -1,
    IMAGE,
    TEXT,
    ANIMATION
};

struct AdInfo
{
    int ad_id=-1;
    int ad_owner=-1;
    char ad_content[2048];
    char jump_url[2048];
    AdType ad_type=NONE;
    GeoInfo ad_point;
    float ad_radius=0;
    int shoplist[10];
    int web_position=-1;
    float show_price = 0.001f;
    float click_price = 1;
    std::atomic<int> show_counter{0};
    std::atomic<int> click_counter{0};
    int valid;
};

struct ShopInfo
{
    int shop_id = -1;
    char shop_name[100];
    char shop_address[100];
    GeoInfo shop_sharp[10];
    int level;
};

struct ShopData
{
    int shop_number;
    ShopInfo shop_list[1];    
};

struct AdData
{
    int mall_id;
    char mall_name [100];
    int size;
    std::atomic<int> number;
    AdInfo ad_list[1];
};

struct UserPosition
{
    unsigned long long mac;
    GeoInfo position;
    int valid;
};

struct UserPositionMap
{
    int size;
    std::atomic<int> number;
    UserPosition list[1];
};

#endif
