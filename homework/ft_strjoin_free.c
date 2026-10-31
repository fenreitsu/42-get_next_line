/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin_free.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 16:24:35 by reiascan          #+#    #+#             */
/*   Updated: 2026/10/31 17:11:58 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../get_next_line.h"

static  size_t	ft_str_len(char *s)
{
	size_t	i;

	i = 0;
	while(s[i])
	{
		i++;
	}
	return (i);
}

static char	*ft_copy(char *dest, char const *src)
{
	size_t			i;

	i = 0;
	if (src == NULL)
		return (NULL);
	while(src[i])
	{
		dest[i] = src[i];
		i++;
	}
	return(&dest[i]);
}

char	*ft_strjoin_free(char *s1, const char *s2)
{
	size_t	total_len;
	char	*new_str;
	char	*next_ptr;

	if (s1 == NULL && s2 == NULL)
		return (NULL);
	total_len = (ft_str_len(s1) + ft_str_len((char *)s2));
	new_str = malloc(total_len + 1);
	if  (!new_str)
	{
		free(new_str);
		return (NULL);
	}
	next_ptr = ft_copy(new_str, s1);
	ft_copy(next_ptr, s2);
	new_str[total_len] = '\0';
	free(s1);
	return (new_str);
}
