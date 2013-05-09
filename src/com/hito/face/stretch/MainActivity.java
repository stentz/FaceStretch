package com.hito.face.stretch;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.PointF;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.media.FaceDetector;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

public class MainActivity extends Activity {
	private ImageView image;
	private Bitmap srcBitmap;
	private Bitmap drawBitmap;
	private Bitmap tmpBitmap;
	private NativeWarp nativeWarp;
	int warp_type = 0;
	private Button growBtn;
	private Button shrinkBtn;
	private Button translateBtn;
	private Button showBtn;
	int bitmapW = 0;
	int bitmapH = 0;

	private Bitmap mFaceBitmap;
	private int mFaceWidth = 200;
	private int mFaceHeight = 200;
	private static final int MAX_FACES = 10;
	private static String TAG = "TutorialOnFaceDetect";
	private static boolean DEBUG = false;
	private int eyeLeftX = 0, eyeRightX = 0;
	private int eyeLeftY = 0, eyeRightY = 0;
	private int drawableId;
	private int radius = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.activity_main);

		growBtn = (Button) findViewById(R.id.grow);
		growBtn.setVisibility(View.GONE);
		growBtn.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				warp_type = 1;
				Log.d("grow", "btn down");
			}
		});
		shrinkBtn = (Button) findViewById(R.id.shrink);
		shrinkBtn.setVisibility(View.GONE);
		shrinkBtn.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				warp_type = 2;
			}
		});
		translateBtn = (Button) findViewById(R.id.translate);
		translateBtn.setVisibility(View.GONE);
		translateBtn.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				warp_type = 0;
			}
		});

		drawableId = R.drawable.taylor;
		radius = 10;
		image = (ImageView) findViewById(R.id.imageView);
		srcBitmap = BitmapFactory.decodeResource(getResources(), drawableId)
				.copy(Bitmap.Config.ARGB_8888, true);
		image.setBackgroundDrawable(
				new BitmapDrawable(srcBitmap));
//		nativeWarp = new NativeWarp();
//
//		nativeWarp.initWarp(srcBitmap, srcBitmap.getWidth(),
//				srcBitmap.getHeight());
		bitmapW = srcBitmap.getWidth();
		bitmapH = srcBitmap.getHeight();
		image.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				long a, b, c = 0, d = 0, e, f, g, h;
				a = System.currentTimeMillis();
				int x = (int) (event.getX() * ((float) bitmapW / image
						.getWidth()));
				int y = (int) (event.getY() * ((float) bitmapH / image
						.getHeight()));
				b = System.currentTimeMillis();
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					c = System.currentTimeMillis();
					nativeWarp.beginWarp(warp_type, radius, x, y);
					d = System.currentTimeMillis();
					break;
				case MotionEvent.ACTION_MOVE:
					// nativeWarp.updateWarp(x, y);
					break;
				case MotionEvent.ACTION_UP:
					e = System.currentTimeMillis();
					nativeWarp.updateWarp(x, y);
					nativeWarp.updateWarp(x, y);
					f = System.currentTimeMillis();
					byte[] src = nativeWarp.endWarp(x, y);
					g = System.currentTimeMillis();
					Bitmap bmp = decodeFrameToBitmap(src);
					h = System.currentTimeMillis();
					Log.d("bmp",
							"run " + bmp.getWidth() + " " + bmp.getHeight()
									+ "\n");
					Drawable drawable = new BitmapDrawable(bmp);
					image.setBackgroundDrawable(drawable);

					Log.d("cost ", "float change cost: " + (b - a) + " ms"
							+ "\n" + "begin warp cost:   " + (d - c) + " ms"
							+ "\n" + "update warp cost:  " + (f - e) + " ms"
							+ "\n" + "end warp cost:     " + (g - f) + " ms"
							+ "\n" + "decode frame cost: " + (h - g) + " ms");
					break;
				default:
					break;
				}

				return true;
			}
		});

		// load the photo
		Bitmap bmp = BitmapFactory.decodeResource(getResources(), drawableId);
//		tmpBitmap = b.copy(Bitmap.Config.RGB_565, true);
		JNILibCurve jniLib = new JNILibCurve();
//		tmpBitmap = jniLib.bigEye(bmp);
		long a,b;
		a = System.currentTimeMillis();
		drawBitmap = jniLib.smallFace(jniLib.bigEye(bmp));
		b = System.currentTimeMillis();
		Log.d("cost time", "big eye and small face cost: " + (b-a) + " ms");
//		tmpBitmap = bigEye(b);
//		drawBitmap = smallFace(tmpBitmap);

		image.setBackgroundDrawable(new 
				BitmapDrawable(drawBitmap));

		showBtn = (Button) findViewById(R.id.show);
		showBtn.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					image.setBackgroundDrawable(
							new BitmapDrawable(srcBitmap));
					break;
				case MotionEvent.ACTION_UP:
					image.setBackgroundDrawable(
							new BitmapDrawable(drawBitmap));
					break;
				}

				return true;
			}
		});
	}

	public Bitmap bigEye(Bitmap srcBmp) {
		nativeWarp.initWarp(srcBmp, srcBmp.getWidth(),
				srcBmp.getHeight());
		mFaceBitmap = srcBmp.copy(Bitmap.Config.RGB_565, true);
		srcBmp.recycle();
		mFaceWidth = mFaceBitmap.getWidth();
		mFaceHeight = mFaceBitmap.getHeight();
		setFace();

		// bigger eye
		radius = 10;
		warp_type = 1;
		nativeWarp.beginWarp(warp_type, radius, eyeLeftX, eyeLeftY);
		nativeWarp.updateWarp(eyeLeftX, eyeLeftY);
		nativeWarp.endWarp(eyeLeftX, eyeLeftY);

		nativeWarp.beginWarp(warp_type, radius, eyeRightX, eyeRightY);
		nativeWarp.updateWarp(eyeRightX, eyeRightY);
		byte[] dst = nativeWarp.endWarp(eyeRightX, eyeRightY);
		
		Bitmap bigEyeBitmap = decodeFrameToBitmap(dst);
		
		return bigEyeBitmap;
	}

	public Bitmap smallFace(Bitmap srcBmp) {
		nativeWarp.initWarp(srcBmp, srcBmp.getWidth(),
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

		nativeWarp.beginWarp(warp_type, radius, eyeLeftX + 2, eyeLeftY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		nativeWarp.updateWarp((eyeLeftX + range), eyeLeftY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		nativeWarp.endWarp((eyeLeftX + range), eyeLeftY
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
		nativeWarp.beginWarp(warp_type, radius, eyeRightX - 2, eyeRightY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		nativeWarp.updateWarp((eyeRightX - range), eyeRightY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		byte[] dst = nativeWarp.endWarp((eyeRightX - range), eyeRightY
				+ (int) ((eyeRightX - eyeLeftX) * 1.5f));
		
		Bitmap smallFaceBitmap = decodeFrameToBitmap(dst);
		return smallFaceBitmap;
	}

	public void save() {
		// save pic
		String path = Environment.getExternalStorageDirectory() + "/my_"
				+ System.currentTimeMillis() + ".jpg";
		File f = new File(path);
		try {
			f.createNewFile();
			FileOutputStream fOut = null;
			fOut = new FileOutputStream(f);
			Bitmap bmptmp = Bitmap.createScaledBitmap(drawBitmap, 480, 700,
					true);

			bmptmp.compress(Bitmap.CompressFormat.JPEG, 100, fOut);
			fOut.flush();
			fOut.close();
			Toast toast = Toast.makeText(this, "保存成功", Toast.LENGTH_SHORT);
			toast.show();
		} catch (IOException e) {
			e.printStackTrace();
			Toast toast = Toast.makeText(this, "保存失败", Toast.LENGTH_SHORT);
			toast.show();
		}
	}

	public static int convertByteToInt(byte data) {
		int heightBit = (int) ((data >> 4) & 0x0F);
		int lowBit = (int) (0x0F & data);

		return heightBit * 16 + lowBit;
	}

	public static int[] convertByteToColor(byte[] data) {
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

	Bitmap decodeFrameToBitmap(byte[] frame) {
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

	public Bitmap pixelTranslate(Bitmap src) {
		int width = src.getWidth();
		int height = src.getHeight();
		int r = 0, g = 0, b = 0, a = 0;
		int[] pixelsImage = new int[width * height];
		int[] pixelsSrc = new int[width * height];
		src.getPixels(pixelsSrc, 0, width, 0, 0, width, height);
		int pixColorSrc = 0;
		Bitmap temp = src.copy(Bitmap.Config.ARGB_8888, true);
		for (int x = 0; x < height; x++) {
			for (int y = 0; y < width; y++) {
				pixColorSrc = pixelsSrc[x * width + y];
				r = Color.red(pixColorSrc);
				g = Color.green(pixColorSrc);
				b = Color.blue(pixColorSrc);
				a = Color.alpha(pixColorSrc);
				if (x > 200 && x < 600 && y > 200 && y < 800) {
					if (y < 500) {
						pixColorSrc = pixelsSrc[(x) * width + y + 5];
					} else {
						pixColorSrc = pixelsSrc[(x) * width + y - 5];
					}
					pixelsImage[(x * width + y)] = pixColorSrc;
					continue;
				}
				int rgbcolor = (a << 24) + (r << 16) + (g << 8) + b;
				pixelsImage[(x * width + y)] = rgbcolor;
			}
		}
		temp.setPixels(pixelsImage, 0, width, 0, 0, width, height);
		return temp;
	}

	public void setFace() {
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
		// mIV.setDisplayPoints(fpx, fpy, count * 2, 1);
	}
}
