#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include <deque>
#include <fstream>

using namespace std;
typedef long double ld;
typedef pair<ld,ld> pld;



struct Rectangle
{
    vector<ld> m_pmin, m_pmax;
    Rectangle(vector<ld> pmin, vector<ld> pmax)
    {
        m_pmin = pmin;
        m_pmax = pmax;
    }
    ld getArea()
    {
        ld area = 1;
        for(int i = 0; i < m_pmin.size(); i++)
        {
            area*=(m_pmax[i]-m_pmin[i]);
        }
        return area;
    }
    void print()
    {
        cout<<"(";
        for(int i = 0; i < m_pmin.size(); i++)
        {
            cout<<m_pmin[i]<<",";
        }
        cout<<"),(";
        for(int i = 0; i < m_pmax.size(); i++)
        {
            cout<<m_pmax[i]<<",";
        }
        cout<<")"<<endl;
    }
    ~Rectangle()
    {
       m_pmin.clear();
       m_pmax.clear();
    }
};

struct Node
{
    vector<Rectangle*> m_rectangles;
    vector<Node*> m_children;
    Node* m_ant;
    Rectangle* m_antRectangle;
    int m_M, m_level;
    Node(int M)
    {
        m_ant = 0;
        m_antRectangle = 0;
        m_M = M;
        m_level = 1;
    }
    bool isLeaf()
    {
        for(unsigned int i = 0; i < m_children.size(); i++)
        {
            if(m_children[i]) return false;
        }
        return true;
    }
};

Rectangle* enlargeRectangle(Rectangle* a, Rectangle* b)
{
    Rectangle* new_a = new Rectangle(a->m_pmin,a->m_pmax);
    for(int i = 0; i < b->m_pmin.size(); i++)
    {
        if(b->m_pmin[i] < new_a->m_pmin[i]) new_a->m_pmin[i] = b->m_pmin[i];
        if(b->m_pmax[i] > new_a->m_pmax[i]) new_a->m_pmax[i] = b->m_pmax[i];
    }
    return new_a;
}

bool overlapRectangle(Rectangle* a, Rectangle* b)
{
    for(int i = 0; i < b->m_pmax.size(); i++)
    {
        if(b->m_pmax[i] < a->m_pmin[i]) return false;
        if(b->m_pmin[i] > a->m_pmax[i]) return false;
    }
    return true;

}

ld diffAreas(Rectangle* a, Rectangle* b)
{
    return abs(a->getArea()-b->getArea());
}


Rectangle* getRectangle(vector<Rectangle*>& rectangles)
{
    Rectangle* globalRectangle = rectangles[0];
    for(int i = 1; i < rectangles.size(); i++)
    {
        globalRectangle = enlargeRectangle(globalRectangle,rectangles[i]);
    }
    return globalRectangle;
}

class RTree
{
public:
    Node* m_head;
    int m_M, m_m, m_dim;
public:
    RTree(int M, int m, int dim)
    {
        m_head = 0;
        m_M = M;
        m_m = m;
        m_dim = dim;
    }
    Node* quadraticSplit(Node* &ptr)
    {
        Node* ptrRight = new Node(m_M),*ptrLeft = new Node(m_M);
        ptrRight->m_level = ptrLeft->m_level = ptr->m_level;
        Rectangle* holder, *leftRectangle, *rightRectangle;
        bool flag = 0;
        ld d = -1, holderD, leftArea, rightArea;
        int new_i, new_j, op, rectanglesLeft = ptr->m_rectangles.size();
        for(int i = 0; i < ptr->m_rectangles.size(); i++)
        {
            for(int j = i+1; j < ptr->m_rectangles.size(); j++)
            {
                holder = enlargeRectangle(ptr->m_rectangles[i],ptr->m_rectangles[j]);
                holderD = holder->getArea()-ptr->m_rectangles[i]->getArea()-ptr->m_rectangles[j]->getArea();
                if(d < holderD or !flag)
                {
                    flag = 1;
                    d = holderD;
                    new_i = i;
                    new_j = j;
                }
                delete holder;
                holder = 0;
            }
        }
        ptrLeft->m_rectangles.push_back(ptr->m_rectangles[new_i]);
        ptrLeft->m_children.push_back(ptr->m_children[new_i]);
        if(ptr->m_children[new_i])
        {
            ptr->m_children[new_i]->m_ant = ptrLeft;
            ptr->m_children[new_i]->m_antRectangle = ptrLeft->m_rectangles[0];
        }
        ptrRight->m_rectangles.push_back(ptr->m_rectangles[new_j]);
        ptrRight->m_children.push_back(ptr->m_children[new_j]);
        if(ptr->m_children[new_j])
        {
            ptr->m_children[new_j]->m_ant = ptrRight;
            ptr->m_children[new_j]->m_antRectangle = ptrRight->m_rectangles[0];
        }
        ptr->m_rectangles[new_i] = 0;
        ptr->m_rectangles[new_j] = 0;
        ptr->m_children[new_i] = 0;
        ptr->m_children[new_j] = 0;
        rectanglesLeft -= 2;
        leftRectangle = ptrLeft->m_rectangles[0];
        rightRectangle = ptrRight->m_rectangles[0];
        while(rectanglesLeft > 0)
        {
            d = -1;
            for(int i = 0; i < ptr->m_rectangles.size(); i++)
            {
                if(ptr->m_rectangles[i])
                {
                    holderD = diffAreas(enlargeRectangle(leftRectangle,ptr->m_rectangles[i]),leftRectangle);
                    holderD = abs(holderD - diffAreas(enlargeRectangle(rightRectangle,ptr->m_rectangles[i]),rightRectangle));
                    if(holderD > d )
                    {
                        d = holderD;
                        new_i = i;
                    }
                }
            }
            leftArea = diffAreas(enlargeRectangle(leftRectangle,ptr->m_rectangles[new_i]),leftRectangle);
            rightArea = diffAreas(enlargeRectangle(rightRectangle,ptr->m_rectangles[new_i]),rightRectangle);
            if(leftArea < rightArea) op = 1;
            else if(leftArea > rightArea) op = 2;
            else
            {
                if(leftRectangle->getArea() < rightRectangle->getArea()) op = 1;
                else if(leftRectangle->getArea() > rightRectangle->getArea()) op = 2;
                else
                {
                    if(ptrLeft->m_rectangles.size() < ptrRight->m_rectangles.size()) op = 1;
                    else op = 2;
                }
            }
            if(op == 1)
            {
                ptrLeft->m_rectangles.push_back(ptr->m_rectangles[new_i]);
                ptrLeft->m_children.push_back(ptr->m_children[new_i]);
                if(ptr->m_children[new_i])
                {
                    ptr->m_children[new_i]->m_ant = ptrLeft;
                    ptr->m_children[new_i]->m_antRectangle = ptr->m_rectangles[new_i];
                }
                leftRectangle = enlargeRectangle(leftRectangle,ptr->m_rectangles[new_i]);
            }
            else
            {
                ptrRight->m_rectangles.push_back(ptr->m_rectangles[new_i]);
                ptrRight->m_children.push_back(ptr->m_children[new_i]);
                if(ptr->m_children[new_i])
                {
                    ptr->m_children[new_i]->m_ant = ptrRight;
                    ptr->m_children[new_i]->m_antRectangle = ptr->m_rectangles[new_i];
                }
                rightRectangle = enlargeRectangle(rightRectangle,ptr->m_rectangles[new_i]);
            }
            ptr->m_rectangles[new_i] = 0;
            ptr->m_children[new_i] = 0;
            rectanglesLeft--;
            if(rectanglesLeft + ptrLeft->m_rectangles.size() == m_m) break;
            if(rectanglesLeft + ptrRight->m_rectangles.size() == m_m) break;
        }
        if(ptrLeft->m_rectangles.size() < m_m)
        {
            for(int i = 0; i < ptr->m_rectangles.size(); i++)
            {
                if(ptr->m_rectangles[i])
                {
                    ptrLeft->m_rectangles.push_back(ptr->m_rectangles[i]);
                    ptrLeft->m_children.push_back(ptr->m_children[i]);
                    if(ptr->m_children[i])
                    {
                        ptr->m_children[i]->m_ant = ptrLeft;
                        ptr->m_children[i]->m_antRectangle = ptr->m_rectangles[i];
                    }
                    leftRectangle = enlargeRectangle(leftRectangle,ptr->m_rectangles[i]);
                    ptr->m_rectangles[i] = 0;
                    ptr->m_children[i] = 0;
                }
            }
        }
        if(ptrRight->m_rectangles.size() < m_m)
        {
            for(int i = 0; i < ptr->m_rectangles.size(); i++)
            {
                if(ptr->m_rectangles[i])
                {
                    ptrRight->m_rectangles.push_back(ptr->m_rectangles[i]);
                    ptrRight->m_children.push_back(ptr->m_children[i]);
                    if(ptr->m_children[i])
                    {
                        ptr->m_children[i]->m_ant = ptrRight;
                        ptr->m_children[i]->m_antRectangle = ptr->m_rectangles[i];
                    }
                    rightRectangle = enlargeRectangle(rightRectangle,ptr->m_rectangles[i]);
                    ptr->m_rectangles[i] = 0;
                    ptr->m_children[i] = 0;
                }
            }
        }
        if(!ptr->m_ant)
        {
            m_head = new Node(m_M);
            m_head->m_level = ptr->m_level+1;
            m_head->m_children.push_back(ptrLeft);
            m_head->m_children.push_back(ptrRight);
            m_head->m_rectangles.push_back(leftRectangle);
            m_head->m_rectangles.push_back(rightRectangle);
            ptrLeft->m_ant = m_head;
            ptrRight->m_ant = m_head;
            ptrLeft->m_antRectangle = m_head->m_rectangles[0];
            ptrRight->m_antRectangle = m_head->m_rectangles[1];
        }
        else
        {
            ptr->m_ant->m_children.push_back(ptrRight);
            ptr->m_ant->m_rectangles.push_back(rightRectangle);
            ptrRight->m_ant = ptr->m_ant;
            ptrLeft->m_ant = ptr->m_ant;
            ptrLeft->m_antRectangle = leftRectangle;
            ptrRight->m_antRectangle = ptr->m_ant->m_rectangles.back();
            for(int i = 0; i < ptr->m_ant->m_children.size(); i++)
            {
                if(ptr->m_ant->m_children[i] == ptr)
                {
                    ptr->m_ant->m_children[i] = ptrLeft;
                    ptr->m_ant->m_rectangles[i] = ptrLeft->m_antRectangle;
                    break;
                }
            }
        }
        return ptrLeft;

    }
    inline pair<Rectangle*,int> chooseChild(Node* ptr, Rectangle* a)
    {
        pair<Rectangle*,int> ans = make_pair(enlargeRectangle(ptr->m_rectangles[0],a),0);
        ld diffarea1, diffarea2;
        Rectangle* holder;
        for(int i = 0; i < ptr->m_rectangles.size(); i++)
        {
            holder = enlargeRectangle(ptr->m_rectangles[i],a);
            diffarea1 = diffAreas(holder,ptr->m_rectangles[i]);
            diffarea2 = diffAreas(ans.first,ptr->m_rectangles[ans.second]);
            if(diffarea1 < diffarea2)
            {
                ans.first = holder;
                ans.second = i;
            }
            else if(diffarea1 == diffarea2)
            {
                if(holder->getArea() < ans.first->getArea())
                {
                    ans.first = holder;
                    ans.second = i;
                }
            }
        }
        return ans;
    }
    void Insert(Rectangle* a, int treeLevel, Node* m_child = 0)
    {
        if(!m_head)
        {
            m_head = new Node(m_M);
            m_head->m_rectangles.push_back(a);
            m_head->m_children.push_back(0);
            return;
        }
        pair<Rectangle*,int> minEnlargement;
        Node* ptr = m_head;
        while(ptr->m_level != treeLevel)
        {
            minEnlargement = chooseChild(ptr,a);
            ptr->m_rectangles[minEnlargement.second] = minEnlargement.first;
            ptr = ptr->m_children[minEnlargement.second];
        }
        ptr->m_rectangles.push_back(a);
        ptr->m_children.push_back(m_child);
        if(m_child)
        {
            m_child->m_ant = ptr;
            m_child->m_antRectangle = ptr->m_rectangles.back();
        }
        while(ptr and ptr->m_rectangles.size() > m_M)
        {
            ptr = quadraticSplit(ptr);
            ptr = ptr->m_ant;
        }
        return;
    }
    void Find(Rectangle* a, Node* ptr)
    {
        if(!ptr) return;
        for(int i = 0; i < ptr->m_rectangles.size(); i++)
        {
            if(overlapRectangle(a,ptr->m_rectangles[i]))
            {
                if(ptr->isLeaf()) ptr->m_rectangles[i]->print();
                Find(a,ptr->m_children[i]);
            }
        }
        return;
    }
    void FindLeaf(Rectangle* a, Node* ptr, Node* &leaf)
    {
        if(!ptr) return;
        for(int i = 0; i < ptr->m_rectangles.size(); i++)
        {
            if(overlapRectangle(a,ptr->m_rectangles[i]))
            {
                if(ptr->isLeaf())
                {
                    if(ptr->m_rectangles[i]->m_pmin == a->m_pmin and ptr->m_rectangles[i]->m_pmax == a->m_pmax)
                    {
                        leaf = ptr;
                        return;
                    }
                }
                FindLeaf(a,ptr->m_children[i],leaf);
            }
        }
        return;
    }
    void Print(Node* ptr)
    {
        if(!ptr) return;
        Node* start = ptr;
        deque<Node*> ptrs;
        ptrs.push_back(start);
        while(!ptrs.empty())
        {
            start = ptrs.front();
            cout<<"imprime: "<<start->m_children.size()<<" "<<start->m_rectangles.size()<<endl;
            ptrs.pop_front();
            for(int i = 0; i < start->m_children.size(); i++)
            {
                if(start->m_children[i])
                {
                    ptrs.push_back(start->m_children[i]);
                }
            }
            cout<<start->m_rectangles.size()<<" en nivel: "<<start->m_level<<endl;
            for(int i = 0; i < start->m_rectangles.size(); i++)
            {
                if(start->m_rectangles[i]) start->m_rectangles[i]->print();
            }
        }
    }
    void CondenseTree(Node* &N)
    {
        vector<Node*> Q;
        Node/* * N = L,*/*P;
        while(N != m_head)
        {
            P = N->m_ant;
            if(N->m_children.size() < m_m)
            {
                Q.push_back(N);
                for(int i = 0; i < P->m_children.size(); i++)
                {
                    if(P->m_children[i] == N)
                    {
                        P->m_rectangles.erase(P->m_rectangles.begin()+i);
                        P->m_children.erase(P->m_children.begin()+i);
                        break;
                    }
                }
            }
            else
            {
                for(int i = 0; i < P->m_children.size(); i++)
                {
                    if(P->m_children[i] == N)
                    {
                        P->m_rectangles[i] = getRectangle(N->m_rectangles);
                        N->m_antRectangle = P->m_rectangles[i];
                        break;
                    }
                }
            }
            N = P;

        }
        for(int i = 0; i < Q.size(); i++)
        {
            for(int j = 0; j < Q[i]->m_children.size(); j++)
            {
                Insert(Q[i]->m_rectangles[j],Q[i]->m_level,Q[i]->m_children[j]);
            }
        }
        return;
    }
    void Delete(Rectangle* a)
    {
        Node* leaf = 0;
        int cont = 0, index = 0;;
        FindLeaf(a,m_head,leaf);
        if(!leaf) return;
        for(int i = 0; i < leaf->m_rectangles.size(); i++)
        {
            if(a->m_pmin == leaf->m_rectangles[i]->m_pmin and a->m_pmax == leaf->m_rectangles[i]->m_pmax)
            {
                leaf->m_rectangles.erase(leaf->m_rectangles.begin()+i);
                leaf->m_children.erase(leaf->m_children.begin()+i);
                break;
            }
        }
        CondenseTree(leaf);
        cont = 1;
        while(cont == 1)
        {
            cont = 0;
            for(int i = 0; i < m_head->m_children.size(); i++)
            {
                if(m_head->m_children[i] != 0)
                {
                    cont++;
                    index = i;
                }
            }
            if(cont == 1)
            {
                m_head = m_head->m_children[index];
                m_head->m_ant->m_children[index] = 0;
                m_head->m_ant = 0;
            }
            if(m_head->m_level == 1) break;
        }
        if(m_head->m_children.size() == 0) m_head = 0;
        return;
    }
};

void fixInput(vector<ld>& pmin, vector<ld>& pmax)
{
    for(int i = 0; i < pmin.size(); i++)
    {
        if(pmin[i] > pmax[i]) swap(pmin[i],pmax[i]);
    }
    return;
}

int main()
{
    int count_dim = 0, dim = 2, op, M, m;
    vector<ld> pmin,pmax,xx,yy;
    ld x;
    cout<<"M m dim"<<endl;
    cin>>M>>m>>dim;
    RTree A(M,m,dim);
    do{
       cout<<"0. Insertar de archivo."<<endl;
       cout<<"1. Insertar manualmente."<<endl;
       cout<<"2. Buscar rango."<<endl;
       cout<<"3. Eliminar elemento manual."<<endl;
       cout<<"4. Imprimir."<<endl;
       cout<<"5. Eliminar de archivo."<<endl;
       cout<<"-1. Salir."<<endl;
       cin>>op;
       if(op == 0)
       {
            ifstream file("objects.txt");
            while(file>>x)
            {
                if(count_dim < dim)
                {
                    pmin.push_back(x);
                    count_dim++;
                }
                else if(count_dim < 2*dim)
                {
                    pmax.push_back(x);
                    count_dim++;
                }
                if(count_dim == 2*dim)
                {
                    fixInput(pmin,pmax);
                    A.Insert(new Rectangle(pmin, pmax),1);
                    pmin.clear();
                    pmax.clear();
                    count_dim = 0;
                }

            }
            file.close();
       }
       else if(op == 4) A.Print(A.m_head);
       else if(op == 5)
       {
            ifstream file("objects_delete.txt");
            count_dim = 0;
            while(file>>x)
            {
                if(count_dim < dim)
                {
                    pmin.push_back(x);
                    count_dim++;
                }
                else if(count_dim < 2*dim)
                {
                    pmax.push_back(x);
                    count_dim++;
                }
                if(count_dim == 2*dim)
                {
                    fixInput(pmin,pmax);
                    A.Delete(new Rectangle(pmin, pmax));
                    /*cout<<"----------------"<<endl;
                    A.Print(A.m_head);
                    cout<<"----------------"<<endl;*/
                    pmin.clear();
                    pmax.clear();
                    count_dim = 0;
                }
            }
            file.close();
       }
       else
       {
           if(op == -1) continue;
           cout<<"Ingrese punto menor."<<endl;
           for(int i = 0; i < dim; i++)
           {
               cin>>x;
               pmin.push_back(x);
           }
           cout<<"Ingrese punto mayor."<<endl;
           for(int i = 0; i < dim; i++)
           {
               cin>>x;
               pmax.push_back(x);
           }
           fixInput(pmin,pmax);
           if(op == 1) A.Insert(new Rectangle(pmin,pmax),1);
           if(op == 2) A.Find(new Rectangle(pmin,pmax), A.m_head);
           if(op == 3) A.Delete(new Rectangle(pmin,pmax));
           pmin.clear();
           pmax.clear();
       }

    }while(op != -1);
    return 0;
}
