// Method's to implement
/*
    - BFS
    - DFS
    -


*/
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <map>
#include <random>
#include <set>
#include <sys/types.h>
#include <utility>
#include <vector>

std::random_device rd{};
std::mt19937_64 generator{rd()};
std::uniform_int_distribution<int8_t> random_xy(-100, 100);
std::uniform_int_distribution<uint8_t> random_z(0, 50);

struct City {
  int8_t x{};
  int8_t y{};
  uint8_t z{};
  City(int8_t x, int8_t y, uint8_t z) : x{x}, y{y}, z{z} {}
};

struct AlgorithmStats {
  std::chrono::nanoseconds time{};
  size_t memory_usage{};
  size_t total_cost{};
};

struct Graph {
  std::vector<City> cities{};
  std::map<std::pair<int, int>, double> paths{};
  bool path_exists(int source_id, int dest_id) const noexcept {
    auto value = paths.find({source_id, dest_id});
    return value != std::end(paths);
  };
};
using SolutionAlgorithm = std::function<AlgorithmStats(const Graph &)>;

Graph create_graph(int city_count, bool is_complete, bool is_symetrical) {
  Graph new_graph{};

  for (int i = 0; i < city_count; i++) {
    new_graph.cities.emplace_back(random_xy(generator), random_xy(generator),
                                  is_symetrical ? 0 : random_z(generator));
  }
  assert(new_graph.cities.size() == city_count);

  for (int i = 0; i < new_graph.cities.size(); i++) {
    for (int j = 0; i < new_graph.cities.size(); j++) {
      if (i == j || new_graph.path_exists(i, j))
        continue;
      const auto &city_i = new_graph.cities.at(i);
      const auto &city_j = new_graph.cities.at(j);
      auto distance = static_cast<int>(std::sqrt(
          std::pow(city_j.x - city_i.x, 2) + std::pow(city_j.y - city_i.y, 2)));
      if (!is_symetrical && (city_i.z != city_j.z)) {
        bool uphill = city_i.z < city_j.z;
        if (uphill) {
          new_graph.paths.at({i, j}) = distance * 1.1;
          new_graph.paths.at({j, i}) = distance * 0.9;
        } else {
          new_graph.paths.at({i, j}) = distance * 0.9;
          new_graph.paths.at({j, i}) = distance * 1.1;
        }
      } else {
        new_graph.paths.at({i, j}) = distance;
        new_graph.paths.at({j, i}) = distance;
      }
    }
  }
  if (!is_complete) {
    std::set<std::pair<int, int>> removed_paths_id{};
    std::uniform_int_distribution<int> id_dist(0, city_count - 1);
    while (removed_paths_id.size() <
           static_cast<int>(0.2 * new_graph.paths.size())) {
      int i = id_dist(generator), j = id_dist(generator);
      if (i == j) {
        continue;
      }
      auto new_removed = std::make_pair(i, j);
      auto new_removed_flipped = std::make_pair(j, i);
      removed_paths_id.emplace(new_removed);
      removed_paths_id.emplace(new_removed_flipped);
    }

    std::for_each(std::begin(removed_paths_id), std::end(removed_paths_id),
                  [&new_graph](const auto &id) { new_graph.paths.erase(id); });
  }
  return std::move(new_graph);
}
auto dfs = [](const Graph &graph) -> AlgorithmStats {};
auto bfs = [](const Graph &graph) -> AlgorithmStats {};
auto nearest_neighbour = [](const Graph &graph) -> AlgorithmStats {};
auto dijikstra = [](const Graph &graph) -> AlgorithmStats {};
auto inadmissible_astar = [](const Graph &graph) -> AlgorithmStats {};
auto admissible_astar = [](const Graph &graph) -> AlgorithmStats {};
auto ant_colony = [](const Graph &graph) -> AlgorithmStats {};

int main() {
  std::vector<SolutionAlgorithm> sols = {dfs,
                                         bfs,
                                         nearest_neighbour,
                                         dijikstra,
                                         inadmissible_astar,
                                         admissible_astar,
                                         ant_colony};
  for (int i = 5; i < 21; i++) {
    Graph complete_symetrical_graph = create_graph(i, true, true);
    Graph complete_asymetrical_graph = create_graph(i, true, false);
    Graph incomplete_symetrical_graph = create_graph(i, false, true);
    Graph incomplete_asymetrical_graph = create_graph(i, false, false);
  }
}