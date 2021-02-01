/*------------------------------------------------------------------------------
�����: LeaderSetTool
���������� ����-�������� �������

����������:
- ���� ��������� ������� ����, �� ������� ������ (Origin) ����� ���� �������� 
  ����� ����� ���������, 
  ���� ���������� ��������� - ������� ����� ����� ��� ����.

Arrow - �����-��������� �� ������
Terminator - "����������" ���������, ��������, �����
----------------------------------------------------------------------------- */
#pragma once

#include <vector>
#include <string>

#include "spds.h"
#include "spdscmd.h"
#include "ElementHelper.h"
#include "TextHelper.h"
#include "TextGrabHelper.h"
#include "CfgVariables.h"

USING_NAMESPACE_BENTLEY_USTN;
USING_NAMESPACE_BENTLEY_USTN_ELEMENT;

#define TOOLNAME ("SpdsLeader")
#define TOOLNAME_W (L"SpdsLeader")

// TODO �������, ���� �� �����:
/* SDK:
Accept functions for element modification commands generally use the 
Element_Modification functions to change the selected element(s). 
When the accept function is called, the point that the user gave to identify 
the element is in statedata.pointstack[0], and is passed as the first argument 
to the data point function.
*/
#define     ptStack statedata.dPointStack /* for readability */

enum STEP {
    STEP_START,
    STEP_FIRST_ARROW_POINT1,
    STEP_FIRST_ARROW_POINT2,
    STEP_TEXT,
    STEP_NEXT_ARROW
};

STEP operator ++(STEP& step);
STEP operator --(STEP& step);

class LeaderSetTool : public MstnElementSetTool {
public:
    LeaderSetTool();
    // LeaderSetTool(int cmdNumber, int cmdName);

    void setInputParams(const TextGrabParam& params);

protected:
    virtual bool OnInstall() override;
    virtual void OnPostInstall() override;
    virtual void OnCleanup() override;
    virtual void OnRestartCommand() override;

    virtual bool WantAccuSnap() { return true; }

    virtual bool NeedPointForDynamics() override;
    virtual void OnComplexDynamics(MstnButtonEventCP ev) override;
    virtual bool OnResetButton(MstnButtonEventCP ev) override;
    virtual bool OnDataButton(MstnButtonEventCP ev) override;

    virtual StatusInt OnElementModify(EditElemHandleR elHandle) override;
    virtual bool OnModifierKeyTransition(bool wentDown, int key) override;

private:
    DPoint3d origin_;
    DPoint3d textOrigin_; // ������� ����� ������
    DPoint3d textLineOrigin_; // ������� ����� ����� ������

    //// ����� ��������� ������������� ��� ����������
    //std::vector<DPoint3d> points_;
    
    DPoint3d curArrowPoints_[3]; // ����� ��� ���������� ������� �����-���������
    DPoint3d firstArrowPoints_[3]; // ����� ������ �����-���������
    
    ElementList elements_; // ������ ���� ��������� ��� ����������
    EditElemHandle textEeh_; // ������ �� ��������� �������

    // ��� �������������� ������� � ����������:
    double arrowRadius_; // ������ ����� - ����������� �����-���������

    // TODO ������������ ������
    double textOffset_; // ����� �� ������

    UInt32 textStyleId_; // ����� ������
    double textStyleHeight_; // ������ ���������� �����
    double textStyleLineSpacing_; // �������� ������ ����� ��������
    
    bool isNextArrowFreeAngle_; // ���� ������������� ���� ������� �����-���������
    bool isReadyToPublish_; // ���� ���������� � ���������� � ������
    bool shouldLeadersFromOnePoint_; // ���� ��������� ����� ��������� �� ����� �����
    
    // TODO ����� �� ������� �����-����������? 
    int nArrows_;
    STEP step_; // ������� ��� ���������� ����-��������

    TextGrabParam grabParams_;
    MSWideChar textWide_[MAX_STR_LEN];

    void CreateAcceptedTransient();
   /* EditElemHandle GetElemHandle(MSElementR element);*/

    // �������� ��������� ��������� �� ������� ���� ����������
    bool createElementsOnStep(ElementList& elems, MstnButtonEventCP ev);
    // �������� ��������� ��������� ������ - ������ � ����� ��� ����
    bool createTextGroupElems(ElementList& elems, DPoint3dCR basePoint, 
        const DPoint3d* orientPointP = NULL, DPoint3dR textOriginOut = DPoint3d());

    // �������������� �������� � ���������� Cell-�������� Leader
    StatusInt CreateCellAndPublish();
};

// ���������� ����������� ����� - ������� ����� (�����)
StatusInt Leader_TextDependencyCallback
(                                  /* <= SUCCESS if callback succeeds; ERROR to undo all changes */
    ElementID          dependentID,    /* => element ID of dependent element */
    DgnModelRefP       modelRef,       /* => modelRef of dependent element */
    DependencyLinkage *pLinkage,       /* => copy of dependent element's linkage */
    UChar             *pRootStatus,    /* => change status of each root in linkage: DEPENDENCY_STATUS_XXX  */
    UChar              selfStatus      /* => change status of dependent itself    : DEPENDENCY_STATUS_XXX  */
);


int modFunc_SetDependencyOnText
(
    MSElement   *elP,   /* element to add ink onto*/
    void		*dataP  /* link data to add */
);

/* !
������ ��������� ������� ������� ��������������:
    int modFunc_
    (
        MSElementUnion      *element,      // <=> element to be modified
        void                *params,       // => user parameter
        DgnModelRefP        modelRef,      // => model to hold current elem
        MSElementDescr      *elmDscrP,     // => element descr for elem
        MSElementDescr      **newDscrPP,   // <= if replacing entire descr
        ModifyElementSource elemSource    // => The source for the element
    );
*/

int modFunc_ResizeBaseLine
(
    MSElementUnion      *element,      // <=> element to be modified
    void                *params,       // => user parameter
    DgnModelRefP        modelRef,      // => model to hold current elem
    MSElementDescr      *elmDscrP,     // => element descr for elem
    MSElementDescr      **newDscrPP,   // <= if replacing entire descr
    ModifyElementSource elemSource    // => The source for the element
);

bool isLeaderToolCell(MSElementCP elCP);