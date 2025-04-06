
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
