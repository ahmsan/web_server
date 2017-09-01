/*
 * ws_main.cpp
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>

#include "ws_comm.h"

using namespace ws;
using namespace std;


static string get_ws_conf_path(int argc, char *argv) {
	char opt;
	string path;

    while ( -1 != ( opt = getopt(argc, argv, "f:")) ) {
        switch ( opt ) {
		case 'f':
			path = optarg;
			break;

		default:
			break;
        }
    }

    // if conf file path is not specified,
    // use the default.
    if ( path.empty() ) {
    	path = ws::get_default_conf_file_path();
    }

	return path;
}


int main(int argc, char *argv) {
	string &conf_path = get_ws_conf_path(argc, argv);

	WebServerMain server;
	int ret = server.Init(conf_path);
	if ( ret != 0 )
	{
		std::cerr << "init web-server failed!" << endl;
		return -1;
	}

	server.Run();

	return 0;
}

