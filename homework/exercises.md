# Practica para preparar get_next_line (GNL)

> **Base asumida:** Libft + ft_printf entregados y aprobados.
> **Objetivo:** obtener los conocimientos y practicar lo que  sabes hasta ahora para llegar al gnl.
> **Formato de trabajo:** nivel a nivel. Cada nivel incluye enunciado, firma exacta, restricciones, casos borde a cubrir y main de prueba sugerido.

---

## Índice

- [Nivel 0 — Calentamiento con `read`](#nivel-0--calentamiento-con-read)
- [Nivel 1 — Buffers y EOF](#nivel-1--buffers-y-eof)
- [Nivel 2 — Strings dinámicas básicas](#nivel-2--strings-dinámicas-básicas)
- [Nivel 3 — Combinar `read` + strings](#nivel-3--combinar-read--strings)
- [Nivel 4 — Estado persistente (clave para GNL)](#nivel-4--estado-persistente-clave-para-gnl)
- [Nivel 5 — Casos borde](#nivel-5--casos-borde)
- [Nivel 6 — Refactor con struct (bonus-style)](#nivel-6--refactor-con-struct-bonus-style)
- [Nivel 7 — Stress y leaks](#nivel-7--stress-y-leaks)
- [Nivel 8 — Norminette final](#nivel-8--norminette-final)
- [Checklist final antes de empezar GNL](#checklist-final-antes-de-empezar-gnl)
- [Notas finales](#notas-finales)

---

## Nivel 0 — Calentamiento con `read`

### Ejercicio 0.1 — `ft_read_file`

**Enunciado:** Abre el archivo cuya ruta se pasa como argumento, léelo con `read()` usando un buffer de tamaño `N` (macro `BUF_SIZE`) e imprime su contenido por stdout.

**Firma:**
```c
void	ft_read_file(const char *path, int buf_size);
```

**Restricciones:**
- Usa `open`, `read`, `close`.
- No uses `printf`/`write` para el contenido salvo para volcar el buffer leído (usa `write(1, buf, n)`).
- Cierra el fd en todos los caminos, incluido error.

**Casos borde:**
- `path == NULL` → no hacer nada.
- `open` devuelve `-1` → no hacer nada (o imprimir error por `write(2, ...)`).
- `read` devuelve `-1` → cerrar y salir.
- Archivo vacío → no imprimir nada.

**Main de prueba sugerido:**
```c
int	main(int ac, char **av)
{
	if (ac != 3)
		return (1);
	ft_read_file(av[1], atoi(av[2]));
	return (0);
}
```

Probar con `buf_size` = 1, 42, 9999 sobre un archivo de texto largo.

---

### Ejercicio 0.2 — `ft_count_bytes`

**Enunciado:** Dado un fd ya abierto, devuelve el número total de bytes que se pueden leer de él, consumiéndolo con `read()`.

**Firma:**
```c
size_t	ft_count_bytes(int fd);
```

**Restricciones:**
- No usar `stat`, `lseek`, ni `fstat`.
- Buffer local de tamaño `BUF_SIZE` (macro).
- Devolver `0` si `fd < 0` o si `read` devuelve `-1`.

**Casos borde:**
- fd inválido (`-1`, `999`).
- fd de un pipe (no seekable).
- Archivo vacío.

---

## Nivel 1 — Buffers y EOF

### Ejercicio 1.1 — `ft_read_until_char`

**Enunciado:** Lee un fd con `read()` y para cuando encuentra el carácter `stop`. Devuelve un string malloc'd con **todo lo leído, incluido el carácter `stop`**. Si llega a EOF sin encontrarlo, devuelve todo lo leído. Si no se leyó nada, devuelve `NULL`.

**Firma:**
```c
char	*ft_read_until_char(int fd, char stop);
```

**Restricciones:**
- Buffer de `BUF_SIZE` bytes.
- Debe funcionar con `BUF_SIZE = 1`.
- Sin leaks en ningún camino (incluido error de malloc).

**Casos borde:**
- `stop` es el primer carácter → devuelve string de 1 byte (`"\n"`).
- `stop` no aparece → devuelve todo el contenido.
- fd inválido → `NULL`.
- `read == -1` → liberar y devolver `NULL`.

---

### Ejercicio 1.2 — `ft_read_chunks`

**Enunciado:** Lee un fd de `chunk` bytes en `chunk` bytes e imprime por stdout cada bloque con el formato `[<contenido>]\n`. Al final imprime `DONE\n`.

**Firma:**
```c
void	ft_read_chunks(int fd, size_t chunk);
```

**Restricciones:**
- Usar `read` y `write` únicamente.
- Manejar `read == 0` (EOF) y `read == -1` (error → imprimir `ERROR\n` y salir).

**Casos borde:**
- `chunk == 0` → no leer, imprimir `ERROR\n`.
- `chunk >` tamaño del archivo.
- fd inválido.

---

## Nivel 2 — Strings dinámicas básicas

### Ejercicio 2.1 — `ft_strjoin_free`

**Enunciado:** Como `ft_strjoin` de Libft, pero **libera el primer string** (`s1`) tras concatenar. Si `s1 == NULL`, se comporta como `ft_strdup(s2)`. Si `s2 == NULL`, devuelve `s1` sin liberar (o `NULL` si ambos son `NULL`).

**Firma:**
```c
char	*ft_strjoin_free(char *s1, char const *s2);
```

**Restricciones:**
- Una sola reserva con `malloc`.
- Liberar `s1` **solo** si la reserva tuvo éxito (o si falla, liberar `s1` también y devolver `NULL`; elige una política y documéntala).
- Norminette: ≤25 líneas.

**Casos borde:**
- `s1 == NULL && s2 == NULL` → `NULL`.
- `s1 == NULL` → `strdup(s2)`.
- `s2 == NULL` → devuelve `s1` tal cual.
- Fallo de malloc → liberar `s1`, devolver `NULL`.

---

### Ejercicio 2.2 — `ft_strndup`

**Enunciado:** Duplica los primeros `n` caracteres de `s`. Si `s` es más corta que `n`, duplica hasta el `'\0'`. Reserva exactamente `len + 1` bytes.

**Firma:**
```c
char	*ft_strndup(const char *s, size_t n);
```

**Restricciones:**
- `s == NULL` → `NULL`.
- Reserva exacta (ni un byte de más).

**Casos borde:**
- `n == 0` → string vacío malloc'd (`""`).
- `s == ""` → string vacío malloc'd.

---

### Ejercicio 2.3 — `ft_realloc`

**Enunciado:** Reimplementa `realloc` casero: reserva `new_size` bytes, copia `min(old_size, new_size)` bytes del bloque antiguo, libera el antiguo y devuelve el nuevo.

**Firma:**
```c
void	*ft_realloc(void *ptr, size_t old_size, size_t new_size);
```

**Restricciones:**
- `ptr == NULL` → equivale a `malloc(new_size)`.
- `new_size == 0` → libera `ptr` y devuelve `NULL`.
- Fallo de malloc → **no** liberar `ptr` (política estándar de `realloc`), devolver `NULL`.

**Casos borde:**
- `new_size < old_size` (truncar).
- `new_size > old_size` (expandir).
- `ptr == NULL && new_size == 0` → `NULL`.

---

## Nivel 3 — Combinar `read` + strings

### Ejercicio 3.1 — `ft_read_line_fd`

**Enunciado:** Lee un fd byte a byte (o con buffer, tú decides) y devuelve **una sola línea** en un string malloc'd, **incluyendo el `'\n'`** si lo hay. Si se llega a EOF sin `'\n'`, devuelve lo leído. Si no hay nada que leer, devuelve `NULL`.

> ⚠️ **Sin `static` todavía.** Esta función consume el fd, así que solo sirve para leer la primera línea.

**Firma:**
```c
char	*ft_read_line_fd(int fd);
```

**Restricciones:**
- Buffer de `BUF_SIZE` bytes (macro).
- Debe funcionar con `BUF_SIZE = 1`.
- Cerrar correctamente en caso de error.
- Sin leaks.

**Casos borde:**
- fd inválido → `NULL`.
- Primera línea vacía (`"\n"` al inicio) → devuelve `"\n"`.
- Archivo sin `'\n'` final → devuelve todo.
- Archivo vacío → `NULL`.
- `read == -1` → `NULL` sin leaks.

---

### Ejercicio 3.2 — `ft_read_all_fd`

**Enunciado:** Lee todo el fd y devuelve un único string malloc'd con todo el contenido. Si el fd está vacío, devuelve `NULL`.

**Firma:**
```c
char	*ft_read_all_fd(int fd);
```

**Restricciones:**
- Usa `ft_strjoin_free` (nivel 2.1) o `ft_realloc` (nivel 2.3) para construir el resultado.
- Buffer de `BUF_SIZE` bytes.
- Sin leaks.

**Casos borde:**
- fd vacío → `NULL`.
- fd inválido → `NULL`.
- `read == -1` → `NULL` sin leaks.
- Archivo binario grande (probar con 1MB).

---

## Nivel 4 — Estado persistente (clave para GNL)

### Ejercicio 4.1 — `ft_read_line_static`

**Enunciado:** Igual que `ft_read_line_fd`, pero usando una **variable `static char *resto`** para que llamadas sucesivas sobre el **mismo fd** devuelvan líneas consecutivas. Cuando se acabe el fd, devuelve `NULL` y libera el `resto`.

**Firma:**
```c
char	*ft_read_line_static(int fd);
```

**Restricciones:**
- `static char *resto` (una sola variable static, sin lista todavía).
- Buffer de `BUF_SIZE` bytes.
- Debe funcionar con `BUF_SIZE = 1`.
- Sin leaks al terminar el fd y en errores.
- Solo soporta **un fd a la vez** (esto es intencional; el bonus vendrá en nivel 6).

**Casos borde:**
- Llamar dos veces seguidas → devuelve línea 1, luego línea 2.
- Última línea sin `'\n'` → devuelve la línea y la siguiente llamada devuelve `NULL`.
- fd inválido → `NULL` sin tocar `resto` (o liberándolo, decide y documenta).
- `read == -1` → `NULL` sin leaks.
- Alternar dos fds → comportamiento indefinido (documentado; se arregla en nivel 6).

**Main de prueba sugerido:**
```c
int	main(int ac, char **av)
{
	int		fd;
	char	*line;

	if (ac != 2)
		return (1);
	fd = open(av[1], O_RDONLY);
	if (fd < 0)
		return (1);
	while ((line = ft_read_line_static(fd)) != NULL)
	{
		write(1, line, strlen(line));
		free(line);
	}
	close(fd);
	return (0);
}
```

Probar con `BUF_SIZE = 1`, `42`, `9999` y un archivo de 1000 líneas.

---

## Nivel 5 — Casos borde

### Ejercicio 5.1 — Extender `ft_read_line_static`

**Enunciado:** Amplía el ejercicio 4.1 para cubrir **todos** estos casos:

1. `BUFFER_SIZE` configurable por macro (`-D BUFFER_SIZE=1`).
2. Archivo sin `'\n'` final.
3. `fd` inválido (`-1`, `999`).
4. `read == -1`.
5. Fallo de `malloc` (simúlalo con un wrapper que falle a la N-ésima llamada).
6. Llamar a la función tras EOF (debe devolver `NULL` sin leaks).
7. Llamar con `BUFFER_SIZE <= 0` (decide comportamiento: `NULL` y documentar).

**Firma:**
```c
char	*ft_read_line_static(int fd);
```

**Restricciones:**
- Sin leaks en **ningún** camino (verifica con valgrind/ASan).
- Norminette: ≤25 líneas por función.

---

### Ejercicio 5.2 — `ft_test_gnl`

**Enunciado:** Escribe un `main` que:

1. Abra **3 fds** distintos (3 archivos).
2. Llame alternando fds: `fd1`, `fd2`, `fd3`, `fd1`, `fd2`, ... hasta que todos devuelvan `NULL`.
3. Imprima cada línea con prefijo `[fdX] `.
4. Libere todo y cierre los fds.

> Con la versión de nivel 5 (una sola `static`) esto **fallará** en el orden de líneas. **Ese fallo es el objetivo pedagógico**: te muestra por qué GNL bonus necesita estado por fd. Documenta el fallo esperado y por qué ocurre.

**Firma:**
```c
int	main(void);
```

**Restricciones:**
- Sin leaks.
- Usa `open`, `read` implícito vía tu función, `close`, `free`.

---

## Nivel 6 — Refactor con struct (bonus-style)

### Ejercicio 6.1 — `t_gnl` con lista enlazada

**Enunciado:** Rehaz el nivel 4/5 usando:

```c
typedef struct s_gnl
{
	int				fd;
	char			*resto;
	struct s_gnl	*next;
}	t_gnl;
```

- Una función `t_gnl *get_state(t_gnl **head, int fd)` que busque/crea el nodo para `fd`.
- `char *ft_gnl(int fd)` que use `get_state` y devuelva la siguiente línea.
- `void ft_gnl_cleanup(t_gnl **head, int fd)` que libere el nodo de `fd`.
- `void ft_gnl_cleanup_all(t_gnl **head)` que libere todos los nodos.

**Firmas:**
```c
t_gnl	*get_state(t_gnl **head, int fd);
char	*ft_gnl(int fd);
void	ft_gnl_cleanup(t_gnl **head, int fd);
void	ft_gnl_cleanup_all(t_gnl **head);
```

**Restricciones:**
- Sin `static` global.
- Sin leaks al llamar `cleanup` o `cleanup_all`.
- Norminette: ≤5 funciones por archivo (reparte en varios `.c`).
- Debe funcionar con `BUFFER_SIZE = 1`.

**Casos borde:**
- Abrir/cerrar fds repetidamente.
- Llamar `cleanup` de un fd que ya no está en la lista (no debe crashear).
- `fd` inválido → `NULL` sin crear nodo.
- `malloc` fallido en `get_state` → `NULL` sin leaks.

---

### Ejercicio 6.2 — Test multi-fd correcto

**Enunciado:** Repite el `ft_test_gnl` del nivel 5.2 pero ahora con `ft_gnl`. Debe imprimir las líneas **en orden correcto por fd** (cada fd mantiene su propio estado).

**Firma:**
```c
int	main(void);
```

**Restricciones:**
- Sin leaks (valgrind/ASan).
- Cerrar todos los fds.
- Llamar `ft_gnl_cleanup_all` al final.

---

## Nivel 7 — Stress y leaks

### Ejercicio 7.1 — Batería de stress

**Enunciado:** Crea un `main` que ejecute, sobre tu `ft_gnl` del nivel 6, estas pruebas:

1. Archivo de 1000 líneas, `BUFFER_SIZE = 1`.
2. Archivo con una línea de 1MB.
3. Archivo vacío.
4. Archivo sin `'\n'` final.
5. 100 fds abiertos simultáneamente, alternando lecturas.
6. Abrir/cerrar el mismo fd 1000 veces.
7. Llamar `ft_gnl(-1)` y `ft_gnl(999)`.
8. Simular fallo de malloc en la llamada N (wrapper global).

**Firma:**
```c
int	main(void);
```

**Restricciones:**
- Compilar con `-fsanitize=address,undefined -g`.
- Pasar `valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes`.
- **Cero** leaks, **cero** errores de ASan/UBSan.

**Criterio de éxito:** todos los tests pasan sin leaks ni crashes.

---

## Nivel 8 — Norminette final

### Ejercicio 8.1 — Refactor completo

**Enunciado:** Refactoriza todo el nivel 6 para que pase `norminette` sin errores:

- ≤25 líneas por función.
- ≤5 funciones por archivo.
- ≤4 parámetros por función.
- Sin `for`, sin `do-while`, sin ternarios anidados.
- Sin líneas >80 columnas.
- Sin comentarios dentro de funciones.
- Cabecera 42 en cada `.c` y `.h`.
- Include guards en el `.h`.
- Sin variables globales.

**Firma:** la del nivel 6.

**Criterio de éxito:** `norminette` sin errores y los tests del nivel 7 siguen pasando.

---

## Checklist final antes de empezar GNL

Antes de abrir el subject de GNL, deberías poder marcar **todas** estas casillas:

- [ ] Sé leer un fd con `read` en cualquier `BUFFER_SIZE`, incluido `1`.
- [ ] Distingo `read == 0` (EOF) de `read == -1` (error).
- [ ] Sé construir strings dinámicas sin leaks (`strjoin_free`, `strndup`, `realloc`).
- [ ] Entiendo el patrón `static char *resto` y por qué es necesario.
- [ ] Sé por qué el bonus de GNL necesita estado **por fd** (lista/array).
- [ ] He pasado valgrind/ASan con **cero** leaks en todos los niveles.
- [ ] Mi código pasa `norminette` sin errores.
- [ ] He probado con `BUFFER_SIZE = 1`, `42`, `9999`.
- [ ] He probado archivo vacío, sin `'\n'` final, fd inválido, `read == -1`.
- [ ] He probado fallo de `malloc` simulado.
- [ ] Sé cerrar fds y liberar todo en `cleanup`.

Si marcas todo, estás listo para GNL. Si algo falla, vuelve al nivel correspondiente.

---

## Notas finales

- **Orden recomendado:** 0 → 1 → 2 → 3 → 4 → 5 → 6 → 7 → 8. No saltes niveles.
- **Cada nivel debe pasar valgrind/ASan antes de avanzar.**
- **Documenta tus decisiones** (política de liberación en `strjoin_free`, comportamiento con `BUFFER_SIZE <= 0`, etc.). GNL te exigirá criterio.
- **El fallo del nivel 5.2 es intencional:** te enseña por qué el bonus existe.
- **No copies código de GNL** durante estos ejercicios. La gracia es que llegues al proyecto con los dedos ya calientes.

¡Ánimo y a por ello!
