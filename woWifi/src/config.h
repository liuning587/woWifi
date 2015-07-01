/**
 ******************************************************************************
 * @file       config.h
 * @brief      配置文件
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
#define DEFAULT_CFG_FILE        "./wowifi.txt"          /**< 默认配置文件 */
#define DEFAULT_LOG_FILE_NAME   "./wowifi.log"          /**< 默认日志文件 */
#else
#define DEFAULT_CFG_FILE        "/etc/wowifi.txt"          /**< 默认配置文件 */
#define DEFAULT_LOG_FILE_NAME   "/tmp/wowifi.log"       /**< 默认日志文件 */
#endif
#define DEFAULT_LOG_ON          (1u)                    /**< 默认打开日志记录 */

#endif /* CONFIG_H_ */
/*-----------------------------End of config.h-------------------------------*/
