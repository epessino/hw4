#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* x);
    void rotateRight(AVLNode<Key, Value>* x);
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    const Key& key = new_item.first;

    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(key, new_item.second, NULL);
        return;
    }

    // Standard BST walk to find the insertion point
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;
    while (curr != NULL) {
        parent = curr;
        if (key < curr->getKey()) {
            curr = curr->getLeft();
        }
        else if (curr->getKey() < key) {
            curr = curr->getRight();
        }
        else {
            // Key already exists: overwrite the value, structure unchanged
            curr->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(key, new_item.second, parent);
    if (key < parent->getKey()) {
        parent->setLeft(n);
    }
    else {
        parent->setRight(n);
    }

    // Balance convention: balance = height(right) - height(left)
    if (parent->getBalance() != 0) {
        // Parent was leaning one way; it is now even and its height didn't change
        parent->setBalance(0);
    }
    else {
        parent->updateBalance(parent->getLeft() == n ? -1 : 1);
        insertFix(parent, n);
    }
}

/*
 * p's height just grew by one because of child n. Propagate up to the
 * grandparent and rotate if it becomes unbalanced.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if (p == NULL) {
        return;
    }
    AVLNode<Key, Value>* g = p->getParent();
    if (g == NULL) {
        return;
    }

    if (g->getLeft() == p) {
        g->updateBalance(-1);
        if (g->getBalance() == 0) {
            return;
        }
        if (g->getBalance() == -1) {
            insertFix(g, p);
            return;
        }
        // g->getBalance() == -2
        if (p->getLeft() == n) {
            // zig-zig
            rotateRight(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {
            // zig-zag
            rotateLeft(p);
            rotateRight(g);
            if (n->getBalance() == -1) {
                p->setBalance(0);
                g->setBalance(1);
            }
            else if (n->getBalance() == 0) {
                p->setBalance(0);
                g->setBalance(0);
            }
            else {
                p->setBalance(-1);
                g->setBalance(0);
            }
            n->setBalance(0);
        }
    }
    else {
        g->updateBalance(1);
        if (g->getBalance() == 0) {
            return;
        }
        if (g->getBalance() == 1) {
            insertFix(g, p);
            return;
        }
        // g->getBalance() == 2
        if (p->getRight() == n) {
            // zig-zig
            rotateLeft(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {
            // zig-zag
            rotateRight(p);
            rotateLeft(g);
            if (n->getBalance() == 1) {
                p->setBalance(0);
                g->setBalance(-1);
            }
            else if (n->getBalance() == 0) {
                p->setBalance(0);
                g->setBalance(0);
            }
            else {
                p->setBalance(1);
                g->setBalance(0);
            }
            n->setBalance(0);
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (n == NULL) {
        return;
    }

    // Two children: swap with predecessor (swaps balances too)
    if (n->getLeft() != NULL && n->getRight() != NULL) {
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    AVLNode<Key, Value>* p = n->getParent();
    AVLNode<Key, Value>* child = (n->getLeft() != NULL) ? n->getLeft() : n->getRight();

    // Removing from p's left makes p lean right (+1), and vice versa
    int diff = 0;
    if (p != NULL) {
        diff = (p->getLeft() == n) ? 1 : -1;
    }

    if (child != NULL) {
        child->setParent(p);
    }
    if (p == NULL) {
        this->root_ = child;
    }
    else if (p->getLeft() == n) {
        p->setLeft(child);
    }
    else {
        p->setRight(child);
    }
    delete n;

    removeFix(p, diff);
}

/*
 * One of n's subtrees just shrank. diff is the change to n's balance
 * (+1 if the left side shrank, -1 if the right side shrank).
 */
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff)
{
    if (n == NULL) {
        return;
    }

    // Figure out the next step up before any rotations change the parent
    AVLNode<Key, Value>* p = n->getParent();
    int ndiff = 0;
    if (p != NULL) {
        ndiff = (p->getLeft() == n) ? 1 : -1;
    }

    int newBal = n->getBalance() + diff;

    if (newBal == -2) {
        AVLNode<Key, Value>* c = n->getLeft();   // taller child
        if (c->getBalance() == -1) {
            rotateRight(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, ndiff);
        }
        else if (c->getBalance() == 0) {
            rotateRight(n);
            n->setBalance(-1);
            c->setBalance(1);
            // height unchanged, done
        }
        else {
            AVLNode<Key, Value>* g = c->getRight();
            rotateLeft(c);
            rotateRight(n);
            if (g->getBalance() == 1) {
                n->setBalance(0);
                c->setBalance(-1);
            }
            else if (g->getBalance() == 0) {
                n->setBalance(0);
                c->setBalance(0);
            }
            else {
                n->setBalance(1);
                c->setBalance(0);
            }
            g->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if (newBal == 2) {
        AVLNode<Key, Value>* c = n->getRight();  // taller child
        if (c->getBalance() == 1) {
            rotateLeft(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, ndiff);
        }
        else if (c->getBalance() == 0) {
            rotateLeft(n);
            n->setBalance(1);
            c->setBalance(-1);
            // height unchanged, done
        }
        else {
            AVLNode<Key, Value>* g = c->getLeft();
            rotateRight(c);
            rotateLeft(n);
            if (g->getBalance() == -1) {
                n->setBalance(0);
                c->setBalance(1);
            }
            else if (g->getBalance() == 0) {
                n->setBalance(0);
                c->setBalance(0);
            }
            else {
                n->setBalance(-1);
                c->setBalance(0);
            }
            g->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if (newBal == 0) {
        // Was leaning toward the side that shrank: height dropped by one
        n->setBalance(0);
        removeFix(p, ndiff);
    }
    else {
        // newBal is -1 or +1: was even, height unchanged, done
        n->setBalance(static_cast<int8_t>(newBal));
    }
}

/*
 * Left rotation about x: x's right child y takes x's place.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* p = x->getParent();

    x->setRight(y->getLeft());
    if (y->getLeft() != NULL) {
        y->getLeft()->setParent(x);
    }

    y->setParent(p);
    if (p == NULL) {
        this->root_ = y;
    }
    else if (p->getLeft() == x) {
        p->setLeft(y);
    }
    else {
        p->setRight(y);
    }

    y->setLeft(x);
    x->setParent(y);
}

/*
 * Right rotation about x: x's left child y takes x's place.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getLeft();
    AVLNode<Key, Value>* p = x->getParent();

    x->setLeft(y->getRight());
    if (y->getRight() != NULL) {
        y->getRight()->setParent(x);
    }

    y->setParent(p);
    if (p == NULL) {
        this->root_ = y;
    }
    else if (p->getLeft() == x) {
        p->setLeft(y);
    }
    else {
        p->setRight(y);
    }

    y->setRight(x);
    x->setParent(y);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif