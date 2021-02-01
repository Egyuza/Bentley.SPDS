/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/myapp/myapp.cpp,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   myapp.mc  $
|   $Revision: 1.2 $
|   	$Date: 2013/06/28 20:40:43 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   myapp.mc - MyApp source code.                                       |
|              Illustrates basic MDL concepts and functionality for     |
|              use with the introductory MDL class.                     |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#include <time.h>
#include <stdarg.h>
#include <mdl.h>        /* MDL Library funcs structures & constants */
#include <tcb.h>        /* MicroStation terminal control block */
#include <global.h>     /* structure definitions for global data areas */
#include <dlogitem.h>   /* Dialog Box Manager structures & constants */
#include <cmdlist.h>    /* MicroStation command numbers */
#include <cexpr.h>      /* C Expression structures & constants */
#include <userfnc.h>    /* definitions for accessing MDL user functions */
#include <mselems.h>    /* structures that define MicroStation elements */
#include <rscdefs.h>    /* resource mgr structure definitions & constants */
#include <scanner.h>    /* typedefs and defines for design file scanner */
#include <keys.h>       /* MicroStation keyboard definitions */
#include <mdlio.h>      /* File I/O definitions and data structs for MDL */
#include <toolset.h>	/* MicroStation tool set definitions */
#include <string.h>
#include <dlogids.h>
#include <stdlib.h>

#include "myapp.h"      /* Contains Dlog IDs and typedefs for this app */
#include "myappcmd.h"   /* Contains Command numbers for this app */

#include <dlogman.fdf>
#include <mssystem.fdf>
#include <msoutput.fdf>
#include <msrsrc.fdf>
#include <mscexpr.fdf>
#include <msstate.fdf>
#include <mselemen.fdf>
#include <mselmdsc.fdf>
#include <msmisc.fdf>
#include <msparse.fdf>
#include <mslocate.fdf>
#include <mswindow.fdf>
#include <deprecated8_9.fdf>
#include <msvar.fdf>

/*----------------------------------------------------------------------+
|                                                                       |
|   Local Function Declarations                                         |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
|									|
| Use of predefined MicroStation symbol MSVERSION to compile the correct|
| version of your code.							|
|									|
|   #if defined (MSVERSION) && (MSVERSION >= 0x551)			|
|	code new to MicroStation Version 5.5.1 and higher		|
|   #endif								|
|									|
|   #if defined (MSVERSION) && (MSVERSION >= 0x550)			|
|	code new to MicroStation PowerDraft and higher			|
|   #endif								|
|									|
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
|                                                                       |
|   Private Global variables                                            |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
| The following variable is referenced in C expression strings used
| by the text and option button items defined in myapp.r.
+----------------------------------------------------------------------*/
Private DlogBoxInfo     dlogBoxInfo;

/*----------------------------------------------------------------------+
|                                                                       |
|   MyApp Code Section                                                  |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* Function to find the fence buttons								    	    *
* @bsimethod myapp_findFenceButtons   					*					    	*
* @return 	nothing			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
MdlPublic  void myapp_findFenceButtons
(
void
)
    {
    BoolInt          enableToggle, enableOption; 
    DialogItem      *diP;
    MSWindow	    *current;
    DialogBox*       currentdbP;
     
    /* if a fence is active then set enable flag to TRUE */
    enableToggle = tcb->fence ? TRUE : FALSE;

    /* if the use fence toggle is on and a fence is active then enable
	the fence mode option button */
    enableOption = (tcb->msToolSettings.general.useFence && enableToggle) ? TRUE : FALSE;
     
    /* check all dialogs to set enabled state of UseFence Toggle and option button */
    for (current=mdlWindow_getFirst(); NULL != current; current=mdlWindow_getNext(current))
        {
	/* if the window is not a dialog box then continue looping */
	if (FALSE == mdlWindow_isDialogBox(current))
	    {
	    continue;
	    }

        currentdbP = (DialogBox*) current;

	/* get a pointer to the "Use Fence" toggle button */
        if (NULL != (diP = mdlDialog_itemGetByTypeAndId (currentdbP, 
					    	RTYPE_ToggleButton,
                                                TOGGLEID_UseFence, 0)))
            {
	    /* set the enabled state of the toggle button */
            mdlDialog_itemSetEnabledState (currentdbP, diP->itemIndex, 
				         enableToggle, FALSE);
     
	    /* get a pointer to the fence mode option button */
            if (NULL != (diP = mdlDialog_itemGetByTypeAndId (currentdbP, 
			    	    RTYPE_OptionButton,
				    OPTIONBUTTONID_LockFenceMode, 0)))
	    	{
		/* set the enabled state of the fence mode option button */
                mdlDialog_itemSetEnabledState (currentdbP, diP->itemIndex, 
		    	     enableOption, FALSE);
	    	}

	    }
    	}
    return;
    }

/*======================================================================+
|                                                                       |
|   Settings Command Functions					    	|
|                                                                       |
+======================================================================*/
/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_setUseFence				      	|
|                                                                       |
| author        BSI                                     10/95           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* Sets the UseFence toggle button				    
 Set the enabled state of all occurrences of the "Use Fence:"
 toggle button and the "Fence mode" option button based on
 the current value of msToolSettings.general.useFence.and
 whether or not a fence currently exists.

  Update the "Use Fence" toggle item to reflect the
  new value of the msToolSettings.general.useFence variable.


* @bsimethod myapp_setUseFence   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
* @see myapp_findFenceButtons
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_setUseFence
(
char            *unparsedP
)
/*cmdNumber       CMD_MYAPPSETTING_USEFENCE_TOGGLE,
	    	CMD_MYAPPSETTING_USEFENCE_ON, 
	    	CMD_MYAPPSETTING_USEFENCE_OFF*/
    {
    DialogBox *dbP;

    /* if a fence is not active then do nothing */
    if (tcb->fence == 0)
    	{
	return;
    	}

    switch (mdlCommandNumber)
    	{
    	case CMD_MYAPPSETTING_USEFENCE_TOGGLE:
	    {
	    /* toggle the useFence setting */
	    tcb->msToolSettings.general.useFence = 
		    (tcb->msToolSettings.general.useFence == 0) ? -1 : 0;

	    break;
	    }
    	case CMD_MYAPPSETTING_USEFENCE_ON:
	    {
	    /* set the useFence value to ON */
	    tcb->msToolSettings.general.useFence = -1;
	    break;
	    }
    	case CMD_MYAPPSETTING_USEFENCE_OFF:
	    {
	    /* set the useFence value to OFF */
	    tcb->msToolSettings.general.useFence = 0;
	    break;
	    }
	default:
	    return;
    	}


    /*------------------------------------------------------------- 
    | Set the enabled state of all occurrences of the "Use Fence:"
    | toggle button and the "Fence mode" option button based on
    | the current value of msToolSettings.general.useFence.and
    | whether or not a fence currently exists.
    +-------------------------------------------------------------*/
    myapp_findFenceButtons ();

    /*---------------------------------------------------------- 
    | Update the "Use Fence" toggle item to reflect the
    | new value of the msToolSettings.general.useFence variable.
    +----------------------------------------------------------*/
    if (NULL != (dbP=mdlDialog_find (DIALOGID_ToolSettings, NULL)))
        {
        mdlDialog_itemSynchByTypeAndId (dbP, RTYPE_ToggleButton,
    				     TOGGLEID_UseFence);
        }

    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_setMyDateItem				      	|
|                                                                       |
| author        BSI                                     10/95           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
 Set the date value in the Myapp variable dlogBoxInfo.fmtStr
 and update the date text item in Myapp's settings dialog box to
 show this new value.
 Note that if no unparsedP input is given on the command line
 then the text item's value is set to NULL.

* 								    	    *
* @bsimethod myapp_setMyDateItem   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_setMyDateItem
(
char            *unparsedP
)
/*cmdNumber       CMD_MYAPPSETTING_DATE*/
    {
    DialogBox *dbP;

    /*------------------------------------------------------------- 
    | Set the date value in the Myapp variable dlogBoxInfo.fmtStr
    | and update the date text item in Myapp's settings dialog box to
    | show this new value.
    | Note that if no unparsedP input is given on the command line
    | then the text item's value is set to NULL.
    +-------------------------------------------------------------*/

    /* copy the command line input into the format string variable */
    strncpy(dlogBoxInfo.fmtStr, unparsedP, sizeof(dlogBoxInfo.fmtStr)-1);

    /*----------------------------------------------------- 
    |   Update the date text item to reflect the
    |   new format string value. 
    +-----------------------------------------------------*/
    if (NULL != (dbP=mdlDialog_find (DIALOGID_MyApp, NULL)))
        {
        mdlDialog_itemSynchByTypeAndId (dbP, RTYPE_Text,
    				     TEXTID_UserInfo);
        }

    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_setMyLineStyle				      	|
|                                                                       |
| author        BSI                                     10/95           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
 Set the linestyle value in the Myapp variable dlogBoxInfo.style
 and update the option button in Myapp's settings dialog box to
 show this new value.
 Note that if no unparsedP input is given on the command line
 then the linestyle is set to zero.
* 								    	    *
* @bsimethod myapp_setMyLineStyle   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_setMyLineStyle
(
char            *unparsedP
)
//cmdNumber       CMD_MYAPPSETTING_LINESTYLE
    {
    int	lineStyleValue;

    /*------------------------------------------------------------- 
    | Set the linestyle value in the Myapp variable dlogBoxInfo.style
    | and update the option button in Myapp's settings dialog box to
    | show this new value.
    | Note that if no unparsedP input is given on the command line
    | then the linestyle is set to zero.
    +-------------------------------------------------------------*/

    /* convert the command line input to a long integer */
    lineStyleValue = atol (unparsedP);

    /* if line style value is invalid then output an error */
    if ((lineStyleValue > 7) || (lineStyleValue < 0))
    	{
	mdlOutput_rscPrintf (MSG_ERROR, NULL, MESSAGELISTID_Msgs, 
		    	MSGID_InvalidLineStyle);
    	}
    else
    	{
	DialogBox *dbP;

    	/* set the linestyle number to the given input */
    	dlogBoxInfo.style = lineStyleValue;

	/*----------------------------------------------------- 
	|   Update the line style option button to reflect the
	|   new line style value. 
	+-----------------------------------------------------*/
    	if (NULL != (dbP=mdlDialog_find (DIALOGID_MyApp, NULL)))
	    {
	    mdlDialog_itemSynchByTypeAndId (dbP, RTYPE_OptionButton,
					     OPTIONBUTTONID_LineStyle);
	    }
    	}
    }

/*======================================================================+
|                                                                       |
|   Open Dialog Command Functions				    	|
|                                                                       |
+======================================================================*/
/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_openMyMainDialog                                  |
|                                                                       |
| author        BSI                                     10/95           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
 Open our main dialog box with the function mdlDialog_open.
 The NULL parameter means to look for the Dialog Box resource
 in the file that the application is from.
* 								    	    *
* @bsimethod myapp_openMyMainDialog   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_openMyMainDialog
(
char            *unparsedP
)
//cmdNumber       CMD_OPEN_MYMAINDIALOG
    {

    /*------------------------------------------------------------- 
    | Open our main dialog box with the function mdlDialog_open.
    | The NULL parameter means to look for the Dialog Box resource
    | in the file that the application is from.
    +-------------------------------------------------------------*/
    if (NULL == mdlDialog_open (NULL, DIALOGID_MyApp))
	{
	/* On error, output a message and unload the application */
	mdlOutput_rscPrintf (MSG_ERROR, NULL, MESSAGELISTID_Msgs, 
				    MSGID_DialogOpen);
	mdlDialog_cmdNumQueueExt (FALSE, CMD_MDL_UNLOAD,
			       mdlSystem_getCurrTaskID(), TRUE, TRUE);
	}
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_openMyToolFrame                                   |
|                                                                       |
| author        BSI                                     10/95           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* open our icon command frame								    	    *
* @bsimethod myapp_openMyToolFrame   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_openMyToolFrame
(
char            *unparsedP
)
//cmdNumber       CMD_OPEN_MYTOOLFRAME
    {

    /* open our icon command frame */
    if (NULL == mdlDialog_open (NULL, DIALOGID_MyAppFrame))
    	{
	/* On error, output a message */
    	mdlOutput_rscPrintf (MSG_ERROR, NULL, MESSAGELISTID_Msgs, 
			    	MSGID_PaletteOpen);
    	}
    }

/*======================================================================+
|                                                                       |
|   Place MyDate State and Dynamics Functions                           |
|                                                                       |
+======================================================================*/
/*----------------------------------------------------------------------+
|                                                                       |
| name          createText                                              |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* Creates the text element
Simple routine to create our date stamp from the
format string contained in the text items access string structure.
								    	    *
* @bsimethod createText   						*					    	*
* @param 	pntP     Data point to create the text	    	*
* @param 	view     view to create the text	    	*
* @param 	elP     The element to create	    	*
* @return 	SUCCESS 			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
MdlPublic  int createText
(
Dpoint3d    *pntP, /*Data point to create the text*/
int         view,  /*view to create the text*/
MSElement   *elP   /*The element to create*/
)
    {
    time_t  t;
    struct  tm timeInfo;
    char    str[255];

    /*-----------------------------------------------------------------
    | Simple routine to create our date stamp from the
    | format string contained in the text items access string structure.
    +-----------------------------------------------------------------*/
    time (&t);
    timeInfo = *gmtime (&t);

    if (strcmpi (dlogBoxInfo.fmtStr, "MM:DD:YY") == 0)
	sprintf (str, "%02d/%02d/%02d", timeInfo.tm_mon+1, 
		    timeInfo.tm_mday, (timeInfo.tm_year+1900)%100);
    else if (strcmpi (dlogBoxInfo.fmtStr, "DD:MM:YY") == 0)
	sprintf (str, "%02d/%02d/%02d", timeInfo.tm_mday, 
		    timeInfo.tm_mon+1, (timeInfo.tm_year+1900)%100);
    else if (strcmpi (dlogBoxInfo.fmtStr, "YY:MM:DD") == 0)
	sprintf (str, "%02d/%02d/%02d", (timeInfo.tm_year+1900)%100, 
		    timeInfo.tm_mon+1, timeInfo.tm_mday);
    else if (strcmpi (dlogBoxInfo.fmtStr, "MM:DD:YYYY") == 0)
	sprintf (str, "%02d/%02d/%04d", timeInfo.tm_mon+1, 
		    timeInfo.tm_mday, timeInfo.tm_year+1900);
    else if (strcmpi (dlogBoxInfo.fmtStr, "DD:MM:YYYY") == 0)
	sprintf (str, "%02d/%02d/%04d", timeInfo.tm_mday, 
		    timeInfo.tm_mon+1, timeInfo.tm_year+1900);
    else if (strcmpi (dlogBoxInfo.fmtStr, "YYYY:MM:DD") == 0)
	sprintf (str, "%04d/%02d/%02d", (char *)timeInfo.tm_year+1900, 
		    timeInfo.tm_mon+1, timeInfo.tm_mday);
    else
	strcpy (str, dlogBoxInfo.fmtStr);

    /* Temporarily create the text element in the element passed in */
    mdlText_create (elP, NULL, str, NULL, NULL, NULL, NULL, NULL);
    mdlElement_offset (elP, elP, pntP);

    return SUCCESS;
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          generateText                                            |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* acts as a layer betweeen the createText and the calling functions for dynamics    
* @bsimethod generateText   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void generateText
(
DPoint3d        *pntP, /*Data point to create the text*/
int             view   /*view to create the text*/
)
    {
    createText (pntP, view, dgnBuf);
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          placeDate_done                                          |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
Start the default command, after you have placed the text element
for the date stamp.  This is a single shot tool.
* 								    	    *
* @bsimethod placeDate_done   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void placeDate_done
(
void
)
    {
    /*-----------------------------------------------------------------
    | Start the default command, after you have placed the text element
    | for the date stamp.  This is a single shot tool.
    +-----------------------------------------------------------------*/
    mdlState_startDefaultCommand ();
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          placeDate_atPoint                                       |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
As the location for our date stamp can be specified by a single
datapoint use the dynamics function to create our text element
in dgnBuf at it's final location.
* 								    	    *
* @bsimethod placeDate_atPoint   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void placeDate_atPoint
(
Dpoint3d        *pntP
)
    {
    MSElement   el;

    /*-----------------------------------------------------------------
    | As the location for our date stamp can be specified by a single
    | datapoint use the dynamics function to create our text element
    | in dgnBuf at it's final location.
    +-----------------------------------------------------------------*/
    createText (pntP, tcb->lstvw, &el);

    /* Display the new text element and add it to the design file */
    mdlElement_display (&el, NORMALDRAW);
    mdlElement_add (&el);

    /* Call our cleanup function terminating the primitive command */
    placeDate_done ();
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          placeDate_start                                         |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
mdlState_startPrimitive:

This function will start a primitive command, which   
is a command that will create or alter design file    
elements.                                               
                                                
The first argument represents the data point function.
This is the function which will get called when we    
hit a datapoint (which in this case is the origin
point of our text element).                                     
                                                
The second argument represents the reset function.    
This is the function which will get called when we    
hit a reset (which in this case means that we want to 
exit the primitive command.                             
                                                
The third and fourth arguments are just indexes into  
our message list, so that we may display the            
appropriate command and prompt messages.        

Start dynamics to display elements when one more datapoint will 
give enough information to write an element to the design file.
For our text element, we can start dynamics right away.

* 								    	    *
* @bsimethod placeDate_start   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
void placeDate_start
(
char            *unparsedP
)
//cmdNumber       CMD_PLACE_MYDATE
    {
    /*------------------------------------------------------
    | mdlState_startPrimitive:
    | 
    | This function will start a primitive command, which   
    | is a command that will create or alter design file    
    | elements.                                               
    |                                                 
    | The first argument represents the data point function.
    | This is the function which will get called when we    
    | hit a datapoint (which in this case is the origin
    | point of our text element).                                     
    |                                                 
    | The second argument represents the reset function.    
    | This is the function which will get called when we    
    | hit a reset (which in this case means that we want to 
    | exit the primitive command.                             
    |                                                 
    | The third and fourth arguments are just indexes into  
    | our message list, so that we may display the            
    | appropriate command and prompt messages.        
    +-------------------------------------------------------*/
    mdlState_startPrimitive (placeDate_atPoint, placeDate_done, 
			    COMMANDID_PlaceDate, PROMPTID_EnterPoint);

    /*-----------------------------------------------------------------
    | Start dynamics to display elements when one more datapoint will 
    | give enough information to write an element to the design file.
    | For our text element, we can start dynamics right away.
    +-----------------------------------------------------------------*/
    mdlState_dynamicUpdate (generateText, TRUE);

    }

/*======================================================================+
|                                                                       |
|   Place MyLine state and dynamic Functions                            |
|                                                                       |
+======================================================================*/
/*----------------------------------------------------------------------+
|                                                                       |
| name          createLine                                              |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* create the line element in the area passed in								    	    *
* @bsimethod createLine   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  int createLine
(
Dpoint3d        *pntP,
int             view,
MSElement       *elP
)
    {
    statedata.dPointStack[1] = *pntP;

    /* create the line element in the area passed in */
    mdlLine_create (elP, NULL, statedata.dPointStack);
    mdlElement_setSymbology (elP, NULL, NULL, &dlogBoxInfo.style);

    return SUCCESS;
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          generateLine                                            |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* create the line element in dynamics								    	    *
* @bsimethod generateLine   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void generateLine
(
DPoint3d        *pntP,
int             view
)
    {
    createLine (pntP, view, dgnBuf);
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          placeLine_done                                          |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
Restart the place line command.  If single-shot is enabled then
the default command will be activated instead of place myline.
* 								    	    *
* @bsimethod placeLine_done   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void placeLine_done
(
void
)
    {
    /*---------------------------------------------------------------
    | Restart the place line command.  If single-shot is enabled then
    | the default command will be activated instead of place myline.
    +--------------------------------------------------------------*/
    mdlState_restartCurrentCommand ();
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          placeLine_secondPoint                                   |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* 
Create line in local storage based on the last datapoint
Display the created line segment and add it to the design file
Place lines until a reset, so prompt for next end point.
mdlState_setFunction:

This function is called because at this point we need 
to set up a reset function.  Now that the user has placed
at least one line segment, we no longer want the reset
function to be placeLine_start().  The call below to the 
function sets up the reset event for the function placeLine_done.
When the user hits a reset, placeLine_done is called by MicroStation
and placeLine_done performs cleanup for this command.      

So that it works like the MicroStation PLACE LINE we start a new
line from the end of the last line.  We will continue
until a reset is pressed.
  								    	    *
* @bsimethod placeLine_secondPoint   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void placeLine_secondPoint
(
Dpoint3d        *pntP,
int             view
)
    {
    MSElementUnion el;
    
    /* Create line in local storage based on the last datapoint */
    createLine (pntP, view, &el);

    /* Display the created line segment and add it to the design file */
    mdlElement_display (&el, NORMALDRAW);
    mdlElement_add (&el);

    /* Place lines until a reset, so prompt for next end point. */
    mdlOutput_rscPrintf (MSG_PROMPT, 0, MESSAGELISTID_Prompts,
			PROMPTID_EnterNext);

    /*--------------------------------------------------------------------
    | mdlState_setFunction:
    | 
    | This function is called because at this point we need 
    | to set up a reset function.  Now that the user has placed
    | at least one line segment, we no longer want the reset
    | function to be placeLine_start().  The call below to the 
    | function sets up the reset event for the function placeLine_done.
    | When the user hits a reset, placeLine_done is called by MicroStation
    | and placeLine_done performs cleanup for this command.      
    +---------------------------------------------------------------------*/
    mdlState_setFunction (STATE_RESET, placeLine_done);

    /*--------------------------------------------------------------------
    | So that it works like the MicroStation PLACE LINE we start a new
    | line from the end of the last line.  We will continue
    | until a reset is pressed.
    +---------------------------------------------------------------------*/
    statedata.dPointStack[0] = *pntP;

    /* Check is single shot mode is enabled */
    mdlState_checkSingleShot();
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          placeLine_firstPoint                                    | 
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
mdlState_setFunction:

This function is called because at this point we need 
to set up a datapoint function.  
The call below to the function sets up the datapoint      
function. Since the reset function is still placeLine_start ()
there is no need to change it.

Start dynamics to display elements as soon as you have enough
information to tell something about them, for a line this means
that we need at least one point.  Until the second point in the
line is entered, our dynamic update routine will just "rubber-band"
a single line from the original starting point.                                          

* 								    	    *
* @bsimethod placeLine_firstPoint   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void placeLine_firstPoint
(
Dpoint3d        *pntP,
int             view
)
    {
    /* Save the first data point  */
    statedata.dPointStack[0] = *pntP;

    /*-----------------------------------------------------------------
    | mdlState_setFunction:
    | 
    | This function is called because at this point we need 
    | to set up a datapoint function.  
    | The call below to the function sets up the datapoint      
    | function. Since the reset function is still placeLine_start ()
    | there is no need to change it.
    +------------------------------------------------------------------*/
    mdlState_setFunction (STATE_DATAPOINT, placeLine_secondPoint);

    /*------------------------------------------------------------------
    | Start dynamics to display elements as soon as you have enough
    | information to tell something about them, for a line this means
    | that we need at least one point.  Until the second point in the
    | line is entered, our dynamic update routine will just "rubber-band"
    | a single line from the original starting point.                                          
    +------------------------------------------------------------------*/
    mdlState_dynamicUpdate (generateLine, FALSE);

    /* Output prompt message requesting the second point */
    mdlOutput_rscPrintf (MSG_PROMPT, 0, MESSAGELISTID_Prompts,
			PROMPTID_EnterNext);
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          placeLine_start                                         |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
mdlState_startPrimitive:

This function will start a primitive command, which   
is a command that will create or alter design file    
elements.                                               
                                                
The first argument represents the data point function.
This is the function which will get called when we    
hit a datapoint (which in this case is the first      
point in our line).                                     
                                                
The second argument represents the reset function.    
This is the function which will get called when we    
hit a reset (which in this case means that we want to 
exit the primitive command.                             
                                                
The third and fourth arguments are just indexes into  
our message list, so that we may display the            
appropriate command and prompt messages.        
* 								    	    *
* @bsimethod placeLine_start   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
void placeLine_start
(
char            *unparsedP
)
//cmdNumber       CMD_PLACE_MYLINE
    {
    /*------------------------------------------------------------------
    | mdlState_startPrimitive:
    | 
    | This function will start a primitive command, which   
    | is a command that will create or alter design file    
    | elements.                                               
    |                                                 
    | The first argument represents the data point function.
    | This is the function which will get called when we    
    | hit a datapoint (which in this case is the first      
    | point in our line).                                     
    |                                                 
    | The second argument represents the reset function.    
    | This is the function which will get called when we    
    | hit a reset (which in this case means that we want to 
    | exit the primitive command.                             
    |                                                 
    | The third and fourth arguments are just indexes into  
    | our message list, so that we may display the            
    | appropriate command and prompt messages.        
    +------------------------------------------------------------------*/

    mdlState_startPrimitive (placeLine_firstPoint, placeLine_start, 
			    COMMANDID_PlaceMyLine, PROMPTID_EnterFirst);
    }

/*======================================================================+
|                                                                       |
|   Change MySymbology state and dynamic Functions                      |
|                                                                       |
+======================================================================*/
/*----------------------------------------------------------------------+
|                                                                       |
| name          modElm                                                  |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
Set symbology for selected element, return MODIFY_STATUS_REPLACE
to rewrite element to file replacing it's old definition.
* 								    	    *
* @bsimethod modElm   							*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  int modElm
(
MSElementUnion  *elP,       /* <=> element to be modified   	*/
void	    	*params,    /*  => user parameter	    	*/
DgnModelRefP   	pModelRef,    /*  => Model ref for current element  	*/
MSElementDescr	*elmDscrP,  /*  => element descr for element	*/
MSElementDescr	**newDscrPP /*  <= if replacing entire descr	*/
)
    {
    UInt32 color, weight;
    int style;

    /*------------------------------------------------------------------
    | Set symbology for selected element, return MODIFY_STATUS_REPLACE
    | to rewrite element to file replacing it's old definition.
    +------------------------------------------------------------------*/
    color  = tcb->symbology.color;
    weight = tcb->symbology.weight;
    style  = dlogBoxInfo.style;

    mdlElement_setSymbology (elP, &color, &weight, &style);

    return  MODIFY_STATUS_REPLACE;
    }

/*----------------------------------------------------------------------+*//**
* !!!Describe Function Completely!!!								    	    *
* @bsimethod modElm2   							*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   MarkAnderson 				07/01 		*
*								        *
+----------------------------------------------------------------------*/
Private  int modElm2
(
MSElementUnion  *elP,       /* <=> element to be modified   	*/
void	    	*params,    /*  => user parameter	    	*/
DgnModelRefP   	pModelRef,    /*  => Model ref for current element  	*/
MSElementDescr	*elmDscrP,  /*  => element descr for element	*/
MSElementDescr	**newDscrPP /*  <= if replacing entire descr	*/
)
    {
    return MODIFY_STATUS_NOCHANGE;
    }

/*----------------------------------------------------------------------+*//**
The located element was accepted using a data point so get the
accepted element and call mdlModify_elementMulti to change it's
symbology.
* 								    	    *
* @bsimethod acceptElm   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void acceptElmDescr
(
Dpoint3d    *pntP,
int         view
)
    {
    ULong	    filePos;
    DgnModelRefP    currFileP;

    /*------------------------------------------------------------------
    | The located element was accepted using a data point so get the
    | accepted element and call mdlModify_elementMulti to change it's
    | symbology.
    +------------------------------------------------------------------*/
    filePos = mdlElement_getFilePos (FILEPOS_CURRENT, &currFileP);
    mdlModify_elementMulti (currFileP, filePos, MODIFY_REQUEST_HEADERS,
			    MODIFY_ORIG, modElm, NULL, TRUE);
    /* Restart the locate logic */
    mdlLocate_restart (FALSE);
    }


/*----------------------------------------------------------------------+
|									|
| name          fenceModElm                                             |
|									|
| author	BSI     				11/92		|
|									|
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* Process Fence Selection								    	    *
* @bsimethod fenceModElm   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  int fenceModElm
(
void	  *arg
)
    {
    ULong	    filePos;
    DgnModelRefP    currFileP;


    filePos = mdlElement_getFilePos (FILEPOS_CURRENT, &currFileP);
    mdlModify_elementSingle (currFileP, filePos, MODIFY_REQUEST_NOHEADERS,
			    MODIFY_ORIG, modElm, NULL, 0L);

    return SUCCESS;
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          acceptElm                                               |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
The located element was accepted using a data point so get the
accepted element and call mdlModify_elementMulti to change it's
symbology.
* 								    	    *
* @bsimethod acceptElm   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void acceptElm
(
Dpoint3d    *pntP,
int         view
)
    {
    ULong	    filePos;
    DgnModelRefP    currFileP;

    /*------------------------------------------------------------------
    | The located element was accepted using a data point so get the
    | accepted element and call mdlModify_elementMulti to change it's
    | symbology.
    +------------------------------------------------------------------*/
    filePos = mdlElement_getFilePos (FILEPOS_CURRENT, &currFileP);
    mdlModify_elementMulti (currFileP, filePos, MODIFY_REQUEST_HEADERS,
			    MODIFY_ORIG, modElm, NULL, TRUE);

    /* Restart the locate logic */
    mdlLocate_restart (FALSE);
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          setElmSearchType                                        |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* Set the search type mask to lines and text only
								    	    *
* @bsimethod setElmSearchType   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void setElmSearchType
(
void
)
    {
    static int  searchType[] = {LINE_ELM, TEXT_ELM,};

    /* Clear search criteria */
    mdlLocate_noElemNoLocked ();
    /* Set search mask to look for text and line elements */
    mdlLocate_setElemSearchMask (sizeof(searchType)/sizeof(int), searchType);
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_changeSymbology                                   |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* The command to change a line or text element symbology
								    	    *
* @bsimethod myapp_changeSymbology   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
void myapp_changeSymbology
(
char            *unparsedP
)
//       CMD_CHANGE_MYSYMBOLOGY
    {

    /* Setup search mask with the type of elements we are looking for */
    setElmSearchType();

    /* If the user wants to use a fence operation */
    if (0 != tcb->msToolSettings.general.useFence)
	{
	if (tcb->fence > 0)
	    {
	    mdlState_startFenceCommand(fenceModElm, /* content func */
		       	   NULL,		    /* outline func */
		       	   NULL,		    /* data point func */
		       	   myapp_changeSymbology,   /* reset func */
		       	   PROMPTID_AcceptFence,    /* prompt message */
		       	   COMMANDID_ChangeSymbologyFence, /* command message */
		       	   FENCE_NO_CLIP);	    /* clipping mode */
	    }
	else
	    {
	    mdlOutput_rscPrintf (MSG_ERROR, NULL, MESSAGELISTID_Msgs, MSGID_NoFence);
	    tcb->msToolSettings.general.useFence = FALSE;
	    }
	}
    else
	{

    	mdlState_startModifyCommand (myapp_changeSymbology, /* reset func */
			   acceptElm,    /* datapoint func */
			   NULL,         /* dynamics func */
			   NULL,         /* show func */
			   NULL,         /* clean func */
			   COMMANDID_ChangeSymbology, /* command field message */
			   PROMPTID_IdentifyElt,      /* prompt message */
			   TRUE,         /* use selection sets */
			   0);           /* points required for accept */

    	/* Start search at beginning of file */
    	mdlLocate_init ();
	}

    }

/*======================================================================+
|                                                                       |
|   Dialog Hook Functions                                               |
|                                                                       |
+======================================================================*/
/*----------------------------------------------------------------------+
|                                                                       |
|   DialogMessage structure                                             |
|                                                                       |
|   The dialogmessage structure that is the only argument to a dialog   |
|   hook function is declared as follows (from "dlogitem.h"):           |
|                                                                       |
|   typedef struct dialogmessage                                        |
|      {                                                                |
|      BoolInt      msgUnderstood; # <= message understood?             |
|      int          messageType;   # => message type                    |
|      DialogBox   *db;            # => dialog box handle               |
|      long         dialogId;      # => resource id of dialog box       |
|      void        *userDataP;     # => set by user during CREATE       |
|      union                                                            |
|          {                                                            |
|          ...                                                          |
|          structures that contain information that                     |
|          depends on messageType.                                      |
|          } u;                                                         |
|      } DialogMessage;                                                 |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_dlogHook                                          |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
DialogMessage structure                                             
                                                                    
The dialogmessage structure that is the only argument to a dialog   <p>
hook function is declared as follows (from "dlogitem.h"):           <p>
                                                                    <p>
typedef struct dialogmessage                                        <p>
   {                                                                <p>
   BoolInt      msgUnderstood; # <= message understood?             <p>
   int          messageType;   # => message type                    <p>
   DialogBox   *db;            # => dialog box handle               <p>
   long         dialogId;      # => resource id of dialog box       <p>
   void        *userDataP;     # => set by user during CREATE       <p>
   union							    <p>
       {                                                            <p>
       ...                                                          <p>
       structures that contain information that                     <p>
       depends on messageType.                                      <p>
       } u;                                                         <p>
   } DialogMessage;                                                 <p>
								    <p>
We want our application to unload if the Close icon is
double-clicked.  The proper way to do this is to queue up a 
command to unload our application.  This is equivalent to the 
user key-in MDL U MYAPP. mdlSystem_getCurrTaskID returns a 
character pointer that points to the current task's task ID.

Also, in case the user is in the middle of
dynamics, etc. we specify a restart of the default command,
usually the element selection tool.


* 								    	    *
* @bsimethod myapp_dlogHook   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_dlogHook
(
DialogMessage   *dmP    /* => a ptr to a dialog message */
)
    {
    dmP->msgUnderstood = TRUE;

    switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_DESTROY:
	    {
	    /*-----------------------------------------------------------
	    | We want our application to unload if the Close icon is
	    | double-clicked.  The proper way to do this is to queue up a 
	    | command to unload our application.  This is equivalent to the 
	    | user key-in MDL U MYAPP. mdlSystem_getCurrTaskID returns a 
	    | character pointer that points to the current task's task ID.
	    | 
	    | Also, in case the user is in the middle of
	    | dynamics, etc. we specify a restart of the default command,
	    | usually the element selection tool.
	    +----------------------------------------------------------*/
	    mdlDialog_cmdNumQueueExt (FALSE, CMD_MDL_UNLOAD,
			          mdlSystem_getCurrTaskID(), TRUE, TRUE);
	    mdlState_startDefaultCommand ();
	    break;
	    };
	default:
	    dmP->msgUnderstood = FALSE;
	    break;
	}
    }

/*----------------------------------------------------------------------+
|                                                                       |
|   DialogItemMessage structure                                         |
|                                                                       |
|   The DialogItemMessage structure that is the only argument to an     |
|   item hook function is declared as follows (from "dlogitem.h"):      |
|                                                                       |
|   typedef struct dialogitemmessage                                    |
|      {                                                                |
|      BoolInt      msgUnderstood;  # <= item understood the message    |
|      int          messageType;    # => message type                   |
|      DialogBox   *db;             # => dialog box handle              |
|      long         dialogId;       # => resource id of dialog box      |
|      int          itemIndex;      # => index of item msg sent to      |
|      DialogItem  *dialogItemP;    # => dialogItem info for item       |
|      int          auxMessageType; # => item specific message type     |
|      void        *auxInfoP;       # => item specific message info     |
|      union                                                            |
|          {                                                            |
|          ...                                                          |
|          structures that contain information that                     |
|          depends on messageType.                                      |
|          } u;                                                         |
|      } DialogItemMessage;                                             |
|                                                                       |
|   Structures that contain information that depends on messageType     |
|   are discussed in the MDL Programmer's Guide in the section on the   |
|   individual dialog item messages.                                    |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_doneHook                                          |
|                                                                       |
| author        BSI                                     8/93            |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
|   DialogItemMessage structure                                         |
|                                                                       |
|   The DialogItemMessage structure that is the only argument to an     |
|   item hook function is declared as follows (from "dlogitem.h"):      |
|                                                                       |
|   typedef struct dialogitemmessage                                    |
|      {                                                                |
|      BoolInt      msgUnderstood;  # <= item understood the message    |
|      int          messageType;    # => message type                   |
|      DialogBox   *db;             # => dialog box handle              |
|      long         dialogId;       # => resource id of dialog box      |
|      int          itemIndex;      # => index of item msg sent to      |
|      DialogItem  *dialogItemP;    # => dialogItem info for item       |
|      int          auxMessageType; # => item specific message type     |
|      void        *auxInfoP;       # => item specific message info     |
|      union                                                            |
|          {                                                            |
|          ...                                                          |
|          structures that contain information that                     |
|          depends on messageType.                                      |
|          } u;                                                         |
|      } DialogItemMessage;                                             |
|                                                                       |
|   Structures that contain information that depends on messageType     |
|   are discussed in the MDL Programmer's Guide in the section on the   |
|   individual dialog item messages. 
	    | We want our application to unload if the Done pushbutton is
	    | pressed.  The proper way to do this is to queue up a command
	    | to unload our application.  This is equivalent to the user
	    | key-in MDL U MYAPP. mdlSystem_getCurrTaskID returns a character
	    | pointer that points to the current task's task ID.
	    | 
	    | Also, in case the user is in the middle of
	    | dynamics, etc. we specify a restart of the default command,
	    | usually the element selection tool.
                                   |
* 								    	    *
* @bsimethod myapp_doneHook   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_doneHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
    {
    dimP->msgUnderstood = TRUE;

    switch (dimP->messageType)
	{
	case DITEM_MESSAGE_QUEUECOMMAND:
	    {
	    /*-----------------------------------------------------------
	    | We want our application to unload if the Done pushbutton is
	    | pressed.  The proper way to do this is to queue up a command
	    | to unload our application.  This is equivalent to the user
	    | key-in MDL U MYAPP. mdlSystem_getCurrTaskID returns a character
	    | pointer that points to the current task's task ID.
	    | 
	    | Also, in case the user is in the middle of
	    | dynamics, etc. we specify a restart of the default command,
	    | usually the element selection tool.
	    +----------------------------------------------------------*/
	    mdlDialog_cmdNumQueueExt (FALSE, CMD_MDL_UNLOAD,
			      mdlSystem_getCurrTaskID(), TRUE, TRUE);
	    mdlState_startDefaultCommand ();
	    break;
	    };
	default:
	    dimP->msgUnderstood = FALSE;
	    break;
	}
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_menuHook                                          |
|                                                                       |
| author        BSI                                     10/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
	    | If MyApp is selected from the Palette Menu pulldown open the
	    | icon command frame.
* 								    	    *
* @bsimethod myapp_menuHook   						*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_menuHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
    {
    dimP->msgUnderstood = TRUE;

    switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	    {
	    /*-----------------------------------------------------------
	    | If MyApp is selected from the Palette Menu pulldown open the
	    | icon command frame.
	    +----------------------------------------------------------*/
	    if (dimP->u.button.buttonTrans == BUTTONTRANS_UP)
		{
		/* queue a command to open our tool frame */
	    	mdlDialog_cmdNumQueueExt (TRUE, CMD_OPEN_MYTOOLFRAME, "",
			    	 TRUE, TRUE);
		}
	    break;
	    };

	default:
	    dimP->msgUnderstood = FALSE;
	    break;
	}
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_useFenceHook                                      |
|                                                                       |
| author        BSI                                     10/95           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
    | If the use fence toggle is on and a fence is active then enable
    | the fence mode option button 
	    | Journal the "MYAPPSETTING USEFENCE TOGGLE" command for 
	    | BASIC macro generation instead of allowing the default 
	    | action of the macro generator to happen.  The default 
	    | action for this event is for the macro generator to add  
	    | BASIC code to set the msToolSettings.general.useFence 
	    | variable to the value entered in the "Use Fence:" toggle
	    | button item.
	    | 
	    | The default activity is overridden so the generated BASIC 
	    | code is more understandable.
	    |
	    |   int 	mdlSystem_journalCommand
	    |   (
	    |   char	*taskIdP,   => task id string (may be NULL)
	    |   long	 cmdnum,    => command number
	    |   char	*unparsedP  => unparsed part of command (may be NULL)
	    |   );

* 								    	    *
* @bsimethod myapp_useFenceHook   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_useFenceHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
    {
    BoolInt 	enableOption, enableToggle; 
    DialogItem  *obtnP;

    /* if a fence is active then set enable flag to TRUE */
    enableToggle = tcb->fence ? TRUE : FALSE;

    /*---------------------------------------------------------------
    | If the use fence toggle is on and a fence is active then enable
    | the fence mode option button 
    +---------------------------------------------------------------*/
    enableOption = (tcb->msToolSettings.general.useFence && enableToggle) ? TRUE : FALSE;

    dimP->msgUnderstood = TRUE;

    switch (dimP->messageType)
	{

        case DITEM_MESSAGE_ALLCREATED:
            {
	    /* set the enabled state of the toggle button */
            mdlDialog_itemSetEnabledState (dimP->db, dimP->itemIndex, 
					    enableToggle, FALSE);
     
	    /* find MicroStation's fence mode option button */
            if (NULL != (obtnP = mdlDialog_itemGetByTypeAndId (dimP->db, 
		    RTYPE_OptionButton,
                    OPTIONBUTTONID_LockFenceMode, 0)))
		{
		    	
		/* set the enabled state of the option button */
                mdlDialog_itemSetEnabledState (dimP->db, obtnP->itemIndex, 
			    	enableOption, FALSE);
		}
     
            break;
            }

        case DITEM_MESSAGE_STATECHANGED:
            {
	    /* set the enabled state of use fence & fence mode items */
            myapp_findFenceButtons ();
            break;
            }

	case DITEM_MESSAGE_JOURNALSTATE:
	    {
	    int status;

	    /*------------------------------------------------------- 
	    | Journal the "MYAPPSETTING USEFENCE TOGGLE" command for 
	    | BASIC macro generation instead of allowing the default 
	    | action of the macro generator to happen.  The default 
	    | action for this event is for the macro generator to add  
	    | BASIC code to set the msToolSettings.general.useFence 
	    | variable to the value entered in the "Use Fence:" toggle
	    | button item.
	    | 
	    | The default activity is overridden so the generated BASIC 
	    | code is more understandable.
	    |
	    |   int 	mdlSystem_journalCommand
	    |   (
	    |   char	*taskIdP,   => task id string (may be NULL)
	    |   long	 cmdnum,    => command number
	    |   char	*unparsedP  => unparsed part of command (may be NULL)
	    |   );
	    |
	    +------------------------------------------------------*/
	    status = mdlSystem_journalCommand (mdlSystem_getCurrTaskID(),
				    	 CMD_MYAPPSETTING_USEFENCE_TOGGLE, 
					 "");

	    	/* if the command was successfully journaled then */
	    if (SUCCESS == status)
		{
		/*-------------------------------------------------
		| Tell the macro generator not to add information
		| to the BASIC macro by setting the hookHandled
		| flag to TRUE
		+------------------------------------------------*/
		dimP->u.value.hookHandled = FALSE;
		}

	    break;
	    };

	default:
	    dimP->msgUnderstood = FALSE;
	    break;
	}
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_dateItemHook				    	|
|                                                                       |
| author        BSI                                     10/95           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
		| Journal the "MYAPPSETTING DATE" command for BASIC macro generation
		| instead of allowing the default action of the macro
		| generator to happen.  The default action for this event
		| is for the macro generator to add BASIC code to set 
		| the dlogBoxInfo.fmtStr variable to the value entered
		| in the date text item.
		| 
		| The default activity is overridden so the generated BASIC 
		| code is more understandable.
		|
	    	|   int 	mdlSystem_journalCommand
	    	|   (
	    	|   char	*taskIdP,   => task id string (may be NULL)
	    	|   long	 cmdnum,    => command number
	    	|   char	*unparsedP  => unparsed part of command (may be NULL)
	    	|   );
* 								    	    *
* @bsimethod myapp_dateItemHook   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_dateItemHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
    {
    dimP->msgUnderstood = TRUE;

    switch (dimP->messageType)
	{
	case DITEM_MESSAGE_JOURNALSTATE:
	    {

	    /* if the value entered in the text item is not NULL */
	    if (NULL != dimP->u.value.stringValueP)
		{
		int status;

		/*------------------------------------------------------- 
		| Journal the "MYAPPSETTING DATE" command for BASIC macro generation
		| instead of allowing the default action of the macro
		| generator to happen.  The default action for this event
		| is for the macro generator to add BASIC code to set 
		| the dlogBoxInfo.fmtStr variable to the value entered
		| in the date text item.
		| 
		| The default activity is overridden so the generated BASIC 
		| code is more understandable.
		|
	    	|   int 	mdlSystem_journalCommand
	    	|   (
	    	|   char	*taskIdP,   => task id string (may be NULL)
	    	|   long	 cmdnum,    => command number
	    	|   char	*unparsedP  => unparsed part of command (may be NULL)
	    	|   );
	    	|
		+------------------------------------------------------*/
	    	status = mdlSystem_journalCommand (mdlSystem_getCurrTaskID(),
				    	 CMD_MYAPPSETTING_DATE, 
					 dimP->u.value.stringValueP);

	    	/* if the command was successfully journaled then */
		if (SUCCESS == status)
		    {
		    /*-------------------------------------------------
		    | Tell the macro generator not to add information
		    | to the BASIC macro by setting the hookHandled
		    | flag to TRUE
		    +------------------------------------------------*/
		    dimP->u.value.hookHandled = TRUE;
		    }
		}

	    break;
	    };

	default:
	    dimP->msgUnderstood = FALSE;
	    break;
	}
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_elementStyleHook                                  |
|                                                                       |
| author        BSI                                     10/95           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
	    |  For the DITEM_MESSAGE_JOURNALSTATE event, the "value"
	    |  member of the DialogItemMessage union is used.
	    |
	    |  Value structure from the DialogItemMessage structure.
	    |	(see dlogitem.h):
	    |
	    |	struct		
	    |       {
	    |	    BoolInt	 hookHandled;   <= only for hooks,TRUE if handled 
	    |	    BoolInt	 valueChanged;  <= on SET's if value changed
	    |	    int		 formatType;    => defined in vartypes.h
	    |	    ValueUnion	 value;
	    |	    char	*stringValueP;  => only if string 
	    |	    char	*formatStrP;    => only if string 
	    |	    int		 maxStringSize; => only if string & GETs
	    |	
	    |	    char	*accessStrP;	=> in case an item has more than one access string 
	    |	    int     	voidDataSize;	=> if void pointer is being passed in ValueUnion 
	    |	} value;
	    |
	    |
	    |	typedef union valueunion    
	    |       {
	    |       unsigned char    uCharFormat;
	    |       char	    *charPFormat;
	    |       unsigned Wordint uWordFormat;
	    |       Wordint	     sWordFormat;
	    |       unsigned Longint uLongFormat;
	    |       Longint	     sLongFormat;
	    |       double	     doubleFormat;
	    |       void	    *voidPFormat;
	    |       } ValueUnion;
	    |
		| Journal the "MYAPPSETTING LINESTYLE" command for BASIC 
		| macro generation instead of allowing the default action 
		| of the macro generator to happen.  The default action 
		| for this event is for the macro generator to add BASIC 
		| code to set the dlogBoxInfo.style variable to the value 
		| chosen on the line style option button.
		| 
		| The default activity is overridden so the generated BASIC 
		| code is more understandable.
		|
	    	|   int 	mdlSystem_journalCommandString
	    	|   (
	    	|   char	*stringP   => String to journal as a command
	    	|   );

* 								    	    *
* @bsimethod myapp_elementStyleHook   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_elementStyleHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
    {
    dimP->msgUnderstood = TRUE;

    switch (dimP->messageType)
	{
	case DITEM_MESSAGE_JOURNALSTATE:
	    {

	    /*------------------------------------------------------- 
	    |  For the DITEM_MESSAGE_JOURNALSTATE event, the "value"
	    |  member of the DialogItemMessage union is used.
	    |
	    |  Value structure from the DialogItemMessage structure.
	    |	(see dlogitem.h):
	    |
	    |	struct		
	    |       {
	    |	    BoolInt	 hookHandled;   <= only for hooks,TRUE if handled 
	    |	    BoolInt	 valueChanged;  <= on SET's if value changed
	    |	    int		 formatType;    => defined in vartypes.h
	    |	    ValueUnion	 value;
	    |	    char	*stringValueP;  => only if string 
	    |	    char	*formatStrP;    => only if string 
	    |	    int		 maxStringSize; => only if string & GETs
	    |	
	    |	    char	*accessStrP;	=> in case an item has more than one access string 
	    |	    int     	voidDataSize;	=> if void pointer is being passed in ValueUnion 
	    |	} value;
	    |
	    |
	    |	typedef union valueunion    
	    |       {
	    |       unsigned char    uCharFormat;
	    |       char	    *charPFormat;
	    |       unsigned Wordint uWordFormat;
	    |       Wordint	     sWordFormat;
	    |       unsigned Longint uLongFormat;
	    |       Longint	     sLongFormat;
	    |       double	     doubleFormat;
	    |       void	    *voidPFormat;
	    |       } ValueUnion;
	    |
	    +------------------------------------------------------*/

	     
	    /* if the type of the chosen value is "long" then */
	    if (FMT_LONG == dimP->u.value.formatType)
		{
		int 	status;
		char	cmdString[80];

		/*------------------------------------------------------- 
		| Journal the "MYAPPSETTING LINESTYLE" command for BASIC 
		| macro generation instead of allowing the default action 
		| of the macro generator to happen.  The default action 
		| for this event is for the macro generator to add BASIC 
		| code to set the dlogBoxInfo.style variable to the value 
		| chosen on the line style option button.
		| 
		| The default activity is overridden so the generated BASIC 
		| code is more understandable.
		|
	    	|   int 	mdlSystem_journalCommandString
	    	|   (
	    	|   char	*stringP   => String to journal as a command
	    	|   );
	    	|
		+------------------------------------------------------*/

		sprintf(cmdString, "MYAPPSETTING LINESTYLE %ld", 
			    	dimP->u.value.value.uLongFormat); 

	    	status = mdlSystem_journalCommandString (cmdString); 

	    	/* if the command was successfully journaled then */
		if (SUCCESS == status)
		    {
		    /*-------------------------------------------------
		    | Tell the macro generator not to add information
		    | to the BASIC macro by setting the hookHandled
		    | flag to TRUE
		    +------------------------------------------------*/
		    dimP->u.value.hookHandled = TRUE;
		    }
		}

	    break;
	    };

	default:
	    dimP->msgUnderstood = FALSE;
	    break;
	}
    }

/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_elementColorHook                                  |
|                                                                       |
| author        BSI                                     10/95           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
* !!!Describe Function Completely!!!								    	    *
* @bsimethod myapp_elementColorHook   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  void myapp_elementColorHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
    {
    dimP->msgUnderstood = TRUE;

    switch (dimP->messageType)
	{
	case DITEM_MESSAGE_JOURNALSTATE:
	    {

	    /* if the value entered in the text item is not NULL */
	    if (NULL != dimP->u.value.stringValueP)
		{
		int status;

		/*------------------------------------------------------- 
		| Journal the "ACTIVE COLOR" command for BASIC macro generation
		| instead of allowing the default action of the macro
		| generator to happen.  The default action for this event
		| is for the macro generator to add BASIC code to set 
		| the tcb->symbology.color variable to the value entered
		| in the color text item.
		| 
		| The default activity is overridden so the generated BASIC 
		| code is more understandable.
		|
	    	|   int 	mdlSystem_journalCommand
	    	|   (
	    	|   char	*taskIdP,   => task id string (may be NULL)
	    	|   long	 cmdnum,    => command number
	    	|   char	*unparsedP  => unparsed part of command (may be NULL)
	    	|   );
	    	|
		+------------------------------------------------------*/
	    	status = mdlSystem_journalCommand (NULL, CMD_ACTIVE_COLOR, 
				    	dimP->u.value.stringValueP);

	    	/* if the command was successfully journaled then */
		if (SUCCESS == status)
		    {
		    /*-------------------------------------------------
		    | Tell the macro generator not to add information
		    | to the BASIC macro by setting the hookHandled
		    | flag to TRUE
		    +------------------------------------------------*/
		    dimP->u.value.hookHandled = TRUE;
		    }
		}

	    break;
	    };

	default:
	    dimP->msgUnderstood = FALSE;
	    break;
	}
    }

/*======================================================================+
|                                                                       |
|   System Event Handler routines                                       |
|                                                                       |
+======================================================================*/
/*----------------------------------------------------------------------+
|                                                                       |
| name          myapp_unloadFunction                                    |
|                                                                       |
| author        BSI                                     11/93           |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
    |	If the value of unloadType is negative, then MicroStation ignores 
    |	the return value of this function.  In this case, we can assume
    |	that MicroStation is shutting down or was terminated so this
    |	function should not attempt to abort the unload.
    | Only want to allow our application to unload when the user clicks
    | the Done button. The Done button queues the unload of our application
    | while the dialog is main MyApp dialog is still open. Open an alert box,
    | Prompting the user if it is OK to unload the application. If the user
    | hits the OK button, continue unloading, otherwise, return a non-zero
    | value to MicroStation to stop the unload.
    | 
    | This has no effect on the double-click on the close icon of the dialog.
    | The dialog is already off the screen by the time this function gets 
    | called, and the call to mdlDialog_find would fail.

* 								    	    *
* @bsimethod myapp_unloadFunction   					*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
Public  int myapp_unloadFunction
(
int             unloadType
)
    {

    /*------------------------------------------------------------------ 
    |	If the value of unloadType is negative, then MicroStation ignores 
    |	the return value of this function.  In this case, we can assume
    |	that MicroStation is shutting down or was terminated so this
    |	function should not attempt to abort the unload.
    +------------------------------------------------------------------*/
    if (unloadType < 0)
    	{
    	return	SUCCESS;
    	}

    /*------------------------------------------------------------------
    | Only want to allow our application to unload when the user clicks
    | the Done button. The Done button queues the unload of our application
    | while the dialog is main MyApp dialog is still open. Open an alert box,
    | Prompting the user if it is OK to unload the application. If the user
    | hits the OK button, continue unloading, otherwise, return a non-zero
    | value to MicroStation to stop the unload.
    | 
    | This has no effect on the double-click on the close icon of the dialog.
    | The dialog is already off the screen by the time this function gets 
    | called, and the call to mdlDialog_find would fail.
    +------------------------------------------------------------------*/
    if (mdlDialog_find (DIALOGID_MyApp, NULL))
	{
	int actionButton;
	char buffer[80];

        mdlResource_loadFromStringList(buffer, NULL, MESSAGELISTID_Msgs, MSGID_SureString);

#if defined (MSVERSION) && (MSVERSION >= 0x550)
    	actionButton = mdlDialog_openMessageBox (
		DIALOGID_MsgBoxYesNo, /* => dialog id of message box */
		buffer,		      /* => message to display */
		MSGBOX_ICON_QUESTION);/* => which icon to show */
	if (ACTIONBUTTON_YES == actionButton)
	    return SUCCESS;
	else
	    return !SUCCESS;
#else
    	actionButton = mdlDialog_openAlert (buffer);
	if (ACTIONBUTTON_OK == actionButton)
	    return SUCCESS;
	else
	    return !SUCCESS;
#endif
	}

    return SUCCESS;
    }





/*----------------------------------------------------------------------+
|                                                                       |
|   Main                                                                |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   The main function is optional. In MDL, it is not mandatory to       |
|   have a main() function. If one is present it is usually just        |
|   initializes the application variables and performs any initial      |
|   setup. Since MDL is a set of functions to handle events, an MDL     |
|   application does most of its work in command functions, state       |
|   functions, and dialog manager hooks.                                |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| In the DialogHookInfo structure we establish a link between       	|
| function pointers for dialog message & item hooks functions		|
| and our hook ids.							|
+----------------------------------------------------------------------*/
Public DialogHookInfo uHooks[]=
    {
    {HOOKDIALOGID_Dlog,         (PFDialogHook) myapp_dlogHook},
    {HOOKITEMID_MyDone,         (PFDialogHook)myapp_doneHook},
    {HOOKITEMID_Palette,        (PFDialogHook)myapp_menuHook},
    {HOOKITEMID_UseFence,       (PFDialogHook)myapp_useFenceHook},
    {HOOKITEMID_MyElementColor, (PFDialogHook)myapp_elementColorHook},
    {HOOKITEMID_MyLineStyle, 	(PFDialogHook)myapp_elementStyleHook},
    {HOOKITEMID_MyDate, 	(PFDialogHook)myapp_dateItemHook},
    };

/*----------------------------------------------------------------------+
|                                                                       |
| name          main                                                    |
|                                                                       |
| author        BSI                                     8/93            |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+*//**
|   The main function is optional. In MDL, it is not mandatory to       |
|   have a main() function. If one is present it is usually just        |
|   initializes the application variables and performs any initial      |
|   setup. Since MDL is a set of functions to handle events, an MDL     |
|   application does most of its work in command functions, state       |
|   functions, and dialog manager hooks. 
    | mdlResource_openFile:
    | 
    | This function opens a resource file, thus making its contents
    | available to the application.  In this case, we need to open
    | MYAPP.MA as a resource file so that we have access to the the
    | string lists for our messages.
    |
    | int mdlResource_openFile  
    |	(
    |	RscFileHandle  *rfHandle,      <= Resource File Handle 
    |	char           *fileName,      => Name of File to open 
    |	ULong           fileAttrMask   => file attributes bit mask 
    |	);                                          
                               |

    | mdlParse_loadCommandTable:
    |
    | This function is going to load the entire command tree into memory.
    | The NULL argument means to use the same resource file that the
    | application was from.
    	|   void mdlOutput_rscPrintf
    	|   	(
    	|   	int            fieldID;		    => MSG_MESSAGE, etc. 
    	|   	RscFileHandle  resourceFileHandle;  => handle or NULL 
    	|   	long           resourceId;          => ID of string list
    	|   	int            stringNumber;        => number of string
    	|   	...
    	|   	);

* 								    	    *
* @bsimethod main   							*					    	*
* @param 	nameOfParam     !!!what this parameter means	    	*
* @return 	!!!what the return value means			    	*
*								        *
* Author:   BSI 				02/01 		*
*								        *
+----------------------------------------------------------------------*/
extern "C" DLLEXPORT  int MdlMain
(
int             argc,
char            *argv[]
)
    {
    RscFileHandle   rscFileH;   /* a resource file handle */
    SymbolSet	    *setP;      /* a ptr to a "C expression symbol set" */

    /*-----------------------------------------------------------------+
    | mdlResource_openFile:
    | 
    | This function opens a resource file, thus making its contents
    | available to the application.  In this case, we need to open
    | MYAPP.MA as a resource file so that we have access to the the
    | string lists for our messages.
    |
    | int mdlResource_openFile  
    |	(
    |	RscFileHandle  *rfHandle,      <= Resource File Handle 
    |	char           *fileName,      => Name of File to open 
    |	ULong           fileAttrMask   => file attributes bit mask 
    |	);                                          
    |
    +-----------------------------------------------------------------*/
    if (mdlResource_openFile (&rscFileH, NULL, RSC_READ) != SUCCESS)
	{

    	/* exit this program and unload it */	
	mdlSystem_exit(	ERROR, /* => status for parent task */
		    	1);    /* => 1 to unload the program */	

	}
Private MdlCommandNumber  commandNumbers [] =

{
{myapp_changeSymbology,     CMD_CHANGE_MYSYMBOLOGY},
{ placeLine_start,          CMD_PLACE_MYLINE},
{ placeDate_start,          CMD_PLACE_MYDATE},
{ myapp_openMyToolFrame,    CMD_OPEN_MYTOOLFRAME},
{ myapp_openMyMainDialog,   CMD_OPEN_MYMAINDIALOG},
{ myapp_setMyLineStyle,     CMD_MYAPPSETTING_LINESTYLE},
{ myapp_setMyDateItem,      CMD_MYAPPSETTING_DATE},
{ myapp_setUseFence,        CMD_MYAPPSETTING_USEFENCE_TOGGLE},
{ myapp_setUseFence,        CMD_MYAPPSETTING_USEFENCE_ON},
{ myapp_setUseFence,        CMD_MYAPPSETTING_USEFENCE_OFF},
0
};

    /* Register commands */
    mdlSystem_registerCommandNumbers (commandNumbers);

    
    /*-----------------------------------------------------------------+
    | mdlParse_loadCommandTable:
    |
    | This function is going to load the entire command tree into memory.
    | The NULL argument means to use the same resource file that the
    | application was from.
    +-----------------------------------------------------------------*/
    if (mdlParse_loadCommandTable (NULL) == NULL)
	{
    	/*-------------------------------------------------------------+
    	|   void mdlOutput_rscPrintf
    	|   	(
    	|   	int            fieldID;		    => MSG_MESSAGE, etc. 
    	|   	RscFileHandle  resourceFileHandle;  => handle or NULL 
    	|   	long           resourceId;          => ID of string list
    	|   	int            stringNumber;        => number of string
    	|   	...
    	|   	);
    	+-------------------------------------------------------------*/
	mdlOutput_rscPrintf (MSG_ERROR, NULL, MESSAGELISTID_Msgs,
			    	 MSGID_LoadCmdTbl);

    	/* exit this program and unload it */	
	mdlSystem_exit(	ERROR, /* => status for parent task */
		    	1);    /* => 1 to unload the program */	
	}


    /*-----------------------------------------------------------------+
    | mdlState_registerStringIds () takes two arguments. The first is the 
    | id of the stringlist holding the Command messages, the second is the id
    | of the stringlist holding the prompt messages.
    +-----------------------------------------------------------------*/
    mdlState_registerStringIds (MESSAGELISTID_Commands, MESSAGELISTID_Prompts);

    /*-----------------------------------------------------------------+
    | mdlDialog_hookPublish:
    |
    | To make the Dialog Box Manager aware of the function pointers
    | that we have equated to our hook ids we need to publish the
    | DialogHookInfo structure, uHooks (declared at the top of this file).
    +-----------------------------------------------------------------*/
    mdlDialog_hookPublish (sizeof (uHooks)/sizeof (DialogHookInfo), uHooks);

    /*-----------------------------------------------------------------+
    | To setup variables evaluated within C expression strings:
    |
    | Since we are using a pointer to a structure we need to allocate
    | memory for it.  Because this symbol will be used in a Dialog Box
    | we specify VISIBILITY_DIALOG_BOX.  Then we publish the pointer to
    | our structure so that the Dialog Box Manager becomes aware of it.
    +-----------------------------------------------------------------*/
    setP = mdlCExpression_initializeSet (VISIBILITY_DIALOG_BOX, 0, TRUE);
    mdlDialog_publishComplexVariable (setP, "dlogboxinfo", "dlogBoxInfo",
				    	 &dlogBoxInfo);

    /*-----------------------------------------------------------------+
    | Setup initial values for dialog box items before opening it.
    | Otherwise values for those items with access strings would be
    | undefined and would most likely display junk.
    +-----------------------------------------------------------------*/

    mdlResource_loadFromStringList(dlogBoxInfo.fmtStr, NULL, MESSAGELISTID_Msgs,
				    	 MSGID_DateString);
    dlogBoxInfo.style = 0;

    /*-----------------------------------------------------------------+
    | We use mdlSystem_setFunction to establish a function pointer for
    | our function to be called when our application unloads.
    +-----------------------------------------------------------------*/
    mdlSystem_setFunction (SYSTEM_UNLOAD_PROGRAM, myapp_unloadFunction);
    mdlDialog_sendMessagesOnUnload (mdlSystem_getCurrMdlDesc(),TRUE);

    /*--------------------------------------------------------------
    | Because we want our setting dialog to come up as soon as the
    | application is loaded we open it now by adding the command
    | CMD_OPEN_MYMAINDIALOG to MicroStation's input queue.
    | Use the extended version of mdlDialog_cmdNumberQueue, which
    | is mdlDialog_cmdNumQueueExt since we don't want to journal
    | the command for BASIC macro generation.  The main dialog box
    | will automatically open when this application is loaded so there
    | is no need to add this command to the BASIC program.
    |
    |	void mdlDialog_cmdNumQueueExt
    |	(
    |	BoolInt      localCmd,      => FALSE means use MicroStations cmdtable
    |	long	     cmdnum,	    => command number to queue 
    |	char	    *unparsed,      => unparsed (string) part of command 
    |	BoolInt      atEndOfQueue,  => FALSE means put at beginning of queue 
    |	BoolInt      journal	    => TRUE means CAD input journal the command 
    |	);
    |
    +---------------------------------------------------------------*/
    mdlDialog_cmdNumQueueExt (TRUE, CMD_OPEN_MYMAINDIALOG, "", TRUE, FALSE);
			       
    return (SUCCESS);
    }

