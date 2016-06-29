// Laufzeit: O(n^2*2^n)
vector<vector<int>> dist; // Entfernung zwischen je zwei Punkten.
vector<int> TSP() {
	int n = dist.size(), m = 1 << n;
	vector<vector<ii>> dp(n, vector<ii>(m, ii(INF, -1)));
	
	for(int c = 0; c < n; c++) dp[c][m-1].first = dist[c][0], dp[c][m-1].second = 0;

	for(int v = m - 2; v >= 0; v--) {
		for(int c = n - 1; c >= 0; c--) {
			for(int g = 0; g < n; g++) { 
				if(g != c && !((1 << g) & v)) {
					if((dp[g][(v | (1 << g))].first + dist[c][g]) < dp[c][v].first) {
						dp[c][v].first = dp[g][(v | (1 << g))].first + dist[c][g];
						dp[c][v].second = g;
	}}}}}
	
	vector<int> res; res.push_back(0); int v = 0;
	while(res.back() != 0 || res.size() == 1) {
		res.push_back(dp[res.back()][(v |= (1 << res.back()))].second);
	}
	return res; // Enthält Knoten 0 zweimal. An erster und letzter Position.
}
