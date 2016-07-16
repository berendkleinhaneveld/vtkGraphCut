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
#include "vtkEdge.h"
#include "vtkEdges.h"
#include "vtkNode.h"
#include "vtkGraphCutDataTypes.h"
#include "vtkGraphCutProtected.h"
#include "vtkNodes.h"


void testSettingSeedPoints();
void testIncomingEdge();
void testIncomingEdgeWithConnectivity(vtkGraphCutProtected*, vtkNodes*, std::vector<vtkEdge>*, int*, vtkConnectivity);


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
