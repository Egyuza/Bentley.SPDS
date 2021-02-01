/*-------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/imodelvisitor/ExternApp/externApp.cpp,v $
|    $RCSfile: externApp.cpp,v $
|   $Revision: 1.3 $
|       $Date: 2012/02/14 19:21:00 $
|     $Author: Mark.Anderson $
|
|  $Copyright: (c) 2012 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "ConnectionManager.h"
#include <conio.h>
#include <iostream>
#include <process.h>
#include <shlwapi.h>

USING_NAMESPACE_BENTLEY_IPC
using namespace std;
/*---------------------------------------------------------------------------------**//**
* @description  How to compile and run the program?
* Set the following path in Project's Property Page:
* [C/C++]->Additional Include Directories: D:\Program Files (x86)\Bentley 08.11.09.263\MicroStation\mdl\MicroStationAPI\IModel
* [Linker]->Additional Dependencies:       D:\Program Files (x86)\Bentley 08.11.09.263\MicroStation\mdl\library\bentley.imodelconnection.lib
* [Debugging]->Working Directories:        D:\Program Files (x86)\Bentley 08.11.09.263\MicroStation\mdlsys\asneeded\
*
+---------------+---------------+---------------+---------------+---------------+------*/

/*---------------------------------------------------------------------------------**//**
* @description  callback method from the mdl application running in Bentley Host.
* @bsimethod                                                          09/11
+---------------+---------------+---------------+---------------+---------------+------*/
void call1 ( wchar_t* a)
    {
    wcout << "\nCall 1: "<< a << "  ";
    }

/*---------------------------------------------------------------------------------**//**
* @description  main entry point for this example application.
* @bsimethod                                                          09/11
+---------------+---------------+---------------+---------------+---------------+------*/
int _tmain(int argc, _TCHAR* argv[])
 {

    ConnectionManager *cm = ConnectionManager::GetManagerObj();

    int count = cm->DiscoverEngines();
    cout << "\n number of engine  " << count;

    cm->GetEngine(1)->SetCallback( call1 );

    IModelEngineP engine = NULL;

      // Loop through the list of engine and print the engine info
    for ( int i = 0; i < count; i++ )
        {
        engine = cm->GetEngine(i);
        std::wstring name;
        engine->GetName (name);
        wprintf ( L"\nIndex %d", i );
        wprintf ( L"\nEngine name %s", name.c_str() );
        engine->GetProgramPath(name);
        wprintf ( L"\nEngine full path %s\n", name.c_str() );
        }


    engine = cm->GetEngine(1);
    std::wstring name;
    engine->GetName(name);
    wprintf ( L" %s", name.c_str() );

    //this needs to be the path to a seed file to open in Bentley app.
    std::wstring seedPath = L"G:\\ddrive\\Data\\imodels\\Building\\structural\\TC_STR_002.i.dgn";
    //an alternate is to have a blank seed file path.
	//std::wstring seedPath = L"";
	ConnectionStatus cs = cm->Connect(engine, seedPath,false );
    cout << "\n Server started" << endl;
	//loop while you get a server not ready.
    
    // return if error occurred
    if ( 0 != cs.status )
        return cs.status;

    //At this point engine is connected and the msg server is up and listening
	//this is the name of the application that is running in Bentley Host.  This app needs to have the command(s)
	//that you will communicate to from this external program.
    std::wstring appPath (L"C:\\Program Files\\Bentley 08.11.09.259\\MicroStation\\mdlapps\\imodelvisitor.ma");
    engine->LoadApp(appPath);

     if ( 0 != status )
        {
        // error occurred. Check the path of the app and the file exist in the path.
        // Or the engine is not responding. In that case wait for sometime and try again
        }

    //this is the keyin that is in the mdl application.
	//std::wstring settingString = L"imodelvisitor set procall ALL";
	//printf ("\n send keyin for process all %d ",engine->StartApp (settingString));

    std::wstring keyinString = L"Imodelstart cmd imodelvisitor action process *";
    engine->StartApp(keyinString);
    printf ( "\n Start App  " );
    
    if ( 0 != status )
        {
        // error occurred. The engine is not responding. In that case wait for sometime and try again
        }

    //pause the program, to keep the server alive.
   // char ch = _getch();

	cm->Disconnect(engine);
    cm->DeleteManagerObj();
    cout << "\n Server stoped" << endl;

    return 0;
}

 