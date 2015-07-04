/**
 ******************************************************************************
 * @file      socket.c
 * @brief     C Source file of socket.c.
 * @details   This file including all API functions's 
 *            implement of socket.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#ifdef __WIN32
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#endif

#include "log.h"

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* NONE */ 

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
static const char *pdevice = NULL;

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
void
set_device(const char *p)
{
    pdevice = p;
}

/**
 ******************************************************************************
 * @brief      �׽��ֳ�ʼ��
 * @param[in]  *pHostName   : ������
 * @param[in]  port         : �˿ں�
 *
 * @retval     0: ��ʼ��ʧ��
 * @retval    >0: ��ʼ���ɹ�
 ******************************************************************************
 */
unsigned int socket_init(const char *pHostName, unsigned short port)
{
#ifdef __WIN32
    SOCKET sockfd;
    SOCKADDR_IN server_addr;
    WSADATA wsaData;
    int time_out = 1000 * 15; //��ʱ15s
#else
    int sockfd = -1;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    struct timeval timeout = {3,0};
    struct ifreq select_if;
#endif
    struct hostent *host;

    do
    {
#ifdef __WIN32
        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
        {
            log_print("Error at WSAStartup()\n");
            break;
        }
#endif

        if ((host = gethostbyname(pHostName)) == NULL)
        {
            log_print("%s gethostname error\n", __FUNCTION__);
            break;
        }

        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            log_print("socket error\n");
            break;
        }
#ifndef __WIN32
        //�󶨱�������
        if (pdevice)
        {
            strncpy(select_if.ifr_name, pdevice, sizeof(select_if.ifr_name));
            if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                          (char *)&select_if, sizeof(select_if)) < 0)
            {
                log_print("setsockopt error\n");
                break;
            }
        }
#endif

        memset(&server_addr, 0x00, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr = *((struct in_addr*)host->h_addr);

        if (connect(sockfd, (struct sockaddr *)(&server_addr),
                sizeof(struct sockaddr)) == -1)
        {
            log_print("connect error\n");
            break;
        }
#ifdef __WIN32
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&time_out, sizeof(int));
#else
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
#endif
        return (unsigned int)sockfd;
    } while(0);

    return 0u;
}

/**
 ******************************************************************************
 * @brief      �׽��ַ�������
 * @param[in]  socket   : �׽��־��
 * @param[in]  *pbuf    : ���������׵�ַ
 * @param[in]  size     : �������ݳ���
 *
 * @retval     -1   : ����ʧ��
 * @retval     size : ���ͳɹ�
 ******************************************************************************
 */
int socket_send(unsigned int socket, const char *pbuf, int size)
{
    if (socket == 0)
    {
        return -1;
    }
    return send(socket, pbuf, size, 0);
}

/**
 ******************************************************************************
 * @brief      �׽��ֽ�������
 * @param[in]  socket   : �׽��־��
 * @param[in]  *pbuf    : ���������׵�ַ
 * @param[in]  size     : ϣ���������ݳ���
 *
 * @retval     -1   : ����ʧ��
 * @retval     size : ���ճɹ�
 ******************************************************************************
 */
int socket_recv(unsigned int socket, char *pbuf, int size)
{
    if (socket == 0)
    {
        return -1;
    }
    return recv(socket, pbuf, size, 0);
}

/**
 ******************************************************************************
 * @brief      �׽��ֹر�
 * @param[in]  socket   : �׽��־��
 *
 * @return  None
 ******************************************************************************
 */
void socket_close(unsigned int socket)
{
    if (socket != 0)
    {

#ifdef __WIN32
        closesocket(socket);
        WSACleanup(); //needed in winsock
#else
        close(socket);
#endif
    }
}

/*----------------------------socket.c--------------------------------*/
