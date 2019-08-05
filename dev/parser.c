#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <id3v2lib.h>

int mframe_size;
char mem_mframe_data[0x70];
int * mframe_data;
char mem_mpasswd[0x20];
int * passwd;
char junk[0x50];

void init_proc(){
    mframe_size=0;
    mframe_data=&mem_mframe_data;
    memset(mframe_data,0,0x70);
    passwd=&mem_mpasswd;
    memset(passwd,0,0x20);
}

int check_password(char * password){
    char filename[0x130];
    struct dirent ** filelist;
    int n = scandir("config/", &filelist, 0, alphasort);
    if (n < 0){
        return 0;
    }else{
        int match=0;
        while(n--){
            char * res;
            res = strrchr(filelist[n]->d_name, '.');
            if (res!=NULL) {
                if (strcmp(res,".passwd")==0){
                    match=1;
                    memset(&filename,0,0x130);
                    strcpy(&filename,"config/");
                    strcat(&filename,filelist[n]->d_name);
                }
            }
            free(filelist[n]);
        }
        free(filelist);
        if (match==0) return 0;
    }
    FILE * fp=fopen(&filename,"r");
    if (fp == NULL) return 0;
    fread(passwd,1,0x18,fp);
    fclose(fp);
    if (strcmp(passwd,password) == 0) return 1;
    return 0;
}

void read_title(char * filename){
    ID3v2_tag * tag = load_tag(filename);
    if (tag != NULL){
        ID3v2_frame* title_frame = tag_get_title(tag);
        ID3v2_frame_text_content * title_content = parse_text_frame_content(title_frame);
        if (strlen(title_content->data)<=0x70) {
            mframe_size=strlen(title_content->data);
            strcpy(&mem_mframe_data,title_content->data);
        }
    }
}

void read_artist(char * filename){
    ID3v2_tag * tag = load_tag(filename);
    if (tag != NULL){
        ID3v2_frame* artist_frame = tag_get_artist(tag);
        ID3v2_frame_text_content * artist_content = parse_text_frame_content(artist_frame);
        if (strlen(artist_content->data)<=0x70) {
            mframe_size=strlen(artist_content->data);
            strcpy(&mem_mframe_data,artist_content->data);
        }
    }
}

void read_album(char * filename){
    ID3v2_tag * tag = load_tag(filename);
    if (tag != NULL){
        ID3v2_frame* album_frame = tag_get_album(tag);
        ID3v2_frame_text_content * album_content = parse_text_frame_content(album_frame);
        if (strlen(album_content->data)<=0x70) {
            mframe_size=strlen(album_content->data);
            strcpy(&mem_mframe_data,album_content->data);
        }
    }
}

int * parse(char * password, char * classname, char * filename){
    init_proc();
    if (check_password(password)==1){
        if (strcmp(classname,"title")==0){
            read_title(filename);
        }else if (strcmp(classname,"artist")==0){
            read_artist(filename);
        }else if (strcmp(classname,"album")==0){
            read_album(filename);
        }
    }
    return &mframe_data;
}
