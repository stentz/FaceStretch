#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "WarperUtil.h"

namespace ImageWarper
{
	/*-------------------------------------------------------------------------*/
	// ImageData
	/*-------------------------------------------------------------------------*/

	ImageData::ImageData() 
	: Data(0), Width(0), Height(0), ScanWidth(0), Bpp(0)
	{	}

	ImageData::ImageData(int width, int height, int scanWidth, int bpp)
	: Data(0), Width(0), Height(0), ScanWidth(0), Bpp(0)
	{
		Resize(width, height, bpp, scanWidth);
	}

	ImageData::ImageData(ImageData &imgData)
	: Data(0), Width(0), Height(0), ScanWidth(0), Bpp(0)
	{
		Resize(imgData.Width, imgData.Height, imgData.Bpp, imgData.ScanWidth);
		memcpy(Data, imgData.Data, imgData.ScanWidth * imgData.Height);
	}

	ImageData::~ImageData()
	{
		if(Data)
			free(Data);
	}
	void ImageData::Resize(int width, int height, int bpp, int scanWidth /*= 0*/)
	{
		assert(width > 0 && height > 0 && bpp >= 0 && scanWidth >= 0 && 
			(scanWidth == 0 || scanWidth >= width*bpp));

		if (Width == width && Height == height)
			return;

		Width = width;
		Height = height;
		Bpp = bpp;
		if (!scanWidth)
		{
			scanWidth = ((Width*Bpp + sizeof(int) - 1)/ sizeof(int))*sizeof(int);
		}
		ScanWidth = scanWidth;
		int iLen = ScanWidth * Height;

		if (Data)
		{
			Data = (unsigned char*)realloc(Data, iLen);
		}
		else
		{
			Data = (unsigned char*)malloc(iLen);
		}
	}

	/*-------------------------------------------------------------------------*/
	// Array2D
	/*-------------------------------------------------------------------------*/

	template<typename T> 
	Array2D<T>::Array2D(int width, int height)
	{
		m_iWidth = width;
		m_iHeight = height;

		int cnt = m_iWidth*m_iHeight;
		m_data = new T[cnt];
	}

	template<typename T> 
	Array2D<T>::~Array2D()
	{
		delete[] m_data;
	}

	template<typename T>
	T* Array2D<T>::GetItem(int i, int j)
	{
		assert(i >= 0 && i < m_iWidth && j >= 0 && j < m_iHeight);
		return (m_data + (j*m_iWidth + i));
	}

	template<typename T>
	void Array2D<T>::SetItem(int i, int j, T &item)
	{
		assert(i >= 0 && i < m_iWidth && j >= 0 && j < m_iHeight);
		m_data[j*m_iWidth + i] = item;
	}

	template<typename T> 
	int Array2D<T>::GetWidth()	
	{	return m_iWidth;	}

	template<typename T> 
	int Array2D<T>::GetHeight()	
	{	return m_iHeight;	}


	template class Array2D<Point>;
	template class Array2D<PointF>;
}
