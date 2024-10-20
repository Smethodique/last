#include "../minishell.h"

static int	*exit_code(void)
{
	static int	c;

	return (&c);
}

int	ft_getter(void)
{
	int	*i;

	i = exit_code();
	return (*i);
}

void	ft_setter(int value)
{
	int	*i;

	i = exit_code();
	*i = value;
}

int	double_pointer_len(char **str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

void	increment_shlvl(char **env)
{
	int	i;
	int	shlvl;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "SHLVL=", 6) == 0)
		{
			shlvl = atoi(env[i] + 6);
			shlvl++;
			  while (env[i])
			  {
				  if(strncmp(env[i], "SHLVL=", 6) == 0)
				  {
					  env[i] = ft_strjoin("SHLVL=", ft_itoa(shlvl));
					  break ;
				  }
			  }
			return ;
		}
		i++;
	}
}
void	add_to_env(char ***env, char *new_var)
{
	int	len;
	int	i;

	len = double_pointer_len(*env);
	char **new_env = malloc(sizeof(char *) * (len + 2));
		// Allocate space for the new variable and the NULL terminator
	if (new_env)
	{
		i = 0;
		while (i < len)
		{
			new_env[i] = (*env)[i];
			i++;
		}
		new_env[i] = new_var;
		new_env[i + 1] = NULL;
		free(*env);
		*env = new_env;
	}
}
void print_env(void)
{
	int	j;
j = 0;
		while (g_vars.env[j])
		{
			if (strcmp(g_vars.env[j],
					"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin") == 0)
			{
				j++;
				continue ;
			}
			printf("%s\n", g_vars.env[j]);
			j++;
		}
}		
void	free_env(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

void	env(t_command *cmd)
{
	int		i;
	char	**new_env;

	i = 1;
	new_env = NULL;
	if (cmd->args && cmd->args[1] && ft_strcmp(cmd->args[1], "-i") == 0)
		i++;
	if ( g_vars.env == NULL || g_vars.env[0] == NULL)
		new_env = create_env();
	else
		new_env = g_vars.env;
	while (cmd->args[i] && ft_strchr(cmd->args[i], '='))
		add_to_env(&new_env, cmd->args[i++]);
	g_vars.env = new_env;
	if (cmd->args[i])
		execute_cmd(&cmd->args[i]);
	else
	{
		print_env();
		ft_setter(0);
	}
	if ( g_vars.env == NULL || g_vars.env[0] == NULL)
	   create_env();
	ft_setter(0);
}

size_t	length(char *s)
{
	size_t	i;

	i = 0;
	while (s[i] && s[i] != '=')
	{
		i++;
	}
	return (i);
}

int	check_dup_env(char *cmd)
{
	int	i;

	i = 0;
	while (g_vars.env[i])
	{
		if (!ft_strncmp(g_vars.env[i], cmd, length(g_vars.env[i])))
			return (i);
		else
			i++;
	}
	return (0);
}

void	unset_helper(char *cmd, int len)
{
	int	checker;
	int	i;

	checker = check_dup_env(cmd);
	if (checker && (length(g_vars.env[checker]) == length(cmd)))
	{
		i = checker;
		while (i < len - 1)
		{
			g_vars.env[i] = g_vars.env[i + 1];
			i++;
		}
		g_vars.env[len - 1] = NULL;
	}
}

void	unset(t_command *cmd)
{
	int i = 1;
	int len;
	if (cmd->args[1])
	{
		while (cmd->args[i])
		{
			len = 0;
			while (g_vars.env[len])
				len++;
			unset_helper(cmd->args[i], len);
			i++;
		}
	}
}