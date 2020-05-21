//build with g++ ./src/mosq.c -lmosquitto
//test with mosquitto_pub -h localhost -p 1883 -t 'test' -m "mytest"
#include <mosquitto.h>
#include <mosquittopp.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

#define mqtt_host "localhost"
#define mqtt_port 1883

void connect_callback( struct mosquitto *mosq, void *obj, int result )
{
    printf( "connect callback, rc=%d\n", result );
}

void message_callback( struct mosquitto *mosq, void *obj, const struct mosquitto_message *message )
{
    bool match = 0;
    printf( "got message '%.*s' for topic '%s'\n", message->payloadlen, (char*)message->payload, message->topic );

    mosquitto_topic_matches_sub( "test", message->topic, &match );
    if( match )
    {
        printf( "got message for test topic\n" );
    }

}

int main( int argc, char *argv[] )
{
    uint8_t reconnect = true;

    struct mosquitto *mosq;
    int rc = 0;

    mosquitto_lib_init();

    mosq = mosquitto_new( NULL, true, 0 );

    if( mosq )
    {
        mosquitto_connect_callback_set( mosq, connect_callback );
        mosquitto_message_callback_set( mosq, message_callback );

        rc = mosquitto_connect( mosq, mqtt_host, mqtt_port, 60 );

        mosquitto_subscribe( mosq, NULL, "test", 0 );

        while( true )
        {
            rc = mosquitto_loop( mosq, -1, 1 );
            if( rc )
            {
                printf( "connection error!\n" );
                sleep( 10 );
                mosquitto_reconnect( mosq );
            }
        }
        mosquitto_destroy( mosq );
    }

    mosquitto_lib_cleanup();

    return rc;
}

