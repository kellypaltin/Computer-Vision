import os
import time
import psutil
import matplotlib.pyplot as plt
from skimage import io, color, exposure, measure
from matplotlib.backends.backend_pdf import PdfPages
from io import StringIO  # Agrega esta línea

# Inicializar listas para almacenar los resultados
entropies_color = []
entropies_gray = []
histogramas_color_individual = []
histogramas_gray_individual = []
memory_usages = []
processing_times = []


# Función para procesar una imagen
def procesar_imagen(img_path):
    # Leer la imagen
    img = io.imread(img_path)

    # Calcular el histograma de la imagen a color
    hist_color, _ = exposure.histogram(img)
    
    # Convertir a escala de grises
    gray_img = color.rgb2gray(img)
    
    hist_gray, _ = exposure.histogram(gray_img)
    
    # Calcular la entropía de la imagen en escala de grises
    entropy_gray = measure.shannon_entropy(gray_img)
    
    # Calcular la entropía de la imagen a color
    entropy_color = measure.shannon_entropy(img)
    
    return img, gray_img, hist_color, hist_gray, entropy_color, entropy_gray

# Ruta donde se encuentran las imágenes
ruta_imagenes = "/home/kelly/Descargas/assignments/PALTIN_GUZMAN_KELLY_DENISE"


    # Crear un objeto PdfPages para guardar las gráficas en un PDF
with PdfPages('resultados.pdf') as pdf_pages:
    # Procesar cada imagen en la carpeta
    for img_name in os.listdir(ruta_imagenes):
        img_path = os.path.join(ruta_imagenes, img_name)
        if os.path.isfile(img_path) and img_name.lower().endswith(('.png', '.jpg', '.jpeg', '.gif', '.bmp')):
            start_time = time.time()
            
            # Procesar la imagen
            img, gray_img, hist_color, hist_gray, entropy_color, entropy_gray = procesar_imagen(img_path)
            
            # Calcular el tiempo de procesamiento
            processing_time = time.time() - start_time
            
            # Almacenar las entropías de las imágenes
            entropies_color.append(entropy_color)
            entropies_gray.append(entropy_gray)
            

            # Almacenar los histogramas de las imágenes individuales
            histogramas_color_individual.append(hist_color)
            
            # Almacenar los histogramas de las imágenes individuales
            histogramas_gray_individual.append(hist_gray)

            # Convertir la imagen en escala de grises a tipo de datos uint8
            gray_img_uint8 = exposure.rescale_intensity(gray_img, out_range=(0, 255)).astype('uint8')

            # Guardar la imagen en escala de grises en formato PNG
            gray_img_path = os.path.splitext(img_path)[0] + '_gray.png'
            io.imsave(gray_img_path, gray_img_uint8)

            # Imprimir la información de la imagen
            print(f"Información para {img_name}:")
            print(f"Ruta de la imagen: {img_path}")
            print(f"Entropía de la imagen a color: {entropy_color}")
            print(f"Entropía de la imagen en escala de grises: {entropy_gray}")
            print(f"Tiempo de procesamiento: {processing_time:.2f} segundos")


            # Guardar el histograma combinado de intensidades en color en formato PNG
            hist_color_path = os.path.splitext(img_path)[0] + '_hist_color.png'
            plt.figure()
            plt.hist(img[:,:,0].ravel(), bins=256, color='red', alpha=0.5, label='Rojo')
            plt.hist(img[:,:,1].ravel(), bins=256, color='green', alpha=0.5, label='Verde')
            plt.hist(img[:,:,2].ravel(), bins=256, color='blue', alpha=0.5, label='Azul')
            plt.xlabel('Intensidad')
            plt.ylabel('Frecuencia')
            plt.title('Histograma combinado de intensidades en color')
            plt.legend()
            plt.savefig(hist_color_path)
            plt.close()

            # Guardar el histograma combinado de intensidades en escala de grises en formato PNG
            hist_gray_path = os.path.splitext(img_path)[0] + '_hist_gray.png'
            plt.figure()
            plt.hist(gray_img.ravel(), bins=256, color='gray', alpha=0.5)
            plt.xlabel('Intensidad')
            plt.ylabel('Frecuencia')
            plt.title('Histograma de intensidades en escala de grises')
            plt.savefig(hist_gray_path)
            plt.close()

            # Guardar el tiempo de procesamiento
            processing_times.append(processing_time)

            
            # Recopilar datos de uso de memoria RAM y CPU
            memory_usage = psutil.virtual_memory().used / 1024  # Uso de memoria RAM en KB
            cpu_usage = psutil.cpu_percent()  # Porcentaje de uso de CPU
            memory_usages.append((memory_usage, cpu_usage))


# Finalmente, generar las visualizaciones restantes
# y guardarlas en el PDF
# Calcular el promedio del uso de memoria RAM y CPU en kilobytes
memory_usage_avg_kb = sum(mem[0] * 1024 for mem in memory_usages) / len(memory_usages)
total_cpu_usage = sum(mem[1] for mem in memory_usages)

# Crear una figura con varios subplots
fig, axs = plt.subplots(2, 2, figsize=(18, 8))

# Generar diagrama de cajas y bigotes para las entropías de las imágenes a color
axs[0, 0].boxplot(entropies_color)
axs[0, 0].set_xlabel('Imágenes a color')
axs[0, 0].set_ylabel('Entropía')
axs[0, 0].set_title('Entropía de las imágenes a color')

# Generar diagrama de cajas y bigotes para las entropías de las imágenes en escala de grises
axs[0, 1].boxplot(entropies_gray)
axs[0, 1].set_xlabel('Imágenes en escala de grises')
axs[0, 1].set_ylabel('Entropía')
axs[0, 1].set_title('Entropía de imágenes en escala de grises')

# Agregar un diagrama de puntos para el tiempo de procesamiento de imágenes
axs[1, 0].scatter(range(len(processing_times)), processing_times)
axs[1, 0].set_xlabel('Imagen')
axs[1, 0].set_ylabel('Tiempo de procesamiento (segundos)')
axs[1, 0].set_title('Tiempo de procesamiento de imágenes')

# Agregar información sobre el uso de memoria RAM y CPU
axs[1, 1].text(0.5, 0.5, f"Uso promedio de RAM: {memory_usage_avg_kb:.2f} kB\nUso % de CPU: {total_cpu_usage:.2f}%",
               horizontalalignment='center', verticalalignment='center', fontsize=12)
axs[1, 1].axis('off')

# Ajustar el espaciado entre los subplots
plt.tight_layout()

# Guardar la figura en el PDF
pdf_pages.savefig(fig)

# Mostrar la figura
plt.show()

# Obtener listas separadas de uso de memoria RAM y CPU
memory_usage_values = [mem[0] * 1024 for mem in memory_usages]
cpu_usage_values = [mem[1] for mem in memory_usages]

# Obtener una lista de tiempos
timestamps = range(len(memory_usages))

# Crear una figura con subplots
fig2, axs = plt.subplots(2, 2, figsize=(12, 8))

# Graficar el uso de memoria RAM en el subplot superior izquierdo
axs[0, 0].plot(timestamps, memory_usage_values, color='blue')
axs[0, 0].set_xlabel('Tiempo')
axs[0, 0].set_ylabel('Uso de Memoria RAM (KB)')
axs[0, 0].set_title('Uso de Memoria RAM')

# Graficar el uso de CPU en el subplot inferior izquierdo
axs[1, 0].plot(timestamps, cpu_usage_values, color='red')
axs[1, 0].set_xlabel('Tiempo')
axs[1, 0].set_ylabel('Uso de CPU (%)')
axs[1, 0].set_title('Uso de CPU')

# Ocultar el subplot superior derecho
axs[0, 1].axis('off')

# Ocultar el subplot inferior derecho
axs[1, 1].axis('off')

# Ajustar el diseño de la figura
plt.tight_layout()
# Guardar la figura en el PDF
pdf_pages.savefig(fig2)

# Mostrar la figura
plt.show()

