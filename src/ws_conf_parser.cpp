/*
 * ws_conf_parser.cpp
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#include <iostream>
#include "ws_conf_parser.h"

namespace ws {


int WSConfParser::OpenConf(const string &conf_path) {
	fp_ = NULL;
	if ( conf_path.empty() ) {
		std::cerr << "conf path is null!" << std::endl;
		return -1;
	}

	FILE* fp = fopen(conf_path, "r");
	if ( fp == NULL ) {
		std::cerr << "conf path(" << conf_path.c_str() << ") is null!" << std::endl;
		return -1;
	}

	fp_ = fp;

	return 0;
}


int WSConfParser::ReadKeyValue(ws_key_value_t &key_value) {
	if ( fp_ == NULL ) {
		std::cerr << "invalid fp_!" << std::endl;
		return -1;
	}

	char buffer[1024] = { 0 };

	while ( fgets(buffer, sizeof(buffer), fp_) ) {
		size_t read_len = strlen(buffer);
		if ( read_len < 2 ) {
			continue;
		}

		if ( buffer[read_len-1] == '\n' ) {
			buffer[read_len-1] = '\0';
		}

		char *line = buffer;
		line = parser_.RemoveComments(line);
		line = parser_.RemoveHeadSpaces(line);
		line = parser_.RemoveTailSpaces(line);

		// now it should be a valid key,value item
		// line: key = value
		char *equal = strchr(line, '=');
		if ( equal == NULL ) {
			continue;
		}

		*equal = '\0';
		++equal;

		key_value.key = parser_.RemoveTailSpaces(line);
		key_value.key = parser_.RemoveHeadSpaces(equal);

		return 0;
	}

	return -1;
}

int WSConfParser::ParseInt (const char *val, int  &int_val) {
	return parser_.ParseInt(val, int_val);
}

int WSConfParser::ParseLong(const char *val, long &long_val) {
	return parser_.ParseLong(val, long_val);
}

int WSConfParser::ParseBool(const char *val, bool &bool_val) {
	return parser_.ParseBool(val, bool_val);
}


char* WSConfParser::removeComments(const char *line) {
	char *comment = strrchr(line, '#');
	if ( comment != NULL ) {
		*comment = '\0';
	}
	return line;
}



int WSConfParser::CloseConf() {
	int ret = 0;
	if ( fp_ ) {
		ret = fclose(fp_);
		fp_ = NULL;
	}
	return ret;
}


WSConfParser::~WSConfParser() {
	CloseConf();
}

}

