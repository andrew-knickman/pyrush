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


//***********************EDITS START HERE

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
char *pyrush_bins[] = {
  "cd",
  "help",
  "exit"
};

int (*pyrush_bin_cmds[]) (char **) = {
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


//***********************EDITS END HERE


//run a program
int pyrush_run(char **args)
{
	printf("\nI got to run");
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

//execute a built-in command or run a program
int pyrush_EXEC(char **args)
{
	printf("\nI got to EXEC");
	int run = 0;
	int i;

	//check for no input
	if(args[0] == NULL)
	{
		printf("\n%s did not declare a move...", username);
		run = 1;
		return run;
	}
	for(i = 0; i < sizeof(pyrush_bins) / sizeof(char *); i++)
	{
		if(strcmp(args[0], pyrush_bins[i])==0)
			return (*pryush_bin_cmds[i])(args);
	}

	return pyrush_run(args);
}
//tokenize pyrush input
char **pyrush_getTKN(char *cmd)
{
	printf("\nI got to getTKN");

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
char *pyrush_getCMD(void)
{
	printf("\nI got to getCMD\n");
	//allocate buffer memory
	char *bf = malloc(sizeof(char) * PYRUSHBFSZ);;
	//reset position
	int pos = 0;
	//character to read
	int c;

	do
	{
		//error if memory not allocated or reallocated
		if(!bf)
		{
		fprintf(stderr, "\nUnable to allocate memory...");
		exit(EXIT_FAILURE);
		}

		//read characters
		c = getchar();

		if(c == EOF)
		{
			exit(EXIT_SUCCESS);
		}
		else if(c == '\n')
		{
			bf[pos] = '\0';
			return bf;
		}
		else
		{
			bf[pos] = c;
		}

		pos++;

		if (pos >= PYRUSHBFSZ) 
		{
			bf = realloc(bf, PYRUSHBFSZ+1024);
		}
	}while(1);
}

void printDir()
{
	char cwd[PYRUSHBFSZ];
	getcwd(cwd, sizeof(cwd));
	int loc = 0;
	char *loclist[6]; //list of location indicators
	loclist[0] = "In the world of";
	loclist[1] = "in the land of";
	loclist[2] = "in the nation of";
	loclist[3] = "in the province of";
	loclist[4] = "in the town of";
	loclist[5] = "in the place of";
	for(char* p = cwd; p = strchr(p, '/'); p++)
	{
		switch(loc)
		{
			case 0:
			{
				*p = *loclist[0];
				loc++;
				break;
			}
			case 1:
			{				
				*p = *loclist[1];
				loc++;
				break;
			}
			case 2:
			{
				*p = *loclist[2];
				loc++;
				break;
			}
			case 3:
			{
				*p = *loclist[3];
				loc++;
				break;
			}
			case 4:
			{
				*p = *loclist[4];
				loc++;
				break;
			}
			default:
			{
				*p = *loclist[5];
				loc++;
				break;
			}
		}
	}
	printf("\nYou are ");
	printf("%s", cwd);
}

//run the pyrush loop
void pyrush_loop(void)
{
	char *cmd; //line of instruction
	char **args;
	int run;
	do
	{
		printDir();
		printf("\n%s, what's your next move?\n", username);
		cmd = pyrush_getCMD();
		args = pyrush_getTKN(cmd);
		run = pyrush_EXEC(args);

		free(cmd);
		free(args);
	}while(run);

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
	
	printf("\n%s is exploring the Pyrush System-to-Game World", username);
	printf("\nWelcome to Pyrush, %s\n", username);
	sleep(1);

	pyrush_loop();
}
int main(int argc, char **argv)
{
	//initialize pyrush
	pyrush_init();

	return EXIT_SUCCESS;
}
