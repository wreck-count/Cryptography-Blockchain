clear
echo "Compiling dependencies.. patience!"
g++ -c src/des.cpp -o src/des.o
g++ -c src/sha256.cpp -o src/sha256.o
echo "Dependencies compiled, ready for use!"
echo "Compiling source.. patience!"
g++ -c Blockchain.cpp -o Blockchain.o
echo "Linking and creating an executable.. final step!!"
g++ Blockchain.o src/des.o src/sha256.o -o Blockchain /usr/local/lib/libcryptopp.a 
echo "All ready to run!!"

