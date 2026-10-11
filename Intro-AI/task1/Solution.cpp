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
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <print>
#include <random>
#include <set>
#include <string>
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
  double total_cost{};
  std::vector<int> best_path{};
};

void printout_stats(const std::string &title, const AlgorithmStats &stats) {
  std::println("{} -\tTime = {}\tMemory Usage = {}\tTotal Cost = {}\nPath = {}",
               title, stats.time, stats.memory_usage, stats.total_cost,
               stats.best_path);
}

struct Graph {
  std::vector<City> cities{};
  std::map<std::pair<int, int>, double> paths{};
  bool path_exists(int source_id, int dest_id) const noexcept {
    auto value = paths.find({source_id, dest_id});
    return value != std::end(paths);
  };
};
using SolutionAlgorithm = std::function<AlgorithmStats(const Graph &)>;
double euclidean_distance(const City &a, const City &b) {
  const double dx = a.x - b.x;
  const double dy = a.y - b.y;
  const double dz = a.z - b.z;
  return std::sqrt(dx * dx + dy * dy + dz * dz);
}
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
      const auto distance = euclidean_distance(city_i, city_j);
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
auto dfs = [](const Graph &graph) -> AlgorithmStats {
  const size_t n = graph.cities.size();
  assert(n > 0);
  AlgorithmStats stats{};

  double best_cost = std::numeric_limits<double>::infinity();
  size_t max_depth = 0;
  std::vector<bool> visited(n, false);
  std::vector<int> path;
  std::vector<int> best_path;
  path.reserve(n);
  best_path.reserve(n);

  std::function<void(int, double)> visit = [&](int current, double cost) {
    max_depth = std::max(max_depth, path.size());

    // Prune (assumes non-negative edge costs).
    if (cost >= best_cost)
      return;

    if (path.size() == n) {
      auto back = graph.paths.find({current, 0});
      if (back != graph.paths.end()) {
        const double total = cost + back->second;
        if (total < best_cost) {
          best_cost = total;
          best_path = path; // copy of the current best tour
        }
      }
      return;
    }

    for (size_t next = 0; next < n; ++next) {
      if (visited[next])
        continue;
      auto edge = graph.paths.find({current, static_cast<int>(next)});
      if (edge == graph.paths.end())
        continue;

      visited[next] = true;
      path.push_back(static_cast<int>(next));
      visit(static_cast<int>(next), cost + edge->second);
      path.pop_back();
      visited[next] = false;
    }
  };

  const auto start_time = std::chrono::steady_clock::now();
  visited[0] = true;
  path.push_back(0);
  visit(0, 0.0);
  const auto end_time = std::chrono::steady_clock::now();
  stats.time = end_time - start_time;
  constexpr size_t approx_frame = sizeof(int) + sizeof(double) + 64;
  // TODO: Actual memory consumption
  stats.memory_usage =
      n / 8 + 2 * n * sizeof(int) + (max_depth + 1) * approx_frame;

  if (std::isfinite(best_cost)) {
    stats.total_cost = static_cast<size_t>(std::llround(best_cost));
    stats.best_path = std::move(best_path);
  }
  return stats;
};
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

    const auto sol_1 = dfs(complete_symetrical_graph);
    const auto sol_2 = dfs(complete_asymetrical_graph);
    const auto sol_3 = dfs(incomplete_symetrical_graph);
    const auto sol_4 = dfs(incomplete_asymetrical_graph);
  }
}