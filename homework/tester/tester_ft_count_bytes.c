/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_count_bytes.c                                                  */
/*                                                                            */
/*   Compilar desde la carpeta tester/ (BUFFER_SIZE es obligatorio):          */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        -D BUFFER_SIZE=42 tester_ft_count_bytes.c -o tester_ft_count_bytes  */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester_ft_count_bytes                                                */
/*                                                                            */
/*   Prueba tambien con -D BUFFER_SIZE=1, 9999, 0 y -1.                       */
/*                                                                            */
/*   Si tu funcion vive en otro archivo, cambia la ruta del include.          */
/*   ASan avisa al terminar de cualquier fuga o acceso fuera de rango.        */
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

#define main	student_main_ft_count_bytes
#include "../ft_count_bytes.c"
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

static void	t_check_int(long got, long exp, const char *name)
{
	char	detail[128];

	snprintf(detail, sizeof(detail), "esperado %ld, obtenido %ld", exp, got);
	t_report(got == exp, name, detail);
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

#if BUFFER_SIZE <= 0

int	main(void)
{
	const char	*data = "0123456789abcdefghijklmnopqrstuvwxyz";
	char		c;
	int			fd;

	setvbuf(stdout, NULL, _IONBF, 0);

	mkdir("tmp", 0755);
	if (t_mkfile("tmp/data.txt", data, strlen(data)) < 0)
	{
		printf("ERROR: no se pudo crear el archivo de prueba\n");
		return (1);
	}
	printf("BUFFER_SIZE = %d\n\n", BUFFER_SIZE);
	fd = open("tmp/data.txt", O_RDONLY);
	t_check_int((long)ft_count_bytes(fd), 0,
		"BUFFER_SIZE <= 0 devuelve 0");
	t_report(read(fd, &c, 1) == 1, "BUFFER_SIZE <= 0 no consume el fd",
		"el fd deberia seguir al principio");
	close(fd);
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}

#else

int	main(void)
{
	const char	*data = "0123456789abcdefghijklmnopqrstuvwxyz";
	char		buf[8];
	int			fd;
	int			p[2];

	setvbuf(stdout, NULL, _IONBF, 0);

	mkdir("tmp", 0755);
	if (t_mkfile("tmp/data.txt", data, strlen(data)) < 0
		|| t_mkfile("tmp/empty.txt", "", 0) < 0)
	{
		printf("ERROR: no se pudieron crear los archivos de prueba\n");
		return (1);
	}
	fd = open("tmp/data.txt", O_RDONLY);
	t_check_int((long)ft_count_bytes(fd), (long)strlen(data),
		"archivo de 36 bytes");
	t_check_int(read(fd, buf, 8), 0, "el fd queda consumido al terminar");
	close(fd);
	fd = open("tmp/empty.txt", O_RDONLY);
	t_check_int((long)ft_count_bytes(fd), 0, "archivo vacio");
	close(fd);
	t_check_int((long)ft_count_bytes(-1), 0, "fd -1");
	t_check_int((long)ft_count_bytes(999), 0, "fd cerrado");
	if (pipe(p) == 0)
	{
		write(p[1], "0123456789", 10);
		close(p[1]);
		t_check_int((long)ft_count_bytes(p[0]), 10, "pipe con 10 bytes");
		close(p[0]);
	}
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}

#endif
