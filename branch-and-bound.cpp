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

// bnb -> dung dap an cua greedy lam bound
void backtrack(int pos, int total_dis = 0) {
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
    if (time_cnt > 1e7) return;
    if (pos > m) return;
    if (total_dis + cMin * (((n * m - pos) / m) + 1)  >= best_ans) return;
    for (int i = 1; i <= m; ++ i) {
        if (vis[i]) continue;
        vis[i] = 1;
        int last_id = 0;
        if (pos > 1) last_id = cur_ans[pos - 1];
        total_dis += d[last_id][i];
        cur_ans[pos] = i;
        for (int u = 1; u <= n; ++ u) {
            cur_q[u] += Q[u][i];
        }
        // bound
        if (total_dis < best_ans) {
            backtrack(pos + 1, total_dis);
        }
        for (int u = 1; u <= n; ++ u) {
            cur_q[u] -= Q[u][i];
        }
        total_dis -= d[last_id][i];
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
    for (int i = 1; i <= n; ++ i) cin >> q[i];
    greedy();
    memset(vis, 0, sizeof(vis));
    memset(cur_q, 0, sizeof(cur_q));
    backtrack(1, 0);
    cout << ans[0] << "\n";
    for (int i = 1; i <= ans[0]; ++ i) {
        cout << ans[i] << " ";
    }
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
