/**
 ******************************************************************************
 * @file      main.c
 * @brief     C Source file of main.c.
 * @details   This file including all API functions's 
 *            implement of main.c.	
 *
 * @copyright Copyrigth(C), 2008-2012,Sanxing Electric Co.,Ltd.
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#ifdef __WIN32
#include <windows.h>
#else
#include <unistd.h>
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
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Global Function Prototypes
 ----------------------------------------------------------------------------*/
extern int
is_relink(void);

//extern int
//is_gw_ok(void);

extern int
dial_up(const char *username,
        const char *password);

extern int
get_usrpw(int pos,
        char *usr,
        char *pw);
//http://172.16.1.1:8060/wifidog/auth?token=9bfb373578a9cfad957d25fa22d44898019b1a6e&info=MiTfgi2wNgjc4OTkwNjUsLDIwMTUwNjMwMTgzMTAz
/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
int
main(int argc, char ** argv)
{
    int pos = 0;
    int err_cnt = 0;

    char usr[64], pw[64];

    log_init();

    while (1)
    {
__fast_start:
        /* 检测是否重新映射*/
        if (is_relink())
        {
            if (!err_cnt)
                log_print("lost!\n");
            /* 检测网关是否存在 */
//            if (is_gw_ok())
            {
                /* 取用户名 密码 */
                if (get_usrpw(pos, usr, pw))
                {
                    /* 尝试拨号 */
                    if (dial_up(usr, pw))
                    {
                        log_print("dial up success!,usr:%s pw:%s!\n", usr, pw);
                        err_cnt = 0;
                    }
                    else
                    {
                        log_print("dial up failed,usr:%s pw:%s!\n", usr, pw);
                        err_cnt++;
                        if (err_cnt % 3 == 0) //3次失败
                        {
                            goto __fast_start;
                        }
                        else
                        {
                            pos++;
#ifdef __WIN32
                            Sleep(1 + err_cnt / 64);
#else
                            sleep(1 + err_cnt / 64);
#endif
                            //todo: sleep 10s 防止太频繁认证服务器压力太大
                            //10 + err_cnt / 64
                        }
                    }
                }
                else
                {
                    log_print("用户名密码获取错误\n");
                }
            }
        }
#ifdef __WIN32
        Sleep(5);
#else
        sleep(5);
#endif
    }
}

/*--------------------------------main.c-------------------------------------*/
