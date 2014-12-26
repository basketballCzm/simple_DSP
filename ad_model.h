struct GeoInfo
{
    float longitude;
    float latitude;
};


struct AdInfo
{
    int ad_owner;
    char ad_content[2048];
    int ad_type;
    GeoInfo ad_point;
    int ad_radius;
    int shoplist[10];
    int web_position;
    float price;
};

struct ShopInfo
{
    int shop_id;
    char shop_name[100];
    char shop_address[100];
    GeoInfo shop_sharp[10];
};

struct MallGeoInfo
{
    int shop_number;
    ShopInfo shop_list[1];    
};

struct AdData
{
    int mall_id;
    char mall_name [100];
    int size;
    unsigned int number;
    AdInfo ad_list[1];
};
