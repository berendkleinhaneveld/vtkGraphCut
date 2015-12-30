//
//  vtkPrioQueueTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/7/12.
//
//

#include <stdio.h>
#include <assert.h>
#include "vtkPrioQueue.h"

class Object {
public:
	int priority;
};

int main() {
	int result = 0;
	
	/**
	 static vtkPrioQueue* New();
	 vtkTypeMacro(vtkPrioQueue, vtkObject);
	 void PrintSelf(ostream& os, vtkIndent indent);
	 
	 void Reset();
	 
	 void Push(void* obj, double priority);
	 void Push(void* obj);
	 void* Pop();
	 void* Pop(double priority);
	 void* Pop(vtkIdType index);
	 void* Peek();
	 
	 double GetPriority(vtkIdType index);
	 vtkIdType GetNumberOfItems();
	 
	 */
	
	vtkPrioQueue* queue = vtkPrioQueue::New();
	// Test for initial state
	assert(queue != NULL);
	assert(queue->GetNumberOfItems() == 0);
	assert(queue->GetReferenceCount() > 0);
	assert(queue->Peek() == NULL);
	
	queue->Push(new Object());
	
	assert(queue->GetNumberOfItems() == 1);
	assert(queue->Peek() != NULL);
	
	Object* obj = (Object*)queue->Pop();
	
	assert(obj != NULL);
	assert(queue->GetNumberOfItems() == 0);
	delete obj;
	
	int numberOfItems = 10;
	for (int i = numberOfItems; i > 0; i--) {
		Object* obj = new Object();
		obj->priority = i;
		queue->Push(obj, obj->priority);
	}
	
	assert(queue->GetNumberOfItems() == numberOfItems);
	obj = (Object*)queue->Peek();
	assert(obj->priority == 1);
	
	queue->Print(std::cout);
	
	obj = (Object*)queue->Pop(3.0);
	assert(obj->priority == 3);
	obj = (Object*)queue->Pop(10.0);
	assert(obj->priority == 10);
	obj = (Object*)queue->Pop(1.0);
	assert(obj->priority == 1);
	
	for (int i = 0; i < 10; i++) {
		Object* obj = new Object();
		obj->priority = i;
		queue->Push(obj, obj->priority);
	}
	
	queue->Print(std::cout);
	
	queue->Delete();
	
	assert(queue->GetReferenceCount() == 0);
	
	return result;
}