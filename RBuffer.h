#pragma once

#include <avr/io.h>

#define RBUFFER_INDEX_INC(x) (x = (((x) >= ((N)-1)) ? (0) : (x+1)))

// use template to set ring buffer data type and size
template <class T, int N>
class RBuffer{
private:
	// buffer
	T rbuffer[N];
	// indexes
	int write_index;
	int read_index;
	// number of elements & sum
	int num_ele;
	long sum;
	// flags
	bool overflow;
public:
	RBuffer(){
		// Constructor
		// initialize private variables
		for(int i = 0; i < N; i++)
			rbuffer[i] = 0;
		write_index = 0;
		read_index = 0;
		num_ele = 0;
		sum = 0;
		overflow = false;
	}
	int push(T val){
		// Add value to ring buffer
		// update sum, write value to ring buffer, increment write index
		// returns number of available slots in buffer 
		sum += val - rbuffer[write_index];
		rbuffer[write_index] = val;
		RBUFFER_INDEX_INC(write_index);
		// update number of elements if possible
		// if not, then data corruption has occured; increment read index
		if(num_ele < N)
			num_ele++;
		else{
			// overflow condition
			RBUFFER_INDEX_INC(read_index);
			overflow = true;
		}
		return N-num_ele;
	}
	bool pop(T &val){
		// Remove value from ring buffer
		// copies popped value to val, update sum, increments read index
		// returns true if success, returns false if failure
		if(num_ele){
			val = rbuffer[read_index];
			RBUFFER_INDEX_INC(read_index);
			sum -= val;
			num_ele--;
			return true;
		} else{
			return false;
		}
	}
	int flush(T *ptr, int max_num){
		// outputs ring buffer to provided array
		// stops when ring buffer is empty, or reaches max number of elements
		// returns number of elements copied
		T cur_val = 0;
		for(int i = 1; i <= max_num; i++){
			if(pop(cur_val)){
				// ring buffer not empty
				*(ptr++) = cur_val;
			}else{
				// ring buffer is empty
				return i-1;
			}
		}
		// reached max number limit
		return max_num;
	}
	int get_num_ele(){
		return num_ele;
	}
	long get_sum(){
		return sum;
	}
	bool get_overflow(){
		return overflow;
	}
	void clear_overflow(){
		overflow = false;
	}
	float get_average(){
		// Returns average of elements in ring buffer
		// Returns 0 if no elements
		if(num_ele)
			return (sum / num_ele);
		else
			return 0;
	}
	int get_size(){
		return N;
	}
};

