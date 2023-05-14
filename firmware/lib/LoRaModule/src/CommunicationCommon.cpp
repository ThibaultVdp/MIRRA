#include "CommunicationCommon.h"
#include <iostream>

uint8_t *Message::to_data(uint8_t *data)
{
    memcpy(data, &this->header, header_length);
    return data;
}
Message Message::from_data(uint8_t *data)
{
    switch (data[0])
    {
    case Message::TIME_CONFIG:
        return TimeConfigMessage(data);
    case Message::SENSOR_DATA:
    case Message::SENSOR_DATA_LAST:
        return SensorDataMessage(data);
    default:
        return Message(data);
    }
}

const Message Message::error = Message();

uint8_t *TimeConfigMessage::to_data(uint8_t *data)
{
    data = Message::to_data(data);
    memcpy(&data[Message::getLength()], &this->body, sizeof(this->body));
    return data;
}

SensorDataMessage::SensorDataMessage(MACAddress src, MACAddress dest, uint32_t time, uint8_t n_values, SensorValue::SensorValueStruct *sensor_values) : Message(Message::SENSOR_DATA, src, dest), body{time, n_values}
{
    memcpy(this->values, sensor_values, this->body.n_values * sizeof(SensorValue::SensorValueStruct));
}

SensorDataMessage::SensorDataMessage(uint8_t *data) : Message(data), body{*reinterpret_cast<SensorDataStruct *>(&data[header_length])}
{
    memcpy(this->values, &data[Message::getLength() + sizeof(SensorDataStruct)], this->body.n_values * sizeof(SensorValue::SensorValueStruct));
};

uint8_t *SensorDataMessage::to_data(uint8_t *data)
{
    data = Message::to_data(data);
    memcpy(&data[Message::getLength()], &this->body, sizeof(this->body));
    memcpy(&data[Message::getLength() + sizeof(SensorDataStruct)], this->values, this->body.n_values * sizeof(SensorValue::SensorValueStruct));
    return data;
}

MACAddress::MACAddress(const uint8_t *address)
{
    memcpy(this->address, address, MACAddress::length);
}

char *MACAddress::toString(char *string) const
{
    snprintf(string, MACAddress::string_length, "%02X:%02X:%02X:%02X:%02X:%02X", this->address[0], this->address[1], this->address[2], this->address[3], this->address[4], this->address[5]);
    return string;
}

bool operator==(const MACAddress &mac1, const MACAddress &mac2)
{
    size_t i = 0;
    for (size_t i = 0; i < MACAddress::length; i++)
    {
        if (mac1.address[i] != mac2.address[i])
            return false;
    }
    return true;
}

const MACAddress MACAddress::broadcast = MACAddress();
char MACAddress::str_buffer[MACAddress::string_length];
