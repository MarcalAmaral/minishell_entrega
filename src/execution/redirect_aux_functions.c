/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_aux_functions.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 19:30:55 by marvin            #+#    #+#             */
/*   Updated: 2024/05/27 19:30:55 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_append(char **matrix_append, char *reference)
{
	int	index;

	index = 0;
	while (matrix_append[index])
	{
		if (ft_strcmp(matrix_append[index], reference) == 0)
		{
			free(reference);
			return (TRUE);
		}	
		index++;
	}
	free(reference);
	return (FALSE);
}

char	**creat_append_mat(t_dlist *tok, int matrix_size)
{
	int		redir_count;
	char	**matrix_append;
	int		index_matrix_append;

	index_matrix_append = 0;
	redir_count = 0;
	matrix_append = ft_calloc(sizeof(char *), (matrix_size + 1));
	while (tok->next != NULL)
	{
		if (tok->tok->type == R_OUT || tok->tok->type == APPEND)
			redir_count++;
		if (tok->tok->type == APPEND)
		{
			matrix_append[index_matrix_append] = ft_itoa(redir_count - 1);
			index_matrix_append++;
		}
		tok = tok->next;
	}
	return (matrix_append);
}

char	**have_append(t_dlist *tokens)
{
	int		i;
	t_dlist	*aux_t;
	char	**appends;

	i = 0;
	aux_t = tokens;
	appends = NULL;
	while (aux_t->next != NULL)
		aux_t = aux_t->next;
	while (aux_t->tok->type != PIPE && aux_t->prev != NULL)
	{
		if (aux_t->tok->type == APPEND)
			i++;
		aux_t = aux_t->prev;
	}
	if (i > 0)
		appends = creat_append_mat(aux_t, i);
	return (appends);
}

int	files_out_control(t_ast *root)
{
	int	i;

	i = 0;
	while (root && root->files[1][i] != NULL)
	{
		if (root->redir_fds[1] != 0)
			close(root->redir_fds[1]);
		if (root->files[2] != NULL)
		{
			if (check_append(root->files[2], ft_itoa(i)) > 0)
				root->redir_fds[1] = open(root->files[1][i],
						O_WRONLY | O_CREAT | O_APPEND, 0000666);
			else
				root->redir_fds[1] = open(root->files[1][i],
						O_WRONLY | O_CREAT | O_TRUNC, 0000666);
		}
		else
			root->redir_fds[1] = open(root->files[1][i],
					O_WRONLY | O_CREAT | O_TRUNC, 0000666);
		if (redirect_out_error(root->files[3], root->files[1][i], i))
			return (EXIT_FAILURE);
		i++;
	}
	return (EXIT_SUCCESS);
}

int	files_in_control(t_ast *root)
{
	int	i;

	i = 0;
	while (root->files[0][i] != NULL)
	{
		if (redirect_in_error(root->files[3], root->files[0][i], i))
			return (EXIT_FAILURE);
		if (root->redir_fds[0] != 0)
			close(root->redir_fds[0]);
		root->redir_fds[0] = open(root->files[0][i], O_RDONLY);
		i++;
	}
	return (EXIT_SUCCESS);
}
