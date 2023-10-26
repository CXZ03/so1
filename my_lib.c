#include "my_lib.h"
//hola
size_t my_strlen(const char *str){
    const char *aux_str = str;
    int i = 0;
    while(str[i++]){
    }
    return aux_str - str;
}

int my_strcmp(const char *str1, const char *str2){
    int i=0;
    while(str1[i++] == str2[i++]){
        if(str1[i] == "\0" && str2[i] == "\0")
        return 0;
    }
    return str1[i] - str2[i];
}

char *my_strcpy(char *dest, const char *src){
    for ( i = 0; src[i] != "\0"; i++)
    {
        dest[i]=src[i];
    }
    dest[++i] == "\0"
    return &dest;
    
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
