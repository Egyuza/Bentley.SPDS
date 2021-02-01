#include "LeaderToolModify.h"
#include "LeaderSetTool.h"

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
bool            LeaderToolModify::OnPostLocate (HitPathCP path, char *cantAcceptReason)
{
    if (!__super::OnPostLocate (path, cantAcceptReason))
        return false;

    mdlDisplayPath_setCursorElemIndex((DisplayPathP)path, 1);

    // Only allow elements of our type to be located...
    EditElemHandle eh (mdlDisplayPath_getElem((DisplayPathP)path, 0), 
        mdlDisplayPath_getPathRoot((DisplayPathP)path));

    if (eh.GetElementType() == CELL_HEADER_ELM) {

        if (!isLeaderToolCell(eh.GetElementCP())) {
            return false;
        }
                
        { // если осталась одна линия указателя, то её не следует удалять
            int strLineNum = 0;
            for (ChildEditElemIter child(eh, EXPOSECHILDREN_Count);
                child.IsValid(); child = child.ToNext()) 
            {
                if (child.GetElementType() == LINE_STRING_ELM) {
                    ++strLineNum;
                }
            }
            if (strLineNum <= 1) {
                return false;
            }
        }

        ElemHandle lineEh(mdlDisplayPath_getElem((DisplayPathP)path, 1),
            mdlDisplayPath_getPathRoot((DisplayPathP)path));

        if (lineEh.GetElementType() == LINE_STRING_ELM)
        {
            elemIdToDelete = lineEh.GetElementCP()->ehdr.uniqueId;
            return true;
        }
    }    
    return false;
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
StatusInt LeaderToolModify::OnElementModify (EditElemHandleR eeh)
{

    int type = eeh.GetElementType();

    if (type == LINE_STRING_ELM) {
        eeh.DeleteFromModel();
    }
    else if (type == CELL_HEADER_ELM) {
                
        DPoint3d pointToDel;

        for (ChildEditElemIter child(eeh, EXPOSECHILDREN_Count);
            child.IsValid(); child = child.ToNext()) 
        {
            if (child.GetElementP()->ehdr.uniqueId == elemIdToDelete) {
                Line_String_3d line = child.GetElementP()->line_string_3d;
                pointToDel = line.vertice[0];
                mdlElmdscr_removeElement(child.GetElemDescrP());
                break;
            }
        }
        // удаление точки указателя
        for (ChildEditElemIter child(eeh, EXPOSECHILDREN_Count);
            child.IsValid(); child = child.ToNext()) 
        {
            if (child.GetElementType() == ELLIPSE_ELM) 
            {
                Ellipse_3d circle = child.GetElementP()->ellipse_3d;
                
                DPoint3d center;
                mdlArc_extract(NULL, NULL, NULL, NULL, NULL, NULL, &center, 
                    child.GetElementP());

                if (center.x == pointToDel.x && center.y == pointToDel.y)
                {
                    mdlElmdscr_removeElement(child.GetElemDescrP());
                    break;
                }
            }
        }
    }

    // TODO редактировать через собственную функцию, собирать cell заново!

    return SUCCESS;
}
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
bool            LeaderToolModify::OnModelNoMotion (MstnButtonEvent const *ev) 
  {
  //  DisplayInfoBalloon (ev);
    return true;
  }
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
void            LeaderToolModify::OnRestartCommand ()
{
    LeaderToolModify* newTool = new LeaderToolModify ();

    newTool->InstallTool(modAction);
}

bool    LeaderToolModify::NeedAcceptPoint()
{
    return false;
}


/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
EditElemHandleP  LeaderToolModify::BuildLocateAgenda (HitPathCP path, MstnButtonEventCP ev)
{

    // чтобы в OnElementModify передать cell-элемент:
    mdlDisplayPath_setCursorElemIndex((DisplayPathP)path, 0);

    // Here we have both the new agenda entry and the current hit path if needed...
    EditElemHandleP elHandle = __super::BuildLocateAgenda (path, ev);

    return  elHandle;
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
void            LeaderToolModify::OnPostInstall ()
{
    __super::OnPostInstall ();
    mdlAccuDraw_setEnabledState (false); // Don't enable AccuDraw w/Dynamics...

    //mdlLocate_allowLocked();
    //mdlLocate_allowSelfRefs();
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
bool            LeaderToolModify::OnInstall ()
    {
    if (!__super::OnInstall ())
        return false;

    SetCmdNumber (0);      // For toolsettings/undo string...
    SetCmdName (0, 0);  // For command prompt...

    //mdlState_startModifyCommand()

    return true;
}

StatusInt LeaderToolModify::InstallTool(ModifyAction modifyType)
{ 
    modAction = modifyType;
    return __super::InstallTool();
}