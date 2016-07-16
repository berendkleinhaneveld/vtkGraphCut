//
//  vtkGraphCutProtectedTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#include <assert.h>
#include <iostream>
#include <vtkPoints.h>
#include "vtkGraphCutCostFunctionSimple.h"
#include "Internal/Edge.h"
#include "Internal/Edges.h"
#include "Internal/Nodes.h"
#include "Internal/vtkNode.h"
#include "vtkGraphCutDataTypes.h"
#include "vtkGraphCutProtected.h"


void testSettingSeedPoints();
void testIncomingEdge();
void testIncomingEdgeWithConnectivity(vtkGraphCutProtected*, Nodes*, std::vector<Edge>*, int*, vtkConnectivity);


int main(int argc, char const *argv[]) {
    
    testSettingSeedPoints();
    
    return 0;
}


/**
 * Tests setting the seedpoints properties of the vtkGraphCut.
 */
void testSettingSeedPoints() {
    std::cout << __FUNCTION__ << "\n";

    vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
    
    vtkPoints* foregroundPoints = vtkPoints::New();
    vtkPoints* backgroundPoints = vtkPoints::New();

    graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

    vtkPoints* foregroundResult = graphCut->GetForegroundPoints();
    assert(foregroundPoints == foregroundResult);
    vtkPoints* backgroundResult = graphCut->GetBackgroundPoints();
    assert(backgroundPoints == backgroundResult);

    graphCut->Delete();
    
    std::cout << "Done!\n";
}
