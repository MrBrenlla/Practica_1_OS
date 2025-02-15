#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

#define MAX  300

void limpiarBuffer(char buf[]);
char * gid_to_string(gid_t id);
char * uid_to_string(uid_t id);
char TipoFichero (mode_t m);
char * ConvierteModo (mode_t m);
void auxInfo(char dir[],char arg[]);

/*
--------------------------------------------------------------------------------
*/
void limpiarBuffer(char buf[]){
  //Póñense todos os campos da cadea a '\0', o carácter nulo
	for (int i  = 0 ; i<MAX ; i++ ){
		buf[i]='\0';
	}
}
/*
--------------------------------------------------------------------------------
*/
char * gid_to_string(gid_t id){
  struct group *grp;
  grp = getgrgid(id);
  if (grp == NULL) {
    perror("getgrgid");
  }
  return grp->gr_name;
}
/*
--------------------------------------------------------------------------------
*/
char * uid_to_string(uid_t id){
  struct passwd *pwd;
  pwd = getpwuid(id);
  if(pwd == NULL) {
      perror("getpwuid");
  }
  return pwd->pw_name;
}
/*
--------------------------------------------------------------------------------
*/
char TipoFichero (mode_t m){
switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
case S_IFSOCK: return 's'; /*socket */
case S_IFLNK: return 'l'; /*symbolic link*/
case S_IFREG: return '-'; /* fichero normal*/
case S_IFBLK: return 'b'; /*block device*/
case S_IFDIR: return 'd'; /*directorio */
case S_IFCHR: return 'c'; /*char device*/
case S_IFIFO: return 'p'; /*pipe*/
default: return '?'; /*desconocido, no deberia aparecer*/
}
}
/*
--------------------------------------------------------------------------------
*/
char * ConvierteModo (mode_t m)
{
char * permisos;
permisos=(char *) malloc (12);
strcpy (permisos,"---------- ");
permisos[0]=TipoFichero(m);
if (m&S_IRUSR) permisos[1]='r'; /*propietario*/
if (m&S_IWUSR) permisos[2]='w';
if (m&S_IXUSR) permisos[3]='x';
if (m&S_IRGRP) permisos[4]='r'; /*grupo*/
if (m&S_IWGRP) permisos[5]='w';
if (m&S_IXGRP) permisos[6]='x';
if (m&S_IROTH) permisos[7]='r'; /*resto*/
if (m&S_IWOTH) permisos[8]='w';
if (m&S_IXOTH) permisos[9]='x';
if (m&S_ISUID) permisos[3]='s'; /*setuid, setgid y stickybit*/
if (m&S_ISGID) permisos[6]='s';
if (m&S_ISVTX) permisos[9]='t';
return (permisos);
}

/*
--------------------------------------------------------------------------------
*/
void auxInfo(char dir[], char arg[]){
  struct tm time ;
  char aux1[128], nom[MAX];
  char * aux2 ;
  if(strncmp("\0",arg,1)==0) strcpy(arg,".");
  struct stat datos;
  if(lstat(arg,&datos)==0){
    aux2 = ConvierteModo(datos.st_mode);
    printf("%7ld %s %2ld ",datos.st_ino,aux2,datos.st_nlink );
    printf("%7s %7s %7ld ",uid_to_string(datos.st_uid),gid_to_string(datos.st_gid),datos.st_size);
    time=*(localtime(&datos.st_mtime));
    strftime(aux1,128,"%d %b %H:%M",&time);
    printf("%s ",aux1);
    limpiarBuffer(nom);
    limpiarBuffer(aux1);
    if(*aux2=='l'){
      readlink(arg,aux1,MAX);
      strcat(strcat(nom," -> "),aux1);
    }
    if (strncmp(arg,"./",2)!=0 && strncmp(arg,".\0",2)!=0 && strncmp(arg,"../",3)!=0){ printf("%s/%s%s\n",dir,arg, nom );}
    else printf("%s/%s\n",dir,nom );
    free(aux2);
  }
  else perror("Error");
}
/*
--------------------------------------------------------------------------------
*/

int main(int argc,char * argv[]){
  if(argc==1) auxInfo(".",".");
    for (int i=1 ; i<argc ; i++ ){
      auxInfo(".",argv[i]);
    }

}
