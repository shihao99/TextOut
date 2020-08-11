/* 解决Sysmets1.c的实现方法的问题：窗口客户区无法显示全部的内容 
注：Sysmets2.cpp和Sysmets1.c不能同时存在于项目中；
*/
#include<Windows.h>
#include"System.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PSTR szCmdLine, int nShowCmd)
{
	static TCHAR szAppName[] = TEXT("Sysmets1");
	HWND hwnd;
	WNDCLASS wndclass;
	MSG msg;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hIcon = LoadIcon(NULL, IDI_SHIELD);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	RegisterClass(&wndclass);
	hwnd = CreateWindow(szAppName,
		TEXT("Get System Metrics No. 1"),
		WS_OVERLAPPEDWINDOW|WS_VSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxChar, cxCaps, cyChar;
	static int cxClient, cyClient, iVscrollPos;
	HDC hdc;
	int i;
	static int iVscrollMax; // 一定要设置成static
	PAINTSTRUCT ps;
	TCHAR szBuffer[12];
	TEXTMETRIC tm;

	switch (message) {
	case WM_CREATE:
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;

		ReleaseDC(hwnd, hdc);

		//iVscrollMax = max(0, NUMLINES - cyClient / cyChar);
		//SetScrollRange(hwnd, SB_VERT, 0, iVscrollMax, TRUE);

		SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE);
		SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		for (i = 0; i < NUMLINES; i++) {
			TextOut(hdc, 0, cyChar*(i-iVscrollPos), sysmetrics[i].szLabel,
				lstrlen(sysmetrics[i].szLabel));
			TextOut(hdc, 22 * cxCaps, cyChar*(i - iVscrollPos), sysmetrics[i].szDesc,
				lstrlen(sysmetrics[i].szDesc));
			SetTextAlign(hdc, TA_RIGHT | TA_TOP);

			TextOut(hdc, 22 * cxCaps + 40 * cxChar, cyChar*(i - iVscrollPos), szBuffer,
				wsprintf(szBuffer, TEXT("%5d"), GetSystemMetrics(sysmetrics[i].iIndex)));
			SetTextAlign(hdc, TA_LEFT | TA_TOP);
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		iVscrollMax = max(0, NUMLINES - cyClient / cyChar);
		SetScrollRange(hwnd, SB_VERT, 0, iVscrollMax, TRUE);
		return 0;
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			iVscrollPos = 0;
			break;
		case SB_BOTTOM:
			iVscrollPos = NUMLINES - 1;
			break;
		case SB_LINEUP:
			iVscrollPos -= 1;
			break;
		case SB_LINEDOWN:
			iVscrollPos += 1;
			break;
		case SB_PAGEUP:
			iVscrollPos -= cyClient / cyChar;
			break;
		case SB_PAGEDOWN:
			iVscrollPos += cyClient / cyChar;
			break;
		case SB_THUMBPOSITION:
			iVscrollPos = HIWORD(wParam);
			break;
		default:
			break;
		}
		iVscrollPos = max(0, min(iVscrollPos, iVscrollMax));
		if (iVscrollPos != GetScrollPos(hwnd, SB_VERT)) {
			SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_HOME:
			SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
			break;
		case VK_END:
			SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
			break;
		case VK_PRIOR:
			SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
			break;
		case VK_NEXT:
			SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			break;
		case VK_UP:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
			break;
		case VK_DOWN:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			break;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
