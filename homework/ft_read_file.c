/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_file.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 12:00:01 by reiascan          #+#    #+#             */
/*   Updated: 2026/10/30 20:54:05 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include  <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// --> The fcntl() function provides for control over open files. 
// The fildes argument is a file descriptor.
#include <fcntl.h> 
//int int open(const char *pathname, int flags); --> open function prototype. 
size_t	ft_strlen(char *s);

void	ft_read_file(const char *path, int buf_size)
{
	int		fd;
	ssize_t		nbr_bytes;
	char	*buffer;

	if (path == NULL || buf_size <= 0)
		return ;
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return ;
	buffer = malloc(buf_size);
	if (!buffer)
	{
		close(fd);
		return ;
	}
	nbr_bytes = read(fd, buffer, buf_size);
	while(nbr_bytes > 0)
	{
		write(1, buffer, nbr_bytes);
		nbr_bytes = read(fd, buffer, buf_size);
	}
	free(buffer);
	close(fd);
}

#include <stdlib.h>

int	main(int ac, char **av)
{
	int	size;

	if (ac != 3 || !av[1])
		return (1);
	size = atoi(av[2]);
	ft_read_file(av[1], size);
	return (0);
}