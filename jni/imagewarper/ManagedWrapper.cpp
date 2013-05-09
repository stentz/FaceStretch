/*===================================================================================*/
// This file defines some static routines so that ImageWarper can be used in managed
// environment, and for simpler interface 
// without the need of managing actual processing classes.
//
// Freely use at your own risk.
//
// Vu Pham
//
/*===================================================================================*/

#include <algorithm>
#include "ManagedWrapper.h"

using namespace ImageWarper;

int CreateWarper(int width, int height, int scanWidth, int bpp, unsigned char* rawData)
{
	ImageData* imgData = new ImageData(width, height, scanWidth, bpp);
	memcpy(imgData->Data, rawData, height*scanWidth);
	Warper* warper = new Warper(imgData);
	int iWarperId = getNextWarperId();
	m_warpers[iWarperId] = warper;
	return iWarperId;
}

int ReleaseWarper(int warperId)
{
	std::map<int, Warper*>::iterator it = m_warpers.find(warperId);
	if (it != m_warpers.end())
	{
		delete it->second;
		m_warpers.erase(it);
	}
	return 0;
}

void BeginWarp(int warperId, int centerX, int centerY, int brushSize, int warperType)
{
	Warper* warper = getMapItem<Warper*>(m_warpers, warperId);
	if (warper)
	{
		warper->BeginWarp(Point(centerX, centerY), brushSize, warperType);
	}
}

unsigned char* UpdateWarp(int warperId, int x, int y, 
				int *xRet, int *yRet, int *width, int *height, int* scanWidth)
{
	if(xRet && yRet && width && height && scanWidth)
	{
		Warper* warper = getMapItem<Warper*>(m_warpers, warperId);
		if (warper)
		{
			WarpedImage* warpedImg = warper->UpdateWarp(Point(x, y));
			*xRet = warpedImg->Position.X;
			*yRet = warpedImg->Position.Y;
			*width = warpedImg->Image.Width;
			*height = warpedImg->Image.Height;
			*scanWidth = warpedImg->Image.ScanWidth;
			
			m_warpedImages[warperId] = warpedImg;

			return warpedImg->Image.Data;
		}
	}
	return NULL;
}

unsigned char* EndWarp(int warperId, 
						  int *xRet, int *yRet, int *width, int *height, int* scanWidth)
{
	if(xRet && yRet && width && height && scanWidth)
	{
		Warper* warper = getMapItem<Warper*>(m_warpers, warperId);
		WarpedImage* warpedImg = getMapItem<WarpedImage*>(m_warpedImages, warperId);
		
		if (warper && warpedImg)
		{
			warpedImg = warper->EndWarp(warpedImg);
			*xRet = warpedImg->Position.X;
			*yRet = warpedImg->Position.Y;
			*width = warpedImg->Image.Width;
			*height = warpedImg->Image.Height;
			*scanWidth = warpedImg->Image.ScanWidth;

			std::map<int, WarpedImage*>::iterator it = m_warpedImages.find(warperId);
			if (it != m_warpedImages.end())
			{
				m_warpedImages.erase(it);
			}
			return warpedImg->Image.Data;
		}
	}
	return NULL;
}

/*======================================================================*/
// Private utilities functions
/*======================================================================*/

int getNextWarperId()
{
	int iId = 0;

	for( ; m_warpers.find(iId) != m_warpers.end(); iId++);
	return iId;
}

template<class T>
static T getMapItem(std::map<int, T> &map, int iId)
{

	typedef typename std::map<int, T>::iterator m_iterator;
	m_iterator	it = map.find(iId);
	if(it != map.end())
		return it->second;
	return NULL;
}
