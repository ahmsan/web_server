/*
 * ws_connection.cpp
 *
 *  Created on: 2016Äê7ÔÂ1ÈÕ
 *      Author: ahmsan
 */

#include "ws_comm.h"
#include "ws_connection.h"
#include "ws_http_response.h"

namespace ws {

int WSConnection::keep_alive_timeout_ = 0;


WSConnection::WSConnection(int fd, int keep_alive_timeout):
		fd_(fd), shutdown_fd_(false), read_len_(0), write_len_(0),
		t_last_wakeup_(0), epoll_(NULL) {
	WSConnection::keep_alive_timeout_ = keep_alive_timeout;
}


void WSConnection::Reset() {
	read_len_	= 0;
	write_len_	= 0;
	epoll_(NULL) = NULL;
	t_last_wakeup_ = time(NULL);
}


int  WSConnection::handleHttpReq() {
	static char s_buffer[8192];		// new Resp object on application level buffer,
									// avoid too many dynamic memory operation
	WSHttpResp *resp = NULL;
	if ( http_request_.IsDynamicReq() ) {
		resp = new (s_buffer) WSDynamicHttpResp();
	} else {
		resp = new (s_buffer) WSStaticHttpResp();
	}

	resp->SetHttpConnPtr(this);
	resp->SetHttpReqPtr(&http_request_);

	return resp->HandleHttpReq();
}

void WSConnection::respErrDirectly(http_status stat, bool shutdown) {
	WSHttpResp resp;
	resp.SetHttpConnPtr(this);
	resp.BuildErrResp(stat);
	shutdown_fd_ = shutdown;
}


int WSConnection::HandleRead() {
	int recv_len =
	read(fd_, read_buffer_ + read_len_, sizeof(read_buffer_) - read_len_);
	if ( recv_len <= 0 ) {
		LOG_ERR("read data from socket: %d failed!", fd_);
		this->Close();
		return -1;
	}

	read_len_ += recv_len;
	t_last_wakeup_ = time(NULL);

	if ( read_len_ == sizeof(read_buff_) ) {
		if ( strcmp(read_buffer_ + read_len_ - 2, CRLF) != 0 ) {
			LOG_ERR("buffer overflow!");
			// TODO Too Large
			respErrDirectly(HTTP_STATUS_REQ_TOO_LARGE);
			this->Reset();
			return -1;
		}
	}

	if ( http_request_.HasCompleteHeader(read_buffer_, read_len) ) {
		http_method method;
		http_request_.GetHttpMethod(method);
		if ( method == HTTP_METHOD_INVALID ) {
			LOG_ERR("invalid http method, close it ");
			this->Close();
			// TODO Bad Request
			respErrDirectly(HTTP_STATUS_BAD_REQ);
			return -1;
		}

		const char *header_end = http_request_.GetHttpBodyStartPos(read_buffer_, read_len_);

		// for POST request, need to check http_body if it's valid
		if ( method == HTTP_METHOD_POST ) {
			if ( header_end == NULL ) {
				LOG_ERR("POST request but without http_body!");
				// TODO Bad Request
				respErrDirectly(HTTP_STATUS_BAD_REQ);
				this->Close();
				return -1;
			}
			const char *http_req = strstr(header_end, "\r\n");
			if ( http_req != NULL ) {
				header_end = http_req + 2;	// 2 is the length of "\r\n"
			} else {
				// not a complete http request
				return 0;
			}
		}

		// a complete http request
		// parse it.
		http_status status;
		http_request_.ParseHttpRequest(read_buffer_, status);
		if ( status != HTTP_OK ) {
			LOG_ERR("parse http request failed!");
			// TODO Parse Err
			respErrDirectly(HTTP_STATUS_INTERNAL_ERR);
			this->Close();
			return -1;
		}

		// parse succ, handle this http request
		// and response it back
		this->handleHttpReq();

		const int len = header_end - read_buffer_;
		if ( len < read_len_ ) {
			memmove(read_buffer_, read_buffer_ + len, read_len_ - len);
			read_len_ -= len;
		} else {
			// len === read_len_
			this->Reset();
		}
	}

	return 0;
}


int WSConnection::HandleWrite() {
	if ( write_len_ > 0 ) {

	} else {

	}
	return 0;
}


int WSConnection::WriteToBuffer(const char *data, int data_len) {

	if ( (size_t)data_len > sizeof(write_buff_) ) {
		write_len_ = 0;
		return -1;
	}

	memcpy(write_buffer_, data, data_len);
	write_len_ = data_len;

	return 0;
}


int WSConnection::Close() {
	close(fd_);
	Reset();

	if ( epoll_ ) {
		return epoll_->DelEPollEvent(fd_);
	}
	return 0;
}

}

