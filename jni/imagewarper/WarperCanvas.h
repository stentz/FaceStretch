#ifndef __WARPERCANVAS_H__
#define __WARPERCANVAS_H__

#include "WarperUtil.h"

namespace ImageWarper
{
	class WarperCanvas
	{
		PointArray2D	*m_GridPoints;		/* Coordinate of the points on Grid */
		PointFArray2D	*m_OffsetPoints;	/* Image offset */
		Rectangle		m_Boundary;			/* Position of this WarperCanvas on the destination image */
		int				m_GridStep;			/* width/height in pixels of each cell in the alignment grid */

		int		m_IncrementStep;	/* Number of pixels of each incremental step when the grid is resized */
		int		m_iMaxWidth;		/* Width and height in pixels of the original image. */
		int		m_iMaxHeight;		/* Used when the grid is resized. */

		void Initialize(Rectangle &rcBoundary);
		void SelectGridStep();
		void ResizeGrid(Point &ptEnd);
		void ResetGridPos();
		void CalcOffsets();
		
	protected:

		static float DistanceSqr(Point &pt1, Point &pt2);
		static float DistanceSqr(Point &pt1, int x, int y);
		static float Distance(Point &pt1, Point &pt2);
		static float Distance(Point &pt1, int x, int y);

		virtual void DoForce(Point &ptStart, Point &ptEnd, PointArray2D *gridPoints, Rectangle &boundary) = 0;

	public:
		PointArray2D* GetGridPoints();
		PointFArray2D* GetOffsetPoints();
		Rectangle* GetBoundary();
		int GetGridStep();

		WarperCanvas(Rectangle &boundary, int iMaxWidth, int iMaxHeight);
		virtual ~WarperCanvas();
		
		// return TRUE if the Bounding box is updated.
		bool Force(Point &ptStart, Point &ptEnd);
	};

}

#endif
