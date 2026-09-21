#pragma once
#include "Application/AppBase.h"
#include "Common/App_State.h"

class AppFactory {
public :
    AppFactory() = delete;
    static AppBase* create(Mode mode);
};