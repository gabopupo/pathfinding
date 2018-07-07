// Gabriel R. S. Pupo - nUSP: 9896250
// THE FOLLOWING CODE USES A* SEARCH TO SOLVE 15-PUZZLES.

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <vector>
#include <list>
#include <ctime>

using namespace std;

struct coord {
	int i, j;
};

struct state {
	int puzzle[4][4];
	char move;
	int f, g, h;
	state *prev;
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

// Evaluate distance of all pieces to goal state using Manhattan distance
int distance_to_goal(int *puzzle) {
	int dist = 0, p_pos, g_pos, goal[16];
	coord p_cor, g_cor;
	memcpy(goal, (int *)goal_puzzle, 16*sizeof(int));

	for (int i = 1; i < 16; ++i) { // don't count the blank tile
		for (int j = 0; j < 16; ++j) {
			if (puzzle[j] == i)
				p_pos = j;
			if (goal[j] == i)
				g_pos = j;
		}

		p_cor.i = p_pos/4; p_cor.j = p_pos%4;
		g_cor.i = g_pos/4; g_cor.j = g_pos%4;

		dist += abs(p_cor.i-g_cor.i) + abs(p_cor.j-g_cor.j);
	}
	return dist;
}

inline bool cmp_min(state *st1, state *st2) {
	return st1->f > st2->f;	
}

list<state*> open; // open list
vector<state*> closed; // closed list
bool solved = false;

bool on_open(state *st) {
	auto first = open.cbegin();
	auto last = open.cend();

	while (first != last) {
		if (cmp_2d((*first)->puzzle, st->puzzle))
			return true;	
		++first;
	}
	return false;
}

int f_open(state *st) {
	auto first = open.cbegin();
	auto last = open.cend();

	while (first != last) {
		if (cmp_2d((*first)->puzzle, st->puzzle))
			return (*first)->f;	
		++first;
	}
	return -1;
}

bool on_closed(state *st) {
	auto first = closed.cbegin();
	auto last = closed.cend();

	while (first != last) {
		if (cmp_2d((*first)->puzzle, st->puzzle))
			return true;
		++first;
	}
	return false;
}

int f_closed(state *st) {
	auto first = closed.cbegin();
	auto last = closed.cend();

	while (first != last) {
		if (cmp_2d((*first)->puzzle, st->puzzle))
			return (*first)->f;
		++first;
	}
	return false;
}

void delete_all() {
	while (not open.empty()) delete open.back(), open.pop_back();
	while (not closed.empty()) delete closed.back(), closed.pop_back();
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

	init->f = init->g = 0;
	init->h = distance_to_goal((int *)init->puzzle);

	open.push_back(init);

	while (not open.empty() and not solved) {
		state* current = open.back();
		open.pop_back();
		vector<state*> next_moves = get_possible_moves(current);
		for (state *move : next_moves) {
			move->g = current->g + 1;
			move->h = distance_to_goal((int *)move->puzzle);
			move->f = move->g + move->h;
			move->prev = current;

			if (is_solved(move->puzzle)) { // move solves puzzle
				solved = true;
				last = move;
			} else {
				if ((on_open(move) and f_open(move) < move->f) or
				(on_closed(move) and f_closed(move) < move->f))
					delete move;
				else open.push_back(move);
			}
		}
		closed.push_back(current);
		open.sort(cmp_min);
	}
	
	closed.push_back(last);

	string path;
	while (last->prev != NULL) {
		path += last->move;
		last = last->prev;
	}

	reverse(path.begin(), path.end());
	cout << path << endl;
	
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
