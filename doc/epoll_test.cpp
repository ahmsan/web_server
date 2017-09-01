/*
 * epoll_test.cpp
 *
 *  Created on: 2016年7月1日
 *      Author: ahmsan
 */


void epoll_test()
{
	// step 1: epoll_create(int maxfds) 创建epoll，得到一个fd
	// step 2: epoll_ctl 添加监听事件
	// step 3: epoll_wait 监听事件，返回被触发事件的个数
	// step 4: close 关闭 epoll_create返回的fd，回收fd，避免fd资源的耗尽

	// ET & LT
	// ET模式（边缘触发）当状态发生变化的时候才获得通知，这里所谓的状态的变化并不包括缓冲区中还有未处理的数据
	// 也就是说，如果要采用ET模式，需要一直read/write知道出错为止
	// 而LT模式是只要是有数据没有处理就会一直通知下去的

	// 使用epoll, 只需要包括 #include <sys/epoll.h>

	// this is a module

	int listen_fd = 2; // 监听fd

	int maxfds = 100;
	int epoll_fd = epoll_create(100);

	struct epoll_event event;
	event.data.fd = listen_fd;
	event.events = EPOLLIN | EPOLLET; // 监听watched_fd的读事件，边缘触发

	// 注册事件
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);

	struct epoll_event events[100];
	for ( ; ; ) {
		// 等待直到有事件的反生
		int nfds = epoll_wait(epoll_fd, events, maxfds, -1);
		for ( int i = 0; i < nfds; ++i ) {
			if ( events[i].data.fd == listen_fd ) {
				int conn_fd = accept();
				struct epoll_event ev;
				ev.data.fd = conn_fd;
				ev.events = EPOLLIN | EPOLLET;
				// 注册
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev);
			} else if ( events[i].events & EPOLLIN ) {
				// 1. read
				// 2. epoll_ctl EPOLL_CTL_MOD
			} else if ( events[i].events & EPOLLOUT ) {
				// 1. write
				// 2. epoll_ctl EPOLL_CTL_MOD
			}
		}
	}
}


int main()
{
	epoll_test();
	return 0;
}
