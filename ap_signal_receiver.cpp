#include <iostream>
#include <sstream>
#include <cstring>
#include <ctime>
#include "librdkafka/rdkafkacpp.h"
#include "user_map.h"
#include "limits.h"

using namespace std;
using namespace user_map;

static const string s_topic="lbs-point";
static const string brokers = "WUSHUU-KAFKA";
static bool exit_eof = false;
//static int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING; 
//static int64_t start_offset = RdKafka::Topic::OFFSET_STORED; 
static int64_t start_offset = RdKafka::Topic::OFFSET_END; 
static int32_t partition = 0; 

static bool run = true;

// parse a ap mac message
// store mac list into tair database

bool msg_start_with(const char* msg, const char* prefix)
{
    int len = (int)strlen(prefix);

    for(int i = 0; i < len; ++i)
    {
        if(msg[i] != prefix[i]) return false;
    }

    return true;
}

void parse_apmac_msg(const char* msg)
{
    int offset = 30;
    int num_macs = (strlen(msg) - 29) / 19;

    unsigned long ap_mac = str_to_uint64(msg + 6);
    int shop_id = apmac_get_shopid(ap_mac);

    for(int i = 0; i < num_macs; ++i, offset += 19)
    {
        unsigned long mac = str_to_uint64(msg + offset);
        std::time_t now = std::time(0);

        update_mac_location_time(2, mac, now);
        update_location_update_time(2, mac, now);
    }
}

void parse_apmac_closer_msg(const char* msg)
{
    int offset = strlen("ApMac-Closer:");
    unsigned long ap_mac = str_to_uint64(msg + offset);
    int shop_id = apmac_get_shopid(ap_mac);

    offset = strlen("ApMac-Closer:60:cd:a9:00:9a:3a Macs:[");
    int num_macs = (strlen(msg) - offset) / 19;

    std::time_t now = std::time(0);
    std::string datetime = datetime_str(now);

    for(int i = 0; i < num_macs; ++i, offset += 19)
    {
        unsigned long mac = str_to_uint64(msg + offset);
        bool is_vip = mac_is_vip(msg + offset, shop_id);
        int user_id = user_get_id(mac);

        if(shop_id)
        {
            if(is_vip)
            {
                update_vip_arrive_time(2, shop_id, user_id, mac, now);
            }
            
            update_user_arrive_time(2, shop_id, user_id, now);
            update_user_location_time(2, shop_id, user_id, mac, now);
        }

        std::time_t last_time = get_user_location_time(2, shop_id, mac);

        // if interval is less than 10 minutes
        // we think the customer still in the shop
        // so add the time gap into duration

        if(now - last_time < 10 * 60)
        {
            update_user_duration(2, shop_id, user_id, datetime, now - last_time);
        }
    }
}

void msg_consume(RdKafka::Message* message, void* opaque)
{
    Mac mac;
    mac.number = 0;

    int x, y;
    int width, height;
    int mall_id = 2;

    const char* msg;

    switch (message->err())
    {
        case RdKafka::ERR__TIMED_OUT:  break;

        case RdKafka::ERR_NO_ERROR:

            /* Real message */

            std::cout << "Read msg at offset " << message->offset() << std::endl;

            if (message->key())
            {
                std::cout << "Key: " << *message->key() << std::endl;
            }
            
            msg = (const char*)message->payload();
            printf("%s\n", msg);

            if(msg_start_with(msg, "Mac:"))
            {
                sscanf(msg, "Mac:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx Rect:%d,%d,%d,%d Group:%d",
                    mac.bytes + 5,
                    mac.bytes + 4,
                    mac.bytes + 3,
                    mac.bytes + 2,
                    mac.bytes + 1,
                    mac.bytes,
                    &x, &y, &width, &height, &mall_id);

                if (mac.number > 0)
                {
                    user_add(mac.number, x+width/2.0, y+height/2.0, INT_MIN, static_cast<int>(message->len()), mall_id);
                }
            }
            else if(msg_start_with(msg, "ApMac:"))
            {
                parse_apmac_msg(msg);
            }
            else if(msg_start_with(msg, "ApMac-Closer:"))
            {
                parse_apmac_closer_msg(msg);
            }

            break;

        case RdKafka::ERR__PARTITION_EOF:

            if (exit_eof)
            {
                run = false;
            }

            break;

        default:
            /* Errors */
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            run = false;
    }
}

class TairConsumeCb : public RdKafka::ConsumeCb
{
public:
    void consume_cb (RdKafka::Message &msg, void *opaque) {
        msg_consume(&msg, opaque);
    }
};

int main ()
{
    string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    conf->set("client.id","tair-rdb-client-cq", errstr);
    tconf->set("auto.commit.enable","true", errstr);
    tconf->set("auto.commit.interval.ms","200", errstr);

    conf->set("metadata.broker.list", brokers, errstr);
    conf->set("group.id","tair-rdb-group1", errstr);
    RdKafka::Consumer *consumer = RdKafka::Consumer::create(conf, errstr);

    if (!consumer)
    {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }

    std::cout << "% Created consumer " << consumer->name() << std::endl;

    /*
     *      * Create topic handle.
     *           */
    RdKafka::Topic *topic = RdKafka::Topic::create(consumer, s_topic,
            tconf, errstr);

    if (!topic)
    {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        exit(1);
    }

    /*
     * Start consumer for topic+partition at start offset
     */
    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);

    if (resp != RdKafka::ERR_NO_ERROR)
    {
        std::cerr << "Failed to start consumer: " <<
            RdKafka::err2str(resp) << std::endl;
        exit(1);
    }

    TairConsumeCb consume_cb;

    /*
     * Consume messages
     */
    run=true;
    while (run)
    {
        RdKafka::Message *msg = consumer->consume(topic, partition, 1000);
        msg_consume(msg, NULL);
        delete msg;
        consumer->poll(0);
    }

    /*
     * Stop consumer
     */
    consumer->stop(topic, partition);

    consumer->poll(1000);

    delete topic;
    delete consumer;

    return 0;
}
