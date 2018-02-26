/*****************************************
 * UW User ID:  uwuserid
 * Submitted for ECE 250
 * Semester of Submission:  (Winter|Spring|Fall) 20NN
 *
 * By  submitting this file, I affirm that
 * I am the author of all modifications to
 * the provided code.
 *****************************************/

#ifndef RPOVDGBQN9TIEO3P
#define RPOVDGBQN9TIEO3P

#include "Exception.h"
#include "ece250.h"
#include <cassert>

template <typename Type>
class Search_tree {
	public:
		class Iterator;

	private:
		class Node {
			public:
				Type node_value;
				int tree_height;

				// The left and right sub-trees
				Node *left_tree;
				Node *right_tree;

				// Point to the previous and next nodes in linear order
				Node *previous_node;
				Node *next_node;

				// Member functions
				Node( Type const & = Type() );

				void update_height();

				int height() const;
				bool is_leaf() const;
				Node *front();
				Node *back();
				Node *find( Type const &obj );

				void clear();
				bool insert( Type const &obj, Node *&to_this );
				bool erase( Type const &obj, Node *&to_this );

				//Rotating functions
				Node *leftRotate(Node *&to_this);
			 	Node *rightRotate(Node *&to_this);

				//Helper function to return node balance
				int getBalance();

				//Insertion balance function
				void balance(Node *&to_this, Type const &obj);

				//Erasing balance function
				void eraseBalance(Node *&to_this, Type const &obj);
		};

		Node *root_node;
		int tree_size;

		//Sentinels for linked list
		Node *front_sentinel;
		Node *back_sentinel;

	public:
		class Iterator {
			private:
				Search_tree *containing_tree;
				Node *current_node;
				bool is_end;

				// The constructor is private so that only the search tree can create an iterator
				Iterator( Search_tree *tree, Node *starting_node );

			public:
				// DO NOT CHANGE THE SIGNATURES FOR ANY OF THESE
				Type operator*() const;
				Iterator &operator++();
				Iterator &operator--();
				bool operator==( Iterator const &rhs ) const;
				bool operator!=( Iterator const &rhs ) const;

			// Make the search tree a friend so that it can call the constructor
			friend class Search_tree;
		};

		// DO NOT CHANGE THE SIGNATURES FOR ANY OF THESE
		Search_tree();
		~Search_tree();

		bool empty() const;
		int size() const;
		int height() const;

		Type front() const;
		Type back() const;

		Iterator begin();
		Iterator end();
		Iterator rbegin();
		Iterator rend();
		Iterator find( Type const & );

		void clear();
		bool insert( Type const & );
		bool erase( Type const & );

	// Friends

	template <typename T>
	friend std::ostream &operator<<( std::ostream &, Search_tree<T> const & );
};

//////////////////////////////////////////////////////////////////////
//                Search Tree Public Member Functions               //
//////////////////////////////////////////////////////////////////////

// The initialization of the front and back sentinels is a hint
template <typename Type>
Search_tree<Type>::Search_tree():
root_node( nullptr ),
tree_size( 0 ),
front_sentinel( new Search_tree::Node( Type() ) ),
back_sentinel( new Search_tree::Node( Type() ) ) {
	front_sentinel->next_node = back_sentinel;
	back_sentinel->previous_node = front_sentinel;
}

template <typename Type>
Search_tree<Type>::~Search_tree() {
	clear();  // might as well use it...
	delete front_sentinel;
	delete back_sentinel;
}

//Returns whether or not the tree is empty
template <typename Type>
bool Search_tree<Type>::empty() const {
	return ( root_node == nullptr );
}

//Returns the number of nodes in the tree
template <typename Type>
int Search_tree<Type>::size() const {
	return tree_size;
}

//Returns the height of the tree
template <typename Type>
int Search_tree<Type>::height() const {
	return root_node->height();
}

//Returns the value of the node at the linear front of the tree
template <typename Type>
Type Search_tree<Type>::front() const {
	if ( empty() ) {
		throw underflow();
	}

	return root_node->front()->node_value;
}

//Returns the value of the node at the linear back of the tree
template <typename Type>
Type Search_tree<Type>::back() const {
	if ( empty() ) {
		throw underflow();
	}

	return root_node->back()->node_value;
}

//Returns an iterator to the front of the tree
template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::begin() {
	return empty() ? Iterator( this, back_sentinel ) : Iterator( this, root_node->front() );
}

template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::end() {
	return Iterator( this, back_sentinel );
}

template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::rbegin() {
	return empty() ? Iterator( this, front_sentinel ) : Iterator( this, root_node->back() );
}

template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::rend() {
	return Iterator( this, front_sentinel );
}

template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::find( Type const &obj ) {
	if ( empty() ) {
		return Iterator( this, back_sentinel );
	}

	typename Search_tree<Type>::Node *search_result = root_node->find( obj );

	if ( search_result == nullptr ) {
		return Iterator( this, back_sentinel );
	} else {
		return Iterator( this, search_result );
	}
}

template <typename Type>
void Search_tree<Type>::clear() {
	if ( !empty() ) {
		root_node->clear();
		root_node = nullptr;
		tree_size = 0;
	}

	// Reinitialize the sentinels
	front_sentinel->next_node = back_sentinel;
	back_sentinel->previous_node = front_sentinel;
}

template <typename Type>
bool Search_tree<Type>::insert( Type const &obj ) {
	if ( empty() ) {
		root_node = new Search_tree::Node( obj );
		tree_size = 1;

		front_sentinel->next_node = root_node;
		root_node->previous_node = front_sentinel;
		root_node->next_node = back_sentinel;
		back_sentinel->previous_node= root_node;

		return true;
	} else if ( root_node->insert( obj, root_node ) ) {
		++tree_size;
		return true;
	} else {
		return false;
	}
}

template <typename Type>
bool Search_tree<Type>::erase( Type const &obj ) {
	if ( !empty() && root_node->erase( obj, root_node ) ) {
		--tree_size;
		return true;
	} else {
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
//                   Node Public Member Functions                   //
//////////////////////////////////////////////////////////////////////

template <typename Type>
Search_tree<Type>::Node::Node( Type const &obj ):
node_value( obj ),
left_tree( nullptr ),
right_tree( nullptr ),
next_node( nullptr ),
previous_node( nullptr ),
tree_height( 0 ) {
	// does nothing
}

template <typename Type>
void Search_tree<Type>::Node::update_height() {
	//Sets the height of the node to be the max of the heights of its children plus 1
	tree_height = std::max( left_tree->height(), right_tree->height() ) + 1;
}

//Returns the height of the node if the node is not a null pointer
template <typename Type>
int Search_tree<Type>::Node::height() const {
	return ( this == nullptr ) ? -1 : tree_height;
}

// Return true if the current node is a leaf node, false otherwise
template <typename Type>
bool Search_tree<Type>::Node::is_leaf() const {
	return ( (left_tree == nullptr) && (right_tree == nullptr) );
}

// Return a pointer to the front node
template <typename Type>
typename Search_tree<Type>::Node *Search_tree<Type>::Node::front() {
	return ( left_tree == nullptr ) ? this : left_tree->front();
}

// Return a pointer to the back node
template <typename Type>
typename Search_tree<Type>::Node *Search_tree<Type>::Node::back() {
	return ( right_tree == nullptr ) ? this : right_tree->back();
}

//Returns a pointer to the first node with the value passed
template <typename Type>
typename Search_tree<Type>::Node *Search_tree<Type>::Node::find( Type const &obj ) {
	if ( obj == node_value ) {
		return this;
	} else if ( obj < node_value ) {
		return (left_tree == nullptr) ? nullptr : left_tree->find( obj );
	} else {
		return ( right_tree == nullptr ) ? nullptr : right_tree->find( obj );
	}
}

// Recursively clear the tree
template <typename Type>
void Search_tree<Type>::Node::clear() {
	if ( left_tree != nullptr ) {
		left_tree->clear();
	}

	if ( right_tree != nullptr ) {
		right_tree->clear();
	}

	delete this;
}

template <typename Type>
bool Search_tree<Type>::Node::insert( Type const &obj, Search_tree<Type>::Node *&to_this ) {
	if ( obj < node_value ) {
		if ( left_tree == nullptr ) {
			//Insert node into the left tree
			left_tree = new Search_tree<Type>::Node( obj );

			//Rebind linked list pointers
			to_this->previous_node->next_node = left_tree;
			left_tree->previous_node = to_this->previous_node;
			left_tree->next_node=to_this;
			to_this->previous_node = left_tree;

			//Update height of the tree
			update_height();
			return true;
		} else {
			//Recurse through left tree
			if ( left_tree->insert( obj, left_tree ) ) {
				//Balance the tree at each node as necessary and update the heights
				balance(to_this, obj);
				update_height();
				return true;

			} else {
				//If the object cannot be inserted return false
				return false;
			}
		}
	} else if ( obj > node_value ) {
		if ( right_tree == nullptr ) {
			//Insert node into right tree
			right_tree = new Search_tree<Type>::Node( obj );

			//Rebind linked list pointers
			to_this->next_node->previous_node = right_tree;
			right_tree->next_node = to_this->next_node;
			right_tree->previous_node=to_this;
			to_this->next_node = right_tree;

			//Update height of the tree
			update_height();
			return true;

		} else {

			//Recurse through right tree
			if ( right_tree->insert( obj, right_tree ) ) {
				//Balance each node if necessary and update the heights
				balance(to_this, obj);
				update_height();
				return true;
			} else {
				//Return false if the object cannot be inserted
				return false;
			}
		}
	} else {
		//If the object is already in the tree, return false
		return false;
	}
}

template <typename Type>
bool Search_tree<Type>::Node::erase( Type const &obj, Search_tree<Type>::Node *&to_this ) {
	if ( obj < node_value ) {
		if ( left_tree == nullptr ) {
			//Return false if the object is smaller than the smallest node in the tree
			return false;
		} else {

			//Recurse through the left tree
			if ( left_tree->erase( obj, left_tree ) ) {

				//Balance the tree as nodes are erased, and update their heights after
				eraseBalance(to_this, obj);
				update_height();
				return true;
			}

			return false;
		}
	} else if ( obj > node_value ) {
		if ( right_tree == nullptr ) {
			//Return false if the object is greater than the greatest node in the tree
			return false;
		} else {
			if ( right_tree->erase( obj, right_tree ) ) {
				//Recurse through the right tree and balance the nodes after erasure
				eraseBalance(to_this, obj);

				//Update the height of the nodes
				update_height();
				return true;
			}

			return false;
		}
	} else {
		assert( obj == node_value );

		//Rebind linked list pointers for deletion
		if(previous_node!=nullptr)
			previous_node->next_node=next_node;
		if(next_node!=nullptr)
			next_node->previous_node = previous_node;
		if ( is_leaf() ) {
			//If the node is a leaf, simply delete it
			to_this = nullptr;
			delete this;

			//Perform necessary swaps for standard BST deletion
		} else if ( left_tree == nullptr ) {
			to_this = right_tree;
			delete this;
		} else if ( right_tree == nullptr ) {
			to_this = left_tree;
			delete this;
		} else {
			node_value = right_tree->front()->node_value;
			right_tree->erase( node_value, right_tree );
			update_height();
		}

		return true;
	}
}

template <typename Type>
typename Search_tree<Type>::Node* Search_tree<Type>::Node::rightRotate(Node *&to_this) {
    Node *lt = to_this->left_tree;
    Node *lt_rt = lt->right_tree;

    // Perform rotation
    lt->right_tree = to_this;
    to_this->left_tree = lt_rt;

    // Update heights
    to_this->update_height();
    lt->update_height();

    // Return new root
    return lt;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
template <typename Type>
typename Search_tree<Type>::Node* Search_tree<Type>::Node::leftRotate(Node *&to_this) {
    Node *rt = to_this->right_tree;
  	Node *rt_lt = rt->left_tree;
    // Perform rotation
    rt->left_tree = to_this;
    to_this->right_tree = rt_lt;

    //  Update heights
  	to_this->update_height();
    rt->update_height();

		return rt;
}

// Returns the difference in height between the left and right subtrees of a node
template <typename Type>
int Search_tree<Type>::Node::getBalance()
{
    if (this == nullptr)
        return 0;
    return left_tree->height() - right_tree->height();
}

template <typename Type>
void Search_tree<Type>::Node::balance(Node *&to_this, Type const &obj){
	int balance = getBalance();

    // If this node becomes unbalanced, then
    // there are 4 cases
		if(to_this->right_tree==nullptr || balance == 2){
			if(obj<to_this->left_tree->node_value){
				//Left-left
				to_this = rightRotate(to_this);
			}
			else if(obj > to_this->left_tree->node_value){
				//Left-Right
				to_this->left_tree = leftRotate(to_this->left_tree);
				to_this = rightRotate(to_this);
			}
		}

		else if(to_this->left_tree==nullptr || balance ==-2){

			//Right-Right
			if(obj > to_this->right_tree->node_value){
				to_this = leftRotate(to_this);
			}
			//Right-Left
			else if (obj <to_this->right_tree->node_value){
				right_tree = rightRotate(to_this->right_tree);
				to_this= leftRotate(to_this);
			}
		}
}
template <typename Type>
void Search_tree<Type>::Node::eraseBalance(Node *&to_this, Type const &obj){
	int balance = to_this->getBalance();

		if(to_this->left_tree==nullptr && to_this->right_tree==nullptr){
			return;
		}

    // Left-Left
    if (balance > 1 && to_this->left_tree->getBalance() >= 0)
        to_this= rightRotate(to_this);

    // Left-Right
    if (balance > 1 && to_this->left_tree->getBalance() < 0){
        to_this->left_tree =  leftRotate(to_this->left_tree);
        to_this=rightRotate(to_this);
    }

    // Right-Right
    if (balance < -1 && to_this->right_tree->getBalance() <= 0)
        to_this=leftRotate(to_this);

    // Right-Left 
    if (balance < -1 && to_this->right_tree->getBalance() > 0){
        to_this->right_tree = rightRotate(to_this->right_tree);
        to_this= leftRotate(to_this);
    }
}
//////////////////////////////////////////////////////////////////////
//                   Iterator Private Constructor                   //
//////////////////////////////////////////////////////////////////////

template <typename Type>
Search_tree<Type>::Iterator::Iterator( Search_tree<Type> *tree, typename Search_tree<Type>::Node *starting_node ):
containing_tree( tree ),
current_node( starting_node ) {
	// This is done for you...
	// Does nothing...
}

//////////////////////////////////////////////////////////////////////
//                 Iterator Public Member Functions                 //
//////////////////////////////////////////////////////////////////////

template <typename Type>
Type Search_tree<Type>::Iterator::operator*() const {
	// This is done for you...
	return current_node->node_value;
}

template <typename Type>
typename Search_tree<Type>::Iterator &Search_tree<Type>::Iterator::operator++() {
	// Update the current node to the node containing the next higher value
	// If we are already at end do nothing
	if (*this != containing_tree->end()) {
			current_node = current_node->next_node;
		}

	return *this;
}

template <typename Type>
typename Search_tree<Type>::Iterator &Search_tree<Type>::Iterator::operator--() {
	// Update the current node to the node containing the next smaller value
	// If we are already at either rend, do nothing
	if (*this != containing_tree->rend()) {
			current_node = current_node->previous_node;
		}
	return *this;
}

template <typename Type>
bool Search_tree<Type>::Iterator::operator==( typename Search_tree<Type>::Iterator const &rhs ) const {
	// This is done for you...
	return ( current_node == rhs.current_node );
}

template <typename Type>
bool Search_tree<Type>::Iterator::operator!=( typename Search_tree<Type>::Iterator const &rhs ) const {
	// This is done for you...
	return ( current_node != rhs.current_node );
}

//////////////////////////////////////////////////////////////////////
//                            Friends                               //
//////////////////////////////////////////////////////////////////////

// You can modify this function however you want:  it will not be tested

template <typename T>
std::ostream &operator<<( std::ostream &out, Search_tree<T> const &list ) {
	out << "not yet implemented";

	return out;
}

#endif
