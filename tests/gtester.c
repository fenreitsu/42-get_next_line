/* ************************************************************************** */
/*                                                                            */
/*   gtester.c                                                                */
/*                                                                            */
/*   Test visual y sencillo de get_next_line. No usa colores ni ncurses:      */
/*   solo texto claro para mirar las lineas con tus propios ojos.             */
/*                                                                            */
/*   Compilacion recomendada:                                                 */
/*     cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 \                          */
/*        get_next_line.c get_next_line_utils.c tests/gtester.c -o gtester    */
/*                                                                            */
/*   Uso:                                                                     */
/*     ./gtester              crea archivos de ejemplo                        */
/*     ./gtester archivo.txt  lee ese archivo                                 */
/*     ./gtester -            lee de stdin                                    */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# error "Compila con -D BUFFER_SIZE=N (por ejemplo, -D BUFFER_SIZE=42)"
#endif

#if BUFFER_SIZE < 1
# error "Este tester requiere BUFFER_SIZE >= 1"
#endif

char	*get_next_line(int fd);

static char	g_dir[128];

static void	print_preview(const char *s, size_t max)
{
	size_t			i;
	unsigned char	c;

	i = 0;
	while (s[i] && i < max)
	{
		c = (unsigned char)s[i];
		if (c == '\n')
			printf("\\n");
		else if (c == '\t')
			printf("\\t");
		else if (c == '\r')
			printf("\\r");
		else if (c == '\\')
			printf("\\\\");
		else if (c >= 32 && c <= 126)
			putchar(c);
		else
			printf("\\x%02x", c);
		i++;
	}
	if (s[i])
		printf("...");
}

static void	show_file(const char *label, int fd)
{
	char	*line;
	size_t	n;
	size_t	len;

	printf("=== %s ===\n", label);
	n = 0;
	while ((line = get_next_line(fd)) != NULL)
	{
		n++;
		len = strlen(line);
		printf("linea %zu: longitud=%zu, salto=%s\n", n, len,
			(len > 0 && line[len - 1] == '\n') ? "si" : "no");
		printf("  texto: \"");
		print_preview(line, 120);
		printf("\"\n");
		free(line);
	}
	printf("[EOF] get_next_line devolvio NULL\n\n");
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

static void	t_unlink(const char *name)
{
	char	path[256];

	snprintf(path, sizeof(path), "%s/%s", g_dir, name);
	unlink(path);
}

static void	t_cleanup(void)
{
	t_unlink("normal.txt");
	t_unlink("vacio.txt");
	t_unlink("saltos.txt");
	t_unlink("larga.txt");
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

static int	t_demo(void)
{
	const char	*normal;
	const char	*saltos;
	char		*long_line;
	char		*long_data;
	size_t		long_len;
	int			fd;

	normal = "Primera linea\nSegunda linea\nTercera sin salto";
	saltos = "\n\n\n";
	long_len = (size_t)BUFFER_SIZE * 2 + 10;
	long_line = t_repeat('L', long_len);
	long_data = malloc(long_len + 8);
	if (!long_line || !long_data)
	{
		free(long_line);
		free(long_data);
		printf("ERROR: sin memoria para el archivo largo\n");
		return (-1);
	}
	memcpy(long_data, long_line, long_len);
	long_data[long_len] = '\n';
	memcpy(long_data + long_len + 1, "final\n", 6);
	long_data[long_len + 7] = '\0';
	if (t_mkfile("normal.txt", normal, strlen(normal)) < 0
		|| t_mkfile("vacio.txt", "", 0) < 0
		|| t_mkfile("saltos.txt", saltos, strlen(saltos)) < 0
		|| t_mkfile("larga.txt", long_data, long_len + 7) < 0)
	{
		printf("ERROR: no se pudieron crear los archivos de ejemplo\n");
		free(long_line);
		free(long_data);
		t_cleanup();
		return (-1);
	}
	free(long_line);
	free(long_data);
	fd = t_open("normal.txt");
	if (fd >= 0)
	{
		show_file("normal.txt", fd);
		close(fd);
	}
	fd = t_open("vacio.txt");
	if (fd >= 0)
	{
		show_file("vacio.txt", fd);
		close(fd);
	}
	fd = t_open("saltos.txt");
	if (fd >= 0)
	{
		show_file("saltos.txt", fd);
		close(fd);
	}
	fd = t_open("larga.txt");
	if (fd >= 0)
	{
		show_file("larga.txt (vista recortada a 120 caracteres)", fd);
		close(fd);
	}
	t_cleanup();
	return (0);
}

int	main(int ac, char **av)
{
	int	fd;

	setvbuf(stdout, NULL, _IONBF, 0);
	printf("get_next_line - test visual\n");
	printf("BUFFER_SIZE = %d\n\n", BUFFER_SIZE);
	if (ac > 2)
	{
		printf("Uso: %s [archivo|-]\n", av[0]);
		return (1);
	}
	if (ac == 2)
	{
		if (strcmp(av[1], "-") == 0)
		{
			show_file("stdin", STDIN_FILENO);
			return (0);
		}
		fd = open(av[1], O_RDONLY);
		if (fd < 0)
		{
			perror(av[1]);
			return (1);
		}
		show_file(av[1], fd);
		close(fd);
		return (0);
	}
	snprintf(g_dir, sizeof(g_dir), "gnl_gtester_%ld", (long)getpid());
	if (mkdir(g_dir, 0755) < 0 && errno != EEXIST)
	{
		perror("mkdir");
		return (1);
	}
	if (t_demo() < 0)
		return (1);
	printf("Fin del test visual.\n");
	return (0);
}
