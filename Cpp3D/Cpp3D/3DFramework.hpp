#include<bits/stdc++.h>
#include<bit>
#include<bitset>
#include<Windows.h>
#include<thread>
#define setc(h,i,j,r,g,b) h(i,j)[0]
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
        memset(bmibuf, 255, sizeof(bmibuf));
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
    BYTE* operator ()(int x, int y,double big=1) {
        x *= big, y *= big;
        return &(g_pBits[y * iWidth * 3 + x * 3]);
    }
};

template<typename T = long double, size_t Len = 3>
struct Vector {
    using vec = Vector<T, Len>;
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
        return acosl(ji / lena / lenb);
    }
    template<typename T>
    Vector<T, 3> NullVec{ 0,0,0 };
}using namespace exVector;

template<typename T = long double>
class Cunity {
public:
    using Vec = Vector<T, 3>;
    Vec poi, cam;//O and cam
    PointSet<T>* data;
    Cunity(PointSet<T>* Data, Vec Cam, Vec Poi = Vec{ 0,0,0 }) {
        data = Data, cam = Cam, poi = Poi;
    }
    void ShowOn(Img& img) {
        auto camlen = cam.Length();
        Vec tVec = Vec{ 0, 0, 1 };
        Vec stdvec, point, stdpoi;
        size_t Index = 0;
        long double camtim = cam * tVec;
        long double deg = acosl(camtim / camlen);
        stdvec = Vec{ cam[0],cam[1],cam[2] - (tanl(deg) + 1) };
        while (data->GetNewPoint(point, Index++)) {
            point = point - poi;
            point = point - cam;
            long double CosR = (cam * point) / (camlen * point.Length());
            long double R = acosl(CosR);
            stdpoi = point - cam * (CosR * point.Length()) * (1.0000 / camlen);
            long double ST = DegOf(stdpoi, stdvec);
            if (ST == ST)
                img(min(500.0000 * ST, 4700), min(500.0000 * R, 470))[0] = 255;

        }
    }
};