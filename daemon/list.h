//////////////////////////////////////////////////////////////////////////
//																		//
// CPUSaver (C)2013, Cristian-Valeriu Soare 							//
// author: Cristian-Valeriu Soare 										//
// e-mail: soare_cristian16@yahoo.com 									//
// supervisor: Conf. Dr. Eng. Florin Pop								//
// Politehnica University of Bucharest, 								//
// Faculty of Automatic Control and Computer Science					//
// 																		//
// This file is part of CPUSaver.										//
// 																		//
// CPUSaver is free software: you can redistribute it and/or modify		//
// it under the terms of the GNU General Public License as published by	//
// the Free Software Foundation, either version 3 of the License, or 	//
// (at your option) any later version.									//
// 																		//
// CPUSaver is distributed in the hope that it will be useful,			//
// but WITHOUT ANY WARRANTY; without even the implied warranty of		//
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 		//
// GNU General Public License for more details.							//
// 																		//
// You should have received a copy of the GNU General Public License 	//
// along with CPUSaver.  If not, see <http://www.gnu.org/licenses/>.	//
//																		//
//////////////////////////////////////////////////////////////////////////

#ifndef LIST_H
#define LIST_H

#include <iostream>
#include <cstdlib>
using namespace std;
 
template <typename tip>
struct Node 
{
	tip* value;
	Node<tip> *next;
	Node<tip> *prev;
};
 
template <typename tip>
class LinkedList
{
private:
	Node<tip> *pFirst;
	bool freerespons;
	int n;
	void errormsg(char*);
	tip* removelastnode();
	void addfirstnode(Node<tip>*&);
	void addextremnode(Node<tip>*&);

public:
	LinkedList();
	~LinkedList();
	void addFirst(tip*);
	void addLast(tip*);
	void addOrder(tip*);
	tip* removeFirst();
	tip* removeLast();
	tip* removeNode(Node<tip>*);
	tip* findNode(tip*);
	tip* peekFirst();
	tip* peekLast();
	int getSize();
	void setFreeResponsability(bool);
	bool isEmpty();
	Node<tip>* begin();
	Node<tip>* end();
	class iterator;

	template <typename ttip>
	friend ostream& operator<<(ostream&, LinkedList<ttip>&);
};

// prints error message and exists program if called
template <typename tip>
void LinkedList<tip>::errormsg(char* s)
{
	cerr << s << '\n';
	exit(EXIT_FAILURE);
}

// removes last node from list
template <typename tip>
tip* LinkedList<tip>::removelastnode()
{
	tip* x;
	x = pFirst->value;
	delete pFirst;
	pFirst = NULL;
	return x;
}

// adds initial node to the list
template <typename tip>
void LinkedList<tip>::addfirstnode(Node<tip>*& p)
{
	p->next = p;
	p->prev = p;
	pFirst = p;
}

// inserts node at the extremity of the list
// (the list is circular)
template <typename tip>
void LinkedList<tip>::addextremnode(Node<tip>*& p)
{
	p->next = pFirst;
	p->prev = pFirst->prev;
	pFirst->prev->next = p;
	pFirst->prev = p;
}

//constructor
template <typename tip>
LinkedList<tip>::LinkedList()
{
	n = 0;
	pFirst = NULL;
	freerespons = true;
}

//destructor
template <typename tip>
LinkedList<tip>::~LinkedList()
{
	if (pFirst)
	{
		Node<tip> *p, *pLast = pFirst->prev;
		while (pFirst != pLast)
		{
			p = pFirst;
			pFirst = pFirst->next;
			if (freerespons)
				delete p->value;
			delete p;
		}
		if (freerespons)
			delete pFirst->value;
		delete pFirst;
	}
}

// sets the responsibility of the LinkedList instance
// to free the memory of its elements when it is being
// deconstructed
template <typename tip>
void LinkedList<tip>::setFreeResponsability(bool ok)
{
	freerespons = ok;
}

// adds element at the begining of list
template <typename tip>
void LinkedList<tip>::addFirst(tip* value)
{
	n++;
	Node<tip> *p = new Node<tip>;
	p->value = value;
	if (pFirst)
	{
		addextremnode(p);
		pFirst = p;
	}
	else
		addfirstnode(p);
}

// adds element at the buttom of the list
template <typename tip>
void LinkedList<tip>::addLast(tip* value)
{
	n++;
	Node<tip> *p = new Node<tip>;
	p->value = value;
	if (pFirst)
		addextremnode(p);
	else
		addfirstnode(p);
}

// inserts an element, while keeping the ascending
// order of the list
template <typename tip>
void LinkedList<tip>::addOrder(tip* value)
{
	n++;
	Node<tip> *p = new Node<tip>;
	p->value = value;
	if (pFirst)
	{
		Node<tip> *loc = pFirst;
		while (*(loc->value) < *value)
		{
			loc = loc->next;
			if (loc == pFirst)
			{
				if (*(loc->value) != *value)
					addextremnode(p);
				else 
				{
					if (freerespons)
						delete p->value;
					delete p;
				}
				return;
			}
		}
		if (*(loc->value) == *value)
		{
			if (freerespons)
				delete p->value;
			delete p;
			return;
		}
		p->prev = loc->prev;
		p->next = loc;
		loc->prev->next = p;
		loc->prev = p;
		if (loc == pFirst)
			pFirst = p;
	}
	else
		addfirstnode(p);
}

// eliminates the element at the top of the list and
// returns a pointer to it
template <typename tip>
tip* LinkedList<tip>::removeFirst()
{
	n--;
	tip* x;
	if (!pFirst)
		errormsg((char*)"ERROR: No element to be removed\n");
	if (pFirst != pFirst->next)
	{
		Node<tip> *p = pFirst;
		x = p->value;
		p->next->prev = pFirst->prev;
		p->prev->next = pFirst->next;
		pFirst = pFirst->next;
		delete p;
		return x;
	}
	return removelastnode();
}

// returns a pointer to the first element
template <typename tip>
tip* LinkedList<tip>::peekFirst()
{
	if (!pFirst)
		errormsg((char*)"ERROR: No element in list\n");
	return pFirst->value;
}

// returns a pointer to the last element
template <typename tip>
tip* LinkedList<tip>::peekLast()
{
	if (!pFirst)
		errormsg((char*)"ERROR: No element to be removed\n");
	return pFirst->prev->value;
}

// removes the last element from the list and returns
// a pointer to it
template <typename tip>
tip* LinkedList<tip>::removeLast()
{
	n--;
	tip* x;
	if (!pFirst)
		errormsg((char*)"ERROR: No element to be removed\n");
	if (pFirst != pFirst->next)
	{
		Node<tip> *p = pFirst->prev;
		x = p->value;
		p->prev->next = pFirst;
		pFirst->prev = p->prev;
		delete p;
		return x;
	}
	return removelastnode();
}

// returns number of elements in list
template <typename tip>
int LinkedList<tip>::getSize()
{
	return n;
}

// removes node knowing a pointer to it
template <typename tip>
tip* LinkedList<tip>::removeNode(Node<tip>* nod)
{
	tip* x = nod->value;
	if (n == 1)
		removelastnode();
	else
	{
		if (pFirst == nod)
			pFirst = pFirst->next;
		nod->prev->next = nod->next;
		nod->next->prev = nod->prev;
		delete nod;
	}
	n--;
	return x;
}

// searches for element and returns pointer to it
// or NULL if not found
template <typename tip>
tip* LinkedList<tip>::findNode(tip* comp)
{
	Node<tip>* p = pFirst;
	if (p)
	{
		if (*(p->value) == *comp)
			return p->value;
		p = p->next;
		while (p != pFirst)
		{
			if (*(p->value) == *comp)
				return p->value;
			p = p->next;
		}
	}
	return NULL;
}

// returns true / false if list is or not empty
template <typename tip>
bool LinkedList<tip>::isEmpty()
{
	return pFirst == NULL;
}

// returns a pointer to the first node
template <typename tip>
Node<tip>* LinkedList<tip>::begin()
{
	return pFirst;
}

// returns a pointer to the last node
template <typename tip>
Node<tip>* LinkedList<tip>::end()
{
	return pFirst->prev;
}

// output stream overload for ease of printing
template <typename tip>
ostream& operator<<(ostream& out, LinkedList<tip>& l)
{
	if (l.pFirst)
	{
		Node<tip>* p = l.pFirst;
		out << *(p->value) << ' ';
		p = p->next;
		while (p != l.pFirst)
		{
			out << *(p->value) << ' ';
			p = p->next;
		}
	}
	return out;
}

// the iterator subclass makes possible iterating through the list's 
// elements without having explicit access to the lists node pointers 

template <typename tip>
class LinkedList<tip>::iterator
{
private:
	Node<tip>* nod;
	bool fstuse;
	Node<tip>* initnod;

public:
	// initializes iterator
	iterator& operator=(Node<tip>* nod)
	{
		fstuse = true;
		this->nod = nod;
		initnod = nod;
		return *this;
	}

	// helps at doing a one cycle loop through list
	bool cycle()
	{
		bool actc = fstuse;
		fstuse = false;
		return actc;
	}

	// comparison of node pointers
	bool operator!=(Node<tip>* nod)
	{
		return this->nod != nod;
	}

	bool operator==(Node<tip>* nod)
	{
		return this->nod == nod;
	}

	// * overload returns pointer to current value of iteration
	tip* operator*()
	{
		return nod->value;
	}

	// ++ overload moves iterator forward
	iterator& operator++()
	{
		nod = nod->next;
		return *this;
	}

	// -- overload moves iterator backward
	iterator& operator--()
	{
		nod = nod->prev;
		return *this;
	}

	// += overload moves iterator x steps forward
	iterator& operator+=(unsigned int x)
	{
		for (unsigned int i = 0; i < x; i++)
			nod = nod->next;
		return *this;
	}

	// -= overload moves iterator x steps backward
	iterator& operator-=(unsigned int x)
	{
		for (unsigned int i = 0; i < x; i++)
			nod = nod->prev;
		return *this;
	}

	// returns pointer to current node
	Node<tip>* getNode()
	{
		return nod;
	}
};

#endif
