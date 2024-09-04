package com.example.tarea;

import static org.opencv.core.Core.ROTATE_90_CLOCKWISE;
import static org.opencv.core.Core.rotate;
import static org.opencv.imgproc.Imgproc.COLOR_BGR2RGB;
import static org.opencv.imgproc.Imgproc.cvtColor;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;

import org.opencv.android.CameraActivity;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

import java.io.File;
import java.util.Collections;
import java.util.List;

public class viewCamara extends CameraActivity {

    String TAG = "CameraActivity";
    Mat img;
    private native String Classification(long mat);
    private native String momentsZernike(long mat);

    private native String momentResult();
    private native long matMascara();

    private native String ClassificationMaterial(long mat);
    private native long matLBP();
    private native long matHistogram();


    CameraBridgeViewBase cameraBridgeViewBase;
    ImageView take_photo_btn;
    int frameHeight, frameWidth;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_view_camara);

        Intent intent = getIntent();
        String paramValue = intent.getStringExtra("param_key");

        getPermission();
        cameraBridgeViewBase = findViewById(R.id.cameraView);

        cameraBridgeViewBase.setCvCameraViewListener(new CameraBridgeViewBase.CvCameraViewListener2() {
            @Override
            public void onCameraViewStarted(int width, int height) {
                frameHeight = height;
                frameWidth = width;
            }

            @Override
            public void onCameraViewStopped() {
            }

            @Override
            public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
                img = inputFrame.rgba();
                return img;
            }
        });

        if(OpenCVLoader.initDebug()){
            cameraBridgeViewBase.enableView();
        }

        take_photo_btn = findViewById(R.id.take_photo_btn);
        take_photo_btn.setOnClickListener(v -> processFrame(img, paramValue));
    }

    private void processFrame(Mat img, String paramValue) {
        Log.i(TAG, "Entro a saveFrame con paramValue: " + paramValue);
        if(paramValue.equals("PartOne")){
            cvtColor(img, img,COLOR_BGR2RGB);
            String as = Classification(img.getNativeObjAddr());
            String asZernike = momentsZernike(img.getNativeObjAddr());
            String moments = momentResult();
            long imgProcess = matMascara();
            Mat newFrameImg = new Mat(imgProcess);
            File filter = new File(getExternalFilesDir(null), paramValue);
            if (!filter.exists()) filter.mkdirs();
            String filename = "Imagen.jpg";
            String nameimg = "ImgProcess.jpg";
            File file = new File(filter, filename);
            File fileTwo = new File(filter, nameimg);
            Imgcodecs.imwrite(file.toString(), img);
            Imgcodecs.imwrite(fileTwo.toString(), newFrameImg);

            Log.i(TAG, "Imagenes guardada correctamente: " + file.getAbsolutePath());
            Log.i(TAG, "Imagen Clasificada en: " + as);

            Intent intent = new Intent(this, PartOne.class);
            intent.putExtra("imagePath", file.getAbsolutePath()); // Asegúrate de pasar una String
            intent.putExtra("clasificacion", as);
            intent.putExtra("momentsZernike",asZernike);
            intent.putExtra("moments",moments);
            intent.putExtra("imageFilter",fileTwo.getAbsolutePath());
            startActivity(intent);
        }else if(paramValue.equals("PartTwo")){
            cvtColor(img, img,COLOR_BGR2RGB);
            rotate(img, img, ROTATE_90_CLOCKWISE);
            String re = ClassificationMaterial(img.getNativeObjAddr());
            long imgLBP = matLBP();
            Mat newFrameImgLBP = new Mat(imgLBP);
            long imgHistograma = matHistogram();
            Mat newFrameImgHistograma = new Mat(imgHistograma);
            File filter = new File(getExternalFilesDir(null), paramValue);
            if (!filter.exists()) filter.mkdirs();
            String filename = "Imag.jpg";
            String nameLBP = "ImgProcess.jpg";
            String nameHis = "Histogram.jpg";

            File file = new File(filter, filename);
            File fileTwo = new File(filter, nameLBP);
            File fileThere = new File(filter, nameHis);

            Imgcodecs.imwrite(file.toString(), img);
            Imgcodecs.imwrite(fileTwo.toString(), newFrameImgLBP);
            Imgcodecs.imwrite(fileThere.toString(), newFrameImgHistograma);

            Log.i(TAG, "Imagenes guardada correctamente: " + file.getAbsolutePath());
            Log.i(TAG, "Imagen Clasificada en: " + re);

            Intent intent = new Intent(this, PartTwo.class);
            intent.putExtra("imagePath", file.getAbsolutePath());
            intent.putExtra("imageLBP", fileTwo.getAbsolutePath());
            intent.putExtra("imageHistogram",fileThere.getAbsolutePath());
            intent.putExtra("clasificacion",re);
            startActivity(intent);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (cameraBridgeViewBase != null) {
            cameraBridgeViewBase.disableView();
        }
    }

    @Override
    protected List<? extends CameraBridgeViewBase> getCameraViewList() {
        return Collections.singletonList(cameraBridgeViewBase);
    }

    void getPermission(){
        if(checkSelfPermission(android.Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED){
            requestPermissions(new String[]{android.Manifest.permission.CAMERA}, 101);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if(grantResults.length > 0 && grantResults[0] != PackageManager.PERMISSION_GRANTED){
            getPermission();
        }
    }
}
