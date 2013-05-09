#include <assert.h>
#include <algorithm>
#include <math.h>

#include "Warper.h"
#include "TranslateCanvas.h"
#include "GrowCanvas.h"

namespace ImageWarper
{
	Warper::Warper(ImageData &img)
	:m_canvas(0),
	m_bEdgeBlurring(false)
	{
		m_imgOriginal = new ImageData(img);
		m_warpedImage = new WarpedImage();
	}

	Warper::Warper(ImageData *img)
		:m_canvas(0),
		m_bEdgeBlurring(false),
		m_imgOriginal(img)
	{
		m_warpedImage = new WarpedImage();
	}

	Warper::~Warper()
	{
		if (m_imgOriginal)
			delete m_imgOriginal;
		if(m_warpedImage)
			delete m_warpedImage;
	}

	/*--------------------------------------------------------------------------*/
	// public interface
	/*--------------------------------------------------------------------------*/
	void Warper::BeginWarp(Point ptCenterPos, int iBrushSize, int iWarperType)
	{
		Point ptTopLeft(std::max(0, ptCenterPos.X - 2*iBrushSize),
			std::max(0, ptCenterPos.Y - 2*iBrushSize));
		Rectangle bound(ptTopLeft.X, ptTopLeft.Y,
			std::min(m_imgOriginal->Width - ptTopLeft.X, 4*iBrushSize),
			std::min(m_imgOriginal->Height - ptTopLeft.Y, 4*iBrushSize));

		m_ptCenterPos = ptCenterPos;

		// create the canvas
		if(m_canvas)
			delete m_canvas;

		switch (iWarperType)
		{
		case WARPER_TRANSLATE:
			m_canvas = new TranslateCanvas(bound, m_imgOriginal->Width, m_imgOriginal->Height);

//			__android_log_print(ANDROID_LOG_VERBOSE, "NativeFilters", "translate");
			break;
		case WARPER_GROW:
			m_canvas = new GrowCanvas(bound, m_imgOriginal->Width, m_imgOriginal->Height);
			break;
		case WARPER_SHRINK:
			m_canvas = new GrowCanvas(bound, m_imgOriginal->Width, m_imgOriginal->Height, false);
			break;
		default:
			assert(false);
		}

		// update the warpedImage (which will be returned by UpdateWarp() and EndWarp()
		m_warpedImage->Position.X = bound.X;
		m_warpedImage->Position.Y = bound.Y;
		m_warpedImage->Image.Resize(bound.Width, bound.Height, m_imgOriginal->Bpp);
	}

	WarpedImage* Warper::UpdateWarp(Point ptMouse)
	{
		if(!m_canvas)
		{
			return NULL;
		}
		
		// if the bounding box of the canvas is changed
		// then we will update the warpedImage
		if (m_canvas->Force(m_ptCenterPos, ptMouse))
		{
			Rectangle *canvasBounds = m_canvas->GetBoundary();
			m_warpedImage->Position.X = canvasBounds->X;
			m_warpedImage->Position.Y = canvasBounds->Y;
			m_warpedImage->Image.Resize(
				canvasBounds->Width, canvasBounds->Height,
				m_imgOriginal->Bpp);
		}
		
		Warper::OffsetFilter(m_imgOriginal, m_canvas->GetOffsetPoints(),
			*(m_canvas->GetBoundary()), &(m_warpedImage->Image));

		return m_warpedImage;
	}

	WarpedImage* Warper::EndWarp(WarpedImage *warpedImg)
	{
		assert(m_warpedImage == warpedImg);
		assert(m_warpedImage->Image.Bpp == m_imgOriginal->Bpp);
		
		Warper::FilterSmooth(m_imgOriginal, m_canvas->GetOffsetPoints(),
			*(m_canvas->GetBoundary()), &(warpedImg->Image));

		if (m_bEdgeBlurring)
		{
			EdgeBlur(m_imgOriginal, *(m_canvas->GetBoundary()), &(warpedImg->Image));
		}

		// update the original image
		ImageData &imgWarped = warpedImg->Image;
		int iLeftBound = warpedImg->Position.X;
		int iTopBound = warpedImg->Position.Y;

		unsigned char *pOriginal = m_imgOriginal->Data + 
			(iTopBound * m_imgOriginal->ScanWidth + iLeftBound*m_imgOriginal->Bpp);
		unsigned char *pNew = imgWarped.Data;
		for (int j = 0; j < imgWarped.Height; ++j)
		{
			memcpy(pOriginal, pNew, imgWarped.Width*imgWarped.Bpp);
			pOriginal += m_imgOriginal->ScanWidth;
			pNew += imgWarped.ScanWidth;
		}
		return warpedImg;
	}

	/*-------------------------------------------------------------------------------*/
	// private utilities
	/*-------------------------------------------------------------------------------*/
	bool Warper::OffsetFilter(ImageData *bmpOriginal, PointFArray2D *offset,
									 Rectangle &bounds, ImageData *bmpNew)
	{
		// stride
		int scanline = bmpOriginal->ScanWidth;

		unsigned char* p = bmpNew->Data;
		unsigned char* pSrc = bmpOriginal->Data;

		int nOffset = bmpNew->ScanWidth - bmpNew->Width * bmpNew->Bpp;
		int nWidth = bmpNew->Width;
		int nHeight = bmpNew->Height;

		int xOffset, yOffset, srcOffset;

		for (int y = 0; y < nHeight; ++y)
		{
			for (int x = 0; x < nWidth; ++x)
			{
				xOffset = std::min(std::max(0, (int)offset->GetItem(x, y)->X), bmpOriginal->Width - 1);
				yOffset = std::min(std::max(0, (int)offset->GetItem(x, y)->Y), bmpOriginal->Height - 1);
				srcOffset = (yOffset * scanline) + (xOffset * bmpOriginal->Bpp);

				for (int k = bmpOriginal->Bpp - 1; k >= 0; --k)
				{
					p[k] = pSrc[srcOffset + k];
				}

//				xOffset = (int)offset->GetItem(x, y)->X;
//				yOffset = (int)offset->GetItem(x, y)->Y;
//
//				if (yOffset >= 0 && yOffset < bmpOriginal->Height &&
//					xOffset >= 0 && xOffset < bmpOriginal->Width)
//				{
//					// inside the boundary
//					srcOffset = (yOffset * scanline) + (xOffset * bmpOriginal->Bpp);
//
//					for (int k = bmpOriginal->Bpp - 1; k >= 0; --k)
//					{
//						p[k] = pSrc[srcOffset + k];
//					}
//				}
//				else
//				{
//					for (int k = bmpOriginal->Bpp - 1; k >= 0; p[k--] = 0);
//				}

				p += bmpNew->Bpp;
			}
			p += nOffset;
		}
		return true;
	}

	bool Warper::FilterSmooth(ImageData *bmpOriginal, PointFArray2D *offset,
							  Rectangle &bounds, ImageData *bmpNew)
	{
		// stride
		int scanline = bmpOriginal->ScanWidth;

		unsigned char* p = (unsigned char*)bmpNew->Data;
		unsigned char* pSrc = (unsigned char*)bmpOriginal->Data;

		int nOffset = bmpNew->ScanWidth - bmpNew->Width * bmpNew->Bpp;
		int nWidth = bmpNew->Width;
		int nHeight = bmpNew->Height;

		float fXOffset, fYOffset, dXLeft, dXRight, dYTop, dYBottom;
		int xOffsetLeft, xOffsetRight, yOffsetTop, yOffsetBottom;
		int srcOffset00, srcOffset01, srcOffset10, srcOffset11;
		unsigned char px00, px01, px10, px11;
		float fPx0, fPx1;

		for (int y = 0; y < nHeight; ++y)
		{
			for (int x = 0; x < nWidth; ++x)
			{
				fXOffset = offset->GetItem(x, y)->X;
				fYOffset = offset->GetItem(x, y)->Y;
				xOffsetLeft =	std::min(std::max((int)floor(fXOffset), 0), bmpOriginal->Width - 1);
				xOffsetRight =	std::min(std::max(xOffsetLeft + 1, 0), bmpOriginal->Width - 1);
				yOffsetTop =	std::min(std::max((int)floor(fYOffset), 0), bmpOriginal->Height - 1);
				yOffsetBottom = std::min(std::max(yOffsetTop + 1, 0), bmpOriginal->Height - 1);

				srcOffset00 = (yOffsetTop * scanline) + (xOffsetLeft * bmpOriginal->Bpp);
				srcOffset01 = (yOffsetTop * scanline) + (xOffsetRight * bmpOriginal->Bpp);
				srcOffset10 = (yOffsetBottom * scanline) + (xOffsetLeft * bmpOriginal->Bpp);
				srcOffset11 = (yOffsetBottom * scanline) + (xOffsetRight * bmpOriginal->Bpp);

				if (fXOffset < xOffsetLeft || fXOffset > xOffsetRight)
				{
					fXOffset = (xOffsetLeft + xOffsetRight) / 2.0f;
				}
				if (fYOffset < yOffsetTop || fYOffset > yOffsetBottom)
				{
					fYOffset = (yOffsetTop + yOffsetBottom) / 2.0f;
				}

				dXLeft = fXOffset - xOffsetLeft;
				dXRight = xOffsetRight - fXOffset;
				if(dXLeft == 0 && dXRight == 0)
					dXLeft = 1;
				dYTop = fYOffset - yOffsetTop;
				dYBottom = yOffsetBottom - fYOffset;
				if (dYTop == 0 && dYBottom == 0)
					dYTop = 1;

				for (int k = bmpOriginal->Bpp - 1; k >= 0; --k)
				{
					px00 = pSrc[srcOffset00 + k];
					px01 = pSrc[srcOffset01 + k];
					px10 = pSrc[srcOffset10 + k];
					px11 = pSrc[srcOffset11 + k];

					fPx0 = px01 * dXLeft + px00 * dXRight;
					fPx1 = px11 * dXLeft + px10 * dXRight;
					p[k] = (unsigned char)(fPx0 * (dYBottom) + fPx1 * (dYTop));
					if(p[k] == 0)
					{
						p[k] = 0;
					}
				}

				//xOffsetLeft = (int)floor(fXOffset);
				//xOffsetRight = xOffsetLeft + 1;
				//yOffsetTop = (int)floor(fYOffset);
				//yOffsetBottom = yOffsetTop + 1;

				//if (yOffsetTop >= 0 && yOffsetBottom < bmpOriginal->Height &&
				//	xOffsetLeft >= 0 && xOffsetRight < bmpOriginal->Width)
				//{
				//	srcOffset00 = (yOffsetTop * scanline) + (xOffsetLeft * bmpOriginal->Bpp);
				//	srcOffset01 = (yOffsetTop * scanline) + (xOffsetRight * bmpOriginal->Bpp);
				//	srcOffset10 = (yOffsetBottom * scanline) + (xOffsetLeft * bmpOriginal->Bpp);
				//	srcOffset11 = (yOffsetBottom * scanline) + (xOffsetRight * bmpOriginal->Bpp);

				//	for (int k = bmpOriginal->Bpp - 1; k >= 0; --k)
				//	{
				//		px00 = pSrc[srcOffset00 + k];
				//		px01 = pSrc[srcOffset01 + k];
				//		px10 = pSrc[srcOffset10 + k];
				//		px11 = pSrc[srcOffset11 + k];

				//		fPx0 = px01 * (fXOffset - xOffsetLeft) + px00 * (xOffsetRight - fXOffset);
				//		fPx1 = px11 * (fXOffset - xOffsetLeft) + px10 * (xOffsetRight - fXOffset);
				//		p[k] = (unsigned char)(fPx0 * (yOffsetBottom - fYOffset) + fPx1 * (fYOffset - yOffsetTop));
				//	}
				//}
				//else
				//{
				//	for (int k = bmpOriginal->Bpp - 1; k >= 0; p[k--] = 0);
				//}

				p += bmpNew->Bpp;
			}
			p += nOffset;
		}

		return true;
	}

	void Warper::EdgeBlur(ImageData *bmpOriginal, 
							 Rectangle &bounds, ImageData *bmpNew)
	{
		assert(bmpOriginal->Bpp == bmpNew->Bpp);

		unsigned char *pNew = bmpNew->Data;
		unsigned char *pSrc = bmpOriginal->Data;
		int iNewStride = bmpNew->ScanWidth;
		int iSrcStride = bmpOriginal->ScanWidth;
		int iBbp = bmpOriginal->Bpp;
		int iSrcWidth = bmpOriginal->Width - 1;
		int iSrcHeight = bmpOriginal->Height - 1;
		float fSum;

		//(pSrc[std::max(0, iTopY - 1)*iSrcStride + std::max(0, i - 1)*iBbp + k] + \
		//pSrc[std::max(0, iTopY - 1)*iSrcStride + std::min(iSrcWidth, i + 1)*iBbp + k] + \
		//pSrc[std::min(iSrcHeight, iTopY + 1)*iSrcStride + std::max(0, i - 1)*iBbp + k] + \
		//pSrc[std::min(iSrcHeight, iTopY + 1)*iSrcStride + std::min(iSrcWidth, i + 1)*iBbp + k] + \
		//2*(	pSrc[std::max(0, iTopY - 1)*iSrcStride + i*iBbp + k] + \
		//pSrc[std::min(iSrcHeight, iTopY + 1)*iSrcStride + i*iBbp + k] + \
		//pSrc[iTopY*iSrcStride + std::max(0, i - 1)*iBbp + k] + \
		//pSrc[iTopY*iSrcStride + std::min(iSrcWidth, i + 1)*iBbp + k]) + \
		//4*	pSrc[iTopY*iSrcStride + i*iBbp + k]);

	#define FILTER_SUM(y, x, k)	(pSrc[std::max(0, y - 1)*iSrcStride + std::max(0, x - 1)*iBbp + k] + \
								pSrc[std::max(0, y - 1)*iSrcStride + std::min(iSrcWidth, x + 1)*iBbp + k] + \
								pSrc[std::min(iSrcHeight, y + 1)*iSrcStride + std::max(0, x - 1)*iBbp + k] + \
								pSrc[std::min(iSrcHeight, y + 1)*iSrcStride + std::min(iSrcWidth, x + 1)*iBbp + k] + \
							2*(	pSrc[std::max(0, y - 1)*iSrcStride + x*iBbp + k] + \
								pSrc[std::min(iSrcHeight, y + 1)*iSrcStride + x*iBbp + k] + \
								pSrc[y*iSrcStride + std::max(0, x - 1)*iBbp + k] + \
								pSrc[y*iSrcStride + std::min(iSrcWidth, x + 1)*iBbp + k]) + \
							4*	pSrc[y*iSrcStride + x*iBbp + k])

		for (int j = 1; j >= 0; --j)
		{
			int iTopY = bounds.Y + j;
			int iBottomY = bounds.Bottom() - j;
			
			// 2 top and bottom rows 
			for (int i = bounds.Right() - 1; i >= bounds.X; --i)
			{
				for (int k = 0; k < iBbp; ++k)
				{
					fSum = (float)FILTER_SUM(iTopY, i, k);
					pNew[j*iNewStride + (i-bounds.X)*iBbp + k] = (unsigned char)(fSum / 16.0f);
					fSum = (float)FILTER_SUM(iBottomY, i, k);
					pNew[(bounds.Height - j)*iNewStride + (i-bounds.X)*iBbp + k] = 
						(unsigned char)(fSum / 16.0f);
				}
			}

			int iLeftX = bounds.X + j;
			int iRightX = bounds.Right() - j;

			// 2 left and right columns
			for (int i = bounds.Bottom() - 1; i >= bounds.Y; --i)
			{
				for (int k = 0; k < iBbp; ++k)
				{
					fSum = (float)FILTER_SUM(i, iLeftX, k);
					pNew[(i-bounds.Y)*iNewStride + j*iBbp + k] = (unsigned char)(fSum / 16.0f);
					fSum = (float)FILTER_SUM(i, iRightX, k);
					pNew[(i-bounds.Y)*iNewStride + (bounds.Width - j)*iBbp + k] = 
						(unsigned char)(fSum / 16.0f);
				}
			}
		}
	}
}

