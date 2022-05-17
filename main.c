#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "treemap.h"
#include "list.h"

typedef struct{                             // struct para guardar las apariciones de cada palabra
    char palabra[128];
    unsigned long cont;
    List *posiciones;                       // lista con las posiciones en donde se encuentra la palabra en el libro
    int apariciones;                        //cantidad de libros en los que aparece la palabra
    int relevancia;                         //relevancia de la palabra
    List *aparicionLibros;
}Palabra;

typedef struct{                             // struct para guardar los datos de cada libro
    char titulo[256];
    unsigned long id;
    unsigned long cantPalabras;
    unsigned long long cantCarac;           // cantidad de caracteres
    TreeMap *palabras;
}Libro;

typedef struct
{
    TreeMap *libros;                        //contiene cada TipoLibro
    TreeMap *palabras;                      //contiene las palabras de todos los libros. key = palabra y value = libro.
    int numDoc;                             //necesitaba un lugar para guardarlo
} MapasGlobales;

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

int calcularRelevancia(MapasGlobales *mapas){  //(creo que no) funciona, (creo que *Libro y *Palabra estan mal definidas (se me quema el cerebro))
    Libro *Libro = mapas->libros;
    Palabra *Palabra = mapas->palabras;

    int chapalele = (Palabra->cont/Libro->cantPalabras) * log(mapas->numDoc/Palabra->apariciones);
    return chapalele;
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

void submenuPalabras(MapasGlobales *);
void submenuLibros(MapasGlobales *);
void menuImportarDocumentos(MapasGlobales *);
void mostrarDocumentosOrdenados(TreeMap *);

int main()
{
    int opcion;

    MapasGlobales *mapas = (MapasGlobales *) malloc (sizeof(MapasGlobales));
    mapas->libros = createTreeMap(lower_than);
    mapas->palabras = createTreeMap(lower_than);

    while(true){
        printf("1. Cargar documentos");
        printf("2. Busqueda por documento");
        printf("3. Busqueda por palabras");
        printf("0. Salir");

        scanf("%i", opcion);

        if(opcion == 1)menuImportarDocumentos(mapas);
        else if(opcion == 2)submenuLibros(mapas);
        else if(opcion == 3)submenuPalabras(mapas);
        else if(opcion == 0)break;
        else printf("opcion seleccionada no valida");
    }
    return 0;
}

void submenuLibros(MapasGlobales *mapas){
    int opcion;

    while(true){
        printf("1. Mostrar documentos ordenados");
        printf("2. Buscar documento por titulo");
        printf("0. volver");

        scanf("%i", opcion);

        if(opcion == 1)mostrarDocumentosOrdenados(mapas->libros);
        /*else if(opcion == 2)buscarLibros();*/
        else if(opcion == 0)break;
        else printf("opcion seleccionada no valida");
    }
}

void submenuPalabras(MapasGlobales *mapas){
    int opcion;

    while(true){
        printf("1. Buscar palabra con mas frecuencia");
        printf("2. Buscar palabra con mas relevancia");
        printf("3. Buscar apariciones de una palabra en los documentos");
        printf("4. Mostrar palabra en contexto");        
        printf("0. volver");

        scanf("%i", opcion);

        /*if(opcion == 1)buscarFrecuencia();
        else if(opcion == 2)buscarRelevancia();
        else if(opcion == 3)buscarApariciones();
        else if(opcion == 4)mostrarContexto();*/
        /*else*/ if(opcion == 0)break;
        else printf("opcion seleccionada no valida");
    }
}

Libro *cargarLibro(char *titulo, char* id, unsigned int cantPalabras, unsigned int cantCarac, TreeMap *mapaPalabra){
    Libro *libro = (Libro*) malloc(sizeof(Libro));

    strcpy(libro->titulo, titulo);
    strcpy(libro->id, id);
    libro->cantPalabras = cantPalabras;
    libro->cantCarac = cantCarac;
    libro->palabras = mapaPalabra;
}

void agregarMapaGlobal(TreeMap *mapa, char *palabra, long pos)
{
    Palabra *search = searchTreeMap(mapa, palabra);
    if (search)
    {
        search->cont++;
    }
    else
    {
        search = (Palabra *) malloc (sizeof(Palabra));
        search->cont = 1;
    }
    pushBack(search->posiciones, pos);
}

void menuImportarDocumentos(MapasGlobales *mapasGlobales){
    char idLibros[1024];

    printf("Ingrese el nombre del o los libros separados por un espacio con la extension .txt: ");
    scanf("%s", &idLibros);

    char *nombreArchivo = strtok(idLibros," \n");

    while(nombreArchivo != NULL){
        TreeMap *mapaPalabra = createTreeMap(lower_than);

        FILE *fp = fopen(nombreArchivo, "r");

        char *extension = strrchr(nombreArchivo, '.'); // retorna la posición del ultimo '.'
        if (strcmp(extension, ".txt") != 0 || !fp)
        {
            printf("El archivo introducido no es válido.\n");
            esperarEnter();
            return;
        }

        char *titulo = fgets(titulo,256,fp);
        char *id = strtok(nombreArchivo, ".");
        unsigned int cantPalabras = 0;
        unsigned int cantCarac = 0;

        char *palabra = next_word(fp);
        while(palabra){
            long pos = ftell(fp);
            cantPalabras++;
            cantCarac += strlen(palabra) + 1;
            agregarMapaGlobal(mapasGlobales->palabras, palabra, pos);
            
            Palabra *aux = searchTreeMap(mapaPalabra, palabra);
            if(aux){
                aux->cont == aux->cont;                     //antes era aux->cont++ pero funcion agregarMapaGlobal ya lo hace
            }
            else{
                aux = (Palabra *) malloc (sizeof(Palabra));
                //aux->cont = 1;                            //mismo que linea 221
                strcpy(aux->palabra,palabra);
                pushBack(aux->aparicionLibros,id);
                aux->apariciones++;
                insertTreeMap(mapaPalabra,palabra,aux);
            }
            pushBack(aux->posiciones, pos);
            palabra = next_word(fp);
        }

        Libro *libro = cargarLibro(titulo,id,cantPalabras,cantCarac,mapaPalabra);
        insertTreeMap(mapasGlobales->libros,libro->titulo,libro);
        mapasGlobales->numDoc++;

        fclose(fp);

        while(mapasGlobales->palabras != NULL){    //se me quema el cerebro ayuda, muchos mapas
            Palabra *aux = mapasGlobales->palabras;
            aux->relevancia = calcularRelevancia(mapasGlobales);
            nextTreeMap(mapasGlobales->palabras);
        }

        nombreArchivo = strtok(NULL," \n");
    }
}

void mostrarDocumentosOrdenados(TreeMap * mapalibros){
    Pair *aux = firstTreeMap(mapalibros);
    Libro *data = aux->value;

    while(mapalibros != NULL){
        printf("%s",aux->key);
        printf("%lu",data->cantCarac);

        aux = nextTreeMap(mapalibros);
        data = aux->value;
    }
}