/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: junkpark <junkpark@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/24 12:32:50 by chukim            #+#    #+#             */
/*   Updated: 2022/07/27 21:23:00 by junkpark         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	get_cnt_of_cmd(t_cmd *cmd)
{
	size_t	i;

	i = 0;
	while (cmd[i].token)
		i++;
	return (i);
}

int	is_builtin(t_cmd *cmd)
{
	if (cmd->argc == 0)
		return (0);
	if (ft_strcmp(cmd->argv[0], "cd") == 0
		|| ft_strcmp(cmd->argv[0], "echo") == 0
		|| ft_strcmp(cmd->argv[0], "pwd") == 0
		|| ft_strcmp(cmd->argv[0], "export") == 0
		|| ft_strcmp(cmd->argv[0], "unset") == 0
		|| ft_strcmp(cmd->argv[0], "env") == 0
		|| ft_strcmp(cmd->argv[0], "exit") == 0)
		return (1);
	return (0);
}

void	init_pipe(int *read_pipe, int *write_pipe)
{
	read_pipe[0] = -1;
	read_pipe[1] = -1;
	write_pipe[0] = -1;
	write_pipe[1] = -1;
}

void	update_pipe(int *read_pipe, int *write_pipe, int i, int	cnt_of_cmd)
{
	if (0 < i && i + 1 <= cnt_of_cmd) // 중간과 끝에 있는 명령어
	{
		read_pipe[READ] = write_pipe[READ];
		read_pipe[WRITE] = write_pipe[WRITE];
	}
	if (i == 0 && i + 1 == cnt_of_cmd)
	{
		write_pipe[READ] = -1;
		write_pipe[WRITE] = STDOUT_FILENO;
	}
	else if (i + 1 == cnt_of_cmd)
	{
		write_pipe[READ] = -1;
		write_pipe[WRITE] = STDOUT_FILENO;
	}
	else if (0 <= i && i + 1 < cnt_of_cmd) // 중간에 있는 명령어
		pipe(write_pipe);
}

void	redirection_in(t_cmd *cmd)
{
	int		fd;
	size_t	i;

	i = 0;
	while (cmd->token[i].type)
	{
		if (cmd->token[i].type == T_REDIRECT
			&& (ft_strcmp(cmd->token[i].str, "<") == 0
			|| ft_strcmp(cmd->token[i].str, "<<")))
		{
			close(cmd->read);
			fd = open(cmd->token[i + 1].str, O_RDONLY);
			if (fd == -1) // 에러메세지출력
				exit(1);
			cmd->read = fd;
		}
		i++;
	}
}

void	redirection_out(t_cmd *cmd)
{
	int		fd;
	size_t	i;

	i = 0;
	while (cmd->token[i].type)
	{
		if (cmd->token[i].type == T_REDIRECT
			&& ft_strcmp(cmd->token[i].str, ">>") == 0)
		{
			close(cmd->write);
			fd = open(cmd->token[i + 1].str, O_WRONLY | O_CREAT | O_APPEND, 420);
			cmd->write = fd;
		}
		else if (cmd->token[i].type == T_REDIRECT
			&& ft_strcmp(cmd->token[i].str, ">") == 0)
		{
			close(cmd->write);
			fd = open(cmd->token[i + 1].str, O_WRONLY | O_CREAT | O_TRUNC, 420);
			if (fd == -1) // 엑시트 코드
				exit(1);
			cmd->write = fd;
		}
		i++;
	}
}

void	ft_exec(t_cmd *cmd)
{
	int		read_pipe[2];
	int		write_pipe[2];
	pid_t	pid;
	size_t	i;
	size_t	cnt_of_cmd;

	cnt_of_cmd = get_cnt_of_cmd(cmd);
	if (cnt_of_cmd == 1 && is_builtin(&(cmd[0])))
	{
		// 빌트인 실행, 리다이렉션도 해줘야함!!!
		return ;
	}
	i = 0;
	init_pipe(read_pipe, write_pipe);
	while (i < cnt_of_cmd)
	{
		update_pipe(read_pipe, write_pipe, i, cnt_of_cmd);
		if (i == 0) // 첫 명령어
		{
			cmd[i].read = STDIN_FILENO;
			cmd[i].write = write_pipe[WRITE];
		}
		else if (0 < i && i + 1 < cnt_of_cmd) // 중간 명령어
		{
			cmd[i].read = read_pipe[READ];
			cmd[i].write = write_pipe[WRITE];
		}
		else if (i + 1 == cnt_of_cmd) // 마지막 명령어의 경우
		{
			cmd[i].read = read_pipe[READ];
			cmd[i].write = STDOUT_FILENO;
		}
		pid = fork();
		if (pid == 0) // 자식 프로세스
		{
			close(read_pipe[WRITE]);
			close(write_pipe[READ]);
			redirection_in(&(cmd[i]));
			redirection_out(&(cmd[i]));
			if (cmd[i].read != 0)
				{
					dup2(cmd[i].read, STDIN_FILENO);
					close(cmd[i].read);
				}
			if (cmd[i].write != 1)
			{
				dup2(cmd[i].write, STDOUT_FILENO);
				close(cmd[i].write);
			}
			// 실행 (is_builtin?, 오픈 가능한지???)
			// exit 해야할 듯?(builtin인 경우 실행 후 exit???, 그 외는 execve)
			exit(1);
		}
		close(read_pipe[READ]);
		close(read_pipe[WRITE]);
		i++;
	}
	while (wait(&g_errno) != -1)
		;
	if (WIFEXITED(g_errno))
		g_errno = WEXITSTATUS(g_errno);

}
