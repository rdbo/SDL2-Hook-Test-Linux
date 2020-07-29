#include <iostream>
#include <pthread.h>
#include <SDL2/SDL.h>
#include "mem/mem.hpp"
#define LIBSDL_MODULE_NAME MEM_STR("/libSDL2")
#define SDL_GL_SWAPWINDOW_STR "SDL_GL_SwapWindow"
#define HOOK_SIZE   9
#define HOOK_METHOD mem::detour_int::method0

mem::bytearray_t o_SDL_GL_SwapWindow;
mem::voidptr_t p_SDL_GL_SwapWindow;

bool check_module(mem::module_t mod);

void hk_SDL_GL_SwapWindow(SDL_Window* window)
{
    system("zenity --info --title=\"sdl2-hook\" --text=\"injected\"");
    mem::in::detour_restore(p_SDL_GL_SwapWindow, o_SDL_GL_SwapWindow);
    return reinterpret_cast<void(*)(SDL_Window*)>(p_SDL_GL_SwapWindow)(window);
}

void* main_thread(void* args)
{
    pthread_join(pthread_self(), NULL);
    void* ret = (void*)-1;
    mem::module_t libSDL2 = mem::in::get_module(LIBSDL_MODULE_NAME);
    if(!check_module(libSDL2)) return ret;

    mem::lib_t lib;
    lib.mode = RTLD_LAZY;
    lib.path = libSDL2.path;
    if(mem::in::load_library(lib, &libSDL2) == (mem::int_t)MEM_BAD_RETURN) return ret;

    p_SDL_GL_SwapWindow = (mem::voidptr_t)(dlsym(libSDL2.handle, SDL_GL_SWAPWINDOW_STR));

    mem::in::detour(p_SDL_GL_SwapWindow, (mem::voidptr_t)&hk_SDL_GL_SwapWindow, HOOK_SIZE, HOOK_METHOD, &o_SDL_GL_SwapWindow);

    ret = 0;
    return (void*)NULL;
}

__attribute__((constructor))
void dl_main()
{
    pthread_t thread;
    pthread_create(&thread, NULL, (void*(*)(void*))main_thread, (void*)NULL);
}

bool check_module(mem::module_t mod)
{
    return
        strcmp(mod.name.c_str(), "") &&
        strcmp(mod.path.c_str(), "") &&
        mod.base != (mem::voidptr_t)MEM_BAD_RETURN &&
        mod.end  != (mem::voidptr_t)MEM_BAD_RETURN;
}