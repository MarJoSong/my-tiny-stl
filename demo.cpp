#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>


using std::cout;
using std::endl;


template <typename T>
void f(T& a) {
    cout << typeid(T).name() << endl; 
    cout << typeid(a).name() << endl << endl; 
}

int main(int argc, char* argv[])
{
  int num = 10;
  const int c_num = 110;
  int *p_num = &num;
  int * const i_c_num = &num;
  const int * c_i_num = &num;
  int& r_num = num;
  const int& cr_num = num;

  cout << typeid(cr_num).name() << endl << endl; 
  cout << ""

  f(num);
  f(c_num);
  f(p_num);
  f(i_c_num);
  f(c_i_num);
  f(r_num);
  f(cr_num);
  return 0;
}