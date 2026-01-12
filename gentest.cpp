#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <numeric>

using namespace std;

int MIN_N = 5, MAX_N = 10;
int MIN_M = 20, MAX_M = 50;
const int MAX_SCOPE = 1000;
const double DEMAND_RATIO = 0.8;

struct Point {
    int x, y;
};

// Hàm tính khoảng cách Euclid làm tròn
int dist(Point p1, Point p2) {
    return round(sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)));
}

int main() {
    random_device rd;
    mt19937 rng(rd());

    ofstream inp("annotshy.inp");

    cin >> MIN_N >> MAX_N >> MIN_M >> MAX_M;
    uniform_int_distribution<int> distN(MIN_N, MAX_N);
    uniform_int_distribution<int> distM(MIN_M, MAX_M);
    int n = distN(rng);
    int m = distM(rng);
    inp << n << " " << m << "\n";
    vector<vector<int>> Q(n + 1, vector<int>(m + 1));
    vector<int> total_supply(n + 1, 0);
    uniform_int_distribution<int> distQ(0, 10); 
    
    // xay dung ma tran thua cho Q
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (distQ(rng) > 4) { 
                Q[i][j] = distQ(rng);
            } else {
                Q[i][j] = 0;
            }
            inp << Q[i][j] << " ";
            total_supply[i] += Q[i][j];
        }
        inp << "\n";
    }

    // sinh khoang cach bang cach gen 2 toa do roi tinh khoang cach :V
    vector<Point> points(m + 1);
    uniform_int_distribution<int> distCoord(0, MAX_SCOPE);
    points[0] = {0, 0}; 
    for (int i = 1; i <= m; ++i) {
        points[i] = {distCoord(rng), distCoord(rng)};
    }
    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= m; ++j) {
            int d = dist(points[i], points[j]);
            inp << d << " ";
        }
        inp << "\n";
    }

    for (int i = 1; i <= n; ++i) {
        if (total_supply[i] == 0) {
            inp << 0 << " ";
            continue;
        }
        int max_demand = (int)(total_supply[i] * DEMAND_RATIO);
        if (max_demand < 1) max_demand = 1;
        uniform_int_distribution<int> distDemand(1, max_demand);
        int demand = distDemand(rng);
        inp << demand << " ";
    }
    inp << "\n";

    inp.close();
    return 0;
}