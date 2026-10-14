package com.example.window;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import android.graphics.Color;

public class MainActivity extends AppCompatActivity {

    private MyView myView;                                          // Class Variable or Field Variable/Member

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().getDecorView().setBackgroundColor(Color.rgb(0, 0, 0));
        
        myView = new MyView(this);
        setContentView(myView);
    }
}