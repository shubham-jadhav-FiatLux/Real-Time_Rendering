package com.example.window;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import android.graphics.Color;
import android.view.View;
import android.view.Window;
import android.content.pm.ActivityInfo;

public class MainActivity extends AppCompatActivity {

    private MyView myView;                                          // Class Variable or Field Variable/Member

    @Override
    protected void onCreate(Bundle savedInstanceState) {            
        super.onCreate(savedInstanceState);                         // onCreate => WM_CREATE
                                                                    // onDestroy => WM_DESTROY
                                                                    // onPaint => WM_PAINT

        // Making Fullscren
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | 
                                                         View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                                                         View.SYSTEM_UI_FLAG_IMMERSIVE);            // SYSTEM_UI_FLAG_LAYOUT_STABLE, SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN , SYSTEM_UI_FLAG_HIDE_LAYOUT_NAVIGATION

        supportRequestWindowFeature(Window.FEATURE_NO_TITLE);

        // Forced Landscape Orientation
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        // Setting Background Color
        getWindow().getDecorView().setBackgroundColor(Color.rgb(0, 0, 0));
        
        myView = new MyView(this);
        setContentView(myView);
    }
}