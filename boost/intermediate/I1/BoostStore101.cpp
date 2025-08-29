// Here we will play with int format=0, which is a binary 
// BoostStore(bool typechecking=true, int format=0), format 0=binary, 1=text, 2=multievent

#include<iostream>
#include "BoostStore.h"

int main(){
        // Instantiate an object from the BoostStore class 
	BoostStore store(true, 1); // 

        // Using the method 'Set' from BoostStore
        // The `Set` is defined as template<typename T> void Set(std::string name,T in)
	store.Set("temperature", 25.5); // Store a double

	// Using 'Get' the values Set before
	double temp;
	store.Get("temperature", temp); // Retrieve it
	std::cout << "Retriving from BoostStore: " << temp << std::endl;

	// Saving the file
	store.Save("data.bin"); // Save to file

	return 0;
}
