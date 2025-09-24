struct Node{
    int key;
    int val;
    int count;
    Node* next;
    Node* prev;
    Node(int key, int val){
        count = 1;
        next = NULL;
        prev = NULL;
        this->key = key;
        this->val = val;
    }
};

struct List{
public:
    Node* head;
    Node* tail;
    int size;

    List(){
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->next = tail;
        tail->prev = head;
        size = 0;
    }

    void addNode(Node* add){
        Node* headRight = head->next;
        add->prev = head;
        add->next = headRight;
        head->next = add;
        headRight->prev = add;
        this->size ++;
    }

    void delNode(Node* del){
        Node* left = del->prev;
        Node* right = del->next;
        left->next = right;
        right->prev = left;
        this->size --;
    }
};

class LFUCache {
private:
    int minf;
    int curSize;
    int maxSize;
    unordered_map<int, Node*> nodeMap;
    unordered_map<int, List*> listMap;

public:
    void updateFreqList(Node* node){
        List* list = listMap[node->count];
        list->delNode(node);
        if(node->count == minf && list->size == 0){
            minf++;
        }
        List* higherList;
        bool newList = false;
        if(listMap.find(node->count + 1) != listMap.end()){
            higherList = listMap[node->count + 1];
        } else {
            newList = true;
            higherList = new List();
        }
        (node->count)++;
        higherList->addNode(node);
        if(newList) listMap[node->count] = higherList;
        // nodeMap[node->key] = node;
    }

    LFUCache(int capacity) {
        maxSize = capacity;
        curSize = 0;
        minf = 0;
    }
    
    int get(int key) {
        if(nodeMap.find(key) !=  nodeMap.end()){
            Node* resnode = nodeMap[key];
            int res = resnode->val;
            updateFreqList(resnode);
            return res;
        }
        return -1;
    }
    
    void put(int key, int value) {
        if(maxSize == 0) return;

        if(nodeMap.find(key) != nodeMap.end()){
            Node* node = nodeMap[key];
            node->val = value;
            updateFreqList(node);
            return;
        }

        if(curSize == maxSize){
            List* delList = listMap[minf];
            Node* toBeDeleted = delList->tail->prev;
            nodeMap.erase(toBeDeleted->key);
            delList->delNode(delList->tail->prev);
            curSize--;
        }

        minf = 1;
        Node* toBeAdded = new Node(key, value);
        List* minList;
        if(listMap.find(minf) != listMap.end()){
            minList = listMap[minf];
        } else {
            minList = new List();
        }
        minList->addNode(toBeAdded);
        curSize++;
        nodeMap[key] = toBeAdded;
        listMap[minf] = minList;
    }
};

/**
 * Your LFUCache object will be instantiated and called as such:
 * LFUCache* obj = new LFUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */
