#include "../minishell.h"

char	**ft_free(char **str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
	return (NULL);
}

char	*check_path(char **cmd, char **path)
{
	int		i;
	char	*new_cmd;
	char	*new_path;

	i = 0;
	while (path[i])
	{
		new_cmd = ft_strjoin("/", cmd[0]);
		new_path = ft_strjoin(path[i], new_cmd);
		free(new_cmd);
		if (access(new_path, F_OK) == 0)
		{
			ft_free(path);
			return (new_path);
		}
		free(new_path);
		i++;
	}
	ft_free(path);
	return (ft_strdup(cmd[0]));
}
char *plo(char **cmd)
{
		if (access(cmd[0], F_OK) == 0)
			return (ft_strdup(cmd[0]));
		else
			return (NULL);
}
char	*get_path(char **cmd)
{
	int		i;
	char	**path;
	char	*env_path;

	if (cmd[0][0] == '/' || (cmd[0][0] == '.' && cmd[0][1] == '/'))
	{
		return (plo(cmd));
	}
	env_path = NULL;
	i = 0;
	while (g_vars.env[i])
	{
		if (ft_strncmp(g_vars.env[i], "PATH=", 5) == 0)
		{
			env_path = g_vars.env[i] + 5;
			break ;
		}
		i++;
	}
	if (env_path == NULL || *env_path == '\0')
		return (NULL);
	path = ft_splitD(env_path, ":");
	return (check_path(cmd, path));
}



static void execute_child(char **cmd, char *path)
{
    if (execve(path, cmd, g_vars.env) == -1)
    {
		ft_putstr_fd("minishell: command not found : ", 2);
		 printf("%s\n", cmd[0]);
        exit(127);
    }
}

// Main function to handle command execution
void execute_cmd(char **cmd)
{
    pid_t pid;
    char *path;
    int status;

    path = get_path(cmd);
    if (path == NULL)
    {
        printf("minishell: %s: command not found\n", cmd[0]);
        g_vars.exit_status = 127;
        return;
	}
	pid = fork();
	if (pid == 0)
		execute_child(cmd, path);
	else if (pid < 0)
	{
		printf("minishell: %s: %s\n", cmd[0], strerror(errno));
		g_vars.exit_status = 1;
	}
	else
	{
		waitpid(pid, &status, 0);
		g_vars.exit_status = WEXITSTATUS(status);
	}
	free(path);
}

void	exec(t_command *cmd)
{
	char	**args;
	int		i;

	i = 0;
	args = malloc(sizeof(char *) * (double_pointer_len(cmd->args) + 1));
	if (!args)
		return ;
	while (cmd->args[i])
	{
		args[i] = ft_strdup(cmd->args[i]);
		i++;
	}
	args[i] = NULL;
	execute_cmd(args);
	ft_free(args);
}

void	execute_single_cmd(t_command *cmd, char **env)
{
	int cmd_type;

	if (cmd == NULL || cmd->args == NULL || cmd->args[0] == NULL)
		return ;
	all_signals();
	cmd_type = is_builtin(cmd);
	if (cmd->redirections)
		ft_redict(cmd, env);
	else
	{
		if (cmd_type == -1)
			exec(cmd);
		else
			execute_builtin(cmd, env, cmd_type);
	}
}