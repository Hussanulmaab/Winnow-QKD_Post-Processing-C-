#pragma once
#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct message_Struct {
	double Iter;
	double Pass;
	vector<int> block_num_Vect;
	vector<int> block_num_Parity_Vect;
	vector<string> block_num_Syndrome_String_Vect;
};