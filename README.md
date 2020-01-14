# Scientific-Data-Visualization-with-VTK
An assignment of Data Visualization Course @TU Dresden. 
[The assignment was done in Visual Studio 2013 verison]


Task 4.2: Scientific Visualization with C++ and VTK

Get The visualization pipeline of VTK is more detailed than the one in ParaView. For every part of the
pipeline, you will have a responsible class instance. First of all, there is a data source that can be
processed by filters. Then, depending on the data type, you will need to choose an appropriate
mapper. To finally show the visualization on the screen, you will need an actor, a renderer, and a
window. 


As a starting point, check the application “assignment4”. There is only one cpp file that provides all the
functionality. Get a big picture of how the components are connected together and how everything
works. Rewrite the file to solve the following tasks:


 show the data in 3D

o use the warp filter (set a scale factor of 2 to see the elevation better)

 generate contour lines

o use the warped model as input

o decide many contour lines shall be drawn (15 is a good starting point)

o generate the contour values equidistantly in the range of the data

o color the lines in the same solid color (do not use a look up table), there is a hint in the
code

 define your own color transfer function for the warped model

o you can utilize vtkLookupTable or vtkColorTransferFunction (both derive from
vtkScalarsToColors)

o connect the color transfer function to the mapper via setLookupTable()

 put it all together in one visualization

o generate two mappers: one for the warped model alone and one for the contour model

o finalize the given function createRenderWindowFromMultipleMappers

o there is a hook for your code (“student begin”), instantiate an actor for every mapper
and add it to the renderer

o do the rendering and interaction as before

 use vtkScalarBarActor to show a legend

o you can add the actor to the renderer from the outside of the method via
someWindow->GetRenderers()->GetFirstRenderer()->AddActor2D( scalarBarActor );

 the final visualization should look like in Fig. 2-1 [look in the pdf file datavis-assignment4.pdf]

 upload the cpp only!

 additional hints:

o Use comments and document your code!

o Code quality matters!

o It‘s okay to build upon existing examples, but: name your sources!
