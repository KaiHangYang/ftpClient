#ifndef FTPAPI_H_INCLUDED
#define FTPAPI_H_INCLUDED
#include <winsock2.h>
#include <iostream>
#include <stdio.h>


/*
    函数名：socket_connect
    作用：创建一个连接服务器的socket 并且返回
    参数：IP或者域名，端口号(默认21)
    返回值：Socket套接字

*/
SOCKET socket_connect(char * host, int port = 21);
/*
    函数名：ftp_connect
    作用：连接FTP服务器
    参数：IP或域名、端口号
    返回：Socket 套接字
*/
SOCKET ftp_connect(char *host, int port = 21);
/*
    函数名：ftp_cmd_send
    作用：传输命令并且获取返回的信息
    参数：已连接FTP的socket，命令，返回的数据buf，返回的数据长度
    返回值：0表示成功，其他表示失败
*/
int ftp_cmd_send(SOCKET socket, char * cmd, char *rec_buf, ssize_t *len);
/*
    函数名：ftp_cmd_send
    作用：传输命令并且获取返回的信息
    参数：已连接FTP的socket，命令
    返回值：ftp状态码
*/
int ftp_cmd_send(SOCKET socket, char * cmd);
/*
    函数名：ftp_login
    作用：用于FTP登录
    参数：socket套接字，用户名，密码
    返回值：0表示成功，其他表示失败
*/

#endif // FTPAPI_H_INCLUDED
