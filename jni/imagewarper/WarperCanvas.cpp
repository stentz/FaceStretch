#include <algorithm>
#include <cmath>
#include "WarperCanvas.h"

namespace ImageWarper
{

#define GRIDSTEP_MIN 5
#define GRIDSTEP_MAX 20
#define GRIDSTEP_DEFAULT 10

	/*---------------------------------------------------------------------*/

	/*---------------------------------------------------------------------*/

	WarperCanvas::WarperCanvas(Rectangle &boundary, int iMaxWidth, int iMaxHeight)
	: m_GridPoints(0),
	m_OffsetPoints(0),
	m_IncrementStep(10),
	m_iMaxHeight(iMaxHeight),
	m_iMaxWidth(iMaxWidth)
	{
		Initialize(boundary);
		m_IncrementStep = boundary.Width / 2;
	}

	WarperCanvas::~WarperCanvas()
	{
		if (m_GridPoints)
			delete m_GridPoints;
		if (m_OffsetPoints)
			delete m_OffsetPoints;
	}


	/*---------------------------------------------------------------------*/
	// Properties
	/*---------------------------------------------------------------------*/

	PointArray2D* WarperCanvas::GetGridPoints()
	{
		return m_GridPoints;
	}

	PointFArray2D* WarperCanvas::GetOffsetPoints()
	{
		return m_OffsetPoints;
	}

	Rectangle* WarperCanvas::GetBoundary()
	{
		return &m_Boundary;
	}

	int WarperCanvas::GetGridStep()
	{
		return m_GridStep;
	}

	/*---------------------------------------------------------------------*/
	// Public interface
	/*---------------------------------------------------------------------*/

	bool WarperCanvas::Force(Point &ptStart, Point &ptEnd)
	{
		bool bGridUpdated = false;

		// make sure the start and end points are in the boundary area
		if ( !m_Boundary.Contains(ptEnd))
		{
			// ResizeGrid() calls ResetGridPos() automatically.
			// This will also update the offset array.
			ResizeGrid(ptEnd);
			bGridUpdated = true;
		}
		else
		{
			ResetGridPos();
		}

		// call the children class.
		DoForce(ptStart, ptEnd, m_GridPoints, m_Boundary);

		// update the offset array
		CalcOffsets();

		return bGridUpdated;
	}

	/*---------------------------------------------------------------------*/
	// Private Utilities
	/*---------------------------------------------------------------------*/

	void WarperCanvas::Initialize(Rectangle &rcBoundary)
	{
		m_Boundary = rcBoundary;

		// Automatically select the grid step.
		SelectGridStep();

		// Reset the grid to match the new grid step.
		ResetGridPos();

		// reset the offset grid to match the new size.
		if(m_OffsetPoints)
			delete m_OffsetPoints;
		m_OffsetPoints = new PointFArray2D(m_Boundary.Width, m_Boundary.Height);
	}

	void WarperCanvas::SelectGridStep()
	{
		int iMinDim = std::min(m_Boundary.Width, m_Boundary.Height);
		m_GridStep = GRIDSTEP_DEFAULT;

		if ((iMinDim / m_GridStep) < (iMinDim / GRIDSTEP_MAX))
			m_GridStep = GRIDSTEP_MAX;
		else if((iMinDim / m_GridStep) > (iMinDim / GRIDSTEP_MIN))
			m_GridStep = GRIDSTEP_MIN;
	}

	void WarperCanvas::ResetGridPos()
	{
		Point ptTmp;
		int iGridPtCntX = (m_Boundary.Width + m_GridStep - 1) / m_GridStep + 1;
		int iGridPtCntY = (m_Boundary.Height + m_GridStep - 1) / m_GridStep + 1;
		if(m_GridPoints)
			delete m_GridPoints;
		m_GridPoints = new PointArray2D(iGridPtCntX, iGridPtCntY);

		for (int i = 0; i < iGridPtCntX; ++i)
		{
			for (int j = 0; j < iGridPtCntY; ++j)
			{
				ptTmp.X = m_Boundary.X + i * m_GridStep;
				ptTmp.Y = m_Boundary.Y + j * m_GridStep;
				
				// last column
				if (i == iGridPtCntX - 1)
				{
					ptTmp.X = m_Boundary.Right();
				}
				// last row
				if (j == iGridPtCntY - 1)
				{
					ptTmp.Y = m_Boundary.Bottom();
				}

				m_GridPoints->SetItem(i, j, ptTmp);
			}
		}
	}

	void WarperCanvas::CalcOffsets()
	{
		float xfrac, yfrac, s, t, u, v;
		Point *ptTL, *ptTR, *ptBL, *ptBR;
		int iPtXLimit, iPtYLimit;
		int iColumnX, iPxCountX;
		int iRowY, iPxCountY;
		int i, j, m, n;
		PointF ptTmp;

		iPtXLimit = m_GridPoints->GetWidth() - 1;
		iPtYLimit = m_GridPoints->GetHeight() - 1;

		for (i = 0; i < iPtXLimit; i++)
		{
			iColumnX = m_GridPoints->GetItem(i, 0)->X;
			iPxCountX = m_GridPoints->GetItem(i + 1, 0)->X - iColumnX;
			
			for (j = 0; j < iPtYLimit; j++)
			{
				iRowY = m_GridPoints->GetItem(0, j)->Y;
				iPxCountY = m_GridPoints->GetItem(0, j + 1)->Y - iRowY;
				xfrac = 1.0f / iPxCountX;
				yfrac = 1.0f / iPxCountY;
				ptTL = m_GridPoints->GetItem(i, j);
				ptTR = m_GridPoints->GetItem(i + 1, j);
				ptBL = m_GridPoints->GetItem(i, j + 1);
				ptBR = m_GridPoints->GetItem(i + 1, j + 1);

				for (m = 0; m < iPxCountX; m++)
				{
					for (n = 0; n < iPxCountY; n++)
					{
						s = (ptTR->X - ptTL->X) * m * xfrac + ptTL->X;
						t = (ptTR->Y - ptTL->Y) * m * xfrac + ptTL->Y;
						u = (ptBR->X - ptBL->X) * m * xfrac + ptBL->X;
						v = (ptBR->Y - ptBL->Y) * m * xfrac + ptBL->Y;

						ptTmp.X = (s + (u - s) * n * yfrac);
						ptTmp.Y = (t + (v - t) * n * yfrac);

						m_OffsetPoints->SetItem(iColumnX + m - m_Boundary.X,
							iRowY + n - m_Boundary.Y, ptTmp); 
					}
				}
			}
		}
	}

	void WarperCanvas::ResizeGrid(Point &ptEnd)
	{
		Rectangle rcNewBounds(m_Boundary);
		bool bChanged = false;
		int iChangeStep;

		// left
		while (ptEnd.X < rcNewBounds.X && rcNewBounds.X >= m_IncrementStep)
		{
			iChangeStep = std::min(rcNewBounds.X, m_IncrementStep);
			rcNewBounds.X -= iChangeStep;
			rcNewBounds.Width += iChangeStep;
			bChanged = true;
		}
		// right
		while (ptEnd.X > rcNewBounds.Right() && rcNewBounds.Right() < m_iMaxWidth - m_IncrementStep)
		{
			rcNewBounds.Width = std::min(m_iMaxWidth, rcNewBounds.Width + m_IncrementStep);
			bChanged = true;
		}
		// top
		while (ptEnd.Y < rcNewBounds.Y && rcNewBounds.Y >= m_IncrementStep)
		{
			iChangeStep = std::min(rcNewBounds.Y, m_IncrementStep);
			rcNewBounds.Y -= iChangeStep;
			rcNewBounds.Height += iChangeStep;
			bChanged = true;
		}
		// bottom
		while (ptEnd.Y > rcNewBounds.Bottom() && rcNewBounds.Bottom() < m_iMaxHeight - m_IncrementStep)
		{
			rcNewBounds.Height = std::min(m_iMaxHeight, rcNewBounds.Height + m_IncrementStep);
			bChanged = true;
		}

		if (bChanged)
		{
			Initialize(rcNewBounds);
		}
	}

	float WarperCanvas::DistanceSqr(Point &pt1, Point &pt2)
	{
		return DistanceSqr(pt1, pt2.X, pt2.Y);
	}

	float WarperCanvas::DistanceSqr(Point &pt1, int x, int y)
	{
		float d = (float)(pt1.X - x) * (pt1.X - x);
		d += (pt1.Y - y) * (pt1.Y - y);
		return d;
	}

	float WarperCanvas::Distance(Point &pt1, Point &pt2)
	{
		return (float)sqrt(WarperCanvas::DistanceSqr(pt1, pt2.X, pt2.Y));
	}

	float WarperCanvas::Distance(Point &pt1, int x, int y)
	{
		return (float)sqrt(WarperCanvas::DistanceSqr(pt1, x, y));
	}
}
