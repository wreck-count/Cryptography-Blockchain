#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

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