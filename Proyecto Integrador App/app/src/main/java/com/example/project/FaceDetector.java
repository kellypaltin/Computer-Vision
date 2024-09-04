package com.example.project;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;
import org.tensorflow.lite.Interpreter;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.util.Arrays;
import java.util.List;

public class FaceDetector extends AppCompatActivity {
    String TAG = "Face";

    private File loadCascadeFile(int resourceId, String fileName) {
        try {
            InputStream is = getResources().openRawResource(resourceId);
            File cascadeDir = getDir("cascade", MODE_PRIVATE);
            File cascadeFile = new File(cascadeDir, fileName);
            FileOutputStream os = new FileOutputStream(cascadeFile);

            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {
                os.write(buffer, 0, bytesRead);
            }
            is.close();
            os.close();

            return cascadeFile;
        } catch (Exception e) {
            Log.e(TAG, "Error loading cascade file: " + fileName, e);
            return null;
        }
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Button face,hog,server,serverPrediciton;
        initCascades();

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_face_detector);


        face=(Button)findViewById(R.id.faceDetector);
        face.setOnClickListener(v -> {
            Intent intent = new Intent(FaceDetector.this, CameraActivity.class);
            intent.putExtra("param_key", "Face");
            startActivity(intent);
        });

        hog=(Button)findViewById(R.id.HOG);
        hog.setOnClickListener(v -> {
            Intent intent = new Intent(FaceDetector.this, CameraActivity.class);
            intent.putExtra("param_key", "Hog");
            startActivity(intent);
        });

        server = (Button)findViewById(R.id.server);
        server.setOnClickListener(v -> new Thread(() -> {
            delete();
            try {
                List<String> namePath = Arrays.asList("FaceDetection","OriginalVi");
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

        serverPrediciton = (Button)findViewById(R.id.serverprediction);
        serverPrediciton.setOnClickListener(v -> new Thread(() -> {
            deleteHog();
            try {
                List<String> namePath = Arrays.asList("Hog");
                for(String path: namePath){
                    File file = new File(getExternalFilesDir(null), path);
                    File[] listaDeArchivos = file.listFiles();
                    for(File archivo : listaDeArchivos){
                        File imgFile= new File(file+"/"+archivo.getName());
                        connect(imgFile,path);
                    }
                }
                Log.d(TAG, "Se subieron los archivos.");
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
    private void initCascades() {

        File faceCascadeFile = loadCascadeFile(R.raw.lbpcascade_frontalface, "lbpcascade_frontalface.xml");
        File eyeCascadeFile = loadCascadeFile(R.raw.haarcascade_eye, "haarcascade_eye.xml");
        File noseCascadeFile = loadCascadeFile(R.raw.haarcascade_mcs_nose, "haarcascade_mcs_nose.xml");
        File mouthCascadeFile = loadCascadeFile(R.raw.haarcascade_mcs_mouth, "haarcascade_mcs_mouth.xml");


        nativeInitCascades(faceCascadeFile.getAbsolutePath(), eyeCascadeFile.getAbsolutePath(), noseCascadeFile.getAbsolutePath(), mouthCascadeFile.getAbsolutePath());
    }

    private native void nativeInitCascades(String faceCascadePath, String eyeCascadePath, String noseCascadePath, String mouthCascadePath);

    private void connect(File file,String name ) throws IOException {
        String boundary = "----WebKitFormBoundary" + System.currentTimeMillis();
        String LINE_FEED = "\r\n";
        URL url = new URL("http://172.20.10.6:4000/upload?name=" + name);
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
        URL url = new URL("http://172.20.10.6:4000/createVideo?name=" + name);
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
            URL url = new URL("http://172.20.10.6:4000/delete");
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

    private void deleteHog() {
        try {
            URL url = new URL("http://172.20.10.6:4000/deleteHog");
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