// bitblt.cpp : 定义应用程序的入口点。
//
#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include<iostream>
#include "Cpp3D.h"
#include <cstddef>
#include <thread>
#include <algorithm>
#include <map>
#include<cmath>
#include "framework.h"
#include "Cpp3D.h"
#include"3DFramework.hpp"
#include <cstddef>
using namespace std;
#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];
// 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

struct pag
{
    HDC hdc;
    int iWidth;
    int iHeight;
    HDC MemDC;
};


DWORD WINAPI T(LPVOID iP)
{
    pag p = *((pag*)iP);
    //BitBlt(p.hdc, 0, 0, p.iWidth, p.iHeight, p.MemDC, 0, 0, SRCCOPY);


    return 0;
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CPP3D, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPP3D));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}
HBITMAP CopyBitmap(HBITMAP hbm, HDC dc) {
    HDC hdcSrc = CreateCompatibleDC(NULL);
    HDC hdcDst = CreateCompatibleDC(NULL);
    HBITMAP hbmOld, hbmOld2, hbmNew;
    BITMAP bm;
    GetObject(hbm, sizeof(bm), &bm);
    hbmOld = (HBITMAP)SelectObject(hdcSrc, hbm);
    hbmNew = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes,
        bm.bmBitsPixel,
        NULL);
    hbmOld2 = (HBITMAP)SelectObject(hdcDst, hbmNew);
    BitBlt(hdcSrc, 0, 0, bm.bmWidth, bm.bmHeight, dc, 0, 0, SRCCOPY);
    SelectObject(hdcSrc, hbmOld);
    SelectObject(hdcDst, hbmOld2);
    DeleteDC(hdcSrc);
    DeleteDC(hdcDst);
    return hbmNew;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPP3D));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CPP3D);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        0, 0, 2000, 1000, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        HDC hdc;
        PAINTSTRUCT ps;
        HDC hdcMem;
        HBITMAP hbmp;	//一张位图的句柄
        BITMAP bmp;
        hdc = BeginPaint(hWnd, &ps);	//为指定句柄hwnd窗口进行绘画准备工作
        for (int i = 0; i < 1000; ++i) {
            int gx = 50, gy = 60,gz=70;
        Img h(new RECT{ 20,20,1600,700 }, hdc, WriteBy::None);
            for (int i = 20; i < 1500; ++i)
               for (int j = 20; j < 480; ++j)
                    h(i, j,1)[0] = 255, h(i, j,1)[1] = 255, h(i, j,1)[2] = 255;
        //memset(h.g_pBits, 255, 1000);
        POINT poi, op; GetCursorPos(&op);
        while(GetCursorPos(&poi),pow(poi.x-op.x,2)+pow(poi.y-op.y,2)<0)op=poi;
        
        double a = poi.x * 0.01, b = poi.y * 0.01;
        /*
        for (int ei = 0; ei < 100; ++ei)
            for (int ej = 0; ej < 100; ++ej)
                for (int ek = 0; ek < 100; ++ek) {
                    double i = ei * cos(b) - ek * sin(b), j = ej, k = ek * cos(b) + ei * sin(b);;
                    double farf = (pow(i - gx, 2) / 100 + pow(j - gy, 2) / 100 + pow(k - gz, 2) / 100 + 0.01);

                    h(i + 80, 100 + j * cos(a) + k * sin(a),2)[0] = ( 6555 / farf) / 2.00;
                }
        */

            for (int ei = 0; ei < 100; ++ei)
                for (int ej = 0; ej < 100; ++ej)
                    for (int ek = 0; ek < 100; ++ek) {
                        double i = ei * cos(b) - ek * sin(b), j=ej,k = ek * cos(b) + ei * sin(b);;
                        //double farf = (pow(i - gx, 2)/100 + pow(j - gy, 2)/100 + pow(k-gz,2)/100+0.01);

                        h(i + 180, 200 + j * cos(a) + k * sin(a))[0] = 255*(ei + ej + ek)/300;;
                    }
            h.show();
        }
        EndPaint(hWnd, &ps);

    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}