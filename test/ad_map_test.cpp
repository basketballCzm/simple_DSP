#include "ad_map.h"
#include "tair_common.h"
#include "gtest/gtest.h"
#include <data_entry.hpp>

TEST(TairCommon,GetDataEntry)
{
    tair::common::data_entry key;
    int mall_id=2;
    float space_id=4.5;
    get_data_entry( key,"ad.space:",mall_id,":",space_id,":ad.group.set");
    EXPECT_STREQ("ad.space:2:4.5:ad.group.set",key.get_data());
}
