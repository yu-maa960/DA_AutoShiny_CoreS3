#include "HAL/MyUI.h"
#include "OS/CoreOS.h"

void setup()
{
    if (!CoreOS::getInstance().boot()) {
        MyUI::showErrorMes("FATAL: Failed to create OS Queue.");
        CoreOS::getInstance().reboot();
    }
}

void loop()
{
    CoreOS::getInstance().run();
}