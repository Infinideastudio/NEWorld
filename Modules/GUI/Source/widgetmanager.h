// 
// GUI: widgetmanager.h
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

#ifndef WIDGET_MANAGER_H
#define WIDGET_MANAGER_H
#include <unordered_map>
#include "widget.h"
#include <memory>

class WidgetManager {
public:
    WidgetManager(nk_context* nkctx): mNkContext(nkctx) {}
    using WidgetType = std::unordered_map<std::string, std::shared_ptr<Widget>>;

    void render() {
        for (auto& widget : mWidgets)
            widget.second->_render(mNkContext);

        nk_end(mNkContext);
        // TODO: add an option to adjust the arguments
        nk_sdl_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
    }

    void update() {
        for (auto& widget : mWidgets)
            widget.second->update();
    }

    void addWidget(std::shared_ptr<Widget> widget) { mWidgets[widget->getName()] = widget; }

    std::shared_ptr<Widget> getWidget(std::string name) {
        auto iter = mWidgets.find(name);
        if (iter == mWidgets.end()) return nullptr;
        return iter->second;
    }

    std::shared_ptr<const Widget> getWidget(std::string name) const {
        auto iter = mWidgets.find(name);
        if (iter == mWidgets.end()) return nullptr;
        return iter->second;
    }

    size_t getSize() const { return mWidgets.size(); }
    WidgetType::iterator begin() { return mWidgets.begin(); }
    WidgetType::iterator end() { return mWidgets.end(); }
    WidgetType::const_iterator begin() const { return mWidgets.begin(); }
    WidgetType::const_iterator end() const { return mWidgets.end(); }

private:
    WidgetType mWidgets;
    nk_context* mNkContext;
};

#endif
