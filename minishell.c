/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: junkpark <junkpark@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/19 10:26:15 by chukim            #+#    #+#             */
/*   Updated: 2022/07/27 21:43:57 by junkpark         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_errno;

void	init_terminal(int argc)
{
	struct termios	term;

	if (argc != 1)
		exit_with_err("argument input error", NULL, 126, 1);
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ECHOCTL);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	set_signal();
}

void	ft_heredoc(t_cmd *cmd, size_t *tmp_file_cnt)
{
	int		fd;
	size_t	i;
	size_t	j;
	char	*path;
	char	*tmp;

	i = 0;
	*tmp_file_cnt = 0;
	while (cmd[i].token)
	{
		j = 0;
		while (cmd[i].token[j].type)
		{
			if (cmd[i].token[j].type == T_REDIRECT
				&& ft_strcmp("<<", cmd[i].token[j].str) == 0)
			{
				path = "/tmp/minishell_tmp_";
				tmp = ft_itoa(*tmp_file_cnt);
				path = ft_strjoin(path, tmp);
				free(tmp);
				fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 420);
				while (1)
				{
					tmp = readline(">");
					if (ft_strcmp(tmp, cmd[i].token[j + 1].str) == 0)
					{
						free(tmp);
						break ;
					}
					write(fd, tmp, ft_strlen(tmp));
					write(fd, "\n", 1);
					free(tmp);
				}
				free(cmd[i].token[j + 1].str);
				*tmp_file_cnt += 1;
				cmd[i].token[j + 1].str = path;
				close(fd);
			}
			j++;
		}
		i++;
	}
}

void	ft_unlink(size_t *tmp_file_cnt)
{
	size_t	i;
	char	*tmp;
	char	*path;

	i = 0;
	while (i < *tmp_file_cnt + 1)
	{
		path = "/tmp/minishell_tmp_";
		tmp = ft_itoa(i);
		path = ft_strjoin(path, tmp);
		free(tmp);
		unlink(path);
		free(path);
		i++;
	}
}

int	main(int argc, char **argv, char **envp)
{
	char	*input;
	char	**envp_copy_arr;
	size_t	tmp_file_cnt;
	t_cmd	*cmd;
	t_env	*envp_copy;
	t_token	*token;

	(void)argv;
	init_terminal(argc);
	envp_copy = copy_envp(envp);
	while (1)
	{
		input = readline("minishell $ ");
		if (input == NULL)
			break ;
		else if (*input != '\0')
		{
			add_history(input);
			token = parse(input, envp_copy);
			if (token)
			{
				envp_copy_arr = get_envp_copy_arr(envp_copy);
				cmd = get_cmd(token, envp_copy, envp_copy_arr);
				ft_heredoc(cmd, &tmp_file_cnt);
				ft_exec(cmd);
				free_cmd(&cmd);
				free_token(&token);
				free_envp_copy_arr(&envp_copy_arr);
				ft_unlink(&tmp_file_cnt);
			}
			else
			{
				// 에러 출력!
			}
		}
		free(input);
	}
	return (0);
}
