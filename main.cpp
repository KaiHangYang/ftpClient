#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#include "ftpCode.h"
#include "ftpAPI.h"

using namespace std;

int main()
{
    // �ҵ�FTP��������ַ 115.28.215.2
    SOCKET s = ftp_connect("115.28.215.2");

    return 0;
}
