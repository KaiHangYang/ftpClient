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
BOOL CALLBACK
clientWindow (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE: {
            DestroyWindow(hWnd);

            break;
        }
        case WM_INITDIALOG: {
            InitCommonControls();
            //��ʼ������
            HWND dfile = GetDlgItem(hWnd, 4);

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
                case 6: {

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
BOOL CALLBACK
loginWindow (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE: {
            EndDialog(hWnd,0);
            socket_clean();
            break;
        }
        case WM_INITDIALOG: {
//             ��¼�����ʼ��
//            ������button����Ϊ��
            HWND annoy = GetDlgItem(hWnd, 6);
            SendMessage(annoy, BM_SETCHECK, BST_CHECKED,0);

            // ������ʾ��Ϣ
            HWND notice = GetDlgItem(hWnd, 9);
            Static_SetText(notice, "���ڵ�½...");

            // ���ö˿�
            HWND port = GetDlgItem(hWnd, 2);
            SetWindowText(port, "21");
            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case 7: {
                    cout << "ȷ��" << endl;

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

                    Static_SetText(notice, "��½��...");
                    GetWindowText(ip, ipc, MAX_IP_LENGTH-1);
                    GetWindowText(port, portc, MAX_PORT_LENGTH-1);
                    GetWindowText(uname, unamec, MAX_COM_LENGTH-1);
                    GetWindowText(pw, pwc, MAX_COM_LENGTH-1);

                    if (strcmp("", ipc) == 0) {
                        Static_SetText(notice, "IP��ַΪ�գ�");
                        return TRUE;
                    }
                    else if (strcmp("", portc) == 0) {
                        Static_SetText(notice, "�˿�Ϊ�գ�");
                        return TRUE;
                    }
                    else if (!isAnnoy && strcmp("", unamec) == 0) {
                        Static_SetText(notice, "�û���Ϊ�գ�");
                        return TRUE;
                    }
                    else if (!isAnnoy && strcmp("", pwc) == 0) {
                        Static_SetText(notice, "����Ϊ�գ�");
                        return TRUE;
                    }

                    // ��ʼ��¼
                    int wsa_result = init_wsa();
                    SOCKET s;
                    if (wsa_result != 0) {
                        Static_SetText(notice, "��ʼ��ʧ�ܣ�");
                        return TRUE;
                    }
                    else {
                        if((s = ftp_connect(ipc, atoi(portc))) == -1) {
                            Static_SetText(notice, "����ʧ�ܣ�����IP�Ͷ˿ڣ�");
                            return TRUE;
                        }
                        if (isAnnoy) {
                            if (ftp_login(s, "anonymous", "") == 0) {
                                Static_SetText(notice, "��¼�ɹ���");
                                // ��������Ķ˿�
                                cmd_socks[0] = s;
                                strcpy(g_username, unamec);
                                strcpy(g_password, pwc);
                                strcpy(g_ip, ipc);
                                strcpy(g_port, portc);
                                // ��FTP�ͻ��˴���
                                DestroyWindow(hWnd);
                                DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), NULL, (DLGPROC)clientWindow, 0);
                                return TRUE;
                            }
                            else {
                                Static_SetText(notice, "������¼ʧ�ܣ�����������δ��������");
                                return TRUE;
                            }
                        }
                        else {
                            if (ftp_login(s, unamec, pwc) == 0) {
                                Static_SetText(notice, "��¼�ɹ���");
                                // ��������Ķ˿�
                                cmd_socks[0] = s;
                                strcpy(g_username, unamec);
                                strcpy(g_password, pwc);
                                strcpy(g_ip, ipc);
                                strcpy(g_port, portc);
                                // ��FTP�ͻ��˴���
                                DestroyWindow(hWnd);
                                DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), NULL, (DLGPROC)clientWindow, 0);
                                return TRUE;
                            }
                            else {
                                Static_SetText(notice, "��½ʧ�ܣ������û��������룡");
                                return TRUE;
                            }
                        }
                    }
                    break;
                }
                case 8: {
                    cout << "ȡ��" << endl;
                    socket_clean();
                    DestroyWindow(hWnd);
                    break;
                }
                case 5: {
                    // ����
                    HWND uname = GetDlgItem(hWnd, 3);
                    HWND pw = GetDlgItem(hWnd, 4);

                    Button_Enable(uname, FALSE);
                    Button_Enable(pw, FALSE);
                    break;
                }
                case 6: {
                    // ȡ������
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
    {//���ؿ���̨
        HWND hwnd = FindWindow(NULL,argv[0]);
        ShowWindow(hwnd,SW_HIDE);
    }
    //���ı���̬��
    HMODULE hRichEditDll = LoadLibrary(TEXT("RICHED20.dll"));
    HINSTANCE handle = GetModuleHandle(NULL);
    DialogBoxParam(handle, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)loginWindow, 0);
//    DialogBoxParam(handle, LPSTR(IDD_DIALOG2), NULL, (DLGPROC)clientWindow, 0);
    FreeLibrary(hRichEditDll);
    return 0;
}
