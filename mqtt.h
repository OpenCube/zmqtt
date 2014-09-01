#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>
#include <stdbool.h>

#define MQTT_CONNECT      1
#define MQTT_CONNACK      2
#define MQTT_PUBLISH      3
#define MQTT_PUBACK       4
#define MQTT_PUBREC       5
#define MQTT_PUBREL       6
#define MQTT_PUBCOMP      7
#define MQTT_SUBSCRIBE    8
#define MQTT_SUBACK       9
#define MQTT_UNSUBSCRIBE  10
#define MQTT_UNSUBACK     11
#define MQTT_PINGREQ      12
#define MQTT_PINGRESP     13
#define MQTT_DISCONNECT   14
#define MQTT_QOS_0         0
#define MQTT_QOS_1         1
#define MQTT_QOS_2         2

#define MQTT_CONNACK_ACCEPTED           0
#define MQTT_CONNACK_WRONG_PROTO        1
#define MQTT_CONNACK_ID_REJECTED        2
#define MQTT_CONNACK_UNAVAILABLE        3
#define MQTT_CONNACK_WRONG_CREDENTIALS  4
#define MQTT_CONNACK_NOT_AUTHORIZED     5

struct mqtt_fixed_header {
    uint8_t control_packet_type;
    uint8_t flags;
    uint32_t remaining_length;
    uint8_t *variable_header;
};

struct mqtt_connect_header {
    uint16_t protocol_name_length;
    uint8_t *protocol_name;
    uint8_t protocol_level;
    bool reserved;
    bool clean_session;
    bool will_flag;
    uint8_t will_qos;
    bool will_retain;
    bool user_name_flag;
    bool password_flag;
    uint16_t keep_alive;
    uint8_t *payload;
};

struct mqtt_connect_payload {
    uint16_t client_identifier_length;
    uint8_t *client_identifier;
    uint16_t will_topic_length;
    uint8_t *will_topic;
    uint16_t will_message_length;
    uint8_t *will_message;
    uint16_t user_name_length;
    uint8_t *user_name;
    uint16_t password_length;
    uint8_t *password;
};

struct mqtt_response {
    uint8_t *buffer;
    uint32_t length;
    bool keep_alive;
};

void mqtt_parse_fixed_header(uint8_t *message, struct mqtt_fixed_header *header);
void mqtt_parse_connect_header(struct mqtt_fixed_header *fixed_header, struct mqtt_connect_header *header);

uint32_t mqtt_compose_connack(uint8_t *message, uint8_t return_code);

uint8_t mqtt_handle(uint8_t* message, struct mqtt_response *response);

#endif