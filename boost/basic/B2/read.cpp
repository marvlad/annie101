// Here we will learn how to read a sample in 'archive.dat' 

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
#include <iostream>

int main() {
    // Using std::ifstream to read
    // 'if' stand for "input file" and stream is for a sequence of data
    // in std::ifstream, the first argument if the file and the second is
    // the type of file, in this case is binary file 
    std::ifstream ifs("./archive.dat", std::ios::binary);

    // ia binary_iarchive for reading?
    boost::archive::binary_iarchive ia(ifs);

    int y = 0;
    ia & y;  // same as ia >> y

    std::cout << "Read value: " << y << std::endl;

    return 0;
}
