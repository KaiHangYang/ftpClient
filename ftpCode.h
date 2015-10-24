#ifndef FTPCODE_H_INCLUDED
#define FTPCODE_H_INCLUDED

#define FTP_SUCCESS 200 // 成功
#define FTP_READY 220 // 服务器就绪
#define FTP_FILE_SUSSFIN 226 // 服务器文件传输成功
#define FTP_LOGIN_SUCCESS 230 // 登录成功
#define FTP_FILE_ACT_FIN 250 // 文件行为完成
#define FTP_FILE_CREATED 257 // 文件创建成功
#define FTP_PW_NEEDED 331 // 要求密码
#define FTP_PW_WRONG 530 // 用户密码错误

#endif // FTPCODE_H_INCLUDED
