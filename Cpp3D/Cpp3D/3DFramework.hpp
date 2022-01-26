#pragma once

#include "framework.h"
#include "Cpp3D.h"
#include <cstddef>
#include <thread>
#include <algorithm>
#include <map>
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

    ~Img(){
        SelectObject(MemDC, OldBmp);
        DeleteObject(Bmp);
        DeleteDC(MemDC);
    }
    Img(RECT* re, HDC dc,WriteBy wb=WriteBy::None) {
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

template<typename T,size_t Len>
struct Vector {
    using vec = Vector<T, Len>;
    T data[Len];
    T& operator[](size_t at) {
        return data[at];
    }
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
        return re;
    }
    vec operator*(T v) {
        vec re;
        for (size_t i = 0; i < Len; ++i) {
            re[i] = data[i] * v;
        }
        return re;
    }
    template<typename JD = double>
    JD Length() {
        JD re = 0;
        for (size_t i = 0; i < Len; ++i) {
            re += data[i] * data[i];
        }
        return sqrt(re);
    }

};
template<typename T=double> 
struct Camera {
    Camera(T X=1, T Y=1, T Z=1) {
        x = X, y = Y, z = Z;
    }
    T x, y, z;
};

template<typename T = double>
class PointSet {
public:
    size_t Size = 0;
    std::map<size_t,Vector<T, 3> > points;
    bool GetNewPoint(T& x, T& y, T& z,size_t Index) {
        auto On = points[Index];
        x = On[0], y = On[1], z = On[2];
        return Index != points.size();
    }
    void SetNewPoint(T x, T y, T z) {
        points[Size++] = Vector<T, 3>{ x, y, z };
    }

    bool GetNewPoint(Vector<T,3>& point, size_t Index) {
        point = points[Index];
        return Index != points.size();
    }
    void SetNewPoint(Vector<T, 3>&& point) {
        points[Size++] = point;
    }
};

namespace exVector {
    template<typename T,size_t Len>
    inline T DegOf(Vector<T, Len> a, Vector<T, Len> b) {
        return acosl((a * b) / (a.Length() * b.Length()));
    }
    template<typename T>
    constexpr Vector<T, 3> NullVec{ 0,0,0 };
}using namespace exVector;

template<typename T=double>
class Cunity {
public:
    Vector<T,3> poi,cam;//O and cam
    PointSet<T>* data;
    Cunity(PointSet<T>* Data, Vector<T, 3>&& Cam, Vector<T, 3>&& Poi = Vector<T, 3>{ 0,0,0 }) {
        data = Data, cam = Cam, poi = Poi;
    }
    void ShowOn(Img& img) {
        auto camlen = cam.Length();
        Vector<T, 3> stdvec,point,stdpoi;
        size_t Index;
        auto camtim = cam * (Vector<T, 3>{0, 0, 1});
        long double deg = acosl(camtim / camlen);
        stdvec = cam * (tanl(deg) + 1);
        while (data->GetNewPoint(point, Index++)) {
            point = point - poi;
            long double CosR = (cam * point) / (camlen * point.Length());
            long double R = acosl(CosR);
            stdpoi = cam * (CosR * point.Length()) * (1 / camlen);
            long double ST = DegOf(stdpoi, stdvec);

            img(ST, R)[0] = 255;
        }
    }
};