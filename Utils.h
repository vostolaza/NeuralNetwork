#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

const int n_pixeles = 784;
const int max_val = 255;

struct Record {
    int row;
    int label;
    double pixeles [n_pixeles];
    void print() {
        cout << "row: " << row << " "; //<< "\n";
        cout << "label: " << label << "\n";
        //for (int i = 0; i < n_pixeles; i++)
          //  cout << pixeles[i] << ",";
    }
};

double normalize (int num) {
    return num/(double)max_val;
}

Record parse_record(string line, int row) {

    Record record;
    record.row = row;

    stringstream ssLine(line);
    string cur;

    getline(ssLine, cur, ',');
    record.label = stoi(cur);

    for (int i = 0; i < n_pixeles; i++) {
        getline(ssLine, cur, ',');
        record.pixeles[i] = normalize(stoi(cur));
    }

    return record;
};


vector <Record> read_file(string filename){

    vector<Record> records;
    // File pointer
    fstream file;

    // Open an existing file
    file.open(filename, ios::in);

    string line;
    getline (file, line);

    int i = 0;
    while (getline (file, line))
        records.push_back(parse_record(line, i++));

    return records;
}