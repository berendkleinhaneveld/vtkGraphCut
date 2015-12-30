//
//  vtkPrioQueueTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/7/12.
//
//

#include <stdio.h>
#include <assert.h>
#include "vtkColorValue.h"

bool isNAN(double var) {
    volatile double temp = var;
    return temp != temp;
}

int main() {
	int result = 0;
	
	/**
	 static void SetMinimum(double minimum);
	 static void SetMaximum(double maximum);
	 
	 static double GetMinimum();
	 static double GetMaximum();
	 
	 double GetIntensity();
	 double GetNormalizedIntensity();
	 double GetComponent(int);
	 
	 void SetNumberOfComponents(int);
	 void SetComponent(int, double);
	 */
	
	vtkColorValue* colorValue = new vtkColorValue();
	
	assert(colorValue != NULL);
	assert(isNAN(colorValue->GetIntensity()));
	assert(isNAN(colorValue->GetComponent(1)));
	assert(isNAN(colorValue->GetIntensity()));
	assert(isNAN(colorValue->GetNormalizedIntensity()));
	
	colorValue->SetNumberOfComponents(3);
	
	assert(!isNAN(colorValue->GetComponent(0)));
	assert(!isNAN(colorValue->GetComponent(1)));
	assert(!isNAN(colorValue->GetComponent(2)));
	assert(isNAN(colorValue->GetComponent(3)));
	assert(colorValue->GetNumberOfComponents() == 3);
	assert(colorValue->GetIntensity() == 0.0);
	
	colorValue->SetComponent(0, 2.0);
	colorValue->SetComponent(1, 4.0);
	colorValue->SetComponent(2, 3.0);
	
	assert(colorValue->GetComponent(0) == 2.0);
	assert(colorValue->GetComponent(1) == 4.0);
	assert(colorValue->GetComponent(2) == 3.0);
	assert(colorValue->GetIntensity() == 3.0);
	
	assert(isNAN(vtkColorValue::GetMinimum()));
	assert(isNAN(vtkColorValue::GetMaximum()));
	assert(isNAN(colorValue->GetNormalizedIntensity()));
	
	vtkColorValue::SetMinimum(-1.0);
	
	assert(!isNAN(vtkColorValue::GetMinimum()));
	assert(vtkColorValue::GetMinimum() == -1.0);
	assert(isNAN(vtkColorValue::GetMaximum()));
	assert(isNAN(colorValue->GetNormalizedIntensity()));
	
	vtkColorValue::SetMaximum(-3.0);
	
	return result;
}