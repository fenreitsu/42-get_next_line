/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_count_bytes.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 10:20:32 by reiascan          #+#    #+#             */
/*   Updated: 2026/10/31 12:16:57 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../get_next_line.h"

static int	free_and_return(char *buffer)
{
	free (buffer);
	return (0);
}

size_t	ft_count_bytes(int fd)
{
	ssize_t	nbr_bytes;
	ssize_t	total_bytes;
	char	*buffer;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (0);
	buffer = malloc(BUFFER_SIZE * sizeof(char));
	if (!buffer)
		return (free_and_return(buffer));
	nbr_bytes = read(fd, buffer, BUFFER_SIZE);
	total_bytes = 0;
	while (nbr_bytes > 0)
	{
		total_bytes += nbr_bytes;
		nbr_bytes = read (fd, buffer, BUFFER_SIZE);
	}
	if (nbr_bytes < 0)
		return (free_and_return(buffer));
	free (buffer);
	return (total_bytes);
}
