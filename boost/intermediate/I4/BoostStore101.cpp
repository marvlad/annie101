#include<iostream>
#include "BoostStore.h"

// ../I1/data.bin
// ../I2/data.bin
// ../I3/events.bin.data


void inspectStore(BoostStore& store){
	store.Print(true);

	//std::cout << "\nKeys in m_variables:\n";

	//for (const auto& entry : store.m_variables){
        //    std::cout << "Hola" << std::endl;
	//}

	/*
	for (const auto& entry : store.m_variables) {
		std::string key = entry.first;
	        std::cout << "Key: " << key;

		if (store.m_typechecking && store.m_type_info.count(key)) {
		    std::string type_id = store.m_type_info[key];
		    std::cout << " (Type: " << type_id << ") - ";

		    // Attempt to deserialize based on common type IDs
		    if (type_id == typeid(int).name()) {
			int value;
			if (store.Get(key, value)) {
			    std::cout << "Value: " << value << " (int)";
			}
		    } else if (type_id == typeid(double).name()) {
			double value;
			if (store.Get(key, value)) {
			    std::cout << "Value: " << value << " (double)";
			}
		    } else if (type_id == typeid(bool).name()) {
			bool value;
			if (store.Get(key, value)) {
			    std::cout << "Value: " << (value ? "true" : "false") << " (bool)";
			}
		    } else if (type_id == typeid(std::string).name()) {
			std::string value;
			if (store.Get(key, value)) {
			    std::cout << "Value: \"" << value << "\" (string)";
			}
		    } else {
			std::cout << "Value: <unknown type, serialized: " << entry.second << ">";
		    }
		} else {
		    std::cout << " - No type info, raw serialized value: " << entry.second;
		}
		std::cout << "\n";
	}
	*/

}

// NOTE: --> BoostStore(bool typechecking=true, int format=0), format 0=binary, 1=text, 2=multievent
int main(){

	// Reading the first output file (type binary and format text)
	// ------------------------------------------------------------------------
	BoostStore store(true, 1); // Binary format with type checking assumed
	std::string path="../I1/data.bin";

        // bool Initialise(std::string filename, int type=0);  file type, 0=boost archive, 1 = ASCII config file
        // In our case is 0, do not get confuse with 'format'
        if (store.Initialise(path, 0)) { // Type 0 for binary input
           std::cout << "Successfully loaded " << path << std::endl;
           inspectStore(store);
    	} else {
           std::cout << "Failed to load " << path << std::endl;
    	}
	
        double temp;
        store.Get("temperature", temp); // Retrieve it
        std::cout << "Retriving from 'temperature': " << temp << std::endl;
	std::cout << ">--------------- end of example 1 -------------------< " <<std::endl;

	// Reading the second output file (type binary and format text)
	// ------------------------------------------------------------------------
	BoostStore store2(true, 0);
	std::string path2="../I2/data.bin";
        if (store2.Initialise(path2, 0)) { // Type 0 for binary input
           std::cout << "Successfully loaded " << path2 << std::endl;
           inspectStore(store2);
    	} else {
           std::cout << "Failed to load " << path2 << std::endl;
    	}
	double temp2;
        store2.Get("energy", temp2);
        std::cout << "Retriving from 'energy': " << temp2 << std::endl;
	std::cout << ">--------------- end of example 2 -------------------< " <<std::endl;
	
	// Reading the second output file (type binary and format text)
	// ------------------------------------------------------------------------
	BoostStore store3(false, 2);
	std::string path3 = "../I3/events.bin.data";
	//bool code = store3.Initialise(path3,0);
        
        if (store3.Initialise(path3,0)) { // Type 0 for binary input
           std::cout << "Successfully loaded " << path3 << std::endl;
           inspectStore(store3);
    	} else {
           std::cout << "Failed to load " << path3 << std::endl;
    	}
        
	//double temp2;
        //store3.Get("energy", temp2);
        //std::cout << "Retriving from 'energy': " << temp2 << std::endl;
	std::cout << ">--------------- end of example 3 -------------------< " <<std::endl;
	
	
	return 0;
}
