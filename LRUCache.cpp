struct Node {
    int val;
    int key;
    Node* left;
    Node* right;

    Node(int key_, int val_) : left(nullptr), right(nullptr), key(key_), val(val_) {}
};

class LRUCache {
private:
    int capacity;
    int currentSize;
    Node* head;   
    Node* tail;
    map<int, Node*> mpp;

public:
    LRUCache(int capacity) : capacity(capacity), currentSize(0) {
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->right = tail;
        tail->left = head;
    }

    void addToFront(Node* node) {
        Node* headRight = head->right;
        head->right = node;
        headRight->left = node;
        node->left = head;
        node->right = headRight;
    }

    void unlinkNode(Node* node) {
        Node* nodeLeft = node->left;
        Node* nodeRight = node->right;
        node->left = nullptr;
        node->right = nullptr;
        nodeLeft->right = nodeRight;
        nodeRight->left = nodeLeft;
    }

    void deleteNode(Node* node) {
        int key = node->key;
        mpp.erase(key);
        unlinkNode(node);
        delete(node);
    }
    
    // if key doesn't exist return -1
    // if it does exist then fetch the correct node from the map and return the value
    // also increase the currentSize
    int get(int key) {
        if(mpp.find(key) != mpp.end()) {
            Node* node = mpp[key];
            unlinkNode(node);
            addToFront(node);
            return node->val;
        }
        return -1;
    }
    
    // if couldn't find the node in the map then make one and inrease the currentSize
    // add it to the map and add the node to front
    void put(int key, int value) {
        Node* node = nullptr;

        if(mpp.find(key) != mpp.end()) {
            node = mpp[key];
            node->val = value;
            unlinkNode(node);
            addToFront(node);
            return;
        }

        if(currentSize == capacity) {
            Node* deletionNode = tail->left;
            mpp.erase(deletionNode->key);
            deleteNode(deletionNode);
            currentSize--;
        }

        node = new Node(key, value);
        addToFront(node);
        mpp[key] = node;
        currentSize++;
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */
