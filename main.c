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
    long cantPalabras;
    long cantCarac;           // cantidad de caracteres
    TreeMap *palabras;
    TreeMap *keywords;
}Libro;

typedef struct{                             // struct para guardar las apariciones de cada palabra
    char *palabra;
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

char* quitarSimbolos(char* string){
    int i;
    for(i=0 ; i < strlen(string) ; i++)
    {
        if (!isalpha(string[i])) // si el caracter no es un alphabet
        {
            printf("%s %i\n", string, i);
            string[i] = '\0'; // termina el string
            break;
        }
    }
    if (!strlen(string)) return NULL;
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
    if (key1 < key2) return 1;
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

    while(true){
        printf("1. Cargar documentos\n"); // 1.
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
        printf("1. Mostrar documentos ordenados\n"); // 2.
        printf("2. Buscar documento por título\n"); // 3.
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
        printf("1. Buscar palabra con mas frecuencia\n"); // 4.
        printf("2. Buscar palabra con mas relevancia\n"); // 5.
        printf("3. Buscar una palabra en los documentos\n");  // 6.
        printf("4. Mostrar palabra en contexto\n"); // 7.     
        printf("0. Volver\n");

        scanf("%i", &opcion);

        if(opcion == 1)menuBuscarFrecuencia(mapas);
        else if(opcion == 2)menuBuscarRelevancia(mapas);
        else if(opcion == 3)menuBuscarPalabra(mapas);
        else if(opcion == 4)menuMostrarContexto(mapas);
        else if(opcion == 0)break;
        else printf("Opción seleccionada no valida\n");
    }
}

/*void calcularRelevancia(Pair *data,Libro *libro, int numDoc){
    Palabra *aux = data->value;
    aux->relevancia = (aux->cont/libro->cantPalabras) * log(numDoc/countList(aux->posiciones));
}*/

Libro *cargarLibro(char *titulo, char* id, TreeMap *mapaPalabra){
    Libro *libro = (Libro*) malloc(sizeof(Libro));

    strcpy(libro->titulo, titulo);
    strcpy(libro->id, id); /*libro->id = id;*/
    libro->palabras = mapaPalabra;
}

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


void crearListaBloqueo(TreeMap *map){ //MAS MAPAAAAAS, VAMOOOOO
    insertTreeMap(map,"the",NULL); insertTreeMap(map,"and",NULL); insertTreeMap(map,"for",NULL);
    insertTreeMap(map,"is",NULL); insertTreeMap(map,"was",NULL); insertTreeMap(map,"were",NULL);
    insertTreeMap(map,"his",NULL); insertTreeMap(map,"she",NULL); insertTreeMap(map,"him",NULL); 
    insertTreeMap(map,"her",NULL); insertTreeMap(map,"they",NULL); insertTreeMap(map,"but",NULL); 
    insertTreeMap(map,"at",NULL); insertTreeMap(map,"a",NULL); insertTreeMap(map,"to",NULL); 
    insertTreeMap(map,"that",NULL); insertTreeMap(map,"you",NULL); insertTreeMap(map,"with",NULL);
    insertTreeMap(map,"in",NULL);
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
            palabra = aMinus(palabra);
            quitarSimbolos(palabra);
            if (searchTreeMap(bloqueo, palabra) || strlen(palabra) < 2)
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

    printf("Introduzca la palabra que se quiere buscar: ");
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
    int *max = (int *) malloc (sizeof(int));
    *max = 0;
    int count;
    while(aux){
        palabra = aux->value;
        count = countList(palabra->posiciones);
        int *key = (int *) malloc (sizeof(int));
        *key = count;
        if (count > *max) *max = count;

        insertTreeMap(palabrasFrecuentes, key, palabra);
        aux = nextTreeMap(libro->palabras);
    }

    aux = upperBound(palabrasFrecuentes, max);
    printf("Las palabras mas frecuentes son:\n");
    for(int i = 1; i <= 10; i++){
        if (!aux) break;
        palabra = aux->value;
        printf("%i. %s %i veces\n", i, palabra->palabra, countList(palabra->posiciones));
        eraseTreeMap(palabrasFrecuentes, aux->key);
        aux = upperBound(palabrasFrecuentes, max);
    }
    esperarEnter();
}

void menuBuscarRelevancia(MapasGlobales *mapas)
{

}

void imprimirContexto(FILE *fp, long pos)
{
    long auxPos = pos - 75;
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
        size -= strlen(string)+1;
        for (int i=0; i<strlen(string); i++) // elimina newlines
        {
            if (string[i] == '\n') string[i] = ' ';
        }
        printf("%s ", string);
        free(string);
    }
    printf("...\n\n");
}

// funciona
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