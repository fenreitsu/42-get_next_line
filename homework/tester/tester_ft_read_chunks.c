/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_read_chunks.c                                                  */
/*                                                                            */
/*   Compilar desde la carpeta tester/:                                       */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        tester_ft_read_chunks.c -o tester_ft_read_chunks                    */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester_ft_read_chunks                                                */
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

#define main	student_main_ft_read_chunks
#include "../ft_read_chunks.c"
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

static FILE	*g_cap;
static int	g_saved;

static void	t_cap_start(void)
{
	fflush(stdout);
	g_cap = tmpfile();
	g_saved = dup(1);
	dup2(fileno(g_cap), 1);
}

static char	*t_cap_stop(size_t *len)
{
	char	*buf;
	long	sz;

	dup2(g_saved, 1);
	close(g_saved);
	fseek(g_cap, 0, SEEK_END);
	sz = ftell(g_cap);
	fseek(g_cap, 0, SEEK_SET);
	buf = malloc((size_t)sz + 1);
	if (buf)
	{
		fread(buf, 1, (size_t)sz, g_cap);
		buf[sz] = '\0';
	}
	fclose(g_cap);
	*len = (size_t)sz;
	return (buf);
}

static void	t_case_file(const char *path, size_t chunk, const char *expected,
		const char *name)
{
	char	*out;
	size_t	olen;
	char	detail[320];
	int		fd;

	fd = open(path, O_RDONLY);
	t_cap_start();
	ft_read_chunks(fd, chunk);
	out = t_cap_stop(&olen);
	close(fd);
	snprintf(detail, sizeof(detail), "esperado \"%s\", obtenido \"%s\"",
		expected, out ? out : "NULL");
	t_report(out && strcmp(out, expected) == 0, name, detail);
	free(out);
}

static void	t_case_fd(int fd, size_t chunk, const char *expected,
		const char *name)
{
	char	*out;
	size_t	olen;
	char	detail[320];

	t_cap_start();
	ft_read_chunks(fd, chunk);
	out = t_cap_stop(&olen);
	snprintf(detail, sizeof(detail), "esperado \"%s\", obtenido \"%s\"",
		expected, out ? out : "NULL");
	t_report(out && strcmp(out, expected) == 0, name, detail);
	free(out);
}

int	main(void)
{
	int		fd;

	setvbuf(stdout, NULL, _IONBF, 0);

	mkdir("tmp", 0755);
	if (t_mkfile("tmp/five.txt", "abcde", 5) < 0
		|| t_mkfile("tmp/empty.txt", "", 0) < 0)
	{
		printf("ERROR: no se pudieron crear los archivos de prueba\n");
		return (1);
	}
	t_case_file("tmp/five.txt", 2, "[ab]\n[cd]\n[e]\nDONE\n",
		"chunk 2 sobre 5 bytes");
	t_case_file("tmp/five.txt", 1, "[a]\n[b]\n[c]\n[d]\n[e]\nDONE\n",
		"chunk 1 sobre 5 bytes");
	t_case_file("tmp/five.txt", 10, "[abcde]\nDONE\n",
		"chunk mayor que el archivo");
	t_case_file("tmp/five.txt", 0, "ERROR\n", "chunk 0 imprime ERROR");
	t_case_file("tmp/empty.txt", 2, "DONE\n",
		"archivo vacio solo imprime DONE");
	fd = open("tmp/five.txt", O_WRONLY);
	t_case_fd(fd, 2, "ERROR\n", "fd sin permiso de lectura imprime ERROR");
	close(fd);
	t_case_fd(-1, 2, "ERROR\n", "fd -1 imprime ERROR");
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}
