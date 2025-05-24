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
  // Si plusieurs voleurs, on suit la stratégie classique : se rapprocher du voleur le plus proche
  if (self->robbers.size != 1)
    {
      size_t best_position = index;     // Par défaut, on reste à sa place
      int best_distance = INT_MAX;      // On cherche à minimiser la distance au voleur

      // Parcourir tous les voisins actuels du gendarme
      board_vertex *current_vertex = self->b.vertices[index];
      for (size_t i = 0; i < current_vertex->degree; i++)
        {
          size_t candidate = current_vertex->neighbors[i]->index;

          // Calculer la distance entre ce voisin et le voleur (on prend le premier voleur)
          int distance_to_robber = board_dist (&self->b, candidate,
                                               self->robbers.
                                               positions[0]->index);

          // Vérifier que ce voisin n'est pas déjà occupé par un autre gendarme
          bool occupied = false;
          for (size_t j = 0; j < self->cops.size; j++)
            {
              if (self->cops.positions[j]->index == candidate)
                {
                  occupied = true;
                  break;
                }
            }

          // Si ce voisin n'est pas occupé et est plus proche du voleur, on le choisit
          if (!occupied && distance_to_robber < best_distance)
            {
              best_distance = distance_to_robber;
              best_position = candidate;
            }
        }

      return best_position;
    }

  // Si un seul voleur, on essaye de l'encercle en occupant ses voisins
  else
    {
      size_t robber_pos = self->robbers.positions[0]->index;
      board_vertex *robber_vertex = self->b.vertices[robber_pos];

      size_t neighbors_count = robber_vertex->degree;
      size_t neighbors[neighbors_count];

      // Récupérer les indices des voisins du voleur
      for (size_t i = 0; i < neighbors_count; i++)
        {
          neighbors[i] = robber_vertex->neighbors[i]->index;
        }

      // Vérifier quels voisins sont déjà occupés par des gendarmes
      bool occupied_neighbors[neighbors_count];
      for (size_t i = 0; i < neighbors_count; i++)
        {
          occupied_neighbors[i] = false;
          for (size_t j = 0; j < self->cops.size; j++)
            {
              if (self->cops.positions[j]->index == neighbors[i])
                {
                  occupied_neighbors[i] = true;
                  break;
                }
            }
        }

      board_vertex *current_vertex = self->b.vertices[index];
      size_t best_position = index;     // Reste sur place par défaut
      int best_score = INT_MAX;

      // On regarde chaque voisin du gendarme actuel
      for (size_t i = 0; i < current_vertex->degree; i++)
        {
          size_t candidate = current_vertex->neighbors[i]->index;

          // Vérifier si ce voisin est déjà occupé par un autre gendarme
          bool occupied = false;
          for (size_t j = 0; j < self->cops.size; j++)
            {
              if (self->cops.positions[j]->index == candidate)
                {
                  occupied = true;
                  break;
                }
            }
          if (occupied)
            {
              continue;         // On saute ce voisin s'il est occupé
            }

          // Chercher la distance minimale entre ce candidat et un voisin libre du voleur
          int min_dist_to_free_neighbor = INT_MAX;
          for (size_t v = 0; v < neighbors_count; v++)
            {
              if (!occupied_neighbors[v])
                {
                  int dist = board_dist (&self->b, candidate, neighbors[v]);
                  if (dist < min_dist_to_free_neighbor)
                    {
                      min_dist_to_free_neighbor = dist;
                    }
                }
            }

          // Si tous les voisins sont occupés, on se rapproche directement du voleur
          if (min_dist_to_free_neighbor == INT_MAX)
            {
              min_dist_to_free_neighbor =
                board_dist (&self->b, candidate, robber_pos);
            }

          // Choisir la position qui minimise cette distance
          if (min_dist_to_free_neighbor < best_score)
            {
              best_score = min_dist_to_free_neighbor;
              best_position = candidate;
            }
        }

      return best_position;
    }
}


unsigned compute_next_position_robbers (game * self, size_t index)
{
  unsigned best_index = index;
  int best_score = INT_MIN;

  board_vertex *current = self->b.vertices[index];

  // On considère aussi rester sur place comme une option possible
  // Donc on ajoute le point actuel en plus des voisins
  size_t candidates_count = current->degree + 1;
  board_vertex *candidates[candidates_count];

  candidates[0] = current;
  for (size_t i = 0; i < current->degree; i++)
    {
      candidates[i + 1] = current->neighbors[i];
    }

  for (size_t c = 0; c < candidates_count; c++)
    {
      board_vertex *pos = candidates[c];

      int min_dist_to_cop = INT_MAX;
      int total_dist_to_cops = 0;

      for (size_t j = 0; j < self->cops.size; j++)
        {
          if (self->cops.positions[j] == NULL)
            continue;
          int dist =
            board_dist (&self->b, pos->index, self->cops.positions[j]->index);
          if (dist < min_dist_to_cop)
            min_dist_to_cop = dist;
          total_dist_to_cops += dist;
        }

      // Score basé principalement sur la distance minimale, 
      // mais aussi la distance totale et la connectivité (plus de voisins = plus de mouvements futurs)
      int score = min_dist_to_cop * 12 + total_dist_to_cops;

      // Bonus pour les positions avec peu de voisins (cachettes)
      if (pos->degree <= 2)
        score += 10;

      // Bonus pour les positions bien connectées (éviter de se coincer)
      if (pos->degree >= 4)
        score += 5;

      // Bonus à rester sur place s'il est sûr (plus sûr que bouger)
      if (pos->index == index)
        score += 8;

      // Petite pénalité si trop proche d’un gendarme (moins de 2 cases)
      if (min_dist_to_cop < 2)
        score -= 25;

      if (score > best_score)
        {
          best_score = score;
          best_index = pos->index;
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
