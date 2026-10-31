/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_until_char.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:24:59 by reiascan          #+#    #+#             */
/*   Updated: 2026/10/31 13:49:48 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../get_next_line.h"

static void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t					i;
	unsigned char			*ptrd;
	const  unsigned char	*ptrs;

	i = 0;
	ptrd = (unsigned char *)dest;
	ptrs = (unsigned char *)src;
	while(i < n)
	{
		ptrd[i] = ptrs[i];
		i++;
	}
	return (dest);
}

static	char	*free_and_return(char *line)
{
	free(line);
	return (NULL);
}

char	*ft_read_until_char(int fd, char stop)
{
	size_t	length;
	size_t	capacity;
	ssize_t	nbr_bytes;
	char	c;
	char	*line;
	char	*tmp_line;

	if (fd < 0)
		return (NULL);

	capacity = 1;
	line = malloc(capacity);
	if (!line)
		return (free_and_return(line));
	nbr_bytes = read(fd, &c, 1);
	length = 0;
	while (nbr_bytes > 0)
	{
		if (length + 1 >= capacity)
		{
			capacity++;
			tmp_line = malloc(capacity);
			if (!tmp_line)
				return (free_and_return(tmp_line));
			ft_memcpy(tmp_line, line, length);
			free(line);
			line = tmp_line;
		}
		line[length++] = c;
		if (c == stop)
			break;
		nbr_bytes = read(fd, &c, 1);
	}
	if (nbr_bytes < 0 || length == 0)
		return (free_and_return(line));
	line[length] = '\0';
	return (line);
}