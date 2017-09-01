/*
 * ws_conf_parser.h
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#ifndef WS_CONF_PARSER_H_
#define WS_CONF_PARSER_H_

#include "ws_comm.h"
#include "ws_parser.h"

#include <string>
using std::string;

#include "uncopyable.h"

namespace ws {


// parse 'key = value' conf file
class WSConfParser: private UnCopyable {
public:
	WSConfParser(): fp_(NULL) {
	}
	~WSConfParser();

	int OpenConf(const string &conf_path);
	int CloseConf();

	int ReadKeyValue(ws_key_value_t &key_value);

	int ParseInt (const char *val, int  &int_val);
	int ParseLong(const char *val, long &long_val);
	int ParseBool(const char *val, bool &bool_val);

private:
	char* removeComments(const char *line);

private:
	FILE*	 fp_;
	WSParser parser_;
};

}



#endif /* WS_CONF_PARSER_H_ */
