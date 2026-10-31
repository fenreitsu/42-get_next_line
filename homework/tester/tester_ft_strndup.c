/* ************************************************************************** */
/*                                                                            */
/*   tester_ft_strndup.c                                                      */
/*                                                                            */
/*   Compilar desde la carpeta tester/:                                       */
/*     cc -Wall -Wextra -fsanitize=address,undefined \                        */
/*        tester_ft_strndup.c -o tester_ft_strndup                            */
/*                                                                            */
/*   Ejecutar:                                                                */
/*     ./tester_ft_strndup                                                    */
/*                                                                            */
/*   Si tu funcion vive en otro archivo, cambia la ruta del include.          */
/*   ASan detecta fugas y accesos fuera de rango.                             */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define main	student_main_ft_strndup
#include "../ft_strndup.c"
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

int	main(void)
{
	char	*s;

	setvbuf(stdout, NULL, _IONBF, 0);

	s = ft_strndup("abcdef", 3);
	t_check_str(s, "abc", "n menor que la longitud");
	free(s);
	s = ft_strndup("abcdef", 6);
	t_check_str(s, "abcdef", "n igual a la longitud");
	free(s);
	s = ft_strndup("abcdef", 10);
	t_check_str(s, "abcdef", "n mayor que la longitud");
	free(s);
	s = ft_strndup("abcdef", 0);
	t_check_str(s, "", "n 0 devuelve string vacio");
	free(s);
	s = ft_strndup("", 5);
	t_check_str(s, "", "string vacio");
	free(s);
	s = ft_strndup(NULL, 5);
	t_check_str(s, NULL, "s NULL devuelve NULL");
	free(s);
	s = ft_strndup("hola", 4);
	t_check_str(s, "hola", "duplica exactamente 4 caracteres");
	free(s);
	printf("\n%d/%d OK\n", g_pass, g_pass + g_fail);
	return (g_fail != 0);
}
