#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include "ciphernode.h"
#include "pdte.h"
#include<seal/seal.h>

using namespace std;
using namespace seal;


//g++ -o cdcmp_cdte -O3 cdcmp_cdte.cpp -I ./CDTE/include -I /usr/local/include/SEAL-4.1 -lseal-4.1 -L ./CDTE/lib -lcdte

//./cdcmp_cdte -t ./data/heart_11bits/model.json -v ./data/heart_11bits/x_test.csv -r 1024 -n 16 -d 3 -e 2



void cdcmp_cdte_rec(seal::Decryptor *decryptor, vector<Ciphertext>& out, CipherNode& cipher_node, Evaluator *evaluator,GaloisKeys* gal_keys_server, RelinKeys *rlk_server, vector<Plaintext> client_input, Plaintext one, BatchEncoder *batch_encoder, int num_cmps, int num_slots_per_element, uint64_t slot_count,uint64_t row_count, uint64_t num_cmps_per_row){
    if (cipher_node.is_leaf()){
        out.push_back(cipher_node.value);
    }else{
        //PIR process
        // client_input * cipher_node.feature_index_cipher;
        Ciphertext client_index_cipher = private_info_retrieval(evaluator, cipher_node.feature_index_cipher, client_input);
        

        //pearent     0           *  parent       1
        //left      0   1  right  *  left       1   0     right
        //note that the index of client_data in the tree starts from 0
        //cmp threshold > x[a]
        cipher_node.right->value = cdcmp(evaluator, gal_keys_server, rlk_server, num_slots_per_element, cipher_node.threshold_cipher[0], client_index_cipher);
        evaluator->negate(cipher_node.right->value, cipher_node.left->value);
        evaluator->add_plain_inplace(cipher_node.left->value, one);

        //travel
        evaluator->add_inplace(cipher_node.left->value, cipher_node.value);
        evaluator->add_inplace(cipher_node.right->value, cipher_node.value);
        cdcmp_cdte_rec(decryptor, out, *(cipher_node.left), evaluator,gal_keys_server, rlk_server, client_input,one, batch_encoder,num_cmps, num_slots_per_element, slot_count, row_count, num_cmps_per_row);
        cdcmp_cdte_rec(decryptor, out, *(cipher_node.right), evaluator,gal_keys_server, rlk_server, client_input,one, batch_encoder,num_cmps, num_slots_per_element, slot_count, row_count, num_cmps_per_row);
    }
}

int main(int argc, char* argv[]){
    string address_data;
    string address_tree;
    int n;
    int data_m;
    int opt;
    int d;
    int e;
    while ((opt = getopt(argc, argv, "ft:v:r:n:d:e:")) != -1) {
        switch (opt) {
        case 't': address_tree = string(optarg); break;
        case 'v': address_data = string(optarg); break;
        case 'r': data_m = atoi(optarg); break;
        case 'n': n = atoi(optarg); break;
        case 'd': d = atoi(optarg); break;
        case 'e': e = atoi(optarg); break;
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
    cout<<  "Init the BFV batch scheme,                     run time is "<<(clock()-start) /1000<<" ms"<<endl;start = clock();

    // load tree
    cout<<"load the tree"<<endl;
    Node root = Node(address_tree);
    int max_index_add_one = root.max_index() + 1;
    print_tree(root);
    cout<<"encrypt the tree .."<<endl;
    CipherNode cipher_root = CipherNode(root, num_cmps, max_index_add_one, batch_encoder, encryptor, slot_count,row_count,num_cmps_per_row,num_slots_per_element);
    //cout<<"verify the encrypt is correct"<<endl;
    //cipher_root.decrypt_the_cipher_tree(decryptor, max_index_add_one, batch_encoder);

    cout<<"encrypt the tree done,                         run time is "<<(clock()-start) /1000<<" ms"<<endl;start = clock();
    
    if(num_cmps < data_m){
        cout<<"data_m : "<<data_m<<" >= num_cmps : "<<num_cmps<<endl;
        cout<<"data_m is too large, please divide different page until the size is small than num_cmps"<<endl;
        exit(0);
    }

    uint64_t tree_depth = cipher_root.get_depth();
    cout<<"tree_depth : "<<tree_depth<<endl;
    if(d < tree_depth){
        cout<<" The d is too small, should bigger than the tree_depth : "<<tree_depth<<"."<<endl;
        exit(0);
    }

    cout<<"******************************* step 1: server end   *******************************"<<endl;
    
    server_send_commun += cipher_root.compute_the_commun();


    cout<<"******************************* step 2: client begin *******************************"<<endl;
    
    
    vector<vector<uint64_t>> client_data = read_csv_to_vector(address_data, data_m);
    data_m = client_data.size();// need client_data size to confirm;
    int data_n = client_data[0].size();
    cout<<"data_m    = "<<data_m<<endl;
    cout<<"data_n    = "<<data_n<<endl;//print_data(client_data); 

    cout<<"load the client_data,                          run time is "<<(clock()-start)/1000 <<" ms"<<endl;start = clock();
    
    vector<vector<uint64_t>> client_date_transpose_temp = Transpose(client_data); 
    vector<vector<uint64_t>> client_date_transpose = Matrix_padding(client_date_transpose_temp, num_cmps);
    vector<Plaintext> client_input;

    for(int i = 0; i < client_date_transpose.size(); i++){
        vector<uint64_t> plain_op_encode = cdcmp_encode_b(client_date_transpose[i],num_slots_per_element,slot_count,row_count,num_cmps_per_row);
        Plaintext pt; 
        batch_encoder->encode(plain_op_encode, pt);
        client_input.push_back(pt);
    }
    cout<<"Init the client data,                          run time is "<<(clock()-start)/1000 <<" ms"<<endl;start = clock();
    
    Plaintext one_one_one = init_one_one_one(batch_encoder, slot_count);
    cipher_root.value = init_zero_zero_zero_cipher(batch_encoder, encryptor, slot_count);

    vector<Ciphertext> cdte_out;

    cdcmp_cdte_rec(decryptor, cdte_out, cipher_root, evaluator, gal_keys_server, rlk_server, client_input, one_one_one, batch_encoder,num_cmps, num_slots_per_element, slot_count, row_count, num_cmps_per_row);

    vector<Ciphertext> leaf_vec_cipher;
    leaf_extract_rec(leaf_vec_cipher,cipher_root);
    cout<<"leaf_vec_cipher.size() = "<< leaf_vec_cipher.size() <<endl;


    uint64_t d_factorial_inv_with_sign = init_the_d_factorial_inv_with_sign(tree_depth,plain_modulus);
    Plaintext d_factorial_inv_with_sign_pt = init_b_b_b(d_factorial_inv_with_sign, batch_encoder, slot_count);
    for(int i = 0; i < cdte_out.size(); i++){
        cdte_out[i] = map_zero_to_one_and_the_other_to_zero(cdte_out[i],batch_encoder,evaluator,rlk_server,tree_depth,d_factorial_inv_with_sign_pt,slot_count);
    }

    Ciphertext client_out = private_info_retrieval(evaluator, rlk_server, leaf_vec_cipher, cdte_out); // cdte +1 client_out +1

    clock_t finish = clock() - start; start = clock();

    cout<<"******************************* step 2: client end   *******************************"<<endl;
    //client_out
    client_send_commun += client_out.save(client_send);

    cout<<"******************************* step 3: server start *******************************"<<endl;

    
    Plaintext pt;
    decryptor->decrypt(client_out,pt);
    vector<uint64_t> res;
    batch_encoder->decode(pt, res);
    vector<uint64_t> expect_result(num_cmps);
    for(int j = 0; j < num_cmps ; j++){
        //jdx = 0            num_cmps_per_row                2 * num_cmps_per_row              ...
        //    = row_count    row_count + num_cmps_per_row    row_count + 2 * num_cmps_per_row  ...
        bool flag = j < num_cmps_per_row; 
        uint64_t jdx = flag ? ( j * num_slots_per_element ) : ( row_count + (j - num_cmps_per_row) * num_slots_per_element);
        expect_result[j] = res[jdx];
    }

    cout<<"decrypt the result ,                           run time is "<<(float)(clock()-start)/1000 <<"ms"<<endl;start = clock();

    for(int j = 0; j < data_m ; j++){
        uint64_t actural_result = root.eval(client_data[j]);
        //cout<< expect_result[j] <<" "<<actural_result<< endl;
        if( !( expect_result[j] == actural_result ) ){
            cout<<"In "<< j <<" col is  error "<< endl;
            cout<<expect_result[j]  <<"  "<<actural_result << endl;
            exit(0);
        }           
    }
    long comm = client_send_commun + server_send_commun;
    cout<<"compare with the real result ,                 run time is "<<(clock()-start)/1000 <<" ms"<<endl;start = clock();
    cout<<"************************************************************************************"<<endl;
    cout<<"address_tree : "<<address_tree<<endl;
    cout<<"n            : "<< n <<endl;
    cout<<"PDTE         "<< data_m <<  "   col data ,         overall run time is "<< finish/1000 <<" ms"<<endl;
    cout<<"PDTE         "<< data_m <<  "   col data ,         overall commun.  is "<< comm/1000 <<" KB"<<endl;
    cout<<"PDTE         "<< data_m <<  "   col data ,         average run time is "<< finish/1000/data_m <<" ms"<<endl;
    cout<<"PDTE         "<< data_m <<  "   col data ,         average commun.  is "<< comm/1000/data_m <<" KB"<<endl;
    cout<<"******************************* step 3: server end   *******************************"<<endl;
    cout<<"all done, overall run time is "<< (clock() - global_start)/1000 <<" ms"<<endl;

    return 0;


}

