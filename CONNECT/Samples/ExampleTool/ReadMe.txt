This application demonstrates creation of different tools using different interfaces provided by MicroStation

This example provides the following tools

Create Tool: Shows how to use DgnPrimitiveTool to write a place line tool. 
Move Tool: Shows how to use DgnElementSetTool to write a basic MOVE ELEMENT tool. The base class populates an ElementAgenda from a pick or selection set on the 1st data button and starts element dynamics. The 2nd data button accepts the modification. This sub-class is responsible for applying the desired modification to the element(s). The _OnElementModify method is called for each entry in the ElementAgenda both during element dynamics as well as for the final accept. 
Modify Tool: Shows how to use DgnElementSetTool to write a MODIFY ELEMENT tool. This tool will move the closest vertex on a line or linestring. 
Flood Tool: Shows how to use DgnRegionElementTool to write a CREATE REGION by FLOOD tool. The base class handles collecting the region boundary from the elements in the view or selection set. The sub-class is responsible for doing something with the new region and optionally processing the original elements that comprise the boundary. 
View Tool: Shows how to use DgnViewTool to write a viewing tool. Zooms the view and centers the accept. Demonstrates using IndexedViewSet and IViewManager to update the view. 
This example provides the following key-ins to demonstrate the above mentioned tools

EXAMPLE LINETOOL
EXAMPLE LSTRINGTOOL
EXAMPLE MOVETOOL
EXAMPLE MODIFYTOOL
EXAMPLE FLOODTOOL
EXAMPLE VIEWTOOL
