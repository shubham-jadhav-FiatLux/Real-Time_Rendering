package com.example.window;

import androidx.appcompat.widget.AppCompatTextView;
import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

// Event Related Packages
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.MotionEvent;
import android.view.GestureDetector.OnGestureListener;

public class MyView extends AppCompatTextView implements OnDoubleTapListener, OnGestureListener {

	private GestureDetector gestureDetector;

	MyView(Context context){
		
		super(context);

		setTextSize(48);
		setTextColor(Color.GREEN);
		setGravity(Gravity.CENTER);
		setText("Hello, World !!!");

		// Gesture Related Code
		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	}

	@Override
	public boolean onTouchEvent(MotionEvent e) {
	
		// Code
		if (!gestureDetector.onTouchEvent(e)) {

			super.onTouchEvent(e);
		}
		return true;
	}

	// Three Methods Of OnDoubleTapListner
	@Override
	public boolean onDoubleTap(MotionEvent e) {
	
		setText("DoubleTap-By SBJ");
		return true;
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e) {
	
		return true;
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e) {
	
		setText("SingleTap-By SBJ");
		return true;
	}

	// Six Methods Of OnGestureListner
	@Override
	public boolean onDown(MotionEvent e) {
	
		return true;
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
	
		return true;
	}

	@Override
	public void onLongPress(MotionEvent e) {

		setText("LongPress-By SBJ");
	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {

		System.exit(0);
		return true;
	}

	@Override
	public void onShowPress(MotionEvent e) {

	}

	@Override
	public boolean onSingleTapUp(MotionEvent e) {
	
		return true;
	}
}
