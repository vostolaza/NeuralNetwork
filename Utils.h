#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

std::vector<std::vector<std::string>> read_record(std::string filename)
{

    // File pointer
    std::fstream fin;

    // Open an existing file
    fin.open(filename, std::ios::in);


    // Read the Data from the file
    // as String Vector
    std::vector<std::vector<std::string>> matrix;
    std::vector<std::string> row;
    std::string line, word, temp;
    int i = 0;

    while (fin >> temp) {

        row.clear();

        // read an entire row and
        // store it in a string variable 'line'
        std::getline(fin, line);

        // used for breaking words
        std::stringstream s(line);

        // read every column data of a row and
        // store it in a string variable, 'word'
        std::cout << line << "\n";
        int j = 0;
        while (std::getline(s, word, ',')) {

            // add all the column data
            // of a row to a vector
            row.push_back(word);
            std::cout << "Pushing column " << ++j << "\n";
        }

        matrix.push_back(row);

        std::cout << "Read line " << ++i << "\n";

    }

    return matrix;
};