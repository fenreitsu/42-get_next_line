/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_read_file.c                                                    */
/*                                                                            */
/*   Compilar desde la carpeta tester/:                                       */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        tester_ft_read_file.c -o tester_ft_read_file                        */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester_ft_read_file                                                  */
/*                                                                            */
/*   El include de abajo compila tu ../ft_read_file.c dentro del tester y     */
/*   renombra su main para que no choque con el de aqui. Si tu funcion vive   */
/*   en otro archivo, cambia la ruta del include.                             */
/*                                                                            */
/*   ASan avisa al terminar de cualquier fuga o acceso fuera de rango.        */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define main	student_main_ft_read_file
#include "../ft_read_file.c"
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

static void	t_case(const char *path, int buf_size, const char *expected,
		size_t elen, const char *name)
{
	char	*out;
	size_t	olen;
	char	detail[160];

	t_cap_start();
	ft_read_file(path, buf_size);
	out = t_cap_stop(&olen);
	snprintf(detail, sizeof(detail), "esperado %zu bytes, obtenido %zu",
		elen, olen);
	t_report(out != NULL && olen == elen && memcmp(out, expected, olen) == 0,
		name, detail);
	free(out);
}

int	main(void)
{
	const char	*content =
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
		"Maecenas bibendum lacus id sodales varius.\n"
		"Integer interdum orci at feugiat tristique.\n"
		"Sin salto de linea al final";
	size_t		len;

	setvbuf(stdout, NULL, _IONBF, 0);

	mkdir("tmp", 0755);
	len = strlen(content);
	if (t_mkfile("tmp/normal.txt", content, len) < 0
		|| t_mkfile("tmp/empty.txt", "", 0) < 0)
	{
		printf("ERROR: no se pudieron crear los archivos de prueba\n");
		return (1);
	}
	t_case("tmp/normal.txt", 1, content, len, "normal con buf_size 1");
	t_case("tmp/normal.txt", 42, content, len, "normal con buf_size 42");
	t_case("tmp/normal.txt", 9999, content, len, "normal con buf_size 9999");
	t_case("tmp/empty.txt", 42, "", 0, "archivo vacio no imprime nada");
	t_case("tmp/normal.txt", 0, "", 0, "buf_size 0 no imprime nada");
	t_case("tmp/normal.txt", -1, "", 0, "buf_size -1 no imprime nada");
	t_case("tmp/no_existe.txt", 42, "", 0, "archivo inexistente");
	t_case(NULL, 42, "", 0, "path NULL");
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}
