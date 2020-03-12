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

#define HTTP_PORT   80


volatile bool isThreadTerminateRequired = false;
volatile bool isThreadRebootRequired = false;

typedef int SocketFileDiscriptor_t;

static void requestAction(char* requestMessage, SocketFileDiscriptor_t clientFd);

//Send Page
static void sendPage(SocketFileDiscriptor_t clientFd);

//dhcp wait timeout (x100msec)
#define DHCP_TIMEOUT 100

//DHCPServerConnection
static bool connectToDHCPServer();

volatile static bool isIPSuppliedByDHCP = false;
volatile static struct netif currentNetIf;
volatile static bool isLinkUp = false;

//Ether callbacks
static void netifStatusCallback(struct netif* netIf);
static void netifLinkCallback(struct netif* netIf);

//Ether down handlertask
static void networkDownHandlerTask(const void* argument);

//TCPSocketSetup
static bool setupTCPSocket_Blocking(SocketFileDiscriptor_t* socketFd);
static void setSocketNonBlocking(SocketFileDiscriptor_t socketFd);


static int readAllBytesFromClient(SocketFileDiscriptor_t clientFd, char* buffer, int bufferSize);



static osThreadId serverThreadId;

static SocketFileDiscriptor_t serverSocketFd = -1;

void SetServerThreadID(osThreadId threadID)
{
    serverThreadId = threadID;
}
int readAllBytesFromClient(SocketFileDiscriptor_t clientFd, char* buffer, int bufferSize)
{
    int readCnt = 0;

    while(readCnt < bufferSize){
        char buf;
        int readByte = lwip_read(clientFd, &buf, 1);

        if(readByte > 0){
            buffer[readCnt] = buf;
            readCnt++;
        }else if(readByte == 0){
            readCnt = 0;
            break;
        }else if(readCnt > 0){
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

    static osThreadId serverTaskHandle;

    if(isLinkUp){//切断状態から接続状態に遷移した場合は, サーバータスクをスタート
        isThreadRebootRequired = true;
        osThreadDef(serverActionTask, ServerThreadFunc, osPriorityNormal, 0, 1024);
        serverThreadId = osThreadCreate(osThread(serverActionTask), NULL);
        
    }else{//接続状態から切断状態に遷移した場合は, サーバータスクを終了
        isThreadTerminateRequired = true;

        lwip_close(serverSocketFd);

        osThreadTerminate(serverThreadId);
    }
}



void networkDownHandlerTask(const void* argument)
{
    while(!(isIPSuppliedByDHCP && isLinkUp)){
        osDelay(100);
    }
}
void ServerThreadFunc()
{
    while(!IsMXInitFinished());

    SetNetIfStatusCallback(netifStatusCallback);
    SetNetIfLinkCallback(netifLinkCallback);

    //Link状態取得
    isLinkUp = (IsLinkUp() >= 0);
    bool isLANConnected = isLinkUp;

    //Connect to DHCP server
    if(isLANConnected){
        isLANConnected = connectToDHCPServer();
    }
    
    //setup tcp socket
    if(isLANConnected){
        isLANConnected = setupTCPSocket_Blocking(&serverSocketFd);
    }

    //server routine
    while(isLANConnected){
        //accept client : blocked
        struct sockaddr_in client_addr;
        int client_addr_len;
        int clientfd = lwip_accept(serverSocketFd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);

        //receive client message
        char receiveBuffer[1024] = { 0 };
        int receivedCnt = 0;
        if(clientfd >= 0){
            //set client to non-block-mode
            setSocketNonBlocking(clientfd);

            receivedCnt = readAllBytesFromClient(clientfd, receiveBuffer, sizeof(receiveBuffer));

            //receivedAction
            if(receivedCnt > 0){
                requestAction(receiveBuffer, clientfd);
            }
        }

        //close connection
        close(clientfd);
    }
    while(1){
    }
}
void requestAction(char* requestMessage, SocketFileDiscriptor_t clientFd)
{
    if(GetMethod(requestMessage) == GET){
        char uri[20];

        GetURI(requestMessage, uri, sizeof(uri));

        if(strcmp(uri, "/") == 0){
            sendPage(clientFd);
        }else if(strcmp(uri, "/UVToggle") == 0){
            // UVToggleFromEther();
            sendPage(clientFd);
        }else if(strcmp(uri, "/UVToggle?") == 0){
            // UVToggleFromEther();
            sendPage(clientFd);
        }
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
void sendPage(SocketFileDiscriptor_t clientFd)
{
    char htmlToSend[1024] = {};

    CageStatus_t cageStatus = GetCageStatus();
    CreateTopPage(htmlToSend, sizeof(htmlToSend), cageStatus.temperature, cageStatus.statusMessage, cageStatus.uvStatusMessage);

    lwip_send(clientFd, htmlToSend, strlen(htmlToSend), 0);
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
        *socketFd = -1;
    }

    return status;
}
