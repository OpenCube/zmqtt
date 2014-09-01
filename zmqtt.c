#include <stdio.h>
#include <czmq.h>
#include "mqtt.h"

int main (void)
{
    void *ctx = zmq_ctx_new ();
    assert (ctx);
    
    /* Create ZMQ_STREAM socket */
    void *socket = zmq_socket (ctx, ZMQ_STREAM);
    assert (socket);
    
    int rc = zmq_bind (socket, "tcp://*:1883");
    assert (rc == 0);
    
    /* Data structure to hold the ZMQ_STREAM ID */
    uint8_t id[256];
    size_t id_size = 256;
    
    /* Data structure to hold the ZMQ_STREAM received data */
    uint8_t in_buffer[65536];
    size_t in_size;
    
    uint8_t out_buffer[65536];
    
    struct mqtt_response response;
    response.buffer = out_buffer;
    
    while (1)
    {
        /*  Get HTTP request; ID frame and then request */
        id_size = zmq_recv(socket, id, 256, 0);
        assert (id_size > 0);

        in_size = zmq_recv(socket, in_buffer, 65536, 0);
        assert (in_size >= 0);

        mqtt_handle(in_buffer, &response);

        if (response.length > 0)
        {
            /* Sends the ID frame followed by the response */
            zmq_send(socket, id, id_size, ZMQ_SNDMORE);
            zmq_send(socket, response.buffer, response.length, ZMQ_SNDMORE);
        }
        
        if (!response.keep_alive)
        {
            /* Closes the connection by sending the ID frame followed by a zero response */
            zmq_send(socket, id, id_size, ZMQ_SNDMORE);
            zmq_send(socket, 0, 0, ZMQ_SNDMORE);
        }
    }
    zmq_close (socket);
    zmq_ctx_destroy (ctx);
}
