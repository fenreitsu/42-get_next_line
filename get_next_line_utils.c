/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 19:17:26 by reiascan          #+#    #+#             */
/*   Updated: 2026/11/01 17:50:39 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen(char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		i++;
	}
	return (i);
}

char	*ft_strdup(const char *s)
{
	size_t	i;
	size_t	len;
	char	*dup;

	len = ft_strlen((char *)s);
	dup = malloc (len + 1);
	if (dup == NULL)
		return (NULL);
	i = 0;
	while (s[i])
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

char	*ft_strchr(const char *s, int c)
{
	int	i;
	int	len;

	i = 0;
	len = ft_strlen((char *)s);
	while (i <= len)
	{
		if (s[i] == (char)c)
			return ((char *)&s[i]);
		i++;
	}
	return (NULL);
}

char	*ft_strjoin(char *s1, char *s2)
{
	size_t	i;
	size_t	j;
	char	*real_str;

	real_str = malloc(ft_strlen(s1) + ft_strlen (s2) + 1);
	if (!real_str)
		return (NULL);
	i = -1;
	while (s1[++i])
	{
		real_str[i] = s1[i];
	}
	j = -1;
	while (s2[++j])
	{
		real_str[i] = s2[j];
		i++;
	}
	real_str[i] = '\0';
	return (real_str);
}

char	*ft_strjoin_free(char *s1, const char *s2)
{
	char	*new_str;

	if (s1 == NULL && s2 == NULL)
		return (NULL);
	if (s1 == NULL)
	{
		new_str = ft_strdup(s2);
		return (new_str);
	}
	if (s2 == NULL)
		return (s1);
	new_str = ft_strjoin(s1, (char *)s2);
	free(s1);
	return (new_str);
}
