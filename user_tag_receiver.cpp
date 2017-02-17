#include <iostream>
#include <sstream>
#include "librdkafka/rdkafkacpp.h"
#include "user_map.h"
#include "limits.h"
#include "string.h"

using namespace std;
using namespace user_map;

static const string s_topic = "user-tag";
static const string brokers = "kafka";
static bool exit_eof = false;
static int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING; 
//static int64_t start_offset = RdKafka::Topic::OFFSET_STORED; 
static int32_t partition = 0; 

static bool run = true;

void msg_consume(RdKafka::Message* message, void* opaque) {

    const char * ori_msg=NULL;
    char * p_msg=NULL;

    switch (message->err()) {

        case RdKafka::ERR__TIMED_OUT: break;

        case RdKafka::ERR_NO_ERROR:  {

            std::cout << "Read msg at offset " << message->offset() << std::endl;

            if (message->key()) {

                std::cout << "Key: " << *message->key() << std::endl;

            }
            
            ori_msg = static_cast<char *>(message->payload());
            p_msg=new char[message->len()+1];
            strncpy(p_msg,ori_msg,message->len());
            p_msg[message->len()]=0;

            std::cout << "Read msg: " << p_msg << std::endl;
            Mac mac;
            char usertag[40];
            memset(usertag,0,40);

            sscanf(p_msg,"Mac %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %s",
                mac.bytes + 5,
                mac.bytes + 4,
                mac.bytes + 3,
                mac.bytes + 2,
                mac.bytes + 1,
                mac.bytes,
                usertag);
            delete p_msg;            
            cout << "user tag is " << usertag << endl;
            cout << "mac number is " << mac.number << endl;

            user_tag_update(mac.number, usertag, 1.0);

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
        if(msg)
        {
            delete msg;
            msg=NULL;
        }
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
