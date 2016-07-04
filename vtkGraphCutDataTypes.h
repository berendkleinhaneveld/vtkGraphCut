#ifndef __vtkGraphCutDataTypes_h
#define __vtkGraphCutDataTypes_h


#include <assert.h>
#include <vtkObjectFactory.h>

enum vtkTreeType
{
	NONE = 0,
	SOURCE = -1,
	SINK = -2,
	INVALID = -3,
};

enum vtkConnectivity
{
    UNCONNECTED = 0,
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
    int parent; // index of parent
};

#endif // __vtkGraphCutDataTypes_h
