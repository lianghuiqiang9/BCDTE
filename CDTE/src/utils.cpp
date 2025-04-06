#include "utils.h"

using namespace std;

void swap(vector<int>& v, int i, int j)
{
    int tmp = v[i];
    v[i] = v[j];
    v[j] = tmp;
}

int BinaryVectorTointeger(const std::vector<uint64_t>& bits)
{
    int pow2 = 1;
    uint64_t res = 0;
    for (int i = 0; i < bits.size(); i++){
        res += bits[i] * pow2;
        pow2 <<=1;
    }
    return res;
}

std::vector<uint64_t> IntegerToBinaryVector(int num, int num_bits) {
    std::vector<uint64_t> binaryVector(num_bits);
        for(int i = 0; i < num_bits; i++){
        binaryVector[i] = num & 1;
        num >>= 1;
    }
    return binaryVector;
}

int bitLength(int num) {
    int length = 0;
    while (num > 0) {
        num >>= 1; 
        length++;
    }
    return length;
}

void print_data(std::vector<std::vector<uint64_t>> data){
        for (const auto& row : data) {
        for (const auto& cell : row) {
            std::cout << cell << ",";
        }
        std::cout << std::endl;
    }
}
std::vector<std::vector<uint64_t>> Transpose(std::vector<std::vector<uint64_t>> A){
    vector<vector<uint64_t>> B;
    for(int i=0;i<A[0].size();i++){
        vector<uint64_t> temp(A.size(), 0ULL);
        for(int j=0;j<A.size();j++){
            temp[j]=A[j][i];
        }
        B.push_back(temp);
    }
    return B;
}

std::vector<std::vector<uint64_t>> Matrix_padding(std::vector<std::vector<uint64_t>> A, int slot_count){
    vector<vector<uint64_t>> B;
    for(int i=0;i<A.size();i++){
        vector<uint64_t> temp(slot_count, 0ULL);
        for(int j=0;j<A[0].size();j++){
            temp[j]=A[i][j];
        }
        for(int j=A[0].size();j<slot_count ;j++){
            temp[j] = 0ULL;
        }
        B.push_back(temp);
    }
    return B;
}


std::vector<uint64_t> random_permutation(int n)
{
	int x;
	std::vector<uint64_t> a(n);
	if (n <= 0) { cout<<"random_permutation error"<<endl; exit(-1); }
	for (int i = 0;i < n;i++)
	{
		x = rand() % (n);
		for (int j = 0;j < i;j++)
		{
			if (x == a[j])
			{
				x = rand() % (n);
				j = -1;
			}
		}
		a[i] = x;
	}
    return a;
}


uint64_t exgcd(uint64_t a,uint64_t b,uint64_t &x,uint64_t &y)
{
	if(b==0)
	{
		x=1,y=0;
		return a;
	}
	uint64_t ret=exgcd(b,a%b,y,x);
	y-=a/b*x;
	return ret;
}
uint64_t getInv(uint64_t a,uint64_t mod)
{
	uint64_t x,y;
	uint64_t d=exgcd(a,mod,x,y);
	return d==1?(x%mod+mod)%mod:-1;
}
uint64_t factorial(uint64_t n) {

    if(n==1)
        return 1;
    return n * factorial(n-1);
}

/*
    return a ^ e mod n
*/
uint64_t mod_exp(uint64_t a, uint64_t e, uint64_t n)
{
    if (e == 0)
        return 1;

    long k = ceil(log2(e));
    uint64_t res;
    res = 1;

    for (long i = k - 1; i >= 0; i--) {
        res = (res * res) % n;
        if ((e / (uint64_t)(pow(2, i))) % 2 == 1)
            res = (res * a) % n;
    }
    return res;
}

uint64_t prime_mod_inverse(uint64_t a, uint64_t n)
{
    return mod_exp(a, n - 2, n);
}

uint64_t init_the_d_factorial_inv_with_sign(uint64_t d, uint64_t mod){
    // \theta_EQzero = 1 / d! \prod ^{d}_{i = 1) (i - x )
    // i = 1...d, \theta_EQzero = 0; i = 0, \theta_EQzero = 1;    
    uint64_t d_factorial_inv = prime_mod_inverse(factorial(d),mod);
    uint64_t d_factorial_inv_with_sign = (d % 2) ? mod - d_factorial_inv : d_factorial_inv;
    return d_factorial_inv_with_sign;
}

std::vector<std::vector<std::vector<uint64_t>>> splitVectorIntoChunks(const std::vector<std::vector<uint64_t>>& data, uint64_t chunkSize) {
    std::vector<std::vector<std::vector<uint64_t>>> chunks;
    uint64_t numRows = data.size();

    for (uint64_t i = 0; i < numRows; i += chunkSize) {
        std::vector<std::vector<uint64_t>> chunk;
        for (uint64_t j = i; j < i + chunkSize && j < numRows; ++j) {
            chunk.push_back(data[j]);
        }
        chunks.push_back(chunk);
    }

    return chunks;
}
