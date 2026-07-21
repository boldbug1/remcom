#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

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
            inQuote = !inQuote;
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

long return_line_offset(const char *filename,int target_line){
    int lines = get_linecount(filename);
    if(target_line > lines || target_line < 1){
        fprintf(stdout,"Out of bounds\n");
        return -1;
    }
    FILE *fp = fopen(filename,"r");
    if(!fp){
        fprintf(stdout,"Error : %s",strerror(errno));
        return -1;
    }

    int ch;
    int current_line = 1;
    bool inQuote = false;
    bool file_empty = true;
    int line_tracker = 0;
    bool line_found = false;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '"') {
            inQuote = !inQuote;
        }
        if (!inQuote && ch == '\n') {
            current_line++;
            if (current_line == target_line) {
                long offset = ftell(fp); // Location of start of target line
                fclose(fp);
                return offset;
            }
        }
    }
    fclose(fp);
    return -1;
}
char *get_range_content(const char *filename,int start,int end){
    //return content withing a certain range
}

int main(void){
    const char *filename = "source.c";
    int lines = get_linecount(filename);

    int start = 1;
    int end = 5;

    if (start < 1 || end > lines || start > end) {
        fprintf(stderr, "Invalid range [%d to %d] for file with %d lines\n", start, end, lines);
        return -1;
    }


    return 0;
}