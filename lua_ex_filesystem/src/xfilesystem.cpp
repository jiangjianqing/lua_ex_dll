#include <stdio.h>
#include "main.h"
#include <math.h>
#include "uv.h"
#include <string>
#include <thread>

#if defined(__linux__)
// Linux系统
#include "boost/filesystem.hpp"
using namespace boost::filesystem;
#elif defined(_WIN32)

// Windows系统
#include <filesystem>
using namespace std::tr2::sys;
#endif

using namespace std;
namespace fs = std::experimental::filesystem;

//命令行最大参数个数
#define MAX_ARG_COUNT 100


/**
 * @brief get_args
 * @param cmd 指令字符串 注：目前不支持 管道 操作
 * @param args 生成分割好的字符串列表，由外部分配好内存
 * @return
 */
int get_args(const char* cmd , char** args)
{

    char *token = NULL;

    char delim[] = " ,!";
    int count = 0;

#if defined(__linux__)
// Linux系统 使用效率更高的strsep
    char* buf = strdup(cmd);//复制一份cmd作为buf，后面用strsep会修改该值
    for(token = strsep(&buf, delim); token != NULL; token = strsep(&buf, delim)) {
        //One difference between strsep and strtok_r is that if the input string contains more
        //than one character from delimiter in a row strsep returns an empty string for each
        //pair of characters from delimiter. This means that a program normally should test
        //for strsep returning an empty string before processing it.
        if(token != NULL && strlen(token) == 0){
            continue;
        }
        args[count] = token;//strdup(token);
        count++;
        printf(token);
        printf("+");
    }

#elif defined(_WIN32)
// Windows vc 没有strsep，所以任然使用strtok代替
//#define LUA_API extern "C" __declspec(dllexport)
    char* buf = (char*)cmd;
    for(token = strtok(buf, delim); token != NULL; token = strtok(NULL, delim)) {
        args[count] = token;//strdup(token);
        printf(token);
        printf("+");
        count++;
    }
#endif

    args[count] = NULL;

    return count;
}

static int rm_rf(lua_State *L)
{
    const char* dirname = luaL_checkstring(L,1);
    uintmax_t iret = 0;
    try{
        iret = remove_all(dirname);
    }catch(const exception& e){
        fprintf(stderr,"rm_rf occur error : %s\r\n",e.what());
        iret = -1;
    }
    lua_pushnumber(L,iret);
    return 1;
}

void CopyFiles(const path &src, const path &dst)
{
    if (! exists(dst))
    {
        create_directories(dst);
    }
    for(directory_iterator it(src); it != directory_iterator(); ++it){

        const path newSrc = src / it->path().filename();
        const path newDst = dst / it->path().filename();
        if (is_directory(newSrc))
        {
            CopyFiles(newSrc, newDst);
        }
        else if (is_regular_file(newSrc))
        {
            copy_file(newSrc, newDst, copy_options::overwrite_existing);
        }
        else
        {
            fprintf(stderr, "Error: unrecognized file - %s", newSrc.string().c_str());
        }
    }
}

static int cp_rf(lua_State *L)
{
    const char* source = luaL_checkstring(L,1);
    const char* dest = luaL_checkstring(L,2);
    fprintf(stdout,"source = %s , dest = %s \r\n",source,dest);
    int iret = 0;
    if(exists(source)){
        try{
            CopyFiles(source,dest);
        }catch(const filesystem_error& e){
            fprintf(stderr,"cp_rf occur error : %s\r\n",e.what());
            iret = -1;
        }
    }
    lua_pushinteger(L,iret);
    return 1;
}


static int my_math_sin (lua_State *L) {
    lua_pushnumber(L, sin(luaL_checknumber(L, 1)));
    return 1;
}

static int my_math_cos (lua_State *L) {
    lua_pushnumber(L, cos(luaL_checknumber(L, 1)));
    return 1;
}

static int averageFunc(lua_State *L)
{
    int n = lua_gettop(L);
    double sum = 0;
    int i;

    /* 循环求参数之和 */
    for (i = 1; i <= n; i++)
        sum += lua_tonumber(L, i);

    lua_pushnumber(L, sum / n);     //压入平均值
    lua_pushnumber(L, sum);         //压入和

    return 2;                       //返回两个结果
}

static int sayHelloFunc(lua_State* L)
{
    printf("hello world!");
    return 0;
}

static const struct luaL_Reg myLib[] =
{
    {"rm_rf", rm_rf},
    {"cp_rf", cp_rf},
    {"average", averageFunc},
    {"sayHello", sayHelloFunc},
    {"my_cos",   my_math_cos},
    {"my_sin",   my_math_sin},
    {NULL, NULL}       //数组中最后一对必须是{NULL, NULL}，用来表示结束
};

/*
** Open my_math library
*/
int luaopen_xfilesystem(lua_State *L)   {
//#ifdef lua5.1
//5.1 下直接使用luaL_register 就好
//    luaL_register(L, "libname", funcs);
//#else //lua5.2
    lua_newtable(L);
    //先把一个table压入VS，然后在调用luaL_setfuncs就会把所以的func存到table中
    //注意不像luaL_register这个table是个无名table，可以在的使用只用一个变量来存入这个table。
    //e.g local clib = require "libname". 这样就不会污染全局环境。比luaL_register更好。
    luaL_setfuncs(L, myLib, 0);
//#endif
  return 1;
}
