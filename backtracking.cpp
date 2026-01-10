#include <bits/stdc++.h>

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
int best_ans = 1e18, time_cnt = 0;
int vis[maxn];
vector<int> ans;

bool check_stock() {
    for (int i = 1; i <= n; ++ i) {
        if (cur_q[i] < q[i]) return 0;
    }
    return 1;
}

void calc(int pos, vector<int> &g, int total_dis = 0) {
    if (check_stock()) {
        if (total_dis + d[g.back()][0] < best_ans) {
            best_ans = total_dis + d[g.back()][0];
            ans = g;
        }
        return;
    }
    time_cnt ++;
    if (time_cnt > 1e7) return;
    if (pos > m) return;
    for (int i = 1; i <= m; ++ i) {
        if (vis[i]) continue;
        vis[i] = 1;
        int last_id = 0;
        if (g.size()) last_id = g.back();
        total_dis += d[i][last_id];
        g.push_back(i);
        for (int u = 1; u <= n; ++ u) {
            cur_q[u] += Q[u][i];
        }
        calc(pos + 1, g, total_dis);
        for (int u = 1; u <= n; ++ u) {
            cur_q[u] -= Q[u][i];
        }
        g.pop_back();
        total_dis -= d[i][last_id];
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
        }
    }
    for (int i = 1; i <= n; ++ i) cin >> q[i];
    vector<int> g;
    calc(1, g, 0);
    cout << ans.size() << "\n";
    for (auto x : ans) cout << x << " ";
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
