/*
 * ws_server.h
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#ifndef WS_SERVER_H_
#define WS_SERVER_H_


#include "ws_comm.h"
#include "ws_thread.h"
#include "ws_connection.h"

namespace ws {

class WSMain: private UnCopyable {
public:
	WSMain();

	int  Init(const string &conf_path);
	int  Run();

private:
	int  initLog();
	int  initDefaultConf();
	int  parseConf(const string &conf_path);

	int  initMainSocket();
	int  initWorkerThreads();

	void schedule(int conn_fd);
	void shutdown();

private:
	WSSocket		ws_main_socket_;
	ws_config_t		ws_config_t_;
	vector<WSConnection> conns_vec_;
	vector<WSWorkThread> work_thread_vec_;
};

}



#endif /* WS_SERVER_H_ */
