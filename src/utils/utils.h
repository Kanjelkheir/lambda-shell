#include <inttypes.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#pragma once

#define HOME getenv("HOME")

typedef char *string;

const char *commands[] = {"exit", "echo", "type", "clear", "pwd"};

typedef enum
{
  CMD_EXIT,
  CMD_ECHO
} CmdType;

typedef struct
{
  CmdType type;
  char **arguments;
} command;

// command *new_command(CmdType type, char **arguments)
// {
//     return &command{
//         .CmdType = type,
//         arguments : arguments,
//     };
// }

typedef struct
{
  char *data;
  size_t index;
} trim_t;

#define BLOCK_TRIM_T sizeof(trim_t)

char *get_data(trim_t *data)
{
  if (!data || !data->data)
    return NULL;

  size_t len = strlen(data->data + data->index);
  char *result = malloc(len + 1);
  if (!result)
    return NULL;

  strcpy(result, data->data + data->index);
  return result;
}

// trim_t *trim(char *text)
// {
//     trim_t *result = malloc(sizeof(trim_t));
//     if (!result)
//         return NULL;

//     result->data = malloc(strlen(text) + 1);
//     if (!result->data)
//     {
//         free(result);
//         return NULL;
//     }
//     strcpy(result->data, text);

//     size_t index = 0;
//     while (result->data[index] == ' ')
//     {
//         index++;
//     }
//     result->index = index;

//     char *trimmed_data = get_data(result);
//     free(result->data);          // Free the original copy
//     result->data = trimmed_data; // Store the trimmed data

//     return result;
// }
void print_tokens(string tokens)
{
  // strtok modifies the input string, so we need a copy if the original needs
  // to be preserved. However, for this function's purpose (printing tokens),
  // modifying the copy is acceptable.
  char *tokens_copy =
      strdup(tokens); // Duplicate the string to avoid modifying the original
  if (tokens_copy == NULL)
  {
    perror("Failed to duplicate string for tokenization");
    return;
  }

  string token = strtok(tokens_copy, " ");

  while (token != NULL)
  {
    printf("%s ", token);
    token = strtok(NULL, " "); // Use NULL for subsequent calls to strtok
  }
  free(tokens_copy); // Free the duplicated string
}

/* return NULL-terminated array of *copies* of words; caller frees */
char **split(const char *text, char sep, size_t *count_out)
{
  if (!text)
  {
    *count_out = 0;
    return NULL;
  }

  /* count words */
  size_t n = 0;
  const char *p = text;
  do
  {
    while (*p == sep)
      ++p;
    if (*p)
      ++n;
    while (*p && *p != sep)
      ++p;
  } while (*p);

  /* allocate array of pointers + 1 sentinel */
  char **arr = malloc((n + 1) * sizeof *arr);
  if (!arr)
  {
    *count_out = 0;
    return NULL;
  }

  /* duplicate each word */
  size_t idx = 0;
  p = text;
  while (*p)
  {
    while (*p == sep)
      ++p;
    if (!*p)
      break;

    const char *start = p;
    while (*p && *p != sep)
      ++p;

    size_t len = p - start;
    arr[idx] = malloc(len + 1);
    if (!arr[idx])
    { /* cleanup on failure */
      for (size_t j = 0; j < idx; ++j)
        free(arr[j]);
      free(arr);
      *count_out = 0;
      return NULL;
    }
    memcpy(arr[idx], start, len);
    arr[idx][len] = '\0';
    ++idx;
  }
  arr[idx] = NULL; /* sentinel */
  *count_out = n;
  return arr;
}

/* return pointer to char at position idx */
const char *slice_from(const char *s, size_t idx)
{
  if (!s)
    return NULL;
  size_t len = strlen(s);
  if (idx > len)
    idx = len; /* clamp */
  return s + idx;
}

size_t len_argv(const char **arr)
{
  size_t n = 0;
  while (arr && arr[n])
    ++n;
  return n;
}

// bool commandType(command *cmd)
// {
//     switch (*cmd)
//     {
//     case CMD_EXIT:
//         return true;
//     case CMD_ECHO:
//         return true;
//     default:
//         return false;
//     }
// }

// char *trim(char *str)
// {
//   if (str == NULL)
//   {
//     return NULL;
//   }

//   int len = strlen(str);
//   char *new_str = (char *)malloc(len + 1); // Max possible length
//   if (new_str == NULL)
//   {
//     return NULL;
//   }

//   int j = 0;
//   for (int i = 0; i < len; i++)
//   {
//     if (!isspace((unsigned char)str[i]))
//     {
//       new_str[j++] = str[i];
//     }
//   }
//   new_str[j] = '\0'; // Null-terminate the new string

//   // Reallocate to the actual size to save memory
//   char *final_str = (char *)realloc(new_str, j + 1);
//   if (final_str == NULL)
//   {
//     // If realloc fails, we still have new_str, so return that.
//     // This is a defensive measure, realloc to smaller size usually doesn't
//     // fail.
//     return new_str;
//   }

//   return final_str;
// }

bool isCommand(const char *command)
{
  size_t num_commands = sizeof(commands) / sizeof(commands[0]);
  for (size_t i = 0; i < num_commands; i++)
  {
    if (strcmp(command, commands[i]) == 0)
    {
      return true;
    }
  }

  return false;
}

char *get_executable(const char *command_name)
{
  char *path_env = getenv("PATH");
  if (!path_env)
  {
    return NULL;
  }

  size_t n;
  char **paths = split(path_env, ':', &n);
  if (!paths)
  {
    return NULL;
  }

  for (size_t i = 0; i < n; ++i)
  {
    DIR *dir = opendir(paths[i]);
    if (!dir)
    {
      continue;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, command_name) == 0)
      {
        // Found a matching file, now check if it's executable
        char *full_path = malloc(strlen(paths[i]) + 1 + strlen(entry->d_name) + 1);
        if (!full_path)
        {
          closedir(dir);
          // Free previously allocated paths
          for (size_t j = 0; j < n; ++j)
          {
            free(paths[j]);
          }
          free(paths);
          return NULL;
        }
        sprintf(full_path, "%s/%s", paths[i], entry->d_name);

        if (access(full_path, X_OK) == 0)
        {
          closedir(dir);
          // Free previously allocated paths
          for (size_t j = 0; j < n; ++j)
          {
            free(paths[j]);
          }
          free(paths);
          return full_path; // Found and executable
        }
        free(full_path); // Not executable, free and continue search
      }
    }
    closedir(dir);
  }

  // Free all allocated path strings
  for (size_t i = 0; i < n; ++i)
  {
    free(paths[i]);
  }
  free(paths);

  return NULL; // Not found or not executable
}

/**
 *find the executable of the input file in the PATH environment variable
 *if the executable is found then run it if it's not found return NULL
 */
int run_executable(const char *file_name)
{
  char *executable = get_executable(file_name);
  if (executable == NULL)
  {
    return 0;
  }

  if (system(executable) == -1)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

/*
 * trims a string whitespaces
 */
void trim(char *s)
{
  int i = 0, j = 0;

  // Skip leading spaces
  while (s[i] == ' ')
    i++;

  // Shift characters to remove leading spaces
  while ((s[j++] = s[i++]))
    ;

  // Null-terminate the string
  s[j - 1] = '\0';
}

/*
 * Returns the current working directory
 */
unsigned int get_working_directory(char *buf)
{
  if (getcwd(buf, sizeof(buf)) != NULL)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*
 * Change the current directory into the argument
 */
void change_directory(const char *dir)
{
  if (chdir(dir) == 0)
  {
    return;
  }
  else
  {
    printf("%s\n", strerror(errno));
    return;
  }
}

/*
 * replace a substring with another string inside a string
 */
char *replaceSubstring(const char *str, const char *oldSub, const char *newSub)
{
  char *result;
  int i, count = 0;
  int newLen = strlen(newSub);
  int oldLen = strlen(oldSub);

  // Count occurrences of oldSub
  for (i = 0; str[i] != '\0'; i++)
  {
    if (strstr(&str[i], oldSub) == &str[i])
    {
      count++;
      i += oldLen - 1; // Move past the old substring
    }
  }

  // Allocate memory for the new string
  result = (char *)malloc(i + count * (newLen - oldLen) + 1);
  i = 0;

  while (*str)
  {
    if (strstr(str, oldSub) == str)
    {
      strcpy(&result[i], newSub);
      i += newLen;
      str += oldLen;
    }
    else
    {
      result[i++] = *str++;
    }
  }
  result[i] = '\0';
  return result;
}
