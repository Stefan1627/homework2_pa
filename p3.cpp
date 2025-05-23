#include <bits/stdc++.h>
#include <fstream>
using namespace std;

static const int INF = 1e9;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // --- I/O ---
    ifstream fin("p3.in");
    ofstream fout("p3.out");
    if (!fin || !fout) {
        cerr << "Error opening p3.in or p3.out\n";
        return 1;
    }

    int T, N;
    fin >> T >> N;

    int target_x, target_y;
    fin >> target_x >> target_y;

    int E1, E2, E3;
    fin >> E1 >> E2 >> E3;

    // Logs endpoints
    vector<int> xs(N), ys(N), xe(N), ye(N);
    for (int i = 0; i < N; i++) {
        fin >> xs[i] >> ys[i] >> xe[i] >> ye[i];
    }
    // Movement strings: for each log, its T‐length directions
    vector<string> moves(N);
    for (int i = 0; i < N; i++) {
        fin >> moves[i];
    }

    // Precompute for each log:
    //  - orientation (horizontal vs vertical)
    //  - segment length (number of steps = |Δx|+|Δy|)
    //  - unit‐vector along the log, (dx,dy) = (1,0) or (0,1)
    vector<bool> horizontal(N);
    vector<int> length(N), dirx(N), diry(N);
    for (int i = 0; i < N; i++) {
        horizontal[i] = (ys[i] == ye[i]);
        length[i]   = horizontal[i]
                      ? abs(xe[i] - xs[i])
                      : abs(ye[i] - ys[i]);
        dirx[i]     = horizontal[i] ? 1 : 0;
        diry[i]     = horizontal[i] ? 0 : 1;
    }

    // Build a flat index for (log i, position p along that log: 0..length[i])
    vector<int> offset(N+1, 0);
    for (int i = 0; i < N; i++) {
        offset[i+1] = offset[i] + (length[i] + 1);
    }
    int S = offset[N];  // total number of (log,pos) states

    // For quick decoding state → (i,p)
    vector<int> state_log(S), state_pos(S);
    for (int i = 0; i < N; i++) {
        for (int p = 0; p <= length[i]; p++) {
            int s = offset[i] + p;
            state_log[s] = i;
            state_pos[s] = p;
        }
    }

    // Precompute cumulative movements of each log: after t steps, how far
    // it has drifted from its initial endpoints.
    // cumdx[i][t], cumdy[i][t] = total Δx,Δy after t moves.
    vector<vector<int>> cumdx(N, vector<int>(T+1, 0)),
                        cumdy(N, vector<int>(T+1, 0));
    auto dir_delta = [&](char c){
        switch (c) {
            case 'N': return make_pair(0, +1);
            case 'S': return make_pair(0, -1);
            case 'E': return make_pair(+1, 0);
            case 'V': return make_pair(-1, 0);
        }
        return make_pair(0, 0);
    };
    for (int i = 0; i < N; i++) {
        for (int t = 1; t <= T; t++) {
            auto d = dir_delta(moves[i][t-1]);
            cumdx[i][t] = cumdx[i][t-1] + d.first;
            cumdy[i][t] = cumdy[i][t-1] + d.second;
        }
    }

    // dp[t][s] = minimal energy to be in state s after t time‐steps
    vector<vector<int>> dp(T+1, vector<int>(S, INF));

    // To reconstruct path:
    //   prev_state[t][s] = previous state‐index at time t-1
    //   prev_act  [t][s] = { action_char,
	//   jump_target_log_index (0-based or -1) }
    vector<vector<int>>     prev_state(T+1, vector<int>(S, -1));
    vector<vector<pair<char, int>>> prev_act(T+1,
							vector<pair<char, int>>(S, {'?', -1}));

    // Initial: at time 0, on log 1 (index 0), at p=0 (the "first" endpoint)
    dp[0][ offset[0] + 0 ] = 0;

    // A little helper to compute absolute position of state s at time t:
    auto get_abs = [&](int s, int t) {
        int i = state_log[s];
        int p = state_pos[s];
        // endpoint1 at time t:
        int ex = xs[i] + cumdx[i][t],
            ey = ys[i] + cumdy[i][t];
        // move p steps along the log:
        return make_pair(ex + p * dirx[i],
                         ey + p * diry[i]);
    };

    // Main DP: for each t → t+1
    for (int t = 0; t < T; t++) {
        // Build a map from absolute coordinate → list of (log i, pos p)
        // so we can find all logs that pass through a given point at time t.
        unordered_map<long long, vector<pair<int, int>>> bucket;
        bucket.reserve(S);
        for (int s = 0; s < S; s++) {
            auto [ax, ay] = get_abs(s, t);
            long long key = ((long long)ax << 32) | (unsigned int)ay;
            bucket[key].emplace_back(state_log[s], state_pos[s]);
        }

        // Relax all transitions from dp[t][*] into dp[t+1][*]
        for (int s = 0; s < S; s++) {
            int cost = dp[t][s];
            if (cost == INF) continue;

            int i = state_log[s];
            int p = state_pos[s];

            // 1) Stay on the same log: pay E1, same (i,p)
            {
                int s2 = s;
                int nc = cost + E1;
                if (nc < dp[t+1][s2]) {
                    dp[t+1][s2]   = nc;
                    prev_state[t+1][s2] = s;
                    prev_act[t+1][s2]   = {'H', -1};
                }
            }

            // 2) Step along the log: pay E2, to (i,p-1) or (i,p+1) if valid
            if (p > 0) {
                int s2 = offset[i] + (p-1);
                int nc = cost + E2;
                char c = horizontal[i] ? 'V' : 'S';
                if (nc < dp[t+1][s2]) {
                    dp[t+1][s2]   = nc;
                    prev_state[t+1][s2] = s;
                    prev_act[t+1][s2]   = {c, -1};
                }
            }
            if (p < length[i]) {
                int s2 = offset[i] + (p+1);
                int nc = cost + E2;
                char c = horizontal[i] ? 'E' : 'N';
                if (nc < dp[t+1][s2]) {
                    dp[t+1][s2]   = nc;
                    prev_state[t+1][s2] = s;
                    prev_act[t+1][s2]   = {c, -1};
                }
            }

            // 3) Jump to any other log that also
			// covers this same absolute point
            {
                auto [ax, ay] = get_abs(s, t);
                long long key = ((long long)ax << 32) | (unsigned int)ay;
                auto &list = bucket[key];
                for (auto &pr : list) {
                    int j = pr.first, pj = pr.second;
                    if (j == i) continue;
                    int s2 = offset[j] + pj;
                    int nc = cost + E3;
                    if (nc < dp[t+1][s2]) {
                        dp[t+1][s2]   = nc;
                        prev_state[t+1][s2] = s;
                        prev_act[t+1][s2]   = {'J', j};
                    }
                }
            }
        }
    }

    // Find the best‐energy way to be exactly at (target_x,target_y)
	// at some time t≤T
    int bestEnergy = INF, bestT = -1, bestS = -1;
    for (int t = 0; t <= T; t++) {
        for (int s = 0; s < S; s++) {
            int cost = dp[t][s];
            if (cost >= bestEnergy) continue;
            auto [ax, ay] = get_abs(s, t);
            if (ax == target_x && ay == target_y) {
                bestEnergy = cost;
                bestT = t;
                bestS = s;
            }
        }
    }

    // If unreachable, print -1 and exit
    if (bestEnergy == INF) {
        fout << "-1\n";
        return 0;
    }

    // Reconstruct the sequence of moves
    vector<string> moves_out;
    int t = bestT, s = bestS;
    while (t > 0) {
        auto [c, j] = prev_act[t][s];
        int ps = prev_state[t][s];
        if (c == 'J') {
            // jump: "J <log_index+1>"
            moves_out.push_back("J " + to_string(j+1));
        } else {
            // 'H', 'N', 'S', 'E', or 'V'
            moves_out.push_back(string(1, c));
        }
        s = ps;
        --t;
    }
    reverse(moves_out.begin(), moves_out.end());

    // Output
    fout << bestEnergy << "\n";
    fout << moves_out.size() << "\n";
    for (auto &mv : moves_out) {
        fout << mv << "\n";
    }

    return 0;
}
