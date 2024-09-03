
#include "Imagen.hpp"

Imagen::Imagen(int codigo){
    this->codigo = codigo;
}

void Imagen::saludo(int conteo){
    for(int i=0;i<conteo;i++){
        cout << "Hola desde C++!" << endl;
    }
}

void Imagen::generarImagen(string nombre){
    ofstream buffer(nombre, ios::out);

    buffer << "P2" << endl;
    buffer << "800 600" << endl;
    buffer << "255" << endl;

    for(int i=0;i<600;i++){
        for(int j=0;j<800;j++){
            if(i==j){
                buffer << "255 ";
            }else{
                buffer << "13 ";
            }
        }
        buffer << endl;
    }
    buffer.close();
}