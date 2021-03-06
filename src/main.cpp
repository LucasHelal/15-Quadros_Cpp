#include <iostream>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <vector>
#include "headers/a_node.h"
#include "headers/heuristics.h"
//OPEN-SET
a_vector open_set;
//OPEN-SET REF
std::unordered_map<std::string, a_node> open_ref;
//OPEN-SET-GARBAGE
std::unordered_map<int, a_node*> garbage;
//CLOSED-SET
std::unordered_map<std::string, a_node> closed_set;

float calc_h(a_node n) {
		return (out_of_order_heuristic(n.matrix)); //+
//		return	out_of_sequence_heuristic(n.matrix); // +
//		return	rect_distance_heuristic(n.matrix);
}

std::string generate_key(int mat[][4]) {
	std::string key = "";
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			if(mat[i][j] < 10) key += "0";
			key += mat[i][j];
		}
	}
	return key;
}

void initialize_sets(a_node _node) {
	_node.parent = "root";
	_node.node_number = 1;
	_node.g = 0;
	_node.h = calc_h(_node);
	_node.f = _node.g + _node.h;
	open_ref.clear();
	open_ref[generate_key(_node.matrix)] = _node;
	open_set.initialize_vec();
	open_set.push_node(&(open_ref[generate_key(_node.matrix)]));
	closed_set.clear();
	garbage.clear();
}

void print(int mat[][4]) {
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			if(mat[i][j] > 9) std::cout << mat[i][j] << "  ";
			else std::cout << mat[i][j] << "   ";
		}
		std::cout << std::endl;
	}	
}

bool node_equal(a_node n1, a_node n2) {
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			if(n1.matrix[i][j] != n2.matrix[i][j]) return false;
		}
	}
	return true;
}

void swap_block(a_node &n, int lin, int col, int lin_t, int col_t) {
	int aux;
	aux = n.matrix[lin][col];
	n.matrix[lin][col] = n.matrix[lin_t][col_t];
	n.matrix[lin_t][col_t] = aux;
}

void create_nodes(std::vector<a_node> &new_nodes, a_node n) {
	a_node new_node = n;
	int i, j;
	for(i=0; i<4; i++) {
		for(j=0; j<4; j++) {
			if(n.matrix[i][j] == 0) break;
		}
		if(n.matrix[i][j] == 0 && j < 4) break;
	}


	if(i != 0) {
		swap_block(new_node, i, j, i-1, j);
		new_nodes.push_back(new_node);
		new_node = n;
	}
	if(i != 3) {
		swap_block(new_node, i, j, i+1, j);
		new_nodes.push_back(new_node);
		new_node = n;
	}
	if(j != 0) {
		swap_block(new_node, i, j, i, j-1);
		new_nodes.push_back(new_node);
		new_node = n;
	}
	if(j != 3) {
		swap_block(new_node, i, j, i, j+1);
		new_nodes.push_back(new_node);
	}
}

a_node *a_star(a_node start, a_node end, int &node_number) {
	a_node *closest_node = new a_node();
	a_node *former_node, *aux;
	float g_try_score;
	std::vector<a_node> new_nodes;
	std::string i_key;

	//iterators
	a_vector::iterator it_vec;
	std::unordered_map<int, a_node*>::iterator it_garb;
	std::unordered_map<std::string, a_node>::iterator it;
	initialize_sets(start);
	while(!open_set.empty()) {
		closest_node = open_set.front();
		open_set.pop_node();
		if((it_garb = garbage.find(closest_node->node_number)) != garbage.end()) {
			if(it_garb->second->ativo) it_garb->second->ativo = false;
			else {
				garbage.erase(closest_node->node_number);
				continue;
			}
		}

		closed_set[generate_key(closest_node->matrix)] = *closest_node;

		//		std::cout << "open: " << open_set.size() << std::endl;

		if(node_equal(*closest_node, end)) return closest_node;

		//		std::cout << "closed: " << closed_set.size() << std::endl;
		std::cout << "number: " << node_number << std::endl;

		create_nodes(new_nodes, *closest_node);
		for(std::vector<a_node>::iterator i=new_nodes.begin(); i!=new_nodes.end(); i++) {
			g_try_score = closest_node->g + 1;
			i_key = generate_key(i->matrix);
			//if(isInsideClosed(closed_set.begin(), closed_set.end(), (*i)) != NULL) continue;
			if(closed_set.find(i_key) != closed_set.end()) continue;
			aux = &(*i);
			//if((former_node = isInsideOpen(open_set.begin(), open_set.end(), it_vec, (*i))) != NULL) {
			if((it = open_ref.find(i_key)) != open_ref.end()) {
				former_node = &(it->second);
				if(former_node->g <= g_try_score) continue;
				//				std::cout << "testeeee " << std::endl;
				aux = former_node;
				garbage[former_node->node_number] = former_node;
			}
			aux->g = g_try_score;
			aux->h = calc_h(*i);
			aux->f = i->g + i->h;
			aux->parent = generate_key(closest_node->matrix);
			if(aux == &(*i)) {
				node_number++;
				aux->node_number = node_number;
				open_ref[i_key] = *i;
			}
			open_set.push_node(&(open_ref[i_key]));
		}
		//		std::cout << "new nodes: " << new_nodes.size() << std::endl << std::endl;
		new_nodes.clear();
		}
	}

	void backtrace(a_node n) {
		a_node last;
		last.parent = generate_key(n.matrix);
		int mov = -1;
		do {
			last = closed_set[last.parent];
			std::cout << std::endl;
			print(last.matrix);
			std::cout << std::endl;
			mov++;
		} while(last.parent != "root");
		std::cout << mov << " movements!" << std::endl;
	}

	void read_mat(int mat[][4]) {
		for(int i=0; i<4; i++)
			scanf("%d %d %d %d", &mat[i][0], &mat[i][1],
					&mat[i][2], &mat[i][3]);
	}

	int main() {
		a_node s, e;
		int n=1;
		for(int i=0; i<4; i++) {
			for(int j=0; j<4; j++) {
				e.matrix[i][j] = n;
				n++;
			}
		}
		e.matrix[3][3] = 0;
		/*
		   int m[16] = {2,6,8,3,1,14,9,11,7,12,13,0,5,15,4,10};
		   for (int o=0; o<16; o++) {
		   s.matrix[o/4][o%4] = m[o];
		   }
		 */
		read_mat(s.matrix);
		print(s.matrix);
		int nodes = 1;
		backtrace(*(a_star(s, e, nodes)));
		std::cout << "nodes_num: " << nodes << std::endl;
	}

