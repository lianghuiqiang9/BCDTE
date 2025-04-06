#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include "ciphernode.h"
#include "pdte.h"
#include<seal/seal.h>

using namespace std;
using namespace seal;

/*

g++ -o pir_cdcmp_cdte -O3 pir_cdcmp_cdte.cpp -I ./CDTE/include -L ./CDTE/lib -lcdte -I /usr/local/include/SEAL-4.1 -lseal-4.1 

./pir_cdcmp_cdte -t ./data/heart_16bits/model.json -v ./data/heart_16bits/x_test.csv -r 1048576 -n 16 -d 3 -e 4

*/


void cdcmp_cdte_rec( vector<Ciphertext>& out, CipherNode& cipher_node, Evaluator *evaluator,GaloisKeys* gal_keys_server, RelinKeys *rlk_server, vector<Ciphertext> client_input, Plaintext one_one_one, int num_slots_per_element){
    if (cipher_node.is_leaf()){
        out.push_back(cipher_node.value);
    }else{
        //PIR process
        // client_input * cipher_node.feature_index_cipher;
        Ciphertext client_index_cipher = private_info_retrieval(evaluator,rlk_server, cipher_node.feature_index_cipher, client_input);
        /**/
        //pearent     0           *  parent       1
        //left      0   1  right  *  left       1   0     right
        //note that the index of client_data in the tree starts from 0
        //cmp threshold > x[a]
        cipher_node.right->value = cdcmp(evaluator, gal_keys_server, rlk_server, num_slots_per_element, cipher_node.threshold_cipher[0], client_index_cipher);
        evaluator->negate(cipher_node.right->value, cipher_node.left->value);
        evaluator->add_plain_inplace(cipher_node.left->value, one_one_one);
        
        //travel
        evaluator->add_inplace(cipher_node.left->value, cipher_node.value);
        evaluator->add_inplace(cipher_node.right->value, cipher_node.value);
        cdcmp_cdte_rec(out, *(cipher_node.left), evaluator,gal_keys_server, rlk_server, client_input, one_one_one, num_slots_per_element);
        cdcmp_cdte_rec(out, *(cipher_node.right), evaluator,gal_keys_server, rlk_server, client_input, one_one_one, num_slots_per_element);
    }
}

int main(int argc, char* argv[]){
    string address_data;
    string address_tree;
    int n;
    int opt;
    int d;
    int e;
    int data_size = 16384;
    int query = 0; // suppose start from 0 
    while ((opt = getopt(argc, argv, "ft:v:n:d:e:r:q:")) != -1) {
        switch (opt) {
        case 't': address_tree = string(optarg); break;
        case 'v': address_data = string(optarg); break;
        case 'n': n = atoi(optarg); break;
        case 'd': d = atoi(optarg); break;
        case 'e': e = atoi(optarg); break;
        case 'r': data_size = atoi(optarg); break;
        case 'q': query = atoi(optarg); break;
        }
    }
    stringstream client_send;
    stringstream server_send;
    long client_send_commun=0;
    long server_send_commun=0;

    cout<<"******************************* step 1: server begin *******************************"<<endl;
    clock_t global_start = clock();
    clock_t start = clock();

    // init the fhe scheme
    cout<<"Init fhe ... " <<endl;
    EncryptionParameters parms;

    parms = cdcmp_init(n,d,e);// e = 2 is ok, or e = 3
    
    SEALContext* context = new SEALContext(parms);
    KeyGenerator keygen(*context);
    PublicKey pk;
    keygen.create_public_key(pk);
    SecretKey sk = keygen.secret_key();
    Encryptor *encryptor = new Encryptor(*context, pk);
    Decryptor *decryptor = new Decryptor(*context, sk);
    Evaluator *evaluator = new Evaluator(*context);
    BatchEncoder* batch_encoder = new BatchEncoder(*context);
    RelinKeys* rlk_server = new RelinKeys();
    keygen.create_relin_keys(*rlk_server);
    GaloisKeys* gal_keys_server = new GaloisKeys();
    keygen.create_galois_keys(*gal_keys_server);    
    uint64_t plain_modulus = parms.plain_modulus().value();

    //explain slot info 
    uint64_t slot_count = batch_encoder->slot_count();
    uint64_t row_count = slot_count / 2;
    uint64_t num_slots_per_element = n;
    uint64_t num_cmps_per_row = row_count / num_slots_per_element; 
    uint64_t num_cmps = 2 * num_cmps_per_row;

    cout << "Init fhe finish ... " <<endl;
    cout << "Plaintext matrix num_cmps:         " << num_cmps << endl;
    cout<<  "Init the BFV batch scheme,                        run time is "<<(clock()-start) /1000<<" ms"<<endl;start = clock();

    // load tree
    cout<<"load the tree"<<endl;
    Node root = Node(address_tree);
    int max_index_add_one = root.max_index() + 1;
//print_tree(root);
    cout<<"encrypt the tree .."<<endl;
    CipherNode cipher_root = CipherNode(root, num_cmps, max_index_add_one, batch_encoder, encryptor, slot_count,row_count,num_cmps_per_row,num_slots_per_element);
    //cout<<"verify the encrypt is correct"<<endl;
    //cipher_root.decrypt_the_cipher_tree(decryptor, max_index_add_one, batch_encoder);

    uint64_t tree_depth = cipher_root.get_depth();
    cout<<"tree_depth : "<<tree_depth<<endl;
    if(d < tree_depth){cout<<" The d is too small, should bigger than the tree_depth : "<<tree_depth<<"."<<endl;exit(0);}

    cout<<"encrypt the tree done,                            run time is "<<(clock()-start) /1000<<" ms"<<endl;start = clock();

    cout<<"******************************* step 1: server end   *******************************"<<endl;
    
    server_send_commun += cipher_root.compute_the_commun();

    cout<<"******************************* step 2: server begin *******************************"<<endl;
    //生成查询向量两个，一个group 即query1，另一个data_m 即query2, query2需要编码进行处理。

    if(query > data_size){cout<<"query should small than the data_size"<<endl;exit(0);}
    int query_first_index_max = data_size / num_cmps + ((data_size % num_cmps) > 0 ? 1:0); //16,  0...15
    int query_second_index_max = num_cmps; //1024, 0...1023

    int query_first_index = query / num_cmps;
    int query_second_index = query % num_cmps;// suppose start from 0 
    vector<Ciphertext> query_first_index_cipher(query_first_index_max);
    for(int i=0;i<query_first_index_max;i++){
        if(i == query_first_index){
            query_first_index_cipher[i] = init_one_one_one_cipher(batch_encoder,encryptor,slot_count);
        }else{
            query_first_index_cipher[i] = init_zero_zero_zero_cipher(batch_encoder,encryptor,slot_count);
        }
    }
    Ciphertext query_second_index_cipher = init_only_index_is_one_cipher(batch_encoder,encryptor,query_second_index,slot_count,num_cmps,num_cmps_per_row,num_slots_per_element,row_count);

    cout<<"generate a query done,                            run time is "<<(clock()-start) /1000<<" ms"<<endl;start = clock();
    cout<<"******************************* step 2: server end   *******************************"<<endl;

    for(int i = 0; i < query_first_index_cipher.size(); i++){
        server_send_commun += query_first_index_cipher[i].save(server_send);
    }
    server_send_commun += query_second_index_cipher.save(server_send);

    cout<<"******************************* step 3: client begin *******************************"<<endl;

    vector<vector<uint64_t>> client_data = read_csv_to_vector(address_data, data_size);
    if(client_data.size() < data_size){
        cout<<"the real client_data_size is small than the input data_size, query may be exceed the client_data_size, "<<endl;
        cout<<"so choose the input data_size again, not exceed the client_data_size "<<client_data.size()<<endl;exit(0);
    }
    cout<<"data_n    = "<<client_data[0].size()<<endl;//print_data(client_data); 

    cout<<"load the client_data,                             run time is "<<(clock()-start)/1000 <<" ms"<<endl;start = clock();

    vector<vector<vector<uint64_t>>> client_data_3D=splitVectorIntoChunks(client_data, num_cmps);
    vector<vector<vector<uint64_t>>> client_data_3D_transpose;
    for(int i=0;i<client_data_3D.size();i++){
        client_data_3D_transpose.push_back(Transpose(client_data_3D[i]));
    }
    client_data_3D_transpose.back() = Matrix_padding(client_data_3D_transpose.back(), num_cmps);

    vector<vector<Plaintext>> client_input;
    for(int i=0;i<client_data_3D_transpose.size();i++){
        vector<Plaintext> chunk;
        for(int j=0;j<client_data_3D_transpose[0].size();j++){
            vector<uint64_t> plain_op_encode = cdcmp_encode_b(client_data_3D_transpose[i][j],num_slots_per_element,slot_count,row_count,num_cmps_per_row);
            Plaintext pt; 
            batch_encoder->encode(plain_op_encode, pt);
            chunk.push_back(pt);
        }
        client_input.push_back(chunk);
    }

    cout<<"Init the client data,                             run time is "<<(clock()-start)/1000 <<" ms"<<endl;start = clock();

    cout<<"******************************* step 3: client end   *******************************"<<endl;

    cout<<"******************************* step 4: client begin *******************************"<<endl;

    //client_input 现在要变成二维的plaintext了, //使用group密文处理client_data, 考虑到内存问题，一块一块读取再乘起来//真正要用的时候，需要注意内存循环情况。
    vector<Ciphertext> client_input_after_query_first = private_info_retrieval(evaluator, query_first_index_cipher, client_input);// +1
    clock_t pir_step_1 = clock()-start;
    cout<<"PIR step 1 in the client data,                    run time is "<<(pir_step_1)/1000 <<" ms"<<endl;start = clock();

    Plaintext one_one_one = init_one_one_one(batch_encoder, slot_count);
    cipher_root.value = init_zero_zero_zero_cipher(batch_encoder, encryptor, slot_count);
    vector<Ciphertext> cdte_out;
    cdcmp_cdte_rec(cdte_out, cipher_root, evaluator,gal_keys_server, rlk_server, client_input_after_query_first,one_one_one, num_slots_per_element);

    uint64_t d_factorial_inv_with_sign = init_the_d_factorial_inv_with_sign(tree_depth,plain_modulus);
    Plaintext d_factorial_inv_with_sign_pt = init_b_b_b(d_factorial_inv_with_sign, batch_encoder, slot_count);
    for(int i = 0; i < cdte_out.size(); i++){
        cdte_out[i] = map_zero_to_one_and_the_other_to_zero(cdte_out[i],batch_encoder,evaluator,rlk_server,tree_depth,d_factorial_inv_with_sign_pt,slot_count);
    }

    vector<Ciphertext> leaf_vec_cipher;
    leaf_extract_rec(leaf_vec_cipher,cipher_root);
    cout<<"leaf_vec_cipher.size() = "<< leaf_vec_cipher.size() <<endl;

    Ciphertext client_out = private_info_retrieval(evaluator, rlk_server, leaf_vec_cipher, cdte_out); // cdte +1 client_out +1
    
    
    cout<<"CDTE            "<< num_cmps <<  "   col data ,         overall run time is "<< (clock() - start)/1000 <<" ms"<<endl;

    evaluator->multiply_inplace(client_out, query_second_index_cipher); //+1

    clock_t pir_step_2 = clock() - start; 
    cout<<"PIR step 2 in the client data,                    run time is "<<(pir_step_2)/1000 <<" ms"<<endl;start = clock();

    cout<<"******************************* step 4: client end   *******************************"<<endl;

    client_send_commun += client_out.save(client_send);

    cout<<"******************************* step 5: server start *******************************"<<endl;

    //query1 * num_cmps + query2

    Plaintext client_out_pt;
    decryptor->decrypt(client_out, client_out_pt);
    vector<uint64_t> client_out_vec;
    batch_encoder->decode(client_out_pt, client_out_vec);
    
    uint64_t expect_result = client_out_vec[(query_second_index < num_cmps_per_row) ? ( query_second_index * num_slots_per_element ) : ( row_count + (query_second_index - num_cmps_per_row) * num_slots_per_element)];

    cout<<"decrypt the result ,                              run time is "<<(float)(clock()-start)/1000 <<"ms"<<endl;start = clock();

    uint64_t actural_result = root.eval(client_data[query]);

    cout<< "the compare result : "<<expect_result <<" "<<actural_result<< endl;
    if(!(expect_result == actural_result)){cout<<"may be the depth_need_min is too small, need add the extra number. "<<endl;exit(0);}

    long comm = client_send_commun + server_send_commun;
    clock_t pir_overall_run_time = pir_step_1 + pir_step_2;

    cout<<"compare with the real result ,                    run time is "<<(clock()-start)/1000 <<" ms"<<endl;start = clock();
    cout<<"************************************************************************************"<<endl;
    cout<<"address_tree : "<<address_tree<<endl;
    cout<<"n            : "<< n <<endl;
    cout<<"PIR step 1      "<< data_size <<  "   col data ,                 run time is "<< pir_step_1/1000 <<" ms"<<endl;
    cout<<"PIR step 2      "<< data_size <<  "   col data ,                 run time is "<< pir_step_2/1000 <<" ms"<<endl;
    cout<<"PIRCDTE         "<< data_size <<  "   col data ,         overall run time is "<< pir_overall_run_time/1000 <<" ms"<<endl;
    cout<<"PIRCDTE         "<< data_size <<  "   col data ,         overall commun.  is "<< comm/1000 <<" KB"<<endl;
    cout<<"******************************* step 5: server end   *******************************"<<endl;
    cout<<"all done,  the overall run time is "<< (clock() - global_start)/1000 <<" ms"<<endl;
    return 0;

}

