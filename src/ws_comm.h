/*
 * ws_comm.h
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#ifndef WS_COMM_H_
#define WS_COMM_H_

#include <stdint.h>

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "ws_log.h"
#include "uncopyable.h"

namespace ws {

const int PATH_LEN_MAX			= 2048;
const int CONN_READ_BUFFER_MAX	= 4096;
const int CONN_WRITE_BUFFER_MAX	= 4096;

enum http_method {
	HTTP_METHOD_GET 	= 1 << 1,
	HTTP_METHOD_POST	= 1 << 2,
	HTTP_METHOD_HEAD	= 1 << 3,

	HTTP_METHOD_INVALID	= -1,
};


enum http_version {
	HTTP_VER_1_0 = 0,
	HTTP_VER_1_1 = 1,

	HTTP_VER_INVALID = -1,
};


enum http_status {
	HTTP_STATUS_OK				= 200,
	HTTP_STATUS_BAD_REQ			= 400,
	HTTP_STATUS_NOT_FOUND		= 404,
	HTTP_STATUS_REQ_TOO_LARGE	= 413,
	HTTP_STATUS_INTERNAL_ERR	= 500,
	HTTP_STATUS_BUFF_OVERFLOW	= 520,
};


enum http_ret_code {
	HTTP_OK		= 0,
	HTTP_FAILED	= -1,
};

struct ws_config_t {
	string 		listen_ip;				// ip used to bind, * means INADDR_ANY
	uint16_t 	listen_port;			// port used to listen.
	uint16_t	listen_backlog;			// backlog.

	uint16_t 	keep_alive_timeout;		// keep alive time(seconds).
	uint16_t 	work_thread_cnt;		// the count of thread, by default it equals to cpu cores.
	uint16_t 	log_level;				// log_level, 0: don't turn on log.
	string		log_file;				// log_file, full path;
	string 		docroot;				// document dir, for static resource;
	string 		cgi_bin;				// cgi dir, for dynamic resource;
	uint16_t	max_fd;					// max fd count.
};


struct ws_key_value_t {
	string key;
	string value;
};


struct http_request_start_line_t {
	http_method		http_mtd_;
	http_version	http_ver_;
	string 			req_path_;
};


}



#endif /* WS_COMM_H_ */
