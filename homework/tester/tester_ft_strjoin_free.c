/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_strjoin_free.c                                                 */
/*                                                                            */
/*   Compilar desde la carpeta tester/:                                       */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        tester_ft_strjoin_free.c -o tester_ft_strjoin_free                  */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester_ft_strjoin_free                                               */
/*                                                                            */
/*   Si tu funcion vive en otro archivo, cambia la ruta del include.          */
/*   ASan detecta tanto las fugas (si no liberas s1) como los dobles free.    */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define main	student_main_ft_strjoin_free
#include "../ft_strjoin_free.c"
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

static char	*t_dupstr(const char *s)
{
	char	*d;
	size_t	len;

	len = strlen(s);
	d = malloc(len + 1);
	if (d)
		memcpy(d, s, len + 1);
	return (d);
}

int	main(void)
{
	char	*s1;
	char	*s2;
	char	*join;
	char	expected[201];
	int		i;

	setvbuf(stdout, NULL, _IONBF, 0);

	s1 = t_dupstr("abc");
	join = ft_strjoin_free(s1, "def");
	t_check_str(join, "abcdef", "concatena dos strings");
	free(join);
	join = ft_strjoin_free(NULL, "def");
	t_check_str(join, "def", "s1 NULL se comporta como strdup");
	free(join);
	s1 = t_dupstr("abc");
	join = ft_strjoin_free(s1, NULL);
	t_check_str(join, "abc", "s2 NULL devuelve s1 sin liberarlo");
	free(join);
	join = ft_strjoin_free(NULL, NULL);
	t_check_str(join, NULL, "s1 y s2 NULL devuelve NULL");
	free(join);
	s1 = t_dupstr("");
	join = ft_strjoin_free(s1, "def");
	t_check_str(join, "def", "s1 vacio");
	free(join);
	s1 = t_dupstr("abc");
	join = ft_strjoin_free(s1, "");
	t_check_str(join, "abc", "s2 vacio");
	free(join);
	s1 = malloc(101);
	s2 = malloc(101);
	i = 0;
	while (i < 100)
	{
		s1[i] = 'a';
		s2[i] = 'b';
		expected[i] = 'a';
		expected[i + 100] = 'b';
		i++;
	}
	s1[100] = '\0';
	s2[100] = '\0';
	expected[200] = '\0';
	join = ft_strjoin_free(s1, s2);
	t_check_str(join, expected, "strings largos de 100 + 100");
	free(join);
	free(s2);
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}
