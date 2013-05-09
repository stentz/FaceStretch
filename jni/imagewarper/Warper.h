#ifndef __WARPER_H__
#define __WARPER_H__

#include "WarperUtil.h"
#include "WarperCanvas.h"

namespace ImageWarper
{
	#define WARPER_TRANSLATE 0
	#define WARPER_GROW		1
	#define WARPER_SHRINK	2

	class  Warper
	{
	public:
		ImageData		*m_imgOriginal;
		WarperCanvas	*m_canvas;
		Point			m_ptCenterPos;
		WarpedImage		*m_warpedImage;
		bool			m_bEdgeBlurring;

		static bool OffsetFilter(ImageData *bmpOriginal, PointFArray2D *offset,
			Rectangle &bounds, ImageData *bmpNew);
		static bool FilterSmooth(ImageData *bmpOriginal, PointFArray2D *offset,
			Rectangle &bounds, ImageData *bmpNew);
		static void EdgeBlur(ImageData *bmpOriginal, 
			Rectangle &bounds, ImageData *bmpNew);

		
		// this constructor will create new ImageData for the Warper itself.
		Warper(ImageData &img);
		// this constructor will NOT create another ImageData*
		Warper(ImageData *img);
		virtual ~Warper();

		// Begin the warping interaction. Called when the interaction begins.
		// ptCenterPos: Location (in pixels) of the mouse click, in regard of the image.
		// iBrushSize: The radius of the effective window, in pixels. This should not be too large (<= 50).
		// iWarpType: WARPER_TRANSLATE, WARPER_GROW or WARPER_SHRINK
		void BeginWarp(Point ptCenterPos, int iBrushSize, int iWarperType);

		// Called at every moving step of the interaction.
		// ptMouse: Location (in pixels) of the current interaction, in regard of the image
		// Returns: a WarpedImage, contains the location and the image data of the warped image patch.
		// Never modify/delete the returned pointer!
		WarpedImage *UpdateWarp(Point ptMouse);

		// Called when the interaction finished.
		// Should be call right after a call to UpdateWarp().
		WarpedImage *EndWarp(WarpedImage *warpedImg);
	};
}


#endif
