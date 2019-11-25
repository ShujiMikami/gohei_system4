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

#define PORT   80


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

//TCPSocketSetup
static bool setupTCPSocket();

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
    if(!isLinkUp){//最初からLinkdownの場合は抜ける
        return;
    }
    
    //Connect to DHCP server
    bool dhcpConnectResult = connectToDHCPServer();

    if(!dhcpConnectResult){//dhcp接続失敗時は抜ける
        return;
    }



    
    //setup tcp socket
    bool serverIsListened = setupTCPSocket();

    //listening for http GET request
    while (serverIsListened) {
        bool isClientConnected = checkClientConnection();

        if(isClientConnected) {
            led2 = true;

            char buffer[1024] = {};
            int receiveStatus = client.receive_all(buffer, sizeof(buffer) - 1);

            switch(receiveStatus) {
                case 0:
                    DEBUG_PRINT("[Server Thread]recieved buffer is empty.\n\r");
                    break;
                case -1:
                    DEBUG_PRINT("[Server Thread]failed to read data from client.\n\r");
                    break;
                default:
                    DEBUG_PRINT("[Server Thread]Recieved Data: %d\n\r\n\r%.*s\n\r",strlen(buffer), strlen(buffer), buffer);
                    requestAction(buffer);
                    break;
            }

            DEBUG_PRINT("[Server Thread]close connection.\n\r");
            client.close();
            led2 = false;
        }
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
bool setupTCPSocket()
{
    bool result = true;

    //setup tcp socket
    if(svr.bind(PORT)< 0) {
        DEBUG_PRINT("[Server Thread]tcp server bind failed.\n\r");
        result = false;
    } else {
        DEBUG_PRINT("[Server Thread]tcp server bind successed.\n\r");
        result = true;
    }

    if(result){
        if(svr.listen(1) < 0) {
            DEBUG_PRINT("[Server Thread]tcp server listen failed.\n\r");
            result = false;
        } else {
            DEBUG_PRINT("[Server Thread]tcp server listening started...\n\r");
            result = true;
        }
    }

    return result;
}
void setupEthernetInterface()
{
    eth.init(); //Use DHCP
    
    //Print MAC Address
    DEBUG_PRINT("[Server Thread]MAC Address = %s\r\n", eth.getMACAddress());
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