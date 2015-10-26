#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <commctrl.h>
#include <string.h>
#include <winsock2.h>
#include "resource.h"
#include "ftpCode.h"
#include "ftpAPI.h"

#define MAX_FILENAME 100
#define MAX_IP_LENGTH 20
#define MAX_PORT_LENGTH 10
#define MAX_COM_LENGTH 25

using namespace std;
SOCKET cmd_socks[5];
char g_username[MAX_COM_LENGTH];
char g_password[MAX_COM_LENGTH];
char g_ip[MAX_IP_LENGTH];
char g_port[MAX_PORT_LENGTH];
int isPort = 0;
HWND clientNotice = NULL;
char currDirPath[100][BUFSIZ];
HWND listView;
HWND mainHandle;
int currLevel;
int dirchangeFlag;

OPENFILENAME
newFileDialog (HWND handler, LPCTSTR ext) {
    OPENFILENAME ofn;
    static char szFile[MAX_FILENAME];
    static char szFileTitle[MAX_FILENAME];
    memset(&ofn,0,sizeof(ofn));
    ofn.lStructSize=sizeof(ofn);
    ofn.hwndOwner=handler;
    ofn.lpstrFilter=NULL;
    ofn.nFilterIndex=1;
    ofn.lpstrFile=szFile;
    ofn.nMaxFile=sizeof(szFile);
    ofn.lpstrFileTitle=szFileTitle;
    ofn.nMaxFileTitle=sizeof(szFileTitle);
    ofn.Flags=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_EXPLORER;
    ofn.lpstrDefExt = ext;
    return ofn;
}

void setClientNotice(char *txt) {
    Static_SetText(clientNotice, txt);
}
DWORD WINAPI getCurrentDir(void *arg) {
    HWND listView = GetDlgItem(mainHandle, 12);
    char *dirList = (char *)malloc(sizeof(char)*BUFSIZ);
    int listLength;
    int result;
    char * mixFilename = (char *)malloc(100);

    result = ftp_list(cmd_socks[0], currDirPath[currLevel], &dirList, &listLength, isPort);
    dirList[listLength] = '\n';
    dirList[listLength+1] = '\0';

    if (result != 0) {
        setClientNotice("获取列表失败！");
        return 1;
    }

    int flag = 0;int i = 0;
    char name[50][BUFSIZ];
    char type[50][BUFSIZ];
    char line[50][BUFSIZ];
    int lineSize = 0;
    //获取格式化后的字符串
    while (listLength-flag >= 2) {
        sscanf(dirList+flag, "%[^\r\n]\r\n", line[i]);
        flag += (strlen(line[i])+2);
        lineSize ++;
        cout << flag << "\t" << strlen(dirList) << "\t" << line[i] << endl;
        ++i;
    }

//     对字符串进行格式化获取文件名和文件类型

    for (int j=0; j < lineSize; j++) {

//        sscanf(line[j], "%*[^<]<%[^>]>%*[\ ]%[^\0]", type[j], name[j]);
        if (strstr(line[j], "<") != NULL && strstr(line[j], ">") != NULL) {
            sscanf(line[j], "%*[^<]<%[^>]>%*[\ ]%[^\0]", type[j], name[j]);
        }
        else {
            strrev(line[j]);
            sscanf(line[j], "%[^\ ]", name[j]);
            strrev(line[j]);
            strrev(name[j]);
            type[j][0] = '\0';

        }
    }
    // 先清空列表
    ListView_DeleteAllItems(listView);

    LV_ITEM item;
    ZeroMemory(&item, sizeof(LV_ITEM));

    item.mask = LVIF_TEXT;
    item.cchTextMax = MAX_PATH;
    item.iItem = 0;
    item.iSubItem = 0;
    for (int j=0; j < lineSize; j++) {
        cout << "type: " << type[j] << "\t name: " << name[j] << endl;
        memset(mixFilename, sizeof(mixFilename), 0);
        sprintf(mixFilename, "%s<%s>", name[j], (strlen(type[j]) == 0 ? "file":"dir"));
        cout << mixFilename << endl;
        item.pszText = mixFilename;
        SendMessage(listView, LVM_INSERTITEM, 0, (LPARAM)&item);
    }

    return 0;
}

DWORD WINAPI changeDir(void *arg) {
    int result;
    result = ftp_cwd(cmd_socks[0], currDirPath[currLevel]);
    if (result != 0) {
        setClientNotice("改变目录失败！");
        if (dirchangeFlag) {
            // 进入文件夹失败
            --currLevel;
            strcpy(currDirPath[currLevel+1], "");
        }
        else {
            ++currLevel;
        }
    }
    if (!dirchangeFlag) {
        // 返回上层文件夹成功之后
        strcpy(currDirPath[currLevel+1], "");
    }
    if (currLevel == 0) {
        Button_Enable(GetDlgItem(mainHandle, 16), FALSE);
    }
    else {
        Button_Enable(GetDlgItem(mainHandle, 16), TRUE);
    }
    Button_SetText(GetDlgItem(mainHandle, 1), currDirPath[currLevel]);
    getCurrentDir(NULL);
}

BOOL CALLBACK
clientWindow (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE: {
            DestroyWindow(hWnd);

            break;
        }
        case WM_INITDIALOG: {
            InitCommonControls();
            //初始化界面
            mainHandle = hWnd;
            HWND dfile = GetDlgItem(hWnd, 4);
            SetWindowText(dfile, "双击左边的文件选定下载的文件！");

            HWND mode = GetDlgItem(hWnd, 6);
            Button_SetCheck(mode, TRUE);

            // 提示信息
            HWND notice = GetDlgItem(hWnd, 10);
            clientNotice = notice;
            Static_SetText(notice, "欢迎使用！");

            // 禁用上层文件
            Button_Enable(GetDlgItem(hWnd, 16), FALSE);
            // 对树型结构的测试
            listView = GetDlgItem(hWnd, 12);

            // 初始化当前路径
            currLevel = 0;
            strcpy(currDirPath[currLevel], "/");


            CreateThread(NULL, 0, &getCurrentDir, NULL, 0, NULL);
            Button_SetText(GetDlgItem(mainHandle, 1), currDirPath[currLevel]);
//            getCurrentDir(NULL);

//            LV_ITEM item;
//            ZeroMemory(&item, sizeof(LV_ITEM));
////
//            item.mask = LVIF_TEXT;
//            item.cchTextMax = MAX_PATH;
//            item.iItem = 0;
//            item.iSubItem = 0;
//            item.pszText = "测试";

            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case 7: {

                    break;
                }
                case 8: {

                }
                case 5: {

                    break;
                }
                case 13: {
                    CreateThread(NULL, 0, &getCurrentDir, NULL, 0, NULL);
                    break;
                }
                case 16: {
                    // 返回上层文件夹
                    currLevel--;
                    dirchangeFlag = 0;

                    CreateThread(NULL, 0, &changeDir, NULL, 0, NULL);

                    break;
                }
                default: {

                    break;
                }
            }
        }
        case WM_NOTIFY :{
            switch (LOWORD((wParam))) {
                case 12 :{
                    LV_ITEM lvi;

                    if (((LPNMHDR)lParam)->code == NM_DBLCLK) {

                        cout << "Double clicked:" << endl;
                        char Text[255];
                        int Selected = SendMessage(listView, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);

                        memset(&lvi, 0, sizeof(lvi));

                        if (Selected == -1) {
                            break;
                        }

                        lvi.mask=LVIF_TEXT;
                        lvi.iSubItem = 0;
                        lvi.pszText = Text;
                        lvi.cchTextMax = 256;
                        lvi.iItem = Selected;
                        SendMessage(listView,LVM_GETITEMTEXT, Selected, (LPARAM)&lvi);

                        if (strstr(Text, "file") != NULL) {
                            // 文件的话
                            cout << "选取的是文件" << endl;
                            char fileName[BUFSIZ];
                            sscanf(Text, "%[^<]", fileName);
                            cout << fileName << endl;

                            // 选择要下载的文件
                            char filePath[BUFSIZ];
                            if (currLevel == 0) {
                                sprintf(filePath, "%s%s", currDirPath[currLevel], fileName);
                            }
                            else {
                                sprintf(filePath, "%s/%s", currDirPath[currLevel], fileName);
                            }

                            cout << "要下载的文件" << filePath << endl;
                            Button_SetText(GetDlgItem(hWnd, 4), filePath);
                        }
                        else if (strstr(Text, "dir") != NULL) {
                            cout << "当前文件夹" << currDirPath[currLevel] << endl;
                            cout << "选取的是文件夹" << endl;
                            char dirName[BUFSIZ];
                            sscanf(Text, "%[^<]", dirName);
                            cout << dirName << endl;

                            // 进入相关的文件夹
                            currLevel++;
                            strcpy(currDirPath[currLevel], currDirPath[currLevel-1]);
                            if (currLevel != 1) {
                                strcat(currDirPath[currLevel], "/");
                            }
                            strcat(currDirPath[currLevel], dirName);
                            dirchangeFlag = 1;
                            // 改变当前的文件夹
                            cout << currLevel << endl;

                            CreateThread(NULL, 0, &changeDir, NULL, 0, NULL);
                        }
                        else {
                            cout << "文件信息格式化错误" << endl;
                            setClientNotice("文件信息格式化过程错误！");
                        }
                    }
                    break;
                }
            }
            break;
        }
        default: {
                return FALSE;
        }
    }
    return TRUE;
}
BOOL CALLBACK
loginWindow (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE: {
            EndDialog(hWnd,0);
            socket_clean();
            break;
        }
        case WM_INITDIALOG: {
//             登录界面初始化
//            将匿名button设置为否
            HWND annoy = GetDlgItem(hWnd, 6);
            SendMessage(annoy, BM_SETCHECK, BST_CHECKED,0);

            // 设置提示信息
            HWND notice = GetDlgItem(hWnd, 9);
            Static_SetText(notice, "正在登陆...");

            // 设置端口
            HWND port = GetDlgItem(hWnd, 2);
            SetWindowText(port, "21");
            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case 7: {
                    cout << "确定" << endl;

                    HWND ip = GetDlgItem(hWnd, 1);
                    HWND port = GetDlgItem(hWnd, 2);
                    HWND uname = GetDlgItem(hWnd, 3);
                    HWND pw = GetDlgItem(hWnd, 4);
                    HWND annoy = GetDlgItem(hWnd, 5);
                    HWND notice = GetDlgItem(hWnd, 9);

                    char ipc[MAX_IP_LENGTH];
                    char portc[MAX_PORT_LENGTH];
                    char unamec[MAX_COM_LENGTH];
                    char pwc[MAX_COM_LENGTH];
                    int isAnnoy = Button_GetCheck(annoy);

                    Static_SetText(notice, "登陆中...");
                    GetWindowText(ip, ipc, MAX_IP_LENGTH-1);
                    GetWindowText(port, portc, MAX_PORT_LENGTH-1);
                    GetWindowText(uname, unamec, MAX_COM_LENGTH-1);
                    GetWindowText(pw, pwc, MAX_COM_LENGTH-1);

                    if (strcmp("", ipc) == 0) {
                        Static_SetText(notice, "IP地址为空！");
                        return TRUE;
                    }
                    else if (strcmp("", portc) == 0) {
                        Static_SetText(notice, "端口为空！");
                        return TRUE;
                    }
                    else if (!isAnnoy && strcmp("", unamec) == 0) {
                        Static_SetText(notice, "用户名为空！");
                        return TRUE;
                    }
                    else if (!isAnnoy && strcmp("", pwc) == 0) {
                        Static_SetText(notice, "密码为空！");
                        return TRUE;
                    }

                    // 开始登录
                    int wsa_result = init_wsa();
                    SOCKET s;
                    if (wsa_result != 0) {
                        Static_SetText(notice, "初始化失败！");
                        return TRUE;
                    }
                    else {
                        if((s = ftp_connect(ipc, atoi(portc))) == -1) {
                            Static_SetText(notice, "连接失败，请检查IP和端口！");
                            return TRUE;
                        }
                        if (isAnnoy) {
                            if (ftp_login(s, "anonymous", "") == 0) {
                                Static_SetText(notice, "登录成功！");
                                // 创建另外的端口
                                cmd_socks[0] = s;
                                strcpy(g_username, unamec);
                                strcpy(g_password, pwc);
                                strcpy(g_ip, ipc);
                                strcpy(g_port, portc);
                                // 打开FTP客户端窗口
                                DestroyWindow(hWnd);
                                DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), NULL, (DLGPROC)clientWindow, 0);
                                return TRUE;
                            }
                            else {
                                Static_SetText(notice, "匿名登录失败，服务器或许未开匿名！");
                                return TRUE;
                            }
                        }
                        else {
                            if (ftp_login(s, unamec, pwc) == 0) {
                                Static_SetText(notice, "登录成功！");
                                // 创建另外的端口
                                cmd_socks[0] = s;
                                strcpy(g_username, unamec);
                                strcpy(g_password, pwc);
                                strcpy(g_ip, ipc);
                                strcpy(g_port, portc);
                                // 打开FTP客户端窗口
                                DestroyWindow(hWnd);
                                DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), NULL, (DLGPROC)clientWindow, 0);
                                return TRUE;
                            }
                            else {
                                Static_SetText(notice, "登陆失败，请检查用户名或密码！");
                                return TRUE;
                            }
                        }
                    }
                    break;
                }
                case 8: {
                    cout << "取消" << endl;
                    socket_clean();
                    DestroyWindow(hWnd);
                    break;
                }
                case 5: {
                    // 匿名
                    HWND uname = GetDlgItem(hWnd, 3);
                    HWND pw = GetDlgItem(hWnd, 4);

                    Button_Enable(uname, FALSE);
                    Button_Enable(pw, FALSE);
                    break;
                }
                case 6: {
                    // 取消匿名
                    HWND uname = GetDlgItem(hWnd, 3);
                    HWND pw = GetDlgItem(hWnd, 4);

                    Button_Enable(uname, TRUE);
                    Button_Enable(pw, TRUE);
                    break;
                }
                default: {

                    break;
                }
            }
        }
        default: {
                return FALSE;
        }
    }
    return TRUE;
}

int main(int argc,char * argv[]) {
    {//隐藏控制台
        HWND hwnd = FindWindow(NULL,argv[0]);
        ShowWindow(hwnd,SW_HIDE);
    }
    //复文本框动态库
    HMODULE hRichEditDll = LoadLibrary(TEXT("RICHED20.dll"));
    HINSTANCE handle = GetModuleHandle(NULL);
    DialogBoxParam(handle, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)loginWindow, 0);
////    DialogBoxParam(handle, LPSTR(IDD_DIALOG2), NULL, (DLGPROC)clientWindow, 0);
    FreeLibrary(hRichEditDll);

    return 0;
}
