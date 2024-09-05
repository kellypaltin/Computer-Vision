package com.example.tarea;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import com.example.tarea.databinding.ActivityMainBinding;

public class PartOne extends AppCompatActivity {

    Button button3,button4;
    private TextView classificationTextView;
    private TextView classificationMoments;
    private TextView classificationZernike;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        setContentView(R.layout.activity_part_one);

        Intent intentPartOne = new Intent(PartOne.this, viewCamara.class);

        button3 = findViewById(R.id.button3);
        button3.setOnClickListener(v -> {
            intentPartOne.putExtra("param_key", "PartOne");
            startActivity(intentPartOne);
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        Intent intent = getIntent();
        String imagePath = intent.getStringExtra("imagePath"); // Recibe como String
        String result = intent.getStringExtra("clasificacion");
        String moments = intent.getStringExtra("moments");
        String momentZernike = intent.getStringExtra("momentsZernike");
        String imagefilter = intent.getStringExtra("imageFilter");
        if (imagePath != null) {
            ImageView imageView = findViewById(R.id.image_view);
            Bitmap bitmap = BitmapFactory.decodeFile(imagePath);
            imageView.setImageBitmap(bitmap);

            ImageView imgFView = findViewById(R.id.image_view2);
            Bitmap bit = BitmapFactory.decodeFile(imagefilter);
            imgFView.setImageBitmap(bit);

            classificationTextView = findViewById(R.id.classfication);
            classificationMoments = findViewById(R.id.classfication2);
            classificationZernike = findViewById(R.id.classfication3);

            if(result.equals("circle")){
                result ="Circulo";
            }else if (result.equals("triangle")){
                result = "Triangulo";
            }else {
                result = "Cuadrado";
            }
            classificationTextView.setText(result);
            classificationMoments.setText(moments);
            classificationZernike.setText(momentZernike);
        }
    }
}
