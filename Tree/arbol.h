#include "nodo.h"

template<class T>
class arbol{
  public:
  nodo<T>* root;
  arbol(){
    root=0;
  }
  void insert(T dato){
    nodo<T>** i=&root;
    while((*i)){
      i=&((*i)->N[!(dato<(*i)->dato)]);
    }
    (*i)=new nodo<T>(dato);
  }
  bool find(T dato,nodo<T>**& i){
    for (i=&root;*i;i=&((*i)->N[!(dato<(*i)->dato)])){
      if ((*i)->dato==dato)
        return true;
    }
    return false;
  }
  void rmin(nodo<T>**& n){
    if(!((*n)->N[0]))
      return ;
    n=&((*n)->N[0]);
    rmin(n);
  }
  void remove(T dato){
    nodo<T>** i;
    if(find(dato,i)){
      if(!((*i)->N[1])){
        if((*i)->N[0])
          *i=(*i)->N[1];
        else
          *i=NULL;
      } else {
        nodo<T>** rMin=&((*i)->N[1]);
        rmin(rMin);
        (*rMin)->N[0]=(*i)->N[0];
        (*i)=(*i)->N[1];
      }
    }
  }
  void convertToList(){
    nodo<T>**i =&root;
    while ((*i)->N[0] || (*i)->N[1]){
      nodo<T>** iMin=i;
      rmin(iMin);
      (*iMin)->N[0]=(*i)->N[1];
      (*i)->N[1]=0;
      i=&((*i)->N[0]);
    }
  }
};

template<class T>
void dfs(nodo<T>*i){
  if (i){
    cout << i->dato << " ";
    dfs(i->N[0]);
    dfs(i->N[1]);
  }
}
