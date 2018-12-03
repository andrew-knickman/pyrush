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

//built-in command function declaratations
int pyrushCOC(char **args);
int pyrushHelp(char **args);
int pyrushQQQ(char **args);
int pyrushMoveTo(char **args);
int pyrushTime(char **args);

void printDirError(char **args)
{
	printf("\nNo such thing as '");
	for(int i=0; i < sizeof(args); i++)
	{
		if(args[i] != NULL)
		{
			if(i < (sizeof(args) - 1) && args[i+1] == NULL)
				printf("%s", args[i]);
			else
				printf("%s ", args[i]);
		}
	}
	printf("' exists in this world \n(could not find command, file, or directory)\n");
}
char *pyrushBINS[] = {
  "CenterOnCell",	//cd equivalent, https://www.creationkit.com/index.php?title=CenterOnCell
  "Help",			//help equivalent, https://www.creationkit.com/index.php?title=Help
  "QuitGame",		//exit equivalent, https://www.creationkit.com/index.php?title=QuitGame
  "MoveTo",			//file transfer, https://www.creationkit.com/index.php?title=MoveTo
  "GetCurrentTime"	//date equivalent, https://www.creationkit.com/index.php?title=GetCurrentTime
};
//built in commands
int (*pyrushBinCMDSs[]) (char **) = 
{
  &pyrushCOC,
  &pyrushHelp,
  &pyrushQQQ
};
int pyrush_num_bins() 
{
  	return sizeof(pyrushBINS) / sizeof(char *);
}
int pyrushCOC(char **args)
{
	if (args[1] == NULL) 
	{
		fprintf(stderr, "\nNo cell passed...");
	} 
	else 
	{
		if (chdir(args[1]) != 0) 
		{
			printDirError(args);
		}
	}
	return 1;
}
int pyrushHelp(char **args)
{
	char *link = "https://www.creationkit.com/index.php?title=Category:Console_Commands";
	int i;
	printf("\nPYRUSH Help\n");
	printf("\nEnter commands or programs and their arguments with Papyrus syntax");
	printf("\nFor list of applicable command formats, see\n\t%s", link);
	printf("\nThe following commands built in:\n");

	for (i = 0; i < pyrush_num_bins(); i++) 
	{
		printf("  %s\n", pyrushBINS[i]);
	}
	return 1;
}
int pyrushQQQ(char **args)
{
	printf("%s, you are now leaving the Pyrush world.", username);
  	return 0;
}


//***********************EDITS END HERE


//run a program
int pyrush_run(char **args)
{
	//printf("\nI got to run"); used in testing
	pid_t pid;
	int run;

	pid = fork();
	if(pid == 0)
	{
		if(execvp(args[0],args) == -1)
		{
			//perror("Pyrush");
			printDirError(args);
		}
		exit(EXIT_FAILURE);
	}
	else if(pid < 0)
	{
		//perror("Pyrush X");
		printDirError(args);

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
	//printf("\nI got to EXEC"); used in testing
	int run = 0;
	int i;

	//check for no input
	if(args[0] == NULL)
	{
		printf("\n%s did not declare a move...", username);
		run = 1;
		return run;
	}
	for(i = 0; i < sizeof(pyrushBINS) / sizeof(char *); i++)
	{
		if(strcmp(args[0], pyrushBINS[i])==0)
			return (*pyrushBinCMDSs[i])(args);
	}

	return pyrush_run(args);
}
//tokenize pyrush input
char **pyrush_getTKN(char *cmd)
{
	//printf("\nI got to getTKN"); used in testing

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
	//printf("\nI got to getCMD\n"); used for testing
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
	printf("\n%s, you are ", username);
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
	printf("\nWelcome to Pyrush, %s. Enter Help for a list of commands.\n", username);
	sleep(1);

	pyrush_loop();
}
int main(int argc, char **argv)
{
	//initialize pyrush
	pyrush_init();

	return EXIT_SUCCESS;
}
