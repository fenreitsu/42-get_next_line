/* ************************************************************************** */
/*                                                                            */
/*   tester.c                                                                 */
/*                                                                            */
/*   Test automatico de get_next_line. No incluye tu codigo: se compila junto */
/*   a get_next_line.c y get_next_line_utils.c.                               */
/*                                                                            */
/*   Compilacion recomendada:                                                 */
/*     cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 \                          */
/*        get_next_line.c get_next_line_utils.c tests/tester.c -o tester      */
/*                                                                            */
/*   Bonus opcional:                                                          */
/*     anade -D TEST_BONUS=1                                                  */
/*                                                                            */
/*   Sanitizers:                                                              */
/*     anade -fsanitize=address,undefined -g                                  */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester                                                               */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# error "Compila con -D BUFFER_SIZE=N (por ejemplo, -D BUFFER_SIZE=42)"
#endif

#if BUFFER_SIZE < 1
# error "Este tester requiere BUFFER_SIZE >= 1"
#endif

#if defined(TEST_BONUS) && TEST_BONUS
# define BONUS_ENABLED 1
#else
# define BONUS_ENABLED 0
#endif

char	*get_next_line(int fd);

static int	g_pass;
static int	g_fail;
static char	g_dir[128];

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

static void	t_format(char *dst, size_t size, const char *src)
{
	size_t			i;
	size_t			j;
	unsigned char	c;

	i = 0;
	j = 0;
	if (!src)
	{
		snprintf(dst, size, "NULL");
		return ;
	}
	while (src[i] && j + 5 < size)
	{
		c = (unsigned char)src[i];
		if (c == '\n')
		{
			dst[j++] = '\\';
			dst[j++] = 'n';
		}
		else if (c == '\t')
		{
			dst[j++] = '\\';
			dst[j++] = 't';
		}
		else if (c == '\r')
		{
			dst[j++] = '\\';
			dst[j++] = 'r';
		}
		else if (c == '\\')
		{
			dst[j++] = '\\';
			dst[j++] = '\\';
		}
		else if (c >= 32 && c <= 126)
			dst[j++] = (char)c;
		else
			j += (size_t)snprintf(dst + j, size - j, "\\x%02x", c);
		i++;
	}
	if (src[i])
	{
		dst[j++] = '.';
		dst[j++] = '.';
		dst[j++] = '.';
	}
	dst[j] = '\0';
}

static void	t_check_str(const char *got, const char *exp, const char *name)
{
	char	detail[512];
	char	got_fmt[240];
	char	exp_fmt[240];

	t_format(got_fmt, sizeof(got_fmt), got);
	t_format(exp_fmt, sizeof(exp_fmt), exp);
	if (!got || !exp)
		snprintf(detail, sizeof(detail), "esperado %s, obtenido %s",
			exp_fmt, got_fmt);
	else
		snprintf(detail, sizeof(detail), "esperado \"%s\", obtenido \"%s\"",
			exp_fmt, got_fmt);
	t_report(got == exp || (got && exp && strcmp(got, exp) == 0),
		name, detail);
}

static void	t_case(int fd, const char *expected, const char *name)
{
	char	*line;

	line = get_next_line(fd);
	t_check_str(line, expected, name);
	free(line);
}

static int	t_write_all(int fd, const char *data, size_t len)
{
	size_t	off;
	ssize_t	n;

	off = 0;
	while (off < len)
	{
		n = write(fd, data + off, len - off);
		if (n <= 0)
			return (-1);
		off += (size_t)n;
	}
	return (0);
}

static int	t_mkfile(const char *name, const char *data, size_t len)
{
	char	path[256];
	int		fd;
	int		rc;

	snprintf(path, sizeof(path), "%s/%s", g_dir, name);
	fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd < 0)
		return (-1);
	rc = t_write_all(fd, data, len);
	if (close(fd) < 0)
		rc = -1;
	return (rc);
}

static int	t_open(const char *name)
{
	char	path[256];

	snprintf(path, sizeof(path), "%s/%s", g_dir, name);
	return (open(path, O_RDONLY));
}

static int	t_open_write(const char *name)
{
	char	path[256];

	snprintf(path, sizeof(path), "%s/%s", g_dir, name);
	return (open(path, O_WRONLY));
}

static void	t_unlink(const char *name)
{
	char	path[256];

	snprintf(path, sizeof(path), "%s/%s", g_dir, name);
	unlink(path);
}

static void	t_cleanup(void)
{
	t_unlink("one.txt");
	t_unlink("empty.txt");
	t_unlink("only_nl.txt");
	t_unlink("no_final.txt");
	t_unlink("final_nl.txt");
	t_unlink("keep.txt");
	t_unlink("write.txt");
	t_unlink("many.txt");
	t_unlink("long.txt");
	t_unlink("exact.txt");
	t_unlink("b1.txt");
	t_unlink("b2.txt");
	t_unlink("b3.txt");
	rmdir(g_dir);
}

static char	*t_repeat(char c, size_t n)
{
	char	*s;
	size_t	i;

	s = malloc(n + 1);
	if (!s)
		return (NULL);
	i = 0;
	while (i < n)
	{
		s[i] = c;
		i++;
	}
	s[i] = '\0';
	return (s);
}

static void	t_many_lines(void)
{
	char	*data;
	char	name[64];
	size_t	i;
	int		fd;

	data = malloc(50 * 2 + 1);
	if (!data)
	{
		t_report(0, "muchas lineas: malloc", "sin memoria");
		return ;
	}
	i = 0;
	while (i < 50)
	{
		data[i * 2] = 'x';
		data[i * 2 + 1] = '\n';
		i++;
	}
	data[i * 2] = '\0';
	if (t_mkfile("many.txt", data, 100) < 0)
	{
		t_report(0, "muchas lineas: archivo", "no se pudo crear");
		free(data);
		return ;
	}
	free(data);
	fd = t_open("many.txt");
	if (fd < 0)
	{
		t_report(0, "muchas lineas: open", "no se pudo abrir");
		return ;
	}
	i = 0;
	while (i < 50)
	{
		snprintf(name, sizeof(name), "muchas lineas: %zu", i + 1);
		t_case(fd, "x\n", name);
		i++;
	}
	t_case(fd, NULL, "muchas lineas: EOF");
	close(fd);
}

static void	t_long_line(void)
{
	size_t	long_len;
	char	*line;
	char	*data;
	char	*expected;
	int		fd;

	long_len = (size_t)BUFFER_SIZE * 3 + 7;
	line = t_repeat('A', long_len);
	data = malloc(long_len + 1 + 5 + 1);
	expected = malloc(long_len + 2);
	if (!line || !data || !expected)
	{
		free(line);
		free(data);
		free(expected);
		t_report(0, "linea larga: malloc", "sin memoria");
		return ;
	}
	memcpy(data, line, long_len);
	data[long_len] = '\n';
	memcpy(data + long_len + 1, "next\n", 5);
	data[long_len + 1 + 5] = '\0';
	memcpy(expected, line, long_len);
	expected[long_len] = '\n';
	expected[long_len + 1] = '\0';
	if (t_mkfile("long.txt", data, long_len + 1 + 5) < 0)
	{
		t_report(0, "linea larga: archivo", "no se pudo crear");
		free(line);
		free(data);
		free(expected);
		return ;
	}
	free(line);
	free(data);
	fd = t_open("long.txt");
	if (fd < 0)
	{
		t_report(0, "linea larga: open", "no se pudo abrir");
		free(expected);
		return ;
	}
	t_case(fd, expected, "linea mas larga que el buffer");
	t_case(fd, "next\n", "linea larga: siguiente linea");
	t_case(fd, NULL, "linea larga: EOF");
	close(fd);
	free(expected);
}

static void	t_exact_buffer(void)
{
	size_t	len;
	char	*line;
	char	*data;
	char	*expected;
	int		fd;

	len = (size_t)BUFFER_SIZE;
	line = t_repeat('B', len);
	data = malloc(len + 3 + 1);
	expected = malloc(len + 2);
	if (!line || !data || !expected)
	{
		free(line);
		free(data);
		free(expected);
		t_report(0, "linea exacta: malloc", "sin memoria");
		return ;
	}
	memcpy(data, line, len);
	data[len] = '\n';
	data[len + 1] = 'Z';
	data[len + 2] = '\n';
	data[len + 3] = '\0';
	memcpy(expected, line, len);
	expected[len] = '\n';
	expected[len + 1] = '\0';
	if (t_mkfile("exact.txt", data, len + 3) < 0)
	{
		t_report(0, "linea exacta: archivo", "no se pudo crear");
		free(line);
		free(data);
		free(expected);
		return ;
	}
	free(line);
	free(data);
	fd = t_open("exact.txt");
	if (fd < 0)
	{
		t_report(0, "linea exacta: open", "no se pudo abrir");
		free(expected);
		return ;
	}
	t_case(fd, expected, "linea justo del tamano del buffer");
	t_case(fd, "Z\n", "linea exacta: siguiente linea");
	t_case(fd, NULL, "linea exacta: EOF");
	close(fd);
	free(expected);
}

static void	t_stdin(void)
{
	const char	*data;
	int			saved;
	int			p[2];
	char		*line;

	data = "stdin\nlinea\n";
	saved = dup(STDIN_FILENO);
	if (saved < 0 || pipe(p) < 0)
	{
		if (saved >= 0)
			close(saved);
		t_report(0, "stdin: preparacion", "no se pudo duplicar o crear pipe");
		return ;
	}
	if (dup2(p[0], STDIN_FILENO) < 0)
	{
		close(p[0]);
		close(p[1]);
		close(saved);
		t_report(0, "stdin: preparacion", "no se pudo redirigir stdin");
		return ;
	}
	close(p[0]);
	if (t_write_all(p[1], data, strlen(data)) < 0)
	{
		close(p[1]);
		dup2(saved, STDIN_FILENO);
		close(saved);
		t_report(0, "stdin: preparacion", "no se pudo escribir en el pipe");
		return ;
	}
	close(p[1]);
	line = get_next_line(STDIN_FILENO);
	t_check_str(line, "stdin\n", "stdin: primera linea");
	free(line);
	line = get_next_line(STDIN_FILENO);
	t_check_str(line, "linea\n", "stdin: segunda linea");
	free(line);
	line = get_next_line(STDIN_FILENO);
	t_check_str(line, NULL, "stdin: EOF");
	free(line);
	dup2(saved, STDIN_FILENO);
	close(saved);
}

#if BONUS_ENABLED

static void	t_bonus(void)
{
	int	f1;
	int	f2;
	int	f3;

	if (t_mkfile("b1.txt", "1a\n1b\n1c", 8) < 0
		|| t_mkfile("b2.txt", "2a\n2b", 5) < 0
		|| t_mkfile("b3.txt", "3a\n3b\n3c\n", 9) < 0)
	{
		t_report(0, "bonus: archivos", "no se pudieron crear");
		return ;
	}
	f1 = t_open("b1.txt");
	f2 = t_open("b2.txt");
	f3 = t_open("b3.txt");
	if (f1 < 0 || f2 < 0 || f3 < 0)
	{
		if (f1 >= 0)
			close(f1);
		if (f2 >= 0)
			close(f2);
		if (f3 >= 0)
			close(f3);
		t_report(0, "bonus: open", "no se pudieron abrir");
		return ;
	}
	t_case(f1, "1a\n", "bonus: fd1 linea 1");
	t_case(f2, "2a\n", "bonus: fd2 linea 1");
	t_case(f3, "3a\n", "bonus: fd3 linea 1");
	t_case(f1, "1b\n", "bonus: fd1 linea 2");
	t_case(f2, "2b", "bonus: fd2 linea 2 sin salto");
	t_case(f3, "3b\n", "bonus: fd3 linea 2");
	t_case(f1, "1c", "bonus: fd1 linea 3 sin salto");
	t_case(f3, "3c\n", "bonus: fd3 linea 3");
	t_case(f2, NULL, "bonus: EOF de fd2");
	t_case(f1, NULL, "bonus: EOF de fd1");
	t_case(f3, NULL, "bonus: EOF de fd3");
	close(f1);
	close(f2);
	close(f3);
}

#endif

int	main(void)
{
	char	*line;
	int		fd;

	setvbuf(stdout, NULL, _IONBF, 0);
	snprintf(g_dir, sizeof(g_dir), "gnl_tester_%ld", (long)getpid());
	if (mkdir(g_dir, 0755) < 0 && errno != EEXIST)
	{
		perror("mkdir");
		return (1);
	}
	printf("get_next_line tester\n");
	printf("BUFFER_SIZE = %d\n", BUFFER_SIZE);
	if (BONUS_ENABLED)
		printf("BONUS = si\n\n");
	else
		printf("BONUS = no (usa -D TEST_BONUS=1 para probar multi-fd)\n\n");
	if (t_mkfile("one.txt", "a\nb\nc", 5) < 0
		|| t_mkfile("empty.txt", "", 0) < 0
		|| t_mkfile("only_nl.txt", "\n\n\n", 3) < 0
		|| t_mkfile("no_final.txt", "sin\nsalto", 9) < 0
		|| t_mkfile("final_nl.txt", "con\nsalto\n", 10) < 0
		|| t_mkfile("keep.txt", "keep\nline\n", 10) < 0
		|| t_mkfile("write.txt", "x", 1) < 0)
	{
		printf("ERROR: no se pudieron crear los archivos de prueba\n");
		t_cleanup();
		return (1);
	}
	fd = t_open("one.txt");
	if (fd >= 0)
	{
		t_case(fd, "a\n", "primera linea");
		t_case(fd, "b\n", "segunda linea");
		t_case(fd, "c", "ultima linea sin salto");
		t_case(fd, NULL, "tras EOF devuelve NULL");
		t_case(fd, NULL, "segunda llamada tras EOF");
		close(fd);
	}
	else
		t_report(0, "abrir one.txt", "no se pudo abrir");
	fd = t_open("empty.txt");
	if (fd >= 0)
	{
		t_case(fd, NULL, "archivo vacio devuelve NULL");
		t_case(fd, NULL, "archivo vacio: segunda llamada");
		close(fd);
	}
	fd = t_open("only_nl.txt");
	if (fd >= 0)
	{
		t_case(fd, "\n", "solo saltos: linea 1");
		t_case(fd, "\n", "solo saltos: linea 2");
		t_case(fd, "\n", "solo saltos: linea 3");
		t_case(fd, NULL, "solo saltos: EOF");
		close(fd);
	}
	fd = t_open("no_final.txt");
	if (fd >= 0)
	{
		t_case(fd, "sin\n", "sin salto final: linea 1");
		t_case(fd, "salto", "sin salto final: ultima linea");
		t_case(fd, NULL, "sin salto final: EOF");
		close(fd);
	}
	fd = t_open("final_nl.txt");
	if (fd >= 0)
	{
		t_case(fd, "con\n", "con salto final: linea 1");
		t_case(fd, "salto\n", "con salto final: linea 2");
		t_case(fd, NULL, "con salto final: EOF");
		close(fd);
	}
	t_many_lines();
	t_long_line();
	t_exact_buffer();
	line = get_next_line(-1);
	t_check_str(line, NULL, "fd -1 devuelve NULL");
	free(line);
	line = get_next_line(9999);
	t_check_str(line, NULL, "fd 9999 devuelve NULL");
	free(line);
	fd = t_open("one.txt");
	if (fd >= 0)
	{
		close(fd);
		line = get_next_line(fd);
		t_check_str(line, NULL, "fd cerrado devuelve NULL");
		free(line);
	}
	fd = t_open_write("write.txt");
	if (fd >= 0)
	{
		line = get_next_line(fd);
		t_check_str(line, NULL, "fd de solo escritura devuelve NULL");
		free(line);
		close(fd);
	}
	fd = t_open("keep.txt");
	if (fd >= 0)
	{
		line = get_next_line(fd);
		t_check_str(line, "keep\n", "no cerrar fd: primera linea");
		free(line);
		t_report(fcntl(fd, F_GETFD) != -1,
			"get_next_line no cierra el fd", "el fd fue cerrado");
		while ((line = get_next_line(fd)) != NULL)
			free(line);
		close(fd);
	}
	t_stdin();
#if BONUS_ENABLED
	t_bonus();
#endif
	t_cleanup();
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}
