package com.example.tarea;

import androidx.appcompat.app.AppCompatActivity;
import com.example.tarea.databinding.ActivityMainBinding;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;


public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    static {
        System.loadLibrary("tarea");
    }

    private ActivityMainBinding binding;
    native double processCorpus(String ruta);
    native String processImg(String ruta);

    private native void logResults();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Button partOne,partTwo;

        super.onCreate(savedInstanceState);

        com.example.tarea.databinding.ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());

        partOne= findViewById(R.id.button);
        partOne.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, PartOne.class);
            startActivity(intent);
        });

        partTwo= findViewById(R.id.button2);
        partTwo.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, PartTwo.class);
            startActivity(intent);
        });

        corpusImg();
    }

    void corpusImg(){
        File externalFilesDir = getExternalFilesDir(null);
        String[] folderNames = {"circle", "triangle", "square"};
        String[] folderTwo = {"Madera", "Tela"};

        for (String folderName : folderNames) {
            File destinationFile = new File(externalFilesDir, folderName);
            processCorpus(destinationFile.toString());
        }

        for(String name : folderTwo){
            File destinationFile = new File(externalFilesDir,name);
            processImg(destinationFile.toString());
        }

        logResults();
    }

    public native String stringFromJNI();
}

