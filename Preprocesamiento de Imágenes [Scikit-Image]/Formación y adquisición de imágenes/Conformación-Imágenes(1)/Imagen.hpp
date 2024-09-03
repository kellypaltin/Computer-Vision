
// En lugar de usar la sentencia "import" en C++/C se usa "include"
// Las librerías/cabeceras de sistema van entre símbolos <>
#include <iostream> // Librería estándar para imprimir en pantalla y leer de teclado
#include <cstdlib> // Ver documentación: https://cplusplus.com/reference/cstdlib/

#include <cstring> // Para manejo de cadenas de texto
#include <fstream> // Librería para leer y escribir archivos de texto

#include <cmath>

using namespace std; // Este espacio de nombres agrupa varias funciones básicas (impresión en pantalla, lectura de teclado, etc)

class Imagen{
    private:
        int codigo;
    
    public:
        Imagen(int);
        void saludo(int = 7);
        void generarImagen(string = "imagenX.pgm");
};