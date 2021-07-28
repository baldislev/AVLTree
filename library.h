#ifndef ALVTREE_LIBRARY_H
#define ALVTREE_LIBRARY_H

#include <iostream>

using namespace std;

template <class K, class T> class AVLTree {
    class Node{
        K key;
        T data;
        Node* parent;
        Node* lchild;
        Node* rchild;
        int height;
    public:
        explicit Node(K key): key(key), data(0), parent(nullptr),
                              rchild(nullptr), lchild(nullptr), height(-1){};

        Node(K key, T data): key(key), data(data), parent(nullptr),
        rchild(nullptr), lchild(nullptr), height(-1){};

        friend AVLTree;
    };

    /*AVLTree private fields:*/

    Node* root;

    /*AVLTree private methods:*/

    Node* update(K key, T data){
        if(!this->root){
            root = new Node(key, data);
            return root;
        }
        Node* traverse = root;
        Node* toAppend = nullptr;
        while(traverse){
            //Existing Node will be updated if the cond holds:
            if(traverse->key == key){
                traverse->data = data;
                return nullptr;
            }
            if(traverse->key < key){
                if(!traverse->rchild) {
                    toAppend = new Node(key, data);
                    traverse->rchild = toAppend;
                    toAppend->parent = traverse;
                    return toAppend;
                }
                traverse = traverse->rchild;
            } else {
                if(!traverse->lchild){
                    toAppend = new Node(key, data);
                    traverse->lchild = toAppend;
                    toAppend->parent = traverse;
                    return toAppend;
                }
                traverse = traverse->lchild;
            }
        }
    }


    Node* SuccNode(Node* curr){
        if(!curr->rchild){
            return curr;
        }
        Node* Succ = curr->rchild;
        while(Succ->lchild){
            Succ = Succ->lchild;
        }
        return Succ;
    }

    int calcBF(Node* node){
        int lheight = node->lchild ? node->lchild->height : -1;
        int rheight = node->rchild ? node->rchild->height : -1;
        return lheight - rheight;
    }

    /*the Node has zero or one child*/
    void extractFromTheChain(Node* toBeExtracted, Node* child){
        Node* parent = toBeExtracted->parent;
        if(!parent){
            root = child;
            child->parent = nullptr;
            return;
        }

        if(parent->lchild == toBeExtracted){
            parent->lchild = child;
        } else{
            parent->rchild = child;
        }

        if(child){
            child->parent = parent;
        }

        //parent->height--;
        while(parent){
            updateHeight(parent);
            parent = parent->parent;
        }
    }

    void rotateLL(Node* node){
        //cout<<"LL"<<endl;
        Node* pivot = node->lchild;
        Node* leftOfNode = pivot->rchild;

        if(node == root){
            root = pivot;
        } else{
            if(node->parent->rchild == node){
                node->parent->rchild = pivot;
            } else {
                node->parent->lchild = pivot;
            }
        }

        pivot->parent = node->parent;

        pivot->rchild = node;
        node->parent = pivot;

        node->lchild = leftOfNode;
        if(leftOfNode) {
            leftOfNode->parent = node;
        }
        updateHeight(node);
        updateHeight(pivot);
    }

    void rotateRL(Node* node){
        //cout<<"RL"<<endl;
        Node* pivot = node->rchild->lchild;
        Node* right = node->rchild;

        if(node == root) {
            root = pivot;
        } else {
            if(node->parent->rchild == node) {
                node->parent->rchild = pivot;
            } else {
                node->parent->lchild = pivot;
            }
        }

        pivot->parent = node->parent;

        right->lchild = pivot->rchild;
        if(pivot->rchild){
            pivot->rchild->parent = right;
        }

        node->rchild = pivot->lchild;
        if(pivot->lchild) {
            pivot->lchild->parent = node;
        }

        pivot->lchild = node;
        node->parent = pivot;

        pivot->rchild = right;
        right->parent = pivot;

        updateHeight(node);
        updateHeight(right);
        updateHeight(pivot);
    }

    void rotateRR(Node* node){
        //cout<<"RR"<<endl;
        Node* pivot = node->rchild;
        Node* rightOfNode = pivot->lchild;

        if(node == root){
            root = pivot;
        } else{
            if(node->parent->rchild == node){
                node->parent->rchild = pivot;
            } else {
                node->parent->lchild = pivot;
            }
        }

        pivot->parent = node->parent;

        pivot->lchild = node;
        node->parent = pivot;

        node->rchild = rightOfNode;
        if(rightOfNode) {
            rightOfNode->parent = node;
        }
        updateHeight(node);
        updateHeight(pivot);
    }

    void rotateLR(Node* node){
        //cout<<"LR"<<endl;
        Node* pivot = node->lchild->rchild;
        Node* left = node->lchild;

        if(node == root) {
            root = pivot;
        } else {
            if(node->parent->rchild == node) {
                node->parent->rchild = pivot;
            } else {
                node->parent->lchild = pivot;
            }
        }

        pivot->parent = node->parent;

        left->rchild = pivot->lchild;
        if(pivot->lchild){
            pivot->lchild->parent = left;
        }

        node->lchild = pivot->rchild;
        if(pivot->rchild) {
            pivot->rchild->parent = node;
        }

        pivot->rchild = node;
        node->parent = pivot;

        pivot->lchild = left;
        left->parent = pivot;

        updateHeight(node);
        updateHeight(left);
        updateHeight(pivot);
    }

    bool decideRotation(Node* target) {
        int bf = calcBF(target);
        if (bf == 2) {
            if (calcBF(target->lchild) >= 0) {
                rotateLL(target);
            } else {
                rotateLR(target);
            }
            return true;
        } else if (bf == -2) {
            if (calcBF(target->rchild) <= 0) {
                rotateRR(target);
            } else {
                rotateRL(target);
            }
            return true;
        }
        return false;
    }

    void repairAfterInsert(Node* node){
        Node* parent = node->parent;
        //start is a leaf;
        node->height = 0;
        //int bf = 0;
        //heights of the nodes in the corresponding subtree inc by 1
        while(node != this->root){
            if(parent->height >= node->height +1){
                break;
            }
            parent->height = node->height + 1;
            if(decideRotation(parent)) break;
            node = parent;
            parent = parent->parent;
        }
    }

    int updateHeight(Node* node){
        int lheight = node->lchild ? node->lchild->height : -1;
        int rheight = node->rchild ? node->rchild->height : -1;
        node->height = lheight >= rheight ? lheight + 1 : rheight + 1;
        return node->height;
    }

    void repairAfterRemove(Node* node){
        if(!node) return;
        int oldHeight = -1;
        //int newHeight = -1;
        int bf = -1;
        while(node){
            oldHeight = updateHeight(node);
            bf = calcBF(node);
            if(bf == 2 || bf == -2){
                oldHeight = node->height;
                decideRotation(node);
                if(oldHeight == node->height){
                    break;
                }
            }
            node = node->parent;
        }
    }

    Node* deleteNode(Node* target){
        Node* parentOfDeleted = nullptr;
        /*Both children are empty*/
        if(!target->lchild && !target->rchild){
            if(target == root){
                root = nullptr;
            } else{
                extractFromTheChain(target, nullptr);
            }
            parentOfDeleted = target->parent;
            delete target;
        }
            /*Both children are kinda alive*/
        else if(target->rchild && target->lchild){
            Node* succ = SuccNode(target);
            K key = succ->key;
            T data = succ->data;
            parentOfDeleted = deleteNode(succ);
            target->key = key;
            target->data = data;
        }
            /*One of the offsprings is there*/
        else{
            Node* childNode = target->lchild ? target->lchild : target->rchild;
            if(target == root){
                root = childNode;
                childNode->parent = nullptr;
            }else{
                childNode->parent = target->parent;
                extractFromTheChain(target, childNode);
            }
            parentOfDeleted = target->parent;
            delete target;
        }
        return parentOfDeleted;
    }

    void printNode(Node* node){
        if(node == nullptr) return;
        printNode(node->lchild);
        cout<<"KEY: "<<node->key<<" Height: "<<node->height<<endl;
        printNode(node->rchild);
    }

    void clearNode(Node* iterator) {
        if(iterator == nullptr) return;
        clearNode(iterator->lchild);
        clearNode(iterator->rchild);
        delete iterator;
    }

public:
    AVLTree(){
        root = nullptr;
    }

    ~AVLTree(){
        this->treeClear();
    }

    //if key exists then the Node that holds it will be updated.
    void insert(K key, T data){
        Node* inserted = update(key, data);
        //in case a new node was inserted may need to rotate the subtree:
        if(inserted){
            repairAfterInsert(inserted);
        }
    }

    void remove(K key){
        Node* target = root;
        Node* parentOfTarget = nullptr;
        while(target){
            if(target->key == key){
                parentOfTarget = deleteNode(target);
                break;
            } else if(target->key < key){
                target = target->rchild;
            } else{
                target = target->lchild;
            }
        }
        repairAfterRemove(parentOfTarget);

    }

    void printTree(){
        cout<<"START TREE"<<endl;
        printNode(this->root);
        cout<<"END TREE"<<endl;
    }

    void treeClear(){
        clearNode(this->root);
        this->root = nullptr;
    }

};

#endif