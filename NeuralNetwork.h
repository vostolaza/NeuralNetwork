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
    std::vector<Layer> layers;

    double accumulate(Perceptron* p) {
        double res = 0;
        // cout << p->a.size() << " " << p->w.size() << "\n";
        for (int i = 0; i < p->a.size(); i++) {
            res += p->a[i] * p->w[i];
            // cout << res << " ";
        }
        //cout << endl;
        // cout << res << " ";
        return res;
    }

    double sigmoid(Perceptron* p) {
        //cout << exp(-accumulate(p)) << " "<< accumulate(p) << endl;
        double res = (double)1/(1 + exp(-accumulate(p)));
        //cout << res << " ";
        return res;
    }

    double relu(Perceptron* p) {
        double res = accumulate(p);
        return res > 0 ? res : 0;
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
                double ans = sigmoid(p);
                //cout << ans << " ";
                return ans;
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

    public:

    NeuralNetwork(Dataset ds, int finalLayer, Method method){
        this->dataset = ds;
        this->method = method;
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

    void testing() {
        cout << layers[1][0]->a.size() << " " << layers[1][0]->w.size() << "\n";
    }

    std::vector<double> classify(Record record) {
        for (int i = 0; i < n_pixeles; i++){
            layers[0][i]->value = record.pixeles[i];
        }

        std::vector<double> res;

        for (int l = 1; l < hiddenLayers + 2; l++){
            cout << "Layer: " << l << "\n";
            for (const auto &p : layers[l]){
                for (const auto &prev : layers[l-1]) {
                    p->a.push_back(prev->value);
                }
                if (l == hiddenLayers+1){
                    // sale nan
                    p->value = eSoftmax(p);
                    res.push_back(p->value);
                } else {
                    p->value = activation(p);   
                }
            }
        }
        // return res;
        
        return softmax(res);
    };

    // TODO: Backpropagation
    void backpropagation(std::vector<double> classification, int expected) {

    }

    void train(int epochs) {
        for (int i = 0; i < epochs; i++){
            int a = 0;
            for (const auto &record : this->dataset){
                auto res = classify(record);
                double acum = 0;
                for (const auto &r : res) {
                    cout << r << " ";
                    acum += r;
                }
                cout << "\n" << acum;
                break;
                // for (const double & r : res)
                //     cout << r << " ";
                //cout << a++ << " ";
                //cout << endl; 
                //backpropagation(res, record.label);
            }
        }
    };
};
