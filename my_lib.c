#include "my_lib.h"

size_t my_strlen(const char *str){
    size_t len = 0;
    for(int i=0;str[i]!='\0';i++){
    len++;
    }
    return len;
}
//compara caracter a caracter hasta encontrar uno difernte y calcula la diferncia
int my_strcmp(const char *str1, const char *str2){
    int i=0;
    while(str1[i]==str2[i]){
       i++; 
    }
    if(str1[i]==str2[i]){
        return 0;
    }else if(str1[i]>str2[i]){
        return str1[i]-str2[i];
    }else 
     return str2[i]-str1[i];
}
//Copia el contenido de src a dest
char *my_strcpy(char *dest, const char *src){
     int i;
    for (i=0;src[i]!='\0';i++){
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}

char *my_strncpy(char *dest, const char *src, size_t n){
    for ( i = 0; i < n; i++)
    {
        dest[i]=src[i];
    }
    dest[++i] == "\0"
    return &dest;
}

char *my_strcat(char *dest, const char *src){
    int dest_size = my_strlen(*dest);
    for ( i = 0; src[i] != "\0"; i++)
    {
        dest[dest_size + i] = src[i];
    }
    dest[++i] == "\0"
    return &dest;
}

char *my_strchr(const char *s, int c){
    const char *aux_s = s;
    while(*aux_s++ != c){
        if(*aux_s){
            return null;
        }
    }
    return (char*)aux_s;
}
