#include <iostream>
#include <stack>
#include <deque>
#include <random>
#include <map>
#include <cmath>
#include <numeric>
#include <queue>
#include <set>
#include <algorithm>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>

using namespace std;
#define all(a) a.begin(), a.end()
#define pb push_back
#define get(v) for (int & iq : v) cin >> iq
#define give(vv) for (int & iqq : vv) cout << iqq << " "
#define vi vector <int>
#define pii pair <int, int>
#define SOLVE int t; cin >> t; while (t--) {solve();}
typedef __int128 lll;
typedef long long ll;
typedef long double ld;
// #define int ll
ll inf = 1e9 + 7, mod = 1e6 + 3;
int N = 1e6;

static const int MIN_N = 500;
static const int MAX_N = 100000;
static const int STEP = 100;
static const int REPEATS = 1;
static const int MIN_RANDOM_VALUE = 0;
static const int MAX_RANDOM_VALUE = 6000;

static const int INSERTION_CUTOFF = 16;

void insertion_sort(vector<int> &a, int l, int r) {
    for (int i = l + 1; i < r; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= l && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

static void heap_sort_range(vector<int> &a, int l, int r) {
    make_heap(a.begin() + l, a.begin() + r);
    sort_heap(a.begin() + l, a.begin() + r);
}

void quick_sort_random(vector<int> &v, int l, int r, mt19937 &rng) {
    if (r - l <= 1) return;
    uniform_int_distribution<int> dist(l, r - 1);
    int pivot = v[dist(rng)];

    int i = l, j = r - 1;
    while (i <= j) {
        while (v[i] < pivot) ++i;
        while (v[j] > pivot) --j;
        if (i <= j) {
            swap(v[i], v[j]);
            ++i;
            --j;
        }
    }
    if (l < j + 1) quick_sort_random(v, l, j + 1, rng);
    if (i < r) quick_sort_random(v, i, r, rng);
}

static int depth_limit_for(int n) {
    if (n <= 1) return 0;
    return 2 * (int) floor(log2((ld) n));
}

void super_quick_sort(vector<int> &v, int l, int r, int depth_limit, mt19937 &rng) {
    int n = r - l;
    if (n <= 1) return;
    if (n < INSERTION_CUTOFF) {
        insertion_sort(v, l, r);
        return;
    }
    if (depth_limit == 0) {
        heap_sort_range(v, l, r);
        return;
    }

    uniform_int_distribution<int> dist(l, r - 1);
    int pivot = v[dist(rng)];

    int i = l, j = r - 1;
    while (i <= j) {
        while (v[i] < pivot) ++i;
        while (v[j] > pivot) --j;
        if (i <= j) {
            swap(v[i], v[j]);
            ++i;
            --j;
        }
    }
    super_quick_sort(v, l, j + 1, depth_limit - 1, rng);
    super_quick_sort(v, i, r, depth_limit - 1, rng);
}

void run_sort(vector<int> &v) {
    mt19937 rng(19 * 19);
    int n = (int) v.size();
    super_quick_sort(v, 0, n, depth_limit_for(n), rng);
}

struct ArrayGenerator {
    vector<int> base_random;
    vector<int> base_reverse;
    vector<int> base_nearly;

    explicit ArrayGenerator(unsigned seed = 19) {
        generate_bases(seed);
    }

    void generate_bases(unsigned seed) {
        base_random.resize(MAX_N);
        mt19937 rng(seed);
        uniform_int_distribution<int> dist(MIN_RANDOM_VALUE, MAX_RANDOM_VALUE);
        for (int i = 0; i < MAX_N; ++i) base_random[i] = dist(rng);

        base_reverse.resize(MAX_N);
        for (int i = 0; i < MAX_N; ++i) base_reverse[i] = MAX_N - i;

        base_nearly.resize(MAX_N);
        iota(base_nearly.begin(), base_nearly.end(), 0);
        for (int k = 0; k < MAX_N / 100; ++k) {
            int i = rng() % MAX_N;
            int j = rng() % MAX_N;
            swap(base_nearly[i], base_nearly[j]);
        }
    }

    static vector<int> take_prefix(const vector<int> &src, int n) {
        return {src.begin(), src.begin() + n};
    }
};

struct SortTester {
    static long long avg_ms_quick(const vector<int> &base, int n) {
        long long sum = 0;
        for (int t = 0; t < REPEATS; ++t) {
            vector<int> a = ArrayGenerator::take_prefix(base, n);
            mt19937 rng(19 * 19 + t);
            auto start = chrono::high_resolution_clock::now();
            quick_sort_random(a, 0, (int) a.size(), rng);
            auto elapsed = chrono::high_resolution_clock::now() - start;
            sum += chrono::duration_cast<chrono::milliseconds>(elapsed).count();
        }
        return sum / REPEATS;
    }

    static long long avg_ms_best_sort(const vector<int> &base, int n) {
        long long sum = 0;
        for (int t = 0; t < REPEATS; ++t) {
            vector<int> a = ArrayGenerator::take_prefix(base, n);
            auto start = chrono::high_resolution_clock::now();
            run_sort(a);
            auto elapsed = chrono::high_resolution_clock::now() - start;
            sum += chrono::duration_cast<chrono::milliseconds>(elapsed).count();
        }
        return sum / REPEATS;
    }
};

signed main() {
#ifdef _LOCAL
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
#endif
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ArrayGenerator gen(19);
    SortTester tester;

    cout << "type, size, algo, threshold, avg_ms\n";

    auto run_group = [&](const string &type_name, const vector<int> &base_vec) {
        for (int n = MIN_N; n <= MAX_N; n += STEP) {
            long long ms_quick = tester.avg_ms_quick(base_vec, n);
            cout << type_name << ", " << n << ", quick, " << -1 << ", " << ms_quick << "\n";

            long long ms_intro = tester.avg_ms_best_sort(base_vec, n);
            cout << type_name << ", " << n << ", best, " << INSERTION_CUTOFF << ", " << ms_intro << "\n";
        }
    };

    run_group("random", gen.base_random);
    run_group("reverse", gen.base_reverse);
    run_group("nearly", gen.base_nearly);

    return 0;
}
