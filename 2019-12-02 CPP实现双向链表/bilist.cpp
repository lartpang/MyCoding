#include "bilist.h"
#include <iostream>

using namespace std;

CList::CList()
{
  cout << "create a new bilist in the list" << endl;
  currentNode = nullptr;
  currIndex   = 0;
  numNode     = 0;
}

CList::~CList()
{
  for (int i = 0; i < numNode; ++i)
  {
    DeleteNode(i);
  }
  cout << "we have delete all node memery" << endl;
}

void CList::InstertBeforeSpecialNode(CNode *newNode, CNode *targetNode)
{
  newNode->nextNode = targetNode;
  newNode->prevNode = targetNode->prevNode;
  if (targetNode->prevNode != nullptr)
    targetNode->prevNode->nextNode = newNode;
  targetNode->prevNode = newNode;
}

void CList::InstertBehindSpecialNode(CNode *newNode, CNode *targetNode)
{
  newNode->prevNode = targetNode;
  newNode->nextNode = targetNode->nextNode;
  if (targetNode->nextNode != nullptr)
    targetNode->nextNode->prevNode = newNode;
  targetNode->nextNode = newNode;
}

CNode *CList::FindWithIndex(int indexNode)
{
  //Before using the function need to check the valid of the indexNode.
  CNode *newNode = currentNode;

  if (indexNode > currIndex)
  {
    // e.g. indexNode=4, currIndex=2
    for (int i = 0; i < indexNode - currIndex; ++i)
    {
      // from the currentNode to find forward
      newNode = newNode->nextNode;
    }
    return newNode;
  }
  else if (indexNode < currIndex)
  {
    // e.g. indexNode=0, currIndex=1
    for (int i = indexNode; i < currIndex - indexNode; ++i)
    {
      // from the currentNode to find backward
      newNode = newNode->prevNode;
    }
    return newNode;
  }
  else
  {
    // e.g. indexNode=2, currIndex=2
    // indexNode == currIndex
    return newNode;
  }
}

bool CList::InsertNode(int indexNode, int lData, int rData)
{
  // if we insert a node in the indexNode=3, the elements after the 3rd element
  // will move...
  if (!(indexNode >= 0 && indexNode <= numNode))
  {
    cerr << "It is a invalid indexNode parameter." << endl;
    return false;
  }

  if (indexNode == numNode)
  {
    AppendNode(lData, rData);
    return true;
  }

  // is not empty, here we should fine the (indexNote-1)th node
  auto * newNodePtr    = new CNode(lData, rData);
  CNode *targetNodePtr = FindWithIndex(indexNode);
  InstertBeforeSpecialNode(newNodePtr, targetNodePtr);

  if (indexNode <= currIndex && !IsEmpty())
  {
    currIndex++;
  }
  numNode++;
  return true;
}

bool CList::AppendNode(int lData, int rData)
{
  auto *newNodePtr = new CNode(lData, rData);

  if (IsEmpty())
  {
    // curr -> nullptr
    // nullptr <- curr
    currentNode = newNodePtr;
    numNode++;
  }
  else
  {
    // is not empty
    CNode *lastNodePtr = FindWithIndex(numNode - 1);
    InstertBehindSpecialNode(newNodePtr, lastNodePtr);
    numNode++;
    // currIndex not change.
  }

  return true;
}

bool CList::DeleteNode(int indexNode)
{
  if (IsEmpty() || IsUnvalidIndex(indexNode))
  {
    cerr << "It is a invalid indexNode parameter." << endl;
    return false;
  }

  if (numNode == 1)
  {
    // the list only have a node, it is just the current node
    numNode = 0;
    return true;
  }

  // numNode >= 2
  // we want to delete the current node
  if (indexNode == currIndex)
  {
    CNode *tempPtr;
    if (IsFirst(currIndex))
    {
      tempPtr               = currentNode;
      currentNode           = currentNode->nextNode;
      currentNode->prevNode = nullptr;
      delete tempPtr;
    }
    else if (IsLast(currIndex))
    {
      tempPtr               = currentNode;
      currentNode           = currentNode->prevNode;
      currentNode->nextNode = nullptr;
      delete tempPtr;
    }
    else
    {
      // the current node is in the list
      tempPtr                     = currentNode;
      currentNode                 = currentNode->nextNode;
      tempPtr->prevNode->nextNode = tempPtr->nextNode;
      tempPtr->nextNode->prevNode = tempPtr->prevNode;
      delete tempPtr;
    }
    numNode--;
    // currIndex do not change
  }
  else
  {
    // valid indexNode
    // we want to delete the non-current node
    CNode *targetNodePtr = FindWithIndex(indexNode);
    // if the target is the first
    if (targetNodePtr->prevNode == nullptr)
    {
      targetNodePtr->nextNode->prevNode = nullptr;
      delete targetNodePtr;
    }
    else if (targetNodePtr->nextNode == nullptr)
    {
      // if the target is the last
      targetNodePtr->prevNode->nextNode = nullptr;
      delete targetNodePtr;
    }
    else
    {
      // is non-head or non-tail
      targetNodePtr->nextNode->prevNode = targetNodePtr->prevNode;
      targetNodePtr->prevNode->nextNode = targetNodePtr->nextNode;
      delete targetNodePtr;
    }

    numNode--;
    if (indexNode < currIndex)
    {
      currIndex--;
    }
  }

  return true;
}

bool CList::TraverseList()
{
  // traverse the list and print all node elements
  if (IsEmpty())
  {
    cout << "the list is empty!" << endl;
    return false;
  }

  // is not empty, first find the first node.
  CNode *targetNodePtr = FindWithIndex(0);
  //    cout << "Node: " << 0 << ", lData: " << targetNode.lData
  //         << ", rData: " << targetNode.rData << endl;
  cout << targetNodePtr->lData << ", " << targetNodePtr->rData << endl;
  for (int i = 1; i < numNode; ++i)
  {
    targetNodePtr = targetNodePtr->nextNode;
    //    cout << "Node: " << i << ", lData: " << targetNode.lData
    //         << ", rData: " << targetNode.rData << endl;
    cout << targetNodePtr->lData << ", " << targetNodePtr->rData << endl;
  }

  return true;
}

bool CList::SetCurrentNode(int indexNode)
{
  if (IsUnvalidIndex(indexNode))
  {
    cerr << "the order num " << indexNode << " is a invalid indexNode parameter." << endl;
    return false;
  }

  cout << "we choose the No. " << indexNode << " node as the current node..." << endl;
  currentNode = FindWithIndex(indexNode);
  currIndex   = indexNode;
  return true;
}
