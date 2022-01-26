// bitblt.cpp : 定义应用程序的入口点。
//
#include "framework.h"
#include "Cpp3D.h"
#include <cstddef>
#include <thread>
#include <algorithm>
#include <map>
using namespace std;
enum class WriteBy { Thread = 1, None = 0 };

class Img {

public:
    BYTE* g_pBits;
    HDC MemDC;
    HBITMAP Bmp, OldBmp;
    RECT* rect;
    HDC hdc;
    int iWidth;
    int iHeight;

    void ready()
    {
        MemDC = ::CreateCompatibleDC(hdc);
        iWidth = rect->right - rect->left;
        iHeight = rect->bottom - rect->top;
        BYTE bmibuf[sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD)];
        memset(bmibuf, 0, sizeof(bmibuf));
        BITMAPINFO* pbmi = (BITMAPINFO*)bmibuf;
        // BITMAPINFO pbmi;
        pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth = iWidth;
        pbmi->bmiHeader.biHeight = iHeight;
        pbmi->bmiHeader.biPlanes = 1;
        pbmi->bmiHeader.biBitCount = 24;
        pbmi->bmiHeader.biCompression = BI_RGB;
        Bmp = ::CreateDIBSection(MemDC, pbmi, DIB_RGB_COLORS, (void**)&g_pBits, 0, 0);
        OldBmp = (HBITMAP)::SelectObject(MemDC, Bmp);
    }

    void show() {
        BitBlt(hdc, 0, 0, iWidth, iHeight, MemDC, 0, 0, SRCCOPY);
        SelectObject(MemDC, OldBmp);
    }

    ~Img() {
        SelectObject(MemDC, OldBmp);
        DeleteObject(Bmp);
        DeleteDC(MemDC);
    }
    Img(RECT* re, HDC dc, WriteBy wb = WriteBy::None) {
        rect = re;
        hdc = dc;
        ready();
        if (wb == WriteBy::Thread) {
            std::thread SWer([](Img* img) {for (int i = 0; i < 1000; ++i) img->show(); }, this);
            SWer.detach();
        }
    }
    BYTE* operator ()(int x, int y) {
        return &(g_pBits[y * iWidth * 3 + x * 3]);
    }
};

template<typename T = long double, size_t Len = 3>
struct Vector {
    using vec = Vector<T,Len>;
    T data[Len];
    T& operator[](size_t at) {
        return data[at];
    }
#pragma region +-* --&
    vec operator+(vec& v) {
        vec t;
        for (size_t i = 0; i < Len; ++i) {
            t[i] = data[i] + v[i];
        }
        return t;
    }
    vec operator-(vec& v) {
        vec t;
        for (size_t i = 0; i < Len; ++i) {
            t[i] = data[i] - v[i];
        }
        return t;
    }
    T operator*(vec& v) {
        T re = 0;
        for (size_t i = 0; i < Len; ++i) {
            re += data[i] * v[i];
        }
        this->data[0];
        return re;
    }
#pragma endregion
#pragma region +-* --&&

    vec operator+(vec&& v) {
        vec t;
        for (size_t i = 0; i < Len; ++i) {
            t[i] = data[i] + v[i];
        }
        return t;
    }
    vec operator-(vec&& v) {
        vec t;
        for (size_t i = 0; i < Len; ++i) {
            t[i] = data[i] - v[i];
        }
        return t;
    }
    T operator*(vec&& v) {
        T re = 0;
        for (size_t i = 0; i < Len; ++i) {
            re += data[i] * v[i];
        }
        this->data[0];
        return re;
    }
#pragma endregion

    
    vec operator*(T v) {
        vec re;
        for (size_t i = 0; i < Len; ++i) {
            re[i] = data[i] * v;
        }
        this->data[0];
        return re;
    }


    template<typename JD = long double>
    JD Length() {
        JD re = 0;
        for (size_t i = 0; i < Len; ++i) {
            re += data[i] * data[i];
        }
        this->data[0];
        auto sadf = sqrtl(re);
        return sqrtl(re);
    }

};
template<typename T = double>
struct Camera {
    Camera(T X = 1, T Y = 1, T Z = 1) {
        x = X, y = Y, z = Z;
    }
    T x, y, z;
};

template<typename T = double>
class PointSet {
public:
    size_t Size = 0;
    std::map<size_t, Vector<T, 3> > points;
    bool GetNewPoint(T& x, T& y, T& z, size_t Index) {
        auto On = points[Index];
        x = On[0], y = On[1], z = On[2];
        return Index != points.size();
    }
    void SetNewPoint(T x, T y, T z) {
        points[Size++] = Vector<T, 3>{ x, y, z };
    }

    bool GetNewPoint(Vector<T, 3>& point, size_t Index) {
        
        point = points[Index];
        return Index < Size;
    }
    void SetNewPoint(Vector<T, 3>&& point) {
        points[Size++] = point;
    }
};

namespace exVector {
    template<typename T, size_t Len>
    inline T DegOf(Vector<T, Len> a, Vector<T, Len> b) {
        auto ji = a * b;
        auto lena = a.Length();
        auto lenb = b.Length();
        return acosl(ji/lena/lenb);
    }
    template<typename T>
    constexpr Vector<T, 3> NullVec{ 0,0,0 };
}using namespace exVector;

template<typename T = double>
class Cunity {
public:
    using Vec = Vector<T, 3>;
    Vec poi, cam;//O and cam
    PointSet<T>* data;
    Cunity(PointSet<T>* Data, Vec&& Cam, Vec&& Poi = Vec{ 0,0,0 }) {
        data = Data, cam = Cam, poi = Poi;
    }
    void ShowOn(Img& img) {
        auto camlen = cam.Length();
        Vec tVec = Vec{ 0, 0, 1 };
        Vec stdvec, point, stdpoi;
        size_t Index = 0;
        auto camtim = cam * tVec;
        long double deg = acosl(camtim / camlen);
        stdvec = Vec{ cam[0],cam[1],cam[2] - (tanl(deg) + 1) };
        while (data->GetNewPoint(point, Index++)) {
            point = point - poi;
            long double CosR = (cam * point) / (camlen * point.Length());
            long double R = acosl(CosR);
            stdpoi = point-cam * (CosR * point.Length()) * (1 / camlen);
            long double ST = DegOf(stdpoi, stdvec);
            if(ST==ST)
            img(100*ST, 100*R)[0] = 255;

        }
    }
};


#include "framework.h"
#include "Cpp3D.h"
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
        Img h(new RECT{ 20,20,160,160 }, hdc, WriteBy::None);

        PointSet<long double> poi;
        for (int i = 0; i < 120; i++)
            for (int j = 0; j < 120; ++j)
                for (int k = 0; k < 120; ++k)
                    poi.SetNewPoint(i, j, k);
        Cunity<long double> cun(&poi, Vector<long double, 3>{150, 150, 100}, Vector<long double, 3>{-150, -150, -100});
        while(!(GetKeyState(VK_UP))) {
            POINT po;
            GetCursorPos(&po);
            int i = po.x;
            cun.cam[2] = i;
            cun.poi[2] = -100;
            cun.ShowOn(h);
            h.show();
            memset(h.g_pBits, 0, sizeof(h.g_pBits));
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