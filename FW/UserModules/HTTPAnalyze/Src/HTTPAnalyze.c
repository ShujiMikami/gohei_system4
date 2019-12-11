#include "HTTPAnalyze.h"
#include <string.h>

void GetRequestLine(char* source, char* buffer, uint16_t bufferSize)
{
    char strBuf[1024];
    strcpy(strBuf, source);

    char* requestLine = strtok(strBuf, "\r\n");
    if(strlen(requestLine) < bufferSize){
        strncpy(buffer, requestLine, strlen(requestLine));
    }
}
HTTPMethod_t GetMethod(char* source)
{
    HTTPMethod_t result = NOT_DEFINED;

    char requestLine[1024];

    GetRequestLine(source, requestLine, sizeof(requestLine));

    if(strncmp(requestLine, "GET", 3) == 0){
        result = GET;
    } else if(strncmp(requestLine, "POST", 4) == 0){
        result = POST;
    } else{
        result = NOT_DEFINED;
    }

    return result;
}
void GetURI(char* source, char* buffer, unsigned short bufferSize)
{
    char strToRead[1024];

    GetRequestLine(source, strToRead, sizeof(strToRead));

    strtok(strToRead, " ");

    char* uri = strtok(NULL, " ");

    if(strlen(uri) < bufferSize){
        strcpy(buffer, uri);
    }
}
void GetProtocolVersion(char* source, char* buffer, unsigned short bufferSize)
{
    char strToRead[1024];

    GetRequestLine(source, strToRead, sizeof(strToRead));

    strtok(strToRead, " ");
    strtok(NULL, " ");

    char* protocol = strtok(NULL, " ");

    if(strlen(buffer) < bufferSize){
        strcpy(buffer, protocol);
    }

}
void GetHeader(char* source, char* buffer, uint16_t bufferSize)
{
    char strBuf[1024];

    strcpy(strBuf, source);

    char* requestLine = strtok(strBuf, "\r\n");

    char* header = strtok(NULL, "\r\n");

    if(strlen(header) + 1 <= bufferSize){
        strcpy(buffer, header);
    }
}
