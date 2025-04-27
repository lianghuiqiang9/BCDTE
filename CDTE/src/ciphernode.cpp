#include "ciphernode.h"



uint64_t CipherNode::get_depth() {
    if (this->is_leaf()) {
        return 0;
    } else {
        auto l = this->left->get_depth();
        auto r = this->right->get_depth();
        if (l > r) {
            return l + 1;
        } else {
            return r + 1;
        }
    }
}

void compute_the_commun_rec(std::shared_ptr<CipherNode> a, long& commun) {
    std::stringstream str;
    if (a->is_leaf()){
        //class_leaf_cipher
       commun+=a->class_leaf_cipher.save(str);
    }else{
        //feature_index_cipher threshold_cipher
        for(int i=0;i<a->feature_index_cipher.size();i++){
            commun+=a->feature_index_cipher[i].save(str);
        }
        for(int i=0;i<a->threshold_cipher.size();i++){
            commun+=a->threshold_cipher[i].save(str);
        }

        compute_the_commun_rec(a->left, commun);
        compute_the_commun_rec(a->right, commun);
    }
}

long CipherNode::compute_the_commun() {
    long commun = 0;
    std::shared_ptr<CipherNode> cipher_node = std::make_shared<CipherNode>(*this);
    compute_the_commun_rec(cipher_node, commun);
    return commun;
}



bool CipherNode::is_leaf() const {
    return this->left == nullptr && this->right == nullptr;
}

void decrypt_the_cipher_tree_rec(seal::Decryptor *decryptor, std::shared_ptr<CipherNode> cipher_node, int max_index_add_one, seal::BatchEncoder *batch_encoder) {
    if (cipher_node->is_leaf()){
        Plaintext class_leaf_pt;
        decryptor->decrypt(cipher_node->class_leaf_cipher, class_leaf_pt);
        vector<uint64_t> class_leaf_vec;
        batch_encoder->decode(class_leaf_pt, class_leaf_vec);
        cout<<"leaf = "<<class_leaf_vec[0]<<endl;//" "<<class_leaf_vec[1]<<endl;

    }else{
        //cout<<"cipher_node->feature_index_cipher.size() = "<<cipher_node->feature_index_cipher.size()<<endl;
        int feature_index =-1;
        for(int i=0;i<max_index_add_one;i++){
            Plaintext feature_index_pt;
            decryptor->decrypt(cipher_node->feature_index_cipher[i], feature_index_pt);
            vector<uint64_t> feature_index_vec;
            batch_encoder->decode(feature_index_pt, feature_index_vec);
            if(feature_index_vec[0] == 1ULL){
                feature_index = i;
            }
        }
        cout<<"feature_index = "<< feature_index <<endl;

        //cout<<"cipher_node->threshold_cipher.size() = "<<cipher_node->threshold_cipher.size()<<endl;
        for(int i = 0;i < cipher_node->threshold_cipher.size(); i++){
            Plaintext threshold_pt;
            decryptor->decrypt(cipher_node->threshold_cipher[i], threshold_pt);
            vector<uint64_t> threshold_vec;
            batch_encoder->decode(threshold_pt, threshold_vec);
            //for(int j=0;j<16;j++){cout<<threshold_vec[j]<<" ";}cout<<endl;
            vector<uint64_t> threshold_vec_temp{threshold_vec.begin(), threshold_vec.begin() + 16};
            int t = BinaryVectorTointeger(threshold_vec_temp);
            cout<<" t : "<<t<<endl;
        }

        decrypt_the_cipher_tree_rec(decryptor, cipher_node->left, max_index_add_one, batch_encoder);
        decrypt_the_cipher_tree_rec(decryptor, cipher_node->right, max_index_add_one, batch_encoder);
    }
}

void CipherNode::decrypt_the_cipher_tree(seal::Decryptor *decryptor, int max_index_add_one, seal::BatchEncoder *batch_encoder){
    std::shared_ptr<CipherNode> cipher_node = std::make_shared<CipherNode>(*this);
    decrypt_the_cipher_tree_rec(decryptor, cipher_node, max_index_add_one, batch_encoder);

}

void build_cipher_tree_from_plain_tree_rec(std::shared_ptr<Node> node, CipherNode& cipher_node, uint64_t num_cmps, int max_index_add_one, seal::BatchEncoder *batch_encoder, seal::Encryptor *encryptor, int slot_count, int row_count,  int num_cmps_per_row, int num_slots_per_element) {
    if (node->is_leaf()){
        //cout<<"node->class_leaf : "<<node->class_leaf<<endl;
        vector<uint64_t> class_leaf_vec(slot_count, node->class_leaf);
        Plaintext class_leaf_vec_pt; 
        batch_encoder->encode(class_leaf_vec, class_leaf_vec_pt);
        Ciphertext class_leaf_vec_ct;
        encryptor->encrypt(class_leaf_vec_pt, class_leaf_vec_ct);
        cipher_node.class_leaf_cipher = class_leaf_vec_ct;
        cipher_node.left = nullptr;
        cipher_node.right = nullptr;
    }else{
        //cout<<"node->feature_index : "<<node->feature_index<<endl;
        //server tree max_index may be less than the client data column;
        for(int i = 0; i < max_index_add_one; i++){
            vector<uint64_t> feature_index_vec(slot_count, 0ULL);
            if(i == node->feature_index){
                vector<uint64_t> temp(slot_count, 1ULL);
                feature_index_vec = temp;
            }
            Plaintext feature_index_vec_pt; 
            batch_encoder->encode(feature_index_vec, feature_index_vec_pt);
            Ciphertext feature_index_vec_ct;
            encryptor->encrypt(feature_index_vec_pt, feature_index_vec_ct);
            cipher_node.feature_index_cipher.push_back(feature_index_vec_ct);
        }

        //cout<<"node->threshold : "<<node->threshold<<endl;
        vector<uint64_t> threshold_vec(num_cmps, node->threshold);
        vector<uint64_t> threshold_vec_encode = cdcmp_encode_a(threshold_vec,num_slots_per_element,slot_count,row_count,num_cmps_per_row);
        Ciphertext temp;
        Plaintext plaintext; 
        batch_encoder->encode(threshold_vec_encode, plaintext);
        encryptor->encrypt(plaintext, temp);       
        cipher_node.threshold_cipher.push_back(temp);

        cipher_node.left = std::make_shared<CipherNode>();
        cipher_node.right = std::make_shared<CipherNode>();

        build_cipher_tree_from_plain_tree_rec(node->left, *cipher_node.left, num_cmps, max_index_add_one, batch_encoder,encryptor,slot_count,row_count,num_cmps_per_row,num_slots_per_element);
        build_cipher_tree_from_plain_tree_rec(node->right, *cipher_node.right, num_cmps, max_index_add_one, batch_encoder,encryptor,slot_count,row_count,num_cmps_per_row,num_slots_per_element);
    }

}

CipherNode::CipherNode(Node& root, uint64_t num_cmps, int max_index_add_one, seal::BatchEncoder *batch_encoder, seal::Encryptor *encryptor, int slot_count, int row_count, int num_cmps_per_row, int num_slots_per_element){
    std::shared_ptr<Node> node = std::make_shared<Node>(root);
    build_cipher_tree_from_plain_tree_rec(node, *this, num_cmps, max_index_add_one, batch_encoder,encryptor,slot_count,row_count, num_cmps_per_row,num_slots_per_element);
}


void leaf_extract_rec(vector<Ciphertext>& out,CipherNode& node ){
    if (node.is_leaf()){
        out.push_back(node.class_leaf_cipher);
    }else{
        leaf_extract_rec(out, *(node.left));
        leaf_extract_rec(out, *(node.right));
    }
}

