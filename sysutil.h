/*
 * @Author: northward
 * @Github: https://github.com/northwardtop
 * @Date: 2019-07-20 22:47:39
 * @LastEditors: northward
 * @LastEditTime: 2019-07-20 22:52:46
 * @Description: 一些系统工具, 自定义函数封装系统函数
 */
#ifndef _SYS_UTIL_H_
#define _SYS_UTIL_H_

#include "common.h"


int tcp_client(unsigned short port);
int tcp_server(const char *host, unsigned short port);

int getlocalip(char *ip);

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);

int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);

ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t recv_peek(int sockfd, void *buf, size_t len);
ssize_t readline(int sockfd, void *buf, size_t maxline);

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
#endif /* _SYS_UTIL_H_ */

