/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/adrwdemo/adrwdemo.cpp,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   adrwdemo.cpp  $
|   $Revision: 1.2 $
|   	$Date: 2013/06/28 20:33:45 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Adrwdemo application source file					|
|									|
|   Demonstrates how to optimize AccuDraw for a particular command.	|
|   Also demonstrates other concepts, such as construction planes,	|
|   and methods of creating rotation matrices from points.		|
|									|
|   Routine Summary -							|
|									|
|    line			-  start place line command		|
|    line_dataPt1		-  datapoint state function		|
|    line_drawLine		-  dynamics state function		|
|    line_dataPt2		-  datapoint state function		|
|									|
|    circle			-  start place circle command		|
|    circle_dataPt1		-  datapoint state function		|
|    circle_drawCircle		-  dynamics state function		|
|    circle_dataPt2		-  datapoint state function		|
|									|
|    rect			-  start place rectangle command	|
|    rect_dataPt1		-  datapoint state function		|
|    rect_drawRectangle 	-  dynamics state function		|
|    rect_dataPt2		-  datapoint state function		|
|									|
|    rect2			-  start place rotated rectangle command|
|    rect2_dataPt1		-  datapoint state function		|
|    rect2_dataPt2		-  datapoint state function		|
|    rect2_dataPt3		-  datapoint state function		|
|    rect2_drawRectangle						|
|									|
|    changeCircle		-  start change circle radius command	|
|    changeCircle_show		-  "show" state function		|
|    changeCircle_draw		-  dynamics state function		|
|    changeCircle_accept	-  accept state function		|
|    changeCircle_cleanup	-  cleanup state function		|
|									|
|    rMatrixFrom2PtsAndView	-  create a rotation matrix from	|
|				   two points and a view.		|
|    rMatrixFrom3Pts		-  create a rotation matrix from	|
|				   three points.			|
|    showRotMatrix		-  debugging utility -- shows a rotation|
|				   matrix graphically			|
|    softConstructionPlane	-  projects cursor onto a plane, defined|
|				   by a point and normal vector. Snapped|
|				   points override the plane.		|
|    hardConstructionPlane	-  projects cursor onto a plane, defined|
|				   by a point and normal vector. Snapped|
|				   points cause the point to be 	|
|				   projected to closest point on plane. |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files							|
|									|
+----------------------------------------------------------------------*/
#include    <mdl.h>
#include    <mselems.h>
#include    <global.h>
#include    <tcb.h>
#include    <mdlerrs.h>
#include    <userfnc.h>
#include    <cexpr.h>
#include    <rscdefs.h>
#include    <dlogitem.h>
#include    <accudraw.h>
#include    <math.h>
#include    <msvar.fdf>
#include    <mssystem.fdf>

#include    "adrwdcmd.h"
#include    "adrwdemo.h"
#include    "adrwdemo.fdf"

#include    <dlogman.fdf>
#include    <msvec.fdf>
#include    <msmisc.fdf>
#include    <mselemen.fdf>
#include    <msrmatrx.fdf>
#include    <mselmdsc.fdf>
#include    <msoutput.fdf>
#include    <mslocate.fdf>
#include    <msparse.fdf>
#include    <msstate.fdf>
#include    <mscexpr.fdf>
#include    <msrsrc.fdf>
#include    <msview.fdf>

#define     X 0
#define     Y 1
#define     Z 2

#define     ptStack statedata.dPointStack /* for readability */


/*----------------------------------------------------------------------+
|									|
|   Private Global variables						|
|									|
+----------------------------------------------------------------------*/
AdrwdemoInfo	adrwdemoInfo;  /* Tool Settings (slam-down) variables */

MSElementDescr	*modifyEdP = NULL;

/*----------------------------------------------------------------------+
|									|
|   Private routines							|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
| name		main-							|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
extern "C" DLLEXPORT  int MdlMain
(
int	     argc,
char	    *argv[]
)
    {
    RscFileHandle   rfHandle;
    SymbolSet	    *setP;

    mdlState_registerStringIds (0, 1);

    mdlResource_openFile (&rfHandle, NULL, FALSE);
		       
    setP = mdlCExpression_initializeSet (VISIBILITY_DIALOG_BOX, 0, 0);
    mdlDialog_publishComplexVariable (setP, "adrwdemoinfo",
				      "adrwdemoInfo", &adrwdemoInfo);

    MdlCommandNumber commandNumbers [] =
        {
            {line, CMD_DEMO_LINE},
            {circle, CMD_DEMO_CIRCLE},
            {rect, CMD_DEMO_RECTANGLE},
            {rect2, CMD_DEMO_RECTANGLE2},
            {changeCircle, CMD_DEMO_CHANGE},
        0
        };
    /* Register commands */
    mdlSystem_registerCommandNumbers (commandNumbers);

    mdlParse_loadCommandTable (NULL);

    mdlState_registerStringIds (MSGLIST_commands, MSGLIST_prompts);

    adrwdemoInfo.showAxes    = FALSE;

    mdlDialog_open (NULL, DIALOGID_Pal);

    return  0;
    }

/*----------------------------------------------------------------------+
|									|
|   DEMO LINE command							|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
| name		line-							|
|									|
| author	BSI					9/94		|
|									|
|									|
| Simple line placement command.  Optimizes accudraw by rotating	|
| its coordinate system such that the x axis is parallel to the 	|
| previous line.  Also saves the length of the line as AccuDraw's last	|
| distance, which the user can index to or recall by pressing PgUp	|									|
+----------------------------------------------------------------------*/
Private void	line
(
char    *unparsed
)
//cmdNumber	CMD_DEMO_LINE
    {
    mdlState_startPrimitive (line_dataPt1, line,
			     MSG_cmdLine, MSG_promptEnterPoint);
    }

/*----------------------------------------------------------------------+
|									|
| name		line_dataPt1-						|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void	line_dataPt1
(
Dpoint3d    *ptP,
int	    view
)
    {
    ptStack[0] = *ptP;
    mdlState_setFunction (STATE_DATAPOINT, line_dataPt2);
    mdlState_setFunction (STATE_COMPLEX_DYNAMICS, line_drawLine);
    mdlOutput_rscPrintf (MSG_PROMPT, NULL, MSGLIST_prompts,
			 MSG_promptEnterPoint);
    }

/*----------------------------------------------------------------------+
|									|
| name		line_drawLine-						|
|									|
| author	BSI					9/94		|
|									|
| dynamics function -- also called explictly from data point function	|
+----------------------------------------------------------------------*/
Private void	line_drawLine
(
Dpoint3d    *ptP,
int	    view,
MstnDrawMode	    drawMode
)
    {
    MSElement elem;
    Dpoint3d points[2];

    points[0] = ptStack[0];
    points[1] = *ptP;
    mdlLine_create (&elem, NULL, points);
    mdlElement_display (&elem, drawMode);

    if (drawMode == NORMALDRAW)
	mdlElement_add (&elem);
    }

/*----------------------------------------------------------------------+
|									|
| name		line_dataPt2-						|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void line_dataPt2
(
Dpoint3d    *ptP,
int	    view
)
    {
    double	length; 	/* length of line placed	       */
    Dpoint3d	xAxisVec;	/* unit vector -- direction of line    */

    line_drawLine (ptP, view, NORMALDRAW);

	/* calculate length and unit vector */
    length = mdlVec_computeNormal (&xAxisVec, ptP, &ptStack[0]);

    mdlState_setAccudrawContext  (
		      ACCUDRAW_SetDistance |
		      ACCUDRAW_SetXAxis,
		      NULL,		      /* origin: last point OK	*/
		      NULL,		      /* delta (not used here)	*/
		      &length,		      /* distance		*/
		      NULL,		      /* angle (not used here)	*/
		      &xAxisVec 	      /* x axis: last line	*/
		      );

    ptStack[0] = *ptP;
    mdlState_setFunction (STATE_COMPLEX_DYNAMICS, line_drawLine);
    mdlOutput_rscPrintf (MSG_PROMPT, NULL, MSGLIST_prompts,
			 MSG_promptEnterPoint);
    }

/*----------------------------------------------------------------------+
|									|
|   DEMO CIRCLE   command						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
| name		circle- 						|
|									|
| author	BSI					9/94		|
|									|
| Places a circle by center and edge point.  Minimal Accudraw		|
| optimization: simply sets the mode to polar coordinates (which makes	|
| it easy for the user to key in a radius -- simply type a value and	|
| press enter).  Also saves the radius as Accudraw's last distance, so	|
| the user can easily place another circle of the same size.		|
|									|
| Aside from Accudraw optimizations, this command is a good example of	|
| deriving a rotation matrix from 2 points and a view.	The user may	|
| turn on "Show Axes" to see graphically the X, Y, and Z axes of the	|
| resultant rotation matrix.						|
+----------------------------------------------------------------------*/
Private void	circle
(
char    *unparsed
)
//cmdNumber	CMD_DEMO_CIRCLE
    {
    mdlState_startPrimitive (circle_dataPt1, circle,
			     MSG_cmdCircle, MSG_promptEnterPoint);
    }

/*----------------------------------------------------------------------+
|									|
| name		circle_dataPt1- 					|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void circle_dataPt1
(
Dpoint3d    *ptP,
int	    view
)
    {
    ptStack[0] = *ptP;
    mdlState_setFunction (STATE_DATAPOINT, circle_dataPt2);
    mdlState_setFunction (STATE_COMPLEX_DYNAMICS, circle_drawCircle);
    mdlOutput_rscPrintf (MSG_PROMPT, NULL, MSGLIST_prompts,
			 MSG_promptEnterPoint);

    mdlState_setAccudrawContext  (
		      ACCUDRAW_SetModePolar,  /* force mode polar coords*/
		      NULL,		      /* default origin OK	*/
		      NULL,		      /* delta			*/
		      NULL,		      /* distance		*/
		      NULL,		      /* angle			*/
		      NULL		      /* default rotation OK	*/
		      );
    }

/*----------------------------------------------------------------------+
|									|
| name		circle_drawCircle-					|
|									|
| author	BSI					9/94		|
|									|
| dynamics function -- also called explictly from data point function	|
+----------------------------------------------------------------------*/
Private void circle_drawCircle
(
Dpoint3d    *ptP,
int	    view,
MstnDrawMode	    drawMode
)
    {
    MSElement	elem;
    RotMatrix	rMatrix;
    Dpoint3d	newPoint;
    double	radius;

    if (adrwdemoInfo.useCPlane)
	{
	Dpoint3d	zVector;

	mdlRMatrix_fromView (&rMatrix, view, TRUE);
	mdlRMatrix_getRowVector (&zVector, &rMatrix, Z);
	softConstructionPlane (&newPoint, ptP, &ptStack[0], &zVector, view);
	}
    else
	{
	newPoint = *ptP;
	}

    radius = mdlVec_distance (&ptStack[0], &newPoint);

    if	(rMatrixFrom2PtsAndView (&rMatrix, &ptStack[0], &newPoint, view)
		!= SUCCESS)
	return; /* may wish to handle special case */

    /* if user has selected "show axes", display x y and z axes */
    if (adrwdemoInfo.showAxes)
	showRotMatrix (&rMatrix, &ptStack[0], radius/2, drawMode);

    /* ellipse rotation matrix is colums, ours is rows */
    mdlRMatrix_invert (&rMatrix, &rMatrix);

    mdlEllipse_create (&elem, NULL, &ptStack[0], radius, radius,
		       &rMatrix, 0);

    mdlElement_display (&elem, drawMode);

    if (drawMode == NORMALDRAW)
	mdlElement_add (&elem);
    }

/*----------------------------------------------------------------------+
|									|
| name		circle_dataPt2- 					|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void circle_dataPt2
(
Dpoint3d    *ptP,
int	    view
)
    {
    double radius;

    circle_drawCircle (ptP, view, NORMALDRAW);
    circle ("");

    /* get radius of circle just placed */
    radius = mdlVec_distance (&ptStack[0], ptP);

    mdlState_setAccudrawContext  (
		      ACCUDRAW_SetOrigin  | /* set origin for next circle */
		      ACCUDRAW_SetDistance, /* save dist for next circle */
		      &ptStack[0],	    /* default origin OK	 */
		      NULL,		    /* delta			 */
		      &radius,		    /* distance 		 */
		      NULL,		    /* angle			 */
		      NULL		    /* rotation 		 */
		      );
    }

/*----------------------------------------------------------------------+
|									|
|   DEMO RECTANGLE command						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
| name		rect-							|
|									|
| author	BSI					9/94		|
|									|
|									|
| Places a rectangle by 2 diagonal points.				|
|									|
| Even less Accudraw optimizations:  simply sets it to rectangular	|
| coordinate mode, which is very effective for this command.		|
|									|
| Good example of using a rotation matrix (from view) to define 	|
| rectangle's								|
| orientation.								|
|									|
|									|
+----------------------------------------------------------------------*/
Private void	rect
(
char    *unparsed
)
//cmdNumber	CMD_DEMO_RECTANGLE
    {
    mdlState_startPrimitive (rect_dataPt1, rect,
			     MSG_cmdRect, MSG_promptEnterPoint);
    }

/*----------------------------------------------------------------------+
|									|
| name		rect_dataPt1-						|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void rect_dataPt1
(
Dpoint3d    *ptP,
int	    view
)
    {
    ptStack[0] = *ptP;

    mdlState_setAccudrawContext (
		      ACCUDRAW_SetModeRect,   /* force mode rectangular */
		      NULL,		      /* origin - default Okay	*/
		      NULL,		      /* delta			*/
		      NULL,		      /* distance		*/
		      NULL,		      /* angle			*/
		      NULL		      /* rotation		*/
		      );

    mdlState_setFunction (STATE_DATAPOINT, rect_dataPt2);
    mdlState_setFunction (STATE_COMPLEX_DYNAMICS, rect_drawRectangle);
    mdlOutput_rscPrintf (MSG_PROMPT, NULL, MSGLIST_prompts,
			 MSG_promptEnterPoint);
    }

/*----------------------------------------------------------------------+
|									|
| name		rect_drawRectangle-					|
|									|
| author	BSI					9/94		|
|									|
| dynamics function -- also called explictly from data point function	|
|									|
| This function, for demonstration purposes, supports a Tool Settings	|
| option in which the user can use a view other than the current one	|
| for orientation information.	This is really only useful if the other |
| Tool Settings option, "Use Construction Plane", is turned on.  Also,	|
| it will have no effect if AccuDraw is active. 			|
|									|
+----------------------------------------------------------------------*/
Private void	rect_drawRectangle
(
Dpoint3d    *ptP,
int	    view,
MstnDrawMode	    drawMode
)
    {
    MSElement	elem;
    RotMatrix	rMatrix;
    Dpoint3d	newPoint, rectPts[5];

    /*	 get rotation matrix from view OR Accudraw  */
    /*	 Important: 3rd argument must be TRUE!	    */

    mdlRMatrix_fromView (&rMatrix, view, TRUE);

    /****************************************************\
     The following supports the Tool Settings item "Use
     Construction Plane".  If AccuDraw is active, this will
     have no effect, since AccuDraw does construction
     planes for you.
    \****************************************************/

    if (adrwdemoInfo.useCPlane)
	{
	Dpoint3d zVector;

	mdlRMatrix_getRowVector (&zVector, &rMatrix, Z);
	hardConstructionPlane (&newPoint, ptP, &ptStack[0], &zVector, view);
	}
    else
	{
	newPoint = *ptP;
	}


    /* rotate points, do simple x and y copying, rotate back */

    rectPts[0] = ptStack[0];
    rectPts[2] = newPoint;
    mdlRMatrix_rotatePoint (&rectPts[0], &rMatrix);
    mdlRMatrix_rotatePoint (&rectPts[2], &rMatrix);

    rectPts[1] = rectPts[0];
    rectPts[1].y = rectPts[2].y;
    rectPts[3] = rectPts[0];
    rectPts[3].x = rectPts[2].x;
    rectPts[4] = rectPts[0];
    rectPts[1].z = rectPts[2].z = rectPts[3].z = rectPts[0].z;

    mdlRMatrix_unrotatePointArray (rectPts, &rMatrix, 5);

    mdlShape_create (&elem, NULL, rectPts, 5, 1);
    mdlElement_display (&elem, drawMode);

    if (drawMode == NORMALDRAW)
	mdlElement_add (&elem);

	/* if user has selected "show axes", display x y and z axes */
    if (adrwdemoInfo.showAxes)
	showRotMatrix (&rMatrix, &ptStack[0],
		       mdlVec_distance (&ptStack[0], &newPoint)/2, drawMode);
    }

/*----------------------------------------------------------------------+
|									|
| name		rect_dataPt2-						|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void	rect_dataPt2
(
Dpoint3d    *ptP,
int	    view
)
    {
    rect_drawRectangle (ptP, view, NORMALDRAW);
    rect ("");
    }

/*----------------------------------------------------------------------+
|									|
|   DEMO RECTANGLE2 (Rotated Rectangle) command 			|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
| name		rect2-							|
|									|
| author	BSI					9/94		|
|									|
|									|
| Places a rectangle by 3 points:  the first defines one corner, the	|
| second defines the rotation, the third defines the other corner.	|
|									|
| By using Accudraw optimizations, this command is greatly enhanced.	|
| After the second point, the rotation is changed such that the first	|
| two points define the x axis.  Rather using the second point as	|
| Accudraw's origin (which would happen by default), the origin is put	|
| back to the first corner point.  Finally, the x value is locked at the|
| distance from the first to the second point.	This effectively gives	|
| the user a choice between which point defines the base of the 	|
| rectangle.  Note that Accudraw's X and Y readout fields will		|
| accurately reflect the width and height of the rectangle.		|
|									|
+----------------------------------------------------------------------*/
Private void	rect2
(
char    *unparsed
)
//cmdNumber	CMD_DEMO_RECTANGLE2

    {
    mdlState_startPrimitive (rect2_dataPt1, rect2,
			     MSG_cmdRect2, MSG_promptEnterPoint);
    }

/*----------------------------------------------------------------------+
|									|
| name		rect2_dataPt1-						|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void	rect2_dataPt1
(
Dpoint3d    *ptP,
int	    view
)
    {
    ptStack[0] = *ptP;

    mdlState_setFunction (STATE_DATAPOINT, rect2_dataPt2);
    mdlState_setFunction (STATE_COMPLEX_DYNAMICS, line_drawLine);
    mdlOutput_rscPrintf (MSG_PROMPT, NULL, MSGLIST_prompts,
			 MSG_promptEnterPoint);
    }

/*----------------------------------------------------------------------+
|									|
| name		rect2_dataPt2-						|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void rect2_dataPt2
(
Dpoint3d    *ptP,
int	    view
)
    {
    Dpoint3d xAxisVec,
	     delta;

    if (mdlVec_pointEqualUOR (&ptStack[0], ptP))
	{
	mdlUtil_beep (1);
	return;
	}

    ptStack[1] = *ptP;

	/* measure distance and create unit vector for x axis */
    delta.x = mdlVec_computeNormal (&xAxisVec, ptP, &ptStack[0]);

    mdlState_setAccudrawContext
		    (
		    ACCUDRAW_SetOrigin	  |
		    ACCUDRAW_SetXAxis	  |
		    ACCUDRAW_SetModeRect  |
		    ACCUDRAW_Lock_X,
		    &ptStack[0],    /* use first point instead	*/
		    &delta,	    /* only x value is used	*/
		    NULL,	    /* distance 		*/
		    NULL,	    /* angle			*/
		    &xAxisVec	    /* rotation 		*/
		    );

    mdlState_setFunction (STATE_DATAPOINT, rect2_dataPt3);
    mdlState_setFunction (STATE_COMPLEX_DYNAMICS, rect2_drawRectangle);
    mdlOutput_rscPrintf (MSG_PROMPT, NULL, MSGLIST_prompts,
			 MSG_promptEnterPoint);
    }

/*----------------------------------------------------------------------+
|									|
| name		rect2_dataPt3-						|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void rect2_dataPt3
(
Dpoint3d    *ptP,
int	    view
)
    {
    rect2_drawRectangle (ptP, view, NORMALDRAW);
    rect2 ("");
    }

/*----------------------------------------------------------------------+
|									|
| name		rect2_drawRectangle-					|
|									|
| author	BSI					9/94		|
|									|
| dynamics function -- also called explictly from data point function	|
+----------------------------------------------------------------------*/
Private void	rect2_drawRectangle
(
Dpoint3d    *ptP,
int	    view,
MstnDrawMode	    drawMode
)
    {
    MSElement	elem;
    RotMatrix	rMatrix;
    Dpoint3d	rectPts[5];


    if (rMatrixFrom3Pts (&rMatrix, &ptStack[0], &ptStack[1], ptP) != SUCCESS)
	return;

    /* if user has selected "show axes", display x y and z axes */
    if (adrwdemoInfo.showAxes)
	showRotMatrix (&rMatrix, &ptStack[0],
		       mdlVec_distance (&ptStack[0], &ptStack[1])/2, drawMode);

    /* rotate points, do simple x and y copying, rotate back */

    rectPts[0] = ptStack[0];
    rectPts[2] = *ptP;
    mdlRMatrix_rotatePoint (&rectPts[0], &rMatrix);
    mdlRMatrix_rotatePoint (&rectPts[2], &rMatrix);
    rectPts[1] = rectPts[0];
    rectPts[1].y = rectPts[2].y;
    rectPts[3] = rectPts[0];
    rectPts[3].x = rectPts[2].x;
    rectPts[4] = rectPts[0];
    rectPts[1].z = rectPts[2].z = rectPts[3].z = rectPts[0].z;
    mdlRMatrix_unrotatePointArray (rectPts, &rMatrix, 5);

    mdlShape_create (&elem, NULL, rectPts, 5, 1);

    mdlElement_display (&elem, drawMode);

    if (drawMode == NORMALDRAW)
	mdlElement_add (&elem);
    }
/*----------------------------------------------------------------------+
|                                                                       |
| name          changeCircle_setElmSearchType                                        |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/
Private void    changeCircle_setElmSearchType
(
void
)
    {
    static int  searchType[] = {ELLIPSE_ELM,};

    /* Clear search criteria */
    mdlLocate_noElemNoLocked ();

    /* Set search mask to look for text and line elements */
    mdlLocate_setElemSearchMask (sizeof(searchType)/sizeof(int), searchType);
    }


/*----------------------------------------------------------------------+
|									|
| name		changeCircle-						|
|									|
| author	BSI					9/94		|
|									|
|									|
| Changes the radius of a circle (or ellipse, which will become a	|
| circle to keep it simple). Similar to Modify Element command's effect |
| on circles.								|
|									|
| Optimizes Accudraw by setting the origin to the center of the circle, |
| setting the rotation to that of the circle, forcing it into polar	|
| coordinates, and saving the radius as the last distance.  The cursor	|
| will be projected onto the plane of the circle.  Note that the	|
| user can simply type a new radius, press Enter, and click a datapoint.|
|									|
| Notice that unlike the other commands, this one supplies a fully	|
| defined rotation to AccuDraw, not just an x axis.			|
|									|
| For this command, construction plane projection is necessary for	|
| the command to be usable in 3d without Accudraw active		|
| (consistant with the Modify Element command's effect on circles)	|
+----------------------------------------------------------------------*/
Private void	changeCircle
(
char    *unparsed
)
//cmdNumber	CMD_DEMO_CHANGE
    {
    changeCircle_cleanup ();
    changeCircle_setElmSearchType ();
//    mdlLocate_noElemNoLocked ();
//    tcb->searchType[0] = TMSK0_ELLIPSE;

    mdlState_startModifyCommand (changeCircle, changeCircle_accept,
				 NULL, changeCircle_show,
				 NULL, MSG_cmdChange,
				 MSG_promptAcceptReject, FALSE, TRUE);

    mdlState_setFunction (STATE_COMMAND_CLEANUP, changeCircle_cleanup);

    mdlLocate_init ();
    }

/*----------------------------------------------------------------------+
|									|
| name		changeCircle_show-					|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void	changeCircle_show
(
void
)
    {
    Dpoint3d	center;
    double	radius;
    RotMatrix	rMatrix;
    ULong	filePos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);

    if (mdlElmdscr_read (&modifyEdP, filePos, MASTERFILE, FALSE, NULL) == 0L)
	return;

    mdlArc_extract (NULL, NULL, NULL, &radius, NULL,
		    &rMatrix, &center, &modifyEdP->el);

    /* circle's rotation matrix is inverse of AccuDraw's */
    mdlRMatrix_invert (&rMatrix, &rMatrix);

    mdlState_setAccudrawContext (
		     ACCUDRAW_SetOrigin   | /* use center as origin    */
		     ACCUDRAW_FixedOrigin | /* don't move until new command */
		     ACCUDRAW_SetDistance | /* use radius as distance  */
		     ACCUDRAW_SetRMatrix  | /* use circle's rotation   */
		     ACCUDRAW_SetModePolar, /* force polar coordinates */
		     &center,		    /* origin	*/
		     NULL,		    /* delta	*/
		     &radius,		    /* distance */
		     NULL,		    /* angle	*/
		     &rMatrix		    /* rotation */
		     );

    mdlState_setFunction (STATE_COMPLEX_DYNAMICS, changeCircle_draw);
    }

/*----------------------------------------------------------------------+
|									|
| name		changeCircle_draw-					|
|									|
| author	BSI					9/94		|
|									|
| dynamics function -- also called explictly from accept function	|
+----------------------------------------------------------------------*/
Private void	changeCircle_draw
(
Dpoint3d *ptP,
int	  view,
MstnDrawMode drawMode	  
)
    {
    double	radius;
    Dpoint3d	newPoint, center;
    RotMatrix	rMatrix;
    MSElement	newElem;

    mdlArc_extract (NULL, NULL, NULL, &radius, NULL,
		    &rMatrix, &center, &modifyEdP->el);

	/* use construction plane to project point onto plane of circle
	   prior to measuring new radius */
    if (adrwdemoInfo.useCPlane)
	{
	Dpoint3d zVector;

	mdlRMatrix_getColumnVector (&zVector, &rMatrix, Z);
	hardConstructionPlane (&newPoint, ptP, &center, &zVector, view);
	}
    else
	{
	newPoint = *ptP;
	}

    radius = mdlVec_distance (&center, &newPoint);

    mdlEllipse_create (&newElem, &modifyEdP->el, &center,
		       radius, radius, &rMatrix, 0);

    /* if user has selected "show axes", display x y and z axes */
    if (adrwdemoInfo.showAxes)
	{
	mdlRMatrix_invert (&rMatrix, &rMatrix);
	showRotMatrix (&rMatrix, &center, radius/2, drawMode);
	}

    if (drawMode == NORMALDRAW)
	{
	mdlElement_display (&modifyEdP->el, ERASE);
	mdlElement_display (&newElem, HILITE);

	mdlElement_rewrite (&newElem, &modifyEdP->el,
			    mdlElement_getFilePos (FILEPOS_CURRENT, NULL));

	mdlElmdscr_freeAll (&modifyEdP);
	mdlElmdscr_new (&modifyEdP, NULL, &newElem);
	}
    else
	{
	mdlElement_display (&newElem, drawMode);
	}
    }

/*----------------------------------------------------------------------+
|									|
| name		changeCircle_accept-					|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void	changeCircle_accept
(
Dpoint3d *ptP,
int	  view
)
    {
    changeCircle_draw (ptP, view, NORMALDRAW);
    }

/*----------------------------------------------------------------------+
|									|
| name		changeCircle_cleanup-					|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private void	changeCircle_cleanup
(
void
)
    {
    if (modifyEdP)
	{
	mdlElement_display (&modifyEdP->el, NORMALDRAW);
	mdlElmdscr_freeAll (&modifyEdP);
	}
    }

/*----------------------------------------------------------------------+
|	Geometry utilities						|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
| name		rMatrixFrom2PtsAndView- 				|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private int rMatrixFrom2PtsAndView
(
RotMatrix   *rMatrixP,		/* <=  */
Dpoint3d    *originP,		/*  => */
Dpoint3d    *xAxisP,		/*  => */
int	     view
)
    {
    Dpoint3d	vecs[3];
    RotMatrix	viewMatrix;

    mdlRMatrix_fromView (&viewMatrix, view, TRUE);
    mdlRMatrix_getRowVector (&vecs[Z], &viewMatrix, Z);

    if (mdlVec_computeNormal (&vecs[X], xAxisP, originP) < fc_epsilon)
	return ERROR;

    mdlVec_crossProduct (&vecs[Y], &vecs[Z], &vecs[X]);
    if (mdlVec_normalize (&vecs[Y]) < fc_epsilon)
	return ERROR;

    mdlVec_crossProduct (&vecs[Z], &vecs[X], &vecs[Y]);
    mdlRMatrix_fromRowVectors (rMatrixP, &vecs[X], &vecs[Y], &vecs[Z]);

    return SUCCESS;
    }

/*----------------------------------------------------------------------+
|									|
| name		rMatrixFrom3Pts-					|
|									|
| author	BSI					9/94		|
|									|
+----------------------------------------------------------------------*/
Private int	rMatrixFrom3Pts
(
RotMatrix   *rMatrixP,		/* <=  */
Dpoint3d    *originP,		/*  => */
Dpoint3d    *xAxisP,		/*  => */
Dpoint3d    *yDirectionP	/*  => */

)
    {
    Dpoint3d	vecs[3];

    if (mdlVec_computeNormal (&vecs[X], xAxisP, originP) < fc_epsilon)
	return ERROR;

    if (mdlVec_computeNormal (&vecs[Y], yDirectionP, originP) < fc_epsilon)
	return ERROR;

    mdlVec_crossProduct (&vecs[Z], &vecs[X], &vecs[Y]);
    if (mdlVec_normalize (&vecs[Z]) < fc_epsilon)
	return ERROR;

    mdlVec_crossProduct (&vecs[Y], &vecs[Z], &vecs[X]);
    mdlRMatrix_fromRowVectors (rMatrixP, &vecs[X], &vecs[Y], &vecs[Z]);

    return SUCCESS;
    }

/*----------------------------------------------------------------------+
|									|
| name	    showRotMatrix-						|
|									|
| author    BSI 					9/94		|
|    draws axes of rot matrix for demo/debugging purposes		|
+----------------------------------------------------------------------*/
Private void	showRotMatrix
(
RotMatrix   *rMatrixP,	/* => */
Dpoint3d    *ptP,	/* => */
double	    scale,	/* => length of lines */
MstnDrawMode	    drawMode	/* => */
)
    {
    static UInt32	colors[3] = {3, 2, 1}; /* red, green and blue indices */
    int 	i;
    Dpoint3d	linePts[2], vec;
    MSElement	elem;
    UInt32	weight = 5;

    linePts[0] = *ptP;

    for (i=X; i<=Z; i++)
	{
	mdlRMatrix_getRowVector (&vec, rMatrixP, i);
	mdlVec_projectPoint (&linePts[1], ptP, &vec, scale);
	mdlLine_create (&elem, NULL, linePts);
       /* Should not access the element structure directly
       	elem.hdr.dhdr.symb.b.color = colors[i];
	elem.hdr.dhdr.symb.b.weight = 5; */
	mdlElement_setSymbology (&elem,&colors[i],&weight,NULL);
	mdlElement_display (&elem, drawMode);

	if (drawMode == NORMALDRAW)
	    mdlElement_add (&elem);
	}
    }

/*----------------------------------------------------------------------+
|									|
| Construction Plane Functions						|
|									|
+----------------------------------------------------------------------*/

#define BEHIND_EYE_POINT		-1
#define PARALLEL			-2

/* warning: this macro will not work in a 2d file! Check for 3d first */
#define RMATRX_VEC(rMatrixP,axis)	(((Dpoint3d *)(rMatrixP))+axis)

/*----------------------------------------------------------------------+
|									|
| name		softConstructionPlane-					|
|									|
| author	BSI					1/94		|
|	 snap or precision overides					|
+----------------------------------------------------------------------*/
Private void	softConstructionPlane
(
Dpoint3d    *outPtP,		/* <=  output point */
Dpoint3d    *inPtP,		/*  => input point */
Dpoint3d    *pointOnPlaneP,	/*  => defines plane's position */
Dpoint3d    *normalVectorP,	/*  => defines plane's rotation */
int	     view		/*  => cursor view */
)
    {
    if (pointIsSnapOrPrecision () || tcb->ndices != 3)
	{
	*outPtP = *inPtP;
	}
    else if (constructionPlane (outPtP, inPtP, pointOnPlaneP, normalVectorP,
	    view, FALSE) != SUCCESS)
	{
	RotMatrix   rMatrix;
	DPoint3d    zVector;

	mdlRMatrix_fromView (&rMatrix,view, FALSE);
	mdlRMatrix_getRowVector (&zVector, &rMatrix, Z);

	constructionPlane (outPtP, inPtP, pointOnPlaneP,
			&zVector ,view , FALSE);
	      /*	RMATRX_VEC(tcb->OLD_view[view].transmatrx, Z) , view, FALSE);*/
	constructionPlane (outPtP, outPtP, pointOnPlaneP,
		normalVectorP, view, TRUE);
	}
    }

/*----------------------------------------------------------------------+
|									|
| name		hardConstructionPlane-					|
|									|
| author	BSI					1/94		|
|	 snap or precision projects to closest point on plane		|
+----------------------------------------------------------------------*/
Private void	hardConstructionPlane
(
Dpoint3d    *outPtP,		/* <=  output point */
Dpoint3d    *inPtP,		/*  => input point */
Dpoint3d    *pointOnPlaneP,	/*  => defines plane's position */
Dpoint3d    *normalVectorP,	/*  => defines plane's rotation */
int	     view		/*  => cursor view */
)
    {
    if (tcb->ndices != 3)
	{
	*outPtP = *inPtP;
	}
    else if (constructionPlane (outPtP, inPtP, pointOnPlaneP, normalVectorP,
				view, pointIsSnapOrPrecision ()) != SUCCESS)
	{
	RotMatrix   rMatrix;
	DPoint3d    zVector;

	mdlRMatrix_fromView (&rMatrix,view, FALSE);
	mdlRMatrix_getRowVector (&zVector, &rMatrix, Z);

	constructionPlane (outPtP, inPtP, pointOnPlaneP, &zVector,view, FALSE);
/*		RMATRX_VEC(tcb->OLD_view[view].transmatrx, Z), view, FALSE);*/
	
/*	constructionPlane (outPtP, inPtP, pointOnPlaneP,
		RMATRX_VEC(tcb->OLD_view[view].transmatrx, Z), view, FALSE);
*/
	constructionPlane (outPtP, outPtP, pointOnPlaneP,
		normalVectorP, view, TRUE);
	}
    }

/*----------------------------------------------------------------------+
|									|
| name		constructionPlane-					|
|									|
| author	BSI					12/93		|
|									|
| Project cursor onto plane in 3d view					|
|									|
| Use the higher level "soft" and "hard" const. plane functions rather	|
| than calling this function directly.	They handle special cases, etc. |
+----------------------------------------------------------------------*/
Private int	constructionPlane
(
Dpoint3d *outPtP,
Dpoint3d *inPtP,
Dpoint3d *pointOnPlaneP,  /* defines plane's position */
Dpoint3d *normalVectorP,  /* defines plane's orientation */
int  view,    /* in and out points will appear identical in this view */
int  perpendicular   /* if TRUE, will ignore view and project to closest */
)
    {
    Dpoint3d	    projectionVector, fromPtP;
    double distance, dotProduct;
    ViewFlags       oViewFlags;
//    ViewInfo	    oViewInfo;

    //because you cannot get this data from the TCB anymore!
    mdlView_getFlags (&oViewFlags,view);
//    mdlView_copyViewInfo (&oViewInfo, view);

    if (perpendicular)
	{
	mdlVec_subtractPoint (&projectionVector, pointOnPlaneP, inPtP);
	distance = mdlVec_dotProduct (&projectionVector, normalVectorP);
	mdlVec_projectPoint (outPtP, inPtP, normalVectorP, distance);
	}
    else
	{
	//if (tcb->OLD_ext_viewflags[view].camera)
	if  (oViewFlags.camera)
	    {
	    //fromPtP = &tcb->OLD_camera[view].position;
	    mdlView_getCamera (&fromPtP,NULL,NULL,NULL,NULL,view);
	    mdlVec_computeNormal (&projectionVector, inPtP, &fromPtP);
	    }
	else
	    {
	    RotMatrix   rMatrix;
	    mdlRMatrix_fromView (&rMatrix,view, FALSE);

	    fromPtP = *inPtP;
	    //projectionVector = *RMATRX_VEC(tcb->OLD_view[view].transmatrx, Z);
	    mdlRMatrix_getRowVector (&projectionVector, &rMatrix, Z);
	    
	    }

	if (fabs (dotProduct = mdlVec_dotProduct (&projectionVector,
				normalVectorP)) < fc_p001)
	    return PARALLEL;

	distance = (mdlVec_dotProduct(normalVectorP, pointOnPlaneP) -
		mdlVec_dotProduct(normalVectorP, &fromPtP))/dotProduct;
	/*tcb->OLD_ext_viewflags[view].camera*/
	if (oViewFlags.camera && distance < fc_epsilon)

//		mdlVec_dotProduct(normalVectorP, fromPtP))/dotProduct;

	    return BEHIND_EYE_POINT;

	mdlVec_projectPoint (outPtP, &fromPtP, &projectionVector, distance);
	}

    return SUCCESS;
    }

/*----------------------------------------------------------------------+
|									|
| name		pointIsSnapOrPrecision- 				|
|									|
| author	BSI					9/93		|
|									|
+----------------------------------------------------------------------*/
Private BoolInt pointIsSnapOrPrecision
(
void
)
    {
    return  (tcb->inDynamics) ?
	    mdlSnap_isActive() 
	    : mdlLocate_getLocateMethod();
    }
