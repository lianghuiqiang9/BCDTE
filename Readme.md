
## Install libcdte.so
cd CDTE
bash installCDTE.sh
cd ..

## build

bash build_cdte.sh
cd build

# new data 

mkdir ../data/heart_16bits 
./new_tree_and_data -i ../data/heart_11bits -o ../data/heart_16bits -n 16 -s 16384 

mkdir ../data/heart_32bits 
./new_tree_and_data -i ../data/heart_11bits -o ../data/heart_32bits -n 32 -s 16384 

mkdir ../data/breast_16bits 
./new_tree_and_data -i ../data/breast_11bits -o ../data/breast_16bits -n 16 -s 16384 

mkdir ../data/breast_32bits 
./new_tree_and_data -i ../data/breast_11bits -o ../data/breast_32bits -n 32 -s 16384 

# pir_cdcmp_cdte

./pir_cdcmp_cdte -t ../data/heart_16bits/model.json -v ../data/heart_16bits/x_test.csv -n 16 -d 3 -e 5 -r 2048 -q 1030

./pir_cdcmp_cdte -t ../data/heart_16bits/model.json -v ../data/heart_16bits/x_test.csv -n 16 -d 3 -e 5 -r 1048576 -q 3049

./pir_cdcmp_cdte -t ../data/breast_16bits/model.json -v ../data/breast_16bits/x_test.csv -n 16 -d 7 -e 4 -r 2048 -q 1031

//1024*128 ok 1024*256 not ok. so the max tree n = 16, d = 8, m = 10; the data 1024 * 128 * 30 = 1024 * 1024 * 3.75
./pir_cdcmp_cdte -t ../data/breast_16bits/model.json -v ../data/breast_16bits/x_test.csv -n 16 -d 7 -e 4 -r 131072 -q 1031


## cmp_branch

cd cmp_bench
bash build_cmp_bench.sh
cd build

./tecmp -l 4 -m 2
./tecmp -l 4 -m 3
./tecmp -l 8 -m 2
./tecmp -l 16 -m 2
./tecmp -l 32 -m 2
./tecmp -l 64 -m 2
./tecmp -l 128 -m 2
./tecmp -l 256 -m 2
./tecmp -l 512 -m 2

./rdcmp -n 8
./rdcmp -n 16
./rdcmp -n 32
./rdcmp -n 64
./rdcmp -n 128
./rdcmp -n 256
./rdcmp -n 512
./rdcmp -n 1024

./cdcmp -n 8
./cdcmp -n 16
./cdcmp -n 32
./cdcmp -n 64
./cdcmp -n 128
./cdcmp -n 256
./cdcmp -n 512
./cdcmp -n 1024

# the most bit precision
./tecmp -l 2048 -m 13



lhq@ubuntu22:~/playground/BCDTE/build$ ./pir_cdcmp_cdte -t ../data/heart_16bits/model.json -v ../data/heart_16bits/x_test.csv -n 16 -d 3 -e 5 -r 2048 -q 1030
******************************* step 1: server begin *******************************
Init fhe ... 
depth_need_min 12
Init fhe finish ... 
Plaintext matrix num_cmps:         1024
Init the BFV batch scheme,                        run time is 1910 ms
load the tree
encrypt the tree ..
tree_depth : 3
encrypt the tree done,                            run time is 538 ms
******************************* step 1: server end   *******************************
******************************* step 2: server begin *******************************
query_first_index_max: 2 query_second_index_max: 1024
num_cmps: 1024
query_first_index: 1 query_second_index: 6
generate a query done,                            run time is 301 ms
******************************* step 2: server end   *******************************
query_first_index_cipher.size(): 2
******************************* step 3: client begin *******************************
data_n    = 13
**    = 2048
load the client_data,                             run time is 20 ms
num_cmps: 1024
2 1024 13
Init the client data,                             run time is 8 ms
******************************* step 3: client end   *******************************
******************************* step 4: client begin *******************************
2 13 2
client_input_after_query_first.size(): 13
PIR step 1 in the client data,                    run time is 1009 ms
leaf_vec_cipher.size() = 5
CDTE            1024   col data ,         overall run time is 6695 ms
PIR step 2 in the client data,                    run time is 6695 ms
******************************* step 4: client end   *******************************
******************************* step 5: server start *******************************
decrypt the result ,                              run time is 80.493ms
the compare result : 0 2
may be the depth_need_min is too small, need add the extra number. 
compare with the real result ,                    run time is 0 ms
************************************************************************************
address_tree : ../data/heart_16bits/model.json
n            : 16
PIR step 1      2048   col data ,                 run time is 1009 ms
PIR step 2      2048   col data ,                 run time is 6695 ms
PIRCDTE         2048   col data ,         overall run time is 7705 ms
PIRCDTE         2048   col data ,         overall commun.  is 104094 KB
******************************* step 5: server end   *******************************
all done,  the overall run time is 10566 ms
lhq@ubuntu22:~/playground/BCDTE/build$ ./pir_cdcmp_cdte -t ../data/heart_16bits/model.json -v ../data/heart_16bits/x_test.csv -n 16 -d 3 -e 5 -r 2048 -q 1029
******************************* step 1: server begin *******************************
Init fhe ... 
depth_need_min 12
Init fhe finish ... 
Plaintext matrix num_cmps:         1024
Init the BFV batch scheme,                        run time is 1946 ms
load the tree
encrypt the tree ..
tree_depth : 3
encrypt the tree done,                            run time is 573 ms
******************************* step 1: server end   *******************************
******************************* step 2: server begin *******************************
query_first_index_max: 2 query_second_index_max: 1024
num_cmps: 1024
query_first_index: 1 query_second_index: 5
generate a query done,                            run time is 330 ms
******************************* step 2: server end   *******************************
query_first_index_cipher.size(): 2
******************************* step 3: client begin *******************************
data_n    = 13
**    = 2048
load the client_data,                             run time is 20 ms
num_cmps: 1024
2 1024 13
Init the client data,                             run time is 7 ms
******************************* step 3: client end   *******************************
******************************* step 4: client begin *******************************
2 13 2
client_input_after_query_first.size(): 13
PIR step 1 in the client data,                    run time is 1029 ms
leaf_vec_cipher.size() = 5
CDTE            1024   col data ,         overall run time is 6634 ms
PIR step 2 in the client data,                    run time is 6634 ms
******************************* step 4: client end   *******************************
******************************* step 5: server start *******************************
decrypt the result ,                              run time is 9.377ms
the compare result : 0 1
may be the depth_need_min is too small, need add the extra number. 
compare with the real result ,                    run time is 0 ms
************************************************************************************
address_tree : ../data/heart_16bits/model.json
n            : 16
PIR step 1      2048   col data ,                 run time is 1029 ms
PIR step 2      2048   col data ,                 run time is 6634 ms
PIRCDTE         2048   col data ,         overall run time is 7664 ms
PIRCDTE         2048   col data ,         overall commun.  is 104094 KB
******************************* step 5: server end   *******************************
all done,  the overall run time is 10552 ms
lhq@ubuntu22:~/playground/BCDTE/build$ ./pir_cdcmp_cdte -t ../data/heart_16bits/model.json -v ../data/heart_16bits/x_test.csv -n 16 -d 3 -e 5 -r 2048 -q 1031
******************************* step 1: server begin *******************************
Init fhe ... 
depth_need_min 12
Init fhe finish ... 
Plaintext matrix num_cmps:         1024
Init the BFV batch scheme,                        run time is 1958 ms
load the tree
encrypt the tree ..
tree_depth : 3
encrypt the tree done,                            run time is 611 ms
******************************* step 1: server end   *******************************
******************************* step 2: server begin *******************************
query_first_index_max: 2 query_second_index_max: 1024
num_cmps: 1024
query_first_index: 1 query_second_index: 7
generate a query done,                            run time is 320 ms
******************************* step 2: server end   *******************************
query_first_index_cipher.size(): 2
******************************* step 3: client begin *******************************
data_n    = 13
**    = 2048
load the client_data,                             run time is 24 ms
num_cmps: 1024
2 1024 13
Init the client data,                             run time is 9 ms
******************************* step 3: client end   *******************************
******************************* step 4: client begin *******************************
2 13 2
client_input_after_query_first.size(): 13
PIR step 1 in the client data,                    run time is 1039 ms
leaf_vec_cipher.size() = 5
CDTE            1024   col data ,         overall run time is 6712 ms
PIR step 2 in the client data,                    run time is 6712 ms
******************************* step 4: client end   *******************************
******************************* step 5: server start *******************************
decrypt the result ,                              run time is 89.654ms
the compare result : 0 2
may be the depth_need_min is too small, need add the extra number. 
compare with the real result ,                    run time is 0 ms
************************************************************************************
address_tree : ../data/heart_16bits/model.json
n            : 16
PIR step 1      2048   col data ,                 run time is 1039 ms
PIR step 2      2048   col data ,                 run time is 6712 ms
PIRCDTE         2048   col data ,         overall run time is 7752 ms
PIRCDTE         2048   col data ,         overall commun.  is 104095 KB
******************************* step 5: server end   *******************************
all done,  the overall run time is 10767 ms