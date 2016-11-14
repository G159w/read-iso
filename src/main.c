#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "iso9660.h"
#include "fct.h"

static void displayh(void)
{
  fprintf(stdout, "help\t: display command help\n");
  fprintf(stdout, "info\t: display volume info\n");
  fprintf(stdout, "ls\t: display directory content\n");
  fprintf(stdout, "cd\t: change current directory\n");
  fprintf(stdout, "tree\t: display the tree of the current directory\n");
  fprintf(stdout, "get\t: copy file to local directory\n");
  fprintf(stdout, "cat\t: display file content\n");
  fprintf(stdout, "quit\t: program exit\n");
}

static int parse3(char command[], struct iso_prim_voldesc *head)
{
  if (!strncmp(command, "cd", 2))
  {
    return cd(command, head); 
  }
  else if (!strncmp(command, "cat", 3))
  {
    return cat(command, head); 
  }
  else if (!strncmp(command, "get", 3))
  {
    return get(command, head); 
  }
  else if (!strncmp(command, "tree", 4))
  {
    struct tree t;
    t.dir = 0;
    t.file = 0;
    t.last = '|';
    t.space = 3;
    t.left = '|';
    tree(command, head, curptr, &t); 
    fprintf(stdout, "\n%d directories, %d files\n", t.dir, t.file);
    fflush(stdout);
    return 1;
  }
  return 0;
}
static int parse2(char command[], char iso[], struct iso_prim_voldesc *head)
{
  if (!strncmp(command, "info", 4) && (!command[4] || command[4] == ' '))
  {
    if (command[4])
      fprintf(stderr, "my_read_iso: info: command does not take an argument\n"); 
    else
      return info(head); 
    return 1;
  }
  if (!strncmp(command, "ls", 2) && (!command[2] || command[2] == ' '))
  {
    if (command[2])
      fprintf(stderr, "my_read_iso: ls: command does not take an argument\n"); 
    else
    {
       return ls(curptr); 
    }
    return 1;
  }
  else if (!command[0])
    return 1;
  else if (parse3(command, head))
    iso[0] = iso[0];
  else 
    return 0;
  return 1;
}
int parse(char command[], char iso[], struct iso_prim_voldesc *head)
{
  if (!strncmp(command, "quit", 4) && (!command[4] || command[4] == ' '))
  {
    if (command[4])
      fprintf(stderr, "my_read_iso: quit: command does not take an argument\n"); 
    else
      return 0;
  }
  else if (!strncmp(command, "help",4) && (!command[4] || command[4] == ' '))
  {
    if (command[4])
      fprintf(stderr, "my_read_iso: help: command does not take an argument\n"); 
    else
     displayh();
  }
  else if (parse2(command, iso, head))
    iso[0] = iso[0];  
  else
  {
     fprintf(stderr, "my_read_iso: ");
     fprintf(stderr, command);
     fprintf(stderr, ": unknown command\n");
  }
  for (int i = 0; i < 99; i++)
    command[i] = '\0';
  return 1;
}
static void leftshift(char str[])
{
  int i;
  for (i = 0; str[i + 1]; i++)
    str[i] = str[i + 1];
  str[i] = '\0';
}
static void makecomm(char command[])
{
  int i;
  for (i = 0; command[i] && (command[i] == ' ' || command[i] == '\t'); i++)
  {
    leftshift(command);
    i--;
  }
  for (; command[i] && command[i + 1] != '\n'; i++)
  {
    if ((command[i] == ' ' ||  command[i] == '\t') 
      && (command[i + 1] == ' ' || command[i + 1] == '\t'))
    {
      leftshift(command + i);
      i--;
    }
  }
  if (command[i] == ' ' || command[i] == '\t')
  {
    command[i] = '\0';
    command[i + 1] = '\0';
  }
  else if (command[i + 1] == '\n')
    command[i + 1] = '\0';
  else 
    command[0] = '\0';
}
static struct iso_prim_voldesc *openiso(char path[])
{
  int filedesc = open(path, O_RDONLY);
  if (filedesc < 0)
  {
    fprintf(stderr, "my_read_iso: %s: No such file or directory\n", path);
    exit(1);
  }
  struct stat buf;
  if (fstat(filedesc, &buf) < 0 || buf.st_size < 17 * 2048)
  {
    fprintf(stderr, "my_read_iso: %s: invalid ISO9660 file\n", path);
    exit(1);
  }
  struct iso_prim_voldesc *head = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, 
    filedesc, 32768);
  if (!strcmp(head->std_identifier, "CD_001"))
  {
    fprintf(stderr, "my_read_iso: %s: invalid ISO9660 file\n", path);
    exit(1);
  }
  void *tmp =  head;
  char *str = tmp;
  str += (head->root_dir.data_blk.le  - 16) * 2048;
  tmp = str;
  curptr = tmp;
  return head;
}
static void term(struct iso_prim_voldesc *head)
{
  char command[1024] = "";
  int nb = read(0, command, 1024);
  if (nb > 0)
  {
    char iso[10] = "useless";
    int ind = 0;
    command[nb] = '\0';
    while (command[ind])
    {
      char str2[100];
      int i = 0;
      for (i = 0; command[ind] && command[ind] != '\n'; i++)
      {  
        str2[i] = command[ind];
        ind++;
      }
      str2[i] = '\n';
      str2[i + 1] = '\0';
      makecomm(str2);
      parse(str2, iso, head);
      ind++;
    }
  }
}
int main(int argc, char *argv[])
{
  int quit = 1;
  if (argc != 2)
  {
    fprintf(stderr, "usage: my_read_iso FILE\n");
    return 1;
  }
  struct iso_prim_voldesc *head = openiso(argv[1]);
  if (!head)
    return 1;
  if (!isatty(0))
  {
    term(head);
    quit = 0;
  }
  while (quit)
  {
    write(0,"> ",2);
    char command[100];
    read(1, command, 1024);
    makecomm(command);
    quit = parse(command, argv[1], head);
  }
  return 0;
}
