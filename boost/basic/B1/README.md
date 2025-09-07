# binary_oarchive

If you have a variable `int`, for example, you can use `boost::archive::binary_oarchive` to serialize it into a binary form inside of `std::stringstream`. 

The difference between ifstream, ofstream and fstream: https://cplusplus.com/img/iostream.gif is `std::ofstream` (Output File Stream), `std::ifstream` (Input File Stream) and `std::stringstream`. Designed to treat strings as if they were input/output streams, allowing both reading from and writing to a string in memory.

How to run it?
```javascript
g++ ExampleBoost01.cpp -I /opt/homebrew/Cellar/boost/1.89.0/include -L /opt/homebrew/Cellar/boost/1.89.0/lib -lboost_serialization -lboost_iostreams -o B1 
```
