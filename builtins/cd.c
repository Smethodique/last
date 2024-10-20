
#include "../minishell.h"

static void update_env_variable(char **env, const char *var_name, const char *new_value)
{
    int i;
    char *new_entry;

    i = 0;
    while (env[i])
    {
        if (ft_strncmp(env[i], var_name, ft_strlen(var_name)) == 0)
        {
            new_entry = ft_strjoin((char *)var_name, (char *)new_value);
            env[i] = new_entry;
            return;
        }
        i++;
    }

    // If the variable doesn't exist, add it
    new_entry = ft_strjoin((char *)var_name, (char *)new_value);
    env = ft_realloc(env, (i + 2) * sizeof(char *));
    env[i] = new_entry;
    env[i + 1] = NULL;
}

static void handle_cd_dash(char **env)
{
    char *oldpwd;

    oldpwd = get_env_value("OLDPWD", env);
    if (oldpwd)
    {
        if (chdir(oldpwd) == 0)
        {
            ft_putendl_fd(oldpwd, STDOUT_FILENO);
        }
        else
        {
            ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
            ft_putstr_fd(oldpwd, STDERR_FILENO);
            ft_putendl_fd(": No such file or directory", STDERR_FILENO);
        }
        free(oldpwd);
    }
    else
    {
        ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
    }
}

static void handle_cd_home(char **env)
{
    char *home;

    home = get_env_value("HOME", env);
    if (home)
    {
        if (chdir(home) != 0)
        {
            ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
            ft_putstr_fd(home, STDERR_FILENO);
            ft_putendl_fd(": No such file or directory", STDERR_FILENO);
        }
    }
    else
    {
        ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
    }
}

void cd(t_command *cmd, char **env)
{
    char *old_pwd;
    char *new_pwd;

    old_pwd = getcwd(NULL, 0);
    if (!old_pwd)
    {
        perror("minishell: cd");
        return;
    }

    if (!cmd->args[1] || ft_strcmp(cmd->args[1], "~") == 0)
    {
        handle_cd_home(env);
    }
    else if (ft_strcmp(cmd->args[1], "-") == 0)
    {
        handle_cd_dash(env);
    }
    else if (chdir(cmd->args[1]) != 0)
    {
        ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
        ft_putstr_fd(cmd->args[1], STDERR_FILENO);
        ft_putendl_fd(": No such file or directory", STDERR_FILENO);
        free(old_pwd);
        return;
    }

    new_pwd = getcwd(NULL, 0);
    if (!new_pwd)
    {
        perror("minishell: cd");
        free(old_pwd);
        return;
    }

    update_env_variable(env, "OLDPWD=", old_pwd);
    update_env_variable(env, "PWD=", new_pwd);

    free(old_pwd);
    free(new_pwd);
}
