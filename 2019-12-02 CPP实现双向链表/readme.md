# CPP实现双向链表

在CPP11的环境下编写调试编译正常。

使用自定义的双向链表结构。

两个核心类：`CNode`和`CList`。

```cpp
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
```

```cpp
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
```

## 感谢

* https://blog.csdn.net/iteye_3759/article/details/82574207

一些隐秘的Bug参考上文得到了启发，进行了修改。
