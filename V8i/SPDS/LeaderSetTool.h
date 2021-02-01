/*------------------------------------------------------------------------------
Класс: LeaderSetTool
Построение спдс-элемента ВЫНОСКА

Примечания:
- если указатель выноски один, то базовой точкой (Origin) всего спдс элемента 
  будет точка указателя, 
  если указателей несколько - базовая точка полки под тект.

Arrow - линия-указатель на объект
Terminator - "наконечник" указателя, например, точка
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

// TODO удалить, если не нужно:
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
    DPoint3d textOrigin_; // базовая точка текста
    DPoint3d textLineOrigin_; // базовая точка полки текста

    //// точки указанные пользователем при построении
    //std::vector<DPoint3d> points_;
    
    DPoint3d curArrowPoints_[3]; // точки для построения текущей линии-указателя
    DPoint3d firstArrowPoints_[3]; // точки первой линии-указателя
    
    ElementList elements_; // список всех элементов для построения
    EditElemHandle textEeh_; // ссылка на текстовый элемент

    // все геометрические размеры в милиметрах:
    double arrowRadius_; // радиус точки - наконечника линии-указателя

    // TODO использовать отступ
    double textOffset_; // отсуп до текста

    UInt32 textStyleId_; // стиль текста
    double textStyleHeight_; // высота текстового стиля
    double textStyleLineSpacing_; // величина зазора между строками
    
    bool isNextArrowFreeAngle_; // флаг произвольного угла очедной линии-указателя
    bool isReadyToPublish_; // флаг готовности к публикации в модель
    bool shouldLeadersFromOnePoint_; // флаг рисования линий указателя из одной точки
    
    // TODO нужен ли подсчёт линий-указателей? 
    int nArrows_;
    STEP step_; // текущий шаг построения спдс-элемента

    TextGrabParam grabParams_;
    MSWideChar textWide_[MAX_STR_LEN];

    void CreateAcceptedTransient();
   /* EditElemHandle GetElemHandle(MSElementR element);*/

    // создание очередных элементов на текущем шаге построения
    bool createElementsOnStep(ElementList& elems, MstnButtonEventCP ev);
    // создание элементов текстовой группы - текста и полки под него
    bool createTextGroupElems(ElementList& elems, DPoint3dCR basePoint, 
        const DPoint3d* orientPointP = NULL, DPoint3dR textOriginOut = DPoint3d());

    // Окончантельное создание и публикация Cell-элемента Leader
    StatusInt CreateCellAndPublish();
};

// Обработчик зависимости текст - базовая линия (полка)
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
другой подробный вариант функции редактирования:
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