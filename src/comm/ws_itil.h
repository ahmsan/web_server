/*
 * itil.h
 *
 *  Created on: 2016Äê6ÔÂ26ÈÕ
 *      Author: mingsanwang
 */


#include <time.h>
#include <string>
#include <vector>

using std::string;
using std::vector;


namespace ws {

	int 	daemonize();
	int 	create_pipe(int pipe_fd[2], int flags);
	int 	get_cpu_cores_cnt();
	bool	is_valid_ip(const char *ip);

	void 	fast_split_in_place(char *mutable_str, char dilimitor,
				vector<char *>& tokens);

	string	get_date_time_str(const time_t &my_time);
	string 	get_cur_date_time_str();

	string	get_default_conf_file_path();
	string	get_default_dir(const string &folder);

	int		set_max_fd_limit(int max_fd);
}
