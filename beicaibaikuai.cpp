#include <Windows.h>
#include <time.h>
#include <MMSystem.h>
#pragma comment(lib,"winmm.lib")

///入口 GUI窗口应用程序
//WINAPI 函数的约定调用
//HWND  窗口句柄类型

#define BLOCK 100

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//

int APIENTRY/*WINAPI*/ wWinMain(_In_ HINSTANCE hInstance,  ///实例句柄？
	_In_opt_ HINSTANCE hPrevInstance, //父实例句柄？
	_In_ LPWSTR    lpCmdLine, ///字符串宽字符集的 控制台参数？
	_In_ int       nCmdShow)  
{
	//别踩白块这个游戏
	//做一个窗口
	//1.设计窗口类
	TCHAR szAppClassName[] = TEXT("TJ");
	WNDCLASS wc;
	wc.cbClsExtra = 0;  //窗口类扩展大小
	wc.cbWndExtra = 0;  //窗口的扩展大小
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//加载白色背景画刷
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//加载光标
	wc.hIcon = NULL;//加载图标
	wc.hInstance = hInstance;//应用程序实例句柄
	wc.lpfnWndProc = WndProc;//窗口处理函数
	wc.lpszClassName = szAppClassName; //窗口类型名
	wc.lpszMenuName = NULL;// 菜单名
	wc.style = CS_HREDRAW | CS_VREDRAW;  //窗口类的风格

	//2,注册窗口类
	RegisterClass(&wc);

	//3.创建窗口
	HWND hWnd = CreateWindow(
		szAppClassName,         //窗口类型名
		TEXT("别踩绿块-何天骄"), //窗口标题
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,//自己参看文档
		200, 100,        //窗口左上角横坐标
		417, 440,         //窗口宽 高。
		NULL,           //父窗口句柄
		NULL,           //菜单句柄
		hInstance,  //应用程序实例句柄
		NULL          //一个传递的参数
	);
	
	//4,显示窗口
	ShowWindow(hWnd, SW_SHOW);
	//5.更新窗口
	UpdateWindow(hWnd);
	//6.消息循环
	MSG msg; //消息机制驱动运行
	while (GetMessage(&msg,NULL,0,0))
	{
		//将虚拟键消息转换成字符消息
		TranslateMessage(&msg);
		//将消息分发给窗口处理函数
		DispatchMessage(&msg);
	}
	
	
	return 0;


}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;   //绘图的数据？
	HDC hDc;         //绘图句柄，DC 相当于一个设备上下文
	static int bw[4]; //每个屏幕有四个黑快，从上往下数。
	RECT rect;   //矩形区域
	HBRUSH hBrush; //画刷句柄
	POINT point;      //鼠标点击的坐标
	static int n = 0;  // 踩的格数
	static int tm = 0;
	//时间
	TCHAR szMsg[200];

	switch (message) //uMsg？老师的
	{
	case WM_CREATE://窗口创建的时候随机数播种
	{
		srand((unsigned)time(NULL));
		for (int i = 0; i < 4; i++)
		{
			bw[i] = rand() % 4;
		}
		//开启定时器
		SetTimer(hWnd,1,10,NULL);
		//播放音乐
		PlaySound(L"1.wav", NULL, SND_NODEFAULT | SND_ASYNC);

	}
	case WM_TIMER:
		tm++;
		break;
	case WM_PAINT:  //窗口绘图消息
		hDc = BeginPaint(hWnd, &ps); 
		//画图
		//Rectangle(hDc, 0, 0, 100, 100); 画了一个矩形
		//画线
		//水平
		/*
		MoveToEx(hDc, 0, 0, NULL); //开始位置
		LineTo(hDc, 400, 0); //结束位置

		MoveToEx(hDc, 0, 100, NULL); //开始位置  第二条
		LineTo(hDc, 400,100); //结束位置

		MoveToEx(hDc, 0, 200, NULL); //开始位置
		LineTo(hDc, 400, 200); //结束位置
		*/
		for (int i = 0; i < 5; i++)
		{
			MoveToEx(hDc, 0, i*BLOCK, NULL); //开始位置
			LineTo(hDc, 400, i*BLOCK); //结束位置
			//竖直
			MoveToEx(hDc,i * BLOCK,0, NULL); //开始位置
			LineTo(hDc,i * BLOCK,400); //结束位置
		}
		//画黑块
		for (int i = 0; i < 4; i++)
		{
			SetRect(&rect, bw[i] * BLOCK, i * BLOCK, bw[i] * BLOCK + 100, i * BLOCK + 100);
			hBrush=CreateSolidBrush(RGB(0, 255, 0));  
			SelectObject(hDc, hBrush);
			Rectangle(hDc,rect.left,rect.top,rect.right,rect.bottom );
			DeleteObject(hBrush);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:   //鼠标左键点击消息
	{
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		if (point.x / BLOCK != bw[3])
		{
			wsprintf(szMsg, L"您输了用时%d.%d秒，格数%d个", tm / 100, tm - (tm / 100)*100,n);
		
			MessageBox(hWnd, szMsg, L"提示",MB_OK);
			PostQuitMessage(0);

		}
		for (int i = 3; i >= 1; i--)
		{
			bw[i] = bw[i - 1];
		}
		bw[0] = rand() % 4;
		n++;
		//滚动窗口
		ScrollWindow(hWnd, 0, BLOCK, NULL, NULL);
	}
	break;

	//case WM_KEYDOWN:  //点击键盘
	//case WM_RBUTTONDOWN: //点击鼠标 右键
	case WM_CLOSE:  //窗口关闭消息，关闭只是把窗口叉了，没有真正干掉，还可以恢复
		DestroyWindow(hWnd); //销毁窗口，不可以恢复了，窗口的按钮都没了。
		break;
	case WM_DESTROY:  //窗口销毁消息
		PostQuitMessage(0); //退出进程。
		break;

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}