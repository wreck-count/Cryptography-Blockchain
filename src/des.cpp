#include "des.hpp"

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