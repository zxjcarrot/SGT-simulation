#include <vector>
#include <iostream>
#include <queue>
#include <stack>
#include <bitset>

#include "Zipf.h"
#include "Random.h"

const int N = 1000; // # transactions in the batch

const long long INFLL = std::numeric_limits<long long>::max();

std::vector<std::pair<int, int>> adj[N];
std::vector<std::pair<int, int>> adj_in[N];


int ind, cnt, num[N], low[N], scc[N];
int scc_size[N];
int scc_node_to_kill[N];
std::stack<int> stack;
std::bitset<N> on_stack;
std::bitset<N> nodes_to_consider;
//std::vector<int> order;
bool has_node[N];
bool visited[N];

int deg[N];
int incoming_raw[N];
bool aborted[N];
bool committed[N];

int aborted_nodes = 0;
int cascading_aborts = 0;
bool visited_cascading_aborts[N];

const int WAR = 0;
const int WAW = 1;
const int RAW = 2;

int find_all_raw_reachable(int u) {
	if (visited_cascading_aborts[u])
		return 0;
	int cnt = 0;
	visited_cascading_aborts[u] = true;
	for (auto [v, w] : adj_in[u]) {
		if (aborted[v] || w != RAW)
			continue;
		//abort_raw_reachable(v);
	}
	visited_cascading_aborts[u] = false;
}

void abort_raw_reachable(int u) {
	if (visited_cascading_aborts[u])
		return;
	if (aborted[u] == false) {
		aborted[u] = true;
		aborted_nodes++;
		cascading_aborts++;
	}
	visited_cascading_aborts[u] = true;
	for (auto [v, w] : adj_in[u]) {
		if (aborted[v] || w != RAW)
			continue;
		abort_raw_reachable(v);
	}
	visited_cascading_aborts[u] = false;
}

void tarjan(int u, int num[N], int low[N], int scc[N], int scc_size[N], std::stack<int> & stack, std::bitset<N> & on_stack)
{
	assert(committed[u] == false);
	visited[u] = true;
	num[u] = low[u] = ++ind;
	stack.push(u);
	on_stack.set(u);
	for (auto [v, w] : adj[u]) {
		if (aborted[v] || committed[v])
			continue;
		if (num[v] == 0) {
			tarjan(v, num, low, scc, scc_size, stack, on_stack);
			low[u] = std::min(low[u], low[v]);
		}
		else if (on_stack.test(v)) {
			low[u] = std::min(low[u], num[v]);
		}
	}

	if (num[u] == low[u])
	{
		++cnt;
		int v;
		do
		{
			v = stack.top();
			stack.pop();
			on_stack.reset(v);
			scc[v] = u;	
            scc_size[u]++;
			assert(committed[v] == false);
		} while (v != u);
	}
}

int main()
{
	Random r;
	int u, v, w;
	while (std::cin >> u) {
		if (u == -1) {
			break;
		}
		std::cin >> v >> w;
		adj[u].emplace_back(v, w);
		adj_in[v].emplace_back(u, w);
		deg[u]++;
		deg[v]++;
		if (w == RAW) {
			incoming_raw[v]++;
		}
		has_node[u] = has_node[v] = true;
	}

	int aborted_already = 0;
	std::cin >> aborted_already;
	for (int i = 0; i < aborted_already; ++i) {
		int aborted_u = 0;
		std::cin >> aborted_u;
		aborted[aborted_u] = true;
		aborted_nodes++;
		//std::cout << "Abort node "<< aborted_u << " with degree " << deg[aborted_u] << std::endl;
		for (auto [v, w] : adj[aborted_u])
			deg[v]--; // decrement the in-degree of outgoing node
		for (auto [u, w] : adj_in[aborted_u]) {
			if (w == RAW) {
				incoming_raw[aborted_u]--;
			}
			deg[u]--; // decrement the out-degree of incoming node
		}
			
		abort_raw_reachable(aborted_u);
	}

	std::cout << "aborted " << aborted_nodes << std::endl; 
    int vertices = 0;
	for (int i = 0; i < N; ++i) {
		if (has_node[i])
			vertices++;
	}


	int scc_more_than_1 = 0;
	int iterations_allowed = 100;
	int iters = 0;
	int total_sccs_now = 0;
	auto begin_1 = std::chrono::steady_clock::now();
	do {
		ind = 0;
		cnt = 0;
		on_stack.reset();
		assert(stack.empty());
		for (int i = 0; i < N; ++i) {
			if (aborted[i] == false && committed[i] == false && has_node[i] && visited[i] == false)
				tarjan(i, num, low, scc, scc_size, stack, on_stack);
		}
		int size_1 = 0;
		scc_more_than_1 = 0;
		for (int i = 0; i < N; ++i) {
			if (has_node[i] == false || aborted[i] || committed[i]) {
				continue;
			}
			// if (aborted[i]) {
			// 	continue;
			// }

			if (scc_size[i] > 1) {
				scc_more_than_1++;
			} else if (scc_size[i] == 1) {
				assert(scc[i] == i);
				committed[i] = true;
			}
		}
		size_1 = cnt - scc_more_than_1;
		total_sccs_now += size_1;
		++iters;
		std::cout << "Number of SCCs found at iter("<< (iters) << "): " << cnt << ", # size>1 SSCs this round: " << scc_more_than_1 << ", # size-1 SSCs this round: " << size_1 << ", # vertices: " << vertices << ", Total # size-1 SCCs now: " << total_sccs_now << ", aborted nodes " << aborted_nodes << std::endl;

		if (scc_more_than_1 > 0) {
			memset(scc_node_to_kill, -1, sizeof(scc_node_to_kill));
			memset(low, 0, sizeof(low));
			memset(num, 0, sizeof(num));
			for (int i = 0; i < N; ++i) {
				if (has_node[i] == false || scc_size[i] == 1 || aborted[i] || committed[i]) {
					continue;
				}
				auto u = scc[i]; // Head of the SCC
				if (scc_node_to_kill[u] == -1) { // Find a node in a SCC that has the most number of edges
					scc_node_to_kill[u] = u;
				}
				if (deg[scc_node_to_kill[u]] < deg[i]) {
					scc_node_to_kill[u] = i;
				}
			}
			for (int i = 0; i < N; ++i) {
				if (has_node[i] == false || scc_size[i] == 1 || aborted[i] || committed[i]) {
					continue;
				}
				scc_size[i] = 0;
				visited[i] = false;
				auto u = scc[i];
				//auto aborted_u = u;
				auto aborted_u = scc_node_to_kill[u];
				if (aborted[aborted_u]) {
					if (r.next() % 100 < 10 && aborted[i] == false) {
						u = i;
						aborted_u = i;
					} else {
						continue;
					}
				}

				aborted[aborted_u] = true;
				aborted_nodes++;
				//std::cout << "Abort node "<< aborted_u << " with degree " << deg[aborted_u] << std::endl;
				for (auto [v, w] : adj[aborted_u])
					deg[v]--; // decrement the in-degree of outgoing node
				for (auto [u, w] : adj_in[aborted_u]) {
					if (w == RAW) {
						incoming_raw[aborted_u]--;
					}
					deg[u]--; // decrement the out-degree of incoming node
				}
					
				abort_raw_reachable(aborted_u);
			}
		}
		
	} while(scc_more_than_1 > 0 && iters < iterations_allowed);
	auto end_1 = std::chrono::steady_clock::now();
	int committed_nodes = 0;
	for (int i = 0; i < N; ++i) {
		if (has_node[i] && committed[i] && aborted[i] == false) {
			committed_nodes++;
		}
	}
	std::cout << "Tarjan: " << std::chrono::duration_cast<std::chrono::microseconds>(end_1 - begin_1).count() 
			  << "us, committed " << committed_nodes << " aborted " << aborted_nodes  << " cascading_aborts " << cascading_aborts
			  << " abort rate " << aborted_nodes / (N + 0.0)  << std::endl;	

	return 0;
}