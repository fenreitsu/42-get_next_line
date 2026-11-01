/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/01 14:53:35 by reiascan          #+#    #+#             */
/*   Updated: 2026/11/01 18:13:03 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*read_until_nl(int fd, char *content, char *buffer)
{
	ssize_t	nbr_bytes;

	nbr_bytes = 1;
	while (nbr_bytes > 0)
	{
		if (content != NULL && ft_strchr((char *)content, '\n'))
			break ;
		nbr_bytes = read(fd, buffer, BUFFER_SIZE);
		if (nbr_bytes < 0)
		{
			free(content);
			return (NULL);
		}
		if (nbr_bytes == 0)
			break ;
		buffer[nbr_bytes] = '\0';
		content = ft_strjoin_free(content, buffer);
		if (content == NULL)
			return (NULL);
	}
	return (content);
}

static char	*extract_line(char *rest)
{
	size_t	i;
	size_t	len;
	char	*line;
	char	*newline;

	if (!rest || rest[0] == '\0')
		return (NULL);
	newline = ft_strchr((char *)rest, '\n');
	if (!newline)
		len = ft_strlen(rest);
	else
		len = ((size_t)(newline - rest)) + 1;
	line = malloc(len + 1);
	if (!line)
		return (NULL);
	i = 0;
	while (i < len)
	{
		line[i] = rest[i];
		i++;
	}
	line[i] = '\0';
	return (line);
}

static char	*rest_trimmer(char	*rest)
{
	char	*newline;
	char	*trimmed;

	newline = ft_strchr(rest, '\n');
	if (!newline || newline[1] == '\0')
	{
		free(rest);
		return (NULL);
	}
	trimmed = ft_strdup(newline + 1);
	free(rest);
	return (trimmed);
}

char	*get_next_line(int fd)
{
	static char	*content;
	char		*line;
	char		*buffer;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	content = read_until_nl(fd, content, buffer);
	free(buffer);
	if (!content)
		return (NULL);
	line = extract_line(content);
	if (!line)
	{
		free(content);
		content = NULL;
		return (NULL);
	}
	content = rest_trimmer(content);
	return (line);
}
