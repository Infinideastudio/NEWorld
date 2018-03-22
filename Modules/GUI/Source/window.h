// 
// GUI: window.h
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

#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "nuklear_helper.h"

struct MouseState {
    int x, y;
    bool left, mid, right, relative = true;
};

class Window {
public:
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void makeCurrentDraw() const { SDL_GL_MakeCurrent(mWindow, mContext); }

    void swapBuffers() const { SDL_GL_SwapWindow(mWindow); }

    static const Uint8* getKeyBoardState() { return SDL_GetKeyboardState(nullptr); }

    int getWidth() const noexcept { return mWidth; }

    int getHeight() const noexcept { return mHeight; }

    void pollEvents();

    static Window& getInstance(const std::string& title = "", int width = 0, int height = 0) {
        static Window win(title, width, height);
        return win;
    }

    bool shouldQuit() const noexcept { return mShouldQuit; }

    nk_context* getNkContext() const noexcept { return mNuklearContext; }

    /**
     * \brief Get the relative motion of mouse
     * \return The relative motion of mouse
     */
    MouseState getMouseMotion() const noexcept {
        MouseState res = mMouse;
        res.x -= mPrevMouse.x;
        res.y -= mPrevMouse.y;
        return res;
    }

    static void lockCursor() { SDL_SetRelativeMouseMode(SDL_TRUE); }
    static void unlockCursor() { SDL_SetRelativeMouseMode(SDL_FALSE); }

private:
    SDL_Window* mWindow = nullptr;
    std::string mTitle;
    int mWidth, mHeight;
    MouseState mMouse, mPrevMouse;
    bool mShouldQuit = false;

    Window(const std::string& title, int width, int height);
    ~Window();

    SDL_GLContext mContext;
    nk_context* mNuklearContext;
};
