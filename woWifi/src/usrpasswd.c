/**
 ******************************************************************************
 * @file      usrpasswd.c
 * @brief     C Source file of usrpasswd.c.
 * @details   This file including all API functions's 
 *            implement of usrpasswd.c.	
 *
 * @copyright Copyrigth(C), 2008-2012,Sanxing Electric Co.,Ltd.
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include "config.h"
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
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
int
usrpw(FILE *fd,
        char *usr,
        char *pw)
{
    char line[80];

    if (fgets(line, sizeof(line), fd))
    {
        if (2 == sscanf(line, "%s %s", usr, pw))
        {
            return 1;
        }
    }

    return 0;
}

int
total_cnt(FILE *fd)
{
    char usr[64], pw[64];
    int cnt = 0;

    fseek(fd, 0, SEEK_SET);
    while (!feof(fd))
    {
        if (usrpw(fd, usr, pw))
        {
            cnt++;
        }
    }

    return cnt;
}

int
get_usrpw(int pos,
        char *usr,
        char *pw)
{
    int ret = 0;
    int cnt = 0;
    FILE *fd = NULL;

    fd = fopen(DEFAULT_CFG_FILE, "r");
    if (!fd)
    {
        log_print("未发现用户名配置文件!\n");
        return ret;
    }

    cnt = total_cnt(fd);
    pos %= cnt;
    cnt = 0;

    fseek(fd, 0, SEEK_SET);
    while (!feof(fd))
    {
        if (usrpw(fd, usr, pw))
        {
            if (cnt == pos)
            {
                ret = 1;
                break;
            }
            cnt++;
        }
    }

    fclose(fd);
    return ret;
}

/*-------------------------------usrpasswd.c---------------------------------*/
