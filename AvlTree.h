#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "dsexceptions.h"
#include <algorithm>
#include <iostream> 
using namespace std;

// AvlTree class
//
// CONSTRUCTION: zero parameter
//
// ******************PUBLIC OPERATIONS*********************
// void insert( x )       --> Insert x
// void remove( x )       --> Remove x (unimplemented)
// bool contains( x )     --> Return true if x is present
// Comparable findMin( )  --> Return smallest item
// Comparable findMax( )  --> Return largest item
// boolean isEmpty( )     --> Return true if empty; else false
// void makeEmpty( )      --> Remove all items
// void printTree( )      --> Print tree in sorted order
// ******************ERRORS********************************
// Throws UnderflowException as warranted

template <typename Comparable>

class AvlTree
{
  public:

    AvlTree( ) : root{ nullptr }
      { }
    
    AvlTree( const AvlTree & rhs ) : root{ nullptr }
    {
        root = clone( rhs.root );
    }

    AvlTree( AvlTree && rhs ) : root{ rhs.root }
    {
        rhs.root = nullptr;
    }
    
    ~AvlTree( )
    {
        makeEmpty( );
    }

    /**
     * Deep copy.
     */
    AvlTree & operator=( const AvlTree & rhs )
    {
        AvlTree copy = rhs;
        std::swap( *this, copy );
        return *this;
    }
        
    /**
     * Move.
     */
    AvlTree & operator=( AvlTree && rhs )
    {
        std::swap( root, rhs.root );
        
        return *this;
    }
    
    /**
     * Find the smallest item in the tree.
     * Throw UnderflowException if empty.
     */
    const Comparable & findMin( ) const
    {
        if( isEmpty( ) )
            throw UnderflowException{ };
        return findMin_private( root )->task_ID;
    }

    /**
     * Find the largest item in the tree.
     * Throw UnderflowException if empty.
     */
    const Comparable & findMax( ) const
    {
        if( isEmpty( ) )
            throw UnderflowException{ };
        return findMax_private( root )->task_ID;
    }

    /**
     * Returns true if x is found in the tree.
     */
    bool contains( const Comparable & x ) const
    {
        return contains_private( x, root );
    }

    /**
     * Test if the tree is logically empty.
     * Return true if empty, false otherwise.
     */
    bool isEmpty( ) const
    {
        return root == nullptr;
    }

    /**
     * Print the tree contents in sorted order.
     */
    void printTree( ) const
    {
        if( isEmpty( ) )
            cout << "Empty tree" << endl;
        else
            printTree_private( root );
    }

    /**
     * Make the tree logically empty.
     */
    void makeEmpty( )
    {
        makeEmpty_private( root );
    }

    /**
     * Insert x into the tree; duplicates are ignored.
     */
    void insert( const Comparable & x , const Comparable & y)
    {
        insert_private( x, y, root );
    }
     
    /**
     * Insert x into the tree; duplicates are ignored.
     */
    void insert( Comparable && x, Comparable && y )
    {
        insert_private( std::move( x ), y, root );
    }
     
    /**
     * Remove x from the tree. Nothing is done if x is not found.
     */
    void remove( const Comparable & x )
    {
        remove_private( x, root );
    }

    void displayTree(ostream & out = cout) const {
        if (isEmpty()){
            out << "tree doesn't exist, cannot print" << endl;
            return;
        }
        else {
            return displayTree_private(root, 0, out);
        }

    }


  private:
    
    struct AvlNode
    {
        Comparable task_ID;
        int heap_index;
        AvlNode   *left;
        AvlNode   *right;
        int       height;

        AvlNode( const Comparable & ele, const int index, AvlNode *lt, AvlNode *rt, int h = 0 )
          : task_ID{ ele }, heap_index{ index }, left{ lt }, right{ rt }, height{ h } { }
        
        AvlNode( Comparable && ele, const int index, AvlNode *lt, AvlNode *rt, int h = 0 )
          : task_ID{ std::move( ele ) }, heap_index{ index }, left{ lt }, right{ rt }, height{ h } { }
    };

    AvlNode *root;

    /**
     * Internal method to insert into a subtree.
     * x is the ID to insert.
     * y is heap index
     * t is the node that roots the subtree.
     * Set the new root of the subtree.
     */
    void insert_private( const Comparable & x, const Comparable & y, AvlNode * & t )
    {
        if( t == nullptr )
            t = new AvlNode{ x, y, nullptr, nullptr };
        else if( x < t->task_ID )
            insert_private( x, y, t->left );
        else if( t->task_ID < x )
            insert_private( x, y, t->right );
        
        balance( t );
    }

    /**
     * Internal method to insert into a subtree.
     * x is the ID to insert.
     * t is the node that roots the subtree.
     * Set the new root of the subtree.
     */
    void insert_private( Comparable && x, Comparable && y, AvlNode * & t )
    {
        if( t == nullptr )
            t = new AvlNode{ std::move( x ), y, nullptr, nullptr };
        else if( x < t->task_ID )
            insert_private( std::move( x ), y, t->left );
        else if( t->task_ID < x )
            insert_private( std::move( x ), y, t->right );
        
        balance( t );
    }
     
    /**
     * Internal method to remove from a subtree.
     * x is the ID of node need to remove.
     * t is the node that roots the subtree.
     * Set the new root of the subtree.
     */
    void remove_private( const Comparable & x, AvlNode * & t )
    {
        if( t == nullptr )
            return;   // Item not found; do nothing
        
        if( x < t->task_ID )
            remove_private( x, t->left );
        else if( t->task_ID < x )
            remove_private( x, t->right );
        else if( t->left != nullptr && t->right != nullptr ) // Two children
        {
            t->task_ID = findMin_private( t->right )->task_ID;
            remove_private( t->task_ID, t->right );
        }
        else
        {
            AvlNode *oldNode = t;
            t = ( t->left != nullptr ) ? t->left : t->right;
            delete oldNode;
        }
        
        balance( t );
    }
    
    static const int ALLOWED_IMBALANCE = 1;

    // Assume t is balanced or within one of being balanced
    void balance( AvlNode * & t ) {
        if( t == nullptr )
            return;
        
        if( height( t->left ) - height( t->right ) > ALLOWED_IMBALANCE ){
            if( height( t->left->left ) >= height( t->left->right ) )
                rotateWithLeftChild( t );
            else
                doubleWithLeftChild( t );
        } else {
            if( height( t->right ) - height( t->left ) > ALLOWED_IMBALANCE ) {
                if( height( t->right->right ) >= height( t->right->left ) )
                    rotateWithRightChild( t );
                else
                    doubleWithRightChild( t );
            }
        }       
        t->height = max( height( t->left ), height( t->right ) ) + 1;
    }
    
    /**
     * Internal method to find the smallest item in a subtree t.
     * Return node containing the smallest item.
     */
    AvlNode * findMin_private( AvlNode *t ) const
    {
        if( t == nullptr )
            return nullptr;
        if( t->left == nullptr )
            return t;
        return findMin_private( t->left );
    }

    /**
     * Internal method to find the largest item in a subtree t.
     * Return node containing the largest item.
     */
    AvlNode * findMax_private( AvlNode *t ) const
    {
        if( t != nullptr )
            while( t->right != nullptr )
                t = t->right;
        return t;
    }


    /**
     * Internal method to test if an item is in a subtree.
     * x is item to search for.
     * t is the node that roots the tree.
     */
    bool contains_private( const Comparable & x, AvlNode *t ) const
    {
        if( t == nullptr )
            return false;
        else if( x < t->task_ID )
            return contains_private( x, t->left );
        else if( t->task_ID < x )
            return contains_private( x, t->right );
        else
            return true;    // Match
    }

    /**
     * Internal method to make subtree empty.
     */
    void makeEmpty_private( AvlNode * & t )
    {
        if( t != nullptr )
        {
            makeEmpty_private( t->left );
            makeEmpty_private( t->right );
            delete t;
        }
        t = nullptr;
    }

    /**
     * Internal method to print a subtree rooted at t in sorted order.
     */
    void printTree_private( AvlNode *t ) const
    {
        if( t != nullptr )
        {
            printTree_private( t->left );
            cout << "task ID: "<< t->task_ID << " and Heap Index: " << t->heap_index << endl;
            printTree_private( t->right );
        }
    }

    void displayTree_private( AvlNode *t, int depth, ostream & out ) const
    {
        const int SHIFT = 4 ;

        if (t != nullptr) 
        {
            for (int i = 0; i < SHIFT*depth; i++){ out << " " ;}
            out << t->task_ID << ") " << "heap index: " << t->heap_index ;
            out << " " << "L) " << t->left->task_ID << " " << "R) " << t->right->task_ID << endl;
            displayTree_private(t->left, depth+1, out);
            displayTree_private(t->right, depth+1, out);
        }
    }

    /**
     * Internal method to clone subtree.
     */
    AvlNode * clone( AvlNode *t ) const
    {
        if( t == nullptr )
            return nullptr;
        else
            return new AvlNode{ t->task_ID, t->heap_index, clone( t->left ), clone( t->right ), t->height };
    }
        // Avl manipulations
    /**
     * Return the height of node t or -1 if nullptr.
     */
    int height( AvlNode *t ) const
    {
        return t == nullptr ? -1 : t->height;
    }

    int max( int lhs, int rhs ) const
    {
        return lhs > rhs ? lhs : rhs;
    }

    /**
     * Rotate binary tree node with left child.
     * For AVL trees, this is a single rotation for case 1.
     * Update heights, then set new root.
     */
    void rotateWithLeftChild( AvlNode * & k2 )
    {
        AvlNode *k1 = k2->left;
        k2->left = k1->right;
        k1->right = k2;
        k2->height = max( height( k2->left ), height( k2->right ) ) + 1;
        k1->height = max( height( k1->left ), k2->height ) + 1;
        k2 = k1;
    }

    /**
     * Rotate binary tree node with right child.
     * For AVL trees, this is a single rotation for case 4.
     * Update heights, then set new root.
     */
    void rotateWithRightChild( AvlNode * & k1 )
    {
        AvlNode *k2 = k1->right;
        k1->right = k2->left;
        k2->left = k1;
        k1->height = max( height( k1->left ), height( k1->right ) ) + 1;
        k2->height = max( height( k2->right ), k1->height ) + 1;
        k1 = k2;
    }

    /**
     * Double rotate binary tree node: first left child.
     * with its right child; then node k3 with new left child.
     * For AVL trees, this is a double rotation for case 2.
     * Update heights, then set new root.
     */
    void doubleWithLeftChild( AvlNode * & k3 )
    {
        rotateWithRightChild( k3->left );
        rotateWithLeftChild( k3 );
    }

    /**
     * Double rotate binary tree node: first right child.
     * with its left child; then node k1 with new right child.
     * For AVL trees, this is a double rotation for case 3.
     * Update heights, then set new root.
     */
    void doubleWithRightChild( AvlNode * & k1 )
    {
        rotateWithLeftChild( k1->right );
        rotateWithRightChild( k1 );
    }
};

#endif
