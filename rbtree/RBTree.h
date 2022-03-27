#include <stack>

template <typename TKey, typename TData>
class Tree
{
public:
    virtual void add(const TKey &key, const TData &data) = 0;
    virtual void pop(const TKey &key) = 0;
    virtual TData find(const TKey &key) const = 0;
    virtual ~Tree(){};
};

template <typename TKey>
class Compare
{
public:
    virtual int doCompare(const TKey &firstKey, const TKey &secondKey) const;
};

template <typename TKey, typename TData>
class RBTree : public Tree<TKey, TData>
{
private:
    class Node
    {
    public:
        TKey key;
        TData data;
        Node *leftPtr, *rightPtr;
        bool isRed;

    public:
        Node(const TKey &key, const TData &data)
        {
            this->key = key;
            this->data = data;

            leftPtr = nullptr;
            rightPtr = nullptr;

            isRed = true;
        }

    public:
        void makeRed()
        {
            this->isRed = true;
        }
        void makeBlack()
        {
            this->isRed = false;
        }

    public:
        bool nodeIsRed()
        {
            return isRed == true;
        }
        bool nodeIsBlack()
        {
            return isRed == false;
        }

    public:
        Node *returnAnotherChild(Node *child)
        {
            if (leftPtr == child)
                return rightPtr;
            else
                return leftPtr;
        }
    };

private:
    Node *head;
    Compare<TKey> *compare;
    unsigned int numberOfNodes;

public:
    RBTree(Compare<TKey> *compare);

public:
    void add(const TKey &key, const TData &data) override;

private:
    void tryAdd(const TKey &key, const TData &data);
    void initStackOfPreviousNodesOrThrowException(std::stack<Node *> &nodeStack, const TKey &keyToFind) const;
    void linkElementWithFatherInInsert(Node *child, Node *father) const;
    bool needToMakeSingleTurnInInsert(Node *grandfather, Node *grandson) const;

public:
    void pop(const TKey &key) override;

private:
    // bool needToMakeDoubleTurn(Node* grandfather, Node* grandson) const;

public:
    TData find(const TKey &key) const override;

private:
public:
    void print(void (*function)(const TKey &, const TData &)) const;

private:
    void doPrint(void (*function)(const TKey &, const TData &), Node *startNode) const;

private:
    void hangNodesAfterTurn(Node *nodeToHang, std::stack<Node *> &nodeStack);
    void makeSingleTurn(Node *grandfather, Node *grandson) const;
    void makeDoubleTurn(Node *grandfather, Node *grandson) const;
    void throwExceptionIfThereIsNoCompare() const;
    Node *pullOutNodeFromStack(std::stack<Node *> &nodeStack) const;
    Node *returnFather(Node *grandfather, Node *grandson) const;
    bool isEmpty() const;
};

template <typename TKey, typename TData>
RBTree<TKey, TData>::RBTree(Compare<TKey> *compare)
{
    head = nullptr;
    numberOfNodes = 0;
    this->compare = compare;
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::add(const TKey &key, const TData &data)
{
    try
    {
        tryAdd(key, data);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::tryAdd(const TKey &key, const TData &data)
{
    if (isEmpty())
    {
        head = new Node(key, data);
        head->makeBlack();
        numberOfNodes = 1;
        return;
    }

    throwExceptionIfThereIsNoCompare();

    std::stack<Node *> nodeStack;
    initStackOfPreviousNodesOrThrowException(nodeStack, key);

    Node *child = new Node(key, data);
    Node *father = pullOutNodeFromStack(nodeStack);

    linkElementWithFatherInInsert(child, father);
    if (father->nodeIsBlack())
    {
        return;
    }
    else
    {
        while (father != nullptr && father->nodeIsRed())
        {
            Node *grandfather = pullOutNodeFromStack(nodeStack);
            Node *uncle = grandfather->returnAnotherChild(father);

            if (uncle == nullptr || uncle->nodeIsBlack()) // makeRotation
            {
                if (needToMakeSingleTurnInInsert(grandfather, child))
                {
                    makeSingleTurn(grandfather, child);
                    hangNodesAfterTurn(father, nodeStack);

                    father->makeBlack();
                }
                else
                {
                    makeDoubleTurn(grandfather, child);
                    hangNodesAfterTurn(child, nodeStack);

                    child->makeBlack();
                }
                grandfather->makeRed();
                return;
            }
            else // make repaint
            {
                father->makeBlack();
                uncle->makeBlack();
                grandfather->makeRed();
            }

            child = father;
            father = pullOutNodeFromStack(nodeStack);
        }
    }

    if (head->nodeIsRed())
        head->makeBlack();
    numberOfNodes++;
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::initStackOfPreviousNodesOrThrowException(
    std::stack<Node *> &nodeStack,
    const TKey &keyToFind) const
{
    Node *nodePtr = head;

    while (nodePtr)
    {
        nodeStack.push(nodePtr);
        int compareResult = compare->doCompare(nodePtr->key, keyToFind);

        if (compareResult < 0)
        {
            nodePtr = nodePtr->rightPtr;
        }
        else if (compareResult > 0)
        {
            nodePtr = nodePtr->leftPtr;
        }
        else
        {
            throw std::invalid_argument("This node already exists!");
        }
    }
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::linkElementWithFatherInInsert(Node *child, Node *father) const
{
    compare->doCompare(father->key, child->key) < 0 ? father->rightPtr = child : father->leftPtr = child;
}

template <typename TKey, typename TData>
bool RBTree<TKey, TData>::needToMakeSingleTurnInInsert(Node *grandfather, Node *grandson) const
{
    Node *father = returnFather(grandfather, grandson);

    int compareGrandfatherAndFather = compare->doCompare(grandfather->key, father->key);
    int compareFatherAndGrandson = compare->doCompare(father->key, grandson->key);

    return compareGrandfatherAndFather == compareFatherAndGrandson;
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::pop(const TKey &key)
{
}

template <typename TKey, typename TData>
TData RBTree<TKey, TData>::find(const TKey &key) const
{
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::print(void (*function)(const TKey &, const TData &)) const
{
    if (head)
        doPrint(function, head);
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::doPrint(void (*function)(const TKey &, const TData &), Node *startNode) const
{
    if (startNode->isRed)
        std::cout << "Red ";
    else
        std::cout << "Black ";

    function(startNode->key, startNode->data);

    if (startNode->leftPtr)
    {
        doPrint(function, startNode->leftPtr);
    }
    if (startNode->rightPtr)
    {
        doPrint(function, startNode->rightPtr);
    }
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::hangNodesAfterTurn(Node *nodeToHang, std::stack<Node *> &nodeStack)
{
    if (nodeStack.empty())
    {
        head = nodeToHang;
        return;
    }

    Node *previousNode = pullOutNodeFromStack(nodeStack);

    int compareNodes = compare->doCompare(previousNode->key, nodeToHang->key);
    compareNodes < 0 ? previousNode->rightPtr = nodeToHang : previousNode->leftPtr = nodeToHang;
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::makeSingleTurn(Node *grandfather, Node *grandson) const
{
    Node *father = returnFather(grandfather, grandson);

    if (father->rightPtr == grandson)
    {
        grandfather->rightPtr = father->leftPtr;
        father->leftPtr = grandfather;
    }
    else
    {
        grandfather->leftPtr = father->rightPtr;
        father->rightPtr = grandfather;
    }
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::makeDoubleTurn(Node *grandfather, Node *grandson) const
{
    Node *father = returnFather(grandfather, grandson);

    if (father->leftPtr == grandson)
    {
        father->leftPtr = grandson->rightPtr;
        grandfather->rightPtr = grandson->leftPtr;

        grandson->rightPtr = father;
        grandson->leftPtr = grandfather;
    }
    else
    {
        father->rightPtr = grandson->leftPtr;
        grandfather->leftPtr = grandson->rightPtr;

        grandson->leftPtr = father;
        grandson->rightPtr = grandfather;
    }
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::throwExceptionIfThereIsNoCompare() const
{
    if (compare == nullptr)
        throw std::overflow_error("Can't use Compare!");
}

template <typename TKey, typename TData>
typename RBTree<TKey, TData>::Node *RBTree<TKey, TData>::pullOutNodeFromStack(std::stack<Node *> &nodeStack) const
{
    if (nodeStack.size() == 0)
        return nullptr;

    Node *nodeToReturn = nodeStack.top();
    nodeStack.pop();
    return nodeToReturn;
}

template <typename TKey, typename TData>
typename RBTree<TKey, TData>::Node *RBTree<TKey, TData>::returnFather(Node *grandfather, Node *grandson) const
{
    Node *father;
    int compareGrandfatherAndGrandson = compare->doCompare(grandfather->key, grandson->key);
    compareGrandfatherAndGrandson < 0 ? father = grandfather->rightPtr : father = grandfather->leftPtr;
    return father;
}

template <typename TKey, typename TData>
bool RBTree<TKey, TData>::isEmpty() const
{
    return numberOfNodes == 0;
}
