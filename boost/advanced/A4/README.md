# ANNIE's **MRD** data from RAWData

With this code we extract the MRD part from the RAWData with all the tools learned in the previous examples. Multiple parts of the this code were shamelessly copy from several parts of the ToolAnalysis

## In linux (example ubuntu)
Install the follorn
```
sudo apt update; sudo apt install git -y; sudo apt install cmake -y; sudo apt install g++ -y
```

For boost in linux
```
wget https://archives.boost.io/release/1.89.0/source/boost_1_89_0.tar.gz
tar xzf boost_1_89_0.tar.gz
cd boost_1_89_0

# Build & install to /usr/local
./bootstrap.sh --with-libraries=all --prefix=/usr/local
./b2 -j$(nproc)
sudo ./b2 install
```

## Mac os
Use `brew`:
```
brew install boost
```
