/** @file afe79xxLog.h
 * 	@brief	This file has function definitions regarding logging.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added documentation Level for SPI Log.
*/

#ifndef _LOGS_H_
#define _LOGS_H_

/* Levels of Error Logging */
#define AFE_LOG_LEVEL_ERROR 0   /* error conditions */
#define AFE_LOG_LEVEL_WARNING 1 /*warning conditions */
#define AFE_LOG_LEVEL_INFO 2    /* informational */
#define AFE_LOG_LEVEL_SPILOG 3  /*SPI-level messages */
#define AFE_LOG_LEVEL_DEBUG 4   /*debug-level messages */

#define afeLogErr(fmt, ...) afeLogmsg(AFE_LOG_LEVEL_ERROR, "[%s][%s][%d]ERROR:" fmt "\r\n", __FILE__, __func__, __LINE__, __VA_ARGS__)
#define afeLogDbg(fmt, ...) afeLogmsg(AFE_LOG_LEVEL_DEBUG, "[%s][%s][%d]DEBUG:" fmt "\r\n", __FILE__, __func__, __LINE__, __VA_ARGS__)
#define afeLogSpiLog(fmt, ...) afeLogmsg(AFE_LOG_LEVEL_SPILOG, "[%s][%s][%d]SPI " fmt "\r\n", __FILE__, __func__, __LINE__, __VA_ARGS__)
#define afeLogInfo(fmt, ...) afeLogmsg(AFE_LOG_LEVEL_INFO, "[%s][%s][%d]INFO:" fmt "\r\n", __FILE__, __func__, __LINE__, __VA_ARGS__)

#endif
