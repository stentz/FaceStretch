#ifndef __WARPERUTIL_H__
#define __WARPERUTIL_H__

//#include <assert.h>

namespace ImageWarper
{
	class Point
	{
	public:
		int X, Y;

		Point():X(0), Y(0)
		{}
		Point(int x, int y):X(x), Y(y)
		{}
		~Point()
		{}
	};

	class PointF
	{
	public:
		float X, Y;

		PointF():X(0.0f), Y(0.0f)
		{}
		PointF(float x, float y):X(x), Y(y)
		{}
		~PointF()
		{}
	};

	class Rectangle
	{
	public:
		int X;
		int Y;
		int Width;
		int Height;

		Rectangle()
			:X(0), Y(0), Width(0), Height(0)
		{ }
		Rectangle(int x, int y, int width, int height)
			:X(x), Y(y), Width(width), Height(height)
		{ }
		Rectangle(Rectangle &rc)
			: X(rc.X), Y(rc.Y), Width(rc.Width), Height(rc.Height)
		{ }

		~Rectangle()
		{ }

		int Right()	{	return X + Width;	}
		int Bottom(){	return Y + Height;	}
		void Set(Rectangle& rc)
		{
			X = rc.X;	Y = rc.Y;
			Width = rc.Width;
			Height = rc.Height;
		}
		bool Contains(int x, int y)
		{
			return (x >= X && x < Right() && y >= Y && y < Bottom());
		}
		bool Contains(Point& pt)
		{
			return Contains(pt.X, pt.Y);
		}
	};

	/* Serialized data for an image */
	class ImageData
	{
	public:
		int Width;			/* Width of the image in pixels */
		int Height;			/* height of image in pixels */
		int ScanWidth;		/* the scan width */
		int Bpp;			/* number of bytes per pixel */
		unsigned char *Data;	/* The raw image data */


		ImageData();
		ImageData(int width, int height, int scanWidth, int bpp);
		ImageData(ImageData &imgData);
		~ImageData();

		void Resize(int width, int height, int bpp, int scanWidth = 0);
	};

	class WarpedImage
	{
	public:
		ImageData Image;
		Point Position;
	};

	/* Template class for 2D array */
	template<typename T>
	class Array2D
	{
		T	*m_data;
		int m_iWidth;
		int m_iHeight;

	public:
		Array2D(int width, int height);
		~Array2D();

		T* GetItem(int i, int j);
		void SetItem(int i, int j, T &item);

		int GetWidth();
		int GetHeight();
	};

	typedef Array2D<Point> PointArray2D;
	typedef Array2D<PointF> PointFArray2D;
}

#endif
