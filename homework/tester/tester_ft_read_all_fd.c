/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_read_all_fd.c                                                  */
/*                                                                            */
/*   Compilar desde la carpeta tester/ (BUFFER_SIZE es obligatorio):          */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        -D BUFFER_SIZE=42 tester_ft_read_all_fd.c -o tester_ft_read_all_fd  */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester_ft_read_all_fd                                                */
/*                                                                            */
/*   Prueba tambien con -D BUFFER_SIZE=1, 9999, 0 y -1.                       */
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

#define main	student_main_ft_read_all_fd
#include "../ft_read_all_fd.c"
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
	char	*content;

	fd = open(path, O_RDONLY);
	content = ft_read_all_fd(fd);
	close(fd);
	if (!content || !expected)
	{
		t_report(content == expected, name,
			content ? "se esperaba NULL" : "se esperaba contenido");
		free(content);
		return ;
	}
	t_report(strcmp(content, expected) == 0, name,
		"el contenido no coincide");
	free(content);
}

#if BUFFER_SIZE <= 0

int	main(void)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	mkdir("tmp", 0755);
	if (t_mkfile("tmp/all.txt", "abc", 3) < 0)
	{
		printf("ERROR: no se pudo crear el archivo de prueba\n");
		return (1);
	}
	printf("BUFFER_SIZE = %d\n\n", BUFFER_SIZE);
	t_case("tmp/all.txt", NULL, "BUFFER_SIZE <= 0 devuelve NULL");
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}

#else

int	main(void)
{
	const char	*text =
		"primera linea\nsegunda linea\ntercera sin salto";
	char		*big;
	char		*content;
	char		detail[128];
	int			fd;
	size_t		big_len;
	size_t		i;

	setvbuf(stdout, NULL, _IONBF, 0);

	mkdir("tmp", 0755);
	if (t_mkfile("tmp/all.txt", text, strlen(text)) < 0
		|| t_mkfile("tmp/empty.txt", "", 0) < 0
		|| t_mkfile("tmp/wo.txt", "datos", 5) < 0)
	{
		printf("ERROR: no se pudieron crear los archivos de prueba\n");
		return (1);
	}
	printf("BUFFER_SIZE = %d\n\n", BUFFER_SIZE);
	t_case("tmp/all.txt", text, "archivo de texto completo");
	t_case("tmp/empty.txt", NULL, "archivo vacio devuelve NULL");
	fd = open("tmp/all.txt", O_RDONLY);
	close(fd);
	content = ft_read_all_fd(-1);
	t_report(content == NULL, "fd -1 devuelve NULL", "");
	free(content);
	fd = open("tmp/wo.txt", O_WRONLY);
	content = ft_read_all_fd(fd);
	close(fd);
	t_report(content == NULL, "fd sin lectura devuelve NULL", "");
	free(content);
	big_len = 1024 * 1024;
	if (BUFFER_SIZE < 4096)
		big_len = 131072;
	if (BUFFER_SIZE <= 1)
		big_len = 32768;
	big = malloc(big_len);
	if (!big)
		return (1);
	memset(big, 'a', big_len);
	t_mkfile("tmp/big.txt", big, big_len);
	fd = open("tmp/big.txt", O_RDONLY);
	content = ft_read_all_fd(fd);
	close(fd);
	i = 0;
	if (content)
		i = strlen(content);
	snprintf(detail, sizeof(detail), "se esperaban %zu bytes", big_len);
	t_report(content && i == big_len && memcmp(content, big, big_len) == 0,
		"archivo grande", detail);
	free(content);
	free(big);
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}

#endif
