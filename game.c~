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
  size_t *targets;
  bool **zone;
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
  self->targets = malloc (self->robbers.size * sizeof (size_t));
  self->zone = malloc (self->cops.size * sizeof (*(self->zone)));
}

void game_destroy (game * self)
{
  if (self == NULL)
    return;
  board_destroy (&(self->b));
  vector_destroy (&(self->cops));
  vector_destroy (&(self->robbers));
  free (self->targets);
  for (size_t i = 0; i < self->b.cops; i++)
    {
      free (self->zone[i]);
    }
  free (self->zone);
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

void compute_zones (game * self)
{
  if (self->cops.size != 0)
    {
      size_t number_of_vertices_per_zone = self->b.size / self->cops.size;
      size_t rest = self->b.size % self->b.cops;
      size_t *nb_of_vertices = malloc (self->b.size * sizeof (size_t));
      for (size_t i = 0; i < self->cops.size; i++)
        {
          self->zone[i] = malloc (self->b.size * sizeof (bool));
          nb_of_vertices[i] = 0;
        }



      for (size_t j = 0; j < self->b.size; j++)
        {
          size_t index_cops = 0;
          size_t min = INT_MAX;
          for (size_t i = self->b.cops; i-- > 0;)
            {

              self->zone[i][j] = false;
              if (board_dist (&self->b, self->cops.positions[i]->index, j) <
                  min && nb_of_vertices[i] <= number_of_vertices_per_zone)
                {
                  min =
                    board_dist (&self->b, self->cops.positions[i]->index, j);
                  index_cops = i;

                }
            }
          self->zone[index_cops][j] = true;
          nb_of_vertices[index_cops]++;
        }
    }

  // for (size_t i = 0; i < self->cops.size; i++)
  //   {
  //     printf ("Tableau de zone du gendarme %d : [", i);
  //     for (size_t j = 0; j < self->b.size; j++)
  //       {
  //         printf ("%d,", self->zone[i][j]);
  //       }
  //     printf ("]\n");
  //   }
}


bool is_in_gamezone (game * self, size_t cops, size_t index)
{
  return self->zone[cops][index];
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
  size_t cops = 0;

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
                  cops = j;
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

size_t compute_targets (game * self, size_t index)
{
  size_t target_index = 0;
  size_t max_dist = INT_MAX;
  for (size_t j = 0; j < self->robbers.size; j++)
    {
      bool already_exists = false;
      size_t dist = board_dist (&self->b, self->cops.positions[index]->index,
                                self->robbers.positions[j]->index);
      if (self->robbers.size == self->b.robbers)
        {
          for (size_t i = 0; i < self->robbers.size; i++)
            {
              if (self->targets[i] != NULL)
                {
                  if (self->targets[i] == j)
                    {
                      already_exists = true;
                    }
                }
            }
        }
      if (dist < max_dist && !already_exists
          && is_in_gamezone (self, index, j))
        {
          target_index = j;
        }
    }
  return target_index;
}


size_t compute_next_position_cops(game * self, size_t index) {
    if (self->remaining_turn == self->b.max_turn) {
        self->targets[index] = compute_targets(self, index);
    }

    size_t current_pos = self->cops.positions[index]->index;
    size_t target_pos = self->robbers.positions[self->targets[index]]->index;
    size_t next_pos = board_next(&self->b, current_pos, target_pos);

    // Si le mouvement reste dans la zone, on y va
    if (is_in_gamezone(self, index, next_pos)) {
        return next_pos;
    }

    // Sinon, recalculer un autre voleur dans la zone
    self->targets[index] = compute_targets(self, index);
    return board_next(&self->b, current_pos, self->robbers.positions[self->targets[index]]->index);
}


unsigned compute_next_position_robbers(game * self, size_t index) {
    size_t current_pos = self->robbers.positions[index]->index;
    board_vertex *v = self->b.vertices[current_pos];
    size_t best_pos = current_pos;
    int max_min_dist = -1;

    for (size_t i = 0; i < v->degree; i++) {

        size_t neighbor = v->neighbors[i];
        int min_dist = INT_MAX;

        // Calculer la distance minimale à tous les gendarmes
        for (size_t j = 0; j < self->cops.size; j++) {
            if (self->cops.positions[j] != NULL) {
                int dist = board_dist(&self->b, neighbor, self->cops.positions[j]->index);
                if (dist < min_dist)
                    min_dist = dist;
            }
        }

        // Choisir le voisin avec la meilleure distance minimale
        if (min_dist > max_min_dist) {
            max_min_dist = min_dist;
            best_pos = neighbor;
        }
    }

    return best_pos;
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
      compute_zones (self);
    }
  else
    // Compute next positions
    for (size_t i = 0; i < current->size; i++)
      {
        if (self->r == COPS)
          {
            // printf ("Cible du gendarme %d : Voleur à position %d\n", i,
            //         self->robbers.positions[self->targets[i]]->index);
            current->positions[i] =
              self->b.vertices[compute_next_position_cops (self, i)];

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
          self->targets[i] = compute_targets (self, i);
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
