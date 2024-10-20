#include "../minishell.h"

//define cntrl c signal handler
void	sigint_handler(int sig)
{
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void reset_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	all_signals(void)
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
}

void	sigint_handlerh(int signum)
{
	(void)signum;
	g_vars.khbi = dup(0);
	close(0);
    g_vars.heredoc_interrupted = 1;
    
}
