#include <iostream>
#include <sstream>
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

void parse_apmac_msg(const char* msg) {

    int offset = 30;
    int num_macs = (strlen(msg) - 29) / 19;
    double time = (double)std::time(0);

    for(int i = 0; i < num_macs; ++i, offset += 19) {

        unsigned long mac = str_to_uint64(msg + offset);
        save_mac(msg, mac, time);

    }

    unsigned long ap_mac = str_to_uint64(msg + 6);
    int shopId = apmac_get_shopid(ap_mac);

    if(shopId) {

        printf("shopId: %d\n", shopId);

        offset = 30;

        for(int i = 0; i < num_macs; ++i, offset += 19) {

            bool is_vip = mac_is_vip(msg + offset, shopId);

            if(is_vip) {

                printf("1\n");
                update_vip_arrive_time(shopId, user_get_id(str_to_uint64(msg + offset)));

            } else {

                printf("0\n");

            }

        }

    }

}

void msg_consume(RdKafka::Message* message, void* opaque) {

    union {
        unsigned long long mac_number;
        unsigned char mac_array[16];
    } mac;

    mac.mac_number = 0;

    int x, y;
    int width, height;
    int mall_id = 2;

    const char* msg;

    switch (message->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;

        case RdKafka::ERR_NO_ERROR:

            /* Real message */

            std::cout << "Read msg at offset " << message->offset() << std::endl;

            if (message->key()) {

                std::cout << "Key: " << *message->key() << std::endl;

            }
            
            msg = (const char*)message->payload();
            printf("%s\n", msg);

            if(msg[0] == 'A') {

                parse_apmac_msg(msg);

            } else {

                sscanf(msg, "Mac:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx Rect:%d,%d,%d,%d Group:%d",
                    mac.mac_array + 5,
                    mac.mac_array + 4,
                    mac.mac_array + 3,
                    mac.mac_array + 2,
                    mac.mac_array + 1,
                    mac.mac_array,
                    &x, &y, &width, &height, &mall_id);

            }

            if ( mac.mac_number > 0 ) {

                user_add(mac.mac_number,x+width/2.0,y+height/2.0,INT_MIN, static_cast<int>(message->len()),mall_id);

            }

            break;

        case RdKafka::ERR__PARTITION_EOF:
            /* Last message */
            if (exit_eof) {
                run = false;
            }
            break;

        default:
            /* Errors */
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            run = false;
    }
}

class TairConsumeCb : public RdKafka::ConsumeCb {
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
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }

    std::cout << "% Created consumer " << consumer->name() << std::endl;

    /*
     *      * Create topic handle.
     *           */
    RdKafka::Topic *topic = RdKafka::Topic::create(consumer, s_topic,
            tconf, errstr);
    if (!topic) {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        exit(1);
    }

    /*
     * Start consumer for topic+partition at start offset
     */
    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);
    if (resp != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Failed to start consumer: " <<
            RdKafka::err2str(resp) << std::endl;
        exit(1);
    }

    TairConsumeCb consume_cb;

    /*
     * Consume messages
     */
    run=true;
    while (run) {
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
