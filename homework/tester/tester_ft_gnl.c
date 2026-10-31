/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_gnl.c                                                          */
/*                                                                            */
/*   Este tester NO incluye tu codigo: declara los prototipos y se enlaza     */
/*   con tus archivos del nivel 6 (los que no tengan main). Ejemplo:          */
/*                                                                            */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        -D BUFFER_SIZE=42 -D MAX_FD=1024 \                                  */
/*        tester_ft_gnl.c ../gnl.c ../gnl_state.c ../gnl_cleanup.c \          */
/*        -o tester_ft_gnl                                                    */
/*                                                                            */
/*   BUFFER_SIZE es obligatorio (aplica a todos los .c enlazados).            */
/*   Si tu MAX_FD no es 1024, compila con -D MAX_FD=tu_valor.                 */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester_ft_gnl                                                        */
/*                                                                            */
/*   ASan detecta fugas de los restos y dobles free al limpiar.               */
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

#ifndef MAX_FD
# define MAX_FD 1024
#endif

char	*ft_gnl(int fd);
void	ft_gnl_cleanup(int fd);
void	ft_gnl_cleanup_all(void);

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

static void	t_case(int fd, const char *expected, const char *name)
{
	char	*line;

	line = ft_gnl(fd);
	t_check_str(line, expected, name);
	free(line);
}

#if BUFFER_SIZE <= 0

int	main(void)
{
	int		fd;

	setvbuf(stdout, NULL, _IONBF, 0);

	mkdir("tmp", 0755);
	if (t_mkfile("tmp/one.txt", "a\nb\nc", 5) < 0)
	{
		printf("ERROR: no se pudo crear el archivo de prueba\n");
		return (1);
	}
	printf("BUFFER_SIZE = %d\nMAX_FD = %d\n\n", BUFFER_SIZE, MAX_FD);
	fd = open("tmp/one.txt", O_RDONLY);
	t_case(fd, NULL, "BUFFER_SIZE <= 0 devuelve NULL");
	t_case(fd, NULL, "BUFFER_SIZE <= 0 sigue devolviendo NULL");
	close(fd);
	ft_gnl_cleanup_all();
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}

#else

int	main(void)
{
	char	*line;
	int		f1;
	int		f2;

	setvbuf(stdout, NULL, _IONBF, 0);

	mkdir("tmp", 0755);
	if (t_mkfile("tmp/one.txt", "a\nb\nc", 5) < 0
		|| t_mkfile("tmp/two.txt", "1a\n1b\n1c", 8) < 0
		|| t_mkfile("tmp/other.txt", "2a\n2b", 5) < 0
		|| t_mkfile("tmp/empty.txt", "", 0) < 0)
	{
		printf("ERROR: no se pudieron crear los archivos de prueba\n");
		return (1);
	}
	printf("BUFFER_SIZE = %d\nMAX_FD = %d\n\n", BUFFER_SIZE, MAX_FD);
	f1 = open("tmp/one.txt", O_RDONLY);
	t_case(f1, "a\n", "primera linea");
	t_case(f1, "b\n", "segunda linea");
	t_case(f1, "c", "ultima linea sin salto");
	t_case(f1, NULL, "tras el EOF devuelve NULL");
	t_case(f1, NULL, "segunda llamada tras el EOF");
	t_report(fcntl(f1, F_GETFD) != -1,
		"ft_gnl no cierra el fd del llamador", "el fd deberia seguir abierto");
	close(f1);
	f1 = open("tmp/empty.txt", O_RDONLY);
	t_case(f1, NULL, "archivo vacio devuelve NULL");
	close(f1);
	line = ft_gnl(-1);
	t_check_str(line, NULL, "fd -1 devuelve NULL");
	free(line);
	line = ft_gnl(999);
	t_check_str(line, NULL, "fd cerrado devuelve NULL");
	free(line);
	line = ft_gnl(MAX_FD);
	t_check_str(line, NULL, "fd >= MAX_FD devuelve NULL");
	free(line);
	f1 = open("tmp/two.txt", O_RDONLY);
	f2 = open("tmp/other.txt", O_RDONLY);
	t_case(f1, "1a\n", "multifd: linea 1 de fd1");
	t_case(f2, "2a\n", "multifd: linea 1 de fd2");
	t_case(f1, "1b\n", "multifd: linea 2 de fd1");
	t_case(f2, "2b", "multifd: linea 2 de fd2 (sin salto)");
	t_case(f1, "1c", "multifd: linea 3 de fd1");
	t_case(f2, NULL, "multifd: EOF de fd2");
	t_case(f1, NULL, "multifd: EOF de fd1");
	close(f1);
	close(f2);
	f1 = open("tmp/two.txt", O_RDONLY);
	t_case(f1, "1a\n", "cleanup: primera linea antes de limpiar");
	close(f1);
	ft_gnl_cleanup(f1);
	f1 = open("tmp/two.txt", O_RDONLY);
	t_case(f1, "1a\n", "cleanup: al reabrir el fd el estado esta limpio");
	t_case(f1, "1b\n", "cleanup: segunda linea tras reabrir");
	close(f1);
	ft_gnl_cleanup(f1);
	ft_gnl_cleanup(999);
	t_report(1, "cleanup de un fd sin estado no crashea", "");
	f1 = open("tmp/one.txt", O_RDONLY);
	t_case(f1, "a\n", "tras cleanup_all el estado vuelve a empezar");
	close(f1);
	ft_gnl_cleanup_all();
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}

#endif
