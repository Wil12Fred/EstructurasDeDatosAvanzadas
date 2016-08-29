template<class T>
class nodo{
  public:
    int dato;
    int altura;
    nodo<T>* N[2];
    nodo(T d){
      dato=d;
      N[0]=N[1]=0;
    }
};
