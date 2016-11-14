#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "iso9660.h"
#include "fct.h"
static void *convert(void *c)
{
  return c;
}
static int getsea(char *arg, struct iso_dir *cur, struct iso_prim_voldesc *head) 
{
  char *str = convert(&(cur->idf_len) + 1);
  char str2[128] = "";
  int i = 0;
  for (i = 0; i < cur->idf_len && str[i] != ';'; i++)
    str2[i] = str[i];
  str2[i] = '\0';
  if (!strcmp(str2, arg))
  {
    if (cur->type == 2 || cur->type == 3)
    {
      fprintf(stderr, "entry '%s' is a directory\n", arg);
      return 1;
    }
    char *str = convert(head);
    str += (cur->data_blk.le  - 16) * 2048;
    int fd = open(str2, O_RDWR | O_CREAT, 0664);
    for (unsigned int i = 0; fd && i < cur->file_size.le; i++)
    {
      write(fd, str, 1);
      str++;
    }
    close(fd);
    return 1;
  }
  return 0;
}

static int get2(char *arg, struct iso_prim_voldesc *head) 
{
  struct iso_dir *cur = curptr;
  void *tmp = cur;
  char *str = tmp;
  while (cur->idf_len)
  {
    if (getsea(arg, cur, head))
      return 1;
    str += cur->dir_size;
    tmp = str;
    cur = tmp;
  }
  fprintf(stderr, "unable to find '");
  fprintf(stderr, arg);
  fprintf(stderr, "' entry\n");
  return 1;
}

int get(char command[], struct iso_prim_voldesc *head) 
{
  char arg[100];
  int i = 0;
  for (i = 0; command[i] && command[i] != ' '; i++)
    continue;
  if (!command[i])
  {
    fprintf(stderr, "my_read_iso: get: command must take an argument\n");
    fflush(stderr);
    return 1;
  }
  int j = 0;
  for (i = i + 1; command[i]; i++)
  {
    arg[j] = command[i];
    j++;
  }
  arg[j] = '\0';
  return get2(arg, head);
}

