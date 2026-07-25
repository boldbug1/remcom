#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

void filter_comments(const char *filecontent);
void printusage();

int main(int argc, char **argv) {
  if (argc < 2 || argc > 4) {
    printusage();
    return 1;
  }
  const char *filename = argv[1];
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    perror("Error opening file");
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *content = malloc(fsize + 1);
  if (!content) {
    perror("Memory alloc failed");
    fclose(fp);
    return 1;
  }

  size_t nread = fread(content,1,fsize,fp);
  fclose(fp);

  if (nread != (size_t)fsize) {
    perror("Failed to read file");
    free(content);
    return 1;
  }

  content[fsize] = '\0';

  int last_non_newline = -1;
  int lines = 0;
  for (int i = 0; content[i]; i++) {
    if (content[i] == '\n') {
      lines++;
    } else {
      last_non_newline = i;
    }
  }

  if (last_non_newline >= 0) {
    lines++;
  }

  int start = argc > 2 ? atoi(argv[2]) : 1;
  int end = argc > 3 ? atoi(argv[3]) : lines;

  if (start < 1 || end > lines || start > end) {
    fprintf(stderr, "Invalid range [%d to %d] for file with %d lines\n", start,
            end, lines);
    free(content);
    return 1;
  }

  int line = 1;
  size_t range_start = 0, range_end = nread;
  for(size_t i= 0;i < nread;i++){
    if (line == start && range_start == 0) {
      range_start = i;
    }
    if (content[i] == '\n') {
      line++;
    }
    if (line > end) {
      range_end = i;
      break;
    }
    }

    content[range_end] = '\0';

    filter_comments(content + range_start);

    return 0;
}

void filter_comments(const char *content) {
  bool in_comment = false;
  bool in_multiline = false;
  bool in_char = false;
  bool in_quote = false;
  bool escaped = false;

  for (size_t i = 0; content[i]; i++) {
    char c = content[i];
    if (escaped) {
      escaped = false;
    } else if (c == '\\') {
      escaped = true;
    } else if (c == '"' && !in_char && !in_comment && !in_multiline) {
      in_quote = !in_quote;
    } else if (c == '\'' && !in_quote && !in_comment && !in_multiline) {
      in_char = !in_char;
    }

    if (!in_quote && !in_char && !in_multiline && c == '/' &&
        content[i + 1] == '/') {
      in_comment = true;
      i++;
      continue;
    }
    if (!in_quote && !in_char && !in_comment && c == '/' &&
        content[i + 1] == '*') {
      in_multiline = true;
      i++;
      continue;
    }
    if (in_comment && c == '\n') {
      in_comment = false;
    }
    if (in_multiline && c == '*' && content[i + 1] == '/') {
      in_multiline = false;
      i++;
      continue;
    }
    if (!in_comment && !in_multiline) {
      putchar(c);
    }
  }
}

void printusage() { printf("Usage : remcom <file> start_line end_line\n"); }