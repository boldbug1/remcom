#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#define BUFFER_SIZE 4096

int get_linecount(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error : Could not open the file!\n");
        return -1;
    }

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

    fclose(fp);
    return lines;
}

long return_line_offset(const char *filename, int target_line) {
    int lines = get_linecount(filename);
    if (target_line > lines || target_line < 1) {
        fprintf(stderr, "Out of bounds\n");
        return -1;
    }

    if (target_line == 1) {
        return 0;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error : %s\n", strerror(errno));
        return -1;
    }

    int ch, prev_ch = EOF;
    int current_line = 1;
    bool inQuote = false, inChar = false, inComment = false, escaped = false;

    while ((ch = fgetc(fp)) != EOF) {
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
            if (current_line == target_line) {
                long offset = ftell(fp);
                fclose(fp);
                return offset;
            }
        }
        prev_ch = ch;
    }

    fclose(fp);
    return -1;
}

char *get_range_content(const char *filename, int start, int end) {
    long offset = return_line_offset(filename, start);

    if (offset < 0) {
        fprintf(stderr, "Error : Line not found\n");
        return NULL;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("File opening failed");
        return NULL;
    }

    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return NULL;
    }

    size_t capacity = BUFFER_SIZE;
    char *buffer = malloc(capacity);
    if (!buffer) {
        perror("Memory alloc failed");
        fclose(fp);
        return NULL;
    }

    int ch, prev_ch = EOF;
    int current_line = start;
    size_t buff_idx = 0;
    bool inQuote = false, inChar = false, inComment = false, escaped = false;

    while ((ch = fgetc(fp)) != EOF && current_line <= end) {
        if (buff_idx + 1 >= capacity) {
            capacity *= 2;
            char *new_buffer = realloc(buffer, capacity);
            if (!new_buffer) {
                perror("Realloc failed");
                free(buffer);
                fclose(fp);
                return NULL;
            }
            buffer = new_buffer;
        }

        buffer[buff_idx++] = (char)ch;

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
    fclose(fp);
    return buffer;
}

int main(void) {
    const char *filename = "source.c";
    int lines = get_linecount(filename);

    int start = 205;
    int end = 226;

    if (start < 1 || end > lines || start > end) {
        fprintf(stderr, "Invalid range [%d to %d] for file with %d lines\n", start, end, lines);
        return -1;
    }

    char *line_content = get_range_content(filename, start, end);
    if (!line_content) {
        fprintf(stderr, "No content found\n");
        return -1;
    }

    printf("%s\n", line_content);
    free(line_content);
    return 0;
}