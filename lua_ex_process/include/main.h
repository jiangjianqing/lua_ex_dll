#ifndef LUA_EX_PROCESS_MAIN_H
#define LUA_EX_PROCESS_MAIN_H

#include "lua.hpp"

#if defined(__linux__)
// Linux系统
//#define LUA_API extern "C"
#elif defined(_WIN32)

//#define LUA_API extern "C" __declspec(dllexport)
// Windows系统
#endif


#ifdef __cplusplus
extern "C" { //2018.01.18 c++ compiler 必须用extern "C" 指定导出
#endif//-----------------------------

//重要：必须在编译参数中加入LUA_BUILD_AS_DLL，否则 LUALIB_API 定义不正确

//extern "C" int luaopen_mylib(lua_State *L);
LUALIB_API int luaopen_xprocess(lua_State *L);



#ifdef __cplusplus//-------------------------------------
}   //end of extern "C" {
#endif


#endif // LUA_EX_PROCESS_MAIN_H
