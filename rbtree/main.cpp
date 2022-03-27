#include <iostream>
#include "RBTree.h"

void doPrint(const int &key, const std::string &data)
{
    std::cout << key << " " << data << std::endl;
}

class ComapreForInt : public Compare<int>
{
public:
    int doCompare(const int &firstKey, const int &secondKey) const override
    {
        if (firstKey > secondKey)
            return 1;
        if (firstKey < secondKey)
            return -1;
        return 0;
    }
};

int main()
{
    ComapreForInt comapreForInt;
    RBTree<int, std::string> tree(&comapreForInt);

    tree.add(20, "))");
    // tree.print(doPrint);

    tree.add(15, "(((");
    // tree.print(doPrint);

    tree.add(25, "))))");
    // tree.print(doPrint);

    tree.add(10, "!!!!");
    // tree.print(doPrint);
    
    tree.add(17, "@@@@");
    // tree.print(doPrint);

    tree.add(5, "hello");
    // tree.print(doPrint);

    tree.add(13, "hjllo");
    // tree.print(doPrint);

    tree.add(18, "nike");
    // tree.print(doPrint);

    tree.add(1, "adidas");
    // tree.print(doPrint);

    tree.add(19, "puma");
    // tree.print(doPrint);

    tree.add(0, "puma");
    // tree.print(doPrint);

    tree.add(30, "puma");
    // tree.print(doPrint);

    tree.add(35, "puma");
    // tree.print(doPrint);

    tree.add(40, "puma");
    tree.print(doPrint);

    system("pause");
    return 0;
}