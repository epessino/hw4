#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// Returns the length of every root-to-leaf path in this subtree if they
// are all equal, -1 if they are not, and 0 for an empty subtree.
int pathLength(Node* node);


bool equalPaths(Node * root)
{
    // Add your code below
    return pathLength(root) != -1;
}


int pathLength(Node* node)
{
    if (node == nullptr) {
        return 0;
    }

    int left = pathLength(node->left);
    int right = pathLength(node->right);

    // A mismatch anywhere below propagates up
    if (left == -1 || right == -1) {
        return -1;
    }

    // Missing child: only the other side has paths (also covers leaves)
    if (node->left == nullptr) {
        return right + 1;
    }
    if (node->right == nullptr) {
        return left + 1;
    }

    // Both children exist: their path lengths must match
    if (left != right) {
        return -1;
    }
    return left + 1;
}