#include <stack>

template <typename TKey, typename TData>
class Tree
{
public:
    virtual void add(const TKey& key, const TData& data) = 0;
    virtual void pop(const TKey& key) = 0;
    virtual TData find(const TKey& key) const = 0;
    virtual ~Tree(){};
};

template <typename TKey>
class Compare
{
public:
    virtual int doCompare(const TKey& firstKey, const TKey& secondKey) const;
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

        Node(const TKey& key, const TData& data)
        {
            this->key = key;
            this->data = data;

            leftPtr = nullptr;
            rightPtr = nullptr;

            isRed = true;
        }

        void makeRed()
        {
            this->isRed = true;
        }
        void makeBlack()
        {
            this->isRed = false;
        }

        bool nodeIsRed() const
        {
            return isRed == true;
        }
        bool nodeIsBlack() const
        {
            return isRed == false;
        }

        Node* returnAnotherChild(Node* child) const
        {
            if (leftPtr == child)
                return rightPtr;
            else if (rightPtr == child)
                return leftPtr;
            else if (leftPtr == nullptr)
                return rightPtr;
            else
                return leftPtr;

        }

        bool nodeIsNotLeaf() const
        {
            return !(leftPtr == nullptr && rightPtr == nullptr);
        }

        bool nodeIsBranch() const 
        {
            return (leftPtr && !rightPtr) || (!leftPtr && rightPtr);
        }

        bool nodeIsNotBranch() const 
        {
            return !nodeIsBranch();
        }

        Node* returnRedChildOrNullptr() const
        {
            if (leftPtr != nullptr && leftPtr->nodeIsRed())
                return leftPtr;
            if (rightPtr != nullptr && rightPtr->nodeIsRed())
                return rightPtr;
            return nullptr;
        }

        Node(const Node& node)
        {
            this->isRed = node.isRed;
            this->data = node.data;
            this->key = node.key;
            this->leftPtr = node.leftPtr;
            this->rightPtr = node.rightPtr;
        }

        bool redGrandsonExists() const
        {
            Node* leftChild = leftPtr;
            Node* rightChild = rightPtr;

            Node *leftRedNephew, *rightRedNephew;
            if (leftChild)
                leftRedNephew = leftChild->returnRedChildOrNullptr();
            if (rightChild)
                rightRedNephew = rightChild->returnRedChildOrNullptr();
            
            if (rightRedNephew || leftRedNephew)
                return true;
            return false;
        }

        Node* returnSon(Node* grandson, Compare<TKey>* _compare) const 
        {
            int compareGrandfatherAndGrandson = _compare->doCompare(key, grandson->key);
            if (compareGrandfatherAndGrandson < 0)
                return rightPtr;
            else
                return leftPtr;
        }

        Node* returnGrandsonByZigzag(Node* greatGrandfather, Compare<TKey>* _compare) const
        {
           int compareGreatGrandfatherAndGrandfather = _compare->doCompare(greatGrandfather->key, key);
           if (compareGreatGrandfatherAndGrandfather > 0)
           {
               Node* grandfather = greatGrandfather->leftPtr;
               Node* father = grandfather->rightPtr;
               return father->leftPtr;
           }
           else
           {
               Node* grandfather = greatGrandfather->rightPtr;
               Node* father = grandfather->leftPtr;
               return father->rightPtr;
           }
        }

        Node* returnSonByZigzag(Node* grandfather, Compare<TKey>* _compare) const
        {
            int compareGrandfatherAndFather =  _compare->doCompare(grandfather->key, key); 
            if (compareGrandfatherAndFather > 0)
            {
                return rightPtr;
            }
            else
            {
                return leftPtr;
            }
        }
    };

private:
    Node* head;
    Compare<TKey>* compare;
    unsigned int numberOfNodes;

public:
    RBTree(Compare<TKey>* compare);

public:
    void add(const TKey& key, const TData& data) override;
private:
    void tryAdd(const TKey& key, const TData& data);
    void initStackOfPreviousNodesInInsertOrThrowException(std::stack<Node*>& nodeStack, const TKey& keyToFind) const;
    void linkElementWithFatherInInsert(Node* child, Node* father);

public:
    void pop(const TKey& key) override;
private:
    void tryPop(const TKey& key);
    void initStackOfPreviousNodesInDeletionOrThrowException(std::stack<Node*>& nodeStack, const TKey& keyToFind) const;
    void deleteNode(Node* toDelete, Node* father);
    void deleteBranch(Node* toDelete, Node* father);
    void deleteRedLeaf(Node* toDelete, Node* father);
    void findMinimalNodeInRightBranchAndUpdateStack(std::stack<Node*>& nodeStack) const;
    void deleteLeafOrBranch(std::stack<Node*>& nodeStack);
    void deleteBranchOrRedLeaf(Node* child, Node* father);

private:
    // bool needToMakeDoubleTurn(Node* grandfather, Node* grandson) const;

public:
    TData find(const TKey& key) const override;
private:

public:
    void print(void (*function)(const TKey&, const TData&)) const;
private:
    void doPrint(void (*function)(const TKey&, const TData&), Node* startNode) const;

private:
    void hangNodesAfterTurn(Node* nodeToHang, std::stack<Node*>& nodeStack);
    void hangNodesAfterTurn(Node* nodeToHang, Node* previousNode) const;
    bool needToMakeSingleTurn(Node* grandfather, Node* grandson) const;
    void makeSingleTurn(Node* grandfather, Node* grandson) const
    ;
    void makeDoubleTurn(Node* grandfather, Node* grandson) const;
    void throwExceptionIfThereIsNoCompare() const;
    Node* pullOutNodeFromStack(std::stack<Node*>& nodeStack) const;
    Node* returnFather(Node* grandfather, Node* grandson) const;
    bool isEmpty() const;
    void swapNodes(Node* first, Node* second) const;
};


template <typename TKey, typename TData>
RBTree<TKey, TData>::RBTree(Compare<TKey>* compare)
{
    head = nullptr;
    numberOfNodes = 0;
    this->compare = compare;
}


template <typename TKey, typename TData>
void RBTree<TKey, TData>::add(const TKey& key, const TData& data)
{
    try
    {
        tryAdd(key, data);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::tryAdd(const TKey& key, const TData& data)
{
    if (isEmpty())
    {
        head = new Node(key, data);
        head->makeBlack();
        numberOfNodes = 1;
        return;
    }

    throwExceptionIfThereIsNoCompare();

    std::stack<Node*> nodeStack;
    initStackOfPreviousNodesInInsertOrThrowException(nodeStack, key);

    Node* child = new Node(key, data);
    Node* father = pullOutNodeFromStack(nodeStack);

    linkElementWithFatherInInsert(child, father);
    if (father->nodeIsBlack())
    {
        return;
    }
    
    while (father != nullptr && father->nodeIsRed())
    {
        Node* grandfather = pullOutNodeFromStack(nodeStack);
        Node* uncle = grandfather->returnAnotherChild(father);

        if (uncle == nullptr || uncle->nodeIsBlack()) // makeRotation
        {
            if (needToMakeSingleTurn(grandfather, child))
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

            child = grandfather;
        }
        father = pullOutNodeFromStack(nodeStack);
    }

    if (head->nodeIsRed())
        head->makeBlack();
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::initStackOfPreviousNodesInInsertOrThrowException(
    std::stack<Node*>& nodeStack,
    const TKey& keyToFind) const
{
    Node* nodePtr = head;

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
void RBTree<TKey, TData>::linkElementWithFatherInInsert(Node* child, Node* father)
{
    numberOfNodes++;
    compare->doCompare(father->key, child->key) < 0 ? father->rightPtr = child : father->leftPtr = child;
}


template <typename TKey, typename TData>
void RBTree<TKey,TData>::pop(const TKey& key)
{
    try
    {
        tryPop(key);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

template <typename TKey, typename TData>
void RBTree<TKey,TData>::tryPop(const TKey& key)
{
    if (isEmpty())
    {
        throw std::invalid_argument("Can't do pop. Tree is empty!");
    }

    throwExceptionIfThereIsNoCompare();

    std::stack<Node*> nodeStack;
    initStackOfPreviousNodesInDeletionOrThrowException(nodeStack, key);

    Node* child = nodeStack.top();
    if (child->nodeIsNotLeaf() && child->nodeIsNotBranch())
    {
        findMinimalNodeInRightBranchAndUpdateStack(nodeStack);
    }
    deleteLeafOrBranch(nodeStack);
}

template <typename TKey, typename TData>
void RBTree<TKey,TData>::initStackOfPreviousNodesInDeletionOrThrowException(
    std::stack<Node*> &nodeStack, 
    const TKey& keyToFind) const
{
    Node* nodePtr = head;

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
            return;
        }
    }
    throw std::invalid_argument("No element in tree!");
}

template <typename TKey, typename TData>
void RBTree<TKey,TData>::findMinimalNodeInRightBranchAndUpdateStack(std::stack<Node*>& nodeStack) const
{
    Node* child = nodeStack.top();
    Node* minimalNode = child->leftPtr;
    nodeStack.push(minimalNode);

    while (minimalNode->rightPtr)
    {
        minimalNode = minimalNode->rightPtr;
        nodeStack.push(minimalNode);
    }

    swapNodes(child, minimalNode);
}

template <typename TKey, typename TData>
void RBTree<TKey,TData>::deleteLeafOrBranch(std::stack<Node*>& nodeStack)
{
    Node* childToDelete = pullOutNodeFromStack(nodeStack);
    Node child = *childToDelete;
    Node* childPtr = &child;

    Node* father = pullOutNodeFromStack(nodeStack);

    if (numberOfNodes == 1)
    {
        delete head;
        numberOfNodes = 0;
        head = nullptr;
        return;
    }

    if (child.nodeIsRed() || child.nodeIsBranch())
    {
        deleteBranchOrRedLeaf(childToDelete, father);
        return;
    }

    // child is black
    deleteNode(childToDelete, father);
    while(father)
    {
        Node* brother = father->returnAnotherChild(childPtr);
        Node* redNephew = brother->returnRedChildOrNullptr();

        if (father->nodeIsRed())
        {
            if (redNephew)
            {
                if (needToMakeSingleTurn(father, redNephew))
                {
                    makeSingleTurn(father, redNephew);
                    hangNodesAfterTurn(brother, nodeStack);

                    brother->makeRed();
                    father->makeBlack();
                    redNephew->makeBlack();
                }
                else
                {
                    makeDoubleTurn(father, redNephew);
                    hangNodesAfterTurn(redNephew, nodeStack);

                    father->makeBlack();
                }
            } 
            else
            {
                father->makeBlack();
                brother->makeRed();
            }
            return;
        }
        else // father is black
        {
            if (brother->nodeIsRed())
            {
                Node *blackNephew, *anotherBlackNephew;

                if (brother->redGrandsonExists())
                {
                    Node* redBrotherGrandson = brother->returnGrandsonByZigzag(father, compare);
                    if (redBrotherGrandson)
                    {
                        blackNephew = brother->returnSon(redBrotherGrandson, compare);
                
                        makeDoubleTurn(father, blackNephew);
                        hangNodesAfterTurn(blackNephew, nodeStack);

                        redBrotherGrandson->makeBlack();
                        return;
                    }
                    
                    blackNephew = brother->returnSonByZigzag(father, compare);
                    redBrotherGrandson = blackNephew->returnAnotherChild(redBrotherGrandson);
                    if (redBrotherGrandson)
                    {
                        makeSingleTurn(father, brother->returnAnotherChild(blackNephew));
                        hangNodesAfterTurn(brother, nodeStack);
                        brother->makeBlack();

                        makeDoubleTurn(father, redBrotherGrandson);
                        hangNodesAfterTurn(redBrotherGrandson, brother);
                        return;
                    }
                    
                    blackNephew->makeRed();
                    brother->makeBlack();

                    blackNephew = brother->returnAnotherChild(blackNephew);
                    makeSingleTurn(father, blackNephew);
                    hangNodesAfterTurn(brother, nodeStack);
                    return;
                }
                else
                {
                    blackNephew = brother->leftPtr;
                    anotherBlackNephew = brother->rightPtr;

                    if (needToMakeSingleTurn(father, blackNephew))
                    {
                        makeSingleTurn(father, blackNephew);
                        hangNodesAfterTurn(brother, nodeStack);
                    }
                    else
                    {
                        makeDoubleTurn(father, blackNephew);
                        hangNodesAfterTurn(blackNephew, nodeStack);
                    }

                    brother->makeBlack();
                    anotherBlackNephew->makeRed();
                }
                return;
            }
            else // brother is black
            {
                if (redNephew)
                {
                    if (needToMakeSingleTurn(father, redNephew))
                    {
                        makeSingleTurn(father, redNephew);
                        hangNodesAfterTurn(brother, nodeStack);
                    }
                    else
                    {
                        makeDoubleTurn(father, redNephew);
                        hangNodesAfterTurn(redNephew, nodeStack);
                    }
                    redNephew->makeBlack();
                    return;
                }
                else
                {
                    brother->makeRed();
                    childPtr = father;
                    father = pullOutNodeFromStack(nodeStack);
                }
            }
        }
    }
}

template <typename TKey, typename TData>
void RBTree<TKey,TData>::deleteNode(Node* toDelete, Node* father)
{
    if (father->leftPtr == toDelete)
        father->leftPtr = nullptr;
    else
        father->rightPtr = nullptr;

    delete toDelete;
    numberOfNodes--;
}

template <typename TKey, typename TData>
void RBTree<TKey,TData>::deleteBranchOrRedLeaf(Node* child, Node* father)
{
    if (child->nodeIsBranch())
    {
        deleteBranch(child, father);
    }
    else
    {
        deleteRedLeaf(child, father);
    }
}

template <typename TKey, typename TData>
void RBTree<TKey,TData>:: deleteBranch(Node* toDelete, Node* father)
{
    Node* toHang;
    if (toDelete->leftPtr)
    {
        toHang = toDelete->leftPtr; 
    }
    else
    {
        toHang = toDelete->rightPtr;
    }

    if (father)
        compare->doCompare(father->key, toHang->key) < 0 ? father->rightPtr = toHang : father->leftPtr = toHang;
    else
        head = toHang;
    toHang->makeBlack();

    numberOfNodes--;
    delete toDelete;
}

template <typename TKey, typename TData>
void RBTree<TKey,TData>::deleteRedLeaf(Node* toDelete, Node* father)
{
    if (father->rightPtr == toDelete)
    {
        father->rightPtr = nullptr;
    }
    else
    {
        father->leftPtr = nullptr;
    }
    numberOfNodes--;
    delete toDelete;
}



template <typename TKey, typename TData>
TData RBTree<TKey, TData>::find(const TKey& key) const
{
    
}


template <typename TKey, typename TData>
void RBTree<TKey, TData>::print(void (*function)(const TKey&, const TData&)) const
{
    if (head)
    {
        std::cout << "Size " << numberOfNodes << std::endl; 
        doPrint(function, head);
    }
    else
        std::cout << "Tree is empty!" << std::endl;
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::doPrint(void (*function)(const TKey&, const TData&), Node* startNode) const
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
void RBTree<TKey, TData>::hangNodesAfterTurn(Node* nodeToHang, std::stack<Node*>& nodeStack)
{
    if (nodeStack.empty())
    {
        head = nodeToHang;
        return;
    }

    Node* previousNode = nodeStack.top();

    int compareNodes = compare->doCompare(previousNode->key, nodeToHang->key);
    compareNodes < 0 ? previousNode->rightPtr = nodeToHang : previousNode->leftPtr = nodeToHang;
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::hangNodesAfterTurn(Node* nodeToHang, Node* previousNode) const
{
    int compareNodes = compare->doCompare(previousNode->key, nodeToHang->key);
    compareNodes < 0 ? previousNode->rightPtr = nodeToHang : previousNode->leftPtr = nodeToHang;
}

template <typename TKey, typename TData>
bool RBTree<TKey, TData>::needToMakeSingleTurn(Node*grandfather, Node* grandson) const
{
    Node* father = returnFather(grandfather, grandson);

    int compareGrandfatherAndFather = compare->doCompare(grandfather->key, father->key);
    int compareFatherAndGrandson = compare->doCompare(father->key, grandson->key);

    return compareGrandfatherAndFather == compareFatherAndGrandson;
}

template <typename TKey, typename TData>
void RBTree<TKey, TData>::makeSingleTurn(Node* grandfather, Node* grandson) const
{
    Node* father = returnFather(grandfather, grandson);

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
void RBTree<TKey,TData>::makeDoubleTurn(Node* grandfather, Node* grandson) const
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
void RBTree<TKey,TData>::throwExceptionIfThereIsNoCompare() const
{
    if (compare == nullptr)
        throw std::overflow_error("Can't use Compare!");
}

template <typename TKey, typename TData>
typename RBTree<TKey, TData>::Node* RBTree<TKey, TData>::pullOutNodeFromStack(std::stack<Node*>& nodeStack) const
{
    if (nodeStack.size() == 0)
        return nullptr;

    Node *nodeToReturn = nodeStack.top();
    nodeStack.pop();
    return nodeToReturn;
}

template <typename TKey, typename TData>
typename RBTree<TKey,TData>::Node* RBTree<TKey, TData>::returnFather(Node* grandfather, Node* grandson) const
{
    Node *father;
    int compareGrandfatherAndGrandson = compare->doCompare(grandfather->key, grandson->key);
    compareGrandfatherAndGrandson < 0 ? father = grandfather->rightPtr : father = grandfather->leftPtr;
    return father;
}

template <typename TKey, typename TData>
bool RBTree<TKey,TData>::isEmpty() const
{
    return numberOfNodes == 0;
}

template <typename TKey, typename TData>
void RBTree<TKey,TData>::swapNodes(Node* first, Node* second) const
{
    TKey firstKey = first->key;
    TData firstData = first->data;

    first->key = second->key;
    first->data = second->data;

    second->key = firstKey;
    second->data = firstData;
}
