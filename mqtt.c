#include <stdio.h>
#include <string.h>
#include "mqtt.h"

void mqtt_parse_fixed_header(uint8_t *message, struct mqtt_fixed_header *header)
{
    uint8_t *buffer = message + 1; // Skip the beginning of the fixed header
    unsigned char bytes_read = 0;
    unsigned char digit;
    unsigned int multiplier = 1;
    unsigned int value = 0;

    // Find the start of the variable header
    do
    {
        digit = buffer[bytes_read];
        bytes_read++;
        value += (digit & 127) * multiplier;
        multiplier *= 128;
    } while ((digit & 128) != 0); // && bytes_read < 4

    header->control_packet_type = message[0] >> 4;
    header->flags = message[0] & 0xF;
    header->remaining_length = value;
    header->variable_header = buffer + bytes_read;
}

void mqtt_parse_connect_header(struct mqtt_fixed_header *fixed_header, struct mqtt_connect_header *header)
{
    uint8_t *buffer = fixed_header->variable_header;

    // Protocol name
    header->protocol_name_length = 256 * buffer[0] + buffer[1];
    header->protocol_name = buffer + 2;
    buffer = header->protocol_name + header->protocol_name_length;

    // Protocol level
    header->protocol_level = buffer[0];
    buffer++;
    
    // Connect flags
    header->reserved = buffer[0] & 0x1;
    header->clean_session = (buffer[0] >> 1) & 0x1;
    header->will_flag = (buffer[0] >> 2) & 0x1;
    header->will_qos = (buffer[0] >> 3) & 0x3;
    header->will_retain = (buffer[0] >> 5) & 0x1;
    header->user_name_flag = (buffer[0] >> 6) & 0x1;
    header->password_flag = (buffer[0] >> 7) & 0x1;
    buffer++;
    
    // Keep alive
    header->keep_alive = 256 * buffer[0] + buffer[1];
    
    // Payload
    header->payload = buffer + 2;
}

void mqtt_parse_connect_payload(struct mqtt_connect_header *header, struct mqtt_connect_payload *payload)
{
    uint8_t* buffer = header->payload;
    payload->client_identifier_length = 256 * buffer[0] + buffer[1];
    buffer += 2;

    payload->client_identifier = buffer;
    buffer += payload->client_identifier_length;

    if (header->will_flag)
    {
        payload->will_topic_length = 256 * buffer[0] + buffer[1];
        buffer += 2;
        
        payload->will_topic = buffer;
        buffer += payload->will_topic_length;
        
        payload->will_message_length = 256 * buffer[0] + buffer[1];
        buffer += 2;
        
        payload->will_message = buffer;
        buffer += payload->will_message_length;
    }
    
    if (header->user_name_flag)
    {
        payload->user_name_length = 256 * buffer[0] + buffer[1];
        buffer += 2;
        
        payload->user_name = buffer;
        buffer += payload->user_name_length;
    }
    
    if (header->password_flag)
    {
        payload->password_length = 256 * buffer[0] + buffer[1];
        buffer += 2;
        
        payload->password = buffer;
        buffer += payload->password_length;
    }
}

uint32_t mqtt_compose_connack(uint8_t *message, uint8_t return_code)
{
    // Fixed header
    message[0] = MQTT_CONNACK << 4;
    message[1] = 2;
    
    // Variable header
    message[2] = 0;
    message[3] = return_code;
    
    return 4;
}

uint8_t mqtt_handle(uint8_t* message, struct mqtt_response *response)
{
    struct mqtt_fixed_header fixed_header;
    struct mqtt_connect_header connect_header;
    struct mqtt_connect_payload connect_payload;
    
    response->length = 0;
    
    int i;
    
    mqtt_parse_fixed_header(message, &fixed_header);
    
    printf("Message received (type %d)\n", fixed_header.control_packet_type);
    
    if (fixed_header.control_packet_type == MQTT_CONNECT)
    {
        mqtt_parse_connect_header(&fixed_header, &connect_header);
        mqtt_parse_connect_payload(&connect_header, &connect_payload);
        
        printf("Connected: ");
        for (i = 0; i < connect_payload.client_identifier_length; i++)
        {
            printf("%c", connect_payload.client_identifier[i]);
        }
        printf("\n");
        
        // Send a CONNACK
        response->length = mqtt_compose_connack(response->buffer, 0);
        response->keep_alive = true;
    }
}

//unsigned char encode_remaining_length(char *buffer, unsigned int length)
//{
//    unsigned char bytes_written = 0;
//    unsigned char digit;
//    unsigned int value = length;
//    
//    do
//    {
//        digit = value % 128;
//        if (value > 0)
//        {
//            digit |= 0x80; // set the top bit of this digit
//        }
//        buffer[bytes_written] = digit;
//        bytes_written++;
//    } while (value > 0);
//    
//    return bytes_written;
//}

