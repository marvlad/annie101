// Here we will learn to serialize using boost's binary_oarchive 
//
// The idea is you have a variable like an int and using boost::archive::binary_oarchive
// we will serialize the value into binary form inside inside of std::stringstream
//
// The difference between ifstream, ofstream and fstream:  https://cplusplus.com/img/iostream.gif
// std::ofstream (Output File Stream), std::ifstream (Input File Stream) and std::stringstream
// Designed to treat strings as if they were input/output streams, allowing both
// reading from and writing to a string in memory

#include <iostream> // for I/O operation
#include <sstream>
#include <string>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>

int main() {

    // let's declare a double
    double pi = 3.14;
    std::cout << "We will serialize pi=" << pi << " in a stringstream using boost's binary_oarchive " << std::endl;
    // from what we learn in stringstream, let's declare a buffer (like a file, but stored in RAM)
    std::stringstream stream;

    // Boost Serialization output archive, writes data in binary format into the given stream
    boost::archive::binary_oarchive oa(stream);

    // From https://www.boost.org/doc/libs/1_89_0/libs/serialization/doc/index.html
    // Serialization: It means the reversible deconstruction of an arbitrary set of C++ data
    // structures to a sequence of bytes. Such a system can be used to reconstitute 
    // an equivalent structure in another program context. 

    // In other words is a process of converting C++ data structures into a sequence 
    // of bytes and, conversely, reconstructing the original data from those bytes

    // Addtionally, "archive" to refer to a specific rendering of this stream of bytes

    // Let's serializes 'pi' into binary form inside stream
    // binary representation of pi is stored inside stream
    oa & pi; // it can be oa << pi
    // oa & pi -> means save pi
    // ia & pi -> will mean load pi

    // As explained we can use 'str' to see inside of stream
    std::cout << "Serialized pi inside of stream: " << stream.str() << std::endl;

    // We can get the string from the buffer
    std::string result = stream.str();

    // Get the size of the serialized value
    std::cout << "Serialized size: " << result.size() << " bytes" << std::endl;
    
    // Let's print the bytes in hex of result
    for (unsigned char c : result) {
        std::cout << std::hex << (int)c << " ";
    }
    std::cout << std::dec << std::endl;

    return 0;
}
