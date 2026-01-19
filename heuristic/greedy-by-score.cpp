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
int best_ans = 1e18;
int vis[maxn];
int cur_ans, ans[maxn];
int MX_QUEUE = 7;

bool check_stock() {
    for (int i = 1; i <= n; ++ i) {
        if (cur_q[i] < q[i]) return 0;
    }
    return 1;
}

void greedy_by_score() {
    int start = 0, total_dis = 0;
    while (!check_stock()) {
        int best = -1;
        double best_score = -1e18;
        for (int i = 1; i <= m; ++ i) {
            if (vis[i]) continue;
            int value = 0;
            for (int j = 1; j <= n; ++ j) {
                value += min(Q[j][i], max(0LL, q[j] - cur_q[j]));
            }
            double score = 1.0 * value / (d[start][i]);
            if (score > best_score) {
                best_score = score;
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
    // greedy();
    greedy_by_score();
    cout << ans[0] << "\n";
    for (int i = 1; i <= ans[0]; ++ i) {
        cout << ans[i] << " ";
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
