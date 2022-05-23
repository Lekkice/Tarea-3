Para compilar la tarea se debe ingresar los siguientes comandos en la terminal de bash: 
- gcc *.c -o main.exe (se puede cambiar el main.exe a tarea3.exe si gusta)
- y luego ./main.exe  (usar ./tarea3.exe si uso la otra forma)

    1. Cargar documentos. El usuario coloca los nombres de una lista de archivos txt separados por espacio y la aplicación los carga.

    2. Mostrar documentos ordenados. La aplicación muestra los id y títulos de los libros en orden alfabético. Además se muestra la cantidad de palabras y caracteres que tiene cada uno de ellos.

    3. Buscar un libro por título. El usuario coloca algunas palabras separadas por espacio y la aplicación muestra los títulos de libros que contienen todas las palabras.

    4. Palabras con mayor frecuencia. El usuario ingresa el id de un libro y la aplicación muestra las 10 palabras que se repiten con mayor frecuencia (indicando la cantidad de veces que aparece cada una de ellas).
    (La frecuencia se calcula como la cantidad de veces que aparece una palabra dividida por el total de palabras en el libro.)

    5. Palabras más relevantes.  El usuario ingresa el título de un libro y la aplicación muestra las 10 palabras más relevantes de este. Para calcular la relevancia de una palabra p en un documento d se debe aplicar la siguiente fórmula (tf-idf):
    ocurrencias de p en el documento dcantidad de palabras en dlog(número de documentosdocumentos que tienen la palabra p)

    6. Buscar por palabra. El usuario ingresa una palabra y la aplicación muestra los libros (id y título) que tienen la palabra en su contenido. Los libros deben ser ordenados por la relevancia de la palabra buscada.
    Por ejemplo, si busco “Jesús”, la biblia debería aparecer en primer lugar. Si busco “mancha”, el Quijote debería salir en primer lugar.

    7. Mostrar palabra en su contexto dentro del libro. El usuario ingresa el título de un libro y el de una palabra a buscar. 
    La aplicación muestra las distintas apariciones de la palabra en el contexto del documento, es decir, para cada aparición, se muestran algunas palabras hacia atrás y hacia adelante de la palabra buscada
    (por ejemplo, la línea completa en la que aparece la palabra, o 5 palabras hacia atrás y 5 hacia adelante).

Opciones que funcionan correctamente y las que no:
-todas las opciones funcionan exceptuando la funcion menuBuscarFrecuencia que en algunos casos se salta algunas palabras palabras 
-esto probablemente debido a los simbolos que hay en las palabras. por ejemplo en libro 11.txt la palabra alice retorna 397 cuando
-deberia retornar 403 en su frecuencia. La funcion menuBuscarRelevancia no fue testeada por mucho tiempo por lo que es propensa a tener fallos.

Aspectos positivos y a mejorar por cada uno de los integrantes:
-Fernando Arancibia: Existen ocasiones en las que las funciones que programaba no era muy eficientes.

-Alejandro Malvacias: Hay veces en las que sobrecomplico las cosas innecesariamente tratando de programar las funciones.

-Elias Bernales: No se explicar las cosas que hago, solo se que hacen sentido y que funcionan.

Puntos premio/castigo:
-Fernando Arancibia: 0/0
-Alejandro Malvacias: 0/0
-Elias Bernales: 0/0
