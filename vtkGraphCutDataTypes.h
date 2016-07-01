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


/**
 * vtkEdge describes an edge between two vtkNodes.
 * The nodes are not saved by reference or pointer
 * but by index.
 * A vtkEdge can have a certain capacity and a flow.
 * The capacity and flow are dependent on the direction,
 * so use the methods *fromNode to manipulate these
 * values.
 */
class vtkEdge
{
    // Current flow through edge
    // Positive means from node1 to node2, negative from node2 to node1
    int _node1;
    int _node2;
    int _capacity;
    int _flow;

public:
    
    /**
     * Constructor
     */
    vtkEdge(int firstNode, int secondNode) {
        _node1 = firstNode;
        _node2 = secondNode;
        _capacity = 0;
        _flow = 0;
    }
    
    /**
     * Returns node1 as an index.
     */
    int node1() {
        return _node1;
    }
    
    /**
     * Returns node2 as an index.
     */
    int node2() {
        return _node2;
    }

    /**
     * Returns the non-root node when this edge
     * is a terminal node. Otherwise returns INVALID.
     */
    int nonRootNode() {
        assert(isTerminal());
        if (_node1 < 0) {
            return _node2;
        }
        if (_node2 < 0) {
            return _node1;
        }
        return INVALID;
    }
    
     * Returns whether this edge is connected to eiter a
     * SOURCE or a SINK node.
     */
    bool isTerminal() {
        return _node1 == SINK || _node1 == SOURCE || _node2 == SINK || _node2 == SOURCE;
    }
    
    /**
     * Returns whether this node is a valid node.
     */
    bool isValid() {
        return _node1 != INVALID && _node2 != INVALID;
    }
    
    /**
     * Set the total capacity that this edge can hold.
     * This capacity is the max capacity in both directions.
     */
    void setCapacity(int cap) {
        _capacity = cap;
    }
    
    /**
     * Adds the given amount of flow to the current flow
     * from the direction of the given node.
     */
    void addFlowFromNode(int node, int addedFlow) {
        assert(node == _node1 || node == _node2);
        assert(capacityFromNode(node) >= addedFlow);
        if (node == _node1) {
            _flow += addedFlow;
        } else {
            _flow -= addedFlow;
        }
    }
    
    /**
     * Returns whether the edge is saturated as seen from
     * the given node.
     */
	bool isSaturatedFromNode(int node) {
        return capacityFromNode(node) == 0;
	}

    /**
     * Returns the current flow from a give node.
     */
	int flowFromNode(int node) {
		if (node == _node1) {
			return _flow;
		} else {
			return -_flow;
		}
	}
    
    /**
     * Returns the capacity of the edge that is left
     * in the given direction.
     */
    int capacityFromNode(int node) {
        return _capacity - flowFromNode(node);
    }

};

#endif // __vtkGraphCutDataTypes_h
