/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chukim <chukim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/19 10:28:49 by chukim            #+#    #+#             */
/*   Updated: 2022/07/28 11:29:03 by chukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <string.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <signal.h>
# include <termios.h>
# include "libft/libft.h"

enum	e_types
{
	T_NULL,
	T_SQUOTES,
	T_DQUOTES,
	T_WORD,
	T_PIPE,
	T_REDIRECT,
	T_FILE,
	T_ERROR
};

# define READ 0
# define WRITE 1

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_token
{
	char			*str;
	int				type;
}	t_token;

typedef struct s_cmd {
	int		argc;
	char	**argv;

	int		read;
	int		write;
	char	**envp_copy_arr;
	t_env	*envp_copy;
	t_token	*token;
}	t_cmd;

extern int	g_errno;

// free.c
void	free_token(t_token **token);
void	free_envp_copy_arr(char ***envp_copy_arr);
void	free_cmd(t_cmd **cmd);

// exec.c
void	ft_exec(t_cmd *cmd);

char	*ft_strndup(const char *s, size_t n);

t_token	*parse(char *input, t_env *envp_copy);

// print.c
void	print_str_arr(char *argv[]);
void	print_export(t_cmd *cmd);

// char	**get_envp(char *envp[]);
// char	*get_cmd_argv(char *path[], char *cmd);
char	**get_envp_copy_arr(t_env *envp_copy);
char	*get_env(t_env *envp_copy, char *key);
size_t	get_cnt_of_cmd(t_cmd *cmd);
t_cmd	*get_cmd(t_token *token, t_env *envp_copy, char **envp_copy_arr);
t_env	*env_new(void);
void	add_env(t_env *envp_copy, char *key, char *value);
void	add_or_update_env(char *str, t_env *envp);
t_env	*copy_envp(char *envp[]);
char	**divide_with_equal(char *str);

void	exit_with_err(char *str1, char *str2, int exit_code, int to_exit);

void	exec_process(t_token *token);

void	set_signal(void);

void	ft_export(t_cmd *cmd);

void	ft_unset(t_cmd *cmd);

void	ft_env(t_env *envp_copy);

void	ft_pwd(t_cmd *cmd);

void	ft_echo(t_cmd *cmd);

void	ft_cd(t_cmd *cmd);

#endif
