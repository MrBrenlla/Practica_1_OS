// Practica 1 se SO
//Parte 2, listar.c
// Data: 23 de Outubro do 2019

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
	//Dado o nome de un archivo do directrio actual devolve o tamaño, ou no seu defecto
	// 0 se houbo un erro
  struct stat datos;
  if(lstat(nom,&datos)==0){
    return datos.st_size;
  }
  else perror("Error");
  return 0;
}
/*
--------------------------------------------------------------------------------
*/
char * gid_to_string(gid_t id){
	//Dado o id dun grupo devolvese o nome deste ou no seu defecto "" se houbo un erro
  struct group *grp;
  grp = getgrgid(id);
  if (grp == NULL) {
    perror("getgrgid");
    return "";
  }
  return grp->gr_name;
}
/*
--------------------------------------------------------------------------------
*/
char * uid_to_string(uid_t id){
	//Dado o id dun usuario devolvese o nome deste ou no seu defecto "" se houbo un erro
  struct passwd *pwd;
  pwd = getpwuid(id);
  if(pwd == NULL) {
      perror("getpwuid");
      return "";
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
char * ConvierteModo (mode_t m){
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
void mostrar(int l,int v,struct dirent * sig , char dir[]){
  //Elixese si se debe mostrar o ficheiro que recive e en caso afirmativo
  //cal é o formato adecuado
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
void auxInfo(char dir[], char arg[]){
  //Recívese un nome e a ruta que fixo falta para chegar ata el para mostrar a
  //sua información ao igual que o comando "ls -li"  de linux
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
    //Mirase se é un link para así poder indicar a que dirixe en caso de selo
    if(*aux2=='l'){
      readlink(arg,aux1,MAX);
      strcat(strcat(nom," -> "),aux1);
    }
    if (strncmp(arg,"./",2)!=0 && strncmp(arg,".\0",2)!=0 && strncmp(arg,"../",3)!=0){
      printf("%s/%s%s\n",dir,arg, nom );
    }
    else printf("%s/%s\n",dir,nom );
    free(aux2);
  }
  else perror(" Error");//Se o nome non existe salta un erro
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
         chdir(dir);
        }
  }
  if(rec==0) printf("********%s\n",actualdir );
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
