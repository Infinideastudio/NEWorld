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

#include "window.h"

MainMenu::MainMenu() : UI::Core::Page()
{
    using namespace UI::Core;
    using namespace UI::Controls;
    using namespace UI::Globalization;
    content = std::make_shared<Grid>();
    content->addChild(std::make_shared<Label>("caption", Margin::StretchLeft(0, 0, 0, 20, 300, 40, 0), Str("Input Something:")));
    UI::Globalization::Service::getInstance().setLang("chinese");
    std::shared_ptr<Button> exitbutton = std::make_shared<Button>(
            "testbutton2", Margin::BottomCenter(0.1, 75, 75, 0, 80), Str("Bye", false), []()
    {
        application->terminate();
    });
    std::shared_ptr<TextBox> testbox = std::make_shared<TextBox>(
                                           "testbox", UI::Core::Margin::CenterStretch(0, 1, 10, 10, 20, 20), L"", UI::Globalization::Str("", false), []()
    {
    });
    content->addChild(testbox);
    content->addChild(exitbutton);
}
