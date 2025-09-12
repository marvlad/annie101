# Basic 
We introduce several introductory examples with the end goal of read the ANNIE's RAWData.

## B1 (binary_oarchive)
We use `boost::archive::binary_oarchive` to serialize it into a binary form inside of `std::stringstream`.

## B2 (Serialize data -> output file)
We learn read and write. The steps are: Serialize data, send the binary bytes into this stream (ofs), which writes them into the file.

## B3 (filtering_stream and gzip_compressor)
It has two examples where we learn `boost::iostreams::filtering_stream` and `boost::iostreams::gzip_compressor()`.
