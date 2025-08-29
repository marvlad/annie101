// From https://cplusplus.com/reference/sstream/stringstream/
// Is a class, its object uses a 'string buffer' that contains a sequence of characters, 
// which can be accessed directly as a string object, using member 'str'.
// 
// Example: 
//    std::stringstream mybuffer; // Create an empty string buffer
//    buffer << "Hello"; // Add "Hello" to the buffer
//    buffer << " World"; // Append " World"
//    
//    //to get the values 'str' comes
//    std::string myresult = buffer.str();
//    std::cout << result << std::endl;

#include <iostream>
#include <sstream>
#include <string>

int main(){
	// Create a buffer
	std::stringstream ss; // or std::stringstream ss2("Initial string");

	// Ading and printing 
	ss << "Hello ANNIE, " << 127 << " world!";
	std::cout << ss.str() << std::endl; // to read we use the 'str'

	// Read a multiple data formats
	std::stringstream ss2("127 3.1415 LAPPD_Pi");

        // declare the type of variables
	int i;
	double d;
	std::string s;

	// get give the format to what we have in ss2
	ss2 >> i >> d >> s;

	// Printing out the variable values
	std::cout << "Int: " << i << ", Double: " << d << ", String: " << s << std::endl;

	// Clear ss and ss2 buffers
	std::cout << "Before clear (ss): " << ss.str() << std::endl;
	ss.str(""); // Clears the content
    	std::cout << "After clear: " << ss.str() << std::endl;

	std::cout << "Before clear (ss2): " << ss2.str() << std::endl;
	ss2.str(""); // Clears the content
    	std::cout << "After clear: " << ss2.str() << std::endl;

	std::cout << "done" << std::endl;
	return 0;
}
