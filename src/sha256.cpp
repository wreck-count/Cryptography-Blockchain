#include "sha256.hpp"

string fSHA256(string msg){
  CryptoPP::SHA256 hash;
  CryptoPP::byte digest[ CryptoPP::SHA256::DIGESTSIZE ];

  hash.CalculateDigest( digest, (CryptoPP::byte*) msg.c_str(), msg.length() );

  CryptoPP::HexEncoder encoder;
  std::string output;
  encoder.Attach( new CryptoPP::StringSink( output ) );
  encoder.Put( digest, sizeof(digest) );
  encoder.MessageEnd();
  // std::cout << output << std::endl;  
  return output;
}