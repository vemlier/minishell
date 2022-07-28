/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: junkpark <junkpark@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/25 17:20:09 by junkpark          #+#    #+#             */
/*   Updated: 2022/07/27 20:57:09 by junkpark         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	get_cnt_of_pipe(t_token *token)
{
	size_t	i;
	size_t	cnt;

	i = 0;
	cnt = 0;
	while (token[i].type)
	{
		if (token[i].type == T_PIPE)
			cnt++;
		i++;
	}
	return (cnt);
}

t_cmd	*set_argc(t_cmd *cmd)
{
	size_t	i;
	size_t	j;
	size_t	argc;

	i = 0;
	while (cmd[i].token)
	{
		j = 0;
		argc = 0;
		while (cmd[i].token[j].type)
		{
			if (cmd[i].token[j].type == T_WORD)
				argc += 1;
			j++;
		}
		cmd[i].argc = argc;
		i++;
	}
	return (cmd);
}

t_cmd	*set_argv(t_cmd *cmd)
{
	size_t	i;
	size_t	j;
	size_t	k;

	i = 0;
	while (cmd[i].token)
	{
		j = 0;
		k = 0;
		cmd[i].argv = ft_calloc(cmd[i].argc + 1, sizeof(char *));
		while (cmd[i].token[j].type)
		{
			if (cmd[i].token[j].type == T_WORD)
			{
				cmd[i].argv[k] = cmd[i].token[j].str;
				k++;
			}
			j++;
		}
		i++;
	}
	return (cmd);
}

t_cmd	*get_cmd(t_token *token, t_env *envp_copy, char **envp_copy_arr)
{
	t_cmd	*cmd;
	size_t	cnt_of_cmd;
	size_t	i;
	size_t	j;
	size_t	k;

	cnt_of_cmd = get_cnt_of_pipe(token) + 1;
	cmd = ft_calloc(cnt_of_cmd + 1, sizeof(t_cmd));
	i = 0;
	j = 0;
	while (i < cnt_of_cmd)
	{
		k = 0;
		while (token[j + k].type != T_PIPE && token[j + k].type)
			k++;
		if (token[j + k].type == T_PIPE)
		{
			token[j + k].type = T_NULL;
			k++;
		}
		cmd[i].token = &(token[j]);
		cmd[i].envp_copy = envp_copy;
		cmd[i].envp_copy_arr = envp_copy_arr;
		j += k;
		i++;
	}
	cmd = set_argc(cmd);
	cmd = set_argv(cmd);
	// // to_print!!!
	// int	x = 0, y;
	// while (cmd[x].token)
	// {
	// 	printf("-----cmd[%d] : argc = %d-----\n", x, cmd[x].argc);
	// 	y = 0;
	// 	while (cmd[x].argv[y])
	// 	{
	// 		printf("|%s|\n", cmd[x].argv[y]);
	// 		y++;
	// 	}
	// 	x++;
	// }
	return (cmd);
}
