/*
 * ws_http_request.cpp
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#include "ws_itil.h"
#include "ws_http_request.h"

#include <iostream>

namespace ws {


static const char *CRLF		= "\r\n";
static const char *CRLFCRLF	= "\r\n\r\n";
static const int   CRLFCRLF_LEN = 4;


WSHttpRequest::WSHttpRequest(): keep_alive_(false), content_len_(0) {
	this->start_line_.http_mtd_ = HTTP_METHOD_INVALID;
	this->start_line_.http_ver_	= HTTP_VER_INVALID;
	this->query_map_.clear();
}

int WSHttpRequest::ParseHttpRequest(const char *http, http_status &stat) {
	stat = HTTP_FAILED;
	do {
		if ( !http ) {
			LOG_ERR("invalid param!");
			break;
		} else {
			const char *line_end = strstr(http, CRLF);
			if ( !line_end ) {
				LOG_ERR("invalid http start line!");
				break;
			}
			*line_end = 0;
			line_end += 2;	// for \r\n

			int ret = this->parseHttpStartLine(http);
			if ( ret != 0 ) {
				LOG_ERR("parse start line failed!");
				break;
			} else {
				http = line_end;
				line_end = strstr(http, CRLFCRLF);
				if ( !line_end ) {
					LOG_ERR("invalid http header!");
					break;
				}
				*line_end = 0;
				line_end += 4; // for \r\n\r\n

				ret = this->parseHttpHeader(http);
				if ( ret != 0 ) {
					LOG_ERR("parse http header failed!");
					break;
				} else {
					if ( start_line_.http_mtd_ == HTTP_METHOD_POST &&
							content_len_ > 0 && *line_end != '\0' ) {
						http = line_end;
						http[content_len_] = 0;
						this->parseHttpQueryString(http);
					}
					// if reach here, we can consider parse http request successfully.
					stat = HTTP_OK;
				}
			}
		}
	} while ( 0 );

	return stat == HTTP_OK ? 0 : -1;
}

int WSHttpRequest::ParseHttpRequestBody(const char *str, http_status &stat) {
	// TODO
	return 0;
}

int WSHttpRequest::GetHttpMethod(const char *method) {
	http_method metd = HTTP_METHOD_INVALID;
	if ( strncmp(metd, "GET", 3) == 0 ) {
		start_line_.http_mtd_ = HTTP_METHOD_GET;
	} else if ( strncmp(metd, "POST", 4) == 0 ) {
		start_line_.http_mtd_ = HTTP_METHOD_POST;
	} else if ( strncmp(metd, "HEAD", 4) == 0 ) {
		start_line_.http_mtd_ = HTTP_METHOD_HEAD;
	}
	return metd;
}

bool WSHttpRequest::HasCompleteHeader(const char *str, int len) {
	return strstr(str, CRLFCRLF) != NULL;
}

const char* WSHttpRequest::GetHttpBodyStartPos(const char *str, int len) {
	const char *pos = strstr(str, CRLFCRLF);
	if ( pos != NULL ) {
		pos += CRLFCRLF_LEN;
	}
	return pos;
}

// parse http start line
// <http_method> <path> <http_version>
int WSHttpRequest::parseHttpStartLine(const char *str) {
	vector<char *> tokens;
	ws::fast_split_in_place(str, ' ', tokens);

	if ( tokens.size() != 3 ) {
		LOG_ERR("start line is not valid!");
		return -1;
	}

	// http_method: 	tokens[0];
	// http_req_path: 	tokens[1];
	// http_ver: 		tokens[2];
	const char *method = tokens[0];
	start_line_.http_mtd_ = this->GetHttpMethod(method);
	if ( start_line_.http_mtd_ == HTTP_METHOD_INVALID ) {
		LOG_ERR("parse http method failed!");
		return -1;
	}

	// http_ver: HTTP/1.0 -- HTTP/1.1
	const char *version = tokens[2];
	if ( strcmp(version, "HTTP/1.0") == 0 ) {
		start_line_.http_ver_ = HTTP_VER_1_0;
	} else if ( strcmp(version, "HTTP/1.1") == 0 ) {
		start_line_.http_ver_ = HTTP_VER_1_1;
	} else {
		LOG_ERR("invalid http version: %s", version);
		return -1;
	}

	const char *path = tokens[1];
	if ( start_line_.http_mtd_ == HTTP_METHOD_GET ) {
		// parse query string in the URL
		const char *ques_mark = strchr(path, '?');
		if ( ques_mark != NULL ) {
			*ques_mark = 0;
			this->parseHttpQueryString(ques_mark + 1);
		}
	}

	start_line_.req_path_ = path;
	if ( start_line_.req_path_[start_line_.req_path_.size() - 1] == '/' ) {
		// if path is directory, reponse defalt .html file under it
		start_line_.req_path_.append("index.html");
	}

	return 0;
}


int WSHttpRequest::parseHttpHeader(const char *str) {
	const char *line = strstr(str, CRLF);

	while ( line != NULL ) {
		// read line
		*line = 0;
		line += 2;	// move to next line

		const char *colon = strchr(str, ':');
		if ( colon ) {
			*colon = 0;
			colon = parser_.RemoveHeadSpaces(colon + 1);
			if ( strcmp(str, "Connection") == 0 ) {
				keep_alive_ = false;
				if ( *colon == 'k' ) {
					keep_alive_ = true;
				}
			} else if ( strcmp(str, "Content-Length") == 0 ) {
				int len = 0;
				content_len_ = 0;
				if ( parser_.ParseInt(colon, len) == 0 ) {
					content_len_ = len;
				} else {
					LOG_ERR("parse Content-Length: %s failed!", colon);
					return -1;
				}
			}
		}

		str = line;
		line = strstr(line, CRLF);
	}

	return 0;
}


string WSHttpRequest::decodeURL(string &str) {
	// TODO Need to be done
	return str;
}

int WSHttpRequest::parseHttpQueryString(const char *str) {
	vector<char *> tokens;
	ws::fast_split_in_place(str, '&', tokens);
	if ( tokens.size() == 0 ) {
		tokens.push_back(str);		// only one query param ?
	}

	// handle query param one by one
	query_map_.clear();
	vector<char *>::iterator iter = tokens.begin();
	while ( iter != tokens.end() ) {
		const char *equal = strchr(iter, '=');
		if ( equal ) {
			*equal = 0;
			string key(iter);
			string val(equal+1);
			val = this->decodeURL(val);
			query_map_.insert(std::make_pair<string, string>(key, val));
			LOG_INFO("query param(key: %s, value: %s)", key.c_str(), val.c_str());
		} else {
			LOG_ERR("invalid query param: %s", iter);
		}
	}

	return 0;
}


void WSHttpRequest::PrintParsedInfo() {
	std::cout << "\tThe HTTP Request Info:" << std::endl;
	std::cout << "\tstart line:" << start_line_.http_mtd_ << "\t"
			<< start_line_.req_path_ << "\t" <<
			start_line_.http_ver_ == 0 ? "HTTP/1.0" : "HTTP/1.1" << std::endl;
	std::cout << "\theader: "<< std::endl;
	std::cout << "\t\tkeep-alive(" << keep_alive_ ? "true" : "false" << ")" << std::endl;
	std::cout << "\t\tcontent_len(" << content_len_ << ")" << std::endl;

	if ( query_map_.size() > 0 ) {
		std::cout << "\query string: "<< std::endl;
		map<string, string>::iterator iter;
		for ( iter = query_map_.begin(); iter != query_map_.end(); ++iter ) {
			std::cout << "\t\t" << iter->first << "=" << iter->second << std::endl;
		}
	}
}



}


