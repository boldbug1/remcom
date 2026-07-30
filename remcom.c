#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#define VERSION "1.1.1"

typedef struct{
  char *lang;
  char *inline_char;
  char *multiline_start;
  char *multiline_end;
}Comment;

void filter_comments(const char *filecontent,size_t length,Comment comment);
void printusage();



int main(int argc, char **argv) {
  Comment comment = {
    .lang = NULL,
    .inline_char = "//",
    .multiline_start = "/*",
    .multiline_end = "*/"
  };
  int opt;
  while((opt = getopt(argc,argv,"vhl:")) != -1){
    switch (opt)
    {
    case 'v':
      printf("Version: %s\n",VERSION);
      return 0;
    case 'h':
      printusage();
      return 0;
    case 'l':
      comment.lang = optarg;
      break;
    default:
      fprintf(stderr, "Try 'remcom -h' for help.\n");
      return 1;
    }
  }

  int remaining = argc - optind;
  if(remaining < 1 || remaining > 3){
    fprintf(stderr, "Error: expected 1-3 arguments after flags, got %d\n", remaining);
    fprintf(stderr,"Try 'remcom -h' for help\n");
    return 1;
  }
  
  const char *filename = argv[optind];
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

  int start = remaining > 1? atoi(argv[optind + 1]) : 1;
  int end = remaining > 2 ? atoi(argv[optind + 2]) : lines;

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

  //python
  if (comment.lang != NULL && strcasecmp(comment.lang, "python") == 0) {
        comment.inline_char = "#";
        comment.multiline_start = "\"\"\"";
        comment.multiline_end = "\"\"\"";
    }
  filter_comments(content + range_start, range_end - range_start,comment);
  free(content);
  return 0;
}

void filter_comments(const char *content, size_t length,Comment comment) {
  bool in_comment = false;
  bool in_multiline = false;
  bool in_char = false;
  bool in_quote = false;
  bool escaped = false;

  size_t single_len = strlen(comment.inline_char);
  size_t multi_len_start = strlen(comment.multiline_start);
  size_t multi_len_end = strlen(comment.multiline_end);

  for (size_t i = 0; i < length; i++) {
    char c = content[i];
    
    //comment end check
    if(in_comment && c == '\n'){
      in_comment = false;
      putchar(c);
      continue;
    }
    
    //multiline end check
    if(in_multiline && (i+multi_len_end <= length) && strncmp(&content[i],comment.multiline_end,multi_len_end) == 0){
      in_multiline = false;
      i+=multi_len_end -1;
      continue;
    }
    //inline
    if(!in_quote && !in_char && !in_comment && !in_multiline && !escaped && (i + single_len <= length) && strncmp(&content[i],comment.inline_char,single_len) == 0){
      in_comment = true;
      i+=single_len -1;
      continue;
    }
    
    //multiline start check
    if(!in_quote && !in_char && !in_comment && !in_multiline && !escaped && (i+ multi_len_start <= length) && strncmp(&content[i],comment.multiline_start,multi_len_start) == 0){
      in_multiline = true;
      i+= multi_len_start - 1;
      continue;
    }
    
    if (escaped) {
      escaped = false;
    } else if (c == '\\' && (in_quote || in_char)) {
      escaped = true;
    } else if (c == '"' && !in_char && !in_comment && !in_multiline) {
      in_quote = !in_quote;
    } else if (c == '\'' && !in_quote && !in_comment && !in_multiline) {
      in_char = !in_char;
    }

    if(!in_comment && !in_multiline){
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