#include "strings.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <cstdlib>
#include <unordered_set>
#include <numeric>
#include <limits>

enum Level {
  LevelMin = 1,
  LevelMax = 10
};

enum class Position {
  Mid,
  Libero, // same as mid, but always plays back row
  Set,
  Out,
  Dia
};

struct Player {
  std::string name;
  Level level;
  bool is_female;
  std::vector<Position> positions;
};

struct Team {
  const Player* set{nullptr};
  const Player* dia{nullptr};
  const Player* mid[2]{nullptr, nullptr};
  const Player* out[2]{nullptr, nullptr};
};

bool Try(Team& team, const Player* player) {
  const auto pos = player->positions.front(); // TODO: only first position is considered for now.
  switch(pos) {
    case Position::Mid:
    case Position::Libero:
      if (team.mid[0] == nullptr) {
        team.mid[0] = player;
        return true;
      } else if (team.mid[1] == nullptr) {
        team.mid[1] = player;
        return true;
      } else {
        return false;
      }
    case Position::Set:
      if (team.set == nullptr) {
        team.set = player;
        return true;
      } else {
        return false;
      }
    case Position::Out:
      if (team.out[0] == nullptr) {
        team.out[0] = player;
        return true;
      } else if (team.out[1] == nullptr) {
        team.out[1] = player;
        return true;
      } else {
        return false;
      }
    case Position::Dia:
      if (team.dia == nullptr) {
        team.dia = player;
        return true;
      } else {
        return false;
      }
  }
}

std::vector<Position> parse_positions(const std::string& str) {
  std::vector<Position> positions;
  const auto tokens = tokenize(str, '/', true);
  for (auto token: tokens) {
    const auto pos = tolower(token);
    if (pos == "all" || pos == "any" || pos == "joker") {
      return {Position::Mid, Position::Out, Position::Dia, Position::Set};
    } else if (pos == "mid") {
      positions.push_back(Position::Mid);
    } else if (pos == "set") {
      positions.push_back(Position::Set);
    } else if (pos == "out") {
      positions.push_back(Position::Out);
    } else if (pos == "dia") {
      positions.push_back(Position::Dia);
    } else if (pos == "libero") {
      positions.push_back(Position::Libero);
    } else {
      std::cerr << "Position `"<< pos << "' is unknown" << std::endl;
      return {};
    }
  }
  return positions;
}

bool is_complete(const Team& team) {
  return team.set != nullptr
      && team.dia != nullptr
      && team.mid[0] != nullptr && team.mid[1] != nullptr
      && team.out[0] != nullptr && team.out[1] != nullptr;
}

int32_t number_of_girls(const Team& team) {
  return team.set->is_female
       + team.dia->is_female
       + team.mid[0]->is_female + team.mid[1]->is_female
       + team.out[0]->is_female + team.out[1]->is_female;
}

int32_t level(const Team& team) {
  return team.set->level
       + team.dia->level
       + team.mid[0]->level + team.mid[1]->level
       + team.out[0]->level + team.out[1]->level;
}

int32_t score(const Team& team) {
  if (!is_complete(team)) {
    return std::numeric_limits<int32_t>::max();
  }
  return level(team);
}

int32_t score(const Team& team1, const Team& team2) {
  if (!is_complete(team1)) {
    return std::numeric_limits<int32_t>::max();
  } else if (!is_complete(team2)) {
    return std::numeric_limits<int32_t>::max();
  }
  return std::abs(score(team1) - score(team2))
    + std::abs(number_of_girls(team1) - number_of_girls(team2)) * 100;
}

int32_t score(const std::vector<Team>& teams) {
  int32_t res{0};
  for (size_t i1 = 0; i1 < teams.size(); ++i1) {
    for (size_t i2 = i1 + 1; i2 < teams.size(); ++i2) {
      res += score(teams[i1], teams[i2]);
    }
  }
  return res;
}

std::ostream& operator<<(std::ostream& os, const Player& player) {
  os << player.name << (player.is_female ? " (F)" : "");
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Team>& teams) {
  os << "team,set,dia,mid,mid,out,out,score,ladies\n";
  for (size_t i = 0; i < teams.size(); ++i) {
    const auto& team = teams[i];
    os << "team" << (i+1) << ","
       << *team.set << ","
       << *team.dia << ","
       << *team.mid[0] << "," << *team.mid[1] << "," 
       << *team.out[0] << "," << *team.out[1] << ","
       << score(team) << ","
       << number_of_girls(team)
       << "\n";
  }
  return os;
}

std::vector<Player> read_file(const char* filename) {
  std::vector<Player> players;
  std::ifstream ifs{filename};
  if (!ifs.is_open()) {
    return {};
  }

  for (std::string line; std::getline(ifs, line); ) {
    line = trim(line);
    if (line.empty()) {
      continue;
    }

    auto tokens = tokenize(line, ',', true);
    if (tokens.size() != 4) {
      std::cerr << "Line `" << line << "' cannot be tokenized correctly." << std::endl;
      return {};
    }
    const auto name = tokens[0];
    const auto is_female = tokens[1] == "F";
    const int level = std::strtol(tokens[2].data(), nullptr, 10);
    if (level < LevelMin || level > LevelMax) {
      std::cerr << "Warning: Level " << level << " is outside of range for " << name << std::endl;
    }
    players.emplace_back(Player{name,
                                (Level)level,
                                is_female,
                                parse_positions(tokens[3])});
  }
  return players;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  auto players = read_file(argv[1]);
  if (players.empty()) {
    std::cerr << "Could not read players" << std::endl;
  } else {
    std::cout << "Read " << players.size() << " players" << std::endl;
  }

  auto teams_count = players.size() / 6;
  if (teams_count * 6 != players.size()) {
    std::cout << "Reducing players from " << players.size() << " to " << teams_count * 6 << '\n';
    players.resize(teams_count * 6);
  }
  std::cout << "We go with " << teams_count << " teams" << std::endl;

  // Santify check - postions
  for (const auto& player: players) {
    if (player.positions.empty()) {
      std::cerr << "Player " << player.name << " has no positions" << std::endl;
      return 1;
    }
  }

  std::vector<const Player*> middles;
  std::vector<const Player*> setters;
  std::vector<const Player*> dias;
  std::vector<const Player*> outs;
  std::vector<const Player*> males;
  std::vector<const Player*> females;

  for (const auto& player: players) {
    const auto pos = player.positions[0]; //! TODO: only first positions are used for now
    switch(pos) {
      case Position::Mid:
      case Position::Libero:
        middles.push_back(&player);
        break;
      case Position::Set:
        setters.push_back(&player);
        break;
      case Position::Out:
        outs.push_back(&player);
        break;
      case Position::Dia:
        dias.push_back(&player);
        break;
    }
    if (player.is_female) {
      females.push_back(&player);
    } else {
      males.push_back(&player);
    }
  }
  std::cout << "Middles = " << middles.size()
    << " setters = " << setters.size()
    << " dias = " << dias.size()
    << " outs = " << outs.size()
    << " males = " << males.size()
    << " females = " << females.size()
    << std::endl;
  // Sanity check - positions, basic
  if (middles.size() < teams_count * 2) {
    std::cerr << "Not enough middles. Found " << middles.size() << std::endl;
    return 1;
  }
  if (setters.size() < teams_count) {
    std::cerr << "Not enough setters. Found " << setters.size() << std::endl;
    return 1;
  }
  if (dias.size() < teams_count) {
    std::cerr << "Not enough dias. Found " << dias.size() << std::endl;
    return 1;
  }
  if (outs.size() < teams_count * 2) {
    std::cerr << "Not enough outs. Found " << outs.size() << std::endl;
    return 1;
  }

  std::vector<int> middle_permutations(middles.size());
  std::iota(middle_permutations.begin(), middle_permutations.end(), 0);

  std::vector<int> outs_permutations(outs.size());
  std::iota(outs_permutations.begin(), outs_permutations.end(), 0);

  std::vector<int> dias_permutations(dias.size());
  std::iota(dias_permutations.begin(), dias_permutations.end(), 0);

  std::vector<int> sets_permutations(setters.size());
  std::iota(sets_permutations.begin(), sets_permutations.end(), 0);

  auto best_score = std::numeric_limits<int32_t>::max();
  while (true)
  {
    std::unordered_set<Player*> placed;
    std::vector<Team> teams{teams_count};

    std::random_shuffle(middles.begin(), middles.end());
    std::random_shuffle(outs.begin(), outs.end());
    std::random_shuffle(dias.begin(), dias.end());
    std::random_shuffle(setters.begin(), setters.end());
    auto middles_it = middles.begin();
    auto outs_it = outs.begin();
    auto dias_it = dias.begin();
    auto sets_it = setters.begin();
    for (auto& team: teams) {
      if (!Try(team, *(middles_it++)))
        break;
      if (!Try(team, *(middles_it++)))
        break;
      if (!Try(team, *(outs_it++)))
        break;
      if (!Try(team, *(outs_it++)))
        break;
      if (!Try(team, *(dias_it++)))
        break;
      if (!Try(team, *(sets_it++)))
        break;
    }

    const auto s = score(teams);
    if (s <= best_score) {
      std::cout << "Found better score = " << s << std::endl;
      std::cout << teams << std::endl;
      best_score = s;
    }
  }

  return 0;
}
