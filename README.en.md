The project is mainly based on three files:

# Project - Police and thieves behavior in a graph

- ** Server.py **: manages the graphical interface and loads the different game platforms (`.txt`) files located in the` test_file` folder.
  The objective was to design the placement and displacement behavior of police and thieves within a graph. ## Performance assessment

## Project structure

This script automatically tests all the game platforms against all the opponents available in the `Bin` file, for each role (gendarmes 👮 and thieves 🥷), and displays an average percentage of success according to the algorithm faced.- ** algo.c **: contains the data structures as well as the basic algorithms necessary for our implementation.
This project was carried out during my second year of university studies.

- ** Game.c **: implements the game mechanics, such as the placement and displacement of the characters, as well as the capture of thieves.

It was a question of putting into practice the algorithmic concepts discussed throughout the semester, through a project combining reflection and analysis.

To assess the project performance against different algorithms, I used a shell script named ** Perf.sh **.
