
# Install libcdte.so
cd CDTE
bash installCDTE.sh
cd ..

# build

bash build_cdte.sh
cd build

# bcdte

./cdcmp_cdte -t ../data/heart_11bits/model.json -v ../data/heart_11bits/x_test.csv -r 1024 -n 16 -d 3 -e 2

./cdcmp_cdte -t ../data/breast_11bits/model.json -v ../data/breast_11bits/x_test.csv -r 1024 -n 16 -d 7 -e 2

./cdcmp_cdte -t ../data/spam_11bits/model.json -v ../data/spam_11bits/x_test.csv -r 1024 -n 16 -d 16 -e 2

killed, memory problem.   
./cdcmp_cdte -t ../data/electricity_10bits/model.json -v ../data/electricity_10bits/x_test.csv -r 1024 -n 16 -d 10 -e 3

# new data 

mkdir ../data/heart_16bits 
./new_tree_and_data -i ../data/heart_11bits -o ../data/heart_16bits -n 16 -s 16384 

mkdir ../data/heart_32bits 
./new_tree_and_data -i ../data/heart_11bits -o ../data/heart_32bits -n 32 -s 16384 

mkdir ../data/breast_16bits 
./new_tree_and_data -i ../data/breast_11bits -o ../data/breast_16bits -n 16 -s 16384 

mkdir ../data/breast_32bits 
./new_tree_and_data -i ../data/breast_11bits -o ../data/breast_32bits -n 32 -s 16384 


# cmp_branch

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

