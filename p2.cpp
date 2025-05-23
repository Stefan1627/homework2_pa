#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ifstream fin("p2.in");
    ofstream fout("p2.out");
    if (!fin || !fout) {
        cerr << "Error opening files\n";
        return 1;
    }

    int N, M;
    long long K;
    fin >> N >> M >> K;

    vector<int> mat(N * M);
    long long global_min = LLONG_MAX, global_max = LLONG_MIN;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            int x;
            fin >> x;
            mat[i*M + j] = x;
            if (x < global_min) global_min = x;
            if (x > global_max) global_max = x;
        }
    }

    // If K covers the entire range, the whole grid is one big zone:
    if (global_max - global_min <= K) {
        fout << (N * M) << "\n";
        return 0;
    }

    // 1) Extract and sort unique values
    vector<int> vals = mat;
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    int U = vals.size();

    // 2) Group cells by their value‐index in `vals`
    vector<vector<int>> pos(U);
    for (int id = 0; id < N*M; id++) {
        int v = mat[id];
        int idx = lower_bound(vals.begin(), vals.end(), v) - vals.begin();
        pos[idx].push_back(id);
    }

    // 3) Sliding‐window over `vals[l]`…`vals[r-1]`
	// satisfying vals[r-1] - vals[l] <= K
    vector<char> allow(N*M, 0);
    vector<int>  seen(N*M, 0);
    int currentStamp = 0;
    int r = 0, best = 0;

    // manual queue to avoid vector overhead
    static int Q[100*100];
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    for (int l = 0; l < U; l++) {
        // remove the old minimum‐value cells
        if (l > 0) {
            for (int id : pos[l-1]) {
                allow[id] = 0;
            }
        }
        // advance r to include as many new values as fit in range K
        while (r < U && (long long)vals[r] - vals[l] <= K) {
            for (int id : pos[r]) {
                allow[id] = 1;
            }
            r++;
        }

        // we'll only start a BFS from each cell whose
		// value is the current minimum
        currentStamp++;
        for (int id0 : pos[l]) {
            if (!allow[id0] || seen[id0] == currentStamp)
                continue;
            // BFS flood‐fill the component containing id0
            seen[id0] = currentStamp;
            int head = 0, tail = 0, area = 1;
            Q[tail++] = id0;
            while (head < tail) {
                int u = Q[head++];
                int ux = u / M, uy = u % M;
                for (int d = 0; d < 4; d++) {
                    int vx = ux + dx[d], vy = uy + dy[d];
                    if (vx < 0 || vx >= N || vy < 0 || vy >= M)
                        continue;
                    int vId = vx*M + vy;
                    if (allow[vId] && seen[vId] != currentStamp) {
                        seen[vId] = currentStamp;
                        Q[tail++] = vId;
                        area++;
                    }
                }
            }
            best = max(best, area);
        }

        // early stop if we've already found the maximum possible
        if (best == N*M)
            break;
    }

    fout << best << "\n";
    return 0;
}
