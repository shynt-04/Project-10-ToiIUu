#include <bits/stdc++.h>
const long long inf = 1e18;
#define int long long
using namespace std;

const int maxn = 1005;
int n, m;
int Q[maxn][maxn];
int d[maxn][maxn];
int q[maxn], cur_q[maxn];
int best_ans = inf;
int vis[maxn];
int ans[maxn];

bool check_stock(){
    for (int i = 1; i<= n; i++){
        if (cur_q[i] < q[i]) return 0;
    }
    return 1;
}

struct State {
    vector<int> path;
    int cur_q[maxn];
    int vis[maxn];
    int last;
    int cost;
};

bool cmp_state(const State &a, const State &b) {
    return a.cost < b.cost;
}

void greedy() {
    memset(cur_q, 0, sizeof(cur_q));
    memset(vis, 0, sizeof(vis));
    ans[0] = 0;

    int start = 0;
    int total_dis = 0;

    while (!check_stock()) {
        int best = -1;
        int best_dist = inf;

        for (int i = 1; i <= m; i++) {
            if (vis[i]) continue;
            if (d[start][i] < best_dist) {
                best_dist = d[start][i];
                best = i;
            }
        }

        vis[best] = 1;
        total_dis += d[start][best];
        for (int i = 1; i <= n; i++)
            cur_q[i] += Q[i][best];

        start = best;
        ans[++ans[0]] = best;
    }

    total_dis += d[start][0];
    best_ans = total_dis;
}

void beam_search(int MAX_DEPTH) {
    const int BEAM_WIDTH = 3;
    vector<State> beam, next_beam;
    State start;
    memset(start.cur_q, 0, sizeof(start.cur_q));
    memset(start.vis, 0, sizeof(start.vis));
    start.last = 0;
    start.cost = 0;
    beam.push_back(start);

    for (int depth = 1; depth <= MAX_DEPTH; depth++) {
        next_beam.clear();
        for (auto &s : beam) {
            if (check_stock()) {
                int total_cost = s.cost + d[s.last][0];
                if (total_cost < best_ans) {
                    best_ans = total_cost;
                    ans[0] = s.path.size();
                    for (int i = 0; i < s.path.size(); i++) {
                        ans[i+1] = s.path[i];
                    }
                }
                continue;
            }

            vector<pair<int, int>> candidates;
            for (int i = 1; i <= m; i++) {
                if (s.vis[i]) continue;
                candidates.push_back({d[s.last][i], i});
            }
            sort(candidates.begin(), candidates.end());

            int limit = min((int)candidates.size(), BEAM_WIDTH);
            for (int idx = 0; idx < limit; idx++) {
                int i = candidates[idx].second;
                State ns = s;
                ns.vis[i] = 1;
                ns.path.push_back(i);
                ns.cost += d[s.last][i];
                ns.last = i;
                for (int j = 1; j <= n; j++)
                    ns.cur_q[j] += Q[j][i];
                if (ns.cost < best_ans)
                    next_beam.push_back(ns);
            }
        }
        if (next_beam.empty()) break;
        sort(next_beam.begin(), next_beam.end(), cmp_state);
        beam.clear();
        for (int i = 0; i < min((int)next_beam.size(), BEAM_WIDTH); i++)
            beam.push_back(next_beam[i]);
    }
}

void solve(){
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

    int MAX_DEPTH = ans[0] + 3;

    beam_search(MAX_DEPTH);

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
