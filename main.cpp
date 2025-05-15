#include "LRU_TTL.h"

int main() {

}
    using namespace std::chrono_literals;

    LRUCacheTTL<std::string, std::string> cache(2);
    cache.put("a", "apple", 2s);
    cache.put("b", "banana", 4s);

    std::cout << cache.get("a") << std::endl;
    std::this_thread::sleep_for(3s);
    std::cout << cache.get("a") << std::endl;
    cache.put("c", "cherry", 3s);


