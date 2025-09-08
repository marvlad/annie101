# I1

Here we use BoostStore store(true, 1), where 1 is text (1=text) boost::archive::text_oarchive.
We set a value, retreve and save it. One may think that the values saved in data.bin are already text (test with cat data.bin).

It won't work because, as we learned we use a filter with compressor:
```
boost::iostreams::filtering_stream<boost::iostreams::output> filter;
    filter.push(boost::iostreams::gzip_compressor());
```

This means data.bin is a comppresed file. Double check with `cat data.bin`. 

If you want to check use `source check.sh` or `gzip -dc data.bin`
