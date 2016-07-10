//
//  vtkGraphCutTestApp.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#include <assert.h>
#include <iostream>
#include "vtkGraphCut.h"
#include "vtkGraphCutCostFunctionSimple.h"


// Test methods
void testGraphCutReset();
void testCostFunctionSimple();
void testBasicRunThrough();

// Convenience method for creating a simple dataset.
vtkImageData* createTestImageData(int dimensions[3]);


int main(int argc, char const *argv[]) {
    testGraphCutReset();
	testCostFunctionSimple();
	testBasicRunThrough();	
    return 0;
}

vtkImageData* createTestImageData(int dimensions[3]) {
    vtkImageData* imageData = vtkImageData::New();
    imageData->SetDimensions(dimensions[0], dimensions[1], dimensions[2]);
    imageData->AllocateScalars(VTK_DOUBLE, 1);
    for (int z = 0; z < dimensions[2]; z++) {
        for (int y = 0; y < dimensions[1]; y++) {
            for (int x = 0; x < dimensions[0]; x++) {
                imageData->SetScalarComponentFromDouble(x, y, z, 0, rand() % 100);
            }
        }
    }
    return imageData;
}

/**
 * Tests the default state of a new vtkGraphCut object and tests whether
 * the Reset function resets all the cached data.
 * - Reset
 * - SetConnectivity
 * - SetSeedPoints
 * - SetCostFunction
 * - SetInput
 * - GetForegroundPoints
 * - GetBackgroundPoints
 * - GetInput
 * - GetCostFunction
 * - GetConnectivity
 * - GetOutput
 */
void testGraphCutReset() {
    std::cout << __FUNCTION__ << "\n";
    
    vtkGraphCut* graphCut = vtkGraphCut::New();
    
    assert(graphCut->GetForegroundPoints() == NULL);
    assert(graphCut->GetBackgroundPoints() == NULL);
    assert(graphCut->GetInput() == NULL);
    assert(graphCut->GetCostFunction() == NULL);
    assert(graphCut->GetConnectivity() == UNCONNECTED);
    assert(graphCut->GetOutput() == NULL);
    
    vtkPoints* foregroundPoints = vtkPoints::New();
    foregroundPoints->SetNumberOfPoints(2);
    vtkPoints* backgroundPoints = vtkPoints::New();
    backgroundPoints->SetNumberOfPoints(2);
    
    assert(graphCut->GetConnectivity() == UNCONNECTED);
    
    graphCut->SetConnectivity(TWENTYSIX);
    graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);
    
    vtkGraphCutCostFunction* costFunction = vtkGraphCutCostFunctionSimple::New();
    graphCut->SetCostFunction(costFunction);
    
    vtkImageData* imageData = vtkImageData::New();
    imageData->SetDimensions(5, 6, 7);
    imageData->AllocateScalars(VTK_DOUBLE, 1);
    graphCut->SetInput(imageData);
    
    assert(graphCut->GetForegroundPoints() == foregroundPoints);
    assert(graphCut->GetBackgroundPoints() == backgroundPoints);
    assert(graphCut->GetInput() == imageData);
    assert(graphCut->GetCostFunction() == costFunction);
    assert(graphCut->GetConnectivity() == TWENTYSIX);
    
    graphCut->Reset();
    
    assert(graphCut->GetForegroundPoints() == NULL);
    assert(graphCut->GetBackgroundPoints() == NULL);
    assert(graphCut->GetInput() == NULL);
    assert(graphCut->GetCostFunction() == NULL);
    assert(graphCut->GetConnectivity() == UNCONNECTED);
    assert(graphCut->GetOutput() == NULL);
    
    graphCut->Delete();
    std::cout << "Done!" << "\n";
}

void testCostFunctionSimple() {
    std::cout << __FUNCTION__ << "\n";
    vtkGraphCut* graphCut = vtkGraphCut::New();
    vtkPoints* foregroundPoints = vtkPoints::New();
    foregroundPoints->SetNumberOfPoints(2);
    foregroundPoints->SetPoint(1, 0, 0, 1);

    vtkPoints* backgroundPoints = vtkPoints::New();
    backgroundPoints->SetNumberOfPoints(2);
    backgroundPoints->SetPoint(0, 2, 3, 4);
    backgroundPoints->SetPoint(1, 2, 3, 5);

    graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

    vtkGraphCutCostFunction* costFunction = vtkGraphCutCostFunctionSimple::New();
    graphCut->SetCostFunction(costFunction);

    assert(graphCut->GetCostFunction() == costFunction);

    vtkImageData* imageData = vtkImageData::New();
    imageData->SetDimensions(5, 6, 7);
    imageData->AllocateScalars(VTK_DOUBLE, 1);
    for (int z = 0; z < 7; z++) {
        for (int y = 0; y < 6; y++) {
            for (int x = 0; x < 5; x++) {
                imageData->SetScalarComponentFromDouble(x, y, z, 0, rand() % 100);
            }
        }
    }
    graphCut->SetInput(imageData);
    graphCut->SetConnectivity(SIX);

    graphCut->Update();
    
    graphCut->Delete();
    std::cout << "Done!" << "\n";
}

void testBasicRunThrough() {
    std::cout << __FUNCTION__ << "\n";

    int dimensions[3] = {2, 3, 4};
    vtkImageData* input = createTestImageData(dimensions);
    input->SetOrigin(0.0, 0.5, 1.0);

    vtkPoints* foregroundPoints = vtkPoints::New();
    foregroundPoints->SetNumberOfPoints(1);
    foregroundPoints->SetPoint(0, 0, 0, 0);
    vtkPoints* backgroundPoints = vtkPoints::New();
    backgroundPoints->SetNumberOfPoints(1);
    foregroundPoints->SetPoint(0, 1, 1, 1);

    vtkGraphCut* graphCut = vtkGraphCut::New();
    graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);
    graphCut->SetInput(input);
    graphCut->SetConnectivity(SIX);

    assert(graphCut->GetOutput() == NULL);

    graphCut->Update();

    vtkImageData* output = graphCut->GetOutput();
    assert(output != NULL);
    int* inputDimensions = input->GetDimensions();
    int* outputDimensions = output->GetDimensions();
    assert(inputDimensions[0] == outputDimensions[0]);
    assert(inputDimensions[1] == outputDimensions[1]);
    assert(inputDimensions[2] == outputDimensions[2]);

    double* inputSpacing = input->GetSpacing();
    double* outputSpacing = output->GetSpacing();
    assert(inputSpacing[0] == outputSpacing[0]);
    assert(inputSpacing[1] == outputSpacing[1]);
    assert(inputSpacing[2] == outputSpacing[2]);

    double* inputOrigin = input->GetOrigin();
    double* outputOrigin = output->GetOrigin();
    assert(inputOrigin[0] == outputOrigin[0]);
    assert(inputOrigin[1] == outputOrigin[1]);
    assert(inputOrigin[2] == outputOrigin[2]);

    float outputPoint1 = output->GetScalarComponentAsFloat(0, 0, 0, 0);
    float outputPoint2 = output->GetScalarComponentAsFloat(1, 1, 1, 0);
    assert(outputPoint2 < outputPoint1);
    assert(outputPoint2 == 0.0);
    assert(outputPoint1 == 1.0);

    graphCut->Reset();
    assert(graphCut->GetOutput() == NULL);

    graphCut->Delete();
    std::cout << "Done!" << "\n";
}
