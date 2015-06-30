/**
 ******************************************************************************
 * @file      log.c
 * @brief     ��־��¼ģ��
 * @details   This file including all API functions's 
 *            implement of log.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "config.h"
#include "log.h"

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#ifndef DEFAULT_LOG_FILE_NAME
# define DEFAULT_LOG_FILE_NAME   "./MStationServer.txt"  /**< Ĭ����־�ļ� */
#endif

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
static char the_log_flag = 1;   /**< ��־��¼��־ */
static FILE *the_log_fp = NULL;

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      ��־ģ���ʼ��
 * @param[in]  *pname : ��־�ļ�
 * @param[in]  logflag: �Ƿ��¼��־
 *
 * @retval     0 : ��ʼ���ɹ�
 * @retval    -1 : ��ʼ��ʧ��
 ******************************************************************************
 */
int
log_init(void)
{
    if (the_log_fp == NULL)
    {
        the_log_fp = fopen(DEFAULT_LOG_FILE_NAME, "a+");
        if (the_log_fp == NULL)
        {
            return -1;
        }
    }
    return 0;
}

/**
 ******************************************************************************
 * @brief      ��־ģ���˳�
 * @param[in]  None
 *
 * @retval  NONE
 ******************************************************************************
 */
void
log_exit(void)
{
    if (the_log_fp != NULL)
    {
        (void)fclose(the_log_fp);
        the_log_fp = NULL;
    }
}

/**
 ******************************************************************************
 * @brief      �����Ƿ��¼��־
 * @param[in]  logflag: �Ƿ��¼��־(0:����¼ 1:��¼)
 *
 * @retval  NONE
 ******************************************************************************
 */
void
log_on(char logflag)
{
    the_log_flag = logflag & 0x01;
}

/**
 ******************************************************************************
 * @brief      ��¼buf
 * @param[in]  *pformat : ����
 * @param[in]  *pbuffer : �����׵�ַ
 * @param[in]  len      : ����
 *
 * @retval     None
 ******************************************************************************
 */
void
log_buf(const char *pformat,
        const char *pbuffer,
        int len)
{
    int i;

    if ((the_log_fp != NULL) && (the_log_flag))
    {
        (void)fprintf(the_log_fp, pformat);
        for (i = 0; i < len; i++)
        {
            (void)fprintf(the_log_fp, "%02X ", *(pbuffer + i));
        }
        (void)fprintf(the_log_fp, "\n");

        (void)printf(pformat);
        for (i = 0; i < len; i++)
        {
            (void)printf("%02X ", *(pbuffer + i));
        }
        (void)printf("\n");
    }
}

/**
 ******************************************************************************
 * @brief      ��־print
 * @param[in]  fmt  : ��־��Ϣ��printf��ͬ
 *
 * @retval  None
 ******************************************************************************
 */
void
log_print(const char *fmt, ...)
{
    va_list args;

    if ((the_log_fp != NULL) && (the_log_flag))
    {
        struct tm daytime;
        time_t stime = time(NULL);
        daytime = *localtime(&stime);
        (void)fprintf(the_log_fp, "[%04d-%02d-%02d %02d:%02d:%02d]",
                daytime.tm_year + 1900,
                daytime.tm_mon + 1,
                daytime.tm_mday,
                daytime.tm_hour,
                daytime.tm_min,
                daytime.tm_sec);
        va_start( args, fmt );
        (void)vfprintf(the_log_fp, fmt, args);
        (void)vprintf(fmt, args);
        fflush(the_log_fp);
        fflush(stdout);
    }
}
/*----------------------------------log.c------------------------------------*/
