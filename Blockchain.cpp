#include "src/des.hpp"
#include "src/sha256.hpp"
#include "src/bigInteger.h"
#include <random>
#include <fstream>
using namespace std;

typedef math::wide_integer::uintwide_t<128U> uint128_t;

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist100(1,100);

const uint128_t lprime("860847302321860279");
const uint128_t generator("128476441339822253");
const int MAX_CAP = 3;

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
void load();
void save();


struct transaction_t{
  string fromAdd, toAdd;
  int amount;
  time_t timestamp;
  transaction_t(){}
  transaction_t(string from, string to, int amount, time_t timestamp){
    fromAdd = from;
    toAdd = to;
    this->amount = amount;
    this->timestamp = timestamp;
  }
  void tak(ifstream &file){
    file.ignore();
    getline(file, fromAdd);
    getline(file, toAdd);
    file>>amount>>timestamp;
    // file>>fromAdd>>toAdd>>amount>>timestamp;
    return;
  }
  void gib(ofstream &file){
    file<<fromAdd<<"\n"<<toAdd<<"\n"<<amount<<"\n"<<timestamp<<"\n";
    return;
  }
};

struct block_t{
  int idx;
  string hash, phash;
  vector<transaction_t>transactions;
  time_t timestamp;
  int nonce;
  block_t(){}
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
    cout<<"> Block "<<idx<<" mined, with seal hash:"<<hash<<'\n';
    return;
  }
  void tak(ifstream &file){
    size_t tsize;
    file.ignore();
    getline(file, hash);
    getline(file, phash);
    file>>idx>>timestamp>>nonce>>tsize;
    // file>>idx>>hash>>phash>>timestamp>>nonce>>tsize;
    transactions.resize(tsize);
    for(auto &transaction : transactions){
      transaction.tak(file);
    }
    return;
  }
  void gib(ofstream &file){
    file<<hash<<"\n"<<phash<<"\n"<<idx<<"\n"<<timestamp<<"\n"<<nonce<<"\n"<<transactions.size()<<"\n";
    for(auto &transaction : transactions)
      transaction.gib(file);
    return;
  }
};

vector<block_t>blockchain;

struct user_t{
    uint64_t public_key, private_key;
    string handle, password;
    vector<transaction_t> transactions;
    user_t(){}
    user_t(string name, string pswd, uint64_t privateKey, uint64_t publicKey){
      private_key = privateKey;
      public_key = publicKey;
      password = pswd;
      handle = name;
    }
    void makePayment(user_t* toAdd, int amount){
      transaction_t newTransaction = transaction_t(handle, toAdd->handle, amount, time(0));
      if(verifyTransaction(newTransaction, this)){
        cout<<endl<<"> -- PAYMENT SUCCESSFUL -- \n"<<endl;
        transactions.push_back(newTransaction);
        toAdd->transactions.push_back(newTransaction);
        blockchain.back().transactions.push_back(newTransaction);
        if(blockchain.back().transactions.size() == MAX_CAP){
          cout<<"> -- Block reached limit, sealing block --\n";
          blockchain.back().mineBlock(1);
          cout<<"> -- Sealed old block, adding new block to chain --\n";
          blockchain.push_back(createBlock());
          cout<<"> -- Added new block -- \n";
        }
      }
      return;
    }
    void tak(ifstream &file){
      size_t tsize;
      file.ignore();
      getline(file, handle);
      getline(file, password);
      file>>private_key>>public_key>>tsize;
      // file>>handle>>password>>private_key>>public_key>>tsize;
      transactions.resize(tsize);
      for(auto &transaction : transactions)
        transaction.tak(file);
      return;
    }
    void gib(ofstream &file){
      file<<handle<<"\n"<<password<<"\n"<<private_key<<"\n"<<public_key<<"\n"<<transactions.size()<<"\n";
      for(auto &transaction : transactions)
        transaction.gib(file);
      return;
    }
    void viewUser(){
      int funds_left = 0;
      for(const block_t& block: blockchain){
        for(const transaction_t& transaction: block.transactions){
          if(handle == transaction.fromAdd)
            funds_left -= transaction.amount;
          else if(handle == transaction.toAdd)
            funds_left += transaction.amount;
        }
      }
      funds_left += 100;//initial funds
      cout<<endl;
      cout<<"-> Account Balance : "<<funds_left<<endl;
      if(transactions.empty()){
        cout<<"-- No transaction history --"<<endl;
      }
      else{
        for(const auto& transaction: transactions){
          cout<<string(50, '-')<<endl;
          cout<<"FROM : "<<transaction.fromAdd<<endl;
          cout<<"TO : "<<transaction.toAdd<<endl;
          cout<<"AMOUNT: "<<transaction.amount<<endl;
          cout<<"TIME : "<<ctime(&transaction.timestamp)<<endl;
          cout<<string(50, '-')<<endl;
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
      cout<<"> !-- This handle already exists. Try a different one --!";
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
    r = (raw*r)%(lprime-1);
    uint128_t h = mpow(generator, r, lprime);
    uint128_t b = (dist100(rng)&1);
    uint128_t s = (r + (b*signature)%(lprime-1))%(lprime-1);
    uint128_t lhs = mpow(generator, s, lprime);
    uint128_t rhs = (h*mpow(publicKey, raw*b, lprime))%lprime;
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
    cout<<"> !-- Inadequate Balance! Transaction terminated --!\n";
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
    cout<<"> !-- Vertification failed, bad integrity. Transaction terminated --!"<<endl;
    return false;
  }
  return true;
}

void load(){
  // uint64_t public_key, private_key;
  // string handle, password;
  // vector<transaction_t> transactions;
  ifstream iu, ib;
  try {
    iu.open("users.txt");
    ib.open("blockchain.txt");
  } catch (const std::exception& e) {
    cerr<<"> !!!-- ummm, something went wrong, file not found --!!!";
    std::cerr << e.what() << '\n';
  }
  size_t szusers, szblocks;
  if(iu >> szusers){
    while(szusers--){
      user_t* user = new user_t();
      user->tak(iu);
      users.push_back(user);
    }
  }
  if(ib >> szblocks){
    while(szblocks--){
      block_t block;
      block.tak(ib);
      blockchain.push_back(block);
    }
  }
  iu.close();
  ib.close();
  return;
}

void save(){
  ofstream ou, ob;
  ou.open("users.txt");
  ob.open("blockchain.txt");
  ou<<users.size()<<"\n";
  for(auto user: users)
    user->gib(ou);
  ob<<blockchain.size()<<"\n";
  for(auto block: blockchain)
    block.gib(ob);
  ou.close();
  ob.close();
  return;
}

int main(){
  cout<<"> Loading.."<<endl;
  load();
  user_t* loggedin_usr = nullptr;
  string cmd;
  if(blockchain.empty()){
    blockchain.push_back(createBlock());
  }
  while(1){
    cout<<endl<<"> Enter command : ";
    cin>>cmd;
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
          cout<<"> !-- The handle to which you want to transfer funds does not exist --!"<<endl;
        }
        else{
          loggedin_usr->makePayment(toAdd, stoi(amount));
        }
      }
      else if(cmd == "view"){
        loggedin_usr->viewUser();
      }
      else if(cmd == "exit"){
        cout<<"> Bye "<<loggedin_usr->handle<<"!"<<endl;
        loggedin_usr = nullptr;
        break;
      }
      else if(cmd == "logout"){
        cout<<"> Bye "<<loggedin_usr->handle<<"!"<<endl;
        loggedin_usr = nullptr;
      }
      else{
        cout<<"> !-- Invalid command --!"<<endl;
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
          cout<<"> !-- Handle does not exist or password does not match --!"<<endl;

        }
        else{
          cout<<"> Welcome "<<handle<<"!"<<endl;
        }
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
          cout<<"> !-- Handle already exists. Try different handle --!"<<endl;
        }
        else{
          cout<<"> -- Account created successfully --"<<endl;
          createUser(handle, password);
        }
      }
      else if(cmd == "exit"){
        break;
      }
      else{
        cout<<"> !-- Invalid command, try logging in --!"<<endl;
      }
    }
  }
  cout<<"> Saving and exiting.."<<endl;
  save();
	return 0;
}