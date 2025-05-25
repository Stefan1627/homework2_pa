// Stefan CALMAC 324CC
#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int main() {
	ifstream fin("p1.in");
	ofstream fout("p1.out");

	int N;
	fin >> N;
	vector<int> v(N + 1);
	for (int i = 1; i <= N; i++) {
		fin >> v[i];

		// every other distance that is not v[1] must be != 0
		if (i > 2 && v[i] == 0) {
			fout << "-1\n";
			return 0;
		}
	}

	// v[1] must be 0
	if (v[1] != 0) {
		fout << "-1\n";
		return 0;
	}

	// Group nodes by distance
	int maxd = *max_element(v.begin() + 1, v.end());
	vector<vector<int>> byDist(maxd + 1);
	for (int i = 1; i <= N; i++) {
		byDist[v[i]].push_back(i);
	}

	// Feasibility check: for any distance d > 0 with nodes,
	// there must be nodes at d-1
	for (int d = 1; d <= maxd; d++) {
		if (!byDist[d].empty() && byDist[d - 1].empty()) {
			fout << "-1\n";
			return 0;
		}
	}

	// Build the graph edges by connecting each node
	// at distance d to one at distance d-1
	vector<pair<int, int>> edges;
	for (int d = 1; d <= maxd; d++) {
		int parent = byDist[d - 1][0];

		for (int u : byDist[d]) {
			edges.emplace_back(parent, u);
		}
	}

	fout << edges.size() << "\n";
	for (auto &e : edges) {
		fout << e.first << " " << e.second << "\n";
	}

	return 0;
}
