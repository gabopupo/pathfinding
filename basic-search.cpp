// Gabriel R. S. Pupo - nUSP: 9896250
// THE FOLLOWING CODE USES BREADTH-FIRST SEARCH TO SOLVE 15-PUZZLES.

#include <iostream>
#include <algorithm>
#include <queue>
#include <vector>
#include <cstdio>
#include <cstring>
#include <ctime>

using namespace std;

struct coord {
	int i, j;
};

struct state {
	int puzzle[4][4];
	state *prev;
	char move;
};

int count_inversions(int puzzle[]) {
	int counter = 0;
	for (int i = 0; i < 15; i++)
		for (int j = i+1; j <= 15; j++)
	        	if (puzzle[j] && puzzle[i] && puzzle[i] > puzzle[j]) counter++;
        counter++;
	return counter;
}

bool is_solvable(int puzzle[4][4], coord zero) {
    	int inv = count_inversions((int *) puzzle);
	return (zero.i % 2 == 0) ? inv % 2 == 0 : inv % 2 != 0;
}

int goal_puzzle[4][4] = { {1, 2, 3, 4},
	          	  {5, 6, 7, 8},
		  	  {9, 10, 11, 12},
		   	  {13, 14, 15, 0}
};

int null_puzzle[4][4] = { {0, 0, 0, 0},
			  {0, 0, 0, 0},
			  {0, 0, 0, 0},
			  {0, 0, 0, 0}
};

void swap(int puzzle[4][4], coord c1, coord c2) {
	int aux = puzzle[c1.i][c1.j];
	puzzle[c1.i][c1.j] = puzzle[c2.i][c2.j];
	puzzle[c2.i][c2.j] = aux;
}

bool cmp_2d(int a[4][4], int b[4][4]) {
	int counter = 0;
	for (int i = 0; i < 4; ++i) 
		for (int j = 0; j < 4; ++j) 
			if (a[i][j] == b[i][j]) counter++;
	if (counter == 16)
		return true;
	return false;
}

inline bool is_solved(int candidate[4][4]) {
	return (cmp_2d(goal_puzzle, candidate));
}

coord find_blank(int puzzle[4][4]) {
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (puzzle[i][j] == 0)
				return {i,j};
	return {-1,-1};
}

vector<state*> get_possible_moves(state *cur) {
	coord blank = find_blank(cur->puzzle);
	vector<state*> possible;

	// UP
	if (blank.i-1 >= 0) {
		state *up = new state;
		memcpy(up->puzzle, cur->puzzle, 16*sizeof(int));
		swap(up->puzzle, blank, { blank.i-1, blank.j });
		up->move = 'U';
		possible.push_back(up);
	}

	// DOWN
	if (blank.i+1 < 4) {
		state *down = new state;
		memcpy(down->puzzle, cur->puzzle, 16*sizeof(int));
		swap(down->puzzle, blank, { blank.i+1, blank.j });
		down->move = 'D';
		possible.push_back(down);
	}

	// LEFT
	if (blank.j-1 >= 0) {
		state *left = new state;
		memcpy(left->puzzle, cur->puzzle, 16*sizeof(int));
		swap(left->puzzle, blank, { blank.i, blank.j-1 });
		left->move = 'L';
		possible.push_back(left);
	}

	// RIGHT
	if (blank.j+1 < 4) {
		state *right = new state;
		memcpy(right->puzzle, cur->puzzle, 16*sizeof(int));
		swap(right->puzzle, blank, { blank.i, blank.j+1 });
		right->move = 'R';
		possible.push_back(right);
	}

	return possible;
}

bool was_visited(state *st, vector<state*> all) {
	for (state *s : all)
		if (cmp_2d(st->puzzle, s->puzzle)) return true;
	return false;
}

bool solved = false;
vector<state*> all_states;
queue<state*> solver;

void delete_all() {
	while (not solver.empty()) solver.pop();
	while (not all_states.empty()) delete all_states.back(), all_states.pop_back();
}

void solve(int puzzle[4][4]) {
	state *init = new state, *last;
	memcpy(init->puzzle, puzzle, 16*sizeof(int));
	init->prev = NULL;
	init->move = 'o';

	if (is_solved(init->puzzle)) {
		delete init;
		cout << "\n";
		return;
	}

	solver.push(init);
	all_states.push_back(init);

	do {
		state *current = solver.front();
		vector<state*> next_moves = get_possible_moves(current);
		for (state *move : next_moves) {
			if (is_solved(move->puzzle)) {
				move->prev = current;
				last = move;
				solved = true;
				all_states.push_back(move);
				break;
			} else if (not was_visited(move, all_states)) {
				move->prev = current;
				solver.push(move);
				all_states.push_back(move);
			} else delete move;
		}
		solver.pop(); // current state is pop'd
	} while (not solver.empty() and not solved);

	string path;
	while (last->prev != NULL) {
		path += last->move;
		last = last->prev;
	}

	reverse(path.begin(), path.end());
	cout << path << "\n";

	delete_all();
}

int main(void) {
	int sets, puzzle[4][4] = {{0}};
	double start_time, end_time;
	coord zero;

	cin >> sets;
	for (int s = 0; s < sets; ++s) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				scanf("%d", &puzzle[i][j]);
				if (puzzle[i][j] == 0) {
					zero.i = i;
					zero.j = j;
				}
			}
		}
		if (not is_solvable(puzzle, zero)) cout << "This puzzle is not solvable.\n";
		else {
			start_time = clock();
			solve(puzzle);
			end_time = clock();

			cout << "\nExecution time for this set: " << (end_time-start_time)/CLOCKS_PER_SEC << " s\n\n";

			memcpy(puzzle, null_puzzle, 16*sizeof(int));
			solved = false;
		}
	}
	return 0;
}
