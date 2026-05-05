#ifndef NODEBOOLTREE_H
#define NODEBOOLTREE_H

#include "BBV.h"
#include "boolinterval.h"
#include "boolequation.h"
#include "Allocator.h"

class NodeBoolTree
{
    DECLARE_ALLOCATOR
public:
    NodeBoolTree(BoolEquation *equation){this->eq = equation;}
    NodeBoolTree(const NodeBoolTree &node){
        this->eq = node.eq;
        this->lt = node.lt;
        this->rt = node.rt;}
    
	NodeBoolTree *lt = nullptr, *rt = nullptr;

	BoolEquation *eq;
};

IMPLEMENT_ALLOCATOR(NodeBoolTree, 0, 0)

#endif // NODEBOOLTREE_H
