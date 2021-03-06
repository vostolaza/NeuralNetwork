#include <vector>
#include <random>
#include "Utils.h"

random_device rd;
default_random_engine eng(rd());
uniform_real_distribution<double> distr(-1, 2);

struct Perceptron {
    std::vector<double> w;
    std::vector<double> a;
    double              delta = 0;
    double              b = -1;
    double              value = -1;
    Perceptron() { }
    Perceptron(size_t size) {
        for (size_t i = 0; i < size; i++) {
            double x = distr(eng);
            w.push_back(x);
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
    int hiddenLayers = 4;
    int perceptronsPerLayer = 30;
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
            for (auto &p : layers[l]){
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
        auto ans = softmax(res);
        for (int i = 0; i <finalLayer; i++)
            layers[hiddenLayers+1][i]->value = ans[i];
        return ans;
    };

    double backpropagation(std::vector<double> classification, int expected, double alpha) {
        vector<double> expectedProb(finalLayer, 0);
        expectedProb[expected] = 1;
        double err = 0;
        for (int i = 0; i < classification.size(); i++)
            err += (1/2)*pow(expectedProb[i] - classification[i], 2);
        
        cout << "Error: " << err << "\n";
        for (int i = hiddenLayers + 1; i > 0; i--){
            for (int j = 0; j < layers[i].size(); j++){
                // Ultima capa
                int desiredValue = 0;
                if (j == expected) desiredValue = 1;
                if (i == hiddenLayers + 1){
                    double memeDeError = -(desiredValue - layers[i][j]->value);
                    double memeDeActivation = derivativeActivation(layers[i][j]);
                    layers[i][j]->delta = memeDeError*memeDeActivation;
                    for (int k = 0; k < layers[i][j]->a.size(); k++){
                        layers[i][j]->w[k] -= layers[i][j]->a[k]*memeDeError*memeDeActivation*alpha;
                    }
                    layers[i][j]->b -= memeDeError*memeDeActivation*alpha;
                }
                // Todo el resto
                else {
                    double memeDeError = 0;
                    double memeDeActivation = derivativeActivation(layers[i][j]);
                    for (int k = 0; k < layers[i+1].size(); k++) {
                        memeDeError += layers[i][k]->delta * layers[i][j]->w[k];
                    }
                    for (int k = 0; k < layers[i][j]->a.size(); k++){
                        layers[i][j]->w[k] -= layers[i][j]->a[k]*memeDeError*memeDeActivation*alpha;
                    }
                    layers[i][j]->delta = memeDeError * memeDeActivation;
                }
            }
        }

        return err;
    }

    void train() {
        int a = 0;
        vector<double> res;
        vector<double> err;
        int expected = -1;
        for (int i = 0; i < 1; i++){
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
                // cout << "Clasifico!\n";
                expected = record.label;
                int accCount = 0;
                err.push_back(backpropagation(res, expected, 0.05));
                // backpropagation(res, expected, 0.05);
                // cout << "Backpropago! " << a++ << "\n";
                // for (const auto &w : layers[hiddenLayers+1][0]->w)
                //     cout << w << " ";
                // cout << "\n";
            }
        }

        // for (const double & r : res){
        //     cout << r << " ";
        // }
        // cout << "\n" << expected << "\n";
        // for (const double & e : err){
        //     cout << e << " ";
        // }

    };

    double getAccuracy(Dataset ds) {
        this->dataset = ds;
        int accCount = 0;
        for (const auto &record : dataset) {
            auto classification = classify(record);
            auto m = max_element(classification.begin(), classification.end());
            if (distance(classification.begin(), m) == record.label) {
                cout << "Hit on label " << record.label << endl;
                accCount++;
            }
        }
        cout << "\nHits: " << accCount << endl;
        return (double)accCount/dataset.size();
    }
};
