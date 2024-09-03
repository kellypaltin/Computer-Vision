
#include "Imagen.hpp"

int main(int argc, char* args[]){
    Imagen imagen(337);

    imagen.saludo();

    imagen.generarImagen();

    double z[3];
    z[0] = 0.1;
    z[1] = 0.2;
    z[2] = 0.3;
    
    int x = 7;
    cout << "x = " << x << endl;

    int *pX = &x;
    cout << "Pos. Memoria = " << pX << " Valor Pos. Memoria = " << *(pX) << endl;

    *(pX) = 27;
    cout << "x = " << x << endl;

    int *arreglo = new int[13];

    for(int i=0;i<13;i++){
        //arreglo[i] = i*10;
        *(arreglo+i) = i*10;
    }


    for(int i=0;i<13;i++){
        cout << arreglo[i] << "#";
    }
    cout << endl;

    // Eliminación de un arreglo tipo puntero de memoria
    delete [] arreglo;


    vector<int> res = imagen.generarSerie();

    for(int i=0;i<res.size(); i++){
        cout << res[i] << "|";
    }
    cout << endl;

    Imagen *pImagen = new Imagen(777);

    pImagen->saludo();

    vector<string> directorio = pImagen->listarDirectorio();

    delete pImagen;

    stringstream ss;
    string cad = "";
    ss << x;
    cad = ss.str();

    cout << cad << endl;

    cad = "123";
    ss.str("");
    ss.clear();

    ss << cad;
    ss >> x;

    cout << "x: " << x << endl;

    ss.str("");
    ss.clear();

    ss << "Hola " << 37 << " " << x << endl;

    cout << ss.str() << endl;

    return 0;
}