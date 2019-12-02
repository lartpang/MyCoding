#include "bilist.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
  CList biList;
  biList.TraverseList();
  biList.AppendNode(2, 4);
  biList.TraverseList();
  biList.InsertNode(1, 1, 2);
  biList.TraverseList();

  biList.SetCurrentNode(1);
  return 0;
}
