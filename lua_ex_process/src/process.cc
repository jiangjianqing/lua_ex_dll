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

//命令行最大参数个数
#define MAX_ARG_COUNT 100

static int64_t process_exit_status = -1;
void on_exit(uv_process_t *req, int64_t exit_status, int term_signal)
{
    process_exit_status = exit_status;
    fprintf(stderr, "Process exited with status %d, signal %d\n", exit_status, term_signal);
    uv_close((uv_handle_t*) req, NULL);
}

int exec_shell(const char** args,uv_exit_cb exit_cb,bool is_sync_exec)
{
    uv_loop_t* loop = uv_default_loop();
    uv_process_t child_req;

    uv_process_options_t options = {0}; // If change options to a local variable, remember to initialize it to null out all unused fields:
    options.exit_cb = exit_cb;
    options.args = (char**)args;
    options.file = args[0]; //特别注意：file和args的第一个参数相同

    path myfile(options.file);  //vc2015 std::tr2::sys::path
    if (exists(myfile)) {
        //printf(myfile.path.c_str());

        string tmp = myfile.parent_path().string();
        options.cwd = strdup(tmp.c_str());
        printf(tmp.c_str());
    }
    if(!is_sync_exec){//注：异步执行时让child_process和parent detach
        options.flags = UV_PROCESS_DETACHED;
    }

    int iret = 0;
    iret = uv_spawn(loop,&child_req, &options);

    if (iret) {
        fprintf(stderr, "%s\n", uv_strerror(iret));
        //lua_pushnumber(L, 1);
        return 1;
    } else {
        fprintf(stderr, "Launched process with ID %d\n", child_req.pid);
    }

    if(!is_sync_exec){
        uv_run(loop, UV_RUN_NOWAIT);
        //thread t{[&loop](){//obsoloted : 用thread异步不好，改用 UV_RUN_NOWAIT + options.flags = UV_PROCESS_DETACHED

        //}};

    }else{//sync exec
        iret = uv_run(loop, UV_RUN_DEFAULT);
        if(process_exit_status == 128){
            //没有该名称的进程
            iret = -1;
        }
    }


    return iret;
}

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

static int kill_by_name(lua_State *L)
{
    const char* process_file_name = luaL_checkstring(L,1);

    char* cmd_name = (char*)"taskkill";

    char* args[MAX_ARG_COUNT] = {0};
    /*
    args[0] = cmd_name;
    args[1] = (char*)"/im";
    args[2] = (char*)process_file_name;
    args[3] = (char*)"-f";
    args[4] = NULL;
    */
    char cmd_buf[1024]={0};
#if defined(__linux__)
    //linux下是否由能够按名称杀进程的命令还需要查阅资料
#elif defined(_WIN32)
// Windows 使用 tasklist \ taskkill完成杀进程操作
    sprintf(cmd_buf,"taskkill /im %s -f",process_file_name);

    //强制杀死PID为processid的进程，PID可通过tasklist查看
    //taskkill /pid processid -f
#endif
    get_args(cmd_buf,args);

    int iret = exec_shell((const char**)args,on_exit,true);

    lua_pushnumber(L , iret);
    return 1;
}

static int exec(lua_State *L)
{
    const char* cmd = luaL_checkstring(L,1);
    char* args[MAX_ARG_COUNT] = {0};
    get_args(cmd , args);

    int iret = exec_shell((const char**)args,on_exit,true);
    lua_pushnumber(L , iret);
    return 1;
}

static int async_exec(lua_State *L)
{
    const char* cmd = luaL_checkstring(L,1);
    char* args[MAX_ARG_COUNT] = {0};
    get_args(cmd , args);

    int iret = exec_shell((const char**)args,on_exit,false);
    lua_pushnumber(L , iret);
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
    {"kill_by_name",kill_by_name},
    {"exec",exec},
    {"async_exec",async_exec},
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
