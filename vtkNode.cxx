//
//  vtkNode.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#include "vtkNode.h"

vtkNode::vtkNode() {
    active = false;
    orphan = false;
    seedPoint = false;
    depthInTree = -1;
    tree = NONE;
    parent = INVALID;
}