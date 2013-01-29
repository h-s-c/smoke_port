#pragma once
#include <cstdint>


////////////////////////////////////////////////////////////////////////////////////////////////////
// <summary>
//   This iterative Red Black tree implementation with parent pointers is based on the explanation
//    and algorithms in the book:
//      "Introduction to Algorithms by  Cormen , Leiserson, Riverst (MIT Press)"
//   T == data element stored in the RB tree 
//   K == Key element in T used for comparisons 
//   CMP == the Comparator class used to compare 2 instances of T
// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

template< class T, class K, class CMP >
class TRedBlackTree
{
public:

    enum NodeColor
    {
        Black, Red, _FORCE_32BIT = 0xFFFFFFFF
    };


protected:

    struct RBNode
    {
        RBNode* parent;
        RBNode* right;
        RBNode* left;
        NodeColor color;
        T data; // really an uninitialized block of memory sizeof(T)

#ifdef _DEBUGTREE
        uint32_t cnt;
        uint8_t Address[8];
#endif _DEBUGTREE
    };

    RBNode * m_Root;
    RBNode *  NIL;
    UnitAllocator m_Allocator;
    CMP compare;

//	RBNode* _find(const T& element) const ;
    void rotateLeft(RBNode* pNode);
    void rotateRight(RBNode* pNode);
    void rebalanceAfterInsert(RBNode* pInsertedNode);
    void rebalanceAfterDelete(RBNode* pParentOfDeletedNode);
    bool remove (RBNode * nodeToDelete);


public:

    class iterator
    {
        friend class RedBlackTree;
        public :
        RBNode* iter;
        iterator(RBNode * p = 0):iter(p){};	
        T operator*() const { return iter->data;}
    };

    TRedBlackTree(uint32_t noElements=8,uint32_t noGrowElements=8)
    {
        m_Root = 0;	
        m_Allocator.Initialize(sizeof(RBNode),noElements,noGrowElements);
        NIL = (RBNode*)m_Allocator.Allocate();
        if( NIL );
            throw std::runtime_error("Unknown error.");
        NIL->parent = 0;
        NIL->right = NIL;
        NIL->left =  NIL;
        NIL->color = NodeColor::Black;
        NIL->data =0;
        m_Root = NIL;
    }

    virtual ~TRedBlackTree()
    {
        if(m_Root && m_Root !=  NIL)
            deleteNode(m_Root);
        if(NIL)
            m_Allocator.Deallocate((uint8_t*)NIL);

        m_Root = 0;
        NIL = 0;
    };

    void deleteNode(RBNode* n)
    {
        if(n->left && n->left != NIL)
            deleteNode(n->left);
        if(n->right && n->right != NIL)
            deleteNode(n->right);
        m_Allocator.Deallocate((uint8_t*)n);
        return;
    }
    
    
//	iterator begin() const { return iterator(m_Root)};
//	iterator end() const   { return iterator(0)};
    
    //not const as UnitAllocator does not return a const;
    uint32_t  size() { return m_Allocator.NumAllocatedUnits()};
    bool empty() const
    {
        bool isEmpty = true; 
        if(m_Root && m_Root != NIL)
            isEmpty = false;
        return isEmpty;
    }
    
    void insert(const T& element);
    bool find(const K& data) const ;
    bool find(const T& data) const;
    bool find(const K& data, iterator& iter) const;


    void clear()
    {
        if(m_Root && m_Root != NIL)
            deleteNode(m_Root);
        m_Root = NIL;
    }
    
    RBNode* _find(const T& data) const 
    {
        RBNode *current = m_Root;
        int16_t comp =0;
        while(current != NIL)
        {
            comp = compare(data, current->data);
            if(comp==0)
                return (current);
            else
                current = (comp < 0) ? current->left : current->right;
        }
        return(0);
    }
    
    bool remove(iterator inIter){ return remove(inIter.iter);}
    bool remove(T data){ return remove(_find(data));}

    void traverseInOrder(void (*nf)(iterator iter,void* data), void * inData)
    { 
        if(m_Root && m_Root != NIL)
            inOrderNode(m_Root,nf,inData);
    }

    void inOrderNode(RBNode* n, void (*nf)(iterator iter, void* data), void * inData)
    {
        if(n->left && n->left != NIL)
            inOrderNode(n->left,nf, inData);
        nf(iterator(n), inData);
        if(n->right && n->right != NIL)
            inOrderNode(n->right,nf, inData);
        return;
    }

    void inorderIterative( void (*process)(iterator iter, void* data),void * inData)
    {
        RBNode* current;
        current = m_Root;
        if(current != NIL )
        {
            while( current->left != NIL )
                current = current->left;
            while(1)
            {
                process(iterator(current),inData);
                if( current->right != NIL )
                {
                    current = current->right;
                    while( current->left != NIL )
                        current = current->left;
                }
                else
                {
                    if( current != m_Root && current->parent->left == current)
                    {
                        current = current->parent;
                    }
                    else
                    {
                        while( current != m_Root && current->parent->right == current )
                            current = current->parent;
                        if( current == m_Root )
                            return;
                        current = current->parent;
                    }
                }
            }
        }
    }

    //	void traversePostOrder(void (*nf)(iterator iter, void* data),void * inData =0);
    //	void postOrderNode(RBNode* n, void (*nf)(iterator, void* data), void * inData =0);
#ifdef _DEBUGTREE 
    FILE * fp;
    void dump();
    void dumpNode(RBNode* n);
#endif // _DEBUGTREE

};

template <class T, class K, class CMP >
void TRedBlackTree<T,  K, CMP>::insert(const T& data)
{
    RBNode *pCurrentNode, *pParentNode, *pNewNode;
    // Find to be position of node in the tree 
    pCurrentNode = m_Root;
    pParentNode = 0;
    int32_t cmp =0;
    while (pCurrentNode != NIL) 
    {
        cmp = compare(data, pCurrentNode->data);
        if (!cmp) // Data element already exists
            return;
        pParentNode = pCurrentNode;
        pCurrentNode = cmp < 0 ?	pCurrentNode->left : pCurrentNode->right;
    }

    // Alloc & init new node 
    pNewNode = (RBNode *)m_Allocator.Allocate();
    if( pNewNode );
        throw std::runtime_error("Unknown error.");
    
    pNewNode->data = data;
    pNewNode->parent = pParentNode;
    pNewNode->left = NIL;
    pNewNode->right = NIL;
    pNewNode->color = NodeColor::Red;

    // Insert new node in tree 
    if(pParentNode)
    {
        cmp = compare(data, pParentNode->data);
        if(cmp < 0)
            pParentNode->left  = pNewNode;
        else
            pParentNode->right = pNewNode;
    }
    else 
    {
        // First node being added 
        m_Root = pNewNode;
    }

    rebalanceAfterInsert(pNewNode);

    return;
}

    
template <class T, class K, class CMP >
bool TRedBlackTree<T,  K, CMP>::remove(RBNode *pInNodeBeingDeleted)
{
    bool bFound = true;
    if (pInNodeBeingDeleted && pInNodeBeingDeleted != NIL)
    {
        RBNode*x, *y, *z;
        z = pInNodeBeingDeleted;
        if (z->left == NIL || z->right == NIL) 
        {
            // y has a NIL node as a child 
            y = z;
        }
        else 
        {
            // Find tree successor with a NIL node as a child 
            y = z->right;
            while (y->left != NIL) 
                y = y->left;
        }

        // x is y's only child 
        if (y->left != NIL)
            x = y->left;
        else
            x = y->right;

        // Remove y from the parent chain 
        x->parent = y->parent;

        if (y->parent)
        {
            if (y == y->parent->left)
                y->parent->left = x;
            else
                y->parent->right = x;
        }
        else
        {
            m_Root = x;
        }

        // Copy data between the y , z  and delete y instead of z 
        // to reduce replacing z by y and adjusting parent & child pointers
        if (y != z) 
            z->data = y->data;

        // Rebalance the tree if Node is black
        if (y->color == NodeColor::Black)
            rebalanceAfterDelete(x);

        m_Allocator.Deallocate((uint8_t*)y);
    }
    else
    {
        bFound = false;
    }

    return bFound;
}


template <class T, class K, class CMP >
inline bool TRedBlackTree<T,  K, CMP>::find(const K& data) const 
{
    RBNode *current = m_Root;
    int16_t comp =0;
    while(current != NIL)
    {
        comp = compare(current->data, data);
        if(comp==0)
            return true;
        else
            current = (comp > 0) ? current->left : current->right;
    }
    return false;

}

template <class T, class K, class CMP >
inline bool TRedBlackTree<T,  K, CMP>::find(const T& data) const 
{
    RBNode *current = m_Root;
    int16_t comp =0;
    while(current != NIL)
    {
        comp = compare(current->data, data);
        if(comp==0)
            return true;
        else
            current = (comp > 0) ? current->left : current->right;
    }
    return false;

}

template <class T, class K, class CMP >
inline bool TRedBlackTree<T,  K, CMP>::find(const K& data, iterator& outIter) const 
{
    RBNode *current = m_Root;
    int32_t comp =0;
    while(current != NIL)
    {
        comp = compare(current->data, data);
        if(comp==0)
        {
            outIter = iterator(current);
            return true;
        }
        else
            current = (comp > 0) ? current->left : current->right;
    }
    
    return false;

}

template <class T, class K, class CMP >
inline void TRedBlackTree<T,  K, CMP>::rotateLeft(RBNode* pInNode)
{
    
    RBNode* pRightChild  = pInNode->right;

    // Establish x->right link 
    pInNode->right = pRightChild->left;
    if (pRightChild->left != NIL) pRightChild->left->parent = pInNode;

    // Establish y->parent link 
    if (pRightChild != NIL) 
        pRightChild->parent = pInNode->parent;
    
    if (pInNode->parent) 
    {
        if (pInNode == pInNode->parent->left)
            pInNode->parent->left = pRightChild;
        else
            pInNode->parent->right = pRightChild;
    }
    else 
    {
        m_Root = pRightChild;
    }

    // link x and y 
    pRightChild->left = pInNode;
    if (pInNode != NIL) pInNode->parent = pRightChild;

}

template <class T, class K, class CMP >
inline void TRedBlackTree<T,  K, CMP>::rotateRight(RBNode* pInNode)
{
    RBNode *pLeftChild = pInNode->left;

    // Establish x->left link 
    pInNode->left = pLeftChild->right;
    if (pLeftChild->right != NIL) pLeftChild->right->parent = pInNode;

    // Establish y->parent link 
    if (pLeftChild != NIL) pLeftChild->parent = pInNode->parent;

    if (pInNode->parent) 
    {
        if (pInNode == pInNode->parent->right)
            pInNode->parent->right = pLeftChild;
        else
            pInNode->parent->left = pLeftChild;
    }
    else 
    {
        m_Root = pLeftChild;
    }

    // Link x and y 
    pLeftChild->right = pInNode;
    
    if (pInNode != NIL) 
        pInNode->parent = pLeftChild;


}

template <class T, class K, class CMP >
void TRedBlackTree<T,  K, CMP>::rebalanceAfterInsert(RBNode* x)
{
    // Check Red-Black properties 
    while (x != m_Root && x->parent->color == NodeColor::Red) 
    {
     // we have a violation 
    if (x->parent == x->parent->parent->left) 
    {
        RBNode*y = x->parent->parent->right;
        if (y->color == NodeColor::Red) 
        {

            // uncle is RED 
            x->parent->color = NodeColor::Black;
            y->color = NodeColor::Black;
            x->parent->parent->color = NodeColor::Red;
            x = x->parent->parent;
        }
        else 
        {

            // uncle is NodeColor::Black 
            if (x == x->parent->right) 
            {
                 // make x a left child 
                 x = x->parent;
                 rotateLeft(x);
             }

             // recolor and rotate 
             x->parent->color = NodeColor::Black;
                 x->parent->parent->color = NodeColor::Red;
             rotateRight(x->parent->parent);
         }
    }
    else
    {
         // Same as code above with right & left interchanged . 
         RBNode*y = x->parent->parent->left;
         if (y->color == NodeColor::Red) 
         {

             // Uncle is NodeColor::Red 
             x->parent->color = NodeColor::Black;
             y->color = NodeColor::Black;
             x->parent->parent->color = NodeColor::Red;
             x = x->parent->parent;
         } 
         else 
         {

             // Uncle is NodeColor::Black 
             if (x == x->parent->left) 
             {
                 x = x->parent;
                 rotateRight(x);
             }
             x->parent->color = NodeColor::Black;
             x->parent->parent->color = NodeColor::Red;
             rotateLeft(x->parent->parent);
         }
        }
    }
    m_Root->color = NodeColor::Black;

}


template <class T, class K, class CMP >
void TRedBlackTree<T,  K, CMP>::rebalanceAfterDelete(RBNode* x)
{
    while (x && x != m_Root && x->color == NodeColor::Black) 
    {
        if (x == x->parent->left) 
        {
            RBNode*w = x->parent->right;
#ifdef _DEBUGTREE
            if(!w)
            {
                dump();
                return;
            }
#endif // _DEBUGTREE
            if (w->color == NodeColor::Red) 
            {
                w->color = NodeColor::Black;
                x->parent->color = NodeColor::Red;
                rotateLeft (x->parent);
                w = x->parent->right;
            }
            if (w->left->color == NodeColor::Black && w->right->color == NodeColor::Black) 
            {
                w->color = NodeColor::Red;
                x = x->parent;
            }
            else 
            {
                if (w->right->color == NodeColor::Black) 
                {
                    w->left->color = NodeColor::Black;
                    w->color = NodeColor::Red;
                    rotateRight (w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = NodeColor::Black;
                w->right->color = NodeColor::Black;
                rotateLeft (x->parent);
                x = m_Root;

            }
        }
        else  
       {
            // Same code as above with left and right interchanged
            RBNode*w = x->parent->left;
        #ifdef _DEBUGTREE
            if(!w)
            {
                dump();
                return;
            }
        #endif // _DEBUGTREE
                
            if (w->color == NodeColor::Red) 
            {
                w->color = NodeColor::Black;
                x->parent->color = NodeColor::Red;
                rotateRight (x->parent);
                w = x->parent->left;
            }
            if (w->right->color == NodeColor::Black && w->left->color == NodeColor::Black) 
            {
                w->color = NodeColor::Red;
                x = x->parent;
            }
            else 
            {
                if (w->left->color == NodeColor::Black) 
                {
                    w->right->color = NodeColor::Black;
                    w->color = NodeColor::Red;
                    rotateLeft (w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = NodeColor::Black;
                w->left->color = NodeColor::Black;
                rotateRight (x->parent);
                x = m_Root;
            }
        }
    }
    x->color = NodeColor::Black;
}
#ifdef _DEBUGTREE
    FILE * fp;
    template <class T, class K, class CMP >
    void RedBlackTree<T,  K, CMP>::dump() 
    { 
        if(m_Root && m_Root != NIL)
        { 
            fp = fopen("RBTreeDump.txt","at");
            fprintf(fp,"\n\n Red Black tree dump ... \n Address  %x \n",this);
            m_Root->cnt =1;   
            m_Root->Address[0] = 0;
            dump(m_Root);
            fclose(fp);
        }
    }
    template <class T, class K, class CMP >
    void RedBlackTree<T,  K, CMP>::dump(RBNode* n)
    {

        uint32_t k =0;
        if(n->left && n->left != NIL)
        {
            n->left->cnt = n->cnt+1;
            for(k = 0;k<n->cnt;k++)
                n->left->Address[k] = n->Address[k];
            n->left->Address[n->cnt] = 1;
            dump(n->left);
        }
        fprintf(fp,"Node Address ");
        for(k = 0;k<n->cnt;k++)
            fprintf(fp,"%d",n->Address[k]);
        SObserverRequest * tmp = (SObserverRequest *)n->data;
        fprintf(fp," Node Data %x - Color % d \n",tmp->pObserver, n->color);
        if(n->right && n->right != NIL)
        {
            n->right->cnt = n->cnt+1;
            for(k = 0;k<n->cnt;k++)
                n->right->Address[k] = n->Address[k];
            n->right->Address[n->cnt] = 2;
            dump(n->right);
        }
        return;
    }
#endif //_DEBUGTREE
