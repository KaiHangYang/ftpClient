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
int ftp_cmd_send(SOCKET socket, char * cmd, char *rec_buf, ssize_t *len);
/*
    ��������ftp_cmd_send
    ���ã���������һ�ȡ���ص���Ϣ
    ������������FTP��socket������
    ����ֵ��ftp״̬��
*/
int ftp_cmd_send(SOCKET socket, char * cmd);
/*
    ��������ftp_login
    ���ã�����FTP��¼
    ������socket�׽��֣��û���������
    ����ֵ��0��ʾ�ɹ���������ʾʧ��
*/

#endif // FTPAPI_H_INCLUDED
