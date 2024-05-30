#include <stdio.h>

#include <unistd.h>
#include <linux/limits.h>

#include <string.h>

#include "helper.h"

using namespace std;

string getFilePath(string file)
{
	char cwd[PATH_MAX];
   	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
   		printf("Current working dir: %s\n", cwd);
	} 
	else 
	{
       perror("getcwd() error");
   	}

	strcat(cwd, "/src/SDL/rc/");
	strcat(cwd, file.c_str());
	printf("Current file: %s\n", cwd);

    string s(cwd);

    return string(cwd);
}