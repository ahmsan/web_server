/*
 * ws_http_response.cpp
 *
 *  Created on: 2016Äê7ÔÂ2ÈÕ
 *      Author: ahmsan
 */

#include "ws_http_response.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace ws {

bool WSRespInfo::is_map_inited_ = false;
map<http_status, const char *> WSRespInfo::http_status_info_map_;
map<http_status, const char *> WSRespInfo::http_method_info_map_;
//
//enum http_status {
//	HTTP_STATUS_OK				= 200,
//	HTTP_STATUS_BAD_REQ			= 400,
//	HTTP_STATUS_NOT_FOUND		= 404,
//	HTTP_STATUS_REQ_TOO_LARGE	= 413,
//	HTTP_STATUS_INTERNAL_ERR	= 500,
//};
void WSRespInfo::init() {
	http_status_info_map_.clear();

	http_status_info_map_[HTTP_STATUS_OK] 			 = "OK";
	http_status_info_map_[HTTP_STATUS_BAD_REQ] 		 = "Bad request";
	http_status_info_map_[HTTP_STATUS_NOT_FOUND]	 = "Not found";
	http_status_info_map_[HTTP_STATUS_REQ_TOO_LARGE] = "Request too large";
	http_status_info_map_[HTTP_STATUS_INTERNAL_ERR]  = "Internal server error";
	http_status_info_map_[HTTP_STATUS_BUFF_OVERFLOW] = "Server buffer overflow";

	http_method_info_map_[HTTP_METHOD_GET]	= "GET";
	http_method_info_map_[HTTP_METHOD_POST]	= "POST";
	http_method_info_map_[HTTP_METHOD_HEAD]	= "HEAD";

	is_map_inited_ = true;
}


const char* WSRespInfo::GetHttpMethodInfo(http_method method) {
	if ( !is_map_inited_ ) {
		init();
	}

	const char *method_info = "null";
	if ( is_map_inited_ ) {
		method_info = http_method_info_map_[method];
	}

	return method_info;
}

const char* WSRespInfo::GetHttpStatusInfo(http_status stat) {
	if ( !is_map_inited_ ) {
		init();
	}

	const char *status_info = "null";
	if ( is_map_inited_ ) {
		status_info = http_status_info_map_[stat];
	}

	return status_info;
}

const char* WSRespInfo::GetHttpMimeType(const char *file_name) {
	const char* unknown_type = "unknown_type";
	if ( file_name == NULL ) {
		return unknown_type;
	}
	const char *last_dot = strrchr(file_name, '.');
	if ( last_dot == NULL ) {
		return unknown_type;
	}
	int comp_len = 3; // match the file extention
	const char *ext_first = last_dot + 1;
	if ( strncmp(ext_first, "htm", comp_len) == 0) {
		return "text/html";
	} else if ( strncmp(ext_first, "txt", comp_len) == 0) {
		return "text/plain";
	} else if ( strncmp(ext_first, "css", comp_len) == 0) {
		return "text/css";
	} else if ( strncmp(ext_first, "jpg", comp_len) == 0) {
		return "image/jpeg";
	} else if ( strncmp(ext_first, "png", comp_len) == 0) {
		return "image/png";
	} else if ( strncmp(ext_first, "js", comp_len - 1) == 0) {
		return "application/javascript";
	} else {
		return unknown_type;
	}
}



static string WSHttpResp::docroot_;
static string WSHttpResp::dynamic_dir_;


int WSHttpResp::HandleHttpReq() {
	return 0;
}

int WSHttpResp::BuildErrResp(http_status stat) {
	const http_request_start_line_t& start_line =
			request_->GetReqStartLineInfo();
	buildStartLine(stat);
	buildHeaderLine("Content-Type", "text/html");
	buildHeaderLine("Connection", "Close");
	buildHeaderLine("Server", "ws_server");
	buildHeaderLine();

	if ( conn_ ) {
		conn_->WriteToBuffer(resp_buffer_, resp_len_);
		conn_->HandleWrite();
	}
	return 0;
}

// build start line
int WSHttpResp::buildStartLine(http_status stat) {

	const http_request_start_line_t& start_line =
			request_->GetReqStartLineInfo();
	int len = snprintf(resp_buffer_, sizeof(resp_buffer_), "HTTP/1.%s %d %s\r\n",
				start_line.http_ver_ == HTTP_VER_1_0 ? "0" : "1",
				stat, WSRespInfo::GetHttpStatusInfo(stat));

	resp_len_ = len;

	return 0;
}

//  header headr line, one by one
int WSHttpResp::buildHeaderLine(const char *key, const char *val) {
	int len = 0;
	if ( key ) {
		len = snprintf(resp_buffer_ + resp_len_, sizeof(resp_buffer_) - resp_len_,
					"%s: %s\r\n", key, val);
	} else {
		len = snprintf(resp_buffer_ + resp_len_, sizeof(resp_buffer_) - resp_len_, "\r\n");
	}
	if ( len < 2 ) {	// invalid
		return -1;
	}
	resp_len_ += len;
	return 0;
}


int WSHttpResp::buildHeaderLine(const char *key, int val) {
	char str[32];
	sprintf(str, "%d", val);
	return buildHeaderLine(key, str);
}

int WSHttpResp::buildHeaderLine() {
	return buildHeaderLine(NULL, NULL);
}


//////////////////////////////////////////////////////////////////////


WSStaticHttpResp::WSStaticHttpResp() {
}

WSStaticHttpResp::~WSStaticHttpResp() {
}

int WSStaticHttpResp::HandleHttpReq() {
	const http_request_start_line_t &start_line
		= request_->GetReqStartLineInfo();

	const string file_full_path = WSHttpResp::docroot_;
	file_full_path.append(start_line.req_path_);

	int file_len = 0;
	if ( !WSFileItil::IsFileExist(file_full_path)) {
		// TODO Not found
		LOG_ERR("file: %s isn't found!", file_full_path.c_str());
		return BuildErrResp(HTTP_STATUS_NOT_FOUND);
	}

	// for performace, this should be use CACHE
	file_len = WSFileItil::GetFileLen(file_full_path);
	if ( file_len != 0 ) {
		// TODO Invalid file -- internal err
		LOG_ERR("get file len failed: %s", file_full_path.c_str());
		return BuildErrResp(HTTP_STATUS_INTERNAL_ERR);
	}

	buildStartLine(HTTP_STATUS_OK);
	buildHeaderLine("Content-Type", WSRespInfo::GetHttpMimeType(file_full_path.c_str()));
	buildHeaderLine("Connection", request_->IsKeepAlive() ? "keep-alive" : "Close");
	buildHeaderLine("Content_Length", file_len);
	buildHeaderLine("Server", "ws_server");
	buildHeaderLine();

	if ( file_len > (int)sizeof(resp_buffer_) - resp_len_ ) {
		// TODO Buffer overflow
		LOG_ERR("buffer is overflow");
		return BuildErrResp(HTTP_STATUS_BUFF_OVERFLOW);
	}

	FIFE *fp = fopen(file_full_path.c_str(), "r");
	if ( fp ) {
		RAIIFpGuard guard(fp);
		fread(resp_buffer_ + resp_len_, file_len, 1, fp);
		conn_->WriteToBuffer(resp_buffer_, resp_len_);
		conn_->HandleWrite();
		LOG_INFO("read file from disk and resp it back succ!");
	} else {
		LOG_ERR("fopen file: %s failed!", file_full_path.c_str());
		return BuildErrResp(HTTP_STATUS_INTERNAL_ERR);
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////


WSDynamicHttpResp::WSDynamicHttpResp() {
}

WSDynamicHttpResp::~WSDynamicHttpResp() {
}

int WSDynamicHttpResp::HandleHttpReq() {

	return 0;
}



}

