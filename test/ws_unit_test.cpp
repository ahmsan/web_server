/*
 * ws_unit_test.cpp
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#include <stdio.h>

#include "ws_comm.h"
#include "ws_conf_parser.h"
#include "ws_http_request.h"


namespace ws {

void test_http_request() {
	WSHttpRequest request;
	http_status stat;

	const char *file_name = "http_proto";
	FILE *fp = fopen("r", file_name);
	if ( fp == NULL ) {
		printf("fopen file: %s failed!\n", file_name);
		return;
	}

	char file_content[4096];
	size_t read_len =  fread(file_content, sizeof(file_content), 1, fp);
	if ( read_len <= 0 ) {
		printf("read file: %s failed!\n", file_name);
		return;
	}

	file_content[read_len] = 0;

	int ret = request.ParseHttpRequest(file_content, stat);
	if ( ret != 0 ) {
		printf("parse failed!");
	} else {
		request.PrintParsedInfo();
	}
}


void test_config_parser() {
	WSConfParser parser;
	parser.OpenConf("ws_server.conf");
	ws_key_value_t key_val_pair;
	while ( parser.ReadKeyValue(key_val_pair) == 0 ) {
		printf("key: %s, value: %s\n", key_val_pair.key.c_str(),
				key_val_pair.value.c_str());
	}
}


int main()
{
	test_config_parser();
	test_http_request();

	return 0;
}

}

