/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_chunks.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 14:13:19 by reiascan          #+#    #+#             */
/*   Updated: 2026/10/31 16:21:24 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../get_next_line.c"

void	ft_read_chunks(int fd, size_t chunk)
{
	ssize_t	nbr_bytes;
	char	*buffer;

	if (fd < 0 || chunk  == 0)
	{
		write(1, "ERROR\n", 6);
		return ;
	}
	buffer = malloc(chunk);
	if (!buffer)
	{
		free(buffer);
		return ;
	}
	nbr_bytes = read(fd, buffer, chunk);
	while (nbr_bytes > 0)
	{
		write(1, "[", 1);
		write(1, buffer, nbr_bytes);
		write(1, "]\n", 2);
		nbr_bytes = read(fd, buffer, chunk);
	}
	free(buffer);
	if (nbr_bytes < 0)
	{
		write(1, "ERROR\n", 6);
		return ;
	}
	else
		write(1, "DONE\n", 5);
}
