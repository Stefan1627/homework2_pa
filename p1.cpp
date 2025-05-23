#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Open input and output files using ifstream/ofstream
    ifstream fin("p1.in");
    ofstream fout("p1.out");
    if (!fin || !fout) {
        cerr << "Error opening file\n";
        return 1;
    }

    int N;
    fin >> N;
    vector<int> v(N+1);
    for (int i = 1; i <= N; i++) {
        fin >> v[i];
    }

    // v[1] must be 0, and no other node can have distance 0
    if (v[1] != 0) {
        fout << "-1\n";
        return 0;
    }
    for (int i = 2; i <= N; i++) {
        if (v[i] == 0) {
            fout << "-1\n";
            return 0;
        }
    }

    // Group nodes by distance
    int maxd = *max_element(v.begin()+1, v.end());
    vector<vector<int>> byDist(maxd+1);
    for (int i = 1; i <= N; i++) {
        // Although v[i] is guaranteed within [0, N], we double-check
        if (v[i] < 0 || v[i] > maxd) {
            fout << "-1\n";
            return 0;
        }
        byDist[v[i]].push_back(i);
    }

    // Feasibility check: for any distance d>0 with nodes,
	// there must be nodes at d-1
    for (int d = 1; d <= maxd; d++) {
        if (!byDist[d].empty() && byDist[d-1].empty()) {
            fout << "-1\n";
            return 0;
        }
    }

    // Build the graph edges by connecting each node
	// at distance d to one at d-1
    vector<pair<int, int>> edges;
    for (int d = 1; d <= maxd; d++) {
        int parent = byDist[d-1][0];
        for (int u : byDist[d]) {
            edges.emplace_back(parent, u);
        }
    }

    // Write output
    fout << edges.size() << "\n";
    for (auto &e : edges) {
        fout << e.first << " " << e.second << "\n";
    }

    return 0;
}
