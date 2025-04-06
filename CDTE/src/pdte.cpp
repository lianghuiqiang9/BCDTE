#include "pdte.h"



vector<vector<Ciphertext>> rdcmp_pdte_clear_line_relation(BatchEncoder *batch_encoder, Evaluator *evaluator, vector<vector<Ciphertext>> out,int leaf_num,int data_m,uint64_t slot_count){
    srand((unsigned)time(NULL));
    vector<uint64_t> leaf_num_permutation = random_permutation(leaf_num); //0 1 2 3 ... leaf_num
    vector<Ciphertext> x(0);
    vector<Ciphertext> y(0);
    for(int i = 0; i < leaf_num; i++){
        x.push_back(out[0][leaf_num_permutation[i]]);
        y.push_back(out[1][leaf_num_permutation[i]]);
    }
    int log_data_m = log(data_m + 1)/log(2);
    cout<<"log_data_m " <<log_data_m<<endl;
    for(int i = 0;i < log_data_m; i++){
        vector<uint64_t> data_m_permutation = random_permutation(data_m);
        vector<uint64_t> W0(slot_count,0);
        vector<uint64_t> W1(slot_count,0);
        int data_m_half = data_m / 2;
        for(int j = 0; j < data_m_half;j++){
            W0[j] = 1;
            W1[data_m_half + j] = 1;
        }
        bool flag_data_m_odd = data_m % 2;
        if(flag_data_m_odd){
            W1[data_m - 1] = 1;
        }
        Plaintext W0_pt, W1_pt;
        batch_encoder->encode(W0, W0_pt);
        batch_encoder->encode(W1, W1_pt);
        vector<uint64_t> L = random_permutation(leaf_num);
        vector<Ciphertext> x_temp0(0);
        vector<Ciphertext> y_temp0(0);
        vector<Ciphertext> x_temp1(0);
        vector<Ciphertext> y_temp1(0);
        for(int j = 0; j < leaf_num; j++){
            x_temp0.push_back(x[j]);
            y_temp0.push_back(y[j]);
            x_temp1.push_back(x[j]);
            y_temp1.push_back(y[j]);
        }
        for(int j = 0;j < leaf_num; j++){
            evaluator->multiply_plain_inplace(x_temp0[j],W0_pt);
            evaluator->multiply_plain_inplace(x_temp1[j],W1_pt);
            evaluator->multiply_plain_inplace(y_temp0[j],W0_pt);
            evaluator->multiply_plain_inplace(y_temp1[j],W1_pt);
        }
        for(int j = 0;j < leaf_num; j++){
            evaluator->add(x_temp0[L[j]],x_temp1[j],x[j]);
            evaluator->add(y_temp0[L[j]],y_temp1[j],y[j]);
        }
    }
    out[0] = x;
    out[1] = y;
    return out;
}

vector<vector<Ciphertext>> tecmp_cdcmp_pdte_clear_line_relation(BatchEncoder *batch_encoder, Evaluator *evaluator, vector<vector<Ciphertext>> out,int leaf_num,int data_m,uint64_t slot_count, uint64_t row_count, uint64_t num_cmps_per_row, uint64_t num_slots_per_element){
    srand((unsigned)time(NULL));
    vector<uint64_t> leaf_num_permutation = random_permutation(leaf_num); //0 1 2 3 ... leaf_num
    vector<Ciphertext> x(0);
    vector<Ciphertext> y(0);
    for(int i = 0; i < leaf_num; i++){
        x.push_back(out[0][leaf_num_permutation[i]]);
        y.push_back(out[1][leaf_num_permutation[i]]);
    }

    int log_data_m = log(data_m + 1)/log(2);

    //cout<<"data_m " <<data_m<<endl;
    cout<<"log_data_m " <<log_data_m<<endl;
    for(int i = 0;i < log_data_m; i++){
        vector<uint64_t> data_m_permutation = random_permutation(data_m);
        vector<uint64_t> W0(slot_count,0);
        vector<uint64_t> W1(slot_count,0);
        int data_m_half = data_m / 2;
        //cout<<"data_m_half "<<data_m_half<<endl;

        for(int j = 0; j < data_m_half;j++){
            bool flag = data_m_permutation[j] < num_cmps_per_row; 
            uint64_t jdx = flag ? ( data_m_permutation[j] * num_slots_per_element ) : ( row_count + (data_m_permutation[j] - num_cmps_per_row) * num_slots_per_element);
            W0[jdx] = 1;
            bool flag2 = data_m_permutation[data_m_half + j] < num_cmps_per_row; 
            uint64_t jdx2 = flag2 ? ( data_m_permutation[data_m_half + j] * num_slots_per_element ) : ( row_count + (data_m_permutation[data_m_half + j] - num_cmps_per_row) * num_slots_per_element);
            W1[jdx2] = 1;
        }

        bool flag_data_m_odd = data_m % 2;

        //cout<<"flag_data_m_odd "<<flag_data_m_odd<<endl;
        if(flag_data_m_odd){
            bool flag2 = data_m_permutation[data_m - 1] < num_cmps_per_row; 
            uint64_t jdx2 = flag2 ? ( data_m_permutation[data_m - 1] * num_slots_per_element ) : ( row_count + (data_m_permutation[data_m - 1] - num_cmps_per_row) * num_slots_per_element);
            W1[jdx2] = 1;
        }

        Plaintext W0_pt, W1_pt;
        batch_encoder->encode(W0, W0_pt);
        batch_encoder->encode(W1, W1_pt);
        vector<uint64_t> L = random_permutation(leaf_num);
        vector<Ciphertext> x_temp0(0);
        vector<Ciphertext> y_temp0(0);
        vector<Ciphertext> x_temp1(0);
        vector<Ciphertext> y_temp1(0);
        for(int j = 0; j < leaf_num; j++){
            x_temp0.push_back(x[j]);
            y_temp0.push_back(y[j]);
            x_temp1.push_back(x[j]);
            y_temp1.push_back(y[j]);
        }
        for(int j = 0;j < leaf_num; j++){
            evaluator->multiply_plain_inplace(x_temp0[j],W0_pt);
            evaluator->multiply_plain_inplace(x_temp1[j],W1_pt);
            evaluator->multiply_plain_inplace(y_temp0[j],W0_pt);
            evaluator->multiply_plain_inplace(y_temp1[j],W1_pt);
        }
        for(int j = 0;j < leaf_num; j++){
            evaluator->add(x_temp0[L[j]],x_temp1[j],x[j]);
            evaluator->add(y_temp0[L[j]],y_temp1[j],y[j]);
        }
    }
    out[0] = x;
    out[1] = y;
    return out;
}

Ciphertext map_zero_to_one_and_the_other_to_zero(Ciphertext ct_in,BatchEncoder *batch_encoder, Evaluator *evaluator, RelinKeys *rlk_server,  uint64_t tree_depth,Plaintext d_factorial_inv_with_sign_pt, uint64_t slot_count){
    Ciphertext ct_out;
    vector<Ciphertext> temp(tree_depth);
    for(int j = 0; j < tree_depth; j++){
        vector<uint64_t> vec_j(slot_count, j+1);
        Plaintext pt_vec_j;
        batch_encoder->encode(vec_j, pt_vec_j); //
        evaluator->sub_plain(ct_in, pt_vec_j, temp[j]);
    }
    evaluator->multiply_many(temp, *rlk_server, ct_in);
    evaluator->multiply_plain(ct_in, d_factorial_inv_with_sign_pt,ct_out);
    return ct_out;
}


Ciphertext private_info_retrieval(Evaluator *evaluator, vector<Ciphertext> in_first, vector<Plaintext> in_second){
    Ciphertext out;
    // client_input * cipher_node.feature_index_cipher;
    evaluator->multiply_plain(in_first[0], in_second[0], out);
    for(int i = 1; i < in_first.size(); i++){
        Ciphertext temp;
        evaluator->multiply_plain(in_first[i], in_second[i], temp);
        evaluator->add_inplace(out, temp);
    }
    return out;
}

Ciphertext private_info_retrieval_with_b_b_b(Evaluator *evaluator, vector<Ciphertext> in_first, vector<Plaintext> in_second, vector<uint64_t> in_second_vec, Ciphertext zero_zero_zero){
    Ciphertext out;
    for(int i=0;i<in_first.size();i++){
        if(in_second_vec[i]!=0){//seal ciphertext can not mult zero
            evaluator->multiply_plain_inplace(in_first[i],in_second[i]);
        }else{
            in_first[i] = zero_zero_zero;
        }
    }
    evaluator->add_many(in_first, out);

    return out;
}

Ciphertext private_info_retrieval(Evaluator *evaluator,RelinKeys *rlk_server, vector<Ciphertext> in_first, vector<Ciphertext> in_second){
    Ciphertext out;
    // client_input * cipher_node.feature_index_cipher;
    evaluator->multiply(in_first[0], in_second[0], out);
    evaluator->relinearize_inplace(out,*rlk_server);
    for(int i = 1; i < in_first.size(); i++){
        Ciphertext temp;
        evaluator->multiply(in_first[i], in_second[i], temp);
        evaluator->relinearize_inplace(temp,*rlk_server);
        evaluator->add_inplace(out, temp);
    }
    return out;
}

//column pir, 
vector<Ciphertext> private_info_retrieval(Evaluator *evaluator, vector<Ciphertext> in_first, vector<vector<Plaintext>> in_second){
    vector<Ciphertext> out(in_second[0].size());
    for(int j=0;j<in_second[0].size();j++){
        evaluator->multiply_plain(in_first[0], in_second[0][j], out[j]);
        for(int i = 1;i<in_first.size();i++){ // from 1, because the 0 is computed in the front.
            Ciphertext temp;
            evaluator->multiply_plain(in_first[i], in_second[i][j], temp);
            evaluator->add_inplace(out[j], temp);
        }
    }
    return out;
}