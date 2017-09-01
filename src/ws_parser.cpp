/*
 * ws_parser.cpp
 *
 *  Created on: 2016Äê6ÔÂ30ÈÕ
 *      Author: ahmsan
 */

namespace ws {


int WSParser::ParseInt (const char *val, int  &int_val) {
	int_val = 0;
    long long_val = 0;

    if ( val == NULL ) {
    	return -1;
    }

    int ret = ParseLong(val, long_val);
    if ( ret != 0 ) {
    	return -2;
    }

    if ((long)(int)long_val != long_val) {
    	return -3;
    }

    int_val = (int)long_val;

    return 0;
}

int WSParser::ParseLong(const char *val, long &long_val) {
	long  value;
	char* endptr = NULL;

	if ( val == NULL ) {
		return -1;
	}

	errno = 0;
	value = strtol(value, &endptr, 0);

	if ( errno != 0 ) {
		return -1;
	}

	if ( *end != '\0' || value == endptr ) {
		return -2;
	}

	long_val = value;

	return 0;
}

int WSParser::ParseBool(const char *val, bool &bool_val) {
	bool_val = false;

	if ( val == NULL ) {
		return -1;
	}

    if ( !strcmp(val, "true")
    	|| !strcmp(val, "on")
    	|| !strcmp(val, "yes")
    	|| !strcmp(val, "1")) {
    	bool_val = true;
    }

    return 0;
}


char* WSParser::RemoveHeadSpaces(const char *line) {
	while ( isspace(*line) ){
		++line;
	}
	return line;
}


char* WSParser::RemoveTailSpaces(const char *line) {
	size_t len = strlen(line);
	for ( size_t i = len - 1; !isspace(line[i]); --i ) {
		line[i] = '\0';
	}
	return line;
}


}
