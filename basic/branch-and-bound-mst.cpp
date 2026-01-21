#include <bits/stdc++.h>
const long long inf = 1e9 +7;
const int maxN = 1e5 +7;
#define int long long
using namespace std;
const int maxn = 1005;

chrono::steady_clock::time_point start_time;
const double TIME_LIMIT = 19.5;

int n, m;
int Q[maxn][maxn];
int d[maxn][maxn];
int q[maxn], cur_q[maxn];
int best_ans = 1e18, time_cnt = 0, cMin = 1e18;
int vis[maxn];
int cur_ans[maxn], ans[maxn];

bool time_exceeded() {
    auto current_time = chrono::steady_clock::now();
    double elapsed = chrono::duration<double>(current_time - start_time).count();
    return elapsed >= TIME_LIMIT;
}

struct UnionFind {
    vector<int> parent, rank;
    UnionFind(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for(int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int x) {
        if(parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if(rootX == rootY) return false;
        if(rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if(rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
        return true;
    }
};

bool check_stock(){
    for (int i = 1; i<= n; i++){
        if (cur_q[i] < q[i]) return 0;
    }
    return 1;
}

long long mst_bound(int current_node, const vector<int>& unvisited) {
    if(unvisited.empty()) return 0;
    vector<int> all_nodes = unvisited;
    all_nodes.push_back(current_node);
    all_nodes.push_back(0);
    int node_count = all_nodes.size();
    vector<pair<int, pair<int, int>>> edges;
    for(int i = 0; i < node_count; i++) {
        for(int j = i + 1; j < node_count; j++) {
            int u = all_nodes[i];
            int v = all_nodes[j];
            edges.push_back({d[u][v], {i, j}});
        }
    }
    sort(edges.begin(), edges.end());
    UnionFind uf(node_count);
    long long mst_cost = 0;
    int edges_used = 0;
    for(auto& edge : edges) {
        int weight = edge.first;
        int u = edge.second.first;
        int v = edge.second.second;
        if(uf.unite(u, v)) {
            mst_cost += weight;
            edges_used++;
            if(edges_used == node_count - 1) break;
        }
    }
    return mst_cost;
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

void backtrack(int pos, int total_dis = 0){
    if(time_exceeded()) return;
    if (check_stock()){
        if (total_dis + d[cur_ans[pos-1]][0] < best_ans){
            best_ans = total_dis + d[cur_ans[pos-1]][0];
            for (int i = 1; i<= pos -1; i++){
                ans[i] = cur_ans[i];
            }
            ans[0] = pos-1;
        }
        return;
    }
    time_cnt ++;
    if (time_cnt > 5e7) return;
    if (pos > m) return;

    vector<int> unvisited;
    for(int j = 1; j <= m; j++) {
        if(!vis[j]) unvisited.push_back(j);
    }
    int last_id = 0;
    if (pos > 1) last_id = cur_ans[pos-1];
    long long bound = total_dis + mst_bound(last_id, unvisited);
    if(!unvisited.empty()) {
        bound = min(bound, (int)(total_dis + cMin * (unvisited.size() + 1)));
    }
    if(bound >= best_ans) return;

    vector<pair<int, int>> candidates;
    for(int j : unvisited) {
        candidates.push_back({d[last_id][j], j});
    }
    sort(candidates.begin(), candidates.end());

    for(auto& candidate : candidates) {
        int i = candidate.second;
        if (vis[i]) continue;
        vis[i] = 1;
        total_dis += d[last_id][i];
        cur_ans[pos] = i;
        bool useful = false;
        for(int u = 1; u <= n; u++){
            if(cur_q[u] < q[u] && Q[u][i] > 0) {
                useful = true;
                break;
            }
        }
        if(useful) {
            for (int u = 1; u<=n; u++){
                cur_q[u] += Q[u][i];
            }
            if (total_dis < best_ans){
                backtrack(pos + 1, total_dis);
            }
            for (int u =1 ; u <= n; u++){
                cur_q[u] -= Q[u][i];
            }
        }
        total_dis -= d[last_id][i];
        vis[i] = 0;
    }
}

void solve(){
    start_time = chrono::steady_clock::now();
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
        cout << ans[i];
        if(i < ans[0]) cout << " ";
    }
    cout << "\n";
}

signed main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int t;
    t = 1;
    while (t--)
        solve();
    return 0;
}
