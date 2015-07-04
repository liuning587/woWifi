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

extern void
set_device(const char *p);

//http://172.16.1.1:8060/wifidog/auth?token=9bfb373578a9cfad957d25fa22d44898019b1a6e&info=MiTfgi2wNgjc4OTkwNjUsLDIwMTUwNjMwMTgzMTAz
/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
int
main(int argc, char ** argv)
{
    int dev = 0;
    int pos[2] = {0, 0};
    int err_cnt[2] = {0, 0};

    const char *pdevice[2] = {"apcli0", "eth0.2"};
    char usr[64], pw[64];

    log_init();
    set_device(pdevice[dev]);

    while (1)
    {
__fast_start:
        /* ����Ƿ�����ӳ��*/
        if (is_relink())
        {
            if (!err_cnt[dev])
            {
                log_print("dev:%s lost!\n", pdevice[dev]);
            }
            {
                /* ȡ�û��� ���� */
                if (get_usrpw(dev, usr, pw))  //dev fixme : ����ֻȡ2������
                {
                    /* ���Բ��� */
                    if (dial_up(usr, pw))
                    {
                        log_print("dial up success!,usr:%s pw:%s!\n", usr, pw);
                        err_cnt[dev] = 0;
                    }
                    else
                    {
                        log_print("dial up failed,usr:%s pw:%s!\n", usr, pw);
                        err_cnt[dev]++;
                        if (err_cnt[dev] % 3 == 0) //3��ʧ��
                        {
                            goto __fast_start;
                        }
                        else
                        {
                            pos[dev]++;
#ifdef __WIN32
                            Sleep(1 + err_cnt[dev] / 64);
#else
                            sleep(1 + err_cnt[dev] / 64);
#endif
                            //todo: sleep 10s ��ֹ̫Ƶ����֤������ѹ��̫��
                            //10 + err_cnt / 64
                        }
                    }
                }
                else
                {
                    log_print("�û��������ȡ����\n");
                }
            }
        }
        dev++;
        dev &= 1;
        set_device(pdevice[dev]);   //�л�����,�������д��̫���˲�̫��!
#ifdef __WIN32
        Sleep(2);
#else
        sleep(2);
#endif
    }
}

/*--------------------------------main.c-------------------------------------*/
