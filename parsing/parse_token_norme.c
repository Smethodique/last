#include "../minishell.h"

void	parse_token_one(t_parse_context *ctx, t_token **tokens)
{
	if ((*tokens)->type == COMMANDE || (*tokens)->type == ARG)
	{
		if (!ctx->current_command)
		{
			ctx->current_command = new_command();
			add_command(&ctx->command_list, ctx->current_command);
		}
		add_argument(ctx->current_command, (*tokens)->value);
	}
}

void	parse_token_two(t_parse_context *ctx, t_token **tokens)
{
	if ((*tokens)->type == ENV_VAR)
	{
		if (!ctx->current_command)
		{
			ctx->current_command = new_command();
			add_command(&ctx->command_list, ctx->current_command);
		}
		ctx->env_value = get_env_value((*tokens)->value + 1, g_vars.env);
		// Skip the '$'
		if (ctx->env_value)
			add_argument(ctx->current_command, ctx->env_value);
		else
			add_argument(ctx->current_command, "");
	}
}

void	parse_token_three(t_parse_context *ctx, t_token **tokens)
{
	if ((*tokens)->type == INPUT || (*tokens)->type == OUTPUT
		|| (*tokens)->type == APPEND)
	{
		if (!ctx->current_command)
		{
			ctx->current_command = new_command();
			add_command(&ctx->command_list, ctx->current_command);
		}
		if (!(*tokens)->next || ((*tokens)->next->type != FILENAME
				&& (*tokens)->next->type != ARG))
		{
			ft_putstr_fd("Error: Missing filename after redirection\n", 2);
			return ;
		}
		add_redirection(ctx->current_command, (*tokens)->type,
				(*tokens)->next->value);
		*tokens = (*tokens)->next; // Skip the filename token
	}
}


void	parse_token_four(t_parse_context *ctx, t_token **tokens)
{
	if ((*tokens)->type == HEREDOC)
	{
		if (!ctx->current_command)
		{
			ctx->current_command = new_command();
			add_command(&ctx->command_list, ctx->current_command);
		}
		if (!(*tokens)->next || (*tokens)->next->type != DELIMITER)
			return ;
		ctx->heredoc_content = handle_heredoc((*tokens)->next->value, 1);
		ft_strcpy(ctx->temp_filename, "/tmp/minishell_heredocXXXXXX");
		
		ctx->fd = my_mkstemp(ctx->temp_filename);
		if(g_vars.heredoc_interrupted)
			reset_signals();
		if (ctx->heredoc_content)
		{
			write(ctx->fd, ctx->heredoc_content,
					ft_strlen(ctx->heredoc_content));
			free(ctx->heredoc_content);
		}
		close(ctx->fd);
		add_redirection(ctx->current_command, HEREDOC, ctx->temp_filename);
		*tokens = (*tokens)->next;
	}
}

void	parse_token_five(t_parse_context *ctx, t_token **tokens)
{
	if ((*tokens)->type == PIPE)
	{
		if (!ctx->current_command)
		{
			ft_putstr_fd("Error: No valid commands or redirections found\n", 2);
			return ;
		}
		ctx->current_command->pipe_next = 1;
		ctx->current_command = NULL;
		// Check the next token after the pipe
		if ((*tokens)->next && (*tokens)->next->type != INPUT
			&& (*tokens)->next->type != OUTPUT &&
			(*tokens)->next->type != APPEND && (*tokens)->next->type != HEREDOC)
			(*tokens)->next->type = COMMANDE; // Set the type to COMMANDE
	}
}
