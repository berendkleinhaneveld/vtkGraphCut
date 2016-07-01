#include "../../vtkGraphCut.h"
#include "vtkImageData.h"
// #include "vtkGraphCutCostFunctionSimple.h"
// #include "vtkGraphCutCostFunctionStandard.h"
#include "vtkXMLImageDataReader.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkMetaImageWriter.h"
#include "vtkPoints.h"
#include <assert.h>
#include <math.h>

int main() {
	printf("This is a test\n");
	vtkGraphCut* graphCut = vtkGraphCut::New();
	if (graphCut != NULL)
	{
		printf("Succesfully made a thing\n");
		return 0;
	}
	printf("Could not make thingy\n");
	return 1;
}



// enum TestCase {
// 	SIMPLETEST = 0,
// 	HITEST,
// 	FILETEST,
// 	INCOMPLETENESSTEST,
// 	TOOTHTEST
// };

// vtkSmartPointer<vtkImageData> CreateHiImageData(int blockSize, int numberOfComponents);
// void WriteOutputFile(vtkSmartPointer<vtkImageData> imageData, const char* filename);
// void WriteRawOutputFile(vtkSmartPointer<vtkImageData> imageData, const char* filename);
// void CreateTestData();
// vtkSmartPointer<vtkPoints> ReadInputFileWithPoints(const char* file);

// int main() {
	
// //	if (true) {
// //		CreateTestData();
// //		return 0;
// //	}
	
	
// 	// Specify which test you want to run here
// 	TestCase testCase = TOOTHTEST;

// 	// If you want to run a test on a file, please specify the filename over here
// 	// Also specify some fore- and background points below that fall within the boundaries of the vti
// 	const char* file = "/Users/beer/Desktop/Stuff/GraphCut_Datasets/Objects.vti";
// 	const char* outputFilename = "/Users/beer/Desktop/result.vti";
	
// 	bool writeOutput = true;

// 	// Create/read some image data to test with
// 	vtkSmartPointer<vtkImageData> imageData;
// 	vtkXMLImageDataReader* reader = vtkXMLImageDataReader::New();

// 	if (testCase == SIMPLETEST) {
// 		imageData = vtkSmartPointer<vtkImageData>::New();
// 		imageData->Register(NULL);
// 		imageData->SetScalarTypeToDouble();
// 		imageData->SetDimensions(3, 3, 1);
// 		imageData->SetNumberOfScalarComponents(1);

// 		imageData->SetScalarComponentFromDouble(0, 0, 0, 0, 95.0);
// 		imageData->SetScalarComponentFromDouble(1, 0, 0, 0, 80.0);
// 		imageData->SetScalarComponentFromDouble(2, 0, 0, 0, 90.0);
// 		imageData->SetScalarComponentFromDouble(0, 1, 0, 0, 90.0);
// 		imageData->SetScalarComponentFromDouble(1, 1, 0, 0, 95.0);
// 		imageData->SetScalarComponentFromDouble(2, 1, 0, 0, 15.0);
// 		imageData->SetScalarComponentFromDouble(0, 2, 0, 0, 30.0);
// 		imageData->SetScalarComponentFromDouble(1, 2, 0, 0, 20.0);
// 		imageData->SetScalarComponentFromDouble(2, 2, 0, 0, 30.0);

// 		vtkSmartPointer<vtkPoints> foregroundPoints = vtkSmartPointer<vtkPoints>::New();
// 		foregroundPoints->InsertNextPoint(0, 0, 0);
// 		vtkSmartPointer<vtkPoints> backgroundPoints = vtkSmartPointer<vtkPoints>::New();
// 		backgroundPoints->InsertNextPoint(2, 2, 0);

// 		// Create the graph cut module
// 		vtkSmartPointer<vtkGraphCut> graphCut = vtkSmartPointer<vtkGraphCut>::New();
// 		graphCut->SetCostFunction(vtkGraphCutCostFunctionStandard::New());
// //		graphCut->SetCostFunction(vtkGraphCutCostFunctionSimple::New());
// 		graphCut->SetInput(imageData);
// 		graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

// 		graphCut->Update();

// 		if (writeOutput) {
// 			// Create an output file
// 			WriteOutputFile(graphCut->GetOutput(), outputFilename);
// 		}
// 	}
// 	else if (testCase == HITEST) {
// 		int size = 10;
// 		imageData = CreateHiImageData(size, 1);

// 		// Add fore and background points
// 		vtkSmartPointer<vtkPoints> foregroundPoints = vtkSmartPointer<vtkPoints>::New();
// 		foregroundPoints->InsertNextPoint(size+1, 3*size+1, size+1);

// 		vtkSmartPointer<vtkPoints> backgroundPoints = vtkSmartPointer<vtkPoints>::New();
// 		backgroundPoints->InsertNextPoint(1*size, 0, 0);

// 		vtkGraphCut* graphCut = vtkGraphCut::New();
// 		vtkGraphCutCostFunctionStandard* costFunction = vtkGraphCutCostFunctionStandard::New();
// //		costFunction->SetLambda(10.0);
// //		graphCut->SetCostFunction(vtkGraphCutCostFunctionSimple::New());
// 		graphCut->SetCostFunction(costFunction);
// 		graphCut->SetInput(imageData);
// 		graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);
// 		graphCut->Update();

// 		if (writeOutput) {
// 			// Write output to vti file for inspection
// 			WriteOutputFile(graphCut->GetOutput(), outputFilename);
// 		}

// 	}
// 	else if (testCase == FILETEST) {
// 		if (!reader->CanReadFile(file)) {
// 			std::cout << "Could not read the specified file, please make sure you gave the correct location." << std::endl;
// 			return 0;
// 		}

// 		// Read an image file from disk
// 		reader->SetFileName(file);
// 		reader->Update();
// 		imageData = reader->GetOutput();

// 		// Add fore and background points
// 		vtkSmartPointer<vtkPoints> foregroundPoints = vtkSmartPointer<vtkPoints>::New();
// //		foregroundPoints->InsertNextPoint(12, 11, 31); // ToothSmallest
// //		foregroundPoints->InsertNextPoint(24, 22, 62); // ToothSmaller
// //		foregroundPoints->InsertNextPoint(48, 44, 124);// Tooth
// //		foregroundPoints->InsertNextPoint(76, 70, 13);

// 		vtkSmartPointer<vtkPoints> backgroundPoints = vtkSmartPointer<vtkPoints>::New();
// //		backgroundPoints->InsertNextPoint(4, 4, 4);
// //		backgroundPoints->InsertNextPoint(8, 8, 8);
// 		foregroundPoints->InsertNextPoint(36, 25, 19);
// 		backgroundPoints->InsertNextPoint(3, 28, 19);
// 		backgroundPoints->InsertNextPoint(25, 7, 13);

// 		vtkGraphCut* graphCut = vtkGraphCut::New();
// 		vtkGraphCutCostFunctionStandard* costFuction = vtkGraphCutCostFunctionStandard::New();
// 		graphCut->SetCostFunction(costFuction);
// 		graphCut->SetInput(imageData);
// 		graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);
// 		graphCut->Update();

// 		if (writeOutput) {
// 			// Write output to vti file for inspection
// 			WriteOutputFile(graphCut->GetOutput(), outputFilename);
// 		}
// 	}
// 	else if (testCase == INCOMPLETENESSTEST) {
// 		// Create graph cut and try to call Update()
// 		// Should return and mention missing input data
// 		vtkSmartPointer<vtkGraphCut> graphCut = vtkSmartPointer<vtkGraphCut>::New();
// 		graphCut->Update();
		
// 		int size = 4;
// 		imageData = CreateHiImageData(size, 1);
		
// 		// Then give the input data, should now warn that a cost function
// 		// is missing (maybe should just create a default cost function)
// 		graphCut->SetInput(imageData);
// 		graphCut->Update();
		
// 		// Set the cost function, should now warn that there are no seed
// 		// points provided
// 		graphCut->SetCostFunction(vtkSmartPointer<vtkGraphCutCostFunctionSimple>::New());
// 		graphCut->Update();
		
// 		// Add fore and background points
// 		vtkSmartPointer<vtkPoints> foregroundPoints = vtkSmartPointer<vtkPoints>::New();
// 		foregroundPoints->InsertNextPoint(size+1, 3*size+1, size+1);
		
// 		vtkSmartPointer<vtkPoints> backgroundPoints = vtkSmartPointer<vtkPoints>::New();
// 		backgroundPoints->InsertNextPoint(1*size, 0, 0);
		
// 		// Set the seed point. Now it should work
// 		graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);
// 		graphCut->Update();
		
// 		foregroundPoints->InsertNextPoint(5*size+1, 3*size+1, size+1);
// 		backgroundPoints->InsertNextPoint(4*size, 0, 0);
		
// 		// Add more seed points and run the algorithm again
// 		graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);
// 		graphCut->Update();
// 	}
// 	else if (testCase == TOOTHTEST) {
// 		// Read an image file from disk
// 		const char* filename = "/Users/beer/Documents/Desktop Stuff/Stuff/Tooth.vti";
// 		reader->SetFileName(filename);
// 		reader->Update();
// 		imageData = reader->GetOutput();
		
// 		const char* foregroundPointsFile = "/Users/beer/Documents/Desktop Stuff/Stuff/ForegroundPoints.txt";
// 		const char* backgroundPointsFile = "/Users/beer/Documents/Desktop Stuff/Stuff/BackgroundPoints.txt";
// 		vtkSmartPointer<vtkPoints> foregroundPoints = ReadInputFileWithPoints(foregroundPointsFile);
// 		vtkSmartPointer<vtkPoints> backgroundPoints = ReadInputFileWithPoints(backgroundPointsFile);
		
// 		vtkGraphCut* graphCut = vtkGraphCut::New();
// //		graphCut->SetCostFunction(vtkGraphCutCostFunctionSimple::New());
// 		graphCut->SetCostFunction(vtkGraphCutCostFunctionStandard::New());
// 		graphCut->SetInput(imageData);
// 		graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);
// 		graphCut->Update();
		
// 		if (writeOutput) {
// 			// Write output to vti file for inspection
// 			WriteOutputFile(graphCut->GetOutput(), outputFilename);
// 		}
// 	}

// 	/* End of real test */
// 	std::cout << "End of test case" << std::endl;

// 	return 0;
// }

// vtkSmartPointer<vtkImageData> CreateHiImageData(int blockSize, int numberOfComponents) {
// 	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
// 	imageData->Register(NULL);
// 	imageData->SetScalarTypeToDouble();
// 	int size = blockSize;
// 	imageData->SetDimensions(7*size, 5*size, 3*size);
// 	imageData->SetNumberOfScalarComponents(numberOfComponents);
	
// 	int dims[3];
// 	imageData->GetDimensions(dims);
	
// 	for (int z = 0; z < dims[2]; z++) {
// 		for (int y = 0; y < dims[1]; y++) {
// 			for (int x = 0; x < dims[0]; x++) {
// 				for (int i = 0; i < imageData->GetNumberOfScalarComponents(); i++) {
// 					double randomValue = (rand() / (double) RAND_MAX) * (255.0 / 2.1);
// 					if ((((x >= size && x < 2*size) || (x >= 3*size && x < 4*size) || (x >= 5*size && x < 6*size))
// 						 && (y >= size && y < 4*size)
// 						 && (z >= size && z < 2*size)))
// 						imageData->SetScalarComponentFromDouble(x, y, z, i, 255.0 - randomValue);
// 					else if ((x >= 2*size && x < 3*size)
// 							 && y >= 2*size && y < 3*size
// 							 && z >= size && z < 2*size)
// 						imageData->SetScalarComponentFromDouble(x, y, z, i, 255.0 - randomValue);
// 					else
// 						imageData->SetScalarComponentFromDouble(x, y, z, i, 0.0 + randomValue);
// 				}
// 			}
// 		}
// 	}
	
// 	return imageData;
// }

// void WriteOutputFile(vtkSmartPointer<vtkImageData> imageData, const char* filename) {
// 	vtkXMLImageDataWriter* writer = vtkXMLImageDataWriter::New();
// 	writer->SetFileName(filename);
// 	writer->SetInput(imageData);
// 	writer->Update();
// 	writer->Write();
// }

// void WriteRawOutputFile(vtkSmartPointer<vtkImageData> imageData, const char* filename) {
// 	vtkMetaImageWriter* writer = vtkMetaImageWriter::New();
// //	vtkXMLImageDataWriter* writer = vtkXMLImageDataWriter::New();
// 	writer->SetFileName(filename);
// 	writer->SetInput(imageData);
// //	writer->Update();
// 	writer->Write();
// }

// vtkSmartPointer<vtkPoints> ReadInputFileWithPoints(const char* file) {
// 	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	
// 	std::ifstream inFile(file);
// 	int a, b, c;
// 	int i = 0;
// 	while (inFile >> a >> b >> c) {
// 		points->InsertNextPoint(a, b, c);
// 		i++;
// 	}
	
// 	std::cout << "Read " << i << " points from file." << std::endl;
	
// 	return points;
// }

// void CreateTestData() {
// 	// Create vtk image data
// 	vtkSmartPointer<vtkImageData> imageData = CreateHiImageData(3, 1);
	
// 	// Write vtk image data to disk
// 	const char* filename = "/Users/beer/Desktop/hi-3.mhd";
// 	WriteRawOutputFile(imageData, filename);
	
// 	// Create vtk image data
// 	vtkSmartPointer<vtkImageData> imageData2 = CreateHiImageData(5, 1);
	
// 	// Write vtk image data to disk
// 	const char* filename2 = "/Users/beer/Desktop/hi-5.mhd";
// 	WriteRawOutputFile(imageData2, filename2);
// }
