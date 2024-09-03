#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    VideoCapture cap("/home/kelly/Descargas/spidey.mp4");
    if (!cap.isOpened()) {
        cout << "Error al abrir el video." << endl;
        return -1;
    }

    Mat frame;
    namedWindow("Filtros", WINDOW_FREERATIO);
    namedWindow("Bordes", WINDOW_FREERATIO);

    int porcentaje_sal = 0;
    int porcentaje_pimienta = 0;
    int kernel_size = 3;

    createTrackbar("Sal", "Filtros", &porcentaje_sal, 100);
    createTrackbar("Pimienta", "Filtros", &porcentaje_pimienta, 100);

    // Crear una barra de seguimiento para el tamaño del kernel
    createTrackbar("Kernel Size", "Filtros", &kernel_size, 20, [](int value, void*){
        if (value % 2 == 0) {
            // Si el valor es par, aumentar en 1 para hacerlo impar
            value++;
            setTrackbarPos("Kernel Size", "Filtros", value);
        }
    });

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Video con ruido de sal y pimienta
        Mat ruido_frame = frame.clone();

        int total_pixels = frame.rows * frame.cols;
        int sal_pixels = total_pixels * porcentaje_sal / 100;
        int pimienta_pixels = total_pixels * porcentaje_pimienta / 100;

        for (int i = 0; i < sal_pixels; ++i) {
            int row = rand() % frame.rows;
            int col = rand() % frame.cols;
            ruido_frame.at<Vec3b>(row, col) = Vec3b(255, 255, 255); 
        }

        for (int i = 0; i < pimienta_pixels; ++i) {
            int row = rand() % frame.rows;
            int col = rand() % frame.cols;
            ruido_frame.at<Vec3b>(row, col) = Vec3b(0, 0, 0); 
        }

        // Filtro de mediana
        Mat filtro_mediana;
        medianBlur(ruido_frame, filtro_mediana, kernel_size);

        // Filtro de blur
        Mat filtro_blur;
        blur(ruido_frame, filtro_blur, Size(kernel_size, kernel_size));

        // Filtro Gaussiano
        Mat filtro_gaussiano;
        GaussianBlur(ruido_frame, filtro_gaussiano, Size(kernel_size, kernel_size), 0);

        // Redimensionar los videos al 50%
        resize(ruido_frame, ruido_frame, Size(ruido_frame.cols / 2, ruido_frame.rows / 2));
        resize(filtro_mediana, filtro_mediana, Size(filtro_mediana.cols / 2, filtro_mediana.rows / 2));
        resize(filtro_blur, filtro_blur, Size(filtro_blur.cols / 2, filtro_blur.rows / 2));
        resize(filtro_gaussiano, filtro_gaussiano, Size(filtro_gaussiano.cols / 2, filtro_gaussiano.rows / 2));

        // Agregar etiquetas a cada frame
        putText(ruido_frame, "Video con Ruido", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(filtro_mediana, "Filtro Mediana", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(filtro_blur, "Filtro Blur", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(filtro_gaussiano, "Filtro Gaussiano", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        // Mostrar los videos en la ventana "Filtros"
        Mat ventana_filtros(ruido_frame.rows * 2, ruido_frame.cols * 2, ruido_frame.type());
        ruido_frame.copyTo(ventana_filtros(Rect(0, 0, ruido_frame.cols, ruido_frame.rows)));
        filtro_mediana.copyTo(ventana_filtros(Rect(ruido_frame.cols, 0, filtro_mediana.cols, filtro_mediana.rows)));
        filtro_blur.copyTo(ventana_filtros(Rect(0, ruido_frame.rows, filtro_blur.cols, filtro_blur.rows)));
        filtro_gaussiano.copyTo(ventana_filtros(Rect(filtro_blur.cols, ruido_frame.rows, filtro_gaussiano.cols, filtro_gaussiano.rows)));

        // Ajustar el tamaño de la ventana "Filtros"
        resizeWindow("Filtros", ventana_filtros.cols, ventana_filtros.rows);

        imshow("Filtros", ventana_filtros);

        // Convertir la imagen con ruido a escala de grises para la detección de bordes
        Mat ruido_gray;
        cvtColor(ruido_frame, ruido_gray, COLOR_BGR2GRAY);

        // Convertir la imagen filtrada a escala de grises para la detección de bordes
        Mat filtro_mediana_gray;
        cvtColor(filtro_mediana, filtro_mediana_gray, COLOR_BGR2GRAY);

        // Detección de bordes con Sobel en la imagen con ruido
        Mat sobel_x_ruido, sobel_y_ruido, sobel_ruido;
        Sobel(ruido_gray, sobel_x_ruido, CV_64F, 1, 0, kernel_size);
        Sobel(ruido_gray, sobel_y_ruido, CV_64F, 0, 1, kernel_size);
        magnitude(sobel_x_ruido, sobel_y_ruido, sobel_ruido);
        sobel_ruido.convertTo(sobel_ruido, CV_8U);

        // Detección de bordes con Canny en la imagen con ruido
        Mat canny_ruido;
        Canny(ruido_gray, canny_ruido, 100, 200, kernel_size);

        // Detección de bordes con Sobel en la imagen filtrada
        Mat sobel_x_mediana, sobel_y_mediana, sobel_mediana;
        Sobel(filtro_mediana_gray, sobel_x_mediana, CV_64F, 1, 0, kernel_size);
        Sobel(filtro_mediana_gray, sobel_y_mediana, CV_64F, 0, 1, kernel_size);
        magnitude(sobel_x_mediana, sobel_y_mediana, sobel_mediana);
        sobel_mediana.convertTo(sobel_mediana, CV_8U);

        // Detección de bordes con Canny en la imagen filtrada
        Mat canny_mediana;
        Canny(filtro_mediana_gray, canny_mediana, 100, 200, kernel_size);

        // Redimensionar las imágenes de Sobel y Canny al 50%
        resize(sobel_ruido, sobel_ruido, Size(sobel_ruido.cols / 2, sobel_ruido.rows / 2));
        resize(canny_ruido, canny_ruido, Size(canny_ruido.cols / 2, canny_ruido.rows / 2));
        resize(sobel_mediana, sobel_mediana, Size(sobel_mediana.cols / 2, sobel_mediana.rows / 2));
        resize(canny_mediana, canny_mediana, Size(canny_mediana.cols / 2, canny_mediana.rows / 2));

        // Convertir Sobel y Canny a BGR para mostrarlos en una ventana
        Mat sobel_ruido_bgr, canny_ruido_bgr, sobel_mediana_bgr, canny_mediana_bgr;
        cvtColor(sobel_ruido, sobel_ruido_bgr, COLOR_GRAY2BGR);
        cvtColor(canny_ruido, canny_ruido_bgr, COLOR_GRAY2BGR);
        cvtColor(sobel_mediana, sobel_mediana_bgr, COLOR_GRAY2BGR);
        cvtColor(canny_mediana, canny_mediana_bgr, COLOR_GRAY2BGR);

        // Agregar etiquetas a Sobel y Canny
        putText(sobel_ruido_bgr, "Sobel con Ruido", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(canny_ruido_bgr, "Canny con Ruido", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(sobel_mediana_bgr, "Sobel Filtrado", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(canny_mediana_bgr, "Canny Filtrado", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        // Mostrar los bordes en la ventana "Bordes"
        Mat ventana_bordes(sobel_ruido_bgr.rows * 2, sobel_ruido_bgr.cols * 2, sobel_ruido_bgr.type());
        sobel_ruido_bgr.copyTo(ventana_bordes(Rect(0, 0, sobel_ruido_bgr.cols, sobel_ruido_bgr.rows)));
        canny_ruido_bgr.copyTo(ventana_bordes(Rect(sobel_ruido_bgr.cols, 0, canny_ruido_bgr.cols, canny_ruido_bgr.rows)));
        sobel_mediana_bgr.copyTo(ventana_bordes(Rect(0, sobel_ruido_bgr.rows, sobel_mediana_bgr.cols, sobel_mediana_bgr.rows)));
        canny_mediana_bgr.copyTo(ventana_bordes(Rect(sobel_mediana_bgr.cols, sobel_ruido_bgr.rows, canny_mediana_bgr.cols, canny_mediana_bgr.rows)));

        // Ajustar el tamaño de la ventana "Bordes"
        resizeWindow("Bordes", ventana_bordes.cols, ventana_bordes.rows);

        imshow("Bordes", ventana_bordes);

        char c = waitKey(25);
        if (c == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
