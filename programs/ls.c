#include "types.h"
#include "console.h"
#include "time.h"
#include "string.h"
#include "m328Pdef.h"
#include "traps.h"

#include <nanofs/nanofs.h>

#define DIRSIZ MAXFNAME

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[64], *p;
  int fd;
  dirent_t de;
  struct stat st;

  if((fd = fs_open(path, FS_O_RDONLY)) < 0){
    printf("ls: cannot open %s\n", path);
    return;
  }

  if(fs_fstat(fd, &st) < 0){
    printf("ls: cannot stat %s\n", path);
    fs_close(fd);
    return;
  }

  switch(st.type){
  case TYPE_FILE:
    printf("%s %d %l\n", fmtname(path), st.type, st.size);
    break;

  case TYPE_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    memcpy(buf, path, strlen(path));
    p = buf+strlen(buf);
    *p++ = '/';

    while(fs_read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(fs_fstat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d\n", fmtname(buf), st.type, st.size);
    }
    break;
  }
  fs_close(fd);
}

int
ls_main(int argc, char *argv[])
{
    if(argc < 1)
        ls("/");
    else
        ls(argv[1]);
    return 0;
}
