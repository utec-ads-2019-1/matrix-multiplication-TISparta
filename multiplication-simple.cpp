#include <bits/stdc++.h>

using namespace std;

int main () {
  ios::sync_with_stdio(false);
  cin.tie(0);
  int n, m, p, q;
  cin >> n >> m;
  vector <vector <int>> a(n, vector <int> (m, 0));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      cin >> a[i][j];
    }
  }
  cin >> p >> q;
  vector <vector <int>> b(p, vector <int> (q, 0));
  for (int i = 0; i < p; i++) {
    for (int j = 0; j < q; j++) {
      cin >> b[i][j];
    }
  }
  vector <vector <int>> c(n, vector <int> (q, 0));
  assert(m == p);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < q; j++) {
      for (int k = 0; k < m; k++) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
  cout << n << ' ' << q << '\n';
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < q; j++) {
      cout << c[i][j] << ' ';
    }
    cout << '\n';
  }
  return (0);
}
