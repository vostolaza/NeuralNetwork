#include <vector>
#include <random>


struct Perceptron {
    std::vector<double> w {};
    std::vector<double> a {};
    double              b = -1;
    double              value = -1;

    Perceptron() {}
    Perceptron(double v): value(v) {}
    Perceptron(size_t size) {
        for (size_t i = 0; i < size; i++)
            w.push_back(rand() % 255 + 1);
        b = rand() % 255 + 1;
    }
};

class NeuralNetwork {
    typedef std::vector<std::vector<double>> Dataset;
    typedef std::vector<Perceptron*> Layer;

    enum Method {
        Sigmoid, Tanh, ReLu
    };


    Dataset dataset;
    std::vector<int> target;
    Method method;
    int hiddenLayers = 2;
    int perceptronsPerLayer = 16;
    std::vector<Layer> layers;

    double sigmoid(Perceptron* p) {
        double res = 0;
        for (int i = 0; i < p->a.size(); i++) {
            res += 1/(1+ exp(-p->a[i] * p->w[i]));
        }
        return res;
    }

    double relu(Perceptron* p) {
        double res = 0;
        for (int i = 0; i < p->a.size(); i++)
            res += p->a[i] * p->w[i];
        return res > 0 ? res : 0;
    }

    double tanH(Perceptron* p) {
        double res = 0;
        for (int i = 0; i < p->a.size(); i++) {
            res += 2/(1+ exp(-2*(p->a[i] * p->w[i]))) -1;
        }
        return res;
    }

    double eSoftmax(Perceptron* p){
        double res = 0;
        for (int i = 0; i < p->a.size(); i++)
            res += p->a[i] * p->w[i];
        return exp(res);
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

public:
    NeuralNetwork(Dataset ds, std::vector<int> target, int finalLayer, Method method){
        this->dataset = ds;
        this->target = target;
        this->method = method;
        Layer temp;

        for (int i = 0; i < ds[0].size(); i++)
            temp.emplace_back(Perceptron());
        layers.push_back(temp);

        size_t prevLayers = ds[0].size();
        for (int i = 0; i < hiddenLayers; i++){
            temp = {};
            for (int j = 0; j < this->perceptronsPerLayer; j++)
                temp.emplace_back(Perceptron(prevLayers));
            layers.push_back(temp);
            prevLayers = perceptronsPerLayer;
        }

        temp = {};
        for (int i = 0; i < finalLayer; i++)
            temp.emplace_back(Perceptron((size_t)perceptronsPerLayer));
    }

    std::vector<double> classify(std::vector<double> row) {
        for (int i = 0; i < row.size(); i++){
            layers[0][i]->value = row[i];
        }

        std::vector<double> res;

        for (int i = 1; i < hiddenLayers + 2; i++){
            for (const auto &p : layers[i]){

                for (const auto &prev : layers[i-1])
                    p->a.push_back(prev->value);


                if (i == hiddenLayers){
                    p->value = eSoftmax(p);
                    res.push_back(p->value);
                }
                else
                    switch (method){
                        case Sigmoid : {
                            p->value = sigmoid(p);
                            break;
                        }
                        case ReLu : {
                            p->value = relu(p);
                            break;
                        }
                        case Tanh : {
                            p->value = tanH(p);
                            break;
                        }
                        default: break;
                    }
            }
        }

        return softmax(res);
    };

    // TODO: Backpropagation
    void backpropagation(std::vector<double> classification, int expected) {

    }

    void train(int epochs) {
        for (int i = 0; i < epochs; i++){
            int t = 0;
            for (const auto &row : this->dataset){
                auto res = classify(row);
                backpropagation(res, this->target[t]);
                t++;
            }
        }
    };
};



[0.4, 0.1, 0.5]
[0,0,1]