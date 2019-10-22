
// Practica 0 se SO
// Data: 4 de Outubro do 2019

// Grupo 1.3
// Brais García Brenlla ; b.brenlla
// Ángel Álvarez Rey ; angel.alvarez.rey






//Inclúense varias librerías necesarias para a realización dos comandos
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

//Defínese a constante MAX, que será o tamaño máximo das cadeas
#define MAX  300



void limpiarBuffer(char buf[]);
long tamanho(char nom[]);
char * gid_to_string(gid_t id);
char * uid_to_string(uid_t id);
char TipoFichero (mode_t m);
char * ConvierteModo (mode_t m);
void auxInfo(char dir[],char arg[]);
void mostrar(int l,int v,struct dirent * sig, char dir[]);
void auxListar(char actualdir[], char aux1[], int rec, int l, int r, int v);

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
long tamanho(char nom[]){
  struct stat datos;
  if(stat(nom,&datos)==0){
    return datos.st_size;
  }
  else perror("Error");
  return 0;
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
  struct tm * time ;
  char output[128];
  if(strncmp("\0",arg,1)==0) strcpy(arg,".");
  struct stat datos;
  if(stat(arg,&datos)==0){
    printf("%7ld %s %2ld ",datos.st_ino,ConvierteModo(datos.st_mode),datos.st_nlink );
    printf("%7s %7s %7ld ",uid_to_string(datos.st_uid),gid_to_string(datos.st_gid),datos.st_size);
    time=localtime(&datos.st_mtime);
    strftime(output,128,"%a %b %d %Y",time);
    printf("%s ",output);
    if (strncmp(arg,"./",2)!=0 && strncmp(arg,".\0",2)!=0){ printf("%s/%s\n",dir, arg );}
    else printf("%s\n",arg );
    free(time);
  }
  else perror("Error");


}
/*
--------------------------------------------------------------------------------
*/
void mostrar(int l,int v,struct dirent * sig , char dir[]){
  char nom[MAX];
  strcpy(nom,sig->d_name);
  if (v==0 || strncmp(nom,".",1)!=0){
    if (l==0) printf("%s %ld\n",nom, tamanho(nom));
    else auxInfo(dir,nom);
  }
}
/*
--------------------------------------------------------------------------------
*/
void auxListar(char actualdir[], char aux1[], int rec, int l, int r, int v){
 char dir[MAX], tmp[MAX];
 int comp;
  if (strncmp(aux1,"\0\0",2)==0){
    strcpy(aux1,".");
    comp=0;
  }
  else{
    limpiarBuffer(dir);
    getcwd(dir,MAX);
    comp = chdir(aux1);
    if(comp==0)
      if(strncmp(aux1,".\0",2)!=0){
        if (strncmp(aux1,"..\0",3)==0) chdir(dir);
        else chdir("..");
        }
  }
  if(rec==0) printf("********%s\n",aux1 );
  if(comp==0){
    if (rec==1) printf("********%s\n",actualdir );
    DIR * direct;
    direct = opendir(aux1);
      struct dirent * sig = readdir(direct);
        chdir(aux1);
          while(sig!=NULL){
            mostrar(l,v,sig,actualdir);
            sig=readdir(direct);
            }
            if(r==1 || rec==1){
              rewinddir(direct);
              sig=readdir(direct);
              while(sig!=NULL){
                if(strncmp(sig->d_name,".\0",2)!=0 && strncmp(sig->d_name,"..\0",3)!=0){
                  limpiarBuffer(tmp);
                  strcat(strcat(strcat(tmp,actualdir),"/"),sig->d_name);
                  auxListar(tmp,sig->d_name,1,l,r,v);
               }
                sig=readdir(direct);
              }
            }
            closedir(direct);
            chdir(dir);
        }
    else if(rec==0) perror("Error");
}
/*
--------------------------------------------------------------------------------
*/
int main(int argc,char * argv[]){
  int c = 0;
  int l = 0;
  int v = 0;
  int r = 0;
  char aux[MAX];
  if(argc==1) auxListar(".",".", 0, 0, 0, 0);
  else{
    for (int i=1 ; i<argc ; i++ ){
      if(strcmp(argv[i],"-r")==0 && c==0)r=1;
      else if(strcmp(argv[i],"-l")==0 && c==0) l=1;
        else if(strcmp(argv[i],"-v")==0 && c==0) v=1;
          else{  if(strncmp(argv[i],"..",2)!=0) auxListar(argv[i],argv[i], 0, l, r, v);
            else{
              getcwd(aux,MAX);
              chdir("..");
              auxListar(argv[i],&argv[i][1], 0, l, r, v);
              chdir(aux);
            }
            c=1;
      }
    }
    if(c==0) auxListar(".",".", 0, l, r, v);
  }

}