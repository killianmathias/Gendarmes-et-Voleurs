#include "algo.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct
{
  board_vertex **positions;
  size_t size;
} vector;

void vector_create (vector * self)
{
  if (self == NULL)
    return;
  self->positions = NULL;
  self->size = 0;
}

void vector_destroy (vector * self)
{
  if (self == NULL)
    return;
  free (self->positions);
}

void vector_remove_at (vector * self, size_t index)
{
  if (self == NULL || index >= self->size || self->size == 0)
    return;
  for (size_t i = index; i < self->size - 1; i++)
    self->positions[i] = self->positions[i + 1];
  self->size--;
}

void vector_print (vector * self)
{
  if (self == NULL)
    return;
  for (size_t i = 0; i < self->size; i++)
    printf ("%zu\n", self->positions[i]->index);
  fflush (stdout);
}

typedef struct
{
  board b;
  vector cops;
  vector robbers;
  size_t remaining_turn;
  enum role r;
} game;

void game_create (game * self)
{
  if (self == NULL)
    return;
  board_create (&(self->b));
  vector_create (&(self->cops));
  vector_create (&(self->robbers));
  self->remaining_turn = 0;
  self->r = COPS;
}

void game_destroy (game * self)
{
  if (self == NULL)
    return;
  board_destroy (&(self->b));
  vector_destroy (&(self->cops));
  vector_destroy (&(self->robbers));
}

/*
 * Update positions of either cops or robbers and exit if the moves
 * are invalid
 */
void game_update_position (game * self, size_t *new)
{
  vector *current = self->r == COPS ? &(self->robbers) : &(self->cops);
  if (current->positions != NULL)
    {
      // Check if moves are valid
      for (size_t i = 0; i < current->size; i++)
        if (!board_is_valid_move (&(self->b), current->positions[i]->index,
                                  new[i]))
          {
            fprintf (stderr, "New position is invalid\n");
            exit (1);
          }
    }
  else
    current->positions = calloc (current->size, sizeof (*current->positions));
  for (size_t i = 0; i < current->size; i++)
    {
      current->positions[i] = self->b.vertices[new[i]];
    }
}

// void echanger(int* a, int* b){
//   int temp = *a;
//   *a = *b;
//   *b = temp;
// }

// int partition (int tableau[], int bas, int haut){
//   int pivot = tableau[haut];
//   int i = bas -1;

//   for (int j = bas; j<haut;j++){
//     if (tableau[j]>pivot){
//       i++;
//       echanger(&tableau[i],&tableau[j]);
//     }
//   }
//   echanger(&tableau[i+1],&tableau[haut]);
//   return i + 1;
// }

// void triRapide(int tableau[],int bas, int haut){
//   if (bas <haut){
//     int indicePivot = partition(tableau,bas,haut);
//     triRapide(tableau,bas,indicePivot-1);
//     triRapide(tableau,indicePivot+1,haut);
//   }
// }
// size_t place_cops(game * self){
//   size_t degrees = malloc(self->b.size * sizeof(size_t));
//   for(size_t i = 0; i < self->b.size; i++){
//     degrees[i]=self->b.vertices[i]->degree;
//   }
//   triRapide(degrees,0,self->b.size-1);

// }

size_t place_cops (game * self)
{
  size_t index = 0;
  size_t max_degree = 0;
  size_t max = 0;
  size_t diff_max = INT_MAX;

  if (self->cops.positions[0] == NULL)
    {
      for (size_t i = 0; i < self->b.size; i++)
        {
          if (max_degree < self->b.vertices[i]->degree)
            {
              bool exists = false;
              for (size_t j = 0; j < self->cops.size; j++)
                {
                  if (self->cops.positions[j] == NULL)
                    {
                      break;
                    }
                  if (self->cops.positions[j]->index ==
                      self->b.vertices[i]->index)
                    {
                      exists = true;
                      break;
                    }
                }
              if (!exists)
                {
                  max_degree = self->b.vertices[i]->degree;
                  index = i;
                }
            }
        }
    }
  else
    {
      for (size_t i = 0; i < self->b.size; i++)
        {
          size_t score = 0;
          bool exists = false;
          size_t diff = 0;
          for (size_t j = 0; j < self->cops.size; j++)
            {
              if (self->cops.positions[j] == NULL)
                {
                  break;
                }
              if (self->cops.positions[j]->index == i)
                {
                  exists = true;
                }
              score +=
                board_dist (&self->b, i, self->cops.positions[j]->index);

              if (j == 0)
                {
                  diff +=
                    board_dist (&self->b, i, self->cops.positions[j]->index);
                }
              else if (j == 1)
                {
                  diff -=
                    board_dist (&self->b, i, self->cops.positions[j]->index);
                }
            }
          if (diff < 0)
            {
              diff = -diff;
            }
          score = score + 2 * self->b.vertices[i]->degree;
          if (score > max && !exists)
            {
              max = score;
              index = i;
            }
          else if (score == max && diff < diff_max)
            {
              max = score;
              diff_max = diff;
              index = i;
            }
        }
    }
  return index;
}

unsigned place_robbers (game * self)
{
  //Calculer la somme des distances partant d'un sommet avec chaque sommet déjà occupé par un voleur
  size_t index = 0;
  size_t max_degree = 0;
  size_t max = 0;
  size_t diff_max = INT_MAX;
  for (size_t i = 0; i < self->b.size; i++)
    {
      size_t score = 0;
      bool exists = false;
      size_t diff = 0;
      for (size_t j = 0; j < self->robbers.size; j++)
        {
          if (self->robbers.positions[j] == NULL)
            {
              break;
            }
          if (self->robbers.positions[j]->index == i)
            {
              exists = true;
            }
          score +=
            board_dist (&self->b, i, self->robbers.positions[j]->index);
          if (j == 0)
            {
              diff +=
                board_dist (&self->b, i, self->robbers.positions[j]->index);
            }
          else if (j == 1)
            {
              diff -=
                board_dist (&self->b, i, self->robbers.positions[j]->index);
            }
        }
      for (size_t j = 0; j < self->cops.size; j++)
        {
          if (self->cops.positions[j] == NULL)
            {
              break;
            }
          if (self->cops.positions[j]->index == i)
            {
              exists = true;
            }
          score += board_dist (&self->b, i, self->cops.positions[j]->index);

          if (j == 0)
            {
              diff +=
                board_dist (&self->b, i, self->cops.positions[j]->index);
            }
          else if (j == 1)
            {
              diff -=
                board_dist (&self->b, i, self->cops.positions[j]->index);
            }
        }
      if (diff < 0)
        {
          diff = -diff;
        }
      score = score + 3 * self->b.vertices[i]->degree;
      if (score > max && !exists)
        {
          max = score;
          index = i;
        }
      else if (score == max && diff < diff_max)
        {
          max = score;
          diff_max = diff;
          index = i;
        }
    }
  return index;

}

// size_t compute_next_position_cops(game *self, size_t index) {
//     int meilleur_score = 0;
//     size_t prochaine_case = index;

//     board_vertex *actuelle = self->b.vertices[index];
//     for (size_t i = 0; i < actuelle->degree; i++) {
//         board_vertex *voisine = actuelle->neighbors[i];
//         int score = 0;

//         for (size_t j = 0; j < self->robbers.size; j++) {
//             if (self->robbers.positions[j] == NULL)
//                 continue;

//             int dist_actuelle = board_dist(&self->b, index, self->robbers.positions[j]->index);
//             int dist_voisine = board_dist(&self->b, voisine->index, self->robbers.positions[j]->index);

//             score += (dist_actuelle - dist_voisine) * 10;

//             if (dist_voisine <= 2)
//                 score += 50;
//         }

//         score += voisine->degree * 2;

//         if (score > meilleur_score) {
//             meilleur_score = score;
//             prochaine_case = voisine->index;
//         }
//     }

//     return prochaine_case;
// }

size_t compute_next_position_cops (game * self, size_t index)
{
  if (self->robbers.size != 1)
    {
      size_t score = INT_MAX;
      size_t next_index = index;
      for (size_t i = 0; i < self->b.vertices[index]->degree; i++)
        {
          size_t new_score = 0;
          new_score =
            board_dist (&self->b,
                        self->b.vertices[index]->neighbors[i]->index,
                        self->robbers.positions[0]->index);


          bool already_used = false;
          for (size_t j = 0; j < self->cops.size; j++)
            {
              if (self->b.vertices[index]->neighbors[i]->index ==
                  self->cops.positions[j])
                {
                  already_used = true;
                }
            }
          if (score > new_score && !already_used)
            {
              score = new_score;
              next_index = self->b.vertices[index]->neighbors[i]->index;
            }
          ;
        }
      return next_index;
    }
  else
    {
      size_t score = INT_MAX;
      size_t next_index = index;
      for (size_t i = 0; i < self->b.vertices[index]->degree; i++)
        {
          size_t candidate = self->b.vertices[index]->neighbors[i]->index;
          size_t new_score = board_dist (&self->b, candidate,
                                         self->robbers.positions[0]->index);

          bool already_used = false;
          for (size_t j = 0; j < self->cops.size; j++)
            {
              if (candidate == self->cops.positions[j])
                {
                  already_used = true;  // <-- correction de '==' en '='
                  break;
                }
            }

          if (!already_used && new_score < score)
            {
              score = new_score;
              next_index = candidate;
            }
        }
      return next_index;
    }
}


unsigned compute_next_position_robbers (game * self, size_t index)
{
  unsigned best_index = index;
  int best_score = INT_MIN;

  board_vertex *current = self->b.vertices[index];

  for (size_t i = 0; i < current->degree; i++)
    {
      board_vertex *neighbor = current->neighbors[i];
      int score = 0;
      int min_dist = INT_MAX;
      int total_dist = 0;

      for (size_t j = 0; j < self->cops.size; j++)
        {
          if (self->cops.positions[j] == NULL)
            continue;
          int dist = board_dist (&self->b, neighbor->index,
                                 self->cops.positions[j]->index);
          if (dist < min_dist)
            min_dist = dist;
          total_dist += dist;
        }

      // Maximiser la distance minimum ET moyenne
      score += min_dist * 10 + total_dist;

      // Bonus si le sommet a peu de voisins = potentiellement une cachette
      if (neighbor->degree <= 2)
        score += 15;

      if (score > best_score)
        {
          best_score = score;
          best_index = neighbor->index;
        }
    }

  return best_index;
}

/*
 * Return the initial or next positions of either the cops or the
 * robbers
 */

vector *game_next_position (game * self)
// Placer le premier gendarme sur la première case puis le second le plus loin possible du 1er, puis le 3e le plus loin possible du 2e sur une case non utilisée, etc 
{
  vector *current = self->r == COPS ? &(self->cops) : &(self->robbers);
  if (current->positions == NULL)
    {
      current->positions =
        calloc (current->size, sizeof (*current->positions));
      // Compute initial positions
      for (size_t i = 0; i < current->size; i++)
        {
          if (self->r == COPS)
            {
              current->positions[i] = self->b.vertices[place_cops (self)];
            }
          else
            current->positions[i] = self->b.vertices[place_robbers (self)];
        }
    }
  else
    // Compute next positions
    for (size_t i = 0; i < current->size; i++)
      {
        if (self->r == COPS)
          {
            current->positions[i] =
              self->b.vertices[compute_next_position_cops
                               (self, current->positions[i]->index)];
          }
        else
          {
            current->positions[i] =
              current->positions[i] =
              self->b.vertices[compute_next_position_robbers
                               (self, current->positions[i]->index)];
          }

      }
  return current;
}

/*
 * Remove robbers that are on same vertices as cops and return number
 * of remaining robbers (infinite if no cops)
 */
size_t game_capture_robbers (game * self)
{
  if (self->cops.positions == NULL || self->robbers.positions == NULL)
    return UINT_MAX;
  for (size_t i = 0; i < self->robbers.size; i++)
    for (size_t j = 0; j < self->cops.size; j++)
      if (self->robbers.positions[i] == self->cops.positions[j])
        {
          fprintf (stderr, "Captured robber at position %zu\n",
                   self->robbers.positions[i]->index);
          vector_remove_at (&(self->robbers), i);
          return game_capture_robbers (self);
        }
  return self->robbers.size;
}

size_t *read_positions (size_t len)
{
  size_t *pos = calloc (len, sizeof (*pos));
  for (size_t i = 0; i < len; i++)
    {
      char buffer[100];
      char *msg = fgets (buffer, sizeof buffer, stdin);
      if (msg == NULL || sscanf (buffer, "%zu", &pos[i]) != 1)
        {
          fprintf (stderr, "Error while reading new positions\n");
          exit (1);
        }
    }
  return pos;
}

int main (int argc, const char *argv[])
{
  struct timeval t1;
  gettimeofday (&t1, NULL);
  srand (t1.tv_usec * t1.tv_sec);
  // Initialize game
  game g;
  game_create (&g);

  // Initialize data structures
  if (argc != 3)
    {
      fprintf (stderr,
               "Incorrect number of arguments: ./game filename 0/1\n");
      exit (-1);
    }
  FILE *file = fopen (argv[1], "r");
  if (file == NULL)
    {
      fprintf (stderr, "Error opening input file");
      exit (-1);
    }
  bool success = board_read_from (&(g.b), file);
  fclose (file);
  if (!success)
    {
      fprintf (stderr, "Error parsing input file");
      exit (-1);
    }
  g.cops.size = g.b.cops;
  g.robbers.size = g.b.robbers;
  g.r = atoi (argv[2]);
  g.remaining_turn = g.b.max_turn + 2;

  // Play each turn
  enum role turn = COPS;
  while (game_capture_robbers (&g) != 0 && g.remaining_turn != 0)
    {
      if (g.remaining_turn > g.b.max_turn)
        fprintf (stderr, "Initial positions for %s\n",
                 turn == COPS ? "cops" : "robbers");
      else
        fprintf (stderr, "Turn for %s (remaining: %zu)\n",
                 turn == COPS ? "cops" : "robbers", g.remaining_turn);
      if (turn == g.r)
        {
          // This is the turn of this program to find new positions
          vector *pos = game_next_position (&g);
          vector_print (pos);
        }
      else
        {
          // This is the turn of the adversary program to find new
          // positions
          size_t len = g.r == COPS ? g.robbers.size : g.cops.size;
          size_t *pos = read_positions (len);
          game_update_position (&g, pos);
          free (pos);
        }
      turn = turn == COPS ? ROBBERS : COPS;
      g.remaining_turn--;
    }

  // Finalization
  if (g.robbers.size != 0)
    fprintf (stderr, "Robbers win!\n");
  else
    fprintf (stderr, "Cops win!\n");
  game_destroy (&g);
}
