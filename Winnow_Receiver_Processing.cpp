#include <iostream>
#include <vector>
#include <fstream>
#include "Winnow_Receiver.h"
#include "Winnow_Transmitter.h"
#include "message_Struct.cpp"
using namespace std;

message_Struct Winnow_Receiver_Processing(message_Struct message) {

	// Syndrome Matrix
	int H3_Syndrome_Cal_Matrix[3][7] = {
										{1, 0, 1, 0, 1, 0, 1},
										{0, 1, 1, 0, 0, 1, 1},
										{0, 0, 0, 1, 1, 1, 1}
	};

	message_Struct message1;

	int i, j, k, l, m, loop2;
	int inital_block_parity = 0;
	int block_reverse_iter = 6;
	int val = 0;
	int Trans_Synd_2;
	int Trans_Synd_1;
	int Trans_Synd_0;
	int Recv_Synd_2;
	int Recv_Synd_1;
	int Recv_Synd_0;
	int Res_Synd_2;
	int Res_Synd_1;
	int Res_Synd_0;

	message_Struct Receiver_Received_Message = message;
	message_Struct Receiver_Send_Message;

	string block = "";
	string synd;
	string resultant_Synd;
	string Trans_Synd;
	string Recv_Synd;

	if (Receiver_Received_Message.Pass == 1) {

		cout << endl;
		cout << "---------------------------------------------------------------------------------- " << endl;
		cout << "----------------------------------Receiver Processing 1-------------------------- " << endl;
		cout << "---------------------------------------------------------------------------------- " << endl;
		cout << endl;

		for (j = 0; j < corrected_Receiver.size() / 7; j++) {

			for (k = 0; k < 7; k++) {
				inital_block_parity += corrected_Receiver[(7 * j) + k];
			}
			inital_block_parity = inital_block_parity % 2;

			if (inital_block_parity != Receiver_Received_Message.block_num_Parity_Vect[j]) {
				Receiver_Send_Message.block_num_Vect.push_back(j);
				Receiver_Send_Message.block_num_Parity_Vect.push_back(1);
			}
			inital_block_parity = 0;
		}
	}

	else if (Receiver_Received_Message.Pass == 2) {

		cout << endl;
		cout << "---------------------------------------------------------------------------------- " << endl;
		cout << "----------------------------------Receiver Processing 2-------------------------- " << endl;
		cout << "---------------------------------------------------------------------------------- " << endl;
		cout << endl;

		// Calculating Syndromes
		for (loop2 = 0; loop2 < Receiver_Received_Message.block_num_Vect.size(); loop2++) {

			for (k = 0; k < 7; k++) {
				block.push_back(corrected_Receiver[(7 * Receiver_Received_Message.block_num_Vect[loop2]) + k]);
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
			Receiver_Send_Message.block_num_Syndrome_String_Vect.push_back(synd);

			//for (k = 0; k < Receiver_Send_Message.block_num_Syndrome_String_Vect.size(); k++) {
			//	cout << Receiver_Send_Message.block_num_Syndrome_String_Vect[k] << endl;
			//}

			Trans_Synd = Receiver_Received_Message.block_num_Syndrome_String_Vect[loop2];
			Recv_Synd = Receiver_Send_Message.block_num_Syndrome_String_Vect[loop2];

			Trans_Synd_2 = Trans_Synd[2] - '0';
			Trans_Synd_1 = Trans_Synd[1] - '0';
			Trans_Synd_0 = Trans_Synd[0] - '0';
			Recv_Synd_2 = Recv_Synd[2] - '0';
			Recv_Synd_1 = Recv_Synd[1] - '0';
			Recv_Synd_0 = Recv_Synd[0] - '0';

			Res_Synd_2 = (Trans_Synd_2 + Recv_Synd_2) % 2;
			Res_Synd_1 = (Trans_Synd_1 + Recv_Synd_1) % 2;
			Res_Synd_0 = (Trans_Synd_0 + Recv_Synd_0) % 2;

			resultant_Synd += to_string(Res_Synd_0);
			resultant_Synd += to_string(Res_Synd_1);
			resultant_Synd += to_string(Res_Synd_2);

			//cout << "Resultant " << resultant_Synd << " " << endl;

			if (resultant_Synd == "000") {
				continue;
			}

			else if (resultant_Synd == "001") {
				corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 6] = corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 6] ^ 1;
			}

			else if (resultant_Synd == "010") {
				corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 5] = corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 5] ^ 1;
			}

			else if (resultant_Synd == "011") {
				corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 4] = corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 4] ^ 1;
			}

			else if (resultant_Synd == "100") {
				corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 3] = corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 3] ^ 1;
			}

			else if (resultant_Synd == "101") {
				corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 2] = corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 2] ^ 1;
			}

			else if (resultant_Synd == "110") {
				corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 1] = corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 1] ^ 1;
			}

			else if (resultant_Synd == "111") {
				corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 0] = corrected_Receiver[(Receiver_Received_Message.block_num_Vect[loop2] * 7) + 0] ^ 1;
			}

			//Discarding
			int discard_Indices[] = { 3, 5, 6 };

			for (int i_disc = 0; i_disc < 3; i_disc++) {
				int index = (Receiver_Received_Message.block_num_Vect[loop2] * 7) + discard_Indices[i_disc];
				corrected_Receiver[index] = 8;
			}
			// discard end

			//Discarding
			int discard_Indices2[] = { 3, 5, 6 };

			for (int i_disc = 0; i_disc < 3; i_disc++) {
				int index = (Receiver_Received_Message.block_num_Vect[loop2] * 7) + discard_Indices2[i_disc];
				sifted_Transmitter[index] = 8;
			}

			// discard end

			resultant_Synd.clear();
			Trans_Synd.clear();
			Recv_Synd.clear();
			block.clear();
			synd.clear();

		}

		//discard part 2

		vector<int> temp_Vec;
		for (int s = 0; s < corrected_Receiver.size(); s++) {
			if (corrected_Receiver[s] != 8) temp_Vec.push_back(corrected_Receiver[s]);
			else continue;
		}
		corrected_Receiver = temp_Vec;
		//discard part2 end

		//discard part 2

		vector<int> temp_Vec2;
		for (int s = 0; s < sifted_Transmitter.size(); s++) {
			if (sifted_Transmitter[s] != 8) temp_Vec2.push_back(sifted_Transmitter[s]);
			else continue;
		}

		sifted_Transmitter = temp_Vec2;

		//discard part2 end

	}

	//return 
	return Receiver_Send_Message;
}