//---------------------------------------------------------------------------
// DAWG.h
//
// A Directed Acyclic Word Graph class.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include <string>

class DAWG
{
  public:
    DAWG();
    ~DAWG();

    void addWord (const std::string& w);
    bool containsWord (const std::string& w) const;

    void print() const;

  private:
    class Node {
      public:
        Node(char c = 0, bool e = false);
        ~Node();
        char letter;
        bool eow;
        Node* next;
        Node* child;
    };

    Node* top;
};
