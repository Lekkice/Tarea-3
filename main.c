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
    Libro *libro;                            // libro asociado al struct
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
void menuBuscarTitulo(TreeMap *);
void menuBuscarFrecuencia(MapasGlobales *);
void menuBuscarRelevancia(MapasGlobales *);
void menuBuscarApariciones(MapasGlobales *);
void menuMostrarContexto(MapasGlobales *);

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
        else if(opcion == 2)menuBuscarTitulo(mapas->libros);
        else if(opcion == 0)break;
        else printf("La opción seleccionada no valida\n");
    }
}

void submenuPalabras(MapasGlobales *mapas){
    int opcion;

    while(true){
        printf("1. Buscar palabra con mas frecuencia\n");
        printf("2. Buscar palabra con mas relevancia\n");
        printf("3. Buscar apariciones de una palabra en los documentos\n");
        printf("4. Mostrar palabra en contexto\n");        
        printf("0. volver\n");

        scanf("%i", &opcion);

        if(opcion == 1)menuBuscarFrecuencia(mapas);
        else if(opcion == 2)menuBuscarRelevancia(mapas);
        else if(opcion == 3)menuBuscarApariciones(mapas);
        else if(opcion == 4)menuMostrarContexto(mapas);
        else if(opcion == 0)break;
        else printf("opcion seleccionada no valida\n");
    }
}

/*void calcularRelevancia(Pair *data,Libro *libro, int numDoc){ 
    Palabra *aux = data->value;
    aux->relevancia = (aux->cont/libro->cantPalabras) * log(numDoc/aux->contApariciones);
}*/

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
    printf("Su seleccion de documentos a sido registrada, por favor espere a que termine el proceso de carga\n");
    printf("*suena una musica de ascensor*\n");

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
        mapasGlobales->numDoc += 1;

        fclose(fp);

        //Pair *auxRelev = firstMap(mapasGlobales->palabras);
        // while(auxRelev != NULL){    //se me quema el cerebro ayuda, muchos mapas
        //      int cont = buscarPalabra(auxRelev->key,mapasGlobales);
        //      menuCalcularRelevancia(auxRelev,libro,mapasGlobales->numDoc,cont);
        //      auxRelev = nextTreeMap(mapasGlobales->palabras);
        // }

        printf("Libro %s cargado\n", libro->titulo);
        archivo = nextList(archivos);
    }
    esperarEnter();
}

void mostrarDocumentosOrdenados(TreeMap * mapalibros){
    Pair *aux = firstTreeMap(mapalibros);
    Libro *data;

    while(aux){
        data = aux->value;

        printf("%s. %s, %i palabras y %lu caracteres\n", data->id, aux->key, data->cantPalabras, data->cantCarac);

        aux = nextTreeMap(mapalibros);
    }
    esperarEnter();
}

void menuBuscarPalabra(MapasGlobales *mapas){
    TreeMap *libros = mapas->libros;
    char palabra[128];
    Pair *aux;
    Libro *data;

    printf("Ingrese la palabra que se quiere buscar");
    scanf("%s\n",&palabra);

    while(1){
        aux = searchTreeMap(libros,palabra);
        data = aux->value;
        if(aux == NULL){
            break;
        }
        else{
            printf("%s",data->titulo);
            printf("%s",data->id);
        }

        aux = nextTreeMap(libros);
    }

}

void menuBuscarTitulo(TreeMap *mapaLibros){
    char clave[1024];
    Pair *n = firstTreeMap(mapaLibros);
    char *titulo;
    char *token;
    List *palabras = (List *) malloc(sizeof(List *));
    List *titulosCorrectos = (List *) malloc(sizeof(List *));
    char *p;
    char cero[2] = "0";

    printf("ingrese las palabras que desea buscar\n");
    scanf("%s", &clave);
    while(1){
        if(strcmp(clave,cero) == 0)break;
        pushFront(palabras,clave);
        printf("ingrese otra palabra o ingrese 0: \n");
        scanf("%s", &clave);
    }
    printf("chapalele");

    while(1){
        titulo = strtok(n->key,",");
        p = firstList(palabras);
        token = strtok(titulo," ");
        while (titulo != NULL)
        {
            if(strcmp(p,token) == 0){
                popCurrent(palabras);
                p = firstList(palabras);
            }
            else if(p = NULL){
                p = firstList(palabras);
                token = strtok(titulo," ");
            }
            else p = nextList(palabras);
        }
        
        p = firstList(palabras);
        if(p == NULL) pushFront(titulosCorrectos, n->key);

        n = nextTreeMap(mapaLibros);
        if(n == NULL)break;
    }

    p = firstList(titulosCorrectos);
    if(p == NULL){
        printf("no se encontro ningun titulo con las palabras especificadas");
    }
    else{
        while(p != NULL){
            printf("%s",p);
            p = nextList(titulosCorrectos);
        }
    }
}

void menuBuscarFrecuencia(MapasGlobales *mapas)
{

}

void menuBuscarRelevancia(MapasGlobales *mapas)
{

}

void menuBuscarApariciones(MapasGlobales *mapas)
{

}

void menuMostrarContexto(MapasGlobales *mapas){
    char titulo[128];
    char palabra[64];
    long *pos;

    printf("ingrese el titulo del libro a buscar\n");
    scanf("%[\n]s",&titulo);
    printf("ingrese la palabra que desea buscar\n");
    scanf("%[\n]",&palabra);

    Pair *aux = searchTreeMap(mapas->libros,titulo);
    char *idLibro = aux->value;

    printf("%s",idLibro);

    if(aux){
        FILE *fp = fopen(idLibro,"r");
        Libro* auxStruct = aux->value;
        TreeMap *mapaPalabras = auxStruct->palabras;
        if(searchTreeMap(mapaPalabras,palabra) != NULL){
            aux = firstTreeMap(mapaPalabras);
            Palabra *laMamaDeLaMama = aux->value;
            List *palabraPos = laMamaDeLaMama->posiciones;
            pos = firstList(palabraPos);
            while(pos != NULL){
                fseek(fp,-10,*pos);
                pos = nextList(palabraPos);
            }
        }
    }
    else{
        printf("No existe el libro");
        esperarEnter();
        return;
    }
}
