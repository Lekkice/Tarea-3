#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.H"

int main()
{
    int opcion;

    while(true){
        printf("1. Cargar documentos");
        printf("2. Busqueda por documento");
        printf("3. Busqueda por palabras");
        printf("0. Salir");

        scanf("%i", opcion);

        if(opcion == 1)importarDocumentos();
        else if(opcion == 2)menuLibros();
        else if(opcion == 3)menuPalabras();
        else if(opcion == 0)break;
        else printf("opcion seleccionada no valida");
    }
    return 0;
}

void menuLibros(){
    int opcion;

    while(true){
        printf("1. Mostrar documentos ordenados");
        printf("2. Buscar documento por titulo");
        printf("0. volver");

        scanf("%i", opcion);

        if(opcion == 1)mostrarDocumentosOrdenados();
        else if(opcion == 2)buscarLibros();
        else if(opcion == 0)break;
        else printf("opcion seleccionada no valida");
    }
}

void menuLibros(){
    int opcion;

    while(true){
        printf("1. Buscar palabra con mas frecuencia");
        printf("2. Buscar palabra con mas relevancia");
        printf("3. Buscar apariciones de una palabra en los documentos");
        printf("4. Mostrar palabra en contexto");        
        printf("0. volver");

        scanf("%i", opcion);

        if(opcion == 1)buscarFrecuencia();
        else if(opcion == 2)buscarRelevancia();
        else if(opcion == 3)buscarApariciones();
        else if(opcion == 4)mostrarContexto();
        else if(opcion == 0)break;
        else printf("opcion seleccionada no valida");
    }
}