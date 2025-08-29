// Here we will learn more of Filtering and Serialization

#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <fstream>
#include <string>
#include <iostream>

int main() {
    // Output file
    std::string filename = "out2.bin";
    std::ofstream ofs(filename, std::ios::binary);

    // Filtering stream with compression
    boost::iostreams::filtering_ostream filter;
    filter.push(boost::iostreams::gzip_compressor()); // Add compression
    filter.push(ofs); // File sink

    // Attach Boost archive to the filter instead of directly to ofs
    boost::archive::binary_oarchive oa(filter);

    // Serialize some data
    int x = 423;
    std::string message = "Boost serialization with gzip!";

    oa & x;        // serialize integer
    oa & message;  // serialize string

    std::cout << "Serialized and compressed data written to "
              << filename << " successfully." << std::endl;

    return 0;
}

