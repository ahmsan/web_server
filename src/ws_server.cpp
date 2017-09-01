/*
 * ws_server.cpp
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#include "ws_server.h"
#include "singleton.h"
#include "ws_socket.h"
#include "ws_http_response.h"

#include <iostream>
using namespace std;

namespace ws {


WSMain::WSMain() {
	initDefaultConf();
}


int WSMain::initDefaultConf() {
	conns_vec_.clear();
	work_thread_vec_.clear();

	ws_config_t_.listen_ip 			= "*";
	ws_config_t_.listen_port		= 8080;
	ws_config_t_.listen_backlog 	= 100;
	ws_config_t_.log_level			= 0;
	ws_config_t_.work_thread_cnt	= 0;

	ws_config_t_.max_fd				= 1 << 13;
	ws_config_t_.docroot 			= ws::get_default_dir("docroot");
	ws_config_t_.cgi_bin 			= ws::get_default_dir("cgi");
	ws_config_t_.log_file			= ws::get_default_dir("log") + "/" + "ws_server.log";

	return 0;
}


int WSMain::parseConf(const string &conf_path) {
	if ( conf_path.empty() || FileItil::IsFileExist(conf_path) ) {
		std::cerr << "conf file is empty or path " << conf_path.c_str()
				<< " doesn't exist!" << std::endl;
		return -1;
	}

	WSConfParser conf_parser;
	int ret = conf_parser.OpenConf(conf_path);
	if ( ret != 0 ) {
		std::cerr << "open conf file failed!" << std::endl;
		return -1;
	}

	ws_key_value_t key_value;
	while ( conf_parser.ReadKeyValue(key_value) == 0 ) {
		const char *key = key_value.key.c_str();
		const char *val = key_value.value.c_str();

		if ( strcmp(key, "listen_ip") == 0 ) {
			if ( ws::is_valid_ip(val) ) {
				ws_config_t_.listen_ip = val;
			}
		} else if ( strcmp(key, "listen_port") == 0 ) {
			int int_val = 0;
			ret = conf_parser.ParseInt(val, int_val);
			if ( ret == 0 && int_val > 0 && int_val < 0x000FFFF ) {
				ws_config_t_.listen_port = (uint16_t)int_val;
			}
		} else if ( strcmp(key, "keep_alive_timeout") == 0 ) {
			int int_val = 0;
			ret = conf_parser.ParseInt(val, int_val);
			if ( ret == 0 && int_val > 0 && int_val < 0x000FFFF ) {
				ws_config_t_.keep_alive_timeout = (uint16_t)int_val;
			}
		} else if ( strcmp(key, "work_thread_cnt") == 0 ) {
			int int_val = 0;
			ret = conf_parser.ParseInt(val, int_val);
			if ( ret == 0 && int_val > 0 && int_val < 0x000FFFF ) {
				ws_config_t_.work_thread_cnt = (uint16_t)int_val;
			}
			if ( ws_config_t_.work_thread_cnt == 0 ) {
				ws_config_t_.work_thread_cnt = ws::get_cpu_cores_cnt();
			}
		} else if ( strcmp(key, "docroot") == 0 ) {
			if ( WSFileItil::IsDirExist(val) ) {
				ws_config_t_.docroot = val;
			}
		} else if ( strcmp(key, "log_file") == 0 ) {
			if ( WSFileItil::IsFileExist(val) ) {
				ws_config_t_.log_file = val;
			}
		} else if ( strcmp(key, "max_fd") == 0 ) {
			int int_val = 0;
			ret = conf_parser.ParseInt(val, int_val);
			if ( ret == 0 && int_val > 0 && int_val <= 0x000FFFF ) {
				ws_config_t_.max_fd = (uint16_t)int_val;
			}
		} else if ( strcmp(key, "cgi_bin") == 0 ) {
			if ( WSFileItil::IsDirExist(val) ) {
				ws_config_t_.cgi_bin = val;
			}
		}
	}

	conf_parser.CloseConf();

	return 0;
}


int WSMain::initLog(){
	Singleton<WSLog>::Instance()->EnableLogger(ws_config_t_.log_level != 0);
	if ( ws_config_t_.log_file.empty() ) {
		Singleton<WSLog>::Instance()->InitLogger(ws_config_t_.log_level);
	} else {
		Singleton<WSLog>::Instance()->InitLogger(ws_config_t_.log_level, ws_config_t_.log_file);
	}
	return 0;
}


int WSMain::initWorkerThreads() {
    WSBarrier barrier;
    uint16_t work_thread_cnt = ws_config_t_.work_thread_cnt;
    barrier.InitBarrier(work_thread_cnt + 1);

    LOG_INFO("prepare to create threads");

    work_thread_vec_.clear();
    int max_socket_fd_num = ws_config_t_.max_fd / ws_config_t_.work_thread_cnt;
    for ( uint16_t i = 0; i < work_thread_cnt; ++i ) {
    	WSWorkThread work_thread(max_socket_fd_num, conns_vec_,
    			ws_config_t_.max_fd, barrier);
    	if ( work_thread.Start() ) {
    		work_thread_vec_.push_back(work_thread);
    	}
    }

    barrier.WaitBarrier();
    barrier.DestoryBarrier();

    LOG_INFO("create threads done!");

	return 0;
}

int WSMain::initMainSocket() {
	int ret = ws_main_socket_.InitSocket(ws_config_t_.listen_ip,
			ws_config_t_.listen_port);
	if ( ret != 0 ) {
		LOG_ERR("init main socket failed, err_msg: %s",
				ws_main_socket_.GetLastErrMsg().c_str());
	}
	return ret;
}


int WSMain::Init(const string &conf_path) {
	int ret = parseConf(conf_path);
	if ( ret != 0 ) {
		std::cerr << "parse conf failed!" << std::endl;
	}

	InitLog();

	// from here, we can use logger
	ws::daemonize();
	ret = ws::set_max_fd_limit(ws_config_t_.max_fd);
	if ( ret != 0 ) {
		LOG_ERR("set max fd limit failed!");
		return -1;
	}

	// set dir info
	WSHttpResp::SetResourceDirInfo(ws_config_t_.docroot,
			ws_config_t_.cgi_bin);

	for ( int i = 0; i < max_fd; ++i ) {
		WSConnection conn(i, ws_config_t_.keep_alive_timeout);
		conns_vec_.push_back(conn);
	}

	ret = initWorkerThreads();
	if ( ret != 0 ) {
		LOG_ERR("init work thread failed!");
		return -1;
	}

	return 0;
}

// main thread: just accept conn and dispatch it to work thread
int WSMain::Run() {
	WSTcpSocket socket;
	if ( socket.Create(ws_config_t_.listen_ip,
			ws_config_t_.listen_port) != 0 ) {
		LOG_ERR("%s", socket.GetLastErrMsg().c_str());
	}
	if ( socket.Bind() != 0 ) {
		LOG_ERR("%s", socket.GetLastErrMsg().c_str());
	}

	if ( socket.Listen(ws_config_t_.listen_backlog) != 0 ) {
		LOG_ERR("%s", socket.GetLastErrMsg().c_str());
	}

	while ( true ) {
		sockaddr_in client_addr;
		memset(&client_addr, 0, sizeof(client_addr));
		int conn_fd = socket.Accept(client_addr, SOCK_NONBLOCK | SOCK_CLOEXEC);
		if ( conn_fd < 0 ) {
			LOG_ERR("%s", socket.GetLastErrMsg().c_str());

			if ( errno == EBADF || errno == ECONNABORTED ) {
				LOG_ERR("error happened! err: %d", errno);
				socket.Close();
				return -1;
			}
		} else {
			schedule(conn_fd);
		}
	}

	socket.Close();
	shutdown();

	return 0;
}

// use simple round-robin to choose work-thread to handle it
void WSMain::schedule(int conn_fd) {
	 static int work_thread_idx = 0;

	 WSWorkThread &thread = work_thread_vec_[work_thread_idx];

	 // notify work thread to handle this connection
	 if ( write(thread.GetWritePipeFd(), &conn_fd, sizeof(int)) < 0 ) {
		 LOG_ERR("write conn_fd(%d) to work-thread(%d) failed!",
				 conn_fd, work_thread_idx);
	 }

	 work_thread_idx = work_thread_idx++ % work_thread_vec_.size();
}

void WSMain::shutdown() {
	if ( conns_ptr_ ) {
		delete [] conns_ptr_;
		conns_ptr = NULL;
	}

	vector<WSWorkThread>::iterator iter;
	for ( iter = work_thread_vec_.begin();
			iter != work_thread_vec_.end(); ++iter ) {
		iter->ShutDown();
	}
}

}

