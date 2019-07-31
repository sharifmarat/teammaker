# teammaker
An application to generate volleyball teams for a group of players.

# How does it work
The app requires list of players. It makes only full teams with 6 players. Extra players (up to 6) are ignored. Example: 5 full teams will be generated from 33 players.

Each player must have known level, position and gender.

The app randomly makes teams and optimizes scoring function. Scoring function is a sum of difference between all team levels.

If the list with players contains mixed genders, then the program will try to balance distribution between male and female players.

# Format of the input file
```
<player-name>, <gender>, <level>, <positions> <EOL>
```
- `player-name` is [aA-zZ ];
- `gender` is either `M` or `F`;
- `level` is an integer number between 1 and 10, 10 is the highest skill level;
- `positions` is a list of positions separated by slash '/'. Possible positions: `mid`, `set`, `dia`, `out`, `libero`, `all`, `any`.

Example of an input file, see `players.example`:
```
player1, F, 3, dia
player2, M, 6, Out
player3, F, 4,  Set
player4, M, 1,  Out/dia
player5, M, 5, mid/Out/Dia
player6, M, 5,  out
player7, F, 3, out/Set
player8, M, 3, mid
player9, M, 10, Mid
player10, F, 4, Set
player11, M, 9,  dia/Out
player12, M, 6,  Mid

```

# Known limitations
1. Only first player positions are taken into account.
2. Each team must include 1 setter, 1 dia, 2 outs, 2 middles. A libero is equal to a middle.
3. Program proposes teams once the better or similar score has been found. It means that it will spam you with many possible teams until stopped.
4. Program does not check if an already found  proposal has been already found before printing it to stdout.
5. If position is `all` or `any`, then this player is added as a middle.

# Building
```
mkdir build
cd build
cmake ..
make
```

# Running
```
cd build
./teammaker ../players.example
```

Possible output:
```
Found better score = 1
team,set,dia,mid,mid,out,out,score,ladies
team1,player3 (F),player11,player8,player5,player7 (F),player2,30,2
team2,player10 (F),player1 (F),player12,player9,player4,player6,29,2
```
