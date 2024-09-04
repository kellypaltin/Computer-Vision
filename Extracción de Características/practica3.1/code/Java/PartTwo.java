package com.example.tarea;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.tarea.databinding.ActivityMainBinding;

public class PartTwo extends AppCompatActivity {

    Button btn;
    private TextView classificationTextView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        setContentView(R.layout.activity_part_two);
        btn= findViewById(R.id.button3);
        btn.setOnClickListener(v -> {
            Intent intent = new Intent(PartTwo.this, viewCamara.class);
            intent.putExtra("param_key", "PartTwo");
            startActivity(intent);
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        Intent intent = getIntent();
        String imagePath = intent.getStringExtra("imagePath");
        String imageLBP = intent.getStringExtra("imageLBP");
        String imageHistogram = intent.getStringExtra("imageHistogram");
        String result = intent.getStringExtra("clasificacion");

        if (imagePath != null) {
            ImageView imageView = findViewById(R.id.image_view);
            Bitmap bitmap = BitmapFactory.decodeFile(imagePath);
            imageView.setImageBitmap(bitmap);

            ImageView imgLBP = findViewById(R.id.image_view3);
            Bitmap bit = BitmapFactory.decodeFile(imageLBP);
            imgLBP.setImageBitmap(bit);

            ImageView imgHistogram = findViewById(R.id.image_view4);
            Bitmap bi = BitmapFactory.decodeFile(imageHistogram);
            imgHistogram.setImageBitmap(bi);

            classificationTextView = findViewById(R.id.textView4);
            classificationTextView.setText(result);
        }
    }
}