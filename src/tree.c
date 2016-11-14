#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "iso9660.h"
#include "fct.h"
static void *cast(void *p)
{
  return p;
}
static int treesea2(char *arg, struct iso_dir *cur, 
  struct iso_prim_voldesc *head, struct tree *t) 
{
  char *str = cast(head);
  str += (cur->data_blk.le - 16) * 2048;
  struct iso_dir *cur2 = cast(str); 
  char newarg[100] = "";
  int i;
  for (i = 0; arg[i]; i++)
    newarg[i] = arg[i];
  int j = 0;
  for (j = 0; j < t->space; j++, i++)
  newarg[i] = ' ';
  newarg[i] = '|';
  newarg[i + j - 1] = '|';
  newarg[i + j] = '\0';
  t->dir = t->dir + 1;
  tree(newarg, head, cur2 , t);
  return 0;
}

static int treesea(char *arg, struct iso_dir *cur, 
  struct iso_prim_voldesc *head, struct tree *t) 
{
  char *str = cast(&(cur->idf_len) + 1);
  char str2[128] = "";
  int i = 0;
  for (i = 0; i < cur->idf_len && str[i] != ';'; i++)
    str2[i] = str[i];
  str2[i] = '\0';
  if (str2[0] == 0 || str2[0] == 1)
    return 0;
  if (arg[0] == '\0' || arg[1] != ' ')
  {
    fprintf(stdout,"%c-- %s", t->last, str2);
    arg[0] = '|';
  }
  else
  {
    int k = strlen(arg) - 1;
    fprintf(stdout, "%.*s%c-- %s", k, arg, t->last, str2);
  }
  if((cur->type == 2 || cur->type == 3))
     fprintf(stdout,"/");
  else
    t->file = t->file + 1;
  fprintf(stdout, "\n");
  fflush(stdout);
  t->last = '|';
  return (cur->type == 2 || cur->type == 3) ? treesea2(arg, cur, head, t) : 0;
}

int tree(char command[], struct iso_prim_voldesc *head, struct iso_dir *cur,
  struct tree *t) 
{
  char arg[100] = "";
  if (!strcmp(command,"tree"))
  {
    fprintf(stdout, ".\n");
    arg[0] = '\0'; 
  }
  else
  {
    int i = 0;
    for (i = 0; command[i]; i++)
      arg[i] = command[i];
    arg[i] = '\0';
  }
  char *str = cast(cur);
  while (cur->idf_len)
  {
    str += cur->dir_size;
    void *tmp = str;
    t->last = '|';
    struct iso_dir *last = tmp;
    if (!last->idf_len)
    {
      t->last = '+';
      if (!strcmp(command,"tree"))
      {
        t->left = ' ';
      }
      arg[strlen(arg) - 1] = ' ';
      arg[strlen(arg)] = '\0';
    }
    arg[0] = t->left;
    treesea(arg, cur, head, t);
    cur = tmp;
  }
  return 1;
}
