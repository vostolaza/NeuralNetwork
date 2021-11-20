#include "NeuralNetwork.h"

int main(){
    vector<Record> records = read_file("sign_mnist_train.csv");
    cout << "parseado \n";
    NeuralNetwork nw = NeuralNetwork(records, 24, NeuralNetwork::ReLu);
    nw.train();
}