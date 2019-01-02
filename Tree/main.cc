#include <iostream>
using namespace std;
#include "arbol.h"

int main(){
  arbol<int> A;
  for (int i=6;i<=12;i+=2){
    A.insert(i);
    dfs(A.root);
    cout << endl;
  }
  for (int i=1;i<=15;i+=3){
    A.insert(i);
    dfs(A.root);
    cout << endl;
  }
  A.convertToList();
  dfs(A.root);
  cout << endl;
}
