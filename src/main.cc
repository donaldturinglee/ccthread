#include <iostream>
#include <thread>

#include "ccthread.h"
using namespace ccthread;

void producer(ccthread::ConcurrencyQueue<int>& queue) {
    for(int i = 0; i < 10; ++i) {
        queue.push(i);
        std::cout << "Producer pushed: " << i << ", size: " << queue.size() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void consumer(ccthread::ConcurrencyQueue<int>& queue) {
    for(int i = 0; i < 10; ++i) {
        auto value = queue.pop_front(true);
        if(value.has_value()) {
            std::cout << "Consumer got: " << value.value() << ", size: " << queue.size() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

int main(int argc, char* argv[]) {
	
	ccthread::ConcurrencyQueue<int> queue;

    std::thread prod(producer, std::ref(queue));
    std::thread cons(consumer, std::ref(queue));

    prod.join();
    cons.join();

	return 0;
}
