/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gameview.h"
#include "renderer.h"
#include <logger.h>

auto StretchStretch(double LeftPct, double RightPct, double TopPct, double BottomPct, double LeftDis, double RightDis, double TopDis, double BottomDis)
{
    return UI::Core::Margin(UI::Base::Rect(LeftPct, RightPct, TopPct, BottomPct), UI::Base::Rect(LeftDis, RightDis, TopDis, BottomDis), UI::Core::HorizontalAlignment::Stretch, UI::Core::VerticalAlignment::Stretch);
}

GameView::GameView() : UI::Core::Page()
{
    content = std::make_shared<UI::Core::Grid>();
    auto view = std::make_shared<UI::Controls::GLContext>("",
                StretchStretch(0.0, 1.0, 0.0, 1.0, 0, 0, 0, 0));
    view->onRenderF = [this]()
    {
        doRender();
    };
    view->onKeyDownF = [this](int scancode)
    {
        onKeyDown(scancode);
    };
    view->onViewResizeF = [this](int w, int h)
    {
        onResize(w, h);
    };
    content->addChild(view);

    Renderer::init();
    Renderer::setViewport(0, 0, windowWidth, windowHeight);

    VertexArray cubeArray(256, VertexFormat(0, 3, 0, 3));
    // Front
    cubeArray.setColor({0.0f, 0.0f, 1.0f});
    cubeArray.addVertex({-0.5f, 0.5f, 0.5f});
    cubeArray.addVertex({-0.5f,-0.5f, 0.5f});
    cubeArray.addVertex({ 0.5f,-0.5f, 0.5f});
    cubeArray.addVertex({ 0.5f, 0.5f, 0.5f});
    // Back
    cubeArray.setColor({0.0f, 1.0f, 0.0f});
    cubeArray.addVertex({ 0.5f, 0.5f,-0.5f});
    cubeArray.addVertex({ 0.5f,-0.5f,-0.5f});
    cubeArray.addVertex({-0.5f,-0.5f,-0.5f});
    cubeArray.addVertex({-0.5f, 0.5f,-0.5f});
    // Top
    cubeArray.setColor({1.0f, 1.0f, 0.0f});
    cubeArray.addVertex({-0.5f, 0.5f,-0.5f});
    cubeArray.addVertex({-0.5f, 0.5f, 0.5f});
    cubeArray.addVertex({ 0.5f, 0.5f, 0.5f});
    cubeArray.addVertex({ 0.5f, 0.5f,-0.5f});
    // Bottom
    cubeArray.setColor({1.0f, 1.0f, 1.0f});
    cubeArray.addVertex({-0.5f,-0.5f, 0.5f});
    cubeArray.addVertex({-0.5f,-0.5f,-0.5f});
    cubeArray.addVertex({ 0.5f,-0.5f,-0.5f});
    cubeArray.addVertex({ 0.5f,-0.5f, 0.5f});
    // Right
    cubeArray.setColor({1.0f, 0.0f, 0.0f});
    cubeArray.addVertex({ 0.5f,-0.5f, 0.5f});
    cubeArray.addVertex({ 0.5f,-0.5f,-0.5f});
    cubeArray.addVertex({ 0.5f, 0.5f,-0.5f});
    cubeArray.addVertex({ 0.5f, 0.5f, 0.5f});
    // Left
    cubeArray.setColor({1.0f, 0.0f, 1.0f});
    cubeArray.addVertex({-0.5f,-0.5f,-0.5f});
    cubeArray.addVertex({-0.5f,-0.5f, 0.5f});
    cubeArray.addVertex({-0.5f, 0.5f, 0.5f});
    cubeArray.addVertex({-0.5f, 0.5f,-0.5f});
    cube = VertexBuffer(cubeArray);
}

void GameView::doRender()
{
    Renderer::clear();
    Renderer::restoreProj();
    Renderer::applyPerspective(60.0f, (float)windowWidth / windowHeight, 0.1f, 100.0f);
    Renderer::restoreScale();
    Renderer::translate(Vec3f(0.0f, 0.0f, -3.0f));
    Renderer::rotate(xrot, Vec3f(1.0f, 0.0f, 0.0f));
    Renderer::rotate(yrot, Vec3f(0.0f, 1.0f, 0.0f));

    cube.render();

    glFlush();
    glFinish();
}

void GameView::onKeyDown(int scancode)
{
    if (scancode == SDL_SCANCODE_LEFT) yrot -= 0.25f;
    else if (scancode == SDL_SCANCODE_RIGHT) yrot += 0.25f;
    else if (scancode == SDL_SCANCODE_UP) xrot -= 0.25f;
    else if (scancode == SDL_SCANCODE_DOWN) xrot += 0.25f;
}

void GameView::onResize(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    Renderer::setViewport(0, 0, windowWidth, windowHeight);
}
