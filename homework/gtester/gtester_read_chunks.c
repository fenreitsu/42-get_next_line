/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gtester_read_chunks.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reiascan <reiascan@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 14:22:06 by reiascan          #+#    #+#             */
/*   Updated: 2026/10/31 16:23:32 by reiascan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../get_next_line.h"
#include "../ft_read_chunks.c"

int main(void)
{
	int 	fd;	
	
	fd = open("../tester/tmp/normal.txt", O_RDONLY, 0644);
	ft_read_chunks(fd, 12);
	close (fd);
	return (0);
}