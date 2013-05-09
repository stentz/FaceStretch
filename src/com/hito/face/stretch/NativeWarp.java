package com.hito.face.stretch;

import android.graphics.Bitmap;

public class NativeWarp {
	
	static {
		System.loadLibrary("renrenFilter");
	}
	
	public NativeWarp() {
		super();
		// TODO Auto-generated constructor stub
	}
	
	native protected void initWarp(Bitmap bitmap, int w, int h);
	native protected void beginWarp(int type, int radius, int x, int y);
	native protected void updateWarp(int x, int y);
	native protected byte[] endWarp(int x, int y);
	
}
