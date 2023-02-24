#include <iostream> 
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <thread>
#include <atomic>
#include <sstream>  
#include <ctime>

// Samu Wallace -- cop4520
// 
// many guests(threads), but only one at a time can 
// view the vase.
// three strategies for viewing the vase:
// 
// 1. stop by and check whether showroom door is open at any time and try to enter room
//      spinlock -> while(TAS(lock)) -> crit sect -> unlock()
//      the TAS spinlock is a primitive approach, inefficient alternative to mutex
//      
//      ** spins on shared bus, clears (invalidates) other threads' local cached copies of the lock, and delays unlock as bus is monopolized by spinners
// 2. guests place an 'available' or 'busy' sign on the door
//      TTAS lock (test-and-test-and-set) 
//      
//      ** this spins on local cache, freeing up the bus
// 3. guests line up in a queue, every exiting guest informs
//    next guest that the room is available. guests can queue multiple times
//      Anderson Queue Lock


#define N 100

//std::atomic_flag lock = ATOMIC_FLAG_INIT; // cannot check lock state ... use atomic<> instead
std::atomic<bool> lock_bool = ATOMIC_FLAG_INIT;
std::atomic<bool> lock_stream = ATOMIC_FLAG_INIT;
bool flags[N] = {false};
std::atomic<int> next(0);
thread_local int my_slot;

std::stringstream stream;


// strategy 1

void tas_lock(int x) {
	// lock
	while(lock_stream.exchange(true, std::memory_order_acquire)); // exchange() equiv. to test_and_set()
	
	// critical section
    stream << "guest #" << x << " looked at the vase.\n"; // use sstream in order to avoid weird parrellel printf behavior
	
	// unlock
    lock_stream.store(false, std::memory_order_release); // I used these memory order macros because the default (memory_order_seq_cst) is 
}														 // overly-restrictive and not tailored for locking/unlocking


// strategy 2 

void ttas_lock(int x) {
    
	// lock
	while (1) {
		while (lock_stream) ; // test -> spin in local cache while lock state = 1
		if (!lock_stream.exchange(true, std::memory_order_acquire)) // test and set once available
			break;
	}
	
    // critical section
	stream << "guest #" << x << " looked at the vase.\n";
    
    // unlock
    lock_stream.store(false, std::memory_order_release);
}


// strategy 3

void arr_lock_queue(int x) {
	
	// lock
	my_slot = next++;
	while (!flags[my_slot % N]) ;
	flags[my_slot % N] = false;
	
	// CS
	stream << "guest #" << x << " looked at the vase.\n";
	
	// unlock
	flags[(my_slot + 1) % N] = true;
}


int main() {
	int time_start = clock();
	
    
	flags[0] = true;
	
	//printf("%u\n", next.load());
	
    std::vector<std::thread> guests;
	
    for (int i = 0; i < N; i++) {
		guests.emplace_back(tas_lock, i);
		//guests.emplace_back(ttas_lock, i);
        //guests.emplace_back(arr_lock_queue, i);
    }
    
	for (auto& th : guests) th.join();
	
	int time_end = clock();
	
    std::cout << stream.str();
	
	printf("%lfs\n", (time_end - time_start) / double(CLOCKS_PER_SEC));
    return 0;   
}
