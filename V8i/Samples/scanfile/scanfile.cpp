/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/scanfile/scanfile.cpp,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   scanfile.cpp  $
|   $Revision: 1.2 $
|   	$Date: 2013/06/28 20:41:09 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   scanfile.mc                                                         |
|                                                                       |
|   This program is an example program to show the use of linkages, and |
|   and example section to do an extended scan for the linkage attached |
|   to an element.						        |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include    <mdl.h>
#include    <mselems.h>
#include    <scanner.h>
#include    <userfnc.h>
#include    <cmdlist.h>
#include    <string.h>
#include    <toolsubs.h>

#include    <dlogman.fdf>
#include    <mssystem.fdf>
#include    <mslinkge.fdf>
#include    <msscan.fdf>
#include    <msoutput.fdf>
#include    <msparse.fdf>
#include    <mselemen.fdf>
#include    <msrsrc.fdf>
#include    <mslocate.fdf>
#include    <msstate.fdf>
#include    <msscancrit.fdf>

#include    "scanfile.h"   /* Local includes */
#include    "scanfcmd.h"    /* Local includes */

#define DEBUG_PRINT 1;
/*----------------------------------------------------------------------+
|									|
|   Global Variables   							|
|									|
+----------------------------------------------------------------------*/
int	    foundLinkage = NOTFOUND;
LinkData    linkData;

/*----------------------------------------------------------------------+
|									|
| name		scanfile_scanAttributes					|
|									|
| author	BSI     				6/95		|
|									|
+----------------------------------------------------------------------*/
Public void  scanfile_scanAttributes 
(
char 	*unparsedP
)
//cmdNumber CMD_SCAN_LINKAGE
    {
    UInt32	    elemAddr[50], filePos;
    int		    scanWords, status, i, numAddr;
    ExtendedAttrBuf extAttrBuf;
    UShort   	    word2, word3, word4;
    ScanCriteria    *scP;

    word2 = 0x000D;   /* Search in word 2 for ID = 13 */
    word3 = 0x5342;   /* Search in word 3 for "BS" */
    word4 = 0x0049;   /* Search in word 4 for "I" */

/*    mdlScan_initScanlist (&scanList);
    mdlScan_noRangeCheck (&scanList);*/

    scP = mdlScanCriteria_create ();
    /*-----------------------------------------------------------------+
    | scan for extended attributes and return filepos (not block/byte) |
    +-----------------------------------------------------------------*/
//    scanList.extendedType   = FILEPOS | EXTATTR;
     mdlScanCriteria_setReturnType (scP,MSSCANCRIT_RETURN_FILEPOS,FALSE,TRUE); 
    /*---------------------------------------------------+
    |  set extended attribute buffer and clear it        |
    +---------------------------------------------------*/
//    scanList.extAttrBuf	    = &extAttrBuf;
    memset (&extAttrBuf, 0, sizeof (ExtendedAttrBuf));

    /*------------------------------------------------------------------+
    | numWords tells the scanner to look at the first numWords of data. |
    |                                                                   |
    |   We will look at the first 4.                                    |
    +------------------------------------------------------------------*/
    extAttrBuf.numWords     = 4;

    /*-----------------------------------------------------------------------+
    |   Scanning for the linkage that was attached with the lINKAGE ATTACH   |
    |   command.  The search will include USER DATA LINKAGE, by setting      |
    |   word 1 to be 0x1000.  Setting word 2 to be 0x000D, looking for the   |
    |   Linkage ID of 13.  Setting word3 to be 0x5342, for the 'BS', and word|
    |   4, 0x0049, for 'I'.                                                  |
    |                                                                        |
    |   This is example is written to run on the PC (a little endian machine)|
    |   The scanner DOES NOT account for BYTE ORDERING.  The extAttrBuf      |
    |   data must be set up to account for byte ordering issues.  For        |
    |   examaple, if this example was to be run on a big endian machine then |
    |   we would scan for 0x0001 and 0x4900 rather than 0x1000 and 0x0049.   |
    +-----------------------------------------------------------------------*/
    
    extAttrBuf.extAttData[0] = 0x1000; /* verify user bit is set */
    extAttrBuf.extAttData[4] = 0x1000;
    
    extAttrBuf.extAttData[1] = 0xffff; /* 2nd word should be '13'*/
    extAttrBuf.extAttData[5] = word2;
    
    extAttrBuf.extAttData[2] = 0xffff; /* 3rd word should be 'BS'*/
    extAttrBuf.extAttData[6] = word3;                              
    
    extAttrBuf.extAttData[3] = 0x00ff; /* 4th word should be 'I' */
    extAttrBuf.extAttData[7] = word4;                                          

#if defined (BIG_ENDIAN)
    /* Swap the words to look for */
    mdlCnv_swapWord(&word2);
    mdlCnv_swapWord(&word3);
    mdlCnv_swapWord(&word4);

    extAttrBuf.extAttData[0] = 0x0010; /* verify user bit is set */
    extAttrBuf.extAttData[4] = 0x0010;
    
    extAttrBuf.extAttData[1] = 0xffff; /* 2nd word should be '13'*/
    extAttrBuf.extAttData[5] = word2;

    
    extAttrBuf.extAttData[2] = 0xffff; /* 3rd word should be 'BS'*/
    extAttrBuf.extAttData[6] = word3;                              
    
    extAttrBuf.extAttData[3] = 0xff00; /* 4th word should be 'I' */
    extAttrBuf.extAttData[7] = word4;  
#endif
      
    mdlScanCriteria_setAttributeTest (scP, 0, 0, &extAttrBuf);
    mdlScanCriteria_setModel (scP,MASTERFILE);
    //mdlScan_initialize (0, &scanList);

    /*----------------------------------------------------------------------+
    | loop through all elements in file - print the file position of the    |
    | elements which match the scan.  mdlScan_file can be called repeatedly |
    | as in the following do-while loop to handle the case where more       |
    | information is found than can fit in the output buffer.               |
    +----------------------------------------------------------------------*/
    do
	{
	scanWords = sizeof(elemAddr)/sizeof(short);
	status    = mdlScanCriteria_scan (scP,elemAddr,&scanWords,&filePos);
	//status	  = mdlScan_file (elemAddr, &scanWords, sizeof(elemAddr),&filePos);
	numAddr   = scanWords / sizeof(short);

//#   	if defined (DEBUG_PRINT)
	for (i=0; i<numAddr; i++)
	    printf ("element with attr at: %ld\n", elemAddr[i]);	    
//#   	endif /* if defined (DEBUG_PRINT) */

	} while (status == BUFF_FULL);
    mdlScanCriteria_free (scP);

    }

/*----------------------------------------------------------------------+
|                                    					|
| name          scanfile_checkLink				     	|
|                                                                       |
| author	BSI     				6/95		|
|							                |
+----------------------------------------------------------------------*/
Private int  scanfile_checkLink
(
LinkData  *linkageP
)
    {
    /*---------------------------------------------------------------+
    |  Set up the structure to be returned from the call             |
    |  mdlLinkage_extractFromElement.  The first two words of the    |
    |  linkage, the linkage header, are automatically converted to   |
    |  internal format.  The remaing structure is converted using the|
    |  ddbID or if convRulesPP is pointing to previously generated   |
    |  set of conversion rules                                       |
    +---------------------------------------------------------------*/
    if  (strcmp(linkageP->linkageInfo.string, "BSI")==0)
        {
        foundLinkage = FOUND;
        memcpy(&linkData, linkageP, sizeof(LinkData));
        return FOUND;
        }
    else
        {
        return NOTFOUND;
        }
    }

/*----------------------------------------------------------------------+
|                                    					|
| name          scanfile_acceptExtract                                  |
|                                                                       |
| author	BSI     				6/95		|
|							                |
+----------------------------------------------------------------------*/
Private void     scanfile_acceptExtract
(
Dpoint3d    *ptP,		/* => data point entered by user */
int         view		/* => view point was in */
)
    {
    ULong             filePos;
    MSElementUnion    el;
    DgnModelRefP      currFileP;

    filePos = mdlElement_getFilePos( FILEPOS_CURRENT ,&currFileP );
    mdlElement_read( &el, 0, filePos );

    foundLinkage = NOTFOUND;

    mdlLinkage_extractFromElement(NULL, &el, 13, 
                         RSCID_DataDef_Link, NULL, scanfile_checkLink, NULL);

    if (foundLinkage == FOUND)
       	{
	mdlOutput_rscPrintf (MSG_STATUS, NULL, MESSAGELISTID_Messages,
			     MSGID_LinkageFound);
       	}
    else
       	{
	mdlOutput_rscPrintf (MSG_STATUS, NULL, MESSAGELISTID_Messages,
			     MSGID_LinkageNotFound);
    	}
    }

/*-----------------------------------------------------------------------+
|       								 |
| name      	scanfile_setElmSearchType                                |
|                                                                        |
| author	BSI     				6/95		 |
|                                                                        |
------------------------------------------------------------------------*/
Private void scanfile_setElmSearchType 
(
void
)
    {
    /*-----------------------------------------------------
    |   Setting my search to include everything            |
    |                                                      |
     -----------------------------------------------------*/
    static int  searchType[] = {CELL_HEADER_ELM, LINE_ELM, LINE_STRING_ELM,
                                SHAPE_ELM, TEXT_NODE_ELM, CURVE_ELM, 
                                CMPLX_STRING_ELM,
                                CONIC_ELM, CMPLX_SHAPE_ELM, ELLIPSE_ELM,
                                ARC_ELM, TEXT_ELM, SURFACE_ELM, SOLID_ELM,
                                BSPLINE_POLE_ELM, POINT_STRING_ELM, CONE_ELM,
                                BSPLINE_SURFACE_ELM, BSURF_BOUNDARY_ELM,
                                BSPLINE_KNOT_ELM, BSPLINE_CURVE_ELM, 
                                BSPLINE_WEIGHT_ELM, DIMENSION_ELM,  
                                MULTILINE_ELM,
                               };

    /*-----------------------------------------------------
    |    Clear search criteria				   |
    |							   |
     -----------------------------------------------------*/
    mdlLocate_noElemNoLocked ();

    /*-----------------------------------------------------
    |   Set search mask to look for text and line elements |
    |                                                      |
     -----------------------------------------------------*/
    mdlLocate_setElemSearchMask (sizeof(searchType)/sizeof(int), searchType);
    }

/*------------------------------------------------------------------------+
|       								  |
| name    scanfile_extractCommand                                         |
|                                                                         |
| author	BSI     				6/95		  |
|       								  |
-------------------------------------------------------------------------*/
Public void scanfile_extractCommand
(
char 	*unparsedP
)
//cmdNumber CMD_LINKAGE_EXTRACT
    {
    /*-----------------------------------------------------
    |    Setup search mask with the type of elements we    |
    |                   are looking for			   |
     -----------------------------------------------------*/
    scanfile_setElmSearchType();

    mdlState_startModifyCommand (scanfile_extractCommand, /* reset func */
				 scanfile_acceptExtract, /* datapoint func */
				 NULL,	       /* dynamics func */
				 NULL,	       /* show func */
                                 NULL,	       /* clean func */
				 NULL,	       /* command field message */
				 NULL,	       /* prompt message */
				 TRUE,	       /* use selection sets */
				 0);	       /* points required for accept */
    mdlAutoLocate_enable (TRUE,TRUE,TRUE);
    /*-----------------------------------------------------
    |	    Start search at beginning of file		   |
    |							   |
     -----------------------------------------------------*/
    mdlLocate_init ();
}

/*----------------------------------------------------------------------+
|                                    					|
| name          scanfile_acceptAttach                                   |
|                                                                       |
| author	BSI     				6/95		|
|							                |
+----------------------------------------------------------------------*/
Private void     scanfile_acceptAttach
(
Dpoint3d    *ptP,		/* => data point entered by user */
int         view		/* => view point was in */
)
    {
    ULong             filePos;
    MSElementUnion    el;
    int               status;
    LinkInfo          linkageInfo;
    LinkageHeader     linkHdr;
    DgnModelRefP      currFile;

    memset (&linkHdr, '\0', sizeof(LinkageHeader));
    linkHdr.primaryID = 13;
    linkHdr.user = 1;
    /*------------------------------------------------------------
    | This is where I fill in my data structure which is only a  |
    |   string                                                   |
     -----------------------------------------------------------*/
    strcpy(linkageInfo.string, "BSI");

    filePos = mdlElement_getFilePos( FILEPOS_CURRENT ,&currFile );
    mdlElement_read( &el, 0, filePos );
    status = mdlLinkage_appendToElement( &el, &linkHdr, &linkageInfo, 
                              RSCID_DataDef_Link, NULL );
    if (status == SUCCESS )
        {
        mdlElement_rewrite( &el, NULL, filePos );

	mdlOutput_rscPrintf (MSG_STATUS, NULL, MESSAGELISTID_Messages,
			     MSGID_LinkageAppended);
        }
    else
        {
	mdlOutput_rscPrintf (MSG_ERROR, NULL, MESSAGELISTID_Messages,
			     MSGID_ErrorAppendingLinkage);
        }
    }

/*------------------------------------------------------------------------+
|       								  |
| name    scanfile_attachCommand                                          |
|                                                                         |
| author	BSI     				6/95		  |
|       								  |
-------------------------------------------------------------------------*/
Public void scanfile_attachCommand
(
char 	*unparsedP
)
//cmdNumber CMD_LINKAGE_ATTACH
    {

    /*-----------------------------------------------------
    |    Setup search mask with the type of elements we    |
    |                   are looking for			   |
     -----------------------------------------------------*/
    scanfile_setElmSearchType();

    mdlState_startModifyCommand (scanfile_attachCommand, /* reset func */
				 scanfile_acceptAttach,    /* datapoint func */
				 NULL,	       /* dynamics func */
				 NULL,	       /* show func */
                                 NULL,	       /* clean func */
				 NULL,	       /* command field message */
				 NULL,	       /* prompt message */
				 TRUE,	       /* use selection sets */
				 0);	       /* points required for accept */
    mdlAutoLocate_enable (TRUE,FALSE,TRUE);
    /*-----------------------------------------------------
    |	    Start search at beginning of file		   |
    |							   |
     -----------------------------------------------------*/
    mdlLocate_init ();
    }

/*----------------------------------------------------------------------+
|									|
| name		main							|
|									|
| author	BSI     				6/95		|
|									|
+----------------------------------------------------------------------*/
extern "C" DLLEXPORT  int MdlMain
(
int             argc,
char            *argv[]
)
    {
    RscFileHandle    rscFileH;

    if (SUCCESS != mdlResource_openFile (&rscFileH, NULL, TRUE))
    	{
	/* unload this application */
	mdlDialog_cmdNumberQueue (FALSE, CMD_MDL_UNLOAD,
				      mdlSystem_getCurrTaskID(), TRUE);
    	return ERROR;
    	}

    Private MdlCommandNumber  commandNumbers [] =
    {
    {scanfile_scanAttributes,CMD_SCAN_LINKAGE},
    {scanfile_extractCommand ,CMD_LINKAGE_EXTRACT},
    {scanfile_attachCommand,CMD_LINKAGE_ATTACH},
    0,
    };
    
    /* Register commands */
    mdlSystem_registerCommandNumbers (commandNumbers);

    /* Load additional command to command table */
    if (mdlParse_loadCommandTable (NULL) == NULL)
    	{
	mdlOutput_rscPrintf (MSG_ERROR, NULL, MESSAGELISTID_Messages,
			     MSGID_ErrorLoadingCmdTable);
	/* unload this application */
	mdlDialog_cmdNumberQueue (FALSE, CMD_MDL_UNLOAD,
				      mdlSystem_getCurrTaskID(), TRUE);

    	}

    return SUCCESS;
    }           
