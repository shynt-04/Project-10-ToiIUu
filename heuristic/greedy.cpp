#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>

#define int long long

using namespace std;

const int maxn = 1005;

int n, m;
int Q[maxn][maxn];
int d[maxn][maxn];
int q[maxn], cur_q[maxn];
int vis[maxn];
vector<int> ans;

// kiểm tra xem kệ 'shelf_idx' có món hàng nào mình đang thiếu không
bool is_useful(int shelf_idx) {
    for (int i = 1; i <= n; ++i) {
        if (cur_q[i] < q[i] && Q[i][shelf_idx] > 0) {
            return true; 
        }
    }
    return false;
}

bool check_stock() {
    for (int i = 1; i <= n; ++ i) {
        if (cur_q[i] < q[i]) return 0;
    }
    return 1;
}

void greedy_smart_nn() {
    int start = 0, total_dis = 0;
    
    while (!check_stock()) {
        int best = -1;
        
        // Tìm kệ gần nhất có ích
        for (int i = 1; i <= m; ++ i) {
            if (vis[i]) continue;
            if (!is_useful(i)) continue; 
            if (best == -1 || d[start][i] < d[start][best]) {
                best = i;
            }
        }
        
        if (best == -1) break; // Không còn đường đi

        vis[best] = 1;
        total_dis += d[start][best];
        
        // Cập nhật kho hàng
        for (int i = 1; i <= n; ++ i) {
            cur_q[i] += Q[i][best]; 
        }
        
        start = best;
        ans.push_back(best);
    }
    
    total_dis += d[start][0];
    
    cout << ans.size() << '\n';
    for (auto x : ans) cout << x << ' ';
}

void solve() {
    cin >> n >> m;
    for (int i = 1; i <= n; ++ i) {
        for (int j = 1; j <= m; ++ j) cin >> Q[i][j];
    }
    for (int i = 0; i <= m; ++ i) {
        for (int j = 0; j <= m; ++ j) cin >> d[i][j];
    }
    for (int i = 1; i <= n; ++ i) cin >> q[i];
    
    greedy_smart_nn();
}

signed main() {
    ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
    if (fopen("annotshy.inp", "r")) {
        freopen("annotshy.inp", "r", stdin);
        freopen("annotshy.out", "w", stdout);
    }
    solve();
}
