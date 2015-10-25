#ifndef FTPAPI_H_INCLUDED
#define FTPAPI_H_INCLUDED
#include <winsock2.h>
#include <iostream>
#include <stdio.h>

/*
    ��������getRandomPort
    ���ã�������������Ķ˿�
*/
int getRandomPort();


/*
    ��������errorInfoStrip
    ���ã���������Ϣȫ��������
    ������sock
    ����ֵ����
*/
void errorInfoStrip(SOCKET sock);


/*
    ��������init_wsa
    ���ã���ʼ��
    ������null
    ����ֵ��0�ɹ� ���û�гɹ������Ļ�����-1

*/
int init_wsa();
/*
    ��������socket_connect
    ���ã�����һ�����ӷ�������socket ���ҷ���
    ������IP�����������˿ں�(Ĭ��21)
    ����ֵ��Socket�׽��� ���û�гɹ������Ļ�����-1

*/
SOCKET socket_connect(char * host, int port = 21);
/*
    ��������socket_clean
    ���ã��ر�WSAģ��
    ������socket�׽���
    ����ֵ��void
*/
void socket_clean();
/*
    ��������ftp_connect
    ���ã�����FTP������
    ������IP���������˿ں�
    ���أ�Socket �׽���
*/
SOCKET ftp_connect(char *host, int port = 21);
/*
    ��������ftp_cmd_send
    ���ã���������һ�ȡ���ص���Ϣ
    ������������FTP��socket��������ص�����buf�����ص����ݳ���
    ����ֵ��-1��ʾʧ��
*/
int ftp_cmd_send(SOCKET sock, char * cmd, char *rec_buf, ssize_t *len);
/*
    ��������ftp_cmd_send
    ���ã���������һ�ȡ���ص���Ϣ
    ������������FTP��socket������
    ����ֵ��ftp״̬�� ʧ�ܵĻ�����-1
*/
int ftp_cmd_send(SOCKET sock, char * cmd);
/*
    ��������ftp_login
    ���ã�����FTP��¼
    ������socket�׽��֣��û���������
    ����ֵ��0��ʾ�ɹ���������ʾʧ��
*/
int ftp_login(SOCKET sock, char *username, char *password);
/*
    ������ftp_logout
    ���ã��Ͽ�FTP��������
    ������socket�׽���
    ����ֵ��0��ʾ�ɹ���������ʾʧ��
*/
int ftp_logout(SOCKET sock);
/*
    ��������ftp_cwd
    ���ã����ķ������ϵĹ���·��
    ������socket�׽��֣�����·��
    ����ֵ��0��ʾ�ɹ���������ʾ���ɹ�
*/
int ftp_cwd(SOCKET sock, char *dir);

/*
    ��������ftp_list (������������)
    ���ã���ʾ��ǰ�ļ��е���Ϣ
    ������socket�׽��ӣ���ʾ·��
    ����ֵ��0��ʾ�ɹ���������ʾʧ��
*/
int ftp_list(SOCKET sock, char *dir, char **data, int *len, int isPort = 0);
/*
    ��������ftp_type
    ���ã�����FTP������������� ascii������Binary(A(ASCII), I(BINARY), E(EBCDIC))
    ������socket�׽��ӣ�ģʽ����
    ����ֵ��0��ʾ�ɹ���-1������ʾ������������(pasv��)ʧ�ܣ�>0��ʾ�����Ĵ������

*/
int ftp_type(SOCKET sock, char type);

/*
    ��������ftp_data_pasv
    ���ã�ͨ������ģʽ ����FTP�������ӣ�����ģʽ�����ǣ��ͻ�����������pasv����֪ͨ�������Լ����ڱ���ģʽ��Ȼ���������һ������Ȩ�˿ڸ��ͻ��ˣ�Ȼ��ͻ��˾Ϳ���ͨ������˿ڷ�������
    ����������socket�׽���
    ���أ�����socket�׽��� ʧ�ܷ���-1
*/
SOCKET ftp_data_pasv(SOCKET instr_sock);

/*
    ��������ftp_data_port
    ���ã�ͨ������ģʽ���ӷ�����
    ����������socket�׽���
    ���أ�����socket�׽��� ʧ�ܷ���-1
*/
SOCKET ftp_data_port(SOCKET instr_sock);

/*
    ��������ftp_file_download (������������)
    ���ã��ӷ������������ļ�
    �����������׽��֣�Ŀ���ļ����ƣ��������ƣ�·�������ļ���С
    ���أ�0��ʾ�ɹ�, >0��ʾ����������Ӧ�룬-1��ʾ�����������ӣ�pasv�ȣ�ʧ�ܣ�-2��ʾ�����ļ�����ʧ��
*/
int ftp_file_download(SOCKET sock, char *server_file, char *local_file, int *file_size, int isPort = 0);

/*
    ��������ftp_file_upload (������������)
    ���ã���������ϴ��ļ�
    �����������׽��֣��������ļ����������ļ������ļ���С
    ���أ�0��ʾ�ɹ�, >0��ʾ����������Ӧ�룬-1��ʾ�����������ӣ�pasv�ȣ�ʧ�ܣ�-2��ʾ�����ļ���ʧ�ܻ����ϴ�ʧ��
*/
int ftp_file_upload(SOCKET sock, char *server_file, char *local_file, int *file_size, int isPort = 0);

/*
    ��������ftp_file_del
    ���ã�ɾ���ļ�
    �����������׽��֣��������ļ�����
    ���أ�0��ʾ�ɹ���������ʾ���ɹ�
*/
int ftp_file_del(SOCKET sock, char *file);

/*
    ��������ftp_quit
    ���ã��˳�FTP������
    �����������׽���
    ���أ�0��ʾ�ɹ���������ʾʧ��
*/
int ftp_quit(SOCKET sock);
#endif // FTPAPI_H_INCLUDED
