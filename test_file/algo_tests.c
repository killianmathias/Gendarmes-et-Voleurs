#include "algo.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { const char *message = test(); \
    if (message) printf ("Test %d failed: %s\n", tests_index, message); \
    else { tests_pass++; } tests_run++; } while (0)
int tests_pass, tests_run, tests_index;

static char *test_board_Floyd_Warshall_chain ()
{
  board b;
  board_create (&b);

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 3\n0 0\n0 0\n0 0\n" "Edges: 2\n0 1\n1 2\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (&b, file);
  board_Floyd_Warshall (&b);
  mu_assert ("error, failure reading board", read == true);
  mu_assert ("error, incorrect distance", board_dist (&b, 0, 0) == 0
             && board_dist (&b, 0, 1) == 1 && board_dist (&b, 0, 2) == 2
             && board_dist (&b, 1, 2) == 1);
  mu_assert ("error, incorrect next vertex", board_next (&b, 0, 0) == 0
             && board_next (&b, 0, 1) == 1 && board_next (&b, 0, 2) == 1
             && board_next (&b, 1, 2) == 2);

  board_destroy (&b);

  return NULL;
}



char *(*tests_functions[]) () = {
  test_board_Floyd_Warshall_chain
};

int main (int argc, const char *argv[])
{
  size_t n = sizeof (tests_functions) / sizeof (tests_functions[0]);
  if (argc == 1)
    {
      for (tests_index = 0; (size_t) tests_index < n; tests_index++)
        mu_run_test (tests_functions[tests_index]);
      if (tests_run == tests_pass)
        printf ("All %d tests passed\n", tests_run);
      else
        printf ("Tests passed/run: %d/%d\n", tests_pass, tests_run);
    }
  else
    {
      tests_index = atoi (argv[1]);
      if (tests_index < 0)
        printf ("%zu\n", n);
      else if ((size_t) tests_index < n)
        {
          mu_run_test (tests_functions[tests_index]);
          if (tests_run == tests_pass)
            printf ("Test %d passed\n", tests_index);
        }
    }
}
