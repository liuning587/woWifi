/**
 ******************************************************************************
 * @file       config.h
 * @brief      �����ļ�
 * @details    This file including all API functions's declare of config.h.
 * @copyright
 *
 ******************************************************************************
 */
#ifndef CONFIG_H_
#define CONFIG_H_ 

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#ifdef __WIN32
#define DEFAULT_CFG_FILE        "./wowifi.txt"          /**< Ĭ�������ļ� */
#define DEFAULT_LOG_FILE_NAME   "./wowifi.log"          /**< Ĭ����־�ļ� */
#else
#define DEFAULT_CFG_FILE        "/etc/wowifi.txt"          /**< Ĭ�������ļ� */
#define DEFAULT_LOG_FILE_NAME   "/tmp/wowifi.log"       /**< Ĭ����־�ļ� */
#endif
#define DEFAULT_LOG_ON          (1u)                    /**< Ĭ�ϴ���־��¼ */

#endif /* CONFIG_H_ */
/*-----------------------------End of config.h-------------------------------*/
