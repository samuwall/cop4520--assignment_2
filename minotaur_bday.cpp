#include <iostream> 
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <thread>
#include <atomic>
#include <sstream>
#include <ctime>
#include <chrono>

// Samu Wallace -- cop4520

// Create a program to simulate the winning strategy (protocol) where each guest is represented by one running thread.

// guests enter one at a time, at the end there is a cake.
// guest may decide whether to eat cake.
// if empty, another is brought, and guest may decide whether to eat cake.
// before party is over, all guests must announce that they have visited the labyrinth
// at least once.
// 

// N number of guests (threads)
#define N 100

std::stringstream stream;

std::atomic_flag lock = ATOMIC_FLAG_INIT;
std::atomic<bool> lock_stream = ATOMIC_FLAG_INIT;

std::atomic<int> r;
thread_local int thread_num;
bool eaten[N] = {false}; // shouldnt need to be atomic, every thread will only check their own index
unsigned int eaten_sum = 0;

// create random num gen, representing which guest gets to go in labyrinth
// have all N threads constantly check if 'r' is their number (thread_local?)
// if so, guest goes in and eats cupcake -- ONLY if they haven't before
// make sure these vars are never being modified by mult. threads (use locks)
// everytime a guest leaves (thread unlocks..) make new rand num
// keep track of when ALL N guests have visited (eaten cupcake)



void labyrinth(int i) {
	thread_num = i;
	
	// lock
	// while (1) {
	// 	if (!lock_stream.exchange(true, std::memory_order_acquire)) // test and set once available
	// 		break;
	// }
	
	while(lock_stream.exchange(true, std::memory_order_acquire)) {
		__builtin_ia32_pause();	
	}
	
	// CS
	if (eaten[thread_num] == false) {
		eaten[thread_num] = true;
		eaten_sum++;
		stream << "guest #" << thread_num << " has eaten the cupcake.\n";
		
	}
	else {
		//printf("guest #%d has not eaten the cupcake\n", thread_num);
		stream << "guest #" << thread_num << " has not eaten the cupcake, since they already had one\n";
	}
	r = rand() % N;
	
	// unlock
	lock_stream.store(false, std::memory_order_release);
}

// void thread_func(int i) {
// 	thread_num = i;
// 	while (1) {
// 		std::this_thread::sleep_for(std::chrono::milliseconds(1));
// 		if (thread_num == r) {
// 			labyrinth();
// 			break;	
// 		}
// 	}
// }


int main() {
    
    // choose which guest goes in
	srand(time(NULL));
	r = rand() % N;
    
    std::vector<std::thread> guests;
    
	
		for (int i = 0; i < N; i++) {
			guests.emplace_back(labyrinth, i);
		}
	
	
	for (auto& th : guests) th.join();
	
	std::cout << stream.str();
	
	printf("All %d guests have eaten a cupcake!\n", N);
    
	
    return 0;
}
