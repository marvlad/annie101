# B3, 1:
  Here in B3 1, uses 'boost::iostreams::filtering_stream' as a "container", then boost::iostreams::gzip_compressor()
  to compress the data.

  - An easy way to check this is by uncompressing manually the output out.bin with `gzip -dc out.bin`

# B3, 2:
  Here in B3 2, we combine boost::iostreams::filtering_stream, boost::iostreams::gzip_compressor() and boost::archive::binary_oarchive oa(filter)
  
  - Question: What the gzip -dc out2.bin will return?
