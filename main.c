#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
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
    TreeMap *keywords;
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
        printf("3. Buscar apariciones de una palabra en los documentos\n");  //REVISAR URGENTE (no la piden en la tarea, sobra.)
        printf("4. Mostrar palabra en contexto\n");        
        printf("0. volver\n");

        scanf("%i", &opcion);

        if(opcion == 1)menuBuscarFrecuencia(mapas);
        else if(opcion == 2)menuBuscarRelevancia(mapas);
        //else if(opcion == 3)menuBuscarApariciones(mapas);
        else if(opcion == 4)menuMostrarContexto(mapas);
        else if(opcion == 0)break;
        else printf("Opción seleccionada no valida\n");
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

char *aMinus(char *string)
{
    for (int i=0; i<strlen(string); i++)
    {
        string[i] = tolower(string[i]);
    }
    return string;
}


void crearListaBloqueo(TreeMap *map){ //MAS MAPAAAAAS, VAMOOOOO
    insertTreeMap(map,"the",NULL);
    insertTreeMap(map,"The",NULL);
    insertTreeMap(map,"and",NULL);
    insertTreeMap(map,"And",NULL);
    insertTreeMap(map,"for",NULL);
    insertTreeMap(map,"For",NULL);
    insertTreeMap(map,"is",NULL);
    insertTreeMap(map,"Is",NULL);
    insertTreeMap(map,"was",NULL);
    insertTreeMap(map,"Was",NULL);
    insertTreeMap(map,"were",NULL);
    insertTreeMap(map,"Were",NULL);
    insertTreeMap(map,"his",NULL);
    insertTreeMap(map,"His",NULL);
    insertTreeMap(map,"she",NULL);
    insertTreeMap(map,"She",NULL);
    insertTreeMap(map,"him",NULL);
    insertTreeMap(map,"Him",NULL);
    insertTreeMap(map,"her",NULL);
    insertTreeMap(map,"Her",NULL);
    insertTreeMap(map,"they",NULL);
    insertTreeMap(map,"They",NULL);
    insertTreeMap(map,"but",NULL);
    insertTreeMap(map,"But",NULL);
    insertTreeMap(map,"at",NULL);
    insertTreeMap(map,"At",NULL);
    insertTreeMap(map,"a",NULL);
    insertTreeMap(map,"A",NULL);
    insertTreeMap(map,"to",NULL);
    insertTreeMap(map,"To",NULL);
    insertTreeMap(map,"that",NULL);
    insertTreeMap(map,"That",NULL);
}

void menuImportarDocumentos(MapasGlobales *mapasGlobales){
    char idLibros[1024];
    TreeMap *bloqueo = createTreeMap(lower_than);
    crearListaBloqueo(bloqueo);

    printf("Ingrese el nombre del o los libros separados por un espacio con la extension .txt: ");
    getchar();
    scanf("%99[^\n]", idLibros);

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
            palabra = aMinus(palabra);
            Pair *search = searchTreeMap(mapaPalabra, palabra);
            Palabra *aux; // lo que hice antes en agregarMapaGlobal

            if(!searchTreeMap(bloqueo,palabra)){
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

        TreeMap *keywords = createTreeMap(lower_than);
        char *token = strtok(titulo,", ’ ");
        while (token)
        {
            insertTreeMap(keywords, aMinus(token), NULL); // sólo se necesita el token
            token = strtok(NULL,", ’ ");
        }
        libro->keywords = keywords;

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
    scanf("%s\n", palabra);

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

List *separarEnClaves(char *string)
{
    List *lista = createList();
    char *token = strtok(string, ", ’ \n");
    while(token){
        pushFront(lista,aMinus(token));
        token = strtok(NULL, ", ’ \n");
    }
    return lista;
}

List *buscarPalabrasClaves(TreeMap *mapaLibros, List *palabras)
{
    List *titulos = createList();
    char *palabra;
    Pair *aux = firstTreeMap(mapaLibros);
    Libro *libro;
    Pair *search;
    TreeMap *keywords;

    while(aux){
        libro = aux->value;
        keywords = libro->keywords;
        
        int flag = 0;
        palabra = firstList(palabras);
        while (palabra)
        {
            search = searchTreeMap(keywords, palabra);
            if (!search) {
                flag = 1; break;
            }
            palabra = nextList(palabras);
        }
        if (flag) continue;
        pushFront(titulos, libro->titulo);
        aux = nextTreeMap(mapaLibros);
    }
    return titulos;
}

void menuBuscarTitulo(TreeMap *mapaLibros){
    char clave[1024];

    printf("Introduzca el título que desea buscar, puede utilizar palabras clave\n");
    getchar();
    fgets(clave, 256, stdin);

    List *palabras = separarEnClaves(clave);

    List *titulosCorrectos = buscarPalabrasClaves(mapaLibros, palabras);

    Libro *p = firstList(titulosCorrectos);
    if(p == NULL){
        printf("No se encontró título con las palabras especificadas\n");
        return;
    }

    while(p != NULL){
        printf("%s\n",p->titulo);
        p = nextList(titulosCorrectos);
    }
    esperarEnter();
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

void imprimirContexto(FILE *fp, long pos)
{
    long auxPos = pos - 50;
    if (auxPos < 0) auxPos = 0;
    fseek(fp, auxPos, SEEK_SET);

    char string[110];
    int size = 100;
    printf("... ");
    while (size > 0)
    {
        fgets(string, size, fp);
        size -= strlen(string)+1;
        for (int i=0; i<strlen(string); i++) // elimina newlines
        {
            if (string[i] == '\n') string[i] = ' ';
        }
        printf("%s", string);
    }
    printf(" ...\n\n");
}

// funciona
void menuMostrarContexto(MapasGlobales *mapas){
    char titulo[256];
    char palabraInput[64];
    long *pos;
    char contexto[100];

    getchar();
    printf("Introduzca el título del libro que desea buscar: ");
    fgets(titulo, 256, stdin);
    
    printf("Introduzca la palabra que desea buscar: ");
    fgets(palabraInput, 64, stdin);
    char *palabra = strtok(palabraInput, "\n"); // elimina el \n al final
    aMinus(palabra);

    List *clavesTitulo = separarEnClaves(titulo);
    Libro *libro = firstList(buscarPalabrasClaves(mapas->libros, clavesTitulo));

    if(libro){
        char *archivo = strdup("Libros\\");
        strcat(archivo, libro->id);
        strcat(archivo, ".txt");
        FILE *fp = fopen(archivo,"r");
        TreeMap *mapaPalabras = libro->palabras;

        Pair *search = searchTreeMap(mapaPalabras, palabra);
        if(search){
            List *apariciones = ((Palabra *)search->value)->posiciones;
            pos = firstList(apariciones);
            while(pos){
                imprimirContexto(fp, *pos);
                pos = nextList(apariciones);
            }
        }
        else
        {
            printf("La palabra no existe en el libro\n");
            esperarEnter();
            return;
        }
    }
    else{
        printf("El libro no existe\n");
        esperarEnter();
        return;
    }
}