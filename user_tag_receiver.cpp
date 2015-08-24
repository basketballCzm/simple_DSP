#include <iostream>
#include <sstream>
#include "librdkafka/rdkafkacpp.h"
#include "user_map.h"
#include "limits.h"

using namespace std;
using namespace user_map;

static const string s_topic="user-tag";
static const string brokers = "WUSHUU-KAFKA";
static bool exit_eof = false;
//static int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING; 
static int64_t start_offset = RdKafka::Topic::OFFSET_STORED; 
static int32_t partition = 0; 

static bool run = true;

void msg_consume(RdKafka::Message* message, void* opaque) {
    switch (message->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;

        case RdKafka::ERR_NO_ERROR:
        {
            /* Real message */
            std::cout << "Read msg at offset " << message->offset() << std::endl;
            if (message->key()) {
                std::cout << "Key: " << *message->key() << std::endl;
            }
            
            int len = static_cast<int>(message->len());
            char* msg = static_cast<char *>(message->payload());
            msg[len] = '\0';
            
            union {
                unsigned long long mac_number;
                unsigned char mac_array[16];//LBF
            }mac;
            mac.mac_number=0;
            char usertag[40];
            sscanf(static_cast<const char *>(message->payload()),"Mac %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %s",
                    mac.mac_array+5,mac.mac_array+4,mac.mac_array+3,mac.mac_array+2,mac.mac_array+1,mac.mac_array,usertag);
            
            cout<<"user tag is "<<usertag<<endl;
            cout<<"mac number is "<<mac.mac_number<<endl;
            user_tag_update(mac.mac_number, usertag, 1.0);
            break;
        }
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
    conf->set("client.id","tair-rdb-client1", errstr);
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
