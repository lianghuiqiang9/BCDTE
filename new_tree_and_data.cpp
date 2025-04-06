#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include "node.h"

using namespace std;

/*
g++ -o new_tree_and_data -O3 new_tree_and_data.cpp -I ./CDTE/include -I /usr/local/include/SEAL-4.1 -lseal-4.1 -L ./CDTE/lib -lcdte
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"""***you***"""/BPDTE-Advance/CDTE/lib
mkdir ./data/heart_16bits 
./new_tree_and_data -i ./data/heart_11bits -o ./data/heart_16bits -n 16 -s 16384 
*/
int main(int argc, char* argv[]){
    string inputfile;
    string outputfile;
    int bit_length = 16;
    int data_size = 16*1024;
    int opt;
    while ((opt = getopt(argc, argv, "ft:i:o:n:s:")) != -1) {
        switch (opt) {
        case 'i': inputfile = string(optarg); break;
        case 'o': outputfile = string(optarg); break;
        case 'n': bit_length = atoi(optarg); break;
        case 's': data_size = atoi(optarg); break;
        }
    }


    if(bit_length>32){cout<<"rand() only support 0 - 2^32-1, please use different method"<<endl;exit(0);}
    uint64_t mod = bit_length<32 ? (1<<bit_length) : 4294967296;
    cout<<" mod : "<< mod <<endl;
    cout<<" data_size : "<< data_size <<endl;

    string input_data_address= inputfile + "/x_test.csv";
    vector<vector<uint64_t>> client_data = read_csv_to_vector(input_data_address, data_size);
    for(int i=0;i<client_data.size();i++){
        for(int j=0;j<client_data[i].size();j++){
            client_data[i][j] = rand()% mod;
        }
    }
    save_vector_to_csv(client_data, outputfile + "/x_test.csv");

    cout<<"Load the tree"<<endl;
    string input_tree_address = inputfile + "/model.json";//"../data/heart_11bits/model.json";
    Node root = Node(input_tree_address);
    /*
    cout<<"print tree"<<endl;
    print_tree(root);
    cout<<"root.leaf_num()  = "<<root.leaf_num()<<endl;
    */
    json tree_json = root.to_json_with_random_value(mod);
    std::ofstream file(outputfile + "/model.json");
    file << tree_json << std::endl;  
    std::cout << "Tree has been saved to tree.json." << std::endl;
    /*
    Node root_with_changed_bitlength = Node(outputfile + "/model.json");
    cout<<"print tree"<<endl;
    print_tree(root_with_changed_bitlength);
    cout<<"root_with_changed_bitlength.leaf_num()  = "<<root_with_changed_bitlength.leaf_num()<<endl;
    */

    cout<<" bit_length : "<< bit_length <<endl;
    cout<<" expect_input_data_size : "<< data_size <<endl;
    cout<<" actual_input_data_size : "<<client_data.size()<<endl;
}