#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.H"

typedef struct{           //struct para guardar las apariciones de cada palabra
    char palabra[128];
    unsigned long cont;
}Palabra;


int stringEqual(const void * key1, const void * key2) {
    const char * A = key1;
    const char * B = key2;
    return strcmp(A, B) == 0;
}

long long stringHash(const void * key) {
    long long hash = 5381;
    const char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash = ((hash << 5) + hash) + tolower(*ptr); /* hash * 33 + c */
    }    
    return hash; 
}

char* next_word (FILE *fp) {
    char x[1024];
    /* assumes no word exceeds length of 1023 */
    if (fscanf(fp, " %1023s", x) == 1)
        return strdup(x);
    else
        return NULL;
}

int hayQueEliminar(char c, char* string_chars){
    for(int i=0 ; i<strlen(string_chars) ; i++){
        if(string_chars[i]==c) return 1;
    }
    return 0;
}

char* quitar_caracteres(char* string, char* c){
    int i;
    int j;
    for(i=0 ; i < strlen(string) ; i++){
        if(hayQueEliminar(string[i], c)){
            for(j=i ; j<strlen(string)-1 ;j++){
                string[j] = string[j+1];
        }
        string[j]='\0';
        i--;
    }
}

    return string;
}


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