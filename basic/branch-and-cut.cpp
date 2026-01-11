// branch and cut

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
int cur_ans[maxn], ans[maxn];
double best_ratio[maxn];

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

bool demand_cut() {
    for (int i = 1; i <= n; ++i) {
        int can_get = cur_q[i];
        for (int j = 1; j <= m; ++j)
            if (!vis[j])
                can_get += Q[i][j];
        if (can_get < q[i]) return true;
    }
    return false;
}

int knapsack_lb() {
    int lb = 0;
    for (int i = 1; i <= n; ++i) {
        if (cur_q[i] < q[i]) {
            lb += (int)ceil((q[i] - cur_q[i]) * best_ratio[i]);
        }
    }
    return lb;
}


void branch_and_cut(int pos, int total_dis = 0) {
    if (check_stock()) {
        if (total_dis + d[cur_ans[pos - 1]][0] < best_ans) {
            best_ans = total_dis + d[cur_ans[pos - 1]][0];
            for (int i = 1; i <= pos - 1; ++ i) {
                ans[i] = cur_ans[i];
            }
            ans[0] = pos - 1;
        }
        return;
    }
    time_cnt ++;
    if (time_cnt > 1e8) return;
    if (pos > m) return;
    
    if (demand_cut()) return;
    int last = (pos > 1 ? cur_ans[pos - 1] : 0);

    // knapsack LB + return LB
    int lb = knapsack_lb() + d[last][0];
    if (total_dis + lb >= best_ans) return;

    
    for (int i = 1; i <= m; ++i) {
        if (vis[i]) continue;

        vis[i] = 1;
        cur_ans[pos] = i;

        for (int u = 1; u <= n; ++u)
            cur_q[u] += Q[u][i];

        branch_and_cut(pos + 1, total_dis + d[last][i]);

        for (int u = 1; u <= n; ++u)
            cur_q[u] -= Q[u][i];

        vis[i] = 0;
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
    for (int i = 1; i <= n; ++i) {
        best_ratio[i] = 1e18;
        for (int j = 1; j <= m; ++j)
            if (Q[i][j] > 0)
                best_ratio[i] = min(best_ratio[i], (double)d[0][j] / Q[i][j]);
    }

    for (int i = 1; i <= n; ++ i) cin >> q[i];
    greedy();
    memset(vis, 0, sizeof(vis));
    memset(cur_q, 0, sizeof(cur_q));
    branch_and_cut(1, 0);
    cout << ans[0] << "\n";
    for (int i = 1; i <= ans[0]; ++ i) {
        cout << ans[i] << " ";
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
