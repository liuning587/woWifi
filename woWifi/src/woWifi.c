/**
 ******************************************************************************
 * @file      woWifi.c
 * @brief     C Source file of woWifi.c.
 * @details   This file including all API functions's 
 *            implement of woWifi.c.	
 *
 * @copyright 
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "socket.h"
#include "urldecode.h"

//http://login.gbcom-wifi.com/cmps/admin.php/api/login/?gw_address=172.16.1.1&gw_port=8060&gw_id=GWIFI-0C30EC&ip=172.17.16.20&mac=8C:21:0A:E9:6E:46&url=http%3A//m.baidu.com/

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#define CHECK_URL       "m.baidu.com"
#define GATEWAY         "172.16.1.1"
#define SERVER_URL      "login.gbcom-wifi.com"

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
static char the_cookie[128];
static char the_gw_address[16];
static int the_gw_port;
static char the_gw_id[32];
static char the_ip[16];
static char the_mac[18];
static char the_token[128];
static char the_info[128];

static char the_buf[4096 * 2];

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Global Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief   检测外网
 * @retval  1 : 连接外网成功
 * @retval  0 : 连接外网失败
 *
 * @note    检测方法，尝试连接m.baidu.com
 ******************************************************************************
 */
int
is_relink(void)
{
    const char *psend =
            "GET / HTTP/1.1\r\n"
            "Host: m.baidu.com\r\n"
            "Connection: Keep-Alive\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.152 Safari/537.36\r\n"
            "Accept-Encoding: gzip, deflate, sdch\r\n"
            "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n"
            "\r\n";
    unsigned int sfd = 0;
    int sendlen;
    int ret = 0;

    sfd = socket_init(CHECK_URL, 80);

    if (sfd <= 0)
    {
        return 0;
    }

    sendlen = strlen(psend);
    if (sendlen != socket_send(sfd, psend, sendlen))
    {
        log_print("%s发送数据数据出错\n", __FUNCTION__);
        return ret;
    }

    memset(the_buf, 0x00, sizeof(the_buf));
    while (0 < socket_recv(sfd, the_buf, sizeof(the_buf)))
    {
        if (strstr(the_buf, "<h2>如果没自动跳转, 请<a href='"))
        {
            ret = 1;
            break;
        }
        memset(the_buf, 0x00, sizeof(the_buf));
    }

    socket_close(sfd);

    return ret;
}

/**
 ******************************************************************************
 * @brief   检测网关
 * @retval  1 : 连接网关成功
 * @retval  0 : 连接网关失败
 *
 * @note    检测方法，尝试连接172.16.1.1:80
 ******************************************************************************
 */
int
is_gw_ok(void)
{
    unsigned int sfd = 0;

    sfd = socket_init(GATEWAY, 80);

    if (sfd <= 0)
    {
        return 0;
    }

    socket_close(sfd);

    return 1;
}

/**
 ******************************************************************************
 * @brief   获取gw gw_port gw_id ip mac
 * @param[out] *gw      :
 * @param[out] *gw_port :
 * @param[out] *gw_id   :
 * @param[out] *ip      :
 * @param[out] *mac     :
 *
 * @retval  1 : 成功
 * @retval  0 : 失败
 *
 * @note    http://172.16.1.1:8062/redirect
 ******************************************************************************
 */
int
get_info(char *gw,
        int *port,
        char *gw_id,
        char *ip,
        char *mac)
{
    const char *pinfo =
            "GET /redirect HTTP/1.1\r\n"
            "Host: 172.16.1.1:8062\r\n"
            "Connection: Keep-Alive\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.152 Safari/537.36\r\n"
            "Accept-Encoding: gzip, deflate, sdch\r\n"
            "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n"
            "\r\n";
#if 0
    strcpy(gw, "172.16.1.1");
    *port = 8060;
    strcpy(gw_id, "GWIFI-0C30EC");
    strcpy(ip, "172.17.16.20");
    strcpy(mac, "8C:21:0A:E9:6E:46");
#endif
    unsigned int sfd = 0;
    int sendlen;
    int ret = 0;

    sfd = socket_init(GATEWAY, 8062);

    if (sfd <= 0)
    {
        return 0;
    }

    sendlen = strlen(pinfo);
    if (sendlen != socket_send(sfd, pinfo, sendlen))
    {
        log_print("%s发送数据数据出错\n", __FUNCTION__);
        return ret;
    }

    memset(the_buf, 0x00, sizeof(the_buf));
    while (0 < socket_recv(sfd, the_buf, sizeof(the_buf)))
    {
        //?gw_address=172.16.1.1&gw_port=8060&gw_id=GWIFI-0C30EC&ip=172.17.10.179&mac=74:F0:6D:9D:81:1B&url=
        if (strstr(the_buf, "<h2>如果没自动跳转, 请<a href='")
                && strstr(the_buf, "?gw_address=")
                && strstr(the_buf, "&gw_port=")
                && strstr(the_buf, "&gw_id=")
                && strstr(the_buf, "&ip=")
                && strstr(the_buf, "&mac=")
                && strstr(the_buf, "&url="))
        {
            strstr(the_buf, "&url=")[0] = 0;
            strcpy(mac, strstr(the_buf, "&mac=") + strlen("&mac="));

            strstr(the_buf, "&mac=")[0] = 0;
            strcpy(ip, strstr(the_buf, "&ip=") + strlen("&ip="));

            strstr(the_buf, "&ip=")[0] = 0;
            strcpy(gw_id, strstr(the_buf, "&gw_id=") + strlen("&gw_id="));

            strstr(the_buf, "&gw_id=")[0] = 0;
            *port = 8060;

            strstr(the_buf, "&gw_port=")[0] = 0;
            strcpy(gw, strstr(the_buf, "?gw_address=") + strlen("?gw_address="));
            ret = 1;
            break;
        }
//        log_print("%s", the_buf);
        memset(the_buf, 0x00, sizeof(the_buf));
    }

    socket_close(sfd);

    return ret;
}

/**
 ******************************************************************************
 * @brief   获取cookie
 * @param[in]  sfd      : sock fd
 * @param[out] *cookie  : cookie
 * @param[out] *t       : t
 *
 * @retval  1 : 成功
 * @retval  0 : 失败
 *
 * @details
 ******************************************************************************
 */
int
get_cookie(unsigned int sfd,
        char *cookie,
        int maxlen,
        time_t *t)
{
    /**
     *  1. 网关地址
     *  2. 端口
     *  3. 无线ID
     *  4. ip
     *  5. mac
     *  6. host
     */
    const char *psend =
            "GET /cmps/admin.php/api/login/?gw_address=%s&gw_port=%d&gw_id=%s&ip=%s&mac=%s&url=http%%3A//m.baidu.com/ HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Connection: Keep-Alive\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.152 Safari/537.36\r\n"
            "Accept-Encoding: gzip, deflate, sdch\r\n"
            "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n"
            "\r\n";
    char *pcookie = NULL;
    char *ppath = NULL;
    char *ptime_start = NULL;
    int sendlen = 0;
    int ret = 0;

    *cookie = 0;
    *t = 0;

    //todo: get_info
    snprintf(the_buf, sizeof(the_buf), psend,
            the_gw_address,
            the_gw_port,
            the_gw_id,
            the_ip,
            the_mac,
            SERVER_URL);  //todo: 判断返回值
//
//    snprintf(the_buf, sizeof(the_buf), psend,
//            "172.16.1.1",
//            8060,
//            "GWIFI-0C30EC",
//            "172.17.16.20",
//            "8C:21:0A:E9:6E:46",
//            SERVER_URL);

    sendlen = strlen(the_buf) + 1;
    if (sendlen != socket_send(sfd, the_buf, sendlen))
    {
        log_print("%s发送数据数据出错\n", __FUNCTION__);
        return ret;
    }

    while (0 < socket_recv(sfd, the_buf, sizeof(the_buf)))
    {
        pcookie = strstr(the_buf, "PHPSESSID=");
        if (pcookie && !cookie[0])
        {
            ppath = strstr(the_buf, "; path=/");
            if (ppath)
            {
                *ppath = '\0';
                strncpy(cookie, pcookie + strlen("PHPSESSID="), maxlen);
                ret = 1;
            }
        }
        ptime_start = strstr(the_buf, "name=\"page_time\" value=\"");//1435744862" />")
        if (ptime_start && !t[0])
        {
            ptime_start += strlen("name=\"page_time\" value=\"");
            if ((*ptime_start >= '0') && (*ptime_start <= '9'))
            {
                for (ret = 0; ret < 20; ret++)
                {
                    if (ptime_start[ret] == '"')
                    {
                        ptime_start[ret] = 0;
                        break;
                    }
                }
                *t = atoi(ptime_start);
            }
        }

        memset(the_buf, 0x00, sizeof(the_buf));
    }


    return (*t && *cookie) ? 1 : 0;
}

/**
 ******************************************************************************
 * @brief   拨号
 * @param[in]  username : 用户名
 * @param[in]  password : 密码
 *
 * @retval  1 : 成功
 * @retval  0 : 失败
 ******************************************************************************
 */
int
dial_up(const char *username,
        const char *password)
{
    char content[600];
    char *pstatus = NULL;
    char *ptoken = NULL;
    char *pinfo = NULL;
    char *pend = NULL;
    int len;
    /**
     *  1. 随机数
     *  2. Content-Length
     *  3. gw_address
     *  4. gw_port
     *  5. gw_id
     *  6. ip
     *  7. mac
     *  8. cookie
     *  9. conntent
     */
    const char *psend =
            "POST /cmps/admin.php/api/loginaction?round=%d HTTP/1.1\r\n"
            "Host: login.gbcom-wifi.com\r\n"
            "Connection: keep-alive\r\n"
            "Content-Length: %d\r\n"
            "Accept: */*\r\n"
            "Origin: http://login.gbcom-wifi.com\r\n"
            "X-Requested-With: XMLHttpRequest\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.152 Safari/537.36\r\n"
            "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n"
            "Referer: http://login.gbcom-wifi.com/cmps/admin.php/api/login/?"
            "gw_address=%s&gw_port=%d&gw_id=%s&ip=%s&mac=%s&url=http%%3A//m.baidu.com/\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n"
            "Cookie: PHPSESSID=%s\r\n"
            "\r\n%s";
    /**
     *  1. gw_id
     *  2. gw_address
     *  3. gw_port
     *  4. ip
     *  5. mac
     *  6. page_time
     *  7. gw_address
     *  8. gw_port
     *  9. gw_id
     *  10. ip
     *  11. mac
     *  12. name
     *  13. password
     */
    const char *pcontent =
            "gw_id=%s&gw_address=%s&gw_port=%d&url=http%%3A%%2F%%2F"CHECK_URL"%%2F&ip=%s"
            "&mac=%s&apinfo=&btype=pc&PHPSESSID=&olduser=1&page_time=%d"
            "&lastaccessurl=%%2Fcmps%%2Fadmin.php%%2Fapi%%2Flogin%%2F%%3Fgw_address%%3D%s%%26gw_port%%3D%d"
            "%%26gw_id%%3D%s%%26ip%%3D%s%%26mac%%3D%s%%26url%%3Dhttp%%253A%%2F%%2Fm.baidu.com%%2F"
            "&name=%s&password=%s";

    const char *pgw =
            "GET /wifidog/auth?token=%s&info=%s HTTP/1.1\r\n"
            "Host: %s:%d"
            "Connection: Keep-Alive\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.152 Safari/537.36\r\n"
            "Accept-Encoding: gzip, deflate, sdch\r\n"
            "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n"
            "\r\n";

//    const char *plast =
//            "GET /cmps/admin.php/api/portal/?gw_id=%stoken=%s HTTP/1.1\r\n"
//            "Host: login.gbcom-wifi.com\r\n"
//            "Connection: Keep-Alive\r\n"
//            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
//            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.152 Safari/537.36\r\n"
//            "Accept-Encoding: gzip, deflate, sdch\r\n"
//            "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n"
//            "Cookie: PHPSESSID=%s\r\n"
//            "\r\n";

    int ret = 0;
    unsigned int sfd = 0;
    char *pmac;
    time_t myt;

    get_info(the_gw_address, &the_gw_port, the_gw_id, the_ip, the_mac);
    log_print("\ngw:%s  port:%d id:%s ip:%s mac:%s\n", the_gw_address, the_gw_port, the_gw_id, the_ip, the_mac);
//return 0;
    sfd = socket_init(SERVER_URL, 80);

    if (sfd <= 0)
    {
        return ret;
    }

    if (get_cookie(sfd, the_cookie, sizeof(the_cookie), &myt))
    {
//        log_print("cookie:%s\n", the_cookie);
        pmac = url_encode(the_mac);
        snprintf(content, sizeof(content), pcontent,
                the_gw_id,
                the_gw_address,
                the_gw_port,
                the_ip,
                pmac,
                myt + 5,
                the_gw_address,
                the_gw_port,
                the_gw_id,
                the_ip,
                pmac,
                username,
                password);
        free(pmac);
        log_print("t:%d\n", time(NULL));
//        log_print("content:%s", content);

        len = strlen(content);

        snprintf(the_buf, sizeof(the_buf), psend,
                (rand() % 100) + 700,
                len,
                the_gw_address,
                the_gw_port,
                the_gw_id,
                the_ip,
                the_mac,
                the_cookie,
                content);
        //todo: 判断返回值

        socket_close(sfd);  //这里必须要关闭
        sfd = socket_init(SERVER_URL, 80);

        if (sfd <= 0)
        {
            return ret;
        }
        len = strlen(the_buf);
        if (len != socket_send(sfd, the_buf, len))
        {
            socket_close(sfd);
            log_print("%s发送数据数据出错\n", __FUNCTION__);
            return ret;
        }
//        Sleep(1);
        memset(the_buf, 0x00, sizeof(the_buf));
        while (0 < socket_recv(sfd, the_buf, sizeof(the_buf)))
        {
            pstatus = strstr(the_buf, "{\"status\":");
            if (pstatus)
            {
                if (pstatus[10] == '1')
                {
                    log_print("密码认证成功!\n");
                    ptoken = strstr(the_buf, "?token=");
                    pinfo = strstr(the_buf, "&info=");
                    pend = strstr(the_buf, "\",\"data\":");

                    if (ptoken && pinfo && pend)
                    {
                        *pinfo = 0;
                        *pend = 0;
                        strncpy(the_token, ptoken + 7, sizeof(the_token));
                        strncpy(the_info, pinfo + 6, sizeof(the_info));
//                        log_print("token:%s\n", the_token);
//                        log_print("info:%s\n", the_info);
                        //告诉wifidog网关，token和info
                        socket_close(sfd);

                        sfd = socket_init(the_gw_address, the_gw_port);
                        if (sfd <= 0)
                        {
                            return ret;
                        }

                        snprintf(the_buf, sizeof(the_buf), pgw,
                                the_token, the_info, the_gw_address, the_gw_port);
                        len = strlen(the_buf);
                        if (len != socket_send(sfd, the_buf, len))
                        {
                            log_print("%s发送数据数据出错\n", __FUNCTION__);
                            socket_close(sfd);
                            return ret;
                        }

//                        Sleep(1);
                        while (0 < socket_recv(sfd, the_buf, sizeof(the_buf)));
#if 0
                        //todo: 通知服务器
                        socket_close(sfd);

                        sfd = socket_init(SERVER_URL, 80);
                        if (sfd <= 0)
                        {
                            return ret;
                        }

                        snprintf(the_buf, sizeof(the_buf), plast,
                                the_gw_id, the_token, the_cookie);
                        len = strlen(the_buf);
                        if (len != socket_send(sfd, the_buf, len))
                        {
                            log_print("%s发送数据数据出错\n", __FUNCTION__);
                            socket_close(sfd);
                            return ret;
                        }

                        while (0 < socket_recv(sfd, the_buf, sizeof(the_buf)));
#endif
                        log_print("拨号成功!\n");
                        ret = 1;
                        break;
                    }
                    else
                    {
                        log_print("bug[%d]\n", __LINE__);
                    }
                }
                else
                {
                    log_print("密码认证失败!\n");
                }
            }
            memset(the_buf, 0x00, sizeof(the_buf));
        }
    }
    else
    {
        log_print("bug[%d] cookie get err!\n", __LINE__);
    }

    socket_close(sfd);
    return ret;
}


/*-------------------------------woWifi.c------------------------------------*/
