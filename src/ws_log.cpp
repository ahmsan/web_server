/*
 * log.cpp
 *
 *  Created on: 2016Äê6ÔÂ26ÈÕ
 *      Author: mingsanwang
 */


#include "ws_itil.h"
#include "ws_file_itil.h"
#include "ws_mutex_lock.h"


namespace ws {


static char WSLog::s_buffer_[4096] = { 0 };

WSLog* WSLog::Instance() {
	WSLog log;
	return &log;
}


bool WSLog::InitLogger(int log_level, FILE *fp) {
	if ( fp == NULL || log_level < 0 ) {
		return false;
	}

	fp_ = fp;
	log_level_ = log_level;

	return true;
}


bool WSLog::InitLogger(int log_level, const string &log_path) {
	if ( log_path.empty() || log_level < 0 ) {
		return false;
	}

	log_level_ = log_level;
	fp_ = fopen(log_path, "a+");

	return fp_ != NULL;
}


void WSLog::EnableLogger(bool enable) {
	enable_logger_ = enable;
}


bool WSLog::isNeedLog(int log_lv) {
	return enable_logger_ && fp_ != NULL && (log_level_ & log_lv);
}



void WSLog::Log(int log_lv, const char *format, ...) {
	if ( isNeedLog(log_lv) ) {
		va_list ap;
		va_start(ap, format);

		{
			MutexLockGuard guard(mutex_lock_);
			if ( fp_ ) {
				fprintf(fp_, "[%s] ", ws::get_cur_date_time_str().c_str() );
				vfprintf(fp_, format, ap);
				fprintf(fp_, "\n");
			}
		}

	    va_end(ap);
	}
}

WSLog::~WSLog() {
	MutexLockGuard guard(mutex_lock_);
	if ( fp_ ) {
		fflush(fp_);
		fclose(fp_);
		fp_ = NULL;
	}
}


}

