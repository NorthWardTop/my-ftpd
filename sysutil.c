/*
 * @Author: northward
 * @Github: https://github.com/northwardtop
 * @Date: 2019-07-20 22:47:47
 * @LastEditors: northward
 * @LastEditTime: 2019-07-23 00:34:46
 * @Description: 对sysutil的实现
 */

#include "common.h"
#include "sysutil.h"

/**
 * @description: 用于启动tcp服务器
 * @param {ip/主机名称,端口号} 
 * @return: 成功返回套接字, 
 */
int tcp_server(const char *host, unsigned short port)
{
	int listenfd;
	int ret = -1;

	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
		ERR_EXIT("tcp server create failed!");
	
	struct sockaddr_in srvaddr;
	memset(&srvaddr, 0, sizeof(srvaddr));
	srvaddr.sin_family = AF_INET;
	if (host != NULL) {
		//点分十进制转化为sin_addr, 返回0则是无效地址(域名),使用hostbyname
		if (inet_aton(host, &srvaddr.sin_addr) == 0) { 
			struct hostent *hp;
			hp = gethostbyname(host);
			if (hp == NULL)
				ERR_EXIT("get host by name failed!");
			srvaddr.sin_addr = *(struct in_addr*)hp->h_addr;
		}
	} else {
		srvaddr.sin_addr.s_addr = htonl(INADDR_ANY); //host为空则默认所有地址
	}
	srvaddr.sin_port = htons(port);

	//设置socket重复利用
	int on = 1;
	ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, \
		(const char*)&on, sizeof(on));
	if (ret < 0) 
		ERR_EXIT("set reuse address failed!");
	ret = bind(listenfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr));
	if (ret < 0) 
		ERR_EXIT("bind address failed!");
	ret = listen(listenfd, SOMAXCONN);
	if (ret < 0) 
		ERR_EXIT("listen failed!");
	
	return listenfd;
}


int tcp_client(unsigned short port)
{
	return 0;
}




int getlocalip(char *ip);

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);

int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);

/**
 * @description: 
 * @param {type} 
 * @return: 
 */
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	//设定超时时间
	if (wait_seconds > 0) {
		fd_set accfd_set;
		struct timeval timeout;
		FD_ZERO(&accfd_set);
		FD_SET(fd, &accfd_set);
		do {
			ret = select(fd + 1, &accfd_set, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR); //发生系统调用中断
		if (ret == -1)
			return -1;
		else if (ret == 0) {
			errno = ETIMEDOUT;
			return -1;
		}
	}
	//不设定超时默认直接使用accept
	if (addr != NULL)
		ret = accept(fd, (struct sockaddr*)addr, &addrlen);
	else 
		ret = accept(fd, NULL, NULL);
	return ret;
}


int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);

//读取n字节
ssize_t readn(int fd, void *buf, size_t count)
{
	size_t nleft = count;
	ssize_t nread;
	char *bufp = (char*)buf;

	while (nleft > 0) {
		if ((nread = read(fd, bufp, nleft)) < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		} else if (nread == 0)
			return count - nleft;
		bufp +=nread;
		nleft -= nread;
	}
	return count;
}


/**
 * @description: 
 * @param {type} 
 * @return: 如果出现中断还可以继续写入的写函数
 */
ssize_t writen(int fd, const void *buf, size_t count)
{
	size_t nleft = count;
	char *bufp = (char*)buf;
	size_t written = 0;

	while (nleft > 0) {
		written = write(fd, bufp, nleft);
		if (written < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		} else if (written == 0)
			continue;
		bufp += written;
		nleft -= written;
	}
	return count;
}

//读取数据但端口不移除,下次依然可以读取
ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
	while (1) {
		int ret = recv(sockfd, buf, len, MSG_PEEK);
		if (ret < 0 && errno == EINTR)
			continue;
		return ret;
	}
}

/**
 * @description: 按行读取数据
 * @param {套接字,接受缓冲区,一行最大长度} 
 * @return: 失败返回-1
 */
ssize_t readline(int sockfd, void *buf, size_t maxline)
{
	int ret, nread, nleft = maxline;
	char *bufp = buf;
	while (1)
	{
		//读取数据但端口不移除,下次依然可以读取
		ret = recv_peek(sockfd, bufp, nleft);
		if (ret <= 0)
			return ret;
		
		nread = ret;
		int i;
		for (i = 0; i < nread; ++i) {
			if (bufp[i] == '\n') {
				ret = readn(sockfd, bufp, i+1); //真正的读取
				if (ret != i+1)
					ERR_EXIT("read failed for readline!");
				return ret;
			}
		}
		if (nread > nleft)
			ERR_EXIT("out of read range!");
		nleft -= nread;
		ret = readn(sockfd, bufp, nread);
		if (ret != nread)
			ERR_EXIT("read failed!");
		bufp += nread;
	}
	return -1;
}

void send_fd(int sock_fd, int fd);
int recv_fd(const int sock_fd);

const char* statbuf_get_perms(struct stat *sbuf);
const char* statbuf_get_date(struct stat *sbuf);

int lock_file_read(int fd);
int lock_file_write(int fd);
int unlock_file(int fd);

long get_time_sec(void);
long get_time_usec(void);
void nano_sleep(double seconds);

void activate_oobinline(int fd);
void activate_sigurg(int fd);