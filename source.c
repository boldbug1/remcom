#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int get_linecount(const char *filename){
    FILE *fp;
    fp = fopen(filename,"r");
    if(!fp){
        printf("Error : Could not open the file!\n");
        return -1;
    }
    int ch;
    int last_ch = EOF;
    int inQuote = 0;
    int lines = 0;
    bool file_empty = true;
    while((ch = fgetc(fp)) != EOF){
        file_empty = false;
        if(ch == '"'){
            inQuote = 1;
        }
        if(inQuote && ch=='"'){
            inQuote = 0;
        }

        if(!inQuote && ch=='\n'){
            lines++;
        }
        last_ch = ch;
    }
    if(!file_empty && last_ch!='\n'){
        lines++;
    }
    fclose(fp);
    return lines;
}

char *get_range_content(int start,int end){
    //returns content withing a certain range
}

int main(void){
    const char *filename = "source.c";
    int lines = get_linecount(filename);
    int line_range_start = 11;
    int line_range_end = 0;

    if(line_range_end > line_range_start){
        if(line_range_end > lines || line_range_start < 0){
            if(line_range_start < 0){
                fprintf(stdout,"Error: line range start cannot be less than 0\n");
            }
            if(line_range_end > lines){
            fprintf(stdout,"Error: Invalid line range , this file contains only %d lines\n",lines);
        }
        return -2;
        }
    }else{
        fprintf(stdout,"Invalid line range;start needs to be less than end\n");
        return -2;
    }
    return 0;
}