/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chukim <chukim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/24 13:24:46 by chukim            #+#    #+#             */
/*   Updated: 2022/07/28 11:31:22 by chukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	get_envp_copy_size(t_env *envp_copy)
{
	size_t	size;
	t_env	*current;

	size = 0;
	current = envp_copy;
	while (current->next)
	{
		size++;
		current = current->next;
	}
	return (size);
}

char	**set_envp_copy_arr(char **envp_copy_arr, t_env *envp_copy)
{
	size_t	i;
	size_t	len;
	t_env	*current;

	current = envp_copy;
	i = 0;
	while (current->next)
	{
		len = ft_strlen(current->key) + ft_strlen(current->value);
		envp_copy_arr[i] = ft_calloc(len + 2, sizeof(char *));
		ft_strlcat(envp_copy_arr[i], current->key, len + 2);
		ft_strlcat(envp_copy_arr[i], "=", len + 2);
		ft_strlcat(envp_copy_arr[i], current->value, len + 2);
		i++;
		current = current->next;
	}
	return (envp_copy_arr);
}

char	**get_envp_copy_arr(t_env *envp_copy)
{
	size_t	envp_copy_size;
	char	**envp_copy_arr;

	envp_copy_size = get_envp_copy_size(envp_copy);
	envp_copy_arr = ft_calloc(envp_copy_size + 1, sizeof(char *));
	envp_copy_arr = set_envp_copy_arr(envp_copy_arr, envp_copy);

	// size_t	to_print = 0;
	// while (envp_copy_arr[to_print])
	// {
	// 	printf("%s\n", envp_copy_arr[to_print]);
	// 	to_print++;
	// }
	return (envp_copy_arr);
}

char	*get_env(t_env *envp_copy, char *key) // key 값을 넣으면 환경변수를 반환 ex) get_env(envp, "USER") => "chukim"
{
	size_t	len;
	t_env	*current;

	current = envp_copy;
	while (current->next != NULL)
	{
		len = ft_strlen(key);
		if (len < ft_strlen(current->key))
			len = ft_strlen(current->key);
		if (ft_strncmp(current->key, key, len) == 0)
			return (ft_strdup(current->value));
		current = current -> next;
	}
	return (ft_strdup(""));
}

t_env	*env_new(void)
{
	t_env	*new;

	new = malloc(sizeof(t_env) * 1);
	if (new == NULL)
		return (NULL);
	new->key = NULL;
	new->value = NULL;
	new->next = NULL;
	return (new);
}

void	add_env(t_env *envp_copy, char *key, char *value)
{
	t_env	*current;

	current = envp_copy;
	while (current->next != NULL)
		current = current->next;
	current->key = key;
	current->value = value;
	current->next = env_new();
}

char	**divide_with_equal(char *str)
{
	int		i;
	int		j;
	int		k;
	char	**ret;

	i = 0;
	ret = ft_calloc(2, sizeof(char *));
	while (str[i] != '=' && str[i] != '\0')
		i++;
	ret[0] = ft_calloc(i + 1, sizeof(char));
	ret[1] = ft_calloc(ft_strlen(str) - i, sizeof(char));
	j = -1;
	while (++j < i)
		ret[0][j] = str[j];
	if (ft_strlen(str) - i == 0)
		{
			ret[1] = "\0";
			return (ret);
		}
	k = 0;
	while (str[j + 1] != '\0')
	{
		ret[1][k] = str[j + 1];
		j++;
		k++;
	}
	return (ret);
}

// '='으로 split 했을 경우! 문제 해결할 것인가?
t_env	*copy_envp(char *envp[])
{
	int		i;
	char	**ret;
	t_env	*envp_copy;

	envp_copy = env_new();
	i = 0;
	while (envp[i] != NULL)
	{
		ret = divide_with_equal(envp[i]);
		add_env(envp_copy, ret[0], ret[1]);
		free(ret);
		i++;
	}
	return (envp_copy);
}
