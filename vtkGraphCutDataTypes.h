#ifndef __vtkGraphCutDataTypes_h
#define __vtkGraphCutDataTypes_h

enum vtkTreeType
{
	NONE = 0,
	SOURCE = -1,
	SINK = -2,
	INVALID = -3,
};

enum vtkConnectivity
{
	SIX = 6,
	EIGHTEEN = 18,
	TWENTYSIX = 26
};

struct vtkNode
{
	bool terminal;
	bool orphan;
	bool active;
	bool seedPoint;
	vtkIdType timeStamp;
	int depthInTree;
	vtkTreeType tree;
};


struct vtkEdge
{
	int node1;
	int node2;

	// Current flow through edge
	// Positive means from node1 to node2, negative from node2 to node1
	int flow;
	int capacity;

    void addFlowFromNode(int node, int addedFlow) {
        if (node == node1) {
            flow += addedFlow;
        } else {
            flow -= addedFlow;
        }
    }
    
	bool isSaturatedFromNode(int node) {
        return capacityFromNode(node) == 0;
	}

	int flowFromNode(int node) {
		if (node == node1) {
			return flow;
		} else {
			return -flow;
		}
	}
    
    int capacityFromNode(int node) {
        return capacity - flowFromNode(node);
    }

	bool isTerminal() {
        return node1 == SINK || node1 == SOURCE || node2 == SINK || node2 == SOURCE;
	}

};

#endif // __vtkGraphCutDataTypes_h
