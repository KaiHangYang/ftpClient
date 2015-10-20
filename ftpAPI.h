#ifndef FTPAPI_H_INCLUDED
#define FTPAPI_H_INCLUDED
#include <winsock2.h>
#include <iostream>
#include <stdio.h>


/*
    ��������socket_connect
    ���ã�����һ�����ӷ�������socket ���ҷ���
    ������IP�����������˿ں�(Ĭ��21)
    ����ֵ��Socket�׽���

*/
SOCKET socket_connect(char * host, int port = 21);
/*
    ��������socket_clean
    ���ã��ر�WSAģ��
    ������socket�׽���
    ����ֵ��void
*/
void socket_clean(SOCKET sock);
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
    ����ֵ��0��ʾ�ɹ���������ʾʧ��
*/
int ftp_cmd_send(SOCKET sock, char * cmd, char *rec_buf, ssize_t *len);
/*
    ��������ftp_cmd_send
    ���ã���������һ�ȡ���ص���Ϣ
    ������������FTP��socket������
    ����ֵ��ftp״̬��
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
    ��������ftp_list (��Ҫ������������)
    ���ã���ʾ��ǰ�ļ��е���Ϣ
    ������socket�׽��ӣ���ʾ·��
    ����ֵ��0��ʾ�ɹ���������ʾʧ��
*/
int ftp_list(SOCKET sock, char *dir);
/*
    ��������ftp_type
    ���ã�����FTP������������� ascii������Binary(A(ASCII), I(BINARY), E(EBCDIC))
    ������socket�׽��ӣ�ģʽ����
    ����ֵ��0��ʾ�ɹ���������ʾʧ��s

*/
int ftp_type(SOCKET sock, char type);

/*
    ��������ftp_data_pasv
    ���ã�ͨ������ģʽ ����FTP��������
    ����������socket�׽���
    ���أ�����socket�׽���
*/
SOCKET ftp_data_pasv(SOCKET instr_sock);
#endif // FTPAPI_H_INCLUDED
