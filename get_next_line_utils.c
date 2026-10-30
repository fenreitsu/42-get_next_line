/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 19:17:26 by reiascan          #+#    #+#             */
/*   Updated: 2026/10/30 20:24:34 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t ft_strlen(char *s)
{
	size_t i;

	i = 0;
	while (s[i])
	{
		i++;
	}
	return (i);
}

/* char	*ft_strjoin(char *content, char *buffer)
{
	size_t	i;
	size_t	j;
	char	*str;

	str = malloc((ft_strlen(content) + ft_strlen(buffer)) + 1);
	if (!str)
	{
		return (NULL);
	}
	
} */