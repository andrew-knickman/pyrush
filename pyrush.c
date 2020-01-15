/*
PYRUSH Shell
COSC 439 Project
Andrew Knickman

Uses pseudo-Papyrus syntax to pass commands to system
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h>
#include<readline/history.h>
#include<time.h>
#include<dirent.h>


#define PYRUSHBFSZ 4096
#define PYRUSHTKSZ 128
#define username getenv("USER")
#define clear() printf("\033[H\033[J") 
#define pyrushdelim " \n"

//built-in command function declaratations
int pyrushCOC(char **args);
int pyrushHelp(char **args);
int pyrushQQQ(char **args);
int pyrushMoveTo(char **args);
int pyrushTime(char **args);
int pyrushGetPlayer(char **args);
int pyrushGetLoc(char **args);
int pyrushGetCell(char **args);
int pyrushEquip(char **args);

void printError(char **args)
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
char* replaceWithLoc(char* str, char* a)
{
	int len  = strlen(str);
	int lena = strlen(a), lenb;
	char* loclist[6];
	loclist[0] = " in the world of ";
	loclist[1] = ", in the land of ";
	loclist[2] = ", in the nation of ";
	loclist[3] = ", in the province of ";
	loclist[4] = ", in the town of ";
	loclist[5] = ", in the place of ";
	int loc = 0;
	for (char* p = str; p = strstr(p, a); ++p) 
	{
		lenb = strlen(loclist[loc]);
		//shifts memory as needed
		if (lena != lenb)
		{
			memmove(p+lenb, p+lena,
				len - (p - str) + lenb+32);
		}
		memcpy(p, loclist[loc], lenb);
		if(loc != 5)
			loc++;
	}
	return str; 
}
void printDir()
{
	char cwd[PYRUSHBFSZ];
	getcwd(cwd, sizeof(cwd)*2);
	printf("\n%s, you are%s", username, replaceWithLoc(cwd, "/"));
}
char *pyrushBINS[] = {
	"Debug.CenterOnCell()",	//cd equivalent, https://www.creationkit.com/index.php?title=CenterOnCell
	"Help",			//help equivalent, https://www.creationkit.com/index.php?title=Help
  	"QuitGame",		//exit equivalent, https://www.creationkit.com/index.php?title=QuitGame
  	"Ref.MoveTo()",	//file transfer, https://www.creationkit.com/index.php?title=MoveTo
  	"GetCurrentTime",	//date equivalent, https://www.creationkit.com/index.php?title=GetCurrentTime
	"GetPlayer()", 	//id equivalent, https://www.creationkit.com/index.php?title=GetPlayer_-_Game
	"GetCurrentLocation()", //ls equivalent, lists all items in current directory as objects in area, https://www.creationkit.com/index.php?title=GetCurrentLocation_-_ObjectReference
	"GetParentCell()", //pwd equivalent, displays current directory, https://www.creationkit.com/index.php?title=GetParentCell_-_ObjectReference
	"EquipItem()" //read 'r' or write 'w' from command line, https://www.creationkit.com/index.php?title=EquipItem_-_Actor
};
//built in commands
int (*pyrushBinCMDSs[]) (char **) = 
{
  &pyrushCOC,
  &pyrushHelp,
  &pyrushQQQ,
  &pyrushMoveTo,
  &pyrushTime,
  &pyrushGetPlayer,
  &pyrushGetLoc,
  &pyrushGetCell,
  &pyrushEquip
};
int pyrushEquip(char **args)
{
	FILE *f; //pointer to file to read or write to file
	char c, txt[255];
	
	if(args[3] != NULL)
	{
		strcat(txt, args[3]);
	}

	if(args[1] == NULL)
	{
		printError(args);
		exit(EXIT_FAILURE);
	}

	printf("\nYou have equipped %s, and are ", args[1]);

	if(strcmp(args[2],"w") == 0)
	{
		f = fopen(args[1], "w");

		printf("using it.\n");
		fprintf(f,"%s", txt);

	}
	else if(strcmp(args[2], "r") == 0)
	{
		f = fopen(args[1], "r");

		printf("observing it.\n");
		c = fgetc(f);
    	while (c != EOF) 
    	{
			printf ("%c", c); 
			c = fgetc(f);
    	}
	}
	else
		printf("doing nothing with it.\n");

	fclose(f);
	return 1;
}
int pyrushGetPlayer(char **args)
{
	printf("\n%s is currently playing in the Pyrush world\n", username);
	return 1;
}
int pyrushGetLoc(char **args)
{
	printf("\nYou look around and see the following items\n");
	struct dirent *de;  //directory entry pointer

	DIR *dr = opendir("."); //directory type pointer

	if (dr == NULL)  //if no directory found
	{
		printError(args);
		exit(EXIT_FAILURE);
	}

	while ((de = readdir(dr)) != NULL) 
			printf("%s\n", de->d_name); 

	closedir(dr);
	return 1;
}
int pyrushGetCell(char **args)
{
	char cwd[PYRUSHBFSZ];
	getcwd(cwd, sizeof(cwd));
	printf("\n%s's is in %s", username, cwd);
	printf("\n");
	return 1;
}
int pyrushCOC(char **args)
{
	if (args[1] == NULL) 
	{
		fprintf(stderr, "\nNo location was indicated...");
	} 
	else 
	{
		if (chdir(args[1]) != 0) 
		{
			printError(args);
		}
	}
	return 1;
}
int pyrushHelp(char **args)
{
	char *link = "https://www.creationkit.com/index.php?title=Category:Console_Commands";
	char *call = "/usr/bin/firefox https://www.creationkit.com/index.php?title=Category:Console_Commands";
	system(call);
	int i;
	printf("\n---------------PYRUSH Help---------------\n");

	printf("\nEnter commands or programs and their arguments with Papyrus syntax");
	printf("\nFor list of applicable command formats and expected syntax, see\n\t%s", link);
	printf("\nThe following commands built in:\n");

	for (i = 0; i < sizeof(pyrushBINS) / sizeof(char *); i++) 
	{
		printf("  %s\n", pyrushBINS[i]);
	}
	printf("\n-----------------------------------------\n");

	return 1;
}
int pyrushQQQ(char **args)
{
	printf("%s, you are now leaving the Pyrush world.", username);
  	return 0;
}
int pyrushMoveTo(char **args)
{
	char c;
	FILE *fsrc, *fdes; //source file and destination file
	fsrc = fopen(args[1], "r");

	if (fsrc == NULL)
		printError(args);
	
	fdes = fopen(args[2], "w");
	if (fdes == NULL)
		printError(args);

	c = fgetc(fsrc);
	while (c  != EOF)
	{
		fputc(c, fdes);
		c = fgetc(fsrc);
	}
	
	printf("\n%s is now found in %s\n", args[1], args[2]);
	
	fclose(fsrc);
	fclose(fdes);
	
	return 1;
}
int pyrushTime(char **args)
{
	printf("\n");
	system("date +%F");
	system("date +%T");
	return 1;
}

//run a program
int pyrush_run(char **args)
{
	//printf("\nI got to run"); used in testing
	pid_t pid = fork();
	int run;

	if(pid == -1)
		printError(args);
	else if(pid == 0)
	{
		if(execvp(args[0],args) < 0)
			printError(args);
		exit(EXIT_FAILURE);
	}
	else
	{
		wait(NULL);
		return 1;
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
	//tokens for reallocation
	char *tknra;

	tkn = strtok(cmd, pyrushdelim);
	
	do
	{
		//allocate memory
		if(!tknbf)
		{
		fprintf(stderr, "\nPyrush is unable to allocate memory...");
		exit(EXIT_FAILURE);
		}

		tknbf[pos] = tkn;
		pos++;

		if (pos >= PYRUSHTKSZ) 
		{
			tknra = tkn;
			tknbf = realloc(tknbf, (PYRUSHTKSZ+1024) * sizeof(char*));
      		if (!tknbf) 
			{
				free(tknra);
				fprintf(stderr, "\nPyrush is unable to allocate memory...");
				exit(EXIT_FAILURE);
			}
    	}

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
	char *bf = malloc(sizeof(char) * PYRUSHBFSZ);
	//reset position
	int pos = 0;
	//character to read
	int c;

	do
	{
		//error if memory not allocated or reallocated
		if(!bf)
		{
		fprintf(stderr, "\nPyrush is unable to allocate memory...");
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

			add_history(bf);
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
	printf("\n-Pyrush Team: Andrew Knickman and Adeniyi Ajayi--");
	printf("\n----Enter 'Help' for a list of commands----------");
	printf("\n------All commands based on Papyrus Syntax-------");
	printf("\n-----------Shell program written in C------------");
	printf("\n-----For OOD devs learning operating systems-----");

	printf("\n");
	
	printf("\n%s is exploring the Pyrush System-to-Game World", username);
	printf("\nWelcome to Pyrush, %s.\n", username);
	sleep(1);

	pyrush_loop();
}
int main(int argc, char **argv)
{
	//initialize pyrush
	pyrush_init();

	return EXIT_SUCCESS;
}
