/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_file.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 12:00:01 by reiascan          #+#    #+#             */
/*   Updated: 2026/10/28 21:33:40 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# define BUF_SIZE 1024

#include  <unistd.h>
#include <stdio.h>
#include <stdlib.h>
// --> The fcntl() function provides for control over open files. 
// The fildes argument is a file descriptor.
#include <fcntl.h> 
//int int open(const char *pathname, int flags); --> open function prototype. 

void	ft_read_all(int fd, int buf_size)
{
	int 	bytes_count;
	char	*content;
	char	*buffer;
	char	*tmp;

	buffer = malloc(buf_size * sizeof(char));
	if  (!buffer)
		return ;
	content = NULL;
	bytes_count = read(fd, buffer, buf_size);
	while (bytes_count > 0)
	{
		ft_bzero(buffer, bytes_count);

	}
}

void	ft_read_file(const char *path, int buf_size)
{
	int		fd;
	char	*content_file;

	buf_size =  BUF_SIZE;

	if (path == NULL || buf_size == 0)
		return ;
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return ;
	content_file = ft_read_all(fd, buf_size);

}

int	main(int ac, char **av)
{
	if (ac != 3)
		return (1);
	ft_read_file(av[1], ft_atoi(av[2]));
	return (0);
}