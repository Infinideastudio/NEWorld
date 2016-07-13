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

    content->addChild(view);
    Renderer::init(852, 480);
}

void GameView::doRender()
{
    Renderer::restoreProj();
    Renderer::applyPerspective(60.0f, 852.0f / 480.0f, 0.1f, 100.0f);
    Renderer::restoreScale();
    Renderer::translate(Vec3f(0.0f, 0.0f, -3.0f));
    Renderer::rotate(xrot, Vec3f(1.0f, 0.0f, 0.0f));
    Renderer::rotate(yrot, Vec3f(0.0f, 1.0f, 0.0f));

    glBegin(GL_QUADS);
    // Front
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    // Back
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    // Top
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    // Bottom
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    // Right
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    // Left
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    glFlush();
    glFinish();
}

void GameView::onKeyDown(int scancode)
{
    if (scancode == SDL_SCANCODE_LEFT) xrot -= 0.25f;
    else if (scancode == SDL_SCANCODE_RIGHT) xrot += 0.25f;
    else if (scancode == SDL_SCANCODE_UP) yrot -= 0.25f;
    else if (scancode == SDL_SCANCODE_DOWN) yrot += 0.25f;
}
