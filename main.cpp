//包含头文件
#include <windows.h>
#include <time.h>
#include "resource.h"
#include <stdlib.h>
#include <stdio.h>

//包含库文件
#pragma comment (lib,"winmm.lib")  //播放音乐 调用lib

typedef struct Node
{
	int x;
	int y;
	struct Node *pnext;

}DiRen,FeiJi,ZiDan;
//TODO：全局初始化代码
DiRen *pDiRen = NULL;
ZiDan *pZiDan = NULL;
FeiJi *pFeiJi = NULL;
static int score = 0;
static wchar_t nScore[20];
static int var = 0;	//判断自己攻击的次数
int Direnmovevar = 10;
//TODO:函数在此声明
void CreateDiRen();
void ShowDiRen(struct Node *pHead, HWND hWnd);
void CreateZiJi(HWND hWnd);
void CreateZiDan();
void ShowZiDan(ZiDan *pHead, HWND hWnd);
void DiRenMove(DiRen *pHead);
void ZiDanMove(ZiDan *pHead);
void Shoot(HWND hWnd, FeiJi *ziji, DiRen **diren, ZiDan **zidan);
void ReleaseDiRen(DiRen **pHead);
void ReleaseZiDan(ZiDan **pHead);
void DirenMoveVar();
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//宏定义 define
#define TIME_DIREN         101
#define TIME_DIRENMOVE     102  
#define TIME_ZIDAN         103
#define TIME_DIRENRELEASE  104
#define TIME_ZIDANMOVE     105
#define TIME_ZIDANRELEASE  107
//WinMain()函数
//
//
//Windows应用程序入口
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,int nCmdShow)
{
	//创建窗口 
	WNDCLASSEX wndClass = { 0 };
	wndClass.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wndClass.cbClsExtra=0;
	wndClass.cbWndExtra=0;
	wndClass.hbrBackground=(HBRUSH)5;
	wndClass.hInstance=hInstance;
	wndClass.hCursor=NULL;
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndClass.lpfnWndProc=WndProc;
	wndClass.lpszClassName=L"AirPlane";
	wndClass.lpszMenuName=NULL;
	wndClass.hIconSm = NULL;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	//注册窗口
	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}
	//创建窗口
	HWND hWnd = CreateWindow(L"AirPlane", L"雷霆战机", WS_OVERLAPPEDWINDOW,100,100,600,600,NULL,NULL,hInstance,NULL);
	//移动 显示 更新窗口
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);

	//PlaySound(L"music.wav",NULL,SND_FILENAME|SND_ASYNC|SND_LOOP); //循环播放背景音乐
	//消息循环
	MSG msg = { 0 };
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i=1;
	int jscore;
	HDC hDC;
	HDC memDC;
	HBITMAP hBitmap;
	BITMAP bBitmap;
	switch (message)
	{
	case WM_TIMER:
		hDC = GetDC(hWnd); //得到设备句柄
		hBitmap = LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP1));  //载入背景
		GetObject(hBitmap,sizeof(hBitmap),&bBitmap);
		memDC = CreateCompatibleDC(hDC);
		SelectObject(memDC,hBitmap);
		BitBlt(hDC,0,0,600,600,memDC,0,0,SRCCOPY);
		wsprintf(nScore, L"%d", score);
		jscore = score;
		while ((jscore = jscore / 10) > 0)
		{
			i++;
		}
		TextOut(hDC, 0, 0, L"分数:", 4);
		TextOut(hDC, 30, 0, nScore, i);
		DeleteDC(memDC);
		ReleaseDC(hWnd, hDC);
		DeleteObject(hBitmap);
		CreateZiJi(hWnd);
		if (TIME_ZIDAN == wParam)
		{
			ZiDanMove(pZiDan);
			ReleaseZiDan(&pZiDan);
		}
		else if (TIME_DIREN == wParam)
		{
			CreateDiRen();
		}
		else if (TIME_DIRENRELEASE == wParam)
		{
			ReleaseDiRen(&pDiRen);
		}
		ShowDiRen(pDiRen, hWnd);
		DiRenMove(pDiRen);
		ShowZiDan(pZiDan,hWnd);
		ZiDanMove(pZiDan);
		Shoot(hWnd, pFeiJi, &pDiRen,&pZiDan);
		break;
	case WM_CREATE:
		srand(time(NULL));
		SetTimer(hWnd, TIME_DIREN, 1000, NULL);
		pFeiJi = (struct Node*)malloc(sizeof(struct Node));
		pFeiJi->x = 200;
		pFeiJi->y = 500;
		SetTimer(hWnd, TIME_DIREN, 1500, NULL);
		SetTimer(hWnd, TIME_DIRENMOVE, 200, NULL);
		SetTimer(hWnd, TIME_ZIDAN, 100, NULL);
		SetTimer(hWnd, TIME_ZIDANMOVE, 50, NULL);
		SetTimer(hWnd,TIME_DIRENRELEASE,300,NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			if (pFeiJi->x > 0)
				pFeiJi->x -= 20;
			break;
		case VK_RIGHT:
			if (pFeiJi->x < 550)
				pFeiJi->x += 20;
			break;
		case VK_UP:
			if (pFeiJi->y >0)
				pFeiJi->y -= 20;
			break;
		case VK_DOWN:
			if (pFeiJi->y < 550)
				pFeiJi->y += 20;
			break;
		case VK_SPACE:
			CreateZiDan();
			break;
		case VK_RETURN:
			int n = 0;
				if (n = 0)
				{
					KillTimer(hWnd, TIME_DIREN);
					break;
				}
			  if (n = 1)
			  {
				SetTimer(hWnd, TIME_DIREN, 1500, NULL);
				n = 0;
				break;
			  }
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
		return DefWindowProc(hWnd, message, wParam, lParam);
}
//制作敌人
void CreateDiRen()
{
	DiRen *u;
	u = (struct Node*)malloc(sizeof(struct Node));
	u->x = rand() % 550;  //敌人随机出现的x坐标
	u->y = -10;
	u->pnext = NULL;
	if (NULL == pDiRen)
	{
		pDiRen = u;
	}
	else
	{
		u->pnext = pDiRen;
		pDiRen = u;
	}
}
//显示敌人
void ShowDiRen(struct Node *pHead,HWND hWnd)
{
	HDC hDC;
	HDC memDC;
	HBITMAP hBitmap;
	BITMAP bBitmap;
	hDC = GetDC(hWnd); //得到设备句柄
	hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2));  //载入敌人
	GetObject(hBitmap, sizeof(hBitmap), &bBitmap);
	memDC = CreateCompatibleDC(hDC);
	SelectObject(memDC, hBitmap);
	while (pHead!=NULL)
	{
		//透明遮罩法
		//哈哈 好戏来了
		BitBlt(hDC, pHead->x, pHead->y, 40, 40, memDC, 40, 0, SRCAND);
		BitBlt(hDC, pHead->x, pHead->y, 40, 40, memDC, 0, 0, SRCPAINT);
		pHead = pHead->pnext;
	}
	DeleteDC(memDC);
	ReleaseDC(hWnd, hDC);
	DeleteObject(hBitmap);
}
//构造自己
void CreateZiJi(HWND hWnd)
{
	HDC hDC;
	HDC memDC;
	HBITMAP hBitmap;
	BITMAP bBitmap;
	hDC = GetDC(hWnd); //得到设备句柄
	hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3));  //载入自己
	GetObject(hBitmap, sizeof(hBitmap), &bBitmap);
	memDC = CreateCompatibleDC(hDC);
	SelectObject(memDC, hBitmap);
		//透明遮罩法
	BitBlt(hDC, pFeiJi->x, pFeiJi->y, 40, 40, memDC, 40, 0, SRCAND);
	BitBlt(hDC, pFeiJi->x, pFeiJi->y, 40, 40, memDC, 0, 0, SRCPAINT);
	DeleteDC(memDC);
	ReleaseDC(hWnd, hDC);
	DeleteObject(hBitmap);
}
void CreateZiDan()
{
	ZiDan *u;
	u = (ZiDan*)malloc(sizeof(ZiDan));
	u->x = pFeiJi->x+20;  //子弹坐标
	u->y = pFeiJi->y+10;
	u->pnext = NULL;
	if (NULL == pZiDan)
	{
		pZiDan = u;
	}
	else
	{
		u->pnext = pZiDan;
		pZiDan = u;
	}
}
//显示敌人
void ShowZiDan(ZiDan *pHead,HWND hWnd)
{
	HDC hDC;
	HDC memDC;
	HBITMAP hBitmap;
	BITMAP bBitmap;
	hDC = GetDC(hWnd); //得到设备句柄
	hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP4));  //载入子弹
	GetObject(hBitmap, sizeof(hBitmap), &bBitmap);
	memDC = CreateCompatibleDC(hDC);
	SelectObject(memDC, hBitmap);
	while (pHead != NULL)
	{
		//透明遮罩法
		//哈哈 好戏来了
		BitBlt(hDC, pHead->x, pHead->y, 15, 25, memDC, 15, 0, SRCAND);
		BitBlt(hDC, pHead->x, pHead->y, 15, 25, memDC, 0, 0, SRCPAINT);
		pHead = pHead->pnext;
	}
	DeleteDC(memDC);
	ReleaseDC(hWnd, hDC);
	DeleteObject(hBitmap);
}
//敌人移动
void DiRenMove(DiRen *pHead)
{
	while (pHead!=NULL)
	{
		pHead->y += Direnmovevar;
		pHead = pHead->pnext;
	}
}
//子弹移动  条件
void ZiDanMove(ZiDan *pHead)
{
	while (pHead != NULL)
	{
		pHead->y -= 20;
		pHead = pHead->pnext;
	}
}
void Shoot(HWND hWnd, FeiJi *ziji, DiRen **diren, ZiDan **zidan)
{
	DiRen *js1 = *diren;
	ZiDan *js2 = *zidan;
	int n = 1;
	//判断自己是否撞击对方
	while (js1 != NULL)
	{
		if ((ziji->x - js1->x<30 && ziji->x - js1->x>-38) && (ziji->y-js1->y<25&&ziji->y-js1->y>-38))
		{
			KillTimer(hWnd, TIME_DIREN);
			KillTimer(hWnd, TIME_ZIDAN);
			KillTimer(hWnd, TIME_DIRENMOVE);
			KillTimer(hWnd, TIME_ZIDANMOVE);
			KillTimer(hWnd, TIME_DIRENRELEASE);
			MessageBox(hWnd, L"游戏结束！经程序精心判定，你被攻击了！  作者：邢庆宇", L"雷霆战机消息", MB_OK);
			PostQuitMessage(0);
			break;
		}
		else
		{
			js1 = js1->pnext;
		}
	}
	//判断敌机中弹
	js1 = *diren;
	js2 = *zidan;
	while ((js1 = *diren) != NULL)
	{
		zidan = &pZiDan;
		n = 0;
		while ((js2 = *zidan) != NULL)
		{
			//判断是否打中敌人
			if ((js2->x - js1->x <= 40 && js2->x - js1->x >= -5) && (js2->y - js1->y <= 40 && js2->y - js1->y >= -5))
			{
				n = 1;
				score += 100;
				*zidan = js2->pnext;
				if (js1->pnext != NULL)
				{
					*diren = js1->pnext;
					diren = &pDiRen;
					free(js1);
					free(js2);
				}
				else
					*diren = NULL;	
				break;
			}
			else
				zidan = &js2->pnext; //如果没中 子弹判断下一个
		}
		if (n != 1)
			diren = &js1->pnext; //敌人判断下一个
	}
}
//释放
void ReleaseZiDan(ZiDan **pHead)
{
	ZiDan *js = *pHead;
	while ((js = *pHead) != NULL)
	{
		if (js->y < 0)
		{
			*pHead = js->pnext;
			free(js);
		}
		else
			pHead = &js->pnext;
	}
}
void ReleaseDiRen(DiRen **pHead)
{
	ZiDan *js = *pHead;
	while ((js = *pHead) != NULL)
	{
		if (js->y >600)
		{
			*pHead = js->pnext;
			free(js);
		}
		else
			pHead = &js->pnext;
	}
}
void DirenMoveVar()
{
	if (var = 20)
		Direnmovevar = 15;

}