#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <set>
#include <algorithm>
#include "Winnow_Transmitter_Processing.h"
#include "Winnow_Transmitter.h"
#include "Winnow_Receiver_Processing.h"
#include "message_Struct.cpp"
#include "Error_Check.h"
#include "Permute.h"
#include "Toeplitz.h"
#include "Dodis.h"
#include "Circulant.h"
using namespace std;

void Winnow_Transmitter_Processing() {

	// Syndrome Matrix
	int H3_Syndrome_Cal_Matrix[3][7] = {
										{1, 0, 1, 0, 1, 0, 1},
										{0, 1, 1, 0, 0, 1, 1},
										{0, 0, 0, 1, 1, 1, 1}
	};

	int i, j, k, loop2, round = 20;
	int inital_block_parity = 0;
	int val = 0;
	int block_reverse_iter = 6;
	int Channel_Uses = 0;
	double Err_Val;

	string synd;

	message_Struct Transmitter_Received_Message;
	message_Struct Transmitter_Send_Message;

	vector<int> block;
	vector<int> T_RA;
	vector<int> R_RA;

	for (i = 0; i < round; i++) {

		// initializing for new round
		Transmitter_Received_Message.block_num_Vect.clear();
		Transmitter_Received_Message.block_num_Parity_Vect.clear();
		Err_Val = 0;
		T_RA.clear();
		R_RA.clear();

		// Winnow Start
		Transmitter_Send_Message.Pass = 1;

		cout << endl;
		cout << "---------------------------------------------------------------------------------- " << endl;
		cout << "----------------------------------Transmitter Processing 1-------------------------- " << endl;
		cout << "---------------------------------------------------------------------------------- " << endl;
		cout << endl;

		for (j = 0; j < sifted_Transmitter.size() / 7; j++) {

			for (k = 0; k < 7; k++) {
				inital_block_parity += sifted_Transmitter[(7 * j) + k];
			}
			inital_block_parity = inital_block_parity % 2;
			Transmitter_Send_Message.block_num_Vect.push_back(j);
			Transmitter_Send_Message.block_num_Parity_Vect.push_back(inital_block_parity);
			inital_block_parity = 0;
		}
		Transmitter_Received_Message = Winnow_Receiver_Processing(Transmitter_Send_Message);
		Transmitter_Send_Message.block_num_Vect.clear();
		Transmitter_Send_Message.block_num_Parity_Vect.clear();
		Transmitter_Send_Message.block_num_Syndrome_String_Vect.clear();

		cout << endl;
		cout << "---------------------------------------------------------------------------------- " << endl;
		cout << "----------------------------------Transmitter Processing 2-------------------------- " << endl;
		cout << "---------------------------------------------------------------------------------- " << endl;
		cout << endl;

		Transmitter_Send_Message.Pass = 2;

		// Calculating Syndromes
		for (loop2 = 0; loop2 < Transmitter_Received_Message.block_num_Vect.size(); loop2++) {

			for (k = 0; k < 7; k++) {
				block.push_back(sifted_Transmitter[(7 * Transmitter_Received_Message.block_num_Vect[loop2]) + k]);
			}
			for (int i_row = 0; i_row < 3; i_row++) {
				for (int j_col = 0; j_col < 7; j_col++) {
					val += H3_Syndrome_Cal_Matrix[i_row][j_col] * block[block_reverse_iter];
					block_reverse_iter = block_reverse_iter - 1;
				}
				val = val % 2;
				synd += to_string(val);
				val = 0;
				block_reverse_iter = 6;
			}
			reverse(synd.begin(), synd.end());
			Transmitter_Send_Message.block_num_Syndrome_String_Vect.push_back(synd);
			block.clear();
			synd.clear();
		}

		Transmitter_Send_Message.block_num_Vect = Transmitter_Received_Message.block_num_Vect;
		Winnow_Receiver_Processing(Transmitter_Send_Message);

		Channel_Uses += 3;

		// Winnow Complete

		// adapting and checking

		ofstream T_RA_Public("T_EC_Bits.txt");
		if (T_RA_Public.is_open()) {
			for (int number : sifted_Transmitter) {
				T_RA_Public << number;
			}
			T_RA_Public.close();
		}
		else {
			cerr << "Unable to open file." << endl;
		}

		ofstream R_RA_Public("R_EC_Bits.txt");
		if (R_RA_Public.is_open()) {
			for (int number : corrected_Receiver) {
				R_RA_Public << number;
			}
			R_RA_Public.close();
		}
		else {
			cerr << "Unable to open file." << endl;
		}
		
		// Adaptable Randomness Extraction
		cout << "Adaptable Randomness Extraction ...." << endl;
		Toeplitz();

		ifstream file1("T_RA_Public_Check_A.txt");
		if (file1.is_open()) {
			char bit;
			while (file1 >> bit) {
				T_RA.push_back(bit - '0');
			}
			file1.close();
		}

		ifstream file2("R_RA_Public_Check_A.txt");
		if (file2.is_open()) {
			char bit;
			while (file2 >> bit) {
				R_RA.push_back(bit - '0');
			}
			file2.close();
		}

		Err_Val = Error_Check(sifted_Transmitter, corrected_Receiver);
		cout << "Err val SIFT : " << Err_Val << "%" << endl;
		Err_Val = Error_Check(T_RA, R_RA);
		cout << "Err val GOSU : " << Err_Val << "%" << endl;

		sifted_Transmitter.clear();
		corrected_Receiver.clear();

		ifstream file3("T_EC_Bits.txt");
		if (file3.is_open()) {
			char bit;
			while (file3 >> bit) {
				sifted_Transmitter.push_back(bit - '0');
			}
			file3.close();
		}

		ifstream file4("R_EC_Bits.txt");
		if (file4.is_open()) {
			char bit;
			while (file4 >> bit) {
				corrected_Receiver.push_back(bit - '0');
			}
			file4.close();
		}
		Channel_Uses += 2;

		// Private Randomness Extraction for Enhanced Security
		if (Err_Val == 0) {
			Channel_Uses += 1;
			Toeplitz();
			ifstream inputFile("Seed_Size.txt");
			int n2_T;
			inputFile >> n2_T;
			cout << "INITIAL Key SIZE ----------------------------------- : " << sifted_Receiver.size() << endl;
			cout << "Final Error Corrected SIZE ----------------------------------- : " << corrected_Receiver.size() << endl;
			cout << "Channel Uses --------------------------------------------- : " << Channel_Uses << endl;
			cout << "Final Seed Size ----------------------------------- : " << n2_T << endl;
			cout << "Final (Randomness Extracted) Length ----------------------------------- : R :: " << R_RA.size() << "|| T:: " << T_RA.size() << endl;
			return;
		}

		Permute();
		Channel_Uses += 1;
	}

	// return 
}
