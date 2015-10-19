#include "ftpAPI.h"
#include "ftpCode.h"

using namespace std;

SOCKET socket_connect(char * host, int port) {
    int i = 0;
    unsigned char ch[4];
    char ip[20];

    //  ����windows���첽�׽��֣�����WSAStartup֮�����ʹ��socket�ĺ���
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 0);
    if (WSAStartup(sockVersion, &wsaData)) {
        cout << "Init socket dll Error!" << endl;
        exit(1);
    }

    // ��������
    struct hostent * server = gethostbyname(host);
    if (!server) {
        cout << "No such host!" << endl;
        return -1;
    }

    // ��Ϊһ��hostname ���ܶ�Ӧ���IP
    while (server->h_addr_list[i] != NULL) {
        memcpy(&ch, server->h_addr_list[i], 4);
        sprintf(ip, "%d.%d.%d.%d", ch[0], ch[1], ch[2], ch[3]);
        ++i;
    }

#ifndef NDEBUG
    cout << ip << endl;
#endif // DEBUG

    // AF_INET ��IPv4������Э���׽���
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET_ERROR == s) {
        cout << "Create Socket Error!" << endl;
        exit(1);
    }

    // Ϊsocket��ӳ�ʱ
    int timeout = 3000;
    int ret = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    ret = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    // ��������ַ
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.S_un.S_addr = inet_addr(ip);
    address.sin_port = htons((unsigned short)port);
#ifndef NDEBUG
    cout << "���ӵķ���������Ϣ��" << address.sin_addr.S_un.S_addr << ":" << address.sin_port << endl;
#endif // NDEBUG
    // ����
    if (SOCKET_ERROR == connect(s, (LPSOCKADDR)&address, sizeof(address))) {
        cout << "Can Not Connect To the Server!" << endl;
        exit(1);
    }
    else {
        cout << "Connect Success!" << endl;
    }
    return s;
}

SOCKET ftp_connect(char *host, int port) {
    SOCKET instr_socket;
    int result;
    ssize_t len;
    char buf[BUFSIZ];

    instr_socket = socket_connect(host, port);

    if (-1 == instr_socket) {
        return -1;
    }

    while ((len = recv(instr_socket, buf, BUFSIZ, 0)) > 0) {
        buf[len] = '\0';
#ifndef NDEBUG
        cout << "�����ӻ�ȡ��socket��Ϣ��" << buf << endl;
#endif // NDEBUG
    }

    sscanf(buf, "%d", &result);

    if (FTP_READY != result) {
        cout << "FTP is not ready, close the socket!" << endl;
        closesocket(instr_socket);
        return -1;
    }
    else {
        cout << "FTP is ready!" << endl;
    }

    return instr_socket;
}
int ftp_cmd_send(SOCKET sock, char * cmd, char *rec_buf, ssize_t *len) {
    ssize_t s_len;
    char s_buf[BUFSIZ];

    if (SOCKET_ERROR == (send(sock, cmd, strlen(cmd), 0))) {
        // ��������ʧ��
        return -1;
    }

    s_len = recv(sock, s_buf, BUFSIZ, 0);

    if (s_len < 1) {
        return -1;
    }
    s_buf[s_len] = '\0';

    if (NULL != len && NULL != rec_buf) {
        *len = s_len;
        sprintf(rec_buf, "%s", s_buf);
    }
    else {
        cout << "ERROR:rec_buf or len is NULL!" << endl;
        return -1;
    }

    return 0;
}
int ftp_cmd_send(SOCKET sock, char *cmd) {
    ssize_t s_len;
    char s_buf[BUFSIZ];
    int result;

    cout << "Client: " << cmd << endl;

    result = ftp_cmd_send(sock, cmd, s_buf, &s_len);

    cout << "Server: " << s_buf << endl;

    if (0 == result) {
        sscanf(s_buf, "d", &result);
    }

    return result;
}
