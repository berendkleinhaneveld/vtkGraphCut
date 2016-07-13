//
//  vtkGraphCutDataTypes.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#ifndef __vtkGraphCutDataTypes_h
#define __vtkGraphCutDataTypes_h


enum vtkTreeType
{
    TREE_NONE = 0,
    TREE_SOURCE = -1,
    TREE_SINK = -2,
    TREE_INVALID = -3,
};


enum NodeIndex
{
    NODE_SOURCE = -1,
    NODE_SINK = -2,
    NODE_NONE = -3,
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
