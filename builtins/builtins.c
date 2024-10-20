#include "../minishell.h"
void handle_cmd_exit(t_command *cmd)
{
	if (cmd->arg_count > 2)
	{
		ft_putstr_fd("exit\n", 2);
		ft_putstr_fd("exit: too many arguments\n", 2);
		g_vars.exit_status = 1;
		return ;
	}
}void	ft_exit(t_command *cmd)
{
	int	i;
	int	status;

	i = 0;
	status = 0;
      handle_cmd_exit(cmd);
	if (cmd->arg_count == 2)
	{
		while (cmd->args[1][i])
		{
			if (!ft_isdigit(cmd->args[1][i]))
			{
				ft_putstr_fd("exit\n", 2);
				ft_putstr_fd(cmd->args[1], 2);
				g_vars.exit_status = 255;
				return ;
			}
			i++;
		}
		status = ft_atoi(cmd->args[1]);
		g_vars.exit_status = status;
	}
	ft_putstr_fd("exit\n", 2);
	exit(g_vars.exit_status);
}

void	execute_builtin(t_command *cmd, char **environment, int index)
{
	if (index == 0)
		cd(cmd, environment);
	else if (index == 1)
		env(cmd);
	else if (index == 2)
		export(cmd);
	else if (index == 3)
		unset(cmd);
	else if (index == 4)
		echo(cmd, environment);
	else if (index == 5)
		pwd(cmd, environment);
	else if (index == 6)
		ft_exit(cmd);
	   else 
		g_vars.exit_status = 0;
}

int	is_builtin(t_command *cmd)
{
	char *built_in[] = {
		"cd", "env", "export", "unset", "echo", "pwd", "exit"};
	int num_builtins = sizeof(built_in) / sizeof(built_in[0]);
	int i;

	i = 0;
	while (i < num_builtins)
	{
		if (ft_strncmp(cmd->args[0], built_in[i], ft_strlen(built_in[i]) + 1) == 0)
		{
			return i;
		}
		i++;
	}
	return (-1);
}
