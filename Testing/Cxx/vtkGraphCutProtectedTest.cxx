//
//  vtkGraphCutProtectedTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#include <assert.h>
#include <vtkPoints.h>
#include "vtkGraphCutProtected.h"


void testSettingSeedPoints();


int main() {
    testSettingSeedPoints();
    
    return 0;
}


/**
 * Tests setting the seedpoints properties of the vtkGraphCut.
 */
void testSettingSeedPoints() {
    vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
    
    vtkPoints* foregroundPoints = vtkPoints::New();
    vtkPoints* backgroundPoints = vtkPoints::New();

    graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

    vtkPoints* foregroundResult = graphCut->GetForegroundPoints();
    assert(foregroundPoints == foregroundResult);
    vtkPoints* backgroundResult = graphCut->GetBackgroundPoints();
    assert(backgroundPoints == backgroundResult);

    graphCut->Delete();
}
