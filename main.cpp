#include "NeuralNetwork.h"

int main(){
    vector<Record> records = read_file("sign_mnist_train.csv");
    cout << "parseado \n";
    NeuralNetwork nw = NeuralNetwork(records, 24, NeuralNetwork::Sigmoid);
    nw.train();
    cout << "\n Accuracy: " << nw.getAccuracy(read_file("sign_mnist_test.csv"));
}