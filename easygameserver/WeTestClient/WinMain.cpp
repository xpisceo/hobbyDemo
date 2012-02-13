// Ӧ�ó������ڵ㡣

#include "WeLogInfo.h"
#include <Windows.h>
#include <Windowsx.h>
#include <mmsystem.h>
#include <assert.h>
#include <string>
using namespace std;

#include "WeTestClient.h"
using namespace We;

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;												// ��ǰʵ��
char szTitle[MAX_LOADSTRING] = "���Կͻ���";					// �������ı�
char szWindowClass[MAX_LOADSTRING] = "WeTestClient";			// ����������

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

#define		ID_EDIT_INFO	101
#define		ID_EDIT_CMD		102

const int FORM_WIDTH = 600;
const int FORM_HEIGHT = 600; 

LRESULT CALLBACK EditCmdProc(HWND, UINT, WPARAM, LPARAM);
WNDPROC s_OldEditCmdProc;

HWND s_Hwnd = 0;
HWND s_Edit_Info;
HWND s_Edit_Cmd;

DWORD s_FPS = 0;
DWORD s_FrameCount = 0;
DWORD s_LastSecondTime = timeGetTime();

TestClient theClient;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	if( !theClient.Init() )
	{
		::MessageBox( s_Hwnd, "����ʧ��", "����ʧ��", 0 );
		return 0;
	}

	MSG msg;
	// ����Ϣѭ��:
	while ( true )
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if(0 == GetMessage(&msg, NULL, 0, 0))// WM_QUIT
            {
				break;
            }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			theClient.Update();
			/// FPS
			DWORD currTime = ::timeGetTime();
			s_FrameCount++;
			DWORD time2 = currTime - s_LastSecondTime;
			if( time2 > 1000 )
			{
				s_FPS = (s_FrameCount*1000)/time2;
				s_FrameCount = 0;
				s_LastSecondTime = currTime;
				char tmp[256];
				::sprintf( tmp, "%s FPS=%d", szTitle, s_FPS );
				::SetWindowText( s_Hwnd, tmp );
			}
			else if(time2 < 0)
			{
				s_FrameCount = 0;
				s_LastSecondTime = currTime;
			}
			Sleep( 1 );
		}
	}

	theClient.Shutdown();

	return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow( szWindowClass, szTitle, WS_SYSMENU|WS_MINIMIZE|WS_MINIMIZEBOX,
	   CW_USEDEFAULT, CW_USEDEFAULT, FORM_WIDTH, FORM_HEIGHT, NULL, (HMENU)NULL, hInstance, NULL);

   s_Hwnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		s_Edit_Info = CreateWindow("edit", NULL, WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE|ES_READONLY|WS_VSCROLL|ES_AUTOHSCROLL,
			10, 90, FORM_WIDTH-20, 450, hWnd, (HMENU)ID_EDIT_INFO, hInst, NULL);

		s_Edit_Cmd = CreateWindow("edit", NULL, WS_CHILD|WS_VISIBLE|WS_BORDER,
			10, FORM_HEIGHT-22-30, FORM_WIDTH-20, 22, hWnd, (HMENU)ID_EDIT_CMD, hInst, NULL);

#pragma warning( disable : 4311 4312 )
		s_OldEditCmdProc = (WNDPROC)SetWindowLong(s_Edit_Cmd, GWL_WNDPROC, (LONG)EditCmdProc);
		SetFocus(s_Edit_Cmd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		LogInfo( "����ֱ�ӹرմ���,��Ҫ��������!" );
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK EditCmdProc(HWND hWnd, UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	if(iMessage == WM_KEYDOWN && wParam == VK_RETURN) // �س�
	{
		char szCmd[256];
		GetWindowText( hWnd, szCmd, 256 );

		LogInfo( "CMD> %s", szCmd );
		SetWindowText(hWnd, "");

		theClient.OnInputCommand( szCmd );
		if( stricmp( szCmd, "quit" ) == 0 )
		{
			/// quit
			PostQuitMessage(0);
		}
	}

	return CallWindowProc(s_OldEditCmdProc, hWnd, iMessage, wParam, lParam);
}

namespace We
{
	void LogInfo( const char* format, ... )
	{
		va_list arg_ptr;
		char str[1024] = {0,};

		va_start(arg_ptr, format);
		vsprintf(str, format, arg_ptr);
		va_end(arg_ptr);

		::strcat( str, "\r\n" );
		unsigned int maxLen  = (unsigned int)(::SendMessage( s_Edit_Info, EM_GETLIMITTEXT, 0, 0 ));
		::SendMessage( s_Edit_Info, EM_SETSEL, maxLen, maxLen );
		::SendMessage( s_Edit_Info, EM_REPLACESEL, FALSE, (LPARAM)str );
	}
}