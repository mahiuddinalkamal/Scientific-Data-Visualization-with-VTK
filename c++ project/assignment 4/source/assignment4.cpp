//
// MAINTAINER MAHIUDDIN AL KAMAL <mahiuddinalkamal@gmail.com>
//

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkDEMReader.h>
#include <vtkContourFilter.h>
#include <vtkWarpScalar.h>
#include <vtkDataSetMapper.h>

#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRendererCollection.h>

#include <vtkColorTransferFunction.h>
#include <vtkScalarBarActor.h>
#include <vtkTextRenderer.h>
#include <vtkLookupTable.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

// standard includes
#include <vector>
#include <algorithm>

// ----- utility functions -----
void setGradientBackground(vtkSmartPointer<vtkRenderer> renderer)
{
	renderer->GradientBackgroundOn();
	renderer->SetBackground(0, 0, 0);
	renderer->SetBackground2(0.2, 0.2, 0.2);
}
// ----- end of utility functions -----

vtkSmartPointer<vtkRenderWindow> createRenderWindowFromMapper(vtkSmartPointer<vtkMapper> mapper)
{
	//create renderer, window and actor
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

	// let's have a fancy background for the canvas
	setGradientBackground(renderer);
	// fallback color, is used whenever mappers do not utilize scalars for coloring
	actor->GetProperty()->SetColor(1, 1, 1);

	// connect everything
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
	window->AddRenderer(renderer);

	// set window size and return
	window->SetSize(800, 800);
	return window;
}

vtkSmartPointer<vtkRenderWindow> createRenderWindowFromMultipleMappers(std::vector<vtkSmartPointer<vtkMapper>> mappers)
{
	// create renderer and window
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();

	// let's have a fancy background for the canvas
	setGradientBackground(renderer);

	// connect window and renderer
	window->AddRenderer(renderer);

	// creating actors
	vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkScalarBarActor> scalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();

	// fallback color, is used whenever mappers do not utilize scalars for coloring
	//actor1->GetProperty()->SetColor(1, 1, 1);
	actor2->GetProperty()->SetColor(1, 1, 1);

	// setting up Legend
	scalarBarActor->SetLookupTable(mappers[0]->GetLookupTable());
	scalarBarActor->SetTitle("Elevation");
	scalarBarActor->SetNumberOfLabels(5);

	// connecting everything
	actor1->SetMapper(mappers[0]);
	actor2->SetMapper(mappers[1]);

	renderer->AddActor(actor1);
	renderer->AddActor(actor2);

	window->GetRenderers()->GetFirstRenderer()->AddActor2D(scalarBarActor);

	//setting up viewing position
	renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
	renderer->GetActiveCamera()->SetPosition(-99900, -21354, 131801);
	renderer->GetActiveCamera()->SetFocalPoint(41461, 41461, 2815);
	renderer->ResetCamera();
	renderer->ResetCameraClippingRange();

	// set window size and return
	window->SetSize(800, 800);
	return window;
}

void doRenderingAndInteraction(vtkSmartPointer<vtkRenderWindow> window)
{
	// create interactor and connect a window
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(window);
	// set an interaction style
	interactor->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());

	// execute render/interaction loop
	interactor->Initialize();
	interactor->Start();

	// close the window when finished
	window->Finalize();
}


int main(int argc, char * argv[])
{
	// -- begin of basic visualization network definition --

	// 1. creating source
	vtkSmartPointer<vtkDEMReader> source = vtkSmartPointer<vtkDEMReader>::New();
	source->SetFileName("../data/SainteHelens.dem");
	source->Update();

	// 2. creating filters
	//a) warp filter
	vtkSmartPointer<vtkWarpScalar> warpFilter = vtkSmartPointer<vtkWarpScalar>::New();

	// setting a scale factor of 2 to see the elevation better
	warpFilter->SetScaleFactor(2);

	// using source as filter input
	warpFilter->SetInputConnection(source->GetOutputPort());

	// warping the surface in the vertical direction
	warpFilter->UseNormalOn();
	warpFilter->SetNormal(0, 0, 1);
	warpFilter->Update();

	// b) contour filter
	vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();

	// using warp model as filter input
	contourFilter->SetInputConnection(warpFilter->GetOutputPort());

	// getting the scalar values from source
	double low = source->GetOutput()->GetScalarRange()[0];
	double high = source->GetOutput()->GetScalarRange()[1];

	// Generating equally spaced contour lines, here number of contours is 15.
	contourFilter->GenerateValues(15, low, high);

	// creating custom color by look up table
	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
	lut->SetHueRange(0, 0.2);
	lut->SetSaturationRange(1.0, 0.5);
	lut->SetValueRange(0.5, 1.0);

	// 3.  create mappers

	// a) warp mapper, show the gradient magnitudes as 2D image 
	vtkSmartPointer<vtkDataSetMapper> warpMapper = vtkSmartPointer<vtkDataSetMapper>::New();

	// connecting to the warp filter output (the pipeline is source->warpFilter->warpMapper->...)
	warpMapper->SetInputConnection(warpFilter->GetOutputPort());
	warpMapper->ScalarVisibilityOn();
	warpMapper->SetScalarRange(low, high);
	warpMapper->SetLookupTable(lut);


	// b) contour mapper, show the regions where the data has a specific value
	vtkSmartPointer<vtkDataSetMapper> contourMapper = vtkSmartPointer<vtkDataSetMapper>::New();

	// connecting to the contour filter output (the pipeline is source->contourFilter->contourMapper->...)
	contourMapper->SetInputConnection(contourFilter->GetOutputPort());

	// avoiding z-buffer fighting with small polygon shift
	contourMapper->SetResolveCoincidentTopologyToPolygonOffset();

	// using falback coloring instead of the scalar value to color the line, see fallback coloring via actor in createRenderWindowFromMapper
	contourMapper->ScalarVisibilityOff();

	// -- end of basic visualization network definition --


	// 4. creating actors, renderers, render windows 	
	//creating mappers
	std::vector<vtkSmartPointer<vtkMapper>> mappers;
	mappers.push_back(warpMapper);
	mappers.push_back(contourMapper);

	vtkSmartPointer<vtkRenderWindow> finalWindow = createRenderWindowFromMultipleMappers(mappers);

	// 5. successively showing each window and allow user interaction (until it is closed)
	doRenderingAndInteraction(finalWindow);

	return 0;
}