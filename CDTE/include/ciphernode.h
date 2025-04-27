#ifndef __CIPHERNODE__
#define __CIPHERNODE__

#include "utils.h"
#include<seal/seal.h>
#include "node.h"
using namespace seal;
using namespace std;

class CipherNode {
public:
    std::shared_ptr<CipherNode> left;
    std::shared_ptr<CipherNode> right;

    vector<Ciphertext> feature_index_cipher;
    vector<Ciphertext> threshold_cipher;

    Ciphertext class_leaf_cipher;
    
    std::string op;

    Ciphertext value;

    CipherNode() = default;
    explicit CipherNode(Node& root, uint64_t num_cmps, int max_index_add_one, seal::BatchEncoder *batch_encoder, seal::Encryptor *encryptor, int slot_count, int row_count, int num_cmps_per_row, int num_slots_per_element);
    bool is_leaf() const;
    uint64_t get_depth();
    void decrypt_the_cipher_tree(seal::Decryptor *decryptor,int max_index_add_one, seal::BatchEncoder *batch_encoder);
    long compute_the_commun();
};

void leaf_extract_rec(vector<Ciphertext>& out,CipherNode& node );



#endif