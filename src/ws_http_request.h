/*
 * ws_http_request.h
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#ifndef WS_HTTP_REQUEST_H_
#define WS_HTTP_REQUEST_H_

#include "ws_comm.h"
#include "ws_parser.h"

#include <map>
using std::map;

namespace ws {


class WSHttpRequest: private UnCopyable {
public:
	WSHttpRequest();

	int  ParseHttpRequest(const char *http, http_status &stat);
	int  ParseHttpRequestBody(const char *str, http_status &stat);
	int  GetHttpMethod(const char *str);
	bool HasCompleteHeader(const char *str, int len);

	const char* GetHttpBodyStartPos(const char *str, int len);

	// static request(read data from disk and response it back)
	// dynamic request
	inline bool IsDynamicReq() {
		return this->query_map_.size() > 0;
	}

	inline const http_request_start_line_t&
	GetReqStartLineInfo() {
		return this->start_line_;
	}

	inline bool IsKeepAlive() {
		return this->keep_alive_;
	}

	// print http request info
	// call it after ParseHttpRequest
	// for test
	void PrintParsedInfo();

private:
	int  parseHttpStartLine(const char *str);
	int  parseHttpHeader(const char *str);
	int  parseHttpQueryString(const char *str);

	string decodeURL(string &str);

private:

	// only parse these two key in the header
	bool 	keep_alive_;
	int		content_len_;

	http_request_start_line_t start_line_;

	map<string, string> query_map_;
	WSParser 			parser_;
};

}


#endif /* WS_HTTP_REQUEST_H_ */
