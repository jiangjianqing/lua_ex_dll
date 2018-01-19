#include <stdio.h>
#include "main.h"
#include <math.h>
#include "uv.h"

using namespace std;

static int64_t process_exit_status = -1;
void on_exit(uv_process_t *req, int64_t exit_status, int term_signal)
{
    process_exit_status = exit_status;
    fprintf(stderr, "Process exited with status %d, signal %d\n", exit_status, term_signal);
    uv_close((uv_handle_t*) req, NULL);
}

static int kill_by_name(lua_State *L)
{
    uv_loop_t* loop = uv_default_loop();
    uv_process_t child_req;

    //const char* process_file_name = luaL_checkstring(L,1);

    char* cmd_name = (char*)"taskkill";

    char* args[5];
    args[0] = cmd_name;
    args[1] = (char*)"/im";
    args[2] = (char*)"FSMachineVision_64WT.exe";
    args[3] = (char*)"-f";
    args[4] = NULL;


    uv_process_options_t options = {0}; // If change options to a local variable, remember to initialize it to null out all unused fields:
    options.exit_cb = on_exit;
    options.args = args;
    options.file = cmd_name;

    int iret = 0;
    iret = uv_spawn(loop,&child_req, &options);

    if (iret) {
        fprintf(stderr, "%s\n", uv_strerror(iret));
        lua_pushnumber(L, 1);
        return 1;
    } else {
        fprintf(stderr, "Launched process with ID %d\n", child_req.pid);
    }

    iret = uv_run(loop, UV_RUN_DEFAULT);
    if(process_exit_status == 128){
        //没有该名称的进程
        iret = 1;
    }
    lua_pushnumber(L, iret);
    return 1;
}

static int exec(lua_State *L)
{
    return 0;
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
    {"kill_by_name",kill_by_name},
    {"average", averageFunc},
    {"sayHello", sayHelloFunc},
    {"my_cos",   my_math_cos},
    {"my_sin",   my_math_sin},
    {NULL, NULL}       //数组中最后一对必须是{NULL, NULL}，用来表示结束
};

/*
** Open my_math library
*/
int luaopen_xprocess(lua_State *L)   {
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
