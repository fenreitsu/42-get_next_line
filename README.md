*Este proyecto ha sido creado como parte del currículo de 42 por reiascan*

# get_next_line

## Descripción

`get_next_line` es un proyecto individual del currículo de 42 cuyo objetivo es escribir una función que devuelva una línea leída desde un descriptor de archivo. Cada llamada devuelve una línea completa, incluyendo el carácter `\n` si existe, y conserva el resto del contenido en una variable estática para la siguiente llamada. La función devuelve `NULL` cuando no quedan datos o si ocurre un error.

Implementar `get_next_line` me ayudó a comprender el funcionamiento de los descriptores de archivo, la lectura por bloques con `read`, la gestión de memoria dinámica y el uso de variables estáticas para conservar estado entre llamadas.

Este README documenta únicamente la **parte obligatoria** del proyecto y los tres archivos que lo componen.

---

## Comportamiento soportado

| Caso | Comportamiento |
| :--- | :--- |
| Línea terminada en `\n` | Devuelve la línea incluyendo el `\n`. |
| Última línea sin `\n` | Devuelve el contenido restante; la siguiente llamada devuelve `NULL`. |
| Archivo vacío | Devuelve `NULL` en la primera llamada. |
| Línea vacía | Devuelve `"\n"`. |
| EOF | Devuelve `NULL`. |
| `fd` inválido | Devuelve `NULL`. |
| `BUFFER_SIZE <= 0` | Devuelve `NULL`. |
| `BUFFER_SIZE` = 1, 42 o 9999 | Debe producir el mismo resultado. |
| Memoria devuelta | Debe liberarse con `free` por quien llama. |
| Varios `fd` | No soportado en la parte obligatoria. |

---

## Descripción general de la librería y descripción de funciones

### 1. Núcleo del programa

* `get_next_line` – Valida el descriptor y `BUFFER_SIZE`, reserva el buffer de lectura, llama a `read_until_nl`, libera el buffer, extrae la línea y actualiza el resto. Utiliza una variable estática local para conservar el contenido entre llamadas. Si no hay línea o hay un error, devuelve `NULL`.
* `read_until_nl` – Lee del descriptor por bloques hasta encontrar un `\n` o llegar a EOF. Concatena cada bloque al contenido anterior mediante `ft_strjoin_free`. Si `read` falla, libera el contenido y devuelve `NULL`.
* `extract_line` – Calcula la longitud de la línea hasta el `\n` incluido, o hasta el final si no hay salto, reserva memoria y copia el resultado. No libera el resto.
* `rest_trimmer` – Busca el `\n` en el resto, duplica lo que queda después de él y libera la cadena anterior. Si no hay `\n` o no queda contenido, libera el resto y devuelve `NULL`.
* `get_next_line.h` – Header único de la librería: define `BUFFER_SIZE` con valor por defecto `42`, incluye las cabeceras necesarias y declara los prototipos.

### 2. Funciones auxiliares

* `ft_strlen` – Devuelve la longitud de una cadena.
* `ft_strdup` – Reserva memoria y duplica una cadena.
* `ft_strchr` – Busca un carácter en una cadena y devuelve un puntero a su primera aparición, o `NULL` si no existe.
* `ft_strjoin` – Reserva memoria y concatena dos cadenas.
* `ft_strjoin_free` – Concatena dos cadenas y libera la primera. Si la primera es `NULL`, duplica la segunda; también contempla el caso de que la segunda sea `NULL`.

### 3. Estructura del proyecto

| Archivo | Contenido |
| :--- | :--- |
| `get_next_line.c` | Lógica principal: lectura, extracción de la línea y gestión del resto. |
| `get_next_line_utils.c` | Funciones auxiliares de cadenas y concatenación con liberación. |
| `get_next_line.h` | Prototipos, `BUFFER_SIZE` y cabeceras necesarias. |

---

## Instrucciones

### Compilación

El proyecto se compila directamente con `cc` junto a un `main` de prueba externo. Ese `main` no forma parte de los tres archivos del proyecto.

~~~bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl
~~~

Para comprobar que la función funciona con distintos tamaños de buffer, cambia el valor de `BUFFER_SIZE`:

~~~bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=1 get_next_line.c get_next_line_utils.c main.c -o gnl
~~~

~~~bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=9999 get_next_line.c get_next_line_utils.c main.c -o gnl
~~~

Si no se define `BUFFER_SIZE`, el header utiliza el valor por defecto `42`.

Si solo quieres compilar los archivos del proyecto sin enlazarlos con un `main`, puedes generar los archivos objeto:

~~~bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 -c get_next_line.c get_next_line_utils.c
~~~

### Integración y ejecución

1. Incluye el header en tu archivo `*.c`:

   ~~~c
   #include "get_next_line.h"
   ~~~

2. Abre el archivo con `open` desde tu programa de prueba, llama a `get_next_line` en bucle, libera cada línea con `free` y cierra el descriptor con `close`.

3. Compila los archivos del proyecto junto a tu `main` con las flags `-Wall -Wextra -Werror`.

### Ejemplo de uso

~~~c
#include "get_next_line.h"

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("archivo.txt", O_RDONLY);
	if (fd < 0)
		return (1);
	line = get_next_line(fd);
	while (line != NULL)
	{
		write(1, line, ft_strlen(line));
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
~~~

Este ejemplo es solo un `main` externo de prueba. La librería no abre ni cierra archivos: recibe un descriptor ya abierto y se limita a leer de él.

---

## Comportamiento y notas

* `get_next_line` devuelve una línea por llamada. La línea incluye el `\n` salvo que sea la última y el archivo no termine en salto de línea.
* Cuando se llega a EOF o se produce un error, la función devuelve `NULL`. Las llamadas posteriores a EOF también devuelven `NULL`.
* La línea devuelta pertenece a quien llama y debe liberarse con `free`.
* El resto interno se conserva en una variable estática y se libera cuando se consume o cuando ya no queda contenido.
* `BUFFER_SIZE` debe ser mayor que cero. Si no se define, el header utiliza `42`. La función debe funcionar con valores como `1`, `42` y `9999`.
* Solo se utilizan `read`, `malloc` y `free`. No se usa `libft`, `lseek` ni variables globales.
* La función no cierra el descriptor de archivo; eso es responsabilidad de quien llama.
* La parte obligatoria trabaja con un único descriptor. No se implementa el bonus multi-`fd`, por lo que no se garantiza el uso simultáneo de varios descriptores.
* La implementación contempla archivos vacíos, líneas vacías, archivos sin salto de línea final y líneas de longitud arbitraria.

---

## Recursos

### Referencias y documentación

* **PDF del subject de 42** – Contiene las reglas específicas del proyecto, los prototipos exactos, los requisitos de normativa y los criterios de evaluación.
* **https://pubs.opengroup.org/onlinepubs/7908799/xsh/fcntl.html** – Especificación y entendimiento del header `<fcntl.h>`: control de archivos abiertos, descriptores, flags y bloqueos. Referencia para entender el contexto de los descriptores que después se leen con `read`.
* **https://man7.org/linux/man-pages/man2/open.2.html** – Manual de `open(2)` en Linux: cómo abrir un archivo y obtener un descriptor, flags de apertura, valores de retorno y errores. Útil para preparar el `fd` que recibe `get_next_line`.
* **https://pasky.wordpress.com/2009/08/04/funciones-openclose-y-readwrite-en-c/** – Entrada en español que introduce `open`, `close`, `read` y `write` con ejemplos. Sirve para practicar la lectura secuencial por descriptores.
* **https://www.ibm.com/docs/es/aix/7.2.0?topic=files-fcntlh-file** – Referencia de IBM AIX para `<fcntl.h>`: valores de `Oflag` de `open` y comandos de `fcntl`. Documentación complementaria sobre flags y control de archivos.
* **El man (manual) de Linux** – Consulta local de `man 2 read`, `man 2 open`, `man 2 close`, `man 3 malloc` y `man 3 free`: prototipos, valores de retorno, errores y gestión de memoria.

### Herramientas de prueba

* **Tests propios** – Mis tests, aunque sencillos, fueron los que me ayudaron a plantearme la mayoría de problemas y/o a cómo abordar mis funciones.
* **https://pythontutor.com/c.html** – Herramienta de visualización paso a paso de código C. Se utilizó para identificar fallos, entender el flujo de ejecución y depurar el manejo de punteros y la recursión.
* **[francinette-image](https://github.com/WaRtr0/francinette-image)** – Se ha utilizado esta herramienta de la comunidad de 42 para testear mi proyecto.

### Sobre el uso de IA

* La IA se utilizó para crear, formatear y estructurar este archivo `README.md` de acuerdo con los requisitos indicados en el subject de este proyecto de 42.
