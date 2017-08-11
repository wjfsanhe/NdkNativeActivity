// ImGui library v1.44 WIP
// See ImGui::ShowTestWindow() for sample code.
// Read 'Programmer guide' below for notes on how to setup ImGui in your codebase.
// Get latest version at https://github.com/ocornut/imgui
// Developed by Omar Cornut and ImGui contributors.

/*

 Index
 - MISSION STATEMENT
 - END-USER GUIDE
 - PROGRAMMER GUIDE (read me!)
 - API BREAKING CHANGES (read me when you update!)
 - FREQUENTLY ASKED QUESTIONS (FAQ), TIPS
   - How do I update to a newer version of ImGui?
   - Can I have multiple widgets with the same label? Can I have widget without a label? (Yes)
   - Why is my text output blurry?
   - How can I load a different font than the default? 
   - How can I load multiple fonts?
   - How can I display and input non-latin characters such as Chinese, Japanese, Korean, Cyrillic?
 - ISSUES & TODO-LIST
 - CODE
 - SAMPLE CODE
 - DEFAULT FONT DATA

 
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
 - click upper right corner to close a window, available when 'bool* p_opened' is passed to ImGui::Begin()
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
 - call and read ImGui::ShowTestWindow() for sample code demonstrating most features.
 - see examples/ folder for standalone sample applications. e.g. examples/opengl_example/
 - customization: PushStyleColor()/PushStyleVar() or the style editor to tweak the look of the interface (e.g. if you want a more compact UI or a different color scheme).

 - getting started:
   - initialisation: call ImGui::GetIO() to retrieve the ImGuiIO structure and fill the 'Settings' data.
   - every frame: 
      1/ in your mainloop or right after you got your keyboard/mouse info, call ImGui::GetIO() and fill the 'Input' data, then call ImGui::NewFrame().
      2/ use any ImGui function you want between NewFrame() and Render()
      3/ ImGui::Render() to render all the accumulated command-lists. it will call your RenderDrawListFn handler that you set in the IO structure.
   - all rendering information are stored into command-lists until ImGui::Render() is called. 
   - ImGui never touches or know about your GPU state. the only function that knows about GPU is the RenderDrawListFn handler that you must provide.
   - effectively it means you can create widgets at any time in your code, regardless of "update" vs "render" considerations.
   - refer to the examples applications in the examples/ folder for instruction on how to setup your code.
   - a typical application skeleton may be:

        // Application init
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = 1920.0f;
        io.DisplaySize.y = 1280.0f;
        io.DeltaTime = 1.0f/60.0f;
        io.IniFilename = "imgui.ini";
        // TODO: Fill others settings of the io structure

        // Load texture
        unsigned char* pixels;
        int width, height, bytes_per_pixels;
        io.Fonts->GetTexDataAsRGBA32(pixels, &width, &height, &bytes_per_pixels);
        // TODO: copy texture to graphics memory. 
        // TODO: store your texture pointer/identifier in 'io.Fonts->TexID'

        // Application main loop
        while (true)
        {
            // 1) get low-level input
            // e.g. on Win32, GetKeyboardState(), or poll your events, etc.
            
            // 2) TODO: fill all fields of IO structure and call NewFrame
            ImGuiIO& io = ImGui::GetIO();
            io.MousePos = mouse_pos;
            io.MouseDown[0] = mouse_button_0;
            io.KeysDown[i] = ...
            ImGui::NewFrame();

            // 3) most of your application code here - you can use any of ImGui::* functions at any point in the frame
            ImGui::Begin("My window");
            ImGui::Text("Hello, world.");
            ImGui::End();
            GameUpdate();
            GameRender();

            // 4) render & swap video buffers
            ImGui::Render();
            // swap video buffer, etc.
        }

   - after calling ImGui::NewFrame() you can read back 'io.WantCaptureMouse' and 'io.WantCaptureKeyboard' to tell if ImGui 
     wants to use your inputs. if it does you can discard/hide the inputs from the rest of your application.

 API BREAKING CHANGES
 ====================

 Occasionally introducing changes that are breaking the API. The breakage are generally minor and easy to fix.
 Here is a change-log of API breaking changes, if you are using one of the functions listed, expect to have to fix some code.
 
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
                     - each ImDrawList now contains both a vertex buffer and an index buffer. For each command, render ElemCount/3 triangles using indices from the index buffer.
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
 - 2015/05/11 (1.40) - changed BeginPopup() API, takes a string identifier instead of a bool. ImGui needs to manage the open/closed state of popups. Call OpenPopup() to actually set the "opened" state of a popup. BeginPopup() returns true if the popup is opened.
 - 2015/05/03 (1.40) - removed style.AutoFitPadding, using style.WindowPadding makes more sense (the default values were already the same).
 - 2015/04/13 (1.38) - renamed IsClipped() to IsRectClipped(). Kept inline redirection function (will obsolete).
 - 2015/04/09 (1.38) - renamed ImDrawList::AddArc() to ImDrawList::AddArcFast() for compatibility with future API
 - 2015/04/03 (1.38) - removed ImGuiCol_CheckHovered, ImGuiCol_CheckActive, replaced with the more general ImGuiCol_FrameBgHovered, ImGuiCol_FrameBgActive.
 - 2014/04/03 (1.38) - removed support for passing -FLT_MAX..+FLT_MAX as the range for a SliderFloat(). Use DragFloat() or Inputfloat() instead.
 - 2015/03/17 (1.36) - renamed GetItemBoxMin()/GetItemBoxMax()/IsMouseHoveringBox() to GetItemRectMin()/GetItemRectMax()/IsMouseHoveringRect(). Kept inline redirection function (will obsolete).
 - 2015/03/15 (1.36) - renamed style.TreeNodeSpacing to style.IndentSpacing, ImGuiStyleVar_TreeNodeSpacing to ImGuiStyleVar_IndentSpacing
 - 2015/03/13 (1.36) - renamed GetWindowIsFocused() to IsWindowFocused(). Kept inline redirection function (will obsolete).
 - 2015/03/08 (1.35) - renamed style.ScrollBarWidth to style.ScrollbarWidth
 - 2015/02/27 (1.34) - renamed OpenNextNode(bool) to SetNextTreeNodeOpened(bool, ImGuiSetCond). Kept inline redirection function (will obsolete).
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
                       it is now recommended your sample the font texture with bilinear interpolation.
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

 Q: How do I update to a newer version of ImGui?
 A: Overwrite the following files:
      imgui.cpp
      imgui.h
      stb_rect_pack.h
      stb_textedit.h
      stb_truetype.h
    Check the "API BREAKING CHANGES" sections for a list of occasional API breaking changes. If you have a problem with a function, search for its name
    in the code, there will likely be a comment about it. Please report any issue to the GitHub page!

 Q: Can I have multiple widgets with the same label? Can I have widget without a label? (Yes)
 A: Yes. A primer on the use of labels/IDs in ImGui..
 
   - Elements that are not clickable, such as Text() items don't need an ID.

   - Interactive widgets require state to be carried over multiple frames (most typically ImGui often needs to remember what is the "active" widget).
     to do so they need an unique ID. unique ID are typically derived from a string label, an integer index or a pointer.
     
       Button("OK");        // Label = "OK",     ID = hash of "OK"
       Button("Cancel");    // Label = "Cancel", ID = hash of "Cancel"

   - ID are uniquely scoped within windows, tree nodes, etc. so no conflict can happen if you have two buttons called "OK" in two different windows
     or in two different locations of a tree.

   - if you have a same ID twice in the same location, you'll have a conflict:

       Button("OK");
       Button("OK");           // ID collision! Both buttons will be treated as the same.

     Fear not! this is easy to solve and there are many ways to solve it!

   - when passing a label you can optionally specify extra unique ID information within string itself. This helps solving the simpler collision cases.
     use "##" to pass a complement to the ID that won't be visible to the end-user:

       Button("Play##0");      // Label = "Play",   ID = hash of "Play##0"
       Button("Play##1");      // Label = "Play",   ID = hash of "Play##1" (different from above)

   - so if you want to hide the label but need an ID:

       Checkbox("##On", &b);   // Label = "",       ID = hash of "##On"

   - occasionally (rarely) you might want change a label while preserving a constant ID. This allows you to animate labels.
     use "###" to pass a label that isn't part of ID:

       Button("Hello###ID";   // Label = "Hello",  ID = hash of "ID"
       Button("World###ID";   // Label = "World",  ID = hash of "ID" (same as above)

   - use PushID() / PopID() to create scopes and avoid ID conflicts within the same Window.
     this is the most convenient way of distinguish ID if you are iterating and creating many UI elements.
     you can push a pointer, a string or an integer value. remember that ID are formed from the addition of everything in the ID stack!

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

   - more example showing that you can stack multiple prefixes into the ID stack:

       Button("Click");     // Label = "Click",  ID = hash of "Click"
       PushID("node");
       Button("Click");     // Label = "Click",  ID = hash of "node" + "Click"
         PushID(my_ptr);
           Button("Click"); // Label = "Click",  ID = hash of "node" + ptr + "Click"
         PopID();
       PopID();

   - tree nodes implicitly creates a scope for you by calling PushID().

       Button("Click");     // Label = "Click",  ID = hash of "Click"
       if (TreeNode("node"))
       {
         Button("Click");   // Label = "Click",  ID = hash of "node" + "Click"
         TreePop();
       }

   - when working with trees, ID are used to preserve the opened/closed state of each tree node.
     depending on your use cases you may want to use strings, indices or pointers as ID. 
      e.g. when displaying a single object that may change over time (1-1 relationship), using a static string as ID will preserve your node open/closed state when the targeted object change.
      e.g. when displaying a list of objects, using indices or pointers as ID will preserve the node open/closed state differently. experiment and see what makes more sense!

 Q: Why is my text output blurry?
 A: In your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)

 Q: How can I load a different font than the default? (default is an embedded version of ProggyClean.ttf, rendered at size 13)
 A: Use the font atlas to load the TTF file you want:

     io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
     io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()

 Q: How can I load multiple fonts?
 A: Use the font atlas to pack them into a single texture:

     ImFont* font0 = io.Fonts->AddFontDefault();
     ImFont* font1 = io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
     ImFont* font2 = io.Fonts->AddFontFromFileTTF("myfontfile2.ttf", size_in_pixels);
     io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()
     // the first loaded font gets used by default
     // use ImGui::PushFont()/ImGui::PopFont() to change the font at runtime

     // Options
     ImFontConfig config;
     config.OversampleH = 3;
     config.OversampleV = 3;
     config.GlyphExtraSpacing.x = 1.0f;
     io.Fonts->LoadFromFileTTF("myfontfile.ttf", size_pixels, &config);

     // Merging input from different fonts into one
     ImWchar ranges[] = { 0xf000, 0xf3ff, 0 };
     ImFontConfig config;
     config.MergeMode = true;
     io.Fonts->AddFontDefault();
     io.Fonts->LoadFromFileTTF("fontawesome-webfont.ttf", 16.0f, &config, ranges);
     io.Fonts->LoadFromFileTTF("myfontfile.ttf", size_pixels, NULL, &config, io.Fonts->GetGlyphRangesJapanese());

 Q: How can I display and input non-latin characters such as Chinese, Japanese, Korean, Cyrillic?
 A: When loading a font, pass custom Unicode ranges to specify the glyphs to load. ImGui will support UTF-8 encoding across the board.
    Character input depends on you passing the right character code to io.AddInputCharacter(). The example applications do that.

     io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels, NULL, io.Fonts->GetGlyphRangesJapanese());  // Load Japanese characters
     io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()
     io.ImeWindowHandle = MY_HWND;      // To input using Microsoft IME, give ImGui the hwnd of your application

 - tip: the construct 'IMGUI_ONCE_UPON_A_FRAME { ... }' will run the block of code only once a frame. You can use it to quickly add custom UI in the middle of a deep nested inner loop in your code.
 - tip: you can create widgets without a Begin()/End() block, they will go in an implicit window called "Debug"
 - tip: you can call Begin() multiple times with the same name during the same frame, it will keep appending to the same window.
 - tip: you can call Render() multiple times (e.g for VR renders).
 - tip: call and read the ShowTestWindow() code for more example of how to use ImGui!


 ISSUES & TODO-LIST
 ==================
 Issue numbers (#) refer to github issues.

 - misc: merge or clarify ImVec4 vs ImRect?
 - window: autofit feedback loop when user relies on any dynamic layout (window width multiplier, column). maybe just clearly drop manual autofit?
 - window: add a way for very transient windows (non-saved, temporary overlay over hundreds of objects) to "clean" up from the global window list. 
 - window: allow resizing of child windows (possibly given min/max for each axis?)
 - window: background options for child windows, border option (disable rounding)
 - window: resizing from any sides? + mouse cursor directives for app.
 - window: get size/pos helpers given names (see discussion in #249)
 - window: a collapsed window can be stuck behing the main menu bar?
 - scrolling: add horizontal scroll
!- scrolling: allow immediately effective change of scroll if we haven't appended items yet
 - widgets: display mode: widget-label, label-widget (aligned on column or using fixed size), label-newline-tab-widget etc.
 - widgets: clean up widgets internal toward exposing everything.
 - widgets: add a disabled/read-only mode (#211) 
 - main: considering adding EndFrame()/Init(). some constructs are awkward in the implementation because of the lack of them.
 - main: IsItemHovered() make it more consistent for various type of widgets, widgets with multiple components, etc. also effectively IsHovered() region sometimes differs from hot region, e.g tree nodes
 - main: IsItemHovered() info stored in a stack? so that 'if TreeNode() { Text; TreePop; } if IsHovered' return the hover state of the TreeNode?
 - input text: add ImGuiInputTextFlags_EnterToApply? (off #218)
 - input text multi-line: way to dynamically grow the buffer without forcing the user to initially allocate for worse case (follow up on #200)
 - input text multi-line: line numbers? status bar? (follow up on #200)
!- input number: large int not reliably supported because of int<>float conversions.
 - input number: optional range min/max for Input*() functions
 - input number: holding [-]/[+] buttons could increase the step speed non-linearly (or user-controlled)
 - input number: use mouse wheel to step up/down
 - input number: non-decimal input.
 - text: proper alignment options
 - layout: horizontal layout helper (#97)
 - layout: more generic alignment state (left/right/centered) for single items?
 - layout: clean up the InputFloatN/SliderFloatN/ColorEdit4 layout code. item width should include frame padding.
 - columns: separator function or parameter that works within the column (currently Separator() bypass all columns) (#125)
 - columns: declare column set (each column: fixed size, %, fill, distribute default size among fills) (#125)
 - columns: columns header to act as button (~sort op) and allow resize/reorder (#125)
 - columns: user specify columns size (#125)
 - popup: border options. richer api like BeginChild() perhaps? (#197)
 - combo: sparse combo boxes (via function call?)
 - combo: turn child handling code into pop up helper
 - combo: contents should extends to fit label if combo widget is small
 - combo/listbox: keyboard control. need inputtext like non-active focus + key handling. considering keybord for custom listbox (pr #203)
 - listbox: multiple selection
 - listbox: user may want to initial scroll to focus on the one selected value?
 - listbox: keyboard navigation.
 - listbox: scrolling should track modified selection.
 - menus: local shortcuts, global shortcuts (#126)
 - menus: icons
 - menus: menubars: some sort of priority / effect of main menu-bar on desktop size?
 - tabs
 - separator: separator on the initial position of a window is not visible (cursorpos.y <= clippos.y)
 - gauge: various forms of gauge/loading bars widgets
 - color: add a better color picker (perhaps a popup).
 - plot: plotlines should use the polygon-stroke facilities (currently issues with averaging normals)
 - plot: make it easier for user to draw extra stuff into the graph (e.g: draw basis, highlight certain points, 2d plots, multiple plots)
 - plot: "smooth" automatic scale over time, user give an input 0.0(full user scale) 1.0(full derived from value)
 - plot: add a helper e.g. Plot(char* label, float value, float time_span=2.0f) that stores values and Plot them for you - probably another function name. and/or automatically allow to plot ANY displayed value (more reliance on stable ID)
 - file selection widget -> build the tool in our codebase to improve model-dialog idioms
 - inputfloat: applying arithmetics ops (+,-,*,/) messes up with text edit undo stack.
 - slider: allow using the [-]/[+] buttons used by InputFloat()/InputInt()
 - slider: initial absolute click is imprecise. change to relative movement slider (same as scrollbar).
 - slider: add dragging-based widgets to edit values with mouse (on 2 axises), saving screen real-estate.
 - slider: tint background based on value (e.g. v_min -> v_max, or use 0.0f either side of the sign)
 - dragfloat: up/down axis
 - text edit: clean up the mess caused by converting UTF-8 <> wchar. the code is rather inefficient right now.
 - text edit: centered text for slider as input text so it matches typical positioning.
 - text edit: flag to disable live update of the user buffer. 
 - text edit: field resize behavior - field could stretch when being edited? hover tooltip shows more text?
 - tree: add treenode/treepush int variants? because (void*) cast from int warns on some platforms/settings
 - textwrapped: figure out better way to use TextWrapped() in an always auto-resize context (tooltip, etc.) (git issue #249)
 - settings: write more decent code to allow saving/loading new fields
 - settings: api for per-tool simple persistent data (bool,int,float,columns sizes,etc.) in .ini file
 - style: store rounded corners in texture to use 1 quad per corner (filled and wireframe). so rounding have minor cost.
 - style: checkbox: padding for "active" color should be a multiplier of the 
 - style: colorbox not always square?
 - text: simple markup language for color change?
 - log: LogButtons() options for specifying depth and/or hiding depth slider
 - log: have more control over the log scope (e.g. stop logging when leaving current tree node scope)
 - log: be able to log anything (e.g. right-click on a window/tree-node, shows context menu? log into tty/file/clipboard)
 - log: let user copy any window content to clipboard easily (CTRL+C on windows? while moving it? context menu?). code is commented because it fails with multiple Begin/End pairs.
 - filters: set a current filter that tree node can automatically query to hide themselves
 - filters: handle wildcards (with implicit leading/trailing *), regexps
 - shortcuts: add a shortcut api, e.g. parse "&Save" and/or "Save (CTRL+S)", pass in to widgets or provide simple ways to use (button=activate, input=focus)
!- keyboard: tooltip & combo boxes are messing up / not honoring keyboard tabbing
 - keyboard: full keyboard navigation and focus.
 - focus: SetKeyboardFocusHere() on with >= 0 offset could be done on same frame (else latch and modulate on beginning of next frame)
 - input: rework IO to be able to pass actual events to fix temporal aliasing issues.
 - input: support track pad style scrolling & slider edit.
 - portability: big-endian test/support (#81)
 - memory: add a way to discard allocs of unused/transient windows. with the current architecture new windows (including popup, opened combos, listbox) perform at least 3 allocs.
 - misc: mark printf compiler attributes on relevant functions
 - misc: provide a way to compile out the entire implementation while providing a dummy API (e.g. #define IMGUI_DUMMY_IMPL)
 - misc: double-clicking on title bar to minimize isn't consistent, perhaps move to single-click on left-most collapse icon?
 - style editor: have a more global HSV setter (e.g. alter hue on all elements). consider replacing active/hovered by offset in HSV space?
 - style editor: color child window height expressed in multiple of line height.
 - optimization/render: merge command-lists with same clip-rect into one even if they aren't sequential? (as long as in-between clip rectangle don't overlap)?
 - optimization: turn some the various stack vectors into statically-sized arrays
 - optimization: better clipping for multi-component widgets
*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#include <ctype.h>      // toupper, isprint
#include <math.h>       // sqrtf, fabsf, fmodf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>      // vsnprintf, sscanf, printf
#include <new>          // new (ptr)
#if defined(_MSC_VER) || defined(__MINGW32__) 
#include <malloc.h>     // alloca
#else
#include <alloca.h>     // alloca
#endif
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#define snprintf _snprintf
#endif

// Clang warnings with -Weverything
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning : format string is not a string literal              // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wexit-time-destructors"  // warning : declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. ImGui coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference it.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             // 
#pragma clang diagnostic ignored "-Wmissing-noreturn"       // warning : function xx could be declared with attribute 'noreturn' warning    // GetDefaultFontData() asserts which some implementation makes it never return.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"// warning : 'xx' is deprecated: The POSIX name for this item.. // for strdup used in demo code (so user can copy & paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast" // warning : cast to 'void *' from smaller integer type 'int'
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      // warning: cast to pointer from integer of different size
#endif

//-------------------------------------------------------------------------
// STB libraries implementation
//-------------------------------------------------------------------------

struct ImGuiTextEditState;

//#define IMGUI_STB_NAMESPACE     ImStb
//#define IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
//#define IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION

#ifdef IMGUI_STB_NAMESPACE
namespace IMGUI_STB_NAMESPACE
{
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

#define STBRP_ASSERT(x)    IM_ASSERT(x)
#ifndef IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STB_RECT_PACK_IMPLEMENTATION
#endif
#include "stb_rect_pack.h"

#define STBTT_malloc(x,u)  ((void)(u), ImGui::MemAlloc(x))
#define STBTT_free(x,u)    ((void)(u), ImGui::MemFree(x))
#define STBTT_assert(x)    IM_ASSERT(x)
#ifndef IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#include "stb_truetype.h"

#undef STB_TEXTEDIT_STRING
#undef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_STRING    ImGuiTextEditState
#define STB_TEXTEDIT_CHARTYPE  ImWchar
#define STB_TEXTEDIT_GETWIDTH_NEWLINE   -1.0f
#include "stb_textedit.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef IMGUI_STB_NAMESPACE
} // namespace ImStb
using namespace IMGUI_STB_NAMESPACE;
#endif

//-------------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------------

struct ImRect;
struct ImGuiColMod;
struct ImGuiStyleMod;
struct ImGuiDrawContext;
struct ImGuiTextEditState;
struct ImGuiIniData;
struct ImGuiState;
struct ImGuiWindow;
typedef int ImGuiLayoutType;      // enum ImGuiLayoutType_
typedef int ImGuiButtonFlags;     // enum ImGuiButtonFlags_

static bool         ButtonBehavior(const ImRect& bb, ImGuiID id, bool* out_hovered, bool* out_held, bool allow_key_modifiers, ImGuiButtonFlags flags = 0);
static void         LogText(const ImVec2& ref_pos, const char* text, const char* text_end = NULL);
static bool         InputTextEx(const char* label, char* buf, int buf_size, const ImVec2& size_arg, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback = NULL, void* user_data = NULL);

static void         RenderText(ImVec2 pos, const char* text, const char* text_end = NULL, bool hide_text_after_hash = true);
static void         RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width);
static void         RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, ImGuiAlign align = ImGuiAlign_Default, const ImVec2* clip_min = NULL, const ImVec2* clip_max = NULL);
static void         RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border = true, float rounding = 0.0f);
static void         RenderCollapseTriangle(ImVec2 p_min, bool opened, float scale = 1.0f, bool shadow = false);
static void         RenderCheckMark(ImVec2 pos, ImU32 col);

static void         SetFont(ImFont* font);
static bool         ItemAdd(const ImRect& bb, const ImGuiID* id);
static void         ItemSize(ImVec2 size, float text_offset_y = 0.0f);
static void         ItemSize(const ImRect& bb, float text_offset_y = 0.0f);
static void         PushColumnClipRect(int column_index = -1);
static bool         IsClippedEx(const ImRect& bb, const ImGuiID* id, bool clip_even_when_logged);

static bool         IsMouseHoveringRect(const ImRect& bb);
static bool         IsKeyPressedMap(ImGuiKey key, bool repeat = true);

static void         Scrollbar(ImGuiWindow* window);
static bool         CloseWindowButton(bool* p_opened = NULL);
static void         FocusWindow(ImGuiWindow* window);
static ImGuiWindow* FindHoveredWindow(ImVec2 pos, bool excluding_childs);
static void         CloseInactivePopups();
static ImGuiWindow* GetFrontMostModalRootWindow();
static void         SetWindowScrollY(ImGuiWindow* window, float scroll_y);

// Helpers: String
static int          ImStricmp(const char* str1, const char* str2);
static int          ImStrnicmp(const char* str1, const char* str2, int count);
static char*        ImStrdup(const char* str);
static int          ImStrlenW(const ImWchar* str);
static const ImWchar* ImStrbolW(const ImWchar* buf_mid_line, const ImWchar* buf_begin); // Find beginning-of-line
static const char*  ImStristr(const char* haystack, const char* needle, const char* needle_end);
static int          ImFormatString(char* buf, int buf_size, const char* fmt, ...);
static int          ImFormatStringV(char* buf, int buf_size, const char* fmt, va_list args);

// Helpers: Misc
static ImU32        ImHash(const void* data, int data_size, ImU32 seed);
static bool         ImLoadFileToMemory(const char* filename, const char* file_open_mode, void** out_file_data, int* out_file_size, int padding_bytes = 0);
static inline int   ImUpperPowerOfTwo(int v) { v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v; }
static inline bool  ImCharIsSpace(int c) { return c == ' ' || c == '\t' || c == 0x3000; }

// Helpers: UTF-8 <> wchar
static inline int   ImTextCharToUtf8(char* buf, int buf_size, unsigned int in_char);                                   // return output UTF-8 bytes count
static int          ImTextStrToUtf8(char* buf, int buf_size, const ImWchar* in_text, const ImWchar* in_text_end);      // return output UTF-8 bytes count
static int          ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end);          // return input UTF-8 bytes count
static int          ImTextStrFromUtf8(ImWchar* buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_remaining = NULL);   // return input UTF-8 bytes count
static int          ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end);                            // return number of UTF-8 code-points (NOT bytes count)
static inline int   ImTextCountUtf8BytesFromChar(unsigned int in_char);                                                // return output UTF-8 bytes count
static int          ImTextCountUtf8BytesFromStr(const ImWchar* in_text, const ImWchar* in_text_end);                   // return number of bytes to express string as UTF-8 code-points

//-----------------------------------------------------------------------------
// Platform dependent default implementations
//-----------------------------------------------------------------------------

static const char*  GetClipboardTextFn_DefaultImpl();
static void         SetClipboardTextFn_DefaultImpl(const char* text);
static void         ImeSetInputScreenPosFn_DefaultImpl(int x, int y);

//-----------------------------------------------------------------------------
// User facing structures
//-----------------------------------------------------------------------------

ImGuiStyle::ImGuiStyle()
{
    Alpha                   = 1.0f;             // Global alpha applies to everything in ImGui
    WindowPadding           = ImVec2(8,8);      // Padding within a window
    WindowMinSize           = ImVec2(32,32);    // Minimum window size
    WindowRounding          = 9.0f;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows
    WindowTitleAlign        = ImGuiAlign_Left;  // Alignment for title bar text
    ChildWindowRounding     = 0.0f;             // Radius of child window corners rounding. Set to 0.0f to have rectangular windows
    FramePadding            = ImVec2(4,3);      // Padding within a framed rectangle (used by most widgets)
    FrameRounding           = 0.0f;             // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
    ItemSpacing             = ImVec2(8,4);      // Horizontal and vertical spacing between widgets/lines
    ItemInnerSpacing        = ImVec2(4,4);      // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
    TouchExtraPadding       = ImVec2(0,0);      // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    WindowFillAlphaDefault  = 0.70f;            // Default alpha of window background, if not specified in ImGui::Begin()
    IndentSpacing           = 22.0f;            // Horizontal spacing when e.g. entering a tree node
    ColumnsMinSpacing       = 6.0f;             // Minimum horizontal spacing between two columns
    ScrollbarWidth          = 16.0f;            // Width of the vertical scrollbar
    ScrollbarRounding       = 0.0f;             // Radius of grab corners rounding for scrollbar
    GrabMinSize             = 10.0f;            // Minimum width/height of a grab box for slider/scrollbar
    GrabRounding            = 0.0f;             // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    DisplayWindowPadding    = ImVec2(22,22);    // Window positions are clamped to be visible within the display area by at least this amount. Only covers regular windows.
    DisplaySafeAreaPadding  = ImVec2(4,4);      // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
    AntiAliasedLines        = true;             // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
    AntiAliasedShapes       = true;             // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)

    Colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    Colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    Colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    Colors[ImGuiCol_ChildWindowBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    Colors[ImGuiCol_Border]                 = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
    Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    Colors[ImGuiCol_FrameBg]                = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);   // Background of checkbox, radio button, plot, slider, text input
    Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
    Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
    Colors[ImGuiCol_TitleBg]                = ImVec4(0.50f, 0.50f, 1.00f, 0.45f);
    Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.50f, 0.50f, 1.00f, 0.55f);
    Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.40f, 0.40f, 0.55f, 0.60f);
    Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.40f, 0.40f, 0.80f, 0.15f);
    Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
    Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
    Colors[ImGuiCol_ComboBg]                = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
    Colors[ImGuiCol_CheckMark]              = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    Colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    Colors[ImGuiCol_Button]                 = ImVec4(0.67f, 0.40f, 0.40f, 0.60f);
    Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.67f, 0.40f, 0.40f, 1.00f);
    Colors[ImGuiCol_ButtonActive]           = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    Colors[ImGuiCol_Header]                 = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    Colors[ImGuiCol_HeaderActive]           = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    Colors[ImGuiCol_Column]                 = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    Colors[ImGuiCol_ColumnHovered]          = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
    Colors[ImGuiCol_ColumnActive]           = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
    Colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    Colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    Colors[ImGuiCol_CloseButton]            = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
    Colors[ImGuiCol_CloseButtonHovered]     = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
    Colors[ImGuiCol_CloseButtonActive]      = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    Colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    Colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    Colors[ImGuiCol_TooltipBg]              = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
    Colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

// Statically allocated font atlas. This is merely a maneuver to keep ImFontAtlas definition at the bottom of the .h file (otherwise it'd be inside ImGuiIO)
// Also we wouldn't be able to new() one at this point, before users may define IO.MemAllocFn.
static ImFontAtlas GDefaultFontAtlas;

ImGuiIO::ImGuiIO()
{
    // Most fields are initialized with zero
    memset(this, 0, sizeof(*this));

    DisplaySize = ImVec2(-1.0f, -1.0f);
    DeltaTime = 1.0f/60.0f;
    IniSavingRate = 5.0f;
    IniFilename = "imgui.ini";
    LogFilename = "imgui_log.txt";
    Fonts = &GDefaultFontAtlas;
    FontGlobalScale = 1.0f;
    MousePos = ImVec2(-1,-1);
    MousePosPrev = ImVec2(-1,-1);
    MouseDoubleClickTime = 0.30f;
    MouseDoubleClickMaxDist = 6.0f;
    MouseDragThreshold = 6.0f;
    for (int i = 0; i < ImGuiKey_COUNT; i++)
        KeyMap[i] = -1;
    KeyRepeatDelay = 0.250f;
    KeyRepeatRate = 0.050f;
    UserData = NULL;

    // User functions
    RenderDrawListsFn = NULL;
    MemAllocFn = malloc;
    MemFreeFn = free;
    GetClipboardTextFn = GetClipboardTextFn_DefaultImpl;   // Platform dependent default implementations
    SetClipboardTextFn = SetClipboardTextFn_DefaultImpl;
    ImeSetInputScreenPosFn = ImeSetInputScreenPosFn_DefaultImpl;
}

//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------

#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))

const float IM_PI = 3.14159265358979323846f;

#ifdef INT_MAX
#define IM_INT_MIN  INT_MIN
#define IM_INT_MAX  INT_MAX
#else
#define IM_INT_MIN  (-2147483647-1)
#define IM_INT_MAX  (2147483647)
#endif

// Play it nice with Windows users. Notepad in 2015 still doesn't display text data with Unix-style \n.
#ifdef _MSC_VER
#define IM_NEWLINE "\r\n"
#else
#define IM_NEWLINE "\n"
#endif

// Pass in translated ASCII characters for text input.
// - with glfw you can get those from the callback set in glfwSetCharCallback()
// - on Windows you can get those using ToAscii+keyboard state, or via the WM_CHAR message
void ImGuiIO::AddInputCharacter(ImWchar c)
{
    const int n = ImStrlenW(InputCharacters);
    if (n + 1 < IM_ARRAYSIZE(InputCharacters))
    {
        InputCharacters[n] = c;
        InputCharacters[n+1] = '\0';
    }
}

void ImGuiIO::AddInputCharactersUTF8(const char* utf8_chars)
{
    // We can't pass more wchars than ImGuiIO::InputCharacters[] can hold so don't convert more
    const int wchars_buf_len = sizeof(ImGuiIO::InputCharacters) / sizeof(ImWchar);
    ImWchar wchars[wchars_buf_len];
    ImTextStrFromUtf8(wchars, wchars_buf_len, utf8_chars, NULL);
    for (int i = 0; wchars[i] != 0 && i < wchars_buf_len; i++)
        AddInputCharacter(wchars[i]);
}

// Math bits
// We are keeping those static in the .cpp file so as not to leak them outside, in the case the user has implicit cast operators between ImVec2 and its own types.
static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)              { return ImVec2(lhs.x*rhs, lhs.y*rhs); }
//static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)            { return ImVec2(lhs.x/rhs, lhs.y/rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x*rhs.x, lhs.y*rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x/rhs.x, lhs.y/rhs.y); }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
//static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)              { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)                  { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
//static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)                { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z, lhs.w-lhs.w); }

static inline int    ImMin(int lhs, int rhs)                                    { return lhs < rhs ? lhs : rhs; }
static inline int    ImMax(int lhs, int rhs)                                    { return lhs >= rhs ? lhs : rhs; }
static inline float  ImMin(float lhs, float rhs)                                { return lhs < rhs ? lhs : rhs; }
static inline float  ImMax(float lhs, float rhs)                                { return lhs >= rhs ? lhs : rhs; }
static inline ImVec2 ImMin(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(ImMin(lhs.x,rhs.x), ImMin(lhs.y,rhs.y)); }
static inline ImVec2 ImMax(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(ImMax(lhs.x,rhs.x), ImMax(lhs.y,rhs.y)); }
static inline int    ImClamp(int v, int mn, int mx)                             { return (v < mn) ? mn : (v > mx) ? mx : v; }
static inline float  ImClamp(float v, float mn, float mx)                       { return (v < mn) ? mn : (v > mx) ? mx : v; }
static inline ImVec2 ImClamp(const ImVec2& f, const ImVec2& mn, ImVec2 mx)      { return ImVec2(ImClamp(f.x,mn.x,mx.x), ImClamp(f.y,mn.y,mx.y)); }
static inline float  ImSaturate(float f)                                        { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
static inline float  ImLerp(float a, float b, float t)                          { return a + (b - a) * t; }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, const ImVec2& t)  { return ImVec2(a.x + (b.x - a.x) * t.x, a.y + (b.y - a.y) * t.y); }
static inline float  ImLengthSqr(const ImVec2& lhs)                             { return lhs.x*lhs.x + lhs.y*lhs.y; }
static inline float  ImLengthSqr(const ImVec4& lhs)                             { return lhs.x*lhs.x + lhs.y*lhs.y + lhs.z*lhs.z + lhs.w*lhs.w; }
static inline float  ImInvLength(const ImVec2& lhs, float fail_value)           { float d = lhs.x*lhs.x + lhs.y*lhs.y; if (d > 0.0f) return 1.0f / sqrtf(d); return fail_value; }

static bool ImIsPointInTriangle(const ImVec2& p, const ImVec2& a, const ImVec2& b, const ImVec2& c)
{
    bool b1 = ((p.x - b.x) * (a.y - b.y) - (p.y - b.y) * (a.x - b.x)) < 0.0f;
    bool b2 = ((p.x - c.x) * (b.y - c.y) - (p.y - c.y) * (b.x - c.x)) < 0.0f;
    bool b3 = ((p.x - a.x) * (c.y - a.y) - (p.y - a.y) * (c.x - a.x)) < 0.0f;
    return ((b1 == b2) && (b2 == b3));
}

static int ImStricmp(const char* str1, const char* str2)
{
    int d;
    while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; }
    return d;
}

static int ImStrnicmp(const char* str1, const char* str2, int count)
{
    int d = 0;
    while (count > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; count--; }
    return d;
}

static char* ImStrdup(const char *str)
{
    char *buff = (char*)ImGui::MemAlloc(strlen(str) + 1);
    IM_ASSERT(buff);
    strcpy(buff, str);
    return buff;
}

static int ImStrlenW(const ImWchar* str)
{
    int n = 0;
    while (*str++) n++;
    return n;
}

static const ImWchar* ImStrbolW(const ImWchar* buf_mid_line, const ImWchar* buf_begin) // find beginning-of-line
{
    while (buf_mid_line > buf_begin && buf_mid_line[-1] != '\n')
        buf_mid_line--;
    return buf_mid_line;
}

static const char* ImStristr(const char* haystack, const char* needle, const char* needle_end)
{
    if (!needle_end)
        needle_end = needle + strlen(needle);

    const char un0 = (char)toupper(*needle);
    while (*haystack)
    {
        if (toupper(*haystack) == un0)
        {
            const char* b = needle + 1;
            for (const char* a = haystack + 1; b < needle_end; a++, b++)
                if (toupper(*a) != toupper(*b))
                    break;
            if (b == needle_end)
                return haystack;
        }
        haystack++;
    }
    return NULL;
}

// Pass data_size==0 for zero-terminated string
// Try to replace with FNV1a hash?
static ImU32 ImHash(const void* data, int data_size, ImU32 seed = 0) 
{ 
    static ImU32 crc32_lut[256] = { 0 };
    if (!crc32_lut[1])
    {
        const ImU32 polynomial = 0xEDB88320;
        for (ImU32 i = 0; i < 256; i++) 
        { 
            ImU32 crc = i; 
            for (ImU32 j = 0; j < 8; j++) 
                crc = (crc >> 1) ^ (ImU32(-int(crc & 1)) & polynomial); 
            crc32_lut[i] = crc; 
        }
    }

    seed = ~seed;
    ImU32 crc = seed; 
    const unsigned char* current = (const unsigned char*)data;

    if (data_size > 0)
    {
        // Known size
        while (data_size--) 
            crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ *current++]; 
    }
    else
    {
        // Zero-terminated string
        while (unsigned char c = *current++)
        {
            // We support a syntax of "label###id" where only "###id" is included in the hash, and only "label" gets displayed.
            // Because this syntax is rarely used we are optimizing for the common case.
            // - If we reach ### in the string we discard the hash so far and reset to the seed. 
            // - We don't do 'current += 2; continue;' after handling ### to keep the code smaller.
            if (c == '#' && current[0] == '#' && current[1] == '#')
                crc = seed;

            crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
        }
    }
    return ~crc; 
} 

static int ImFormatString(char* buf, int buf_size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int w = vsnprintf(buf, buf_size, fmt, args);
    va_end(args);
    buf[buf_size-1] = 0;
    return (w == -1) ? buf_size : w;
}

static int ImFormatStringV(char* buf, int buf_size, const char* fmt, va_list args)
{
    int w = vsnprintf(buf, buf_size, fmt, args);
    buf[buf_size-1] = 0;
    return (w == -1) ? buf_size : w;
}

ImU32 ImGui::ColorConvertFloat4ToU32(const ImVec4& in)
{
    ImU32 out  = ((ImU32)(ImSaturate(in.x)*255.f));
    out |= ((ImU32)(ImSaturate(in.y)*255.f) << 8);
    out |= ((ImU32)(ImSaturate(in.z)*255.f) << 16);
    out |= ((ImU32)(ImSaturate(in.w)*255.f) << 24);
    return out;
}

// Convert rgb floats ([0-1],[0-1],[0-1]) to hsv floats ([0-1],[0-1],[0-1]), from Foley & van Dam p592
// Optimized http://lolengine.net/blog/2013/01/13/fast-rgb-to-hsv
void ImGui::ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v)
{
    float K = 0.f;
    if (g < b)
    {
        const float tmp = g; g = b; b = tmp;
        K = -1.f;
    }
    if (r < g)
    {
        const float tmp = r; r = g; g = tmp;
        K = -2.f / 6.f - K;
    }

    const float chroma = r - (g < b ? g : b);
    out_h = fabsf(K + (g - b) / (6.f * chroma + 1e-20f));
    out_s = chroma / (r + 1e-20f);
    out_v = r;
}

// Convert hsv floats ([0-1],[0-1],[0-1]) to rgb floats ([0-1],[0-1],[0-1]), from Foley & van Dam p593
// also http://en.wikipedia.org/wiki/HSL_and_HSV
void ImGui::ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
{   
    if (s == 0.0f)
    {
        // gray
        out_r = out_g = out_b = v;
        return;
    }

    h = fmodf(h, 1.0f) / (60.0f/360.0f);
    int   i = (int)h;
    float f = h - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i)
    {
    case 0: out_r = v; out_g = t; out_b = p; break;
    case 1: out_r = q; out_g = v; out_b = p; break;
    case 2: out_r = p; out_g = v; out_b = t; break;
    case 3: out_r = p; out_g = q; out_b = v; break;
    case 4: out_r = t; out_g = p; out_b = v; break;
    case 5: default: out_r = v; out_g = p; out_b = q; break;
    }
}

// Load file content into memory
// Memory allocated with ImGui::MemAlloc(), must be freed by user using ImGui::MemFree()
static bool ImLoadFileToMemory(const char* filename, const char* file_open_mode, void** out_file_data, int* out_file_size, int padding_bytes)
{
    IM_ASSERT(filename && file_open_mode && out_file_data && out_file_size);
    *out_file_data = NULL;
    *out_file_size = 0;

    FILE* f;
    if ((f = fopen(filename, file_open_mode)) == NULL)
        return false;

    long file_size_signed;
    if (fseek(f, 0, SEEK_END) || (file_size_signed = ftell(f)) == -1 || fseek(f, 0, SEEK_SET))
    {
        fclose(f);
        return false;
    }

    int file_size = (int)file_size_signed;
    void* file_data = ImGui::MemAlloc(file_size + padding_bytes);
    if (file_data == NULL)
    {
        fclose(f);
        return false;
    }
    if (fread(file_data, 1, (size_t)file_size, f) != (size_t)file_size)
    {
        fclose(f);
        ImGui::MemFree(file_data);
        return false;
    }
    if (padding_bytes > 0)
        memset((void *)(((char*)file_data) + file_size), 0, padding_bytes);

    fclose(f);
    *out_file_data = file_data;
    *out_file_size = file_size;

    return true;
}

//-----------------------------------------------------------------------------

enum ImGuiLayoutType_
{
    ImGuiLayoutType_Vertical,
    ImGuiLayoutType_Horizontal          // FIXME: this is in development, not exposed/functional as a generic feature yet.
};

enum ImGuiButtonFlags_
{
    ImGuiButtonFlags_Repeat             = 1 << 0,
    ImGuiButtonFlags_PressedOnClick     = 1 << 1,   // return pressed on click only (default requires click+release)
    ImGuiButtonFlags_PressedOnRelease   = 1 << 2,   // return pressed on release only (default requires click+release)
    ImGuiButtonFlags_FlattenChilds      = 1 << 3,
    ImGuiButtonFlags_DontClosePopups    = 1 << 4,
    ImGuiButtonFlags_Disabled           = 1 << 5,
    ImGuiButtonFlags_AlignTextBaseLine  = 1 << 6
};

enum ImGuiSelectableFlagsPrivate_
{
    // NB: need to be in sync with last value of ImGuiSelectableFlags_
    ImGuiSelectableFlags_Menu               = 1 << 2,
    ImGuiSelectableFlags_MenuItem           = 1 << 3,
    ImGuiSelectableFlags_Disabled           = 1 << 4,
    ImGuiSelectableFlags_DrawFillAvailWidth = 1 << 5
};

struct ImGuiColMod       // Color modifier, backup of modified data so we can restore it
{
    ImGuiCol    Col;
    ImVec4      PreviousValue;
};

struct ImGuiStyleMod    // Style modifier, backup of modified data so we can restore it
{
    ImGuiStyleVar Var;
    ImVec2      PreviousValue;
};

struct ImRect           // 2D axis aligned bounding-box
{
    ImVec2      Min;
    ImVec2      Max;

    ImRect()                                        : Min(FLT_MAX,FLT_MAX), Max(-FLT_MAX,-FLT_MAX)  {}
    ImRect(const ImVec2& min, const ImVec2& max)    : Min(min), Max(max)                            {}
    ImRect(const ImVec4& v)                         : Min(v.x, v.y), Max(v.z, v.w)                  {}
    ImRect(float x1, float y1, float x2, float y2)  : Min(x1, y1), Max(x2, y2)                      {}

    ImVec2      GetCenter() const                   { return ImVec2((Min.x+Max.x)*0.5f, (Min.y+Max.y)*0.5f); }
    ImVec2      GetSize() const                     { return ImVec2(Max.x-Min.x,Max.y-Min.y); }
    float       GetWidth() const                    { return Max.x-Min.x; }
    float       GetHeight() const                   { return Max.y-Min.y; }
    ImVec2      GetTL() const                       { return Min; }
    ImVec2      GetTR() const                       { return ImVec2(Max.x,Min.y); }
    ImVec2      GetBL() const                       { return ImVec2(Min.x,Max.y); }
    ImVec2      GetBR() const                       { return Max; }
    bool        Contains(const ImVec2& p) const     { return p.x >= Min.x && p.y >= Min.y && p.x < Max.x && p.y < Max.y; }
    bool        Contains(const ImRect& r) const     { return r.Min.x >= Min.x && r.Min.y >= Min.y && r.Max.x < Max.x && r.Max.y < Max.y; }
    bool        Overlaps(const ImRect& r) const     { return r.Min.y < Max.y && r.Max.y > Min.y && r.Min.x < Max.x && r.Max.x > Min.x; }
    void        Add(const ImVec2& rhs)              { Min.x = ImMin(Min.x, rhs.x); Min.y = ImMin(Min.y, rhs.y); Max.x = ImMax(Max.x, rhs.x); Max.y = ImMax(Max.y, rhs.y); }
    void        Add(const ImRect& rhs)              { Min.x = ImMin(Min.x, rhs.Min.x); Min.y = ImMin(Min.y, rhs.Min.y); Max.x = ImMax(Max.x, rhs.Max.x); Max.y = ImMax(Max.y, rhs.Max.y); }
    void        Expand(const float amount)          { Min.x -= amount; Min.y -= amount; Max.x += amount; Max.y += amount; }
    void        Expand(const ImVec2& amount)        { Min.x -= amount.x; Min.y -= amount.y; Max.x += amount.x; Max.y += amount.y; }
    void        Reduce(const ImVec2& amount)        { Min.x += amount.x; Min.y += amount.y; Max.x -= amount.x; Max.y -= amount.y; }
    void        Clip(const ImRect& clip)            { Min.x = ImMax(Min.x, clip.Min.x); Min.y = ImMax(Min.y, clip.Min.y); Max.x = ImMin(Max.x, clip.Max.x); Max.y = ImMin(Max.y, clip.Max.y); }
    void        Round()                             { Min.x = (float)(int)Min.x; Min.y = (float)(int)Min.y; Max.x = (float)(int)Max.x; Max.y = (float)(int)Max.y; } 
    ImVec2      GetClosestPoint(ImVec2 p, bool on_edge) const
    {
        if (!on_edge && Contains(p))
            return p;
        if (p.x > Max.x) p.x = Max.x;
        else if (p.x < Min.x) p.x = Min.x;
        if (p.y > Max.y) p.y = Max.y;
        else if (p.y < Min.y) p.y = Min.y;
        return p;
    }
};

struct ImGuiGroupData
{
    ImVec2 BackupCursorPos;
    ImVec2 BackupCursorMaxPos;
    float  BackupColumnsStartX;
    float  BackupCurrentLineHeight;
    float  BackupCurrentLineTextBaseOffset;
    float  BackupLogLinePosY;
    bool   AdvanceCursor;
};

// Simple column measurement currently used for menu items. This is very short-sighted for now.
struct ImGuiSimpleColumns
{
    int    Count;
    float  Spacing;
    float  Width, NextWidth;
    float  Pos[8], NextWidths[8];

    ImGuiSimpleColumns() { Count = 0; Spacing = 0.0f; Width = 0.0f; NextWidth = 0.0f; memset(Pos, 0, sizeof(Pos)); memset(NextWidths, 0, sizeof(NextWidths)); }
    void Update(int count, float spacing, bool clear)
    {
        IM_ASSERT(Count <= IM_ARRAYSIZE(Pos));
        Count = count;
        Width = NextWidth = 0.0f;
        Spacing = spacing;
        if (clear) memset(NextWidths, 0, sizeof(NextWidths));
        for (int i = 0; i < Count; i++)
        {
            if (i > 0 && NextWidths[i] > 0.0f)
                Width += Spacing;
            Pos[i] = (float)(int)Width;
            Width += NextWidths[i];
            NextWidths[i] = 0.0f;
        }
    }
    float DeclColumns(float w0, float w1, float w2) // not using va_arg because they promote float to double
    {
        NextWidth = 0.0f;
        NextWidths[0] = ImMax(NextWidths[0], w0);
        NextWidths[1] = ImMax(NextWidths[1], w1);
        NextWidths[2] = ImMax(NextWidths[2], w2);
        for (int i = 0; i < 3; i++)
            NextWidth += NextWidths[i] + ((i > 0 && NextWidths[i] > 0.0f) ? Spacing : 0.0f);
        return ImMax(Width, NextWidth);
    }
    float CalcExtraSpace(float avail_w)
    {
        return ImMax(0.0f, avail_w - Width);
    }
};

// Temporary per-window data, reset at the beginning of the frame
struct ImGuiDrawContext
{
    ImVec2                  CursorPos;
    ImVec2                  CursorPosPrevLine;
    ImVec2                  CursorStartPos;
    ImVec2                  CursorMaxPos;           // Implicitly calculate the size of our contents, always extending. Saved into window->SizeContents at the end of the frame
    float                   CurrentLineHeight;
    float                   CurrentLineTextBaseOffset;
    float                   PrevLineHeight;
    float                   PrevLineTextBaseOffset;
    float                   LogLinePosY;
    int                     TreeDepth;
    ImGuiID                 LastItemID;
    ImRect                  LastItemRect;
    bool                    LastItemHoveredAndUsable;
    bool                    LastItemHoveredRect;
    bool                    MenuBarAppending;
    float                   MenuBarOffsetX;
    ImVector<ImGuiWindow*>  ChildWindows;
    ImGuiStorage*           StateStorage;
    ImGuiLayoutType         LayoutType;

    // We store the current settings outside of the vectors to increase memory locality (reduce cache misses). The vectors are rarely modified. Also it allows us to not heap allocate for short-lived windows which are not using those settings.
    bool                    ButtonRepeat;           // == ButtonRepeatStack.back() [empty == false]
    bool                    AllowKeyboardFocus;     // == AllowKeyboardFocusStack.back() [empty == true]
    float                   ItemWidth;              // == ItemWidthStack.back(). 0.0: default, >0.0: width in pixels, <0.0: align xx pixels to the right of window
    float                   TextWrapPos;            // == TextWrapPosStack.back() [empty == -1.0f]
    ImVector<bool>          ButtonRepeatStack;
    ImVector<bool>          AllowKeyboardFocusStack;
    ImVector<float>         ItemWidthStack;
    ImVector<float>         TextWrapPosStack;
    ImVector<ImGuiGroupData>GroupStack;
    ImGuiColorEditMode      ColorEditMode;
    int                     StackSizesBackup[6];    // Store size of various stacks for asserting

    float                   ColumnsStartX;          // Indentation / start position from left of window (increased by TreePush/TreePop, etc.)
    float                   ColumnsOffsetX;         // Offset to the current column (if ColumnsCurrent > 0). FIXME: This and the above should be a stack to allow use cases like Tree->Column->Tree. Need revamp columns API.
    int                     ColumnsCurrent;
    int                     ColumnsCount;
    ImVec2                  ColumnsStartPos;
    float                   ColumnsCellMinY;
    float                   ColumnsCellMaxY;
    bool                    ColumnsShowBorders;
    ImGuiID                 ColumnsSetID;
    ImVector<float>         ColumnsOffsetsT;        // Columns offset normalized 0.0 (far left) -> 1.0 (far right)

    ImGuiDrawContext()
    {
        CursorPos = CursorPosPrevLine = CursorStartPos = CursorMaxPos = ImVec2(0.0f, 0.0f);
        CurrentLineHeight = PrevLineHeight = 0.0f;
        CurrentLineTextBaseOffset = PrevLineTextBaseOffset = 0.0f;
        LogLinePosY = -1.0f;
        TreeDepth = 0;
        LastItemID = 0;
        LastItemRect = ImRect(0.0f,0.0f,0.0f,0.0f);
        LastItemHoveredAndUsable = LastItemHoveredRect = false;
        MenuBarAppending = false;
        MenuBarOffsetX = 0.0f;
        StateStorage = NULL;
        LayoutType = ImGuiLayoutType_Vertical;
        ItemWidth = 0.0f;
        ButtonRepeat = false;
        AllowKeyboardFocus = true;
        TextWrapPos = -1.0f;
        ColorEditMode = ImGuiColorEditMode_RGB;
        memset(StackSizesBackup, 0, sizeof(StackSizesBackup));

        ColumnsStartX = 0.0f;
        ColumnsOffsetX = 0.0f;
        ColumnsCurrent = 0;
        ColumnsCount = 1;
        ColumnsStartPos = ImVec2(0.0f, 0.0f);
        ColumnsCellMinY = ColumnsCellMaxY = 0.0f;
        ColumnsShowBorders = true;
        ColumnsSetID = 0;
    }
};

// Internal state of the currently focused/edited text input box
struct ImGuiTextEditState
{
    ImGuiID             Id;                             // widget id owning the text state
    ImVector<ImWchar>   Text;                           // edit buffer, we need to persist but can't guarantee the persistence of the user-provided buffer. so we copy into own buffer.
    ImVector<char>      InitialText;                    // backup of end-user buffer at the time of focus (in UTF-8, unaltered)
    ImVector<char>      TempTextBuffer;
    int                 CurLenA, CurLenW;               // we need to maintain our buffer length in both UTF-8 and wchar format.
    int                 BufSizeA;                       // end-user buffer size
    float               ScrollX;
    STB_TexteditState   StbState;
    float               CursorAnim;
    bool                CursorFollow;
    ImVec2              InputCursorScreenPos;           // Cursor position in screen space to be used by IME callback.
    bool                SelectedAllMouseLock;

    ImGuiTextEditState()                                { memset(this, 0, sizeof(*this)); }
    void                CursorAnimReset()               { CursorAnim = -0.30f; }                                                // After a user-input the cursor stays on for a while without blinking
    bool                CursorIsVisible() const         { return CursorAnim <= 0.0f || fmodf(CursorAnim, 1.20f) <= 0.80f; }     // Blinking
    bool                HasSelection() const            { return StbState.select_start != StbState.select_end; }
    void                ClearSelection()                { StbState.select_start = StbState.select_end = StbState.cursor; }
    void                SelectAll()                     { StbState.select_start = 0; StbState.select_end = CurLenW; StbState.cursor = StbState.select_end; StbState.has_preferred_x = false; }
    void                OnKeyPressed(int key);
};

// Data saved in imgui.ini file
struct ImGuiIniData
{
    char*   Name;
    ImGuiID ID;
    ImVec2  Pos;
    ImVec2  Size;
    bool    Collapsed;
};

struct ImGuiMouseCursorData
{
    ImGuiMouseCursor    Type;
    ImVec2              Offset;
    ImVec2              Size;
    ImVec2              TexUvMin[2];
    ImVec2              TexUvMax[2];
};

struct ImGuiPopupRef
{
    ImGuiID             PopupID;        // Set on OpenPopup()
    ImGuiWindow*        Window;         // Resolved on BeginPopup() - may stay unresolved if user never calls OpenPopup()
    ImGuiWindow*        ParentWindow;   // Set on OpenPopup()
    ImGuiID             ParentMenuSet;  // Set on OpenPopup()

    ImGuiPopupRef(ImGuiID id, ImGuiWindow* parent_window, ImGuiID parent_menu_set) { PopupID = id; Window = NULL; ParentWindow = parent_window; ParentMenuSet = parent_menu_set; }
};

// Main state for ImGui
struct ImGuiState
{
    bool                    Initialized;
    ImGuiIO                 IO;
    ImGuiStyle              Style;
    ImFont*                 Font;                               // (Shortcut) == FontStack.empty() ? IO.Font : FontStack.back()
    float                   FontSize;                           // (Shortcut) == FontBaseSize * g.CurrentWindow->FontWindowScale == window->FontSize()
    float                   FontBaseSize;                       // (Shortcut) == IO.FontGlobalScale * Font->Scale * Font->FontSize. Size of characters.
    ImVec2                  FontTexUvWhitePixel;                // (Shortcut) == Font->TexUvForWhite

    float                   Time;
    int                     FrameCount;
    int                     FrameCountRendered;
    ImVector<ImGuiWindow*>  Windows;
    ImVector<ImGuiWindow*>  WindowsSortBuffer;
    ImGuiWindow*            CurrentWindow;                      // Being drawn into
    ImVector<ImGuiWindow*>  CurrentWindowStack;
    ImGuiWindow*            FocusedWindow;                      // Will catch keyboard inputs
    ImGuiWindow*            HoveredWindow;                      // Will catch mouse inputs
    ImGuiWindow*            HoveredRootWindow;                  // Will catch mouse inputs (for focus/move only)
    ImGuiID                 HoveredId;                          // Hovered widget
    ImGuiID                 HoveredIdPreviousFrame;
    ImGuiID                 ActiveId;                           // Active widget
    ImGuiID                 ActiveIdPreviousFrame;
    bool                    ActiveIdIsAlive;
    bool                    ActiveIdIsJustActivated;            // Set at the time of activation for one frame
    bool                    ActiveIdIsFocusedOnly;              // Set only by active widget. Denote focus but no active interaction
    ImGuiWindow*            ActiveIdWindow;
    ImGuiWindow*            MovedWindow;                        // Track the child window we clicked on to move a window. Pointer is only valid if ActiveID is the "#MOVE" identifier of a window.
    float                   SettingsDirtyTimer;
    ImVector<ImGuiIniData>  Settings;
    int                     DisableHideTextAfterDoubleHash;
    ImVector<ImGuiColMod>   ColorModifiers;
    ImVector<ImGuiStyleMod> StyleModifiers;
    ImVector<ImFont*>       FontStack;
    ImVector<ImGuiPopupRef> OpenedPopupStack;                   // Which popups are open
    ImVector<ImGuiPopupRef> CurrentPopupStack;                  // Which level of BeginPopup() we are in (reset every frame)

    ImVec2                  SetNextWindowPosVal;
    ImGuiSetCond            SetNextWindowPosCond;
    ImVec2                  SetNextWindowSizeVal;
    ImGuiSetCond            SetNextWindowSizeCond;
    bool                    SetNextWindowCollapsedVal;
    ImGuiSetCond            SetNextWindowCollapsedCond;
    bool                    SetNextWindowFocus;
    bool                    SetNextTreeNodeOpenedVal;
    ImGuiSetCond            SetNextTreeNodeOpenedCond;

    // Render
    ImVector<ImDrawList*>   RenderDrawLists[3];
    float                   ModalWindowDarkeningRatio;
    ImDrawList              OverlayDrawList;                    // Optional software render of mouse cursors, if io.MouseDrawCursor is set + a few debug overlays
    ImGuiMouseCursor        MouseCursor;
    ImGuiMouseCursorData    MouseCursorData[ImGuiMouseCursor_Count_];

    // Widget state
    ImGuiTextEditState      InputTextState;
    ImGuiID                 ScalarAsInputTextId;                // Temporary text input when CTRL+clicking on a slider, etc.
    ImGuiStorage            ColorEditModeStorage;               // Store user selection of color edit mode
    ImVec2                  ActiveClickDeltaToCenter;
    float                   DragCurrentValue;                   // current dragged value, always float, not rounded by end-user precision settings
    ImVec2                  DragLastMouseDelta;
    float                   DragSpeedDefaultRatio;              // if speed == 0.0f, uses (max-min) * DragSpeedDefaultRatio
    float                   DragSpeedScaleSlow;
    float                   DragSpeedScaleFast;
    float                   ScrollbarClickDeltaToGrabCenter;    // distance between mouse and center of grab box, normalized in parent space
    char                    Tooltip[1024];
    char*                   PrivateClipboard;                   // if no custom clipboard handler is defined

    // Logging
    bool                    LogEnabled;
    FILE*                   LogFile;
    ImGuiTextBuffer*        LogClipboard;                       // pointer so our GImGui static constructor doesn't call heap allocators.
    int                     LogStartDepth;
    int                     LogAutoExpandMaxDepth;

    // Misc
    float                   FramerateSecPerFrame[120];          // calculate estimate of framerate for user
    int                     FramerateSecPerFrameIdx;
    float                   FramerateSecPerFrameAccum;
    bool                    CaptureMouseNextFrame;              // explicit capture via CaptureInputs() sets those flags
    bool                    CaptureKeyboardNextFrame;
    char                    TempBuffer[1024*3+1];               // temporary text buffer

    ImGuiState()
    {
        Initialized = false;
        Font = NULL;
        FontSize = FontBaseSize = 0.0f;
        FontTexUvWhitePixel = ImVec2(0.0f, 0.0f);

        Time = 0.0f;
        FrameCount = 0;
        FrameCountRendered = -1;
        CurrentWindow = NULL;
        FocusedWindow = NULL;
        HoveredWindow = NULL;
        HoveredRootWindow = NULL;
        HoveredId = 0;
        HoveredIdPreviousFrame = 0;
        ActiveId = 0;
        ActiveIdPreviousFrame = 0;
        ActiveIdIsAlive = false;
        ActiveIdIsJustActivated = false;
        ActiveIdIsFocusedOnly = false;
        ActiveIdWindow = NULL;
        MovedWindow = NULL;
        SettingsDirtyTimer = 0.0f;
        DisableHideTextAfterDoubleHash = 0;

        SetNextWindowPosVal = ImVec2(0.0f, 0.0f);
        SetNextWindowPosCond = 0;
        SetNextWindowSizeVal = ImVec2(0.0f, 0.0f);
        SetNextWindowSizeCond = 0;
        SetNextWindowCollapsedVal = false;
        SetNextWindowCollapsedCond = 0;
        SetNextWindowFocus = false;
        SetNextTreeNodeOpenedVal = false;
        SetNextTreeNodeOpenedCond = 0;

        ScalarAsInputTextId = 0;
        ActiveClickDeltaToCenter = ImVec2(0.0f, 0.0f);
        DragCurrentValue = 0.0f;
        DragLastMouseDelta = ImVec2(0.0f, 0.0f);
        DragSpeedDefaultRatio = 0.01f;
        DragSpeedScaleSlow = 0.01f;
        DragSpeedScaleFast = 10.0f;
        ScrollbarClickDeltaToGrabCenter = 0.0f;
        memset(Tooltip, 0, sizeof(Tooltip));
        PrivateClipboard = NULL;

        ModalWindowDarkeningRatio = 0.0f;
        OverlayDrawList._OwnerName = "##Overlay"; // Give it a name for debugging
        MouseCursor = ImGuiMouseCursor_Arrow;

        LogEnabled = false;
        LogFile = NULL;
        LogClipboard = NULL;
        LogStartDepth = 0;
        LogAutoExpandMaxDepth = 2;

        memset(FramerateSecPerFrame, 0, sizeof(FramerateSecPerFrame));
        FramerateSecPerFrameIdx = 0;
        FramerateSecPerFrameAccum = 0.0f;
        CaptureMouseNextFrame = CaptureKeyboardNextFrame = false;
    }
};

static ImGuiState   GImDefaultState;                            // Internal state storage
static ImGuiState*  GImGui = &GImDefaultState;                  // We access everything through this pointer. NB: this pointer is always assumed to be != NULL

struct ImGuiWindow
{
    char*                   Name;
    ImGuiID                 ID;
    ImGuiWindowFlags        Flags;
    ImVec2                  PosFloat;
    ImVec2                  Pos;                                // Position rounded-up to nearest pixel
    ImVec2                  Size;                               // Current size (==SizeFull or collapsed title bar size)
    ImVec2                  SizeFull;                           // Size when non collapsed
    ImVec2                  SizeContents;                       // Size of contents (== extents reach of the drawing cursor) from previous frame
    ImVec2                  WindowPadding;                      // Window padding at the time of begin. We need to lock it, in particular manipulation of the ShowBorder would have an effect
    ImGuiID                 MoveID;                             // == window->GetID("#MOVE")
    float                   ScrollY;
    float                   ScrollTargetRelY;                   // target scroll position. stored as cursor position with scrolling canceled out, so the highest point is always 0.0f. (FLT_MAX for no change)
    float                   ScrollTargetCenterRatioY;           // 0.0f = scroll so that target position is at top, 0.5f = scroll so that target position is centered
    bool                    ScrollbarY;
    bool                    Active;                             // Set to true on Begin()
    bool                    WasActive;
    bool                    Accessed;                           // Set to true when any widget access the current window
    bool                    Collapsed;                          // Set when collapsing window to become only title-bar
    bool                    SkipItems;                          // == Visible && !Collapsed
    int                     BeginCount;                         // Number of Begin() during the current frame (generally 0 or 1, 1+ if appending via multiple Begin/End pairs)
    ImGuiID                 PopupID;                            // ID in the popup stack when this window is used as a popup/menu (because we use generic Name/ID for recycling) 
    int                     AutoFitFramesX, AutoFitFramesY;
    bool                    AutoFitOnlyGrows;
    int                     AutoPosLastDirection;
    int                     HiddenFrames;
    int                     SetWindowPosAllowFlags;             // bit ImGuiSetCond_*** specify if SetWindowPos() call will succeed with this particular flag. 
    int                     SetWindowSizeAllowFlags;            // bit ImGuiSetCond_*** specify if SetWindowSize() call will succeed with this particular flag. 
    int                     SetWindowCollapsedAllowFlags;       // bit ImGuiSetCond_*** specify if SetWindowCollapsed() call will succeed with this particular flag. 
    bool                    SetWindowPosCenterWanted;

    ImGuiDrawContext        DC;                                 // Temporary per-window data, reset at the beginning of the frame
    ImVector<ImGuiID>       IDStack;                            // ID stack. ID are hashes seeded with the value at the top of the stack
    ImVec4                  ClipRect;                           // = DrawList->clip_rect_stack.back(). Scissoring / clipping rectangle. x1, y1, x2, y2.
    ImRect                  ClippedWindowRect;                  // = ClipRect just after setup in Begin()
    int                     LastFrameDrawn;
    float                   ItemWidthDefault;
    ImGuiSimpleColumns      MenuColumns;                        // Simplified columns storage for menu items
    ImGuiStorage            StateStorage;
    float                   FontWindowScale;                    // Scale multiplier per-window
    ImDrawList*             DrawList;
    ImGuiWindow*            RootWindow;
    ImGuiWindow*            RootNonPopupWindow;

    // Focus
    int                     FocusIdxAllCounter;                 // Start at -1 and increase as assigned via FocusItemRegister()
    int                     FocusIdxTabCounter;                 // (same, but only count widgets which you can Tab through)
    int                     FocusIdxAllRequestCurrent;          // Item being requested for focus
    int                     FocusIdxTabRequestCurrent;          // Tab-able item being requested for focus
    int                     FocusIdxAllRequestNext;             // Item being requested for focus, for next update (relies on layout to be stable between the frame pressing TAB and the next frame)
    int                     FocusIdxTabRequestNext;             // "

public:
    ImGuiWindow(const char* name);
    ~ImGuiWindow();

    ImGuiID     GetID(const char* str, const char* str_end = NULL);
    ImGuiID     GetID(const void* ptr);

    bool        FocusItemRegister(bool is_active, bool tab_stop = true);      // Return true if focus is requested
    void        FocusItemUnregister();

    ImRect      Rect() const                            { return ImRect(Pos, Pos+Size); }
    float       CalcFontSize() const                    { return GImGui->FontBaseSize * FontWindowScale; }
    float       TitleBarHeight() const                  { return (Flags & ImGuiWindowFlags_NoTitleBar) ? 0.0f : CalcFontSize() + GImGui->Style.FramePadding.y * 2.0f; }
    ImRect      TitleBarRect() const                    { return ImRect(Pos, Pos + ImVec2(SizeFull.x, TitleBarHeight())); }
    float       MenuBarHeight() const                   { return (Flags & ImGuiWindowFlags_MenuBar) ? CalcFontSize() + GImGui->Style.FramePadding.y * 2.0f : 0.0f; }
    ImRect      MenuBarRect() const                     { float y1 = Pos.y + TitleBarHeight(); return ImRect(Pos.x, y1, Pos.x + SizeFull.x, y1 + MenuBarHeight()); }
    float       ScrollbarWidth() const                  { return ScrollbarY ? GImGui->Style.ScrollbarWidth : 0.0f; }
    ImU32       Color(ImGuiCol idx, float a=1.f) const  { ImVec4 c = GImGui->Style.Colors[idx]; c.w *= GImGui->Style.Alpha * a; return ImGui::ColorConvertFloat4ToU32(c); }
    ImU32       Color(const ImVec4& col) const          { ImVec4 c = col; c.w *= GImGui->Style.Alpha; return ImGui::ColorConvertFloat4ToU32(c); }
};

static inline ImGuiWindow* GetCurrentWindow()
{
    // If this ever crash it probably means that ImGui::NewFrame() hasn't been called. We should always have a CurrentWindow in the stack (there is an implicit "Debug" window)
    ImGuiState& g = *GImGui;
    g.CurrentWindow->Accessed = true;
    return g.CurrentWindow;
}

static inline void SetCurrentWindow(ImGuiWindow* window)
{
    ImGuiState& g = *GImGui;
    g.CurrentWindow = window;
    if (window)
        g.FontSize = window->CalcFontSize();
}

static inline ImGuiWindow* GetParentWindow()
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(g.CurrentWindowStack.Size >= 2);
    return g.CurrentWindowStack[g.CurrentWindowStack.Size - 2];
}

static void SetActiveId(ImGuiID id, ImGuiWindow* window = NULL) 
{
    ImGuiState& g = *GImGui;
    g.ActiveId = id; 
    g.ActiveIdIsFocusedOnly = false;
    g.ActiveIdIsJustActivated = true;
    g.ActiveIdWindow = window;
}

static void RegisterAliveId(ImGuiID id)
{
    ImGuiState& g = *GImGui;
    if (g.ActiveId == id)
        g.ActiveIdIsAlive = true;
}

//-----------------------------------------------------------------------------

// Helper: Key->value storage
void ImGuiStorage::Clear()
{
    Data.clear();
}

// std::lower_bound but without the bullshit
static ImVector<ImGuiStorage::Pair>::iterator LowerBound(ImVector<ImGuiStorage::Pair>& data, ImU32 key)
{
    ImVector<ImGuiStorage::Pair>::iterator first = data.begin();
    ImVector<ImGuiStorage::Pair>::iterator last = data.end();
    int count = (int)(last - first);
    while (count > 0)
    {
        int count2 = count / 2;
        ImVector<ImGuiStorage::Pair>::iterator mid = first + count2;
        if (mid->key < key)
        {
            first = ++mid;
            count -= count2 + 1;
        }
        else
        {
            count = count2;
        }
    }
    return first;
}

int ImGuiStorage::GetInt(ImU32 key, int default_val) const
{
    ImVector<Pair>::iterator it = LowerBound(const_cast<ImVector<ImGuiStorage::Pair>&>(Data), key);
    if (it == Data.end() || it->key != key)
        return default_val;
    return it->val_i;
}

float ImGuiStorage::GetFloat(ImU32 key, float default_val) const
{
    ImVector<Pair>::iterator it = LowerBound(const_cast<ImVector<ImGuiStorage::Pair>&>(Data), key);
    if (it == Data.end() || it->key != key)
        return default_val;
    return it->val_f;
}

void* ImGuiStorage::GetVoidPtr(ImGuiID key) const
{
    ImVector<Pair>::iterator it = LowerBound(const_cast<ImVector<ImGuiStorage::Pair>&>(Data), key);
    if (it == Data.end() || it->key != key)
        return NULL;
    return it->val_p;
}

// References are only valid until a new value is added to the storage. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
int* ImGuiStorage::GetIntRef(ImGuiID key, int default_val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
        it = Data.insert(it, Pair(key, default_val));
    return &it->val_i;
}

float* ImGuiStorage::GetFloatRef(ImGuiID key, float default_val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
        it = Data.insert(it, Pair(key, default_val));
    return &it->val_f;
}

void** ImGuiStorage::GetVoidPtrRef(ImGuiID key, void* default_val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
        it = Data.insert(it, Pair(key, default_val));
    return &it->val_p;
}

// FIXME-OPT: Need a way to reuse the result of lower_bound when doing GetInt()/SetInt() - not too bad because it only happens on explicit interaction (maximum one a frame)
void ImGuiStorage::SetInt(ImU32 key, int val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
    {
        Data.insert(it, Pair(key, val));
        return;
    }
    it->val_i = val;
}

void ImGuiStorage::SetFloat(ImU32 key, float val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
    {
        Data.insert(it, Pair(key, val));
        return;
    }
    it->val_f = val;
}

void ImGuiStorage::SetVoidPtr(ImU32 key, void* val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
    {
        Data.insert(it, Pair(key, val));
        return;
    }
    it->val_p = val;
}

void ImGuiStorage::SetAllInt(int v)
{
    for (int i = 0; i < Data.Size; i++)
        Data[i].val_i = v;
}

//-----------------------------------------------------------------------------

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
ImGuiTextFilter::ImGuiTextFilter(const char* default_filter)
{
    if (default_filter)
    {
        ImFormatString(InputBuf, IM_ARRAYSIZE(InputBuf), "%s", default_filter);
        Build();
    }
    else
    {
        InputBuf[0] = 0;
        CountGrep = 0;
    }
}

void ImGuiTextFilter::Draw(const char* label, float width)
{
    if (width > 0.0f)
        ImGui::PushItemWidth(width);
    ImGui::InputText(label, InputBuf, IM_ARRAYSIZE(InputBuf));
    if (width > 0.0f)
        ImGui::PopItemWidth();
    Build();
}

void ImGuiTextFilter::TextRange::split(char separator, ImVector<TextRange>& out)
{
    out.resize(0);
    const char* wb = b;
    const char* we = wb;
    while (we < e)
    {
        if (*we == separator)
        {
            out.push_back(TextRange(wb, we));
            wb = we + 1;
        }
        we++;
    }
    if (wb != we)
        out.push_back(TextRange(wb, we));
}

void ImGuiTextFilter::Build()
{
    Filters.resize(0);
    TextRange input_range(InputBuf, InputBuf+strlen(InputBuf));
    input_range.split(',', Filters);

    CountGrep = 0;
    for (int i = 0; i != Filters.Size; i++)
    {
        Filters[i].trim_blanks();
        if (Filters[i].empty())
            continue;
        if (Filters[i].front() != '-')
            CountGrep += 1;
    }
}

bool ImGuiTextFilter::PassFilter(const char* val) const
{
    if (Filters.empty())
        return true;

    if (val == NULL)
        val = "";

    for (int i = 0; i != Filters.Size; i++)
    {
        const TextRange& f = Filters[i];
        if (f.empty())
            continue;
        if (f.front() == '-')
        {
            // Subtract
            if (ImStristr(val, f.begin()+1, f.end()) != NULL)
                return false;
        }
        else
        {
            // Grep
            if (ImStristr(val, f.begin(), f.end()) != NULL)
                return true;
        }
    }

    // Implicit * grep
    if (CountGrep == 0)
        return true;

    return false;
}

// On some platform vsnprintf() takes va_list by reference and modifies it. 
// va_copy is the 'correct' way to copy a va_list but Visual Studio prior to 2013 doesn't have it.
#ifndef va_copy
#define va_copy(dest, src) (dest = src)
#endif

// Helper: Text buffer for logging/accumulating text
void ImGuiTextBuffer::appendv(const char* fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);

    int len = vsnprintf(NULL, 0, fmt, args);         // FIXME-OPT: could do a first pass write attempt, likely successful on first pass.
    if (len <= 0)
        return;

    const int write_off = Buf.Size;
    const int needed_sz = write_off + len;
    if (write_off + len >= Buf.Capacity)
    {
        int double_capacity = Buf.Capacity * 2;
        Buf.reserve(needed_sz > double_capacity ? needed_sz : double_capacity);
    }

    Buf.resize(needed_sz);
    ImFormatStringV(&Buf[write_off] - 1, len+1, fmt, args_copy);
}

void ImGuiTextBuffer::append(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    appendv(fmt, args);
    va_end(args);
}

//-----------------------------------------------------------------------------

ImGuiWindow::ImGuiWindow(const char* name)
{
    Name = ImStrdup(name);
    ID = ImHash(name, 0); 
    IDStack.push_back(ID);
    MoveID = GetID("#MOVE");

    Flags = 0;
    PosFloat = Pos = ImVec2(0.0f, 0.0f);
    Size = SizeFull = ImVec2(0.0f, 0.0f);
    SizeContents = ImVec2(0.0f, 0.0f);
    WindowPadding = ImVec2(0.0f, 0.0f);
    ScrollY = 0.0f;
    ScrollTargetRelY = FLT_MAX;
    ScrollTargetCenterRatioY = 0.5f;
    ScrollbarY = false;
    Active = WasActive = false;
    Accessed = false;
    Collapsed = false;
    SkipItems = false;
    BeginCount = 0;
    PopupID = 0;
    AutoFitFramesX = AutoFitFramesY = -1;
    AutoFitOnlyGrows = false;
    AutoPosLastDirection = -1;
    HiddenFrames = 0;
    SetWindowPosAllowFlags = SetWindowSizeAllowFlags = SetWindowCollapsedAllowFlags = ImGuiSetCond_Always | ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing;
    SetWindowPosCenterWanted = false;

    LastFrameDrawn = -1;
    ItemWidthDefault = 0.0f;
    FontWindowScale = 1.0f;

    DrawList = (ImDrawList*)ImGui::MemAlloc(sizeof(ImDrawList));
    new(DrawList) ImDrawList();
    DrawList->_OwnerName = Name;
    RootWindow = NULL;
    RootNonPopupWindow = NULL;

    FocusIdxAllCounter = FocusIdxTabCounter = -1;
    FocusIdxAllRequestCurrent = FocusIdxTabRequestCurrent = IM_INT_MAX;
    FocusIdxAllRequestNext = FocusIdxTabRequestNext = IM_INT_MAX;
}

ImGuiWindow::~ImGuiWindow()
{
    DrawList->~ImDrawList();
    ImGui::MemFree(DrawList);
    DrawList = NULL;
    ImGui::MemFree(Name);
    Name = NULL;
}

ImGuiID ImGuiWindow::GetID(const char* str, const char* str_end)
{
    ImGuiID seed = IDStack.back();
    ImGuiID id = ImHash(str, str_end ? (int)(str_end - str) : 0, seed);
    RegisterAliveId(id);
    return id;
}

ImGuiID ImGuiWindow::GetID(const void* ptr)
{
    ImGuiID seed = IDStack.back();
    ImGuiID id = ImHash(&ptr, sizeof(void*), seed);
    RegisterAliveId(id);
    return id;
}

bool ImGuiWindow::FocusItemRegister(bool is_active, bool tab_stop)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    const bool allow_keyboard_focus = window->DC.AllowKeyboardFocus;
    FocusIdxAllCounter++;
    if (allow_keyboard_focus)
        FocusIdxTabCounter++;

    // Process keyboard input at this point: TAB, Shift-TAB switch focus
    // We can always TAB out of a widget that doesn't allow tabbing in.
    if (tab_stop && FocusIdxAllRequestNext == IM_INT_MAX && FocusIdxTabRequestNext == IM_INT_MAX && is_active && IsKeyPressedMap(ImGuiKey_Tab))
    {
        // Modulo on index will be applied at the end of frame once we've got the total counter of items.
        FocusIdxTabRequestNext = FocusIdxTabCounter + (g.IO.KeyShift ? (allow_keyboard_focus ? -1 : 0) : +1);
    }

    if (FocusIdxAllCounter == FocusIdxAllRequestCurrent)
        return true;

    if (allow_keyboard_focus)
        if (FocusIdxTabCounter == FocusIdxTabRequestCurrent)
            return true;

    return false;
}

void ImGuiWindow::FocusItemUnregister()
{
    FocusIdxAllCounter--;
    FocusIdxTabCounter--;
}

static inline void AddDrawListToRenderList(ImVector<ImDrawList*>& out_render_list, ImDrawList* draw_list)
{
    if (!draw_list->CmdBuffer.empty() && !draw_list->VtxBuffer.empty())
    {
        if (draw_list->CmdBuffer.back().ElemCount == 0)
            draw_list->CmdBuffer.pop_back();
        out_render_list.push_back(draw_list);
        GImGui->IO.MetricsRenderVertices += draw_list->VtxBuffer.Size;
        GImGui->IO.MetricsRenderIndices += draw_list->IdxBuffer.Size;
    }
}

static void AddWindowToRenderList(ImVector<ImDrawList*>& out_render_list, ImGuiWindow* window)
{
    AddDrawListToRenderList(out_render_list, window->DrawList);
    for (int i = 0; i < window->DC.ChildWindows.Size; i++)
    {
        ImGuiWindow* child = window->DC.ChildWindows[i];
        if (!child->Active) // clipped children may have been marked not active
            continue;
        if ((child->Flags & ImGuiWindowFlags_Popup) && child->HiddenFrames > 0)
            continue;
        AddWindowToRenderList(out_render_list, child);
    }
}

//-----------------------------------------------------------------------------

void* ImGui::MemAlloc(size_t sz)
{
    GImGui->IO.MetricsAllocs++;
    return GImGui->IO.MemAllocFn(sz);
}

void ImGui::MemFree(void* ptr)
{
    if (ptr) GImGui->IO.MetricsAllocs--;
    return GImGui->IO.MemFreeFn(ptr);
}
    
static ImGuiIniData* FindWindowSettings(const char* name)
{
    ImGuiState& g = *GImGui;
    ImGuiID id = ImHash(name, 0);
    for (int i = 0; i != g.Settings.Size; i++)
    {
        ImGuiIniData* ini = &g.Settings[i];
        if (ini->ID == id)
            return ini;
    }
    return NULL;
}

static ImGuiIniData* AddWindowSettings(const char* name)
{
    GImGui->Settings.resize(GImGui->Settings.Size + 1);
    ImGuiIniData* ini = &GImGui->Settings.back();
    ini->Name = ImStrdup(name);
    ini->ID = ImHash(name, 0);
    ini->Collapsed = false;
    ini->Pos = ImVec2(FLT_MAX,FLT_MAX);
    ini->Size = ImVec2(0,0);
    return ini;
}

// Zero-tolerance, poor-man .ini parsing
// FIXME: Write something less rubbish
static void LoadSettings()
{
    ImGuiState& g = *GImGui;
    const char* filename = g.IO.IniFilename;
    if (!filename)
        return;

    char* file_data;
    int file_size;
    if (!ImLoadFileToMemory(filename, "rb", (void**)&file_data, &file_size, 1))
        return;

    ImGuiIniData* settings = NULL;
    const char* buf_end = file_data + file_size;
    for (const char* line_start = file_data; line_start < buf_end; )
    {
        const char* line_end = line_start;
        while (line_end < buf_end && *line_end != '\n' && *line_end != '\r')
            line_end++;
        
        if (line_start[0] == '[' && line_end > line_start && line_end[-1] == ']')
        {
            char name[64];
            ImFormatString(name, IM_ARRAYSIZE(name), "%.*s", line_end-line_start-2, line_start+1);
            settings = FindWindowSettings(name);
            if (!settings)
                settings = AddWindowSettings(name);
        }
        else if (settings)
        {
            float x, y;
            int i;
            if (sscanf(line_start, "Pos=%f,%f", &x, &y) == 2)
                settings->Pos = ImVec2(x, y);
            else if (sscanf(line_start, "Size=%f,%f", &x, &y) == 2)
                settings->Size = ImMax(ImVec2(x, y), g.Style.WindowMinSize);
            else if (sscanf(line_start, "Collapsed=%d", &i) == 1)
                settings->Collapsed = (i != 0);
        }

        line_start = line_end+1;
    }

    ImGui::MemFree(file_data);
}

static void SaveSettings()
{
    ImGuiState& g = *GImGui;
    const char* filename = g.IO.IniFilename;
    if (!filename)
        return;

    // Gather data from windows that were active during this session
    for (int i = 0; i != g.Windows.Size; i++)
    {
        ImGuiWindow* window = g.Windows[i];
        if (window->Flags & ImGuiWindowFlags_NoSavedSettings)
            continue;
        ImGuiIniData* settings = FindWindowSettings(window->Name);
        settings->Pos = window->Pos;
        settings->Size = window->SizeFull;
        settings->Collapsed = window->Collapsed;
    }

    // Write .ini file
    // If a window wasn't opened in this session we preserve its settings
    FILE* f = fopen(filename, "wt");
    if (!f)
        return;
    for (int i = 0; i != g.Settings.Size; i++)
    {
        const ImGuiIniData* settings = &g.Settings[i];
        if (settings->Pos.x == FLT_MAX)
            continue;
        const char* name = settings->Name;
        if (const char* p = strstr(name, "###"))  // Skip to the "###" marker if any. We don't skip past to match the behavior of GetID()
            name = p;
        fprintf(f, "[%s]\n", name);
        fprintf(f, "Pos=%d,%d\n", (int)settings->Pos.x, (int)settings->Pos.y);
        fprintf(f, "Size=%d,%d\n", (int)settings->Size.x, (int)settings->Size.y);
        fprintf(f, "Collapsed=%d\n", settings->Collapsed);
        fprintf(f, "\n");
    }

    fclose(f);
}

static void MarkSettingsDirty()
{
    ImGuiState& g = *GImGui;
    if (g.SettingsDirtyTimer <= 0.0f)
        g.SettingsDirtyTimer = g.IO.IniSavingRate;
}

const char* ImGui::GetVersion()
{
    return IMGUI_VERSION;
}

// Internal state access - if you want to share ImGui state between modules (e.g. DLL) or allocate it yourself
// Note that we still point to some static data and members (such as GFontAtlas), so the state instance you end up using will point to the static data within its module
void* ImGui::GetInternalState()
{
    return GImGui;
}

size_t ImGui::GetInternalStateSize()
{
    return sizeof(ImGuiState);
}

void ImGui::SetInternalState(void* state, bool construct)
{
    if (construct)
        new (state) ImGuiState();
    
    GImGui = (ImGuiState*)state;
}

ImGuiIO& ImGui::GetIO()
{
    return GImGui->IO;
}

ImGuiStyle& ImGui::GetStyle()
{
    return GImGui->Style;
}

void ImGui::NewFrame()
{
    ImGuiState& g = *GImGui;

    // Check user data
    IM_ASSERT(g.IO.DeltaTime >= 0.0f);
    IM_ASSERT(g.IO.DisplaySize.x >= 0.0f && g.IO.DisplaySize.y >= 0.0f);
    IM_ASSERT(g.IO.RenderDrawListsFn != NULL);       // Must be implemented
    IM_ASSERT(g.IO.Fonts->Fonts.Size > 0);           // Font Atlas not created. Did you call io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?
    IM_ASSERT(g.IO.Fonts->Fonts[0]->IsLoaded());     // Font Atlas not created. Did you call io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?

    if (!g.Initialized)
    {
        // Initialize on first frame
        g.LogClipboard = (ImGuiTextBuffer*)ImGui::MemAlloc(sizeof(ImGuiTextBuffer));
        new(g.LogClipboard) ImGuiTextBuffer();

        IM_ASSERT(g.Settings.empty());
        LoadSettings();
        g.Initialized = true;
    }

    SetFont(g.IO.Fonts->Fonts[0]);

    g.Time += g.IO.DeltaTime;
    g.FrameCount += 1;
    g.Tooltip[0] = '\0';
    g.OverlayDrawList.Clear();
    g.OverlayDrawList.PushTextureID(g.IO.Fonts->TexID);
    g.OverlayDrawList.PushClipRectFullScreen();
    g.OverlayDrawList.AddDrawCmd();

    // Update inputs state
    if (g.IO.MousePos.x < 0 && g.IO.MousePos.y < 0)
        g.IO.MousePos = ImVec2(-9999.0f, -9999.0f);
    if ((g.IO.MousePos.x < 0 && g.IO.MousePos.y < 0) || (g.IO.MousePosPrev.x < 0 && g.IO.MousePosPrev.y < 0))   // if mouse just appeared or disappeared (negative coordinate) we cancel out movement in MouseDelta
        g.IO.MouseDelta = ImVec2(0.0f, 0.0f);
    else
        g.IO.MouseDelta = g.IO.MousePos - g.IO.MousePosPrev;
    g.IO.MousePosPrev = g.IO.MousePos;
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++)
    {
        g.IO.MouseDownDurationPrev[i] = g.IO.MouseDownDuration[i];
        g.IO.MouseDownDuration[i] = g.IO.MouseDown[i] ? (g.IO.MouseDownDuration[i] < 0.0f ? 0.0f : g.IO.MouseDownDuration[i] + g.IO.DeltaTime) : -1.0f;
        g.IO.MouseClicked[i] = g.IO.MouseDownDuration[i] == 0.0f;
        g.IO.MouseReleased[i] = g.IO.MouseDownDurationPrev[i] >= 0.0f && !g.IO.MouseDown[i];
        g.IO.MouseDoubleClicked[i] = false;
        if (g.IO.MouseClicked[i])
        {
            if (g.Time - g.IO.MouseClickedTime[i] < g.IO.MouseDoubleClickTime)
            {
                if (ImLengthSqr(g.IO.MousePos - g.IO.MouseClickedPos[i]) < g.IO.MouseDoubleClickMaxDist * g.IO.MouseDoubleClickMaxDist)
                    g.IO.MouseDoubleClicked[i] = true;
                g.IO.MouseClickedTime[i] = -FLT_MAX;    // so the third click isn't turned into a double-click
            }
            else
            {
                g.IO.MouseClickedTime[i] = g.Time;
            }
            g.IO.MouseClickedPos[i] = g.IO.MousePos;
            g.IO.MouseDragMaxDistanceSqr[i] = 0.0f;
        }
        else if (g.IO.MouseDown[i])
        {
            g.IO.MouseDragMaxDistanceSqr[i] = ImMax(g.IO.MouseDragMaxDistanceSqr[i], ImLengthSqr(g.IO.MousePos - g.IO.MouseClickedPos[i]));
        }
    }
    memcpy(g.IO.KeysDownDurationPrev, g.IO.KeysDownDuration, sizeof(g.IO.KeysDownDuration));
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.KeysDown); i++)
        g.IO.KeysDownDuration[i] = g.IO.KeysDown[i] ? (g.IO.KeysDownDuration[i] < 0.0f ? 0.0f : g.IO.KeysDownDuration[i] + g.IO.DeltaTime) : -1.0f;

    // Calculate frame-rate for the user, as a purely luxurious feature
    g.FramerateSecPerFrameAccum += g.IO.DeltaTime - g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx];
    g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx] = g.IO.DeltaTime;
    g.FramerateSecPerFrameIdx = (g.FramerateSecPerFrameIdx + 1) % IM_ARRAYSIZE(g.FramerateSecPerFrame);
    g.IO.Framerate = 1.0f / (g.FramerateSecPerFrameAccum / (float)IM_ARRAYSIZE(g.FramerateSecPerFrame));

    // Clear reference to active widget if the widget isn't alive anymore
    g.HoveredIdPreviousFrame = g.HoveredId;
    g.HoveredId = 0;
    if (!g.ActiveIdIsAlive && g.ActiveIdPreviousFrame == g.ActiveId && g.ActiveId != 0)
        SetActiveId(0);
    g.ActiveIdPreviousFrame = g.ActiveId;
    g.ActiveIdIsAlive = false;
    g.ActiveIdIsJustActivated = false;
    if (!g.ActiveId)
        g.MovedWindow = NULL;

    // Delay saving settings so we don't spam disk too much
    if (g.SettingsDirtyTimer > 0.0f)
    {
        g.SettingsDirtyTimer -= g.IO.DeltaTime;
        if (g.SettingsDirtyTimer <= 0.0f)
            SaveSettings();
    }

    // Find the window we are hovering. Child windows can extend beyond the limit of their parent so we need to derive HoveredRootWindow from HoveredWindow
    g.HoveredWindow = FindHoveredWindow(g.IO.MousePos, false);
    if (g.HoveredWindow && (g.HoveredWindow->Flags & ImGuiWindowFlags_ChildWindow))
        g.HoveredRootWindow = g.HoveredWindow->RootWindow;
    else
        g.HoveredRootWindow = FindHoveredWindow(g.IO.MousePos, true);

    if (ImGuiWindow* modal_window = GetFrontMostModalRootWindow())
    {
        g.ModalWindowDarkeningRatio = ImMin(g.ModalWindowDarkeningRatio + g.IO.DeltaTime * 6.0f, 1.0f);
        if (g.HoveredRootWindow != modal_window)
            g.HoveredRootWindow = g.HoveredWindow = NULL;
    }
    else
    {
        g.ModalWindowDarkeningRatio = 0.0f;
    }

    // Are we using inputs? Tell user so they can capture/discard the inputs away from the rest of their application.
    // When clicking outside of a window we assume the click is owned by the application and won't request capture.
    int mouse_earliest_button_down = -1;
    bool mouse_any_down = false;
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++)
    {
        if (g.IO.MouseClicked[i])
            g.IO.MouseDownOwned[i] = (g.HoveredWindow != NULL) || (!g.OpenedPopupStack.empty());
        mouse_any_down |= g.IO.MouseDown[i];
        if (g.IO.MouseDown[i])
            if (mouse_earliest_button_down == -1 || g.IO.MouseClickedTime[mouse_earliest_button_down] > g.IO.MouseClickedTime[i])
                mouse_earliest_button_down = i;
    }
    bool mouse_owned_by_application = mouse_earliest_button_down != -1 && !g.IO.MouseDownOwned[mouse_earliest_button_down];
    g.IO.WantCaptureMouse = (!mouse_owned_by_application && g.HoveredWindow != NULL) || (!mouse_owned_by_application && mouse_any_down) || (g.ActiveId != 0) || (!g.OpenedPopupStack.empty()) || (g.CaptureMouseNextFrame);
    g.IO.WantCaptureKeyboard = (g.ActiveId != 0) || (g.CaptureKeyboardNextFrame);
    g.MouseCursor = ImGuiMouseCursor_Arrow;
    g.CaptureMouseNextFrame = g.CaptureKeyboardNextFrame = false;

    // If mouse was first clicked outside of ImGui bounds we also cancel out hovering.
    if (mouse_owned_by_application)
        g.HoveredWindow = g.HoveredRootWindow = NULL;

    // Scale & Scrolling
    if (g.HoveredWindow && g.IO.MouseWheel != 0.0f && !g.HoveredWindow->Collapsed)
    {
        ImGuiWindow* window = g.HoveredWindow;
        if (g.IO.KeyCtrl)
        {
            if (g.IO.FontAllowUserScaling)
            {
                // Zoom / Scale window
                float new_font_scale = ImClamp(window->FontWindowScale + g.IO.MouseWheel * 0.10f, 0.50f, 2.50f);
                float scale = new_font_scale / window->FontWindowScale;
                window->FontWindowScale = new_font_scale;

                const ImVec2 offset = window->Size * (1.0f - scale) * (g.IO.MousePos - window->Pos) / window->Size;
                window->Pos += offset;
                window->PosFloat += offset;
                window->Size *= scale;
                window->SizeFull *= scale;
            }
        }
        else
        {
            // Scroll
            if (!(window->Flags & ImGuiWindowFlags_NoScrollWithMouse))
            {
                const int scroll_lines = (window->Flags & ImGuiWindowFlags_ComboBox) ? 3 : 5;
                SetWindowScrollY(window, window->ScrollY - g.IO.MouseWheel * window->CalcFontSize() * scroll_lines);
            }
        }
    }

    // Pressing TAB activate widget focus
    // NB: Don't discard FocusedWindow if it isn't active, so that a window that go on/off programatically won't lose its keyboard focus.
    if (g.ActiveId == 0 && g.FocusedWindow != NULL && g.FocusedWindow->Active && IsKeyPressedMap(ImGuiKey_Tab, false))
        g.FocusedWindow->FocusIdxTabRequestNext = 0;

    // Mark all windows as not visible
    for (int i = 0; i != g.Windows.Size; i++)
    {
        ImGuiWindow* window = g.Windows[i];
        window->WasActive = window->Active;
        window->Active = false;
        window->Accessed = false;
    }

    // No window should be open at the beginning of the frame.
    // But in order to allow the user to call NewFrame() multiple times without calling Render(), we are doing an explicit clear.
    g.CurrentWindowStack.resize(0);
    g.CurrentPopupStack.resize(0);
    CloseInactivePopups();

    // Create implicit window - we will only render it if the user has added something to it.
    ImGui::SetNextWindowSize(ImVec2(400,400), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Debug");
}

// NB: behavior of ImGui after Shutdown() is not tested/guaranteed at the moment. This function is merely here to free heap allocations.
void ImGui::Shutdown()
{
    ImGuiState& g = *GImGui;
    if (!g.Initialized)
        return;

    SaveSettings();

    for (int i = 0; i < g.Windows.Size; i++)
    {
        g.Windows[i]->~ImGuiWindow();
        ImGui::MemFree(g.Windows[i]);
    }
    g.Windows.clear();
    g.WindowsSortBuffer.clear();
    g.CurrentWindowStack.clear();
    g.FocusedWindow = NULL;
    g.HoveredWindow = NULL;
    g.HoveredRootWindow = NULL;
    for (int i = 0; i < g.Settings.Size; i++)
        ImGui::MemFree(g.Settings[i].Name);
    g.Settings.clear();
    g.ColorModifiers.clear();
    g.StyleModifiers.clear();
    g.FontStack.clear();
    g.OpenedPopupStack.clear();
    g.CurrentPopupStack.clear();
    for (int i = 0; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
        g.RenderDrawLists[i].clear();
    g.OverlayDrawList.ClearFreeMemory();
    g.ColorEditModeStorage.Clear();
    if (g.PrivateClipboard)
    {
        ImGui::MemFree(g.PrivateClipboard);
        g.PrivateClipboard = NULL;
    }
    g.InputTextState.Text.clear();
    g.InputTextState.InitialText.clear();
    g.InputTextState.TempTextBuffer.clear();

    if (g.LogFile && g.LogFile != stdout)
    {
        fclose(g.LogFile);
        g.LogFile = NULL;
    }
    if (g.LogClipboard)
    {
        g.LogClipboard->~ImGuiTextBuffer();
        ImGui::MemFree(g.LogClipboard);
    }

    if (g.IO.Fonts) // Testing for NULL to allow user to NULLify in case of running Shutdown() on multiple contexts. Bit hacky.
        g.IO.Fonts->Clear();

    g.Initialized = false;
}

// FIXME: Add a more explicit sort order in the window structure.
static int ChildWindowComparer(const void* lhs, const void* rhs)
{
    const ImGuiWindow* a = *(const ImGuiWindow**)lhs;
    const ImGuiWindow* b = *(const ImGuiWindow**)rhs;
    if (int d = (a->Flags & ImGuiWindowFlags_Popup) - (b->Flags & ImGuiWindowFlags_Popup))
        return d;
    if (int d = (a->Flags & ImGuiWindowFlags_Tooltip) - (b->Flags & ImGuiWindowFlags_Tooltip))
        return d;
    if (int d = (a->Flags & ImGuiWindowFlags_ComboBox) - (b->Flags & ImGuiWindowFlags_ComboBox))
        return d;
    return 0;
}

static void AddWindowToSortedBuffer(ImVector<ImGuiWindow*>& out_sorted_windows, ImGuiWindow* window)
{
    out_sorted_windows.push_back(window);
    if (window->Active)
    {
        int count = window->DC.ChildWindows.Size;
        if (count > 1)
            qsort(window->DC.ChildWindows.begin(), (size_t)count, sizeof(ImGuiWindow*), ChildWindowComparer);
        for (int i = 0; i < count; i++)
        {
            ImGuiWindow* child = window->DC.ChildWindows[i];
            if (child->Active)
                AddWindowToSortedBuffer(out_sorted_windows, child);
        }
    }
}

static void PushClipRect(const ImVec4& clip_rect, bool clipped = true)
{
    ImGuiWindow* window = GetCurrentWindow();

    ImVec4 cr = clip_rect;
    if (clipped)
    {
        // Clip with existing clip rect
        const ImVec4 cur_cr = window->ClipRect;
        cr = ImVec4(ImMax(cr.x, cur_cr.x), ImMax(cr.y, cur_cr.y), ImMin(cr.z, cur_cr.z), ImMin(cr.w, cur_cr.w));
    }
    cr.z = ImMax(cr.x, cr.z);
    cr.w = ImMax(cr.y, cr.w);

    IM_ASSERT(cr.x <= cr.z && cr.y <= cr.w);
    window->ClipRect = cr;
    window->DrawList->PushClipRect(cr);
}

static void PopClipRect()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DrawList->PopClipRect();
    window->ClipRect = window->DrawList->_ClipRectStack.back();
}

void ImGui::Render()
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(g.Initialized);                           // Forgot to call ImGui::NewFrame()

    const bool first_render_of_the_frame = (g.FrameCountRendered != g.FrameCount);
    g.FrameCountRendered = g.FrameCount;
    
    if (first_render_of_the_frame)
    {
        // Hide implicit "Debug" window if it hasn't been used
        IM_ASSERT(g.CurrentWindowStack.Size == 1);    // Mismatched Begin/End 
        if (g.CurrentWindow && !g.CurrentWindow->Accessed)
            g.CurrentWindow->Active = false;
        ImGui::End();

        // Click to focus window and start moving (after we're done with all our widgets)
        if (!g.ActiveId)
            g.MovedWindow = NULL;
        if (g.ActiveId == 0 && g.HoveredId == 0 && g.IO.MouseClicked[0])
        {
            if (!(g.FocusedWindow && !g.FocusedWindow->WasActive && g.FocusedWindow->Active)) // Unless we just made a popup appear
            {
                if (g.HoveredRootWindow != NULL)
                {
                    g.MovedWindow = g.HoveredWindow;
                    SetActiveId(g.HoveredRootWindow->MoveID, g.HoveredRootWindow);
                }
                else if (g.FocusedWindow != NULL && GetFrontMostModalRootWindow() == NULL)
                {
                    // Clicking on void disable focus
                    FocusWindow(NULL);
                }
            }
        }

        // Sort the window list so that all child windows are after their parent
        // We cannot do that on FocusWindow() because childs may not exist yet
        g.WindowsSortBuffer.resize(0);
        g.WindowsSortBuffer.reserve(g.Windows.Size);
        for (int i = 0; i != g.Windows.Size; i++)
        {
            ImGuiWindow* window = g.Windows[i];
            if (window->Flags & ImGuiWindowFlags_ChildWindow)       // if a child is active its parent will add it
                if (window->Active)
                    continue;
            AddWindowToSortedBuffer(g.WindowsSortBuffer, window);
        }
        IM_ASSERT(g.Windows.Size == g.WindowsSortBuffer.Size);  // we done something wrong
        g.Windows.swap(g.WindowsSortBuffer);

        // Clear Input data for next frame
        g.IO.MouseWheel = 0.0f;
        memset(g.IO.InputCharacters, 0, sizeof(g.IO.InputCharacters));
    }

    // Skip render altogether if alpha is 0.0
    // Note that vertex buffers have been created and are wasted, so it is best practice that you don't create windows in the first place, or respond to Begin() returning false.
    if (g.Style.Alpha > 0.0f)
    {
        // Render tooltip
        if (g.Tooltip[0])
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(g.Tooltip);
            ImGui::EndTooltip();
        }

        // Gather windows to render
        g.IO.MetricsRenderVertices = g.IO.MetricsRenderIndices = g.IO.MetricsActiveWindows = 0;
        for (int i = 0; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
            g.RenderDrawLists[i].resize(0);
        for (int i = 0; i != g.Windows.Size; i++)
        {
            ImGuiWindow* window = g.Windows[i];
            if (window->Active && window->HiddenFrames <= 0 && (window->Flags & (ImGuiWindowFlags_ChildWindow)) == 0)
            {
                // FIXME: Generalize this with a proper layering system so e.g. user can draw in specific layers, below text, ..
                g.IO.MetricsActiveWindows++;
                if (window->Flags & ImGuiWindowFlags_Popup)
                    AddWindowToRenderList(g.RenderDrawLists[1], window);
                else if (window->Flags & ImGuiWindowFlags_Tooltip)
                    AddWindowToRenderList(g.RenderDrawLists[2], window);
                else
                    AddWindowToRenderList(g.RenderDrawLists[0], window);
            }
        }

        // Flatten layers
        int n = g.RenderDrawLists[0].Size;
        int flattened_size = n;
        for (int i = 1; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
            flattened_size += g.RenderDrawLists[i].Size;
        g.RenderDrawLists[0].resize(flattened_size);
        for (int i = 1; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
        {
            ImVector<ImDrawList*>& layer = g.RenderDrawLists[i];
            if (!layer.empty())
            {
                memcpy(&g.RenderDrawLists[0][n], &layer[0], layer.Size * sizeof(ImDrawList*));
                n += layer.Size;
            }
        }

        if (g.IO.MouseDrawCursor)
        {
            const ImGuiMouseCursorData& cursor_data = g.MouseCursorData[g.MouseCursor];
            const ImVec2 pos = g.IO.MousePos - cursor_data.Offset;
            const ImVec2 size = cursor_data.Size;
            const ImTextureID tex_id = g.IO.Fonts->TexID;
            g.OverlayDrawList.PushTextureID(tex_id);
            g.OverlayDrawList.AddImage(tex_id, pos+ImVec2(1,0), pos+ImVec2(1,0) + size, cursor_data.TexUvMin[1], cursor_data.TexUvMax[1], 0x30000000); // Shadow
            g.OverlayDrawList.AddImage(tex_id, pos+ImVec2(2,0), pos+ImVec2(2,0) + size, cursor_data.TexUvMin[1], cursor_data.TexUvMax[1], 0x30000000); // Shadow
            g.OverlayDrawList.AddImage(tex_id, pos,             pos + size,             cursor_data.TexUvMin[1], cursor_data.TexUvMax[1], 0xFF000000); // Black border
            g.OverlayDrawList.AddImage(tex_id, pos,             pos + size,             cursor_data.TexUvMin[0], cursor_data.TexUvMax[0], 0xFFFFFFFF); // White fill
            g.OverlayDrawList.PopTextureID();
        }
        if (!g.OverlayDrawList.VtxBuffer.empty())
            AddDrawListToRenderList(g.RenderDrawLists[0], &g.OverlayDrawList);

        // Render
        if (!g.RenderDrawLists[0].empty())
        {
            ImDrawData data;
            data.CmdLists = &g.RenderDrawLists[0][0];
            data.CmdListsCount = g.RenderDrawLists[0].Size;
            data.TotalVtxCount = g.IO.MetricsRenderVertices;
            data.TotalIdxCount = g.IO.MetricsRenderIndices;
            g.IO.RenderDrawListsFn(&data);
        }
    }
}

// Find the optional ## from which we stop displaying text.
static const char*  FindTextDisplayEnd(const char* text, const char* text_end = NULL)
{
    const char* text_display_end = text;
    if (!text_end)
        text_end = (const char*)-1;

    ImGuiState& g = *GImGui;
    if (g.DisableHideTextAfterDoubleHash > 0)
    {
        while (text_display_end < text_end && *text_display_end != '\0')
            text_display_end++;
    }
    else
    {
        while (text_display_end < text_end && *text_display_end != '\0' && (text_display_end[0] != '#' || text_display_end[1] != '#'))
            text_display_end++;
    }
    return text_display_end;
}

// Pass text data straight to log (without being displayed)
void ImGui::LogText(const char* fmt, ...)
{
    ImGuiState& g = *GImGui;
    if (!g.LogEnabled)
        return;

    va_list args;
    va_start(args, fmt);
    if (g.LogFile)
    {
        vfprintf(g.LogFile, fmt, args);
    }
    else
    {
        g.LogClipboard->appendv(fmt, args);
    }
    va_end(args);
}

// Internal version that takes a position to decide on newline placement and pad items according to their depth.
// We split text into individual lines to add current tree level padding
static void LogText(const ImVec2& ref_pos, const char* text, const char* text_end)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    if (!text_end)
        text_end = FindTextDisplayEnd(text, text_end);

    const bool log_new_line = ref_pos.y > window->DC.LogLinePosY+1;
    window->DC.LogLinePosY = ref_pos.y;

    const char* text_remaining = text;
    if (g.LogStartDepth > window->DC.TreeDepth)  // Re-adjust padding if we have popped out of our starting depth
        g.LogStartDepth = window->DC.TreeDepth;
    const int tree_depth = (window->DC.TreeDepth - g.LogStartDepth);
    for (;;)
    {
        // Split the string. Each new line (after a '\n') is followed by spacing corresponding to the current depth of our log entry.
        const char* line_end = text_remaining;
        while (line_end < text_end)
            if (*line_end == '\n')
                break;
            else
                line_end++;
        if (line_end >= text_end)
            line_end = NULL;

        const bool is_first_line = (text == text_remaining);
        bool is_last_line = false;
        if (line_end == NULL)
        {
            is_last_line = true;
            line_end = text_end;
        }
        if (line_end != NULL && !(is_last_line && (line_end - text_remaining)==0))
        {
            const int char_count = (int)(line_end - text_remaining);
            if (log_new_line || !is_first_line)
                ImGui::LogText(IM_NEWLINE "%*s%.*s", tree_depth*4, "", char_count, text_remaining);
            else
                ImGui::LogText(" %.*s", char_count, text_remaining);
        }

        if (is_last_line)
            break;
        text_remaining = line_end + 1;
    }
}

static float CalcWrapWidthForPos(const ImVec2& pos, float wrap_pos_x)
{
    if (wrap_pos_x < 0.0f)
        return 0.0f;

    ImGuiWindow* window = GetCurrentWindow();
    if (wrap_pos_x == 0.0f)
        wrap_pos_x = ImGui::GetContentRegionMax().x;
    if (wrap_pos_x > 0.0f)
        wrap_pos_x += window->Pos.x; // wrap_pos_x is provided is window local space
    
    const float wrap_width = wrap_pos_x > 0.0f ? ImMax(wrap_pos_x - pos.x, 0.00001f) : 0.0f;
    return wrap_width;
}

// Internal ImGui functions to render text
// RenderText***() functions calls ImDrawList::AddText() calls ImBitmapFont::RenderText()
static void RenderText(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    // Hide anything after a '##' string
    const char* text_display_end;
    if (hide_text_after_hash)
    {
        text_display_end = FindTextDisplayEnd(text, text_end);
    }
    else
    {
        if (!text_end)
            text_end = text + strlen(text); // FIXME-OPT
        text_display_end = text_end;
    }

    const int text_len = (int)(text_display_end - text);
    if (text_len > 0)
    {
        // Render
        window->DrawList->AddText(g.Font, g.FontSize, pos, window->Color(ImGuiCol_Text), text, text_display_end);

        // Log as text
        if (g.LogEnabled)
            LogText(pos, text, text_display_end);
    }
}

static void RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    if (!text_end)
        text_end = text + strlen(text); // FIXME-OPT

    const int text_len = (int)(text_end - text);
    if (text_len > 0)
    {
        window->DrawList->AddText(g.Font, g.FontSize, pos, window->Color(ImGuiCol_Text), text, text_end, wrap_width);
        if (g.LogEnabled)
            LogText(pos, text, text_end);
    }
}

static void RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, ImGuiAlign align, const ImVec2* clip_min, const ImVec2* clip_max)
{
    // Hide anything after a '##' string
    const char* text_display_end = FindTextDisplayEnd(text, text_end);
    const int text_len = (int)(text_display_end - text);
    if (text_len == 0)
        return;

    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    // Perform CPU side clipping for single clipped element to avoid using scissor state
    ImVec2 pos = pos_min;
    const ImVec2 text_size = text_size_if_known ? *text_size_if_known : ImGui::CalcTextSize(text, text_display_end, false, 0.0f);

    if (!clip_max) clip_max = &pos_max;
    bool need_clipping = (pos.x + text_size.x >= clip_max->x) || (pos.y + text_size.y >= clip_max->y);
    if (!clip_min) clip_min = &pos_min; else need_clipping |= (pos.x < clip_min->x) || (pos.y < clip_min->y);

    // Align
    if (align & ImGuiAlign_Center) pos.x = ImMax(pos.x, (pos.x + pos_max.x - text_size.x) * 0.5f);
    else if (align & ImGuiAlign_Right) pos.x = ImMax(pos.x, pos_max.x - text_size.x);
    if (align & ImGuiAlign_VCenter) pos.y = ImMax(pos.y, (pos.y + pos_max.y - text_size.y) * 0.5f);

    // Render
    if (need_clipping)
    {
        ImVec4 fine_clip_rect(clip_min->x, clip_min->y, clip_max->x, clip_max->y);
        window->DrawList->AddText(g.Font, g.FontSize, pos, window->Color(ImGuiCol_Text), text, text_display_end, 0.0f, &fine_clip_rect);
    }
    else
    {
        window->DrawList->AddText(g.Font, g.FontSize, pos, window->Color(ImGuiCol_Text), text, text_display_end, 0.0f, NULL);
    }
    if (g.LogEnabled)
        LogText(pos, text, text_display_end);
}

// Render a rectangle shaped with optional rounding and borders
static void RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding)
{
    ImGuiWindow* window = GetCurrentWindow();

    window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
    if (border && (window->Flags & ImGuiWindowFlags_ShowBorders))
    {
        window->DrawList->AddRect(p_min+ImVec2(1,1), p_max, window->Color(ImGuiCol_BorderShadow), rounding);
        window->DrawList->AddRect(p_min, p_max-ImVec2(1,1), window->Color(ImGuiCol_Border), rounding);
    }
}

// Render a triangle to denote expanded/collapsed state
static void RenderCollapseTriangle(ImVec2 p_min, bool opened, float scale, bool shadow)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    const float h = g.FontSize * 1.00f;
    const float r = h * 0.40f * scale;
    ImVec2 center = p_min + ImVec2(h*0.50f, h*0.50f*scale);

    ImVec2 a, b, c;
    if (opened)
    {
        center.y -= r*0.25f;
        a = center + ImVec2(0,1)*r;
        b = center + ImVec2(-0.866f,-0.5f)*r;
        c = center + ImVec2(0.866f,-0.5f)*r;
    }
    else
    {
        a = center + ImVec2(1,0)*r;
        b = center + ImVec2(-0.500f,0.866f)*r;
        c = center + ImVec2(-0.500f,-0.866f)*r;
    }
    
    if (shadow && (window->Flags & ImGuiWindowFlags_ShowBorders) != 0)
        window->DrawList->AddTriangleFilled(a+ImVec2(2,2), b+ImVec2(2,2), c+ImVec2(2,2), window->Color(ImGuiCol_BorderShadow));
    window->DrawList->AddTriangleFilled(a, b, c, window->Color(ImGuiCol_Text));
}

static void RenderCheckMark(ImVec2 pos, ImU32 col)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImVec2 a, b, c;
    float start_x = (float)(int)(g.FontSize * 0.307f + 0.5f);
    float rem_third = (float)(int)((g.FontSize - start_x) / 3.0f);
    a.x = pos.x + 0.5f + start_x;
    b.x = a.x + rem_third;
    c.x = a.x + rem_third * 3.0f;
    b.y = pos.y - 1.0f + (float)(int)(g.Font->Ascent * (g.FontSize / g.Font->FontSize) + 0.5f) + (float)(int)(g.Font->DisplayOffset.y);
    a.y = b.y - rem_third;
    c.y = b.y - rem_third * 2.0f;

    window->DrawList->PathLineTo(a);
    window->DrawList->PathLineTo(b);
    window->DrawList->PathLineTo(c);
    window->DrawList->PathStroke(col, false);
}

// Calculate text size. Text can be multi-line. Optionally ignore text after a ## marker.
// CalcTextSize("") should return ImVec2(0.0f, GImGui->FontSize)
ImVec2 ImGui::CalcTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)
{
    ImGuiState& g = *GImGui;

    const char* text_display_end;
    if (hide_text_after_double_hash)
        text_display_end = FindTextDisplayEnd(text, text_end);      // Hide anything after a '##' string
    else
        text_display_end = text_end;

    ImFont* font = g.Font;
    const float font_size = g.FontSize;
    ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text, text_display_end, NULL);

    // Cancel out character spacing for the last character of a line (it is baked into glyph->XAdvance field)
    const float font_scale = font_size / font->FontSize; 
    const float character_spacing_x = 1.0f * font_scale;
    if (text_size.x > 0.0f)
        text_size.x -= character_spacing_x;

    return text_size;
}

// Helper to calculate coarse clipping of large list of evenly sized items. 
// NB: Prefer using the ImGuiListClipper higher-level helper if you can!
// If you are displaying thousands of items and you have a random access to the list, you can perform clipping yourself to save on CPU.
// {
//    float item_height = ImGui::GetTextLineHeightWithSpacing();
//    int display_start, display_end;
//    ImGui::CalcListClipping(count, item_height, &display_start, &display_end);            // calculate how many to clip/display
//    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (display_start) * item_height);         // advance cursor
//    for (int i = display_start; i < display_end; i++)                                     // display only visible items
//        // TODO: display visible item
//    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (count - display_end) * item_height);   // advance cursor
// }
void ImGui::CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (g.LogEnabled)
    {
        // If logging is active, do not perform any clipping
        *out_items_display_start = 0;
        *out_items_display_end = items_count;
        return;
    }

    const ImVec2 pos = window->DC.CursorPos;
    const float clip_y1 = window->ClipRect.y;
    const float clip_y2 = window->ClipRect.w;

    int start = (int)((clip_y1 - pos.y) / items_height);
    int end = (int)((clip_y2 - pos.y) / items_height);
    start = ImClamp(start, 0, items_count);
    end = ImClamp(end + 1, start, items_count);
    *out_items_display_start = start;
    *out_items_display_end = end;
}

// Find window given position, search front-to-back
static ImGuiWindow* FindHoveredWindow(ImVec2 pos, bool excluding_childs)
{
    ImGuiState& g = *GImGui;
    for (int i = g.Windows.Size-1; i >= 0; i--)
    {
        ImGuiWindow* window = g.Windows[i];
        if (!window->Active)
            continue;
        if (window->Flags & ImGuiWindowFlags_NoInputs)
            continue;
        if (excluding_childs && (window->Flags & ImGuiWindowFlags_ChildWindow) != 0)
            continue;

        // Using the clipped AABB so a child window will typically be clipped by its parent.
        ImRect bb(window->ClippedWindowRect.Min - g.Style.TouchExtraPadding, window->ClippedWindowRect.Max + g.Style.TouchExtraPadding);
        if (bb.Contains(pos))
            return window;
    }
    return NULL;
}

// Test if mouse cursor is hovering given rectangle
// NB- Rectangle is clipped by our current clip setting
// NB- Expand the rectangle to be generous on imprecise inputs systems (g.Style.TouchExtraPadding)
static bool IsMouseHoveringRect(const ImRect& rect)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    // Clip
    ImRect rect_clipped = rect;
    rect_clipped.Clip(window->ClipRect);

    // Expand for touch input
    const ImRect rect_for_touch(rect_clipped.Min - g.Style.TouchExtraPadding, rect_clipped.Max + g.Style.TouchExtraPadding);
    return rect_for_touch.Contains(g.IO.MousePos);
}

bool ImGui::IsMouseHoveringRect(const ImVec2& rect_min, const ImVec2& rect_max)
{
    return IsMouseHoveringRect(ImRect(rect_min, rect_max));
}

bool ImGui::IsMouseHoveringWindow()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    return g.HoveredWindow == window;
}

bool ImGui::IsMouseHoveringAnyWindow()
{
    ImGuiState& g = *GImGui;
    return g.HoveredWindow != NULL;
}

bool ImGui::IsPosHoveringAnyWindow(const ImVec2& pos)
{
    return FindHoveredWindow(pos, false) != NULL;
}

static bool IsKeyPressedMap(ImGuiKey key, bool repeat)
{
    const int key_index = GImGui->IO.KeyMap[key];
    return ImGui::IsKeyPressed(key_index, repeat);
}

int ImGui::GetKeyIndex(ImGuiKey key)
{
    IM_ASSERT(key >= 0 && key < ImGuiKey_COUNT);
    return GImGui->IO.KeyMap[key];
}

bool ImGui::IsKeyDown(int key_index)
{
    if (key_index < 0) return false;
    IM_ASSERT(key_index >= 0 && key_index < IM_ARRAYSIZE(GImGui->IO.KeysDown));
    return GImGui->IO.KeysDown[key_index];
}

bool ImGui::IsKeyPressed(int key_index, bool repeat)
{
    ImGuiState& g = *GImGui;
    if (key_index < 0) return false;
    IM_ASSERT(key_index >= 0 && key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    const float t = g.IO.KeysDownDuration[key_index];
    if (t == 0.0f)
        return true;

    if (repeat && t > g.IO.KeyRepeatDelay)
    {
        float delay = g.IO.KeyRepeatDelay, rate = g.IO.KeyRepeatRate;
        if ((fmodf(t - delay, rate) > rate*0.5f) != (fmodf(t - delay - g.IO.DeltaTime, rate) > rate*0.5f))
            return true;
    }
    return false;
}

bool ImGui::IsKeyReleased(int key_index)
{
    ImGuiState& g = *GImGui;
    if (key_index < 0) return false;
    IM_ASSERT(key_index >= 0 && key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    if (g.IO.KeysDownDurationPrev[key_index] >= 0.0f && !g.IO.KeysDown[key_index])
        return true;
    return false;
}

bool ImGui::IsMouseDown(int button)
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseDown[button];
}

bool ImGui::IsMouseClicked(int button, bool repeat)
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    const float t = g.IO.MouseDownDuration[button];
    if (t == 0.0f)
        return true;

    if (repeat && t > g.IO.KeyRepeatDelay)
    {
        float delay = g.IO.KeyRepeatDelay, rate = g.IO.KeyRepeatRate;
        if ((fmodf(t - delay, rate) > rate*0.5f) != (fmodf(t - delay - g.IO.DeltaTime, rate) > rate*0.5f))
            return true;
    }

    return false;
}

bool ImGui::IsMouseReleased(int button)
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseReleased[button];
}

bool ImGui::IsMouseDoubleClicked(int button)
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseDoubleClicked[button];
}

bool ImGui::IsMouseDragging(int button, float lock_threshold)
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    if (!g.IO.MouseDown[button])
        return false;
    if (lock_threshold < 0.0f)
        lock_threshold = g.IO.MouseDragThreshold;
    return g.IO.MouseDragMaxDistanceSqr[button] >= lock_threshold * lock_threshold;
}

ImVec2 ImGui::GetMousePos()
{
    return GImGui->IO.MousePos;
}

ImVec2 ImGui::GetMouseDragDelta(int button, float lock_threshold)
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    if (lock_threshold < 0.0f)
        lock_threshold = g.IO.MouseDragThreshold;
    if (g.IO.MouseDown[button])
        if (g.IO.MouseDragMaxDistanceSqr[button] >= lock_threshold * lock_threshold)
            return g.IO.MousePos - g.IO.MouseClickedPos[button];     // Assume we can only get active with left-mouse button (at the moment).
    return ImVec2(0.0f, 0.0f);
}

void ImGui::ResetMouseDragDelta(int button)
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    // NB: We don't need to reset g.IO.MouseDragMaxDistanceSqr
    g.IO.MouseClickedPos[button] = g.IO.MousePos;
}

ImGuiMouseCursor ImGui::GetMouseCursor()
{
    return GImGui->MouseCursor;
}

void ImGui::SetMouseCursor(ImGuiMouseCursor cursor_type)
{
    GImGui->MouseCursor = cursor_type;
}

void ImGui::CaptureKeyboardFromApp()
{
    GImGui->CaptureKeyboardNextFrame = true;
}

void ImGui::CaptureMouseFromApp()
{
    GImGui->CaptureMouseNextFrame = true;
}

bool ImGui::IsItemHovered()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.LastItemHoveredAndUsable;
}

bool ImGui::IsItemHoveredRect()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.LastItemHoveredRect;
}

bool ImGui::IsItemActive()
{
    ImGuiState& g = *GImGui;
    if (g.ActiveId)
    {
        ImGuiWindow* window = GetCurrentWindow();
        return g.ActiveId == window->DC.LastItemID;
    }
    return false;
}

bool ImGui::IsAnyItemHovered()
{
    return GImGui->HoveredId != 0 || GImGui->HoveredIdPreviousFrame != 0;
}

bool ImGui::IsAnyItemActive()
{
    return GImGui->ActiveId != 0;
}

bool ImGui::IsItemVisible()
{
    ImGuiWindow* window = GetCurrentWindow();
    ImRect r(window->ClipRect);
    return r.Overlaps(window->DC.LastItemRect);
}

ImVec2 ImGui::GetItemRectMin()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.LastItemRect.Min;
}

ImVec2 ImGui::GetItemRectMax()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.LastItemRect.Max;
}

ImVec2 ImGui::GetItemRectSize()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.LastItemRect.GetSize();
}

ImVec2 ImGui::CalcItemRectClosestPoint(const ImVec2& pos, bool on_edge, float outward)
{
    ImGuiWindow* window = GetCurrentWindow();
    ImRect rect = window->DC.LastItemRect;
    rect.Expand(outward);
    return rect.GetClosestPoint(pos, on_edge);
}

// Tooltip is stored and turned into a BeginTooltip()/EndTooltip() sequence at the end of the frame. Each call override previous value.
void ImGui::SetTooltipV(const char* fmt, va_list args)
{
    ImGuiState& g = *GImGui;
    ImFormatStringV(g.Tooltip, IM_ARRAYSIZE(g.Tooltip), fmt, args);
}

void ImGui::SetTooltip(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SetTooltipV(fmt, args);
    va_end(args);
}

float ImGui::GetTime()
{
    return GImGui->Time;
}

int ImGui::GetFrameCount()
{
    return GImGui->FrameCount;
}

static ImVec4 GetVisibleRect()
{
    ImGuiState& g = *GImGui;
    if (g.IO.DisplayVisibleMin.x != g.IO.DisplayVisibleMax.x && g.IO.DisplayVisibleMin.y != g.IO.DisplayVisibleMax.y)
        return ImVec4(g.IO.DisplayVisibleMin.x, g.IO.DisplayVisibleMin.y, g.IO.DisplayVisibleMax.x, g.IO.DisplayVisibleMax.y);
    return ImVec4(0.0f, 0.0f, g.IO.DisplaySize.x, g.IO.DisplaySize.y);
}

void ImGui::BeginTooltip()
{
    ImGuiState& g = *GImGui;
    ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::Begin("##Tooltip", NULL, ImVec2(0,0), g.Style.Colors[ImGuiCol_TooltipBg].w, flags);
}

void ImGui::EndTooltip()
{
    IM_ASSERT(GetCurrentWindow()->Flags & ImGuiWindowFlags_Tooltip);
    ImGui::End();
}

static bool IsPopupOpen(ImGuiID id)
{
    ImGuiState& g = *GImGui;
    const bool opened = g.OpenedPopupStack.Size > g.CurrentPopupStack.Size && g.OpenedPopupStack[g.CurrentPopupStack.Size].PopupID == id;
    return opened;
}

// One open popup per level of the popup hierarchy (NB: when assigning we reset the Window member of ImGuiPopupRef to NULL)
void ImGui::OpenPopup(const char* str_id)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiID id = window->GetID(str_id);
    int current_stack_size = g.CurrentPopupStack.Size;
    ImGuiPopupRef popup_ref = ImGuiPopupRef(id, window, window->GetID("##menus")); // Tagged as new ref because constructor sets Window to NULL (we are passing the ParentWindow info here)
    if (g.OpenedPopupStack.Size < current_stack_size + 1)
        g.OpenedPopupStack.push_back(popup_ref);
    else if (g.OpenedPopupStack[current_stack_size].PopupID != id)
    {
        g.OpenedPopupStack.resize(current_stack_size+1);
        g.OpenedPopupStack[current_stack_size] = popup_ref;
    }
}

static void CloseInactivePopups()
{
    ImGuiState& g = *GImGui;
    if (g.OpenedPopupStack.empty())
        return;

    // When popups are stacked, clicking on a lower level popups puts focus back to it and close popups above it.
    // Don't close our own child popup windows
    int n = 0;
    if (g.FocusedWindow)
    {
        for (n = 0; n < g.OpenedPopupStack.Size; n++)
        {
            ImGuiPopupRef& popup = g.OpenedPopupStack[n];
            if (!popup.Window)
                continue;
            IM_ASSERT((popup.Window->Flags & ImGuiWindowFlags_Popup) != 0);
            if (popup.Window->Flags & ImGuiWindowFlags_ChildWindow)
                continue;

            bool has_focus = false;
            for (int m = n; m < g.OpenedPopupStack.Size && !has_focus; m++)
                has_focus = (g.OpenedPopupStack[m].Window && g.OpenedPopupStack[m].Window->RootWindow == g.FocusedWindow->RootWindow);
            if (!has_focus)
                break;
        }
    }
    if (n < g.OpenedPopupStack.Size)   // This test is not required but it allows to set a useful breakpoint on the line below
        g.OpenedPopupStack.resize(n);
}

static ImGuiWindow* GetFrontMostModalRootWindow()
{
    ImGuiState& g = *GImGui;
    if (!g.OpenedPopupStack.empty())
        if (ImGuiWindow* front_most_popup = g.OpenedPopupStack.back().Window)
            if (front_most_popup->Flags & ImGuiWindowFlags_Modal)
                return front_most_popup;
    return NULL;
}

static void ClosePopupToLevel(int remaining)
{
    ImGuiState& g = *GImGui;
    if (remaining > 0)
        FocusWindow(g.OpenedPopupStack[remaining-1].Window);
    else
        FocusWindow(g.OpenedPopupStack[0].ParentWindow);
    g.OpenedPopupStack.resize(remaining);
}

static void ClosePopup(ImGuiID id)
{
    if (!IsPopupOpen(id))
        return;
    ImGuiState& g = *GImGui;
    ClosePopupToLevel(g.OpenedPopupStack.Size - 1);
}

// Close the popup we have begin-ed into.
void ImGui::CloseCurrentPopup()
{
    ImGuiState& g = *GImGui;
    int popup_idx = g.CurrentPopupStack.Size - 1;
    if (popup_idx < 0 || popup_idx > g.OpenedPopupStack.Size || g.CurrentPopupStack[popup_idx].PopupID != g.OpenedPopupStack[popup_idx].PopupID)
        return;
    while (popup_idx > 0 && g.OpenedPopupStack[popup_idx].Window && (g.OpenedPopupStack[popup_idx].Window->Flags & ImGuiWindowFlags_ChildMenu))
        popup_idx--;
    ClosePopupToLevel(popup_idx);
}

static void ClearSetNextWindowData()
{
    ImGuiState& g = *GImGui;
    g.SetNextWindowPosCond = g.SetNextWindowSizeCond = g.SetNextWindowCollapsedCond = g.SetNextWindowFocus = 0;
}

static bool BeginPopupEx(const char* str_id, ImGuiWindowFlags extra_flags)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    const ImGuiID id = window->GetID(str_id);
    if (!IsPopupOpen(id))
    {
        ClearSetNextWindowData(); // We behave like Begin() and need to consume those values
        return false;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGuiWindowFlags flags = extra_flags|ImGuiWindowFlags_Popup|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_AlwaysAutoResize;

    char name[32];
    if (flags & ImGuiWindowFlags_ChildMenu)
        ImFormatString(name, 20, "##menu_%d", g.CurrentPopupStack.Size);    // Recycle windows based on depth
    else
        ImFormatString(name, 20, "##popup_%08x", id); // Not recycling, so we can close/open during the same frame
    float alpha = 1.0f;

    bool opened = ImGui::Begin(name, NULL, ImVec2(0.0f, 0.0f), alpha, flags);
    if (!(window->Flags & ImGuiWindowFlags_ShowBorders))
        GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;
    if (!opened) // opened can be 'false' when the popup is completely clipped (e.g. zero size display)
        ImGui::EndPopup();

    return opened;
}

bool ImGui::BeginPopup(const char* str_id)
{
    return BeginPopupEx(str_id, ImGuiWindowFlags_ShowBorders);
}

bool ImGui::BeginPopupModal(const char* name, bool* p_opened, ImGuiWindowFlags extra_flags)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    const ImGuiID id = window->GetID(name);
    if (!IsPopupOpen(id))
    {
        ClearSetNextWindowData(); // We behave like Begin() and need to consume those values
        return false;
    }

    ImGuiWindowFlags flags = extra_flags|ImGuiWindowFlags_Popup|ImGuiWindowFlags_Modal|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings;
    bool opened = ImGui::Begin(name, p_opened, ImVec2(0.0f, 0.0f), -1.0f, flags);
    if (!opened || (p_opened && !*p_opened)) // Opened can be 'false' when the popup is completely clipped (e.g. zero size display)
    {
        ImGui::EndPopup();
        if (opened)
            ClosePopup(id);
        return false;
    }

    return opened;
}

void ImGui::EndPopup()
{
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(window->Flags & ImGuiWindowFlags_Popup);
    IM_ASSERT(GImGui->CurrentPopupStack.Size > 0);
    ImGui::End();
    if (!(window->Flags & ImGuiWindowFlags_Modal))
        ImGui::PopStyleVar();
}

bool ImGui::BeginPopupContextItem(const char* str_id, int mouse_button)
{
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(mouse_button))
        ImGui::OpenPopup(str_id);
    return ImGui::BeginPopup(str_id);
}

bool ImGui::BeginPopupContextWindow(bool also_over_items, const char* str_id, int mouse_button)
{
    if (!str_id) str_id = "window_context_menu";
    if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(mouse_button))
        if (also_over_items || !ImGui::IsAnyItemHovered())
            ImGui::OpenPopup(str_id);
    return ImGui::BeginPopup(str_id);
}

bool ImGui::BeginPopupContextVoid(const char* str_id, int mouse_button)
{
    if (!str_id) str_id = "void_context_menu";
    if (!ImGui::IsMouseHoveringAnyWindow() && ImGui::IsMouseClicked(mouse_button))
        ImGui::OpenPopup(str_id);
    return ImGui::BeginPopup(str_id);
}

bool ImGui::BeginChild(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_ChildWindow;

    const ImVec2 content_max = window->Pos + ImGui::GetContentRegionMax();
    const ImVec2 cursor_pos = window->Pos + ImGui::GetCursorPos();
    ImVec2 size = size_arg;
    if (size.x <= 0.0f)
    {
        if (size.x == 0.0f)
            flags |= ImGuiWindowFlags_ChildWindowAutoFitX;
        size.x = ImMax(content_max.x - cursor_pos.x, 4.0f) - fabsf(size.x); // Arbitrary minimum zeroish child size of 4.0f
    }
    if (size.y <= 0.0f)
    {
        if (size.y == 0.0f)
            flags |= ImGuiWindowFlags_ChildWindowAutoFitY;
        size.y = ImMax(content_max.y - cursor_pos.y, 4.0f) - fabsf(size.y);
    }
    if (border)
        flags |= ImGuiWindowFlags_ShowBorders;
    flags |= extra_flags;

    char title[256];
    ImFormatString(title, IM_ARRAYSIZE(title), "%s.%s", window->Name, str_id);

    const float alpha = 1.0f;
    bool ret = ImGui::Begin(title, NULL, size, alpha, flags);

    if (!(window->Flags & ImGuiWindowFlags_ShowBorders))
        GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

    return ret;
}

bool ImGui::BeginChild(ImGuiID id, const ImVec2& size, bool border, ImGuiWindowFlags extra_flags)
{
    char str_id[32];
    ImFormatString(str_id, IM_ARRAYSIZE(str_id), "child_%08x", id);
    bool ret = ImGui::BeginChild(str_id, size, border, extra_flags);
    return ret;
}

void ImGui::EndChild()
{
    ImGuiWindow* window = GetCurrentWindow();

    IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);
    if ((window->Flags & ImGuiWindowFlags_ComboBox) || window->BeginCount > 1)
    {
        ImGui::End();
    }
    else
    {
        // When using auto-filling child window, we don't provide full width/height to ItemSize so that it doesn't feed back into automatic size-fitting.
        ImGuiState& g = *GImGui;
        ImVec2 sz = ImGui::GetWindowSize();
        if (window->Flags & ImGuiWindowFlags_ChildWindowAutoFitX) // Arbitrary minimum zeroish child size of 4.0f
            sz.x = ImMax(4.0f, sz.x - g.Style.WindowPadding.x);
        if (window->Flags & ImGuiWindowFlags_ChildWindowAutoFitY)
            sz.y = ImMax(4.0f, sz.y - g.Style.WindowPadding.y);
        
        ImGui::End();

        window = GetCurrentWindow();
        ImRect bb(window->DC.CursorPos, window->DC.CursorPos + sz);
        ItemSize(sz);
        ItemAdd(bb, NULL);
    }
}

// Helper to create a child window / scrolling region that looks like a normal widget frame.
bool ImGui::BeginChildFrame(ImGuiID id, const ImVec2& size)
{
    ImGuiState& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, style.Colors[ImGuiCol_FrameBg]);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, style.FrameRounding);
    return ImGui::BeginChild(id, size);
}

void ImGui::EndChildFrame()
{
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// Save and compare stack sizes on Begin()/End() to detect usage errors
static void CheckStacksSize(ImGuiWindow* window, bool write)
{
    // NOT checking: DC.ItemWidth, DC.AllowKeyboardFocus, DC.ButtonRepeat, DC.TextWrapPos (per window) to allow user to conveniently push once and not pop (they are cleared on Begin)
    ImGuiState& g = *GImGui;
    int* p_backup = &window->DC.StackSizesBackup[0];
    { int current = window->IDStack.Size;       if (write) *p_backup = current; else IM_ASSERT(*p_backup == current); p_backup++; }    // User forgot PopID()
    { int current = window->DC.GroupStack.Size; if (write) *p_backup = current; else IM_ASSERT(*p_backup == current); p_backup++; }    // User forgot EndGroup()
    { int current = g.CurrentPopupStack.Size;   if (write) *p_backup = current; else IM_ASSERT(*p_backup == current); p_backup++; }    // User forgot EndPopup()/EndMenu()
    { int current = g.ColorModifiers.Size;      if (write) *p_backup = current; else IM_ASSERT(*p_backup == current); p_backup++; }    // User forgot PopStyleColor()
    { int current = g.StyleModifiers.Size;      if (write) *p_backup = current; else IM_ASSERT(*p_backup == current); p_backup++; }    // User forgot PopStyleVar()
    { int current = g.FontStack.Size;           if (write) *p_backup = current; else IM_ASSERT(*p_backup == current); p_backup++; }    // User forgot PopFont()
    IM_ASSERT(p_backup == window->DC.StackSizesBackup + IM_ARRAYSIZE(window->DC.StackSizesBackup));
}

static ImVec2 FindBestPopupWindowPos(const ImVec2& base_pos, const ImVec2& size, ImGuiWindowFlags flags, int* last_dir, const ImRect& r_inner)
{
    const ImGuiStyle& style = GImGui->Style;

    // Clamp into visible area while not overlapping the cursor
    ImRect r_outer(GetVisibleRect()); 
    r_outer.Reduce(style.DisplaySafeAreaPadding);
    ImVec2 base_pos_clamped = ImClamp(base_pos, r_outer.Min, r_outer.Max - size);

    for (int n = (*last_dir != -1) ? -1 : 0; n < 4; n++)   // Right, down, up, left. Favor last used direction.
    {
        const int dir = (n == -1) ? *last_dir : n;
        ImRect rect(dir == 0 ? r_inner.Max.x : r_outer.Min.x, dir == 1 ? r_inner.Max.y : r_outer.Min.y, dir == 3 ? r_inner.Min.x : r_outer.Max.x, dir == 2 ? r_inner.Min.y : r_outer.Max.y);
        if (rect.GetWidth() < size.x || rect.GetHeight() < size.y)
            continue;
        *last_dir = dir;
        return ImVec2(dir == 0 ? r_inner.Max.x : dir == 3 ? r_inner.Min.x - size.x : base_pos_clamped.x, dir == 1 ? r_inner.Max.y : dir == 2 ? r_inner.Min.y - size.y : base_pos_clamped.y);
    }

    // Fallback
    *last_dir = -1;
    if (flags & ImGuiWindowFlags_Tooltip) // For tooltip we prefer avoiding the cursor at all cost even if it means that part of the tooltip won't be visible.
        return base_pos + ImVec2(2,2);

    // Otherwise try to keep within display
    ImVec2 pos = base_pos;
    pos.x = ImMax(ImMin(pos.x + size.x, r_outer.Max.x) - size.x, r_outer.Min.x);
    pos.y = ImMax(ImMin(pos.y + size.y, r_outer.Max.y) - size.y, r_outer.Min.y);
    return pos;
}

static ImGuiWindow* FindWindowByName(const char* name)
{
    // FIXME-OPT: Store sorted hashes -> pointers.
    ImGuiState& g = *GImGui;
    ImGuiID id = ImHash(name, 0);
    for (int i = 0; i < g.Windows.Size; i++)
        if (g.Windows[i]->ID == id)
            return g.Windows[i];
    return NULL;
}

static ImGuiWindow* CreateNewWindow(const char* name, ImVec2 size, ImGuiWindowFlags flags)
{
    ImGuiState& g = *GImGui;

    // Create window the first time
    ImGuiWindow* window = (ImGuiWindow*)ImGui::MemAlloc(sizeof(ImGuiWindow));
    new(window) ImGuiWindow(name);
    window->Flags = flags;

    if (flags & ImGuiWindowFlags_NoSavedSettings)
    {
        // User can disable loading and saving of settings. Tooltip and child windows also don't store settings.
        window->Size = window->SizeFull = size;
    }
    else
    {
        // Retrieve settings from .ini file
        // Use SetWindowPos() or SetNextWindowPos() with the appropriate condition flag to change the initial position of a window.
        window->PosFloat = ImVec2(60, 60);
        window->Pos = ImVec2((float)(int)window->PosFloat.x, (float)(int)window->PosFloat.y);

        ImGuiIniData* settings = FindWindowSettings(name);
        if (!settings)
        {
            settings = AddWindowSettings(name);
        }
        else
        {
            window->SetWindowPosAllowFlags &= ~ImGuiSetCond_FirstUseEver;
            window->SetWindowSizeAllowFlags &= ~ImGuiSetCond_FirstUseEver;
            window->SetWindowCollapsedAllowFlags &= ~ImGuiSetCond_FirstUseEver;
        }

        if (settings->Pos.x != FLT_MAX)
        {
            window->PosFloat = settings->Pos;
            window->Pos = ImVec2((float)(int)window->PosFloat.x, (float)(int)window->PosFloat.y);
            window->Collapsed = settings->Collapsed;
        }

        if (ImLengthSqr(settings->Size) > 0.00001f && !(flags & ImGuiWindowFlags_NoResize))
            size = settings->Size;
        window->Size = window->SizeFull = size;
    }

    if ((flags & ImGuiWindowFlags_AlwaysAutoResize) != 0)
    {
        window->AutoFitFramesX = window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = false;
    }
    else
    {
        if (window->Size.x <= 0.0f)
            window->AutoFitFramesX = 2;
        if (window->Size.y <= 0.0f)
            window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = (window->AutoFitFramesX > 0) || (window->AutoFitFramesY > 0);
    }

    g.Windows.push_back(window);
    return window;
}

// Push a new ImGui window to add widgets to. 
// - A default window called "Debug" is automatically stacked at the beginning of every frame so you can use widgets without explicitly calling a Begin/End pair.
// - Begin/End can be called multiple times during the frame with the same window name to append content.
// - 'size_on_first_use' for a regular window denote the initial size for first-time creation (no saved data) and isn't that useful. Use SetNextWindowSize() prior to calling Begin() for more flexible window manipulation.
// - The window name is used as a unique identifier to preserve window information across frames (and save rudimentary information to the .ini file). 
//   You can use the "##" or "###" markers to use the same label with different id, or same id with different label. See documentation at the top of this file.
// - Return false when window is collapsed, so you can early out in your code. You always need to call ImGui::End() even if false is returned.
// - Passing 'bool* p_opened' displays a Close button on the upper-right corner of the window, the pointed value will be set to false when the button is pressed.
// - Passing non-zero 'size' is roughly equivalent to calling SetNextWindowSize(size, ImGuiSetCond_FirstUseEver) prior to calling Begin().
bool ImGui::Begin(const char* name, bool* p_opened, ImGuiWindowFlags flags)
{
    return ImGui::Begin(name, p_opened, ImVec2(0.f, 0.f), -1.0f, flags);
}

bool ImGui::Begin(const char* name, bool* p_opened, const ImVec2& size_on_first_use, float bg_alpha, ImGuiWindowFlags flags)
{
    ImGuiState& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    IM_ASSERT(g.Initialized);                       // Forgot to call ImGui::NewFrame()
    IM_ASSERT(name != NULL);                        // Must pass a name

    if (flags & ImGuiWindowFlags_NoInputs)
        flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    // Find or create
    bool window_is_new = false;
    ImGuiWindow* window = FindWindowByName(name);
    if (!window)
    {
        window = CreateNewWindow(name, size_on_first_use, flags);
        window_is_new = true;
    }
    window->Flags = (ImGuiWindowFlags)flags;
        
    // Add to stack
    ImGuiWindow* parent_window = !g.CurrentWindowStack.empty() ? g.CurrentWindowStack.back() : NULL;
    g.CurrentWindowStack.push_back(window);
    SetCurrentWindow(window);
    CheckStacksSize(window, true);
    IM_ASSERT(parent_window != NULL || !(flags & ImGuiWindowFlags_ChildWindow));

    const int current_frame = ImGui::GetFrameCount();
    bool window_was_visible = (window->LastFrameDrawn == current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
    if (flags & ImGuiWindowFlags_Popup)
    {
        ImGuiPopupRef& popup_ref = g.OpenedPopupStack[g.CurrentPopupStack.Size];
        window_was_visible &= (window->PopupID == popup_ref.PopupID);
        window_was_visible &= (window == popup_ref.Window);
        popup_ref.Window = window;
        g.CurrentPopupStack.push_back(popup_ref);
        window->PopupID = popup_ref.PopupID;
    }

    // Process SetNextWindow***() calls
    bool window_pos_set_by_api = false, window_size_set_by_api = false;
    if (g.SetNextWindowPosCond)
    {
        const ImVec2 backup_cursor_pos = window->DC.CursorPos;                  // FIXME: not sure of the exact reason of this anymore :( need to look into that.
        if (!window_was_visible) window->SetWindowPosAllowFlags |= ImGuiSetCond_Appearing;
        window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.SetNextWindowPosCond) != 0;
        if (window_pos_set_by_api && ImLengthSqr(g.SetNextWindowPosVal - ImVec2(-FLT_MAX,-FLT_MAX)) < 0.001f)
        {
            window->SetWindowPosCenterWanted = true;                            // May be processed on the next frame if this is our first frame and we are measuring size
            window->SetWindowPosAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);
        }
        else
        {
            ImGui::SetWindowPos(g.SetNextWindowPosVal, g.SetNextWindowPosCond);
        }
        window->DC.CursorPos = backup_cursor_pos;
        g.SetNextWindowPosCond = 0;
    }
    if (g.SetNextWindowSizeCond)
    {
        if (!window_was_visible) window->SetWindowSizeAllowFlags |= ImGuiSetCond_Appearing;
        window_size_set_by_api = (window->SetWindowSizeAllowFlags & g.SetNextWindowSizeCond) != 0;
        ImGui::SetWindowSize(g.SetNextWindowSizeVal, g.SetNextWindowSizeCond);
        g.SetNextWindowSizeCond = 0;
    }
    if (g.SetNextWindowCollapsedCond)
    {
        if (!window_was_visible) window->SetWindowCollapsedAllowFlags |= ImGuiSetCond_Appearing;
        ImGui::SetWindowCollapsed(g.SetNextWindowCollapsedVal, g.SetNextWindowCollapsedCond);
        g.SetNextWindowCollapsedCond = 0;
    }
    if (g.SetNextWindowFocus)
    {
        ImGui::SetWindowFocus();
        g.SetNextWindowFocus = false;
    }

    // Update known root window (if we are a child window, otherwise window == window->RootWindow)
    int root_idx, root_non_popup_idx;
    for (root_idx = g.CurrentWindowStack.Size - 1; root_idx > 0; root_idx--)
        if (!(g.CurrentWindowStack[root_idx]->Flags & ImGuiWindowFlags_ChildWindow))
            break;
    for (root_non_popup_idx = root_idx; root_non_popup_idx > 0; root_non_popup_idx--)
        if (!(g.CurrentWindowStack[root_non_popup_idx]->Flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Popup)))
            break;
    window->RootWindow = g.CurrentWindowStack[root_idx];
    window->RootNonPopupWindow = g.CurrentWindowStack[root_non_popup_idx];      // This is merely for displaying the TitleBgActive color.

    // Default alpha
    if (bg_alpha < 0.0f)
        bg_alpha = style.WindowFillAlphaDefault;

    // When reusing window again multiple times a frame, just append content (don't need to setup again)
    const bool first_begin_of_the_frame = (window->LastFrameDrawn != current_frame);
    if (first_begin_of_the_frame)
    {
        window->Active = true;
        window->BeginCount = 0;
        window->DrawList->Clear();
        window->ClipRect = ImVec4(-FLT_MAX,-FLT_MAX,+FLT_MAX,+FLT_MAX);
        window->LastFrameDrawn = current_frame;
        window->IDStack.resize(1);

        // Setup texture, outer clipping rectangle
        window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
        if ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & (ImGuiWindowFlags_ComboBox|ImGuiWindowFlags_Popup)))
            PushClipRect(parent_window->ClipRect);
        else
            PushClipRect(GetVisibleRect());

        // New windows appears in front
        if (!window_was_visible)
        {
            window->AutoPosLastDirection = -1;

            if (!(flags & (ImGuiWindowFlags_ChildWindow|ImGuiWindowFlags_Tooltip)) || (flags & ImGuiWindowFlags_Popup))
                FocusWindow(window);

            // Popup first latch mouse position, will position itself when it appears next frame
            if ((flags & ImGuiWindowFlags_Popup) != 0 && !window_pos_set_by_api)
                window->PosFloat = g.IO.MousePos;
        }

        // Collapse window by double-clicking on title bar
        // At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
        if (!(flags & ImGuiWindowFlags_NoTitleBar) && !(flags & ImGuiWindowFlags_NoCollapse))
        {
            if (g.HoveredWindow == window && IsMouseHoveringRect(window->TitleBarRect()) && g.IO.MouseDoubleClicked[0])
            {
                window->Collapsed = !window->Collapsed;
                if (!(flags & ImGuiWindowFlags_NoSavedSettings))
                    MarkSettingsDirty();
                FocusWindow(window);
            }
        }
        else
        {
            window->Collapsed = false;
        }

        const bool window_appearing_after_being_hidden = (window->HiddenFrames == 1);
        if (window->HiddenFrames > 0)
            window->HiddenFrames--;

        // SIZE

        // Save contents size from last frame for auto-fitting
        window->SizeContents = window_is_new ? ImVec2(0.0f, 0.0f) : window->DC.CursorMaxPos - window->Pos;
        window->SizeContents.y += window->ScrollY;

        // Hide popup/tooltip window when first appearing while we measure size (because we recycle them)
        if ((flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_Tooltip)) != 0 && !window_was_visible)
        {
            window->HiddenFrames = 1;
            if (flags & ImGuiWindowFlags_AlwaysAutoResize)
            {
                if (!window_size_set_by_api)
                    window->Size = window->SizeFull = ImVec2(0.f, 0.f);
                window->SizeContents = ImVec2(0.f, 0.f);
            }
        }

        // Lock window padding so that altering the ShowBorders flag for childs doesn't have side-effects.
        window->WindowPadding = ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & (ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_ComboBox | ImGuiWindowFlags_Popup))) ? ImVec2(0,0) : style.WindowPadding;

        // Calculate auto-fit size
        ImVec2 size_auto_fit;
        if ((flags & ImGuiWindowFlags_Tooltip) != 0)
        {
            // Tooltip always resize. We keep the spacing symmetric on both axises for aesthetic purpose.
            size_auto_fit = window->SizeContents + window->WindowPadding - ImVec2(0.0f, style.ItemSpacing.y);
        }
        else
        {
            size_auto_fit = ImClamp(window->SizeContents + window->WindowPadding, style.WindowMinSize, ImMax(style.WindowMinSize, g.IO.DisplaySize - window->WindowPadding));
            if (size_auto_fit.y < window->SizeContents.y && !(flags & ImGuiWindowFlags_NoScrollbar))
                size_auto_fit.x += style.ScrollbarWidth;
            size_auto_fit.y = ImMax(size_auto_fit.y - style.ItemSpacing.y, 0.0f);
        }

        // Handle automatic resize
        if (window->Collapsed)
        {
            // We still process initial auto-fit on collapsed windows to get a window width,
            // But otherwise we don't honor ImGuiWindowFlags_AlwaysAutoResize when collapsed.
            if (window->AutoFitFramesX > 0)
                window->SizeFull.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
            if (window->AutoFitFramesY > 0)
                window->SizeFull.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
            window->Size = window->TitleBarRect().GetSize();
        }
        else
        {
            if ((flags & ImGuiWindowFlags_AlwaysAutoResize) && !window_size_set_by_api)
            {
                window->SizeFull = size_auto_fit;
            }
            else if ((window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0) && !window_size_set_by_api)
            {
                // Auto-fit only grows during the first few frames
                if (window->AutoFitFramesX > 0)
                    window->SizeFull.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
                if (window->AutoFitFramesY > 0)
                    window->SizeFull.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
                if (!(flags & ImGuiWindowFlags_NoSavedSettings))
                    MarkSettingsDirty();
            }
            window->Size = window->SizeFull;
        }

        // Minimum window size
        if (!(flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_AlwaysAutoResize)))
        {
            window->SizeFull = ImMax(window->SizeFull, style.WindowMinSize);
            if (!window->Collapsed)
                window->Size = window->SizeFull;
        }

        // POSITION

        // Position child window
        if (flags & ImGuiWindowFlags_ChildWindow)
            parent_window->DC.ChildWindows.push_back(window);
        if ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & ImGuiWindowFlags_Popup))
        {
            window->Pos = window->PosFloat = parent_window->DC.CursorPos;
            window->Size = window->SizeFull = size_on_first_use; // NB: argument name 'size_on_first_use' misleading here, it's really just 'size' as provided by user.
        }

        bool window_pos_center = false;
        window_pos_center |= (window->SetWindowPosCenterWanted && window->HiddenFrames == 0);
        window_pos_center |= ((flags & ImGuiWindowFlags_Modal) && !window_pos_set_by_api && window_appearing_after_being_hidden);
        if (window_pos_center)
        {
            // Center (any sort of window)
            ImRect fullscreen_rect(GetVisibleRect());
            SetWindowPos(ImMax(style.DisplaySafeAreaPadding, fullscreen_rect.GetCenter() - window->SizeFull * 0.5f));
        }
        else if (flags & ImGuiWindowFlags_ChildMenu)
        {
            IM_ASSERT(window_pos_set_by_api);
            ImRect rect_to_avoid;
            if (parent_window->DC.MenuBarAppending)
                rect_to_avoid = ImRect(-FLT_MAX, parent_window->Pos.y + parent_window->TitleBarHeight(), FLT_MAX, parent_window->Pos.y + parent_window->TitleBarHeight() + parent_window->MenuBarHeight());
            else
                rect_to_avoid = ImRect(parent_window->Pos.x + style.ItemSpacing.x, -FLT_MAX, parent_window->Pos.x + parent_window->Size.x - style.ItemSpacing.x - parent_window->ScrollbarWidth(), FLT_MAX); // We want some overlap to convey the relative depth of each popup (here hard-coded to 4)
            window->PosFloat = FindBestPopupWindowPos(window->PosFloat, window->Size, flags, &window->AutoPosLastDirection, rect_to_avoid);
        }
        else if ((flags & ImGuiWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_appearing_after_being_hidden)
        {
            ImRect rect_to_avoid(window->PosFloat.x - 1, window->PosFloat.y - 1, window->PosFloat.x + 1, window->PosFloat.y + 1);
            window->PosFloat = FindBestPopupWindowPos(window->PosFloat, window->Size, flags, &window->AutoPosLastDirection, rect_to_avoid);
        }

        // Position tooltip (always follows mouse)
        if ((flags & ImGuiWindowFlags_Tooltip) != 0 && !window_pos_set_by_api)
        {
            ImRect rect_to_avoid(g.IO.MousePos.x - 16, g.IO.MousePos.y - 8, g.IO.MousePos.x + 24, g.IO.MousePos.y + 24); // FIXME: Completely hard-coded. Perhaps center on cursor hit-point instead?
            window->PosFloat = FindBestPopupWindowPos(g.IO.MousePos, window->Size, flags, &window->AutoPosLastDirection, rect_to_avoid);
        }

        // User moving window (at the beginning of the frame to avoid input lag or sheering). Only valid for root windows.
        RegisterAliveId(window->MoveID);
        if (g.ActiveId == window->MoveID)
        {
            if (g.IO.MouseDown[0])
            {
                if (!(flags & ImGuiWindowFlags_NoMove))
                {
                    window->PosFloat += g.IO.MouseDelta;
                    if (!(flags & ImGuiWindowFlags_NoSavedSettings))
                        MarkSettingsDirty();
                }
                IM_ASSERT(g.MovedWindow != NULL);
                FocusWindow(g.MovedWindow);
            }
            else
            {
                SetActiveId(0);
                g.MovedWindow = NULL;   // Not strictly necessary but doing it for sanity.
            }
        }

        // Clamp position so it stays visible
        if (!(flags & ImGuiWindowFlags_ChildWindow) && !(flags & ImGuiWindowFlags_Tooltip))
        {
            if (!window_pos_set_by_api && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
            {
                ImVec2 padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
                window->PosFloat = ImMax(window->PosFloat + window->Size, padding) - window->Size;
                window->PosFloat = ImMin(window->PosFloat, g.IO.DisplaySize - padding);
            }
        }
        window->Pos = ImVec2((float)(int)window->PosFloat.x, (float)(int)window->PosFloat.y);

        // Default item width. Make it proportional to window size if window manually resizes
        if (window->Size.x > 0.0f && !(flags & ImGuiWindowFlags_Tooltip) && !(flags & ImGuiWindowFlags_AlwaysAutoResize))
            window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
        else
            window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

        // Prepare for focus requests
        window->FocusIdxAllRequestCurrent = (window->FocusIdxAllRequestNext == IM_INT_MAX || window->FocusIdxAllCounter == -1) ? IM_INT_MAX : (window->FocusIdxAllRequestNext + (window->FocusIdxAllCounter+1)) % (window->FocusIdxAllCounter+1);
        window->FocusIdxTabRequestCurrent = (window->FocusIdxTabRequestNext == IM_INT_MAX || window->FocusIdxTabCounter == -1) ? IM_INT_MAX : (window->FocusIdxTabRequestNext + (window->FocusIdxTabCounter+1)) % (window->FocusIdxTabCounter+1);
        window->FocusIdxAllCounter = window->FocusIdxTabCounter = -1;
        window->FocusIdxAllRequestNext = window->FocusIdxTabRequestNext = IM_INT_MAX;

        // Apply scrolling
        if (window->ScrollTargetRelY < FLT_MAX)
        {
            float center_ratio_y = window->ScrollTargetCenterRatioY;
            window->ScrollY = window->ScrollTargetRelY - ((1.0f - center_ratio_y) * window->TitleBarHeight()) - (center_ratio_y * window->SizeFull.y);
            window->ScrollTargetRelY = FLT_MAX;
        }
        window->ScrollY = ImMax(window->ScrollY, 0.0f);
        if (!window->Collapsed && !window->SkipItems)
            window->ScrollY = ImMin(window->ScrollY, ImMax(0.0f, window->SizeContents.y - window->SizeFull.y));

        // Modal window darkens what is behind them
        if ((flags & ImGuiWindowFlags_Modal) != 0 && window == GetFrontMostModalRootWindow())
        {
            ImRect fullscreen_rect = GetVisibleRect();
            window->DrawList->AddRectFilled(fullscreen_rect.Min, fullscreen_rect.Max, window->Color(ImGuiCol_ModalWindowDarkening, g.ModalWindowDarkeningRatio));
        }

        // Draw window + handle manual resize
        ImRect title_bar_rect = window->TitleBarRect();
        const float window_rounding = (flags & ImGuiWindowFlags_ChildWindow) ? style.ChildWindowRounding : style.WindowRounding;
        if (window->Collapsed)
        {
            // Draw title bar only
            window->DrawList->AddRectFilled(title_bar_rect.GetTL(), title_bar_rect.GetBR(), window->Color(ImGuiCol_TitleBgCollapsed), window_rounding);
            if (flags & ImGuiWindowFlags_ShowBorders)
            {
                window->DrawList->AddRect(title_bar_rect.GetTL()+ImVec2(1,1), title_bar_rect.GetBR()+ImVec2(1,1), window->Color(ImGuiCol_BorderShadow), window_rounding);
                window->DrawList->AddRect(title_bar_rect.GetTL(), title_bar_rect.GetBR(), window->Color(ImGuiCol_Border), window_rounding);
            }
        }
        else
        {
            ImU32 resize_col = 0;
            const float resize_corner_size = ImMax(g.FontSize * 1.35f, window_rounding + 1.0f + g.FontSize * 0.2f);
            if (!(flags & ImGuiWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && !(flags & ImGuiWindowFlags_NoResize))
            {
                // Manual resize
                const ImVec2 br = window->Rect().GetBR();
                const ImRect resize_rect(br - ImVec2(resize_corner_size * 0.75f, resize_corner_size * 0.75f), br);
                const ImGuiID resize_id = window->GetID("#RESIZE");
                bool hovered, held;
                ButtonBehavior(resize_rect, resize_id, &hovered, &held, true, ImGuiButtonFlags_FlattenChilds);
                resize_col = window->Color(held ? ImGuiCol_ResizeGripActive : hovered ? ImGuiCol_ResizeGripHovered : ImGuiCol_ResizeGrip);

                if (hovered || held)
                    g.MouseCursor = ImGuiMouseCursor_ResizeNWSE;

                if (g.HoveredWindow == window && held && g.IO.MouseDoubleClicked[0])
                {
                    // Manual auto-fit when double-clicking
                    window->SizeFull = size_auto_fit;
                    if (!(flags & ImGuiWindowFlags_NoSavedSettings))
                        MarkSettingsDirty();
                    SetActiveId(0);
                }
                else if (held)
                {
                    window->SizeFull = ImMax(window->SizeFull + g.IO.MouseDelta, style.WindowMinSize);
                    if (!(flags & ImGuiWindowFlags_NoSavedSettings))
                        MarkSettingsDirty();
                }

                window->Size = window->SizeFull;
                title_bar_rect = window->TitleBarRect();
            }

            // Scrollbar
            window->ScrollbarY = (window->SizeContents.y > window->Size.y + style.ItemSpacing.y) && !(flags & ImGuiWindowFlags_NoScrollbar);

            // Window background
            if (bg_alpha > 0.0f)
            {
                if ((flags & ImGuiWindowFlags_ComboBox) != 0)
                    window->DrawList->AddRectFilled(window->Pos, window->Pos+window->Size, window->Color(ImGuiCol_ComboBg, bg_alpha), window_rounding);
                else if ((flags & ImGuiWindowFlags_Tooltip) != 0)
                    window->DrawList->AddRectFilled(window->Pos, window->Pos+window->Size, window->Color(ImGuiCol_TooltipBg, bg_alpha), window_rounding);
                else if ((flags & ImGuiWindowFlags_Popup) != 0)
                    window->DrawList->AddRectFilled(window->Pos, window->Pos+window->Size, window->Color(ImGuiCol_WindowBg, bg_alpha), window_rounding);
                else if ((flags & ImGuiWindowFlags_ChildWindow) != 0)
                    window->DrawList->AddRectFilled(window->Pos, window->Pos+window->Size-ImVec2(window->ScrollbarWidth(),0.0f), window->Color(ImGuiCol_ChildWindowBg, bg_alpha), window_rounding, window->ScrollbarY ? (1|8) : (0xF));
                else
                    window->DrawList->AddRectFilled(window->Pos, window->Pos+window->Size, window->Color(ImGuiCol_WindowBg, bg_alpha), window_rounding);
            }

            // Title bar
            if (!(flags & ImGuiWindowFlags_NoTitleBar))
                window->DrawList->AddRectFilled(title_bar_rect.GetTL(), title_bar_rect.GetBR(), window->Color((g.FocusedWindow && window->RootNonPopupWindow == g.FocusedWindow->RootNonPopupWindow) ? ImGuiCol_TitleBgActive : ImGuiCol_TitleBg), window_rounding, 1|2);

            // Menu bar
            if (flags & ImGuiWindowFlags_MenuBar)
            {
                ImRect menu_bar_rect = window->MenuBarRect();
                window->DrawList->AddRectFilled(menu_bar_rect.GetTL(), menu_bar_rect.GetBR(), window->Color(ImGuiCol_MenuBarBg), (flags & ImGuiWindowFlags_NoTitleBar) ? window_rounding : 0.0f, 1|2);
            }

            // Borders
            if (flags & ImGuiWindowFlags_ShowBorders)
            {
                window->DrawList->AddRect(window->Pos+ImVec2(1,1), window->Pos+window->Size+ImVec2(1,1), window->Color(ImGuiCol_BorderShadow), window_rounding);
                window->DrawList->AddRect(window->Pos, window->Pos+window->Size, window->Color(ImGuiCol_Border), window_rounding);
                if (!(flags & ImGuiWindowFlags_NoTitleBar))
                    window->DrawList->AddLine(title_bar_rect.GetBL(), title_bar_rect.GetBR(), window->Color(ImGuiCol_Border));
            }

            // Scrollbar
            if (window->ScrollbarY)
                Scrollbar(window);

            // Render resize grip
            // (after the input handling so we don't have a frame of latency)
            if (!(flags & ImGuiWindowFlags_NoResize))
            {
                const ImVec2 br = window->Rect().GetBR();
                window->DrawList->PathLineTo(br + ImVec2(-resize_corner_size, 0.0f));
                window->DrawList->PathLineTo(br + ImVec2(0.0f, -resize_corner_size));
                window->DrawList->PathArcToFast(ImVec2(br.x - window_rounding, br.y - window_rounding), window_rounding, 0, 3);
                window->DrawList->PathFill(resize_col);
            }
        }

        // Setup drawing context
        window->DC.ColumnsStartX = window->WindowPadding.x;
        window->DC.ColumnsOffsetX = 0.0f;
        window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.ColumnsStartX + window->DC.ColumnsOffsetX, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y) - ImVec2(0.0f, window->ScrollY);
        window->DC.CursorPos = window->DC.CursorStartPos;
        window->DC.CursorPosPrevLine = window->DC.CursorPos;
        window->DC.CursorMaxPos = window->DC.CursorStartPos;
        window->DC.CurrentLineHeight = window->DC.PrevLineHeight = 0.0f;
        window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
        window->DC.MenuBarAppending = false;
        window->DC.MenuBarOffsetX = ImMax(window->DC.ColumnsStartX, style.ItemSpacing.x);
        window->DC.LogLinePosY = window->DC.CursorPos.y - 9999.0f;
        window->DC.ChildWindows.resize(0);
        window->DC.LayoutType = ImGuiLayoutType_Vertical;
        window->DC.ItemWidth = window->ItemWidthDefault;
        window->DC.ItemWidthStack.resize(0); 
        window->DC.ButtonRepeat = false;
        window->DC.ButtonRepeatStack.resize(0);
        window->DC.AllowKeyboardFocus = true;
        window->DC.AllowKeyboardFocusStack.resize(0);
        window->DC.TextWrapPos = -1.0f; // disabled
        window->DC.TextWrapPosStack.resize(0);
        window->DC.ColorEditMode = ImGuiColorEditMode_UserSelect;
        window->DC.ColumnsCurrent = 0;
        window->DC.ColumnsCount = 1;
        window->DC.ColumnsStartPos = window->DC.CursorPos;
        window->DC.ColumnsCellMinY = window->DC.ColumnsCellMaxY = window->DC.ColumnsStartPos.y;
        window->DC.TreeDepth = 0;
        window->DC.StateStorage = &window->StateStorage;
        window->DC.GroupStack.resize(0);
        window->MenuColumns.Update(3, style.ItemSpacing.x, !window_was_visible);

        if (window->AutoFitFramesX > 0)
            window->AutoFitFramesX--;
        if (window->AutoFitFramesY > 0)
            window->AutoFitFramesY--;

        // Title bar
        if (!(flags & ImGuiWindowFlags_NoTitleBar))
        {
            if (p_opened != NULL)
                CloseWindowButton(p_opened);

            const ImVec2 text_size = CalcTextSize(name, NULL, true);
            if (!(flags & ImGuiWindowFlags_NoCollapse))
                RenderCollapseTriangle(window->Pos + style.FramePadding, !window->Collapsed, 1.0f, true);
            
            ImVec2 text_min = window->Pos + style.FramePadding;
            ImVec2 text_max = window->Pos + ImVec2(window->Size.x - style.FramePadding.x, style.FramePadding.y*2 + text_size.y);
            ImVec2 clip_max = ImVec2(window->Pos.x + window->Size.x - (p_opened ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x), text_max.y); // Match the size of CloseWindowButton()
            bool pad_left = (flags & ImGuiWindowFlags_NoCollapse) == 0;
            bool pad_right = (p_opened != NULL);
            if (style.WindowTitleAlign & ImGuiAlign_Center) pad_right = pad_left;
            if (pad_left) text_min.x += g.FontSize + style.ItemInnerSpacing.x;
            if (pad_right) text_max.x -= g.FontSize + style.ItemInnerSpacing.x;
            RenderTextClipped(text_min, text_max, name, NULL, &text_size, style.WindowTitleAlign, NULL, &clip_max);
        }

        // Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
        window->ClippedWindowRect = window->Rect();
        window->ClippedWindowRect.Clip(window->ClipRect);

        // Pressing CTRL+C while holding on a window copy its content to the clipboard
        // This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
        // Maybe we can support CTRL+C on every element?
        /*
        if (g.ActiveId == move_id)
            if (g.IO.KeyCtrl && IsKeyPressedMap(ImGuiKey_C))
                ImGui::LogToClipboard();
        */
    }
    window->BeginCount++;

    // Inner clipping rectangle
    // We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
    // Note that if our window is collapsed we will end up with a null clipping rectangle which is the correct behavior.
    const ImRect title_bar_rect = window->TitleBarRect();
    ImVec4 clip_rect(title_bar_rect.Min.x+0.5f+window->WindowPadding.x*0.5f, title_bar_rect.Max.y+window->MenuBarHeight()+0.5f, window->Rect().Max.x+0.5f-window->WindowPadding.x*0.5f, window->Rect().Max.y-1.5f);
    if (window->ScrollbarY)
        clip_rect.z -= style.ScrollbarWidth;
    PushClipRect(clip_rect);

    // Clear 'accessed' flag last thing
    if (first_begin_of_the_frame)
        window->Accessed = false;

    // Child window can be out of sight and have "negative" clip windows.
    // Mark them as collapsed so commands are skipped earlier (we can't manually collapse because they have no title bar).
    if (flags & ImGuiWindowFlags_ChildWindow)
    {
        IM_ASSERT((flags & ImGuiWindowFlags_NoTitleBar) != 0);
        window->Collapsed = parent_window && parent_window->Collapsed;

        if (!(flags & ImGuiWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
        {
            const ImVec4 clip_rect_t = window->ClipRect;
            window->Collapsed |= (clip_rect_t.x >= clip_rect_t.z || clip_rect_t.y >= clip_rect_t.w);
        }

        // We also hide the window from rendering because we've already added its border to the command list.
        // (we could perform the check earlier in the function but it is simpler at this point)
        if (window->Collapsed)
            window->Active = false;
    }
    if (style.Alpha <= 0.0f)
        window->Active = false;

    // Return false if we don't intend to display anything to allow user to perform an early out optimization
    window->SkipItems = (window->Collapsed || !window->Active) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0;
    return !window->SkipItems;
}

void ImGui::End()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    ImGui::Columns(1, "#CloseColumns");
    PopClipRect();   // inner window clip rectangle

    // Stop logging
    if (!(window->Flags & ImGuiWindowFlags_ChildWindow))    // FIXME: add more options for scope of logging
        ImGui::LogFinish();

    // Pop
    // NB: we don't clear 'window->RootWindow'. The pointer is allowed to live until the next call to Begin().
    g.CurrentWindowStack.pop_back();
    if (window->Flags & ImGuiWindowFlags_Popup)
        g.CurrentPopupStack.pop_back();
    CheckStacksSize(window, false);
    SetCurrentWindow(g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back());
}

// Vertical scrollbar
// The entire piece of code below is rather confusing because:
// - We handle absolute seeking (when first clicking outside the grab) and relative manipulation (afterward or when clicking inside the grab)
// - We store values as ratio and in a form that allows the window content to change while we are holding on a scrollbar
static void Scrollbar(ImGuiWindow* window)
{
    ImGuiState& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID("#SCROLLY");

    // Render background
    ImRect bb(window->Rect().Max.x - style.ScrollbarWidth, window->Pos.y + window->TitleBarHeight()+1, window->Rect().Max.x, window->Rect().Max.y-1);
    window->DrawList->AddRectFilled(bb.Min, bb.Max, window->Color(ImGuiCol_ScrollbarBg));
    bb.Expand(-3.0f);
    const float scrollbar_height = bb.GetHeight();

    // The grabable box size generally represent the amount visible (vs the total scrollable amount)
    // But we maintain a minimum size in pixel to allow for the user to still aim inside.
    const float grab_h_pixels = ImMin(ImMax(scrollbar_height * ImSaturate(window->Size.y / ImMax(window->SizeContents.y, window->Size.y)), style.GrabMinSize), scrollbar_height);
    const float grab_h_norm = grab_h_pixels / scrollbar_height;

    // Handle input right away. None of the code of Begin() is relying on scrolling position before calling Scrollbar().
    bool held = false;
    bool hovered = false;
    const bool previously_held = (g.ActiveId == id);
    ButtonBehavior(bb, id, &hovered, &held, true);

    const float scroll_max = ImMax(1.0f, window->SizeContents.y - window->Size.y);
    float scroll_ratio = ImSaturate(window->ScrollY / scroll_max);
    float grab_y_norm = scroll_ratio * (scrollbar_height - grab_h_pixels) / scrollbar_height;
    if (held)
    {
        const float clicked_y_norm = ImSaturate((g.IO.MousePos.y - bb.Min.y) / scrollbar_height);     // Click position in scrollbar space (0.0f->1.0f)
        g.HoveredId = id;

        bool seek_absolute = false;
        if (!previously_held)
        {
            // On initial click calculate the distance between mouse and the center of the grab
            if (clicked_y_norm >= grab_y_norm && clicked_y_norm <= grab_y_norm + grab_h_norm)
            {
                g.ScrollbarClickDeltaToGrabCenter = clicked_y_norm - grab_y_norm - grab_h_norm*0.5f;
            }
            else
            {
                seek_absolute = true;
                g.ScrollbarClickDeltaToGrabCenter = 0;
            }
        }

        // Apply scroll
        // It is ok to modify ScrollY here because we are being called in Begin() after the calculation of SizeContents and before setting up our starting position
        const float scroll_y_norm = ImSaturate((clicked_y_norm - g.ScrollbarClickDeltaToGrabCenter - grab_h_norm*0.5f) / (1.0f - grab_h_norm));
        window->ScrollY = (float)(int)(0.5f + scroll_y_norm * (window->SizeContents.y - window->Size.y));

        // Update values for rendering
        scroll_ratio = ImSaturate(window->ScrollY / scroll_max);
        grab_y_norm = scroll_ratio * (scrollbar_height - grab_h_pixels) / scrollbar_height;

        // Update distance to grab now that we have seeked and saturated
        if (seek_absolute)
            g.ScrollbarClickDeltaToGrabCenter = clicked_y_norm - grab_y_norm - grab_h_norm*0.5f;
    }

    // Render
    const ImU32 grab_col = window->Color(held ? ImGuiCol_ScrollbarGrabActive : hovered ? ImGuiCol_ScrollbarGrabHovered : ImGuiCol_ScrollbarGrab);
    window->DrawList->AddRectFilled(ImVec2(bb.Min.x, ImLerp(bb.Min.y, bb.Max.y, grab_y_norm)), ImVec2(bb.Max.x, ImLerp(bb.Min.y, bb.Max.y, grab_y_norm) + grab_h_pixels), grab_col, style.ScrollbarRounding);
}

// Moving window to front of display (which happens to be back of our sorted list)
static void FocusWindow(ImGuiWindow* window)
{
    ImGuiState& g = *GImGui;

    // Always mark the window we passed as focused. This is used for keyboard interactions such as tabbing.
    g.FocusedWindow = window;

    // Passing NULL allow to disable keyboard focus
    if (!window)
        return;

    // And move its root window to the top of the pile 
    if (window->RootWindow)
        window = window->RootWindow;

    // Steal focus on active widgets
    if (window->Flags & ImGuiWindowFlags_Popup) // FIXME: This statement should be unnecessary. Need further testing before removing it..
        if (g.ActiveId != 0 && g.ActiveIdWindow && g.ActiveIdWindow->RootWindow != window)
            SetActiveId(0);

    if (g.Windows.back() == window)
        return;

    for (int i = 0; i < g.Windows.Size; i++)
        if (g.Windows[i] == window)
        {
            g.Windows.erase(g.Windows.begin() + i);
            break;
        }
    g.Windows.push_back(window);
}

void ImGui::PushItemWidth(float item_width)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ItemWidth = (item_width == 0.0f ? window->ItemWidthDefault : item_width);
    window->DC.ItemWidthStack.push_back(window->DC.ItemWidth);
}

static void PushMultiItemsWidths(int components, float w_full = 0.0f)
{
    ImGuiWindow* window = GetCurrentWindow();
    const ImGuiStyle& style = GImGui->Style;
    if (w_full <= 0.0f)
        w_full = ImGui::CalcItemWidth();
    const float w_item_one  = ImMax(1.0f, (float)(int)((w_full - (style.FramePadding.x*2.0f + style.ItemInnerSpacing.x) * (components-1)) / (float)components));
    const float w_item_last = ImMax(1.0f, (float)(int)(w_full - (w_item_one + style.FramePadding.x*2.0f + style.ItemInnerSpacing.x) * (components-1)));
    window->DC.ItemWidthStack.push_back(w_item_last);
    for (int i = 0; i < components-1; i++)
        window->DC.ItemWidthStack.push_back(w_item_one);
    window->DC.ItemWidth = window->DC.ItemWidthStack.back();
}

void ImGui::PopItemWidth()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ItemWidthStack.pop_back();
    window->DC.ItemWidth = window->DC.ItemWidthStack.empty() ? window->ItemWidthDefault : window->DC.ItemWidthStack.back();
}

float ImGui::CalcItemWidth()
{
    ImGuiWindow* window = GetCurrentWindow();
    float w = window->DC.ItemWidth;
    if (w < 0.0f)
    {
        // Align to a right-side limit. We include 1 frame padding in the calculation because this is how the width is always used (we add 2 frame padding to it), but we could move that responsibility to the widget as well.
        ImGuiState& g = *GImGui;
        float width_to_right_edge = window->Pos.x + ImGui::GetContentRegionMax().x - window->DC.CursorPos.x;
        w = ImMax(1.0f, width_to_right_edge + w - g.Style.FramePadding.x * 2.0f);
    }
    w = (float)(int)w;
    return w;
}

static void SetFont(ImFont* font)
{
    ImGuiState& g = *GImGui;
    IM_ASSERT(font && font->IsLoaded());
    IM_ASSERT(font->Scale > 0.0f);
    g.Font = font;
    g.FontBaseSize = g.IO.FontGlobalScale * g.Font->FontSize * g.Font->Scale;
    g.FontSize = g.CurrentWindow ? g.CurrentWindow->CalcFontSize() : 0.0f;
    g.FontTexUvWhitePixel = g.Font->ContainerAtlas->TexUvWhitePixel;
}

void ImGui::PushFont(ImFont* font)
{
    ImGuiState& g = *GImGui;
    if (!font)
        font = g.IO.Fonts->Fonts[0];
    SetFont(font);
    g.FontStack.push_back(font);
    g.CurrentWindow->DrawList->PushTextureID(font->ContainerAtlas->TexID);
}

void  ImGui::PopFont()
{
    ImGuiState& g = *GImGui;
    g.CurrentWindow->DrawList->PopTextureID();
    g.FontStack.pop_back();
    SetFont(g.FontStack.empty() ? g.IO.Fonts->Fonts[0] : g.FontStack.back());
}

void ImGui::PushAllowKeyboardFocus(bool allow_keyboard_focus)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.AllowKeyboardFocus = allow_keyboard_focus;
    window->DC.AllowKeyboardFocusStack.push_back(allow_keyboard_focus);
}

void ImGui::PopAllowKeyboardFocus()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.AllowKeyboardFocusStack.pop_back();
    window->DC.AllowKeyboardFocus = window->DC.AllowKeyboardFocusStack.empty() ? true : window->DC.AllowKeyboardFocusStack.back();
}

void ImGui::PushButtonRepeat(bool repeat)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ButtonRepeat = repeat;
    window->DC.ButtonRepeatStack.push_back(repeat);
}

void ImGui::PopButtonRepeat()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ButtonRepeatStack.pop_back();
    window->DC.ButtonRepeat = window->DC.ButtonRepeatStack.empty() ? false : window->DC.ButtonRepeatStack.back();
}

void ImGui::PushTextWrapPos(float wrap_pos_x)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.TextWrapPos = wrap_pos_x;
    window->DC.TextWrapPosStack.push_back(wrap_pos_x);
}

void ImGui::PopTextWrapPos()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.TextWrapPosStack.pop_back();
    window->DC.TextWrapPos = window->DC.TextWrapPosStack.empty() ? -1.0f : window->DC.TextWrapPosStack.back();
}

void ImGui::PushStyleColor(ImGuiCol idx, const ImVec4& col)
{
    ImGuiState& g = *GImGui;
    ImGuiColMod backup;
    backup.Col = idx;
    backup.PreviousValue = g.Style.Colors[idx];
    g.ColorModifiers.push_back(backup);
    g.Style.Colors[idx] = col;
}

void ImGui::PopStyleColor(int count)
{
    ImGuiState& g = *GImGui;
    while (count > 0)
    {
        ImGuiColMod& backup = g.ColorModifiers.back();
        g.Style.Colors[backup.Col] = backup.PreviousValue;
        g.ColorModifiers.pop_back();
        count--;
    }
}

static float* GetStyleVarFloatAddr(ImGuiStyleVar idx)
{
    ImGuiState& g = *GImGui;
    switch (idx)
    {
    case ImGuiStyleVar_Alpha: return &g.Style.Alpha;
    case ImGuiStyleVar_WindowRounding: return &g.Style.WindowRounding;
    case ImGuiStyleVar_ChildWindowRounding: return &g.Style.ChildWindowRounding;
    case ImGuiStyleVar_FrameRounding: return &g.Style.FrameRounding;
    case ImGuiStyleVar_IndentSpacing: return &g.Style.IndentSpacing;
    case ImGuiStyleVar_GrabMinSize: return &g.Style.GrabMinSize;
    }
    return NULL;
}

static ImVec2* GetStyleVarVec2Addr(ImGuiStyleVar idx)
{
    ImGuiState& g = *GImGui;
    switch (idx)
    {
    case ImGuiStyleVar_WindowPadding: return &g.Style.WindowPadding;
    case ImGuiStyleVar_WindowMinSize: return &g.Style.WindowMinSize;
    case ImGuiStyleVar_FramePadding: return &g.Style.FramePadding;
    case ImGuiStyleVar_ItemSpacing: return &g.Style.ItemSpacing;
    case ImGuiStyleVar_ItemInnerSpacing: return &g.Style.ItemInnerSpacing;
    }
    return NULL;
}

void ImGui::PushStyleVar(ImGuiStyleVar idx, float val)
{
    ImGuiState& g = *GImGui;
    float* pvar = GetStyleVarFloatAddr(idx);
    IM_ASSERT(pvar != NULL); // Called function with wrong-type? Variable is not a float.
    ImGuiStyleMod backup;
    backup.Var = idx;
    backup.PreviousValue = ImVec2(*pvar, 0.0f);
    g.StyleModifiers.push_back(backup);
    *pvar = val;
}


void ImGui::PushStyleVar(ImGuiStyleVar idx, const ImVec2& val)
{
    ImGuiState& g = *GImGui;
    ImVec2* pvar = GetStyleVarVec2Addr(idx);
    IM_ASSERT(pvar != NULL); // Called function with wrong-type? Variable is not a ImVec2.
    ImGuiStyleMod backup;
    backup.Var = idx;
    backup.PreviousValue = *pvar;
    g.StyleModifiers.push_back(backup);
    *pvar = val;
}

void ImGui::PopStyleVar(int count)
{
    ImGuiState& g = *GImGui;
    while (count > 0)
    {
        ImGuiStyleMod& backup = g.StyleModifiers.back();
        if (float* pvar_f = GetStyleVarFloatAddr(backup.Var))
            *pvar_f = backup.PreviousValue.x;
        else if (ImVec2* pvar_v = GetStyleVarVec2Addr(backup.Var))
            *pvar_v = backup.PreviousValue;
        g.StyleModifiers.pop_back();
        count--;
    }
}

const char* ImGui::GetStyleColName(ImGuiCol idx)
{
    // Create switch-case from enum with regexp: ImGuiCol_{.*}, --> case ImGuiCol_\1: return "\1";
    switch (idx)
    {
    case ImGuiCol_Text: return "Text";
    case ImGuiCol_TextDisabled: return "TextDisabled";
    case ImGuiCol_WindowBg: return "WindowBg";
    case ImGuiCol_ChildWindowBg: return "ChildWindowBg";
    case ImGuiCol_Border: return "Border";
    case ImGuiCol_BorderShadow: return "BorderShadow";
    case ImGuiCol_FrameBg: return "FrameBg";
    case ImGuiCol_FrameBgHovered: return "FrameBgHovered";
    case ImGuiCol_FrameBgActive: return "FrameBgActive";
    case ImGuiCol_TitleBg: return "TitleBg";
    case ImGuiCol_TitleBgCollapsed: return "TitleBgCollapsed";
    case ImGuiCol_TitleBgActive: return "TitleBgActive";
    case ImGuiCol_MenuBarBg: return "MenuBarBg";
    case ImGuiCol_ScrollbarBg: return "ScrollbarBg";
    case ImGuiCol_ScrollbarGrab: return "ScrollbarGrab";
    case ImGuiCol_ScrollbarGrabHovered: return "ScrollbarGrabHovered";
    case ImGuiCol_ScrollbarGrabActive: return "ScrollbarGrabActive";
    case ImGuiCol_ComboBg: return "ComboBg";
    case ImGuiCol_CheckMark: return "CheckMark";
    case ImGuiCol_SliderGrab: return "SliderGrab";
    case ImGuiCol_SliderGrabActive: return "SliderGrabActive";
    case ImGuiCol_Button: return "Button";
    case ImGuiCol_ButtonHovered: return "ButtonHovered";
    case ImGuiCol_ButtonActive: return "ButtonActive";
    case ImGuiCol_Header: return "Header";
    case ImGuiCol_HeaderHovered: return "HeaderHovered";
    case ImGuiCol_HeaderActive: return "HeaderActive";
    case ImGuiCol_Column: return "Column";
    case ImGuiCol_ColumnHovered: return "ColumnHovered";
    case ImGuiCol_ColumnActive: return "ColumnActive";
    case ImGuiCol_ResizeGrip: return "ResizeGrip";
    case ImGuiCol_ResizeGripHovered: return "ResizeGripHovered";
    case ImGuiCol_ResizeGripActive: return "ResizeGripActive";
    case ImGuiCol_CloseButton: return "CloseButton";
    case ImGuiCol_CloseButtonHovered: return "CloseButtonHovered";
    case ImGuiCol_CloseButtonActive: return "CloseButtonActive";
    case ImGuiCol_PlotLines: return "PlotLines";
    case ImGuiCol_PlotLinesHovered: return "PlotLinesHovered";
    case ImGuiCol_PlotHistogram: return "PlotHistogram";
    case ImGuiCol_PlotHistogramHovered: return "PlotHistogramHovered";
    case ImGuiCol_TextSelectedBg: return "TextSelectedBg";
    case ImGuiCol_TooltipBg: return "TooltipBg";
    case ImGuiCol_ModalWindowDarkening: return "ModalWindowDarkening";
    }
    IM_ASSERT(0);
    return "Unknown";
}

bool ImGui::IsWindowHovered()
{
    ImGuiState& g = *GImGui;
    return g.HoveredWindow == g.CurrentWindow;
}

bool ImGui::IsWindowFocused()
{
    ImGuiState& g = *GImGui;
    return g.FocusedWindow == g.CurrentWindow;
}

bool ImGui::IsRootWindowFocused()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* root_window = g.CurrentWindow->RootWindow;
    return g.FocusedWindow == root_window;
}

bool ImGui::IsRootWindowOrAnyChildFocused()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* root_window = g.CurrentWindow->RootWindow;
    return g.FocusedWindow && g.FocusedWindow->RootWindow == root_window;
}

float ImGui::GetWindowWidth()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Size.x;
}

ImVec2 ImGui::GetWindowPos()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    return window->Pos;
}

static void SetWindowScrollY(ImGuiWindow* window, float new_scroll_y)
{
    window->DC.CursorMaxPos.y += window->ScrollY;
    window->ScrollY = new_scroll_y;
    window->DC.CursorMaxPos.y -= window->ScrollY;
}

static void SetWindowPos(ImGuiWindow* window, const ImVec2& pos, ImGuiSetCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if (cond && (window->SetWindowPosAllowFlags & cond) == 0)
        return;
    window->SetWindowPosAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);
    window->SetWindowPosCenterWanted = false;

    // Set
    const ImVec2 old_pos = window->Pos;
    window->PosFloat = pos;
    window->Pos = ImVec2((float)(int)window->PosFloat.x, (float)(int)window->PosFloat.y);
    window->DC.CursorPos += (window->Pos - old_pos);    // As we happen to move the window while it is being appended to (which is a bad idea - will smear) let's at least offset the cursor
    window->DC.CursorMaxPos += (window->Pos - old_pos); // And more importantly we need to adjust this so size calculation doesn't get affected.
}

void ImGui::SetWindowPos(const ImVec2& pos, ImGuiSetCond cond)
{
    ImGuiWindow* window = GetCurrentWindow();
    SetWindowPos(window, pos, cond);
}

void ImGui::SetWindowPos(const char* name, const ImVec2& pos, ImGuiSetCond cond)
{
    ImGuiWindow* window = FindWindowByName(name);
    if (window)
        SetWindowPos(window, pos, cond);
}

ImVec2 ImGui::GetWindowSize()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->Size;
}

static void SetWindowSize(ImGuiWindow* window, const ImVec2& size, ImGuiSetCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if (cond && (window->SetWindowSizeAllowFlags & cond) == 0)
        return;
    window->SetWindowSizeAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);

    // Set
    if (size.x > 0.0f)
    {
		window->AutoFitFramesX = 0;
        window->SizeFull.x = size.x;
    }
    else
    {
        window->AutoFitFramesX = 2;
        window->AutoFitOnlyGrows = false;
    }
    if (size.y > 0.0f)
    {
		window->AutoFitFramesY = 0;
        window->SizeFull.y = size.y;
    }
    else
    {
        window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = false;
    }
}

void ImGui::SetWindowSize(const ImVec2& size, ImGuiSetCond cond)
{
    ImGuiWindow* window = GetCurrentWindow();
    SetWindowSize(window, size, cond);
}

void ImGui::SetWindowSize(const char* name, const ImVec2& size, ImGuiSetCond cond)
{
    ImGuiWindow* window = FindWindowByName(name);
    if (window)
        SetWindowSize(window, size, cond);
}

static void SetWindowCollapsed(ImGuiWindow* window, bool collapsed, ImGuiSetCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if (cond && (window->SetWindowCollapsedAllowFlags & cond) == 0)
        return;
    window->SetWindowCollapsedAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);

    // Set
    window->Collapsed = collapsed;
}

void ImGui::SetWindowCollapsed(bool collapsed, ImGuiSetCond cond)
{
    ImGuiWindow* window = GetCurrentWindow();
    SetWindowCollapsed(window, collapsed, cond);
}

bool ImGui::IsWindowCollapsed()
{
    return GImGui->CurrentWindow->Collapsed;
}

void ImGui::SetWindowCollapsed(const char* name, bool collapsed, ImGuiSetCond cond)
{
    ImGuiWindow* window = FindWindowByName(name);
    if (window)
        SetWindowCollapsed(window, collapsed, cond);
}

void ImGui::SetWindowFocus()
{
    ImGuiWindow* window = GetCurrentWindow();
    FocusWindow(window);
}

void ImGui::SetWindowFocus(const char* name)
{
    if (name)
    {
        ImGuiWindow* window = FindWindowByName(name);
        if (window)
            FocusWindow(window);
    }
    else
    {
        FocusWindow(NULL);
    }
}

void ImGui::SetNextWindowPos(const ImVec2& pos, ImGuiSetCond cond)
{
    ImGuiState& g = *GImGui;
    g.SetNextWindowPosVal = pos;
    g.SetNextWindowPosCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::SetNextWindowPosCenter(ImGuiSetCond cond)
{
    ImGuiState& g = *GImGui;
    g.SetNextWindowPosVal = ImVec2(-FLT_MAX, -FLT_MAX);
    g.SetNextWindowPosCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::SetNextWindowSize(const ImVec2& size, ImGuiSetCond cond)
{
    ImGuiState& g = *GImGui;
    g.SetNextWindowSizeVal = size;
    g.SetNextWindowSizeCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::SetNextWindowCollapsed(bool collapsed, ImGuiSetCond cond)
{
    ImGuiState& g = *GImGui;
    g.SetNextWindowCollapsedVal = collapsed;
    g.SetNextWindowCollapsedCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::SetNextWindowFocus()
{
    ImGuiState& g = *GImGui;
    g.SetNextWindowFocus = true;
}

ImVec2 ImGui::GetContentRegionMax()
{
    ImGuiWindow* window = GetCurrentWindow();
    ImVec2 mx = window->Size - window->WindowPadding;
    if (window->DC.ColumnsCount != 1)
        mx.x = ImGui::GetColumnOffset(window->DC.ColumnsCurrent + 1) - window->WindowPadding.x;
    else
        mx.x -= window->ScrollbarWidth();
    return mx;
}

ImVec2 ImGui::GetWindowContentRegionMin()
{
    ImGuiWindow* window = GetCurrentWindow();
    return ImVec2(0, window->TitleBarHeight() + window->MenuBarHeight()) + window->WindowPadding;
}

ImVec2 ImGui::GetWindowContentRegionMax()
{
    ImGuiWindow* window = GetCurrentWindow();
    ImVec2 m = window->Size - window->WindowPadding;
    m.x -= window->ScrollbarWidth();
    return m;
}

float ImGui::GetTextLineHeight()
{
    ImGuiState& g = *GImGui;
    return g.FontSize;
}

float ImGui::GetTextLineHeightWithSpacing()
{
    ImGuiState& g = *GImGui;
    return g.FontSize + g.Style.ItemSpacing.y;
}

float ImGui::GetItemsLineHeightWithSpacing()
{
    ImGuiState& g = *GImGui;
    return g.FontSize + g.Style.FramePadding.y * 2.0f + g.Style.ItemSpacing.y;
}

ImDrawList* ImGui::GetWindowDrawList()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DrawList;
}

ImFont* ImGui::GetWindowFont()
{
    ImGuiState& g = *GImGui;
    return g.Font;
}

float ImGui::GetWindowFontSize()
{
    ImGuiState& g = *GImGui;
    return g.FontSize;
}

void ImGui::SetWindowFontScale(float scale)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->FontWindowScale = scale;
    g.FontSize = window->CalcFontSize();
}

// User generally sees positions in window coordinates. Internally we store CursorPos in absolute screen coordinates because it is more convenient.
// Conversion happens as we pass the value to user, but it makes our naming convention confusing because GetCursorPos() == (DC.CursorPos - window.Pos). May want to rename 'DC.CursorPos'.
ImVec2 ImGui::GetCursorPos()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.CursorPos - window->Pos;
}

float ImGui::GetCursorPosX()
{
    return ImGui::GetCursorPos().x;
}

float ImGui::GetCursorPosY()
{
    return ImGui::GetCursorPos().y;
}

void ImGui::SetCursorPos(const ImVec2& pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos = window->Pos + pos;
    window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}

void ImGui::SetCursorPosX(float x)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.x = window->Pos.x + x;
    window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPos.x);
}

void ImGui::SetCursorPosY(float y)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.y = window->Pos.y + y;
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y);
}

ImVec2 ImGui::GetCursorStartPos()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.CursorStartPos - window->Pos;
}

ImVec2 ImGui::GetCursorScreenPos()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.CursorPos;
}

void ImGui::SetCursorScreenPos(const ImVec2& screen_pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos = screen_pos;
}

float ImGui::GetScrollY()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->ScrollY;
}

float ImGui::GetScrollMaxY()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->SizeContents.y - window->SizeFull.y;
}

void ImGui::SetScrollY(float scroll_y)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->ScrollTargetRelY = scroll_y + window->TitleBarHeight(); // title bar height cancelled out when using ScrollTargetRelY
    window->ScrollTargetCenterRatioY = 0.0f;
}

void ImGui::SetScrollFromPosY(float pos_y, float center_y_ratio)
{
    // We store a target position so centering can occur on the next frame when we are guaranteed to have a known window size
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(center_y_ratio >= 0.0f && center_y_ratio <= 1.0f);
    window->ScrollTargetRelY = (float)(int)(pos_y + window->ScrollY);
    if (center_y_ratio <= 0.0f && window->ScrollTargetRelY <= window->WindowPadding.y)    // Minor hack to make "scroll to top" take account of WindowPadding, else it would scroll to (WindowPadding.y - ItemSpacing.y)
        window->ScrollTargetRelY = 0.0f;
    window->ScrollTargetCenterRatioY = center_y_ratio;
}

void ImGui::SetScrollHere(float center_y_ratio)
{
    ImGuiWindow* window = GetCurrentWindow();
    float target_y = window->DC.CursorPosPrevLine.y + (window->DC.PrevLineHeight * center_y_ratio) + (GImGui->Style.ItemSpacing.y * (center_y_ratio - 0.5f) * 2.0f); // Precisely aim above, in the middle or below the last line.
    ImGui::SetScrollFromPosY(target_y - window->Pos.y, center_y_ratio);
}

void ImGui::SetKeyboardFocusHere(int offset)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->FocusIdxAllRequestNext = window->FocusIdxAllCounter + 1 + offset;
    window->FocusIdxTabRequestNext = IM_INT_MAX;
}

void ImGui::SetStateStorage(ImGuiStorage* tree)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.StateStorage = tree ? tree : &window->StateStorage;
}

ImGuiStorage* ImGui::GetStateStorage()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.StateStorage;
}

void ImGui::TextV(const char* fmt, va_list args)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const char* text_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    TextUnformatted(g.TempBuffer, text_end);
}

void ImGui::Text(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextV(fmt, args);
    va_end(args);
}

void ImGui::TextColoredV(const ImVec4& col, const char* fmt, va_list args)
{
    ImGui::PushStyleColor(ImGuiCol_Text, col);
    TextV(fmt, args);
    ImGui::PopStyleColor();
}

void ImGui::TextColored(const ImVec4& col, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextColoredV(col, fmt, args);
    va_end(args);
}

void ImGui::TextDisabledV(const char* fmt, va_list args)
{
    ImGui::PushStyleColor(ImGuiCol_Text, GImGui->Style.Colors[ImGuiCol_TextDisabled]);
    TextV(fmt, args);
    ImGui::PopStyleColor();
}

void ImGui::TextDisabled(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextDisabledV(fmt, args);
    va_end(args);
}

void ImGui::TextWrappedV(const char* fmt, va_list args)
{
    ImGui::PushTextWrapPos(0.0f);
    TextV(fmt, args);
    ImGui::PopTextWrapPos();
}

void ImGui::TextWrapped(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextWrappedV(fmt, args);
    va_end(args);
}

void ImGui::TextUnformatted(const char* text, const char* text_end)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    IM_ASSERT(text != NULL);
    const char* text_begin = text;
    if (text_end == NULL)
        text_end = text + strlen(text); // FIXME-OPT

    const float wrap_pos_x = window->DC.TextWrapPos;
    const bool wrap_enabled = wrap_pos_x >= 0.0f;
    if (text_end - text > 2000 && !wrap_enabled)
    {
        // Long text!
        // Perform manual coarse clipping to optimize for long multi-line text
        // From this point we will only compute the width of lines that are visible. Optimization only available when word-wrapping is disabled. 
        // We also don't vertically center the text within the line full height, which is unlikely to matter because we are likely the biggest and only item on the line.
        const char* line = text;
        const float line_height = ImGui::GetTextLineHeight();
        const ImVec2 text_pos = window->DC.CursorPos + ImVec2(0.0f, window->DC.CurrentLineTextBaseOffset);
        const ImVec4 clip_rect = window->ClipRect;
        ImVec2 text_size(0,0);

        if (text_pos.y <= clip_rect.w)
        {
            ImVec2 pos = text_pos;

            // Lines to skip (can't skip when logging text)
            if (!g.LogEnabled)
            {
                int lines_skippable = (int)((clip_rect.y - text_pos.y) / line_height);
                if (lines_skippable > 0)
                {
                    int lines_skipped = 0;
                    while (line < text_end && lines_skipped < lines_skippable)
                    {
                        const char* line_end = strchr(line, '\n');
                        line = line_end + 1;
                        lines_skipped++;
                    }
                    pos.y += lines_skipped * line_height;
                }
            }

            // Lines to render
            if (line < text_end)
            {
                ImRect line_rect(pos, pos + ImVec2(ImGui::GetWindowWidth(), line_height));
                while (line < text_end)
                {
                    const char* line_end = strchr(line, '\n');
                    if (IsClippedEx(line_rect, NULL, false))
                        break;

                    const ImVec2 line_size = CalcTextSize(line, line_end, false);
                    text_size.x = ImMax(text_size.x, line_size.x);
                    RenderText(pos, line, line_end, false);
                    if (!line_end)
                        line_end = text_end;
                    line = line_end + 1;
                    line_rect.Min.y += line_height;
                    line_rect.Max.y += line_height;
                    pos.y += line_height;
                }

                // Count remaining lines
                int lines_skipped = 0;
                while (line < text_end)
                {
                    const char* line_end = strchr(line, '\n');
                    if (!line_end)
                        line_end = text_end;
                    line = line_end + 1;
                    lines_skipped++;
                }
                pos.y += lines_skipped * line_height;
            }

            text_size.y += (pos - text_pos).y;
        }

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(bb);
        ItemAdd(bb, NULL);
    }
    else
    {
        const float wrap_width = wrap_enabled ? CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
        const ImVec2 text_size = CalcTextSize(text_begin, text_end, false, wrap_width);

        // Account of baseline offset
        ImVec2 text_pos = window->DC.CursorPos;
        text_pos.y += window->DC.CurrentLineTextBaseOffset;

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(bb.GetSize());
        if (!ItemAdd(bb, NULL))
            return;

        // Render (we don't hide text after ## in this end-user function)
        RenderTextWrapped(bb.Min, text_begin, text_end, wrap_width);
    }
}

void ImGui::AlignFirstTextHeightToWidgets()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    // Declare a dummy item size to that upcoming items that are smaller will center-align on the newly expanded line height.
    ItemSize(ImVec2(0, g.FontSize + g.Style.FramePadding.y*2), g.Style.FramePadding.y);
    ImGui::SameLine(0, 0);
}

// Add a label+text combo aligned to other label+value widgets
void ImGui::LabelTextV(const char* label, const char* fmt, va_list args)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImGuiStyle& style = g.Style;
    const float w = ImGui::CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect value_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + style.FramePadding.x*2, label_size.y + style.FramePadding.y*2));
    const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + style.FramePadding.x*2 + (label_size.x > 0.0f ? style.ItemInnerSpacing.x : 0.0f), style.FramePadding.y*2) + label_size);
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, NULL))
        return;

    // Render
    const char* value_text_begin = &g.TempBuffer[0];
    const char* value_text_end = value_text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    RenderTextClipped(value_bb.Min, value_bb.Max, value_text_begin, value_text_end, NULL, ImGuiAlign_VCenter);
    RenderText(ImVec2(value_bb.Max.x + style.ItemInnerSpacing.x, value_bb.Min.y + style.FramePadding.y), label);
}

void ImGui::LabelText(const char* label, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LabelTextV(label, fmt, args);
    va_end(args);
}

static inline bool IsWindowContentHoverable(ImGuiWindow* window)
{
    // An active popup disable hovering on other windows (apart from its own children)
    ImGuiState& g = *GImGui;
    if (ImGuiWindow* focused_window = g.FocusedWindow)
        if (ImGuiWindow* focused_root_window = focused_window->RootWindow)
            if ((focused_root_window->Flags & ImGuiWindowFlags_Popup) != 0 && focused_root_window->WasActive && focused_root_window != window->RootWindow)
                return false;

    return true;
}

static bool IsHovered(const ImRect& bb, ImGuiID id, bool flatten_childs = false)
{
    ImGuiState& g = *GImGui;
    if (g.HoveredId == 0 || g.HoveredId == id)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (g.HoveredWindow == window || (flatten_childs && g.HoveredRootWindow == window->RootWindow))
            if ((g.ActiveId == 0 || g.ActiveId == id || g.ActiveIdIsFocusedOnly) && IsMouseHoveringRect(bb))
                if (IsWindowContentHoverable(g.HoveredRootWindow))
                    return true;
    }
    return false;
}

static bool ButtonBehavior(const ImRect& bb, ImGuiID id, bool* out_hovered, bool* out_held, bool allow_key_modifiers, ImGuiButtonFlags flags)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    if (flags & ImGuiButtonFlags_Disabled)
    {
        if (out_hovered) *out_hovered = false;
        if (out_held) *out_held = false;
        if (g.ActiveId == id) SetActiveId(0);
        return false;
    }

    bool pressed = false;
    const bool hovered = IsHovered(bb, id, (flags & ImGuiButtonFlags_FlattenChilds) != 0);
    if (hovered)
    {
        g.HoveredId = id;
        if (allow_key_modifiers || (!g.IO.KeyCtrl && !g.IO.KeyShift && !g.IO.KeyAlt))
        {
            if (g.IO.MouseClicked[0])
            {
                if (flags & ImGuiButtonFlags_PressedOnClick)
                {
                    pressed = true;
                    SetActiveId(0);
                }
                else
                {
                    SetActiveId(id, window);
                }
                FocusWindow(window);
            }
            else if (g.IO.MouseReleased[0] && (flags & ImGuiButtonFlags_PressedOnRelease))
            {
                pressed = true;
                SetActiveId(0);
            }
            else if ((flags & ImGuiButtonFlags_Repeat) && g.ActiveId == id && ImGui::IsMouseClicked(0, true))
            {
                pressed = true;
            }
        }
    }

    bool held = false;
    if (g.ActiveId == id)
    {
        if (g.IO.MouseDown[0])
        {
            held = true;
        }
        else
        {
            if (hovered)
                pressed = true;
            SetActiveId(0);
        }
    }

    if (out_hovered) *out_hovered = hovered;
    if (out_held) *out_held = held;

    return pressed;
}

static inline ImVec2 CalcItemSize(ImVec2 size, float default_x, float default_y)
{
    ImGuiState& g = *GImGui;
    ImVec2 content_max;
    if (size.x < 0.0f || size.y < 0.0f)
        content_max = g.CurrentWindow->Pos + ImGui::GetContentRegionMax();
    if (size.x <= 0.0f)
        size.x = (size.x == 0.0f) ? default_x : ImMax(content_max.x - g.CurrentWindow->DC.CursorPos.x, 4.0f) + size.x;
    if (size.y <= 0.0f)
        size.y = (size.y == 0.0f) ? default_y : ImMax(content_max.y - g.CurrentWindow->DC.CursorPos.y, 4.0f) + size.y;
    return size;
}

static bool ButtonEx(const char* label, const ImVec2& size_arg = ImVec2(0,0), ImGuiButtonFlags flags = 0)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiState& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
        pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(bb, &id))
        return false;

    if (window->DC.ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, true, flags);

    // Render
    const ImU32 col = window->Color((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
    RenderTextClipped(bb.Min, bb.Max, label, NULL, &label_size, ImGuiAlign_Center | ImGuiAlign_VCenter);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    ImGui::CloseCurrentPopup();

    return pressed;
}

bool ImGui::Button(const char* label, const ImVec2& size_arg)
{
    return ButtonEx(label, size_arg, 0);
}

// Small buttons fits within text without additional vertical spacing.
bool ImGui::SmallButton(const char* label)
{
    ImGuiState& g = *GImGui;
    float backup_padding_y = g.Style.FramePadding.y;
    g.Style.FramePadding.y = 0.0f;
    bool pressed = ButtonEx(label, ImVec2(0,0), ImGuiButtonFlags_AlignTextBaseLine);
    g.Style.FramePadding.y = backup_padding_y;
    return pressed;
}

// Tip: use ImGui::PushID()/PopID() to push indices or pointers in the ID stack.
// Then you can keep 'str_id' empty or the same for all your buttons (instead of creating a string based on a non-string id)
bool ImGui::InvisibleButton(const char* str_id, const ImVec2& size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiID id = window->GetID(str_id);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb);
    if (!ItemAdd(bb, &id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, true);

    return pressed;
}

// Upper-right button to close a window.
static bool CloseWindowButton(bool* p_opened)
{
    ImGuiWindow* window = GetCurrentWindow();

    const ImGuiID id = window->GetID("#CLOSE");
    const float size = window->TitleBarHeight() - 4.0f;
    const ImRect bb(window->Rect().GetTR() + ImVec2(-3.0f-size,2.0f), window->Rect().GetTR() + ImVec2(-3.0f,2.0f+size));

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, true);

    // Render
    const ImU32 col = window->Color((held && hovered) ? ImGuiCol_CloseButtonActive : hovered ? ImGuiCol_CloseButtonHovered : ImGuiCol_CloseButton);
    const ImVec2 center = bb.GetCenter();
    window->DrawList->AddCircleFilled(center, ImMax(2.0f,size*0.5f), col, 16);

    const float cross_extent = (size * 0.5f * 0.7071f) - 1.0f;
    if (hovered)
    {
        window->DrawList->AddLine(center + ImVec2(+cross_extent,+cross_extent), center + ImVec2(-cross_extent,-cross_extent), window->Color(ImGuiCol_Text));
        window->DrawList->AddLine(center + ImVec2(+cross_extent,-cross_extent), center + ImVec2(-cross_extent,+cross_extent), window->Color(ImGuiCol_Text));
    }

    if (p_opened != NULL && pressed)
        *p_opened = !*p_opened;

    return pressed;
}

void ImGui::Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    if (border_col.w > 0.0f)
        bb.Max += ImVec2(2,2);
    ItemSize(bb);
    if (!ItemAdd(bb, NULL))
        return;

    if (border_col.w > 0.0f)
    {
        window->DrawList->AddRect(bb.Min, bb.Max, window->Color(border_col), 0.0f);
        window->DrawList->AddImage(user_texture_id, bb.Min+ImVec2(1,1), bb.Max-ImVec2(1,1), uv0, uv1, window->Color(tint_col));
    }
    else
    {
        window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max, uv0, uv1, window->Color(tint_col));
    }
}

// frame_padding < 0: uses FramePadding from style (default)
// frame_padding = 0: no framing
// frame_padding > 0: set framing size
// The color used are the button colors.
bool ImGui::ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;

    // Default to using texture ID as ID. User can still push string/integer prefixes.
    // We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
    ImGui::PushID((void *)user_texture_id);
    const ImGuiID id = window->GetID("#image");
    ImGui::PopID();

    const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding*2);
    const ImRect image_bb(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size); 
    ItemSize(bb);
    if (!ItemAdd(bb, &id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, true);

    // Render
    const ImU32 col = window->Color((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    RenderFrame(bb.Min, bb.Max, col);
    if (bg_col.w > 0.0f)
        window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, window->Color(bg_col));
    window->DrawList->AddImage(user_texture_id, image_bb.Min, image_bb.Max, uv0, uv1, window->Color(tint_col));

    return pressed;
}

// Start logging ImGui output to TTY
void ImGui::LogToTTY(int max_depth)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (g.LogEnabled)
        return;

    g.LogEnabled = true;
    g.LogFile = stdout;
    g.LogStartDepth = window->DC.TreeDepth;
    if (max_depth >= 0)
        g.LogAutoExpandMaxDepth = max_depth;
}

// Start logging ImGui output to given file
void ImGui::LogToFile(int max_depth, const char* filename)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (g.LogEnabled)
        return;
    if (!filename)
    {
        filename = g.IO.LogFilename;
        if (!filename)
            return;
    }

    g.LogFile = fopen(filename, "ab");
    if (!g.LogFile)
    {
        IM_ASSERT(g.LogFile != NULL); // Consider this an error
        return;
    }
    g.LogEnabled = true;
    g.LogStartDepth = window->DC.TreeDepth;
    if (max_depth >= 0)
        g.LogAutoExpandMaxDepth = max_depth;
}

// Start logging ImGui output to clipboard
void ImGui::LogToClipboard(int max_depth)
{
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiState& g = *GImGui;
    if (g.LogEnabled)
        return;

    g.LogEnabled = true;
    g.LogFile = NULL;
    g.LogStartDepth = window->DC.TreeDepth;
    if (max_depth >= 0)
        g.LogAutoExpandMaxDepth = max_depth;
}

void ImGui::LogFinish()
{
    ImGuiState& g = *GImGui;
    if (!g.LogEnabled)
        return;

    ImGui::LogText(IM_NEWLINE);
    g.LogEnabled = false;
    if (g.LogFile != NULL)
    {
        if (g.LogFile == stdout)
            fflush(g.LogFile);
        else
            fclose(g.LogFile);
        g.LogFile = NULL;
    }
    if (g.LogClipboard->size() > 1)
    {
        if (g.IO.SetClipboardTextFn)
            g.IO.SetClipboardTextFn(g.LogClipboard->begin());
        g.LogClipboard->clear();
    }
}

// Helper to display logging buttons
void ImGui::LogButtons()
{
    ImGuiState& g = *GImGui;

    ImGui::PushID("LogButtons");
    const bool log_to_tty = ImGui::Button("Log To TTY");
    ImGui::SameLine();      
    const bool log_to_file = ImGui::Button("Log To File");
    ImGui::SameLine();
    const bool log_to_clipboard = ImGui::Button("Log To Clipboard");
    ImGui::SameLine();

    ImGui::PushItemWidth(80.0f);
    ImGui::PushAllowKeyboardFocus(false);
    ImGui::SliderInt("Depth", &g.LogAutoExpandMaxDepth, 0, 9, NULL);
    ImGui::PopAllowKeyboardFocus();
    ImGui::PopItemWidth();
    ImGui::PopID();

    // Start logging at the end of the function so that the buttons don't appear in the log
    if (log_to_tty)
        LogToTTY(g.LogAutoExpandMaxDepth);
    if (log_to_file)
        LogToFile(g.LogAutoExpandMaxDepth, g.IO.LogFilename);
    if (log_to_clipboard)
        LogToClipboard(g.LogAutoExpandMaxDepth);
}

bool ImGui::CollapsingHeader(const char* label, const char* str_id, bool display_frame, bool default_open)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;

    IM_ASSERT(str_id != NULL || label != NULL);
    if (str_id == NULL)
        str_id = label;
    if (label == NULL)
        label = str_id;
    const ImGuiID id = window->GetID(str_id);

    // We only write to the tree storage if the user clicks (or explicitely use SetNextTreeNode*** functions)
    ImGuiStorage* storage = window->DC.StateStorage;
    bool opened;
    if (g.SetNextTreeNodeOpenedCond != 0)
    {
        if (g.SetNextTreeNodeOpenedCond & ImGuiSetCond_Always)
        {
            opened = g.SetNextTreeNodeOpenedVal;
            storage->SetInt(id, opened);
        }
        else
        {
            // We treat ImGuiSetCondition_Once and ImGuiSetCondition_FirstUseEver the same because tree node state are not saved persistently.
            const int stored_value = storage->GetInt(id, -1);
            if (stored_value == -1)
            {
                opened = g.SetNextTreeNodeOpenedVal;
                storage->SetInt(id, opened);
            }
            else
            {
                opened = stored_value != 0;
            }
        }
        g.SetNextTreeNodeOpenedCond = 0;
    }
    else
    {
        opened = storage->GetInt(id, default_open) != 0;
    }

    // Framed header expand a little outside the default padding
    const ImVec2 window_padding = window->WindowPadding;
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImVec2 pos_min = window->DC.CursorPos;
    const ImVec2 pos_max = window->Pos + GetContentRegionMax();
    ImRect bb = ImRect(pos_min, ImVec2(pos_max.x, pos_min.y + label_size.y));
    if (display_frame)
    {
        bb.Min.x -= (float)(int)(window_padding.x*0.5f) - 1;
        bb.Max.x += (float)(int)(window_padding.x*0.5f) - 1;
        bb.Max.y += style.FramePadding.y * 2;
    }

    // FIXME: we don't provide our width so that it doesn't get feed back into AutoFit. Should manage that better so we can still hover without extending ContentsSize
    const ImRect text_bb(bb.Min, bb.Min + ImVec2(g.FontSize + style.FramePadding.x*2*2,0) + label_size);
    ItemSize(ImVec2(text_bb.GetSize().x, bb.GetSize().y), display_frame ? style.FramePadding.y : 0.0f);

    // When logging is enabled, if automatically expand tree nodes (but *NOT* collapsing headers.. seems like sensible behavior).
    // NB- If we are above max depth we still allow manually opened nodes to be logged.
    if (g.LogEnabled && !display_frame && window->DC.TreeDepth < g.LogAutoExpandMaxDepth)
        opened = true;

    if (!ItemAdd(bb, &id))
        return opened;

    bool hovered, held;
    bool pressed = ButtonBehavior(display_frame ? bb : text_bb, id, &hovered, &held, false);
    if (pressed)
    {
        opened = !opened;
        storage->SetInt(id, opened);
    }

    // Render
    const ImU32 col = window->Color((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
    if (display_frame)
    {
        // Framed type
        RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
        RenderCollapseTriangle(bb.Min + style.FramePadding, opened, 1.0f, true);
        if (g.LogEnabled)
        {
            // NB: '##' is normally used to hide text (as a library-wide feature), so we need to specify the text range to make sure the ## aren't stripped out here.
            const char log_prefix[] = "\n##";
            LogText(bb.Min + style.FramePadding, log_prefix, log_prefix+3);
        }
        RenderTextClipped(bb.Min + style.FramePadding + ImVec2(g.FontSize + style.FramePadding.x*2,0), bb.Max, label, NULL, &label_size);
        if (g.LogEnabled)
        {
            const char log_suffix[] = "##";
            LogText(bb.Min + style.FramePadding, log_suffix, log_suffix+2);
        }
    }
    else
    {
        // Unframed typed for tree nodes
        if ((held && hovered) || hovered)
            RenderFrame(bb.Min, bb.Max, col, false);
        RenderCollapseTriangle(bb.Min + ImVec2(style.FramePadding.x, g.FontSize*0.15f), opened, 0.70f, false);
        if (g.LogEnabled)
            LogText(bb.Min, ">");
        RenderText(bb.Min + ImVec2(g.FontSize + style.FramePadding.x*2,0), label);
    }

    return opened;
}

void ImGui::Bullet()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImGuiStyle& style = g.Style;
    const float line_height = g.FontSize;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(line_height, line_height));
    ItemSize(bb);
    if (!ItemAdd(bb, NULL))
        return;

    // Render
    const float bullet_size = line_height*0.15f;
    window->DrawList->AddCircleFilled(bb.Min + ImVec2(style.FramePadding.x + line_height*0.5f, line_height*0.5f), bullet_size, window->Color(ImGuiCol_Text));

    // Stay on same line
    ImGui::SameLine(0, -1);
}

// Text with a little bullet aligned to the typical tree node.
void ImGui::BulletTextV(const char* fmt, va_list args)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    
    const char* text_begin = g.TempBuffer;
    const char* text_end = text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);

    const ImGuiStyle& style = g.Style;
    const float line_height = g.FontSize;
    const ImVec2 label_size = CalcTextSize(text_begin, text_end, true);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(line_height + (label_size.x > 0.0f ? (style.FramePadding.x*2) : 0.0f),0) + label_size);  // Empty text doesn't add padding
    ItemSize(bb);
    if (!ItemAdd(bb, NULL))
        return;

    // Render
    const float bullet_size = line_height*0.15f;
    window->DrawList->AddCircleFilled(bb.Min + ImVec2(style.FramePadding.x + line_height*0.5f, line_height*0.5f), bullet_size, window->Color(ImGuiCol_Text));
    RenderText(bb.Min+ImVec2(g.FontSize + style.FramePadding.x*2,0), text_begin, text_end);
}

void ImGui::BulletText(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    BulletTextV(fmt, args);
    va_end(args);
}

// If returning 'true' the node is open and the user is responsible for calling TreePop
bool ImGui::TreeNodeV(const char* str_id, const char* fmt, va_list args)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);

    if (!str_id || !str_id[0])
        str_id = fmt;

    ImGui::PushID(str_id);
    const bool opened = ImGui::CollapsingHeader(g.TempBuffer, "", false);
    ImGui::PopID();

    if (opened)
        ImGui::TreePush(str_id);

    return opened;
}

bool ImGui::TreeNode(const char* str_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool s = TreeNodeV(str_id, fmt, args);
    va_end(args);
    return s;
}

// If returning 'true' the node is open and the user is responsible for calling TreePop
bool ImGui::TreeNodeV(const void* ptr_id, const char* fmt, va_list args)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);

    if (!ptr_id)
        ptr_id = fmt;

    ImGui::PushID(ptr_id);
    const bool opened = ImGui::CollapsingHeader(g.TempBuffer, "", false);
    ImGui::PopID();

    if (opened)
        ImGui::TreePush(ptr_id);

    return opened;
}

bool ImGui::TreeNode(const void* ptr_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool s = TreeNodeV(ptr_id, fmt, args);
    va_end(args);
    return s;
}

bool ImGui::TreeNode(const char* str_label_id)
{
    return TreeNode(str_label_id, "%s", str_label_id);
}

void ImGui::SetNextTreeNodeOpened(bool opened, ImGuiSetCond cond)
{
    ImGuiState& g = *GImGui;
    g.SetNextTreeNodeOpenedVal = opened;
    g.SetNextTreeNodeOpenedCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::PushID(const char* str_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.push_back(window->GetID(str_id));
}

void ImGui::PushID(const char* str_id_begin, const char* str_id_end)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.push_back(window->GetID(str_id_begin, str_id_end));
}

void ImGui::PushID(const void* ptr_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.push_back(window->GetID(ptr_id));
}

void ImGui::PushID(int int_id)
{
    const void* ptr_id = (void*)(intptr_t)int_id;
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.push_back(window->GetID(ptr_id));
}

void ImGui::PopID()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.pop_back();
}

ImGuiID ImGui::GetID(const char* str_id)
{
    return GImGui->CurrentWindow->GetID(str_id);
}

ImGuiID ImGui::GetID(const char* str_id_begin, const char* str_id_end)
{
    return GImGui->CurrentWindow->GetID(str_id_begin, str_id_end);
}

ImGuiID ImGui::GetID(const void* ptr_id)
{
    return GImGui->CurrentWindow->GetID(ptr_id);
}

// User can input math operators (e.g. +100) to edit a numerical values.
// NB: only call right after InputText because we are using its InitialValue storage
static void InputTextApplyArithmeticOp(const char* buf, float *v)
{
    while (ImCharIsSpace(*buf))
        buf++;

    // We don't support '-' op because it would conflict with inputing negative value.
    // Instead you can use +-100 to subtract from an existing value
    char op = buf[0];
    if (op == '+' || op == '*' || op == '/')
    {
        buf++;
        while (ImCharIsSpace(*buf))
            buf++;
    }
    else
    {
        op = 0;
    }
    if (!buf[0])
        return;

    float ref_v = *v;
    if (op)
        if (sscanf(GImGui->InputTextState.InitialText.begin(), "%f", &ref_v) < 1)
            return;

    float op_v = 0.0f;
    if (sscanf(buf, "%f", &op_v) < 1)
        return;

    if (op == '+')
        *v = ref_v + op_v;  // add (uses "+-" to substract)
    else if (op == '*')
        *v = ref_v * op_v;  // multiply
    else if (op == '/')
    {
        if (op_v == 0.0f)   // divide
            return;
        *v = ref_v / op_v;
    }
    else
        *v = op_v;          // Constant
}

// Create text input in place of a slider (when CTRL+Clicking on slider)
static bool InputFloatReplaceWidget(const ImRect& aabb, const char* label, float* v, ImGuiID id, int decimal_precision)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    char text_buf[64];
    ImFormatString(text_buf, IM_ARRAYSIZE(text_buf), "%.*f", decimal_precision, *v);

    SetActiveId(g.ScalarAsInputTextId, window);
    g.HoveredId = 0;

    // Our replacement widget will override the focus ID (registered previously to allow for a TAB focus to happen)
    window->FocusItemUnregister();

    bool value_changed = InputTextEx(label, text_buf, (int)IM_ARRAYSIZE(text_buf), aabb.GetSize() - g.Style.FramePadding*2.0f, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll);
    if (g.ScalarAsInputTextId == 0)
    {
        // First frame
        IM_ASSERT(g.ActiveId == id);    // InputText ID expected to match the Slider ID (else we'd need to store them both, which is also possible)
        g.ScalarAsInputTextId = g.ActiveId;
        g.HoveredId = id;
    }
    else if (g.ActiveId != g.ScalarAsInputTextId)
    {
        // Release
        g.ScalarAsInputTextId = 0;
    }
    if (value_changed)
    {
        InputTextApplyArithmeticOp(text_buf, v);
    }
    return value_changed;
}

// Parse display precision back from the display format string
static inline int ParseFormatPrecision(const char* fmt, int default_precision)
{
    int precision = default_precision;
    while ((fmt = strchr(fmt, '%')) != NULL)
    {
        fmt++;
        if (fmt[0] == '%') { fmt++; continue; } // Ignore "%%"
        while (*fmt >= '0' && *fmt <= '9')
            fmt++;
        if (*fmt == '.')
        {
            precision = atoi(fmt + 1);
            if (precision < 0 || precision > 10)
                precision = default_precision;
        }
        break;
    }
    return precision;
}

static inline float RoundScalar(float value, int decimal_precision)
{
    // Round past decimal precision
    //    0: 1, 1: 0.1, 2: 0.01, etc.
    // So when our value is 1.99999 with a precision of 0.001 we'll end up rounding to 2.0
    // FIXME: Investigate better rounding methods
    float min_step = 1.0f / powf(10.0f, (float)decimal_precision);
    bool negative = value < 0.0f;
    value = fabsf(value);
    float remainder = fmodf(value, min_step);
    if (remainder <= min_step*0.5f)
        value -= remainder;
    else
        value += (min_step - remainder);
    return negative ? -value : value;
}

static bool SliderBehavior(const ImRect& frame_bb, ImGuiID id, float* v, float v_min, float v_max, float power, int decimal_precision, bool horizontal)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    const ImGuiStyle& style = g.Style;

    // Draw frame
    RenderFrame(frame_bb.Min, frame_bb.Max, window->Color(ImGuiCol_FrameBg), true, style.FrameRounding);

    const bool is_non_linear = fabsf(power - 1.0f) > 0.0001f;

    const float padding = horizontal ? style.FramePadding.x : style.FramePadding.y;
    const float slider_sz = horizontal ? (frame_bb.GetWidth() - padding * 2.0f) : (frame_bb.GetHeight() - padding * 2.0f);
    float grab_sz;
    if (decimal_precision > 0)
        grab_sz = ImMin(style.GrabMinSize, slider_sz);
    else
        grab_sz = ImMin(ImMax(1.0f * (slider_sz / (v_max-v_min+1.0f)), style.GrabMinSize), slider_sz);  // Integer sliders, if possible have the grab size represent 1 unit
    const float slider_usable_sz = slider_sz - grab_sz;
    const float slider_usable_pos_min = (horizontal ? frame_bb.Min.x : frame_bb.Min.y) + padding + grab_sz*0.5f;
    const float slider_usable_pos_max = (horizontal ? frame_bb.Max.x : frame_bb.Max.y) - padding - grab_sz*0.5f;

    bool value_changed = false;

    // For logarithmic sliders that cross over sign boundary we want the exponential increase to be symmetric around 0.0f
    float linear_zero_pos = 0.0f;   // 0.0->1.0f
    if (v_min * v_max < 0.0f)
    {
        // Different sign
        const float linear_dist_min_to_0 = powf(fabsf(0.0f - v_min), 1.0f/power);
        const float linear_dist_max_to_0 = powf(fabsf(v_max - 0.0f), 1.0f/power);
        linear_zero_pos = linear_dist_min_to_0 / (linear_dist_min_to_0+linear_dist_max_to_0);
    }
    else
    {
        // Same sign
        linear_zero_pos = v_min < 0.0f ? 1.0f : 0.0f;
    }

    // Process clicking on the slider
    if (g.ActiveId == id)
    {
        if (g.IO.MouseDown[0])
        {
            const float mouse_abs_pos = horizontal ? g.IO.MousePos.x : g.IO.MousePos.y;
            float normalized_pos = ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f);
            if (!horizontal)
                normalized_pos = 1.0f - normalized_pos;

            float new_value;
            if (is_non_linear)
            {
                // Account for logarithmic scale on both sides of the zero
                if (normalized_pos < linear_zero_pos)
                {
                    // Negative: rescale to the negative range before powering
                    float a = 1.0f - (normalized_pos / linear_zero_pos);
                    a = powf(a, power);
                    new_value = ImLerp(ImMin(v_max,0.0f), v_min, a);
                }
                else
                {
                    // Positive: rescale to the positive range before powering
                    float a;
                    if (fabsf(linear_zero_pos - 1.0f) > 1.e-6)
                        a = (normalized_pos - linear_zero_pos) / (1.0f - linear_zero_pos);
                    else
                        a = normalized_pos;
                    a = powf(a, power);
                    new_value = ImLerp(ImMax(v_min,0.0f), v_max, a);
                }
            }
            else
            {
                // Linear slider
                new_value = ImLerp(v_min, v_max, normalized_pos);
            }

            // Round past decimal precision
            new_value = RoundScalar(new_value, decimal_precision);
            if (*v != new_value)
            {
                *v = new_value;
                value_changed = true;
            }
        }
        else
        {
            SetActiveId(0);
        }
    }

    // Calculate slider grab positioning
    float grab_t;
    if (is_non_linear)
    {
        float v_clamped = ImClamp(*v, v_min, v_max);
        if (v_clamped < 0.0f)
        {
            const float f = 1.0f - (v_clamped - v_min) / (ImMin(0.0f,v_max) - v_min);
            grab_t = (1.0f - powf(f, 1.0f/power)) * linear_zero_pos;
        }
        else
        {
            const float f = (v_clamped - ImMax(0.0f,v_min)) / (v_max - ImMax(0.0f,v_min));
            grab_t = linear_zero_pos + powf(f, 1.0f/power) * (1.0f - linear_zero_pos);
        }
    }
    else
    {
        // Linear slider
        grab_t = (ImClamp(*v, v_min, v_max) - v_min) / (v_max - v_min);
    }

    // Draw
    if (!horizontal)
        grab_t = 1.0f - grab_t;
    const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
    ImRect grab_bb;
    if (horizontal)
        grab_bb = ImRect(ImVec2(grab_pos - grab_sz*0.5f, frame_bb.Min.y + 2.0f), ImVec2(grab_pos + grab_sz*0.5f, frame_bb.Max.y - 2.0f));
    else
        grab_bb = ImRect(ImVec2(frame_bb.Min.x + 2.0f, grab_pos - grab_sz*0.5f), ImVec2(frame_bb.Max.x - 2.0f, grab_pos + grab_sz*0.5f));
    window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, window->Color(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

    return value_changed;
}

// Use power!=1.0 for logarithmic sliders.
// Adjust display_format to decorate the value with a prefix or a suffix.
//   "%.3f"         1.234
//   "%5.2f secs"   01.23 secs
//   "Gold: %.0f"   Gold: 1
bool ImGui::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* display_format, float power)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = ImGui::CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y) + style.FramePadding*2.0f);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    // NB- we don't call ItemSize() yet because we may turn into a text edit box below
    if (!ItemAdd(total_bb, &id))
    {
        ItemSize(total_bb, style.FramePadding.y);
        return false;
    }

    const bool hovered = IsHovered(frame_bb, id);
    if (hovered)
        g.HoveredId = id;

    if (!display_format)
        display_format = "%.3f";
    int decimal_precision = ParseFormatPrecision(display_format, 3);

    // Tabbing or CTRL-clicking on Slider turns it into an input box
    bool start_text_input = false;
    const bool tab_focus_requested = window->FocusItemRegister(g.ActiveId == id);
    if (tab_focus_requested || (hovered && g.IO.MouseClicked[0]))
    {
        SetActiveId(id, window);
        FocusWindow(window);

        if (tab_focus_requested || g.IO.KeyCtrl)
        {
            start_text_input = true;
            g.ScalarAsInputTextId = 0;
        }
    }
    if (start_text_input || (g.ActiveId == id && g.ScalarAsInputTextId == id))
        return InputFloatReplaceWidget(frame_bb, label, v, id, decimal_precision);

    ItemSize(total_bb, style.FramePadding.y);

    // Actual slider behavior + render grab
    const bool value_changed = SliderBehavior(frame_bb, id, v, v_min, v_max, power, decimal_precision, true);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), display_format, *v);
    RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImGuiAlign_Center|ImGuiAlign_VCenter);

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    return value_changed;
}

bool ImGui::VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* display_format, float power)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const ImRect bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(frame_bb, &id))
        return false;

    const bool hovered = IsHovered(frame_bb, id);
    if (hovered)
        g.HoveredId = id;

    if (!display_format)
        display_format = "%.3f";
    int decimal_precision = ParseFormatPrecision(display_format, 3);

    if (hovered && g.IO.MouseClicked[0])
    {
        SetActiveId(id, window);
        FocusWindow(window);
    }

    // Actual slider behavior + render grab
    bool value_changed = SliderBehavior(frame_bb, id, v, v_min, v_max, power, decimal_precision, false);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    // For the vertical slider we allow centered text to overlap the frame padding
    char value_buf[64];
    char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), display_format, *v);
    RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, value_buf, value_buf_end, NULL, ImGuiAlign_Center);

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    return value_changed;
}

bool ImGui::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max)
{
    float v_deg = (*v_rad) * 360.0f / (2*IM_PI);
    bool value_changed = ImGui::SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, "%.0f deg", 1.0f);
    *v_rad = v_deg * (2*IM_PI) / 360.0f;
    return value_changed;
}

bool ImGui::SliderInt(const char* label, int* v, int v_min, int v_max, const char* display_format)
{
    if (!display_format)
        display_format = "%.0f";
    float v_f = (float)*v;
    bool value_changed = ImGui::SliderFloat(label, &v_f, (float)v_min, (float)v_max, display_format, 1.0f);
    *v = (int)v_f;
    return value_changed;
}

bool ImGui::VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* display_format)
{
    if (!display_format)
        display_format = "%.0f";
    float v_f = (float)*v;
    bool value_changed = ImGui::VSliderFloat(label, size, &v_f, (float)v_min, (float)v_max, display_format, 1.0f);
    *v = (int)v_f;
    return value_changed;
}

// Add multiple sliders on 1 line for compact edition of multiple components
static bool SliderFloatN(const char* label, float* v, int components, float v_min, float v_max, const char* display_format, float power)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    bool value_changed = false;
    ImGui::BeginGroup();
    ImGui::PushID(label);
    PushMultiItemsWidths(components);
    for (int i = 0; i < components; i++)
    {
        ImGui::PushID(i);
        value_changed |= ImGui::SliderFloat("##v", &v[i], v_min, v_max, display_format, power);
        ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();

    ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
    ImGui::EndGroup();

    return value_changed;
}

bool ImGui::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* display_format, float power)
{
    return SliderFloatN(label, v, 2, v_min, v_max, display_format, power);
}

bool ImGui::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* display_format, float power)
{
    return SliderFloatN(label, v, 3, v_min, v_max, display_format, power);
}

bool ImGui::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* display_format, float power)
{
    return SliderFloatN(label, v, 4, v_min, v_max, display_format, power);
}

static bool SliderIntN(const char* label, int* v, int components, int v_min, int v_max, const char* display_format)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    bool value_changed = false;
    ImGui::BeginGroup();
    ImGui::PushID(label);
    PushMultiItemsWidths(components);
    for (int i = 0; i < components; i++)
    {
        ImGui::PushID(i);
        value_changed |= ImGui::SliderInt("##v", &v[i], v_min, v_max, display_format);
        ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();

    ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
    ImGui::EndGroup();

    return value_changed;
}

bool ImGui::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* display_format)
{
    return SliderIntN(label, v, 2, v_min, v_max, display_format);
}

bool ImGui::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* display_format)
{
    return SliderIntN(label, v, 3, v_min, v_max, display_format);
}

bool ImGui::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* display_format)
{
    return SliderIntN(label, v, 4, v_min, v_max, display_format);
}

// FIXME-WIP: Work in progress. May change API / behavior.
static bool DragBehavior(const ImRect& frame_bb, ImGuiID id, float* v, float v_speed, float v_min, float v_max, int decimal_precision, float power)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    const ImGuiStyle& style = g.Style;

    // Draw frame
    const ImU32 frame_col = window->Color(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

    bool value_changed = false;

    // Process clicking on the drag
    if (g.ActiveId == id)
    {
        if (g.IO.MouseDown[0])
        {
            if (g.ActiveIdIsJustActivated)
            {
                // Lock current value on click
                g.DragCurrentValue = *v;
                g.DragLastMouseDelta = ImVec2(0.f, 0.f);
            }

            const ImVec2 mouse_drag_delta = ImGui::GetMouseDragDelta(0, 1.0f);
            if (fabsf(mouse_drag_delta.x - g.DragLastMouseDelta.x) > 0.0f)
            {
                float speed = v_speed;
                if (speed == 0.0f && (v_max - v_min) != 0.0f && (v_max - v_min) < FLT_MAX)
                    speed = (v_max - v_min) * g.DragSpeedDefaultRatio;
                if (g.IO.KeyShift && g.DragSpeedScaleFast >= 0.0f)
                    speed = speed * g.DragSpeedScaleFast;
                if (g.IO.KeyAlt && g.DragSpeedScaleSlow >= 0.0f)
                    speed = speed * g.DragSpeedScaleSlow;

                float v_cur = g.DragCurrentValue;
                float delta = (mouse_drag_delta.x - g.DragLastMouseDelta.x) * speed;
                if (fabsf(power - 1.0f) > 0.001f)
                {
                    // Logarithmic curve on both side of 0.0
                    float v0_abs = v_cur >= 0.0f ? v_cur : -v_cur;
                    float v0_sign = v_cur >= 0.0f ? 1.0f : -1.0f;
                    float v1 = powf(v0_abs, 1.0f / power) + (delta * v0_sign);
                    float v1_abs = v1 >= 0.0f ? v1 : -v1;
                    float v1_sign = v1 >= 0.0f ? 1.0f : -1.0f;          // Crossed sign line
                    v_cur = powf(v1_abs, power) * v0_sign * v1_sign;    // Reapply sign
                }
                else
                {
                    v_cur += delta;
                }
                g.DragLastMouseDelta.x = mouse_drag_delta.x;

                // Clamp
                if (v_min < v_max)
                    v_cur = ImClamp(v_cur, v_min, v_max);
                g.DragCurrentValue = v_cur;

                // Round to user desired precision, then apply
                v_cur = RoundScalar(v_cur, decimal_precision);
                if (*v != v_cur)
                {
                    *v = v_cur;
                    value_changed = true;
                }
            }
        }
        else
        {
            SetActiveId(0);
        }
    }

    return value_changed;
}

bool ImGui::DragFloat(const char* label, float *v, float v_speed, float v_min, float v_max, const char* display_format, float power)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = ImGui::CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y) + style.FramePadding*2.0f);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    // NB- we don't call ItemSize() yet because we may turn into a text edit box below
    if (!ItemAdd(total_bb, &id))
    {
        ItemSize(total_bb, style.FramePadding.y);
        return false;
    }

    const bool hovered = IsHovered(frame_bb, id);
    if (hovered)
        g.HoveredId = id;

    if (!display_format)
        display_format = "%.3f";
    int decimal_precision = ParseFormatPrecision(display_format, 3);

    // Tabbing or CTRL-clicking on Drag turns it into an input box
    bool start_text_input = false;
    const bool tab_focus_requested = window->FocusItemRegister(g.ActiveId == id);
    if (tab_focus_requested || (hovered && (g.IO.MouseClicked[0] | g.IO.MouseDoubleClicked[0])))
    {
        SetActiveId(id, window);
        FocusWindow(window);

        if (tab_focus_requested || g.IO.KeyCtrl || g.IO.MouseDoubleClicked[0])
        {
            start_text_input = true;
            g.ScalarAsInputTextId = 0;
        }
    }
    if (start_text_input || (g.ActiveId == id && g.ScalarAsInputTextId == id))
        return InputFloatReplaceWidget(frame_bb, label, v, id, decimal_precision);

    ItemSize(total_bb, style.FramePadding.y);

    // Actual drag behavior
    const bool value_changed = DragBehavior(frame_bb, id, v, v_speed, v_min, v_max, decimal_precision, power);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), display_format, *v);
    RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImGuiAlign_Center|ImGuiAlign_VCenter);

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);

    return value_changed;
}

static bool DragFloatN(const char* label, float* v, int components, float v_speed, float v_min, float v_max, const char* display_format, float power)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    bool value_changed = false;
    ImGui::BeginGroup();
    ImGui::PushID(label);
    PushMultiItemsWidths(components);
    for (int i = 0; i < components; i++)
    {
        ImGui::PushID(i);
        value_changed |= ImGui::DragFloat("##v", &v[i], v_speed, v_min, v_max, display_format, power);
        ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();

    ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
    ImGui::EndGroup();

    return value_changed;
}

bool ImGui::DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* display_format, float power)
{
    return DragFloatN(label, v, 2, v_speed, v_min, v_max, display_format, power);
}

bool ImGui::DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* display_format, float power)
{
    return DragFloatN(label, v, 3, v_speed, v_min, v_max, display_format, power);
}

bool ImGui::DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* display_format, float power)
{
    return DragFloatN(label, v, 4, v_speed, v_min, v_max, display_format, power);
}

bool ImGui::DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* display_format, const char* display_format_max, float power)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGui::PushID(label);
    ImGui::BeginGroup();
    PushMultiItemsWidths(2);

    bool value_changed = ImGui::DragFloat("##min", v_current_min, v_speed, (v_min >= v_max) ? -FLT_MAX : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), display_format, power);
    ImGui::PopItemWidth();
    ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= ImGui::DragFloat("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? FLT_MAX : v_max, display_format_max ? display_format_max : display_format, power);
    ImGui::PopItemWidth();
    ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

    ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
    ImGui::EndGroup();
    ImGui::PopID();

    return value_changed;
}

// NB: v_speed is float to allow adjusting the drag speed with more precision
bool ImGui::DragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
{
    if (!display_format)
        display_format = "%.0f";
    float v_f = (float)*v;
    bool value_changed = ImGui::DragFloat(label, &v_f, v_speed, (float)v_min, (float)v_max, display_format);
    *v = (int)v_f;
    return value_changed;
}

static bool DragIntN(const char* label, int* v, int components, float v_speed, int v_min, int v_max, const char* display_format)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    bool value_changed = false;
    ImGui::BeginGroup();
    ImGui::PushID(label);
    PushMultiItemsWidths(components);
    for (int i = 0; i < components; i++)
    {
        ImGui::PushID(i);
        value_changed |= ImGui::DragInt("##v", &v[i], v_speed, v_min, v_max, display_format);
        ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();

    ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
    ImGui::EndGroup();

    return value_changed;
}

bool ImGui::DragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* display_format)
{
    return DragIntN(label, v, 2, v_speed, v_min, v_max, display_format);
}

bool ImGui::DragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* display_format)
{
    return DragIntN(label, v, 3, v_speed, v_min, v_max, display_format);
}

bool ImGui::DragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* display_format)
{
    return DragIntN(label, v, 4, v_speed, v_min, v_max, display_format);
}

bool ImGui::DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* display_format, const char* display_format_max)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGui::PushID(label);
    ImGui::BeginGroup();
    PushMultiItemsWidths(2);

    bool value_changed = ImGui::DragInt("##min", v_current_min, v_speed, (v_min >= v_max) ? IM_INT_MIN : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), display_format);
    ImGui::PopItemWidth();
    ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= ImGui::DragInt("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? IM_INT_MAX : v_max, display_format_max ? display_format_max : display_format);
    ImGui::PopItemWidth();
    ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

    ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
    ImGui::EndGroup();
    ImGui::PopID();

    return value_changed;
}

enum ImGuiPlotType
{
    ImGuiPlotType_Lines,
    ImGuiPlotType_Histogram
};

static void Plot(ImGuiPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImGuiStyle& style = g.Style;

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    if (graph_size.x == 0.0f)
        graph_size.x = ImGui::CalcItemWidth() + (style.FramePadding.x * 2);
    if (graph_size.y == 0.0f)
        graph_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, NULL))
        return;

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < values_count; i++)
        {
            const float v = values_getter(data, i);
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, window->Color(ImGuiCol_FrameBg), true, style.FrameRounding);

    int res_w = ImMin((int)graph_size.x, values_count);
    if (plot_type == ImGuiPlotType_Lines)
        res_w -= 1;

    // Tooltip on hover
    int v_hovered = -1;
    if (IsHovered(inner_bb, 0))
    {
        const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
        const int v_idx = (int)(t * (values_count + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0)));
        IM_ASSERT(v_idx >= 0 && v_idx < values_count);
        
        const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
        const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
        if (plot_type == ImGuiPlotType_Lines)
            ImGui::SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx+1, v1);
        else if (plot_type == ImGuiPlotType_Histogram)
            ImGui::SetTooltip("%d: %8.4g", v_idx, v0);
        v_hovered = v_idx;
    }

    const float t_step = 1.0f / (float)res_w;

    float v0 = values_getter(data, (0 + values_offset) % values_count);
    float t0 = 0.0f;
    ImVec2 p0 = ImVec2( t0, 1.0f - ImSaturate((v0 - scale_min) / (scale_max - scale_min)) );

    const ImU32 col_base = window->Color((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
    const ImU32 col_hovered = window->Color((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);

    for (int n = 0; n < res_w; n++)
    {
        const float t1 = t0 + t_step;
        const int v_idx = (int)(t0 * values_count);
        IM_ASSERT(v_idx >= 0 && v_idx < values_count);
        const float v1 = values_getter(data, (v_idx + values_offset + 1) % values_count);
        const ImVec2 p1 = ImVec2( t1, 1.0f - ImSaturate((v1 - scale_min) / (scale_max - scale_min)) );

        // NB- Draw calls are merged together by the DrawList system.
        if (plot_type == ImGuiPlotType_Lines)
            window->DrawList->AddLine(ImLerp(inner_bb.Min, inner_bb.Max, p0), ImLerp(inner_bb.Min, inner_bb.Max, p1), v_hovered == v_idx ? col_hovered : col_base);
        else if (plot_type == ImGuiPlotType_Histogram)
            window->DrawList->AddRectFilled(ImLerp(inner_bb.Min, inner_bb.Max, p0), ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(p1.x, 1.0f))+ImVec2(-1,0), v_hovered == v_idx ? col_hovered : col_base);

        t0 = t1;
        p0 = p1;
    }

    // Text overlay
    if (overlay_text)
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImGuiAlign_Center);

    RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}

struct ImGuiPlotArrayGetterData
{
    const float* Values;
    int Stride;

    ImGuiPlotArrayGetterData(const float* values, int stride) { Values = values; Stride = stride; }
};

static float Plot_ArrayGetter(void* data, int idx)
{
    ImGuiPlotArrayGetterData* plot_data = (ImGuiPlotArrayGetterData*)data;
    const float v = *(float*)(void*)((unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
    return v;
}

void ImGui::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterData data(values, stride);
    Plot(ImGuiPlotType_Lines, label, &Plot_ArrayGetter, (void*)&data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotLines(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    Plot(ImGuiPlotType_Lines, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterData data(values, stride);
    Plot(ImGuiPlotType_Histogram, label, &Plot_ArrayGetter, (void*)&data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotHistogram(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    Plot(ImGuiPlotType_Histogram, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

bool ImGui::Checkbox(const char* label, bool* v)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.y*2, label_size.y + style.FramePadding.y*2));
    ItemSize(check_bb, style.FramePadding.y);

    ImRect total_bb = check_bb;
    if (label_size.x > 0)
        SameLine(0, style.ItemInnerSpacing.x);
    const ImRect text_bb(window->DC.CursorPos + ImVec2(0,style.FramePadding.y), window->DC.CursorPos + ImVec2(0,style.FramePadding.y) + label_size);
    if (label_size.x > 0)
    {
        ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
        total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
    }

    if (!ItemAdd(total_bb, &id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held, true);
    if (pressed)
        *v = !(*v);

    RenderFrame(check_bb.Min, check_bb.Max, window->Color((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
    if (*v)
    {
        const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
        const float pad = check_sz < 8.0f ? 1.0f : check_sz < 13.0f ? 2.0f : 3.0f;
        window->DrawList->AddRectFilled(check_bb.Min+ImVec2(pad,pad), check_bb.Max-ImVec2(pad,pad), window->Color(ImGuiCol_CheckMark), style.FrameRounding);
    }

    if (g.LogEnabled)
        LogText(text_bb.GetTL(), *v ? "[x]" : "[ ]");
    RenderText(text_bb.GetTL(), label);

    return pressed;
}

bool ImGui::CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value)
{
    bool v = (*flags & flags_value) ? true : false;
    bool pressed = ImGui::Checkbox(label, &v);
    if (v)
        *flags |= flags_value;
    else
        *flags &= ~flags_value;
    return pressed;
}

bool ImGui::RadioButton(const char* label, bool active)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.y*2-1, label_size.y + style.FramePadding.y*2-1));
    ItemSize(check_bb, style.FramePadding.y);

    ImRect total_bb = check_bb;
    if (label_size.x > 0)
        SameLine(0, style.ItemInnerSpacing.x);
    const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
    if (label_size.x > 0)
    {
        ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
        total_bb.Add(text_bb);
    }

    if (!ItemAdd(total_bb, &id))
        return false;

    ImVec2 center = check_bb.GetCenter();
    center.x = (float)(int)center.x + 0.5f;
    center.y = (float)(int)center.y + 0.5f;
    const float radius = check_bb.GetHeight() * 0.5f;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held, true);

    window->DrawList->AddCircleFilled(center, radius, window->Color((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 16);
    if (active)
    {
        const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
        const float pad = check_sz < 8.0f ? 1.0f : check_sz < 13.0f ? 2.0f : 3.0f;
        window->DrawList->AddCircleFilled(center, radius-pad, window->Color(ImGuiCol_CheckMark), 16);
    }

    if (window->Flags & ImGuiWindowFlags_ShowBorders)
    {
        window->DrawList->AddCircle(center+ImVec2(1,1), radius, window->Color(ImGuiCol_BorderShadow), 16);
        window->DrawList->AddCircle(center, radius, window->Color(ImGuiCol_Border), 16);
    }

    if (g.LogEnabled)
        LogText(text_bb.GetTL(), active ? "(x)" : "( )");
    RenderText(text_bb.GetTL(), label);

    return pressed;
}

bool ImGui::RadioButton(const char* label, int* v, int v_button)
{
    const bool pressed = ImGui::RadioButton(label, *v == v_button);
    if (pressed)
    {
        *v = v_button;
    }
    return pressed;
}

static int InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end)
{
    int line_count = 0;
    const char* s = text_begin;
    while (char c = *s++) // We are only matching for \n so we can ignore UTF-8 decoding
        if (c == '\n')
            line_count++;
    s--;
    if (s[0] != '\n' && s[0] != '\r')
        line_count++;
    *out_text_end = s;
    return line_count;
}

static ImVec2 InputTextCalcTextSizeW(const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining = NULL, ImVec2* out_offset = NULL, bool stop_on_new_line = false)
{
    ImFont* font = GImGui->Font;
    const float line_height = GImGui->FontSize;
    const float scale = line_height / font->FontSize;

    ImVec2 text_size = ImVec2(0,0);
    float line_width = 0.0f;

    const ImWchar* s = text_begin;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)(*s++);
        if (c == '\n')
        {
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += line_height;
            line_width = 0.0f;
            if (stop_on_new_line)
                break;
            continue;
        }
        if (c == '\r')
            continue;

        const float char_width = font->GetCharAdvance((unsigned short)c) * scale;
        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (out_offset)
        *out_offset = ImVec2(line_width, text_size.y + line_height);  // offset allow for the possibility of sitting after a trailing \n

    if (line_width > 0 || text_size.y == 0.0f)                        // whereas size.y will ignore the trailing \n
        text_size.y += line_height;

    if (remaining)
        *remaining = s;

    return text_size;
}

// Wrapper for stb_textedit.h to edit text (our wrapper is for: statically sized buffer, single-line, wchar characters. InputText converts between UTF-8 and wchar)
static int     STB_TEXTEDIT_STRINGLEN(const STB_TEXTEDIT_STRING* obj)                             { return obj->CurLenW; }
static ImWchar STB_TEXTEDIT_GETCHAR(const STB_TEXTEDIT_STRING* obj, int idx)                      { return obj->Text[idx]; }
static float   STB_TEXTEDIT_GETWIDTH(STB_TEXTEDIT_STRING* obj, int line_start_idx, int char_idx)  { ImWchar c = obj->Text[line_start_idx+char_idx]; if (c == '\n') return STB_TEXTEDIT_GETWIDTH_NEWLINE; return GImGui->Font->GetCharAdvance(c) * (GImGui->FontSize / GImGui->Font->FontSize); }
static int     STB_TEXTEDIT_KEYTOTEXT(int key)                                                    { return key >= 0x10000 ? 0 : key; }
static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* r, STB_TEXTEDIT_STRING* obj, int line_start_idx)
{
    const ImWchar* text = obj->Text.Data;
    const ImWchar* text_remaining = NULL;
    const ImVec2 size = InputTextCalcTextSizeW(text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
    r->x0 = 0.0f;
    r->x1 = size.x;
    r->baseline_y_delta = size.y;
    r->ymin = 0.0f;
    r->ymax = size.y;
    r->num_chars = (int)(text_remaining - (text + line_start_idx));
}

static bool is_separator(unsigned int c)                                                          { return c==',' || c==';' || c=='(' || c==')' || c=='{' || c=='}' || c=='[' || c==']' || c=='|'; }
#define STB_TEXTEDIT_IS_SPACE(CH)                                                                 ( ImCharIsSpace((unsigned int)CH) || is_separator((unsigned int)CH) )
static void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING* obj, int pos, int n)
{
    ImWchar* dst = obj->Text.Data + pos;

    // We maintain our buffer length in both UTF-8 and wchar formats
    obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
    obj->CurLenW -= n;

    // Offset remaining text
    const ImWchar* src = obj->Text.Data + pos + n; 
    while (ImWchar c = *src++)
        *dst++ = c; 
    *dst = '\0';
}

static bool STB_TEXTEDIT_INSERTCHARS(STB_TEXTEDIT_STRING* obj, int pos, const ImWchar* new_text, int new_text_len)
{
    const int text_len = obj->CurLenW;
    if (new_text_len + text_len + 1 > obj->Text.Size)
        return false;

    const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_text_len);
    if (new_text_len_utf8 + obj->CurLenA + 1 > obj->BufSizeA)
        return false;

    ImWchar* text = obj->Text.Data;
    if (pos != text_len)
        memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(ImWchar));
    memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(ImWchar));

    obj->CurLenW += new_text_len;
    obj->CurLenA += new_text_len_utf8;
    obj->Text[obj->CurLenW] = '\0';

    return true;
}

// We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define STB_TEXTEDIT_K_LEFT         0x10000 // keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x10001 // keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x10002 // keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x10003 // keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x10004 // keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x10005 // keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x10006 // keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x10007 // keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x10008 // keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x10009 // keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x1000A // keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x1000B // keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x1000C // keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x1000D // keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_SHIFT        0x20000

#ifdef IMGUI_STB_NAMESPACE
namespace IMGUI_STB_NAMESPACE
{
#endif
#define STB_TEXTEDIT_IMPLEMENTATION
#include "stb_textedit.h"
#ifdef IMGUI_STB_NAMESPACE
}
#endif

void ImGuiTextEditState::OnKeyPressed(int key)
{ 
    stb_textedit_key(this, &StbState, key); 
    CursorFollow = true;
    CursorAnimReset(); 
}

// Public API to manipulate UTF-8 text
// We expose UTF-8 to the user (unlike the STB_TEXTEDIT_* functions which are manipulating wchar)
void ImGuiTextEditCallbackData::DeleteChars(int pos, int bytes_count)
{
    char* dst = Buf + pos;
    const char* src = Buf + pos + bytes_count;
    while (char c = *src++)
        *dst++ = c;
    *dst = '\0';

    BufDirty = true;
    if (CursorPos + bytes_count >= pos)
        CursorPos -= bytes_count;
    else if (CursorPos >= pos)
        CursorPos = pos;
    SelectionStart = SelectionEnd = CursorPos;
}

void ImGuiTextEditCallbackData::InsertChars(int pos, const char* new_text, const char* new_text_end)
{
    const int text_len = (int)strlen(Buf);
    if (!new_text_end)
        new_text_end = new_text + strlen(new_text);
    const int new_text_len = (int)(new_text_end - new_text);

    if (new_text_len + text_len + 1 >= BufSize)
        return;

    if (text_len != pos)
        memmove(Buf + pos + new_text_len, Buf + pos, (size_t)(text_len - pos));
    memcpy(Buf + pos, new_text, (size_t)new_text_len * sizeof(char));
    Buf[text_len + new_text_len] = '\0';

    BufDirty = true;
    if (CursorPos >= pos)
        CursorPos += new_text_len;
    SelectionStart = SelectionEnd = CursorPos;
}

// Return false to discard a character.
static bool InputTextFilterCharacter(unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
{
    unsigned int c = *p_char;

    if (c < 128 && c != ' ' && !isprint((int)(c & 0xFF)))
    {
        bool pass = false;
        pass |= (c == '\n' && (flags & ImGuiInputTextFlags_Multiline));
        pass |= (c == '\t' && (flags & ImGuiInputTextFlags_AllowTabInput));
        if (!pass)
            return false;
    }

    if (c >= 0xE000 && c <= 0xF8FF) // Filter private Unicode range. I don't imagine anybody would want to input them. GLFW on OSX seems to send private characters for special keys like arrow keys.
        return false;

    if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank))
    {
        if (flags & ImGuiInputTextFlags_CharsDecimal)
            if (!(c >= '0' && c <= '9') && (c != '.') && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
                return false;

        if (flags & ImGuiInputTextFlags_CharsHexadecimal)
            if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
                return false;

        if (flags & ImGuiInputTextFlags_CharsUppercase)
            if (c >= 'a' && c <= 'z')
                *p_char = (c += (unsigned int)('A'-'a'));

        if (flags & ImGuiInputTextFlags_CharsNoBlank)
            if (ImCharIsSpace(c))
                return false;
    }

    if (flags & ImGuiInputTextFlags_CallbackCharFilter)
    {
        ImGuiTextEditCallbackData callback_data;
        memset(&callback_data, 0, sizeof(ImGuiTextEditCallbackData));
        callback_data.EventFlag = ImGuiInputTextFlags_CallbackCharFilter; 
        callback_data.EventChar = (ImWchar)c;
        callback_data.Flags = flags;
        callback_data.UserData = user_data;
        if (callback(&callback_data) != 0)
            return false;
        *p_char = callback_data.EventChar;
        if (!callback_data.EventChar)
            return false;
    }

    return true;
}

// Edit a string of text
static bool InputTextEx(const char* label, char* buf, int buf_size, const ImVec2& size_arg, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline))); // Can't use both together (they both use up/down keys)
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

    ImGuiState& g = *GImGui;
    const ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const ImGuiID id = window->GetID(label);
    const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;

    ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 size = CalcItemSize(size_arg, ImGui::CalcItemWidth(), is_multiline ? ImGui::GetTextLineHeight() * 8.0f : label_size.y); // Arbitrary default of 8 lines high for multi-line
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size + style.FramePadding*2.0f);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? (style.ItemInnerSpacing.x + label_size.x) : 0.0f, 0.0f));

    ImGuiWindow* draw_window = window;
    if (is_multiline)
    {
        ImGui::BeginGroup();
        if (!ImGui::BeginChildFrame(id, frame_bb.GetSize()))
        {
            ImGui::EndChildFrame();
            ImGui::EndGroup();
            return false;
        }
        draw_window = GetCurrentWindow();
        draw_window->DC.CursorPos += style.FramePadding;
        size.x -= draw_window->ScrollbarWidth();
    }
    else
    {
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, &id))
            return false;
    }

    // NB: we are only allowed to access 'edit_state' if we are the active widget.
    ImGuiTextEditState& edit_state = g.InputTextState;

    const bool is_ctrl_down = io.KeyCtrl;
    const bool is_shift_down = io.KeyShift;
    const bool is_alt_down = io.KeyAlt;
    const bool focus_requested = window->FocusItemRegister(g.ActiveId == id, (flags & (ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_AllowTabInput)) == 0);    // Using completion callback disable keyboard tabbing
    const bool focus_requested_by_code = focus_requested && (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent);
    const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

    const bool hovered = IsHovered(frame_bb, id);
    if (hovered)
    {
        g.HoveredId = id;
        g.MouseCursor = ImGuiMouseCursor_TextInput;
    }
    const bool user_clicked = hovered && io.MouseClicked[0];
    const bool user_scrolled = is_multiline && g.ActiveId == 0 && edit_state.Id == id && g.ActiveIdPreviousFrame == draw_window->GetID("#SCROLLY");

    bool select_all = (g.ActiveId != id) && (flags & ImGuiInputTextFlags_AutoSelectAll) != 0;
    if (focus_requested || user_clicked || user_scrolled)
    {
        if (g.ActiveId != id)
        {
            // Start edition
            // Take a copy of the initial buffer value (both in original UTF-8 format and converted to wchar)
            // From the moment we focused we are ignoring the content of 'buf'
            edit_state.Text.resize(buf_size);        // wchar count <= utf-8 count
            edit_state.InitialText.resize(buf_size); // utf-8
            ImFormatString(edit_state.InitialText.Data, edit_state.InitialText.Size, "%s", buf);
            const char* buf_end = NULL;
            edit_state.CurLenW = ImTextStrFromUtf8(edit_state.Text.Data, edit_state.Text.Size, buf, NULL, &buf_end);
            edit_state.CurLenA = (int)(buf_end - buf); // We can't get the result from ImFormatString() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.
            edit_state.InputCursorScreenPos = ImVec2(-1.f, -1.f);
            edit_state.CursorAnimReset();

            if (edit_state.Id != id)
            {
                edit_state.Id = id;
                edit_state.ScrollX = 0.f;
                stb_textedit_initialize_state(&edit_state.StbState, !is_multiline); 
                if (!is_multiline && focus_requested_by_code)
                    select_all = true;
            }
            else
            {
                // Recycle existing cursor/selection/undo stack but clamp position
                // Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
                edit_state.StbState.cursor = ImMin(edit_state.StbState.cursor, edit_state.CurLenW);
                edit_state.StbState.select_start = ImMin(edit_state.StbState.select_start, edit_state.CurLenW);
                edit_state.StbState.select_end = ImMin(edit_state.StbState.select_end, edit_state.CurLenW);
            }
            if (flags & ImGuiInputTextFlags_AlwaysInsertMode)
                edit_state.StbState.insert_mode = true;
            if (!is_multiline && (focus_requested_by_tab || (user_clicked && is_ctrl_down)))
                select_all = true;
        }
        SetActiveId(id, window);
        FocusWindow(window);
    }
    else if (io.MouseClicked[0])
    {
        // Release focus when we click outside
        if (g.ActiveId == id)
            SetActiveId(0);
    }

    bool value_changed = false;
    bool cancel_edit = false;
    bool enter_pressed = false;

    if (g.ActiveId == id)
    {
        edit_state.BufSizeA = buf_size;

        // Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
        // Down the line we should have a cleaner concept of focused vs active in the library.
        g.ActiveIdIsFocusedOnly = !io.MouseDown[0];

        // Edit in progress
        const float mouse_x = (g.IO.MousePos.x - frame_bb.Min.x - style.FramePadding.x) + edit_state.ScrollX;
        const float mouse_y = (is_multiline ? (g.IO.MousePos.y - draw_window->DC.CursorPos.y - style.FramePadding.y) : (g.FontSize*0.5f));

        if (select_all || (hovered && io.MouseDoubleClicked[0]))
        {
            edit_state.SelectAll();
            edit_state.SelectedAllMouseLock = true;
        }
        else if (io.MouseClicked[0] && !edit_state.SelectedAllMouseLock)
        {
            stb_textedit_click(&edit_state, &edit_state.StbState, mouse_x, mouse_y);
            edit_state.CursorAnimReset();
        }
        else if (io.MouseDown[0] && !edit_state.SelectedAllMouseLock)
        {
            stb_textedit_drag(&edit_state, &edit_state.StbState, mouse_x, mouse_y);
            edit_state.CursorAnimReset();
        }
        if (edit_state.SelectedAllMouseLock && !io.MouseDown[0])
            edit_state.SelectedAllMouseLock = false;

        if (g.IO.InputCharacters[0])
        {
            // Process text input (before we check for Return because using some IME will effectively send a Return?)
            for (int n = 0; n < IM_ARRAYSIZE(g.IO.InputCharacters) && g.IO.InputCharacters[n]; n++)
            {
                if (unsigned int c = (unsigned int)g.IO.InputCharacters[n])
                {
                    // Insert character if they pass filtering
                    if (!InputTextFilterCharacter(&c, flags, callback, user_data))
                        continue;
                    edit_state.OnKeyPressed((int)c);
                }
            }

            // Consume characters
            memset(g.IO.InputCharacters, 0, sizeof(g.IO.InputCharacters));
        }

        const int k_mask = (is_shift_down ? STB_TEXTEDIT_K_SHIFT : 0);
        const bool is_ctrl_only = is_ctrl_down && !is_alt_down && !is_shift_down;
        if (IsKeyPressedMap(ImGuiKey_LeftArrow))                        { edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_WORDLEFT | k_mask : STB_TEXTEDIT_K_LEFT | k_mask); }
        else if (IsKeyPressedMap(ImGuiKey_RightArrow))                  { edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_WORDRIGHT | k_mask  : STB_TEXTEDIT_K_RIGHT | k_mask); }
        else if (is_multiline && IsKeyPressedMap(ImGuiKey_UpArrow))     { if (is_ctrl_down) SetWindowScrollY(draw_window, draw_window->ScrollY - g.FontSize); else edit_state.OnKeyPressed(STB_TEXTEDIT_K_UP | k_mask); }
        else if (is_multiline && IsKeyPressedMap(ImGuiKey_DownArrow))   { if (is_ctrl_down) SetWindowScrollY(draw_window, draw_window->ScrollY + g.FontSize); else edit_state.OnKeyPressed(STB_TEXTEDIT_K_DOWN| k_mask); }
        else if (IsKeyPressedMap(ImGuiKey_Home))                        { edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); }
        else if (IsKeyPressedMap(ImGuiKey_End))                         { edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); }
        else if (IsKeyPressedMap(ImGuiKey_Delete))                      { edit_state.OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask); }
        else if (IsKeyPressedMap(ImGuiKey_Backspace))                   { edit_state.OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask); }
        else if (IsKeyPressedMap(ImGuiKey_Enter))
        {
            bool ctrl_enter_for_new_line = (flags & ImGuiInputTextFlags_CtrlEnterForNewLine) != 0;
            if (!is_multiline || (ctrl_enter_for_new_line && !is_ctrl_down) || (!ctrl_enter_for_new_line && is_ctrl_down))
            { 
                SetActiveId(0); 
                enter_pressed = true; 
            }
            else // New line
            {
                unsigned int c = '\n';
                if (InputTextFilterCharacter(&c, flags, callback, user_data))
                    edit_state.OnKeyPressed((int)c);
            }
        }
        else if ((flags & ImGuiInputTextFlags_AllowTabInput) && IsKeyPressedMap(ImGuiKey_Tab) && !is_ctrl_down && !is_shift_down && !is_alt_down)
        {
            unsigned int c = '\t';
            if (InputTextFilterCharacter(&c, flags, callback, user_data))
                edit_state.OnKeyPressed((int)c);
        }
        else if (IsKeyPressedMap(ImGuiKey_Escape))              { SetActiveId(0); cancel_edit = true; }
        else if (is_ctrl_only && IsKeyPressedMap(ImGuiKey_Z))   { edit_state.OnKeyPressed(STB_TEXTEDIT_K_UNDO); edit_state.ClearSelection(); }
        else if (is_ctrl_only && IsKeyPressedMap(ImGuiKey_Y))   { edit_state.OnKeyPressed(STB_TEXTEDIT_K_REDO); edit_state.ClearSelection(); }
        else if (is_ctrl_only && IsKeyPressedMap(ImGuiKey_A))   { edit_state.SelectAll(); edit_state.CursorFollow = true; }
        else if (is_ctrl_only && (IsKeyPressedMap(ImGuiKey_X) || IsKeyPressedMap(ImGuiKey_C)) && (!is_multiline || edit_state.HasSelection()))
        {
            // Cut, Copy
            const bool cut = IsKeyPressedMap(ImGuiKey_X);
            if (cut && !edit_state.HasSelection())
                edit_state.SelectAll();

            if (g.IO.SetClipboardTextFn)
            {
                const int ib = edit_state.HasSelection() ? ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end) : 0;
                const int ie = edit_state.HasSelection() ? ImMax(edit_state.StbState.select_start, edit_state.StbState.select_end) : edit_state.CurLenW;
                edit_state.TempTextBuffer.resize((ie-ib) * 4 + 1);
                ImTextStrToUtf8(edit_state.TempTextBuffer.Data, edit_state.TempTextBuffer.Size, edit_state.Text.Data+ib, edit_state.Text.Data+ie);
                g.IO.SetClipboardTextFn(edit_state.TempTextBuffer.Data);
            }

            if (cut)
            {
                edit_state.CursorFollow = true;
                stb_textedit_cut(&edit_state, &edit_state.StbState);
            }
        }
        else if (is_ctrl_only && IsKeyPressedMap(ImGuiKey_V))
        {
            // Paste
            if (g.IO.GetClipboardTextFn)
            {
                if (const char* clipboard = g.IO.GetClipboardTextFn())
                {
                    // Remove new-line from pasted buffer
                    const int clipboard_len = (int)strlen(clipboard);
                    ImWchar* clipboard_filtered = (ImWchar*)ImGui::MemAlloc((clipboard_len+1) * sizeof(ImWchar));
                    int clipboard_filtered_len = 0;
                    for (const char* s = clipboard; *s; )
                    {
                        unsigned int c;
                        s += ImTextCharFromUtf8(&c, s, NULL);
                        if (c == 0)
                            break;
                        if (c >= 0x10000 || !InputTextFilterCharacter(&c, flags, callback, user_data))
                            continue;
                        clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
                    }
                    clipboard_filtered[clipboard_filtered_len] = 0;
                    if (clipboard_filtered_len > 0) // If everything was filtered, ignore the pasting operation
                    {
                        stb_textedit_paste(&edit_state, &edit_state.StbState, clipboard_filtered, clipboard_filtered_len);
                        edit_state.CursorFollow = true;
                    }
                    ImGui::MemFree(clipboard_filtered);
                }
            }
        }

        if (cancel_edit)
        {
            // Restore initial value
            ImFormatString(buf, buf_size, "%s", edit_state.InitialText.Data);
            value_changed = true;
        }
        else
        {
            // Apply new value immediately - copy modified buffer back
            // Note that as soon as we can focus into the input box, the in-widget value gets priority over any underlying modification of the input buffer
            // FIXME: We actually always render 'buf' in RenderTextScrolledClipped
            // FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks
            edit_state.TempTextBuffer.resize(edit_state.Text.Size * 4);
            ImTextStrToUtf8(edit_state.TempTextBuffer.Data, edit_state.TempTextBuffer.Size, edit_state.Text.Data, NULL);

            // User callback
            if ((flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackAlways)) != 0)
            {
                IM_ASSERT(callback != NULL);

                // The reason we specify the usage semantic (Completion/History) is that Completion needs to disable keyboard TABBING at the moment.
                ImGuiInputTextFlags event_flag = 0;
                ImGuiKey event_key = ImGuiKey_COUNT;
                if ((flags & ImGuiInputTextFlags_CallbackCompletion) != 0 && IsKeyPressedMap(ImGuiKey_Tab))
                {
                    event_flag = ImGuiInputTextFlags_CallbackCompletion;
                    event_key = ImGuiKey_Tab;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_UpArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_UpArrow;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_DownArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_DownArrow;
                }

                if (event_key != ImGuiKey_COUNT || (flags & ImGuiInputTextFlags_CallbackAlways) != 0)
                {
                    ImGuiTextEditCallbackData callback_data;
                    callback_data.EventFlag = event_flag; 
                    callback_data.EventKey = event_key;
                    callback_data.Buf = edit_state.TempTextBuffer.Data;
                    callback_data.BufSize = edit_state.BufSizeA;
                    callback_data.BufDirty = false;
                    callback_data.Flags = flags;
                    callback_data.UserData = user_data;

                    // We have to convert from position from wchar to UTF-8 positions
                    ImWchar* text = edit_state.Text.Data;
                    const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.cursor);
                    const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.select_start);
                    const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.select_end);

                    // Call user code
                    callback(&callback_data);

                    // Read back what user may have modified
                    IM_ASSERT(callback_data.Buf == edit_state.TempTextBuffer.Data);  // Invalid to modify those fields
                    IM_ASSERT(callback_data.BufSize == edit_state.BufSizeA);
                    IM_ASSERT(callback_data.Flags == flags);
                    if (callback_data.CursorPos != utf8_cursor_pos)            edit_state.StbState.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos);
                    if (callback_data.SelectionStart != utf8_selection_start)  edit_state.StbState.select_start = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart);
                    if (callback_data.SelectionEnd != utf8_selection_end)      edit_state.StbState.select_end = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd);
                    if (callback_data.BufDirty)
                    {
                        edit_state.CurLenW = ImTextStrFromUtf8(text, edit_state.Text.Size, edit_state.TempTextBuffer.Data, NULL);
                        edit_state.CurLenA = (int)strlen(edit_state.TempTextBuffer.Data);
                        edit_state.CursorAnimReset();
                    }
                }
            }

            if (strcmp(edit_state.TempTextBuffer.Data, buf) != 0)
            {
                ImFormatString(buf, buf_size, "%s", edit_state.TempTextBuffer.Data);
                value_changed = true;
            }
        }
    }

    if (!is_multiline)
        RenderFrame(frame_bb.Min, frame_bb.Max, window->Color(ImGuiCol_FrameBg), true, style.FrameRounding);

    ImVec2 render_pos = is_multiline ? draw_window->DC.CursorPos : frame_bb.Min + style.FramePadding;

    ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x + style.FramePadding.x*2.0f, frame_bb.Min.y + size.y + style.FramePadding.y*2.0f);
    ImVec2 text_size(0.f, 0.f);
    if (g.ActiveId == id || (edit_state.Id == id && is_multiline && g.ActiveId == draw_window->GetID("#SCROLLY")))
    {
        edit_state.CursorAnim += g.IO.DeltaTime;

        // We need to:
        // - Display the text (this can be more easily clipped)
        // - Handle scrolling, highlight selection, display cursor (those all requires some form of 1d->2d cursor position calculation)
        // - Measure text height (for scrollbar)
        // We are attempting to do most of that in one main pass to minimize the computation cost (non-negligible for large amount of text) + 2nd pass for selection rendering (we could merge them by an extra refactoring effort)
        const ImWchar* text_begin = edit_state.Text.Data;
        const ImWchar* text_end = text_begin + edit_state.CurLenW;
        ImVec2 cursor_offset, select_start_offset;

        {
            // Count lines + find lines numbers of cursor and select_start
            int matches_remaining = 0;
            int matches_line_no[2] = { -1, -999 };
            const ImWchar* matches_ptr[2] = { NULL, NULL };
            matches_ptr[0] = text_begin + edit_state.StbState.cursor; matches_remaining++;
            if (edit_state.StbState.select_start != edit_state.StbState.select_end)
            {
                matches_ptr[1] = text_begin + ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end);
                matches_line_no[1] = -1;
                matches_remaining++;
            }
            matches_remaining += is_multiline ? 1 : 0;     // So that we never exit the loop until all lines are counted.

            int line_count = 0;
            for (const ImWchar* s = text_begin; s < text_end+1; s++)
                if ((*s) == '\n' || s == text_end)
                {
                    line_count++;
                    if (matches_line_no[0] == -1 && s >= matches_ptr[0]) { matches_line_no[0] = line_count; if (--matches_remaining <= 0) break; }
                    if (matches_line_no[1] == -1 && s >= matches_ptr[1]) { matches_line_no[1] = line_count; if (--matches_remaining <= 0) break; }
                }

            // Calculate 2d position
            IM_ASSERT(matches_line_no[0] != -1);
            cursor_offset.x = InputTextCalcTextSizeW(ImStrbolW(matches_ptr[0], text_begin), matches_ptr[0]).x;
            cursor_offset.y = matches_line_no[0] * g.FontSize;
            if (matches_line_no[1] >= 0)
            {
                select_start_offset.x = InputTextCalcTextSizeW(ImStrbolW(matches_ptr[1], text_begin), matches_ptr[1]).x;
                select_start_offset.y = matches_line_no[1] * g.FontSize;
            }

            // Calculate text height
            if (is_multiline)
                text_size = ImVec2(size.x, line_count * g.FontSize);
        }

        // Scroll
        if (edit_state.CursorFollow)
        {
            // Horizontal scroll in chunks of quarter width
            if (!(flags & ImGuiInputTextFlags_NoHorizontalScroll))
            {
                const float scroll_increment_x = size.x * 0.25f;
                if (cursor_offset.x < edit_state.ScrollX)
                    edit_state.ScrollX = ImMax(0.0f, cursor_offset.x - scroll_increment_x);    
                else if (cursor_offset.x - size.x >= edit_state.ScrollX)
                    edit_state.ScrollX = cursor_offset.x - size.x + scroll_increment_x;
            }
            else
            {
                edit_state.ScrollX = 0.0f;
            }

            // Vertical scroll
            if (is_multiline)
            {
                float scroll_y = draw_window->ScrollY;
                if (cursor_offset.y - g.FontSize < scroll_y)
                    scroll_y = ImMax(0.0f, cursor_offset.y - g.FontSize);
                else if (cursor_offset.y - size.y >= scroll_y)
                    scroll_y = cursor_offset.y - size.y;
                draw_window->DC.CursorPos.y += (draw_window->ScrollY - scroll_y);   // To avoid a frame of lag
                draw_window->ScrollY = scroll_y;
                render_pos.y = draw_window->DC.CursorPos.y;
            }
        }
        edit_state.CursorFollow = false;
        ImVec2 render_scroll = ImVec2(edit_state.ScrollX, 0.0f);

        // Draw selection
        if (edit_state.StbState.select_start != edit_state.StbState.select_end)
        {
            const ImWchar* text_selected_begin = text_begin + ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end);
            const ImWchar* text_selected_end = text_begin + ImMax(edit_state.StbState.select_start, edit_state.StbState.select_end);

            float bg_offy_up = is_multiline ? 0.0f : -1.0f;    // FIXME: those offsets should be part of the style? they don't play so well with multi-line selection.
            float bg_offy_dn = is_multiline ? 0.0f : 2.0f;
            ImU32 bg_color = draw_window->Color(ImGuiCol_TextSelectedBg);
            ImVec2 rect_pos = render_pos + select_start_offset - render_scroll;
            for (const ImWchar* p = text_selected_begin; p < text_selected_end; )
            {
                if (rect_pos.y > clip_rect.w + g.FontSize)
                    break;
                if (rect_pos.y < clip_rect.y)
                {
                    while (p < text_selected_end)
                        if (*p++ == '\n')
                            break;
                }
                else
                {
                    ImVec2 rect_size = InputTextCalcTextSizeW(p, text_selected_end, &p, NULL, true);
                    if (rect_size.x <= 0.0f) rect_size.x = (float)(int)(g.Font->GetCharAdvance((unsigned short)' ') * 0.50f); // So we can see selected empty lines
                    ImRect rect(rect_pos + ImVec2(0.0f, bg_offy_up - g.FontSize), rect_pos +ImVec2(rect_size.x, bg_offy_dn));
                    rect.Clip(clip_rect);
                    if (rect.Overlaps(clip_rect))
                        draw_window->DrawList->AddRectFilled(rect.Min, rect.Max, bg_color);
                }
                rect_pos.x = render_pos.x - render_scroll.x;
                rect_pos.y += g.FontSize;
            }
        }

        draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos - render_scroll, draw_window->Color(ImGuiCol_Text), buf, buf+edit_state.CurLenA, 0.0f, is_multiline ? NULL : &clip_rect);

        // Draw blinking cursor
        ImVec2 cursor_screen_pos = render_pos + cursor_offset - render_scroll;
        if (g.InputTextState.CursorIsVisible())
            draw_window->DrawList->AddLine(cursor_screen_pos + ImVec2(0.0f,-g.FontSize+0.5f), cursor_screen_pos + ImVec2(0.0f,-1.5f), window->Color(ImGuiCol_Text));
        
        // Notify OS of text input position for advanced IME
        if (io.ImeSetInputScreenPosFn && ImLengthSqr(edit_state.InputCursorScreenPos - cursor_screen_pos) > 0.0001f)
            io.ImeSetInputScreenPosFn((int)cursor_screen_pos.x - 1, (int)(cursor_screen_pos.y - g.FontSize));   // -1 x offset so that Windows IME can cover our cursor. Bit of an extra nicety.

        edit_state.InputCursorScreenPos = cursor_screen_pos;
    }
    else
    {
        // Render text only
        const char* buf_end = NULL;
        if (is_multiline)
            text_size = ImVec2(size.x, InputTextCalcTextLenAndLineCount(buf, &buf_end) * g.FontSize); // We don't need width
        draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos, draw_window->Color(ImGuiCol_Text), buf, buf_end, 0.0f, is_multiline ? NULL : &clip_rect);
    }

    if (is_multiline)
    {
        ImGui::Dummy(text_size + ImVec2(0.0f, g.FontSize)); // Always add room to scroll an extra line
        ImGui::EndChildFrame();
        ImGui::EndGroup();
    }

    // Log as text
    if (GImGui->LogEnabled)
        LogText(render_pos, buf, NULL);

    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    if ((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0)
        return enter_pressed;
    else
        return value_changed;
}

bool ImGui::InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()
    bool ret = InputTextEx(label, buf, (int)buf_size, ImVec2(0,0), flags, callback, user_data);
    return ret;
}

bool ImGui::InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
{
    bool ret = InputTextEx(label, buf, (int)buf_size, size, flags | ImGuiInputTextFlags_Multiline, callback, user_data);
    return ret;
}

bool ImGui::InputFloat(const char* label, float *v, float step, float step_fast, int decimal_precision, ImGuiInputTextFlags extra_flags)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const float w = ImGui::CalcItemWidth();
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y) + style.FramePadding*2.0f);

    ImGui::BeginGroup();
    ImGui::PushID(label);
    const ImVec2 button_sz = ImVec2(g.FontSize, g.FontSize) + style.FramePadding * 2;
    if (step > 0.0f)
        ImGui::PushItemWidth(ImMax(1.0f, w - (button_sz.x + style.ItemInnerSpacing.x)*2));

    char buf[64];
    if (decimal_precision < 0)
        ImFormatString(buf, IM_ARRAYSIZE(buf), "%f", *v);       // Ideally we'd have a minimum decimal precision of 1 to visually denote that it is a float, while hiding non-significant digits?
    else
        ImFormatString(buf, IM_ARRAYSIZE(buf), "%.*f", decimal_precision, *v);
    bool value_changed = false;
    const ImGuiInputTextFlags flags = extra_flags | (ImGuiInputTextFlags_CharsDecimal|ImGuiInputTextFlags_AutoSelectAll);
    if (ImGui::InputText("", buf, IM_ARRAYSIZE(buf), flags))
    {
        InputTextApplyArithmeticOp(buf, v);
        value_changed = true;
    }

    // Step buttons
    if (step > 0.0f)
    {
        ImGui::PopItemWidth();
        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("-", button_sz, ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups))
        {
            *v -= g.IO.KeyCtrl && step_fast > 0.0f ? step_fast : step;
            value_changed = true;
        }
        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("+", button_sz, ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups))
        {
            *v += g.IO.KeyCtrl && step_fast > 0.0f ? step_fast : step;
            value_changed = true;
        }
    }
    ImGui::PopID();

    if (label_size.x > 0)
    {
        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        RenderText(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), label);
        ItemSize(label_size, style.FramePadding.y);
    }
    ImGui::EndGroup();

    return value_changed;
}

bool ImGui::InputInt(const char* label, int *v, int step, int step_fast, ImGuiInputTextFlags extra_flags)
{
    float f = (float)*v;
    const bool value_changed = ImGui::InputFloat(label, &f, (float)step, (float)step_fast, 0, extra_flags);
    if (value_changed)
        *v = (int)f;
    return value_changed;
}

static bool InputFloatN(const char* label, float* v, int components, int decimal_precision, ImGuiInputTextFlags extra_flags)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    bool value_changed = false;
    ImGui::BeginGroup();
    ImGui::PushID(label);
    PushMultiItemsWidths(components);
    for (int i = 0; i < components; i++)
    {
        ImGui::PushID(i);
        value_changed |= ImGui::InputFloat("##v", &v[i], 0, 0, decimal_precision, extra_flags);
        ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();

    window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.CurrentLineTextBaseOffset, g.Style.FramePadding.y);
    ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
    ImGui::EndGroup();

    return value_changed;
}

bool ImGui::InputFloat2(const char* label, float v[2], int decimal_precision, ImGuiInputTextFlags extra_flags)
{
    return InputFloatN(label, v, 2, decimal_precision, extra_flags);
}

bool ImGui::InputFloat3(const char* label, float v[3], int decimal_precision, ImGuiInputTextFlags extra_flags)
{
    return InputFloatN(label, v, 3, decimal_precision, extra_flags);
}

bool ImGui::InputFloat4(const char* label, float v[4], int decimal_precision, ImGuiInputTextFlags extra_flags)
{
    return InputFloatN(label, v, 4, decimal_precision, extra_flags);
}

static bool InputIntN(const char* label, int* v, int components, ImGuiInputTextFlags extra_flags)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    bool value_changed = false;
    ImGui::BeginGroup();
    ImGui::PushID(label);
    PushMultiItemsWidths(components);
    for (int i = 0; i < components; i++)
    {
        ImGui::PushID(i);
        value_changed |= ImGui::InputInt("##v", &v[i], 0, 0, extra_flags);
        ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();

    window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.CurrentLineTextBaseOffset, g.Style.FramePadding.y);
    ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
    ImGui::EndGroup();

    return value_changed;
}

bool ImGui::InputInt2(const char* label, int v[2], ImGuiInputTextFlags extra_flags)
{
    return InputIntN(label, v, 2, extra_flags);
}

bool ImGui::InputInt3(const char* label, int v[3], ImGuiInputTextFlags extra_flags)
{
    return InputIntN(label, v, 3, extra_flags);
}

bool ImGui::InputInt4(const char* label, int v[4], ImGuiInputTextFlags extra_flags)
{
    return InputIntN(label, v, 4, extra_flags);
}

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char** items = (const char**)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}

static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
{
    // FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
    const char* items_separated_by_zeros = (const char*)data;
    int items_count = 0;
    const char* p = items_separated_by_zeros;
    while (*p)
    {
        if (idx == items_count)
            break;
        p += strlen(p) + 1;
        items_count++;
    }
    if (!*p)
        return false;
    if (out_text)
        *out_text = p;
    return true;
}

// Combo box helper allowing to pass an array of strings.
bool ImGui::Combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items)
{
    const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
    return value_changed;
}

// Combo box helper allowing to pass all items in a single string.
bool ImGui::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
{
    int items_count = 0;
    const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
    while (*p)
    {
        p += strlen(p) + 1;
        items_count++;
    }
    bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
    return value_changed;
}

// Combo box function.
bool ImGui::Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = ImGui::CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y) + style.FramePadding*2.0f);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, &id))
        return false;

    const float arrow_size = (g.FontSize + style.FramePadding.x * 2.0f);
    const bool hovered = IsHovered(frame_bb, id);

    const ImRect value_bb(frame_bb.Min, frame_bb.Max - ImVec2(arrow_size, 0.0f));
    RenderFrame(frame_bb.Min, frame_bb.Max, window->Color(ImGuiCol_FrameBg), true, style.FrameRounding);
    RenderFrame(ImVec2(frame_bb.Max.x-arrow_size, frame_bb.Min.y), frame_bb.Max, window->Color(hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button), true, style.FrameRounding);	// FIXME-ROUNDING
    RenderCollapseTriangle(ImVec2(frame_bb.Max.x-arrow_size, frame_bb.Min.y) + style.FramePadding, true);

    if (*current_item >= 0 && *current_item < items_count)
    {
        const char* item_text;
        if (items_getter(data, *current_item, &item_text))
            RenderTextClipped(frame_bb.Min + style.FramePadding, value_bb.Max, item_text, NULL, NULL);
    }

    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);
    
    bool menu_toggled = false;
    if (hovered)
    {
        g.HoveredId = id;
        if (g.IO.MouseClicked[0])
        {
            SetActiveId(0);
            if (IsPopupOpen(id))
            {
                ClosePopup(id);
            }
            else
            {
            	FocusWindow(window);
                ImGui::OpenPopup(label);
                menu_toggled = true;
            }
        }
    }
    
    bool value_changed = false;
    if (IsPopupOpen(id))
    {
        // Size default to hold ~7 items
        if (height_in_items < 0)
            height_in_items = 7;

        float popup_height = (label_size.y + style.ItemSpacing.y) * ImMin(items_count, height_in_items) + (style.FramePadding.y * 3);
        ImRect popup_rect(ImVec2(frame_bb.Min.x, frame_bb.Max.y), ImVec2(frame_bb.Max.x, frame_bb.Max.y + popup_height));
        popup_rect.Max.y = ImMin(popup_rect.Max.y, g.IO.DisplaySize.y - style.DisplaySafeAreaPadding.y); // Adhoc height limit for Combo. Ideally should be handled in Begin() along with other popups size, we want to have the possibility of moving the popup above as well.
        ImGui::SetNextWindowPos(popup_rect.Min);
        ImGui::SetNextWindowSize(popup_rect.GetSize());
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);

        const ImGuiWindowFlags flags = ImGuiWindowFlags_ComboBox | ((window->Flags & ImGuiWindowFlags_ShowBorders) ? ImGuiWindowFlags_ShowBorders : 0);
        if (BeginPopupEx(label, flags))
        {
            // Display items
            ImGui::Spacing();
            for (int i = 0; i < items_count; i++)
            {
                ImGui::PushID((void*)(intptr_t)i);
                const bool item_selected = (i == *current_item);
                const char* item_text;
                if (!items_getter(data, i, &item_text))
                    item_text = "*Unknown item*";
                if (ImGui::Selectable(item_text, item_selected))
                {
                    SetActiveId(0);
                    value_changed = true;
                    *current_item = i;
                }
                if (item_selected && menu_toggled)
                    ImGui::SetScrollHere();
                ImGui::PopID();
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();
    }
    return value_changed;
}

// Tip: pass an empty label (e.g. "##dummy") then you can use the space to draw other text or image.
// But you need to make sure the ID is unique, e.g. enclose calls in PushID/PopID.
bool ImGui::Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.ColumnsCount > 1)
        PopClipRect();

    const ImGuiStyle& style = g.Style;
    ImGuiID id = window->GetID(label);
    ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrentLineTextBaseOffset;
    ImRect bb(pos, pos + size);
    ItemSize(bb);

    // Fill horizontal space.
    ImVec2 window_padding = window->WindowPadding;
    float max_x = (flags & ImGuiSelectableFlags_SpanAllColumns) ? ImGui::GetWindowContentRegionMax().x : ImGui::GetContentRegionMax().x;
    float w_draw = ImMax(label_size.x, window->Pos.x + max_x - window_padding.x - window->DC.CursorPos.x);
    ImVec2 size_draw((size_arg.x != 0 && !(flags & ImGuiSelectableFlags_DrawFillAvailWidth)) ? size_arg.x : w_draw, size_arg.y != 0.0f ? size_arg.y : size.y);
    ImRect bb_with_spacing(pos, pos + size_draw);
    if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_DrawFillAvailWidth))
        bb_with_spacing.Max.x += window_padding.x;

    // Selectables are tightly packed together, we extend the box to cover spacing between selectable.
    float spacing_L = (float)(int)(style.ItemSpacing.x * 0.5f);
    float spacing_U = (float)(int)(style.ItemSpacing.y * 0.5f);
    float spacing_R = style.ItemSpacing.x - spacing_L;
    float spacing_D = style.ItemSpacing.y - spacing_U;
    bb_with_spacing.Min.x -= spacing_L;
    bb_with_spacing.Min.y -= spacing_U;
    bb_with_spacing.Max.x += spacing_R;
    bb_with_spacing.Max.y += spacing_D;
    if (!ItemAdd(bb_with_spacing, &id))
    {
        if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.ColumnsCount > 1)
            PushColumnClipRect();
        return false;
    }

    ImGuiButtonFlags button_flags = 0;
    if (flags & ImGuiSelectableFlags_Menu) button_flags |= ImGuiButtonFlags_PressedOnClick;
    if (flags & ImGuiSelectableFlags_MenuItem) button_flags |= ImGuiButtonFlags_PressedOnClick|ImGuiButtonFlags_PressedOnRelease;
    if (flags & ImGuiSelectableFlags_Disabled) button_flags |= ImGuiButtonFlags_Disabled;
    bool hovered, held;
    bool pressed = ButtonBehavior(bb_with_spacing, id, &hovered, &held, true, button_flags);
    if (flags & ImGuiSelectableFlags_Disabled)
        selected = false;

    // Render
    if (hovered || selected)
    {
        const ImU32 col = window->Color((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
        RenderFrame(bb_with_spacing.Min, bb_with_spacing.Max, col, false, 0.0f);
    }

    if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.ColumnsCount > 1)
    {
        PushColumnClipRect();
        bb_with_spacing.Max.x -= (ImGui::GetContentRegionMax().x - max_x);
    }

    if (flags & ImGuiSelectableFlags_Disabled) ImGui::PushStyleColor(ImGuiCol_Text, g.Style.Colors[ImGuiCol_TextDisabled]);
    RenderTextClipped(bb.Min, bb_with_spacing.Max, label, NULL, &label_size);
    if (flags & ImGuiSelectableFlags_Disabled) ImGui::PopStyleColor();

    // Automatically close popups
    if (pressed && !(flags & ImGuiSelectableFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
        ImGui::CloseCurrentPopup();
    return pressed;
}

bool ImGui::Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    if (ImGui::Selectable(label, *p_selected, flags, size_arg))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

// Helper to calculate the size of a listbox and display a label on the right.
// Tip: To have a list filling the entire window width, PushItemWidth(-1) and pass an empty label "##empty"
bool ImGui::ListBoxHeader(const char* label, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = ImGui::GetStyle();
    const ImGuiID id = ImGui::GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    // Size default to hold ~7 items. Fractional number of items helps seeing that we can scroll down/up without looking at scrollbar.
    ImVec2 size = CalcItemSize(size_arg, ImGui::CalcItemWidth() + style.FramePadding.x * 2.0f, ImGui::GetTextLineHeightWithSpacing() * 7.4f + style.ItemSpacing.y);
    ImVec2 frame_size = ImVec2(size.x, ImMax(size.y, label_size.y));
    ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    ImRect bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    window->DC.LastItemRect = bb;

    ImGui::BeginGroup();
    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    ImGui::BeginChildFrame(id, frame_bb.GetSize());
    return true;
}

bool ImGui::ListBoxHeader(const char* label, int items_count, int height_in_items)
{
    // Size default to hold ~7 items. Fractional number of items helps seeing that we can scroll down/up without looking at scrollbar.
    // However we don't add +0.40f if items_count <= height_in_items. It is slightly dodgy, because it means a dynamic list of items will make the widget resize occasionally when it crosses that size.
    // I am expecting that someone will come and complain about this behavior in a remote future, then we can advise on a better solution.
    if (height_in_items < 0)
        height_in_items = ImMin(items_count, 7);
    float height_in_items_f = height_in_items < items_count ? (height_in_items + 0.40f) : (height_in_items + 0.00f);

    // We include ItemSpacing.y so that a list sized for the exact number of items doesn't make a scrollbar appears. We could also enforce that by passing a flag to BeginChild().
    ImVec2 size;
    size.x = 0.0f;
    size.y = ImGui::GetTextLineHeightWithSpacing() * height_in_items_f + ImGui::GetStyle().ItemSpacing.y;
    return ImGui::ListBoxHeader(label, size);
}

void ImGui::ListBoxFooter()
{
    ImGuiWindow* parent_window = GetParentWindow();
    const ImRect bb = parent_window->DC.LastItemRect;
    const ImGuiStyle& style = ImGui::GetStyle();
    
    ImGui::EndChildFrame();

    // Redeclare item size so that it includes the label (we have stored the full size in LastItemRect)
    // We call SameLine() to restore DC.CurrentLine* data
    ImGui::SameLine();
    parent_window->DC.CursorPos = bb.Min;
    ItemSize(bb, style.FramePadding.y);
    ImGui::EndGroup();
}

bool ImGui::ListBox(const char* label, int* current_item, const char** items, int items_count, int height_items)
{
    const bool value_changed = ListBox(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_items);
    return value_changed;
}

bool ImGui::ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
{
    if (!ImGui::ListBoxHeader(label, items_count, height_in_items))
        return false;

    // Assume all items have even height (= 1 line of text). If you need items of different or variable sizes you can create a custom version of ListBox() in your code without using the clipper.
    bool value_changed = false;
    ImGuiListClipper clipper(items_count, ImGui::GetTextLineHeightWithSpacing());
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
    {
        const bool item_selected = (i == *current_item);
        const char* item_text;
        if (!items_getter(data, i, &item_text))
            item_text = "*Unknown item*";

        ImGui::PushID(i);
        if (ImGui::Selectable(item_text, item_selected))
        {
            *current_item = i;
            value_changed = true;
        }
        ImGui::PopID();
    }
    clipper.End();
    ImGui::ListBoxFooter();
    return value_changed;
}

bool ImGui::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 label_size = CalcTextSize(label, NULL, true);
    ImVec2 shortcut_size = shortcut ? CalcTextSize(shortcut, NULL) : ImVec2(0.0f, 0.0f);
    float w = window->MenuColumns.DeclColumns(label_size.x, shortcut_size.x, (float)(int)(g.FontSize * 1.20f)); // Feedback for next frame
    float extra_w = ImMax(0.0f, window->Pos.x + ImGui::GetContentRegionMax().x - pos.x - w);

    bool pressed = ImGui::Selectable(label, false, ImGuiSelectableFlags_MenuItem | ImGuiSelectableFlags_DrawFillAvailWidth | (!enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(w, 0.0f));
    if (shortcut_size.x > 0.0f)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, g.Style.Colors[ImGuiCol_TextDisabled]);
        RenderText(pos + ImVec2(window->MenuColumns.Pos[1] + extra_w, 0.0f), shortcut, NULL, false);
        ImGui::PopStyleColor();
    }

    if (selected)
        RenderCheckMark(pos + ImVec2(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.20f, 0.0f), window->Color(ImGuiCol_Text));

    return pressed;
}

bool ImGui::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled)
{
    if (ImGui::MenuItem(label, shortcut, p_selected ? *p_selected : false, enabled))
    {
        if (p_selected)
            *p_selected = !*p_selected;
        return true;
    }
    return false;
}

bool ImGui::BeginMainMenuBar()
{
    ImGuiState& g = *GImGui;
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(g.IO.DisplaySize.x, g.FontBaseSize + g.Style.FramePadding.y * 2.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0,0));
    if (!ImGui::Begin("##MainMenuBar", NULL, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_MenuBar)
        || !ImGui::BeginMenuBar())
    {
        ImGui::End();
        ImGui::PopStyleVar(2);
        return false;
    }
    g.CurrentWindow->DC.MenuBarOffsetX += g.Style.DisplaySafeAreaPadding.x;
    return true;
}

void ImGui::EndMainMenuBar()
{
    ImGui::EndMenuBar();
    ImGui::End();
    ImGui::PopStyleVar(2);
}

bool ImGui::BeginMenuBar()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    if (!(window->Flags & ImGuiWindowFlags_MenuBar))
        return false;

    IM_ASSERT(!window->DC.MenuBarAppending);
    ImGui::BeginGroup(); // Save position
    ImGui::PushID("##menubar");
    ImRect rect = window->MenuBarRect();
    PushClipRect(ImVec4(rect.Min.x+0.5f, rect.Min.y-0.5f, rect.Max.x+0.5f, rect.Max.y-1.5f), false);
    window->DC.CursorPos = ImVec2(rect.Min.x + window->DC.MenuBarOffsetX, rect.Min.y);// + g.Style.FramePadding.y);
    window->DC.LayoutType = ImGuiLayoutType_Horizontal;
    window->DC.MenuBarAppending = true;
    ImGui::AlignFirstTextHeightToWidgets();
    return true;
}

void ImGui::EndMenuBar()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    IM_ASSERT(window->Flags & ImGuiWindowFlags_MenuBar);
    IM_ASSERT(window->DC.MenuBarAppending);
    PopClipRect();
    ImGui::PopID();
    window->DC.MenuBarOffsetX = window->DC.CursorPos.x - window->MenuBarRect().Min.x;
    window->DC.GroupStack.back().AdvanceCursor = false;
    ImGui::EndGroup();
    window->DC.LayoutType = ImGuiLayoutType_Vertical;
    window->DC.MenuBarAppending = false;
}

bool ImGui::BeginMenu(const char* label, bool enabled)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    
    ImGuiState& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 label_size = CalcTextSize(label, NULL, true);
    ImGuiWindow* backed_focused_window = g.FocusedWindow;

    bool pressed;
    bool opened = IsPopupOpen(id);
    bool menuset_opened = !(window->Flags & ImGuiWindowFlags_Popup) && (g.OpenedPopupStack.Size > g.CurrentPopupStack.Size && g.OpenedPopupStack[g.CurrentPopupStack.Size].ParentMenuSet == window->GetID("##menus"));
    if (menuset_opened)
        g.FocusedWindow = window;

    ImVec2 popup_pos, pos = window->DC.CursorPos;
    if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
    {
        popup_pos = ImVec2(pos.x - window->WindowPadding.x, pos.y - style.FramePadding.y + window->MenuBarHeight());
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * 0.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, style.ItemSpacing * 2.0f);
        float w = label_size.x;
        pressed = ImGui::Selectable(label, opened, ImGuiSelectableFlags_Menu | ImGuiSelectableFlags_DontClosePopups | (!enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(w, 0.0f));
        ImGui::PopStyleVar();
        ImGui::SameLine();
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * 0.5f);
    }
    else
    {
        popup_pos = ImVec2(pos.x, pos.y - style.WindowPadding.y);
        float w = window->MenuColumns.DeclColumns(label_size.x, 0.0f, (float)(int)(g.FontSize * 1.20f)); // Feedback to next frame
        float extra_w = ImMax(0.0f, window->Pos.x + ImGui::GetContentRegionMax().x - pos.x - w);
        pressed = ImGui::Selectable(label, opened, ImGuiSelectableFlags_Menu | ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_DrawFillAvailWidth | (!enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(w, 0.0f));
        if (!enabled) ImGui::PushStyleColor(ImGuiCol_Text, g.Style.Colors[ImGuiCol_TextDisabled]);
        RenderCollapseTriangle(pos + ImVec2(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.20f, 0.0f), false);
        if (!enabled) ImGui::PopStyleColor();
    }

    bool hovered = enabled && IsHovered(window->DC.LastItemRect, id);
    if (menuset_opened)
        g.FocusedWindow = backed_focused_window;

    bool want_open = false, want_close = false;
    if (window->Flags & (ImGuiWindowFlags_Popup|ImGuiWindowFlags_ChildMenu))
    {
        // Implement http://bjk5.com/post/44698559168/breaking-down-amazons-mega-dropdown to avoid using timers so menus feel more reactive.
        bool moving_within_opened_triangle = false;
        if (g.HoveredWindow == window && g.OpenedPopupStack.Size > g.CurrentPopupStack.Size && g.OpenedPopupStack[g.CurrentPopupStack.Size].ParentWindow == window)
        {
            if (ImGuiWindow* next_window = g.OpenedPopupStack[g.CurrentPopupStack.Size].Window)
            {
                ImRect next_window_rect = next_window->Rect();
                ImVec2 ta = g.IO.MousePos - g.IO.MouseDelta;
                ImVec2 tb = (window->Pos.x < next_window->Pos.x) ? next_window_rect.GetTL() : next_window_rect.GetTR();
                ImVec2 tc = (window->Pos.x < next_window->Pos.x) ? next_window_rect.GetBL() : next_window_rect.GetBR();
                float extra = ImClamp(fabsf(ta.x - tb.x) * 0.30f, 5.0f, 30.0f); // add a bit of extra slack.
                ta.x += (window->Pos.x < next_window->Pos.x) ? -0.5f : +0.5f;   // to avoid numerical issues
                tb.y = ta.y + ImMax((tb.y - extra) - ta.y, -100.0f);            // triangle is maximum 200 high to limit the slope and the bias toward large sub-menus
                tc.y = ta.y + ImMin((tc.y + extra) - ta.y, +100.0f);
                moving_within_opened_triangle = ImIsPointInTriangle(g.IO.MousePos, ta, tb, tc);
                //window->DrawList->PushClipRectFullScreen(); window->DrawList->AddTriangleFilled(ta, tb, tc, moving_within_opened_triangle ? 0x80008000 : 0x80000080); window->DrawList->PopClipRect(); // Debug
            }
        }
            
        want_close = (opened && !hovered && g.HoveredWindow == window && g.HoveredIdPreviousFrame != 0 && g.HoveredIdPreviousFrame != id && !moving_within_opened_triangle);
        want_open = (!opened && hovered && !moving_within_opened_triangle) || (!opened && hovered && pressed);
    }
    else if (opened && pressed && menuset_opened) // menu-bar: click open menu to close
    {
        want_close = true;
        want_open = opened = false;
    }
    else if (pressed || (hovered && menuset_opened && !opened)) // menu-bar: first click to open, then hover to open others
        want_open = true;

    if (want_close && IsPopupOpen(id))
        ClosePopupToLevel(GImGui->CurrentPopupStack.Size);

    if (!opened && want_open && g.OpenedPopupStack.Size > g.CurrentPopupStack.Size)
    {
        // Don't recycle same menu level in the same frame, first close the other menu and yield for a frame.
        ImGui::OpenPopup(label);
        return false;
    }

    opened |= want_open;
    if (want_open)
        ImGui::OpenPopup(label);

    if (opened)
    {
        ImGui::SetNextWindowPos(popup_pos, ImGuiSetCond_Always);
        ImGuiWindowFlags flags = ImGuiWindowFlags_ShowBorders | ((window->Flags & (ImGuiWindowFlags_Popup|ImGuiWindowFlags_ChildMenu)) ? ImGuiWindowFlags_ChildMenu|ImGuiWindowFlags_ChildWindow : ImGuiWindowFlags_ChildMenu);
        opened = BeginPopupEx(label, flags); // opened can be 'false' when the popup is completely clipped (e.g. zero size display)
    }

    return opened;
}

void ImGui::EndMenu()
{
    ImGui::EndPopup();
}

// A little colored square. Return true when clicked.
bool ImGui::ColorButton(const ImVec4& col, bool small_height, bool outline_border)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID("#colorbutton");
    const float square_size = g.FontSize;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(square_size + style.FramePadding.y*2, square_size + (small_height ? 0 : style.FramePadding.y*2)));
    ItemSize(bb, small_height ? 0.0f : style.FramePadding.y);
    if (!ItemAdd(bb, &id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, true);
    RenderFrame(bb.Min, bb.Max, window->Color(col), outline_border, style.FrameRounding);

    if (hovered)
    {
        int ix = (int)(col.x * 255.0f + 0.5f);
        int iy = (int)(col.y * 255.0f + 0.5f);
        int iz = (int)(col.z * 255.0f + 0.5f);
        int iw = (int)(col.w * 255.0f + 0.5f);
        ImGui::SetTooltip("Color:\n(%.2f,%.2f,%.2f,%.2f)\n#%02X%02X%02X%02X", col.x, col.y, col.z, col.w, ix, iy, iz, iw);
    }

    return pressed;
}

bool ImGui::ColorEdit3(const char* label, float col[3])
{
    float col4[4];
    col4[0] = col[0];
    col4[1] = col[1];
    col4[2] = col[2];
    col4[3] = 1.0f;
    const bool value_changed = ImGui::ColorEdit4(label, col4, false);
    col[0] = col4[0];
    col[1] = col4[1];
    col[2] = col4[2];
    return value_changed;
}

// Edit colors components (each component in 0.0f..1.0f range
// Use CTRL-Click to input value and TAB to go to next item.
bool ImGui::ColorEdit4(const char* label, float col[4], bool alpha)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w_full = ImGui::CalcItemWidth();
    const float square_sz = (g.FontSize + style.FramePadding.y * 2.0f);

    ImGuiColorEditMode edit_mode = window->DC.ColorEditMode;
    if (edit_mode == ImGuiColorEditMode_UserSelect || edit_mode == ImGuiColorEditMode_UserSelectShowButton)
        edit_mode = g.ColorEditModeStorage.GetInt(id, 0) % 3;

    float f[4] = { col[0], col[1], col[2], col[3] };

    if (edit_mode == ImGuiColorEditMode_HSV)
        ImGui::ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

    int i[4] = { (int)(f[0] * 255.0f + 0.5f), (int)(f[1] * 255.0f + 0.5f), (int)(f[2] * 255.0f + 0.5f), (int)(f[3] * 255.0f + 0.5f) };

    int components = alpha ? 4 : 3;
    bool value_changed = false;

    ImGui::BeginGroup();
    ImGui::PushID(label);

    const bool hsv = (edit_mode == 1);
    switch (edit_mode)
    {
    case ImGuiColorEditMode_RGB:
    case ImGuiColorEditMode_HSV:
        {
            // RGB/HSV 0..255 Sliders
            const float w_items_all = w_full - (square_sz + style.ItemInnerSpacing.x);
            const float w_item_one  = ImMax(1.0f, (float)(int)((w_items_all - (style.FramePadding.x*2.0f + style.ItemInnerSpacing.x) * (components-1)) / (float)components));
            const float w_item_last = ImMax(1.0f, (float)(int)(w_items_all - (w_item_one + style.FramePadding.x*2.0f + style.ItemInnerSpacing.x) * (components-1)));

            const bool hide_prefix = (w_item_one <= CalcTextSize("M:999").x);
            const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
            const char* fmt_table[3][4] = 
            {
                {   "%3.0f",   "%3.0f",   "%3.0f",   "%3.0f" }, 
                { "R:%3.0f", "G:%3.0f", "B:%3.0f", "A:%3.0f" },
                { "H:%3.0f", "S:%3.0f", "V:%3.0f", "A:%3.0f" } 
            };
            const char** fmt = hide_prefix ? fmt_table[0] : hsv ? fmt_table[2] : fmt_table[1]; 

            ImGui::PushItemWidth(w_item_one);
            for (int n = 0; n < components; n++)
            {
                if (n > 0)
                    ImGui::SameLine(0, style.ItemInnerSpacing.x);
                if (n + 1 == components)
                    ImGui::PushItemWidth(w_item_last);
                value_changed |= ImGui::DragInt(ids[n], &i[n], 1.0f, 0, 255, fmt[n]);
            }
            ImGui::PopItemWidth();
            ImGui::PopItemWidth();
        }
        break;
    case ImGuiColorEditMode_HEX:
        {
            // RGB Hexadecimal Input
            const float w_slider_all = w_full - square_sz;
            char buf[64];
            if (alpha)
                ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", i[0], i[1], i[2], i[3]);
            else
                ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", i[0], i[1], i[2]);
            ImGui::PushItemWidth(w_slider_all - style.ItemInnerSpacing.x);
            value_changed |= ImGui::InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
            ImGui::PopItemWidth();
            char* p = buf;
            while (*p == '#' || ImCharIsSpace(*p)) 
                p++;

            // Treat at unsigned (%X is unsigned)
            i[0] = i[1] = i[2] = i[3] = 0;
            if (alpha)
                sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]);
            else
                sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
        }
        break;
    }

    ImGui::SameLine(0, style.ItemInnerSpacing.x);

    const ImVec4 col_display(col[0], col[1], col[2], 1.0f);
    if (ImGui::ColorButton(col_display))
        g.ColorEditModeStorage.SetInt(id, (edit_mode + 1) % 3); // Don't set local copy of 'edit_mode' right away!

    if (window->DC.ColorEditMode == ImGuiColorEditMode_UserSelectShowButton)
    {
        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        const char* button_titles[3] = { "RGB", "HSV", "HEX" };
        if (ButtonEx(button_titles[edit_mode], ImVec2(0,0), ImGuiButtonFlags_DontClosePopups))
            g.ColorEditModeStorage.SetInt(id, (edit_mode + 1) % 3); // Don't set local copy of 'edit_mode' right away!
        ImGui::SameLine();
    }
    else
    {
        ImGui::SameLine(0, style.ItemInnerSpacing.x);
    }

    ImGui::TextUnformatted(label, FindTextDisplayEnd(label));

    // Convert back
    for (int n = 0; n < 4; n++)
        f[n] = i[n] / 255.0f;
    if (edit_mode == 1)
        ImGui::ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);

    if (value_changed)
    {
        col[0] = f[0];
        col[1] = f[1];
        col[2] = f[2];
        if (alpha)
            col[3] = f[3];
    }

    ImGui::PopID();
    ImGui::EndGroup();

    return value_changed;
}

void ImGui::ColorEditMode(ImGuiColorEditMode mode)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ColorEditMode = mode;
}

// Horizontal separating line.
void ImGui::Separator()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    if (window->DC.ColumnsCount > 1)
        PopClipRect();

    float x1 = window->Pos.x;
    float x2 = window->Pos.x + window->Size.x;
    if (!window->DC.GroupStack.empty())
        x1 += window->DC.ColumnsStartX;

    const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y));
    ItemSize(ImVec2(0.0f, bb.GetSize().y)); // NB: we don't provide our width so that it doesn't get feed back into AutoFit
    if (!ItemAdd(bb, NULL))
    {
        if (window->DC.ColumnsCount > 1)
            PushColumnClipRect();
        return;
    }

    window->DrawList->AddLine(bb.Min, bb.Max, window->Color(ImGuiCol_Border));

    ImGuiState& g = *GImGui;
    if (g.LogEnabled)
        ImGui::LogText(IM_NEWLINE "--------------------------------");

    if (window->DC.ColumnsCount > 1)
    {
        PushColumnClipRect();
        window->DC.ColumnsCellMinY = window->DC.CursorPos.y;
    }
}

void ImGui::Spacing()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    ItemSize(ImVec2(0,0));
}

void ImGui::Dummy(const ImVec2& size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    ItemSize(size);
}

// Advance cursor given item size for layout.
static void ItemSize(ImVec2 size, float text_offset_y)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    // Always align ourselves on pixel boundaries
    const float line_height = ImMax(window->DC.CurrentLineHeight, size.y);
    const float text_base_offset = ImMax(window->DC.CurrentLineTextBaseOffset, text_offset_y);
    window->DC.CursorPosPrevLine = ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y);
    window->DC.CursorPos = ImVec2((float)(int)(window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX), (float)(int)(window->DC.CursorPos.y + line_height + g.Style.ItemSpacing.y));
    window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPosPrevLine.x);
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y);

    //window->DrawList->AddCircle(window->DC.CursorMaxPos, 3.0f, 0xFF0000FF, 4); // Debug

    window->DC.PrevLineHeight = line_height;
    window->DC.PrevLineTextBaseOffset = text_base_offset;
    window->DC.CurrentLineHeight = window->DC.CurrentLineTextBaseOffset = 0.0f;
}

static inline void ItemSize(const ImRect& bb, float text_offset_y)
{ 
    ItemSize(bb.GetSize(), text_offset_y); 
}

static bool IsClippedEx(const ImRect& bb, const ImGuiID* id, bool clip_even_when_logged)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    if (!bb.Overlaps(ImRect(window->ClipRect)))
    {
        if (!id || *id != GImGui->ActiveId)
            if (clip_even_when_logged || !g.LogEnabled)
                return true;
    }
    return false;
}

bool ImGui::IsRectVisible(const ImVec2& size)
{
    ImGuiWindow* window = GetCurrentWindow();
    ImRect r(window->ClipRect);
    return r.Overlaps(ImRect(window->DC.CursorPos, window->DC.CursorPos + size));
}

// Declare item bounding box for clipping and interaction.
// Note that the size can be different than the one provided to ItemSize(). Typically, widgets that spread over available surface
// declares their minimum size requirement to ItemSize() and then use a larger region for drawing/interaction, which is passed to ItemAdd().
static bool ItemAdd(const ImRect& bb, const ImGuiID* id)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.LastItemID = id ? *id : 0;
    window->DC.LastItemRect = bb;
    if (IsClippedEx(bb, id, false))
    {
        window->DC.LastItemHoveredAndUsable = window->DC.LastItemHoveredRect = false;
        return false;
    }

    // This is a sensible default, but widgets are free to override it after calling ItemAdd()
    ImGuiState& g = *GImGui;
    if (IsMouseHoveringRect(bb))
    {
        // Matching the behavior of IsHovered() but ignore if ActiveId==window->MoveID (we clicked on the window background)
        // So that clicking on items with no active id such as Text() still returns true with IsItemHovered()
        window->DC.LastItemHoveredRect = true;
        window->DC.LastItemHoveredAndUsable = false;
        if (g.HoveredRootWindow == window->RootWindow)
            if (g.ActiveId == 0 || (id && g.ActiveId == *id) || g.ActiveIdIsFocusedOnly || (g.ActiveId == window->MoveID))
                if (IsWindowContentHoverable(window))
                    window->DC.LastItemHoveredAndUsable = true;
    }
    else
    {
        window->DC.LastItemHoveredAndUsable = window->DC.LastItemHoveredRect = false;
    }

    return true;
}

void ImGui::BeginGroup()
{
    ImGuiWindow* window = GetCurrentWindow();

    window->DC.GroupStack.resize(window->DC.GroupStack.Size + 1);
    ImGuiGroupData& group_data = window->DC.GroupStack.back();
    group_data.BackupCursorPos = window->DC.CursorPos;
    group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
    group_data.BackupColumnsStartX = window->DC.ColumnsStartX;
    group_data.BackupCurrentLineHeight = window->DC.CurrentLineHeight;
    group_data.BackupCurrentLineTextBaseOffset = window->DC.CurrentLineTextBaseOffset;
    group_data.BackupLogLinePosY = window->DC.LogLinePosY;
    group_data.AdvanceCursor = true;

    window->DC.ColumnsStartX = window->DC.CursorPos.x - window->Pos.x;
    window->DC.CursorMaxPos = window->DC.CursorPos;
    window->DC.CurrentLineHeight = 0.0f;
    window->DC.LogLinePosY = window->DC.CursorPos.y - 9999.0f;
}

void ImGui::EndGroup()
{
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiStyle& style = ImGui::GetStyle();

    IM_ASSERT(!window->DC.GroupStack.empty());

    ImGuiGroupData& group_data = window->DC.GroupStack.back();

    ImRect group_bb(group_data.BackupCursorPos, window->DC.CursorMaxPos);
    group_bb.Max.y -= style.ItemSpacing.y;      // Cancel out last vertical spacing because we are adding one ourselves.
    group_bb.Max = ImMax(group_bb.Min, group_bb.Max);

    window->DC.CursorPos = group_data.BackupCursorPos;
    window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
    window->DC.CurrentLineHeight = group_data.BackupCurrentLineHeight;
    window->DC.CurrentLineTextBaseOffset = group_data.BackupCurrentLineTextBaseOffset;
    window->DC.ColumnsStartX = group_data.BackupColumnsStartX;
    window->DC.LogLinePosY = window->DC.CursorPos.y - 9999.0f;

    if (group_data.AdvanceCursor)
    {
        window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrentLineTextBaseOffset);      // FIXME: Incorrect, we should grab the base offset from the *first line* of the group but it is hard to obtain now.
        ItemSize(group_bb.GetSize(), group_data.BackupCurrentLineTextBaseOffset);
        ItemAdd(group_bb, NULL);
    }

    window->DC.GroupStack.pop_back();

    //window->DrawList->AddRect(group_bb.Min, group_bb.Max, 0xFFFF00FF);   // Debug
}

// Gets back to previous line and continue with horizontal layout
//      pos_x == 0   : follow on previous item
//      pos_x != 0   : align to specified column
//      spacing_w < 0   : use default spacing if column_x==0, no spacing if column_x!=0
//      spacing_w >= 0  : enforce spacing
void ImGui::SameLine(float pos_x, float spacing_w)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    
    float x, y;
    if (pos_x != 0.0f)
    {
        if (spacing_w < 0.0f) spacing_w = 0.0f;
        x = window->Pos.x + pos_x + spacing_w;
        y = window->DC.CursorPosPrevLine.y;
    }
    else
    {
        if (spacing_w < 0.0f) spacing_w = g.Style.ItemSpacing.x;
        x = window->DC.CursorPosPrevLine.x + spacing_w;
        y = window->DC.CursorPosPrevLine.y;
    }
    window->DC.CurrentLineHeight = window->DC.PrevLineHeight;
    window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
    window->DC.CursorPos = ImVec2(x, y);
}

void ImGui::NextColumn()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    if (window->DC.ColumnsCount > 1)
    {
        ImGui::PopItemWidth();
        PopClipRect();

        window->DC.ColumnsCellMaxY = ImMax(window->DC.ColumnsCellMaxY, window->DC.CursorPos.y);
        if (++window->DC.ColumnsCurrent < window->DC.ColumnsCount)
        {
            window->DC.ColumnsOffsetX = ImGui::GetColumnOffset(window->DC.ColumnsCurrent) - window->DC.ColumnsStartX + g.Style.ItemSpacing.x;
            window->DrawList->ChannelsSetCurrent(window->DC.ColumnsCurrent);
        }
        else
        {
            window->DC.ColumnsCurrent = 0;
            window->DC.ColumnsOffsetX = 0.0f;
            window->DC.ColumnsCellMinY = window->DC.ColumnsCellMaxY;
            window->DrawList->ChannelsSetCurrent(0);
        }
        window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX);
        window->DC.CursorPos.y = window->DC.ColumnsCellMinY;
        window->DC.CurrentLineHeight = 0.0f;
        window->DC.CurrentLineTextBaseOffset = 0.0f;

        PushColumnClipRect();
        ImGui::PushItemWidth(ImGui::GetColumnWidth() * 0.65f);  // FIXME
    }
}

int ImGui::GetColumnIndex()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.ColumnsCurrent;
}

int ImGui::GetColumnsCount()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.ColumnsCount;
}

static float GetDraggedColumnOffset(int column_index)
{
    // Active (dragged) column always follow mouse. The reason we need this is that dragging a column to the right edge of an auto-resizing
    // window creates a feedback loop because we store normalized positions/ So while dragging we enforce absolute positioning
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(column_index > 0); // We cannot drag column 0. If you get this assert you may have a conflict between the ID of your columns and another widgets.
    IM_ASSERT(g.ActiveId == window->DC.ColumnsSetID + ImGuiID(column_index));

    float x = g.IO.MousePos.x + g.ActiveClickDeltaToCenter.x - window->Pos.x;
    x = ImClamp(x, ImGui::GetColumnOffset(column_index-1)+g.Style.ColumnsMinSpacing, ImGui::GetColumnOffset(column_index+1)-g.Style.ColumnsMinSpacing);

    return (float)(int)x;
}

float ImGui::GetColumnOffset(int column_index)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (column_index < 0)
        column_index = window->DC.ColumnsCurrent;

    if (g.ActiveId)
    {
        const ImGuiID column_id = window->DC.ColumnsSetID + ImGuiID(column_index);
        if (g.ActiveId == column_id)
            return GetDraggedColumnOffset(column_index);
    }

    // Read from cache
    IM_ASSERT(column_index < window->DC.ColumnsOffsetsT.Size);
    const float t = window->DC.ColumnsOffsetsT[column_index];

    const float min_x = window->DC.ColumnsStartX;
    const float max_x = window->Size.x - ((window->Flags & ImGuiWindowFlags_NoScrollbar) ? 0 : g.Style.ScrollbarWidth);// - window->WindowPadding().x;
    const float x = min_x + t * (max_x - min_x);
    return (float)(int)x;
}

void ImGui::SetColumnOffset(int column_index, float offset)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (column_index < 0)
        column_index = window->DC.ColumnsCurrent;

    IM_ASSERT(column_index < window->DC.ColumnsOffsetsT.Size);
    const ImGuiID column_id = window->DC.ColumnsSetID + ImGuiID(column_index);

    const float min_x = window->DC.ColumnsStartX;
    const float max_x = window->Size.x - ((window->Flags & ImGuiWindowFlags_NoScrollbar) ? 0 : g.Style.ScrollbarWidth);// - window->WindowPadding().x;
    const float t = (offset - min_x) / (max_x - min_x);
    window->DC.StateStorage->SetFloat(column_id, t);
    window->DC.ColumnsOffsetsT[column_index] = t;
}

float ImGui::GetColumnWidth(int column_index)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (column_index < 0)
        column_index = window->DC.ColumnsCurrent;

    const float w = GetColumnOffset(column_index+1) - GetColumnOffset(column_index);
    return w;
}

static void PushColumnClipRect(int column_index)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (column_index < 0)
        column_index = window->DC.ColumnsCurrent;

    const float x1 = window->Pos.x + ImGui::GetColumnOffset(column_index) - 1;
    const float x2 = window->Pos.x + ImGui::GetColumnOffset(column_index+1) - 1;
    PushClipRect(ImVec4(x1,-FLT_MAX,x2,+FLT_MAX));
}

void ImGui::Columns(int columns_count, const char* id, bool border)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    if (window->DC.ColumnsCount != 1)
    {
        if (window->DC.ColumnsCurrent != 0)
            ItemSize(ImVec2(0,0));   // Advance to column 0
        ImGui::PopItemWidth();
        PopClipRect();
        window->DrawList->ChannelsMerge(window->DC.ColumnsCount);

        window->DC.ColumnsCellMaxY = ImMax(window->DC.ColumnsCellMaxY, window->DC.CursorPos.y);
        window->DC.CursorPos.y = window->DC.ColumnsCellMaxY;
    }

    // Draw columns borders and handle resize at the time of "closing" a columns set
    if (window->DC.ColumnsCount != columns_count && window->DC.ColumnsCount != 1 && window->DC.ColumnsShowBorders && !window->SkipItems)
    {
        const float y1 = window->DC.ColumnsStartPos.y;
        const float y2 = window->DC.CursorPos.y;
        for (int i = 1; i < window->DC.ColumnsCount; i++)
        {
            float x = window->Pos.x + GetColumnOffset(i);
            const ImGuiID column_id = window->DC.ColumnsSetID + ImGuiID(i);
            const ImRect column_rect(ImVec2(x-4,y1),ImVec2(x+4,y2));
            if (IsClippedEx(column_rect, &column_id, false))
                continue;

            bool hovered, held;
            ButtonBehavior(column_rect, column_id, &hovered, &held, true);
            if (hovered || held)
                g.MouseCursor = ImGuiMouseCursor_ResizeEW;

            // Draw before resize so our items positioning are in sync with the line being drawn
            const ImU32 col = window->Color(held ? ImGuiCol_ColumnActive : hovered ? ImGuiCol_ColumnHovered : ImGuiCol_Column);
            const float xi = (float)(int)x;
            window->DrawList->AddLine(ImVec2(xi, y1+1.0f), ImVec2(xi, y2), col);

            if (held)
            {
                if (g.ActiveIdIsJustActivated)
                    g.ActiveClickDeltaToCenter.x = x - g.IO.MousePos.x;

                x = GetDraggedColumnOffset(i);
                SetColumnOffset(i, x);
            }
        }
    }

    // Set state for first column
    ImGui::PushID(0x11223344); // Differentiate column ID with an arbitrary/random prefix for cases where users name their columns set the same as another non-scope widget
    window->DC.ColumnsSetID = window->GetID(id ? id : "");
    ImGui::PopID();
    window->DC.ColumnsCurrent = 0;
    window->DC.ColumnsCount = columns_count;
    window->DC.ColumnsShowBorders = border;
    window->DC.ColumnsStartPos = window->DC.CursorPos;
    window->DC.ColumnsCellMinY = window->DC.ColumnsCellMaxY = window->DC.CursorPos.y;
    window->DC.ColumnsOffsetX = 0.0f;
    window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX);

    if (window->DC.ColumnsCount != 1)
    {
        // Cache column offsets
        window->DC.ColumnsOffsetsT.resize(columns_count + 1);
        for (int column_index = 0; column_index < columns_count + 1; column_index++)
        {
            const ImGuiID column_id = window->DC.ColumnsSetID + ImGuiID(column_index);
            RegisterAliveId(column_id);
            const float default_t = column_index / (float)window->DC.ColumnsCount;
            const float t = window->DC.StateStorage->GetFloat(column_id, default_t);      // Cheaply store our floating point value inside the integer (could store an union into the map?)
            window->DC.ColumnsOffsetsT[column_index] = t;
        }
        window->DrawList->ChannelsSplit(window->DC.ColumnsCount);
        PushColumnClipRect();
        ImGui::PushItemWidth(ImGui::GetColumnWidth() * 0.65f);
    }
    else
    {
        window->DC.ColumnsOffsetsT.resize(2);
        window->DC.ColumnsOffsetsT[0] = 0.0f;
        window->DC.ColumnsOffsetsT[1] = 1.0f;
    }
}

void ImGui::Indent()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ColumnsStartX += g.Style.IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX;
}

void ImGui::Unindent()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ColumnsStartX -= g.Style.IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX;
}

void ImGui::TreePush(const char* str_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    ImGui::Indent();
    window->DC.TreeDepth++;
    PushID(str_id ? str_id : "#TreePush");
}

void ImGui::TreePush(const void* ptr_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    ImGui::Indent();
    window->DC.TreeDepth++;
    PushID(ptr_id ? ptr_id : (const void*)"#TreePush");
}

void ImGui::TreePop()
{
    ImGuiWindow* window = GetCurrentWindow();
    ImGui::Unindent();
    window->DC.TreeDepth--;
    PopID();
}

void ImGui::Value(const char* prefix, bool b)
{
    ImGui::Text("%s: %s", prefix, (b ? "true" : "false"));
}

void ImGui::Value(const char* prefix, int v)
{
    ImGui::Text("%s: %d", prefix, v);
}

void ImGui::Value(const char* prefix, unsigned int v)
{
    ImGui::Text("%s: %d", prefix, v);
}

void ImGui::Value(const char* prefix, float v, const char* float_format)
{
    if (float_format)
    {
        char fmt[64];
        ImFormatString(fmt, IM_ARRAYSIZE(fmt), "%%s: %s", float_format);
        ImGui::Text(fmt, prefix, v);
    }
    else
    {
        ImGui::Text("%s: %.3f", prefix, v);
    }
}

void ImGui::Color(const char* prefix, const ImVec4& v)
{
    ImGui::Text("%s: (%.2f,%.2f,%.2f,%.2f)", prefix, v.x, v.y, v.z, v.w);
    ImGui::SameLine();
    ImGui::ColorButton(v, true);
}

void ImGui::Color(const char* prefix, unsigned int v)
{
    ImGui::Text("%s: %08X", prefix, v);
    ImGui::SameLine();

    ImVec4 col;
    col.x = (float)((v >> 0) & 0xFF) / 255.0f;
    col.y = (float)((v >> 8) & 0xFF) / 255.0f;
    col.z = (float)((v >> 16) & 0xFF) / 255.0f;
    col.w = (float)((v >> 24) & 0xFF) / 255.0f;
    ImGui::ColorButton(col, true);
}

//-----------------------------------------------------------------------------
// ImDrawList
//-----------------------------------------------------------------------------

static ImVec4 GNullClipRect(-9999.0f,-9999.0f, +9999.0f, +9999.0f);

void ImDrawList::Clear()
{
    CmdBuffer.resize(0);
    IdxBuffer.resize(0);
    VtxBuffer.resize(0);
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.resize(0);
    _TextureIdStack.resize(0);
    _Path.resize(0);
    _ChannelCurrent = 0;
    // NB: Do not clear channels so our allocations are re-used after the first frame.
}

void ImDrawList::ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _ChannelCurrent = 0;
    for (int i = 0; i < _Channels.Size; i++)
    {
        if (i == 0) memset(&_Channels[0], 0, sizeof(_Channels[0]));  // channel 0 is a copy of CmdBuffer/IdxBuffer, don't destruct again
        _Channels[i].CmdBuffer.clear();
        _Channels[i].IdxBuffer.clear();
    }
    _Channels.clear();
}

void ImDrawList::AddDrawCmd()
{
    ImDrawCmd draw_cmd;
    draw_cmd.ElemCount = 0;
    draw_cmd.ClipRect = _ClipRectStack.Size ? _ClipRectStack.back() : GNullClipRect;
    draw_cmd.TextureId = _TextureIdStack.Size ? _TextureIdStack.back() : NULL;
    draw_cmd.UserCallback = NULL;
    draw_cmd.UserCallbackData = NULL;

    IM_ASSERT(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
    CmdBuffer.push_back(draw_cmd);
}

void ImDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{
    ImDrawCmd* current_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!current_cmd || current_cmd->ElemCount != 0 || current_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        current_cmd = &CmdBuffer.back();
    }
    current_cmd->UserCallback = callback;
    current_cmd->UserCallbackData = callback_data;

    // Force a new command after us (we function this way so that the most common calls AddLine, AddRect, etc. always have a command to add to without doing any check).
    AddDrawCmd();
}

void ImDrawList::ChannelsSplit(int channel_count)
{
    IM_ASSERT(_ChannelCurrent == 0);
    int old_channels_count = _Channels.Size;
    if (old_channels_count < channel_count)
        _Channels.resize(channel_count);
    for (int i = 0; i < channel_count; i++)
        if (i >= old_channels_count)
            new(&_Channels[i]) ImDrawChannel();
        else
            _Channels[i].CmdBuffer.resize(0), _Channels[i].IdxBuffer.resize(0);
}

void ImDrawList::ChannelsMerge(int channel_count)
{
    // Note that we never use or rely on channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    // This is why this function takes 'channel_count' as a parameter of how many channels to merge (the user knows)
    if (channel_count < 2)
        return;

    ChannelsSetCurrent(0);
    if (CmdBuffer.Size && CmdBuffer.back().ElemCount == 0) 
        CmdBuffer.pop_back();

    int new_cmd_buffer_count = 0, new_idx_buffer_count = 0;
    for (int i = 1; i < channel_count; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (ch.CmdBuffer.Size && ch.CmdBuffer.back().ElemCount == 0)
            ch.CmdBuffer.pop_back();
        new_cmd_buffer_count += ch.CmdBuffer.Size;
        new_idx_buffer_count += ch.IdxBuffer.Size;
    }
    CmdBuffer.resize(CmdBuffer.Size + new_cmd_buffer_count);
    IdxBuffer.resize(IdxBuffer.Size + new_idx_buffer_count);

    ImDrawCmd* cmd_write = CmdBuffer.Data + CmdBuffer.Size - new_cmd_buffer_count;
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size - new_idx_buffer_count;
    for (int i = 1; i < channel_count; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (int sz = ch.CmdBuffer.Size) { memcpy(cmd_write, ch.CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if (int sz = ch.IdxBuffer.Size) { memcpy(_IdxWritePtr, ch.IdxBuffer.Data, sz * sizeof(ImDrawIdx)); _IdxWritePtr += sz; }
    }
    AddDrawCmd();
}

void ImDrawList::ChannelsSetCurrent(int idx)
{
    if (_ChannelCurrent == idx) return;
    memcpy(&_Channels.Data[_ChannelCurrent].CmdBuffer, &CmdBuffer, sizeof(CmdBuffer));
    memcpy(&_Channels.Data[_ChannelCurrent].IdxBuffer, &IdxBuffer, sizeof(IdxBuffer));
    _ChannelCurrent = idx;
    memcpy(&CmdBuffer, &_Channels.Data[_ChannelCurrent].CmdBuffer, sizeof(CmdBuffer));
    memcpy(&IdxBuffer, &_Channels.Data[_ChannelCurrent].IdxBuffer, sizeof(IdxBuffer));
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size;
}

void ImDrawList::UpdateClipRect()
{
    ImDrawCmd* current_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!current_cmd || (current_cmd->ElemCount != 0) || current_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
    }
    else
    {
        ImVec4 current_clip_rect = _ClipRectStack.Size ? _ClipRectStack.back() : GNullClipRect;
        if (CmdBuffer.Size >= 2 && ImLengthSqr(CmdBuffer.Data[CmdBuffer.Size-2].ClipRect - current_clip_rect) < 0.00001f)
            CmdBuffer.pop_back();
        else
            current_cmd->ClipRect = current_clip_rect;
    }
}

// Scissoring. The values in clip_rect are x1, y1, x2, y2.
void ImDrawList::PushClipRect(const ImVec4& clip_rect)
{
    _ClipRectStack.push_back(clip_rect);
    UpdateClipRect();
}

void ImDrawList::PushClipRectFullScreen()
{
    PushClipRect(GNullClipRect);

    // This would be more correct but we're not supposed to access ImGuiState from here?
    //ImGuiState& g = *GImGui;
    //PushClipRect(GetVisibleRect());
}

void ImDrawList::PopClipRect()
{
    IM_ASSERT(_ClipRectStack.Size > 0);
    _ClipRectStack.pop_back();
    UpdateClipRect();
}

void ImDrawList::UpdateTextureID()
{
    ImDrawCmd* current_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    const ImTextureID texture_id = _TextureIdStack.Size ? _TextureIdStack.back() : NULL;
    if (!current_cmd || (current_cmd->ElemCount != 0 && current_cmd->TextureId != texture_id) || current_cmd->UserCallback != NULL)
        AddDrawCmd();
    else
        current_cmd->TextureId = texture_id;
}

void ImDrawList::PushTextureID(const ImTextureID& texture_id)
{
    _TextureIdStack.push_back(texture_id);
    UpdateTextureID();
}

void ImDrawList::PopTextureID()
{
    IM_ASSERT(_TextureIdStack.Size > 0);
    _TextureIdStack.pop_back();
    UpdateTextureID();
}

// NB: this can be called with negative count for removing primitives (as long as the result does not underflow)
void ImDrawList::PrimReserve(int idx_count, int vtx_count)
{
    ImDrawCmd& draw_cmd = CmdBuffer.Data[CmdBuffer.Size-1];
    draw_cmd.ElemCount += idx_count;
        
    int vtx_buffer_size = VtxBuffer.Size;
    VtxBuffer.resize(vtx_buffer_size + vtx_count);
    _VtxWritePtr = VtxBuffer.Data + vtx_buffer_size;

    int idx_buffer_size = IdxBuffer.Size;
    IdxBuffer.resize(idx_buffer_size + idx_count);
    _IdxWritePtr = IdxBuffer.Data + idx_buffer_size;
}

void ImDrawList::PrimRect(const ImVec2& a, const ImVec2& c, ImU32 col)
{
    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    const ImVec2 b(c.x, a.y);
    const ImVec2 d(a.x, c.y);
    _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+2); 
    _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx+2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx+3); 
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col; 
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col; 
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col; 
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimRectUV(const ImVec2& a, const ImVec2& c, const ImVec2& uv_a, const ImVec2& uv_c, ImU32 col)
{
	const ImVec2 b(c.x, a.y);
	const ImVec2 d(a.x, c.y);
	const ImVec2 uv_b(uv_c.x, uv_a.y);
	const ImVec2 uv_d(uv_a.x, uv_c.y);
    _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+2); 
    _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx+2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx+3); 
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col; 
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col; 
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col; 
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
void ImDrawList::AddPolyline(const ImVec2* points, const int points_count, ImU32 col, bool closed, float thickness, bool anti_aliased)
{
    if (points_count < 2)
        return;

    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    anti_aliased &= GImGui->Style.AntiAliasedLines;
    //if (ImGui::GetIO().KeyCtrl) anti_aliased = false;

    int count = points_count;
    if (!closed)
        count = points_count-1;

    const bool thick_line = thickness > 1.0f;
    if (anti_aliased)
    {
        // Anti-aliased stroke
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & 0x00ffffff;

        const int idx_count = thick_line ? count*18 : count*12;
        const int vtx_count = thick_line ? points_count*4 : points_count*3;
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(ImVec2));
        ImVec2* temp_points = temp_normals + points_count;

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1+1) == points_count ? 0 : i1+1; 
            ImVec2 diff = points[i2] - points[i1];
            diff *= ImInvLength(diff, 1.0f);
            temp_normals[i1].x = diff.y;
            temp_normals[i1].y = -diff.x;
        }
        if (!closed)
            temp_normals[points_count-1] = temp_normals[points_count-2];

        if (!thick_line)
        {
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
                temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
                temp_points[(points_count-1)*2+0] = points[points_count-1] + temp_normals[points_count-1] * AA_SIZE;
                temp_points[(points_count-1)*2+1] = points[points_count-1] - temp_normals[points_count-1] * AA_SIZE;
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1+1) == points_count ? 0 : i1+1;
                unsigned int idx2 = (i1+1) == points_count ? _VtxCurrentIdx : idx1+3;

                // Average normals
                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dmr2 = dm.x*dm.x + dm.y*dm.y;
                if (dmr2 > 0.000001f)
                {
                    float scale = 1.0f / dmr2;
                    if (scale > 100.0f) scale = 100.0f;
                    dm *= scale;
                }
                dm *= AA_SIZE;
                temp_points[i2*2+0] = points[i2] + dm;
                temp_points[i2*2+1] = points[i2] - dm;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2+0); _IdxWritePtr[1] = (ImDrawIdx)(idx1+0); _IdxWritePtr[2] = (ImDrawIdx)(idx1+2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1+2); _IdxWritePtr[4] = (ImDrawIdx)(idx2+2); _IdxWritePtr[5] = (ImDrawIdx)(idx2+0);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2+1); _IdxWritePtr[7] = (ImDrawIdx)(idx1+1); _IdxWritePtr[8] = (ImDrawIdx)(idx1+0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1+0); _IdxWritePtr[10]= (ImDrawIdx)(idx2+0); _IdxWritePtr[11]= (ImDrawIdx)(idx2+1);
                _IdxWritePtr += 12;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = points[i];          _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
                _VtxWritePtr[1].pos = temp_points[i*2+0]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;
                _VtxWritePtr[2].pos = temp_points[i*2+1]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col_trans;
                _VtxWritePtr += 3;
            }
        }
        else
        {
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
                temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
                temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count-1)*4+0] = points[points_count-1] + temp_normals[points_count-1] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count-1)*4+1] = points[points_count-1] + temp_normals[points_count-1] * (half_inner_thickness);
                temp_points[(points_count-1)*4+2] = points[points_count-1] - temp_normals[points_count-1] * (half_inner_thickness);
                temp_points[(points_count-1)*4+3] = points[points_count-1] - temp_normals[points_count-1] * (half_inner_thickness + AA_SIZE);
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1+1) == points_count ? 0 : i1+1; 
                unsigned int idx2 = (i1+1) == points_count ? _VtxCurrentIdx : idx1+4;

                // Average normals
                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dmr2 = dm.x*dm.x + dm.y*dm.y;
                if (dmr2 > 0.000001f)
                {
                    float scale = 1.0f / dmr2;
                    if (scale > 100.0f) scale = 100.0f;
                    dm *= scale;
                }
                ImVec2 dm_out = dm * (half_inner_thickness + AA_SIZE);
                ImVec2 dm_in = dm * half_inner_thickness;
                temp_points[i2*4+0] = points[i2] + dm_out;
                temp_points[i2*4+1] = points[i2] + dm_in;
                temp_points[i2*4+2] = points[i2] - dm_in;
                temp_points[i2*4+3] = points[i2] - dm_out;

                // Add indexes
                _IdxWritePtr[0]  = (ImDrawIdx)(idx2+1); _IdxWritePtr[1]  = (ImDrawIdx)(idx1+1); _IdxWritePtr[2]  = (ImDrawIdx)(idx1+2);
                _IdxWritePtr[3]  = (ImDrawIdx)(idx1+2); _IdxWritePtr[4]  = (ImDrawIdx)(idx2+2); _IdxWritePtr[5]  = (ImDrawIdx)(idx2+1);
                _IdxWritePtr[6]  = (ImDrawIdx)(idx2+1); _IdxWritePtr[7]  = (ImDrawIdx)(idx1+1); _IdxWritePtr[8]  = (ImDrawIdx)(idx1+0);
                _IdxWritePtr[9]  = (ImDrawIdx)(idx1+0); _IdxWritePtr[10] = (ImDrawIdx)(idx2+0); _IdxWritePtr[11] = (ImDrawIdx)(idx2+1);
                _IdxWritePtr[12] = (ImDrawIdx)(idx2+2); _IdxWritePtr[13] = (ImDrawIdx)(idx1+2); _IdxWritePtr[14] = (ImDrawIdx)(idx1+3);
                _IdxWritePtr[15] = (ImDrawIdx)(idx1+3); _IdxWritePtr[16] = (ImDrawIdx)(idx2+3); _IdxWritePtr[17] = (ImDrawIdx)(idx2+2);
                _IdxWritePtr += 18;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = temp_points[i*4+0]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col_trans;
                _VtxWritePtr[1].pos = temp_points[i*4+1]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
                _VtxWritePtr[2].pos = temp_points[i*4+2]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
                _VtxWritePtr[3].pos = temp_points[i*4+3]; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col_trans;
                _VtxWritePtr += 4;
            }
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Stroke
        const int idx_count = count*6;
        const int vtx_count = count*4;      // FIXME-OPT: Not sharing edges
        PrimReserve(idx_count, vtx_count);

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1+1) == points_count ? 0 : i1+1; 
            const ImVec2& p1 = points[i1];
            const ImVec2& p2 = points[i2];
            ImVec2 diff = p2 - p1;
            diff *= ImInvLength(diff, 1.0f);

            const float dx = diff.x * (thickness * 0.5f);
            const float dy = diff.y * (thickness * 0.5f);
            _VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
            _VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
            _VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
            _VtxWritePtr += 4;

            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+2); 
            _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx+2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx+3); 
            _IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}

void ImDrawList::AddConvexPolyFilled(const ImVec2* points, const int points_count, ImU32 col, bool anti_aliased)
{
    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    anti_aliased &= GImGui->Style.AntiAliasedShapes;
    //if (ImGui::GetIO().KeyCtrl) anti_aliased = false;

    if (anti_aliased)
    {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & 0x00ffffff;
        const int idx_count = (points_count-2)*3 + points_count*6;
        const int vtx_count = (points_count*2);
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx+1;
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+((i-1)<<1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx+(i<<1));
            _IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2));
        for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            ImVec2 diff = p1 - p0;
            diff *= ImInvLength(diff, 1.0f);
            temp_normals[i0].x = diff.y;
            temp_normals[i0].y = -diff.x;
        }

        for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            // Average normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
            ImVec2 dm = (n0 + n1) * 0.5f;
            float dmr2 = dm.x*dm.x + dm.y*dm.y;
            if (dmr2 > 0.000001f)
            {
                float scale = 1.0f / dmr2;
                if (scale > 100.0f) scale = 100.0f;
                dm *= scale;
            }
            dm *= AA_SIZE * 0.5f;

            // Add vertices
            _VtxWritePtr[0].pos = (points[i1] - dm); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
            _VtxWritePtr[1].pos = (points[i1] + dm); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
            _VtxWritePtr += 2;

            // Add indexes for fringes
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx+(i1<<1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+(i0<<1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx+(i0<<1));
            _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx+(i0<<1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx+(i1<<1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx+(i1<<1));
            _IdxWritePtr += 6;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count-2)*3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            _VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+i-1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+i); 
            _IdxWritePtr += 3;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}

void ImDrawList::PathArcToFast(const ImVec2& centre, float radius, int amin, int amax)
{
    static ImVec2 circle_vtx[12];
    static bool circle_vtx_builds = false;
    const int circle_vtx_count = IM_ARRAYSIZE(circle_vtx);
    if (!circle_vtx_builds)
    {
        for (int i = 0; i < circle_vtx_count; i++)
        {
            const float a = ((float)i / (float)circle_vtx_count) * 2*IM_PI;
            circle_vtx[i].x = cosf(a);
            circle_vtx[i].y = sinf(a);
        }
        circle_vtx_builds = true;
    }

    if (amin > amax) return;
    if (radius == 0.0f)
    {
        _Path.push_back(centre);
    }
    else
    {
        _Path.reserve(_Path.Size + (amax - amin + 1));
        for (int a = amin; a <= amax; a++)
        {
            const ImVec2& c = circle_vtx[a % circle_vtx_count];
            _Path.push_back(ImVec2(centre.x + c.x * radius, centre.y + c.y * radius));
        }
    }
}

void ImDrawList::PathArcTo(const ImVec2& centre, float radius, float amin, float amax, int num_segments)
{
    if (radius == 0.0f)
        _Path.push_back(centre);
    _Path.reserve(_Path.Size + (num_segments + 1));
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
        _Path.push_back(ImVec2(centre.x + cosf(a) * radius, centre.y + sinf(a) * radius));
    }
}

void ImDrawList::PathRect(const ImVec2& a, const ImVec2& b, float rounding, int rounding_corners)
{
    float r = rounding;
    r = ImMin(r, fabsf(b.x-a.x) * ( ((rounding_corners&(1|2))==(1|2)) || ((rounding_corners&(4|8))==(4|8)) ? 0.5f : 1.0f ) - 1.0f);
    r = ImMin(r, fabsf(b.y-a.y) * ( ((rounding_corners&(1|8))==(1|8)) || ((rounding_corners&(2|4))==(2|4)) ? 0.5f : 1.0f ) - 1.0f);

    if (r == 0.0f || rounding_corners == 0)
    {
        PathLineTo(a);
        PathLineTo(ImVec2(b.x,a.y));
        PathLineTo(b);
        PathLineTo(ImVec2(a.x,b.y));
    }
    else
    {
        const float r0 = (rounding_corners & 1) ? r : 0.0f;
        const float r1 = (rounding_corners & 2) ? r : 0.0f;
        const float r2 = (rounding_corners & 4) ? r : 0.0f;
        const float r3 = (rounding_corners & 8) ? r : 0.0f;
        PathArcToFast(ImVec2(a.x+r0,a.y+r0), r0, 6, 9);
        PathArcToFast(ImVec2(b.x-r1,a.y+r1), r1, 9, 12);
        PathArcToFast(ImVec2(b.x-r2,b.y-r2), r2, 0, 3);
        PathArcToFast(ImVec2(a.x+r3,b.y-r3), r3, 3, 6);
    }
}

void ImDrawList::AddLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness)
{
    if ((col >> 24) == 0)
        return;
    PathLineTo(a + ImVec2(0.5f,0.5f));
    PathLineTo(b + ImVec2(0.5f,0.5f));
    PathStroke(col, false, thickness);
}

void ImDrawList::AddRect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners)
{
    if ((col >> 24) == 0)
        return;
    PathRect(a + ImVec2(0.5f,0.5f), b + ImVec2(0.5f,0.5f), rounding, rounding_corners);
    PathStroke(col, true);
}

void ImDrawList::AddRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners)
{
    if ((col >> 24) == 0)
        return;
    if (rounding > 0.0f)
    {
        PathRect(a, b, rounding, rounding_corners);
        PathFill(col);
    }
    else
    {
        PrimReserve(6, 4);
        PrimRect(a, b, col);
    }
}

void ImDrawList::AddRectFilledMultiColor(const ImVec2& a, const ImVec2& c, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) >> 24) == 0)
        return;

    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+1)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+2));
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+2)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+3));
    PrimWriteVtx(a, uv, col_upr_left);
    PrimWriteVtx(ImVec2(c.x, a.y), uv, col_upr_right);
    PrimWriteVtx(c, uv, col_bot_right);
    PrimWriteVtx(ImVec2(a.x, c.y), uv, col_bot_left);
}

void ImDrawList::AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col)
{
    if ((col >> 24) == 0)
        return;
    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathFill(col);
}

void ImDrawList::AddCircle(const ImVec2& centre, float radius, ImU32 col, int num_segments)
{
    if ((col >> 24) == 0)
        return;

    const float a_max = IM_PI*2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius, 0.0f, a_max, num_segments);
    PathStroke(col, true);
}

void ImDrawList::AddCircleFilled(const ImVec2& centre, float radius, ImU32 col, int num_segments)
{
    if ((col >> 24) == 0)
        return;

    const float a_max = IM_PI*2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius, 0.0f, a_max, num_segments);
    PathFill(col);
}

void ImDrawList::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
    if ((col >> 24) == 0)
        return;

    if (text_end == NULL)
        text_end = text_begin + strlen(text_begin);
    if (text_begin == text_end)
        return;

    IM_ASSERT(font->ContainerAtlas->TexID == _TextureIdStack.back());  // Use high-level ImGui::PushFont() or low-level ImDrawList::PushTextureId() to change font.

    // reserve vertices for worse case
    const int char_count = (int)(text_end - text_begin);
    const int vtx_count_max = char_count * 4;
    const int idx_count_max = char_count * 6;
    const int vtx_begin = VtxBuffer.Size;
    const int idx_begin = IdxBuffer.Size;
    PrimReserve(idx_count_max, vtx_count_max);

    ImVec4 clip_rect = _ClipRectStack.back();
    if (cpu_fine_clip_rect)
    {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }
    font->RenderText(font_size, pos, col, clip_rect, text_begin, text_end, this, wrap_width, cpu_fine_clip_rect != NULL);

    // give back unused vertices
    // FIXME-OPT
    VtxBuffer.resize((int)(_VtxWritePtr - VtxBuffer.Data));
    IdxBuffer.resize((int)(_IdxWritePtr - IdxBuffer.Data));
    int vtx_unused = vtx_count_max - (VtxBuffer.Size - vtx_begin);
    int idx_unused = idx_count_max - (IdxBuffer.Size - idx_begin);
    CmdBuffer.back().ElemCount -= idx_unused;
    _VtxWritePtr -= vtx_unused;
    _IdxWritePtr -= idx_unused;
}

// This is one of the few function breaking the encapsulation of ImDrawLst, but it is just so useful.
void ImDrawList::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
    if ((col >> 24) == 0)
        return;
    AddText(ImGui::GetWindowFont(), ImGui::GetWindowFontSize(), pos, col, text_begin, text_end);
}

void ImDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv0, const ImVec2& uv1, ImU32 col)
{
    if ((col >> 24) == 0)
        return;

    // FIXME-OPT: This is wasting draw calls.
    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimRectUV(a, b, uv0, uv1, col);

    if (push_texture_id)
        PopTextureID();
}

//-----------------------------------------------------------------------------
// ImDrawData
//-----------------------------------------------------------------------------

// For backward compatibility: convert all buffers from indexed to de-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
void ImDrawData::DeIndexAllBuffers()
{
    ImVector<ImDrawVert> new_vtx_buffer;
    TotalVtxCount = TotalIdxCount = 0;
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        if (cmd_list->IdxBuffer.empty())
            continue;
        new_vtx_buffer.resize(cmd_list->IdxBuffer.Size);
        for (int j = 0; j < cmd_list->IdxBuffer.Size; j++)
            new_vtx_buffer[j] = cmd_list->VtxBuffer[cmd_list->IdxBuffer[j]];
        cmd_list->VtxBuffer.swap(new_vtx_buffer);
        cmd_list->IdxBuffer.resize(0);
        TotalVtxCount += cmd_list->VtxBuffer.Size;
    }
}

//-----------------------------------------------------------------------------
// ImFontAtlias
//-----------------------------------------------------------------------------

ImFontConfig::ImFontConfig()
{
    FontData = NULL;
    FontDataSize = 0;
    FontDataOwnedByAtlas = true;
    FontNo = 0;
    SizePixels = 0.0f;
    OversampleH = 3;
    OversampleV = 1;
    PixelSnapH = false;
    GlyphExtraSpacing = ImVec2(0.0f, 0.0f);
    GlyphRanges = NULL;
    MergeMode = false;
    MergeGlyphCenterV = false;
    DstFont = NULL;
    memset(Name, 0, sizeof(Name));
}

ImFontAtlas::ImFontAtlas()
{
    TexID = NULL;
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
    TexWidth = TexHeight = 0;
    TexUvWhitePixel = ImVec2(0, 0);
}

ImFontAtlas::~ImFontAtlas()
{
    Clear();
}

void    ImFontAtlas::ClearInputData()
{
    for (int i = 0; i < ConfigData.Size; i++)
        if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas)
        {
            ImGui::MemFree(ConfigData[i].FontData);
            ConfigData[i].FontData = NULL;
        }

    // When clearing this we lose access to the font name and other information used to build the font.
    for (int i = 0; i < Fonts.Size; i++)
        if (Fonts[i]->ConfigData >= ConfigData.Data && Fonts[i]->ConfigData < ConfigData.Data + ConfigData.Size)
        {
            Fonts[i]->ConfigData = NULL;
            Fonts[i]->ConfigDataCount = 0;
        }
    ConfigData.clear();
}

void    ImFontAtlas::ClearTexData()
{
    if (TexPixelsAlpha8)
        ImGui::MemFree(TexPixelsAlpha8);
    if (TexPixelsRGBA32)
        ImGui::MemFree(TexPixelsRGBA32);
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
}

void    ImFontAtlas::ClearFonts()
{
    for (int i = 0; i < Fonts.Size; i++)
    {
        Fonts[i]->~ImFont();
        ImGui::MemFree(Fonts[i]);
    }
    Fonts.clear();
}

void    ImFontAtlas::Clear()
{
    ClearInputData(); 
    ClearTexData();
    ClearFonts();
}

void    ImFontAtlas::GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Lazily build
    if (TexPixelsAlpha8 == NULL)
    {
        if (ConfigData.empty())
            AddFontDefault();
        Build();
    }

    *out_pixels = TexPixelsAlpha8;
    if (out_width) *out_width = TexWidth;
    if (out_height) *out_height = TexHeight;
    if (out_bytes_per_pixel) *out_bytes_per_pixel = 1;
}

void    ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Lazily convert to RGBA32 format
    // Although it is likely to be the most commonly used format, our font rendering is 8 bpp
    if (!TexPixelsRGBA32)
    {
        unsigned char* pixels;
        GetTexDataAsAlpha8(&pixels, NULL, NULL);
        TexPixelsRGBA32 = (unsigned int*)ImGui::MemAlloc((size_t)(TexWidth * TexHeight * 4));
        const unsigned char* src = pixels;
        unsigned int* dst = TexPixelsRGBA32;
        for (int n = TexWidth * TexHeight; n > 0; n--)
            *dst++ = ((unsigned int)(*src++) << 24) | 0x00FFFFFF;
    }

    *out_pixels = (unsigned char*)TexPixelsRGBA32;
    if (out_width) *out_width = TexWidth;
    if (out_height) *out_height = TexHeight;
    if (out_bytes_per_pixel) *out_bytes_per_pixel = 4;
}

ImFont* ImFontAtlas::AddFont(const ImFontConfig* font_cfg)
{
    IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
    IM_ASSERT(font_cfg->SizePixels > 0.0f);

    // Create new font
    if (!font_cfg->MergeMode)
    {
        ImFont* font = (ImFont*)ImGui::MemAlloc(sizeof(ImFont));
        new (font) ImFont();
        Fonts.push_back(font);
    }

    ConfigData.push_back(*font_cfg);
    ImFontConfig& new_font_cfg = ConfigData.back();
    new_font_cfg.DstFont = Fonts.back();
    if (!new_font_cfg.FontDataOwnedByAtlas)
    {
        new_font_cfg.FontData = ImGui::MemAlloc(new_font_cfg.FontDataSize);
        new_font_cfg.FontDataOwnedByAtlas = true;
        memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
    }

    // Invalidate texture
    ClearTexData();
    return Fonts.back();
}

// Default font ttf is compressed with stb_compress then base85 encoded (see extra_fonts/binary_to_compressed_c.cpp for encoder)
static unsigned int stb_decompress_length(unsigned char *input);
static unsigned int stb_decompress(unsigned char *output, unsigned char *i, unsigned int length);
static const char*  GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c)                                    { return c >= '\\' ? c-36 : c-35; }
static void         Decode85(const unsigned char* src, unsigned int* dst)   { for (; *src; src += 5) *dst++ = Decode85Byte(src[0]) + 85*(Decode85Byte(src[1]) + 85*(Decode85Byte(src[2]) + 85*(Decode85Byte(src[3]) + 85*Decode85Byte(src[4])))); }

// Load embedded ProggyClean.ttf at size 13
ImFont* ImFontAtlas::AddFontDefault(const ImFontConfig* font_cfg_template)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (!font_cfg_template)
    {
        font_cfg.OversampleH = font_cfg.OversampleV = 1;
        font_cfg.PixelSnapH = true;
    }
    if (font_cfg.Name[0] == '\0') strcpy(font_cfg.Name, "<default>");

    const char* ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
    ImFont* font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85, 13.0f, &font_cfg, GetGlyphRangesDefault());
    return font;
}

ImFont* ImFontAtlas::AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    void* data = NULL;
    int data_size = 0;
    if (!ImLoadFileToMemory(filename, "rb", (void**)&data, &data_size))
    {
        IM_ASSERT(0); // Could not load file.
        return NULL;
    }
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (font_cfg.Name[0] == '\0')
    {
        const char* p; 
        for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {}
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s", p);
    }
    return AddFontFromMemoryTTF(data, data_size, size_pixels, &font_cfg, glyph_ranges);
}

// Transfer ownership of 'ttf_data' to ImFontAtlas, will be deleted after Build()
ImFont* ImFontAtlas::AddFontFromMemoryTTF(void* ttf_data, int ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL); 
    font_cfg.FontData = ttf_data;
    font_cfg.FontDataSize = ttf_size;
    font_cfg.SizePixels = size_pixels;
    if (glyph_ranges)
        font_cfg.GlyphRanges = glyph_ranges;
    return AddFont(&font_cfg);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedTTF(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    const unsigned int buf_decompressed_size = stb_decompress_length((unsigned char*)compressed_ttf_data);
    unsigned char* buf_decompressed_data = (unsigned char *)ImGui::MemAlloc(buf_decompressed_size);
    stb_decompress(buf_decompressed_data, (unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL); 
    font_cfg.FontDataOwnedByAtlas = true;
    return AddFontFromMemoryTTF(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, font_cfg_template, glyph_ranges);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressed_ttf_data_base85, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges)
{
    int compressed_ttf_size = (((int)strlen(compressed_ttf_data_base85) + 4) / 5) * 4;
    void* compressed_ttf = ImGui::MemAlloc(compressed_ttf_size);
    Decode85((const unsigned char*)compressed_ttf_data_base85, (unsigned int*)compressed_ttf);
    ImFont* font = AddFontFromMemoryCompressedTTF(compressed_ttf, compressed_ttf_size, size_pixels, font_cfg, glyph_ranges);
    ImGui::MemFree(compressed_ttf);
    return font;
}

bool    ImFontAtlas::Build()
{
    IM_ASSERT(ConfigData.Size > 0);

    TexID = NULL;
    TexWidth = TexHeight = 0;
    TexUvWhitePixel = ImVec2(0, 0);
    ClearTexData();

    struct ImFontTempBuildData
    {
        stbtt_fontinfo      FontInfo;
        stbrp_rect*         Rects;
        stbtt_pack_range*   Ranges;
        int                 RangesCount;
    };
    ImFontTempBuildData* tmp_array = (ImFontTempBuildData*)ImGui::MemAlloc((size_t)ConfigData.Size * sizeof(ImFontTempBuildData));

    // Initialize font information early (so we can error without any cleanup) + count glyphs
    int total_glyph_count = 0;
    int total_glyph_range_count = 0;
    for (int input_i = 0; input_i < ConfigData.Size; input_i++)
    {
        ImFontConfig& cfg = ConfigData[input_i];
        ImFontTempBuildData& tmp = tmp_array[input_i];

        IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == this));
        const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData, cfg.FontNo);
        IM_ASSERT(font_offset >= 0);
        if (!stbtt_InitFont(&tmp.FontInfo, (unsigned char*)cfg.FontData, font_offset)) 
            return false;

        if (!cfg.GlyphRanges)
            cfg.GlyphRanges = GetGlyphRangesDefault();
        for (const ImWchar* in_range = cfg.GlyphRanges; in_range[0] && in_range[1]; in_range += 2)
        {
            total_glyph_count += (in_range[1] - in_range[0]) + 1;
            total_glyph_range_count++;
        }
    }

    // Start packing
    TexWidth = (total_glyph_count > 1000) ? 1024 : 512;  // Width doesn't actually matters.
    TexHeight = 0;
    const int max_tex_height = 1024*32;
    stbtt_pack_context spc;
    stbtt_PackBegin(&spc, NULL, TexWidth, max_tex_height, 0, 1, NULL);

    // Pack our extra data rectangles first, so it will be on the upper-left corner of our texture (UV will have small values).
    ImVector<stbrp_rect> extra_rects;
    RenderCustomTexData(0, &extra_rects);
    stbtt_PackSetOversampling(&spc, 1, 1);
    stbrp_pack_rects((stbrp_context*)spc.pack_info, &extra_rects[0], extra_rects.Size);
    for (int i = 0; i < extra_rects.Size; i++)
        if (extra_rects[i].was_packed)
            TexHeight = ImMax(TexHeight, extra_rects[i].y + extra_rects[i].h);

    // Allocate packing character data and flag packed characters buffer as non-packed (x0=y0=x1=y1=0)
    int buf_packedchars_n = 0, buf_rects_n = 0, buf_ranges_n = 0;
    stbtt_packedchar* buf_packedchars = (stbtt_packedchar*)ImGui::MemAlloc(total_glyph_count * sizeof(stbtt_packedchar));
    stbrp_rect* buf_rects = (stbrp_rect*)ImGui::MemAlloc(total_glyph_count * sizeof(stbrp_rect));
    stbtt_pack_range* buf_ranges = (stbtt_pack_range*)ImGui::MemAlloc(total_glyph_range_count * sizeof(stbtt_pack_range));
    memset(buf_packedchars, 0, total_glyph_count * sizeof(stbtt_packedchar));
    memset(buf_rects, 0, total_glyph_count * sizeof(stbrp_rect));              // Unnecessary but let's clear this for the sake of sanity.
    memset(buf_ranges, 0, total_glyph_range_count * sizeof(stbtt_pack_range));

    // First font pass: pack all glyphs (no rendering at this point, we are working with glyph sizes only)
    for (int input_i = 0; input_i < ConfigData.Size; input_i++)
    {
        ImFontConfig& cfg = ConfigData[input_i];
        ImFontTempBuildData& tmp = tmp_array[input_i];

        // Setup ranges
        int glyph_count = 0;
        int glyph_ranges_count = 0;
        for (const ImWchar* in_range = cfg.GlyphRanges; in_range[0] && in_range[1]; in_range += 2)
        {
            glyph_count += (in_range[1] - in_range[0]) + 1;
            glyph_ranges_count++;
        }
        tmp.Ranges = buf_ranges + buf_ranges_n;
        tmp.RangesCount = glyph_ranges_count;
        buf_ranges_n += glyph_ranges_count;
        for (int i = 0; i < glyph_ranges_count; i++)
        {
            const ImWchar* in_range = &cfg.GlyphRanges[i * 2];
            stbtt_pack_range& range = tmp.Ranges[i];
            range.font_size = cfg.SizePixels;
            range.first_unicode_codepoint_in_range = in_range[0];
            range.num_chars = (in_range[1] - in_range[0]) + 1;
            range.chardata_for_range = buf_packedchars + buf_packedchars_n;
            buf_packedchars_n += range.num_chars;
        }

        // Pack
        tmp.Rects = buf_rects + buf_rects_n;
        buf_rects_n += glyph_count;
        stbtt_PackSetOversampling(&spc, cfg.OversampleH, cfg.OversampleV);
        int n = stbtt_PackFontRangesGatherRects(&spc, &tmp.FontInfo, tmp.Ranges, tmp.RangesCount, tmp.Rects);
        stbrp_pack_rects((stbrp_context*)spc.pack_info, tmp.Rects, n);

        // Extend texture height
        for (int i = 0; i < n; i++)
            if (tmp.Rects[i].was_packed)
                TexHeight = ImMax(TexHeight, tmp.Rects[i].y + tmp.Rects[i].h);
    }
    IM_ASSERT(buf_rects_n == total_glyph_count);
    IM_ASSERT(buf_packedchars_n == total_glyph_count);
    IM_ASSERT(buf_ranges_n == total_glyph_range_count);

    // Create texture
    TexHeight = ImUpperPowerOfTwo(TexHeight);
    TexPixelsAlpha8 = (unsigned char*)ImGui::MemAlloc(TexWidth * TexHeight);
    memset(TexPixelsAlpha8, 0, TexWidth * TexHeight);
    spc.pixels = TexPixelsAlpha8;
    spc.height = TexHeight;

    // Second pass: render characters
    for (int input_i = 0; input_i < ConfigData.Size; input_i++)
    {
        ImFontConfig& cfg = ConfigData[input_i];
        ImFontTempBuildData& tmp = tmp_array[input_i];
        stbtt_PackSetOversampling(&spc, cfg.OversampleH, cfg.OversampleV);
        stbtt_PackFontRangesRenderIntoRects(&spc, &tmp.FontInfo, tmp.Ranges, tmp.RangesCount, tmp.Rects);
        tmp.Rects = NULL;
    }

    // End packing
    stbtt_PackEnd(&spc);
    ImGui::MemFree(buf_rects);
    buf_rects = NULL;

    // Third pass: setup ImFont and glyphs for runtime
    for (int input_i = 0; input_i < ConfigData.Size; input_i++)
    {
        ImFontConfig& cfg = ConfigData[input_i];
        ImFontTempBuildData& tmp = tmp_array[input_i];
        ImFont* dst_font = cfg.DstFont;

        float font_scale = stbtt_ScaleForPixelHeight(&tmp.FontInfo, cfg.SizePixels);
        int unscaled_ascent, unscaled_descent, unscaled_line_gap;
        stbtt_GetFontVMetrics(&tmp.FontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

        float ascent = unscaled_ascent * font_scale;
        float descent = unscaled_descent * font_scale;
        if (!cfg.MergeMode)
        {
            dst_font->ContainerAtlas = this;
            dst_font->ConfigData = &cfg;
            dst_font->ConfigDataCount = 0;
            dst_font->FontSize = cfg.SizePixels;
            dst_font->Ascent = ascent;
            dst_font->Descent = descent;
            dst_font->Glyphs.resize(0);
        }
        dst_font->ConfigDataCount++;
        float off_y = (cfg.MergeMode && cfg.MergeGlyphCenterV) ? (ascent - dst_font->Ascent) * 0.5f : 0.0f;

        dst_font->FallbackGlyph = NULL; // Always clear fallback so FindGlyph can return NULL. It will be set again in BuildLookupTable()
        for (int i = 0; i < tmp.RangesCount; i++)
        {
            stbtt_pack_range& range = tmp.Ranges[i];
            for (int char_idx = 0; char_idx < range.num_chars; char_idx += 1)
            {
                const stbtt_packedchar& pc = range.chardata_for_range[char_idx];
                if (!pc.x0 && !pc.x1 && !pc.y0 && !pc.y1)
                    continue;

                const int codepoint = range.first_unicode_codepoint_in_range + char_idx;
                if (cfg.MergeMode && dst_font->FindGlyph((unsigned short)codepoint))
                    continue;

                stbtt_aligned_quad q;
                float dummy_x = 0.0f, dummy_y = 0.0f;
                stbtt_GetPackedQuad(range.chardata_for_range, TexWidth, TexHeight, char_idx, &dummy_x, &dummy_y, &q, 0);

                dst_font->Glyphs.resize(dst_font->Glyphs.Size + 1);
                ImFont::Glyph& glyph = dst_font->Glyphs.back();
                glyph.Codepoint = (ImWchar)codepoint;
                glyph.X0 = q.x0; glyph.Y0 = q.y0; glyph.X1 = q.x1; glyph.Y1 = q.y1;                
                glyph.U0 = q.s0; glyph.V0 = q.t0; glyph.U1 = q.s1; glyph.V1 = q.t1;
                glyph.Y0 += (float)(int)(dst_font->Ascent + off_y + 0.5f);
                glyph.Y1 += (float)(int)(dst_font->Ascent + off_y + 0.5f);
                glyph.XAdvance = (pc.xadvance + cfg.GlyphExtraSpacing.x);  // Bake spacing into XAdvance
                if (cfg.PixelSnapH)
                    glyph.XAdvance = (float)(int)(glyph.XAdvance + 0.5f);
            }
        }
        cfg.DstFont->BuildLookupTable();
    }

    // Cleanup temporaries
    ImGui::MemFree(buf_packedchars);
    ImGui::MemFree(buf_ranges);
    ImGui::MemFree(tmp_array);

    // Render into our custom data block
    RenderCustomTexData(1, &extra_rects);

    return true;
}

void ImFontAtlas::RenderCustomTexData(int pass, void* p_rects)
{
    // . = white layer, X = black layer, others are blank
    const int TEX_DATA_W = 90;
    const int TEX_DATA_H = 27;
    const char texture_data[TEX_DATA_W*TEX_DATA_H+1] =
    {
        "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX"
        "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X"
        "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X"
        "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X"
        "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X"
        "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X"
        "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX"
        "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      "
        "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       "
        "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        "
        "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         "
        "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          "
        "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           "
        "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            "
        "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           "
        "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          "
        "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          "
        "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       ------------------------------------"
        "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           "
        "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           "
        "      X..X          -  X...X  -         X...X         -  X..X           X..X  -           "
        "       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           "
        "------------        -    X    -           X           -X.....................X-           "
        "                    ----------------------------------- X...XXXXXXXXXXXXX...X -           "
        "                                                      -  X..X           X..X  -           "
        "                                                      -   X.X           X.X   -           "
        "                                                      -    XX           XX    -           "
    };

    ImVector<stbrp_rect>& rects = *(ImVector<stbrp_rect>*)p_rects;
    if (pass == 0)
    {
        stbrp_rect r;
        memset(&r, 0, sizeof(r));
        r.w = (TEX_DATA_W*2)+1;
        r.h = TEX_DATA_H+1;
        rects.push_back(r);
    }
    else if (pass == 1)
    {
        // Copy pixels
        const stbrp_rect& r = rects[0];
        for (int y = 0, n = 0; y < TEX_DATA_H; y++)
            for (int x = 0; x < TEX_DATA_W; x++, n++)
            {
                const int offset0 = (int)(r.x + x) + (int)(r.y + y) * TexWidth;
                const int offset1 = offset0 + 1 + TEX_DATA_W;
                TexPixelsAlpha8[offset0] = texture_data[n] == '.' ? 0xFF : 0x00;
                TexPixelsAlpha8[offset1] = texture_data[n] == 'X' ? 0xFF : 0x00;
            }
        const ImVec2 tex_uv_scale(1.0f / TexWidth, 1.0f / TexHeight);
        TexUvWhitePixel = ImVec2(r.x + 0.5f, r.y + 0.5f) * tex_uv_scale;

        const ImVec2 cursor_datas[ImGuiMouseCursor_Count_][3] =
        {
            // Pos ........ Size ......... Offset ......
            { ImVec2(0,3),  ImVec2(12,19), ImVec2( 0, 0) }, // ImGuiMouseCursor_Arrow
            { ImVec2(13,0), ImVec2(7,16),  ImVec2( 4, 8) }, // ImGuiMouseCursor_TextInput
            { ImVec2(31,0), ImVec2(23,23), ImVec2(11,11) }, // ImGuiMouseCursor_Move
            { ImVec2(21,0), ImVec2( 9,23), ImVec2( 5,11) }, // ImGuiMouseCursor_ResizeNS
            { ImVec2(55,18),ImVec2(23, 9), ImVec2(11, 5) }, // ImGuiMouseCursor_ResizeEW
            { ImVec2(73,0), ImVec2(17,17), ImVec2( 9, 9) }, // ImGuiMouseCursor_ResizeNESW
            { ImVec2(55,0), ImVec2(17,17), ImVec2( 9, 9) }, // ImGuiMouseCursor_ResizeNWSE
        };

        for (int type = 0; type < ImGuiMouseCursor_Count_; type++)
        {
            ImGuiMouseCursorData& cursor_data = GImGui->MouseCursorData[type];
            ImVec2 pos = cursor_datas[type][0] + ImVec2((float)r.x, (float)r.y);
            const ImVec2 size = cursor_datas[type][1];
            cursor_data.Type = type;
            cursor_data.Size = size;
            cursor_data.Offset = cursor_datas[type][2];
            cursor_data.TexUvMin[0] = (pos) * tex_uv_scale;
            cursor_data.TexUvMax[0] = (pos + size) * tex_uv_scale;
            pos.x += TEX_DATA_W+1;
            cursor_data.TexUvMin[1] = (pos) * tex_uv_scale;
            cursor_data.TexUvMax[1] = (pos + size) * tex_uv_scale;
        }
    }
}

// Retrieve list of range (2 int per range, values are inclusive)
const ImWchar*   ImFontAtlas::GetGlyphRangesDefault()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesChinese()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0x4e00, 0x9FAF, // CJK Ideograms
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesJapanese()
{
    // Store the 1946 ideograms code points as successive offsets from the initial unicode codepoint 0x4E00. Each offset has an implicit +1.
    // This encoding helps us reduce the source code size.
    static const short offsets_from_0x4E00[] = 
    {
        -1,0,1,3,0,0,0,0,1,0,5,1,1,0,7,4,6,10,0,1,9,9,7,1,3,19,1,10,7,1,0,1,0,5,1,0,6,4,2,6,0,0,12,6,8,0,3,5,0,1,0,9,0,0,8,1,1,3,4,5,13,0,0,8,2,17,
        4,3,1,1,9,6,0,0,0,2,1,3,2,22,1,9,11,1,13,1,3,12,0,5,9,2,0,6,12,5,3,12,4,1,2,16,1,1,4,6,5,3,0,6,13,15,5,12,8,14,0,0,6,15,3,6,0,18,8,1,6,14,1,
        5,4,12,24,3,13,12,10,24,0,0,0,1,0,1,1,2,9,10,2,2,0,0,3,3,1,0,3,8,0,3,2,4,4,1,6,11,10,14,6,15,3,4,15,1,0,0,5,2,2,0,0,1,6,5,5,6,0,3,6,5,0,0,1,0,
        11,2,2,8,4,7,0,10,0,1,2,17,19,3,0,2,5,0,6,2,4,4,6,1,1,11,2,0,3,1,2,1,2,10,7,6,3,16,0,8,24,0,0,3,1,1,3,0,1,6,0,0,0,2,0,1,5,15,0,1,0,0,2,11,19,
        1,4,19,7,6,5,1,0,0,0,0,5,1,0,1,9,0,0,5,0,2,0,1,0,3,0,11,3,0,2,0,0,0,0,0,9,3,6,4,12,0,14,0,0,29,10,8,0,14,37,13,0,31,16,19,0,8,30,1,20,8,3,48,
        21,1,0,12,0,10,44,34,42,54,11,18,82,0,2,1,2,12,1,0,6,2,17,2,12,7,0,7,17,4,2,6,24,23,8,23,39,2,16,23,1,0,5,1,2,15,14,5,6,2,11,0,8,6,2,2,2,14,
        20,4,15,3,4,11,10,10,2,5,2,1,30,2,1,0,0,22,5,5,0,3,1,5,4,1,0,0,2,2,21,1,5,1,2,16,2,1,3,4,0,8,4,0,0,5,14,11,2,16,1,13,1,7,0,22,15,3,1,22,7,14,
        22,19,11,24,18,46,10,20,64,45,3,2,0,4,5,0,1,4,25,1,0,0,2,10,0,0,0,1,0,1,2,0,0,9,1,2,0,0,0,2,5,2,1,1,5,5,8,1,1,1,5,1,4,9,1,3,0,1,0,1,1,2,0,0,
        2,0,1,8,22,8,1,0,0,0,0,4,2,1,0,9,8,5,0,9,1,30,24,2,6,4,39,0,14,5,16,6,26,179,0,2,1,1,0,0,0,5,2,9,6,0,2,5,16,7,5,1,1,0,2,4,4,7,15,13,14,0,0,
        3,0,1,0,0,0,2,1,6,4,5,1,4,9,0,3,1,8,0,0,10,5,0,43,0,2,6,8,4,0,2,0,0,9,6,0,9,3,1,6,20,14,6,1,4,0,7,2,3,0,2,0,5,0,3,1,0,3,9,7,0,3,4,0,4,9,1,6,0,
        9,0,0,2,3,10,9,28,3,6,2,4,1,2,32,4,1,18,2,0,3,1,5,30,10,0,2,2,2,0,7,9,8,11,10,11,7,2,13,7,5,10,0,3,40,2,0,1,6,12,0,4,5,1,5,11,11,21,4,8,3,7,
        8,8,33,5,23,0,0,19,8,8,2,3,0,6,1,1,1,5,1,27,4,2,5,0,3,5,6,3,1,0,3,1,12,5,3,3,2,0,7,7,2,1,0,4,0,1,1,2,0,10,10,6,2,5,9,7,5,15,15,21,6,11,5,20,
        4,3,5,5,2,5,0,2,1,0,1,7,28,0,9,0,5,12,5,5,18,30,0,12,3,3,21,16,25,32,9,3,14,11,24,5,66,9,1,2,0,5,9,1,5,1,8,0,8,3,3,0,1,15,1,4,8,1,2,7,0,7,2,
        8,3,7,5,3,7,10,2,1,0,0,2,25,0,6,4,0,10,0,4,2,4,1,12,5,38,4,0,4,1,10,5,9,4,0,14,4,2,5,18,20,21,1,3,0,5,0,7,0,3,7,1,3,1,1,8,1,0,0,0,3,2,5,2,11,
        6,0,13,1,3,9,1,12,0,16,6,2,1,0,2,1,12,6,13,11,2,0,28,1,7,8,14,13,8,13,0,2,0,5,4,8,10,2,37,42,19,6,6,7,4,14,11,18,14,80,7,6,0,4,72,12,36,27,
        7,7,0,14,17,19,164,27,0,5,10,7,3,13,6,14,0,2,2,5,3,0,6,13,0,0,10,29,0,4,0,3,13,0,3,1,6,51,1,5,28,2,0,8,0,20,2,4,0,25,2,10,13,10,0,16,4,0,1,0,
        2,1,7,0,1,8,11,0,0,1,2,7,2,23,11,6,6,4,16,2,2,2,0,22,9,3,3,5,2,0,15,16,21,2,9,20,15,15,5,3,9,1,0,0,1,7,7,5,4,2,2,2,38,24,14,0,0,15,5,6,24,14,
        5,5,11,0,21,12,0,3,8,4,11,1,8,0,11,27,7,2,4,9,21,59,0,1,39,3,60,62,3,0,12,11,0,3,30,11,0,13,88,4,15,5,28,13,1,4,48,17,17,4,28,32,46,0,16,0,
        18,11,1,8,6,38,11,2,6,11,38,2,0,45,3,11,2,7,8,4,30,14,17,2,1,1,65,18,12,16,4,2,45,123,12,56,33,1,4,3,4,7,0,0,0,3,2,0,16,4,2,4,2,0,7,4,5,2,26,
        2,25,6,11,6,1,16,2,6,17,77,15,3,35,0,1,0,5,1,0,38,16,6,3,12,3,3,3,0,9,3,1,3,5,2,9,0,18,0,25,1,3,32,1,72,46,6,2,7,1,3,14,17,0,28,1,40,13,0,20,
        15,40,6,38,24,12,43,1,1,9,0,12,6,0,6,2,4,19,3,7,1,48,0,9,5,0,5,6,9,6,10,15,2,11,19,3,9,2,0,1,10,1,27,8,1,3,6,1,14,0,26,0,27,16,3,4,9,6,2,23,
        9,10,5,25,2,1,6,1,1,48,15,9,15,14,3,4,26,60,29,13,37,21,1,6,4,0,2,11,22,23,16,16,2,2,1,3,0,5,1,6,4,0,0,4,0,0,8,3,0,2,5,0,7,1,7,3,13,2,4,10,
        3,0,2,31,0,18,3,0,12,10,4,1,0,7,5,7,0,5,4,12,2,22,10,4,2,15,2,8,9,0,23,2,197,51,3,1,1,4,13,4,3,21,4,19,3,10,5,40,0,4,1,1,10,4,1,27,34,7,21,
        2,17,2,9,6,4,2,3,0,4,2,7,8,2,5,1,15,21,3,4,4,2,2,17,22,1,5,22,4,26,7,0,32,1,11,42,15,4,1,2,5,0,19,3,1,8,6,0,10,1,9,2,13,30,8,2,24,17,19,1,4,
        4,25,13,0,10,16,11,39,18,8,5,30,82,1,6,8,18,77,11,13,20,75,11,112,78,33,3,0,0,60,17,84,9,1,1,12,30,10,49,5,32,158,178,5,5,6,3,3,1,3,1,4,7,6,
        19,31,21,0,2,9,5,6,27,4,9,8,1,76,18,12,1,4,0,3,3,6,3,12,2,8,30,16,2,25,1,5,5,4,3,0,6,10,2,3,1,0,5,1,19,3,0,8,1,5,2,6,0,0,0,19,1,2,0,5,1,2,5,
        1,3,7,0,4,12,7,3,10,22,0,9,5,1,0,2,20,1,1,3,23,30,3,9,9,1,4,191,14,3,15,6,8,50,0,1,0,0,4,0,0,1,0,2,4,2,0,2,3,0,2,0,2,2,8,7,0,1,1,1,3,3,17,11,
        91,1,9,3,2,13,4,24,15,41,3,13,3,1,20,4,125,29,30,1,0,4,12,2,21,4,5,5,19,11,0,13,11,86,2,18,0,7,1,8,8,2,2,22,1,2,6,5,2,0,1,2,8,0,2,0,5,2,1,0,
        2,10,2,0,5,9,2,1,2,0,1,0,4,0,0,10,2,5,3,0,6,1,0,1,4,4,33,3,13,17,3,18,6,4,7,1,5,78,0,4,1,13,7,1,8,1,0,35,27,15,3,0,0,0,1,11,5,41,38,15,22,6,
        14,14,2,1,11,6,20,63,5,8,27,7,11,2,2,40,58,23,50,54,56,293,8,8,1,5,1,14,0,1,12,37,89,8,8,8,2,10,6,0,0,0,4,5,2,1,0,1,1,2,7,0,3,3,0,4,6,0,3,2,
        19,3,8,0,0,0,4,4,16,0,4,1,5,1,3,0,3,4,6,2,17,10,10,31,6,4,3,6,10,126,7,3,2,2,0,9,0,0,5,20,13,0,15,0,6,0,2,5,8,64,50,3,2,12,2,9,0,0,11,8,20,
        109,2,18,23,0,0,9,61,3,0,28,41,77,27,19,17,81,5,2,14,5,83,57,252,14,154,263,14,20,8,13,6,57,39,38,
    };
    static int ranges_unpacked = false;
    static ImWchar ranges[8 + IM_ARRAYSIZE(offsets_from_0x4E00)*2 + 1] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
    };
    if (!ranges_unpacked)
    {
        // Unpack
        int codepoint = 0x4e00;
        ImWchar* dst = &ranges[8];
        for (int n = 0; n < IM_ARRAYSIZE(offsets_from_0x4E00); n++, dst += 2)
            dst[0] = dst[1] = (ImWchar)(codepoint += (offsets_from_0x4E00[n] + 1));
        dst[0] = 0;
        ranges_unpacked = true;
    }
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesCyrillic()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0,
    };
    return &ranges[0];
}

//-----------------------------------------------------------------------------
// ImFont
//-----------------------------------------------------------------------------

ImFont::ImFont()
{
    Scale = 1.0f;
    FallbackChar = (ImWchar)'?';
    Clear();
}

ImFont::~ImFont()
{
    // Invalidate active font so that the user gets a clear crash instead of a dangling pointer.
    // If you want to delete fonts you need to do it between Render() and NewFrame().
    ImGuiState& g = *GImGui;
    if (g.Font == this)
        g.Font = NULL;
    Clear();
}

void    ImFont::Clear()
{
    FontSize = 0.0f;
    DisplayOffset = ImVec2(0.0f, 1.0f);
    ConfigData = NULL;
    ConfigDataCount = 0;
    Ascent = Descent = 0.0f;
    ContainerAtlas = NULL;
    Glyphs.clear();
    FallbackGlyph = NULL;
    FallbackXAdvance = 0.0f;
    IndexXAdvance.clear();
    IndexLookup.clear();
}

void ImFont::BuildLookupTable()
{
    int max_codepoint = 0;
    for (int i = 0; i != Glyphs.Size; i++)
        max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

    IndexXAdvance.clear();
    IndexXAdvance.resize(max_codepoint + 1);
    IndexLookup.clear();
    IndexLookup.resize(max_codepoint + 1);
    for (int i = 0; i < max_codepoint + 1; i++)
    {
        IndexXAdvance[i] = -1.0f;
        IndexLookup[i] = -1;
    }
    for (int i = 0; i < Glyphs.Size; i++)
    {
        int codepoint = (int)Glyphs[i].Codepoint;
        IndexXAdvance[codepoint] = Glyphs[i].XAdvance;
        IndexLookup[codepoint] = i;
    }

    // Create a glyph to handle TAB
    // FIXME: Needs proper TAB handling but it needs to be contextualized (can arbitrary say that each string starts at "column 0"
    if (FindGlyph((unsigned short)' '))
    {
        if (Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times
            Glyphs.resize(Glyphs.Size + 1);
        ImFont::Glyph& tab_glyph = Glyphs.back();
        tab_glyph = *FindGlyph((unsigned short)' ');
        tab_glyph.Codepoint = '\t';
        tab_glyph.XAdvance *= 4;
        IndexXAdvance[(int)tab_glyph.Codepoint] = (float)tab_glyph.XAdvance;
        IndexLookup[(int)tab_glyph.Codepoint] = (int)(Glyphs.Size-1);
    }

    FallbackGlyph = NULL;
    FallbackGlyph = FindGlyph(FallbackChar);
    FallbackXAdvance = FallbackGlyph ? FallbackGlyph->XAdvance : 0.0f;
    for (int i = 0; i < max_codepoint + 1; i++)
        if (IndexXAdvance[i] < 0.0f)
            IndexXAdvance[i] = FallbackXAdvance;
}

void ImFont::SetFallbackChar(ImWchar c)
{
    FallbackChar = c;
    BuildLookupTable();
}

const ImFont::Glyph* ImFont::FindGlyph(unsigned short c) const
{
    if (c < IndexLookup.Size)
    {
        const int i = IndexLookup[c];
        if (i != -1)
            return &Glyphs[i];
    }
    return FallbackGlyph;
}

// Convert UTF-8 to 32-bits character, process single character input.
// Based on stb_from_utf8() from github.com/nothings/stb/
// We handle UTF-8 decoding error by skipping forward.
static int ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end)
{
    unsigned int c = (unsigned int)-1;
    const unsigned char* str = (const unsigned char*)in_text;
    if (!(*str & 0x80))
    {
        c = (unsigned int)(*str++);
        *out_char = c;
        return 1;
    }
    if ((*str & 0xe0) == 0xc0) 
    {
        *out_char = 0;
        if (in_text_end && in_text_end - (const char*)str < 2) return 0; 
        if (*str < 0xc2) return 0;
        c = (unsigned int)((*str++ & 0x1f) << 6);
        if ((*str & 0xc0) != 0x80) return 0;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 2;
    }
    if ((*str & 0xf0) == 0xe0) 
    {
        *out_char = 0;
        if (in_text_end && in_text_end - (const char*)str < 3) return 0;
        if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return 0;
        if (*str == 0xed && str[1] > 0x9f) return 0; // str[1] < 0x80 is checked below
        c = (unsigned int)((*str++ & 0x0f) << 12);
        if ((*str & 0xc0) != 0x80) return 0;
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 0;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 3;
    }
    if ((*str & 0xf8) == 0xf0) 
    {
        *out_char = 0;
        if (in_text_end && in_text_end - (const char*)str < 4) return 0;
        if (*str > 0xf4) return 0;
        if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return 0;
        if (*str == 0xf4 && str[1] > 0x8f) return 0; // str[1] < 0x80 is checked below
        c = (unsigned int)((*str++ & 0x07) << 18);
        if ((*str & 0xc0) != 0x80) return 0;
        c += (unsigned int)((*str++ & 0x3f) << 12);
        if ((*str & 0xc0) != 0x80) return 0;
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 0;
        c += (*str++ & 0x3f);
        // utf-8 encodings of values used in surrogate pairs are invalid
        if ((c & 0xFFFFF800) == 0xD800) return 0;
        *out_char = c;
        return 4;
    }
    *out_char = 0;
    return 0;
}

static int ImTextStrFromUtf8(ImWchar* buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_text_remaining)
{
    ImWchar* buf_out = buf;
    ImWchar* buf_end = buf + buf_size;
    while (buf_out < buf_end-1 && (!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c;
        in_text += ImTextCharFromUtf8(&c, in_text, in_text_end);
        if (c == 0)
            break;
        if (c < 0x10000)    // FIXME: Losing characters that don't fit in 2 bytes
            *buf_out++ = (ImWchar)c;
    }
    *buf_out = 0;
    if (in_text_remaining)
        *in_text_remaining = in_text;
    return (int)(buf_out - buf);
}

static int ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end)
{
    int char_count = 0;
    while ((!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c;
        in_text += ImTextCharFromUtf8(&c, in_text, in_text_end);
        if (c == 0)
            break;
        if (c < 0x10000)
            char_count++;
    }
    return char_count;
}

// Based on stb_to_utf8() from github.com/nothings/stb/
static inline int ImTextCharToUtf8(char* buf, int buf_size, unsigned int c)
{
    if (c < 0x80) 
    {
        buf[0] = (char)c;
        return 1;
    } 
    if (c < 0x800) 
    {
        if (buf_size < 2) return 0;
        buf[0] = (char)(0xc0 + (c >> 6));
        buf[1] = (char)(0x80 + (c & 0x3f));
        return 2;
    }
    if (c >= 0xdc00 && c < 0xe000)
    {
        return 0;
    } 
    if (c >= 0xd800 && c < 0xdc00) 
    {
        if (buf_size < 4) return 0;
        buf[0] = (char)(0xf0 + (c >> 18));
        buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
        buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
        buf[3] = (char)(0x80 + ((c ) & 0x3f));
        return 4;
    }
    //else if (c < 0x10000)
    {
        if (buf_size < 3) return 0;
        buf[0] = (char)(0xe0 + (c >> 12));
        buf[1] = (char)(0x80 + ((c>> 6) & 0x3f));
        buf[2] = (char)(0x80 + ((c ) & 0x3f));
        return 3;
    }
}

static inline int ImTextCountUtf8BytesFromChar(unsigned int c)
{
    if (c < 0x80) return 1;
    if (c < 0x800) return 2;
    if (c >= 0xdc00 && c < 0xe000) return 0;
    if (c >= 0xd800 && c < 0xdc00) return 4;
    return 3;
}

static int ImTextStrToUtf8(char* buf, int buf_size, const ImWchar* in_text, const ImWchar* in_text_end)
{
    char* buf_out = buf;
    const char* buf_end = buf + buf_size;
    while (buf_out < buf_end-1 && (!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c = (unsigned int)(*in_text++);
        if (c < 0x80)
            *buf_out++ = (char)c;
        else
            buf_out += ImTextCharToUtf8(buf_out, (int)(buf_end-buf_out-1), c);
    }
    *buf_out = 0;
    return (int)(buf_out - buf);
}

static int ImTextCountUtf8BytesFromStr(const ImWchar* in_text, const ImWchar* in_text_end)
{
    int bytes_count = 0;
    while ((!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c = (unsigned int)(*in_text++);
        if (c < 0x80)
            bytes_count++;
        else
            bytes_count += ImTextCountUtf8BytesFromChar(c);
    }
    return bytes_count;
}

const char* ImFont::CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const
{
    // Simple word-wrapping for English, not full-featured. Please submit failing cases!
    // FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

    // For references, possible wrap point marked with ^
    //  "aaa bbb, ccc,ddd. eee   fff. ggg!"
    //      ^    ^    ^   ^   ^__    ^    ^

    // List of hardcoded separators: .,;!?'"

    // Skip extra blanks after a line returns (that includes not counting them in width computation)
    // e.g. "Hello    world" --> "Hello" "World"

    // Cut words that cannot possibly fit within one line.
    // e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

    float line_width = 0.0f;
    float word_width = 0.0f;
    float blank_width = 0.0f;

    const char* word_end = text;
    const char* prev_word_end = NULL;
    bool inside_word = true;

    const char* s = text;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)*s;
        const char* next_s;
        if (c < 0x80)
            next_s = s + 1;
        else
            next_s = s + ImTextCharFromUtf8(&c, s, text_end);
        if (c == 0)
            break;

        if (c < 32)
        {
            if (c == '\n')
            {
                line_width = word_width = blank_width = 0.0f;
                inside_word = true;
                s = next_s;
                continue;
            }
            if (c == '\r')
            {
                s = next_s;
                continue;
            }
        }

        const float char_width = ((int)c < IndexXAdvance.Size) ? IndexXAdvance[(int)c] * scale : FallbackXAdvance;
        if (ImCharIsSpace(c))
        {
            if (inside_word)
            {
                line_width += blank_width;
                blank_width = 0.0f;
            }
            blank_width += char_width;
            inside_word = false;
        }
        else
        {
            word_width += char_width;
            if (inside_word)
            {
                word_end = next_s;
            }
            else
            {
                prev_word_end = word_end;
                line_width += word_width + blank_width;
                word_width = blank_width = 0.0f;
            }

            // Allow wrapping after punctuation.
            inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
        }

        // We ignore blank width at the end of the line (they can be skipped)
        if (line_width + word_width >= wrap_width)
        {
            // Words that cannot possibly fit within an entire line will be cut anywhere.
            if (word_width < wrap_width)
                s = prev_word_end ? prev_word_end : word_end;
            break;
        }

        s = next_s;
    }

    return s;
}

ImVec2 ImFont::CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

    const float line_height = size;
    const float scale = size / FontSize;

    ImVec2 text_size = ImVec2(0,0);
    float line_width = 0.0f;

    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    const char* s = text_begin;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                if (text_size.x < line_width)
                    text_size.x = line_width;
                text_size.y += line_height;
                line_width = 0.0f;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsSpace(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source (handle unlikely UTF-8 decoding failure by skipping to the next byte)
        const char* prev_s = s;
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        }
        else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0)
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                text_size.x = ImMax(text_size.x, line_width);
                text_size.y += line_height;
                line_width = 0.0f;
                continue;
            }
            if (c == '\r')
                continue;
        }

        const float char_width = ((int)c < IndexXAdvance.Size ? IndexXAdvance[(int)c] : FallbackXAdvance) * scale;
        if (line_width + char_width >= max_width)
        {
            s = prev_s;
            break;
        }

        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (line_width > 0 || text_size.y == 0.0f)
        text_size.y += line_height;

    if (remaining)
        *remaining = s;

    return text_size;
}

void ImFont::RenderText(float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, ImDrawList* draw_list, float wrap_width, bool cpu_fine_clip) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin);

    // Align to be pixel perfect
    pos.x = (float)(int)pos.x + DisplayOffset.x;
    pos.y = (float)(int)pos.y + DisplayOffset.y;
    float x = pos.x;
    float y = pos.y;
    if (y > clip_rect.w)
        return;

    const float scale = size / FontSize;
    const float line_height = FontSize * scale;
    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    ImDrawVert* vtx_write = draw_list->_VtxWritePtr;
    ImDrawIdx* idx_write = draw_list->_IdxWritePtr;
    unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

    const char* s = text_begin;
    if (!word_wrap_enabled && y + line_height < clip_rect.y)
        while (s < text_end && *s != '\n')  // Fast-forward to next line
            s++;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                x = pos.x;
                y += line_height;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsSpace(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source (handle unlikely UTF-8 decoding failure by skipping to the next byte)
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        }
        else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0)
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                x = pos.x;
                y += line_height;

                if (y > clip_rect.w)
                    break;
                if (!word_wrap_enabled && y + line_height < clip_rect.y)
                    while (s < text_end && *s != '\n')  // Fast-forward to next line
                        s++;
                continue;
            }
            if (c == '\r')
                continue;
        }

        float char_width = 0.0f;
        if (const Glyph* glyph = FindGlyph((unsigned short)c))
        {
            char_width = glyph->XAdvance * scale;

            // Clipping on Y is more likely
            if (c != ' ' && c != '\t')
            {
                // We don't do a second finer clipping test on the Y axis (todo: do some measurement see if it is worth it, probably not)
                float y1 = (float)(y + glyph->Y0 * scale);
                float y2 = (float)(y + glyph->Y1 * scale);

                float x1 = (float)(x + glyph->X0 * scale);
                float x2 = (float)(x + glyph->X1 * scale);
                if (x1 <= clip_rect.z && x2 >= clip_rect.x)
                {
                    // Render a character
                    float u1 = glyph->U0;
                    float v1 = glyph->V0;
                    float u2 = glyph->U1;
                    float v2 = glyph->V1;

                    // CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads
                    if (cpu_fine_clip)
                    {
                        if (x1 < clip_rect.x)
                        {
                            u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
                            x1 = clip_rect.x;
                        }
                        if (y1 < clip_rect.y)
                        {
                            v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
                            y1 = clip_rect.y;
                        }
                        if (x2 > clip_rect.z)
                        {
                            u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
                            x2 = clip_rect.z;
                        }
                        if (y2 > clip_rect.w)
                        {
                            v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
                            y2 = clip_rect.w;
                        }
                        if (y1 >= y2)
                        {
                            x += char_width;
                            continue;
                        }
                    }

                    // NB: we are not calling PrimRectUV() here because non-inlined causes too much overhead in a debug build.
                    // inlined:
                    {
                        idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx+1); idx_write[2] = (ImDrawIdx)(vtx_current_idx+2); 
                        idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx+2); idx_write[5] = (ImDrawIdx)(vtx_current_idx+3); 
                        vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
                        vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
                        vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
                        vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
                        vtx_write += 4;
                        vtx_current_idx += 4;
                        idx_write += 6;
                    }
                }
            }
        }

        x += char_width;
    }

    draw_list->_VtxWritePtr = vtx_write;
    draw_list->_VtxCurrentIdx = vtx_current_idx;
    draw_list->_IdxWritePtr = idx_write;
}

//-----------------------------------------------------------------------------
// PLATFORM DEPENDANT HELPERS
//-----------------------------------------------------------------------------

#if defined(_MSC_VER) && !defined(IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS)

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#pragma comment(lib, "user32")

// Win32 API clipboard implementation
static const char* GetClipboardTextFn_DefaultImpl()
{
    static char* buf_local = NULL;
    if (buf_local)
    {
        ImGui::MemFree(buf_local);
        buf_local = NULL;
    }
    if (!OpenClipboard(NULL)) 
        return NULL;
    HANDLE wbuf_handle = GetClipboardData(CF_UNICODETEXT); 
    if (wbuf_handle == NULL)
        return NULL;
    if (ImWchar* wbuf_global = (ImWchar*)GlobalLock(wbuf_handle))
    {
        int buf_len = ImTextCountUtf8BytesFromStr(wbuf_global, NULL) + 1;
        buf_local = (char*)ImGui::MemAlloc(buf_len * sizeof(char));
        ImTextStrToUtf8(buf_local, buf_len, wbuf_global, NULL);
    }
    GlobalUnlock(wbuf_handle); 
    CloseClipboard(); 
    return buf_local;
}

// Win32 API clipboard implementation
static void SetClipboardTextFn_DefaultImpl(const char* text)
{
    if (!OpenClipboard(NULL))
        return;

    const int wbuf_length = ImTextCountCharsFromUtf8(text, NULL) + 1;
    HGLOBAL wbuf_handle = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)wbuf_length * sizeof(ImWchar)); 
    if (wbuf_handle == NULL)
        return;
    ImWchar* wbuf_global = (ImWchar*)GlobalLock(wbuf_handle); 
    ImTextStrFromUtf8(wbuf_global, wbuf_length, text, NULL);
    GlobalUnlock(wbuf_handle); 
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, wbuf_handle);
    CloseClipboard();
}

#else

// Local ImGui-only clipboard implementation, if user hasn't defined better clipboard handlers
static const char* GetClipboardTextFn_DefaultImpl()
{
    return GImGui->PrivateClipboard;
}

// Local ImGui-only clipboard implementation, if user hasn't defined better clipboard handlers
static void SetClipboardTextFn_DefaultImpl(const char* text)
{
    ImGuiState& g = *GImGui;
    if (g.PrivateClipboard)
    {
        ImGui::MemFree(g.PrivateClipboard);
        g.PrivateClipboard = NULL;
    }
    const char* text_end = text + strlen(text);
    g.PrivateClipboard = (char*)ImGui::MemAlloc((size_t)(text_end - text) + 1);
    memcpy(g.PrivateClipboard, text, (size_t)(text_end - text));
    g.PrivateClipboard[(int)(text_end - text)] = 0;
}

#endif

#if defined(_MSC_VER) && !defined(IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCS)

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <Imm.h>
#pragma comment(lib, "imm32")

static void ImeSetInputScreenPosFn_DefaultImpl(int x, int y)
{
    // Notify OS Input Method Editor of text input position
    if (HWND hwnd = (HWND)GImGui->IO.ImeWindowHandle)
        if (HIMC himc = ImmGetContext(hwnd))
        {
            COMPOSITIONFORM cf;
            cf.ptCurrentPos.x = x;
            cf.ptCurrentPos.y = y;
            cf.dwStyle = CFS_FORCE_POSITION;
            ImmSetCompositionWindow(himc, &cf);
        }
}

#else

static void ImeSetInputScreenPosFn_DefaultImpl(int, int)
{
}

#endif

//-----------------------------------------------------------------------------
// HELP
//-----------------------------------------------------------------------------

void ImGui::ShowMetricsWindow(bool* opened)
{
    if (ImGui::Begin("ImGui Metrics", opened))
    {
        ImGui::Text("ImGui %s", ImGui::GetVersion());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("%d vertices, %d triangles", ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices / 3);
        ImGui::Text("%d allocations", ImGui::GetIO().MetricsAllocs);
        static bool show_clip_rects = true;
        ImGui::Checkbox("Show clipping rectangles when hovering ImDrawList", &show_clip_rects);
        ImGui::Separator();

        struct Funcs
        {
            static void NodeDrawList(ImDrawList* draw_list, const char* label)
            {
                bool node_opened = ImGui::TreeNode(draw_list, "%s: '%s' %d vtx, %d indices, %d cmds", label, draw_list->_OwnerName ? draw_list->_OwnerName : "", draw_list->VtxBuffer.Size, draw_list->IdxBuffer.Size, draw_list->CmdBuffer.Size);
                if (draw_list == ImGui::GetWindowDrawList())
                {
                    ImGui::SameLine();
                    ImGui::TextColored(ImColor(255,100,100), "CURRENTLY APPENDING"); // Can't display stats for active draw list! (we don't have the data double-buffered)
                }
                if (!node_opened)
                    return;

                int elem_offset = 0;
                for (const ImDrawCmd* pcmd = draw_list->CmdBuffer.begin(); pcmd < draw_list->CmdBuffer.end(); elem_offset += pcmd->ElemCount, pcmd++)
                {
                    if (pcmd->UserCallback)
                        ImGui::BulletText("Callback %p, user_data %p", pcmd->UserCallback, pcmd->UserCallbackData);
                    else
                    {
                        ImGui::BulletText("Draw %d indexed vtx, tex = %p, clip_rect = (%.0f,%.0f)..(%.0f,%.0f)", pcmd->ElemCount, pcmd->TextureId, pcmd->ClipRect.x, pcmd->ClipRect.y, pcmd->ClipRect.z, pcmd->ClipRect.w);
                        if (show_clip_rects && ImGui::IsItemHovered())
                        {
                            ImRect clip_rect = pcmd->ClipRect;
                            ImRect vtxs_rect;
                            for (int i = elem_offset; i < elem_offset + (int)pcmd->ElemCount; i++)
                                vtxs_rect.Add(draw_list->VtxBuffer[draw_list->IdxBuffer[i]].pos);
                            GImGui->OverlayDrawList.PushClipRectFullScreen();
                            clip_rect.Round(); GImGui->OverlayDrawList.AddRect(clip_rect.Min, clip_rect.Max, ImColor(255,255,0)); 
                            vtxs_rect.Round(); GImGui->OverlayDrawList.AddRect(vtxs_rect.Min, vtxs_rect.Max, ImColor(255,0,255)); 
                            GImGui->OverlayDrawList.PopClipRect();
                        }
                    }
                }
                ImGui::TreePop();
            }

            static void NodeWindows(ImVector<ImGuiWindow*>& windows, const char* label)
            {
                if (!ImGui::TreeNode(label, "%s (%d)", label, windows.Size))
                    return;
                for (int i = 0; i < windows.Size; i++)
                    Funcs::NodeWindow(windows[i], "Window");
                ImGui::TreePop();
            }

            static void NodeWindow(ImGuiWindow* window, const char* label)
            {
                if (!ImGui::TreeNode(window, "%s '%s', %d @ 0x%p", label, window->Name, window->Active || window->WasActive, window))
                    return;
                NodeDrawList(window->DrawList, "DrawList");
                if (window->RootWindow != window) NodeWindow(window->RootWindow, "RootWindow");
                if (window->DC.ChildWindows.Size > 0) NodeWindows(window->DC.ChildWindows, "ChildWindows");
                ImGui::BulletText("Storage: %d bytes", window->StateStorage.Data.Size * sizeof(ImGuiStorage::Pair));
                ImGui::TreePop();
            }
        };

        ImGuiState& g = *GImGui;                // Access private state
        g.DisableHideTextAfterDoubleHash++;     // Not exposed (yet). Disable processing that hides text after '##' markers.
        Funcs::NodeWindows(g.Windows, "Windows");
        if (ImGui::TreeNode("DrawList", "Active DrawLists (%d)", g.RenderDrawLists[0].Size))
        {
            for (int i = 0; i < g.RenderDrawLists[0].Size; i++)
                Funcs::NodeDrawList(g.RenderDrawLists[0][i], "DrawList");
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Popups", "Opened Popups Stack (%d)", g.OpenedPopupStack.Size))
        {
            for (int i = 0; i < g.OpenedPopupStack.Size; i++)
            {
                ImGuiWindow* window = g.OpenedPopupStack[i].Window;
                ImGui::BulletText("PopupID: %08x, Window: '%s'%s%s", g.OpenedPopupStack[i].PopupID, window ? window->Name : "NULL", window && (window->Flags & ImGuiWindowFlags_ChildWindow) ? " ChildWindow" : "", window && (window->Flags & ImGuiWindowFlags_ChildMenu) ? " ChildMenu" : "");
            }
            ImGui::TreePop();
        }
        g.DisableHideTextAfterDoubleHash--;
    }
    ImGui::End();
}

void ImGui::ShowUserGuide()
{
    ImGui::BulletText("Double-click on title bar to collapse window.");
    ImGui::BulletText("Click and drag on lower right corner to resize window.");
    ImGui::BulletText("Click and drag on any empty space to move window.");
    ImGui::BulletText("Mouse Wheel to scroll.");
    if (ImGui::GetIO().FontAllowUserScaling)
        ImGui::BulletText("CTRL+Mouse Wheel to zoom window contents.");
    ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
    ImGui::BulletText("CTRL+Click on a slider or drag box to input text.");
    ImGui::BulletText(
        "While editing text:\n"
        "- Hold SHIFT or use mouse to select text\n"
        "- CTRL+Left/Right to word jump\n"
        "- CTRL+A or double-click to select all\n"
        "- CTRL+X,CTRL+C,CTRL+V clipboard\n"
        "- CTRL+Z,CTRL+Y undo/redo\n"
        "- ESCAPE to revert\n"
        "- You can apply arithmetic operators +,*,/ on numerical values.\n"
        "  Use +- to subtract.\n");
}

void ImGui::ShowStyleEditor(ImGuiStyle* ref)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const ImGuiStyle def; // Default style
    if (ImGui::Button("Revert Style"))
        style = ref ? *ref : def;
    if (ref)
    {
        ImGui::SameLine();
        if (ImGui::Button("Save Style"))
            *ref = style;
    }

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.55f);

    if (ImGui::TreeNode("Rendering"))
    {
        ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
        ImGui::Checkbox("Anti-aliased shapes", &style.AntiAliasedShapes);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Sizes"))
    {
        ImGui::SliderFloat("Alpha", &style.Alpha, 0.20f, 1.0f, "%.2f");                 // Not exposing zero here so user doesn't "lose" the UI. But application code could have a toggle to switch between zero and non-zero.
        ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat("ChildWindowRounding", &style.ChildWindowRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
        ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarWidth", &style.ScrollbarWidth, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 16.0f, "%.0f");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Colors"))
    {
        static int output_dest = 0;
        static bool output_only_modified = false;
        if (ImGui::Button("Output Colors"))
        {
            if (output_dest == 0)
                ImGui::LogToClipboard();
            else
                ImGui::LogToTTY();
            ImGui::LogText("ImGuiStyle& style = ImGui::GetStyle();" IM_NEWLINE);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const ImVec4& col = style.Colors[i];
                const char* name = ImGui::GetStyleColName(i);
                if (!output_only_modified || memcmp(&col, (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
                    ImGui::LogText("style.Colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 22 - strlen(name), "", col.x, col.y, col.z, col.w);
            }
            ImGui::LogFinish();
        }
        ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY"); ImGui::PopItemWidth();
        ImGui::SameLine(); ImGui::Checkbox("Only Modified Fields", &output_only_modified);

        static ImGuiColorEditMode edit_mode = ImGuiColorEditMode_RGB;
        ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditMode_RGB);
        ImGui::SameLine();
        ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditMode_HSV);
        ImGui::SameLine();
        ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditMode_HEX);
        //ImGui::Text("Tip: Click on colored square to change edit mode.");

        static ImGuiTextFilter filter;
        filter.Draw("Filter colors", 200);

        ImGui::BeginChild("#colors", ImVec2(0, 300), true);
        ImGui::PushItemWidth(-160);
        ImGui::ColorEditMode(edit_mode);
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColName(i);
            if (!filter.PassFilter(name))
                continue;
            ImGui::PushID(i);
            ImGui::ColorEdit4(name, (float*)&style.Colors[i], true);
            if (memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
            {
                ImGui::SameLine(); if (ImGui::Button("Revert")) style.Colors[i] = ref ? ref->Colors[i] : def.Colors[i];
                if (ref) { ImGui::SameLine(); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i]; }
            }
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
        ImGui::EndChild();

        ImGui::TreePop();
    }

    ImGui::PopItemWidth();
}

//-----------------------------------------------------------------------------
// SAMPLE CODE
//-----------------------------------------------------------------------------

#ifndef IMGUI_DISABLE_TEST_WINDOWS

static void ShowExampleAppConsole(bool* opened);
static void ShowExampleAppLayout(bool* opened);
static void ShowExampleAppLongText(bool* opened);
static void ShowExampleAppAutoResize(bool* opened);
static void ShowExampleAppFixedOverlay(bool* opened);
static void ShowExampleAppManipulatingWindowTitle(bool* opened);
static void ShowExampleAppCustomRendering(bool* opened);
static void ShowExampleAppMainMenuBar();
static void ShowExampleMenuFile();

static void ShowHelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)"); 
    if (ImGui::IsItemHovered()) 
        ImGui::SetTooltip(desc);
}

// Demonstrate most ImGui features (big function!)
void ImGui::ShowTestWindow(bool* opened)
{
    // Examples apps
    static bool show_app_metrics = false;
    static bool show_app_main_menu_bar = false;
    static bool show_app_console = false;
    static bool show_app_layout = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_fixed_overlay = false;
    static bool show_app_custom_rendering = false;
    static bool show_app_manipulating_window_title = false;
    static bool show_app_about = false;
    if (show_app_metrics) ImGui::ShowMetricsWindow(&show_app_metrics);
    if (show_app_main_menu_bar) ShowExampleAppMainMenuBar();
    if (show_app_console) ShowExampleAppConsole(&show_app_console);
    if (show_app_layout) ShowExampleAppLayout(&show_app_layout);
    if (show_app_long_text) ShowExampleAppLongText(&show_app_long_text);
    if (show_app_auto_resize) ShowExampleAppAutoResize(&show_app_auto_resize);
    if (show_app_fixed_overlay) ShowExampleAppFixedOverlay(&show_app_fixed_overlay);
    if (show_app_manipulating_window_title) ShowExampleAppManipulatingWindowTitle(&show_app_manipulating_window_title);
    if (show_app_custom_rendering) ShowExampleAppCustomRendering(&show_app_custom_rendering);
    if (show_app_about)
    {
        ImGui::Begin("About ImGui", &show_app_about, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("ImGui %s", ImGui::GetVersion());
        ImGui::Separator();
        ImGui::Text("By Omar Cornut and all github contributors.");
        ImGui::Text("ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::End();
    }

    static bool no_titlebar = false;
    static bool no_border = true;
    static bool no_resize = false;
    static bool no_move = false;
    static bool no_scrollbar = false;
    static bool no_collapse = false;
    static bool no_menu = false;
    static float bg_alpha = 0.65f;

    // Demonstrate the various window flags. Typically you would just use the default.
    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (!no_border)   window_flags |= ImGuiWindowFlags_ShowBorders;
    if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
    if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
    if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
    if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("ImGui Test", opened, ImVec2(550,680), bg_alpha, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    
    //ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // 2/3 of the space for widget and 1/3 for labels
    ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels

    ImGui::Text("ImGui says hello.");

    // Menu
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples"))
        {
            ImGui::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
            ImGui::MenuItem("Console", NULL, &show_app_console);
            ImGui::MenuItem("Simple layout", NULL, &show_app_layout);
            ImGui::MenuItem("Long text display", NULL, &show_app_long_text);
            ImGui::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            ImGui::MenuItem("Simple overlay", NULL, &show_app_fixed_overlay);
            ImGui::MenuItem("Manipulating window title", NULL, &show_app_manipulating_window_title);
            ImGui::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
            ImGui::MenuItem("About ImGui", NULL, &show_app_about);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::TextWrapped("This window is being created by the ShowTestWindow() function. Please refer to the code for programming reference.\n\nUser Guide:");
        ImGui::ShowUserGuide();
    }

    if (ImGui::CollapsingHeader("Window options"))
    {
        ImGui::Checkbox("no titlebar", &no_titlebar); ImGui::SameLine(150);
        ImGui::Checkbox("no border", &no_border); ImGui::SameLine(300);
        ImGui::Checkbox("no resize", &no_resize); 
        ImGui::Checkbox("no move", &no_move); ImGui::SameLine(150);
        ImGui::Checkbox("no scrollbar", &no_scrollbar); ImGui::SameLine(300);
        ImGui::Checkbox("no collapse", &no_collapse);
        ImGui::Checkbox("no menu", &no_menu);
        ImGui::SliderFloat("bg alpha", &bg_alpha, 0.0f, 1.0f);

        if (ImGui::TreeNode("Style"))
        {
            ImGui::ShowStyleEditor();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Fonts", "Fonts (%d)", ImGui::GetIO().Fonts->Fonts.Size))
        {
            ImGui::TextWrapped("Tip: Load fonts with io.Fonts->AddFontFromFileTTF().");
            ImFontAtlas* atlas = ImGui::GetIO().Fonts;
            if (ImGui::TreeNode("Atlas texture"))
            {
                ImGui::Text("%dx%d pixels", atlas->TexWidth, atlas->TexHeight);
                ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
                ImGui::TreePop();
            }
            ImGui::PushItemWidth(100);
            for (int i = 0; i < atlas->Fonts.Size; i++)
            {
                ImFont* font = atlas->Fonts[i];
                ImGui::BulletText("Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
                ImGui::TreePush((void*)i);
                if (i > 0) { ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) { atlas->Fonts[i] = atlas->Fonts[0]; atlas->Fonts[0] = font; } }
                ImGui::PushFont(font);
                ImGui::Text("The quick brown fox jumps over the lazy dog");
                ImGui::PopFont();
                if (ImGui::TreeNode("Details"))
                {
                    ImGui::DragFloat("font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");             // scale only this font
                    ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                    ImGui::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                    for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
                        ImGui::BulletText("Input %d: \'%s\'", config_i, font->ConfigData[config_i].Name);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            static float window_scale = 1.0f;
            ImGui::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f");              // scale only this window
            ImGui::DragFloat("global scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f"); // scale everything
            ImGui::PopItemWidth();
            ImGui::SetWindowFontScale(window_scale);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Logging"))
        {
            ImGui::TextWrapped("The logging API redirects all text output so you can easily capture the content of a window or a block. Tree nodes can be automatically expanded. You can also call ImGui::LogText() to output directly to the log without a visual output.");
            ImGui::LogButtons();
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Widgets"))
    {
        if (ImGui::TreeNode("Tree"))
        {
            for (int i = 0; i < 5; i++)
            {
                if (ImGui::TreeNode((void*)i, "Child %d", i))
                {
                    ImGui::Text("blah blah");
                    ImGui::SameLine();
                    if (ImGui::SmallButton("print"))
                        printf("Child %d pressed", i);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Bullets"))
        {
            ImGui::BulletText("Bullet point 1");
            ImGui::BulletText("Bullet point 2\nOn multiple lines");
            ImGui::Bullet(); ImGui::Text("Bullet point 3 (two calls)");
            ImGui::Bullet(); ImGui::SmallButton("Button");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Colored Text"))
        {
            // Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
            ImGui::TextColored(ImVec4(1.0f,0.0f,1.0f,1.0f), "Pink");
            ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "Yellow");
            ImGui::TextDisabled("Disabled");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Word Wrapping"))
        {
            // Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
            ImGui::TextWrapped("This text should automatically wrap on the edge of the window. The current implementation for text wrapping follows simple rules suitable for English and possibly other languages.");
            ImGui::Spacing();

            static float wrap_width = 200.0f;
            ImGui::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");

            ImGui::Text("Test paragraph 1:");
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight()), 0xFFFF00FF);
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
            ImGui::Text("lazy dog. This paragraph is made to fit within %.0f pixels. The quick brown fox jumps over the lazy dog.", wrap_width);
            ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 0xFF00FFFF);
            ImGui::PopTextWrapPos();

            ImGui::Text("Test paragraph 2:");
            pos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight()), 0xFFFF00FF);
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
            ImGui::Text("aaaaaaaa bbbbbbbb, cccccccc,dddddddd. eeeeeeee   ffffffff. gggggggg!hhhhhhhh");
            ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 0xFF00FFFF);
            ImGui::PopTextWrapPos();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("UTF-8 Text"))
        {
            // UTF-8 test with Japanese characters
            // (needs a suitable font, try Arial Unicode or M+ fonts http://mplus-fonts.sourceforge.jp/mplus-outline-fonts/index-en.html)
            // Most compiler appears to support UTF-8 in source code (with Visual Studio you need to save your file as 'UTF-8 without signature')
            // However for the sake for maximum portability here we are *not* including raw UTF-8 character in this source file, instead we encode the string with hexadecimal constants.
            // In your own application be reasonable and use UTF-8 in source or retrieve the data from file system!
            // Note that characters values are preserved even if the font cannot be displayed, so you can safely copy & paste garbled characters into another application.
            ImGui::TextWrapped("CJK text will only appears if the font was loaded with the appropriate CJK character ranges. Call io.Font->LoadFromFileTTF() manually to load extra character ranges.");
            ImGui::Text("Hiragana: \xe3\x81\x8b\xe3\x81\x8d\xe3\x81\x8f\xe3\x81\x91\xe3\x81\x93 (kakikukeko)");
            ImGui::Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
            static char buf[32] = "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
            ImGui::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Images"))
        {
            ImGui::TextWrapped("Below we are displaying the font texture (which is the only texture we have access to in this demo). Use the 'ImTextureID' type as storage to pass pointers or identifier to your own texture data. Hover the texture for a zoomed view!");
            ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
            float tex_w = (float)ImGui::GetIO().Fonts->TexWidth;
            float tex_h = (float)ImGui::GetIO().Fonts->TexHeight;
            ImTextureID tex_id = ImGui::GetIO().Fonts->TexID;
            ImGui::Text("%.0fx%.0f", tex_w, tex_h);
            ImGui::Image(tex_id, ImVec2(tex_w, tex_h), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                float focus_sz = 32.0f;
                float focus_x = ImClamp(ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f, 0.0f, tex_w - focus_sz);
                float focus_y = ImClamp(ImGui::GetMousePos().y - tex_screen_pos.y - focus_sz * 0.5f, 0.0f, tex_h - focus_sz);
                ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
                ImGui::Text("Max: (%.2f, %.2f)", focus_x + focus_sz, focus_y + focus_sz);
                ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
                ImVec2 uv1 = ImVec2((focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
                ImGui::Image(tex_id, ImVec2(128,128), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
                ImGui::EndTooltip();
            }
            ImGui::TextWrapped("And now some textured buttons..");
            static int pressed_count = 0;
            for (int i = 0; i < 8; i++)
            {
                if (i > 0)
                    ImGui::SameLine();
                ImGui::PushID(i);
                int frame_padding = -1 + i;     // -1 = uses default padding
                if (ImGui::ImageButton(tex_id, ImVec2(32,32), ImVec2(0,0), ImVec2(32.0f/tex_w,32/tex_h), frame_padding, ImColor(0,0,0,255)))
                    pressed_count += 1;
                ImGui::PopID();
            }
            ImGui::Text("Pressed %d times.", pressed_count);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Selectables"))
        {
            if (ImGui::TreeNode("Basic"))
            {
                static bool selected[3] = { false, true, false };
                ImGui::Selectable("1. I am selectable", &selected[0]);
                ImGui::Selectable("2. I am selectable", &selected[1]);
                ImGui::Text("3. I am not selectable");
                ImGui::Selectable("4. I am selectable", &selected[2]);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Rendering more text into the same block"))
            {
                static bool selected[3] = { false, false, false };
                ImGui::Selectable("main.c", &selected[0]);    ImGui::SameLine(300); ImGui::Text(" 2,345 bytes");
                ImGui::Selectable("Hello.cpp", &selected[1]); ImGui::SameLine(300); ImGui::Text("12,345 bytes");
                ImGui::Selectable("Hello.h", &selected[2]);   ImGui::SameLine(300); ImGui::Text(" 2,345 bytes");
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Grid"))
            {
                static bool selected[16] = { true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true };
                for (int i = 0; i < 16; i++)
                {
                    ImGui::PushID(i);
                    if (ImGui::Selectable("Me", &selected[i], 0, ImVec2(50,50)))
                    {
                        int x = i % 4, y = i / 4;
                        if (x > 0) selected[i - 1] ^= 1;
                        if (x < 3) selected[i + 1] ^= 1;
                        if (y > 0) selected[i - 4] ^= 1;
                        if (y < 3) selected[i + 4] ^= 1;
                    }
                    if ((i % 4) < 3) ImGui::SameLine();
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Filtered Text Input"))
        {
            static char buf1[64] = ""; ImGui::InputText("default", buf1, 64);
            static char buf2[64] = ""; ImGui::InputText("decimal", buf2, 64, ImGuiInputTextFlags_CharsDecimal);
            static char buf3[64] = ""; ImGui::InputText("hexadecimal", buf3, 64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
            static char buf4[64] = ""; ImGui::InputText("uppercase", buf4, 64, ImGuiInputTextFlags_CharsUppercase);
            static char buf5[64] = ""; ImGui::InputText("no blank", buf5, 64, ImGuiInputTextFlags_CharsNoBlank);
            struct TextFilters { static int FilterImGuiLetters(ImGuiTextEditCallbackData* data) { if (data->EventChar < 256 && strchr("imgui", (char)data->EventChar)) return 0; return 1; } };
            static char buf6[64] = ""; ImGui::InputText("\"imgui\" letters", buf6, 64, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterImGuiLetters);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Multi-line Text Input"))
        {
            static char text[1024*16] = "// F00F bug\nlabel:\n\tlock cmpxchg8b eax\n";
            ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
            ImGui::TreePop();
        }

        static bool a=false;
        if (ImGui::Button("Button")) { printf("Clicked\n"); a ^= 1; }
        if (a)
        {
            ImGui::SameLine(); 
            ImGui::Text("Thanks for clicking me!");
        }

        static bool check = true;
        ImGui::Checkbox("checkbox", &check);

        static int e = 0;
        ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
        ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
        ImGui::RadioButton("radio c", &e, 2);

        // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        for (int i = 0; i < 7; i++)
        {
            if (i > 0) ImGui::SameLine();
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(i/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(i/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(i/7.0f, 0.8f, 0.8f));
            ImGui::Button("Click");
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

        ImGui::Text("Hover over me");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("I am a tooltip");

        ImGui::SameLine();
        ImGui::Text("- or me");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("I am a fancy tooltip");
            static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
            ImGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
            ImGui::EndTooltip();
        }

        // Testing IMGUI_ONCE_UPON_A_FRAME macro
        //for (int i = 0; i < 5; i++)
        //{
        //  IMGUI_ONCE_UPON_A_FRAME
        //  {
        //      ImGui::Text("This will be displayed only once.");
        //  }
        //}

        ImGui::Separator();

        ImGui::LabelText("label", "Value");

        static int item = 1;
        ImGui::Combo("combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");   // Combo using values packed in a single constant string (for really quick combo)

        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK" };
        static int item2 = -1;
        ImGui::Combo("combo scroll", &item2, items, IM_ARRAYSIZE(items));   // Combo using proper array. You can also pass a callback to retrieve array value, no need to create/copy an array just for that.

        {
            static char str0[128] = "Hello, world!";
            static int i0=123;
            static float f0=0.001f;
            ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));
            ImGui::SameLine(); ShowHelpMarker("Hold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n");

            ImGui::InputInt("input int", &i0);
            ImGui::SameLine(); ShowHelpMarker("You can apply arithmetic operators +,*,/ on numerical values.\n  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\nUse +- to subtract.\n");

            ImGui::InputFloat("input float", &f0, 0.01f, 1.0f);

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            ImGui::InputFloat3("input float3", vec4a);
        }

        {
            static int i1=50, i2=42;
            ImGui::DragInt("drag int", &i1, 1);
            ImGui::SameLine(); ShowHelpMarker("Click and drag to edit value.\nHold SHIFT/ALT for faster/slower edit.\nDouble-click or CTRL+click to input value.");

            ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%.0f%%");

            static float f1=1.00f, f2=0.0067f;
            ImGui::DragFloat("drag float", &f1, 0.005f);
            ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
        }

        {
            static int i1=0;
            ImGui::SliderInt("slider int", &i1, -1, 3);
            ImGui::SameLine(); ShowHelpMarker("CTRL+click to input value.");

            static float f1=0.123f, f2=0.0f;
            ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            ImGui::SliderFloat("slider log float", &f2, -10.0f, 10.0f, "%.4f", 3.0f);
            static float angle = 0.0f;
            ImGui::SliderAngle("slider angle", &angle);
        }

        static float col1[3] = { 1.0f,0.0f,0.2f };
        static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
        ImGui::ColorEdit3("color 1", col1);
        ImGui::SameLine(); ShowHelpMarker("Click on the colored square to change edit mode.\nCTRL+click on individual component to input value.\n");
        
        ImGui::ColorEdit4("color 2", col2);

        const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
        static int listbox_item_current = 1;
        ImGui::ListBox("listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

        //static int listbox_item_current2 = 2;
        //ImGui::PushItemWidth(-1);
        //ImGui::ListBox("##listbox2", &listbox_item_current2, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
        //ImGui::PopItemWidth();

        if (ImGui::TreeNode("Range Widgets"))
        {
            ImGui::Unindent();

            static float begin = 10, end = 90;
            static int begin_i = 100, end_i = 1000;
            ImGui::DragFloatRange2("range", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");
            ImGui::DragIntRange2("range int (no bounds)", &begin_i, &end_i, 5, 0, 0, "Min: %.0f units", "Max: %.0f units");

            ImGui::Indent();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Multi-component Widgets"))
        {
            ImGui::Unindent();

            static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            static int vec4i[4] = { 1, 5, 100, 255 };

            ImGui::InputFloat2("input float2", vec4f);
            ImGui::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
            ImGui::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
            ImGui::DragInt2("drag int2", vec4i, 1, 0, 255);
            ImGui::InputInt2("input int2", vec4i);
            ImGui::SliderInt2("slider int2", vec4i, 0, 255);
            ImGui::Spacing();

            ImGui::InputFloat3("input float3", vec4f);
            ImGui::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
            ImGui::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
            ImGui::DragInt3("drag int3", vec4i, 1, 0, 255);
            ImGui::InputInt3("input int3", vec4i);
            ImGui::SliderInt3("slider int3", vec4i, 0, 255);
            ImGui::Spacing();

            ImGui::InputFloat4("input float4", vec4f);
            ImGui::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
            ImGui::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
            ImGui::InputInt4("input int4", vec4i);
            ImGui::DragInt4("drag int4", vec4i, 1, 0, 255);
            ImGui::SliderInt4("slider int4", vec4i, 0, 255);

            ImGui::Indent();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Vertical Sliders"))
        {
            ImGui::Unindent();
            const float spacing = 4;
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

            static int int_value = 0;
            ImGui::VSliderInt("##int", ImVec2(18,160), &int_value, 0, 5);
            ImGui::SameLine();

            static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
            ImGui::PushID("set1");
            for (int i = 0; i < 7; i++)
            {
                if (i > 0) ImGui::SameLine();
                ImGui::PushID(i);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(i/7.0f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(i/7.0f, 0.6f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(i/7.0f, 0.7f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(i/7.0f, 0.9f, 0.9f));
                ImGui::VSliderFloat("##v", ImVec2(18,160), &values[i], 0.0f, 1.0f, "");
                if (ImGui::IsItemActive() || ImGui::IsItemHovered())
                    ImGui::SetTooltip("%.3f", values[i]);
                ImGui::PopStyleColor(4);
                ImGui::PopID();
            }
            ImGui::PopID();

            ImGui::SameLine();
            ImGui::PushID("set2");
            static float values2[4] = { 0.20f, 0.80f, 0.40f, 0.25f };
            const int rows = 3;
            const ImVec2 small_slider_size(18, (160.0f-(rows-1)*spacing)/rows);
            for (int nx = 0; nx < 4; nx++)
            {
                if (nx > 0) ImGui::SameLine();
                ImGui::BeginGroup();
                for (int ny = 0; ny < rows; ny++)
                {
                    ImGui::PushID(nx*rows+ny);
                    ImGui::VSliderFloat("##v", small_slider_size, &values2[nx], 0.0f, 1.0f, "");
                    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
                        ImGui::SetTooltip("%.3f", values2[nx]);
                    ImGui::PopID();
                }
                ImGui::EndGroup();
            }
            ImGui::PopID();

            ImGui::SameLine();
            ImGui::PushID("set3");
            for (int i = 0; i < 4; i++)
            {
                if (i > 0) ImGui::SameLine();
                ImGui::PushID(i);
                ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 40);
                ImGui::VSliderFloat("##v", ImVec2(40,160), &values[i], 0.0f, 1.0f, "%.2f");
                ImGui::PopStyleVar();
                ImGui::PopID();
            }
            ImGui::PopID();
            ImGui::PopStyleVar();

            ImGui::Indent();
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Graphs widgets"))
    {
        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

        static bool pause;
        static ImVector<float> values; if (values.empty()) { values.resize(90); memset(values.Data, 0, values.Size*sizeof(float)); } 
        static int values_offset = 0; 
        if (!pause) 
        {
            static float refresh_time = ImGui::GetTime(); // Create dummy data at fixed 60 hz rate for the demo
            for (; ImGui::GetTime() > refresh_time + 1.0f/60.0f; refresh_time += 1.0f/60.0f)
            {
                static float phase = 0.0f;
                values[values_offset] = cosf(phase); 
                values_offset = (values_offset+1)%values.Size; 
                phase += 0.10f*values_offset; 
            }
        }
        ImGui::PlotLines("##Graph", values.Data, values.Size, values_offset, "avg 0.0", -1.0f, 1.0f, ImVec2(0,80));
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x); 
        ImGui::BeginGroup();
        ImGui::Text("Graph");
        ImGui::Checkbox("pause", &pause);
        ImGui::EndGroup();
        ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0,80));
    }

    if (ImGui::CollapsingHeader("Layout"))
    {
        if (ImGui::TreeNode("Child regions"))
        {
            ImGui::Text("Without border");
            static int line = 50;
            bool goto_line = ImGui::Button("Goto");
            ImGui::SameLine(); 
            ImGui::PushItemWidth(100);
            goto_line |= ImGui::InputInt("##Line", &line, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();
            ImGui::BeginChild("Sub1", ImVec2(ImGui::GetWindowWidth() * 0.5f,300));
            for (int i = 0; i < 100; i++)
            {
                ImGui::Text("%04d: scrollable region", i);
                if (goto_line && line == i)
                    ImGui::SetScrollHere();
            }
            if (goto_line && line >= 100)
                ImGui::SetScrollHere();
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
            ImGui::BeginChild("Sub2", ImVec2(0,300), true);
            ImGui::Text("With border");
            ImGui::Columns(2);
            for (int i = 0; i < 100; i++)
            {
                if (i == 50)
                    ImGui::NextColumn();
                char buf[32];
                sprintf(buf, "%08x", i*5731);
                ImGui::Button(buf, ImVec2(ImGui::GetContentRegionMax().x - ImGui::GetCursorPosX(), 0.0f));
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Widgets Alignment"))
        {
            static float f = 0.0f;
            ImGui::Text("Fixed: 100 pixels");
            ImGui::PushItemWidth(100);
            ImGui::InputFloat("float##1", &f);
            ImGui::PopItemWidth();

            ImGui::Text("Proportional: 50%% of window width");
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::InputFloat("float##2", &f);
            ImGui::PopItemWidth();

            ImGui::Text("Right-aligned: Leave 100 pixels for label");
            ImGui::PushItemWidth(-100);
            ImGui::InputFloat("float##3", &f);
            ImGui::PopItemWidth();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Basic Horizontal Layout"))
        {
            ImGui::TextWrapped("(Use ImGui::SameLine() to keep adding items to the right of the preceeding item)");

            // Text
            ImGui::Text("Two items: Hello"); ImGui::SameLine();
            ImGui::TextColored(ImVec4(1,1,0,1), "Sailor");

            // Adjust spacing
            ImGui::Text("More spacing: Hello"); ImGui::SameLine(0, 20);
            ImGui::TextColored(ImVec4(1,1,0,1), "Sailor");

            // Button
            ImGui::AlignFirstTextHeightToWidgets();
            ImGui::Text("Normal buttons"); ImGui::SameLine();
            ImGui::Button("Banana"); ImGui::SameLine();
            ImGui::Button("Apple"); ImGui::SameLine();
            ImGui::Button("Corniflower");

            // Button
            ImGui::Text("Small buttons"); ImGui::SameLine();
            ImGui::SmallButton("Like this one"); ImGui::SameLine();
            ImGui::Text("can fit within a text block.");

            // Aligned to arbitrary position. Easy/cheap column.
            ImGui::Text("Aligned"); 
            ImGui::SameLine(150); ImGui::Text("x=150");
            ImGui::SameLine(300); ImGui::Text("x=300");
            ImGui::Text("Aligned");
            ImGui::SameLine(150); ImGui::SmallButton("x=150");
            ImGui::SameLine(300); ImGui::SmallButton("x=300");

            // Checkbox
            static bool c1=false,c2=false,c3=false,c4=false;
            ImGui::Checkbox("My", &c1); ImGui::SameLine();
            ImGui::Checkbox("Tailor", &c2); ImGui::SameLine();
            ImGui::Checkbox("Is", &c3); ImGui::SameLine();
            ImGui::Checkbox("Rich", &c4);

            // Various
            static float f0=1.0f, f1=2.0f, f2=3.0f;
            ImGui::PushItemWidth(80);
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
            static int item = -1;
            ImGui::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); ImGui::SameLine();
            ImGui::SliderFloat("X", &f0, 0.0f,5.0f); ImGui::SameLine();
            ImGui::SliderFloat("Y", &f1, 0.0f,5.0f); ImGui::SameLine();
            ImGui::SliderFloat("Z", &f2, 0.0f,5.0f); 
            ImGui::PopItemWidth();

            ImGui::PushItemWidth(80);
            ImGui::Text("Lists:");
            static int selection[4] = { 0, 1, 2, 3 };
            for (int i = 0; i < 4; i++)
            {
                if (i > 0) ImGui::SameLine();
                ImGui::PushID(i);
                ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
                ImGui::PopID();
                //if (ImGui::IsItemHovered()) ImGui::SetTooltip("ListBox %d hovered", i); 
            }
            ImGui::PopItemWidth();

            // Dummy
            ImVec2 sz(30,30);
            ImGui::Button("A", sz); ImGui::SameLine();
            ImGui::Dummy(sz); ImGui::SameLine();
            ImGui::Button("B", sz);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Groups"))
        {
            ImGui::TextWrapped("(Using ImGui::BeginGroup()/EndGroup() to layout items)");
            ImGui::BeginGroup();
            {
                ImGui::BeginGroup();
                ImGui::Button("AAA");
                ImGui::SameLine();
                ImGui::Button("BBB");
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Button("CCC");
                ImGui::Button("DDD");
                ImGui::EndGroup();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Group hovered");
                ImGui::SameLine();
                ImGui::Button("EEE");
                ImGui::EndGroup();
            }
            // Capture the group size and create widgets using the same size
            ImVec2 size = ImGui::GetItemRectSize();
            const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
            ImGui::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

            ImGui::Button("ACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x)*0.5f,size.y));
            ImGui::SameLine();
            ImGui::Button("REACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x)*0.5f,size.y));
            ImGui::EndGroup();
            ImGui::SameLine();

            ImGui::Button("LEVERAGE\nBUZZWORD", size);
            ImGui::SameLine();

            ImGui::ListBoxHeader("List", size);
            ImGui::Selectable("Selected", true);
            ImGui::Selectable("Not Selected", false);
            ImGui::ListBoxFooter();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Text Baseline Alignment"))
        {
            ImGui::TextWrapped("(This is testing the vertical alignment that occurs on text to keep it at the same baseline as widgets. Lines only composed of text or \"small\" widgets fit in less vertical spaces than lines with normal widgets)"); 

            ImGui::Text("One\nTwo\nThree"); ImGui::SameLine();            
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Text("Banana"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("One\nTwo\nThree");

            ImGui::Button("HOP"); ImGui::SameLine();
            ImGui::Text("Banana"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Button("HOP"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Button("TEST"); ImGui::SameLine();
            ImGui::Text("TEST"); ImGui::SameLine();
            ImGui::SmallButton("TEST");

            ImGui::AlignFirstTextHeightToWidgets(); // If your line starts with text, call this to align it to upcoming widgets.
            ImGui::Text("Text aligned to Widget"); ImGui::SameLine();
            ImGui::Button("Widget"); ImGui::SameLine();
            ImGui::Text("Widget"); ImGui::SameLine();
            ImGui::SmallButton("Widget");

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Scrolling"))
        {
            ImGui::TextWrapped("Use SetScrollHere() or SetScrollFromPosY() to scroll to a given position.");
            static bool track = true;
            static int track_line = 50, scroll_to_px = 200;
            ImGui::Checkbox("Track", &track);
            ImGui::SameLine(130); track |= ImGui::DragInt("##line", &track_line, 0.25f, 0, 9999, "Line %.0f");
            bool scroll_to = ImGui::Button("Scroll To");
            ImGui::SameLine(130); scroll_to |= ImGui::DragInt("##pos_y", &scroll_to_px, 1.00f, 0, 9999, "y = %.0f px");
            if (scroll_to) track = false;

            for (int i = 0; i < 5; i++)
            {
                if (i > 0) ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("%s", i == 0 ? "Top" : i == 1 ? "25%" : i == 2 ? "Center" : i == 3 ? "75%" : "Bottom");
                ImGui::BeginChild(ImGui::GetID((void *)(intptr_t)i), ImVec2(ImGui::GetWindowWidth() * 0.17f, 200.0f), true);
                if (scroll_to)
                    ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + scroll_to_px, i * 0.25f);
                for (int line = 0; line < 100; line++)
                {
                    if (track && line == track_line)
                    {
                        ImGui::TextColored(ImColor(255,255,0), "Line %d", line);
                        ImGui::SetScrollHere(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
                    }
                    else
                    {
                        ImGui::Text("Line %d", line);
                    }
                }
                ImGui::EndChild();
                ImGui::EndGroup();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Clipping"))
        {
            static ImVec2 size(100, 100), offset(50, 20);
            ImGui::TextWrapped("On a per-widget basis we are occasionally clipping text CPU-side if it won't fit in its frame. Otherwise we are doing coarser clipping + passing a scissor rectangle to the renderer. The system is designed to try minimizing both execution and CPU/GPU rendering cost.");
            ImGui::DragFloat2("size", (float*)&size, 0.5f, 0.0f, 200.0f, "%.0f");
            ImGui::DragFloat2("offset", (float*)&offset, 0.5f, -200, 200.0f, "%.0f");
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec4 clip_rect(pos.x, pos.y, pos.x+size.x, pos.y+size.y);
            ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x+size.x,pos.y+size.y), ImColor(90,90,120,255));
            ImGui::GetWindowDrawList()->AddText(ImGui::GetWindowFont(), ImGui::GetWindowFontSize()*2.0f, ImVec2(pos.x+offset.x,pos.y+offset.y), ImColor(255,255,255,255), "Line 1 hello\nLine 2 clip me!", NULL, 0.0f, &clip_rect);
            ImGui::Dummy(size);
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Popups & Modal windows"))
    {
        if (ImGui::TreeNode("Popups"))
        {
            ImGui::TextWrapped("When a popup is active, it inhibits interacting with windows that are behind the popup. Clicking outside the popup closes it.");

            static int selected_fish = -1;
            const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
            static bool toggles[] = { true, false, false, false, false };

            if (ImGui::Button("Select.."))
                ImGui::OpenPopup("select");
            ImGui::SameLine();
            ImGui::Text(selected_fish == -1 ? "<None>" : names[selected_fish]);
            if (ImGui::BeginPopup("select"))
            {
                ImGui::Text("Aquarium");
                ImGui::Separator();
                for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                    if (ImGui::Selectable(names[i]))
                        selected_fish = i;
                ImGui::EndPopup();
            }

            if (ImGui::Button("Toggle.."))
                ImGui::OpenPopup("toggle");
            if (ImGui::BeginPopup("toggle"))
            {
                for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                    ImGui::MenuItem(names[i], "", &toggles[i]);
                if (ImGui::BeginMenu("Sub-menu"))
                {
                    ImGui::MenuItem("Click me");
                    ImGui::EndMenu();
                }

                ImGui::Separator();
                ImGui::Text("Tooltip here");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("I am a tooltip over a popup");

                if (ImGui::Button("Stacked Popup"))
                    ImGui::OpenPopup("another popup");
                if (ImGui::BeginPopup("another popup"))
                {
                    for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                        ImGui::MenuItem(names[i], "", &toggles[i]);
                    if (ImGui::BeginMenu("Sub-menu"))
                    {
                        ImGui::MenuItem("Click me");
                        ImGui::EndMenu();
                    }
                    ImGui::EndPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::Button("Popup Menu.."))
                ImGui::OpenPopup("popup from button");
            if (ImGui::BeginPopup("popup from button"))
            {
                ShowExampleMenuFile();
                ImGui::EndPopup();
            }

            ImGui::TextWrapped("Below we are testing adding menu items to a regular window. It's rather unusual but should work!");
            ImGui::Separator();
            ImGui::MenuItem("Menu item", "CTRL+M");
            if (ImGui::BeginMenu("Menu"))
            {
                ShowExampleMenuFile();
                ImGui::EndMenu();
            }
            ImGui::Separator();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Context menus"))
        {
            static float value = 0.5f;
            ImGui::Text("Value = %.3f (<-- right-click here)", value);
            if (ImGui::BeginPopupContextItem("item context menu"))
            {
                if (ImGui::Selectable("Set to zero")) value = 0.0f; 
                if (ImGui::Selectable("Set to PI")) value = 3.1415f; 
                ImGui::EndPopup();
            }

            static ImVec4 color = ImColor(1.0f, 0.0f, 1.0f, 1.0f);
            ImGui::ColorButton(color);
            if (ImGui::BeginPopupContextItem("color context menu"))
            {
                ImGui::Text("Edit color");
                ImGui::ColorEdit3("##edit", (float*)&color);
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            ImGui::SameLine(); ImGui::Text("(<-- right-click here)");

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Modals"))
        {  
            ImGui::TextWrapped("Modal windows are like popups but the user cannot close them by clicking outside the window.");

            if (ImGui::Button("Delete.."))
                ImGui::OpenPopup("Delete?");
            if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
                ImGui::Separator();
                
                static bool dont_ask_me_next_time = false;
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
                ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
                ImGui::PopStyleVar();

                if (ImGui::Button("OK", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            if (ImGui::Button("Stacked modals.."))
                ImGui::OpenPopup("Stacked 1");
            if (ImGui::BeginPopupModal("Stacked 1"))
            {
                ImGui::Text("Hello from Stacked The First");

                if (ImGui::Button("Another one.."))
                    ImGui::OpenPopup("Stacked 2");
                if (ImGui::BeginPopupModal("Stacked 2"))
                {
                    ImGui::Text("Hello from Stacked The Second");
                    if (ImGui::Button("Close"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }

                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Columns"))
    {
        // Basic columns
        ImGui::Text("Basic:");
        ImGui::Columns(4, "mycolumns");
        ImGui::Separator();
        ImGui::Text("ID"); ImGui::NextColumn();
        ImGui::Text("Name"); ImGui::NextColumn();
        ImGui::Text("Path"); ImGui::NextColumn();
        ImGui::Text("Flags"); ImGui::NextColumn();
        ImGui::Separator();
        const char* names[3] = { "One", "Two", "Three" };
        const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
        static int selected = -1;
        for (int i = 0; i < 3; i++)
        {
            char label[32];
            sprintf(label, "%04d", i);
            if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                selected = i;
            ImGui::NextColumn();
            ImGui::Text(names[i]); ImGui::NextColumn();
            ImGui::Text(paths[i]); ImGui::NextColumn(); 
            ImGui::Text("...."); ImGui::NextColumn();
        }
        ImGui::Columns(1);

        ImGui::Separator();
        ImGui::Spacing();

        // Scrolling columns
        /*
        ImGui::Text("Scrolling:");
        ImGui::BeginChild("##header", ImVec2(0, ImGui::GetTextLineHeightWithSpacing()+ImGui::GetStyle().ItemSpacing.y));
        ImGui::Columns(3);
        ImGui::Text("ID"); ImGui::NextColumn();
        ImGui::Text("Name"); ImGui::NextColumn();
        ImGui::Text("Path"); ImGui::NextColumn();
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::EndChild();
        ImGui::BeginChild("##scrollingregion", ImVec2(0, 60));
        ImGui::Columns(3);
        for (int i = 0; i < 10; i++)
        {
            ImGui::Text("%04d", i); ImGui::NextColumn();
            ImGui::Text("Foobar"); ImGui::NextColumn();
            ImGui::Text("/path/foobar/%04d/", i); ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Spacing();
        */

        // Create multiple items in a same cell before switching to next column
        ImGui::Text("Mixed items:");
        ImGui::Columns(3, "mixed");
        ImGui::Separator();

        static int e = 0;
        ImGui::Text("Hello"); 
        ImGui::Button("Banana");
        ImGui::RadioButton("radio a", &e, 0); 
        ImGui::NextColumn();

        ImGui::Text("ImGui"); 
        ImGui::Button("Apple");
        ImGui::RadioButton("radio b", &e, 1);
        static float foo = 1.0f;
        ImGui::InputFloat("red", &foo, 0.05f, 0, 3); 
        ImGui::Text("An extra line here.");
        ImGui::NextColumn();
        
        ImGui::Text("Sailor");
        ImGui::Button("Corniflower");
        ImGui::RadioButton("radio c", &e, 2);
        static float bar = 1.0f;
        ImGui::InputFloat("blue", &bar, 0.05f, 0, 3); 
        ImGui::NextColumn();

        if (ImGui::CollapsingHeader("Category A")) ImGui::Text("Blah blah blah"); ImGui::NextColumn();
        if (ImGui::CollapsingHeader("Category B")) ImGui::Text("Blah blah blah"); ImGui::NextColumn();
        if (ImGui::CollapsingHeader("Category C")) ImGui::Text("Blah blah blah"); ImGui::NextColumn();
        ImGui::Columns(1);

        ImGui::Separator();
        ImGui::Spacing();

        // Tree items
        ImGui::Text("Tree items:");
        ImGui::Columns(2, "tree items");
        ImGui::Separator();
        if (ImGui::TreeNode("Hello")) { ImGui::BulletText("Sailor"); ImGui::TreePop(); } ImGui::NextColumn();
        if (ImGui::TreeNode("Bonjour")) { ImGui::BulletText("Marin"); ImGui::TreePop(); } ImGui::NextColumn();
        ImGui::Columns(1);

        ImGui::Separator();
        ImGui::Spacing();

        // Word-wrapping
        ImGui::Text("Word-wrapping:");
        ImGui::Columns(2, "word-wrapping");
        ImGui::Separator();
        ImGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
        ImGui::Text("Hello Left");
        ImGui::NextColumn();
        ImGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
        ImGui::Text("Hello Right");
        ImGui::Columns(1);

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::TreeNode("Inside a tree.."))
        {
            if (ImGui::TreeNode("node 1 (with borders)"))
            {
                ImGui::Columns(4);
                for (int i = 0; i < 8; i++)
                {
                    ImGui::Text("%c%c%c", 'a'+i, 'a'+i, 'a'+i);
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("node 2 (without borders)"))
            {
                ImGui::Columns(4, NULL, false);
                for (int i = 0; i < 8; i++)
                {
                    ImGui::Text("%c%c%c", 'a'+i, 'a'+i, 'a'+i);
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Filtering"))
    {
        static ImGuiTextFilter filter;
        ImGui::Text("Filter usage:\n"
                    "  \"\"         display all lines\n"
                    "  \"xxx\"      display lines containing \"xxx\"\n"
                    "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                    "  \"-xxx\"     hide lines containing \"xxx\"");
        filter.Draw();
        const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
        for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
            if (filter.PassFilter(lines[i]))
                ImGui::BulletText("%s", lines[i]);
    }

    if (ImGui::CollapsingHeader("Keyboard, Mouse & Focus"))
    {
        if (ImGui::TreeNode("Tabbing"))
        {
            ImGui::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "dummy";
            ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("3", buf, IM_ARRAYSIZE(buf));
            ImGui::PushAllowKeyboardFocus(false);
            ImGui::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            //ImGui::SameLine(); ShowHelperMarker("Use ImGui::PushAllowKeyboardFocus(bool)\nto disable tabbing through certain widgets.");
            ImGui::PopAllowKeyboardFocus();
            ImGui::InputText("5", buf, IM_ARRAYSIZE(buf));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Focus from code"))
        {
            bool focus_1 = ImGui::Button("Focus on 1"); ImGui::SameLine();
            bool focus_2 = ImGui::Button("Focus on 2"); ImGui::SameLine();
            bool focus_3 = ImGui::Button("Focus on 3");
            int has_focus = 0;
            static char buf[128] = "click on a button to set focus";
            
            if (focus_1) ImGui::SetKeyboardFocusHere();
            ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive()) has_focus = 1;
            
            if (focus_2) ImGui::SetKeyboardFocusHere();
            ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive()) has_focus = 2;

            ImGui::PushAllowKeyboardFocus(false);
            if (focus_3) ImGui::SetKeyboardFocusHere();
            ImGui::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive()) has_focus = 3;
            ImGui::PopAllowKeyboardFocus();
            if (has_focus)
                ImGui::Text("Item with focus: %d", has_focus);
            else 
                ImGui::Text("Item with focus: <none>");
            ImGui::TextWrapped("Cursor & selection are preserved when refocusing last used item in code.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Dragging"))
        {
            ImGui::TextWrapped("You can use ImGui::GetItemActiveDragDelta() to query for the dragged amount on any widget.");
            ImGui::Button("Drag Me");
            if (ImGui::IsItemActive())
            {
                // Draw a line between the button and the mouse cursor
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->PushClipRectFullScreen();
                draw_list->AddLine(ImGui::CalcItemRectClosestPoint(ImGui::GetIO().MousePos, true, -2.0f), ImGui::GetIO().MousePos, ImColor(ImGui::GetStyle().Colors[ImGuiCol_Button]), 4.0f);
                draw_list->PopClipRect();
                ImVec2 value_raw = ImGui::GetMouseDragDelta(0, 0.0f);
                ImVec2 value_with_lock_threshold = ImGui::GetMouseDragDelta(0);
                ImGui::SameLine(); ImGui::Text("Raw (%.1f, %.1f), WithLockThresold (%.1f, %.1f)", value_raw.x, value_raw.y, value_with_lock_threshold.x, value_with_lock_threshold.y);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Keyboard & Mouse State"))
        {
            ImGuiIO& io = ImGui::GetIO();

            ImGui::Text("MousePos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            ImGui::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f)   { ImGui::SameLine(); ImGui::Text("%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            ImGui::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseClicked(i))          { ImGui::SameLine(); ImGui::Text("%d", i); }
            ImGui::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseReleased(i))         { ImGui::SameLine(); ImGui::Text("%d", i); }
            ImGui::Text("MouseWheel: %.1f", io.MouseWheel);

            ImGui::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f)     { ImGui::SameLine(); ImGui::Text("%d (%.02f secs)", i, io.KeysDownDuration[i]); }
            ImGui::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyPressed(i))             { ImGui::SameLine(); ImGui::Text("%d", i); }
            ImGui::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyReleased(i))            { ImGui::SameLine(); ImGui::Text("%d", i); }
            ImGui::Text("KeyMods: %s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "");

            ImGui::Text("WantCaptureMouse: %s", io.WantCaptureMouse ? "true" : "false");
            ImGui::Text("WantCaptureKeyboard: %s", io.WantCaptureKeyboard ? "true" : "false");

            ImGui::Button("Hover me\nto enforce\ninputs capture");
            if (ImGui::IsItemHovered())
                ImGui::CaptureKeyboardFromApp();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Mouse cursors"))
        {
            ImGui::TextWrapped("Your application can render a different mouse cursor based on what ImGui::GetMouseCursor() returns. You can also set io.MouseDrawCursor to ask ImGui to render the cursor for you in software.");
            ImGui::Checkbox("io.MouseDrawCursor", &ImGui::GetIO().MouseDrawCursor);
            ImGui::Text("Hover to see mouse cursors:");
            for (int i = 0; i < ImGuiMouseCursor_Count_; i++)
            {
                char label[32];
                sprintf(label, "Mouse cursor %d", i);
                ImGui::Bullet(); ImGui::Selectable(label, false); 
                if (ImGui::IsItemHovered()) 
                    ImGui::SetMouseCursor(i);
            }
            ImGui::TreePop();
        }
    }

    ImGui::End();
}

static void ShowExampleAppMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

static void ShowExampleMenuFile()
{
    ImGui::MenuItem("(dummy menu)", NULL, false, false);
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            if (ImGui::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}
    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Colors"))
    {
        for (int i = 0; i < ImGuiCol_COUNT; i++)
            ImGui::MenuItem(ImGui::GetStyleColName((ImGuiCol)i));
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}

static void ShowExampleAppAutoResize(bool* opened)
{
    if (!ImGui::Begin("Example: Auto-resizing window", opened, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    static int lines = 10;
    ImGui::Text("Window will resize every-frame to the size of its content.\nNote that you probably don't want to query the window size to\noutput your content because that would create a feedback loop.");
    ImGui::SliderInt("Number of lines", &lines, 1, 20);
    for (int i = 0; i < lines; i++)
        ImGui::Text("%*sThis is line %d", i*4, "", i); // Pad with space to extend size horizontally
    ImGui::End();
}

static void ShowExampleAppFixedOverlay(bool* opened)
{
    ImGui::SetNextWindowPos(ImVec2(10,10));
    if (!ImGui::Begin("Example: Fixed Overlay", opened, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Simple overlay\non the top-left side of the screen.");
    ImGui::Separator();
    ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y); 
    ImGui::End();
}

static void ShowExampleAppManipulatingWindowTitle(bool* opened)
{
    (void)opened;

    // By default, Windows are uniquely identified by their title.
    // You can use the "##" and "###" markers to manipulate the display/ID. Read FAQ at the top of this file!

    // Using "##" to display same title but have unique identifier.
    ImGui::SetNextWindowPos(ImVec2(100,100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Same title as another window##1");
    ImGui::Text("This is window 1.\nMy title is the same as window 2, but my identifier is unique.");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(100,200), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Same title as another window##2");
    ImGui::Text("This is window 2.\nMy title is the same as window 1, but my identifier is unique.");
    ImGui::End();

    // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
    char buf[128];
    sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(ImGui::GetTime()/0.25f)&3], rand());
    ImGui::SetNextWindowPos(ImVec2(100,300), ImGuiSetCond_FirstUseEver);
    ImGui::Begin(buf);
    ImGui::Text("This window has a changing title.");
    ImGui::End();
}

static void ShowExampleAppCustomRendering(bool* opened)
{
    ImGui::SetNextWindowSize(ImVec2(300,350), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Example: Custom rendering", opened))
    {
        ImGui::End();
        return;
    }

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of overloaded operators, etc.
    // Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your types and ImVec2/ImVec4.
    // ImGui defines overloaded operators but they are internal to imgui.cpp and not exposed outside (to avoid messing with your types)
    // In this example we aren't using the operators.

    static ImVector<ImVec2> points;
    static bool adding_line = false;
    if (ImGui::Button("Clear")) points.clear();
    if (points.Size >= 2) { ImGui::SameLine(); if (ImGui::Button("Undo")) { points.pop_back(); points.pop_back(); } }
    ImGui::Text("Left-click and drag to add lines");
    ImGui::Text("Right-click to undo");

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use IsItemHovered()
    // However you can draw directly and poll mouse/keyboard by yourself. You can manipulate the cursor using GetCursorPos() and SetCursorPos().
    // If you only use the ImDrawList API, you can notify the owner window of its extends by using SetCursorPos(max).
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
    ImVec2 canvas_size = ImVec2(ImMax(50.0f,ImGui::GetWindowContentRegionMax().x-ImGui::GetCursorPos().x), ImMax(50.0f,ImGui::GetWindowContentRegionMax().y-ImGui::GetCursorPos().y));    // Resize canvas what's available
    draw_list->AddRectFilledMultiColor(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), ImColor(0,0,0), ImColor(255,0,0), ImColor(255,255,0), ImColor(0,255,0));
    draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), ImColor(255,255,255));
    bool adding_preview = false;
    ImGui::InvisibleButton("canvas", canvas_size);
    if (ImGui::IsItemHovered())
    {
        ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - canvas_pos.x, ImGui::GetIO().MousePos.y - canvas_pos.y);
        if (!adding_line && ImGui::GetIO().MouseClicked[0])
        {
            points.push_back(mouse_pos_in_canvas);
            adding_line = true;
        }
        if (adding_line)
        {
            adding_preview = true;
            points.push_back(mouse_pos_in_canvas);
            if (!ImGui::GetIO().MouseDown[0])
                adding_line = adding_preview = false;
        }
        if (ImGui::GetIO().MouseClicked[1] && !points.empty())
        {
            adding_line = false;
            points.pop_back();
            points.pop_back();
        }
    }
    draw_list->PushClipRect(ImVec4(canvas_pos.x, canvas_pos.y, canvas_pos.x+canvas_size.x, canvas_pos.y+canvas_size.y));      // clip lines within the canvas (if we resize it, etc.)
    for (int i = 0; i < points.Size - 1; i += 2)
        draw_list->AddLine(ImVec2(canvas_pos.x + points[i].x, canvas_pos.y + points[i].y), ImVec2(canvas_pos.x + points[i+1].x, canvas_pos.y + points[i+1].y), 0xFF00FFFF);
    draw_list->PopClipRect();
    if (adding_preview)
        points.pop_back();
    ImGui::End();
}

// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    bool                  ScrollToBottom;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> Commands;

    ExampleAppConsole()
    {
        ClearLog();
        HistoryPos = -1;
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");  // "classify" is here to provide an example of "C"+[tab] completing to "CL" and displaying matches.
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < Items.Size; i++) 
            free(History[i]); 
    }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++) 
            free(Items[i]); 
        Items.clear();
        ScrollToBottom = true;
    }

    void    AddLog(const char* fmt, ...)
    {
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        Items.push_back(strdup(buf));
        ScrollToBottom = true;
    }

    void    Run(const char* title, bool* opened)
    {
        ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiSetCond_FirstUseEver);
        if (!ImGui::Begin(title, opened))
        {
            ImGui::End();
            return;
        }

        ImGui::TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

        // TODO: display items starting from the bottom

        if (ImGui::SmallButton("Add Dummy Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); } ImGui::SameLine(); 
        if (ImGui::SmallButton("Add Dummy Error")) AddLog("[error] something went wrong"); ImGui::SameLine(); 
        if (ImGui::SmallButton("Clear")) ClearLog();
        //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
        static ImGuiTextFilter filter;
        filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::PopStyleVar();
        ImGui::Separator();

        // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient. You can seek and display only the lines that are visible - CalcListClipping() is a helper to compute this information.
        // If your items are of variable size you may want to implement code similar to what CalcListClipping() does. Or split your data into fixed height items to allow random-seeking into your list.
        ImGui::BeginChild("ScrollingRegion", ImVec2(0,-ImGui::GetItemsLineHeightWithSpacing()));
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            if (!filter.PassFilter(item))
                continue;
            ImVec4 col = ImColor(255,255,255); // A better implementation may store a type per-item. For the sample let's just parse the text.
            if (strstr(item, "[error]")) col = ImColor(255,100,100);
            else if (strncmp(item, "# ", 2) == 0) col = ImColor(255,200,150);
            ImGui::PushStyleColor(ImGuiCol_Text, col);
            ImGui::TextUnformatted(item);
            ImGui::PopStyleColor();
        }
        if (ScrollToBottom)
            ImGui::SetScrollHere();
        ScrollToBottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
        {
            char* input_end = InputBuf+strlen(InputBuf);
            while (input_end > InputBuf && input_end[-1] == ' ') input_end--; *input_end = 0;
            if (InputBuf[0])
                ExecCommand(InputBuf);
            strcpy(InputBuf, "");
        }

        // Demonstrate keeping auto focus on the input box
        if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
            ImGui::SetKeyboardFocusHere(-1); // Auto focus

        ImGui::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size-1; i >= 0; i--)
            if (ImStricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(strdup(command_line));

        // Process command
        if (ImStricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        }
        else if (ImStricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        }
        else if (ImStricmp(command_line, "HISTORY") == 0)
        {
            for (int i = History.Size >= 10 ? History.Size - 10 : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        }
        else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }
    }

    static int TextEditCallbackStub(ImGuiTextEditCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(ImGuiTextEditCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_CallbackCompletion:
            {
                // Example of TEXT COMPLETION

                // Locate beginning of current word
                const char* word_end = data->Buf + data->CursorPos;
                const char* word_start = word_end;
                while (word_start > data->Buf)
                {
                    const char c = word_start[-1];
                    if (c == ' ' || c == '\t' || c == ',' || c == ';')
                        break;
                    word_start--;
                }

                // Build a list of candidates
                ImVector<const char*> candidates;
                for (int i = 0; i < Commands.Size; i++)
                    if (ImStrnicmp(Commands[i], word_start, (int)(word_end-word_start)) == 0)
                        candidates.push_back(Commands[i]);

                if (candidates.Size == 0)
                {
                    // No match
                    AddLog("No match for \"%.*s\"!\n", word_end-word_start, word_start);
                }
                else if (candidates.Size == 1)
                {
                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
                    data->DeleteChars((int)(word_start-data->Buf), (int)(word_end-word_start));
                    data->InsertChars(data->CursorPos, candidates[0]);
                    data->InsertChars(data->CursorPos, " ");
                }
                else
                {
                    // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                    int match_len = (int)(word_end - word_start);
                    for (;;)
                    {
                        int c = 0;
                        bool all_candidates_matches = true;
                        for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                            if (i == 0)
                                c = toupper(candidates[i][match_len]);
                            else if (c != toupper(candidates[i][match_len]))
                                all_candidates_matches = false;
                        if (!all_candidates_matches)
                            break;
                        match_len++;
                    }

                    if (match_len > 0)
                    {
                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end-word_start));
                        data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                    }

                    // List matches
                    AddLog("Possible matches:\n");
                    for (int i = 0; i < candidates.Size; i++)
                        AddLog("- %s\n", candidates[i]);
                }

                break;
            }
        case ImGuiInputTextFlags_CallbackHistory:
            {
                // Example of HISTORY
                const int prev_history_pos = HistoryPos;
                if (data->EventKey == ImGuiKey_UpArrow)
                {
                    if (HistoryPos == -1)
                        HistoryPos = History.Size - 1;
                    else if (HistoryPos > 0)
                        HistoryPos--;
                }
                else if (data->EventKey == ImGuiKey_DownArrow)
                {
                    if (HistoryPos != -1)
                        if (++HistoryPos >= History.Size)
                            HistoryPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if (prev_history_pos != HistoryPos)
                {
                    snprintf(data->Buf, data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
                    data->BufDirty = true;
                    data->CursorPos = data->SelectionStart = data->SelectionEnd = (int)strlen(data->Buf);
                }
            }
        }
        return 0;
    }
};

static void ShowExampleAppConsole(bool* opened)
{
    static ExampleAppConsole console;
    console.Run("Example: Console", opened);
}

static void ShowExampleAppLayout(bool* opened)
{
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiSetCond_FirstUseEver);
    if (ImGui::Begin("Example: Layout", opened, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close")) *opened = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // left
        static int selected = 0;
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        for (int i = 0; i < 100; i++)
        {
            char label[128];
            sprintf(label, "MyObject %d", i);
            if (ImGui::Selectable(label, selected == i))
                selected = i;
        }
        ImGui::EndChild();
        ImGui::SameLine();
        
        // right
        ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing())); // Leave room for 1 line below us
                ImGui::Text("MyObject: %d", selected);
                ImGui::Separator();
                ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
            ImGui::EndChild();
            ImGui::BeginChild("buttons");
                if (ImGui::Button("Revert")) {}
                ImGui::SameLine();
                if (ImGui::Button("Save")) {}
            ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::End();
}

static void ShowExampleAppLongText(bool* opened)
{
    ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Example: Long text display", opened))
    {
        ImGui::End();
        return;
    }

    static int test_type = 0;
    static ImGuiTextBuffer log;
    static int lines = 0;
    ImGui::Text("Printing unusually long amount of text.");
    ImGui::Combo("Test type", &test_type, "Single call to TextUnformatted()\0Multiple calls to Text(), clipped manually\0Multiple calls to Text(), not clipped"); 
    ImGui::Text("Buffer contents: %d lines, %d bytes", lines, log.size());
    if (ImGui::Button("Clear")) { log.clear(); lines = 0; }
    ImGui::SameLine();
    if (ImGui::Button("Add 1000 lines"))
    {
        for (int i = 0; i < 1000; i++)
            log.append("%i The quick brown fox jumps over the lazy dog\n", lines+i);
        lines += 1000;
    }
    ImGui::BeginChild("Log");
    switch (test_type)
    {
    case 0:
        // Single call to TextUnformatted() with a big buffer
        ImGui::TextUnformatted(log.begin(), log.end());
        break;
    case 1:
        // Multiple calls to Text(), manually coarsely clipped - demonstrate how to use the CalcListClipping() helper.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
        int display_start, display_end;
        ImGui::CalcListClipping(lines, ImGui::GetTextLineHeight(), &display_start, &display_end);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (display_start) * ImGui::GetTextLineHeight());
        for (int i = display_start; i < display_end; i++)
            ImGui::Text("%i The quick brown fox jumps over the lazy dog\n", i);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (lines - display_end) * ImGui::GetTextLineHeight());
        ImGui::PopStyleVar();
        break;
    case 2:
        // Multiple calls to Text(), not clipped (slow)
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
        for (int i = 0; i < lines; i++)
            ImGui::Text("%i The quick brown fox jumps over the lazy dog\n", i);
        ImGui::PopStyleVar();
        break;
    }
    ImGui::EndChild();
    ImGui::End();
}

// End of Sample code
#else

void ImGui::ShowTestWindow(bool*) {}

#endif

//-----------------------------------------------------------------------------
// DEFAULT FONT DATA
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array.
// Use the program in extra_fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(unsigned char *input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char *stb__barrier, *stb__barrier2, *stb__barrier3, *stb__barrier4;
static unsigned char *stb__dout;
static void stb__match(unsigned char *data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    IM_ASSERT (stb__dout + length <= stb__barrier);
    if (stb__dout + length > stb__barrier) { stb__dout += length; return; }
    if (data < stb__barrier4) { stb__dout = stb__barrier+1; return; }
    while (length--) *stb__dout++ = *data++;
}

static void stb__lit(unsigned char *data, unsigned int length)
{
    IM_ASSERT (stb__dout + length <= stb__barrier);
    if (stb__dout + length > stb__barrier) { stb__dout += length; return; }
    if (data < stb__barrier2) { stb__dout = stb__barrier+1; return; }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static unsigned char *stb_decompress_token(unsigned char *i)
{
    if (*i >= 0x20) { // use fewer if's for cases that expand small
        if (*i >= 0x80)       stb__match(stb__dout-i[1]-1, i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40)  stb__match(stb__dout-(stb__in2(0) - 0x4000 + 1), i[2]+1), i += 3;
        else /* *i >= 0x20 */ stb__lit(i+1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)       stb__match(stb__dout-(stb__in3(0) - 0x180000 + 1), i[3]+1), i += 4;
        else if (*i >= 0x10)  stb__match(stb__dout-(stb__in3(0) - 0x100000 + 1), stb__in2(3)+1), i += 5;
        else if (*i >= 0x08)  stb__lit(i+2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
        else if (*i == 0x07)  stb__lit(i+3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
        else if (*i == 0x06)  stb__match(stb__dout-(stb__in3(1)+1), i[4]+1), i += 5;
        else if (*i == 0x04)  stb__match(stb__dout-(stb__in3(1)+1), stb__in2(4)+1), i += 6;
    }
    return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen, i;

    blocklen = buflen % 5552;
    while (buflen) {
        for (i=0; i + 7 < blocklen; i += 8) {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i)
            s1 += *buffer++, s2 += s1;

        s1 %= ADLER_MOD, s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char *output, unsigned char *i, unsigned int length)
{
    unsigned int olen;
    if (stb__in4(0) != 0x57bC0000) return 0;
    if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
    olen = stb_decompress_length(i);
    stb__barrier2 = i;
    stb__barrier3 = i+length;
    stb__barrier = output + olen;
    stb__barrier4 = output;
    i += 16;

    stb__dout = output;
    for (;;) {
        unsigned char *old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i) {
            if (*i == 0x05 && i[1] == 0xfa) {
                IM_ASSERT(stb__dout == output + olen);
                if (stb__dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int) stb__in4(2))
                    return 0;
                return olen;
            } else {
                IM_ASSERT(0); /* NOTREACHED */
                return 0;
            }
        }
        IM_ASSERT(stb__dout <= output + olen); 
        if (stb__dout > output + olen)
            return 0;
    }
}

//-----------------------------------------------------------------------------
// ProggyClean.ttf
// Copyright (c) 2004, 2005 Tristan Grimmer
// MIT license (see License.txt in http://www.upperbounds.net/download/ProggyClean.ttf.zip)
// Download and more information at http://upperbounds.net
//-----------------------------------------------------------------------------
// File: 'ProggyClean.ttf' (41208 bytes)
// Exported using binary_to_compressed_c.cpp
//-----------------------------------------------------------------------------
static const char proggy_clean_ttf_compressed_data_base85[11980+1] =
    "7])#######hV0qs'/###[),##/l:$#Q6>##5[n42>c-TH`->>#/e>11NNV=Bv(*:.F?uu#(gRU.o0XGH`$vhLG1hxt9?W`#,5LsCp#-i>.r$<$6pD>Lb';9Crc6tgXmKVeU2cD4Eo3R/"
    "2*>]b(MC;$jPfY.;h^`IWM9<Lh2TlS+f-s$o6Q<BWH`YiU.xfLq$N;$0iR/GX:U(jcW2p/W*q?-qmnUCI;jHSAiFWM.R*kU@C=GH?a9wp8f$e.-4^Qg1)Q-GL(lf(r/7GrRgwV%MS=C#"
    "`8ND>Qo#t'X#(v#Y9w0#1D$CIf;W'#pWUPXOuxXuU(H9M(1<q-UE31#^-V'8IRUo7Qf./L>=Ke$$'5F%)]0^#0X@U.a<r:QLtFsLcL6##lOj)#.Y5<-R&KgLwqJfLgN&;Q?gI^#DY2uL"
    "i@^rMl9t=cWq6##weg>$FBjVQTSDgEKnIS7EM9>ZY9w0#L;>>#Mx&4Mvt//L[MkA#W@lK.N'[0#7RL_&#w+F%HtG9M#XL`N&.,GM4Pg;-<nLENhvx>-VsM.M0rJfLH2eTM`*oJMHRC`N"
    "kfimM2J,W-jXS:)r0wK#@Fge$U>`w'N7G#$#fB#$E^$#:9:hk+eOe--6x)F7*E%?76%^GMHePW-Z5l'&GiF#$956:rS?dA#fiK:)Yr+`&#0j@'DbG&#^$PG.Ll+DNa<XCMKEV*N)LN/N"
    "*b=%Q6pia-Xg8I$<MR&,VdJe$<(7G;Ckl'&hF;;$<_=X(b.RS%%)###MPBuuE1V:v&cX&#2m#(&cV]`k9OhLMbn%s$G2,B$BfD3X*sp5#l,$R#]x_X1xKX%b5U*[r5iMfUo9U`N99hG)"
    "tm+/Us9pG)XPu`<0s-)WTt(gCRxIg(%6sfh=ktMKn3j)<6<b5Sk_/0(^]AaN#(p/L>&VZ>1i%h1S9u5o@YaaW$e+b<TWFn/Z:Oh(Cx2$lNEoN^e)#CFY@@I;BOQ*sRwZtZxRcU7uW6CX"
    "ow0i(?$Q[cjOd[P4d)]>ROPOpxTO7Stwi1::iB1q)C_=dV26J;2,]7op$]uQr@_V7$q^%lQwtuHY]=DX,n3L#0PHDO4f9>dC@O>HBuKPpP*E,N+b3L#lpR/MrTEH.IAQk.a>D[.e;mc."
    "x]Ip.PH^'/aqUO/$1WxLoW0[iLA<QT;5HKD+@qQ'NQ(3_PLhE48R.qAPSwQ0/WK?Z,[x?-J;jQTWA0X@KJ(_Y8N-:/M74:/-ZpKrUss?d#dZq]DAbkU*JqkL+nwX@@47`5>w=4h(9.`G"
    "CRUxHPeR`5Mjol(dUWxZa(>STrPkrJiWx`5U7F#.g*jrohGg`cg:lSTvEY/EV_7H4Q9[Z%cnv;JQYZ5q.l7Zeas:HOIZOB?G<Nald$qs]@]L<J7bR*>gv:[7MI2k).'2($5FNP&EQ(,)"
    "U]W]+fh18.vsai00);D3@4ku5P?DP8aJt+;qUM]=+b'8@;mViBKx0DE[-auGl8:PJ&Dj+M6OC]O^((##]`0i)drT;-7X`=-H3[igUnPG-NZlo.#k@h#=Ork$m>a>$-?Tm$UV(?#P6YY#"
    "'/###xe7q.73rI3*pP/$1>s9)W,JrM7SN]'/4C#v$U`0#V.[0>xQsH$fEmPMgY2u7Kh(G%siIfLSoS+MK2eTM$=5,M8p`A.;_R%#u[K#$x4AG8.kK/HSB==-'Ie/QTtG?-.*^N-4B/ZM"
    "_3YlQC7(p7q)&](`6_c)$/*JL(L-^(]$wIM`dPtOdGA,U3:w2M-0<q-]L_?^)1vw'.,MRsqVr.L;aN&#/EgJ)PBc[-f>+WomX2u7lqM2iEumMTcsF?-aT=Z-97UEnXglEn1K-bnEO`gu"
    "Ft(c%=;Am_Qs@jLooI&NX;]0#j4#F14;gl8-GQpgwhrq8'=l_f-b49'UOqkLu7-##oDY2L(te+Mch&gLYtJ,MEtJfLh'x'M=$CS-ZZ%P]8bZ>#S?YY#%Q&q'3^Fw&?D)UDNrocM3A76/"
    "/oL?#h7gl85[qW/NDOk%16ij;+:1a'iNIdb-ou8.P*w,v5#EI$TWS>Pot-R*H'-SEpA:g)f+O$%%`kA#G=8RMmG1&O`>to8bC]T&$,n.LoO>29sp3dt-52U%VM#q7'DHpg+#Z9%H[K<L"
    "%a2E-grWVM3@2=-k22tL]4$##6We'8UJCKE[d_=%wI;'6X-GsLX4j^SgJ$##R*w,vP3wK#iiW&#*h^D&R?jp7+/u&#(AP##XU8c$fSYW-J95_-Dp[g9wcO&#M-h1OcJlc-*vpw0xUX&#"
    "OQFKNX@QI'IoPp7nb,QU//MQ&ZDkKP)X<WSVL(68uVl&#c'[0#(s1X&xm$Y%B7*K:eDA323j998GXbA#pwMs-jgD$9QISB-A_(aN4xoFM^@C58D0+Q+q3n0#3U1InDjF682-SjMXJK)("
    "h$hxua_K]ul92%'BOU&#BRRh-slg8KDlr:%L71Ka:.A;%YULjDPmL<LYs8i#XwJOYaKPKc1h:'9Ke,g)b),78=I39B;xiY$bgGw-&.Zi9InXDuYa%G*f2Bq7mn9^#p1vv%#(Wi-;/Z5h"
    "o;#2:;%d&#x9v68C5g?ntX0X)pT`;%pB3q7mgGN)3%(P8nTd5L7GeA-GL@+%J3u2:(Yf>et`e;)f#Km8&+DC$I46>#Kr]]u-[=99tts1.qb#q72g1WJO81q+eN'03'eM>&1XxY-caEnO"
    "j%2n8)),?ILR5^.Ibn<-X-Mq7[a82Lq:F&#ce+S9wsCK*x`569E8ew'He]h:sI[2LM$[guka3ZRd6:t%IG:;$%YiJ:Nq=?eAw;/:nnDq0(CYcMpG)qLN4$##&J<j$UpK<Q4a1]MupW^-"
    "sj_$%[HK%'F####QRZJ::Y3EGl4'@%FkiAOg#p[##O`gukTfBHagL<LHw%q&OV0##F=6/:chIm0@eCP8X]:kFI%hl8hgO@RcBhS-@Qb$%+m=hPDLg*%K8ln(wcf3/'DW-$.lR?n[nCH-"
    "eXOONTJlh:.RYF%3'p6sq:UIMA945&^HFS87@$EP2iG<-lCO$%c`uKGD3rC$x0BL8aFn--`ke%#HMP'vh1/R&O_J9'um,.<tx[@%wsJk&bUT2`0uMv7gg#qp/ij.L56'hl;.s5CUrxjO"
    "M7-##.l+Au'A&O:-T72L]P`&=;ctp'XScX*rU.>-XTt,%OVU4)S1+R-#dg0/Nn?Ku1^0f$B*P:Rowwm-`0PKjYDDM'3]d39VZHEl4,.j']Pk-M.h^&:0FACm$maq-&sgw0t7/6(^xtk%"
    "LuH88Fj-ekm>GA#_>568x6(OFRl-IZp`&b,_P'$M<Jnq79VsJW/mWS*PUiq76;]/NM_>hLbxfc$mj`,O;&%W2m`Zh:/)Uetw:aJ%]K9h:TcF]u_-Sj9,VK3M.*'&0D[Ca]J9gp8,kAW]"
    "%(?A%R$f<->Zts'^kn=-^@c4%-pY6qI%J%1IGxfLU9CP8cbPlXv);C=b),<2mOvP8up,UVf3839acAWAW-W?#ao/^#%KYo8fRULNd2.>%m]UK:n%r$'sw]J;5pAoO_#2mO3n,'=H5(et"
    "Hg*`+RLgv>=4U8guD$I%D:W>-r5V*%j*W:Kvej.Lp$<M-SGZ':+Q_k+uvOSLiEo(<aD/K<CCc`'Lx>'?;++O'>()jLR-^u68PHm8ZFWe+ej8h:9r6L*0//c&iH&R8pRbA#Kjm%upV1g:"
    "a_#Ur7FuA#(tRh#.Y5K+@?3<-8m0$PEn;J:rh6?I6uG<-`wMU'ircp0LaE_OtlMb&1#6T.#FDKu#1Lw%u%+GM+X'e?YLfjM[VO0MbuFp7;>Q&#WIo)0@F%q7c#4XAXN-U&VB<HFF*qL("
    "$/V,;(kXZejWO`<[5??ewY(*9=%wDc;,u<'9t3W-(H1th3+G]ucQ]kLs7df($/*JL]@*t7Bu_G3_7mp7<iaQjO@.kLg;x3B0lqp7Hf,^Ze7-##@/c58Mo(3;knp0%)A7?-W+eI'o8)b<"
    "nKnw'Ho8C=Y>pqB>0ie&jhZ[?iLR@@_AvA-iQC(=ksRZRVp7`.=+NpBC%rh&3]R:8XDmE5^V8O(x<<aG/1N$#FX$0V5Y6x'aErI3I$7x%E`v<-BY,)%-?Psf*l?%C3.mM(=/M0:JxG'?"
    "7WhH%o'a<-80g0NBxoO(GH<dM]n.+%q@jH?f.UsJ2Ggs&4<-e47&Kl+f//9@`b+?.TeN_&B8Ss?v;^Trk;f#YvJkl&w$]>-+k?'(<S:68tq*WoDfZu';mM?8X[ma8W%*`-=;D.(nc7/;"
    ")g:T1=^J$&BRV(-lTmNB6xqB[@0*o.erM*<SWF]u2=st-*(6v>^](H.aREZSi,#1:[IXaZFOm<-ui#qUq2$##Ri;u75OK#(RtaW-K-F`S+cF]uN`-KMQ%rP/Xri.LRcB##=YL3BgM/3M"
    "D?@f&1'BW-)Ju<L25gl8uhVm1hL$##*8###'A3/LkKW+(^rWX?5W_8g)a(m&K8P>#bmmWCMkk&#TR`C,5d>g)F;t,4:@_l8G/5h4vUd%&%950:VXD'QdWoY-F$BtUwmfe$YqL'8(PWX("
    "P?^@Po3$##`MSs?DWBZ/S>+4%>fX,VWv/w'KD`LP5IbH;rTV>n3cEK8U#bX]l-/V+^lj3;vlMb&[5YQ8#pekX9JP3XUC72L,,?+Ni&co7ApnO*5NK,((W-i:$,kp'UDAO(G0Sq7MVjJs"
    "bIu)'Z,*[>br5fX^:FPAWr-m2KgL<LUN098kTF&#lvo58=/vjDo;.;)Ka*hLR#/k=rKbxuV`>Q_nN6'8uTG&#1T5g)uLv:873UpTLgH+#FgpH'_o1780Ph8KmxQJ8#H72L4@768@Tm&Q"
    "h4CB/5OvmA&,Q&QbUoi$a_%3M01H)4x7I^&KQVgtFnV+;[Pc>[m4k//,]1?#`VY[Jr*3&&slRfLiVZJ:]?=K3Sw=[$=uRB?3xk48@aeg<Z'<$#4H)6,>e0jT6'N#(q%.O=?2S]u*(m<-"
    "V8J'(1)G][68hW$5'q[GC&5j`TE?m'esFGNRM)j,ffZ?-qx8;->g4t*:CIP/[Qap7/9'#(1sao7w-.qNUdkJ)tCF&#B^;xGvn2r9FEPFFFcL@.iFNkTve$m%#QvQS8U@)2Z+3K:AKM5i"
    "sZ88+dKQ)W6>J%CL<KE>`.d*(B`-n8D9oK<Up]c$X$(,)M8Zt7/[rdkqTgl-0cuGMv'?>-XV1q['-5k'cAZ69e;D_?$ZPP&s^+7])$*$#@QYi9,5P&#9r+$%CE=68>K8r0=dSC%%(@p7"
    ".m7jilQ02'0-VWAg<a/''3u.=4L$Y)6k/K:_[3=&jvL<L0C/2'v:^;-DIBW,B4E68:kZ;%?8(Q8BH=kO65BW?xSG&#@uU,DS*,?.+(o(#1vCS8#CHF>TlGW'b)Tq7VT9q^*^$$.:&N@@"
    "$&)WHtPm*5_rO0&e%K&#-30j(E4#'Zb.o/(Tpm$>K'f@[PvFl,hfINTNU6u'0pao7%XUp9]5.>%h`8_=VYbxuel.NTSsJfLacFu3B'lQSu/m6-Oqem8T+oE--$0a/k]uj9EwsG>%veR*"
    "hv^BFpQj:K'#SJ,sB-'#](j.Lg92rTw-*n%@/;39rrJF,l#qV%OrtBeC6/,;qB3ebNW[?,Hqj2L.1NP&GjUR=1D8QaS3Up&@*9wP?+lo7b?@%'k4`p0Z$22%K3+iCZj?XJN4Nm&+YF]u"
    "@-W$U%VEQ/,,>>#)D<h#`)h0:<Q6909ua+&VU%n2:cG3FJ-%@Bj-DgLr`Hw&HAKjKjseK</xKT*)B,N9X3]krc12t'pgTV(Lv-tL[xg_%=M_q7a^x?7Ubd>#%8cY#YZ?=,`Wdxu/ae&#"
    "w6)R89tI#6@s'(6Bf7a&?S=^ZI_kS&ai`&=tE72L_D,;^R)7[$s<Eh#c&)q.MXI%#v9ROa5FZO%sF7q7Nwb&#ptUJ:aqJe$Sl68%.D###EC><?-aF&#RNQv>o8lKN%5/$(vdfq7+ebA#"
    "u1p]ovUKW&Y%q]'>$1@-[xfn$7ZTp7mM,G,Ko7a&Gu%G[RMxJs[0MM%wci.LFDK)(<c`Q8N)jEIF*+?P2a8g%)$q]o2aH8C&<SibC/q,(e:v;-b#6[$NtDZ84Je2KNvB#$P5?tQ3nt(0"
    "d=j.LQf./Ll33+(;q3L-w=8dX$#WF&uIJ@-bfI>%:_i2B5CsR8&9Z&#=mPEnm0f`<&c)QL5uJ#%u%lJj+D-r;BoF&#4DoS97h5g)E#o:&S4weDF,9^Hoe`h*L+_a*NrLW-1pG_&2UdB8"
    "6e%B/:=>)N4xeW.*wft-;$'58-ESqr<b?UI(_%@[P46>#U`'6AQ]m&6/`Z>#S?YY#Vc;r7U2&326d=w&H####?TZ`*4?&.MK?LP8Vxg>$[QXc%QJv92.(Db*B)gb*BM9dM*hJMAo*c&#"
    "b0v=Pjer]$gG&JXDf->'StvU7505l9$AFvgYRI^&<^b68?j#q9QX4SM'RO#&sL1IM.rJfLUAj221]d##DW=m83u5;'bYx,*Sl0hL(W;;$doB&O/TQ:(Z^xBdLjL<Lni;''X.`$#8+1GD"
    ":k$YUWsbn8ogh6rxZ2Z9]%nd+>V#*8U_72Lh+2Q8Cj0i:6hp&$C/:p(HK>T8Y[gHQ4`4)'$Ab(Nof%V'8hL&#<NEdtg(n'=S1A(Q1/I&4([%dM`,Iu'1:_hL>SfD07&6D<fp8dHM7/g+"
    "tlPN9J*rKaPct&?'uBCem^jn%9_K)<,C5K3s=5g&GmJb*[SYq7K;TRLGCsM-$$;S%:Y@r7AK0pprpL<Lrh,q7e/%KWK:50I^+m'vi`3?%Zp+<-d+$L-Sv:@.o19n$s0&39;kn;S%BSq*"
    "$3WoJSCLweV[aZ'MQIjO<7;X-X;&+dMLvu#^UsGEC9WEc[X(wI7#2.(F0jV*eZf<-Qv3J-c+J5AlrB#$p(H68LvEA'q3n0#m,[`*8Ft)FcYgEud]CWfm68,(aLA$@EFTgLXoBq/UPlp7"
    ":d[/;r_ix=:TF`S5H-b<LI&HY(K=h#)]Lk$K14lVfm:x$H<3^Ql<M`$OhapBnkup'D#L$Pb_`N*g]2e;X/Dtg,bsj&K#2[-:iYr'_wgH)NUIR8a1n#S?Yej'h8^58UbZd+^FKD*T@;6A"
    "7aQC[K8d-(v6GI$x:T<&'Gp5Uf>@M.*J:;$-rv29'M]8qMv-tLp,'886iaC=Hb*YJoKJ,(j%K=H`K.v9HggqBIiZu'QvBT.#=)0ukruV&.)3=(^1`o*Pj4<-<aN((^7('#Z0wK#5GX@7"
    "u][`*S^43933A4rl][`*O4CgLEl]v$1Q3AeF37dbXk,.)vj#x'd`;qgbQR%FW,2(?LO=s%Sc68%NP'##Aotl8x=BE#j1UD([3$M(]UI2LX3RpKN@;/#f'f/&_mt&F)XdF<9t4)Qa.*kT"
    "LwQ'(TTB9.xH'>#MJ+gLq9-##@HuZPN0]u:h7.T..G:;$/Usj(T7`Q8tT72LnYl<-qx8;-HV7Q-&Xdx%1a,hC=0u+HlsV>nuIQL-5<N?)NBS)QN*_I,?&)2'IM%L3I)X((e/dl2&8'<M"
    ":^#M*Q+[T.Xri.LYS3v%fF`68h;b-X[/En'CR.q7E)p'/kle2HM,u;^%OKC-N+Ll%F9CF<Nf'^#t2L,;27W:0O@6##U6W7:$rJfLWHj$#)woqBefIZ.PK<b*t7ed;p*_m;4ExK#h@&]>"
    "_>@kXQtMacfD.m-VAb8;IReM3$wf0''hra*so568'Ip&vRs849'MRYSp%:t:h5qSgwpEr$B>Q,;s(C#$)`svQuF$##-D,##,g68@2[T;.XSdN9Qe)rpt._K-#5wF)sP'##p#C0c%-Gb%"
    "hd+<-j'Ai*x&&HMkT]C'OSl##5RG[JXaHN;d'uA#x._U;.`PU@(Z3dt4r152@:v,'R.Sj'w#0<-;kPI)FfJ&#AYJ&#//)>-k=m=*XnK$>=)72L]0I%>.G690a:$##<,);?;72#?x9+d;"
    "^V'9;jY@;)br#q^YQpx:X#Te$Z^'=-=bGhLf:D6&bNwZ9-ZD#n^9HhLMr5G;']d&6'wYmTFmL<LD)F^%[tC'8;+9E#C$g%#5Y>q9wI>P(9mI[>kC-ekLC/R&CH+s'B;K-M6$EB%is00:"
    "+A4[7xks.LrNk0&E)wILYF@2L'0Nb$+pv<(2.768/FrY&h$^3i&@+G%JT'<-,v`3;_)I9M^AE]CN?Cl2AZg+%4iTpT3<n-&%H%b<FDj2M<hH=&Eh<2Len$b*aTX=-8QxN)k11IM1c^j%"
    "9s<L<NFSo)B?+<-(GxsF,^-Eh@$4dXhN$+#rxK8'je'D7k`e;)2pYwPA'_p9&@^18ml1^[@g4t*[JOa*[=Qp7(qJ_oOL^('7fB&Hq-:sf,sNj8xq^>$U4O]GKx'm9)b@p7YsvK3w^YR-"
    "CdQ*:Ir<($u&)#(&?L9Rg3H)4fiEp^iI9O8KnTj,]H?D*r7'M;PwZ9K0E^k&-cpI;.p/6_vwoFMV<->#%Xi.LxVnrU(4&8/P+:hLSKj$#U%]49t'I:rgMi'FL@a:0Y-uA[39',(vbma*"
    "hU%<-SRF`Tt:542R_VV$p@[p8DV[A,?1839FWdF<TddF<9Ah-6&9tWoDlh]&1SpGMq>Ti1O*H&#(AL8[_P%.M>v^-))qOT*F5Cq0`Ye%+$B6i:7@0IX<N+T+0MlMBPQ*Vj>SsD<U4JHY"
    "8kD2)2fU/M#$e.)T4,_=8hLim[&);?UkK'-x?'(:siIfL<$pFM`i<?%W(mGDHM%>iWP,##P`%/L<eXi:@Z9C.7o=@(pXdAO/NLQ8lPl+HPOQa8wD8=^GlPa8TKI1CjhsCTSLJM'/Wl>-"
    "S(qw%sf/@%#B6;/U7K]uZbi^Oc^2n<bhPmUkMw>%t<)'mEVE''n`WnJra$^TKvX5B>;_aSEK',(hwa0:i4G?.Bci.(X[?b*($,=-n<.Q%`(X=?+@Am*Js0&=3bh8K]mL<LoNs'6,'85`"
    "0?t/'_U59@]ddF<#LdF<eWdF<OuN/45rY<-L@&#+fm>69=Lb,OcZV/);TTm8VI;?%OtJ<(b4mq7M6:u?KRdF<gR@2L=FNU-<b[(9c/ML3m;Z[$oF3g)GAWqpARc=<ROu7cL5l;-[A]%/"
    "+fsd;l#SafT/f*W]0=O'$(Tb<[)*@e775R-:Yob%g*>l*:xP?Yb.5)%w_I?7uk5JC+FS(m#i'k.'a0i)9<7b'fs'59hq$*5Uhv##pi^8+hIEBF`nvo`;'l0.^S1<-wUK2/Coh58KKhLj"
    "M=SO*rfO`+qC`W-On.=AJ56>>i2@2LH6A:&5q`?9I3@@'04&p2/LVa*T-4<-i3;M9UvZd+N7>b*eIwg:CC)c<>nO&#<IGe;__.thjZl<%w(Wk2xmp4Q@I#I9,DF]u7-P=.-_:YJ]aS@V"
    "?6*C()dOp7:WL,b&3Rg/.cmM9&r^>$(>.Z-I&J(Q0Hd5Q%7Co-b`-c<N(6r@ip+AurK<m86QIth*#v;-OBqi+L7wDE-Ir8K['m+DDSLwK&/.?-V%U_%3:qKNu$_b*B-kp7NaD'QdWQPK"
    "Yq[@>P)hI;*_F]u`Rb[.j8_Q/<&>uu+VsH$sM9TA%?)(vmJ80),P7E>)tjD%2L=-t#fK[%`v=Q8<FfNkgg^oIbah*#8/Qt$F&:K*-(N/'+1vMB,u()-a.VUU*#[e%gAAO(S>WlA2);Sa"
    ">gXm8YB`1d@K#n]76-a$U,mF<fX]idqd)<3,]J7JmW4`6]uks=4-72L(jEk+:bJ0M^q-8Dm_Z?0olP1C9Sa&H[d&c$ooQUj]Exd*3ZM@-WGW2%s',B-_M%>%Ul:#/'xoFM9QX-$.QN'>"
    "[%$Z$uF6pA6Ki2O5:8w*vP1<-1`[G,)-m#>0`P&#eb#.3i)rtB61(o'$?X3B</R90;eZ]%Ncq;-Tl]#F>2Qft^ae_5tKL9MUe9b*sLEQ95C&`=G?@Mj=wh*'3E>=-<)Gt*Iw)'QG:`@I"
    "wOf7&]1i'S01B+Ev/Nac#9S;=;YQpg_6U`*kVY39xK,[/6Aj7:'1Bm-_1EYfa1+o&o4hp7KN_Q(OlIo@S%;jVdn0'1<Vc52=u`3^o-n1'g4v58Hj&6_t7$##?M)c<$bgQ_'SY((-xkA#"
    "Y(,p'H9rIVY-b,'%bCPF7.J<Up^,(dU1VY*5#WkTU>h19w,WQhLI)3S#f$2(eb,jr*b;3Vw]*7NH%$c4Vs,eD9>XW8?N]o+(*pgC%/72LV-u<Hp,3@e^9UB1J+ak9-TN/mhKPg+AJYd$"
    "MlvAF_jCK*.O-^(63adMT->W%iewS8W6m2rtCpo'RS1R84=@paTKt)>=%&1[)*vp'u+x,VrwN;&]kuO9JDbg=pO$J*.jVe;u'm0dr9l,<*wMK*Oe=g8lV_KEBFkO'oU]^=[-792#ok,)"
    "i]lR8qQ2oA8wcRCZ^7w/Njh;?.stX?Q1>S1q4Bn$)K1<-rGdO'$Wr.Lc.CG)$/*JL4tNR/,SVO3,aUw'DJN:)Ss;wGn9A32ijw%FL+Z0Fn.U9;reSq)bmI32U==5ALuG&#Vf1398/pVo"
    "1*c-(aY168o<`JsSbk-,1N;$>0:OUas(3:8Z972LSfF8eb=c-;>SPw7.6hn3m`9^Xkn(r.qS[0;T%&Qc=+STRxX'q1BNk3&*eu2;&8q$&x>Q#Q7^Tf+6<(d%ZVmj2bDi%.3L2n+4W'$P"
    "iDDG)g,r%+?,$@?uou5tSe2aN_AQU*<h`e-GI7)?OK2A.d7_c)?wQ5AS@DL3r#7fSkgl6-++D:'A,uq7SvlB$pcpH'q3n0#_%dY#xCpr-l<F0NR@-##FEV6NTF6##$l84N1w?AO>'IAO"
    "URQ##V^Fv-XFbGM7Fl(N<3DhLGF%q.1rC$#:T__&Pi68%0xi_&[qFJ(77j_&JWoF.V735&T,[R*:xFR*K5>>#`bW-?4Ne_&6Ne_&6Ne_&n`kr-#GJcM6X;uM6X;uM(.a..^2TkL%oR(#"
    ";u.T%fAr%4tJ8&><1=GHZ_+m9/#H1F^R#SC#*N=BA9(D?v[UiFY>>^8p,KKF.W]L29uLkLlu/+4T<XoIB&hx=T1PcDaB&;HH+-AFr?(m9HZV)FKS8JCw;SD=6[^/DZUL`EUDf]GGlG&>"
    "w$)F./^n3+rlo+DB;5sIYGNk+i1t-69Jg--0pao7Sm#K)pdHW&;LuDNH@H>#/X-TI(;P>#,Gc>#0Su>#4`1?#8lC?#<xU?#@.i?#D:%@#HF7@#LRI@#P_[@#Tkn@#Xw*A#]-=A#a9OA#"
    "d<F&#*;G##.GY##2Sl##6`($#:l:$#>xL$#B.`$#F:r$#JF.%#NR@%#R_R%#Vke%#Zww%#_-4&#3^Rh%Sflr-k'MS.o?.5/sWel/wpEM0%3'/1)K^f1-d>G21&v(35>V`39V7A4=onx4"
    "A1OY5EI0;6Ibgr6M$HS7Q<)58C5w,;WoA*#[%T*#`1g*#d=#+#hI5+#lUG+#pbY+#tnl+#x$),#&1;,#*=M,#.I`,#2Ur,#6b.-#;w[H#iQtA#m^0B#qjBB#uvTB##-hB#'9$C#+E6C#"
    "/QHC#3^ZC#7jmC#;v)D#?,<D#C8ND#GDaD#KPsD#O]/E#g1A5#KA*1#gC17#MGd;#8(02#L-d3#rWM4#Hga1#,<w0#T.j<#O#'2#CYN1#qa^:#_4m3#o@/=#eG8=#t8J5#`+78#4uI-#"
    "m3B2#SB[8#Q0@8#i[*9#iOn8#1Nm;#^sN9#qh<9#:=x-#P;K2#$%X9#bC+.#Rg;<#mN=.#MTF.#RZO.#2?)4#Y#(/#[)1/#b;L/#dAU/#0Sv;#lY$0#n`-0#sf60#(F24#wrH0#%/e0#"
    "TmD<#%JSMFove:CTBEXI:<eh2g)B,3h2^G3i;#d3jD>)4kMYD4lVu`4m`:&5niUA5@(A5BA1]PBB:xlBCC=2CDLXMCEUtiCf&0g2'tN?PGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CP"
    "GT4CPGT4CPGT4CPGT4CPGT4CPGT4CP-qekC`.9kEg^+F$kwViFJTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5o,^<-28ZI'O?;xp"
    "O?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xp;7q-#lLYI:xvD=#";

static const char* GetDefaultCompressedFontDataTTFBase85() 
{ 
    return proggy_clean_ttf_compressed_data_base85; 
}

//-----------------------------------------------------------------------------

//---- Include imgui_user.inl at the end of imgui.cpp
//---- So you can include code that extends ImGui using its private data/functions.
#ifdef IMGUI_INCLUDE_IMGUI_USER_INL
#include "imgui_user.inl"
#endif

//-----------------------------------------------------------------------------
