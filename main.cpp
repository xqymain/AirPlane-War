/*
 * This project GPL3.0 open source，https://github.com/xqymain/AirPlane-War/
 * Be sure to follow the GPLv3 agreement
 * Thank you for your support
 * by xqymain
 */

// Include header file
#include <windows.h>
#include <time.h>
#include "resource.h"
#include <stdlib.h>
#include <stdio.h>

// Include library file
#pragma comment (lib,"winmm.lib")

typedef struct Node
{
	int x;
	int y;
	struct Node *pnext;

}Enemy,Plane,Bullet;

// TODO：Global initialization code
Enemy *pEnemy = NULL;
Bullet *pBullet = NULL;
Plane *pPlane = NULL;
static int score = 0;
static wchar_t nScore[20];
static int shootCount = 0;
int EnemyMoveSpeed = 10;

// TODO: The function is declared here
void CreateEnemy();
void ShowEnemy(struct Node *pHead, HWND hWnd);
void CreateSelf(HWND hWnd);
void CreateBullet();
void ShowBullet(Bullet *pHead, HWND hWnd);
void EnemyMove(Enemy *pHead);
void BulletMove(Bullet *pHead);
void Shoot(HWND hWnd, Plane *ziji, Enemy **Enemy, Bullet **Bullet);
void ReleaseEnemy(Enemy **pHead);
void ReleaseBullet(Bullet **pHead);
void EnemyMoveChange();
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Macro definition
#define TIME_Enemy         101
#define TIME_EnemyMOVE     102  
#define TIME_Bullet         103
#define TIME_EnemyRELEASE  104
#define TIME_BulletMOVE     105
#define TIME_BulletRELEASE  107

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,int nCmdShow)
{ 
	WNDCLASSEX wndClass = { 0 };
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)5;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = NULL;
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = L"AirPlane";
	wndClass.lpszMenuName = NULL;
	wndClass.hIconSm = NULL;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	HWND hWnd = CreateWindow(L"AirPlane", L"Game - AirPlane", WS_OVERLAPPEDWINDOW, 100, 100, 600, 600, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	PlaySound(L"res\\music.wav",NULL,SND_FILENAME|SND_ASYNC|SND_LOOP);
	
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
		hDC = GetDC(hWnd);
		hBitmap = LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP_BG));
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
		TextOut(hDC, 0, 0, L"SC:", 3);
		TextOut(hDC, 30, 0, nScore, i);
		DeleteDC(memDC);
		ReleaseDC(hWnd, hDC);
		DeleteObject(hBitmap);
		CreateSelf(hWnd);
			
		if (TIME_Bullet == wParam)
		{
			BulletMove(pBullet);
			ReleaseBullet(&pBullet);
		}
		else if (TIME_Enemy == wParam)
		{
			CreateEnemy();
		}
		else if (TIME_EnemyRELEASE == wParam)
		{
			ReleaseEnemy(&pEnemy);
		}
		ShowEnemy(pEnemy, hWnd);
		EnemyMove(pEnemy);
		ShowBullet(pBullet,hWnd);
		BulletMove(pBullet);
		Shoot(hWnd, pPlane, &pEnemy,&pBullet);
		break;
	case WM_CREATE:
		srand(time(NULL));
		SetTimer(hWnd, TIME_Enemy, 1000, NULL);
		pPlane = (struct Node*)malloc(sizeof(struct Node));
		pPlane->x = 200;
		pPlane->y = 500;
		SetTimer(hWnd, TIME_Enemy, 1500, NULL);
		SetTimer(hWnd, TIME_EnemyMOVE, 200, NULL);
		SetTimer(hWnd, TIME_Bullet, 100, NULL);
		SetTimer(hWnd, TIME_BulletMOVE, 50, NULL);
		SetTimer(hWnd,TIME_EnemyRELEASE,300,NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			if (pPlane->x > 0)
				pPlane->x -= 20;
			break;
		case VK_RIGHT:
			if (pPlane->x < 550)
				pPlane->x += 20;
			break;
		case VK_UP:
			if (pPlane->y >0)
				pPlane->y -= 20;
			break;
		case VK_DOWN:
			if (pPlane->y < 550)
				pPlane->y += 20;
			break;
		case VK_SPACE:
			CreateBullet();
			break;
		case VK_RETURN:
			int n = 0;
				if (n = 0)
				{
					KillTimer(hWnd, TIME_Enemy);
					break;
				}
			  if (n = 1)
			  {
				SetTimer(hWnd, TIME_Enemy, 1500, NULL);
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

void CreateEnemy()
{
	Enemy *u;
	u = (struct Node*)malloc(sizeof(struct Node));
	u->x = rand() % 550;
	u->y = -10;
	u->pnext = NULL;
	if (NULL == pEnemy)
	{
		pEnemy = u;
	}
	else
	{
		u->pnext = pEnemy;
		pEnemy = u;
	}
}

void ShowEnemy(struct Node *pHead,HWND hWnd)
{
	HDC hDC;
	HDC memDC;
	HBITMAP hBitmap;
	BITMAP bBitmap;
	hDC = GetDC(hWnd);
	hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_EM));
	GetObject(hBitmap, sizeof(hBitmap), &bBitmap);
	memDC = CreateCompatibleDC(hDC);
	SelectObject(memDC, hBitmap);
	while (pHead!=NULL)
	{
		// Transparent mask method
		BitBlt(hDC, pHead->x, pHead->y, 40, 40, memDC, 40, 0, SRCAND);
		BitBlt(hDC, pHead->x, pHead->y, 40, 40, memDC, 0, 0, SRCPAINT);
		pHead = pHead->pnext;
	}
	DeleteDC(memDC);
	ReleaseDC(hWnd, hDC);
	DeleteObject(hBitmap);
}

void CreateSelf(HWND hWnd)
{
	HDC hDC;
	HDC memDC;
	HBITMAP hBitmap;
	BITMAP bBitmap;
	hDC = GetDC(hWnd);
	hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_SF));
	GetObject(hBitmap, sizeof(hBitmap), &bBitmap);
	memDC = CreateCompatibleDC(hDC);
	SelectObject(memDC, hBitmap);

	BitBlt(hDC, pPlane->x, pPlane->y, 40, 40, memDC, 40, 0, SRCAND);
	BitBlt(hDC, pPlane->x, pPlane->y, 40, 40, memDC, 0, 0, SRCPAINT);
	DeleteDC(memDC);
	ReleaseDC(hWnd, hDC);
	DeleteObject(hBitmap);
}

void CreateBullet()
{
	Bullet *u;
	u = (Bullet*)malloc(sizeof(Bullet));
	u->x = pPlane->x+20;
	u->y = pPlane->y+10;
	u->pnext = NULL;
	if (NULL == pBullet)
	{
		pBullet = u;
	}
	else
	{
		u->pnext = pBullet;
		pBullet = u;
	}
}

void ShowBullet(Bullet *pHead,HWND hWnd)
{
	HDC hDC;
	HDC memDC;
	HBITMAP hBitmap;
	BITMAP bBitmap;
	hDC = GetDC(hWnd);
	hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BL));
	GetObject(hBitmap, sizeof(hBitmap), &bBitmap);
	memDC = CreateCompatibleDC(hDC);
	SelectObject(memDC, hBitmap);
	while (pHead != NULL)
	{
		BitBlt(hDC, pHead->x, pHead->y, 15, 25, memDC, 15, 0, SRCAND);
		BitBlt(hDC, pHead->x, pHead->y, 15, 25, memDC, 0, 0, SRCPAINT);
		pHead = pHead->pnext;
	}
	DeleteDC(memDC);
	ReleaseDC(hWnd, hDC);
	DeleteObject(hBitmap);
}

void EnemyMove(Enemy *pHead)
{
	while (pHead!=NULL)
	{
		pHead->y += EnemyMoveSpeed;
		pHead = pHead->pnext;
	}
}

void BulletMove(Bullet *pHead)
{
	while (pHead != NULL)
	{
		pHead->y -= 20;
		pHead = pHead->pnext;
	}
}

void Shoot(HWND hWnd, Plane *self, Enemy **ppEnemy, Bullet **ppBullet)
{
	Enemy *js1 = *ppEnemy;
	Bullet *js2 = *ppBullet;
	int n = 1;
	// Judge whether it hits
	while (js1 != NULL)
	{
		if ((self->x - js1->x<30 && self->x - js1->x>-38) && (self->y-js1->y<25&& self->y-js1->y>-38))
		{
			KillTimer(hWnd, TIME_Enemy);
			KillTimer(hWnd, TIME_Bullet);
			KillTimer(hWnd, TIME_EnemyMOVE);
			KillTimer(hWnd, TIME_BulletMOVE);
			KillTimer(hWnd, TIME_EnemyRELEASE);
			MessageBox(hWnd, L"Game Over! XD", L"AirPlane Massage", MB_OK);
			PostQuitMessage(0);
			break;
		}
		else
		{
			js1 = js1->pnext;
		}
	}

	js1 = *ppEnemy;
	js2 = *ppBullet;
	while ((js1 = *ppEnemy) != NULL)
	{
		ppBullet = &pBullet;
		n = 0;
		while ((js2 = *ppBullet) != NULL)
		{
			if ((js2->x - js1->x <= 40 && js2->x - js1->x >= -5) && (js2->y - js1->y <= 40 && js2->y - js1->y >= -5))
			{
				n = 1;
				score += 100;
				*ppBullet = js2->pnext;
				if (js1->pnext != NULL)
				{
					*ppEnemy = js1->pnext;
					ppEnemy = &pEnemy;
					free(js1);
					free(js2);
				}
				else
					*ppEnemy = NULL;	
				break;
			}
			else
				ppBullet = &js2->pnext;
		}
		if (n != 1)
			ppEnemy = &js1->pnext;
	}
}
 
void ReleaseBullet(Bullet **pHead)
{
	Bullet *js = *pHead;
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

void ReleaseEnemy(Enemy **pHead)
{
	Bullet *js = *pHead;
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

void EnemyMoveChange()
{
	if (shootCount == 20)
        {
		EnemyMoveSpeed += 5;
		shootCount = 0;
        }

}
