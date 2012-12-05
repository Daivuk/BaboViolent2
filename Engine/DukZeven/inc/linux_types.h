#ifndef __LINUX_TYPES_H__
#define __LINUX_TYPES_H__

typedef void * LPVOID;
typedef unsigned long HRESULT;

#define APIENTRY


#define SUCCEEDED(o) ((o) == 0)
#define FAILED(o) ((o) != 0)

typedef unsigned long HWND;
typedef unsigned long HINSTANCE;
typedef bool BOOL;
typedef unsigned long HDC;
typedef unsigned long HGLRC;

#define TRUE 1
#define FALSE 0


typedef unsigned long LRESULT;
typedef unsigned int UINT;
typedef unsigned short WPARAM;
typedef unsigned long LPARAM;


struct POINT
{
  long x;
  long y;

  inline POINT() : x(0), y(0)
  {
  }

  inline POINT(long _x, long _y) : x(_x), y(_y)
  {
  }

  inline POINT(const POINT & rp)
  {
	x = rp.x;
	y = rp.y;
  }
};


typedef POINT * LPPOINT;

struct RECT
{
  long left;
  long top;
  long right;
  long bottom;

  inline RECT() : left(0), top(0), right(0), bottom(0)
  {
  }

  inline RECT(long l, long t, long r, long b) : left(l), top(t), right(r), bottom(b)
  {
  }
  inline RECT(const RECT & rr)
  {
	left = rr.left;
	top = rr.top;
	right = rr.right;
	bottom = rr.bottom;
  }
};

typedef RECT * LPRECT;

struct DIMOUSESTATE2
{
  long lX;
  long lY;
  long lZ;
  unsigned char rgbButtons[8];
  
};

typedef  DIMOUSESTATE2 * LPDIMOUSESTATE2;


#endif

