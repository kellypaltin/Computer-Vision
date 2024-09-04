package com.example.project;
import androidx.appcompat.app.AppCompatActivity;
import android.content.Intent;

import android.os.Bundle;
import android.provider.Telephony;
import android.widget.Button;
import com.example.project.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    String TAG = "MainActivity";

    Button partOne,partTwo;

    static {
        System.loadLibrary("project");
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        partOne = (Button)findViewById(R.id.Part1);
        partOne.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, PartOne.class);
            startActivity(intent);
        });

        partTwo = (Button)findViewById(R.id.Part2);
        partTwo.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, FaceDetector.class);
            startActivity(intent);
        });

    }


}