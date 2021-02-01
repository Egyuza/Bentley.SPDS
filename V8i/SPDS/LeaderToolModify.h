#pragma once

#include    <WString.h>
#include    <mdl.h>
#include    <stdio.h>
#include    <string.h>
#include    <malloc.h>
#include    <mselemen.fdf>
#include    <mselmdsc.fdf>
#include    <mslinkge.fdf>
#include    <msscancrit.fdf>
#include    <mstagdat.fdf>
#include    <mselems.h>
#include    <mscell.fdf>
#include    <leveltable.fdf>
#include    <mslstyle.fdf>
#include    <msstrlst.h>
#include    <mscnv.fdf>
#include    <msdgnobj.fdf>
#include    <msmodel.fdf>
#include    <msview.fdf>
#include    <msviewinfo.fdf>
#include    <msvar.fdf>
#include    <dlmsys.fdf>
#include    <msdialog.fdf>

#include    <msrmgr.h>
#include    <mssystem.fdf>
#include    <msparse.fdf>
#include    <RefCounted.h>
#include    <toolsubs.h>

#include <elementref.h>
#include <msdependency.fdf>
#include <msassoc.fdf>
#include <msmisc.fdf>
#include <mslocate.fdf>
#include <msstate.fdf>
#include <msoutput.fdf>
#include <mstypes.h>
#include <mstmatrx.fdf>
#include <MicroStationAPI.h>

#include "spdscmd.h"

USING_NAMESPACE_BENTLEY;
USING_NAMESPACE_BENTLEY_USTN;
USING_NAMESPACE_BENTLEY_USTN_ELEMENT;

enum ModifyAction {
    MOD_ADD,
    MOD_DELETE,
    MOD_REPLACE
};

class LeaderToolModify : public MstnElementSetTool
{
public:
    StatusInt InstallTool(ModifyAction modifyType);

private:
    ElementID       elemIdToDelete;
    ModifyAction    modAction;

    bool        WantAccuSnap () {return false;}
    bool        NeedPointForDynamics () {return SOURCE_Pick == GetElemSource ();} // Can check GetHitSource to detect EditAction
    bool        OnPostLocate (HitPathCP path, char *cantAcceptReason) override;

    bool        NeedAcceptPoint();

    //bool        OnDataButton(MstnButtonEventCP ev) override;
    bool        OnModelNoMotion (MstnButtonEventCP) override;
    StatusInt   OnElementModify (EditElemHandleR elHandle) override;
    void        OnRestartCommand () override;
    void        OnPostInstall () override;
    EditElemHandleP BuildLocateAgenda (HitPathCP path, MstnButtonEventCP ev) override;
    bool        OnInstall () override;

};