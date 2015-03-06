#include <iostream>
#include <zmq.hpp>

//  Convert string to 0MQ string and send to socket
static bool
s_send (zmq::socket_t & socket, const std::string & string) {

    zmq::message_t message(string.size());
    memcpy (message.data(), string.data(), string.size());

    bool rc = socket.send (message);
    return (rc);
}
//  Receive 0MQ string from socket and convert into string
static std::string
s_recv (zmq::socket_t & socket) {

    zmq::message_t message;
    socket.recv(&message);

    return std::string(static_cast<char*>(message.data()), message.size());
}

int main ()
{
    zmq::context_t context(1);
    zmq::socket_t server(context, ZMQ_REP);
    server.bind("tcp://*:5555");

    while (1) {
        std::string request = s_recv (server);

        std::cout << "I: normal request (" << request << ")" << std::endl;
        sleep (1); // Do some heavy work
        s_send (server, request);
    }   
    return 0;
}
