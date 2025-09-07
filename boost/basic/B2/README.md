# To run the example
# g++ ExampleBoost02.cpp -I /opt/homebrew/Cellar/boost/1.89.0/include -L /opt/homebrew/Cellar/boost/1.89.0/lib -lboost_serialization -lboost_iostreams -o B2

# Read and write

With the ofstream we create an 'archive.dat'. In std::ofstream, of stands for 'output file' and the stream is for a sequence of data. The first argument is for the file and the second for the type of the file, in this case a binary `std::ofstream ofs("archive.dat", std::ios::binary);`

The Boost.Serialization has a class boost::archive::binary_oarchive facilitates the serialization of data into a binary format. In boost, archive is a concept representing a mechanism for 'storing' or 'retrieving' serialized data In this context 'o' in binary_oarchive is output. So, boost::archive::binary_oarchive is an output archive used for serializing data. The line below will serialize  into binary form inside stream Boost Serialization output archive, writes data in binary format into the given stream (std::ofstream) `boost::archive::binary_oarchive oa(ofs);`
```
int x = 423;
oa & x;  // serialized into file "archive.dat"
```
x (int = 423) 
    ---> oa (serializes into binary format) 
    ---> ofs (writes bytes) 
    ---> archive.dat (stores raw data)

  oa does the serialization
  fs does the writing to file

The order is as follows:
    - ofs = opens the file → a “pipe” to archive.dat.
    - oa = constructed, and attached to ofs
    Serialization happens when you do 
    oa & x;

- Serialization happens inside binary_oarchive when you call oa & x;
- Saving happens immediately after, because oa is writing into ofs, and ofs is connected to archive.dat.

Think this way: When you serialize data, send the binary bytes into this stream (ofs), which writes them into the file
    
`std::ofstream ofs("archive.dat", std::ios::binary);`
    --> opens the file archive.dat in binary mode, ready to accept bytes. (ofs = the pipe).

`oost::archive::binary_oarchive oa(ofs);`
    --> builds a serializer machine (oa) that is wired directly to the pipe (ofs). It doesn’t store things itself — it just transforms objects into binary and hands them off to the stream.

`a & x;`
    --> takes x, serializes it, and immediately pushes the result through ofs into archive.dat. Nothing is buffered inside oa for later; it goes straight to the file as soon as you call it.

`x` (C++ object) --> oa (binary_oarchive = serializer machine) --> ofs (output file stream = pipe to disk) --> "archive.dat" (file with serialized bytes)  


How to run it? Use `compile.sh` script. `chmod +x compile.sh`
```javascript
./compile.sh read.cpp 
```

```javascript
./compile.sh write.cpp 
```
