#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils/utils.h"

/*
 * TODO:
 * add an invalid command error on invalid commands
 */

#pragma once;

#define echo(...) printf(__VA__ARGS__)
#define flushbuf setbuf(stdout, NULL)
int main(int argc __attribute__((unused)), string argv[] __attribute__((unused)))
{

  // Read the PATH environment variables
  const char *pathvar __attribute__((unused)) = getenv("PATH") ? getenv("PATH") : NULL;

  // Wait for user input
  char input[100];
  while (1)
  {
    flushbuf;
    printf("\xCE\xBB ");
    printf("DEBUG: Before fgets\n");
    if (fgets(input, 100, stdin) == NULL)
    {
      break;
    }
    printf("DEBUG: After fgets, input: '%s'\n", input);

    // Remove trailing newline character, if present
    input[strcspn(input, "\n")] = '\0';
    printf("DEBUG: After strcspn, input: '%s'\n", input);

    // read and evaluate commands
    if (strlen(input) == 0)
      continue;

    printf("DEBUG: Input length > 0, input: '%s'\n", input);

    if (strncmp(input, "exit", 4) == 0)
    {
      if (strlen(input) > 4)
      {
        printf("exited");
        const char *args = slice_from(input, 4);
        size_t n;
        char **arguments = split(args, ' ', &n);
        int code = arguments[0][0] ? arguments[0][0] : 0;
        exit(code);
      }
      else
      {
        exit(0);
      }
    }

    if (strncmp(input, "echo ", 5) == 0)
    {
      printf("DEBUG: Inside echo command block\n");
      // get the arguments
      const char *args =
          slice_from(input, 5); // Changed index to 5 to skip "echo "
      printf("DEBUG: After slice_from, args: '%s'\n", args);
      printf("%s\n", args);
      continue;
    }

    if (strncmp(input, "type ", 5) == 0)
    {
      const char *type_args_str = slice_from(input, 5);
      size_t n;
      char **args = split(type_args_str, ' ', &n);

      size_t length =
          len_argv((const char **)args); // Cast to const char** for len_argv

      for (size_t i = 0; i < length; i++) // Changed loop variable to size_t
      {
        const char *command_name = args[i]; // Corrected to use args[i]
        if (isCommand(command_name))
        {
          printf("%s is a shell builtin\n",
                 command_name); // Added newline for better output
        }
        else
        {
          // check if the command is found in /usr/lib/
          // if it is found log out the location
          char *executable = get_executable(command_name);
          if (executable == NULL)
          {
            printf("%s not found\n", command_name); // Added output for not found commands
          }
          else
          {
            printf("%s found in %s\n", command_name, executable);
          }
        }
        free(args[i]); // Free each argument string
      }
      free(args); // Free the array of pointers
      continue;   // Continue to next prompt after handling type command
    }

    if (strncmp(input, "pwd", 3) == 0)
    {
      trim(input);
      // check if the input is an invalid pwd command
      if (strlen(input) == 3)
      {
        // get the current directory
        char directory[100];
        int state = get_working_directory(directory);
        if (state)
        {
          printf("%s\n", directory);
          continue;
        }
      }
    }

    if (strncmp(input, "cd ", 3) == 0)
    {
      // get the arguments
      const char *in = slice_from(input, 3);
      size_t n;
      char **arguments = split(in, ' ', &n);
      size_t length = len_argv((const char **)arguments);
      if (length > 1 || length == 0)
      {
        printf("cd: string not in pwd: %s", arguments[0]);
        continue;
      }

      const char *path = arguments[0];

      const char *dir = replaceSubstring(path, "~", HOME);

      change_directory(dir);
    }

    // execute any other commands that are not shell built in's
    int execute = run_executable(input);
    if (execute == 0)
    {
      char errMessage[150];
      sprintf(errMessage, "command not found: %s", input);
    }
  }
  return 0;
}
