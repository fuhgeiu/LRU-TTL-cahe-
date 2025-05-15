#include <iostream>
#include <unordered_map>
#include <chrono>

template <typename K, typename V>

class LRUCacheTTL {

    using TimePoint = std::chrono::steady_clock::time_point;

    struct Node {

        K key;
        V value;
        TimePoint expiry;
        Node* prev;
        Node* next;
        Node(const K& k, const V& v, TimePoint exp)
                : key(k), value(v), expiry(exp), prev(nullptr), next(nullptr) {}
    };

    int capacity;
    std::unordered_map<K, Node*> cache;
    Node* head;
    Node* tail;

    void addToFront(Node* node) {

        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void removeNode(Node* node) {

        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void moveToFront(Node* node) {

        removeNode(node);
        addToFront(node);
    }

    void evictIfExpired(const K& key) {

        auto it = cache.find(key);

        if (it != cache.end()) {

            Node* node = it->second;

            if (std::chrono::steady_clock::now() >= node->expiry) {

                removeNode(node);
                cache.erase(key);
                delete node;
            }
        }
    }

public:

    LRUCacheTTL(int cap) : capacity(cap) {

        head = new Node(K(), V(), TimePoint::min());
        tail = new Node(K(), V(), TimePoint::min());
        head->next = tail;
        tail->prev = head;
    }

    V get(const K& key) {

        evictIfExpired(key);
        auto it = cache.find(key);

        if (it == cache.end()) return V();

        Node* node = it->second;
        moveToFront(node);

        return node->value;
    }

    void put(const K& key, const V& value, std::chrono::seconds ttl) {

        TimePoint expireTime = std::chrono::steady_clock::now() + ttl;

        evictIfExpired(key);

        if (cache.find(key) != cache.end()) {

            Node* node = cache[key];
            node->value = value;
            node->expiry = expireTime;
            moveToFront(node);

        } else {

            if (cache.size() == capacity) {

                Node* lru = tail->prev;

                while (lru != head && std::chrono::steady_clock::now() >= lru->expiry) {

                    Node* prev = lru->prev;
                    removeNode(lru);
                    cache.erase(lru->key);
                    delete lru;
                    lru = prev;
                }

                if (lru != head) {

                    removeNode(lru);
                    cache.erase(lru->key);
                    delete lru;
                }
            }

            Node* newNode = new Node(key, value, expireTime);
            cache[key] = newNode;
            addToFront(newNode);
        }
    }

    ~LRUCacheTTL() {

        Node* curr = head;

        while (curr) {

            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }
};
