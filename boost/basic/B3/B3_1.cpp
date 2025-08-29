// Here we will learn filtering and compression (check https://www.boost.org/doc/libs/1_41_0/libs/iostreams/doc/index.html?path=2.2.2)
// We will:
// 1.- Open "out.bin" as a binary file.
// 2.- With filtering (let's think as pipeline where you can attach stages (filters)). filtering_stream  acts as a container.
// 3.- Now the pipeline has a compressor stage. Anything written into filter will first go through the gzip compressor.
// In summary: "Hello, this is a simple test..." --> gzip_compressor (filter stage) --> ofs (output stream) --> out.bin (compressed file on disk)

// Filtering capabilities:
// It allows you to chain together various "filters" (e.g., compression, encryption, encoding) 
// that modify the data as it flows through the stream.

#include <iostream> // for I/O operation
#include <sstream>
#include <string>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

int main() {

    // Let's define the output file name
    std::string filename = "out.bin";

    // Now, create an output file stream
    std::ofstream ofs(filename, std::ios::binary);
    // std::ofstream is just like std::cout, but its output goes into a file instead of the console
    
    // Set up the filtering stream for compression
    boost::iostreams::filtering_stream<boost::iostreams::output> filter;
    filter.push(boost::iostreams::gzip_compressor()); // Add gzip compression, but we can add more stuff
    filter.push(ofs); // Add the file as the output sink
                      
    // Write data to the compressed file
    std::string data = "Hello, this is a simple test for gzip compression!";
    filter << data << std::flush; // Write data and flush to ensure it's written

    // Clean up (ofs closes automatically when it goes out of scope)
    std::cout << "Data written to " << filename << " successfully." << std::endl;    
   
    return 0;
}
