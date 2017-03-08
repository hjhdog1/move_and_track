/*
**                        Copyright 1998 by KVASER AB
**            P.O Box 4076 SE-51104 KINNAHULT, SWEDEN Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** This program serves as an extremely simple wrapper around simple.c which,
** in turn, serves as a demonstration of certain CANLIB programming techniques.
**
** The author realizes that this program is not useful for anything else.
**
*/
#include <stdarg.h>
#include <windows.h>
#include <math.h>
#include "main.h"

// These are needed for the calls to CANLIB
#include <canlib.h>

// Prototypes for simple.c (that contains most of the CANLIB stuff)
#include "simple.h"


#define CLASSNAME "CANLIBTest"

HINSTANCE hInst;

#ifdef __BORLANDC__
#pragma argsused
#endif
int WINAPI WinMain(HINSTANCE hinstCurrent,
				   HINSTANCE hinstPrevious,
				   LPSTR lpszCmdLine,
				   int nCmdShow)
{
	MSG msg;

    if (!InitApplication(hinstCurrent)) {
        return FALSE;
	}

	if (!InitInstance(hinstCurrent, nCmdShow))
		return FALSE;


	while (GetMessage(&msg, NULL, 0, 0) == TRUE) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(msg.wParam);
}

BOOL InitApplication(HINSTANCE hinstCurrent)
{
	WNDCLASS wc;

	wc.style = 0;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstCurrent;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = "GenericMenu";
	wc.lpszClassName = CLASSNAME;

	return RegisterClass(&wc);
}


BOOL InitInstance(HINSTANCE hinstCurrent, int nCmdShow)
{
	HWND hWnd;

	hInst = hinstCurrent;
	hWnd = CreateWindow(
		CLASSNAME,
		"CANLIB Simple Test",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hinstCurrent,
		NULL);

	if (hWnd == NULL)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


LRESULT WINAPI MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND:

            switch (wParam) {

                case IDM_ABOUT:
                    DialogBox(hInst, "AboutBox", hWnd, (DLGPROC) About);
                    return 1;

                case IDM_TEST:
                    DialogBox(hInst, "TestBox", hWnd, (DLGPROC) Test);
                    return 1;

                case IDM_EXIT:
                    PostQuitMessage(0);
                    break;

                default:
                    return (DefWindowProc(hWnd, message, wParam, lParam));
            }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC dc;
            char s[100];

            dc = BeginPaint(hWnd, &ps);
            wsprintf(s,"OK, it's not much of a GUI.. but have a look at the File menu.");
            TextOut(dc, 10, 10, s, strlen(s));
            EndPaint(hWnd, &ps);
        }
        break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;

}


#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI About(HWND hDlg, WORD message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		return 1;

	case WM_COMMAND:
		if (wParam == IDOK || wParam == IDCANCEL) {
			EndDialog(hDlg, TRUE);
			return 1;
		}
		break;
	}

	return 0;
}

//
// A printf()-alike function for adding strings to a listbox.
//
HWND HandleForPrint;

int Print(char *fmt, ...)
{
    char buffer[100];
    va_list argptr;
    int cnt;
    MSG msg;
    int items;

    va_start(argptr, fmt);
    cnt = wvsprintf(buffer, fmt, argptr);
    va_end(argptr);

    SendDlgItemMessage(HandleForPrint, IDC_MESSAGES, LB_ADDSTRING, 0, (LPARAM)buffer);

    items = SendDlgItemMessage(HandleForPrint, IDC_MESSAGES,
                               LB_GETCOUNT, 0, 0);
    SendDlgItemMessage(HandleForPrint, IDC_MESSAGES,
                       LB_SETCARETINDEX, items-1, MAKELPARAM(FALSE, 0));
    
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (!IsDialogMessage(HandleForPrint, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    return cnt;
}

//
// This routine is executed "something happens" in the dialog created by
// choosing File|Test from the main menu.
// In order to keep the code relatively clean we frequently call routines
// contained in simple.c from here.
//
#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI Test(HWND hDlg, WORD message, WPARAM wParam, LPARAM lParam)
{
    static long Speed = 0;
    static int handle1, handle2;
    
	switch (message)
	{
		case WM_INITDIALOG:
            HandleForPrint = hDlg;
            handle1 = canINVALID_HANDLE;
            handle2 = canINVALID_HANDLE;
            //
            // Mark the "125k" radio button as checked.
            //
            SendDlgItemMessage(hDlg, IDC_125K,
                               BM_SETCHECK,
                               BST_CHECKED, 0);
            Speed = canBITRATE_125K;

            //
            // Initialize canlib.
            //
            canInitializeLibrary();
            
            return TRUE;

		case WM_COMMAND:
            switch (wParam) {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, TRUE);
                    return TRUE;

                case IDC_125K:
                    Speed = canBITRATE_125K;
                    break;
                    
                case IDC_250K:
                    Speed = canBITRATE_250K;
                    break;

                case IDC_500K:
                    Speed = canBITRATE_500K;
                    break;

                case IDC_1000K:
                    Speed = canBITRATE_1M;
                    break;

                case IDC_MESSAGES:
                    break;
                    
                case IDC_INIT1:
                    handle1 = InitCtrl(0, Speed);
                    break;
                    
                case IDC_INIT2:
                    handle2 = InitCtrl(1, Speed);
                    break;
                    
                case IDC_START:
                    if (handle1 == canINVALID_HANDLE) {
                        Print("First controller isn't initialized.");
                        break;
                    }
                    if (handle2 == canINVALID_HANDLE) {
                        Print("Second controller isn't initialized.");
                        break;
                    }
                    PerformTest(handle1, handle2);
                    Print("");
                    Print("Ready.");
                    handle1 = canINVALID_HANDLE;
                    handle2 = canINVALID_HANDLE;
                    break;
            }
            return TRUE;
    }

	return 0;
}


