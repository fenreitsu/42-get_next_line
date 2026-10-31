/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_read_line_fd.c                                                 */
/*                                                                            */
/*   Compilar desde la carpeta tester/ (BUFFER_SIZE es obligatorio):          */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        -D BUFFER_SIZE=42 tester_ft_read_line_fd.c -o tester_ft_read_line_fd*/
/*                                                                            */
/*   Repetir con distintos buffers:                                           */
/*     cc ... -D BUFFER_SIZE=1  tester_ft_read_line_fd.c ...                  */
/*     cc ... -D BUFFER_SIZE=42 tester_ft_read_line_fd.c ...                  */
/*     cc ... -D BUFFER_SIZE=0  tester_ft_read_line_fd.c ...                  */
/*     cc ... -D BUFFER_SIZE=-1 tester_ft_read_line_fd.c ...                  */
/*                                                                            */
/*   Si tu funcion vive en otro archivo, cambia la ruta del include.          */
/*   ASan detecta fugas, dobles free y desbordes.                             */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# error "Compila con -D BUFFER_SIZE=N (ej. -D BUFFER_SIZE=42)"
#endif

enum	e_tester_buffer { tester_buffer_size = BUFFER_SIZE };

#define main	student_main_ft_read_line_fd
#include "../ft_read_line_fd.c"
#undef main

_Static_assert(BUFFER_SIZE == tester_buffer_size,
	"BUFFER_SIZE cambio al incluir tu .c: no la definas ahi");

static int	g_pass;
static int	g_fail;

static void	t_report(int ok, const char *name, const char *detail)
{
	if (ok)
	{
		g_pass++;
		printf("PASS  %s\n", name);
	}
	else
	{
		g_fail++;
		printf("FAIL  %s", name);
		if (detail && detail[0])
			printf("  [%s]", detail);
		printf("\n");
	}
}

static void	t_check_str(const char *got, const char *exp, const char *name)
{
	char	detail[320];

	if (!got || !exp)
		snprintf(detail, sizeof(detail), "esperado %s, obtenido %s",
			exp ? exp : "NULL", got ? got : "NULL");
	else
		snprintf(detail, sizeof(detail), "esperado \"%s\", obtenido \"%s\"",
			exp, got);
	t_report(got == exp || (got && exp && strcmp(got, exp) == 0),
		name, detail);
}

static int	t_mkfile(const char *path, const char *data, size_t len)
{
	int		fd;
	ssize_t	n;

	fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd < 0)
		return (-1);
	n = write(fd, data, len);
	close(fd);
	return (n == (ssize_t)len ? 0 : -1);
}

static void	t_case(const char *path, const char *expected, const char *name)
{
	int		fd;
	char	*line;

	fd = open(path, O_RDONLY);
	line = ft_read_line_fd(fd);
	t_check_str(line, expected, name);
	free(line);
	if (fd >= 0)
		close(fd);
}

#if BUFFER_SIZE <= 0

int	main(void)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	mkdir("tmp", 0755);
	if (t_mkfile("tmp/lines.txt", "line1\nline2\n", 12) < 0)
	{
		printf("ERROR: no se pudo crear el archivo de prueba\n");
		return (1);
	}
	printf("BUFFER_SIZE = %d\n\n", BUFFER_SIZE);
	t_case("tmp/lines.txt", NULL, "BUFFER_SIZE <= 0 devuelve NULL");
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}

#else

int	main(void)
{
	char	*line;
	int		fd;

	setvbuf(stdout, NULL, _IONBF, 0);

	mkdir("tmp", 0755);
	if (t_mkfile("tmp/lines.txt", "line1\nline2\n", 12) < 0
		|| t_mkfile("tmp/emptyline.txt", "\nabc", 4) < 0
		|| t_mkfile("tmp/nonl.txt", "abc", 3) < 0
		|| t_mkfile("tmp/empty.txt", "", 0) < 0
		|| t_mkfile("tmp/wo.txt", "datos", 5) < 0)
	{
		printf("ERROR: no se pudieron crear los archivos de prueba\n");
		return (1);
	}
	printf("BUFFER_SIZE = %d\n\n", BUFFER_SIZE);
	t_case("tmp/lines.txt", "line1\n", "primera linea con salto");
	t_case("tmp/emptyline.txt", "\n", "primera linea vacia");
	t_case("tmp/nonl.txt", "abc", "archivo sin salto final");
	t_case("tmp/empty.txt", NULL, "archivo vacio devuelve NULL");
	line = ft_read_line_fd(-1);
	t_check_str(line, NULL, "fd -1 devuelve NULL");
	free(line);
	fd = open("tmp/wo.txt", O_WRONLY);
	line = ft_read_line_fd(fd);
	t_check_str(line, NULL, "fd sin lectura devuelve NULL");
	free(line);
	close(fd);
	fd = open("tmp/lines.txt", O_RDONLY);
	line = ft_read_line_fd(fd);
	free(line);
	t_report(fcntl(fd, F_GETFD) != -1,
		"la funcion no cierra el fd del llamador", "el fd deberia seguir abierto");
	close(fd);
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}

#endif
