//
//  vtkGraphCutDataTypes.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

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

struct vtkNodeStatistics
{
    double minimum;
    double maximum;
    double mean;
    double variance;
    double foregroundMean;
    double foregroundVariance;
    double backgroundMean;
    double backgroundVariance;
};

#endif // __vtkGraphCutDataTypes_h
