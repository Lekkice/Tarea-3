#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "treemap.h"
#include "list.h"

typedef struct{                             // struct para guardar los datos de cada libro
    char titulo[256];
    char id[16];
    unsigned long cantPalabras;
    unsigned long long cantCarac;           // cantidad de caracteres
    TreeMap *palabras;
}Libro;

typedef struct{                             // struct para guardar las apariciones de cada palabra
    char palabra[128];
    Libro libro;                            // libro asociado al struct
    unsigned long cont;
    List *posiciones;                       // lista con las posiciones en donde se encuentra la palabra en el libro
    int relevancia;                         //relevancia de la palabra
}Palabra;

typedef struct
{
    TreeMap *libros;                        //contiene cada TipoLibro
    TreeMap *palabras;                      //contiene las palabras de todos los libros. key = palabra y value = mapa de libros.
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
/*lo que falla es que estamos entregando los arboles con la informacion hay que sacar en especifico lo que necesitamos
por lo tanto, o lo pasamos desde el importar o pasamos las llaves para buscarlo aca dentro*/
int calcularRelevancia(MapasGlobales *mapas){  //(creo que no) funciona, (creo que *Libro y *Palabra estan mal definidas (se me quema el cerebro))
    TreeMap *libros = mapas->libros;
    TreeMap *palabras = mapas->palabras;
    int chapalele;
    // int chapalele = (Palabra->cont/Libro->cantPalabras) * log(mapas->numDoc/Palabra->apariciones); crear una función para encontrar apariciones
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
        printf("1. Cargar documentos\n");
        printf("2. Busqueda por documento\n");
        printf("3. Busqueda por palabras\n");
        printf("0. Salir\n");

        scanf("%i", &opcion);

        if(opcion == 1)menuImportarDocumentos(mapas);
        else if(opcion == 2)submenuLibros(mapas);
        else if(opcion == 3)submenuPalabras(mapas);
        else if(opcion == 0)break;
        else printf("La opcion seleccionada no válida\n");
    }
    return 0;
}

void submenuLibros(MapasGlobales *mapas){
    int opcion;

    while(true){
        printf("1. Mostrar documentos ordenados\n");
        printf("2. Buscar documento por título\n");
        printf("0. Volver\n");

        scanf("%i", &opcion);

        if(opcion == 1)mostrarDocumentosOrdenados(mapas->libros);
        /*else if(opcion == 2)buscarLibros();*/
        else if(opcion == 0)break;
        else printf("La opción seleccionada no valida\n");
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

        scanf("%i", &opcion);

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
    strcpy(libro->id, id); /*libro->id = id;*/
    libro->cantPalabras = cantPalabras;
    libro->cantCarac = cantCarac;
    libro->palabras = mapaPalabra;
}

void agregarMapaGlobal(TreeMap *mapa, char *tituloLibro, Palabra *palabra)
{
    Pair *search = searchTreeMap(mapa, tituloLibro);
    TreeMap *map;
    if (!search)
    {
        map = createTreeMap(lower_than);
        insertTreeMap(mapa, tituloLibro, map);
    }
    else map = search->value;
    insertTreeMap(map, tituloLibro, palabra);
}

void menuImportarDocumentos(MapasGlobales *mapasGlobales){
    char idLibros[1024];

    printf("Ingrese el nombre del o los libros separados por un espacio con la extension .txt: ");
    getchar();
    scanf("%99[^\n]", &idLibros);

    char *token = strtok(idLibros," \n");
    List *archivos = createList();
    while (token)
    {
        pushBack(archivos, token);
        token = strtok(NULL," \n");
    }
    
    char *archivo = firstList(archivos);
    while(archivo != NULL){
        char *carpeta = strdup("Libros\\");
        strcat(carpeta, archivo);
        FILE *fp = fopen(carpeta, "r");

        if (!fp)
        {
            printf("El archivo introducido no es válido.\n");
            esperarEnter();
            return;
        }

        TreeMap *mapaPalabra = createTreeMap(lower_than);

        char tituloAux[256];
        fgets(tituloAux,255,fp);
        tituloAux[strlen(tituloAux)-1] = '\0'; // elimina el \n
        char *titulo = strdup(tituloAux);
        titulo = titulo+34; // elimina la parte de "The Project Gutenberg eBook"

        char *id = strtok(archivo, ".");
        unsigned int cantPalabras = 0;
        unsigned int cantCarac = 0;

        char *palabra = next_word(fp);
        while(palabra){
            long posAux = ftell(fp);
            cantPalabras++;
            cantCarac += strlen(palabra) + 1;
            
            /*Palabra *aux = searchTreeMap(mapaPalabra, palabra);*/
            Pair *search = searchTreeMap(mapaPalabra, palabra);
            Palabra *aux; // lo que hice antes en agregarMapaGlobal
            if(search){
                aux = search->value;
                aux->cont++;
            }
            else{
                aux = (Palabra *) malloc (sizeof(Palabra));
                aux->posiciones = createList();
                agregarMapaGlobal(mapasGlobales->palabras, titulo, aux);
                aux->cont = 1;
                strcpy(aux->palabra,palabra);
                insertTreeMap(mapaPalabra,palabra,aux);
            }
            
            long *pos = (long *) malloc (sizeof(long));
            *pos = posAux;

            pushBack(aux->posiciones, pos);
            palabra = next_word(fp);
        }

        Libro *libro = cargarLibro(titulo,id,cantPalabras,cantCarac,mapaPalabra);
        insertTreeMap(mapasGlobales->libros,libro->titulo,libro);
        mapasGlobales->numDoc++;

        fclose(fp);

        // while(mapasGlobales->palabras != NULL){    //se me quema el cerebro ayuda, muchos mapas
        //     Palabra *aux = searchTreeMap(mapasGlobales->palabras,); //hay que entregar la palabra o el tipo palabra en especifico
        //     aux->relevancia = calcularRelevancia(mapasGlobales);
        //     nextTreeMap(mapasGlobales->palabras);
        // }

        printf("Libro %s cargado\n", libro->titulo);
        archivo = nextList(archivos);
    }
    esperarEnter();
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