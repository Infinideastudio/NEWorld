// 
// GUI: widget.h
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

#include <functional>
#include "nuklear_helper.h"
#include <string>

// widget base class
class Widget {
public:
    Widget(std::string name, struct nk_rect size, int flags)
        : mName(std::move(name)), mSize(size), mFlags(flags) {}

    virtual ~Widget() {}

    void _render(nk_context* ctx) {
        if (nk_begin(ctx, mName.c_str(), mSize, mFlags))
            render(ctx);
    }

    virtual void update() = 0;

    void setOpen(bool open) { mOpen = open; }
    std::string getName() const { return mName; }
protected:
    virtual void render(nk_context* ctx) = 0;

private:
    std::string mName;
    bool mOpen = true;
    struct nk_rect mSize;
    int mFlags;
};

// callback style widget
class WidgetCallback : public Widget {
public:
    using RenderCallback = std::function<void(nk_context*)>;
    using UpdateCallback = std::function<void()>;

    WidgetCallback(std::string name, struct nk_rect size, int flags,
                   RenderCallback renderFunc, UpdateCallback updateFunc = nullptr) :
        Widget(name, size, flags), mRenderFunc(renderFunc), mUpdateFunc(updateFunc) {}

    void render(nk_context* ctx) override { mRenderFunc(ctx); }
    void update() override { if (mUpdateFunc) mUpdateFunc(); }

private:
    RenderCallback mRenderFunc;
    UpdateCallback mUpdateFunc;
};
