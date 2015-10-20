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
    函数名：socket_clean
    作用：关闭WSA模块
    参数：socket套接字
    返回值：void
*/
void socket_clean(SOCKET sock);
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
int ftp_cmd_send(SOCKET sock, char * cmd, char *rec_buf, ssize_t *len);
/*
    函数名：ftp_cmd_send
    作用：传输命令并且获取返回的信息
    参数：已连接FTP的socket，命令
    返回值：ftp状态码
*/
int ftp_cmd_send(SOCKET sock, char * cmd);
/*
    函数名：ftp_login
    作用：用于FTP登录
    参数：socket套接字，用户名，密码
    返回值：0表示成功，其他表示失败
*/
int ftp_login(SOCKET sock, char *username, char *password);
/*
    函数名ftp_logout
    作用：断开FTP服务连接
    参数：socket套接字
    返回值：0表示成功，其他表示失败
*/
int ftp_logout(SOCKET sock);
/*
    函数名：ftp_cwd
    作用：更改服务器上的工作路径
    参数：socket套接字，工作路径
    返回值：0表示成功，其他表示不成功
*/
int ftp_cwd(SOCKET sock, char *dir);

/*
    函数名：ftp_list (需要建立数据连接)
    作用：显示当前文件夹的信息
    参数：socket套接子，显示路径
    返回值：0表示成功，其他表示失败
*/
int ftp_list(SOCKET sock, char *dir);
/*
    函数名：ftp_type
    作用：设置FTP传输的数据类型 ascii或者是Binary(A(ASCII), I(BINARY), E(EBCDIC))
    参数：socket套接子，模式类型
    返回值：0表示成功，其他表示失败s

*/
int ftp_type(SOCKET sock, char type);

/*
    函数名：ftp_data_pasv
    作用：通过被动模式 创建FTP数据链接
    参数：命令socket套接子
    返回：数据socket套接子
*/
SOCKET ftp_data_pasv(SOCKET instr_sock);
#endif // FTPAPI_H_INCLUDED
