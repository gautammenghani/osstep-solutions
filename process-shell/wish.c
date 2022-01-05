#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<sys/wait.h>

struct command {
  char *commandName;
  char *argv[10];
};

char *path[10]={"/bin", "/usr/bin/", NULL};

void printArgv(char **argv) {
  int i=0;
  while (argv[i]!=NULL) {
    printf("boi : '%s'\n",argv[i]);
    i++;
  }
}

bool isBuiltInCommand(char *cmdName) {
  if(strcmp(cmdName, "cd")==0 || strcmp(cmdName, "path")==0 || strcmp(cmdName, "exit")==0)
    return true;
  else
    return false;
}

int executeBuiltInCommand(char *cmdName) {
  if (strcmp(cmdName, "cd")==0) {
    printf("cd cmdn\n");
  }
  else if(strcmp(cmdName, "path")==0) {
    printf("path cmdn\n");
  }
  else {
    //exit
    printf("exit cmdn\n");
    exit(0);
  }
  return 0;
}

char* commandCanExecute(char *cmdName) {
  int i=0;
  char *absPath;
  while(path[i]!=NULL){
    absPath = strdup(path[i]);
    strcat(absPath, "/");
    strcat(absPath, cmdName);
    if(access(absPath, X_OK)==0) {
      return absPath;
    }
    i++;
  } 
  return NULL;
}

int executeCommand(struct command cmd) {
  if (isBuiltInCommand(cmd.commandName))
    return executeBuiltInCommand(cmd.commandName);
    
  //check if user command is found and can be executed
  char *absPath = commandCanExecute(cmd.commandName);
  if (!absPath)
    return 1;

  int rc = fork();
  int res;
  if (rc<0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } 
  else if (rc==0) {
    //run the command in forked process
    res = execv(absPath, cmd.argv);
    if (res<0) 
      exit(1);
    else
      exit(0);
  }
  else {
    wait(&res);
  }
  return res;
}

struct command parseCommand(char *line) {
  char *token;
  char *command = strdup(line);
  char *commandName;
  char *args[20];
  int i=1;
  struct command cmd;

  //remove trailing '\n'
  command[strlen(command)-1]='\0';

  //first arg needs to be the program name
  args[0]="wish";

  commandName = strsep(&command, " ");
  while (1){
    token = strsep(&command, " ");
    if (token == NULL) {
      args[i] = NULL;
      break;
    }
    args[i]=token;
    i++;
  }
  memcpy(cmd.argv, args, sizeof args);
  cmd.commandName = commandName;
  return cmd;
}

int main(int argc, char **args) {
  char *line;
  int l,res;
  size_t len=0;
  struct command cmd;
  //struct command *cptr = &cmd;

  printf("wish> ");
  while ((l=getline(&line, &len, stdin))) {
    if (l>1) {
      cmd = parseCommand(line);
      res = executeCommand(cmd);
      if (res!=0)
        printf("command did not execute\n");
    }
    printf("wish> ");
  }

  return 0;
}
