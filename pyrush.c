/*
PYRUSH Shell
COSC 439 Project
Pyrush Team: Andrew Knickman, Niyi Adjayi

Uses Papyrus syntax to pass commands to system
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h>
#include<readline/history.h>

#define PYRUSHBFSZ 2048
#define PYRUSHTKSZ 64
#define username getenv("USER")
#define clear() printf("\033[H\033[J") 
#define pyrushdelim " \n"

//run a program
int pyrush_run(char **args)
{
	pid_t pid;
	int run;

	pid = fork();
	if(pid == 0)
	{
		if(execvp(args[0],args) == -1)
		{
			perror("Pyrush");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid < 0)
	{
		perror("Pyrush");
	}
	else
	{
		do
		{
			waitpid(pid, &run, WUNTRACED);
		}while(!WIFEXITED(run) && !WIFSIGNALED(run));
	}
	return 1;
}

//EDITS START HERE

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};
int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}
int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}
int lsh_exit(char **args)
{
  return 0;
}


//EDITS END HERE


//execute a built-in command or run a program
int pyrush_EXEC(char **args)
{
	int run = 0;
	int i;

	//check for no input
	if(args[0] == NULL)
	{
		printf("%s did not declare a move...", username);
		run = 1;
		return run;
	}
	for(i = 0; i < lsh_num_builtins(); i++)
	{
		if(strcmp(args[0], builtin_str[i] ==0))
			return (*builtin_func[i])(args);
	}

	return pyrush_run(args);
}
//tokenize pyrush input
char **pyrush_getTKN(char *cmd)
{
	//allocate token parsing memory
	char **tknbf = malloc(sizeof(char*) * PYRUSHTKSZ);;
	//token pointer
	char *tkn;
	//reset position
	int pos = 0;

	tkn = strtok(cmd, pyrushdelim);
	
	do
	{
		//allocate memory
		if(!tknbf)
		{
		fprintf(stderr, "\nUnable to allocate memory...");
		exit(EXIT_FAILURE);
		}

		tknbf[pos] = tkn;
		pos++;

		tkn = strtok(NULL, pyrushdelim);
	}while(tkn != NULL);
	
	tknbf[pos] = NULL;
	return tknbf;
}

//read pyrush input
char *pyrush_getCMD()
{
	//allocate buffer memory
	char *bf = malloc(sizeof(char) * PYRUSHBFSZ);;
	//reset position
	int pos = 0;
	//character to read
	int c;

	do
	{
		//error if memory not allocated
		if(!bf)
		{
		fprintf(stderr, "\nUnable to allocate memory...");
		exit(EXIT_FAILURE);
		}

		//read characters
		c = getchar();
		if(c != EOF || c!= '\n')
		{
			bf[pos] = c;
			pos++;
		}
		else if(c == '\n')
		{
			bf[pos] = '\0';
			pos++;

		}
		else if(c == EOF)
		{
			add_history(bf);
			exit(EXIT_SUCCESS);
		}
	}while(1);
}

void printDir()
{
	char cwd[PYRUSHBFSZ];
	getcwd(cwd, sizeof(cwd));
	printf("\nIn the world of %s", cwd);
}

//run the pyrush loop
void pyrush_loop()
{
	char *cmd; //line of instruction
	char **args;
	int run;
	do
	{
		printDir();
		printf("\nWhat's your next move?");
		cmd = pyrush_getCMD();
		args = pyrush_getTKN(cmd);
		run = pyrush_EXEC(args);

		free(cmd);
		free(args);
	}while(run==1);

	printf("\n");
}
//initialize pyrush
void pyrush_init()
{
	clear();
	printf("\n----------PYRUSH: Papyrus-Based C Shell----------");
	printf("\n-Pyrush Team: Andrew Knickman and Adeniyi Adjayi-");
	printf("\n----Enter Help.Show() for a list of commands.----");
	printf("\n------All commands based on Papyrus Syntax-------");
	printf("\n-----------Shell program written in C------------");
	printf("\n-----For OOD devs learning operating systems-----");

	printf("\n");
	
	printf("\n%s (user) is exploring the Pyrush System-to-Game environment", username);
	printf("\nPlease wait while Pyrush is initialized...\n");
	sleep(10);

	pyrush_loop();
}
int main()
{
	//initialize pyrush
	pyrush_init();
}
