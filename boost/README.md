# Boost for ANNIE's ToolAnalysis

The dir `Tools` has the following classes from ANNIE's ToolAnalysis: PointerWrapperBase, BoostStore and SerialisableObject.

# Basic
Covers boost serialization `boost::archive::binary_oarchive`, `boost::iostreams::filtering_stream` and `boost::iostreams::gzip_compressor()`

# Intermediate
Covers `BoostStore store` (store and retrieve) and `BoostStore multiStore(true, 2)`.

# Advanced
Covers ANNIE RAWData first inspections, PMT, MRD and LAPPD decoder.
