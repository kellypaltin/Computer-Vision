package com.example.project;

import androidx.appcompat.app.AppCompatActivity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;

import org.opencv.android.CameraActivity;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;
import java.util.Arrays;
import java.util.List;

public class PartOne extends AppCompatActivity {

    String TAG = "PartOne";

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Button camera;
        Button server;
        Button filterOne, filterTwo,filterThree,filterFour;
        File f1 = new File(getExternalFilesDir(null), "FiltroOne");
        File f2 = new File(getExternalFilesDir(null), "FiltroTwo");
        File f3 = new File(getExternalFilesDir(null), "FiltroThree");
        File f4 = new File(getExternalFilesDir(null), "FiltroFour");

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_part_one);


        camera=(Button)findViewById(R.id.button);
        camera.setOnClickListener(v -> {
            Intent intent = new Intent(PartOne.this, CameraActivity.class);
            intent.putExtra("param_key", "PartOne");
            startActivity(intent);
        });

        filterOne = (Button)findViewById(R.id.filter1);
        if (f1.exists()) {
            filterOne.setOnClickListener(v -> {
                Intent i = new Intent(PartOne.this, FiltroOne.class);
                startActivity(i);
            });
        } else {
            filterOne.setEnabled(false);
        }


        filterTwo = (Button)findViewById(R.id.filter2);
        if (f2.exists()) {
            filterTwo.setOnClickListener(v -> {
                Intent i = new Intent(PartOne.this,FiltroTwo.class);
                startActivity(i);
            });
        } else {
            filterTwo.setEnabled(false);
        }


        filterThree = (Button)findViewById(R.id.filter3);
        if (f3.exists()) {
            filterThree.setOnClickListener(v -> {
                Intent i = new Intent(PartOne.this,FiltroThree.class);
                startActivity(i);
            });
        } else {
            filterThree.setEnabled(false);
        }

        filterFour = (Button)findViewById(R.id.filter4);
        if (f4.exists()) {
            filterFour.setOnClickListener(v -> {
                Intent i = new Intent(PartOne.this,FiltroFour.class);
                startActivity(i);
            });
        } else {
            filterFour.setEnabled(false);
        }


        server = (Button)findViewById(R.id.server);
        server.setOnClickListener(v -> new Thread(() -> {
            delete();
            try {
                List<String> namePath = Arrays.asList("Camara", "FiltroOne", "FiltroTwo", "FiltroThree", "FiltroFour");
                for(String path: namePath){
                    File file = new File(getExternalFilesDir(null), path);
                    File[] listaDeArchivos = file.listFiles();
                    for(File archivo : listaDeArchivos){
                        File imgFile= new File(file+"/"+archivo.getName());
                        connect(imgFile,path);
                    }
                }
                Log.d(TAG, "Se subieron los archivos.");
                for(String path:namePath){
                    getvideo(path);
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }).start());
    }

    @Override
    protected void onResume() {
        super.onResume();
        Intent intent = getIntent();
        String imagePath = intent.getStringExtra("imagePath");
        if (imagePath != null) {
            ImageView imageView = findViewById(R.id.image_view);
            Bitmap bitmap = BitmapFactory.decodeFile(imagePath);
            imageView.setImageBitmap(bitmap);
        }
    }
    private void connect(File file,String name ) throws IOException {
        String boundary = "----WebKitFormBoundary" + System.currentTimeMillis();
        String LINE_FEED = "\r\n";
        URL url = new URL("http://192.168.1.24:4000/upload?name=" + name);
        HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
        urlConnection.setRequestMethod("POST");
        urlConnection.setRequestProperty("Content-Type", "multipart/form-data; boundary=" + boundary);

        try (DataOutputStream outputStream = new DataOutputStream(urlConnection.getOutputStream())) {
            outputStream.writeBytes("--" + boundary + LINE_FEED);
            outputStream.writeBytes("Content-Disposition: form-data; name=\"file\"; filename=\"" + file.getName() + "\"" + LINE_FEED);
            outputStream.writeBytes("Content-Type: " + HttpURLConnection.guessContentTypeFromName(file.getName()) + LINE_FEED);
            outputStream.writeBytes("Content-Transfer-Encoding: binary" + LINE_FEED);
            outputStream.writeBytes(LINE_FEED);

            try (FileInputStream inputStream = new FileInputStream(file)) {
                byte[] buffer = new byte[4096];
                int bytesRead;
                while ((bytesRead = inputStream.read(buffer)) != -1) {
                    outputStream.write(buffer, 0, bytesRead);
                }
                outputStream.flush();
            }
            outputStream.writeBytes(LINE_FEED);
            outputStream.writeBytes("--" + boundary + "--" + LINE_FEED);
            outputStream.flush();
        }

        int responseCode = urlConnection.getResponseCode();
        if (responseCode == HttpURLConnection.HTTP_OK) {
            Log.d(TAG, "File uploaded successfully.");
        } else {
            Log.d(TAG, "File upload failed with response code: " + responseCode);
        }
    }

    private void getvideo(String name) throws IOException {
        URL url = new URL("http://192.168.1.24:4000/createVideo?name=" + name);
        URLConnection connection = url.openConnection();

        try (BufferedReader in = new BufferedReader(
                new InputStreamReader(connection.getInputStream())))
        {
            String line;
            while ((line = in.readLine()) != null) {
                System.out.println(line);
            }
        }
    }
    private void delete() {
        try {
            URL url = new URL("http://192.168.1.24:4000/delete");
            HttpURLConnection httpCon = (HttpURLConnection) url.openConnection();
            httpCon.setDoOutput(true);
            httpCon.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
            httpCon.setRequestMethod("DELETE");
            httpCon.connect();

            int responseCode = httpCon.getResponseCode();
            if (responseCode == HttpURLConnection.HTTP_OK) {
                System.out.println("La solicitud DELETE fue exitosa.");
            } else {
                System.out.println("Hubo un error con la solicitud DELETE: " + responseCode);
            }

            httpCon.disconnect();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}