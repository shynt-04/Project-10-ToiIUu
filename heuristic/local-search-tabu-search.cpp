// tabu search
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <queue>
#include <random>
#include <limits>
#include <set>
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
int best_ans = 1e18, time_cnt = 0, cMin = 1e18;
int vis[maxn];
int cur_ans, ans[maxn], best_ans_tabu[maxn];
set<pair<int, pair<int,int>>> tabu_set;
queue<pair<int, pair<int,int>>> Q_tabu;
int MX_QUEUE = 7;
int no_improve_cnt = 0;

random_device rd;
mt19937 rng(rd());

// dat time limit theo giay
double time_limit_sec = 80;

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

bool try_2opt() {
    int best_new_dis = 1e18;
    int best_i = -1, best_j = -1;
    int best_u = -1, best_v = -1;
    int sample_size = min(50000LL, ans[0] * ans[0]);
    for (int t = 1; t <= sample_size; ++t) {
        int i = rng() % ans[0] + 1;
        int j = rng() % ans[0] + 1;
        if (i == j) continue;
        if (i > j) swap(i, j);
        int u = (i == 1) ? 0 : ans[i - 1];
        int v = (j == ans[0]) ? 0 : ans[j + 1];

        int new_dis = cur_ans;
        new_dis -= d[u][ans[i]] + d[ans[j]][v];
        new_dis += d[u][ans[j]] + d[ans[i]][v];

        for (int k = i; k < j; ++k) {
            new_dis -= d[ans[k]][ans[k + 1]];
            new_dis += d[ans[k + 1]][ans[k]];
        }

        bool is_tabu = tabu_set.count({1, {ans[i], u}}) > 0;
        is_tabu |= tabu_set.count({1, {ans[j], v}}) > 0;
        if (is_tabu && new_dis >= best_ans) continue;
        if (new_dis < best_new_dis) {
            best_new_dis = new_dis;
            best_i = i;
            best_j = j;
            best_u = u;
            best_v = v;
        }
    }

    if (best_i == -1) return 0;

    tabu_set.insert({1, {ans[best_i], best_u}});
    tabu_set.insert({1, {ans[best_j], best_v}});
    Q_tabu.push({1, {ans[best_i], best_u}});
    Q_tabu.push({1, {ans[best_j], best_v}});

    reverse(ans + best_i, ans + best_j + 1);
    cur_ans = best_new_dis;

    return 1;
}

bool try_swap_shelves() {
    int best_new_dis = 1e18;
    int best_i = -1, best_j = -1;
    int best_a = -1, best_b = -1;

    int sample_size = min(50000LL, ans[0] * ans[0]);
    
    for (int t = 1; t <= sample_size; ++t) {
        int i = rng() % ans[0] + 1;
        int j = rng() % ans[0] + 1;
        if (i == j) continue;
        if (i > j) swap(i, j);
        int a = ans[i];
        int b = ans[j];

        int u = (i == 1) ? 0 : ans[i - 1];
        int v = (j == ans[0]) ? 0 : ans[j + 1];

        int new_dis = cur_ans;

        if (j == i + 1) {
            new_dis -= d[u][a] + d[a][b] + d[b][v];
            new_dis += d[u][b] + d[b][a] + d[a][v];
        } else {
            int mid1 = ans[i + 1];
            int mid2 = ans[j - 1];
            new_dis -= d[u][a] + d[a][mid1] + d[mid2][b] + d[b][v];
            new_dis += d[u][b] + d[b][mid1] + d[mid2][a] + d[a][v];
        }

        bool is_tabu = tabu_set.count({2, {a, b}}) > 0;
        if (is_tabu && new_dis >= best_ans) continue;
        if (new_dis < best_new_dis) {
            best_new_dis = new_dis;
            best_i = i;
            best_j = j;
            best_a = a;
            best_b = b;
        }
    }

    if (best_i == -1) return 0;

    swap(ans[best_i], ans[best_j]);
    cur_ans = best_new_dis;

    tabu_set.insert({2, {best_a, best_b}});
    Q_tabu.push({2, {best_a, best_b}});

    return 1;
}

bool try_remove_shelf() {
    int best_new_dis = 1e18;
    int best_i = -1;
    
    for (int i = 1; i <= ans[0]; ++ i) {
        int new_dis = cur_ans;
        int good_rem = 1;
        for (int j = 1; j <= n; ++ j) {
            if (cur_q[j] - Q[j][ans[i]] < q[j]) {
                good_rem = 0;
                break;
            }
        }
        if (good_rem == 0) continue;
        int u = (i == 1) ? 0 : ans[i - 1];
        int v = (i == ans[0]) ? 0 : ans[i + 1];
        new_dis -= d[u][ans[i]] + d[ans[i]][v];
        new_dis += d[u][v];

        bool is_tabu = tabu_set.count({3, {ans[i], 0}}) > 0;
        if (is_tabu && new_dis >= best_ans) continue;

        if (new_dis < best_new_dis) {
            best_new_dis = new_dis;
            best_i = i;
        }
    }
    
    if (best_i == -1) return 0;
    
    tabu_set.insert({4, {ans[best_i], 0}});
    Q_tabu.push({4, {ans[best_i], 0}});

    for (int j = 1; j <= n; ++ j) {
        cur_q[j] -= Q[j][ans[best_i]];
    }
    vis[ans[best_i]] = 0;
    for (int j = best_i; j < ans[0]; ++ j) {
        ans[j] = ans[j + 1];
    }
    -- ans[0];
    cur_ans = best_new_dis;
    return 1;
}

bool try_add_shelf() {
    int best_new_dis = 1e18;
    int best_i = -1;
    int best_shelf = -1;
    
    for (int i = 1; i <= ans[0] + 1; ++ i) {
        for (int shelf = 1; shelf <= m; ++ shelf) {
            if (vis[shelf]) continue;
            int new_dis = cur_ans;
            int u = (i == 1) ? 0 : ans[i - 1];
            int v = (i > ans[0]) ? 0 : ans[i];
            new_dis -= d[u][v];
            new_dis += d[u][shelf] + d[shelf][v];
            bool is_tabu = tabu_set.count({4, {shelf, 0}}) > 0;
            if (is_tabu && new_dis >= best_ans) continue;
            if (new_dis < best_new_dis) {
                best_new_dis = new_dis;
                best_i = i;
                best_shelf = shelf;
            }
        }
    }
    
    if (best_i == -1) return 0;
    
    tabu_set.insert({3, {best_shelf, 0}});
    Q_tabu.push({3, {best_shelf, 0}});

    vis[best_shelf] = 1;
    for (int j = 1; j <= n; ++ j) {
        cur_q[j] += Q[j][best_shelf];
    }
    for (int j = ans[0] + 1; j > best_i; -- j) {
        ans[j] = ans[j - 1];
    }
    ans[best_i] = best_shelf;
    ++ ans[0];
    cur_ans = best_new_dis;
    return 1;
}

void diversify() {
    int num_random_swaps = ans[0] / 4;
    for (int k = 0; k < num_random_swaps; ++k) {
        if (ans[0] < 2) break;
        int i = rng() % ans[0] + 1;
        int j = rng() % ans[0] + 1;
        if (i != j) {
            swap(ans[i], ans[j]);
        }
    }
    
    cur_ans = 0;
    for (int i = 1; i <= ans[0]; ++i) {
        int u = (i == 1) ? 0 : ans[i - 1];
        cur_ans += d[u][ans[i]];
    }
    cur_ans += d[ans[ans[0]]][0];
}

void local_search_with_tabu() {
    cur_ans = best_ans;
    for (int i = 0; i <= ans[0]; ++ i) {
        best_ans_tabu[i] = ans[i];
    }
    MX_QUEUE = max(10LL, ans[0] / 2);
    int cnt = 0;
    no_improve_cnt = 0;

    auto start_time = chrono::high_resolution_clock::now();

    while (1) {

        auto now = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = now - start_time;
        if (elapsed.count() > time_limit_sec) break;

        ++ cnt;
        int op = rng() % 4;
        bool moved = false;
        
        if (op == 0) {
            moved = try_2opt();
        } else if (op == 1) {
            moved = try_swap_shelves();
        } else if (op == 2) {
            moved = try_remove_shelf();
        } else {
            moved = try_add_shelf();
        }
        
        while (!Q_tabu.empty() && Q_tabu.size() > MX_QUEUE) {
            pair<int, pair<int,int>> p = Q_tabu.front();
            tabu_set.erase(p);
            Q_tabu.pop();
        }

        if (!moved) continue;
        
        if (cur_ans < best_ans) {
            best_ans = cur_ans;
            for (int i = 0; i <= ans[0]; ++ i) {
                best_ans_tabu[i] = ans[i];
            }
            no_improve_cnt = 0;
        } else {
            no_improve_cnt++;
        }
        
        if (no_improve_cnt >= 500) {
            diversify();
            no_improve_cnt = 0;
            tabu_set.clear();
            while (!Q_tabu.empty()) Q_tabu.pop();
        }
        
        if (cnt % 1000 == 0) {
            cur_ans = best_ans;
            for (int i = 0; i <= best_ans_tabu[0]; ++ i)
                ans[i] = best_ans_tabu[i];
            memset(cur_q, 0, sizeof(cur_q));
            memset(vis, 0, sizeof(vis));
            for (int i = 1; i <= ans[0]; ++i) {
                vis[ans[i]] = 1;
                for (int j = 1; j <= n; ++j) {
                    cur_q[j] += Q[j][ans[i]];
                }
            }
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
            if(d[i][j] > 0) cMin = min(cMin, d[i][j]);
        }
    }
    for (int i = 1; i <= n; ++ i) cin >> q[i];
    greedy();
    local_search_with_tabu();
    cout << best_ans_tabu[0] << "\n";
    for (int i = 1; i <= best_ans_tabu[0]; ++ i) {
        cout << best_ans_tabu[i] << " ";
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
