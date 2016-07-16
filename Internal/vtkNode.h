//
//  vtkNode.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#ifndef vtkNode_h
#define vtkNode_h

#include "vtkGraphCutDataTypes.h"

class vtkNode
{
public:
    vtkNode();
    
    vtkTreeType tree;
    int depthInTree;
    int parent;
    bool active;
    bool orphan;
    bool seedPoint;
};

#endif /* vtkNode_h */
