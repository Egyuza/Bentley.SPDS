/*----------------------------------------------------------------------+
|									
|   Include Files   							
|									
+----------------------------------------------------------------------*/
#include <rscdefs.h>
#include <cmdclass.h>

/*----------------------------------------------------------------------+
|									
|   Local Defines							
|									
+----------------------------------------------------------------------*/
#define	    CT_NONE	        0
#define	    CT_MAIN	        1
#define     CT_TOOL         2
#define     CT_ACTION       3
#define     CT_SUB_ACTION   4
#define     CT_DIALOG       5

/*----------------------------------------------------------------------+
|                                                                       
|   Application command syntax   					
|                                                                       
+----------------------------------------------------------------------*/
Table CT_MAIN =
{ 
    { 1, CT_TOOL,       PLACEMENT,  REQ,    "SPDS" }, 
}

Table CT_TOOL =
{ 
    { 1, CT_ACTION,     INHERIT,	NONE,   "LEADER" },
}

Table CT_ACTION =
{ 
    { 1, CT_NONE,       INHERIT,	NONE,   "PLACE" },
    { 2, CT_SUB_ACTION, INHERIT,	NONE,   "MODIFY" },
}

Table CT_SUB_ACTION =
{ 
    { 1, CT_NONE,       INHERIT,	NONE,   "ADD" },
    { 2, CT_NONE,       INHERIT,	NONE,   "DELETE" },
    { 3, CT_NONE,       INHERIT,	NONE,   "REPLACE" },
}
