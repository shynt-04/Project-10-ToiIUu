// simulated annealing
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <random>
#include <limits>
#include <chrono>

#define MOD 1000000007
#define Task "annotshy"
#define F first
#define S second
#define int long long

using namespace std;

const int maxn = 1005;

int n, m;
int Q[maxn][maxn];
int d[maxn][maxn];
int q[maxn], cur_q[maxn];
int best_ans = 1e18;
int vis[maxn];
int cur_ans, ans[maxn], best_ans_arr[maxn];
float cur_temp;

random_device rd;
mt19937 rng(rd());

// dat time limit
double time_limit_sec = 60; 

bool check_stock() {
    for (int i = 1; i <= n; ++ i) {
        if (cur_q[i] < q[i]) return 0;
    }
    return 1;
}

void greedy() {
    int start = 0, total_dis = 0;
    while (!check_stock()) {
        int best = -1;
        for (int i = 1; i <= m; ++ i) {
            if (vis[i]) continue;
            if (best == -1 || d[start][i] < d[start][best]) {
                best = i;
            }
        }
        vis[best] = 1;
        total_dis += d[start][best];
        for (int i = 1; i <= n; ++ i) {
            cur_q[i] += Q[i][best];
        }
        start = best;
        ans[++ ans[0]] = best;
    }
    total_dis += d[start][0];
    best_ans = total_dis;
}

bool accept_worse(int current, int next, double T) {
    if (next <= current) return true;
    double delta = (double)(next - current);
    double prob = exp(-delta / T);
    uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng) < prob;
}

void try_2opt() {
    int i = 1 + rng() % ans[0];
    int j = 1 + rng() % ans[0];
    if (i > j) swap(i, j);
    if (i == j || j == i + 1) return;
    int new_dis = cur_ans;
    int u = (i == 1) ? 0 : ans[i - 1];
    int v = (j == ans[0]) ? 0 : ans[j + 1];
    new_dis -= d[u][ans[i]] + d[ans[j]][v];
    new_dis += d[u][ans[j]] + d[ans[i]][v];
    if (accept_worse(cur_ans, new_dis, cur_temp)) {
        reverse(ans + i, ans + j + 1);
        cur_ans = new_dis;
    }
}

void try_swap_shelves() {
    int i = 1 + rng() % ans[0];
    int j = 1 + rng() % ans[0];
    if (i == j) return;
    if (i > j) swap(i, j);
    
    int new_dis = cur_ans;
    int u = (i == 1) ? 0 : ans[i - 1];
    int v = (j == ans[0]) ? 0 : ans[j + 1];
    if (j == i + 1) {
        new_dis -= d[u][ans[i]] + d[ans[i]][ans[j]] + d[ans[j]][v];
        new_dis += d[u][ans[j]] + d[ans[j]][ans[i]] + d[ans[i]][v];
    } else {
        new_dis -= d[u][ans[i]] + d[ans[i]][ans[i + 1]] + d[ans[j - 1]][ans[j]] + d[ans[j]][v];
        new_dis += d[u][ans[j]] + d[ans[j]][ans[i + 1]] + d[ans[j - 1]][ans[i]] + d[ans[i]][v];
    }
    
    if (accept_worse(cur_ans, new_dis, cur_temp)) {
        swap(ans[i], ans[j]);
        cur_ans = new_dis;
    }
}

void try_remove_shelf() {
    int i = 1 + rng() % ans[0];
    int new_dis = cur_ans;
    int good_rem = 1;
    for (int j = 1; j <= n; ++ j) {
        if (cur_q[j] - Q[j][ans[i]] < q[j]) {
            good_rem = 0;
            break;
        }
    }
    if (good_rem == 0) return;
    int u = (i == 1) ? 0 : ans[i - 1];
    int v = (i == ans[0]) ? 0 : ans[i + 1];
    new_dis -= d[u][ans[i]] + d[ans[i]][v];
    new_dis += d[u][v];
    if (accept_worse(cur_ans, new_dis, cur_temp)) {
        for (int j = 1; j <= n; ++ j) {
            cur_q[j] -= Q[j][ans[i]];
        }
        vis[ans[i]] = 0;
        for (int j = i; j < ans[0]; ++ j) {
            ans[j] = ans[j + 1];
        }
        -- ans[0];
        cur_ans = new_dis;
    }
}

void try_add_shelf() {
    vector<int> unvisited;
    for (int i = 1; i <= m; ++i) {
        if (!vis[i]) unvisited.push_back(i);
    }
    if (unvisited.empty()) return;
    
    int shelf = unvisited[rng() % unvisited.size()];
    int pos = 1 + rng() % (ans[0] + 1);
    
    int u = (pos == 1) ? 0 : ans[pos - 1];
    int v = (pos > ans[0]) ? 0 : ans[pos];
    int new_dis = cur_ans - d[u][v] + d[u][shelf] + d[shelf][v];
    
    if (accept_worse(cur_ans, new_dis, cur_temp)) {
        vis[shelf] = 1;
        for (int j = 1; j <= n; ++j) {
            cur_q[j] += Q[j][shelf];
        }
        for (int j = ans[0] + 1; j > pos; --j) {
            ans[j] = ans[j - 1];
        }
        ans[pos] = shelf;
        ++ans[0];
        cur_ans = new_dis;
    }
}

void update_best() {
    best_ans_arr[0] = ans[0];
    for (int i = 1; i <= ans[0]; ++ i) {
        best_ans_arr[i] = ans[i];
    }
}

void local_search_simulated_annealing() {
    cur_ans = best_ans;
    update_best();
    
    double T_init = 100.0;  // FIXED initial temperature
    double T_min = 0.01;
    
    auto start_time = chrono::high_resolution_clock::now();
    int iterations = 0;
    int accepts = 0;
    int improves = 0;
    
    while (1) {
        auto now = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = now - start_time;
        if (elapsed.count() > time_limit_sec) break;
        
        double progress = elapsed.count() / time_limit_sec;
        double T = T_init * pow(T_min / T_init, progress);
        cur_temp = T;
        
        int old_ans = cur_ans;
        
        int move_type = rng() % 100;
        if (move_type < 60) {
            try_2opt(); 
        } else if (move_type < 75) {
            try_swap_shelves(); 
        } else if (move_type < 90) {
            try_remove_shelf(); 
        } else {
            try_add_shelf();  
        }
        
        if (cur_ans != old_ans) accepts++;
        if (cur_ans < old_ans) improves++;
        
        if (cur_ans < best_ans) {
            best_ans = cur_ans;
            update_best();
        }
        
        iterations++;
        
        if (iterations % 10000 == 0) {
            double accept_rate = (double)accepts / 10000;
            double improve_rate = (double)improves / 10000;
            
            if (accept_rate < 0.01 && T > T_min) {
                cur_ans = best_ans;
                ans[0] = best_ans_arr[0];
                for (int i = 1; i <= best_ans_arr[0]; ++i) {
                    ans[i] = best_ans_arr[i];
                }
                memset(cur_q, 0, sizeof(cur_q));
                memset(vis, 0, sizeof(vis));
                for (int i = 1; i <= ans[0]; ++i) {
                    vis[ans[i]] = 1;
                    for (int j = 1; j <= n; ++j) {
                        cur_q[j] += Q[j][ans[i]];
                    }
                }
                T = T_init * 0.5;
            }
            
            accepts = 0;
            improves = 0;
        }
    }
}

void solve(int Test) {
    cin >> n >> m;
    for (int i = 1; i <= n; ++ i) {
        for (int j = 1; j <= m; ++ j) {
            cin >> Q[i][j];
        }
    }
    for (int i = 0; i <= m; ++ i) {
        for (int j = 0; j <= m; ++ j) {
            cin >> d[i][j];
        }
    }
    for (int i = 1; i <= n; ++ i) cin >> q[i];
    greedy();
    local_search_simulated_annealing();
    cout << best_ans_arr[0] << "\n";
    for (int i = 1; i <= best_ans_arr[0]; ++ i) {
        cout << best_ans_arr[i] << " ";
    }
    cout << "\n";
    cout << best_ans << "\n";
}

signed main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0), cout.tie(0);
    if (fopen(Task".inp", "r")) {
        freopen(Task".inp", "r", stdin);
        freopen(Task".out", "w", stdout);
    }
    int test = 1;
    for (int i = 1; i <= test; ++ i) {
        solve(i);
    }
}
