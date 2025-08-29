#include<iostream>
#include "BoostStore.h"

int main(){
	BoostStore multiStore(true, 2); // Multi-event store
	multiStore.Header->Set("experiment", "ANNIE");
	multiStore.Set("event1", 123);
	multiStore.Save("events.bin");

	return 0;
}
