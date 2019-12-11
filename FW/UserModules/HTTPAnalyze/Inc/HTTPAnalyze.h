#ifndef _HTTPANALYZE_H_
#define _HTTPANALYZE_H_

#include <stdint.h>

typedef enum{
    GET,
    POST,
    NOT_DEFINED
}HTTPMethod_t;

void GetRequestLine(char* source, char* buffer, uint16_t bufferSize);
HTTPMethod_t GetMethod(char* source);
void GetURI(char* source, char* buffer, uint16_t bufferSize);
void GetProtocolVersion(char* source, char* buffer, uint16_t bufferSize);
void GetHeader(char* source, char* buffer, uint16_t bufferSize);

#endif
