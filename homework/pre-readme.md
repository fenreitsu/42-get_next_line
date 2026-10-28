*Este proyecto ha sido creado como parte del currículo de 42 por reiascan*

# get_next_line

## Descripción

---

## Conversiones soportadas

El  objetivo de este proyecto es replicar el comportamiento de printf, bajo los esppecificadores que se nos han planteado desde  la escuela de 42. No se incluyen los demas especificadores de la funcion `CAMBIAR`original, solo los de la lista siguiente. 

| Especificador | Argumento | Descripción |
| :---: | :--- | :--- |


---

## Descripción general de la librería y descripción de funciones

### 1. Núcleo del programa

* `CAMBIAR` – CAMBIAR

### 2. Funciones auxiliares



### 3. Estructura del proyecto

| Archivo | Contenido |
| :--- | :--- |


---

## Instrucciones

### Compilación

El proyecto utiliza un `Makefile` que compila todos los archivos fuente con `cc` y las flags `-Wall -Wextra -Werror`, y genera la librería estática `CAMBIAR.a` mediante `ar`.

1. **Compilar la librería:**

   ~~~bash
   make
   ~~~

   Compila todos los archivos `.c` listados en `SRC` y genera la librería estática `CAMBIAR.a`.

2. **Limpiar los archivos objeto:**

   ~~~bash
   make clean
   ~~~

   Elimina todos los archivos objeto (`.o`) generados durante la compilación.

3. **Limpiar los archivos objeto y `CAMBIAR.a`:**

   ~~~bash
   make fclean
   ~~~

   Elimina tanto los archivos objeto como la librería estática.

4. **Recompilar todo desde cero:**

   ~~~bash
   make re
   ~~~

   Ejecuta `fclean` y luego `all`, recompilando el proyecto completo.

### Integración y ejecución

Para usar `CAMBIAR`:

1. Incluye el header en tu archivo `*.c`:

   ~~~c
   #include "CAMBIAR" 
   ~~~

2. Compila enlazando la librería:

   ~~~bash
   cc -Wall -Wextra -Werror main.c CAMBIAR.a
   ~~~

   - `-Wall` para activar los warnings importantes.
   - `-Wextra` activa warnings adicionales.
   - `-Werror` convierte los warnings en errores.

   También puedes enlazarla por nombre, indicando al enlazador dónde buscar:

   ~~~bash
   cc -Wall -Wextra -Werror main.c -L. -CAMBIAR
   ~~~

   - `-L.` indica al enlazador que busque librerías en el directorio actual.
   - `-CAMBIAR` indica que enlace con `CAMBIAR.a` (el prefijo `lib` y la extensión `.a` se omiten).

   En ambos casos, la librería debe colocarse **después** de los archivos que la usan.

### Ejemplo de uso

~~~c

~~~

---

## Comportamiento y notas



---

## Recursos

### Referencias y documentación

* **PDF del subject de 42** – Contiene las reglas específicas del proyecto, los prototipos exactos, los requisitos de normativa y los criterios de evaluación.
* **https://pubs.opengroup.org/onlinepubs/7908799/xsh/fcntl.html** – [PDTE-DESCRIPCION]
* **https://man7.org/linux/man-pages/man2/open.2.html** – [PDTE-DESCRIPCION]
* **https://pasky.wordpress.com/2009/08/04/funciones-openclose-y-readwrite-en-c/** – [PDTE-DESCRIPCION]
* **https://www.ibm.com/docs/es/aix/7.2.0?topic=files-fcntlh-file** – [PDTE-DESCRIPCION]
* **El man (manual) de Linux** – [PDTE-DESCRIPCION]

### Herramientas de prueba

* **Tests propios** – Mis tests, aunque sencillos, fueron los que me ayudaron a plantearme la mayoría de problemas y/o a como abordar mis funciones.
* **https://pythontutor.com/c.html** – Herramienta de visualización paso a paso de código C. Se utilizó para identificar fallos, entender el flujo de ejecución y depurar el manejo de punteros y la recursión.
* **[francinette-image](https://github.com/WaRtr0/francinette-image)** –  Se ha utilizado esta herramienta de la comunidad de 42 para testear mi proyecto

### Sobre el uso de IA

* La IA se utilizó para crear y estructurar este archivo `README.md` de acuerdo con los requisitos indicados en el subject de este proyecto de 42.
