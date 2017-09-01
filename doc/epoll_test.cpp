/*
 * epoll_test.cpp
 *
 *  Created on: 2016��7��1��
 *      Author: ahmsan
 */


void epoll_test()
{
	// step 1: epoll_create(int maxfds) ����epoll���õ�һ��fd
	// step 2: epoll_ctl ��Ӽ����¼�
	// step 3: epoll_wait �����¼������ر������¼��ĸ���
	// step 4: close �ر� epoll_create���ص�fd������fd������fd��Դ�ĺľ�

	// ET & LT
	// ETģʽ����Ե��������״̬�����仯��ʱ��Ż��֪ͨ��������ν��״̬�ı仯���������������л���δ���������
	// Ҳ����˵�����Ҫ����ETģʽ����Ҫһֱread/write֪������Ϊֹ
	// ��LTģʽ��ֻҪ��������û�д���ͻ�һֱ֪ͨ��ȥ��

	// ʹ��epoll, ֻ��Ҫ���� #include <sys/epoll.h>

	// this is a module

	int listen_fd = 2; // ����fd

	int maxfds = 100;
	int epoll_fd = epoll_create(100);

	struct epoll_event event;
	event.data.fd = listen_fd;
	event.events = EPOLLIN | EPOLLET; // ����watched_fd�Ķ��¼�����Ե����

	// ע���¼�
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);

	struct epoll_event events[100];
	for ( ; ; ) {
		// �ȴ�ֱ�����¼��ķ���
		int nfds = epoll_wait(epoll_fd, events, maxfds, -1);
		for ( int i = 0; i < nfds; ++i ) {
			if ( events[i].data.fd == listen_fd ) {
				int conn_fd = accept();
				struct epoll_event ev;
				ev.data.fd = conn_fd;
				ev.events = EPOLLIN | EPOLLET;
				// ע��
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
