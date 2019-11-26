#include "ServerAction.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <cmsis_os.h>
#include "HTTPAnalyze.h"
#include "HTTPGenerator.h"
#include "CageDriver.h"

#include "lwip/sockets.h"
#include "lwip.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) while(0)
#endif



EthernetInterface eth;

TCPSocketServer svr;

TCPSocketConnection client;

DigitalOut led2(LED2); //socket connecting status

//なぜかTickを宣言しないとEthernetがこける
//Ticker dummyTick;

void requestAction(char* requestMessage);

//EthernetInterfaceSetup
static void setupEthernetInterface();

//DHCPServerConnection
static void connectToDHCPServer();


//check client connection
static bool checkClientConnection();

//ForDebug
static void printRequestLine(HTTPRequest_t request);
static void printURI(HTTPRequest_t request);
static void printProtocol(HTTPRequest_t request);

//Send Page
static void sendPage();

//dhcp wait timeout (x100msec)
#define DHCP_TIMEOUT 100

//DHCPServerConnection
static void connectToDHCPServer();

volatile bool isIPSuppliedByDHCP = false;
volatile struct netif currentNetIf;
volatile bool isLinkUp = false;

//Ether callbacks
static void netifStatusCallback(struct netif* netIf);
static void netifLinkCallback(struct netif* netIf);

//Ether down handlertask
static void networkDownHandlerTask(const void* argument);

//TCPSocketSetup
typedef int SocketFileDiscriptor_t;
static bool setupTCPSocket_Blocking(SocketFileDiscriptor_t* socketFd);
static void setSocketNonBlocking(SocketFileDiscriptor_t socketFd);

#define HTTP_PORT   80

static int readAllBytesFromClient(SocketFileDiscriptor_t clientFd, char* buffer, int bufferSize);

int readAllBytesFromClient(SocketFileDiscriptor_t clientFd, char* buffer, int bufferSize)
{
    int readCnt = 0;

    while(readCnt < bufferSize){
        char buf;
        int readByte = lwip_read(clientFd, &buf, 1);

        if(readByte > 0){
            buffer[readCnt] = buf;
            readCnt++;
        }else{
            break;
        }
    }

    return readCnt;
}
void setSocketNonBlocking(SocketFileDiscriptor_t socketFd)
{
    int currentVal = lwip_fcntl(socketFd, F_GETFL, 0);
    lwip_fcntl(socketFd, F_SETFL, currentVal | O_NONBLOCK);
}
void netifStatusCallback(struct netif* netIf)
{
  isIPSuppliedByDHCP = true;
  currentNetIf = *netIf;
}
void netifLinkCallback(struct netif* netIf)
{
    isLinkUp = ((netIf->flags & NETIF_FLAG_LINK_UP) != 0);
    currentNetIf = *netIf;
}


void networkDownHandlerTask(const void* argument)
{
    while(!(isIPSuppliedByDHCP && isLinkUp)){
        osDelay(100);
    }
}
void ServerThreadFunc()
{
    bool isLANConnected = true;
    if(!isLinkUp){//最初からLinkdownの場合は抜ける
        isLANConnected = false;
    }
    
    //Connect to DHCP server
    if(isLANConnected){
        isLANConnected = connectToDHCPServer();
    }
    
    //setup tcp socket
    SocketFileDiscriptor_t serverSocketFd = -1;
    if(isLANConnected){
        isLANConnected = setupTCPSocket_Blocking(&serverSocketFd);
    }

    //server routine
    while(isLANConnected){
        //accept client : blocked
        struct sockaddr_in client_addr;
        int client_addr_len;
        int clientfd = accept(serverSocketFd, (struct sockaddr*)&client_addr, &client_addr_len);

        //receive client message
        char receiveBuffer[1024] = { 0 };
        int receivedCnt = 0;
        if(clientfd >= 0){
            //set client to non-block-mode
            setSocketNonBlocking(clientfd);

            receivedCnt = readAllBytesFromClient(clientfd, receiveBuffer, sizeof(receiveBuffer));

            //receivedAction
        }

        //close connection
        close(clientfd);
    }
}
void requestAction(char* requestMessage)
{
    //request line
    HTTPRequest_t request(requestMessage);

    printRequestLine(request);
    printURI(request);
    printProtocol(request);

    char uri[128];
    request.GetURI(uri, sizeof(uri));

    if(strcmp(uri, "/") == 0){
        DEBUG_PRINT("[Server Thread]Top page access\r\n");

        sendPage();
    }else if(strcmp(uri, "/UVToggle") == 0){
        UVToggleFromEther();

        DEBUG_PRINT("[Server Thread]toggle page access\r\n");

        sendPage();
    }else if(strcmp(uri, "/UVToggle?") == 0){
        UVToggleFromEther();

        DEBUG_PRINT("[Server Thread]toggle page access\r\n");

        sendPage();
    }
}

bool connectToDHCPServer()
{
    int cntLimit = 0;

    while(!isIPSuppliedByDHCP && cntLimit < DHCP_TIMEOUT){
        cntLimit++;
        osDelay(100);
    }

    return (cntLimit < DHCP_TIMEOUT);
}
void printRequestLine(HTTPRequest_t request)
{
    char requestLine[250];
    request.GetRequestLine(requestLine, sizeof(requestLine));
    DEBUG_PRINT("request line = %s\r\n", requestLine);
}
void printURI(HTTPRequest_t request)
{
    char uri[250];
    request.GetURI(uri, sizeof(uri));
    DEBUG_PRINT("uri is = %s\r\n", uri);
}
void sendPage()
{
    char htmlToSend[1024] = {};

    CageStatus_t cageStatus = GetCageStatus();
    CreateTopPage(htmlToSend, sizeof(htmlToSend), cageStatus.temperature, cageStatus.statusMessage, cageStatus.uvStatusMessage);
    
    DEBUG_PRINT("[Server Thread]send : %s", htmlToSend);

    client.send(htmlToSend, strlen(htmlToSend));
}
void printProtocol(HTTPRequest_t request)
{
    char uri[250];
    request.GetProtocolVersion(uri, sizeof(uri));
    DEBUG_PRINT("[Server Thread]protocol is %s \r\n", uri);
}
bool setupTCPSocket_Blocking(SocketFileDiscriptor_t* socketFd)
{
    bool status = true;

    //create socket
    SocketFileDiscriptor_t sockfd = lwip_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    status = (sockfd >= 0);

    //bind socket
    if(status){
        struct sockaddr_in reader_addr;
        memset(&reader_addr, 0, sizeof(reader_addr));
        reader_addr.sin_family = AF_INET;
        reader_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        reader_addr.sin_port = htons(HTTP_PORT);

        int result = bind(sockfd, (struct sockaddr*)&reader_addr, sizeof(reader_addr));

        status = (result >= 0);
    }

    //listen
    if(status){
        int result = listen(sockfd, 5);
        status = (result >= 0);
    }

    //reuturn sockfd
    if(status){
        *socketFd = sockfd;
    }else{
        socketFd = -1;
    }

    return status;
}

bool checkClientConnection()
{
    DEBUG_PRINT("[Server Thread]waiting for client connection...\n\r");

    bool result = false;

    //blocking mode(never timeout)
    if(svr.accept(client) < 0) {
        DEBUG_PRINT("[Server Thread]failed to accept connection.\n\r");
        result = false;
    } else {
        DEBUG_PRINT("[Server Thread]connection success!\n\rIP: %s\n\r",client.get_address());
        result = true;
    }

    return result;
}