#ifndef LOG_H
#define LOG_H

#include <stdio.h> /* for FILE */

enum log_level {
	LOG_LEVEL_EMERG = 0,
	LOG_LEVEL_ALERT = 1,
	LOG_LEVEL_CRIT = 2,
	LOG_LEVEL_ERR = 3,
	LOG_LEVEL_WARNING = 4,
	LOG_LEVEL_NOTICE = 5,
	LOG_LEVEL_INFO = 6,
	LOG_LEVEL_DEBUG = 7
};
#define LOG_LEVEL_EMERGENCY LOG_LEVEL_EMERG
#define LOG_LEVEL_CRITICAL LOG_LEVEL_CRIT
#define LOG_LEVEL_ERROR LOG_LEVEL_ERR
#define LOG_LEVEL_WARN LOG_LEVEL_WARNING

int log_emit(enum log_level lvl, const char *file, unsigned lineno,
	     const char *fmt, ...);

enum log_level log_level(void);
void log_level_set(enum log_level lvl);

FILE *log_file(void);
void log_file_set(FILE *f);

#define log_emerg(FMT) log_emit(LOG_LEVEL_EMERG, __FILE__, __LINE__, FMT)
#define log_emerg1(FMT, A1) \
	log_emit(LOG_LEVEL_EMERG, __FILE__, __LINE__, FMT, A1)
#define log_emerg2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_EMERG, __FILE__, __LINE__, FMT, A1, A2)
#define log_emerg3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_EMERG, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_emerg4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_EMERG, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_emerg5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_EMERG, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#define log_alert(FMT) log_emit(LOG_LEVEL_ALERT, __FILE__, __LINE__, FMT)
#define log_alert1(FMT, A1) \
	log_emit(LOG_LEVEL_ALERT, __FILE__, __LINE__, FMT, A1)
#define log_alert2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_ALERT, __FILE__, __LINE__, FMT, A1, A2)
#define log_alert3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_ALERT, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_alert4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_ALERT, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_alert5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_ALERT, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#define log_crit(FMT) log_emit(LOG_LEVEL_CRIT, __FILE__, __LINE__, FMT)
#define log_crit1(FMT, A1) log_emit(LOG_LEVEL_CRIT, __FILE__, __LINE__, FMT, A1)
#define log_crit2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_CRIT, __FILE__, __LINE__, FMT, A1, A2)
#define log_crit3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_CRIT, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_crit4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_CRIT, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_crit5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_CRIT, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#define log_err(FMT) log_emit(LOG_LEVEL_ERR, __FILE__, __LINE__, FMT)
#define log_err1(FMT, A1) log_emit(LOG_LEVEL_ERR, __FILE__, __LINE__, FMT, A1)
#define log_err2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_ERR, __FILE__, __LINE__, FMT, A1, A2)
#define log_err3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_ERR, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_err4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_ERR, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_err5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_ERR, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#define log_warning(FMT) log_emit(LOG_LEVEL_WARNING, __FILE__, __LINE__, FMT)
#define log_warning1(FMT, A1) \
	log_emit(LOG_LEVEL_WARNING, __FILE__, __LINE__, FMT, A1)
#define log_warning2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_WARNING, __FILE__, __LINE__, FMT, A1, A2)
#define log_warning3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_WARNING, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_warning4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_WARNING, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_warning5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_WARNING, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#define log_notice(FMT) log_emit(LOG_LEVEL_NOTICE, __FILE__, __LINE__, FMT)
#define log_notice1(FMT, A1) \
	log_emit(LOG_LEVEL_NOTICE, __FILE__, __LINE__, FMT, A1)
#define log_notice2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_NOTICE, __FILE__, __LINE__, FMT, A1, A2)
#define log_notice3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_NOTICE, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_notice4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_NOTICE, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_notice5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_NOTICE, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#define log_info(FMT) log_emit(LOG_LEVEL_INFO, __FILE__, __LINE__, FMT)
#define log_info1(FMT, A1) log_emit(LOG_LEVEL_INFO, __FILE__, __LINE__, FMT, A1)
#define log_info2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_INFO, __FILE__, __LINE__, FMT, A1, A2)
#define log_info3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_INFO, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_info4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_INFO, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_info5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_INFO, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#define log_debug(FMT) log_emit(LOG_LEVEL_DEBUG, __FILE__, __LINE__, FMT)
#define log_debug1(FMT, A1) \
	log_emit(LOG_LEVEL_DEBUG, __FILE__, __LINE__, FMT, A1)
#define log_debug2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_DEBUG, __FILE__, __LINE__, FMT, A1, A2)
#define log_debug3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_DEBUG, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_debug4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_DEBUG, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_debug5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_DEBUG, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#define log_emergency(FMT) \
	log_emit(LOG_LEVEL_EMERGENCY, __FILE__, __LINE__, FMT)
#define log_emergency1(FMT, A1) \
	log_emit(LOG_LEVEL_EMERGENCY, __FILE__, __LINE__, FMT, A1)
#define log_emergency2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_EMERGENCY, __FILE__, __LINE__, FMT, A1, A2)
#define log_emergency3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_EMERGENCY, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_emergency4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_EMERGENCY, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_emergency5(FMT, A1, A2, A3, A4, A5)                                \
	log_emit(LOG_LEVEL_EMERGENCY, __FILE__, __LINE__, FMT, A1, A2, A3, A4, \
		 A5)

#define log_critical(FMT) log_emit(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, FMT)
#define log_critical1(FMT, A1) \
	log_emit(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, FMT, A1)
#define log_critical2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, FMT, A1, A2)
#define log_critical3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_critical4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_critical5(FMT, A1, A2, A3, A4, A5)                                \
	log_emit(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, FMT, A1, A2, A3, A4, \
		 A5)

#define log_error(FMT) log_emit(LOG_LEVEL_ERROR, __FILE__, __LINE__, FMT)
#define log_error1(FMT, A1) \
	log_emit(LOG_LEVEL_ERROR, __FILE__, __LINE__, FMT, A1)
#define log_error2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_ERROR, __FILE__, __LINE__, FMT, A1, A2)
#define log_error3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_ERROR, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_error4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_ERROR, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_error5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_ERROR, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#define log_warn(FMT) log_emit(LOG_LEVEL_WARN, __FILE__, __LINE__, FMT)
#define log_warn1(FMT, A1) log_emit(LOG_LEVEL_WARN, __FILE__, __LINE__, FMT, A1)
#define log_warn2(FMT, A1, A2) \
	log_emit(LOG_LEVEL_WARN, __FILE__, __LINE__, FMT, A1, A2)
#define log_warn3(FMT, A1, A2, A3) \
	log_emit(LOG_LEVEL_WARN, __FILE__, __LINE__, FMT, A1, A2, A3)
#define log_warn4(FMT, A1, A2, A3, A4) \
	log_emit(LOG_LEVEL_WARN, __FILE__, __LINE__, FMT, A1, A2, A3, A4)
#define log_warn5(FMT, A1, A2, A3, A4, A5) \
	log_emit(LOG_LEVEL_WARN, __FILE__, __LINE__, FMT, A1, A2, A3, A4, A5)

#endif /* LOG_H */
