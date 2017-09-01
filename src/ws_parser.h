/*
 * ws_parser.h
 *
 *  Created on: 2016Äê6ÔÂ30ÈÕ
 *      Author: ahmsan
 */

#ifndef WS_PARSER_H_
#define WS_PARSER_H_


namespace ws {

class WSParser {
public:
	char* RemoveHeadSpaces(const char *line);
	char* RemoveTailSpaces(const char *line);

	int ParseInt (const char *val, int  &int_val);
	int ParseLong(const char *val, long &long_val);
	int ParseBool(const char *val, bool &bool_val);
};

}


#endif /* WS_PARSER_H_ */
