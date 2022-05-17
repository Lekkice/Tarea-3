#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "treemap.h"
#include "list.h"

typedef struct{                             //struct para guardar las apariciones de cada palabra
    char palabra[128];
    unsigned long cont;
    List *posiciones; // lista con las posiciones en donde se encuentra la palabra en el libro
}Palabra;

typedef struct{                             //struct para guardar los datos de cada libro
    char titulo[256];
    unsigned long id;
    unsigned long cantPalabras;
    unsigned long long cantCarac;           //cantidad de caracteres
    TreeMap *palabras;
}Libro;


int stringEqual(const void * key1, const void * key2) {
    const char * A = key1;
    const char * B = key2;
    return strcmp(A, B) == 0;
}

char* next_word (FILE *fp) {
    char x[1024];
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

// espera a que el usuario presione ENTER
void esperarEnter() 
{
    printf("Presione ENTER para continuar\n");
    getchar();getchar();
}

int lower_than(void *key1, void *key2)
{
    if (strcmp(key1, key2) < 0) return 1;
    return 0;
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

        if(opcion == 1)menuImportarDocumentos();
        else if(opcion == 2)submenuLibros();
        else if(opcion == 3)submenuPalabras();
        else if(opcion == 0)break;
        else printf("opcion seleccionada no valida");
    }
    return 0;
}

void submenuLibros(){
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

void submenuPalabras(){
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

Libro *cargarLibro(char *titulo, char* id, char* cantPalabras, char* cantCarac){
    Libro *libro = (Libro*) malloc(sizeof(Libro));

    strcpy(libro->titulo, titulo);
    strcpy(libro->id, id);
    strcpy(libro->cantPalabras, cantPalabras);
    strcpy(libro->cantCarac, cantCarac);
}

void menuImportarDocumentos(){
    TreeMap *listaLibros = createMap(lower_than);
    char idLibros[12800];
    Libro *libro;
    Palabra *palabraStruct; //falta colocar un mapa para rellenarlo
    TreeMap *mapaPalabra = createMap(lower_than);

    printf("Ingrese el nombre del o los libros separados por un espacio con la extension .txt: ");
    scanf("%s", &idLibros);

    char *nombreArchivo = strtok(idLibros," \n");

    while(nombreArchivo != NULL){
        FILE *fp = fopen(nombreArchivo, "r");
        char *extension = strrchr(nombreArchivo, '.'); // retorna la posición del ultimo '.'
        if (strcmp(extension, ".txt") != 0 || !fp)
        {
            printf("El archivo introducido no es válido.\n");
            esperarEnter();
            return;
        }

        char *titulo = fgets(titulo,256,fp);
        char *id = strtok(nombreArchivo, ",\n");
        unsigned long cantPalabras;
        unsigned long cantCarac = 0;

        char *palabras = next_word(fp);
        while(palabras){
            Palabra *aux = searchTreeMap(mapaPalabra,palabras);
            if(aux){
                aux->cont++;
            }
            else{
                cantPalabras++;
                strcpy(palabraStruct->palabra,palabras);
                palabraStruct->cont++;
                insertTreeMap(mapaPalabra,palabras,palabraStruct);
            }
            palabras = next_word(fp);
        }
        
        while(1){
            int caracter = fgetc(fp);

            if(caracter == EOF)break;
            if(caracter == 10)continue;
            cantCarac++;
        }

        libro = cargarLibro(titulo,id,cantPalabras,cantCarac);
        insertTreeMap(listaLibros,libro->titulo,libro);
        fclose(fp);
        nombreArchivo = strtok(NULL," ");
    }   
}