// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.


#include "support.h"

template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap();
    ~MyMap();
    void clear();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;
    
private:
    struct Node
    {
        KeyType valKey;
        ValueType valValue;
        Node *left;
        Node *right;
    };
    
    Node *rootptr;
    int m_size;
    
    void freeTree(Node *cur);
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap()
{
    rootptr = nullptr;  //new map has rootptr as null and no size
    m_size = 0;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap()
{
    clear();    //destructor calls clear
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear()
{
    freeTree(rootptr);  //frees the whole tree and sets size back to 0
    m_size = 0;
    rootptr = nullptr;
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const
{
    return m_size;  //returns the size of tree
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    if (rootptr == nullptr) //if at base
    {
        rootptr = new Node;
        rootptr->valKey = key;  //create a new node, store the key and value, set left and right to null
        rootptr->valValue = value;
        rootptr->left = nullptr;
        rootptr->right = nullptr;
        m_size++;   //add 1
        return;
    }
    
    Node *cur = rootptr;    //start at the rootptr
    
    for(;;)
    {
        if (key == cur->valKey) //if the key is correct
        {
            cur->valValue = value;  //simply edit the value if there is a key already
            return;
        }
        
        if (key < cur->valKey)  //if the key to add is less than the current key
        {
            if(cur->left != nullptr)
                cur = cur->left;    //go left if there is a node left
            else
            {
                cur->left = new Node;
                cur->left->valKey = key;    //otherwise make a new node at left and set the new node's key and values
                cur->left->valValue = value;
                cur->left->left = nullptr;  //new node's left and right to null
                cur->left->right = nullptr;
                m_size++;
            }
        }//if (key > cur->valKey)
        else    //otherwise it is greater than
        {
            if(cur->right != nullptr)
                cur = cur->right;   //go right if there is a node to the right
            else
            {
                cur->right = new Node;
                cur->right->valKey = key;   //otherwise make a new node at right and set the new node's key and values
                cur->right->valValue = value;
                cur->right->left = nullptr; //new node's left and right to null
                cur->right->right = nullptr;
                m_size++;
            }
        }
    }
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
    Node* tempRoot = rootptr;
    while (tempRoot != nullptr)
    {
        if (key == tempRoot->valKey)    //if the key matches, return the value
            return &(tempRoot->valValue);
        else if (key < tempRoot->valKey)    // if what searching for is less, go left
            tempRoot = tempRoot->left;
        else
            tempRoot = tempRoot->right; //otherwise go right
    }
    return nullptr; //nullptr if cant find at the end
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::freeTree(Node *cur)
{
    if(cur == nullptr)
        return;
    
    freeTree(cur->left);    //frees everything to the left
    freeTree(cur->right);   //frees everything to the right
    
    delete cur; //deletes the last thing
}
