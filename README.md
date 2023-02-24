# cop4520--assignment_2

## Overall statements

Locks are implemented using C++'s atomic and the attached .exchange() , .store() , etc. functions. 

Efficiency is achieved through the streamlined design and the use of std::memory_order macros.



## Part 1: Minotaur's Birthday Party



## Part 2: Minotaur's Crystal Vase

Out of the three strategies, the guests should choose strategy 3, which employs an array-based queue lock. This is because compared to the first two strategies which employ test-and-set locking, invalidation is greatly reduced and performance is much closer to ideal, with scalable performance. The disadvantage however is that such an array-based queue lock has a high space-complexity, with the flags boolean array needing to be at least the size of the number of processes. 

If space-complexity is a problem, strategy 2 (TTAS) might be a good option. This is because, unlike the strategy 1 (TAS) spinlock which spins on a shared bus continuously invalidating other threads' local cached copies of the lock, the TTAS spinlock first spins on their own local cache while checking the lock status, freeing up the bus and causing less invalidations. 

With N = 10000, I got around 1.62 with arr_lock_queue, 1.67 with ttas_lock, and 1.68 with tas_lock. 

