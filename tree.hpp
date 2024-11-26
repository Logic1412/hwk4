// We are going to build a binary search tree with an explicit
// stack based iterator.  We are doing this as a "header-only"
// c++ implementation, as it is templated and therefore needs
// to be included to be used.  This provides basically the same
// high level functionality as std::map, that is, a key/value
// data store with a sorted order

// We need to include the following headers...

#include <memory>
#include <stack>
#include <utility>

//#define HERE {std::cout << "IMPLEMENT HERE\n";}

// C++ require declaration before use, so we define
// our three classes here.
template <class K, class V>
class BinaryTree;
template <class K, class V>
class BinaryTreeIterator;
template <class K, class V>
class BinaryTreeNode;

// This iterator is returned for both start and
// end but only the start iterator matters, the end 
// iterator is effectively ignored.

// It is considered "undefined behavior" (that is,
// things are allowed to crash in obscure ways if)

// A: The iterator has a longer lifetime than the 
// underlying tree (translation: don't return the iterator)

// B: Nodes are added or removed from the tree
// (translation: its OK to use the iterator to change
// the data associated with keys using the iterator in a 
// for loop, but it is not OK to
// add new keys or remove keys)
template <class K, class V>
class BinaryTreeIterator : public std::input_iterator_tag
{
    friend class BinaryTree<K, V>;

    // The constructor.  For end you don't actually
    // need to do anything (the end is effectively a dummy).
    // For the start iterator, you should set current to root
    // and then call incr() so that the iterator is ready to 
    // actually start the traversal
    BinaryTreeIterator(BinaryTreeNode<K, V> *root, bool start)
    {
        (void) root; (void) start;
        current = nullptr;
        if (start && root)
        {
            current = root;
            incr();
        }
    }

public:
    // This should return TRUE if there is still
    // items left in the iteration:
    // Basically, check to see if the stack is empty
    // and current is null, and, if not, there is 
    // still more work to do...
    bool operator!=(BinaryTreeIterator<K, V> &other)
    {
        (void)other;
        bool curEnd = (current == nullptr && working_stack.empty());
        bool otherEnd = (other.current == nullptr && other.working_stack.empty());
        return !(curEnd && otherEnd);
    }

    // This is the heart of the tree traversal algorithm.
    // It needs to be called once in the constructor and 
    // once by each invocation of the ++ operator.
    // The idea in english...
    //
    // While current is not null, push the current
    // pointer onto the stack and take current->left 
    // and make it the new current.
    // 
    // Once current is null, then take the top of the stack
    // and set it to current...
    void incr()
    {
        while (current != nullptr)
    {
        working_stack.push(current);
        current = current->left;
    }

    if (!working_stack.empty())
    {
        current = working_stack.top();
        working_stack.pop();
    }
    else
    {
        current = nullptr;
    }
    }

    // This should just call incr
    void operator++()
    {
        incr();
    }

    // And this visits the node itself...
    // In visiting the node, it should 
    // both set current to be the current node's right
    // and then return a pair constructed from the old 
    // current node's key and value.
    std::pair<K, V> operator*()
    {
        if (current)
        {
            return std::make_pair(current->key, current->value);
        }
        throw std::logic_error("Dereference of an invalid iterator");
    }

private:
    // A pointer to the current node
    BinaryTreeNode<K, V> *current;

    // And a stack for the traversal visit of the tree
    // nodes.
    std::stack<BinaryTreeNode<K, V> *> working_stack;
};


// The class for the binary tree itself.
template <class K, class V>
class BinaryTree
{
public:
    BinaryTree() : root(nullptr)
    {
    }
    
    // The [] operation is for both getting and setting.
    // If the key exists in the tree a reference to the
    // associated value is returned.  Otherwise, it will
    // create a new tree node and return that value.

    // What you should do is first check if the root is null
    // and, if so, just create a new node.
    // then simply invoke root->find(key) and return
    // that value as find is already set up to return
    // a reference.
    V &operator[](const K &key)
    {
        if (!root)
        {
            root = new BinaryTreeNode<K, V>(key);
        }
        return root->find(key);
    }

    // This should return false if the tree
    // is empty, otherwise return root->contains
    bool contains(const K &key)
    {
        (void) key;
        if (!root)
        {
        return false;
        }
        else return root->contains(key);
    }

    // Erases a node if a key matches.  If the
    // key does not match it simply is an operation
    // that does nothing.
    void erase(const K &key)
    {
        (void) key;
        if (root)
        root = root->erase(key);
    }

    // And the destructor for the binary tree.
    // In order to prevent memory leaks and keep with
    // the C++ "RAII" convention, it should see
    // if there is a root and, if so, call freetree()
    // on the root.
    ~BinaryTree()
    {
        if (root)
    {
        root->freetree();
    }
    }

    // This returns the iterators.
    BinaryTreeIterator<K, V> begin()
    {
        return BinaryTreeIterator(root, true);
    }
    BinaryTreeIterator<K, V> end()
    {
        return BinaryTreeIterator(root, false);
    }

protected:
    BinaryTreeNode<K, V> *root;
};

// And the class for the binary tree node itself.
template <class K, class V>
class BinaryTreeNode
{
    friend class BinaryTree<K, V>;
    friend class BinaryTreeIterator<K, V>;

public:
    // The constructor, it simply setts the key and the left/right pointers.
    // Data defaults to whatever the default value is for the data type.
    BinaryTreeNode(const K &keyin) : key(keyin), left(nullptr), right(nullptr)
    {
    }

    // This should recursively free the tree.
    // It should call freetree on left and 
    // right and then, as the last act,
    // call delete on this.  

    // Yes, you can "suicide" an object in C++,
    // and this is a case where you want to do it.
    void freetree()
    {
        if (left)
        {
            left->freetree();
            left = nullptr;
        }
        if (right)
        {
            right->freetree();
            right = nullptr;
        }
        delete this;
    }

protected:

    // Removing a node from a binary tree, returning
    // a pointer to the now modified tree.

    // If you aren't at the node you are looking for, just set
    // your new left or right pointer (as approriate, and if it exists) 
    // to the  result of recursively calling erase, and then return this.

    // If this node does match the key however, check the
    // child nodes.  If there is no left, assign the right node to a temporary
    // value, delete this, and return the right node.
    // If there is no right node, do the same thing.
    // If there is both a left and right node, check if the left node
    // doesn't have a right node of its own.  If so, set that node's right
    // to the current right, assign left to a temporary, and delete this and
    // return that left node.

    // Finally, if the left node DOES have a right node, keep
    // following the right hand side of that subtree until you
    // find a node without a right hand branch.  In that case set
    // the node above it's right to that node's left, assign that
    // node to a temporary, have its left point to the current node's left
    // its right to the current node's right, delete this and return that
    // node.
    BinaryTreeNode<K, V> *erase(const K &k)
    {
        (void) k;
        if (k < key)
        {
            if (left)
                left = left->erase(k);
        }
        else if (k > key)
        {
            if (right)
                right = right->erase(k);
        }
        else
        {
            if (!left)
            {
                BinaryTreeNode<K, V> *temp = right;
                delete this;
                return temp;
            }
            else if (!right)
            {
                BinaryTreeNode<K, V> *temp = left;
                delete this;
                return temp;
            }
            else
            {
                BinaryTreeNode<K, V> *successor = left;
                while (successor->right)
                    successor = successor->right;
                key = successor->key;
                value = successor->value;
                left = left->erase(successor->key);
            }
        }
        // Again, not what you will always want to return...
        return this;
    }

    // This seeks to recursively find a key, creating
    // a new node if necessary.
    // If k == this node's key, return the value.
    // If k < this key it is to the left.
    // If there is no left node, create it with k as the key.
    // Then recursively return find on the left.  Similar for
    // the right. 
    V &find(const K &k)
    {
        if (k == key)
        {
            return value;
        }
        if (k < key)
        {
            if (!left)
                left = new BinaryTreeNode<K, V>(k);
         return left->find(k);
        }
        else // k > key
        {
            if (!right)
                right = new BinaryTreeNode<K, V>(k);
            return right->find(k);
        }
    }

    // And contains is a recursive search that doesn't
    // create new nodes, just checks if the key exists.
    bool contains(const K &k)
    {
        (void) k;
        if (k == key)
        {
            return true;
        }
        else if (k < key)
        {
            return left ? left->contains(k) : false;
        }
        else if (k > key)
        {
            return right ? right->contains(k) : false;
        }
    }

    K key;
    V value;
    BinaryTreeNode<K, V> *left;
    BinaryTreeNode<K, V> *right;
};
    V value;
    BinaryTreeNode<K, V> *left;
    BinaryTreeNode<K, V> *right;
};
