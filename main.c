#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "treemap.h"
#include "list.h"

typedef struct{                             // struct para guardar los datos de cada libro
    char titulo[256];
    char id[16];
    long cantPalabras;
    long cantCarac;
    TreeMap *palabras;                      // mapa de búsqueda libro -> palabras
    TreeMap *keywords;                      // separa el titulo en palabras claves para facilitar búsqueda
}Libro;

typedef struct{                             // struct para guardar las apariciones de cada palabra
    char *palabra;
    unsigned long cont;
    List *posiciones;                       // lista con las posiciones en donde se encuentra la palabra en el libro
    float relevancia;                       // relevancia de la palabra
}Palabra;

typedef struct
{
    TreeMap *libros;                        // contiene cada TipoLibro
    TreeMap *palabras;                      // mapa de búsqueda palabra -> libros
    int numDoc;                             // cantidad de documentos cargados
} MapasGlobales;

char* next_word (FILE *fp) { // retorna la siguiente palabra respecto a la posición del fp
    char x[1024];
    if (fscanf(fp, " %1023s", x) == 1)
        return strdup(x);
    else
        return NULL;
}

char* quitarSimbolos(char* string){ // quita caracteres que son símbolos
    int i;
    for(i=0 ; i < strlen(string) ; i++)
    {
        if (!isalpha(string[i])) // si el caracter no es un alphabet
        {
            string[i] = '\0'; // termina el string
            break;
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

int lower_than_int(void *key1, void *key2)
{
    int *i1 = (int*) key1;
    int *i2 = (int*) key2;
    if (*i1 < *i2) return 1;
    return 0;
}

int lower_than_float(void *key1, void *key2)
{
    float *i1 = (float*) key1;
    float *i2 = (float*) key2;
    if (*i1 < *i2) return 1;
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
void menuBuscarPalabra(MapasGlobales *);

int main()
{
    int opcion;

    MapasGlobales *mapas = (MapasGlobales *) malloc (sizeof(MapasGlobales));
    mapas->libros = createTreeMap(lower_than);
    mapas->palabras = createTreeMap(lower_than);
    mapas->numDoc = 0;

    while(1){
        printf(" || Menú ||\n");
        printf("1. Cargar documentos\n"); // 1.
        printf("2. Búsqueda por documento\n");
        printf("3. Búsqueda por palabras\n");
        printf("0. Salir\n");

        scanf("%i", &opcion);

        if(opcion == 1)menuImportarDocumentos(mapas);
        else if(opcion == 2)submenuLibros(mapas);
        else if(opcion == 3)submenuPalabras(mapas);
        else if(opcion == 0)break;
        else printf("La opción seleccionada no es válida\n");
    }
    return 0;
}

void submenuLibros(MapasGlobales *mapas){
    int opcion;

    while(1){
        printf(" || Menú ||\n");
        printf("1. Mostrar documentos ordenados\n"); // 2.
        printf("2. Buscar documento por título\n"); // 3.
        printf("0. Volver\n");

        scanf("%i", &opcion);

        if(opcion == 1)mostrarDocumentosOrdenados(mapas->libros);
        else if(opcion == 2)menuBuscarTitulo(mapas->libros);
        else if(opcion == 0)break;
        else printf("La opción seleccionada no es válida\n");
    }
}

void submenuPalabras(MapasGlobales *mapas){
    int opcion;

    while(1){
        printf(" || Menú ||\n");
        printf("1. Buscar palabra con más frecuencia\n"); // 4.
        printf("2. Buscar palabra con más relevancia\n"); // 5.
        printf("3. Buscar una palabra en los documentos\n");  // 6.
        printf("4. Mostrar palabra en contexto\n"); // 7.     
        printf("0. Volver\n");

        scanf("%i", &opcion);

        if(opcion == 1)menuBuscarFrecuencia(mapas);
        else if(opcion == 2)menuBuscarRelevancia(mapas);
        else if(opcion == 3)menuBuscarPalabra(mapas);
        else if(opcion == 4)menuMostrarContexto(mapas);
        else if(opcion == 0)break;
        else printf("La opción seleccionada no es válida\n");
    }
}

// calcula la relevancia utilizando la fórmula
void calcularRelevancia(MapasGlobales *mapas, Palabra *palabra, Libro *libro){
    Pair *search = searchTreeMap(mapas->palabras, palabra->palabra);
    if (!search) return;
    int cont = countList(search->value);

    float factorPalabras = ((float)palabra->cont / (float)libro->cantPalabras);
    float factorLog = log((float)mapas->numDoc / (float)cont);

    float relevancia = factorPalabras * factorLog;
    palabra->relevancia = relevancia;
}

Libro *cargarLibro(char *titulo, char* id, TreeMap *mapaPalabra){
    Libro *libro = (Libro*) malloc(sizeof(Libro));
    strcpy(libro->titulo, titulo);
    strcpy(libro->id, id);
    libro->palabras = mapaPalabra;
}

// agrega una palabra al mapa global de palabras
void agregarMapaGlobal(TreeMap *mapa, char *palabra, Libro *libro)
{
    Pair *search = searchTreeMap(mapa, palabra);
    List *list;
    if (!search)
    {
        list = createList();
        insertTreeMap(mapa, palabra, list);
    }
    else list = search->value;
    pushBack(list, libro);
}

char *aMinus(char *string)
{
    for (int i=0; i<strlen(string); i++)
    {
        string[i] = tolower(string[i]);
    }
    return string;
}

// salta las palabras que están contenidas en la lista
void crearListaBloqueo(TreeMap *map){ //MAS MAPAAAAAS, VAMOOOOO
    insertTreeMap(map,"the",NULL); insertTreeMap(map,"and",NULL); insertTreeMap(map,"for",NULL);
    insertTreeMap(map,"is",NULL); insertTreeMap(map,"was",NULL); insertTreeMap(map,"were",NULL);
    insertTreeMap(map,"his",NULL); insertTreeMap(map,"she",NULL); insertTreeMap(map,"him",NULL); 
    insertTreeMap(map,"her",NULL); insertTreeMap(map,"they",NULL); insertTreeMap(map,"but",NULL); 
    insertTreeMap(map,"at",NULL); insertTreeMap(map,"a",NULL); insertTreeMap(map,"to",NULL); 
    insertTreeMap(map,"that",NULL); insertTreeMap(map,"you",NULL); insertTreeMap(map,"with",NULL);
    insertTreeMap(map,"in",NULL); insertTreeMap(map,"of",NULL); insertTreeMap(map,"it",NULL);
    insertTreeMap(map,"as",NULL); insertTreeMap(map,"on",NULL); insertTreeMap(map,"be",NULL);
    insertTreeMap(map,"or",NULL); insertTreeMap(map,"so",NULL); insertTreeMap(map,"not",NULL);
    insertTreeMap(map,"he",NULL);
}

void menuImportarDocumentos(MapasGlobales *mapasGlobales){
    char idLibros[1024];
    TreeMap *bloqueo = createTreeMap(lower_than);
    crearListaBloqueo(bloqueo);

    printf("Introduzca el nombre del o los libros separados por un espacio con la extension .txt: ");
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
        while (strlen(tituloAux) < 6) fgets(tituloAux,255,fp); // si el titulo no está en la primera linea
        tituloAux[strlen(tituloAux)-1] = '\0'; // elimina el \n
        char *titulo = strdup(tituloAux);
        titulo = titulo+34; // elimina la parte de "The Project Gutenberg eBook"

        char *id = strtok(archivo, ".");
        long cantPalabras = 0;
        long cantCarac = 0;
        Libro *libro = cargarLibro(titulo,id,mapaPalabra);

        char *palabra = next_word(fp);
        while(palabra){
            cantCarac += strlen(palabra) + 1;
            palabra = quitarSimbolos(palabra);
            palabra = aMinus(palabra);
            if (!palabra || strlen(palabra) < 2 || searchTreeMap(bloqueo, palabra))
            {
                palabra = next_word(fp);
                continue;
            }

            long posAux = ftell(fp);
            cantPalabras++;
            
            Pair *search = searchTreeMap(mapaPalabra, palabra);
            Palabra *aux;

            if (search){
                aux = search->value;
                aux->cont++;
            }
            else{
                aux = (Palabra *) malloc (sizeof(Palabra));
                aux->posiciones = createList();
                aux->cont = 1;
                aux->palabra = strdup(palabra);
                insertTreeMap(mapaPalabra, palabra, aux);
                agregarMapaGlobal(mapasGlobales->palabras, palabra, libro);
            }

            long *pos = (long *) malloc (sizeof(long));
            *pos = posAux;

            pushBack(aux->posiciones, pos);
            palabra = next_word(fp);
        }
        libro->cantCarac = cantCarac;
        libro->cantPalabras = cantPalabras;
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
    char input[128];

    printf("Introduzca la palabra que desea buscar: ");
    scanf("%s",&input);
    Pair *aux = searchTreeMap(mapas->palabras, input); if (!aux) return;
    List *listaLibros = aux->value;
    Libro *libro = firstList(listaLibros);

    while (libro)
    {
        printf("Libro: %s\n", libro->titulo);
        libro = nextList(listaLibros);
    }
    esperarEnter();
}

// separa el string en palabras clave para facilitar la búsqueda
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

// usa una lista con palabras claves para buscar libros
List *buscarPalabrasClaves(TreeMap *mapaLibros, List *palabras)
{
    List *titulos = createList();
    char *palabra;
    Libro *libro;
    Pair *search;
    TreeMap *keywords;

    Pair *aux = firstTreeMap(mapaLibros);
    while(aux){
        libro = aux->value;
        keywords = libro->keywords;
        
        int flag = 0;
        palabra = firstList(palabras);
        while (palabra)
        {
            search = searchTreeMap(keywords, palabra);
            if (!search) { // si la palabra no está en el título
                flag = 1; break;
            }
            palabra = nextList(palabras);
        }
        if (!flag) pushFront(titulos, libro->titulo);
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
    char id[16];
    Pair *aux = firstTreeMap(mapas->libros);
    Libro *libro;
    
    printf("Introduzca el id del libro que desea buscar\n");
    scanf("%s",id);

    while(aux){
        libro = aux->value;
        if(strcmp(id,libro->id) == 0) break;
        else{
            aux = nextTreeMap(mapas->libros);}
    }

    aux = firstTreeMap(libro->palabras);
    Palabra *palabra = aux->value;
    TreeMap *palabrasFrecuentes = createTreeMap(lower_than_int); // mapa para ordenar palabras por frecuencia
    int count;
    while(aux){
        palabra = aux->value;
        count = countList(palabra->posiciones);
        int *key = (int *) malloc (sizeof(int));
        *key = count;

        insertTreeMap(palabrasFrecuentes, key, palabra);
        aux = nextTreeMap(libro->palabras);
    }

    List *lista = createList(); // se usa como pila para recorrer el arbol en reversa
    aux = firstTreeMap(palabrasFrecuentes);
    while (aux)
    {
        palabra = aux->value;
        pushFront(lista, palabra);
        aux = nextTreeMap(palabrasFrecuentes);
    }
    
    palabra = firstList(lista);
    printf("Las palabras más frecuentes son:\n");
    for(int i = 1; i <= 10; i++){
        if (!palabra) break;
        printf("%i. %s %i veces\n", i, palabra->palabra, countList(palabra->posiciones));
        palabra = nextList(lista);
    }
    esperarEnter();
}

void menuBuscarRelevancia(MapasGlobales *mapas)
{
    char titulo[256];

    getchar();
    printf("Introduzca el titulo del libro: ");
    fgets(titulo, 256, stdin);
    List *clavesTitulo = separarEnClaves(titulo);
    Libro *libro = firstList(buscarPalabrasClaves(mapas->libros, clavesTitulo));

    if(!libro)
    {
        printf("El libro no se ha encontrado\n");
        esperarEnter();
        return;
    }

    TreeMap *palabras = libro->palabras;

    Pair *aux = firstTreeMap(palabras);
    Palabra *palabra;
    while (aux)
    {
        palabra = aux->value;
        calcularRelevancia(mapas, palabra, libro);
        aux = nextTreeMap(palabras);
    }

    aux = firstTreeMap(libro->palabras);
    palabra = aux->value;
    TreeMap *palabrasRelevantes = createTreeMap(lower_than_float); // mapa para ordenar palabras por relevancia
    float relev;
    while(aux){
        palabra = aux->value;
        relev = palabra->relevancia;
        float *key = (float *) malloc (sizeof(float));
        *key = relev;

        insertTreeMap(palabrasRelevantes, key, palabra);
        aux = nextTreeMap(libro->palabras);
    }

    List *lista = createList(); // se usa como pila para recorrer el arbol en reversa
    aux = firstTreeMap(palabrasRelevantes);
    while (aux)
    {
        palabra = aux->value;
        pushFront(lista, palabra);
        aux = nextTreeMap(palabrasRelevantes);
    }

    palabra = firstList(lista);
    printf("Las palabras más relevantes son:\n");
    for(int i = 1; i <= 10; i++){
        if (!palabra) break;
        printf("%i. %s\n", i, palabra->palabra, palabra->relevancia);
        palabra = nextList(lista);
    }
    esperarEnter();
}

// imprime el contexto de una palabra
void imprimirContexto(FILE *fp, long pos)
{
    long auxPos = pos - 75; // empieza 75 caracteres antes de la palabra
    if (auxPos < 0) auxPos = 0;
    fseek(fp, auxPos, SEEK_SET);

    char *string;
    int size = 150;
    int len;
    printf("... ");
    next_word(fp);
    while (size > 0)
    {
        string = next_word(fp);
        len = strlen(string);
        if (len > size) break;
        size -= strlen(string)+1; // disminuye el size por el largo del string
        for (int i=0; i<strlen(string); i++) // elimina newlines
        {
            if (string[i] == '\n') string[i] = ' ';
        }
        printf("%s ", string);
        free(string);
    }
    printf("...\n\n");
}


void menuMostrarContexto(MapasGlobales *mapas){
    char titulo[256];
    char palabraInput[64];
    long *pos;

    getchar();
    printf("Introduzca el título del libro que desea buscar: ");
    fgets(titulo, 256, stdin);
    
    printf("Introduzca la palabra que desea buscar: ");
    fgets(palabraInput, 64, stdin);
    char *palabra = strtok(palabraInput, "\n"); // elimina el \n al final
    aMinus(palabra);

    List *clavesTitulo = separarEnClaves(titulo); // búsqueda por palabras claves
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