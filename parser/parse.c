/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: junkpark <junkpark@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 14:31:28 by chukim            #+#    #+#             */
/*   Updated: 2022/07/27 20:56:13 by junkpark         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		is_valid_quote(char *input)
{
	int	squote;
	int	dquote;

	squote = 0;
	dquote = 0;
	while (*input)
	{
		if (*input == '\'' && (squote == 0 && dquote == 0))
			squote = 1;
		else if (*input == '\"' && (squote == 0 && dquote == 0))
			dquote = 1;
		else if (*input == '\'' && squote == 1)
			squote = 0;
		else if (*input == '\"' && dquote == 1)
			dquote = 0;
		input++;
	}
	if (squote == 1 || dquote == 1)
		return (0);
	return (1);
}

void	init_in_quote(char *input, char *in_quote)
{
	int	squote;
	int	dquote;

	squote = 0;
	dquote = 0;
	while (*input)
	{
		if (((*input == '\'' && *(input + 1) == '\'')
			|| (*input == '\"' && *(input + 1) == '\"'))
			&& (squote == 0 && dquote == 0))
		{
			*in_quote = 3;
			*(in_quote + 1) = 3;
			*input = ' ';
			*(input + 1) = ' ';
		}
		else if (*input == '\'' && (squote == 0 && dquote == 0))
		{
			squote = 1;
			*input = ' ';
		}
		else if (*input == '\"' && (squote == 0 && dquote == 0))
		{
			dquote = 1;
			*input = ' ';
		}
		else if (*input == '\'' && squote == 1)
		{
			squote = 0;
			*input = ' ';
		}
		else if (*input == '\"' && dquote == 1)
		{
			dquote = 0;
			*input = ' ';
		}
		else if (squote == 1)
			*in_quote = 1;
		else if (dquote == 1)
			*in_quote = 2;
		else
			*in_quote = 0;
		input++;
		in_quote++;
	}
}

size_t	get_token_size(char *input, char *is_in_quote)
{
	size_t	ret;
	size_t	i;

	ret = 0;
	while (*input)
	{
		while (ft_isspace(*input) && !*is_in_quote)
		{
			input++;
			is_in_quote++;
		}
		i = 0;
		while (input[i] && is_in_quote[i])
			i++;
		if (i == 0)
		{
			while (input[i] && !is_in_quote[i] && !ft_isspace(input[i]))
				i++;
		}
		if (i != 0)
		{
			input += i;
			is_in_quote += i;
			ret += 1;
		}
	}
	return (ret);
}

// strndup 실패 시 ft_split처럼 free 해주고 return (NULL) 해줘야함,,
t_token	*get_token(char *input, char *in_quote)
{
	t_token	*token;
	size_t	token_size;
	size_t	i;
	size_t	j;

	j = 0;
	token_size = get_token_size(input, in_quote);
	token = ft_calloc(sizeof(t_token),  token_size + 1);
	if (token == NULL)
		return (NULL);
	while (*input)
	{
		if (in_quote[i] == 3)
		{
			token[j].str = ft_strdup("");
			i += 1;
			j += 1;
			input += i;
			in_quote += i;
			continue ;
		}
		while (ft_isspace(*input) && !*in_quote)
		{
			input++;
			in_quote++;
		}
		i = 0;
		while (input[i] && in_quote[i] && in_quote[i] != 3)
			i++;
		if (i != 0)
		{
			token[j].str = ft_strndup(input, i);
			token[j].type = *in_quote;
			j++;
			input += i;
			in_quote += i;
		}
		i = 0;
		while (input[i] && !in_quote[i] && !ft_isspace(input[i]))
			i++;
		if (i != 0)
		{
			token[j].str = ft_strndup(input, i);
			token[j].type = T_NULL;
			j++;
			input += i;
			in_quote += i;
		}
	}

	token[token_size].str = NULL;
	token[token_size].type = T_NULL;
	return (token);
}

t_token	*label_token(t_token *token)
{
	size_t	i;

	i = 0;
	while (token[i].str)
	{
		if (token[i].type == T_NULL)
		{
			if (!ft_strncmp(token[i].str, "|", 2))
				token[i].type = T_PIPE;
			else if (!ft_strncmp(token[i].str, "<", 2))
				token[i].type = T_REDIRECT;
			else if (!ft_strncmp(token[i].str, "<<", 3))
				token[i].type = T_REDIRECT;
			else if (!ft_strncmp(token[i].str, ">", 2))
				token[i].type = T_REDIRECT;
			else if (!ft_strncmp(token[i].str, ">>", 3))
				token[i].type = T_REDIRECT;
			else if (!ft_strncmp(token[i].str, "||", 3))
				token[i].type = T_ERROR;
			else if (!ft_strncmp(token[i].str, "&&", 3))
				token[i].type = T_ERROR;
			else if (!ft_strncmp(token[i].str, "&", 2))
				token[i].type = T_ERROR;
			else if (!ft_strncmp(token[i].str, ";", 2))
				token[i].type = T_ERROR;
			else if (!ft_strncmp(token[i].str, ";;", 3))
				token[i].type = T_ERROR;
			else
				token[i].type = T_WORD;
		}
		i++;
	}
	return (token);
}

// get_token 실패 시, return (NULL);
t_token	*lexcial_analysis(char *input, char *in_quote)
{
	t_token *token;

	token = get_token(input, in_quote);
	token = label_token(token);
	// size_t	i = 0;
	// while (token[i].str != NULL)
	// {
	// 	printf("token[%zu].str is [%s] & token[%zu].type is [%d]\n", i, token[i].str, i, token[i].type);
	// 	i++;
	// }
	return (token);
}


void	init_expanded(char *input, char *in_quote, char *expanded_input, char *expanded_in_quote)
{
	while (*input)
	{
		if (*in_quote)
		{
			*expanded_input = *input;
			*expanded_in_quote = *in_quote;
		}
		else
		{
			if (*input == '>' || *input == '<' || *input == '|' || *input == ';' || *input == '&')
			{
				*expanded_input = ' ';
				expanded_input += 1;
				expanded_in_quote += 1;
				*expanded_input = *input;
				if (*(input + 1) == *(input))
				{
					input += 1;
					in_quote += 1;
					expanded_input += 1;
					expanded_in_quote += 1;
					*expanded_input = *input;
				}
				expanded_input += 1;
				expanded_in_quote += 1;
				*expanded_input = ' ';
			}
			else
				*expanded_input = *input;
		}
		input += 1;
		in_quote += 1;
		expanded_input += 1;
		expanded_in_quote += 1;
		*expanded_input = '\0';
	}
}

// 시간 복잡도 엄청 남, 수정 필요!
t_token	*env_analysis(t_token *token, t_env *envp_copy)
{
	size_t	i;
	size_t	j;
	size_t	k;
	char	*env;
	char	*str;
	char	*tmp;
	char	*key;

	i = 0;
	while (token[i].str)
	{
		if (token[i].type == T_DQUOTES)
			token[i].type = T_WORD;
		if (token[i].type == T_WORD)
		{
			k = 0;
			str = ft_strdup("");
			while (token[i].str[k])
			{
				j = 0;
				if (token[i].str[j + k] == '$')
				{
					token[i].str[j + k] = '\0';
					tmp = str;
					str = ft_strjoin(str, &(token[i].str[k]));
					free(tmp);
					j++;
					if (token[i].str[k + j] == '?')
					{
						env = ft_itoa(g_errno);
						tmp = str;
						str = ft_strjoin(str, env);
						free(tmp);
						free(env);
						j++;
					}
					else if (ft_isdigit(token[i].str[k + j]))
					{
						while (ft_isdigit(token[i].str[k + j]))
							j++;
					}
					else
					{
						while (!ft_isspace(token[i].str[k + j]) && (ft_isalnum(token[i].str[k + j]) || token[i].str[k + j] == '_'))
							j++;
						if (j == 1)
						{
							tmp = str;
							str = ft_strjoin(str, "$");
							free(tmp);
						}
						else
						{
							key = ft_strndup(&(token[i].str[k + 1]), j - 1);
							env = get_env(envp_copy, key);
							tmp = str;
							str = ft_strjoin(str, env);
							free(key);
							free(env);
							free(tmp);
						}
					}
				}
				else
				{
					while (token[i].str[k + j] && token[i].str[k + j] != '$')
						j++;
					if (token[i].str[k + j] == '$')
					{
						token[i].str[k + j] = '\0';
						tmp = str;
						str = ft_strjoin(str, &(token[i].str[k]));
						free(tmp);
						token[i].str[k + j] = '$';
					}
					else
					{
						tmp = str;
						str = ft_strjoin(str, &(token[i].str[k]));
						free(tmp);
					}
				}
				k += j;
			}
			free(token[i].str);
			token[i].str = str;
		}
		else if (token[i].type == T_SQUOTES)
			token[i].type = T_WORD;
		i++;
	}
	return (token);
}

t_token	*syntax_analysis(t_token *token)
{
	size_t	i;

	i = 0;
	while (token[i].str)
	{
		if (token[i].type == T_PIPE)
		{
			if (token[i + 1].type != T_WORD
				&& token[i + 1].type != T_REDIRECT)
				token[i].type = T_ERROR;
		}
		else if (token[i].type == T_REDIRECT)
		{
			if (token[i + 1].type != T_WORD)
				token[i].type = T_ERROR;
			else if (token[i + 1].type != T_NULL)
				token[i + 1].type = T_FILE;
		}
		// if (token[i].type == T_WORD)
		// 	printf("word\t|%s|\n", token[i].str);
		// else if (token[i].type == T_FILE)
		// 	printf("file\t|%s|\n", token[i].str);
		// else if (token[i].type == T_ERROR)
		// 	printf("error\t|%s|\n", token[i].str);
		// else if (token[i].type == T_PIPE)
		// 	printf("pipe\t|%s|\n", token[i].str);
		i++;
	}
	return (token);
}

int	is_token_error(t_token *token)
{
	size_t	i;

	i = 0;
	while (token[i].str)
	{
		if (token[i].type == T_ERROR)
			return (1);
		i++;
	}
	return (0);
}

// expanded, in_quote 할당 안될 경우 예외처리
t_token	*parse(char *input, t_env *envp_copy)
{
	char	*in_quote;
	char	*expanded_input;
	char	*expanded_in_quote;
	t_token	*token;

	if (!is_valid_quote(input))
	{
		g_errno = -1;
		return (NULL);
	}
	in_quote = ft_calloc(ft_strlen(input) + 1, 1);
	expanded_input = ft_calloc(ft_strlen(input) * 3 + 1, 1);
	expanded_in_quote = ft_calloc(ft_strlen(input) * 3 + 1, 1);
	init_in_quote(input, in_quote);
	init_expanded(input, in_quote, expanded_input, expanded_in_quote);
	token = lexcial_analysis(expanded_input, expanded_in_quote);
	token = env_analysis(token, envp_copy);
	token = syntax_analysis(token);
	if (is_token_error(token))
	{
		free_token(&token);
		token = NULL;
		g_errno = -1;
	}
	free(expanded_in_quote);
	free(expanded_input);
	free(in_quote);
	return (token);
}
