#ifndef FTPAPI_H_INCLUDED
#define FTPAPI_H_INCLUDED
#include <winsock2.h>
#include <iostream>
#include <stdio.h>

/*
    函数名：getRandomPort
    作用：用来产生随机的端口
*/
int getRandomPort();


/*
    函数名：errorInfoStrip
    作用：将错误信息全部导出晚。
    参数：sock
    返回值：无
*/
void errorInfoStrip(SOCKET sock);


/*
    函数名：init_wsa
    作用：初始化
    参数：null
    返回值：0成功 如果没有成功创建的话返回-1

*/
int init_wsa();
/*
    函数名：socket_connect
    作用：创建一个连接服务器的socket 并且返回
    参数：IP或者域名，端口号(默认21)
    返回值：Socket套接字 如果没有成功创建的话返回-1

*/
SOCKET socket_connect(char * host, int port = 21);
/*
    函数名：socket_clean
    作用：关闭WSA模块
    参数：socket套接字
    返回值：void
*/
void socket_clean();
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
    返回值：-1表示失败
*/
int ftp_cmd_send(SOCKET sock, char * cmd, char *rec_buf, ssize_t *len);
/*
    函数名：ftp_cmd_send
    作用：传输命令并且获取返回的信息
    参数：已连接FTP的socket，命令
    返回值：ftp状态码 失败的话返回-1
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
    函数名：ftp_list (调用数据连接)
    作用：显示当前文件夹的信息
    参数：socket套接子，显示路径
    返回值：0表示成功，其他表示失败
*/
int ftp_list(SOCKET sock, char *dir, char **data, int *len, int isPort = 0);
/*
    函数名：ftp_type
    作用：设置FTP传输的数据类型 ascii或者是Binary(A(ASCII), I(BINARY), E(EBCDIC))
    参数：socket套接子，模式类型
    返回值：0表示成功，-1其他表示创建数据连接(pasv等)失败，>0表示其他的错误代码

*/
int ftp_type(SOCKET sock, char type);

/*
    函数名：ftp_data_pasv
    作用：通过被动模式 创建FTP数据链接，被动模式过程是，客户端主动发送pasv命令通知服务器自己处于被动模式，然后服务器传一个非特权端口给客户端，然后客户端就可以通过这个端口发送数据
    参数：命令socket套接子
    返回：数据socket套接子 失败返回-1
*/
SOCKET ftp_data_pasv(SOCKET instr_sock);

/*
    函数名：ftp_data_port
    作用：通过主动模式连接服务器
    参数：命令socket套接子
    返回：数据socket套接子 失败返回-1
*/
SOCKET ftp_data_port(SOCKET instr_sock);

/*
    函数名：ftp_file_download (调用数据连接)
    作用：从服务器上下载文件
    参数：命令套接字，目标文件名称，本地名称（路径），文件大小
    返回：0表示成功, >0表示其他错误相应码，-1表示创建数据连接（pasv等）失败，-2表示本地文件创建失败
*/
int ftp_file_download(SOCKET sock, char *server_file, char *local_file, int *file_size, int isPort = 0);

/*
    函数名：ftp_file_upload (调用数据连接)
    作用：向服务器上传文件
    参数：命令套接字，服务器文件名，本地文件名，文件大小
    返回：0表示成功, >0表示其他错误相应码，-1表示创建数据连接（pasv等）失败，-2表示本地文件打开失败或者上传失败
*/
int ftp_file_upload(SOCKET sock, char *server_file, char *local_file, int *file_size, int isPort = 0);

/*
    函数名：ftp_file_del
    作用：删除文件
    参数：命令套接字，服务器文件名称
    返回：0表示成功，其他表示不成功
*/
int ftp_file_del(SOCKET sock, char *file);

/*
    函数名：ftp_quit
    作用：退出FTP服务器
    参数：命令套接子
    返回：0表示成功，其他表示失败
*/
int ftp_quit(SOCKET sock);
#endif // FTPAPI_H_INCLUDED
