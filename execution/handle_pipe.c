#include "../minishell.h"

void	setup_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
}

static void setup_redirections(t_command *cmd, int in_fd, int out_fd)
{
    int red_in = get_in(cmd, in_fd);
    int red_out = get_out(cmd, out_fd);

    if (red_in != STDIN_FILENO)
    {
        dup2(red_in, STDIN_FILENO);
        close(red_in);
    }
    else if (in_fd != STDIN_FILENO)
    {
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }

    if (red_out != STDOUT_FILENO)
    {
        dup2(red_out, STDOUT_FILENO);
        close(red_out);
    }
    else if (out_fd != STDOUT_FILENO)
    {
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }
}

static void close_unused_fds(int red_in, int red_out, int in_fd, int out_fd)
{
    int fd = 3;
    while (fd < 256)
    {
        if (fd != red_in && fd != red_out && fd != in_fd && fd != out_fd)
        {
            close(fd);
        }
        fd++;
    }
}

static void execute_command(t_command *cmd, char **env)
{
    if (is_builtin(cmd) != NOT_BUILT_IN)
    {
        execute_builtin(cmd, env, is_builtin(cmd));
        exit(g_vars.exit_status);
    }
    else
    {
        cmd->args[0] = get_path(cmd->args);
        if (!cmd->args[0])
        {
            fprintf(stderr, "minishell: command not found: %s\n", cmd->args[0]);
            exit(127);
        }
        if (execve(cmd->args[0], cmd->args, env) == -1)
        {
            perror("minishell: execution failed");
            exit(127);
        }
    }
}

pid_t execute_piped_command(t_command *cmd, int in_fd, int out_fd, char **env)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork failed");
        exit(1);
    }
    if (pid == 0)
    {
        setup_child_signals();
        setup_redirections(cmd, in_fd, out_fd);
        close_unused_fds(get_in(cmd, in_fd), get_out(cmd, out_fd), in_fd, out_fd);
        execute_command(cmd, env);
    }
    return pid;
}

static int count_pipes(t_command *commands)
{
    int pipe_count = 0;
    t_command *current = commands;
    while (current)
    {
        pipe_count++;
        current = current->next;
    }
    return pipe_count;
}

static void setup_pipe(int pipes[2][2], int i)
{
    if (pipe(pipes[i % 2]) == -1)
    {
        perror("pipe failed");
        exit(1);
    }
}

static void determine_fds(int *in_fd, int *out_fd, int in_pipe, int out_pipe)
{
	if (in_pipe != -1)
		*in_fd = in_pipe;
	else
		*in_fd = STDIN_FILENO;
	if (out_pipe != -1)
		*out_fd = out_pipe;
	else
		*out_fd = STDOUT_FILENO;
}

static void close_pipe_fds(int i, t_command *current, int pipes[2][2])
{
    if (i > 0)
        close(pipes[(i + 1) % 2][0]);
    if (current->next)
        close(pipes[i % 2][1]);
}

static void wait_for_children(pid_t *pids, int pipe_count)
{
    int i = 0;
    int status;
    while (i < pipe_count)
    {
        waitpid(pids[i], &status, 0);
        if (i == pipe_count - 1)
        {
            if (WIFEXITED(status))
                g_vars.exit_status = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                g_vars.exit_status = 128 + WTERMSIG(status);
        }
        i++;
    }
}

void handle_pipes(t_command *commands, char **env)
{
    int pipe_count = count_pipes(commands);
    pid_t *pids = malloc(sizeof(pid_t) * pipe_count);
    if (!pids)
        return;
    int pipes[2][2];
    int i = 0;
    t_command *current = commands;
    while (current)
    {
        int in_fd, out_fd;
        if (current->next)
            setup_pipe(pipes, i);
        determine_fds(&in_fd, &out_fd, pipes[(i + 1) % 2][0], pipes[i % 2][1]);
        pids[i] = execute_piped_command(current, in_fd, out_fd, env);
        close_pipe_fds(i, current, pipes);

        current = current->next;
        i++;
    }
    wait_for_children(pids, pipe_count);
    free(pids);
}
