#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "iso9660.h"
#include "fct.h"

static void *cast(void *p)
{
  return p;
}
int info(struct iso_prim_voldesc *head)
{
  char str[33];
  for (int i = 0; i < 32; i++)
    str[i] = head->syidf[i];
  str[32] = 0;
  fprintf(stdout, "System Identifier: %s\n", str);
  for (int i = 0; i < 32; i++)
    str[i] = head->vol_idf[i];
  fprintf(stdout, "Volume Identifier: %s\n", str);
  fprintf(stdout, "Block count: %u\n", head->vol_blk_count.le);
  fprintf(stdout, "Block size: %u\n", head->vol_blk_size.le);
  for (int i = 0; i < 17; i++)
    str[i] = head->date_creat[i];
  str[17] = '\0';
  fprintf(stdout, "Creation date: %s\n", str);
  char str2[129];
  for (int i = 0; i < 128; i++)
    str2[i] = head->app_idf[i];
  str2[128] = 0;
  fprintf(stdout, "Application Identifier: %s\n", str2); 
  return 1;
}
static void printls(struct iso_dir *cur)
{
  char *str = cast(&(cur->idf_len) + 1);
  char d = cur->type & ISO_FILE_ISDIR ? 'd' : '-';
  char h = cur->type & ISO_FILE_HIDDEN ? 'h' : '-';
  if (cur->date[0])
    fprintf(stdout, "%c%c %9u %04d/%02d/%02d %02d:%02d ", d, h, 
      cur->file_size.le, cur->date[0] + 1900, cur->date[1], cur->date[2],
      cur->date[3], cur->date[4]);
  else
    fprintf(stdout, "%c%c %9u Unspecified      ", d, h, cur->file_size.le);
  if (str[0] == 0)
    fprintf(stdout, ".\n");
  else if (str[0] == 1)
    fprintf(stdout, "..\n");
  else
  {
    char str2[128];
    int i = 0;
    for (i = 0; i < cur->idf_len && str[i] != ';'; i++)
      str2[i] = str[i];
    str2[i] = '\0';
    fprintf(stdout, "%s\n", str2);
  }
}
int ls(struct iso_dir *cur)
{ 
  void *tmp = cur;
  char *str = tmp;
  while (cur->idf_len)
  {
    printls(cur);
    str += cur->dir_size;
    tmp = str;
    cur = tmp;
  } 
  return 1;
}
static int  movecd2(char *str, struct iso_dir *cur, char *arg, 
  struct iso_prim_voldesc *head)
{
  char str2[128];
  int i = 0;
  for (i = 0; i < cur->idf_len && str[i] != ';'; i++)
    str2[i] = str[i];
  str2[i] = '\0';
  if (!strcmp(str2,arg))
  {
    if (!(cur->type == 2 || cur->type == 3))
    {
      fprintf(stderr, "entry '%s' is not a directory\n", arg);
      fflush(stderr);
      return 1;
    }
    void *tmp = head;
    char *str = tmp;
    fprintf(stdout, "Changing to '%s' directory\n", arg);
    str += (cur->data_blk.le - 16) * 2048;
    tmp = str;
    curptr = tmp;
    fflush(stdout);
    return 1;
  }
  return 0;
}
static int movecd(struct iso_dir *cur, char *arg, struct iso_prim_voldesc *head)
{

  void *tmp = &(cur->idf_len) + 1;
  char *str = tmp;
  if (!strcmp(arg, ".") && str[0] == 0)
  {
    fprintf(stdout, "Changing to '%s' directory\n", arg);
    str += (cur->data_blk.le - 16) * 2048;
    curptr = cur;
    return 1;
  }
  else if (!strcmp(arg, "..") && str[0] == 1)
  {
    void *tmp = head;
    char *str = tmp;
    fprintf(stdout, "Changing to '%s' directory\n", arg);
    str += (cur->data_blk.le - 16) * 2048;
    tmp = str;
    curptr = tmp;
    fflush(stdout);
    return 1;
  }
  else
    return movecd2(str, cur, arg, head);
}
static int cd2(char *arg, struct iso_prim_voldesc *head) 
{
  struct iso_dir *cur = curptr;
  void *tmp = cur;
  char *str = tmp;
  while (cur->idf_len)
  {
    if (movecd(cur, arg, head))
      return 1;
    str += cur->dir_size;
    tmp = str;
    cur = tmp;
  }
  fprintf(stderr, "unable to find '");
  fprintf(stderr, arg);
  fprintf(stderr, "' directory entry\n");
  return 1;
}
int cd(char command[], struct iso_prim_voldesc *head)
{
  char arg[100];
  int i = 0;
  for (i = 0; command[i] && command[i] != ' '; i++)
    continue;
  if (!command[i])
  {
    void *tmp =  head;
    char *str = tmp;
    str += (head->root_dir.data_blk.le  - 16) * 2048;
    tmp = str;
    curptr = tmp;
    write(1, "Changing to 'root dir' directory\n", 34);
    return 1;
  }
  int j = 0;
  for (i = i + 1; command[i]; i++)
  {
    arg[j] = command[i];
    j++;
  }
  arg[j] = '\0';
  return cd2(arg, head);
}
