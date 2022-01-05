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

char *path[10]={"/bin", NULL};

void printArgv(char **argv) {
  int i=0;
  while (argv[i]!=NULL) {
    printf("element: '%s'\n",argv[i]);
    i++;
  }
}

void printPath(){
  int i=0;
  while(path[i]) {
    printf("%d : %s\n",i, path[i]);
    i++;
  }
}

bool isBuiltInCommand(char *cmdName) {
  if(strcmp(cmdName, "cd")==0 || strcmp(cmdName, "path")==0 || strcmp(cmdName, "exit")==0)
    return true;
  else
    return false;
}

int executeBuiltInCommand(struct command cmd) {
  int i=0;
  if (strcmp(cmd.commandName, "cd")==0) {
    //if number(args) is not 1, throw error
    i=1;
    while(cmd.argv[i])
      i++;
    if (i!=2) {
      fprintf(stderr, "cd has wrong number of args\n");
      return 1;
    }
    if(chdir(cmd.argv[1])==0) {
      printf("Current dir changed to : %s\n", cmd.argv[1]);
    }
    else {
      printf("Could not change dir\n");
      return 1;
    }
  }
  else if(strcmp(cmd.commandName, "path")==0) {
    if (cmd.argv[1]==NULL) {
      //path should be blank
      path[0]=NULL;
    }
    else {
      //set path to specified path
      i=0;
      while(cmd.argv[i+1]){
        path[i]=cmd.argv[i+1];
        i++;
      }
      path[i]=NULL;
    }
  }
  else 
    exit(0);
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
    return executeBuiltInCommand(cmd);
    
  //check if user command is found and can be executed
  char *absPath = commandCanExecute(cmd.commandName);
  if (!absPath) {
    printf("command cannot exec or not found\n");
    return 1;
  }
  int rc = fork();
  int res;
  if (rc<0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } 
  else if (rc==0) {
    //run the command in forked process
    res = execv(absPath, cmd.argv);
    if (res<0) { 
      printf("command could not execute");
      exit(1);
    }
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
  int l;
  size_t len=0;
  struct command cmd;
  //struct command *cptr = &cmd;

  printf("wish> ");
  while ((l=getline(&line, &len, stdin))) {
    if (l>1) {
      cmd = parseCommand(line);
      executeCommand(cmd);
    }
    printf("wish> ");
  }

  return 0;
}
