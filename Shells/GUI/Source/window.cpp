// 
// GUI: window.cpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#include "window.h"
#include "Common/Debug.h"
#include "renderer/renderer.h"

void Window::pollEvents() {
    if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
        Uint32 buttons = SDL_GetRelativeMouseState(&mMouse.x, &mMouse.y);
        mMouse.left = buttons & SDL_BUTTON_LEFT;
        mMouse.right = buttons & SDL_BUTTON_RIGHT;
        mMouse.mid = buttons & SDL_BUTTON_MIDDLE;
        mMouse.relative = true;
    }
    else {
        mPrevMouse = mMouse;
        Uint32 buttons = SDL_GetMouseState(&mMouse.x, &mMouse.y);
        mMouse.left = buttons & SDL_BUTTON_LEFT;
        mMouse.right = buttons & SDL_BUTTON_RIGHT;
        mMouse.mid = buttons & SDL_BUTTON_MIDDLE;
        if (mMouse.relative) mPrevMouse = mMouse;
        mMouse.relative = false;
    }

    nk_input_begin(mNuklearContext);
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        nk_sdl_handle_event(&e);
        switch (e.type) {
        case SDL_QUIT:
            mShouldQuit = true;
            break;
        case SDL_WINDOWEVENT:
            switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                mWidth = e.window.data1;
                mHeight = e.window.data2;
                break;
            }
            break;
        }
    }
    nk_input_end(mNuklearContext);
}

Window::Window(const std::string& title, int width, int height)
    : mTitle(title), mWidth(width), mHeight(height) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    mWindow = SDL_CreateWindow(mTitle.c_str(), 100, 100, mWidth, mHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (mWindow == nullptr)
        fatalstream << "Failed to create SDL window!" << SDL_GetError();
    Assert(mWindow != nullptr);
    glewExperimental = 1;
    glewInit();

    mContext = SDL_GL_CreateContext(mWindow);
    if (mContext == nullptr)
        fatalstream << "Failed to create GL context! " << SDL_GetError();
    Assert(mContext != nullptr);
    SDL_GL_SetSwapInterval(0); // VSync
    makeCurrentDraw();
    Renderer::init();
    mNuklearContext = nk_sdl_init(mWindow);
    {
        struct nk_font_atlas* atlas;
        nk_sdl_font_stash_begin(&atlas);
        //struct nk_font *sourceHanSans = nk_font_atlas_add_from_file(atlas, "res/Fonts/SourceHanSansCN-Normal.otf", 12, 0);
        nk_sdl_font_stash_end();
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        //nk_style_set_font(mNuklearContext, &sourceHanSans->handle);
    }
}

Window::~Window() {
    nk_sdl_shutdown();
    SDL_DestroyWindow(mWindow);
    SDL_GL_DeleteContext(mContext);
    SDL_Quit();
}
