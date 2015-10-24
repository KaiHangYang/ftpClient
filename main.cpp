#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#include "ftpCode.h"
#include "ftpAPI.h"

using namespace std;

int main()
{
    // windows下自己配置的FTP服务器
    SOCKET s = ftp_connect("127.0.0.1");
    if (s == SOCKET_ERROR) {
        return -1;
    }

    ftp_login(s, "ftp", "123456");
//
    ftp_cwd(s, "Download");
    ftp_cwd(s, "haha");
    ftp_cwd(s, "../");
    char * list_data;
    int list_len = 0;

    ftp_list(s, "./", &list_data, &list_len, 0);
    list_data[list_len] = 0;
    cout << list_data << endl;
    ftp_cwd(s, "Download");

//    ftp_list(s, "./", &list_data, &list_len, 1);
//    list_data[list_len] = 0;
//    cout << "Dir content: \n" << list_data << endl;
//
    int file_size;
    int ahah = ftp_file_download(s, "n.png", "a.png", &file_size, 1);
    cout <<  "Error download result -> " << ahah << endl;
    ftp_cwd(s, "../Upload");
    ahah = ftp_file_upload(s, "c.png", "a.png", &file_size, 1);
    cout <<  "Error download result -> " << ahah << endl;
//     上传文件
//    ftp_cwd(s, "../Upload");
//    int file_len;
//    int result = ftp_file_upload(s, "hehe.txt", "haha.txt", &file_len, 1);
//    cout << result << endl;
    ftp_quit(s);
    return 0;
}
