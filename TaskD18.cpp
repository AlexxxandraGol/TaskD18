#include<Windows.h>
#include<tchar.h>
#include<process.h>
#include<stdlib.h>
#include<random>
#include<ctime>
typedef struct
{
	HWND hwnd;
	int cxClient;
	int cyClient;
	int cyChar;
	int cxChar;
	BOOL bKill;
}
PARAMS, * PPARAMS;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	srand(time(0));
	TCHAR szAppName[] = _T("MainFrame");
	MSG msg;
	HWND hWnd;
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;
	wc.hIconSm = LoadIcon(hInstance, szAppName);
	//Регистрируем класс окна
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Cannot registr class", "Error", MB_OK);
		return 0;
	}
	//Создаем основное окно приложения
	hWnd = CreateWindow(szAppName, _T("2 потока"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
		hInstance, NULL);

	if (!hWnd) {
		MessageBox(NULL, _T("Cannot create main window"), _T("Error"), MB_OK);
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return msg.wParam;

}
bool state = false;
VOID Thred1(PVOID pvoid) {
	HDC hdc;
	PPARAMS pparams = (PPARAMS)pvoid;
	while (!pparams->bKill) {
		hdc = GetDC(pparams->hwnd);
		SetTextColor(hdc, RGB(255, 100, 100));
		if (state)
		{
			int r1 = rand() % pparams->cxClient, r2 = rand() % pparams->cyClient;
			TextOut(hdc, r1, r2, "@", 1);
		}
		ReleaseDC(pparams->hwnd, hdc);
		Sleep(1000);
		state = true;
	}
	_endthread();
}
LRESULT APIENTRY WndProc1(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static PARAMS params;

	switch (msg)
	{
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thred1, 0, &params);
		break;
	case WM_SIZE:
		params.cyClient = HIWORD(lParam);
		params.cxClient = LOWORD(lParam);
		break;
	case WM_DESTROY:
		params.bKill = TRUE;
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool state2 = true;
VOID Thred2(PVOID pvoid) {
	RECT rc;
	HDC hdc;
	PPARAMS pparams = (PPARAMS)pvoid;
	HPEN hPen = CreatePen(PS_SOLID, 6, RGB(255, 0, 0));
	while (!pparams->bKill) {
		hdc = GetDC(pparams->hwnd);
		GetClientRect((HWND)pparams->hwnd, &rc);
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
		SelectObject(hdc, hPen);
		if(state2)
			Arc(hdc, (pparams->cxClient / 2) - 50, (pparams->cyClient / 2) - 50, (pparams->cxClient / 2) + 50, (pparams->cyClient / 2) + 50,
				(pparams->cxClient / 2), (pparams->cyClient / 2) - 50, (pparams->cxClient / 2), (pparams->cyClient / 2) + 50);
		else
			Arc(hdc, (pparams->cxClient / 2) - 50, (pparams->cyClient / 2) - 50, (pparams->cxClient / 2) + 50, (pparams->cyClient / 2) + 50,
				(pparams->cxClient / 2), (pparams->cyClient / 2) + 50, (pparams->cxClient / 2), (pparams->cyClient / 2) - 50);
		state2 = !state2;
		ReleaseDC(pparams->hwnd, hdc);
		Sleep(1000);
	}
	_endthread();
}

LRESULT APIENTRY WndProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static PARAMS params;
	switch (msg)
	{
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thred2, 0, &params);
		break;
	case WM_SIZE:
		params.cyClient = HIWORD(lParam);
		params.cxClient = LOWORD(lParam);
		break;
	case WM_DESTROY:
		params.bKill = TRUE;
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static LPCSTR szChildClass[4] = { "Child1", "Child2" };
	static HWND hwndChild[4];
	static WNDPROC ChildProc[] = { WndProc1, WndProc2 };
	HINSTANCE hInstance;
	int i, cxClient, cyClient;
	WNDCLASSEX wc;
	switch (uMsg) {
	case WM_CREATE:
		hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.hIconSm = NULL;
		for (i = 0; i < 2; i++)
		{
			wc.lpfnWndProc = ChildProc[i];
			wc.lpszClassName = szChildClass[i];
			RegisterClassEx(&wc);
			hwndChild[i] = CreateWindow(szChildClass[i], NULL, WS_CHILDWINDOW | WS_BORDER | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)i, hInstance, NULL);
		}
		break;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		for (i = 0; i < 2; i++)
			MoveWindow(hwndChild[i], (i > 1) * cxClient / 2, (i % 2) * cyClient / 2, cxClient , cyClient / 2, TRUE);
		break;
	case WM_CHAR:

		if (wParam == '\x18')
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}