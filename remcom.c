#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#define VERSION "1.0.0"

void filter_comments(const char *filecontent,size_t length);
void printusage();


int main(int argc, char **argv) {
  if (argc < 2 || argc > 4) {
    printusage();
    return 1;
  }

  int opt;
  while((opt = getopt(argc,argv,"+vh")) != -1){
    switch (opt)
    {
    case 'v':
      printf("Version: %s\n",VERSION);
      return 0;
    case 'h':
      printusage();
      return 0;
    default:
      printusage();
      return 1;
    }
  }
  
  const char *filename = argv[1];
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    perror("Error opening file");
    return 1;
  }
  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if(fsize < 0){
    fprintf(stderr,"Error reading file\n");
    fclose(fp);
    return 1;
  }

  char *content = malloc(fsize + 1);
  if (!content) {
    perror("Memory alloc failed");
    fclose(fp);
    return 1;
  }

  size_t nread = fread(content, 1, fsize, fp);
  fclose(fp);

  if (nread != (size_t)fsize) {
    perror("Failed to read file");
    free(content);
    return 1;
  }

  content[fsize] = '\0';

  int lines = 0;
  for (size_t i = 0; i < nread; i++) {
    if (content[i] == '\n') {
      lines++;
    }
  }

  if (nread > 0 && content[nread - 1] != '\n') {
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
  bool found_start = false;
  for (size_t i = 0; i < nread; i++) {
    if (line == start && found_start == false) {
      range_start = i;
      found_start = true;
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

  filter_comments(content + range_start, range_end - range_start);
  free(content);
  return 0;
}

void filter_comments(const char *content, size_t length) {
  bool in_comment = false;
  bool in_multiline = false;
  bool in_char = false;
  bool in_quote = false;
  bool escaped = false;

  for (size_t i = 0; i < length; i++) {
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

void printusage() {
  printf("remcom %s - strip comments from a source file, optionally by line range\n\n", VERSION);
  printf("Usage:\n");
  printf("  remcom <file> [start_line] [end_line]\n");
  printf("  remcom -v            Show version\n");
  printf("  remcom -h            Show this help message\n\n");
  printf("Arguments:\n");
  printf("  file         Path to the source file to process\n");
  printf("  start_line   First line to include (default: 1)\n");
  printf("  end_line     Last line to include (default: last line of file)\n\n");
  printf("Examples:\n");
  printf("  remcom main.c              Strip comments from the whole file\n");
  printf("  remcom main.c 10 20        Strip comments from lines 10-20 only\n");
}