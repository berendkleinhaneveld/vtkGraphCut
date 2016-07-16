//
//  Node.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#ifndef Node_h
#define Node_h

#include "vtkGraphCutDataTypes.h"

class Node
{
public:
    Node();
    
    vtkTreeType tree;
    int depthInTree;
    int parent;
    bool active;
    bool orphan;
    bool seedPoint;
};

#endif /* Node_h */
