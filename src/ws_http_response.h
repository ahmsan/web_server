/*
 * ws_http_response.h
 *
 *  Created on: 2016Äê7ÔÂ2ÈÕ
 *      Author: ahmsan
 */

#ifndef WS_HTTP_RESPONSE_H_
#define WS_HTTP_RESPONSE_H_

#include "ws_comm.h"
#include "ws_connection.h"
#include "ws_http_request.h"

#include <map>
using std::map;

namespace ws {

class WSRespInfo {
public:
	static const char* GetHttpMethodInfo(http_method method);
	static const char* GetHttpStatusInfo(http_status status);
	static const char* GetHttpMimeType(const char *file_name);

private:
	static void init();

private:
	static bool	is_map_inited_;
	static map<http_method, string> http_method_info_map_;
	static map<http_status, string> http_status_info_map_;
};




class WSHttpResp {
public:
	WSHttpResp(): resp_len_(0), conn_(NULL),
		request_(NULL) {
	}

	virtual ~WSHttpResp() {
	};

	inline void SetHttpReqPtr(const WSHttpRequest* request) {
		request_ = request;
	}

	inline void SetHttpConnPtr(const WSConnection* conn) {
		conn_ = conn;
	}

	static void SetResourceDirInfo(const string& docroot,
			const string& dyn_dir ) {
		WSHttpResp::docroot_ = docroot;
		WSHttpResp::dynamic_dir_ = dyn_dir;
	}

	int BuildErrResp(http_status stat);

	virtual int HandleHttpReq();

protected:
	// build start line
	int  buildStartLine(http_status stat);

	//  header headr line, one by one
	int  buildHeaderLine(const char *key, const char *val);
	int  buildHeaderLine(const char *key, int val);
	int  buildHeaderLine();

protected:
	int		resp_len_;
	char	resp_buffer_[CONN_WRITE_BUFFER_MAX];

	static string docroot_;			// folder to hold static html resource
	static string dynamic_dir_;		// folder to hold dynamic cgi programs

	WSConnection*	conn_;
	WSHttpRequest* 	request_;
};



// just read file from disk and send it back
class WSStaticHttpResp: public WSHttpResp {
public:
	WSStaticHttpResp();
	virtual ~WSStaticHttpResp();
	virtual int HandleHttpReq();
};



// call cgi to handle request and send it back
class WSDynamicHttpResp: public WSHttpResp {
public:
	WSDynamicHttpResp();
	virtual ~WSDynamicHttpResp();
	virtual int HandleHttpReq();
};


}


#endif /* WS_HTTP_RESPONSE_H_ */
