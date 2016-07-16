//
//  vtkGraphCutHelperFunctions.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 05/07/16.
//
//

#ifndef vtkGraphCutHelperFunctions_h
#define vtkGraphCutHelperFunctions_h

#include <assert.h>
#include "Edge.h"
#include "Edges.h"
#include "vtkGraphCutDataTypes.h"
#include <vtkImageData.h>

namespace vtkGraphCutHelper
{
    // Helper functions

    bool CalculateCoordinateForIndex(int index, int* dimensions, int* coordinate) {
        if (index >= dimensions[0] * dimensions[1] * dimensions[2] || index < 0) {
            return false;
        }
        
        int dims = dimensions[0] * dimensions[1];
        int rest = index;
        coordinate[2] = rest / dims;
        rest -= coordinate[2] * dims;
        dims = dimensions[0];
        coordinate[1] = rest / dims;
        rest -= coordinate[1] * dims;
        coordinate[0] = rest;
        
        return true;
    }
    
    double GetIntensityForVoxel(vtkImageData* imageData, int x, int y, int z) {
        int numberOfComponents = imageData->GetNumberOfScalarComponents();
        double result = 0.0;
        for (int i = 0; i < numberOfComponents; i++) {
            result += imageData->GetScalarComponentAsDouble(x, y, z, i);
        }
        result /= (double)numberOfComponents;
        return result;
    }
    
    double GetIntensityForVoxel(vtkImageData* imageData, int* xyz) {
        return GetIntensityForVoxel(imageData, xyz[0], xyz[1], xyz[2]);
    }
    
    double GetIntensityForVoxel(vtkImageData* imageData, int index) {
        int coordinate[3] = {0, 0, 0};
        CalculateCoordinateForIndex(index, imageData->GetDimensions(), coordinate);
        return GetIntensityForVoxel(imageData, coordinate);
    }
    
    double GetIntensityForVoxel(vtkImageData* imageData, double* xyz) {
        return GetIntensityForVoxel(imageData, (int)xyz[0], (int)xyz[1], (int)xyz[2]);
    }
    
    double CalculateTerminalCapacity(double intensity, double mean, double variance) {
        // The mean and var values have already been normalized. So now the costs are
        // calculated as the absolute difference from the mean devided by the variance
        return fabs(intensity - mean) / variance;
    }
    
    double CalculateRegionalCapacity(vtkImageData* imageData, Edge* edge, double variance) {
        assert(!edge->isTerminal());
        double intensity1 = GetIntensityForVoxel(imageData, edge->node1());
        double intensity2 = GetIntensityForVoxel(imageData, edge->node2());
        
        // TODO: could be expanded with distance information
        double result = exp(- pow(intensity1 - intensity2, 2) / (2 * pow(variance, 2)));
        // std::cout << intensity1 << ", " << intensity2 << ", " << variance << ", " << result << "\n";
        return result;
    }
    
    double CalculateCapacity(vtkImageData* imageData, Edge* edge, vtkNodeStatistics statistics) {
        if (edge->isTerminal()) {
            int nodeIndex = edge->nonRootNode();
            assert(nodeIndex >= 0);
            int coordinate[3] = {0, 0, 0};
            CalculateCoordinateForIndex(nodeIndex, imageData->GetDimensions(), coordinate);
            double intensity = GetIntensityForVoxel(imageData, coordinate);
            double mean = edge->node1() == NODE_SINK ? statistics.foregroundMean : statistics.backgroundMean;
            double variance = edge->node1() == NODE_SINK ? statistics.foregroundVariance : statistics.backgroundVariance;
            return CalculateTerminalCapacity(intensity, mean, variance);
        } else {
            return CalculateRegionalCapacity(imageData, edge, statistics.variance);
        }
    }
    
}

#endif /* vtkGraphCutHelperFunctions_h */
