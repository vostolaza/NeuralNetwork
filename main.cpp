#include "Utils.h"

int main(){

    auto m = read_record("sign_mnist_test.csv");

    // std::cout << m[0][0] << " " << m[0][1] << " " << m[0][2];
    std::cout << m[0].size();
}