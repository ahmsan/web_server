/*
 * ws_file_itil.cpp
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ws_file_itil.h"

namespace ws {


int  WSFileItil::GetFileLen(const string &full_path) {
	struct stat file_stat;

	int ret = stat(full_path.c_str(), &file_stat);
	if ( ret == 0 )
	{
		return file_stat.st_size;
	}

	return errno == ENOENT ? WS_FILE_OPE_NOT_EXIST :
			WS_FILE_OPE_FAILED;
}


bool WSFileItil::IsFileExist(const string &full_path) {
	return access(full_path.c_str(), F_OK) == 0;
}


bool WSFileItil::IsDirExist(const string &full_path) {
	if ( full_path.empty() ) {
		return false;
	}
	return opendir(full_path.c_str()) != NULL;
}

}

