/*
**                        Copyright 1997 by KVASER AB
**            P.O Box 4076 SE-51104 KINNAHULT, SWEDEN Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#define IDC_125K	101
#define IDC_250K	102
#define IDC_500K	103
#define IDC_1000K	104
#define IDC_SPEEDBOX1	105
#define IDC_MESSAGES	106
#define IDC_INIT1	107
#define IDC_INIT2	108
#define IDC_START	109
#define IDM_ABOUT 900
#define IDM_TEST  901
#define IDM_EXIT  902


int WINAPI WinMain(HINSTANCE hinstCurrent,
				   HINSTANCE hinstPrevious,
				   LPSTR lpszCmdLine,
				   int nCmdShow);

BOOL InitApplication(HINSTANCE hinstCurrent);
BOOL InitInstance(HINSTANCE hinstCurrent, int nCmdShow);
LRESULT WINAPI MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL WINAPI About(HWND hDlg, WORD message, WPARAM wParam, LPARAM lParam);
BOOL WINAPI Test(HWND hDlg, WORD message, WPARAM wParam, LPARAM lParam);

int Print(char *fmt, ...);
