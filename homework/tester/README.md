# Testers automáticos

Un archivo autocontenido por función. Cada tester incluye tu `../<funcion>.c`
(renombrando su `main`), ejecuta casos normales, bordes y de error, imprime
`PASS`/`FAIL` y termina con código distinto de 0 si algo falla.

No hay `Makefile` ni runner común: compilas y ejecutas el tester que te interese.

## Uso

Siempre **desde esta carpeta** (`homework/tester/`). Los testers que no usan
buffer se compilan así:

```sh
cc -Wall -Wextra -fsanitize=address,undefined tester_ft_read_file.c -o tester_ft_read_file
./tester_ft_read_file
```

Los que usan `BUFFER_SIZE` **obligatoriamente** llevan `-D BUFFER_SIZE=N`:

```sh
cc -Wall -Wextra -fsanitize=address,undefined \
   -D BUFFER_SIZE=42 tester_ft_count_bytes.c -o tester_ft_count_bytes
./tester_ft_count_bytes
```

El `#include "../<funcion>.c"` asume que tu archivo se llama igual que la
función (`ft_read_file.c`, `ft_count_bytes.c`, ...). Si lo tienes en otro sitio
o con otro nombre, cambia la ruta del include dentro del tester.

## Mapa ejercicio → tester

| Ejercicio | Función | Tester | ¿Necesita `-D`? |
|---|---|---|---|
| 0.1 | `ft_read_file` | `tester_ft_read_file.c` | No |
| 0.2 | `ft_count_bytes` | `tester_ft_count_bytes.c` | `BUFFER_SIZE` |
| 1.1 | `ft_read_until_char` | `tester_ft_read_until_char.c` | No |
| 1.2 | `ft_read_chunks` | `tester_ft_read_chunks.c` | No |
| 2.1 | `ft_strjoin_free` | `tester_ft_strjoin_free.c` | No |
| 2.2 | `ft_strndup` | `tester_ft_strndup.c` | No |
| 2.3 | `ft_realloc` | `tester_ft_realloc.c` | No |
| 3.1 | `ft_read_line_fd` | `tester_ft_read_line_fd.c` | `BUFFER_SIZE` |
| 3.2 | `ft_read_all_fd` | `tester_ft_read_all_fd.c` | `BUFFER_SIZE` |
| 4.1 / 5.1 | `ft_read_line_static` | `tester_ft_read_line_static.c` | `BUFFER_SIZE` |
| 5.2 | `ft_test_gnl` (main) | `tester_ft_read_line_static.c` | `BUFFER_SIZE` |
| 6.1 / 8.1 | `ft_gnl` + estado por fd | `tester_ft_gnl.c` | `BUFFER_SIZE` (+ `MAX_FD`) |
| 6.2 | test multi-fd (main) | `tester_ft_gnl.c` | `BUFFER_SIZE` (+ `MAX_FD`) |
| 7.1 | stress (main) | `tester_ft_gnl.c` | `BUFFER_SIZE` (+ `MAX_FD`) |

## Qué prueba cada tester

Cada subsección lista los casos en el orden en que se ejecutan. El texto entre
comillas es el nombre que imprime el tester; la descripción dice qué se compara
o qué se espera. Los casos de `BUFFER_SIZE <= 0` solo existen al compilar con
ese valor.

### `tester_ft_read_file.c` (8 casos)

Firma: `void ft_read_file(const char *path, int buf_size)`. El tester redirige
stdout a un archivo temporal y compara los bytes exactos que imprime tu función.

| Caso | Qué comprueba |
|---|---|
| `normal con buf_size 1` | Imprime el archivo completo (varias líneas, sin `\n` final) leyendo de byte en byte. |
| `normal con buf_size 42` | El mismo archivo se imprime completo con un buffer de 42 bytes. |
| `normal con buf_size 9999` | El mismo archivo se imprime completo con un buffer mayor que el archivo. |
| `archivo vacio no imprime nada` | Un archivo de 0 bytes no produce salida. |
| `buf_size 0 no imprime nada` | Con `buf_size == 0` no imprime nada y no crashea. |
| `buf_size -1 no imprime nada` | Con `buf_size == -1` no imprime nada y no crashea. |
| `archivo inexistente` | Si `open` falla, no imprime nada por stdout y no hay leak. |
| `path NULL` | Con `path == NULL` no crashea y no imprime nada por stdout. |

### `tester_ft_count_bytes.c` (6 casos + 2 con `BUFFER_SIZE <= 0`)

Firma: `size_t ft_count_bytes(int fd)`. Lee hasta EOF, cuenta los bytes y deja
el fd consumido. El tester compara el valor devuelto y, en algunos casos, hace
un `read` después para comprobar la posición del fd.

| Caso | Qué comprueba |
|---|---|
| `archivo de 36 bytes` | Devuelve 36 para el archivo de prueba de 36 bytes. |
| `el fd queda consumido al terminar` | Después de contar, `read(fd, buf, 8)` devuelve 0: no quedan bytes. |
| `archivo vacio` | Devuelve 0. |
| `fd -1` | Devuelve 0 sin crashear. |
| `fd cerrado` | Devuelve 0 para el fd 999 cerrado/no abierto, sin leaks. |
| `pipe con 10 bytes` | Escribe 10 bytes en un pipe, cierra el extremo de escritura y espera 10. |
| `BUFFER_SIZE <= 0 devuelve 0` | No debe reservar ni leer; devuelve 0. |
| `BUFFER_SIZE <= 0 no consume el fd` | Después de la llamada, `read(fd, &c, 1)` sigue leyendo el primer byte. |

### `tester_ft_read_until_char.c` (8 casos)

Firma: `char *ft_read_until_char(int fd, char stop)`. Devuelve un string
malloc'd con lo leído, incluyendo `stop` si aparece; devuelve `NULL` si no hay
nada o hay error. Consume el fd.

| Caso | Qué comprueba |
|---|---|
| `para en el primer salto de linea` | Sobre `"abc\ndef"` con `'\n'`, devuelve `"abc\n"`. |
| `segunda llamada: resto sin salto` | La siguiente llamada sobre el mismo fd devuelve `"def"` (sin `\n` final). |
| `tercera llamada: EOF devuelve NULL` | Al no quedar bytes, devuelve `NULL`. |
| `stop en el primer caracter` | Sobre `"\nabc"` con `'\n'`, devuelve `"\n"`. |
| `sin stop devuelve todo` | Si el carácter no aparece, devuelve el resto completo (`"abcdef"`). |
| `archivo vacio devuelve NULL` | Un archivo de 0 bytes devuelve `NULL`. |
| `fd -1 devuelve NULL` | Un fd inválido devuelve `NULL` sin crashear. |
| `fd sin permiso de lectura devuelve NULL` | Un fd abierto solo para escritura devuelve `NULL`, sin leaks. |

### `tester_ft_read_chunks.c` (7 casos)

Firma: `void ft_read_chunks(int fd, size_t chunk)`. Imprime cada bloque leído
como `[datos]\n`; al llegar a EOF imprime `DONE\n`, y ante un error imprime
`ERROR\n`. El tester captura stdout y compara el texto exacto.

| Caso | Qué comprueba |
|---|---|
| `chunk 2 sobre 5 bytes` | Sobre `"abcde"` imprime `[ab]\n[cd]\n[e]\nDONE\n`. |
| `chunk 1 sobre 5 bytes` | Con bloques de 1 byte imprime un bloque por carácter y luego `DONE`. |
| `chunk mayor que el archivo` | Con `chunk = 10` imprime `[abcde]\nDONE\n`. |
| `chunk 0 imprime ERROR` | Un tamaño de bloque 0 imprime `ERROR\n` y no `DONE`. |
| `archivo vacio solo imprime DONE` | Un archivo vacío imprime únicamente `DONE\n`. |
| `fd sin permiso de lectura imprime ERROR` | Un fd abierto solo para escritura imprime `ERROR\n`. |
| `fd -1 imprime ERROR` | Un fd inválido imprime `ERROR\n`. |

### `tester_ft_strjoin_free.c` (7 casos)

Firma: `char *ft_strjoin_free(char *s1, char const *s2)`. Concatena `s1` y `s2` en
un string nuevo y libera `s1`; si `s2` es `NULL`, devuelve `s1` sin liberarlo.
ASan detecta si olvidas liberar `s1` o si la liberas dos veces.

| Caso | Qué comprueba |
|---|---|
| `concatena dos strings` | `"abc"` + `"def"` devuelve `"abcdef"` y libera el `s1` original. |
| `s1 NULL se comporta como strdup` | `NULL` + `"def"` devuelve una copia nueva de `"def"`. |
| `s2 NULL devuelve s1 sin liberarlo` | `"abc"` + `NULL` devuelve el mismo `s1`; el llamador sigue siendo dueño y lo libera. |
| `s1 y s2 NULL devuelve NULL` | Ambos `NULL` devuelven `NULL` sin crashear. |
| `s1 vacio` | `""` + `"def"` devuelve `"def"` y libera el `s1` vacío. |
| `s2 vacio` | `"abc"` + `""` devuelve `"abc"`. |
| `strings largos de 100 + 100` | Concatena 100 `'a'` y 100 `'b'` en un string de 200 caracteres. |

### `tester_ft_strndup.c` (7 casos)

Firma: `char *ft_strndup(const char *s, size_t n)`. Reserva y copia como máximo
`n` caracteres de `s`, terminando en `'\0'`. ASan comprueba que no lees fuera
del string y que no hay leaks.

| Caso | Qué comprueba |
|---|---|
| `n menor que la longitud` | `"abcdef"` con `n = 3` devuelve `"abc"`. |
| `n igual a la longitud` | `"abcdef"` con `n = 6` devuelve `"abcdef"`. |
| `n mayor que la longitud` | `"abcdef"` con `n = 10` devuelve `"abcdef"` sin leer de más. |
| `n 0 devuelve string vacio` | Con `n = 0` devuelve un string vacío reservado, no `NULL`. |
| `string vacio` | `""` con `n = 5` devuelve `""`. |
| `s NULL devuelve NULL` | Con `s == NULL` devuelve `NULL` sin crashear. |
| `duplica exactamente 4 caracteres` | `"hola"` con `n = 4` devuelve `"hola"`. |

### `tester_ft_realloc.c` (11 casos)

Firma: `void *ft_realloc(void *ptr, size_t old_size, size_t new_size)`. Debe
comportarse como `malloc` si `ptr` es `NULL`, conservar el prefijo común,
liberar el bloque viejo y devolver `NULL` si `new_size == 0`. ASan detecta el
bloque viejo sin liberar, dobles `free` y accesos fuera de rango.

| Caso | Qué comprueba |
|---|---|
| `ptr NULL equivale a malloc` | `ft_realloc(NULL, 0, 16)` devuelve un puntero válido. |
| `ptr NULL y new_size 0 devuelve NULL` | `ft_realloc(NULL, 0, 0)` devuelve `NULL`. |
| `expandir devuelve puntero valido` | De 5 a 20 bytes devuelve un puntero no nulo. |
| `expandir conserva el contenido` | Los primeros 5 bytes siguen siendo `"aaaaa"`. |
| `truncar devuelve puntero valido` | De 10 a 4 bytes devuelve un puntero no nulo. |
| `truncar conserva el prefijo` | Los primeros 4 bytes siguen siendo `"bbbb"`. |
| `mismo tamano devuelve puntero valido` | De 8 a 8 bytes devuelve un puntero no nulo. |
| `mismo tamano conserva el contenido` | Los 8 bytes siguen siendo `"cccccccc"`. |
| `new_size 0 libera y devuelve NULL` | Con `new_size = 0` libera el bloque antiguo y devuelve `NULL`. |
| `expansion grande devuelve puntero valido` | De 1 a 4096 bytes devuelve un puntero no nulo. |
| `expansion grande conserva el contenido` | El primer byte sigue siendo `'e'`. |

### `tester_ft_read_line_fd.c` (7 casos + 1 con `BUFFER_SIZE <= 0`)

Firma: `char *ft_read_line_fd(int fd)`. Devuelve una línea malloc'd desde la
posición actual del fd, incluyendo `'\n'` si lo hay, o `NULL` en EOF/error. No
cierra el fd. Esta versión no tiene estado `static`: el tester solo comprueba
la primera línea de cada fd; el encadenamiento se prueba en
`tester_ft_read_line_static.c`.

| Caso | Qué comprueba |
|---|---|
| `primera linea con salto` | Sobre `"line1\nline2\n"` devuelve `"line1\n"`. |
| `primera linea vacia` | Sobre `"\nabc"` devuelve `"\n"`. |
| `archivo sin salto final` | Sobre `"abc"` devuelve `"abc"`. |
| `archivo vacio devuelve NULL` | Un archivo de 0 bytes devuelve `NULL`. |
| `fd -1 devuelve NULL` | Un fd inválido devuelve `NULL` sin crashear. |
| `fd sin lectura devuelve NULL` | Un fd abierto solo para escritura devuelve `NULL`, sin leaks. |
| `la funcion no cierra el fd del llamador` | Después de leer, `fcntl(fd, F_GETFD)` confirma que el fd sigue abierto. |
| `BUFFER_SIZE <= 0 devuelve NULL` | Con un buffer no positivo devuelve `NULL`. |

### `tester_ft_read_all_fd.c` (5 casos + 1 con `BUFFER_SIZE <= 0`)

Firma: `char *ft_read_all_fd(int fd)`. Lee todo el fd y devuelve un string
malloc'd; devuelve `NULL` si el fd está vacío o hay error.

| Caso | Qué comprueba |
|---|---|
| `archivo de texto completo` | Devuelve exactamente `"primera linea\nsegunda linea\ntercera sin salto"`. |
| `archivo vacio devuelve NULL` | Un archivo de 0 bytes devuelve `NULL`. |
| `fd -1 devuelve NULL` | Un fd inválido devuelve `NULL` sin crashear. |
| `fd sin lectura devuelve NULL` | Un fd abierto solo para escritura devuelve `NULL`, sin leaks. |
| `archivo grande` | Lee un archivo lleno de `'a'` y comprueba longitud y contenido: 32 KB con `BUFFER_SIZE = 1`, 128 KB con `2..4095`, 1 MB con `4096` o más. |
| `BUFFER_SIZE <= 0 devuelve NULL` | Con un buffer no positivo devuelve `NULL`. |

### `tester_ft_read_line_static.c` (16 casos + 2 con `BUFFER_SIZE <= 0`)

Firma: `char *ft_read_line_static(int fd)`. Mantiene un `static char *resto` y
devuelve líneas consecutivas del mismo fd. El tester no alterna dos fds: esta
versión solo soporta uno a la vez; el estado por fd se prueba en
`tester_ft_gnl.c`.

| Caso | Qué comprueba |
|---|---|
| `primera llamada devuelve la primera linea` | Sobre `"a\nb\nc"` devuelve `"a\n"`. |
| `segunda llamada devuelve la segunda linea` | La siguiente llamada devuelve `"b\n"`. |
| `ultima linea sin salto final` | La siguiente devuelve `"c"`. |
| `tras el EOF devuelve NULL` | Al no quedar datos devuelve `NULL`. |
| `segunda llamada tras el EOF sigue devolviendo NULL` | Insistir después del EOF sigue dando `NULL`, sin leaks. |
| `linea vacia intermedia (1)` | Sobre `"\n\nx"` devuelve `"\n"`. |
| `linea vacia intermedia (2)` | La siguiente devuelve otro `"\n"`. |
| `linea final sin salto` | La siguiente devuelve `"x"`. |
| `EOF del archivo con lineas vacias` | Después de `"x"` devuelve `NULL`. |
| `archivo vacio devuelve NULL` | Un archivo de 0 bytes devuelve `NULL`. |
| `fd -1 devuelve NULL` | Un fd inválido devuelve `NULL` sin crashear. |
| `fd sin lectura devuelve NULL` | Un fd abierto solo para escritura devuelve `NULL`, sin leaks. |
| `tras los errores el estado sigue limpio` | Después de los fd inválidos, al reabrir `"a\nb\nc"` vuelve a devolver `"a\n"`. |
| `segunda linea tras los errores` | La siguiente devuelve `"b\n"`. |
| `tercera linea tras los errores` | La siguiente devuelve `"c"`. |
| `EOF final` | La siguiente devuelve `NULL`. |
| `BUFFER_SIZE <= 0 devuelve NULL` | Con un buffer no positivo la primera llamada devuelve `NULL`. |
| `BUFFER_SIZE <= 0 sigue devolviendo NULL` | La segunda llamada también devuelve `NULL`, sin leaks. |

### `tester_ft_gnl.c` (22 casos + 2 con `BUFFER_SIZE <= 0`)

Firma: `char *ft_gnl(int fd)`, más `void ft_gnl_cleanup(int fd)` y
`void ft_gnl_cleanup_all(void)`. Este tester no incluye tu `.c`: declara los
prototipos y se enlaza con tus archivos del nivel 6. `MAX_FD` debe coincidir con
el tamaño de tu array (por defecto 1024). Al final llama a
`ft_gnl_cleanup_all()` para que ASan no vea restos.

| Caso | Qué comprueba |
|---|---|
| `primera linea` | Sobre `"a\nb\nc"` devuelve `"a\n"`. |
| `segunda linea` | La siguiente llamada devuelve `"b\n"`. |
| `ultima linea sin salto` | La siguiente devuelve `"c"`. |
| `tras el EOF devuelve NULL` | Al no quedar datos devuelve `NULL`. |
| `segunda llamada tras el EOF` | Insistir después del EOF sigue dando `NULL`. |
| `ft_gnl no cierra el fd del llamador` | Después de leer, `fcntl(fd, F_GETFD)` confirma que el fd sigue abierto. |
| `archivo vacio devuelve NULL` | Un archivo de 0 bytes devuelve `NULL`. |
| `fd -1 devuelve NULL` | Un fd inválido devuelve `NULL` sin crashear. |
| `fd cerrado devuelve NULL` | El fd 999 cerrado/no abierto devuelve `NULL`, sin crear estado. |
| `fd >= MAX_FD devuelve NULL` | Un fd igual a `MAX_FD` queda fuera del array y devuelve `NULL`. |
| `multifd: linea 1 de fd1` | Primer fd: devuelve `"1a\n"`. |
| `multifd: linea 1 de fd2` | Segundo fd: devuelve `"2a\n"` sin perder el estado del primero. |
| `multifd: linea 2 de fd1` | Vuelve al primer fd y devuelve `"1b\n"`. |
| `multifd: linea 2 de fd2 (sin salto)` | Vuelve al segundo fd y devuelve `"2b"`. |
| `multifd: linea 3 de fd1` | Vuelve al primer fd y devuelve `"1c"`. |
| `multifd: EOF de fd2` | El segundo fd ya no tiene datos: `NULL`. |
| `multifd: EOF de fd1` | El primer fd ya no tiene datos: `NULL`. |
| `cleanup: primera linea antes de limpiar` | Abre `"1a\n1b\n1c"` y lee `"1a\n"` antes de cerrar y limpiar. |
| `cleanup: al reabrir el fd el estado esta limpio` | Tras `ft_gnl_cleanup(fd)` y reabrir el archivo, vuelve a devolver `"1a\n"`, no `"1b\n"`. |
| `cleanup: segunda linea tras reabrir` | La siguiente llamada devuelve `"1b\n"`. |
| `cleanup de un fd sin estado no crashea` | `ft_gnl_cleanup(999)` sobre un fd sin estado no crashea. |
| `tras cleanup_all el estado vuelve a empezar` | Después de `ft_gnl_cleanup_all()`, abrir de nuevo el archivo devuelve su primera línea. |
| `BUFFER_SIZE <= 0 devuelve NULL` | Con un buffer no positivo la primera llamada devuelve `NULL`. |
| `BUFFER_SIZE <= 0 sigue devolviendo NULL` | La segunda llamada también devuelve `NULL`, sin leaks. |

## Notas

- **Leaks y crashes:** `-fsanitize=address,undefined` detecta fugas, dobles
  `free` y accesos fuera de rango. Aunque el resumen diga `N/N OK`, si ASan
  reporta un leak el programa termina con error y el mensaje aparece después
  del resumen. Míralo siempre.
- **`BUFFER_SIZE` obligatorio:** el `.h` ya no define la macro; la única
  fuente es la línea de compilación. Los testers con buffer tienen

  ```c
  #ifndef BUFFER_SIZE
  # error "Compila con -D BUFFER_SIZE=N (ej. -D BUFFER_SIZE=42)"
  #endif
  ```

  así que si olvidas la flag no compilan, en vez de usar un valor fijo oculto.
  El tester imprime el valor con el que se compiló. Prueba al menos:

  ```sh
  -D BUFFER_SIZE=1
  -D BUFFER_SIZE=42
  -D BUFFER_SIZE=9999
  -D BUFFER_SIZE=0
  -D BUFFER_SIZE=-1
  ```

  Tu `.c` no debe redefinir la macro: los testers guardan el valor antes de
  incluir tu archivo y fallan con un mensaje claro si cambia
  (`_Static_assert`).

- **Comportamiento con `BUFFER_SIZE <= 0`:** los testers esperan una política
  concreta para que cualquier `-D` sea ejecutable:

  | Función | Resultado esperado |
  |---|---|
  | `ft_count_bytes` | `0`, sin consumir el fd |
  | `ft_read_line_fd` | `NULL` |
  | `ft_read_all_fd` | `NULL` |
  | `ft_read_line_static` | `NULL` |
  | `ft_gnl` | `NULL` |

  Si tu implementación decide otra cosa, cámbiala o ajusta el tester, pero
  documenta la decisión. Para que `-D BUFFER_SIZE=-1` compile, evita
  `char buf[BUFFER_SIZE]` (array de tamaño negativo): usa `malloc` o protege
  la declaración con `#if BUFFER_SIZE > 0`.

  `tester_ft_read_all_fd.c` reduce el archivo grande cuando el buffer es
  pequeño (32 KB con `1`, 128 KB por debajo de `4096`) para que una
  implementación O(n²) no tarde horas; con buffers grandes prueba 1 MB.
- **`MAX_FD`:** `tester_ft_gnl.c` no incluye tu código; declara los prototipos y
  se enlaza con tus `.c` del nivel 6 (los que no tengan `main`). La flag
  `-D BUFFER_SIZE` debe aplicarse a todos los `.c` enlazados:

  ```sh
  cc -Wall -Wextra -fsanitize=address,undefined \
     -D BUFFER_SIZE=42 -D MAX_FD=1024 \
     tester_ft_gnl.c ../gnl.c ../gnl_state.c ../gnl_cleanup.c \
     -o tester_ft_gnl
  ./tester_ft_gnl
  ```

  Si tu `MAX_FD` no es 1024, compila con `-D MAX_FD=tu_valor` para que el
  tester pruebe el límite correcto.
- **Ficheros temporales:** los testers crean `tester/tmp/` con los archivos de
  prueba. Se regeneran en cada ejecución.
- **Norminette:** los testers no están pensados para pasar norminette; son
  herramientas de verificación, no entregables.
