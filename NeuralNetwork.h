#include <vector>
#include <random>
#include "Utils.h"

random_device rd;
default_random_engine eng(rd());
uniform_real_distribution<double> distr(-1, 2);

struct Perceptron {
    std::vector<double> w;
    std::vector<double> a;
    double              b = -1;
    double              value = -1;
    Perceptron() { }
    Perceptron(size_t size) {
        for (size_t i = 0; i < size; i++) {
            double x = distr(eng);
            w.push_back(x);
            //cout << x << " ";
        }
        b = rand() % 255 + 1;
    }

};

class NeuralNetwork {

    public:

    enum Method {
        Sigmoid, Tanh, ReLu
    };

    private:

    typedef std::vector<Record> Dataset;
    typedef std::vector<Perceptron*> Layer;

    Dataset dataset;
    Method method;
    int hiddenLayers = 2;
    int perceptronsPerLayer = 16;
    int finalLayer;
    std::vector<Layer> layers;

    double accumulate(Perceptron* p) {
        double res = 0;
        for (int i = 0; i < p->a.size(); i++)
            res += p->a[i] * p->w[i];
        return res;
    }

    double sigmoid(Perceptron* p) {
        auto res =(double)1/(1 + exp(-accumulate(p)));
        // cout << res << " ";
        return res;
    }

    double relu(Perceptron* p) {
        double res = accumulate(p);
        return res > 0 ? 1 : 0;
    }

    double tanH(Perceptron* p) {
        return 2/(1 + exp(- 2 * accumulate(p))) - 1;
    }

    double eSoftmax(Perceptron* p){
        return exp(accumulate(p));
    }

    std::vector<double> softmax(std::vector<double> values) {
        std::vector<double> res;
        double acum = 0;
        for (const auto &e : values)
            acum += e;
        for (const auto &e : values)
            res.push_back(e/acum);
        return res;
    }

    double activation(Perceptron* p) {
        switch (method){
            case Sigmoid : {
                return sigmoid(p);
            }
            case ReLu : {
                return relu(p);
            }
            case Tanh : {
                return tanH(p);
            }
        }
        return -1;
    }

    double derivativeActivation(Perceptron *p) {
        switch (method){
            case Sigmoid : {
                auto sig = sigmoid(p);
                return sig * (1-sig);
            }
            case ReLu : {
                double res = accumulate(p);
                return res > 0 ? 1 : 0;
            }
            case Tanh : {
                return 1- pow(tanH(p),2);
            }
        }
        return -1;
    }

    public:

    NeuralNetwork(Dataset ds, int finalLayer, Method method){
        this->dataset = ds;
        this->method = method;
        this->finalLayer = finalLayer;
        Layer temp;

        for (int i = 0; i < n_pixeles; i++)
            temp.push_back(new Perceptron());
        layers.push_back(temp);

        size_t prevLayers = n_pixeles;
        for (int i = 0; i < hiddenLayers; i++){
            temp = {};
            for (int j = 0; j < this->perceptronsPerLayer; j++) {
                temp.push_back(new Perceptron(prevLayers));
            }
            layers.push_back(temp);
            prevLayers = perceptronsPerLayer;
        }

        temp = {};
        for (int i = 0; i < finalLayer; i++)
            temp.push_back(new Perceptron((size_t)perceptronsPerLayer));

        layers.push_back(temp);
    }

    std::vector<double> classify(Record record) {
        for (int i = 0; i < n_pixeles; i++){
            layers[0][i]->value = record.pixeles[i];
        }

        std::vector<double> res;

        for (int l = 1; l < hiddenLayers + 2; l++){
            for (const auto &p : layers[l]){
                p->a.clear();
                for (const auto &prev : layers[l-1]) {
                    p->a.push_back(prev->value);
                }
                if (l == hiddenLayers+1){
                    p->value = eSoftmax(p);
                    res.push_back(p->value);
                } else {
                    p->value = activation(p);
                }
            }
        }
        // for (const auto &r : res) {
        //     cout << r << " ";
        // }
        auto ans = softmax(res);
        for (int i = 0; i <finalLayer; i++)
            layers[hiddenLayers+1][i]->value = ans[i];
        return ans;
    };

    // TODO: Backpropagation
    void backpropagation(std::vector<double> classification, int expected, double alpha) {
        vector<double> expectedProb(finalLayer, 0);
        expectedProb[expected] = 1;
        double err = 0;
        for (int i = 0; i < classification.size(); i++)
            err += pow(classification[i] - expectedProb[i],2);
        err /= finalLayer;

        for (int i = hiddenLayers + 1; i > 0; i--){
            for (int j = 0; j < layers[i].size(); j++){
                // Ultima capa
                if (i == hiddenLayers + 1){
                    int desiredValue = 0;
                    if (j == expected) desiredValue = 1;
                    double derivative = -(desiredValue-layers[i][j]->value)*layers[i][j]->value*(1-layers[i][j]->value)*alpha;
                    // cout << "Derivada " << derivative << "  Value: " << layers[i][j]->value << endl;
                    for (int k = 0; k < layers[i][j]->a.size(); k++){
                        layers[i][j]->w[k] -= layers[i][j]->a[k]*derivative;
                    }
                }
                // Todo el resto
                else {
                    auto derivative = derivativeActivation(layers[i][j]);
                    for (auto &w : layers[i][j]->w)
                        w -= derivative*alpha;
                }
            }
        }

    }

    void train() {
        int a = 0;
        vector<double> res;
        int expected = -1;
        for (const auto &record : this->dataset){
            res = classify(record);
            // double acum = 0;
            // for (const double & r : res){
            //     // acum += r;
            //     cout << r << " ";
            // }
            // cout << "\n" << acum << "\n";
            // for (const auto &w : layers[hiddenLayers+1][0]->w)
            //     cout << w << " ";
            // cout << "\n";
            // break;
            cout << "Clasifico!\n";
            expected = record.label;
            backpropagation(res, expected, 0.01);
            cout << "Backpropago! " << a++ << "\n";
            // for (const auto &w : layers[hiddenLayers+1][0]->w)
            //     cout << w << " ";
            // cout << "\n";
        }
        for (const double & r : res){
            cout << r << " ";
        }
        cout << "\n" << expected << "\n";
    };
};
