//
//  vtkPrioQueueTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/7/12.
//
//

#include <stdio.h>
#include <assert.h>
#include "vtkGraphCutNodeQueue.h"
#include "vtkGraphCutNode.h"

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
	
	vtkGraphCutNodeQueue* queue = vtkGraphCutNodeQueue::New();
	// Test for initial state
	assert(queue != NULL);
	assert(queue->GetNumberOfItems() == 0);
	assert(queue->GetReferenceCount() > 0);
	assert(queue->Peek() == NULL);
	
	vtkGraphCutNode* node = new vtkGraphCutNode();
	node->SetDepthInTree(0);
	queue->Push(node);
	
	assert(queue->GetNumberOfItems() == 1);
	assert(queue->Peek() != NULL);
	
	vtkGraphCutNode* obj = (vtkGraphCutNode*)queue->Pop();
	
	assert(obj != NULL);
	assert(queue->GetNumberOfItems() == 0);
	delete obj;
	
	int numberOfItems = 1000;
	for (int i = numberOfItems; i > 0; i--) {
		vtkGraphCutNode* obj = new vtkGraphCutNode();
		obj->SetDepthInTree(i);
//		obj->priority = i;
		queue->Push(obj);
	}
	
	assert(queue->GetNumberOfItems() == numberOfItems);
	obj = (vtkGraphCutNode*)queue->Peek();
	assert(obj->GetDepthInTree() == 1.0);
	
//	queue->Print(std::cout);
	
//	obj = (Object*)queue->Pop(3.0);
//	assert(obj->priority == 3);
//	obj = (Object*)queue->Pop(10.0);
//	assert(obj->priority == 10);
//	obj = (Object*)queue->Pop(1.0);
//	assert(obj->priority == 1);
	
	for (int i = 0; i < 10000000; i++) {
		vtkGraphCutNode* obj = new vtkGraphCutNode();
		obj->SetDepthInTree((int)(100.0 * (rand()/(float)RAND_MAX)));
		queue->Push(obj);
	}
	
	double depth = 0.0;
	while (queue->GetNumberOfItems() > 0) {
		vtkGraphCutNode* node = (vtkGraphCutNode*)queue->Pop();
		assert(depth <= node->GetDepthInTree());
		depth = node->GetDepthInTree();
//		std::cout << "Priority: " << node->GetDepthInTree() << std::endl;
		delete node;
	}
	
//	queue->Print(std::cout);
	
	queue->Delete();
	
	assert(queue->GetReferenceCount() == 0);
	
	std::cout << "Succes" << std::endl;
	
	return result;
}