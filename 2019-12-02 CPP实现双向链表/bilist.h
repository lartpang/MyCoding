#ifndef BILIST_H
#define BILIST_H

#include <iostream>
#include <string>

using namespace std;

class CNode
{
  private:
  public:
  int    lData;
  int    rData;
  CNode *prevNode;
  CNode *nextNode;

  CNode(int ldata, int rdata)
  {
    lData    = ldata;
    rData    = rdata;
    prevNode = nullptr;
    nextNode = nullptr;
  }
  ~CNode() = default;
};

class CList
{
  private:
  CNode *currentNode;
  int    numNode;
  int    currIndex;

  bool IsEmpty() { return numNode == 0; }
  bool IsFirst(int indexNode) { return indexNode == 0; }
  bool IsLast(int indexNode) { return indexNode == numNode - 1; }
  bool IsUnvalidIndex(int indexNode)
  {
    return !(indexNode >= 0 && indexNode < numNode);
  }
  static void InstertBeforeSpecialNode(CNode *newNode, CNode *targetNode);
  static void InstertBehindSpecialNode(CNode *newNode, CNode *targetNode);
  CNode *     FindWithIndex(int indexNode);

  public:
  CList();

  ~CList();

  bool InsertNode(int indexNode, int lData, int rData);
  bool AppendNode(int lData, int rData);
  bool DeleteNode(int indexNode);
  bool TraverseList();
  bool SetCurrentNode(int indexNode);
};

#endif    // BILIST_H
