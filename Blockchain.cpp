#include<bits/stdc++.h>
#include<cryptopp/cryptlib.h>
#include<cryptopp/sha.h>
#include<cryptopp/hex.h>
#include "bigInteger.h"
using namespace std;
using uint128_t = math::wide_integer::uintwide_t<128U>;

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist100(1,100);

const uint128_t lprime("860847302321860279");
const uint128_t generator("128476441339822253");
const int MAX_CAP = 1;

string DESkey = "99373cbd6702f191";
// string DESkey = "133457799BBCDFF1";
// string DESkey = "AABB09182736CCDD";
int ip[64] = { 58, 50, 42, 34, 26, 18, 10, 2,
               60, 52, 44, 36, 28, 20, 12, 4,
               62, 54, 46, 38, 30, 22, 14, 6,
               64, 56, 48, 40, 32, 24, 16, 8,
               57, 49, 41, 33, 25, 17, 9, 1,
               59, 51, 43, 35, 27, 19, 11, 3,
               61, 53, 45, 37, 29, 21, 13, 5,
               63, 55, 47, 39, 31, 23, 15, 7 };
                                                          
int ebit[48] = { 32, 1, 2, 3, 4, 5, 4, 5,
                  6, 7, 8, 9, 8, 9, 10, 11,
                  12, 13, 12, 13, 14, 15, 16, 17,
                  16, 17, 18, 19, 20, 21, 20, 21,
                  22, 23, 24, 25, 24, 25, 26, 27,
                  28, 29, 28, 29, 30, 31, 32, 1 };

int sbox[8][4][16] = { { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
                          0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
                          4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
                          15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 },
                        { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
                          3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
                          0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
                          13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 },
  
                        { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
                          13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
                          13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
                          1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 },
                        { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
                          13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
                          10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
                          3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 },
                        { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
                          14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
                          4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
                          11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 },
                        { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
                          10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
                          9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
                          4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 },
                        { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
                          13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
                          1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
                          6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 },
                        { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
                          1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
                          7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
                          2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 } };

int p[32] = { 16, 7, 20, 21,
                29, 12, 28, 17,
                1, 15, 23, 26,
                5, 18, 31, 10,
                2, 8, 24, 14,
                32, 27, 3, 9,
                19, 13, 30, 6,
                22, 11, 4, 25 };

int pc1[56] = { 57, 49, 41, 33, 25, 17, 9,
                 1, 58, 50, 42, 34, 26, 18,
                 10, 2, 59, 51, 43, 35, 27,
                 19, 11, 3, 60, 52, 44, 36,
                 63, 55, 47, 39, 31, 23, 15,
                 7, 62, 54, 46, 38, 30, 22,
                 14, 6, 61, 53, 45, 37, 29,
                 21, 13, 5, 28, 20, 12, 4 };

int shifts[16] = { 1, 1, 2, 2,
                  2, 2, 2, 2,
                  1, 2, 2, 2,
                  2, 2, 2, 1 };

int pc2[48] = { 14, 17, 11, 24, 1, 5,
                 3, 28, 15, 6, 21, 10,
                 23, 19, 12, 4, 26, 8,
                 16, 7, 27, 20, 13, 2,
                 41, 52, 31, 37, 47, 55,
                 30, 40, 51, 45, 33, 48,
                 44, 49, 39, 56, 34, 53,
                 46, 42, 50, 36, 29, 32 };
                                                    
int iip[64] = { 40, 8, 48, 16, 56, 24, 64, 32,
               39, 7, 47, 15, 55, 23, 63, 31,
               38, 6, 46, 14, 54, 22, 62, 30,
               37, 5, 45, 13, 53, 21, 61, 29,
               36, 4, 44, 12, 52, 20, 60, 28,
               35, 3, 43, 11, 51, 19, 59, 27,
               34, 2, 42, 10, 50, 18, 58, 26,
               33, 1, 41, 9, 49, 17, 57, 25 };

struct transaction_t;
struct user_t;
struct block_t;

uint64_t mpow(uint128_t base,uint128_t exponent,uint128_t modulus);
uint64_t genRand64();
uint64_t customHash(const string& key);
bool createUser(string username, string password);
block_t createBlock();
bool ZKP(uint128_t signature, uint128_t raw, uint128_t publicKey);
bool verifyTransaction(transaction_t newTransaction, struct user_t* from);

string exor(const string& A, const string& B); 
string f(string R, string K);
string lsh(string s, int sh);
string btoh(const string& bin);
string htob(const string& hex);
int htod(const string& hex);
int btod(const string& bin);
string dtoh(int dec, int n);
string dtob(int dec, int n);
string permute(const string& str, int *parr, int sz);
string DES(string msg);
string fSHA256(string msg);




struct transaction_t{
  string fromAdd, toAdd;
  int amount;
  time_t timestamp;
  transaction_t(string from, string to, int amount, time_t timestamp){
    fromAdd = from;
    toAdd = to;
    this->amount = amount;
    this->timestamp = timestamp;
  }
};

//blockchain.txt
//idx hash phash transactions.size() fromAdd toAdd amount timestamp timestamp nonce idx hash phash transactions.size() fromAdd toAdd amount timestamp timestamp nonce
//users.txt
//public_key private_key handle password transactions.size() [transactions] 

struct block_t{
  int idx;
  string hash, phash;
  vector<transaction_t>transactions;
  time_t timestamp;
  int nonce;
  block_t(string phash, time_t timestamp, int idx){
    this->phash = phash;
    this->timestamp = timestamp;
    this->idx = idx;
    this->nonce = 0;
  }
  void hashify(){
    hash.clear();
    string final_data;
    final_data += phash;
    for(const transaction_t& t: transactions){
      final_data += t.fromAdd;
      final_data += t.toAdd;
      final_data += to_string(t.amount);
    }
    final_data += to_string(timestamp);
    final_data += to_string(nonce);
    string thash = fSHA256(final_data);
    for(int i=0;i<(1<<6);i+=(1<<4))
      hash += DES(thash.substr(i,1<<4));
  }
  void mineBlock(int difficulty){
    string pfw = string(difficulty, '0');
    hashify();
    while(hash.substr(0, difficulty) != pfw){
      ++nonce;
      // int dum;cin>>dum;
      hashify();
    }
    cout<<"Block "<<idx<<" mined, with seal hash:"<<hash<<'\n';
    return;
  }
};

vector<block_t> blockchain;

struct user_t{
    uint64_t public_key, private_key;
    string handle, password;
    vector<transaction_t> transactions;
    user_t(string name, string pswd, uint64_t privateKey, uint64_t publicKey){
      private_key = privateKey;
      public_key = publicKey;
      password = pswd;
      handle = name;
    }
    void makePayment(user_t* toAdd, int amount){
      transaction_t newTransaction = transaction_t(handle, toAdd->handle, amount, time(0));
      if(verifyTransaction(newTransaction, this)){
        cout<<"Payment successful.\n";
        transactions.push_back(newTransaction);
        toAdd->transactions.push_back(newTransaction);
        blockchain.back().transactions.push_back(newTransaction);
        if(blockchain.back().transactions.size() == MAX_CAP){
          cout<<"Block reached limit, sealing block..\n";
          blockchain.back().mineBlock(1);
          cout<<"Sealed old block, adding new block to chain..\n";
          blockchain.push_back(createBlock());
          cout<<"Added new block.\n";
        }
      }
      return;
    }
};

vector<user_t*>users;

uint64_t mpow(uint128_t base,uint128_t exponent,uint128_t modulus){
  if(modulus==1)return 0;
  uint128_t result = 1;
  base=base%modulus;
  while(exponent>0){
    if((exponent%2) == 1)
      result=(base*result)%modulus;
    exponent=exponent>>1;
    base=(base*base)%modulus;
  }
  return uint64_t(result);
}

uint64_t genRand64(){
  uint64_t key = 0;
  for(uint64_t i=0;i<64;++i){
    uint64_t bit = (dist100(rng)&1);
    key += (uint64_t(1)<<i)*(bit);
  }
  return key;
}

uint64_t customHash(const string& key) {
  size_t i = 0;
  uint64_t hash = 0;
  size_t length = key.length();
  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}

bool createUser(string username, string password){
  for(auto user: users){
    //check if the username already exists
    if(user->handle == username){
      cout<<"This username already exists. Try a different one.";
      return false;
    }
  }
  uint64_t privateKey = genRand64();
  uint64_t publicKey = mpow(generator, privateKey, lprime);
  // cout<<privateKey<<' '<<publicKey<<'\n';
  user_t* user = new user_t(username, password, privateKey, publicKey);
  users.push_back(user);
  return true;
} 

block_t createBlock(){
  if(blockchain.empty()){
    //introduce a genesis block with previous hash as "0"
    return block_t("0", time(0), 0);
  }
  return block_t(blockchain.back().hash, time(0), blockchain.size());
}

bool ZKP(uint128_t signature, uint128_t raw, uint128_t publicKey){

  for(int round = 0; round < 100; ++round){
    uint128_t r = genRand64();
    // cout<<"r :"<<r<<'\n';
    r = (raw*r)%(lprime-1);
    // cout<<"r*raw :"<<r<<'\n';
    uint128_t h = mpow(generator, r, lprime);
    // cout<<"h :"<<h<<'\n';
    uint128_t b = (dist100(rng)&1);
    // cout<<"b :"<<b<<'\n';
    uint128_t s = (r + (b*signature)%(lprime-1))%(lprime-1);
    // cout<<"s :"<<s<<'\n';
    uint128_t lhs = mpow(generator, s, lprime);
    // cout<<"lhs :"<<lhs<<' ';
    uint128_t rhs = (h*mpow(publicKey, raw*b, lprime))%lprime;
    // cout<<"rhs :"<<rhs<<'\n';
    // cout<<lhs<<' '<<rhs<<'\n';
    if(lhs != rhs)
      return false;
  }

  return true;
}

bool verifyTransaction(transaction_t newTransaction, user_t* from){ 
  // user_t fromAdd = newTtransaction.fromAdd;
  // user_t toAdd = newTtransaction.toAdd;
  // int amount = newTtransaction.amount;
  //check funds
  int funds_left = 0;
  for(const block_t& block: blockchain){
    for(const transaction_t& transaction: block.transactions){
      if(newTransaction.fromAdd == transaction.fromAdd)
        funds_left -= transaction.amount;
      else if(newTransaction.fromAdd == transaction.toAdd)
        funds_left += transaction.amount;
    }
  }
  funds_left += 100;//initial funds
  if(funds_left < newTransaction.amount){
    cout<<"Inadequate Balance! Transaction terminated.\n";
    return false;
  }
  //get signature of transaction
  //"from" + "to" + "amount" + "privateKey" + "timestamp"
  uint128_t fromAdd = customHash(newTransaction.fromAdd);
  uint128_t toAdd = customHash(newTransaction.toAdd);
  uint128_t timestamp = newTransaction.timestamp;
  uint128_t amount = newTransaction.amount;
  uint128_t raw = fromAdd*toAdd%(lprime-1)*amount%(lprime-1)*timestamp%(lprime-1);
  uint64_t privateKey = from->private_key;
  uint64_t publicKey = from->public_key;
  uint64_t signature = uint64_t(raw*privateKey%(lprime-1));
  // cout<<fromAdd<<'\n'<<toAdd<<'\n'<<timestamp<<'\n'<<amount<<'\n'<<raw<<'\n'<<privateKey<<'\n'<<publicKey<<'\n'<<signature<<'\n';
  bool status = ZKP(signature, raw, publicKey);
  if(!status){
    cout<<"Vertification failed, bad integrity. Transaction terminated.\n";
    return false;
  }
  return true;
}



string permute(const string& str, int *parr, int sz){
  string ret;
  for(int i=0;i<sz;++i)
    ret+=str[parr[i]-1];
  return ret;
}

string dtob(int dec, int n){
  string bin;
  int creep = (1<<1)-1;
  for(int i=0;i<n;++i){
    int val = ((dec>>i)&creep);
    bin += '0' + val;
  }
  reverse(bin.begin(),bin.end());
  return bin;
}

string dtoh(int dec, int n){
  string hex;
  int creep = (1<<4)-1;
  for(int i=0;i<n;++i){
    int val = ((dec>>(i<<2))&creep);
    if(val>=10)hex+='A'+val-10;
    else hex+='0'+val;
  }
  reverse(hex.begin(),hex.end());
  return hex;
} 

int btod(const string& bin){
  if(bin.empty())return 0;
  int dec = 0;
  for(int i=0,j=bin.size()-1;i<bin.size();++i,--j)
    dec +=  (bin[i]-'0')*(1<<j);
  return dec;
}

int htod(const string& hex){
  int dec = 0;
  for(int i = hex.size()-1,j=0;i>=0;--i,++j){
    if(hex[i]>='A')dec+=(hex[i]-'A'+10)*(1<<(j<<2));
    else dec+=(hex[i]-'0')*(1<<(j<<2));
  }
  return dec;
}

string htob(const string& hex){
  string bin;
  for(int i=0;i<hex.size();++i)
    bin+=dtob(htod(string(1,hex[i])),4);
  return bin;
}

string btoh(const string& bin){
  string ret;
  for(int i=0;i<bin.size();i+=4){
    string _4bit = bin.substr(i,4);
    ret+=dtoh(btod(_4bit),1);
  }
  return ret;
}

string lsh(string s, int sh){
  string ret; 
  int n = s.size();
  sh%=n;
  int cur = sh;
  while(1){
    ret+=s[cur];
    cur = (cur + 1)%n;
    if(cur == sh)break;
  }
  return ret;
}

string exor(const string& A, const string& B){
  string ret;
  int n = min(A.size(), B.size());
  for(int i=0;i<n;++i)
    ret+='0'+((A[i]-'0')^(B[i]-'0'));
  return ret;
}

string f(string R, string K){
  R = permute(R, ebit, sizeof(ebit)/sizeof(*ebit));
  K = exor(K, R);
  vector<string>B;
  for(int i=0;i<(int)K.size();i+=6)
    B.push_back(K.substr(i,6));
  string fB;
  for(int i=0;i<(1<<3);++i){
    string row = string(1,B[i][0])+string(1,B[i][5]);
    string col = B[i].substr(1,4);
    fB += dtob(sbox[i][btod(row)][btod(col)], 4);
  }
  return permute(fB, p, sizeof(p)/sizeof(*p));
}

string DES(string s){
  transform(DESkey.begin(), DESkey.end(), DESkey.begin(), ::toupper);
  transform(s.begin(), s.end(), s.begin(), ::toupper);
  string K = htob(DESkey);
  string KP = permute(K, pc1, sizeof(pc1)/sizeof(*pc1));
  string C = KP.substr(0,28);
  string D = KP.substr(28);
  vector<string>kb;
  vector<string>kh;
  for(int i=0;i<(1<<4);++i){
    C = lsh(C, shifts[i]);
    D = lsh(D, shifts[i]);
    string k =  permute(C + D, pc2, sizeof(pc2)/sizeof(*pc2));
    kb.push_back(k);
    kh.push_back(btoh(k));
  }
  s = htob(s);
  string ipm = permute(s, ip, sizeof(ip)/sizeof(*ip));
  string L = ipm.substr(0,32);
  string R = ipm.substr(32,32);
  for(int i=0;i<(1<<4);++i){
    string tR = R;
    R = exor(L, f(R, kb[i]));
    L = tR;
  }
  return btoh(permute(R + L, iip, sizeof(iip)/sizeof(*iip)));
}

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

int main(){
  user_t* loggedin_usr = nullptr;
  string cmd;
  if(blockchain.empty()){
    blockchain.push_back(createBlock());
  }
  while(1){
    cin>>cmd;
    // cout<<cmd;
    if(loggedin_usr){
      if(cmd == "pay"){
        string handle;
        string amount;
        cin>>handle>>amount;

        user_t* toAdd = nullptr;
        for(auto user: users){
          if(user->handle == handle){
            toAdd = user;
            break;
          }
        }
        if(!toAdd){
          cout<<"The handle to which you want to transfer funds does not exist.\n";
        }
        else{
          loggedin_usr->makePayment(toAdd, stoi(amount));
        }
      }
      else if(cmd == "view"){
        if(loggedin_usr->transactions.empty())
          cout<<"no transaction history.\n";
        for(const auto& transaction: loggedin_usr->transactions){
          cout<<"from : "<<transaction.fromAdd<<'\n';
          cout<<"to : "<<transaction.toAdd<<'\n';
          cout<<"amount: "<<transaction.amount<<'\n';
          cout<<"time : "<<transaction.timestamp<<'\n';
          cout<<"\n";
        }
      }
      else if(cmd == "exit"){
        break;
      }
      else if(cmd == "logout"){
        cout<<"Bye "<<loggedin_usr->handle<<", logged out.\n";
        loggedin_usr = nullptr;
      }
      else{
        cout<<"invalid command.\n";
      }
    }
    else{
      if(cmd == "login"){
        string handle,  password;
        cin>>handle>>password;
        for(auto user: users){
          if(user->handle == handle){
            loggedin_usr = user;
            break;
          }
        }
        if(!loggedin_usr or loggedin_usr->password != password){
          loggedin_usr = nullptr;
          cout<<"username does not exist or password does not match.\n";
        }
        cout<<"Welcome "<<handle<<", logged in.\n";
      }
      else if(cmd == "create"){
        string handle, password;
        cin>>handle>>password;
        bool userFound = 0;
        for(user_t* user: users){
          if(handle == user->handle){
            userFound = 1;
            break;
          }
        }
        if(userFound){
          cout<<"Username already exists. Try different handle.\n";
        }
        else{
          cout<<"Account created successfully.\n";
          createUser(handle, password);
        }
      }
      else if(cmd == "exit"){
        break;
      }
      else{
        cout<<"invalid command, try logging in.\n";
      }
    }
  }
  // save();
	return 0;
}