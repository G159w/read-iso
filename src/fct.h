#include "iso9660.h"

struct iso_dir *curptr;
struct tree
{
  int dir;
  int file;
  char last;
  int space;
  char left;
};


int ls(struct iso_dir *cur);
int info(struct iso_prim_voldesc *head);
int cd(char command[], struct iso_prim_voldesc *head);
int cat(char commant[], struct iso_prim_voldesc *head);
int get(char commant[], struct iso_prim_voldesc *head);
int tree(char commant[], struct iso_prim_voldesc *head, struct iso_dir *cur,
struct tree *t);
