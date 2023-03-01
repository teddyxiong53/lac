#include "PikaMain.h"
#include "PikaParser.h"
#include "PikaStdLib_MemChecker.h"
#include "PikaVM.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataStrs.h"
#include "pikaScript.h"
#include <time.h>



int main(int argc, char* argv[]) {
    if (1 == argc) {
        pikaScriptShell(pikaScriptInit());
        return 0;
    }
    if (2 == argc) {
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
        __platform_printf("======[pikascript packages installed]======\r\n");
        pks_printVersion();
        __platform_printf("===========================================\r\n");
        pikaVM_runFile(pikaMain, argv[1]);
        obj_deinit(pikaMain);
        return 0;
    }
}