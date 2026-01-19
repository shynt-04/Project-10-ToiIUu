
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <iomanip>
#include <set>
#include <tuple>
#include <fstream>

using namespace std;

// --- Helper Types & Constants ---

const long long INF_COST = 1e18;

#define Task "annotshy"

struct BRKGAParams {
    int population = 200;
    double elite_frac = 0.20;
    double mutant_frac = 0.10;
    double bias = 0.70;
    int max_generations = 250;
    double time_limit_sec = 6;
    int seed = 0;
    bool two_opt = true;
    int two_opt_max_swaps = 4000;
    int elite_local_search = 8;
    int local_search_moves = 800;
    int multi_start = 3;
    bool is_debug = false;
};

// --- Logic Functions ---

bool covers_all(const vector<int>& collected, const vector<int>& required) {
    for (size_t i = 0; i < collected.size(); ++i) {
        if (collected[i] < required[i]) {
            return false;
        }
    }
    return true;
}

long long distance_of_route(const vector<int>& route, const vector<vector<int>>& d) {
    if (route.empty()) {
        return 0;
    }
    long long total = d[0][route[0]];
    for (size_t i = 0; i < route.size() - 1; ++i) {
        total += d[route[i]][route[i+1]];
    }
    total += d[route.back()][0];
    return total;
}

bool is_symmetric_distance(const vector<vector<int>>& d) {
    int n = d.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (d[i][j] != d[j][i]) {
                return false;
            }
        }
    }
    return true;
}

vector<int> prune_cost_aware(vector<int> route, const vector<vector<int>>& Q, const vector<int>& required, const vector<vector<int>>& d) {
    if (route.empty()) return route;

    int n_products = required.size();
    vector<int> collected(n_products, 0);

    for (int shelf : route) {
        for (int i = 0; i < n_products; ++i) {
            collected[i] += Q[i][shelf - 1];
        }
    }

    while (true) {
        long long best_gain = 0;
        int best_idx = -1;
        
        for (size_t idx = 0; idx < route.size(); ++idx) {
            int shelf = route[idx];
            
            // Feasibility check if removed
            bool ok = true;
            for (int i = 0; i < n_products; ++i) {
                if (collected[i] - Q[i][shelf - 1] < required[i]) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;

            int prev_node = (idx == 0) ? 0 : route[idx - 1];
            int next_node = (idx == route.size() - 1) ? 0 : route[idx + 1];
            
            // Gain = old - new
            long long gain = (long long)d[prev_node][shelf] + d[shelf][next_node] - d[prev_node][next_node];
            
            if (gain > best_gain) {
                best_gain = gain;
                best_idx = idx;
            }
        }

        if (best_idx < 0) break;

        int removed = route[best_idx];
        route.erase(route.begin() + best_idx);
        for (int i = 0; i < n_products; ++i) {
            collected[i] -= Q[i][removed - 1];
        }
    }
    return route;
}

int best_insertion_position(const vector<int>& route, int node, const vector<vector<int>>& d) {
    if (route.empty()) return 0;

    int best_pos = 0;
    // Insert at 0: 0 -> node -> route[0] vs 0 -> route[0]
    long long best_delta = (long long)d[0][node] + d[node][route[0]] - d[0][route[0]];

    for (size_t pos = 1; pos < route.size(); ++pos) {
        int prev_node = route[pos - 1];
        int next_node = route[pos];
        long long delta = (long long)d[prev_node][node] + d[node][next_node] - d[prev_node][next_node];
        if (delta < best_delta) {
            best_delta = delta;
            best_pos = pos;
        }
    }

    // Insert at end
    long long end_delta = (long long)d[route.back()][node] + d[node][0] - d[route.back()][0];
    if (end_delta < best_delta) {
        best_pos = route.size();
    }

    return best_pos;
}

vector<int> relocate_local_search(vector<int> route, const vector<vector<int>>& d, int max_moves = 2000) {
    int moves = 0;
    bool improved = true;

    while (improved && moves < max_moves) {
        improved = false;
        int n = route.size();

        for (int i = 0; i < n; ++i) {
            int b = route[i];
            int a = (i == 0) ? 0 : route[i - 1];
            int c = (i == n - 1) ? 0 : route[i + 1];
            long long remove_change = (long long)d[a][c] - d[a][b] - d[b][c];

            long long best_j_change = 0;
            int best_j = -1;

            // Tạo bản sao đã xóa i để tìm vị trí chèn j tốt nhất
            vector<int> reduced = route;
            reduced.erase(reduced.begin() + i);

            for (size_t j = 0; j <= reduced.size(); ++j) {
                int p = (j == 0) ? 0 : reduced[j - 1];
                int q = (j == reduced.size()) ? 0 : reduced[j];
                
                long long insert_change = (long long)d[p][b] + d[b][q] - d[p][q];
                long long change = remove_change + insert_change;

                if (change < best_j_change) {
                    best_j_change = change;
                    best_j = j;
                }
            }

            if (best_j != -1) {
                // Thực hiện thay đổi ngay lập tức
                route.erase(route.begin() + i);
                route.insert(route.begin() + best_j, b);
                
                moves++;
                improved = true;

                // Cấu trúc route đã thay đổi, cần break để quét lại từ đầu
                break; 
            }
        }
    }
    return route;
}

vector<int> swap_local_search(vector<int> route, const vector<vector<int>>& d, int max_swaps = 2000) {
    int n = route.size();
    if (n < 4) return route;

    int swaps = 0;
    while (swaps < max_swaps) {
        long long best_change = 0;
        int best_i = -1;
        int best_j = -1;

        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                long long change;
                if (j == i + 1) {
                    // Adjacent
                    int ai = (i == 0) ? 0 : route[i - 1];
                    int bi = route[i];
                    int bj = route[j];
                    int cj = (j == n - 1) ? 0 : route[j + 1];
                    long long old_cost = (long long)d[ai][bi] + d[bi][bj] + d[bj][cj];
                    long long new_cost = (long long)d[ai][bj] + d[bj][bi] + d[bi][cj];
                    change = new_cost - old_cost;
                } else {
                    int ai = (i == 0) ? 0 : route[i - 1];
                    int bi = route[i];
                    int ci = route[i + 1];
                    int aj = route[j - 1];
                    int bj = route[j];
                    int cj = (j == n - 1) ? 0 : route[j + 1];
                    long long old_cost = (long long)d[ai][bi] + d[bi][ci] + d[aj][bj] + d[bj][cj];
                    long long new_cost = (long long)d[ai][bj] + d[bj][ci] + d[aj][bi] + d[bi][cj];
                    change = new_cost - old_cost;
                }

                if (change < best_change) {
                    best_change = change;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        if (best_i < 0) break;
        std::swap(route[best_i], route[best_j]);
        swaps++;
    }
    return route;
}

vector<int> two_opt(vector<int> route, const vector<vector<int>>& d, int max_swaps = 10000) {
    int n = route.size();
    if (n < 4) return route;

    bool improved = true;
    int swaps_done = 0;
    
    while (improved && swaps_done < max_swaps) {
        improved = false;
        
        // Edges between route nodes
        for (int i = 0; i < n - 2; ++i) {
            int a = (i == 0) ? 0 : route[i - 1];
            int b = route[i];
            for (int k = i + 1; k < n - 1; ++k) {
                int c = route[k];
                int d_next = route[k + 1];

                long long before = (long long)d[a][b] + d[c][d_next];
                long long after = (long long)d[a][c] + d[b][d_next];

                if (after < before) {
                    std::reverse(route.begin() + i, route.begin() + k + 1);
                    improved = true;
                    swaps_done++;
                    if (swaps_done >= max_swaps) goto end_loops;
                }
            }
            if (swaps_done >= max_swaps) break;
        }
        if (swaps_done >= max_swaps) break;

        // End edge wrapping to 0
        for (int i = 1; i < n - 2; ++i) {
            int a = route[i - 1];
            int b = route[i];
            int c = route.back();
            int d0 = 0;

            long long before = (long long)d[a][b] + d[c][d0];
            long long after = (long long)d[a][c] + d[b][d0];

            if (after < before) {
                std::reverse(route.begin() + i, route.end());
                improved = true;
                swaps_done++;
                if (swaps_done >= max_swaps) goto end_loops;
            }
        }
    }
    end_loops:;
    return route;
}

// Returns {cost, route}
pair<long long, vector<int>> decode(
    const vector<double>& keys, 
    const vector<int>& candidates, 
    const vector<vector<int>>& Q, 
    const vector<int>& required, 
    const vector<vector<int>>& d, 
    bool do_two_opt, 
    int two_opt_max_swaps, 
    bool do_intensify, 
    bool allow_two_opt, 
    int ls_moves
) {
    // Sort candidates by keys
    vector<int> order(candidates.size());
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b){
        return keys[a] < keys[b];
    });

    int n_products = required.size();
    vector<int> collected(n_products, 0);
    vector<int> route;

    for (int idx : order) {
        int shelf = candidates[idx];
        if (covers_all(collected, required)) break;
        
        int pos = best_insertion_position(route, shelf, d);
        route.insert(route.begin() + pos, shelf);
        
        for (int i = 0; i < n_products; ++i) {
            collected[i] += Q[i][shelf - 1];
        }
    }

    if (!covers_all(collected, required)) {
        return {INF_COST, {}};
    }

    route = prune_cost_aware(route, Q, required, d);

    if (do_intensify) {
        route = relocate_local_search(route, d, ls_moves);
        route = swap_local_search(route, d, max(50, ls_moves / 2));
        if (do_two_opt && allow_two_opt) {
            route = two_opt(route, d, two_opt_max_swaps);
        }
        route = prune_cost_aware(route, Q, required, d);
    }

    return {distance_of_route(route, d), route};
}

vector<int> solve_brkga(const vector<vector<int>>& Q, const vector<vector<int>>& d, const vector<int>& required, const BRKGAParams& params, int run_id, ofstream* log_file) {
    int n_products = required.size();
    int m_shelves = Q[0].size();

    // Global feasibility
    vector<int> total_supply(n_products, 0);
    for (int j = 0; j < m_shelves; ++j) {
        for (int i = 0; i < n_products; ++i) {
            total_supply[i] += Q[i][j];
        }
    }
    if (!covers_all(total_supply, required)) return {};

    bool allow_two_opt = is_symmetric_distance(d);
    
    vector<int> candidates(m_shelves);
    iota(candidates.begin(), candidates.end(), 1);

    int k = candidates.size();
    mt19937 rng(params.seed);
    uniform_real_distribution<double> dist(0.0, 1.0);

    int pop_size = max(30, params.population);
    int elite_size = max(1, (int)(pop_size * params.elite_frac));
    int mutant_size = max(1, (int)(pop_size * params.mutant_frac));
    int offspring_size = pop_size - elite_size - mutant_size;
    if (offspring_size < 1) {
        offspring_size = 1;
        mutant_size = max(1, pop_size - elite_size - offspring_size);
    }

    // Seeded individual
    vector<double> seeded_keys(k);
    for (int i = 0; i < k; ++i) seeded_keys[i] = (double)i / max(1, k);

    vector<vector<double>> population(pop_size);
    population[0] = seeded_keys;
    for (int i = 1; i < pop_size; ++i) {
        population[i].resize(k);
        for (int j = 0; j < k; ++j) population[i][j] = dist(rng);
    }

    // Evaluated: {cost, route}
    vector<pair<long long, vector<int>>> fitness_routes(pop_size);
    for (int i = 0; i < pop_size; ++i) {
        fitness_routes[i] = decode(population[i], candidates, Q, required, d, 
                                   params.two_opt, params.two_opt_max_swaps, false, allow_two_opt, params.local_search_moves);
    }

    long long best_cost = INF_COST;
    vector<int> best_route;

    // Initial Best find
    for (const auto& fr : fitness_routes) {
        if (fr.first < best_cost) {
            best_cost = fr.first;
            best_route = fr.second;
        }
    }

    auto start_time = chrono::high_resolution_clock::now();

    for (int gen = 0; gen < params.max_generations; ++gen) {
        auto now = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = now - start_time;
        if (elapsed.count() > params.time_limit_sec) break;

        // Rank
        vector<int> ranked(pop_size);
        iota(ranked.begin(), ranked.end(), 0);
        sort(ranked.begin(), ranked.end(), [&](int a, int b) {
            return fitness_routes[a].first < fitness_routes[b].first;
        });

        // Update best
        if (fitness_routes[ranked[0]].first < best_cost) {
            best_cost = fitness_routes[ranked[0]].first;
            best_route = fitness_routes[ranked[0]].second;
        }

        vector<vector<double>> next_population;
        next_population.reserve(pop_size);
        
        // Elites được chuyển luôn sang thế hệ tiếp theo
        vector<int> elite_indices;
        for (int i = 0; i < elite_size; ++i) {
            next_population.push_back(population[ranked[i]]);
            elite_indices.push_back(ranked[i]);
        }

        // Offspring
        vector<int> non_elite_indices;
        for (int i = elite_size; i < pop_size; ++i) non_elite_indices.push_back(ranked[i]);

        for (int i = 0; i < offspring_size; ++i) {
            int e_idx = elite_indices[rng() % elite_size];
            int n_idx;
            if (!non_elite_indices.empty()) {
                n_idx = non_elite_indices[rng() % non_elite_indices.size()];
            } else {
                n_idx = elite_indices[rng() % elite_size];
            }

            const auto& pA = population[e_idx];
            const auto& pB = population[n_idx];
            vector<double> child(k);
            for (int g = 0; g < k; ++g) {
                child[g] = (dist(rng) < params.bias) ? pA[g] : pB[g];
            }
            next_population.push_back(child);
        }

        // Mutants
        for (int i = 0; i < mutant_size; ++i) {
            vector<double> mutant(k);
            for (int g = 0; g < k; ++g) mutant[g] = dist(rng);
            next_population.push_back(mutant);
        }

        population = next_population;
        
        // Evaluate
        for (int i = 0; i < pop_size; ++i) {
             fitness_routes[i] = decode(population[i], candidates, Q, required, d, 
                                        params.two_opt, params.two_opt_max_swaps, false, allow_two_opt, params.local_search_moves);
        }

        // Intensify top elites
        // Intensify top elites
        if (params.is_debug || gen == params.max_generations - 1) {
            vector<int> ranked2(pop_size);
            iota(ranked2.begin(), ranked2.end(), 0);
            sort(ranked2.begin(), ranked2.end(), [&](int a, int b) {
                return fitness_routes[a].first < fitness_routes[b].first;
            });

            int limit_intensify = min(params.elite_local_search, pop_size);
            for (int i = 0; i < limit_intensify; ++i) {
                auto now2 = chrono::high_resolution_clock::now();
                chrono::duration<double> el2 = now2 - start_time;
                if (el2.count() > params.time_limit_sec) break;

                int idx = ranked2[i];
                auto res = decode(population[idx], candidates, Q, required, d, 
                                 params.two_opt, params.two_opt_max_swaps, true, allow_two_opt, params.local_search_moves);
                fitness_routes[idx] = res;
            }
        }

        // Final best check for this gen
        for (const auto& fr : fitness_routes) {
            if (fr.first < best_cost) {
                best_cost = fr.first;
                best_route = fr.second;
            }
        }

        // Log best cost after each generation (only if debug mode)
        if (log_file) {
            *log_file << run_id << "," << gen << "," << best_cost << "\n";
        }
    }

    return best_route;
}

vector<int> solve(const vector<vector<int>>& Q, const vector<vector<int>>& d, const vector<int>& required, const BRKGAParams& params) {
    auto start = chrono::high_resolution_clock::now();
    long long best_cost = INF_COST;
    vector<int> best_route;

    int runs = max(1, params.multi_start);
    double per_run = max(0.2, params.time_limit_sec / runs);

    // Open log file for convergence data (only if debug mode)
    ofstream* log_file = nullptr;
    if (params.is_debug) {
        log_file = new ofstream("ga_convergence.csv");
        *log_file << "run_id,generation,best_cost\n";
    }

    for (int r = 0; r < runs; ++r) {
        auto now = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = now - start;
        double remaining = params.time_limit_sec - elapsed.count();
        
        if (remaining <= 0) break;

        BRKGAParams run_params = params;
        run_params.time_limit_sec = min(per_run, remaining);
        run_params.seed = params.seed + 97 * r;

        vector<int> route = solve_brkga(Q, d, required, run_params, r, log_file);
        long long cost = distance_of_route(route, d);
        
        if (!route.empty() && cost < best_cost) {
            best_cost = cost;
            best_route = route;
        }
    }

    if (log_file) {
        log_file->close();
        delete log_file;
    }
    return best_route;
}

int main(int argc, char** argv) {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // File-based I/O like greedy.cpp
    // Prefer Task.txt if present; otherwise fall back to Task.inp
    if (FILE* f = fopen(Task ".txt", "r")) {
        fclose(f);
        freopen(Task ".txt", "r", stdin);
        freopen(Task ".out", "w", stdout);
    } else if (FILE* f2 = fopen(Task ".inp", "r")) {
        fclose(f2);
        freopen(Task ".inp", "r", stdin);
        freopen(Task ".out", "w", stdout);
    }

    BRKGAParams params;

    // Basic CLI parsing
    int i = 1;
    while (i < argc) {
        string arg = argv[i];
        if (arg == "--time" && i + 1 < argc) {
            params.time_limit_sec = stod(argv[i + 1]);
            i += 2;
        } else if (arg == "--seed" && i + 1 < argc) {
            params.seed = stoi(argv[i + 1]);
            i += 2;
        } else if (arg == "--pop" && i + 1 < argc) {
            params.population = stoi(argv[i + 1]);
            i += 2;
        } else if (arg == "--gen" && i + 1 < argc) {
            params.max_generations = stoi(argv[i + 1]);
            i += 2;
        } else if (arg == "--no-2opt") {
            params.two_opt = false;
            i += 1;
        } else if (arg == "--no-debug") {
            params.is_debug = false;
            i += 1;
        } else {
            i += 1;
        }
    }

    int N, M;
    if (!(cin >> N >> M)) return 0;

    // Q: N lines, each has M ints
    vector<vector<int>> Q(N, vector<int>(M));
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < M; ++c) {
            cin >> Q[r][c];
        }
    }

    // d: M+1 lines, each has M+1 ints
    vector<vector<int>> d(M + 1, vector<int>(M + 1));
    for (int r = 0; r <= M; ++r) {
        for (int c = 0; c <= M; ++c) {
            cin >> d[r][c];
        }
    }

    // required: 1 line, N ints
    vector<int> required(N);
    for (int r = 0; r < N; ++r) {
        cin >> required[r];
    }

    vector<int> route = solve(Q, d, required, params);

    cout << route.size() << "\n";
    if (!route.empty()) {
        for (size_t k = 0; k < route.size(); ++k) {
            cout << route[k] << (k == route.size() - 1 ? "" : " ");
        }
        cout << "\n";
    } else {
        cout << "\n";
    }

    return 0;
}
