#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#include "ftpCode.h"
#include "ftpAPI.h"

using namespace std;

int main()
{
    // 我的FTP服务器地址 115.28.215.2
    SOCKET s = ftp_connect("115.28.215.2");
    if (s == SOCKET_ERROR) {
        return -1;
    }
    ftp_login(s, "fog", "y1995kh221");
//    ftp_type(s, 'A');
//    ftp_cwd(s, "./haha");
//    ftp_list(s, "./");
    return 0;
}
