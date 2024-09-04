package com.example.project;

import static org.opencv.core.Core.ROTATE_180;
import static org.opencv.core.Core.ROTATE_90_CLOCKWISE;
import static org.opencv.core.Core.rotate;

import androidx.annotation.NonNull;

import android.app.ActivityManager;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Debug;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.tensorflow.lite.Interpreter;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;


public class
CameraActivity extends org.opencv.android.CameraActivity {
    String TAG = "CameraActivity";
    Mat mRgba;

    private static final String[] CLASS_NAMES = {"0","1", "2", "3", "4","5","6","7","8","9"};

    int frameHeight, frameWhit;
    private final List<Mat> framesList = new ArrayList<>();
    private final List<Mat> framesListOri = new ArrayList<>();
    private long lastFrameTime = 0;
    private int frameCount = 0;
    private TextView fpsTextView;
    private TextView memoryTextView;
    private TextView cpuTextView;
    private TextView inferenceTimeTextView;
    private Interpreter tflite;
    String className;
    private boolean capturingFrames = false;

    native long processFace(long mat);
    native long processHog(long mat);
    native long camprocess(long mat);
    public native float[] computeHOG(long matAddr);
    CameraBridgeViewBase cameraBridgeViewBase;
    ImageView take_photo_btn;
    Mat frame;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera2);

        Intent intent = getIntent();
        String paramValue = intent.getStringExtra("param_key");

        fpsTextView = findViewById(R.id.fps_text_view);
        memoryTextView = findViewById(R.id.memory_text_view);
        cpuTextView = findViewById(R.id.cpu_text_view);

        getPermission();
        cameraBridgeViewBase = findViewById(R.id.cameraView);


        cameraBridgeViewBase.setCvCameraViewListener(new CameraBridgeViewBase.CvCameraViewListener2() {
            @Override
            public void onCameraViewStarted(int width, int height) {
                frameHeight = height;
                frameWhit = width;
                lastFrameTime = System.currentTimeMillis();
                new Handler(Looper.getMainLooper()).postDelayed(updateUsageInfoRunnable, 1000);
            }

            @Override
            public void onCameraViewStopped() {

            }


            @Override
            public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {

                long currentTime = System.currentTimeMillis();
                if (lastFrameTime != 0) {
                    long elapsedTime = currentTime - lastFrameTime;
                    frameCount++;
                    if (elapsedTime >= 1000) {
                        final double fps = frameCount / (elapsedTime / 1000.0);
                        runOnUiThread(() -> fpsTextView.setText(String.format("FPS: %.2f", fps)));

                        frameCount = 0;
                        lastFrameTime = currentTime;
                    }
                } else {
                    lastFrameTime = currentTime;
                }

                mRgba = inputFrame.rgba();
                Mat imgO = mRgba.clone();
                if(paramValue.equals("Face")){
                    frame = mRgba.t();
                    processFace(frame.nativeObj);
                    if (capturingFrames) {
                        framesList.add(frame);
                        framesListOri.add(imgO);
                    }

                }else if(paramValue.equals("Hog")){
                    frame = imgO;
                }
                return frame;
            }
        });

        if(OpenCVLoader.initDebug()){
            cameraBridgeViewBase.enableView();
        }

        take_photo_btn = findViewById(R.id.take_photo_btn);
        if(paramValue.equals("Face")){
            take_photo_btn.setOnClickListener(v -> startCapturingFramesForEightSeconds(paramValue));
        }else if(paramValue.equals("Hog")){
            take_photo_btn.setOnClickListener(v -> predicModel(frame));
        }

    }

    private final Runnable updateUsageInfoRunnable = new Runnable() {
        @Override
        public void run() {
            Info();
            new Handler(Looper.getMainLooper()).postDelayed(this, 1000);
        }
    };

    private void Info() {
        ActivityManager activityManager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        activityManager.getMemoryInfo(memoryInfo);
        long availableMegs = memoryInfo.availMem / 1048576L;
        long totalMegs = memoryInfo.totalMem / 1048576L;
        runOnUiThread(() -> memoryTextView.setText(String.format("RAM: %dMB / %dMB", availableMegs, totalMegs)));

        int[] pids = {android.os.Process.myPid()};
        Debug.MemoryInfo[] memoryInfoArray = activityManager.getProcessMemoryInfo(pids);
        int memoryUsage = memoryInfoArray[0].getTotalPss();
        runOnUiThread(() -> cpuTextView.setText(String.format("CPU: %dMB", memoryUsage)));
    }

    private void startCapturingFramesForEightSeconds(String paramValue) {
        capturingFrames = true;
        Log.i(TAG, paramValue);
        new Handler().postDelayed(() -> {
            capturingFrames = false;
            if (paramValue.equals("Face")) {
                saveFrame(framesList, "FaceDetection");
                save(framesListOri,"OriginalVi");
            } else if (paramValue.equals("Hog")) {
                // Handle Hog case if needed
            }
        }, 8000);  // Changed to 8000 for eight seconds
    }

    private void predicModel(Mat img) {
        Log.i(TAG, img.toString());
        Mat hog  = img.clone();
        long mt = processHog(hog.nativeObj);
        Mat imgHog = new Mat(mt);

        float[] hogFeatures = computeHOG(imgHog.getNativeObjAddr());

        Log.d("HOG Features", "Number of HOG features: " + hogFeatures.length);
        Log.d("Model Info", "Input Shape: " + Arrays.toString(hogFeatures));

        try {
            tflite = new Interpreter(loadModelFile());
            int[] inputShape = tflite.getInputTensor(0).shape();
            int[] outputShape = tflite.getOutputTensor(0).shape();

            Log.d("Model Info", "Input Shape: " + Arrays.toString(inputShape));
            Log.d("Model Info", "Output Shape: " + Arrays.toString(outputShape));

            if (inputShape.length > 3) {
                int inputChannels = inputShape[3];
                Log.d("Model Info", "Input Channels: " + inputChannels);
            } else {
                Log.d("Model Info", "Input Shape does not have 4 dimensions, cannot get input channels.");
            }

            Log.d(TAG, "TensorFlow Lite model loaded successfully.");
        } catch (IOException e) {
            e.printStackTrace();
            Log.d(TAG, "TensorFlow Lite model loaded error.");
        }
        float[][] result = runInference(hogFeatures);
        for (int i = 0; i < result.length; i++) {
            StringBuilder sb = new StringBuilder();
            for (int j = 0; j < result[i].length; j++) {
                sb.append(result[i][j]).append(" ");
            }
            Log.d("HOGResult", "Result row " + i + ": " + sb.toString());
        }

        float maxValue = Float.MIN_VALUE;
        int maxRow = -1;
        int maxCol = -1;

        for (int i = 0; i < result.length; i++) {
            for (int j = 0; j < result[i].length; j++) {
                if (result[i][j] > maxValue) {
                    maxValue = result[i][j];
                    maxRow = i;
                    maxCol = j;
                }
            }
        }

        className = CLASS_NAMES[maxCol];

        Log.d("ResultAnalysis", "Best result value: " + maxValue);
        Log.d("ResultAnalysis", "Position: Row " + maxRow + ", Column " + maxCol);
        Log.d("ResultAnalysis ",  "Class name: " +className);
        saveHog(imgHog, "Hog");


    }

    private float[][] runInference(float[] bitmap) {
        float[][] output = new float[1][10];
        tflite.run(bitmap, output);
        return output;
    }

    // Save images
    private void saveFrame(List<Mat> mat, String name) {
        Intent intent = new Intent(this, FaceDetector.class);;

        File filter = new File(getExternalFilesDir(null), name);
        if (!filter.exists()) filter.mkdirs();

        File[] files = filter.listFiles();
        if (files != null) {
            for (File file : files) {
                file.delete();
            }
        }

        for (int i = 0; i < mat.size(); i++) {
            long mt = camprocess(mat.get(i).nativeObj);
            Mat frame = new Mat(mt);
            String filename = "Frame" + i + ".jpg";
            File file = new File(filter, filename);
            boolean bool = Imgcodecs.imwrite(file.toString(), frame);
            frame.release();
            if (bool) {
                Log.i(TAG, "Image saved successfully: " + file);
                intent.putExtra("imagePath", file.getAbsolutePath());
            } else {
                Log.i(TAG, "Failed to write image to external storage");
            }
        }
        framesList.clear();
        startActivity(intent);
    }

    private void saveHog(Mat mat, String name) {
        Intent intent = new Intent(this, FaceDetector.class);;

        File filter = new File(getExternalFilesDir(null), name);
        if (!filter.exists()) filter.mkdirs();

        File[] files = filter.listFiles();
        if (files != null) {
            for (File file : files) {
                file.delete();
            }
        }

        Mat frame = mat.clone();
        String filename = className+".jpg";
        File file = new File(filter, filename);
        boolean bool = Imgcodecs.imwrite(file.toString(), frame);
        frame.release();
        if (bool) {
            Log.i(TAG, "Image saved successfully: " + file);
            intent.putExtra("imagePath", file.getAbsolutePath());
        } else {
            Log.i(TAG, "Failed to write image to external storage");
        }

        startActivity(intent);
    }

    private void save(List<Mat> matIng, String name) {
        File filter = new File(getExternalFilesDir(null), name);
        if (!filter.exists()) filter.mkdirs();

        File[] files = filter.listFiles();
        if (files != null) {
            for (File file : files) {
                file.delete();
            }
        }

        for (int i = 0; i < matIng.size(); i++) {
            long mt = camprocess(matIng.get(i).nativeObj);
            Mat frame = new Mat(mt);
            String filename = "Frame" + i + ".jpg";
            File file = new File(filter, filename);
            boolean bool = Imgcodecs.imwrite(file.toString(), frame);
            frame.release();
            if (bool) {
                Log.i(TAG, "SUCCESS writing image to external storage: " + file);
            } else {
                Log.i(TAG, "Failed to write image to external storage");
            }
        }
    }

    private MappedByteBuffer loadModelFile() throws IOException {
        Resources resources = getResources();
        int modelResourceId = R.raw.mlp_model;
        FileInputStream inputStream = new FileInputStream(resources.openRawResourceFd(modelResourceId).getFileDescriptor());
        FileChannel fileChannel = inputStream.getChannel();
        long startOffset = resources.openRawResourceFd(modelResourceId).getStartOffset();
        long declaredLength = resources.openRawResourceFd(modelResourceId).getDeclaredLength();
        return fileChannel.map(FileChannel.MapMode.READ_ONLY, startOffset, declaredLength);
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
        if(checkSelfPermission(android.Manifest.permission.CAMERA)!= PackageManager.PERMISSION_GRANTED){
            requestPermissions(new String[]{android.Manifest.permission.CAMERA},101);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if(grantResults.length > 0 && grantResults[0]!= PackageManager.PERMISSION_GRANTED){
            getPermission();
        }
    }

}