This is my player which first second in CodeCup competition 2018 with the
game "blackhole".

For an overview of the rules of game:
[blackhole-rules](https://www.codecup.nl/rules_blackhole.php)

# *AI description*

I used Monte Carlo Tree Search (MCTS) as the basic algorithm to find the best
move to play.

# *Position representation and conventions*

a position consist of those data:
- player to move
- how many turns played
- two bitmasks encoding the used values for each player
- a bitmask of the filled places
- non hole places count(will be explaned bellow)
- list of tiles

*non hole place means an empty place that has at least 1 empty neighbor*.

a tile itself is composed of:
- the value it holds
- delta (for red) of values surrounding the place
- empty fields around tile count

Position is considered in the filling phase if there is no non holes places

# *Enhancements*

- I used move grouping by place to look for place first then value to reduce move generation and to expand tree just on promising parts.

- playout was done not until the game end but just until there is non hole places.
when there is no non hole places the game result is computed from the deltas on
current holes. i.e:
if deltas = (delta1, delta2, .., deltan) then
game value = delta(index) where index = (n-1)/2 (integer division)

- I used an epsilon greedy instead of UCT for tree selection

- One of the big challenges was to model your oponent correctly. I adopted an approach
of adding the average of deltas with a weight that increases regarding the game value
so the player acts normaly with strong players but whenever the game value is big it
starts to be greedy. May be the wieght can be optimized further but it did the trick.

- On a filling position I just choose the worse place for me and I fill it my very
 least value.

- The playout policy was quite simple choose either to explore space by putting your
  largest value at the tile having more empty neighbors or fill the worse place for
  you. first choice is favoured at the begining second favoured at the end.

- When a move get more than half of the maximum of simulations I break the search which help to stop re-checking trivial good moves and save time for later use. 

# *Time management*

A player has 5 seconds in total. I used it like this:
The player is stopped with max simulations only when the total is less than 4 seconds but after that it divide remaining time by the number of remaining moves.

This management was done like this because game is somewhat decided from the very begining sometimes.


# *Building*

you need cmake to build this player. in a terminal:

```
cd path/to/blackhole/sources
mkdir build
cd build
cmake ..
make
```
