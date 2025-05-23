#include <bits/stdc++.h>
#define MAXN 105
#define MAXT 505
using namespace std;

struct Point {
    int x, y;

    Point() {}
    Point(int x, int y): x(x), y(y) {}
};

enum Orientation {
    VERTICAL,
    HORIZONTAL
};

int dx[] = {0, 1, 0, -1}, dy[] = {1, 0, -1, 0};
int T, n, movements[MAXN][MAXT];
Point end_point;
int e1, e2, e3;
pair<Point, Point> positions[MAXN][MAXT];

void DIE(float points, const char *msg) {
    FILE *f;

    f = fopen("score.verif", "wt");
    fprintf(f, "%.1f", points);
    fclose(f);

    f = fopen("output.verif", "wt");
    fprintf(f, "%s", msg);
    fclose(f);

    exit(0);
}

int chr_to_dir(char c) {
    switch (c) {
        case 'N': return 0;
        case 'E': return 1;
        case 'S': return 2;
        case 'V': return 3;
    }

    assert (false);
    return -1;
}

void read_input(FILE *file_in) {
    assert (fscanf(file_in, "%d %d", &T, &n) == 2);
    assert (fscanf(file_in, "%d %d", &end_point.x, &end_point.y) == 2);
    assert (fscanf(file_in, "%d %d %d", &e1, &e2, &e3) == 3);
    for (int i = 1; i <= n; i++)
        assert (fscanf(file_in, "%d %d %d %d",
                       &positions[i][0].first.x, &positions[i][0].first.y,
                       &positions[i][0].second.x, &positions[i][0].second.y) == 4);
    for (int i = 1; i <= n; i++) {
        char movement[MAXT];
        assert (fscanf(file_in, "%s\n", movement) == 1);
        assert ((int) strlen(movement) == T);
        for (int j = 0; j < T; j++)
            movements[i][j] = chr_to_dir(movement[j]);
    }
}

Point move_point(Point &p, int dir) {
    return Point(p.x + dx[dir], p.y + dy[dir]);
}

pair<Point, Point> move_segm(pair<Point, Point> &segm, int dir) {
    return make_pair(move_point(segm.first, dir), move_point(segm.second, dir));
}

int segm_length(pair<Point, Point> &segm) {
    if (segm.first.x == segm.second.x)
        return abs(segm.first.y - segm.second.y);
    assert (segm.first.y == segm.second.y);
    return abs(segm.first.x - segm.second.x);
}

int segm_orientation(pair<Point, Point> &segm) {
    if (segm.first.x == segm.second.x)
        return VERTICAL;
    return HORIZONTAL;
}

Point rel_pos_to_abs_pos(pair<Point, Point> &segm, int offset) {
    if (segm.first.x == segm.second.x) {
        assert (segm.first.y < segm.second.y);
        return Point(segm.first.x, segm.first.y + offset);
    }
    assert (segm.first.y == segm.second.y);
    assert (segm.first.x < segm.second.x);
    return Point(segm.first.x + offset, segm.first.y);
}

int get_jump_offset(int s_idx1, int offset1, int s_idx2, int t) {
    Point p = rel_pos_to_abs_pos(positions[s_idx1][t], offset1);

    pair<Point, Point> &s2 = positions[s_idx2][t];
    if (s2.first.x == s2.second.x) {
        if (p.x == s2.first.x && s2.first.y <= p.y && p.y <= s2.second.y)
            return p.y - s2.first.y;
        return -1;
    }
    assert (s2.first.y == s2.second.y);
    if (p.y == s2.first.y && s2.first.x <= p.x && p.x <= s2.second.x)
        return p.x - s2.first.x;
    return -1;
}

void compute_positions() {
    for (int t = 0; t < T; t++)
        for (int i = 1; i <= n; i++)
            positions[i][t + 1] = move_segm(positions[i][t], movements[i][t]);
}

void check_moves(vector<pair<char, int>> &moves, int expected_effort) {
    char msg[100];

    compute_positions();

    int segm_idx = 1, offset = 0;
    int t = 0, effort = 0;
    for (auto &m: moves) {
        int segm_lg = segm_length(positions[segm_idx][0]);
        int segm_or = segm_orientation(positions[segm_idx][0]);
        int new_segm_idx;

        switch (m.first) {
            case 'H':
                effort += e1;
                break;
            
            case 'N':
            case 'S':
                if (segm_or == HORIZONTAL) {
                    sprintf(msg, "WA (mutare N/S pe un bustean orizontal la timpul %d)", t);
                    DIE(0, msg);
                }
                offset += (m.first == 'N' ? 1 : -1);
                if (offset < 0 || offset > segm_lg) {
                    sprintf(msg, "WA (mutare peste capatul busteanului la timpul %d)", t);
                    DIE(0, msg);
                }
                effort += e2;
                break;

            case 'E':
            case 'V':
                if (segm_or == VERTICAL) {
                    sprintf(msg, "WA (mutare E/V pe un bustean vertical at time %d)", t);
                    DIE(0, msg);
                }
                offset += (m.first == 'E' ? 1 : -1);
                if (offset < 0 || offset > segm_lg) {
                    sprintf(msg, "WA (mutare peste capatul busteanului la timpul %d)", t);
                    DIE(0, msg);
                }
                effort += e2;
                break;

            case 'J':
                new_segm_idx = m.second;

                offset = get_jump_offset(segm_idx, offset, new_segm_idx, t);
                if (offset < 0) {
                    sprintf(msg, "WA (jump incorect la timpul %d)", t);
                    DIE(0, msg);
                }
                segm_idx = new_segm_idx;
                effort += e3;
                break;

            default:
                sprintf(msg, "WA (mutare incorecta la timpul %d)", t);
                DIE(0, msg);
        }

        t += 1;
    }

    Point final_point = rel_pos_to_abs_pos(positions[segm_idx][t], offset);
    if (final_point.x != end_point.x || final_point.y != end_point.y) {
        sprintf(msg, "WA (invalid final position (%d, %d) vs (%d, %d))",
                final_point.x, final_point.y, end_point.x, end_point.y);
        DIE(0, msg);
    }

    assert (effort >= expected_effort);
    if (effort > expected_effort) {
        sprintf(msg, "WA (effort is higher than reported %d vs %d)", effort, expected_effort);
        DIE(0, msg);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        DIE(0, "Usage ./verif problem_name test_points");
    }

    string in_name = string(argv[1]) + string(".in");
    string out_name = string(argv[1]) + string(".out");

    float points = atof(argv[2]);

    FILE *file_in = fopen(in_name.c_str(), "rt"),
         *file_out = fopen(out_name.c_str(), "rt"),
         *file_ref = fopen("res.ok", "rt");

    if (file_ref == NULL) {
        DIE(0, "Nu exista fisierul de referinta");
    }

    if (file_out == NULL) {
        DIE(0, "Nu exista fisier de output");
    }

    if (file_in == NULL) {
        DIE(0, "Nu exista fisier de input");
    }

    long long sol_out, sol_ref;
    assert (fscanf(file_ref, "%lld", &sol_ref) == 1);
    if (fscanf(file_out, "%lld", &sol_out) <= 0)
        DIE(0, "Fisierul de iesire nu contine energia minima pe prima linie.");

    if (sol_out != sol_ref) {
        char c[100];

        #ifdef ONLINE_JUDGE
        sprintf(c, "WA (energie minima incorecta)");
        #else
        sprintf(c, "WA (energie minima incorecta %lld vs %lld)", sol_out, sol_ref);
        #endif

        DIE(0, c);
    }

    read_input(file_in);
    int out_m;

    if (fscanf(file_out, "%d\n", &out_m) <= 0)
        DIE(0, "WA (numar de mutari negasit)");

    if (out_m <= 0 || out_m > T)
        DIE(0, "WA (numar de mutari invalid)");

    char move[50];
    vector<pair<char, int>> moves;
    for (int i = 0; i < out_m; i++) {
        if (fscanf(file_out, "%s", move) <= 0)
            DIE(0, "WA (numar de mutari invalid)");
        int x = 0;
        if (move[0] == 'J') {
            if (fscanf(file_out, "%d", &x) <= 0) {
                char msg[50];
                sprintf(msg, "WA (jump incorect la timpul %d)", i);
                DIE(0, msg);
            }
        }
        moves.push_back(make_pair(move[0], x));
    }

    check_moves(moves, sol_ref);

    DIE(points, "OK ");

    return 0;
}
