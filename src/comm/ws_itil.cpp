/*
 * itil.cpp
 *
 *  Created on: 2016Äê6ÔÂ26ÈÕ
 *      Author: mingsanwang
 */

#include "ws_itil.h"

namespace ws {


static string get_proj_dir() {
	string path;
	ssize_t path_len;
	char buffer[PATH_LEN_MAX];

	path_len = readlink("/proc/self/exe", buffer, PATH_LEN_MAX);
	if ( path_len >= 0 || path_len < PATH_LEN_MAX) {
		buffer[path_len] = '\0';
	}

	char *path_tmp = strrchr(buffer, '/');
	if ( path_tmp ) {
		*path_tmp = '\0';
		path_tmp = strrchr(buffer, '/');
		if ( path_tmp )
		{
			*path_tmp = '\0';
			path = buffer;
		}
	}
	return path;
}


int daemonize() {
	pid_t pid = fork();
	if ( pid > 0 ) {
		exit(0);	// terminate father process
	} else if ( pid < 0 ) {
		exit(1);	// fork failed
	}

	setsid();
	pid = fork();
	if ( pid > 0 ) {
		exit(0);
	} else if ( pid < 0 ) {
		exit(1);
	}

	for ( int i = 0; i < 3; ++i ) {
		close(i);
	}

	chdir("/tmp");
	umask(0);

	return 0;
}


int create_pipe(int pipe_fd[2], int flags) {
   int ret = pipe(pipe_fd);
   if ( ret < 0 ) {
      return -1;
   }

   if ( fcntl(pipe_fd[0], F_SETFL, flags) < 0 ||
		fcntl(pipe_fd[1], F_SETFL, flags) < 0 ) {
      close(pipe_fd[0]);
      close(pipe_fd[1]);
      return -1;
   }

   return 0;
}


int get_cpu_cores_cnt() {
    long cpu_cores_cnt = sysconf(_SC_NPROCESSORS_ONLN);
    if ( cpu_cores_cnt < 0 ) {
        return 1;
    }
    return (int)cpu_cores_cnt;
}


bool is_valid_ip(const char *ip) {
	int a, b, c, d;
	a = b = c = d = 0;

	if ( !ip ) {
		return false;
	}

	sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d);

#define BYTE_RANGE(x) \
	if ( (x) < 0 || (x) > 0x000000FF ) {\
		return false;\
	}

	BYTE_RANGE(a);
	BYTE_RANGE(b);
	BYTE_RANGE(c);
	BYTE_RANGE(d);

	return true;
}


void fast_split_in_place(char *mutable_str, char dilimitor, vector<char *>& tokens) {
    char *begin = mutable_str;
    char *dilimitor_pos	= NULL;

    while ( *begin == dilimitor ) {
    	begin++;
    }

    tokens.clear();
    while ( (dilimitor_pos = strchr( begin, dilimitor )) != NULL
    		&& *begin != '\0' ) {
    	*dilimitor_pos = '\0';
    	tokens.push_back(begin);

        begin = dilimitor_pos + 1;
        while ( *begin == dilimitor ) {
            begin++;
        }
    }

    if ( *begin != '\0' ) {
    	tokens.push_back(begin);
    }
}



string get_default_conf_file_path() {
	string &path = get_proj_dir();
	if ( !path.empty() ) {
		path.append("/conf/ws_server.conf");
	}
	return path;
}

string	get_default_dir(const string &folder) {
	string &path = get_proj_dir();
	if ( !path.empty() ) {
		path.append("/");
		path.append(folder);
	}
	return path;
}


string get_date_time_str(const time_t &my_time) {
	char buffer[64];
	struct tm curr = *localtime(mytime);
	sprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
			curr.tm_year + 2000, curr.tm_mon + 1, curr.tm_mday,
			curr.tm_hour, curr.tm_min, curr.tm_sec);
	return buffer;
}


string get_cur_date_time_str() {
	time_t now = time(NULL);
	return GetDateTimeStr(now);
}


int set_max_fd_limit(int max_fd) {
    struct rlimit rlmt;

    getrlimit(RLIMIT_NOFILE, &rlmt);
    rlmt.rlim_max = rlmt.rlim_cur = max_fd;
    if ( setrlimit(RLIMIT_NOFILE, &rlmt)) {
        return -1;
    }
    return 0;
}

}

