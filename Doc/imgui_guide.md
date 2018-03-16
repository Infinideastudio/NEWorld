// dear imgui, v1.50 WIP
// (main code and documentation)

// See ImGui::ShowTestWindow() in imgui_demo.cpp for demo code.
// Newcomers, read 'Programmer guide' below for notes on how to setup ImGui in your codebase.
// Get latest version at https://github.com/ocornut/imgui
// Releases change-log at https://github.com/ocornut/imgui/releases
// Developed by Omar Cornut and every direct or indirect contributors to the GitHub.
// This library is free but I need your support to sustain development and maintenance.
// If you work for a company, please consider financial support, e.g: https://www.patreon.com/imgui

/*

 Index
 - MISSION STATEMENT
 - END-USER GUIDE
 - PROGRAMMER GUIDE (read me!)
 - API BREAKING CHANGES (read me when you update!)
 - FREQUENTLY ASKED QUESTIONS (FAQ), TIPS
   - How can I help?
   - How do I update to a newer version of ImGui?
   - What is ImTextureID and how do I display an image?
   - I integrated ImGui in my engine and the text or lines are blurry..
   - I integrated ImGui in my engine and some elements are clipping or disappearing when I move windows around..
   - How can I have multiple widgets with the same label? Can I have widget without a label? (Yes). A primer on the purpose of labels/IDs.
   - How can I tell when ImGui wants my mouse/keyboard inputs and when I can pass them to my application?
   - How can I load a different font than the default?
   - How can I load multiple fonts?
   - How can I display and input non-latin characters such as Chinese, Japanese, Korean, Cyrillic?
   - How can I use the drawing facilities without an ImGui window? (using ImDrawList API)
 - ISSUES & TODO-LIST
 - CODE


 MISSION STATEMENT
 =================

 - easy to use to create code-driven and data-driven tools
 - easy to use to create ad hoc short-lived tools and long-lived, more elaborate tools
 - easy to hack and improve
 - minimize screen real-estate usage
 - minimize setup and maintenance
 - minimize state storage on user side
 - portable, minimize dependencies, run on target (consoles, phones, etc.)
 - efficient runtime (NB- we do allocate when "growing" content - creating a window / opening a tree node for the first time, etc. - but a typical frame won't allocate anything)
 - read about immediate-mode gui principles @ http://mollyrocket.com/861, http://mollyrocket.com/forums/index.html

 Designed for developers and content-creators, not the typical end-user! Some of the weaknesses includes:
 - doesn't look fancy, doesn't animate
 - limited layout features, intricate layouts are typically crafted in code
 - occasionally uses statically sized buffers for string manipulations - won't crash, but some very long pieces of text may be clipped. functions like ImGui::TextUnformatted() don't have such restriction.


 END-USER GUIDE
 ==============

 - double-click title bar to collapse window
 - click upper right corner to close a window, available when 'bool* p_open' is passed to ImGui::Begin()
 - click and drag on lower right corner to resize window
 - click and drag on any empty space to move window
 - double-click/double-tap on lower right corner grip to auto-fit to content
 - TAB/SHIFT+TAB to cycle through keyboard editable fields
 - use mouse wheel to scroll
 - use CTRL+mouse wheel to zoom window contents (if IO.FontAllowScaling is true)
 - CTRL+Click on a slider or drag box to input value as text
 - text editor:
   - Hold SHIFT or use mouse to select text.
   - CTRL+Left/Right to word jump
   - CTRL+Shift+Left/Right to select words
   - CTRL+A our Double-Click to select all
   - CTRL+X,CTRL+C,CTRL+V to use OS clipboard
   - CTRL+Z,CTRL+Y to undo/redo
   - ESCAPE to revert text to its original value
   - You can apply arithmetic operators +,*,/ on numerical values. Use +- to subtract (because - would set a negative value!)


 PROGRAMMER GUIDE
 ================

 - read the FAQ below this section!
 - your code creates the UI, if your code doesn't run the UI is gone! == very dynamic UI, no construction/destructions steps, less data retention on your side, no state duplication, less sync, less bugs.
 - call and read ImGui::ShowTestWindow() for demo code demonstrating most features.
 - see examples/ folder for standalone sample applications. Prefer reading examples/opengl2_example/ first as it is the simplest.
   you may be able to grab and copy a ready made imgui_impl_*** file from the examples/.
 - customization: PushStyleColor()/PushStyleVar() or the style editor to tweak the look of the interface (e.g. if you want a more compact UI or a different color scheme).

 - getting started:
   - init: call ImGui::GetIO() to retrieve the ImGuiIO structure and fill the fields marked 'Settings'.
   - init: call io.Fonts->GetTexDataAsRGBA32(...) and load the font texture pixels into graphics memory.
   - every frame:
      1/ in your mainloop or right after you got your keyboard/mouse info, call ImGui::GetIO() and fill the fields marked 'Input'
      2/ call ImGui::NewFrame() as early as you can!
      3/ use any ImGui function you want between NewFrame() and Render()
      4/ call ImGui::Render() as late as you can to end the frame and finalize render data. it will call your RenderDrawListFn handler that you set in the IO structure.
         (if you don't need to render, you still need to call Render() and ignore the callback, or call EndFrame() instead. if you call neither some aspects of windows focusing/moving will appear broken.)
   - all rendering information are stored into command-lists until ImGui::Render() is called.
   - ImGui never touches or know about your GPU state. the only function that knows about GPU is the RenderDrawListFn handler that you provide.
   - effectively it means you can create widgets at any time in your code, regardless of considerations of being in "update" vs "render" phases of your own application.
   - refer to the examples applications in the examples/ folder for instruction on how to setup your code.
   - a typical application skeleton may be:

        // Application init
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = 1920.0f;
        io.DisplaySize.y = 1280.0f;
        io.IniFilename = "imgui.ini";
        io.RenderDrawListsFn = my_render_function;  // Setup a render function, or set to NULL and call GetDrawData() after Render() to access the render data.
        // TODO: Fill others settings of the io structure

        // Load texture atlas
        // There is a default font so you don't need to care about choosing a font yet
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(pixels, &width, &height);
        // TODO: At this points you've got a texture pointed to by 'pixels' and you need to upload that your your graphic system
        // TODO: Store your texture pointer/identifier (whatever your engine uses) in 'io.Fonts->TexID'

        // Application main loop
        while (true)
        {
            // 1) get low-level inputs (e.g. on Win32, GetKeyboardState(), or poll your events, etc.)
            // TODO: fill all fields of IO structure and call NewFrame
            ImGuiIO& io = ImGui::GetIO();
            io.DeltaTime = 1.0f/60.0f;
            io.MousePos = mouse_pos;
            io.MouseDown[0] = mouse_button_0;
            io.MouseDown[1] = mouse_button_1;
            io.KeysDown[i] = ...

            // 2) call NewFrame(), after this point you can use ImGui::* functions anytime
            ImGui::NewFrame();

            // 3) most of your application code here
            MyGameUpdate(); // may use any ImGui functions, e.g. ImGui::Begin("My window"); ImGui::Text("Hello, world!"); ImGui::End();
            MyGameRender(); // may use any ImGui functions

            // 4) render & swap video buffers
            ImGui::Render();
            SwapBuffers();
        }

   - You can read back 'io.WantCaptureMouse', 'io.WantCaptureKeybord' etc. flags from the IO structure to tell how ImGui intends to use your
     inputs and to know if you should share them or hide them from the rest of your application. Read the FAQ below for more information.


 API BREAKING CHANGES
 ====================

 Occasionally introducing changes that are breaking the API. The breakage are generally minor and easy to fix.
 Here is a change-log of API breaking changes, if you are using one of the functions listed, expect to have to fix some code.
 Also read releases logs https://github.com/ocornut/imgui/releases for more details.

 - 2016/10/15 (1.50) - avoid 'void* user_data' parameter to io.SetClipboardTextFn/io.GetClipboardTextFn pointers. We pass io.ClipboardUserData to it.
 - 2016/09/25 (1.50) - style.WindowTitleAlign is now a ImVec2 (ImGuiAlign enum was removed). set to (0.5f,0.5f) for horizontal+vertical centering, (0.0f,0.0f) for upper-left, etc.
 - 2016/07/30 (1.50) - SameLine(x) with x>0.0f is now relative to left of column/group if any, and not always to left of window. This was sort of always the intent and hopefully breakage should be minimal.
 - 2016/05/12 (1.49) - title bar (using ImGuiCol_TitleBg/ImGuiCol_TitleBgActive colors) isn't rendered over a window background (ImGuiCol_WindowBg color) anymore. 
                       If your TitleBg/TitleBgActive alpha was 1.0f or you are using the default theme it will not affect you. 
                       However if your TitleBg/TitleBgActive alpha was <1.0f you need to tweak your custom theme to readjust for the fact that we don't draw a WindowBg background behind the title bar.
                       This helper function will convert an old TitleBg/TitleBgActive color into a new one with the same visual output, given the OLD color and the OLD WindowBg color.
                           ImVec4 ConvertTitleBgCol(const ImVec4& win_bg_col, const ImVec4& title_bg_col)
                           {
                               float new_a = 1.0f - ((1.0f - win_bg_col.w) * (1.0f - title_bg_col.w)), k = title_bg_col.w / new_a;
                               return ImVec4((win_bg_col.x * win_bg_col.w + title_bg_col.x) * k, (win_bg_col.y * win_bg_col.w + title_bg_col.y) * k, (win_bg_col.z * win_bg_col.w + title_bg_col.z) * k, new_a);
                           }
                       If this is confusing, pick the RGB value from title bar from an old screenshot and apply this as TitleBg/TitleBgActive. Or you may just create TitleBgActive from a tweaked TitleBg color.
 - 2016/05/07 (1.49) - removed confusing set of GetInternalState(), GetInternalStateSize(), SetInternalState() functions. Now using CreateContext(), DestroyContext(), GetCurrentContext(), SetCurrentContext().
 - 2016/05/02 (1.49) - renamed SetNextTreeNodeOpened() to SetNextTreeNodeOpen(), no redirection.
 - 2016/05/01 (1.49) - obsoleted old signature of CollapsingHeader(const char* label, const char* str_id = NULL, bool display_frame = true, bool default_open = false) as extra parameters were badly designed and rarely used. You can replace the "default_open = true" flag in new API with CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen).
 - 2016/04/26 (1.49) - changed ImDrawList::PushClipRect(ImVec4 rect) to ImDraw::PushClipRect(Imvec2 min,ImVec2 max,bool intersect_with_current_clip_rect=false). Note that higher-level ImGui::PushClipRect() is preferable because it will clip at logic/widget level, whereas ImDrawList::PushClipRect() only affect your renderer.
 - 2016/04/03 (1.48) - removed style.WindowFillAlphaDefault setting which was redundant. Bake default BG alpha inside style.Colors[ImGuiCol_WindowBg] and all other Bg color values. (ref github issue #337).
 - 2016/04/03 (1.48) - renamed ImGuiCol_TooltipBg to ImGuiCol_PopupBg, used by popups/menus and tooltips. popups/menus were previously using ImGuiCol_WindowBg. (ref github issue #337)
 - 2016/03/21 (1.48) - renamed GetWindowFont() to GetFont(), GetWindowFontSize() to GetFontSize(). Kept inline redirection function (will obsolete).
 - 2016/03/02 (1.48) - InputText() completion/history/always callbacks: if you modify the text buffer manually (without using DeleteChars()/InsertChars() helper) you need to maintain the BufTextLen field. added an assert.
 - 2016/01/23 (1.48) - fixed not honoring exact width passed to PushItemWidth(), previously it would add extra FramePadding.x*2 over that width. if you had manual pixel-perfect alignment in place it might affect you.
 - 2015/12/27 (1.48) - fixed ImDrawList::AddRect() which used to render a rectangle 1 px too large on each axis.
 - 2015/12/04 (1.47) - renamed Color() helpers to ValueColor() - dangerously named, rarely used and probably to be made obsolete.
 - 2015/08/29 (1.45) - with the addition of horizontal scrollbar we made various fixes to inconsistencies with dealing with cursor position.
                       GetCursorPos()/SetCursorPos() functions now include the scrolled amount. It shouldn't affect the majority of users, but take note that SetCursorPosX(100.0f) puts you at +100 from the starting x position which may include scrolling, not at +100 from the window left side.
                       GetContentRegionMax()/GetWindowContentRegionMin()/GetWindowContentRegionMax() functions allow include the scrolled amount. Typically those were used in cases where no scrolling would happen so it may not be a problem, but watch out!
 - 2015/08/29 (1.45) - renamed style.ScrollbarWidth to style.ScrollbarSize
 - 2015/08/05 (1.44) - split imgui.cpp into extra files: imgui_demo.cpp imgui_draw.cpp imgui_internal.h that you need to add to your project.
 - 2015/07/18 (1.44) - fixed angles in ImDrawList::PathArcTo(), PathArcToFast() (introduced in 1.43) being off by an extra PI for no justifiable reason
 - 2015/07/14 (1.43) - add new ImFontAtlas::AddFont() API. For the old AddFont***, moved the 'font_no' parameter of ImFontAtlas::AddFont** functions to the ImFontConfig structure.
                       you need to render your textured triangles with bilinear filtering to benefit from sub-pixel positioning of text.
 - 2015/07/08 (1.43) - switched rendering data to use indexed rendering. this is saving a fair amount of CPU/GPU and enables us to get anti-aliasing for a marginal cost.
                       this necessary change will break your rendering function! the fix should be very easy. sorry for that :(
                     - if you are using a vanilla copy of one of the imgui_impl_XXXX.cpp provided in the example, you just need to update your copy and you can ignore the rest.
                     - the signature of the io.RenderDrawListsFn handler has changed!
                            ImGui_XXXX_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
                       became:
                            ImGui_XXXX_RenderDrawLists(ImDrawData* draw_data).
                              argument   'cmd_lists'        -> 'draw_data->CmdLists'
                              argument   'cmd_lists_count'  -> 'draw_data->CmdListsCount'
                              ImDrawList 'commands'         -> 'CmdBuffer'
                              ImDrawList 'vtx_buffer'       -> 'VtxBuffer'
                              ImDrawList  n/a               -> 'IdxBuffer' (new)
                              ImDrawCmd  'vtx_count'        -> 'ElemCount'
                              ImDrawCmd  'clip_rect'        -> 'ClipRect'
                              ImDrawCmd  'user_callback'    -> 'UserCallback'
                              ImDrawCmd  'texture_id'       -> 'TextureId'
                     - each ImDrawList now contains both a vertex buffer and anindex buffer. For each command, render ElemCount/3 triangles using indices from the index buffer.
                     - if you REALLY cannot render indexed primitives, you can call the draw_data->DeIndexAllBuffers() method to de-index the buffers. This is slow and a waste of CPU/GPU. Prefer using indexed rendering!
                     - refer to code in the examples/ folder or ask on the GitHub if you are unsure of how to upgrade. please upgrade!
 - 2015/07/10 (1.43) - changed SameLine() parameters from int to float.
 - 2015/07/02 (1.42) - renamed SetScrollPosHere() to SetScrollFromCursorPos(). Kept inline redirection function (will obsolete).
 - 2015/07/02 (1.42) - renamed GetScrollPosY() to GetScrollY(). Necessary to reduce confusion along with other scrolling functions, because positions (e.g. cursor position) are not equivalent to scrolling amount.
 - 2015/06/14 (1.41) - changed ImageButton() default bg_col parameter from (0,0,0,1) (black) to (0,0,0,0) (transparent) - makes a difference when texture have transparence
 - 2015/06/14 (1.41) - changed Selectable() API from (label, selected, size) to (label, selected, flags, size). Size override should have been rarely be used. Sorry!
 - 2015/05/31 (1.40) - renamed GetWindowCollapsed() to IsWindowCollapsed() for consistency. Kept inline redirection function (will obsolete).
 - 2015/05/31 (1.40) - renamed IsRectClipped() to IsRectVisible() for consistency. Note that return value is opposite! Kept inline redirection function (will obsolete).
 - 2015/05/27 (1.40) - removed the third 'repeat_if_held' parameter from Button() - sorry! it was rarely used and inconsistent. Use PushButtonRepeat(true) / PopButtonRepeat() to enable repeat on desired buttons.
 - 2015/05/11 (1.40) - changed BeginPopup() API, takes a string identifier instead of a bool. ImGui needs to manage the open/closed state of popups. Call OpenPopup() to actually set the "open" state of a popup. BeginPopup() returns true if the popup is opened.
 - 2015/05/03 (1.40) - removed style.AutoFitPadding, using style.WindowPadding makes more sense (the default values were already the same).
 - 2015/04/13 (1.38) - renamed IsClipped() to IsRectClipped(). Kept inline redirection function until 1.50.
 - 2015/04/09 (1.38) - renamed ImDrawList::AddArc() to ImDrawList::AddArcFast() for compatibility with future API
 - 2015/04/03 (1.38) - removed ImGuiCol_CheckHovered, ImGuiCol_CheckActive, replaced with the more general ImGuiCol_FrameBgHovered, ImGuiCol_FrameBgActive.
 - 2014/04/03 (1.38) - removed support for passing -FLT_MAX..+FLT_MAX as the range for a SliderFloat(). Use DragFloat() or Inputfloat() instead.
 - 2015/03/17 (1.36) - renamed GetItemBoxMin()/GetItemBoxMax()/IsMouseHoveringBox() to GetItemRectMin()/GetItemRectMax()/IsMouseHoveringRect(). Kept inline redirection function until 1.50.
 - 2015/03/15 (1.36) - renamed style.TreeNodeSpacing to style.IndentSpacing, ImGuiStyleVar_TreeNodeSpacing to ImGuiStyleVar_IndentSpacing
 - 2015/03/13 (1.36) - renamed GetWindowIsFocused() to IsWindowFocused(). Kept inline redirection function until 1.50.
 - 2015/03/08 (1.35) - renamed style.ScrollBarWidth to style.ScrollbarWidth (casing)
 - 2015/02/27 (1.34) - renamed OpenNextNode(bool) to SetNextTreeNodeOpened(bool, ImGuiSetCond). Kept inline redirection function until 1.50.
 - 2015/02/27 (1.34) - renamed ImGuiSetCondition_*** to ImGuiSetCond_***, and _FirstUseThisSession becomes _Once.
 - 2015/02/11 (1.32) - changed text input callback ImGuiTextEditCallback return type from void-->int. reserved for future use, return 0 for now.
 - 2015/02/10 (1.32) - renamed GetItemWidth() to CalcItemWidth() to clarify its evolving behavior
 - 2015/02/08 (1.31) - renamed GetTextLineSpacing() to GetTextLineHeightWithSpacing()
 - 2015/02/01 (1.31) - removed IO.MemReallocFn (unused)
 - 2015/01/19 (1.30) - renamed ImGuiStorage::GetIntPtr()/GetFloatPtr() to GetIntRef()/GetIntRef() because Ptr was conflicting with actual pointer storage functions.
 - 2015/01/11 (1.30) - big font/image API change! now loads TTF file. allow for multiple fonts. no need for a PNG loader.
              (1.30) - removed GetDefaultFontData(). uses io.Fonts->GetTextureData*() API to retrieve uncompressed pixels.
                       this sequence:
                           const void* png_data;
                           unsigned int png_size;
                           ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
                           // <Copy to GPU>
                       became:
                           unsigned char* pixels;
                           int width, height;
                           io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
                           // <Copy to GPU>
                           io.Fonts->TexID = (your_texture_identifier);
                       you now have much more flexibility to load multiple TTF fonts and manage the texture buffer for internal needs.
                       it is now recommended that you sample the font texture with bilinear interpolation.
              (1.30) - added texture identifier in ImDrawCmd passed to your render function (we can now render images). make sure to set io.Fonts->TexID.
              (1.30) - removed IO.PixelCenterOffset (unnecessary, can be handled in user projection matrix)
              (1.30) - removed ImGui::IsItemFocused() in favor of ImGui::IsItemActive() which handles all widgets
 - 2014/12/10 (1.18) - removed SetNewWindowDefaultPos() in favor of new generic API SetNextWindowPos(pos, ImGuiSetCondition_FirstUseEver)
 - 2014/11/28 (1.17) - moved IO.Font*** options to inside the IO.Font-> structure (FontYOffset, FontTexUvForWhite, FontBaseScale, FontFallbackGlyph)
 - 2014/11/26 (1.17) - reworked syntax of IMGUI_ONCE_UPON_A_FRAME helper macro to increase compiler compatibility
 - 2014/11/07 (1.15) - renamed IsHovered() to IsItemHovered()
 - 2014/10/02 (1.14) - renamed IMGUI_INCLUDE_IMGUI_USER_CPP to IMGUI_INCLUDE_IMGUI_USER_INL and imgui_user.cpp to imgui_user.inl (more IDE friendly)
 - 2014/09/25 (1.13) - removed 'text_end' parameter from IO.SetClipboardTextFn (the string is now always zero-terminated for simplicity)
 - 2014/09/24 (1.12) - renamed SetFontScale() to SetWindowFontScale()
 - 2014/09/24 (1.12) - moved IM_MALLOC/IM_REALLOC/IM_FREE preprocessor defines to IO.MemAllocFn/IO.MemReallocFn/IO.MemFreeFn
 - 2014/08/30 (1.09) - removed IO.FontHeight (now computed automatically)
 - 2014/08/30 (1.09) - moved IMGUI_FONT_TEX_UV_FOR_WHITE preprocessor define to IO.FontTexUvForWhite
 - 2014/08/28 (1.09) - changed the behavior of IO.PixelCenterOffset following various rendering fixes


 FREQUENTLY ASKED QUESTIONS (FAQ), TIPS
 ======================================

 Q: How can I help?
 A: - If you are experienced enough with ImGui and with C/C++, look at the todo list and see how you want/can help!
    - Become a Patron/donate. Convince your company to become a Patron or provide serious funding for development time.

 Q: How do I update to a newer version of ImGui?
 A: Overwrite the following files:
      imgui.cpp
      imgui.h
      imgui_demo.cpp
      imgui_draw.cpp
      imgui_internal.h
      stb_rect_pack.h
      stb_textedit.h
      stb_truetype.h
    Don't overwrite imconfig.h if you have made modification to your copy.
    Check the "API BREAKING CHANGES" sections for a list of occasional API breaking changes. If you have a problem with a function, search for its name
    in the code, there will likely be a comment about it. Please report any issue to the GitHub page!


 Q: What is ImTextureID and how do I display an image?
 A: ImTextureID is a void* used to pass renderer-agnostic texture references around until it hits your render function.
    ImGui knows nothing about what those bits represent, it just passes them around. It is up to you to decide what you want the void* to carry!
    It could be an identifier to your OpenGL texture (cast GLuint to void*), a pointer to your custom engine material (cast MyMaterial* to void*), etc.
    At the end of the chain, your renderer takes this void* to cast it back into whatever it needs to select a current texture to render.
    Refer to examples applications, where each renderer (in a imgui_impl_xxxx.cpp file) is treating ImTextureID as a different thing.
    (c++ tip: OpenGL uses integers to identify textures. You can safely store an integer into a void*, just cast it to void*, don't take it's address!)
    To display a custom image/texture within an ImGui window, you may use ImGui::Image(), ImGui::ImageButton(), ImDrawList::AddImage() functions.
    ImGui will generate the geometry and draw calls using the ImTextureID that you passed and which your renderer can use.
    It is your responsibility to get textures uploaded to your GPU.

 Q: I integrated ImGui in my engine and the text or lines are blurry..
 A: In your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f).
    Also make sure your orthographic projection matrix and io.DisplaySize matches your actual framebuffer dimension.

 Q: I integrated ImGui in my engine and some elements are clipping or disappearing when I move windows around..
 A: Most likely you are mishandling the clipping rectangles in your render function. Rectangles provided by ImGui are defined as (x1=left,y1=top,x2=right,y2=bottom) and NOT as (x1,y1,width,height).

 Q: Can I have multiple widgets with the same label? Can I have widget without a label? (Yes)
 A: Yes. A primer on the use of labels/IDs in ImGui..

   - Elements that are not clickable, such as Text() items don't need an ID.

   - Interactive widgets require state to be carried over multiple frames (most typically ImGui often needs to remember what is the "active" widget).
     to do so they need a unique ID. unique ID are typically derived from a string label, an integer index or a pointer.

       Button("OK");        // Label = "OK",     ID = hash of "OK"
       Button("Cancel");    // Label = "Cancel", ID = hash of "Cancel"

   - ID are uniquely scoped within windows, tree nodes, etc. so no conflict can happen if you have two buttons called "OK" in two different windows
     or in two different locations of a tree.

   - If you have a same ID twice in the same location, you'll have a conflict:

       Button("OK");
       Button("OK");           // ID collision! Both buttons will be treated as the same.

     Fear not! this is easy to solve and there are many ways to solve it!

   - When passing a label you can optionally specify extra unique ID information within string itself. This helps solving the simpler collision cases.
     use "##" to pass a complement to the ID that won't be visible to the end-user:

       Button("Play");         // Label = "Play",   ID = hash of "Play"
       Button("Play##foo1");   // Label = "Play",   ID = hash of "Play##foo1" (different from above)
       Button("Play##foo2");   // Label = "Play",   ID = hash of "Play##foo2" (different from above)

   - If you want to completely hide the label, but still need an ID:

       Checkbox("##On", &b);   // Label = "",       ID = hash of "##On" (no label!)

   - Occasionally/rarely you might want change a label while preserving a constant ID. This allows you to animate labels.
     For example you may want to include varying information in a window title bar (and windows are uniquely identified by their ID.. obviously)
     Use "###" to pass a label that isn't part of ID:

       Button("Hello###ID";   // Label = "Hello",  ID = hash of "ID"
       Button("World###ID";   // Label = "World",  ID = hash of "ID" (same as above)

       sprintf(buf, "My game (%f FPS)###MyGame");
       Begin(buf);            // Variable label,   ID = hash of "MyGame"

   - Use PushID() / PopID() to create scopes and avoid ID conflicts within the same Window.
     This is the most convenient way of distinguishing ID if you are iterating and creating many UI elements.
     You can push a pointer, a string or an integer value. Remember that ID are formed from the concatenation of everything in the ID stack!

       for (int i = 0; i < 100; i++)
       {
         PushID(i);
         Button("Click");   // Label = "Click",  ID = hash of integer + "label" (unique)
         PopID();
       }

       for (int i = 0; i < 100; i++)
       {
         MyObject* obj = Objects[i];
         PushID(obj);
         Button("Click");   // Label = "Click",  ID = hash of pointer + "label" (unique)
         PopID();
       }

       for (int i = 0; i < 100; i++)
       {
         MyObject* obj = Objects[i];
         PushID(obj->Name);
         Button("Click");   // Label = "Click",  ID = hash of string + "label" (unique)
         PopID();
       }

   - More example showing that you can stack multiple prefixes into the ID stack:

       Button("Click");     // Label = "Click",  ID = hash of "Click"
       PushID("node");
       Button("Click");     // Label = "Click",  ID = hash of "node" + "Click"
         PushID(my_ptr);
           Button("Click"); // Label = "Click",  ID = hash of "node" + ptr + "Click"
         PopID();
       PopID();

   - Tree nodes implicitly creates a scope for you by calling PushID().

       Button("Click");     // Label = "Click",  ID = hash of "Click"
       if (TreeNode("node"))
       {
         Button("Click");   // Label = "Click",  ID = hash of "node" + "Click"
         TreePop();
       }

   - When working with trees, ID are used to preserve the open/close state of each tree node.
     Depending on your use cases you may want to use strings, indices or pointers as ID.
      e.g. when displaying a single object that may change over time (1-1 relationship), using a static string as ID will preserve your node open/closed state when the targeted object change.
      e.g. when displaying a list of objects, using indices or pointers as ID will preserve the node open/closed state differently. experiment and see what makes more sense!

 Q: How can I tell when ImGui wants my mouse/keyboard inputs and when I can pass them to my application?
 A: You can read the 'io.WantCaptureXXX' flags in the ImGuiIO structure. Preferably read them after calling ImGui::NewFrame() to avoid those flags lagging by one frame, but either should be fine.
    When 'io.WantCaptureMouse' or 'io.WantCaptureKeyboard' flags are set you may want to discard/hide the inputs from the rest of your application.
    When 'io.WantInputsCharacters' is set to may want to notify your OS to popup an on-screen keyboard, if available.
    ImGui is tracking dragging and widget activity that may occur outside the boundary of a window, so 'io.WantCaptureMouse' is a more accurate and complete than testing for ImGui::IsMouseHoveringAnyWindow().
    (Advanced note: text input releases focus on Return 'KeyDown', so the following Return 'KeyUp' event that your application receive will typically have 'io.WantcaptureKeyboard=false'. 
     Depending on your application logic it may or not be inconvenient. You might want to track which key-downs were for ImGui (e.g. with an array of bool) and filter out the corresponding key-ups.)

 Q: How can I load a different font than the default? (default is an embedded version of ProggyClean.ttf, rendered at size 13)
 A: Use the font atlas to load the TTF file you want:

      ImGuiIO& io = ImGui::GetIO();
      io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
      io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()

 Q: How can I load multiple fonts?
 A: Use the font atlas to pack them into a single texture:
    (Read extra_fonts/README.txt and the code in ImFontAtlas for more details.)

      ImGuiIO& io = ImGui::GetIO();
      ImFont* font0 = io.Fonts->AddFontDefault();
      ImFont* font1 = io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
      ImFont* font2 = io.Fonts->AddFontFromFileTTF("myfontfile2.ttf", size_in_pixels);
      io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()
      // the first loaded font gets used by default
      // use ImGui::PushFont()/ImGui::PopFont() to change the font at runtime

      // Options
      ImFontConfig config;
      config.OversampleH = 3;
      config.OversampleV = 1;
      config.GlyphExtraSpacing.x = 1.0f;
      io.Fonts->LoadFromFileTTF("myfontfile.ttf", size_pixels, &config);

      // Combine multiple fonts into one
      ImWchar ranges[] = { 0xf000, 0xf3ff, 0 };
      ImFontConfig config;
      config.MergeMode = true;
      io.Fonts->AddFontDefault();
      io.Fonts->LoadFromFileTTF("fontawesome-webfont.ttf", 16.0f, &config, ranges);
      io.Fonts->LoadFromFileTTF("myfontfile.ttf", size_pixels, NULL, &config, io.Fonts->GetGlyphRangesJapanese());

 Q: How can I display and input non-Latin characters such as Chinese, Japanese, Korean, Cyrillic?
 A: When loading a font, pass custom Unicode ranges to specify the glyphs to load. 
    All your strings needs to use UTF-8 encoding. Specifying literal in your source code using a local code page (such as CP-923 for Japanese or CP-1251 for Cyrillic) will not work.
    In C++11 you can encode a string literal in UTF-8 by using the u8"hello" syntax. Otherwise you can convert yourself to UTF-8 or load text data from file already saved as UTF-8.
    You can also try to remap your local codepage characters to their Unicode codepoint using font->AddRemapChar(), but international users may have problems reading/editing your source code.

      io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels, NULL, io.Fonts->GetGlyphRangesJapanese());  // Load Japanese characters
      io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()
      io.ImeWindowHandle = MY_HWND;      // To input using Microsoft IME, give ImGui the hwnd of your application

    As for text input, depends on you passing the right character code to io.AddInputCharacter(). The example applications do that.

 Q: How can I use the drawing facilities without an ImGui window? (using ImDrawList API)
 A: The easiest way is to create a dummy window. Call Begin() with NoTitleBar|NoResize|NoMove|NoScrollbar|NoSavedSettings|NoInputs flag, zero background alpha, 
    then retrieve the ImDrawList* via GetWindowDrawList() and draw to it in any way you like.

 - tip: the construct 'IMGUI_ONCE_UPON_A_FRAME { ... }' will run the block of code only once a frame. You can use it to quickly add custom UI in the middle of a deep nested inner loop in your code.
 - tip: you can create widgets without a Begin()/End() block, they will go in an implicit window called "Debug"
 - tip: you can call Begin() multiple times with the same name during the same frame, it will keep appending to the same window. this is also useful to set yourself in the context of another window (to get/set other settings)
 - tip: you can call Render() multiple times (e.g for VR renders).
 - tip: call and read the ShowTestWindow() code in imgui_demo.cpp for more example of how to use ImGui!


 ISSUES & TODO-LIST
 ==================
 Issue numbers (#) refer to github issues listed at https://github.com/ocornut/imgui/issues
 The list below consist mostly of notes of things to do before they are requested/discussed by users (at that point it usually happens on the github)

 - doc: add a proper documentation+regression testing system (#435)
 - window: add a way for very transient windows (non-saved, temporary overlay over hundreds of objects) to "clean" up from the global window list. perhaps a lightweight explicit cleanup pass.
 - window: calling SetNextWindowSize() every frame with <= 0 doesn't do anything, may be useful to allow (particularly when used for a single axis) (#690)
 - window: auto-fit feedback loop when user relies on any dynamic layout (window width multiplier, column) appears weird to end-user. clarify.
 - window: allow resizing of child windows (possibly given min/max for each axis?)
 - window: background options for child windows, border option (disable rounding)
 - window: add a way to clear an existing window instead of appending (e.g. for tooltip override using a consistent api rather than the deferred tooltip)
 - window: resizing from any sides? + mouse cursor directives for app.
!- window: begin with *p_open == false should return false.
 - window: get size/pos helpers given names (see discussion in #249)
 - window: a collapsed window can be stuck behind the main menu bar?
 - window: when window is small, prioritize resize button over close button.
 - window: detect extra End() call that pop the "Debug" window out and assert at call site instead of later.
 - window/tooltip: allow to set the width of a tooltip to allow TextWrapped() etc. while keeping the height automatic.
 - window: increase minimum size of a window with menus or fix the menu rendering so that it doesn't look odd.
 - draw-list: maintaining bounding box per command would allow to merge draw command when clipping isn't relied on (typical non-scrolling window or non-overflowing column would merge with previous command).
!- scrolling: allow immediately effective change of scroll if we haven't appended items yet
 - splitter/separator: formalize the splitter idiom into an official api (we want to handle n-way split) (#319)
 - widgets: display mode: widget-label, label-widget (aligned on column or using fixed size), label-newline-tab-widget etc.
 - widgets: clean up widgets internal toward exposing everything.
 - widgets: add disabled and read-only modes (#211)
 - main: considering adding an Init() function? some constructs are awkward in the implementation because of the lack of them.
!- main: make it so that a frame with no window registered won't refocus every window on subsequent frames (~bump LastFrameActive of all windows).
 - main: IsItemHovered() make it more consistent for various type of widgets, widgets with multiple components, etc. also effectively IsHovered() region sometimes differs from hot region, e.g tree nodes
 - main: IsItemHovered() info stored in a stack? so that 'if TreeNode() { Text; TreePop; } if IsHovered' return the hover state of the TreeNode?
 - input text: clean up the mess caused by converting UTF-8 <> wchar. the code is rather inefficient right now and super fragile.
 - input text: reorganize event handling, allow CharFilter to modify buffers, allow multiple events? (#541)
 - input text: expose CursorPos in char filter event (#816)
 - input text: flag to disable live update of the user buffer (also applies to float/int text input) 
 - input text: resize behavior - field could stretch when being edited? hover tooltip shows more text?
 - input text: add ImGuiInputTextFlags_EnterToApply? (off #218)
 - input text: add discard flag (e.g. ImGuiInputTextFlags_DiscardActiveBuffer) or make it easier to clear active focus for text replacement during edition (#725)
 - input text multi-line: don't directly call AddText() which does an unnecessary vertex reserve for character count prior to clipping. and/or more line-based clipping to AddText(). and/or reorganize TextUnformatted/RenderText for more efficiency for large text (e.g TextUnformatted could clip and log separately, etc).
 - input text multi-line: way to dynamically grow the buffer without forcing the user to initially allocate for worse case (follow up on #200)
 - input text multi-line: line numbers? status bar? (follow up on #200)
 - input text multi-line: behave better when user changes input buffer while editing is active (even though it is illegal behavior). namely, the change of buffer can create a scrollbar glitch (#725)
 - input text: allow centering/positioning text so that ctrl+clicking Drag or Slider keeps the textual value at the same pixel position.
 - input number: optional range min/max for Input*() functions
 - input number: holding [-]/[+] buttons could increase the step speed non-linearly (or user-controlled)
 - input number: use mouse wheel to step up/down
 - input number: applying arithmetics ops (+,-,*,/) messes up with text edit undo stack.
 - button: provide a button that looks framed.
 - text: proper alignment options
 - image/image button: misalignment on padded/bordered button?
 - image/image button: parameters are confusing, image() has tint_col,border_col whereas imagebutton() has bg_col/tint_col. Even thou they are different parameters ordering could be more consistent. can we fix that?
 - layout: horizontal layout helper (#97)
 - layout: horizontal flow until no space left (#404)
 - layout: more generic alignment state (left/right/centered) for single items?
 - layout: clean up the InputFloatN/SliderFloatN/ColorEdit4 layout code. item width should include frame padding.
 - layout: BeginGroup() needs a border option.
 - columns: declare column set (each column: fixed size, %, fill, distribute default size among fills) (#513, #125)
 - columns: add a conditional parameter to SetColumnOffset() (#513, #125)
 - columns: separator function or parameter that works within the column (currently Separator() bypass all columns) (#125)
 - columns: columns header to act as button (~sort op) and allow resize/reorder (#513, #125)
 - columns: user specify columns size (#513, #125)
 - columns: flag to add horizontal separator above/below?
 - columns/layout: setup minimum line height (equivalent of automatically calling AlignFirstTextHeightToWidgets)
 - combo: sparse combo boxes (via function call?) / iterators
 - combo: contents should extends to fit label if combo widget is small
 - combo/listbox: keyboard control. need InputText-like non-active focus + key handling. considering keyboard for custom listbox (pr #203)
 - listbox: multiple selection
 - listbox: user may want to initial scroll to focus on the one selected value?
 - listbox: keyboard navigation.
 - listbox: scrolling should track modified selection.
!- popups/menus: clarify usage of popups id, how MenuItem/Selectable closing parent popups affects the ID, etc. this is quite fishy needs improvement! (#331, #402)
 - popups: add variant using global identifier similar to Begin/End (#402)
 - popups: border options. richer api like BeginChild() perhaps? (#197)
 - tooltip: tooltip that doesn't fit in entire screen seems to lose their "last preferred button" and may teleport when moving mouse
 - menus: local shortcuts, global shortcuts (#456, #126)
 - menus: icons
 - menus: menubars: some sort of priority / effect of main menu-bar on desktop size?
 - menus: calling BeginMenu() twice with a same name doesn't seem to append nicely
 - statusbar: add a per-window status bar helper similar to what menubar does.
 - tabs (#261, #351)
 - separator: separator on the initial position of a window is not visible (cursorpos.y <= clippos.y)
!- color: the color helpers/typing is a mess and needs sorting out.
 - color: add a better color picker (#346)
 - node/graph editor (#306)
 - pie menus patterns (#434)
 - drag'n drop, dragging helpers (carry dragging info, visualize drag source before clicking, drop target, etc.) (#143, #479)
 - plot: PlotLines() should use the polygon-stroke facilities (currently issues with averaging normals)
 - plot: make it easier for user to draw extra stuff into the graph (e.g: draw basis, highlight certain points, 2d plots, multiple plots)
 - plot: "smooth" automatic scale over time, user give an input 0.0(full user scale) 1.0(full derived from value)
 - plot: add a helper e.g. Plot(char* label, float value, float time_span=2.0f) that stores values and Plot them for you - probably another function name. and/or automatically allow to plot ANY displayed value (more reliance on stable ID)
 - slider: allow using the [-]/[+] buttons used by InputFloat()/InputInt()
 - slider: initial absolute click is imprecise. change to relative movement slider (same as scrollbar).
 - slider: add dragging-based widgets to edit values with mouse (on 2 axises), saving screen real-estate.
 - slider: tint background based on value (e.g. v_min -> v_max, or use 0.0f either side of the sign)
 - slider & drag: int data passing through a float
 - drag float: up/down axis
 - drag float: added leeway on edge (e.g. a few invisible steps past the clamp limits)
 - tree node / optimization: avoid formatting when clipped.
 - tree node: tree-node/header right-most side doesn't take account of horizontal scrolling.
 - tree node: add treenode/treepush int variants? not there because (void*) cast from int warns on some platforms/settings?
 - tree node: try to apply scrolling at time of TreePop() if node was just opened and end of node is past scrolling limits?
 - tree node / selectable render mismatch which is visible if you use them both next to each other (e.g. cf. property viewer)
 - tree node: tweak color scheme to distinguish headers from selected tree node (#581)
 - textwrapped: figure out better way to use TextWrapped() in an always auto-resize context (tooltip, etc.) (#249)
 - settings: write more decent code to allow saving/loading new fields
 - settings: api for per-tool simple persistent data (bool,int,float,columns sizes,etc.) in .ini file
 - style: add window shadows.
 - style/optimization: store rounded corners in texture to use 1 quad per corner (filled and wireframe) to lower the cost of rounding.
 - style: color-box not always square?
 - style: a concept of "compact style" that the end-user can easily rely on (e.g. PushStyleCompact()?) that maps to other settings? avoid implementing duplicate helpers such as SmallCheckbox(), etc.
 - style: try to make PushStyleVar() more robust to incorrect parameters (to be more friendly to edit & continues situation).
 - style: global scale setting.
 - style: WindowPadding needs to be EVEN needs the 0.5 multiplier probably have a subtle effect on clip rectangle
 - text: simple markup language for color change?
 - font: dynamic font atlas to avoid baking huge ranges into bitmap and make scaling easier.
 - font: small opt: for monospace font (like the defalt one) we can trim IndexXAdvance as long as trailing value is == FallbackXAdvance
 - font: add support for kerning, probably optional. perhaps default to (32..128)^2 matrix ~ 36KB then hash fallback.
 - font: add a simpler CalcTextSizeA() api? current one ok but not welcome if user needs to call it directly (without going through ImGui::CalcTextSize)
 - font: fix AddRemapChar() to work before font has been built.
 - log: LogButtons() options for specifying depth and/or hiding depth slider
 - log: have more control over the log scope (e.g. stop logging when leaving current tree node scope)
 - log: be able to log anything (e.g. right-click on a window/tree-node, shows context menu? log into tty/file/clipboard)
 - log: let user copy any window content to clipboard easily (CTRL+C on windows? while moving it? context menu?). code is commented because it fails with multiple Begin/End pairs.
 - filters: set a current filter that tree node can automatically query to hide themselves
 - filters: handle wildcards (with implicit leading/trailing *), regexps
 - shortcuts: add a shortcut api, e.g. parse "&Save" and/or "Save (CTRL+S)", pass in to widgets or provide simple ways to use (button=activate, input=focus)
!- keyboard: tooltip & combo boxes are messing up / not honoring keyboard tabbing
 - keyboard: full keyboard navigation and focus. (#323)
 - focus: preserve ActiveId/focus stack state, e.g. when opening a menu and close it, previously selected InputText() focus gets restored (#622)
 - focus: SetKeyboardFocusHere() on with >= 0 offset could be done on same frame (else latch and modulate on beginning of next frame)
 - input: rework IO system to be able to pass actual ordered/timestamped events. (~#335, #71)
 - input: allow to decide and pass explicit double-clicks (e.g. for windows by the CS_DBLCLKS style).
 - input: support track pad style scrolling & slider edit.
 - misc: provide a way to compile out the entire implementation while providing a dummy API (e.g. #define IMGUI_DUMMY_IMPL)
 - misc: double-clicking on title bar to minimize isn't consistent, perhaps move to single-click on left-most collapse icon?
 - misc: provide HoveredTime and ActivatedTime to ease the creation of animations.
 - style editor: have a more global HSV setter (e.g. alter hue on all elements). consider replacing active/hovered by offset in HSV space? (#438)
 - style editor: color child window height expressed in multiple of line height.
 - remote: make a system like RemoteImGui first-class citizen/project (#75)
 - drawlist: move Font, FontSize, FontTexUvWhitePixel inside ImDrawList and make it self-contained (apart from drawing settings?)
 - drawlist: end-user probably can't call Clear() directly because we expect a texture to be pushed in the stack.
 - examples: directx9: save/restore device state more thoroughly.
 - examples: window minimize, maximize (#583)
 - optimization: add a flag to disable most of rendering, for the case where the user expect to skip it (#335)
 - optimization: use another hash function than crc32, e.g. FNV1a
 - optimization/render: merge command-lists with same clip-rect into one even if they aren't sequential? (as long as in-between clip rectangle don't overlap)?
 - optimization: turn some the various stack vectors into statically-sized arrays
 - optimization: better clipping for multi-component widgets
