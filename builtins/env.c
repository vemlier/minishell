/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chukim <chukim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/27 13:35:52 by chukim            #+#    #+#             */
/*   Updated: 2022/07/27 13:35:54 by chukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_env(t_env *envp_copy)
{
	t_env	*current;

	current = envp_copy;
	while (current->next != NULL)
	{
		printf("%s=%s\n", current->key, current->value);
		current = current->next;
	}
}
