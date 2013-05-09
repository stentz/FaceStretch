#include <algorithm>
#include <cmath>
#include "GrowCanvas.h"

namespace ImageWarper
{
	#define GROWSTEP 0.05f

	/*---------------------------------------------------------------------*/

	/*---------------------------------------------------------------------*/

	GrowCanvas::GrowCanvas(Rectangle &boundary, int iMaxWidth, int iMaxHeight, bool bGrow /*= true*/)
	: WarperCanvas(boundary, iMaxWidth, iMaxHeight),
	m_GrowFactor(0.0f),
	m_bGrow(bGrow)
	{

	}

	GrowCanvas::~GrowCanvas()
	{

	}

	/*---------------------------------------------------------------------*/

	/*---------------------------------------------------------------------*/


	void GrowCanvas::DoForce(Point &ptStart, Point &ptEnd, 
								  PointArray2D *gridPoints, Rectangle &boundary)
	{
		float fCrossDistance = (float)std::min(
			std::min(ptEnd.X - boundary.X, boundary.Right() - ptEnd.X),
			std::min(ptEnd.Y - boundary.Y, boundary.Bottom() - ptEnd.Y));
		Point *ptTmp;
		float r;

		if (m_GrowFactor < 1)
			m_GrowFactor += GROWSTEP;

		if (m_bGrow)
		{
			// Calculating the new coordinate of the grid points.
			for (int i = gridPoints->GetWidth() - 1; i >= 0; --i)
			{
				for (int j = gridPoints->GetHeight() - 1; j >= 0; --j)
				{
					ptTmp = gridPoints->GetItem(i, j);
					r = Distance(*ptTmp, ptEnd.X, ptEnd.Y)/fCrossDistance;

					if (r < 1)
					{
						r = pow(r, m_GrowFactor);
						ptTmp->X = ptEnd.X + (int)((ptTmp->X - ptEnd.X)*r);
//						__android_log_print(ANDROID_LOG_FATAL, "LOG", "grow canvas: %d", (int)((ptTmp->X - ptEnd.X)*r));
//						ptTmp->X = ptStart.X + 15.005;
						ptTmp->Y = ptEnd.Y + (int)((ptTmp->Y - ptEnd.Y)*r);
//						ptTmp->Y = ptStart.Y + 15.005;
					}
				}
			}
		}
		else
		{
			// Calculating the new coordinate of the grid points.
			for (int i = gridPoints->GetWidth() - 1; i >= 0; --i)
			{
				for (int j = gridPoints->GetHeight() - 1; j >= 0; --j)
				{
					ptTmp = gridPoints->GetItem(i, j);
					r = Distance(*ptTmp, ptEnd.X, ptEnd.Y)/fCrossDistance;

					if (r < 1 && r > 0.01)
					{
						r = pow(r, m_GrowFactor);
						ptTmp->X = ptEnd.X + (int)((ptTmp->X - ptEnd.X)/r);
						ptTmp->Y = ptEnd.Y + (int)((ptTmp->Y - ptEnd.Y)/r);
					}
				}
			}
		}
	}
}

