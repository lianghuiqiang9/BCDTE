#ifndef __PDTE__
#define __PDTE__

#include "utils.h"
#include <unistd.h>
#include<seal/seal.h>
using namespace seal;
using namespace std;


vector<vector<Ciphertext>> rdcmp_pdte_clear_line_relation(BatchEncoder *batch_encoder, Evaluator *evaluator, vector<vector<Ciphertext>> out,int leaf_num,int data_m,uint64_t slot_count);
vector<vector<Ciphertext>> tecmp_cdcmp_pdte_clear_line_relation(BatchEncoder *batch_encoder, Evaluator *evaluator, vector<vector<Ciphertext>> out,int leaf_num,int data_m,uint64_t slot_count, uint64_t row_count, uint64_t num_cmps_per_row, uint64_t num_slots_per_element);

Ciphertext map_zero_to_one_and_the_other_to_zero(Ciphertext ct_in,BatchEncoder *batch_encoder, Evaluator *evaluator, RelinKeys *rlk_server,  uint64_t tree_depth,Plaintext d_factorial_inv_with_sign_pt, uint64_t slot_count);
Ciphertext private_info_retrieval(Evaluator *evaluator, vector<Ciphertext> in_first, vector<Plaintext> in_second);
Ciphertext private_info_retrieval_with_b_b_b(Evaluator *evaluator, vector<Ciphertext> in_first, vector<Plaintext> in_second, vector<uint64_t> in_second_vec, Ciphertext zero_zero_zero);
Ciphertext private_info_retrieval(Evaluator *evaluator,RelinKeys *rlk_server, vector<Ciphertext> in_first, vector<Ciphertext> in_second);
vector<Ciphertext> private_info_retrieval(Evaluator *evaluator, vector<Ciphertext> in_first, vector<vector<Plaintext>> in_second);



#endif