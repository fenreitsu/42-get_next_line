/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_realloc.c                                                      */
/*                                                                            */
/*   Compilar desde la carpeta tester/:                                       */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        tester_ft_realloc.c -o tester_ft_realloc                            */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester_ft_realloc                                                    */
/*                                                                            */
/*   Si tu funcion vive en otro archivo, cambia la ruta del include.          */
/*   ASan detecta fugas (incluido el bloque viejo sin liberar) y desbordes.   */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define main	student_main_ft_realloc
#include "../ft_realloc.c"
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

static void	t_check_ptr(void *got, int should_exist, const char *name)
{
	t_report((got != NULL) == should_exist, name,
		should_exist ? "se esperaba un puntero valido" : "se esperaba NULL");
}

static void	t_check_bytes(void *got, const char *exp, size_t n, const char *name)
{
	char	detail[160];

	snprintf(detail, sizeof(detail), "se esperaban %zu bytes \"%s\"", n, exp);
	t_report(got != NULL && memcmp(got, exp, n) == 0, name, detail);
}

static void	*t_fill(size_t size, char c)
{
	void	*p;

	p = malloc(size);
	if (p)
		memset(p, c, size);
	return (p);
}

int	main(void)
{
	void	*p;
	void	*r;

	setvbuf(stdout, NULL, _IONBF, 0);

	p = ft_realloc(NULL, 0, 16);
	t_check_ptr(p, 1, "ptr NULL equivale a malloc");
	if (p)
		memset(p, 'x', 16);
	free(p);
	r = ft_realloc(NULL, 0, 0);
	t_check_ptr(r, 0, "ptr NULL y new_size 0 devuelve NULL");
	free(r);
	p = t_fill(5, 'a');
	r = ft_realloc(p, 5, 20);
	t_check_ptr(r, 1, "expandir devuelve puntero valido");
	t_check_bytes(r, "aaaaa", 5, "expandir conserva el contenido");
	free(r);
	p = t_fill(10, 'b');
	r = ft_realloc(p, 10, 4);
	t_check_ptr(r, 1, "truncar devuelve puntero valido");
	t_check_bytes(r, "bbbb", 4, "truncar conserva el prefijo");
	free(r);
	p = t_fill(8, 'c');
	r = ft_realloc(p, 8, 8);
	t_check_ptr(r, 1, "mismo tamano devuelve puntero valido");
	t_check_bytes(r, "cccccccc", 8, "mismo tamano conserva el contenido");
	free(r);
	p = t_fill(8, 'd');
	r = ft_realloc(p, 8, 0);
	t_check_ptr(r, 0, "new_size 0 libera y devuelve NULL");
	p = t_fill(1, 'e');
	r = ft_realloc(p, 1, 4096);
	t_check_ptr(r, 1, "expansion grande devuelve puntero valido");
	t_check_bytes(r, "e", 1, "expansion grande conserva el contenido");
	free(r);
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}
