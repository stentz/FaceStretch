package com.hito.face.stretch;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.media.FaceDetector;
import android.util.Log;

public class JNILibCurve {
	private Bitmap mFaceBitmap;
	private int mFaceWidth = 200;
	private int mFaceHeight = 200;
	private static final int MAX_FACES = 10;
	int warp_type = 0;
	private int radius = 0;
	private static boolean DEBUG = false;
	private int eyeLeftX = 0, eyeRightX = 0;
	private int eyeLeftY = 0, eyeRightY = 0;
	private static final String TAG = "JNILibCurve";
 
	static{
	  System.loadLibrary("renrenFilter");	
	  Log.i(TAG,"load renrenFilter");
	}

	public JNILibCurve() {
		super();
		// TODO Auto-generated constructor stub
	}
	
	static native void gg(float[] sCurve, float control_X, float control_Y);
	native protected void initWarp(Bitmap bitmap, int w, int h);
	native protected void beginWarp(int type, int radius, int x, int y);
	native protected void updateWarp(int x, int y);
	native protected byte[] endWarp(int x, int y);
	
	public Bitmap bigEye(Bitmap srcBmp) {
		initWarp(srcBmp, srcBmp.getWidth(),
				srcBmp.getHeight());
		mFaceBitmap = srcBmp.copy(Bitmap.Config.RGB_565, true);
		srcBmp.recycle();
		mFaceWidth = mFaceBitmap.getWidth();
		mFaceHeight = mFaceBitmap.getHeight();
		setFace();

		// bigger eye
		radius = 20;
		warp_type = 1;
		beginWarp(warp_type, radius, eyeLeftX, eyeLeftY);
		updateWarp(eyeLeftX, eyeLeftY);
		endWarp(eyeLeftX, eyeLeftY);

		beginWarp(warp_type, radius, eyeRightX, eyeRightY);
		updateWarp(eyeRightX, eyeRightY);
		byte[] dst = endWarp(eyeRightX, eyeRightY);
		
		Bitmap bigEyeBitmap = decodeFrameToBitmap(dst, srcBmp);
		
		return bigEyeBitmap;
	}
	
	public Bitmap smallFace(Bitmap srcBmp) {
		initWarp(srcBmp, srcBmp.getWidth(),
				srcBmp.getHeight());
		mFaceBitmap = srcBmp.copy(Bitmap.Config.RGB_565, true);
		srcBmp.recycle();
		mFaceWidth = mFaceBitmap.getWidth();
		mFaceHeight = mFaceBitmap.getHeight();
		setFace();		

		// smaller face
		warp_type = 0;
		radius = (int) ((eyeRightX - eyeLeftX) * 0.42f);
		int range = 7;
//		int y_dis = 3;
		// left face
		// Point 1 x=nose
		// nativeWarp.beginWarp(warp_type, radius,
		// (int) (eyeLeftX - (float) (eyeRightX - eyeLeftX) / 3), eyeLeftY
		// + (eyeRightX - eyeLeftX) - y_dis);
		// nativeWarp.updateWarp(
		// (int) (eyeLeftX - (float) (eyeRightX - eyeLeftX) / 3 + range),
		// eyeLeftY
		// + (eyeRightX - eyeLeftX) - y_dis);
		// nativeWarp.endWarp(
		// (int) (eyeLeftX - (float) (eyeRightX - eyeLeftX) / 3 + range),
		// eyeLeftY
		// + (eyeRightX - eyeLeftX) - y_dis);
		// //Point 2 x=mouth
		// nativeWarp.beginWarp(warp_type, radius,
		// (int) (eyeLeftX - (float) (eyeRightX - eyeLeftX) / 3), eyeLeftY
		// + (eyeRightX - eyeLeftX) + y_dis);
		// nativeWarp.updateWarp(
		// (int) (eyeLeftX - (float) (eyeRightX - eyeLeftX) / 3 + range),
		// eyeLeftY
		// + (eyeRightX - eyeLeftX) + y_dis);
		// nativeWarp.endWarp(
		// (int) (eyeLeftX - (float) (eyeRightX - eyeLeftX) / 3 + range),
		// eyeLeftY
		// + (eyeRightX - eyeLeftX) + y_dis);
		// Point x=chain & y behind leftEye

		beginWarp(warp_type, radius, eyeLeftX + 2, eyeLeftY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		updateWarp((eyeLeftX + range), eyeLeftY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		endWarp((eyeLeftX + range), eyeLeftY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));

		// right face
		// Point 3 x=nose
		// nativeWarp.beginWarp(warp_type, radius,
		// (int) (eyeRightX + (float) (eyeRightX - eyeLeftX) / 4),
		// eyeRightY + (eyeRightX - eyeLeftX) - y_dis);
		// nativeWarp.updateWarp(
		// (int) (eyeRightX + (float) (eyeRightX - eyeLeftX) / 4 - range),
		// eyeRightY + (eyeRightX - eyeLeftX) - y_dis);
		// nativeWarp.endWarp(
		// (int) (eyeRightX + (float) (eyeRightX - eyeLeftX) / 4 - range),
		// eyeRightY + (eyeRightX - eyeLeftX) - y_dis);
		// //Point 4 x=mouth
		// nativeWarp.beginWarp(warp_type, radius,
		// (int) (eyeRightX + (float) (eyeRightX - eyeLeftX) / 4),
		// eyeRightY + (eyeRightX - eyeLeftX) + y_dis);
		// nativeWarp.updateWarp(
		// (int) (eyeRightX + (float) (eyeRightX - eyeLeftX) / 4 - range),
		// eyeRightY + (eyeRightX - eyeLeftX) + y_dis);
		// byte[] dst = nativeWarp.endWarp(
		// (int) (eyeRightX + (float) (eyeRightX - eyeLeftX) / 4 - range),
		// eyeRightY + (eyeRightX - eyeLeftX) + y_dis);

		// Point x=chain & y behind rightEye
		beginWarp(warp_type, radius, eyeRightX - 2, eyeRightY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		updateWarp((eyeRightX - range), eyeRightY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		byte[] dst = endWarp((eyeRightX - range), eyeRightY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		
		Bitmap smallFaceBitmap = decodeFrameToBitmap(dst, srcBmp);
		return smallFaceBitmap;
	}
	
	
	private void setFace() {
		FaceDetector fd;
		FaceDetector.Face[] faces = new FaceDetector.Face[MAX_FACES];
		PointF eyescenter = new PointF();
		float eyesdist = 0.0f;
		int[] fpx = null;
		int[] fpy = null;
		int count = 0;

		try {
			fd = new FaceDetector(mFaceWidth, mFaceHeight, MAX_FACES);
			count = fd.findFaces(mFaceBitmap, faces);
		} catch (Exception e) {
			Log.e(TAG, "setFace(): " + e.toString());
			return;
		}

		// check if we detect any faces
		if (count > 0) {
			fpx = new int[count * 2];
			fpy = new int[count * 2];
			for (int i = 0; i < count; i++) {
				try {
					faces[i].getMidPoint(eyescenter);
					eyesdist = faces[i].eyesDistance();
	
					// set up left eye location
					fpx[2 * i] = (int) (eyescenter.x - eyesdist / 2);
					fpy[2 * i] = (int) eyescenter.y;

					// set up right eye location
					fpx[2 * i + 1] = (int) (eyescenter.x + eyesdist / 2);
					fpy[2 * i + 1] = (int) eyescenter.y;

					if (!DEBUG)
						Log.e(TAG,
								"setFace(): face "
										+ i
										+ ": confidence = "
										+ faces[i].confidence()
										+ ", eyes distance = "
										+ faces[i].eyesDistance()
										+ ", pose = ("
										+ faces[i]
												.pose(FaceDetector.Face.EULER_X)
										+ ","
										+ faces[i]
												.pose(FaceDetector.Face.EULER_Y)
										+ ","
										+ faces[i]
												.pose(FaceDetector.Face.EULER_Z)
										+ ")" + ", eyes midpoint = ("
										+ eyescenter.x + "," + eyescenter.y
										+ ")" + "\n" + "left eye: x="
										+ fpx[2 * i] + " y=" + fpy[2 * i]
										+ "\n" + "right eye: x="
										+ fpx[2 * i + 1] + " y="
										+ fpy[2 * i + 1]);
				} catch (Exception e) {
					Log.e(TAG, "setFace(): face " + i + ": " + e.toString());
				}
			}
		}
		eyeLeftX = fpx[0];
		eyeLeftY = fpy[0];
		eyeRightX = fpx[1];
		eyeRightY = fpy[1];
	}

	private Bitmap decodeFrameToBitmap(byte[] frame, Bitmap srcBitmap) {
		int[] colors = convertByteToColor(frame);
		if (colors == null) {
			Log.d("null", "null");
			return null;
		}
		Bitmap bmp = Bitmap.createBitmap(colors, 0, srcBitmap.getWidth(),
				srcBitmap.getWidth(), srcBitmap.getHeight(),
				Bitmap.Config.ARGB_8888);
		return bmp;
	}
	
	private static int convertByteToInt(byte data) {
		int heightBit = (int) ((data >> 4) & 0x0F);
		int lowBit = (int) (0x0F & data);

		return heightBit * 16 + lowBit;
	}
	
	private static int[] convertByteToColor(byte[] data) {
		int size = data.length;
		if (size == 0) {
			return null;
		}

		int arg = 0;
		if (size % 4 != 0) {
			arg = 1;
		}

		int[] color = new int[size / 4 + arg];
		int red, green, blue;

		if (arg == 0) {
			for (int i = 0; i < color.length; ++i) {
				red = convertByteToInt(data[i * 4]);
				green = convertByteToInt(data[i * 4 + 1]);
				blue = convertByteToInt(data[i * 4 + 2]);

				color[i] = (red << 16) | (green << 8) | blue | 0xFF000000;
			}
		} else {
			for (int i = 0; i < color.length - 1; ++i) {
				red = convertByteToInt(data[i * 4]);
				green = convertByteToInt(data[i * 4 + 1]);
				blue = convertByteToInt(data[i * 4 + 2]);
				color[i] = (red << 16) | (green << 8) | blue | 0xFF000000;
			}

			color[color.length - 1] = 0xFF000000;
		}

		return color;
	}
}
