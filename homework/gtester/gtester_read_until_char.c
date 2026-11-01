/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gtester_read_until_char.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 13:44:36 by reiascan          #+#    #+#             */
/*   Updated: 2026/11/01 17:11:02 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "../../get_next_line.h"
#include "../ft_read_until_char.c"

int main(void)
{
	int 	fd;
	char 	*line;	
	
	fd = open("../tester/tmp/normal.txt", O_RDONLY, 0644);
	line = ft_read_until_char(fd, 'g');
	close (fd);
	if (!line)
		return (1);
	printf("%s", line);
	return (0);
}