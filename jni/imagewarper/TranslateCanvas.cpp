#include <algorithm>
#include "TranslateCanvas.h"

namespace ImageWarper
{
	/*---------------------------------------------------------------------*/

	/*---------------------------------------------------------------------*/

	TranslateCanvas::TranslateCanvas(Rectangle &boundary, int iMaxWidth, int iMaxHeight)
	 : WarperCanvas(boundary, iMaxWidth, iMaxHeight)
	{

	}

	TranslateCanvas::~TranslateCanvas()
	{

	}

	/*---------------------------------------------------------------------*/

	/*---------------------------------------------------------------------*/

	void TranslateCanvas::DoForce(Point &ptStart, Point &ptEnd, 
								  PointArray2D *gridPoints, Rectangle &boundary)
	{
		Point ptDelta(ptEnd.X - ptStart.X, ptEnd.Y - ptStart.Y);
		float fCrossDistance = (float)std::min(
			std::max(ptStart.X - boundary.X, boundary.Right() - ptStart.X),
			std::max(ptStart.Y - boundary.Y, boundary.Bottom() - ptStart.Y));
		float fDist;
		Point *ptTmp;

		// Calculating the new coordinate of the grid points.
		for (int i = gridPoints->GetWidth() - 1; i >= 1; --i)
		{

//			__android_log_print(ANDROID_LOG_FATAL, "LOG", "NativeFilters %d  %d  %d", i, gridPoints->GetWidth(), gridPoints->GetHeight());
			for (int j = gridPoints->GetHeight() - 1; j >= 1; --j)
			{
				ptTmp = gridPoints->GetItem(i, j);
				fDist = (Distance(*ptTmp, ptStart) / fCrossDistance) - 1;
				if (fDist < 0)
				{
					ptTmp->X += (int)(ptDelta.X * fDist);
					ptTmp->Y += (int)(ptDelta.Y * fDist);
				}
			}
		}
	}


}

