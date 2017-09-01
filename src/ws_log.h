/*
 * ws_log.h
 *
 *  Created on: 2016Äê6ÔÂ26ÈÕ
 *      Author: mingsanwang
 */

#ifndef WS_LOG_H_
#define WS_LOG_H_

#include "singleton.h"
#include "ws_mutex_lock.h"

namespace ws {

enum log_level {
	ERR		= 1 << 0,
	WARN	= 1 << 1,
	DBG		= 1 << 2,
	INFO	= 1 << 3,
};


////////////////////////////////////////////////


// not thread-safe
// should call Instance before construct threads.
//
// A simple logger
//
class WSLog : private UnCopyable {
public:
	~WSLog();

	bool InitLogger(int log_level, FILE *fp = stdout);
	bool InitLogger(int log_level, const string &log_path);

	void EnableLogger(bool enable);

	void Log(int log_lv, const char *format, ...);

private:
	WSLog(): fp_(NULL), log_level_(0),
		enable_logger_(false), mutex_lock_() {
	}

	bool isNeedLog(int log_lv);

private:
	FILE *			fp_;
	int				log_level_;
	bool			enable_logger_;
	MutexLock		mutex_lock_;

	static char 	s_buffer_[4096];
};


#define LOG_INFO(fmt, args...)  \
	do { \
		Singleton<WSLog>::Instance()->Log(INFO, "%s|%s|%d|%s|"fmt, "INFO",  __FILE__, __LINE__, __FUNCTION__ , ##args); \
	} while ( 0 )

#define LOG_DBG(fmt, args...)  \
	do { \
		Singleton<WSLog>::Instance()->Log(DBG, "%s|%s|%d|%s|"fmt, "DBG",  __FILE__, __LINE__, __FUNCTION__ , ##args); \
	} while ( 0 )

#define LOG_WARN(fmt, args...)  \
	do { \
		Singleton<WSLog>::Instance()->Log(WARN, "%s|%s|%d|%s|"fmt, "WARN",  __FILE__, __LINE__, __FUNCTION__ , ##args); \
	} while ( 0 )

#define LOG_ERR(fmt, args...)  \
	do { \
		Singleton<WSLog>::Instance()->Log(ERR, "%s|%s|%d|%s|"fmt, "ERR",  __FILE__, __LINE__, __FUNCTION__ , ##args); \
	} while ( 0 )


}


#endif /* WS_LOG_H_ */
