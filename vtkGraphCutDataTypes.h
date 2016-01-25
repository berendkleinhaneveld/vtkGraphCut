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
	int flow;
	int capacity;
};

#endif // __vtkGraphCutDataTypes_h
