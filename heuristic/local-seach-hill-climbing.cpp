// hill climbing
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>

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
int cur_ans, ans[maxn];

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

bool try_2opt() {
    for (int i = 1; i < ans[0]; ++ i) {
        for (int j = i + 1; j <= ans[0]; ++ j) {
            int new_dis = cur_ans;
            int u = (i == 1) ? 0 : ans[i - 1];
            int v = (j == ans[0]) ? 0 : ans[j + 1];
            new_dis -= d[u][ans[i]] + d[ans[j]][v];
            new_dis += d[u][ans[j]] + d[ans[i]][v];
            for (int k = i; k < j; ++ k) {
                new_dis -= d[ans[k]][ans[k + 1]];
                new_dis += d[ans[k + 1]][ans[k]];
            }
            if (new_dis < cur_ans) {
                reverse(ans + i, ans + j + 1);
                cur_ans = new_dis;
                return 1;
            }
        }
    }
    return 0;
}

bool try_swap_shelves() {
    for (int i = 1; i <= ans[0]; ++ i) {
        for (int j = i + 1; j <= ans[0]; ++ j) {
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
            if (new_dis < cur_ans) {
                swap(ans[i], ans[j]);
                cur_ans = new_dis;
                return 1;
            }
        }
    }
    return 0;
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

void local_search() {
    bool improved = 1;
    cur_ans = best_ans;
    
    while (improved) {
        improved = 0;
        if (try_2opt()) {
            improved = 1;
            continue;
        }
        if (try_swap_shelves()) {
            improved = 1;
            continue;
        }
        if (try_remove_shelf()) {
            improved = 1;
            continue;
        }
        if (try_add_shelf()) {
            improved = 1;
            continue;
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
    local_search();
    cout << ans[0] << "\n";
    for (int i = 1; i <= ans[0]; ++ i) {
        cout << ans[i] << " ";
    }
    // cout << "\n" << cur_ans << "\n";
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
