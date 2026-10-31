/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_read_until_char.c                                              */
/*                                                                            */
/*   Compilar desde la carpeta tester/:                                       */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        tester_ft_read_until_char.c -o tester_ft_read_until_char            */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester_ft_read_until_char                                            */
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

#define main	student_main_ft_read_until_char
#include "../ft_read_until_char.c"
#undef main

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

int	main(void)
{
	char	*line;
	int		fd;

	setvbuf(stdout, NULL, _IONBF, 0);

	mkdir("tmp", 0755);
	if (t_mkfile("tmp/lines.txt", "abc\ndef", 7) < 0
		|| t_mkfile("tmp/first.txt", "\nabc", 4) < 0
		|| t_mkfile("tmp/nostop.txt", "abcdef", 6) < 0
		|| t_mkfile("tmp/empty.txt", "", 0) < 0
		|| t_mkfile("tmp/wo.txt", "datos", 5) < 0)
	{
		printf("ERROR: no se pudieron crear los archivos de prueba\n");
		return (1);
	}
	fd = open("tmp/lines.txt", O_RDONLY);
	line = ft_read_until_char(fd, '\n');
	t_check_str(line, "abc\n", "para en el primer salto de linea");
	free(line);
	line = ft_read_until_char(fd, '\n');
	t_check_str(line, "def", "segunda llamada: resto sin salto");
	free(line);
	line = ft_read_until_char(fd, '\n');
	t_check_str(line, NULL, "tercera llamada: EOF devuelve NULL");
	free(line);
	close(fd);
	fd = open("tmp/first.txt", O_RDONLY);
	line = ft_read_until_char(fd, '\n');
	t_check_str(line, "\n", "stop en el primer caracter");
	free(line);
	close(fd);
	fd = open("tmp/nostop.txt", O_RDONLY);
	line = ft_read_until_char(fd, 'X');
	t_check_str(line, "abcdef", "sin stop devuelve todo");
	free(line);
	close(fd);
	fd = open("tmp/empty.txt", O_RDONLY);
	line = ft_read_until_char(fd, '\n');
	t_check_str(line, NULL, "archivo vacio devuelve NULL");
	free(line);
	close(fd);
	line = ft_read_until_char(-1, '\n');
	t_check_str(line, NULL, "fd -1 devuelve NULL");
	free(line);
	fd = open("tmp/wo.txt", O_WRONLY);
	line = ft_read_until_char(fd, '\n');
	t_check_str(line, NULL, "fd sin permiso de lectura devuelve NULL");
	free(line);
	close(fd);
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}
