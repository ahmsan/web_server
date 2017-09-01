/*
 * ws_file_itil.h
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#ifndef WS_FILE_ITIL_H_
#define WS_FILE_ITIL_H_

#include <stdio.h>

#include <string>
using std::string;

namespace ws {

enum {
	WS_FILE_OPE_OK			= 0,
	WS_FILE_OPE_FAILED		= -1,
	WS_FILE_OPE_NOT_EXIST	= -2,
};



class RAIIFpGuard {
public:
    explicit RAIIFpGuard(FILE *fp): fp_(fp) {
    }

    ~RAIIFpGuard() {
        if ( fp_ >= 0 ) {
            fclose(fp_);
        }
    }

private:
    FILE *fp_;
};


class WSFileItil {
public:
	static int   GetFileLen(const string &full_path);
	static bool  IsFileExist(const string &full_path);
	static bool  IsDirExist(const string &full_path);
};

}


#endif /* WS_FILE_ITIL_H_ */
