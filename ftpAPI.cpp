#include "ftpAPI.h"
#include "ftpCode.h"
#include "time.h"
using namespace std;

// ��portģʽ��һ����Э
int getRandomPort() {
    static int counter = 0;
    srand((int)time(NULL)+counter);
    ++counter;
    return (rand()%(10240) + 2250);
}
// һ��С��hack������
int msgCheck(SOCKET sock) {
    int result;
    char code[3];
    char buf[BUFSIZ];
    result = recv(sock, buf, BUFSIZ, MSG_PEEK);
    strncpy(code, buf, 3);
    if (!((code[0] >= '0' && code[0] <= '9') && (code[1] >= '0' && code[1] <= '9') || (code[1] >= '0' && code[1] <= '9'))) {
        recv(sock, buf, BUFSIZ, 0);
    }
}
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
void socket_clean(SOCKET sock) {
    WSACleanup();
}
int ftp_cmd_send(SOCKET sock, char * cmd, char *rec_buf, ssize_t *len) {
    ssize_t s_len;
    char s_buf[BUFSIZ];

    if (SOCKET_ERROR == (send(sock, cmd, strlen(cmd), 0))) {
        // ��������ʧ��
        return -1;
    }

    msgCheck(sock);
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
//    char tmpcmd[4];

    cout << "Client:" << cmd << endl;

    result = ftp_cmd_send(sock, cmd, s_buf, &s_len);

    cout << "Server:" << s_buf << endl;

    if (0 == result) {
        sscanf(s_buf, "%d", &result);
    }

    if (result == 550) {
        cout << result << endl;

//        strncpy(tmpcmd, cmd, 3);
//        tmpcmd[3] = 0;
//
//         Ŀǰֻ��list �ڳ��������ʱ��û������
//        if (strcmp(tmpcmd, "LIS") != 0) {
//            errorInfoStrip(sock);
//        }
    }
    cout << result << endl;
    return result;
}
int ftp_login (SOCKET sock, char *username, char *password) {
    char buf[BUFSIZ];
    int result;
    sprintf(buf, "USER %s\r\n", username);

    int timeout = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    result = ftp_cmd_send(sock, buf);

#ifndef NDEBUG
    cout << "Login result��" << result << endl;
#endif

    if (FTP_LOGIN_SUCCESS == result) {
        // ����ֱ�ӵ�¼ ��½�ɹ�
        return 0;
    }
    else if (FTP_PW_NEEDED == result) {
        sprintf(buf, "PASS %s\r\n", password);
        result = ftp_cmd_send(sock, buf);

        if (FTP_LOGIN_SUCCESS == result) {
            // ��½�ɹ�
            cout << "��¼�ɹ�!" << endl;
            return 0;
        }
        else {
            // ���������ߵ�¼����
            return -1;
        }
    }
    return -1;
}
int ftp_logout(SOCKET sock) {
    int result = ftp_cmd_send(sock, "QUIT\r\n");
    closesocket(sock);
    socket_clean(sock);
    return result;
}
int ftp_cwd(SOCKET sock, char *dir) {
    int result;
    char buf[BUFSIZ];

    sprintf(buf, "CWD %s\r\n", dir);
    result = ftp_cmd_send(sock, buf);
#ifndef NDEBUG
    cout << "����Ŀ¼�ı�����" << result << endl;
#endif
    if (FTP_FILE_ACT_FIN == result) {
        return 0;
    }

    return -1;
}

int ftp_type(SOCKET sock, char type) {
    char buf[BUFSIZ];
    int result;

    sprintf(buf, "TYPE %c\r\n", type);

    result = ftp_cmd_send(sock, buf);

#ifndef NDEBUG
    cout << "CHANGE TYPE Result: " << result << endl;
#endif
    if (FTP_SUCCESS == result) {
        return 0;
    }
    return -1;
}
SOCKET ftp_data_pasv(SOCKET instr_sock) {
    SOCKET data_sock;

    int result;
    ssize_t len;
    int addr[6];
    char buf[BUFSIZ];
    result = ftp_cmd_send(instr_sock, "PASV\r\n", buf, &len);

    if (result == 0) {
        // ���óɹ�
        sscanf(buf, "%d", &result);
        // ʹ������ƥ���ȡ��ַ
        sscanf(buf, "%*[^(](%d,%d,%d,%d,%d,%d)", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);
#ifndef NDEBUG
    cout << "Pasv Result: " << result << "\t IP: " << addr[0] << "." << addr[1] << "." << addr[2] << "." << addr[3] << "\t Port: " << addr[4]*256+addr[5] << endl;
#endif // NDEBUG
    }
    else {
        return -1;
    }

    // �������ӵ����ݽӿڵ�socket
    memset(buf, sizeof(buf), 0);
    sprintf(buf, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
    data_sock = socket_connect(buf, 256*addr[4]+addr[5]);

    if (-1 == data_sock) {
        cout << "ftp_data_pasv: Data_sock init Error!" << endl;
        return -1;
    }
#ifndef NDEBUG
    cout << "Pasv connect inited!" << endl;
#endif // NDEBUG
    return data_sock;
}
SOCKET ftp_data_port (SOCKET sock) {
    SOCKET data_sock;
    struct sockaddr_in portAddr;
    int result;
    char buf[BUFSIZ];
    int p1 = 9, p2 = 4;

    int port = getRandomPort();
    cout << "Random port is " << port << endl;
    p1 = port / 256;
    p2 = port % 256;

    data_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET_ERROR == data_sock) {
        cout << "ftp_data_port: build socket error!" << endl;
        return -1;
    }


    portAddr.sin_family = AF_INET;
    portAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    portAddr.sin_port = htons((unsigned short)(p1*256+p2)); // ͨ�������ÿͻ����Լ�������ж˿�

    while ((result = bind(data_sock, (struct sockaddr *)&portAddr, sizeof(portAddr))) != 0) {
        if (WSAEADDRINUSE == result) {
            // ���ڶ˿��Ѿ�ʹ�� ���»�ȡһ���˿�
            cout << "ftp_data_port: port has been used, trying the new one!" << endl;
            ++p2;
            if (p2 >= 256) {
                p1 += p2/256;
                p2 = p2%256;
            }
            if (p1 >= 30) {
                p1 = 8;
            }

            portAddr.sin_port = htons((unsigned short)(p1*256+p2));
        }
        else {
            cout << "ftp_data_port: bind failed! code->" << result << endl;
            return -1;
        }
    }
    // �����

    result = listen(data_sock, 64);
    if (0 != result) {
        cout << "ftp_data_port: listen error!" << endl;
        return -1;
    }

//    int portLen;
//    result = getsockname(data_sock, (struct sockaddr *)&portAddr, &portLen);
//    p1 = portAddr.sin_port / 256;
//    p2 = portAddr.sin_port % 256;
    // �ɹ�����֮��Ϳ��Է���ָ����
    memset(buf, BUFSIZ, 0);
    sprintf(buf, "PORT 127,0,0,1,%d,%d\r\n", p1, p2);
    result = ftp_cmd_send(sock, buf);
#ifndef NDEBUG
    cout << "ftp_data_port: PORT result ->" << result << endl;
#endif // NDEBUG
    if (FTP_SUCCESS != result) {
        return -1;
    }

    return data_sock;
}
int ftp_list(SOCKET sock, char *dir, char **data, int *data_len, int isPort) {
    SOCKET data_sock;
    SOCKET tmp_sock;
    char buf[BUFSIZ];
    char * data_buf;
    int result;
    ssize_t len, total_len, buf_len;
    int usedPort = 1;


    if (!isPort) {
        data_sock = ftp_data_pasv(sock);
        usedPort = 0;
    }
    else {
        data_sock = ftp_data_port(sock);

        if (-1 == data_sock) {
            // ʹ��portģʽ����ʧ�ܻ���pasvģʽ
            data_sock = ftp_data_pasv(sock);
            usedPort = 0;
        }
    }

    if (-1 == data_sock) {
        return  -1;
    }

    memset(buf, sizeof(buf), 0);
    sprintf(buf, "LIST %s\r\n", dir);

    result = ftp_cmd_send(sock, buf);
#ifndef NDEBUG
    cout << "ftp_list: send reuslt ->" << result << endl;
#endif // NDEBUG
    if (result >= 300 || result == -1) {
        return result;
    }

    // ����ͳɹ� �����Portģʽ������ȡ��������

    if (usedPort) {
        // ʹ����Portģʽ�Ļ��ͻ�ȡdata����
        tmp_sock = data_sock;
        data_sock = accept(tmp_sock, NULL, NULL);
        if (data_sock == INVALID_SOCKET) {
            return -1;
        }
    }

    total_len = len = 0;
    buf_len = BUFSIZ;
    data_buf = (char *)malloc(buf_len);
    cout << "Begin to accept datas" << endl;

    while ((len = recv(data_sock, buf, BUFSIZ, 0)) > 0) {
        // ������ܵ������ݳ���������buf�������·���
        cout << "recevied something!" << endl;
        if (total_len+len > buf_len) {
            buf_len *= 2;
            char * data_buf_tmp = (char *)malloc(buf_len);
            memcpy(data_buf_tmp, data_buf, total_len);
            free(data_buf);
            data_buf = data_buf_tmp;
        }
        memcpy(data_buf+total_len, buf, len);
        total_len += len;
    }

    if (usedPort) {
        closesocket(tmp_sock);
    }
    closesocket(data_sock);


    // ������˻�ȡ����

    memset(buf, BUFSIZ, 0);

    len = recv(sock, buf, BUFSIZ, 0);
    buf[len] = 0;
    // ��ȡ״̬��
    sscanf(buf, "%d", &result);
#ifndef NDEBUG
    cout << "ftp_list: List data finished code -> " << result << endl;
#endif
    if (FTP_FILE_SUSSFIN != result) {
        // ���û�гɹ������ļ��������
        free(data_buf);
        return result;
    }
    *data = data_buf;
    *data_len = total_len;
    return 0;
}
int ftp_file_download(SOCKET sock, char *server_file, char *local_file, int *file_size, int isPort) {

    SOCKET data_sock, tmp_sock;
    FILE * fp;
    char buf[BUFSIZ];
    int result;
    ssize_t len, write_len;
    int usedPort = 1;


    if (NULL == fp) {
        cout << "ftp_file_download: Create in local failed!" << endl;
        return -2;
    }

    // �����ļ���������Ϊ������
    ftp_type(sock, 'I');

    if (!isPort) {
        // Ĭ��ʹ��pasvģʽ
        data_sock = ftp_data_pasv(sock);
        usedPort = 0;
    }
    else {
        data_sock = ftp_data_port(sock);
        if (-1 == data_sock) {
            // ʹ��portģʽ����ʧ�ܻ���pasvģʽ
            data_sock = ftp_data_pasv(sock);
            usedPort = 0;
        }
    }


    if (-1 == data_sock) {
        // ������������ʧ��
        return -1;
    }

    // �����ļ���������
    memset(buf, BUFSIZ, 0);
    sprintf(buf, "RETR %s\r\n", server_file);
    result = ftp_cmd_send(sock, buf);
#ifndef NDEBUG
    cout << "ftp_file_download: RETR result -> " << result << endl;
#endif // NDEBUG
    if (result >= 300 || -1 == result) {
        // ���ִ���
        return result;
    }

    if (usedPort) {
        tmp_sock = data_sock;
        data_sock = accept(tmp_sock, NULL, NULL);

        if (INVALID_SOCKET == data_sock) {
            return -1;
        }
    }

    // ���һ�������Ϳ�ʼ��������
    fp = fopen(local_file, "wb");
    memset(buf, BUFSIZ, 0);
    while ((len = recv(data_sock, buf, BUFSIZ, 0)) > 0) {
        write_len = fwrite(&buf, len, 1, fp);

        if (1 != write_len) {
            // ��ʱ�ļ�д��ʧ��
            fclose(fp);
            closesocket(data_sock);
            return -2;
        }
        if (NULL != file_size) {
            *file_size += write_len;
        }
    }
    // �������
    if (usedPort) {
        closesocket(tmp_sock);
    }

    closesocket(data_sock);


    fclose(fp);

    //�ӷ��������ܷ�������
    memset(buf, BUFSIZ, 0);
    len = recv(sock, buf, BUFSIZ, 0);
    buf[len] = 0;
    sscanf(buf, "%d", &result);
#ifndef NDEBUG
    cout << "ftp_file_down: File download finish result -> " << result << endl;
#endif
    if (FTP_FILE_SUSSFIN == result) {
        return 0;
    }
    return result;
}
int ftp_file_upload(SOCKET sock, char *server_file, char *local_file, int *file_size, int isPort) {
    SOCKET data_sock, tmp_sock;
    char buf[BUFSIZ];
    int result;
    FILE *fp;
    int usedPort = 1;

    ssize_t len, write_len;
    // �򿪱����ļ�
    fp = fopen(local_file, "rb");
    if (NULL == fp) {
        fclose(fp);
        cout << "�ļ���ʧ�ܣ�" << endl;
        return -2;
    }

    // �������ݴ�������
    ftp_type(sock, 'I');

    // ������������
    if (!isPort) {
        data_sock = ftp_data_pasv(sock);

        usedPort = 0;
    }
    else {
        data_sock = ftp_data_port(sock);

        if (-1 == data_sock) {
            // ʹ��portģʽ����ʧ�ܻ���pasvģʽ
            data_sock = ftp_data_pasv(sock);
            usedPort = 0;
        }
    }

    if (-1 == data_sock) {
        fclose(fp);
        return -1;
    }

    // �����ļ��ϴ�����
    memset(buf, BUFSIZ, 0);
    // ����û������ļ����ֵ������
    sprintf(buf, "STOR %s\r\n", server_file);

    result = ftp_cmd_send(sock, buf);
#ifndef NDEBUG
    cout << "ftp_file_upload: STOR result -> " << result << endl;
#endif // NDEBUG

    if (result >= 300 || -1 == result) {
        // �ļ���������ʧ��
        fclose(fp);
        closesocket(sock);
        return result;
    }

    if (usedPort) {
        tmp_sock = data_sock;
        data_sock = accept(tmp_sock, NULL, NULL);

        if (INVALID_SOCKET == data_sock) {
            return -1;
        }
    }

    // ��ʼ�ϴ�����
    memset(buf, BUFSIZ, 0);
    len = write_len = 0;

    while ((len = fread(buf, 1, BUFSIZ, fp)) > 0) {
        write_len = send(data_sock, buf, len, 0);
        if (write_len != len) {
            // ���ݴ���Ĳ�����
            fclose(fp);
            closesocket(data_sock);
            return -2;
        }
        if (NULL != file_size) {
            *file_size += len;
        }
    }
    // �ϴ����
    if (usedPort) {
        closesocket(tmp_sock);
    }
    closesocket(data_sock);


    fclose(fp);

    memset(buf, BUFSIZ, 0);
    len = recv(sock, buf, BUFSIZ, 0);
    sscanf(buf, "%d", &result);
    if (FTP_FILE_SUSSFIN != result) {
        return result;
    }

#ifndef NDEBUG
    cout << "ftp_file_upload: Upload finished!" << endl;
#endif // NDEBUG
    return 0;
}
int ftp_file_del(SOCKET sock, char *file) {
    int result;
    char buf[BUFSIZ];

    memset(buf, BUFSIZ, 0);
    sprintf(buf, "DELE %s\r\n", file);

    result = ftp_cmd_send(sock, buf);

    if (0 != result) {
        return result;
    }
    return 0;
}
int ftp_quit(SOCKET sock) {
    int result;

    result = ftp_cmd_send(sock, "QUIT\r\n");
    if (0 != result) {
        return result;
    }
    return 0;
}
