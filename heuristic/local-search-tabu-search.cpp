#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <queue>

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
int tabu[maxn][maxn];
queue<pair<int, int> > Q_tabu;
int MX_QUEUE = 7;

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

// local search algorithm
// tabu search 
bool try_swap_shelves() {
    int best_new_dis = 1e18;
    int best_i = -1, best_j = -1;
    int best_a = -1, best_b = -1;

    while (!Q_tabu.empty() && Q_tabu.size() > MX_QUEUE) {
        auto p = Q_tabu.front();
        tabu[p.F][p.S] = 0;
        Q_tabu.pop();
    }

    for (int t = 1; t <= 20000; ++t) {
        int i = rand() % ans[0] + 1;
        int j = rand() % ans[0] + 1;
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

        bool is_tabu = tabu[a][b];

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

    tabu[best_b][best_a] = 1;
    Q_tabu.push({best_b, best_a});

    return 1;
}

bool try_remove_shelf() {
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
        if (new_dis < cur_ans) {
            for (int j = 1; j <= n; ++ j) {
                cur_q[j] -= Q[j][ans[i]];
            }
            vis[ans[i]] = 0;
            for (int j = i; j < ans[0]; ++ j) {
                ans[j] = ans[j + 1];
            }
            -- ans[0];
            cur_ans = new_dis;
            return 1;
        }
    }
    return 0;
}

bool try_add_shelf() {
    for (int i = 1; i <= ans[0]; ++ i) {
        for (int shelf = 1; shelf <= m; ++ shelf) {
            if (vis[shelf]) continue;
            int new_dis = cur_ans;
            int u = (i == 1) ? 0 : ans[i - 1];
            int v = ans[i];
            new_dis -= d[u][v];
            new_dis += d[u][shelf] + d[shelf][v];
            if (new_dis < cur_ans) {
                vis[shelf] = 1;
                for (int j = 1; j <= n; ++ j) {
                    cur_q[j] += Q[j][shelf];
                }
                for (int j = ans[0] + 1; j > i; -- j) {
                    ans[j] = ans[j - 1];
                }
                ans[i] = shelf;
                ++ ans[0];
                cur_ans = new_dis;
                return 1;
            }
        }
    } 
    return 0;
}

void local_search_with_tabu() {
    cur_ans = best_ans;
    for (int i = 0; i <= ans[0]; ++ i) {
        best_ans_tabu[i] = ans[i];
    }
    MX_QUEUE = max(7LL, ans[0] / 2);
    int cnt = 0;

    while (cnt < 5000) {
        ++ cnt;
        if (try_swap_shelves()) {
            if (cur_ans < best_ans) {
                best_ans = cur_ans;
                for (int i = 0; i <= ans[0]; ++ i) {
                    best_ans_tabu[i] = ans[i];
                }
            }
            continue;
        }
        if (try_remove_shelf()) {
            if (cur_ans < best_ans) {
                best_ans = cur_ans;
                for (int i = 0; i <= ans[0]; ++ i) {
                    best_ans_tabu[i] = ans[i];
                }
            }
            continue;
        }
        if (try_add_shelf()) {
            if (cur_ans < best_ans) {
                best_ans = cur_ans;
                for (int i = 0; i <= ans[0]; ++ i) {
                    best_ans_tabu[i] = ans[i];
                }
            }
        }
        if (cnt % 200 == 0) {
            cur_ans = best_ans;
            for (int i = 0; i <= best_ans_tabu[0]; ++i)
                ans[i] = best_ans_tabu[i];
        }
    }
    
}

void solve(int Test) {
    srand(time(0));
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
    cout << "\n" << best_ans << "\n";
}

signed main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0), cout.tie(0);
    if (fopen(Task".inp", "r")) {
        freopen(Task".inp", "r", stdin);
        freopen(Task".out", "w", stdout);
    }
    int test = 1;
    // cin >> test;
    for (int i = 1; i <= test; ++ i) {
        // cout << "Case #" << i << ": ";
        solve(i);
    }
}
// g++ .\annotshy.cpp -o annotshy.exe -Wall -Wextra -std=c++17
