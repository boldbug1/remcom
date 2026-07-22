#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#define BUFFER_SIZE 4096

int get_linecount(FILE *fp) {
    rewind(fp);
    int ch, prev_ch = EOF, last_ch = EOF;
    bool inQuote = false, inChar = false, inComment = false, escaped = false;
    int lines = 0;
    bool file_empty = true;

    while ((ch = fgetc(fp)) != EOF) {
        file_empty = false;

        if (!inQuote && !inChar && prev_ch == '/' && ch == '/') {
            inComment = true;
        }

        if (ch == '\n') {
            inComment = false;
            inChar = false;
        }

        if ((inQuote || inChar) && prev_ch == '\\' && !escaped) {
            escaped = true;
        } else {
            escaped = false;
        }

        if (ch == '\'' && !inQuote && !inComment && !escaped) {
            inChar = !inChar;
        }

        if (ch == '"' && !inChar && !inComment && !escaped) {
            inQuote = !inQuote;
        }

        if (!inQuote && ch == '\n') {
            lines++;
        }

        prev_ch = ch;
        last_ch = ch;
    }

    if (!file_empty && last_ch != '\n') {
        lines++;
    }

    return lines;
}

char *get_range_content(FILE *fp, int start, int end) {
    rewind(fp);

    size_t capacity = BUFFER_SIZE;
    char *buffer = malloc(capacity);
    if (!buffer) {
        perror("Memory alloc failed");
        return NULL;
    }

    int ch, prev_ch = EOF;
    int current_line = 1;
    size_t buff_idx = 0;
    bool inQuote = false, inChar = false, inComment = false, escaped = false;

    while ((ch = fgetc(fp)) != EOF && current_line <= end) {
    if(current_line >=start){
        if (buff_idx + 1 >= capacity) {
            capacity *= 2;
            char *new_buffer = realloc(buffer, capacity);
            if (!new_buffer) {
                perror("Realloc failed");
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
        buffer[buff_idx++] = (char)ch;
    }


        if (!inQuote && !inChar && prev_ch == '/' && ch == '/') {
            inComment = true;
        }

        if (ch == '\n') {
            inComment = false;
            inChar = false;
        }

        if ((inQuote || inChar) && prev_ch == '\\' && !escaped) {
            escaped = true;
        } else {
            escaped = false;
        }

        if (ch == '\'' && !inQuote && !inComment && !escaped) {
            inChar = !inChar;
        }

        if (ch == '"' && !inChar && !inComment && !escaped) {
            inQuote = !inQuote;
        }

        if (!inQuote && ch == '\n') {
            current_line++;
        }

        prev_ch = ch;
    }

    buffer[buff_idx] = '\0';
    return buffer;
}

int main(void) {
    const char *filename = "source.c";
    FILE *fp = fopen(filename,"r");
    if(!fp){
        perror("Error opening file");
        return -1;
    }
    int lines = get_linecount(fp);

    int start = 122;
    int end = 150;

    if (start < 1 || end > lines || start > end) {
        fprintf(stderr, "Invalid range [%d to %d] for file with %d lines\n", start, end, lines);
        return -1;
    }

    char *line_content = get_range_content(fp, start, end);
    if (!line_content) {
        fprintf(stderr, "No content found\n");
        fclose(fp);
        return -1;
    }

    printf("%s\n", line_content);
    free(line_content);
    fclose(fp);
    return 0;
}