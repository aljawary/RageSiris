// dear imgui, v1.77 WIP
// (main code and documentation)

// Help:
// - Read FAQ at http://dearimgui.org/faq
// - Newcomers, read 'Programmer guide' below for notes on how to setup Dear ImGui in your codebase.
// - Call and read ImGui::ShowDemoWindow() in imgui_demo.cpp. All applications in examples/ are doing that.
// Read imgui.cpp for details, links and comments.

// Resources:
// - FAQ                   http://dearimgui.org/faq
// - Homepage & latest     https://github.com/ocornut/imgui
// - Releases & changelog  https://github.com/ocornut/imgui/releases
// - Gallery               https://github.com/ocornut/imgui/issues/3075 (please post your screenshots/video there!)
// - Glossary              https://github.com/ocornut/imgui/wiki/Glossary
// - Wiki                  https://github.com/ocornut/imgui/wiki
// - Issues & support      https://github.com/ocornut/imgui/issues

// Developed by Omar Cornut and every direct or indirect contributors to the GitHub.
// See LICENSE.txt for copyright and licensing details (standard MIT License).
// This library is free but I need your support to sustain development and maintenance.
// Businesses: you can support continued development via invoiced technical support, maintenance and sponsoring contracts. Please reach out to "contact AT dearimgui.org".
// Individuals: you can support continued development via donations. See docs/README or web page.

// It is recommended that you don't modify imgui.cpp! It will become difficult for you to update the library.
// Note that 'ImGui::' being a namespace, you can add functions into the namespace from your own source files, without
// modifying imgui.h or imgui.cpp. You may include imgui_internal.h to access internal data structures, but it doesn't
// come with any guarantee of forward compatibility. Discussing your changes on the GitHub Issue Tracker may lead you
// to a better solution or official support for them.

/*

Index of this file:

DOCUMENTATION

- MISSION STATEMENT
- END-USER GUIDE
- PROGRAMMER GUIDE
  - READ FIRST
  - HOW TO UPDATE TO A NEWER VERSION OF DEAR IMGUI
  - GETTING STARTED WITH INTEGRATING DEAR IMGUI IN YOUR CODE/ENGINE
  - HOW A SIMPLE APPLICATION MAY LOOK LIKE
  - HOW A SIMPLE RENDERING FUNCTION MAY LOOK LIKE
  - USING GAMEPAD/KEYBOARD NAVIGATION CONTROLS
- API BREAKING CHANGES (read me when you update!)
- FREQUENTLY ASKED QUESTIONS (FAQ)
  - Read all answers online: https://www.dearimgui.org/faq, or in docs/FAQ.md (with a Markdown viewer)

CODE
(search for "[SECTION]" in the code to find them)

// [SECTION] INCLUDES
// [SECTION] FORWARD DECLARATIONS
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
// [SECTION] USER FACING STRUCTURES (ImGuiStyle, ImGuiIO)
// [SECTION] MISC HELPERS/UTILITIES (Geometry functions)
// [SECTION] MISC HELPERS/UTILITIES (String, Format, Hash functions)
// [SECTION] MISC HELPERS/UTILITIES (File functions)
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
// [SECTION] MISC HELPERS/UTILITIES (Color functions)
// [SECTION] ImGuiStorage
// [SECTION] ImGuiTextFilter
// [SECTION] ImGuiTextBuffer
// [SECTION] ImGuiListClipper
// [SECTION] STYLING
// [SECTION] RENDER HELPERS
// [SECTION] MAIN CODE (most of the code! lots of stuff, needs tidying up!)
// [SECTION] ERROR CHECKING
// [SECTION] LAYOUT
// [SECTION] SCROLLING
// [SECTION] TOOLTIPS
// [SECTION] POPUPS
// [SECTION] KEYBOARD/GAMEPAD NAVIGATION
// [SECTION] DRAG AND DROP
// [SECTION] LOGGING/CAPTURING
// [SECTION] SETTINGS
// [SECTION] PLATFORM DEPENDENT HELPERS
// [SECTION] METRICS/DEBUG WINDOW

*/

//-----------------------------------------------------------------------------
// DOCUMENTATION
//-----------------------------------------------------------------------------

/*

 MISSION STATEMENT
 =================

 - Easy to use to create code-driven and data-driven tools.
 - Easy to use to create ad hoc short-lived tools and long-lived, more elaborate tools.
 - Easy to hack and improve.
 - Minimize screen real-estate usage.
 - Minimize setup and maintenance.
 - Minimize state storage on user side.
 - Portable, minimize dependencies, run on target (consoles, phones, etc.).
 - Efficient runtime and memory consumption (NB- we do allocate when "growing" content e.g. creating a window,.
   opening a tree node for the first time, etc. but a typical frame should not allocate anything).

 Designed for developers and content-creators, not the typical end-user! Some of the weaknesses includes:
 - Doesn't look fancy, doesn't animate.
 - Limited layout features, intricate layouts are typically crafted in code.


 END-USER GUIDE
 ==============

 - Double-click on title bar to collapse window.
 - Click upper right corner to close a window, available when 'bool* p_open' is passed to ImGui::Begin().
 - Click and drag on lower right corner to resize window (double-click to auto fit window to its contents).
 - Click and drag on any empty space to move window.
 - TAB/SHIFT+TAB to cycle through keyboard editable fields.
 - CTRL+Click on a slider or drag box to input value as text.
 - Use mouse wheel to scroll.
 - Text editor:
   - Hold SHIFT or use mouse to select text.
   - CTRL+Left/Right to word jump.
   - CTRL+Shift+Left/Right to select words.
   - CTRL+A our Double-Click to select all.
   - CTRL+X,CTRL+C,CTRL+V to use OS clipboard/
   - CTRL+Z,CTRL+Y to undo/redo.
   - ESCAPE to revert text to its original value.
   - You can apply arithmetic operators +,*,/ on numerical values. Use +- to subtract (because - would set a negative value!)
   - Controls are automatically adjusted for OSX to match standard OSX text editing operations.
 - General Keyboard controls: enable with ImGuiConfigFlags_NavEnableKeyboard.
 - General Gamepad controls: enable with ImGuiConfigFlags_NavEnableGamepad. See suggested mappings in imgui.h ImGuiNavInput_ + download PNG/PSD at http://goo.gl/9LgVZW


 PROGRAMMER GUIDE
 ================

 READ FIRST
 ----------
 - Remember to read the FAQ (https://www.dearimgui.org/faq)
 - Your code creates the UI, if your code doesn't run the UI is gone! The UI can be highly dynamic, there are no construction
   or destruction steps, less superfluous data retention on your side, less state duplication, less state synchronization, less bugs.
 - Call and read ImGui::ShowDemoWindow() for demo code demonstrating most features.
 - The library is designed to be built from sources. Avoid pre-compiled binaries and packaged versions. See imconfig.h to configure your build.
 - Dear ImGui is an implementation of the IMGUI paradigm (immediate-mode graphical user interface, a term coined by Casey Muratori).
   You can learn about IMGUI principles at http://www.johno.se/book/imgui.html, http://mollyrocket.com/861 & more links in the FAQ.
 - Dear ImGui is a "single pass" rasterizing implementation of the IMGUI paradigm, aimed at ease of use and high-performances.
   For every application frame your UI code will be called only once. This is in contrast to e.g. Unity's own implementation of an IMGUI,
   where the UI code is called multiple times ("multiple passes") from a single entry point. There are pros and cons to both approaches.
 - Our origin are on the top-left. In axis aligned bounding boxes, Min = top-left, Max = bottom-right.
 - This codebase is also optimized to yield decent performances with typical "Debug" builds settings.
 - Please make sure you have asserts enabled (IM_ASSERT redirects to assert() by default, but can be redirected).
   If you get an assert, read the messages and comments around the assert.
 - C++: this is a very C-ish codebase: we don't rely on C++11, we don't include any C++ headers, and ImGui:: is a namespace.
 - C++: ImVec2/ImVec4 do not expose math operators by default, because it is expected that you use your own math types.
   See FAQ "How can I use my own math types instead of ImVec2/ImVec4?" for details about setting up imconfig.h for that.
   However, imgui_internal.h can optionally export math operators for ImVec2/ImVec4, which we use in this codebase.
 - C++: pay attention that ImVector<> manipulates plain-old-data and does not honor construction/destruction (avoid using it in your code!).


 HOW TO UPDATE TO A NEWER VERSION OF DEAR IMGUI
 ----------------------------------------------
 - Overwrite all the sources files except for imconfig.h (if you have made modification to your copy of imconfig.h)
 - Or maintain your own branch where you have imconfig.h modified.
 - Read the "API BREAKING CHANGES" section (below). This is where we list occasional API breaking changes.
   If a function/type has been renamed / or marked obsolete, try to fix the name in your code before it is permanently removed
   from the public API. If you have a problem with a missing function/symbols, search for its name in the code, there will
   likely be a comment about it. Please report any issue to the GitHub page!
 - Try to keep your copy of dear imgui reasonably up to date.


 GETTING STARTED WITH INTEGRATING DEAR IMGUI IN YOUR CODE/ENGINE
 ---------------------------------------------------------------
 - Run and study the examples and demo in imgui_demo.cpp to get acquainted with the library.
 - In the majority of cases you should be able to use unmodified back-ends files available in the examples/ folder.
 - Add the Dear ImGui source files to your projects or using your preferred build system.
   It is recommended you build and statically link the .cpp files as part of your project and NOT as shared library (DLL).
 - You can later customize the imconfig.h file to tweak some compile-time behavior, such as integrating Dear ImGui types with your own maths types.
 - When using Dear ImGui, your programming IDE is your friend: follow the declaration of variables, functions and types to find comments about them.
 - Dear ImGui never touches or knows about your GPU state. The only function that knows about GPU is the draw function that you provide.
   Effectively it means you can create widgets at any time in your code, regardless of considerations of being in "update" vs "render"
   phases of your own application. All rendering information are stored into command-lists that you will retrieve after calling ImGui::Render().
 - Refer to the bindings and demo applications in the examples/ folder for instruction on how to setup your code.
 - If you are running over a standard OS with a common graphics API, you should be able to use unmodified imgui_impl_*** files from the examples/ folder.


 HOW A SIMPLE APPLICATION MAY LOOK LIKE
 --------------------------------------
 EXHIBIT 1: USING THE EXAMPLE BINDINGS (= imgui_impl_XXX.cpp files from the examples/ folder).
 The sub-folders in examples/ contains examples applications following this structure.

     // Application init: create a dear imgui context, setup some options, load fonts
     ImGui::CreateContext();
     ImGuiIO& io = ImGui::GetIO();
     // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
     // TODO: Fill optional fields of the io structure later.
     // TODO: Load TTF/OTF fonts if you don't want to use the default font.

     // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32.cpp and imgui_impl_dx11.cpp)
     ImGui_ImplWin32_Init(hwnd);
     ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

     // Application main loop
     while (true)
     {
         // Feed inputs to dear imgui, start new frame
         ImGui_ImplDX11_NewFrame();
         ImGui_ImplWin32_NewFrame();
         ImGui::NewFrame();

         // Any application code here
         ImGui::Text("Hello, world!");

         // Render dear imgui into screen
         ImGui::Render();
         ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
         g_pSwapChain->Present(1, 0);
     }

     // Shutdown
     ImGui_ImplDX11_Shutdown();
     ImGui_ImplWin32_Shutdown();
     ImGui::DestroyContext();

 EXHIBIT 2: IMPLEMENTING CUSTOM BINDING / CUSTOM ENGINE

     // Application init: create a dear imgui context, setup some options, load fonts
     ImGui::CreateContext();
     ImGuiIO& io = ImGui::GetIO();
     // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
     // TODO: Fill optional fields of the io structure later.
     // TODO: Load TTF/OTF fonts if you don't want to use the default font.

     // Build and load the texture atlas into a texture
     // (In the examples/ app this is usually done within the ImGui_ImplXXX_Init() function from one of the demo Renderer)
     int width, height;
     unsigned char* pixels = NULL;
     io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

     // At this point you've got the texture data and you need to upload that your your graphic system:
     // After we have created the texture, store its pointer/identifier (_in whichever format your engine uses_) in 'io.Fonts->TexID'.
     // This will be passed back to your via the renderer. Basically ImTextureID == void*. Read FAQ for details about ImTextureID.
     MyTexture* texture = MyEngine::CreateTextureFromMemoryPixels(pixels, width, height, TEXTURE_TYPE_RGBA32)
     io.Fonts->TexID = (void*)texture;

     // Application main loop
     while (true)
     {
        // Setup low-level inputs, e.g. on Win32: calling GetKeyboardState(), or write to those fields from your Windows message handlers, etc.
        // (In the examples/ app this is usually done within the ImGui_ImplXXX_NewFrame() function from one of the demo Platform bindings)
        io.DeltaTime = 1.0f/60.0f;              // set the time elapsed since the previous frame (in seconds)
        io.DisplaySize.x = 1920.0f;             // set the current display width
        io.DisplaySize.y = 1280.0f;             // set the current display height here
        io.MousePos = my_mouse_pos;             // set the mouse position
        io.MouseDown[0] = my_mouse_buttons[0];  // set the mouse button states
        io.MouseDown[1] = my_mouse_buttons[1];

        // Call NewFrame(), after this point you can use ImGui::* functions anytime
        // (So you want to try calling NewFrame() as early as you can in your mainloop to be able to use Dear ImGui everywhere)
        ImGui::NewFrame();

        // Most of your application code here
        ImGui::Text("Hello, world!");
        MyGameUpdate(); // may use any Dear ImGui functions, e.g. ImGui::Begin("My window"); ImGui::Text("Hello, world!"); ImGui::End();
        MyGameRender(); // may use any Dear ImGui functions as well!

        // Render dear imgui, swap buffers
        // (You want to try calling EndFrame/Render as late as you can, to be able to use Dear ImGui in your own game rendering code)
        ImGui::EndFrame();
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        MyImGuiRenderFunction(draw_data);
        SwapBuffers();
     }

     // Shutdown
     ImGui::DestroyContext();

 To decide whether to dispatch mouse/keyboard inputs to Dear ImGui to the rest your application,
 you should read the 'io.WantCaptureMouse', 'io.WantCaptureKeyboard' and 'io.WantTextInput' flags!
 Please read the FAQ and example applications for details about this!


 HOW A SIMPLE RENDERING FUNCTION MAY LOOK LIKE
 ---------------------------------------------
 The bindings in impl_impl_XXX.cpp files contains many working implementations of a rendering function.

    void void MyImGuiRenderFunction(ImDrawData* draw_data)
    {
       // TODO: Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
       // TODO: Setup viewport covering draw_data->DisplayPos to draw_data->DisplayPos + draw_data->DisplaySize
       // TODO: Setup orthographic projection matrix cover draw_data->DisplayPos to draw_data->DisplayPos + draw_data->DisplaySize
       // TODO: Setup shader: vertex { float2 pos, float2 uv, u32 color }, fragment shader sample color from 1 texture, multiply by vertex color.
       for (int n = 0; n < draw_data->CmdListsCount; n++)
       {
          const ImDrawList* cmd_list = draw_data->CmdLists[n];
          const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;  // vertex buffer generated by Dear ImGui
          const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;   // index buffer generated by Dear ImGui
          for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
          {
             const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
             if (pcmd->UserCallback)
             {
                 pcmd->UserCallback(cmd_list, pcmd);
             }
             else
             {
                 // The texture for the draw call is specified by pcmd->TextureId.
                 // The vast majority of draw calls will use the Dear ImGui texture atlas, which value you have set yourself during initialization.
                 MyEngineBindTexture((MyTexture*)pcmd->TextureId);

                 // We are using scissoring to clip some objects. All low-level graphics API should supports it.
                 // - If your engine doesn't support scissoring yet, you may ignore this at first. You will get some small glitches
                 //   (some elements visible outside their bounds) but you can fix that once everything else works!
                 // - Clipping coordinates are provided in imgui coordinates space (from draw_data->DisplayPos to draw_data->DisplayPos + draw_data->DisplaySize)
                 //   In a single viewport application, draw_data->DisplayPos will always be (0,0) and draw_data->DisplaySize will always be == io.DisplaySize.
                 //   However, in the interest of supporting multi-viewport applications in the future (see 'viewport' branch on github),
                 //   always subtract draw_data->DisplayPos from clipping bounds to convert them to your viewport space.
                 // - Note that pcmd->ClipRect contains Min+Max bounds. Some graphics API may use Min+Max, other may use Min+Size (size being Max-Min)
                 ImVec2 pos = draw_data->DisplayPos;
                 MyEngineScissor((int)(pcmd->ClipRect.x - pos.x), (int)(pcmd->ClipRect.y - pos.y), (int)(pcmd->ClipRect.z - pos.x), (int)(pcmd->ClipRect.w - pos.y));

                 // Render 'pcmd->ElemCount/3' indexed triangles.
                 // By default the indices ImDrawIdx are 16-bit, you can change them to 32-bit in imconfig.h if your engine doesn't support 16-bit indices.
                 MyEngineDrawIndexedTriangles(pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer, vtx_buffer);
             }
             idx_buffer += pcmd->ElemCount;
          }
       }
    }


 USING GAMEPAD/KEYBOARD NAVIGATION CONTROLS
 ------------------------------------------
 - The gamepad/keyboard navigation is fairly functional and keeps being improved.
 - Gamepad support is particularly useful to use Dear ImGui on a console system (e.g. PS4, Switch, XB1) without a mouse!
 - You can ask questions and report issues at https://github.com/ocornut/imgui/issues/787
 - The initial focus was to support game controllers, but keyboard is becoming increasingly and decently usable.
 - Keyboard:
    - Set io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard to enable.
      NewFrame() will automatically fill io.NavInputs[] based on your io.KeysDown[] + io.KeyMap[] arrays.
    - When keyboard navigation is active (io.NavActive + ImGuiConfigFlags_NavEnableKeyboard), the io.WantCaptureKeyboard flag
      will be set. For more advanced uses, you may want to read from:
       - io.NavActive: true when a window is focused and it doesn't have the ImGuiWindowFlags_NoNavInputs flag set.
       - io.NavVisible: true when the navigation cursor is visible (and usually goes false when mouse is used).
       - or query focus information with e.g. IsWindowFocused(ImGuiFocusedFlags_AnyWindow), IsItemFocused() etc. functions.
      Please reach out if you think the game vs navigation input sharing could be improved.
 - Gamepad:
    - Set io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad to enable.
    - Backend: Set io.BackendFlags |= ImGuiBackendFlags_HasGamepad + fill the io.NavInputs[] fields before calling NewFrame().
      Note that io.NavInputs[] is cleared by EndFrame().
    - See 'enum ImGuiNavInput_' in imgui.h for a description of inputs. For each entry of io.NavInputs[], set the following values:
         0.0f= not held. 1.0f= fully held. Pass intermediate 0.0f..1.0f values for analog triggers/sticks.
    - We uses a simple >0.0f test for activation testing, and won't attempt to test for a dead-zone.
      Your code will probably need to transform your raw inputs (such as e.g. remapping your 0.2..0.9 raw input range to 0.0..1.0 imgui range, etc.).
    - You can download PNG/PSD files depicting the gamepad controls for common controllers at: http://goo.gl/9LgVZW.
    - If you need to share inputs between your game and the imgui parts, the easiest approach is to go all-or-nothing, with a buttons combo
      to toggle the target. Please reach out if you think the game vs navigation input sharing could be improved.
 - Mouse:
    - PS4 users: Consider emulating a mouse cursor with DualShock4 touch pad or a spare analog stick as a mouse-emulation fallback.
    - Consoles/Tablet/Phone users: Consider using a Synergy 1.x server (on your PC) + uSynergy.c (on your console/tablet/phone app) to share your PC mouse/keyboard.
    - On a TV/console system where readability may be lower or mouse inputs may be awkward, you may want to set the ImGuiConfigFlags_NavEnableSetMousePos flag.
      Enabling ImGuiConfigFlags_NavEnableSetMousePos + ImGuiBackendFlags_HasSetMousePos instructs dear imgui to move your mouse cursor along with navigation movements.
      When enabled, the NewFrame() function may alter 'io.MousePos' and set 'io.WantSetMousePos' to notify you that it wants the mouse cursor to be moved.
      When that happens your back-end NEEDS to move the OS or underlying mouse cursor on the next frame. Some of the binding in examples/ do that.
      (If you set the NavEnableSetMousePos flag but don't honor 'io.WantSetMousePos' properly, imgui will misbehave as it will see your mouse as moving back and forth!)
      (In a setup when you may not have easy control over the mouse cursor, e.g. uSynergy.c doesn't expose moving remote mouse cursor, you may want
       to set a boolean to ignore your other external mouse positions until the external source is moved again.)


 API BREAKING CHANGES
 ====================

 Occasionally introducing changes that are breaking the API. We try to make the breakage minor and easy to fix.
 Below is a change-log of API breaking changes only. If you are using one of the functions listed, expect to have to fix some code.
 When you are not sure about a old symbol or function name, try using the Search/Find function of your IDE to look for comments or references in all imgui files.
 You can read releases logs https://github.com/ocornut/imgui/releases for more details.

 - 2020/04/23 (1.77) - Removed unnecessary ID (first arg) of ImFontAtlas::AddCustomRectRegular().
 - 2020/01/22 (1.75) - ImDrawList::AddCircle()/AddCircleFilled() functions don't accept negative radius any more.
 - 2019/12/17 (1.75) - [undid this change in 1.76] made Columns() limited to 64 columns by asserting above that limit. While the current code technically supports it, future code may not so we're putting the restriction ahead.
 - 2019/12/13 (1.75) - [imgui_internal.h] changed ImRect() default constructor initializes all fields to 0.0f instead of (FLT_MAX,FLT_MAX,-FLT_MAX,-FLT_MAX). If you used ImRect::Add() to create bounding boxes by adding multiple points into it, you may need to fix your initial value.
 - 2019/12/08 (1.75) - removed redirecting functions/enums that were marked obsolete in 1.53 (December 2017):
                       - ShowTestWindow()                    -> use ShowDemoWindow()
                       - IsRootWindowFocused()               -> use IsWindowFocused(ImGuiFocusedFlags_RootWindow)
                       - IsRootWindowOrAnyChildFocused()     -> use IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
                       - SetNextWindowContentWidth(w)        -> use SetNextWindowContentSize(ImVec2(w, 0.0f)
                       - GetItemsLineHeightWithSpacing()     -> use GetFrameHeightWithSpacing()
                       - ImGuiCol_ChildWindowBg              -> use ImGuiCol_ChildBg
                       - ImGuiStyleVar_ChildWindowRounding   -> use ImGuiStyleVar_ChildRounding
                       - ImGuiTreeNodeFlags_AllowOverlapMode -> use ImGuiTreeNodeFlags_AllowItemOverlap
                       - IMGUI_DISABLE_TEST_WINDOWS          -> use IMGUI_DISABLE_DEMO_WINDOWS
 - 2019/12/08 (1.75) - obsoleted calling ImDrawList::PrimReserve() with a negative count (which was the vaguely documented and rarely if ever used). Instead we added an explicit PrimUnreserve() API.
 - 2019/12/06 (1.75) - removed implicit default parameter to IsMouseDragging(int button = 0) to be consistent with other mouse functions (none of the other functions have it).
 - 2019/11/21 (1.74) - ImFontAtlas::AddCustomRectRegular() now requires an ID larger than 0x110000 (instead of 0x10000) to conform with supporting Unicode planes 1-16 in a future update. ID below 0x110000 will now assert.
 - 2019/11/19 (1.74) - renamed IMGUI_DISABLE_FORMAT_STRING_FUNCTIONS to IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS for consistency.
 - 2019/11/19 (1.74) - renamed IMGUI_DISABLE_MATH_FUNCTIONS to IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS for consistency.
 - 2019/10/22 (1.74) - removed redirecting functions/enums that were marked obsolete in 1.52 (October 2017):
                       - Begin() [old 5 args version]        -> use Begin() [3 args], use SetNextWindowSize() SetNextWindowBgAlpha() if needed
                       - IsRootWindowOrAnyChildHovered()     -> use IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows)
                       - AlignFirstTextHeightToWidgets()     -> use AlignTextToFramePadding()
                       - SetNextWindowPosCenter()            -> use SetNextWindowPos() with a pivot of (0.5f, 0.5f)
                       - ImFont::Glyph                       -> use ImFontGlyph
 - 2019/10/14 (1.74) - inputs: Fixed a miscalculation in the keyboard/mouse "typematic" repeat delay/rate calculation, used by keys and e.g. repeating mouse buttons as well as the GetKeyPressedAmount() function.
                       if you were using a non-default value for io.KeyRepeatRate (previous default was 0.250), you can add +io.KeyRepeatDelay to it to compensate for the fix.
                       The function was triggering on: 0.0 and (delay+rate*N) where (N>=1). Fixed formula responds to (N>=0). Effectively it made io.KeyRepeatRate behave like it was set to (io.KeyRepeatRate + io.KeyRepeatDelay).
                       If you never altered io.KeyRepeatRate nor used GetKeyPressedAmount() this won't affect you.
 - 2019/07/15 (1.72) - removed TreeAdvanceToLabelPos() which is rarely used and only does SetCursorPosX(GetCursorPosX() + GetTreeNodeToLabelSpacing()). Kept redirection function (will obsolete).
 - 2019/07/12 (1.72) - renamed ImFontAtlas::CustomRect to ImFontAtlasCustomRect. Kept redirection typedef (will obsolete).
 - 2019/06/14 (1.72) - removed redirecting functions/enums names that were marked obsolete in 1.51 (June 2017): ImGuiCol_Column*, ImGuiSetCond_*, IsItemHoveredRect(), IsPosHoveringAnyWindow(), IsMouseHoveringAnyWindow(), IsMouseHoveringWindow(), IMGUI_ONCE_UPON_A_FRAME. Grep this log for details and new names, or see how they were implemented until 1.71.
 - 2019/06/07 (1.71) - rendering of child window outer decorations (bg color, border, scrollbars) is now performed as part of the parent window. If you have
                       overlapping child windows in a same parent, and relied on their relative z-order to be mapped to their submission order, this will affect your rendering.
                       This optimization is disabled if the parent window has no visual output, because it appears to be the most common situation leading to the creation of overlapping child windows.
                       Please reach out if you are affected.
 - 2019/05/13 (1.71) - renamed SetNextTreeNodeOpen() to SetNextItemOpen(). Kept inline redirection function (will obsolete).
 - 2019/05/11 (1.71) - changed io.AddInputCharacter(unsigned short c) signature to io.AddInputCharacter(unsigned int c).
 - 2019/04/29 (1.70) - improved ImDrawList thick strokes (>1.0f) preserving correct thickness up to 90 degrees angles (e.g. rectangles). If you have custom rendering using thick lines, they will appear thicker now.
 - 2019/04/29 (1.70) - removed GetContentRegionAvailWidth(), use GetContentRegionAvail().x instead. Kept inline redirection function (will obsolete).
 - 2019/03/04 (1.69) - renamed GetOverlayDrawList() to GetForegroundDrawList(). Kept redirection function (will obsolete).
 - 2019/02/26 (1.69) - renamed ImGuiColorEditFlags_RGB/ImGuiColorEditFlags_HSV/ImGuiColorEditFlags_HEX to ImGuiColorEditFlags_DisplayRGB/ImGuiColorEditFlags_DisplayHSV/ImGuiColorEditFlags_DisplayHex. Kept redirection enums (will obsolete).
 - 2019/02/14 (1.68) - made it illegal/assert when io.DisplayTime == 0.0f (with an exception for the first frame). If for some reason your time step calculation gives you a zero value, replace it with a dummy small value!
 - 2019/02/01 (1.68) - removed io.DisplayVisibleMin/DisplayVisibleMax (which were marked obsolete and removed from viewport/docking branch already).
 - 2019/01/06 (1.67) - renamed io.InputCharacters[], marked internal as was always intended. Please don't access directly, and use AddInputCharacter() instead!
 - 2019/01/06 (1.67) - renamed ImFontAtlas::GlyphRangesBuilder to ImFontGlyphRangesBuilder. Kept redirection typedef (will obsolete).
 - 2018/12/20 (1.67) - made it illegal to call Begin("") with an empty string. This somehow half-worked before but had various undesirable side-effects.
 - 2018/12/10 (1.67) - renamed io.ConfigResizeWindowsFromEdges to io.ConfigWindowsResizeFromEdges as we are doing a large pass on configuration flags.
 - 2018/10/12 (1.66) - renamed misc/stl/imgui_stl.* to misc/cpp/imgui_stdlib.* in prevision for other C++ helper files.
 - 2018/09/28 (1.66) - renamed SetScrollHere() to SetScrollHereY(). Kept redirection function (will obsolete).
 - 2018/09/06 (1.65) - renamed stb_truetype.h to imstb_truetype.h, stb_textedit.h to imstb_textedit.h, and stb_rect_pack.h to imstb_rectpack.h.
                       If you were conveniently using the imgui copy of those STB headers in your project you will have to update your include paths.
 - 2018/09/05 (1.65) - renamed io.OptCursorBlink/io.ConfigCursorBlink to io.ConfigInputTextCursorBlink. (#1427)
 - 2018/08/31 (1.64) - added imgui_widgets.cpp file, extracted and moved widgets code out of imgui.cpp into imgui_widgets.cpp. Re-ordered some of the code remaining in imgui.cpp.
                       NONE OF THE FUNCTIONS HAVE CHANGED. THE CODE IS SEMANTICALLY 100% IDENTICAL, BUT _EVERY_ FUNCTION HAS BEEN MOVED.
                       Because of this, any local modifications to imgui.cpp will likely conflict when you update. Read docs/CHANGELOG.txt for suggestions.
 - 2018/08/22 (1.63) - renamed IsItemDeactivatedAfterChange() to IsItemDeactivatedAfterEdit() for consistency with new IsItemEdited() API. Kept redirection function (will obsolete soonish as IsItemDeactivatedAfterChange() is very recent).
 - 2018/08/21 (1.63) - renamed ImGuiTextEditCallback to ImGuiInputTextCallback, ImGuiTextEditCallbackData to ImGuiInputTextCallbackData for consistency. Kept redirection types (will obsolete).
 - 2018/08/21 (1.63) - removed ImGuiInputTextCallbackData::ReadOnly since it is a duplication of (ImGuiInputTextCallbackData::Flags & ImGuiInputTextFlags_ReadOnly).
 - 2018/08/01 (1.63) - removed per-window ImGuiWindowFlags_ResizeFromAnySide beta flag in favor of a global io.ConfigResizeWindowsFromEdges [update 1.67 renamed to ConfigWindowsResizeFromEdges] to enable the feature.
 - 2018/08/01 (1.63) - renamed io.OptCursorBlink to io.ConfigCursorBlink [-> io.ConfigInputTextCursorBlink in 1.65], io.OptMacOSXBehaviors to ConfigMacOSXBehaviors for consistency.
 - 2018/07/22 (1.63) - changed ImGui::GetTime() return value from float to double to avoid accumulating floating point imprecisions over time.
 - 2018/07/08 (1.63) - style: renamed ImGuiCol_ModalWindowDarkening to ImGuiCol_ModalWindowDimBg for consistency with other features. Kept redirection enum (will obsolete).
 - 2018/06/08 (1.62) - examples: the imgui_impl_xxx files have been split to separate platform (Win32, Glfw, SDL2, etc.) from renderer (DX11, OpenGL, Vulkan,  etc.).
                       old bindings will still work as is, however prefer using the separated bindings as they will be updated to support multi-viewports.
                       when adopting new bindings follow the main.cpp code of your preferred examples/ folder to know which functions to call.
                       in particular, note that old bindings called ImGui::NewFrame() at the end of their ImGui_ImplXXXX_NewFrame() function.
 - 2018/06/06 (1.62) - renamed GetGlyphRangesChinese() to GetGlyphRangesChineseFull() to distinguish other variants and discourage using the full set.
 - 2018/06/06 (1.62) - TreeNodeEx()/TreeNodeBehavior(): the ImGuiTreeNodeFlags_CollapsingHeader helper now include the ImGuiTreeNodeFlags_NoTreePushOnOpen flag. See Changelog for details.
 - 2018/05/03 (1.61) - DragInt(): the default compile-time format string has been changed from "%.0f" to "%d", as we are not using integers internally any more.
                       If you used DragInt() with custom format strings, make sure you change them to use %d or an integer-compatible format.
                       To honor backward-compatibility, the DragInt() code will currently parse and modify format strings to replace %*f with %d, giving time to users to upgrade their code.
                       If you have IMGUI_DISABLE_OBSOLETE_FUNCTIONS enabled, the code will instead assert! You may run a reg-exp search on your codebase for e.g. "DragInt.*%f" to help you find them.
 - 2018/04/28 (1.61) - obsoleted InputFloat() functions taking an optional "int decimal_precision" in favor of an equivalent and more flexible "const char* format",
                       consistent with other functions. Kept redirection functions (will obsolete).
 - 2018/04/09 (1.61) - IM_DELETE() helper function added in 1.60 doesn't clear the input _pointer_ reference, more consistent with expectation and allows passing r-value.
 - 2018/03/20 (1.60) - renamed io.WantMoveMouse to io.WantSetMousePos for consistency and ease of understanding (was added in 1.52, _not_ used by core and only honored by some binding ahead of merging the Nav branch).
 - 2018/03/12 (1.60) - removed ImGuiCol_CloseButton, ImGuiCol_CloseButtonActive, ImGuiCol_CloseButtonHovered as the closing cross uses regular button colors now.
 - 2018/03/08 (1.60) - changed ImFont::DisplayOffset.y to default to 0 instead of +1. Fixed rounding of Ascent/Descent to match TrueType renderer. If you were adding or subtracting to ImFont::DisplayOffset check if your fonts are correctly aligned vertically.
 - 2018/03/03 (1.60) - renamed ImGuiStyleVar_Count_ to ImGuiStyleVar_COUNT and ImGuiMouseCursor_Count_ to ImGuiMouseCursor_COUNT for consistency with other public enums.
 - 2018/02/18 (1.60) - BeginDragDropSource(): temporarily removed the optional mouse_button=0 parameter because it is not really usable in many situations at the moment.
 - 2018/02/16 (1.60) - obsoleted the io.RenderDrawListsFn callback, you can call your graphics engine render function after ImGui::Render(). Use ImGui::GetDrawData() to retrieve the ImDrawData* to display.
 - 2018/02/07 (1.60) - reorganized context handling to be more explicit,
                       - YOU NOW NEED TO CALL ImGui::CreateContext() AT THE BEGINNING OF YOUR APP, AND CALL ImGui::DestroyContext() AT THE END.
                       - removed Shutdown() function, as DestroyContext() serve this purpose.
                       - you may pass a ImFontAtlas* pointer to CreateContext() to share a font atlas between contexts. Otherwise CreateContext() will create its own font atlas instance.
                       - removed allocator parameters from CreateContext(), they are now setup with SetAllocatorFunctions(), and shared by all contexts.
                       - removed the default global context and font atlas instance, which were confusing for users of DLL reloading and users of multiple contexts.
 - 2018/01/31 (1.60) - moved sample TTF files from extra_fonts/ to misc/fonts/. If you loaded files directly from the imgui repo you may need to update your paths.
 - 2018/01/11 (1.60) - obsoleted IsAnyWindowHovered() in favor of IsWindowHovered(ImGuiHoveredFlags_AnyWindow). Kept redirection function (will obsolete).
 - 2018/01/11 (1.60) - obsoleted IsAnyWindowFocused() in favor of IsWindowFocused(ImGuiFocusedFlags_AnyWindow). Kept redirection function (will obsolete).
 - 2018/01/03 (1.60) - renamed ImGuiSizeConstraintCallback to ImGuiSizeCallback, ImGuiSizeConstraintCallbackData to ImGuiSizeCallbackData.
 - 2017/12/29 (1.60) - removed CalcItemRectClosestPoint() which was weird and not really used by anyone except demo code. If you need it it's easy to replicate on your side.
 - 2017/12/24 (1.53) - renamed the emblematic ShowTestWindow() function to ShowDemoWindow(). Kept redirection function (will obsolete).
 - 2017/12/21 (1.53) - ImDrawList: renamed style.AntiAliasedShapes to style.AntiAliasedFill for consistency and as a way to explicitly break code that manipulate those flag at runtime. You can now manipulate ImDrawList::Flags
 - 2017/12/21 (1.53) - ImDrawList: removed 'bool anti_aliased = true' final parameter of ImDrawList::AddPolyline() and ImDrawList::AddConvexPolyFilled(). Prefer manipulating ImDrawList::Flags if you need to toggle them during the frame.
 - 2017/12/14 (1.53) - using the ImGuiWindowFlags_NoScrollWithMouse flag on a child window forwards the mouse wheel event to the parent window, unless either ImGuiWindowFlags_NoInputs or ImGuiWindowFlags_NoScrollbar are also set.
 - 2017/12/13 (1.53) - renamed GetItemsLineHeightWithSpacing() to GetFrameHeightWithSpacing(). Kept redirection function (will obsolete).
 - 2017/12/13 (1.53) - obsoleted IsRootWindowFocused() in favor of using IsWindowFocused(ImGuiFocusedFlags_RootWindow). Kept redirection function (will obsolete).
                     - obsoleted IsRootWindowOrAnyChildFocused() in favor of using IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows). Kept redirection function (will obsolete).
 - 2017/12/12 (1.53) - renamed ImGuiTreeNodeFlags_AllowOverlapMode to ImGuiTreeNodeFlags_AllowItemOverlap. Kept redirection enum (will obsolete).
 - 2017/12/10 (1.53) - removed SetNextWindowContentWidth(), prefer using SetNextWindowContentSize(). Kept redirection function (will obsolete).
 - 2017/11/27 (1.53) - renamed ImGuiTextBuffer::append() helper to appendf(), appendv() to appendfv(). If you copied the 'Log' demo in your code, it uses appendv() so that needs to be renamed.
 - 2017/11/18 (1.53) - Style, Begin: removed ImGuiWindowFlags_ShowBorders window flag. Borders are now fully set up in the ImGuiStyle structure (see e.g. style.FrameBorderSize, style.WindowBorderSize). Use ImGui::ShowStyleEditor() to look them up.
                       Please note that the style system will keep evolving (hopefully stabilizing in Q1 2018), and so custom styles will probably subtly break over time. It is recommended you use the StyleColorsClassic(), StyleColorsDark(), StyleColorsLight() functions.
 - 2017/11/18 (1.53) - Style: removed ImGuiCol_ComboBg in favor of combo boxes using ImGuiCol_PopupBg for consistency.
 - 2017/11/18 (1.53) - Style: renamed ImGuiCol_ChildWindowBg to ImGuiCol_ChildBg.
 - 2017/11/18 (1.53) - Style: renamed style.ChildWindowRounding to style.ChildRounding, ImGuiStyleVar_ChildWindowRounding to ImGuiStyleVar_ChildRounding.
 - 2017/11/02 (1.53) - obsoleted IsRootWindowOrAnyChildHovered() in favor of using IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
 - 2017/10/24 (1.52) - renamed IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS/IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCS to IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS/IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS for consistency.
 - 2017/10/20 (1.52) - changed IsWindowHovered() default parameters behavior to return false if an item is active in another window (e.g. click-dragging item from another window to this window). You can use the newly introduced IsWindowHovered() flags to requests this specific behavior if you need it.
 - 2017/10/20 (1.52) - marked IsItemHoveredRect()/IsMouseHoveringWindow() as obsolete, in favor of using the newly introduced flags for IsItemHovered() and IsWindowHovered(). See https://github.com/ocornut/imgui/issues/1382 for details.
                       removed the IsItemRectHovered()/IsWindowRectHovered() names introduced in 1.51 since they were merely more consistent names for the two functions we are now obsoleting.
                         IsItemHoveredRect()        --> IsItemHovered(ImGuiHoveredFlags_RectOnly)
                         IsMouseHoveringAnyWindow() --> IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
                         IsMouseHoveringWindow()    --> IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) [weird, old behavior]
 - 2017/10/17 (1.52) - marked the old 5-parameters version of Begin() as obsolete (still available). Use SetNextWindowSize()+Begin() instead!
 - 2017/10/11 (1.52) - renamed AlignFirstTextHeightToWidgets() to AlignTextToFramePadding(). Kept inline redirection function (will obsolete).
 - 2017/09/26 (1.52) - renamed ImFont::Glyph to ImFontGlyph. Kept redirection typedef (will obsolete).
 - 2017/09/25 (1.52) - removed SetNextWindowPosCenter() because SetNextWindowPos() now has the optional pivot information to do the same and more. Kept redirection function (will obsolete).
 - 2017/08/25 (1.52) - io.MousePos needs to be set to ImVec2(-FLT_MAX,-FLT_MAX) when mouse is unavailable/missing. Previously ImVec2(-1,-1) was enough but we now accept negative mouse coordinates. In your binding if you need to support unavailable mouse, make sure to replace "io.MousePos = ImVec2(-1,-1)" with "io.MousePos = ImVec2(-FLT_MAX,-FLT_MAX)".
 - 2017/08/22 (1.51) - renamed IsItemHoveredRect() to IsItemRectHovered(). Kept inline redirection function (will obsolete). -> (1.52) use IsItemHovered(ImGuiHoveredFlags_RectOnly)!
                     - renamed IsMouseHoveringAnyWindow() to IsAnyWindowHovered() for consistency. Kept inline redirection function (will obsolete).
                     - renamed IsMouseHoveringWindow() to IsWindowRectHovered() for consistency. Kept inline redirection function (will obsolete).
 - 2017/08/20 (1.51) - renamed GetStyleColName() to GetStyleColorName() for consistency.
 - 2017/08/20 (1.51) - added PushStyleColor(ImGuiCol idx, ImU32 col) overload, which _might_ cause an "ambiguous call" compilation error if you are using ImColor() with implicit cast. Cast to ImU32 or ImVec4 explicily to fix.
 - 2017/08/15 (1.51) - marked the weird IMGUI_ONCE_UPON_A_FRAME helper macro as obsolete. prefer using the more explicit ImGuiOnceUponAFrame type.
 - 2017/08/15 (1.51) - changed parameter order for BeginPopupContextWindow() from (const char*,int buttons,bool also_over_items) to (const char*,int buttons,bool also_over_items). Note that most calls relied on default parameters completely.
 - 2017/08/13 (1.51) - renamed ImGuiCol_Column to ImGuiCol_Separator, ImGuiCol_ColumnHovered to ImGuiCol_SeparatorHovered, ImGuiCol_ColumnActive to ImGuiCol_SeparatorActive. Kept redirection enums (will obsolete).
 - 2017/08/11 (1.51) - renamed ImGuiSetCond_Always to ImGuiCond_Always, ImGuiSetCond_Once to ImGuiCond_Once, ImGuiSetCond_FirstUseEver to ImGuiCond_FirstUseEver, ImGuiSetCond_Appearing to ImGuiCond_Appearing. Kept redirection enums (will obsolete).
 - 2017/08/09 (1.51) - removed ValueColor() helpers, they are equivalent to calling Text(label) + SameLine() + ColorButton().
 - 2017/08/08 (1.51) - removed ColorEditMode() and ImGuiColorEditMode in favor of ImGuiColorEditFlags and parameters to the various Color*() functions. The SetColorEditOptions() allows to initialize default but the user can still change them with right-click context menu.
                     - changed prototype of 'ColorEdit4(const char* label, float col[4], bool show_alpha = true)' to 'ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0)', where passing flags = 0x01 is a safe no-op (hello dodgy backward compatibility!). - check and run the demo window, under "Color/Picker Widgets", to understand the various new options.
                     - changed prototype of rarely used 'ColorButton(ImVec4 col, bool small_height = false, bool outline_border = true)' to 'ColorButton(const char* desc_id, ImVec4 col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0))'
 - 2017/07/20 (1.51) - removed IsPosHoveringAnyWindow(ImVec2), which was partly broken and misleading. ASSERT + redirect user to io.WantCaptureMouse
 - 2017/05/26 (1.50) - removed ImFontConfig::MergeGlyphCenterV in favor of a more multipurpose ImFontConfig::GlyphOffset.
 - 2017/05/01 (1.50) - renamed ImDrawList::PathFill() (rarely used directly) to ImDrawList::PathFillConvex() for clarity.
 - 2016/11/06 (1.50) - BeginChild(const char*) now applies the stack id to the provided label, consistently with other functions as it should always have been. It shouldn't affect you unless (extremely unlikely) you were appending multiple times to a same child from different locations of the stack id. If that's the case, generate an id with GetId() and use it instead of passing string to BeginChild().
 - 2016/10/15 (1.50) - avoid 'void* user_data' parameter to io.SetClipboardTextFn/io.GetClipboardTextFn pointers. We pass io.ClipboardUserData to it.
 - 2016/09/25 (1.50) - style.WindowTitleAlign is now a ImVec2 (ImGuiAlign enum was removed). set to (0.5f,0.5f) for horizontal+vertical centering, (0.0f,0.0f) for upper-left, etc.
 - 2016/07/30 (1.50) - SameLine(x) with x>0.0f is now relative to left of column/group if any, and not always to left of window. This was sort of always the intent and hopefully breakage should be minimal.
 - 2016/05/12 (1.49) - title bar (using ImGuiCol_TitleBg/ImGuiCol_TitleBgActive colors) isn't rendered over a window background (ImGuiCol_WindowBg color) anymore.
                       If your TitleBg/TitleBgActive alpha was 1.0f or you are using the default theme it will not affect you, otherwise if <1.0f you need tweak your custom theme to readjust for the fact that we don't draw a WindowBg background behind the title bar.
                       This helper function will convert an old TitleBg/TitleBgActive color into a new one with the same visual output, given the OLD color and the OLD WindowBg color:
                       ImVec4 ConvertTitleBgCol(const ImVec4& win_bg_col, const ImVec4& title_bg_col) { float new_a = 1.0f - ((1.0f - win_bg_col.w) * (1.0f - title_bg_col.w)), k = title_bg_col.w / new_a; return ImVec4((win_bg_col.x * win_bg_col.w + title_bg_col.x) * k, (win_bg_col.y * win_bg_col.w + title_bg_col.y) * k, (win_bg_col.z * win_bg_col.w + title_bg_col.z) * k, new_a); }
                       If this is confusing, pick the RGB value from title bar from an old screenshot and apply this as TitleBg/TitleBgActive. Or you may just create TitleBgActive from a tweaked TitleBg color.
 - 2016/05/07 (1.49) - removed confusing set of GetInternalState(), GetInternalStateSize(), SetInternalState() functions. Now using CreateContext(), DestroyContext(), GetCurrentContext(), SetCurrentContext().
 - 2016/05/02 (1.49) - renamed SetNextTreeNodeOpened() to SetNextTreeNodeOpen(), no redirection.
 - 2016/05/01 (1.49) - obsoleted old signature of CollapsingHeader(const char* label, const char* str_id = NULL, bool display_frame = true, bool default_open = false) as extra parameters were badly designed and rarely used. You can replace the "default_open = true" flag in new API with CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen).
 - 2016/04/26 (1.49) - changed ImDrawList::PushClipRect(ImVec4 rect) to ImDrawList::PushClipRect(Imvec2 min,ImVec2 max,bool intersect_with_current_clip_rect=false). Note that higher-level ImGui::PushClipRect() is preferable because it will clip at logic/widget level, whereas ImDrawList::PushClipRect() only affect your renderer.
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
                       old: ImGui_XXXX_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
                       new: ImGui_XXXX_RenderDrawLists(ImDrawData* draw_data).
                         parameters: 'cmd_lists' becomes 'draw_data->CmdLists', 'cmd_lists_count' becomes 'draw_data->CmdListsCount'
                         ImDrawList: 'commands' becomes 'CmdBuffer', 'vtx_buffer' becomes 'VtxBuffer', 'IdxBuffer' is new.
                         ImDrawCmd:  'vtx_count' becomes 'ElemCount', 'clip_rect' becomes 'ClipRect', 'user_callback' becomes 'UserCallback', 'texture_id' becomes 'TextureId'.
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
 - 2015/01/11 (1.30) - removed GetDefaultFontData(). uses io.Fonts->GetTextureData*() API to retrieve uncompressed pixels.
                       - old:  const void* png_data; unsigned int png_size; ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size); [..Upload texture to GPU..];
                       - new:  unsigned char* pixels; int width, height; io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height); [..Upload texture to GPU..]; io.Fonts->TexId = YourTexIdentifier;
                       you now have more flexibility to load multiple TTF fonts and manage the texture buffer for internal needs. It is now recommended that you sample the font texture with bilinear interpolation.
 - 2015/01/11 (1.30) - added texture identifier in ImDrawCmd passed to your render function (we can now render images). make sure to set io.Fonts->TexID.
 - 2015/01/11 (1.30) - removed IO.PixelCenterOffset (unnecessary, can be handled in user projection matrix)
 - 2015/01/11 (1.30) - removed ImGui::IsItemFocused() in favor of ImGui::IsItemActive() which handles all widgets
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


 FREQUENTLY ASKED QUESTIONS (FAQ)
 ================================

 Read all answers online: https://www.dearimgui.org/faq, or in docs/FAQ.md (with a Markdown viewer)
 Some answers are copied down here to facilitate searching in code.

 Q&A: Basics
 ===========

 Q: Where is the documentation?
 A: This library is poorly documented at the moment and expects of the user to be acquainted with C/C++.
    - Run the examples/ and explore them.
    - See demo code in imgui_demo.cpp and particularly the ImGui::ShowDemoWindow() function.
    - The demo covers most features of Dear ImGui, so you can read the code and see its output.
    - See documentation and comments at the top of imgui.cpp + effectively imgui.h.
    - Dozens of standalone example applications using e.g. OpenGL/DirectX are provided in the
      examples/ folder to explain how to integrate Dear ImGui with your own engine/application.
    - The Wiki (https://github.com/ocornut/imgui/wiki) has many resources and links.
    - The Glossary (https://github.com/ocornut/imgui/wiki/Glossary) page also may be useful.
    - Your programming IDE is your friend, find the type or function declaration to find comments
      associated to it.

 Q: What is this library called?
 Q: Which version should I get?
 >> This library is called "Dear ImGui", please don't call it "ImGui" :)
 >> See https://www.dearimgui.org/faq

 Q&A: Integration
 ================

 Q: How can I tell whether to dispatch mouse/keyboard to Dear ImGui or to my application?
 A: You should read the 'io.WantCaptureMouse', 'io.WantCaptureKeyboard' and 'io.WantTextInput' flags!
 >> See https://www.dearimgui.org/faq for fully detailed answer. You really want to read this.

 Q. How can I enable keyboard controls?
 Q: How can I use this without a mouse, without a keyboard or without a screen? (gamepad, input share, remote display)
 Q: I integrated Dear ImGui in my engine and the text or lines are blurry..
 Q: I integrated Dear ImGui in my engine and some elements are clipping or disappearing when I move windows around..
 Q: I integrated Dear ImGui in my engine and some elements are displaying outside their expected windows boundaries..
>> See https://www.dearimgui.org/faq

 Q&A: Usage
 ----------

 Q: Why are multiple widgets reacting when I interact with a single one?
 Q: How can I have multiple widgets with the same label or with an empty label?
 A: A primer on labels and the ID Stack...

    Dear ImGui internally need to uniquely identify UI elements.
    Elements that are typically not clickable (such as calls to the Text functions) don't need an ID.
    Interactive widgets (such as calls to Button buttons) need a unique ID.
    Unique ID are used internally to track active widgets and occasionally associate state to widgets.
    Unique ID are implicitly built from the hash of multiple elements that identify the "path" to the UI element.

   - Unique ID are often derived from a string label:

       Button("OK");          // Label = "OK",     ID = hash of (..., "OK")
       Button("Cancel");      // Label = "Cancel", ID = hash of (..., "Cancel")

   - ID are uniquely scoped within windows, tree nodes, etc. which all pushes to the ID stack. Having
     two buttons labeled "OK" in different windows or different tree locations is fine.
     We used "..." above to signify whatever was already pushed to the ID stack previously:

       Begin("MyWindow");
       Button("OK");          // Label = "OK",     ID = hash of ("MyWindow", "OK")
       End();
       Begin("MyOtherWindow");
       Button("OK");          // Label = "OK",     ID = hash of ("MyOtherWindow", "OK")
       End();

   - If you have a same ID twice in the same location, you'll have a conflict:

       Button("OK");
       Button("OK");          // ID collision! Interacting with either button will trigger the first one.

     Fear not! this is easy to solve and there are many ways to solve it!

   - Solving ID conflict in a simple/local context:
     When passing a label you can optionally specify extra ID information within string itself.
     Use "##" to pass a complement to the ID that won't be visible to the end-user.
     This helps solving the simple collision cases when you know e.g. at compilation time which items
     are going to be created:

       Begin("MyWindow");
       Button("Play");        // Label = "Play",   ID = hash of ("MyWindow", "Play")
       Button("Play##foo1");  // Label = "Play",   ID = hash of ("MyWindow", "Play##foo1")  // Different from above
       Button("Play##foo2");  // Label = "Play",   ID = hash of ("MyWindow", "Play##foo2")  // Different from above
       End();

   - If you want to completely hide the label, but still need an ID:

       Checkbox("##On", &b);  // Label = "",       ID = hash of (..., "##On")   // No visible label, just a checkbox!

   - Occasionally/rarely you might want change a label while preserving a constant ID. This allows
     you to animate labels. For example you may want to include varying information in a window title bar,
     but windows are uniquely identified by their ID. Use "###" to pass a label that isn't part of ID:

       Button("Hello###ID");  // Label = "Hello",  ID = hash of (..., "###ID")
       Button("World###ID");  // Label = "World",  ID = hash of (..., "###ID")  // Same as above, even though the label looks different

       sprintf(buf, "My game (%f FPS)###MyGame", fps);
       Begin(buf);            // Variable title,   ID = hash of "MyGame"

   - Solving ID conflict in a more general manner:
     Use PushID() / PopID() to create scopes and manipulate the ID stack, as to avoid ID conflicts
     within the same window. This is the most convenient way of distinguishing ID when iterating and
     creating many UI elements programmatically.
     You can push a pointer, a string or an integer value into the ID stack.
     Remember that ID are formed from the concatenation of _everything_ pushed into the ID stack.
     At each level of the stack we store the seed used for items at this level of the ID stack.

     Begin("Window");
       for (int i = 0; i < 100; i++)
       {
         PushID(i);           // Push i to the id tack
         Button("Click");     // Label = "Click",  ID = hash of ("Window", i, "Click")
         PopID();
       }
       for (int i = 0; i < 100; i++)
       {
         MyObject* obj = Objects[i];
         PushID(obj);
         Button("Click");     // Label = "Click",  ID = hash of ("Window", obj pointer, "Click")
         PopID();
       }
       for (int i = 0; i < 100; i++)
       {
         MyObject* obj = Objects[i];
         PushID(obj->Name);
         Button("Click");     // Label = "Click",  ID = hash of ("Window", obj->Name, "Click")
         PopID();
       }
       End();

   - You can stack multiple prefixes into the ID stack:

       Button("Click");       // Label = "Click",  ID = hash of (..., "Click")
       PushID("node");
       Button("Click");       // Label = "Click",  ID = hash of (..., "node", "Click")
         PushID(my_ptr);
           Button("Click");   // Label = "Click",  ID = hash of (..., "node", my_ptr, "Click")
         PopID();
       PopID();

   - Tree nodes implicitly creates a scope for you by calling PushID().

       Button("Click");       // Label = "Click",  ID = hash of (..., "Click")
       if (TreeNode("node"))  // <-- this function call will do a PushID() for you (unless instructed not to, with a special flag)
       {
         Button("Click");     // Label = "Click",  ID = hash of (..., "node", "Click")
         TreePop();
       }

   - When working with trees, ID are used to preserve the open/close state of each tree node.
     Depending on your use cases you may want to use strings, indices or pointers as ID.
      e.g. when following a single pointer that may change over time, using a static string as ID
       will preserve your node open/closed state when the targeted object change.
      e.g. when displaying a list of objects, using indices or pointers as ID will preserve the
       node open/closed state differently. See what makes more sense in your situation!

 Q: How can I display an image? What is ImTextureID, how does it works?
 >> See https://www.dearimgui.org/faq and https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples

 Q: How can I use my own math types instead of ImVec2/ImVec4?
 Q: How can I interact with standard C++ types (such as std::string and std::vector)?
 Q: How can I display custom shapes? (using low-level ImDrawList API)
 >> See https://www.dearimgui.org/faq

 Q&A: Fonts, Text
 ================

 Q: How should I handle DPI in my application?
 Q: How can I load a different font than the default?
 Q: How can I easily use icons in my application?
 Q: How can I load multiple fonts?
 Q: How can I display and input non-Latin characters such as Chinese, Japanese, Korean, Cyrillic?
 >> See https://www.dearimgui.org/faq and https://github.com/ocornut/imgui/edit/master/docs/FONTS.md

 Q&A: Concerns
 =============

 Q: Who uses Dear ImGui?
 Q: Can you create elaborate/serious tools with Dear ImGui?
 Q: Can you reskin the look of Dear ImGui?
 Q: Why using C++ (as opposed to C)?
 >> See https://www.dearimgui.org/faq

 Q&A: Community
 ==============

 Q: How can I help?
 A: - Businesses: please reach out to "contact AT dearimgui.org" if you work in a place using Dear ImGui!
      We can discuss ways for your company to fund development via invoiced technical support, maintenance or sponsoring contacts.
      This is among the most useful thing you can do for Dear ImGui. With increased funding we can hire more people working on this project.
    - Individuals: you can support continued development via PayPal donations. See README.
    - If you are experienced with Dear ImGui and C++, look at the github issues, look at the Wiki, read docs/TODO.txt
      and see how you want to help and can help!
    - Disclose your usage of Dear ImGui via a dev blog post, a tweet, a screenshot, a mention somewhere etc.
      You may post screenshot or links in the gallery threads (github.com/ocornut/imgui/issues/3075). Visuals are ideal as they inspire other programmers.
      But even without visuals, disclosing your use of dear imgui help the library grow credibility, and help other teams and programmers with taking decisions.
    - If you have issues or if you need to hack into the library, even if you don't expect any support it is useful that you share your issues (on github or privately).

*/

//-------------------------------------------------------------------------
// [SECTION] INCLUDES
//-------------------------------------------------------------------------

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DISABLE

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"

// System includes
#include <ctype.h>      // toupper
#include <stdio.h>      // vsnprintf, sscanf, printf
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

// [Windows] OS specific includes (optional)
#if defined(_WIN32) && defined(IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS) && defined(IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS) && defined(IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#define IMGUI_DISABLE_WIN32_FUNCTIONS
#endif
#if defined(_WIN32) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef __MINGW32__
#include <Windows.h>        // _wfopen, OpenClipboard
#else
#include <windows.h>
#endif
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP) // UWP doesn't have all Win32 functions
#define IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
#define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
#endif
#endif

// [Apple] OS specific includes
#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127)             // condition expression is constant
#pragma warning (disable: 4996)             // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#if defined(_MSC_VER) && _MSC_VER >= 1922   // MSVC 2019 16.2 or later
#pragma warning (disable: 5054)             // operator '|': deprecated between enumerations of different types
#endif
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunknown-pragmas"        // warning : unknown warning group '-Wformat-pedantic *'        // not all warnings are known by all clang versions.. so ignoring warnings triggers new warnings on some configuration. great!
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants (typically 0.0f) is ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning : format string is not a string literal              // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wexit-time-destructors"  // warning : declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. ImGui coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference is.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#pragma clang diagnostic ignored "-Wformat-pedantic"        // warning : format specifies type 'void *' but the argument has type 'xxxx *' // unreasonable, would lead to casting every %p arg to void*. probably enabled by -pedantic.
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"       // warning : cast to 'void *' from smaller integer type 'int'
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant              // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"       // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#elif defined(__GNUC__)
// We disable -Wpragmas because GCC doesn't provide an has_warning equivalent and some forks/patches may not following the warning/version association.
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat"                   // warning: format '%p' expects argument of type 'void*', but argument 6 has type 'ImGuiWindow*'
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wformat-nonliteral"        // warning: format not a string literal, format string not checked
#pragma GCC diagnostic ignored "-Wstrict-overflow"          // warning: assuming signed overflow does not occur when assuming that (X - c) > X is always false
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

// Debug options
#define IMGUI_DEBUG_NAV_SCORING     0   // Display navigation scoring preview when hovering items. Display last moving direction matches when holding CTRL
#define IMGUI_DEBUG_NAV_RECTS       0   // Display the reference navigation rectangle for each window
#define IMGUI_DEBUG_INI_SETTINGS    0   // Save additional comments in .ini file (particularly helps for Docking, but makes saving slower)

// When using CTRL+TAB (or Gamepad Square+L/R) we delay the visual a little in order to reduce visual noise doing a fast switch.
static const float NAV_WINDOWING_HIGHLIGHT_DELAY = 0.20f;    // Time before the highlight and screen dimming starts fading in
static const float NAV_WINDOWING_LIST_APPEAR_DELAY = 0.15f;    // Time before the window list starts to appear

// Window resizing from edges (when io.ConfigWindowsResizeFromEdges = true and ImGuiBackendFlags_HasMouseCursors is set in io.BackendFlags by back-end)
static const float WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS = 4.0f;     // Extend outside and inside windows. Affect FindHoveredWindow().
static const float WINDOWS_RESIZE_FROM_EDGES_FEEDBACK_TIMER = 0.04f;    // Reduce visual noise by only highlighting the border after a certain time.
static const float WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER = 2.00f;    // Lock scrolled window (so it doesn't pick child windows that are scrolling through) for a certain time, unless mouse moved.

//-------------------------------------------------------------------------
// [SECTION] FORWARD DECLARATIONS
//-------------------------------------------------------------------------

static void             SetCurrentWindow(ImGuiWindow* window);
static void             FindHoveredWindow();
static ImGuiWindow* CreateNewWindow(const char* name, ImGuiWindowFlags flags);
static ImVec2           CalcNextScrollFromScrollTargetAndClamp(ImGuiWindow* window, bool snap_on_edges);

static void             AddDrawListToDrawData(ImVector<ImDrawList*>* out_list, ImDrawList* draw_list);
static void             AddWindowToSortBuffer(ImVector<ImGuiWindow*>* out_sorted_windows, ImGuiWindow* window);

static ImRect           GetViewportRect();

// Settings
static void             WindowSettingsHandler_ClearAll(ImGuiContext*, ImGuiSettingsHandler*);
static void* WindowSettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);
static void             WindowSettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line);
static void             WindowSettingsHandler_ApplyAll(ImGuiContext*, ImGuiSettingsHandler*);
static void             WindowSettingsHandler_WriteAll(ImGuiContext*, ImGuiSettingsHandler*, ImGuiTextBuffer* buf);

// Platform Dependents default implementation for IO functions
static const char* GetClipboardTextFn_DefaultImpl(void* user_data);
static void             SetClipboardTextFn_DefaultImpl(void* user_data, const char* text);
static void             ImeSetInputScreenPosFn_DefaultImpl(int x, int y);

namespace ImGui
{
    // Navigation
    static void             NavUpdate();
    static void             NavUpdateWindowing();
    static void             NavUpdateWindowingOverlay();
    static void             NavUpdateMoveResult();
    static float            NavUpdatePageUpPageDown();
    static inline void      NavUpdateAnyRequestFlag();
    static void             NavEndFrame();
    static bool             NavScoreItem(ImGuiNavMoveResult* result, ImRect cand);
    static void             NavProcessItem(ImGuiWindow* window, const ImRect& nav_bb, ImGuiID id);
    static ImVec2           NavCalcPreferredRefPos();
    static void             NavSaveLastChildNavWindowIntoParent(ImGuiWindow* nav_window);
    static ImGuiWindow* NavRestoreLastChildNavWindow(ImGuiWindow* window);
    static int              FindWindowFocusIndex(ImGuiWindow* window);

    // Error Checking
    static void             ErrorCheckNewFrameSanityChecks();
    static void             ErrorCheckEndFrameSanityChecks();
    static void             ErrorCheckBeginEndCompareStacksSize(ImGuiWindow* window, bool write);

    // Misc
    static void             UpdateSettings();
    static void             UpdateMouseInputs();
    static void             UpdateMouseWheel();
    static void             UpdateTabFocus();
    static void             UpdateDebugToolItemPicker();
    static bool             UpdateWindowManualResize(ImGuiWindow* window, const ImVec2& size_auto_fit, int* border_held, int resize_grip_count, ImU32 resize_grip_col[4]);
    static void             RenderWindowOuterBorders(ImGuiWindow* window);
    static void             RenderWindowDecorations(ImGuiWindow* window, const ImRect& title_bar_rect, bool title_bar_is_highlight, int resize_grip_count, const ImU32 resize_grip_col[4], float resize_grip_draw_size);
    static void             RenderWindowTitleBarContents(ImGuiWindow* window, const ImRect& title_bar_rect, const char* name, bool* p_open);

}

//-----------------------------------------------------------------------------
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
//-----------------------------------------------------------------------------

// Current context pointer. Implicitly used by all Dear ImGui functions. Always assumed to be != NULL.
// ImGui::CreateContext() will automatically set this pointer if it is NULL. Change to a different context by calling ImGui::SetCurrentContext().
// 1) Important: globals are not shared across DLL boundaries! If you use DLLs or any form of hot-reloading: you will need to call
//    SetCurrentContext() (with the pointer you got from CreateContext) from each unique static/DLL boundary, and after each hot-reloading.
//    In your debugger, add GImGui to your watch window and notice how its value changes depending on which location you are currently stepping into.
// 2) Important: Dear ImGui functions are not thread-safe because of this pointer.
//    If you want thread-safety to allow N threads to access N different contexts, you can:
//    - Change this variable to use thread local storage so each thread can refer to a different context, in imconfig.h:
//          struct ImGuiContext;
//          extern thread_local ImGuiContext* MyImGuiTLS;
//          #define GImGui MyImGuiTLS
//      And then define MyImGuiTLS in one of your cpp file. Note that thread_local is a C++11 keyword, earlier C++ uses compiler-specific keyword.
//    - Future development aim to make this context pointer explicit to all calls. Also read https://github.com/ocornut/imgui/issues/586
//    - If you need a finite number of contexts, you may compile and use multiple instances of the ImGui code from different namespace.
#ifndef GImGui
ImGuiContext* GImGui = NULL;
#endif

// Memory Allocator functions. Use SetAllocatorFunctions() to change them.
// If you use DLL hotreloading you might need to call SetAllocatorFunctions() after reloading code from this file.
// Otherwise, you probably don't want to modify them mid-program, and if you use global/static e.g. ImVector<> instances you may need to keep them accessible during program destruction.
#ifndef IMGUI_DISABLE_DEFAULT_ALLOCATORS
static void* MallocWrapper(size_t size, void* user_data) { IM_UNUSED(user_data); return malloc(size); }
static void    FreeWrapper(void* ptr, void* user_data) { IM_UNUSED(user_data); free(ptr); }
#else
static void* MallocWrapper(size_t size, void* user_data) { IM_UNUSED(user_data); IM_UNUSED(size); IM_ASSERT(0); return NULL; }
static void    FreeWrapper(void* ptr, void* user_data) { IM_UNUSED(user_data); IM_UNUSED(ptr); IM_ASSERT(0); }
#endif

static void* (*GImAllocatorAllocFunc)(size_t size, void* user_data) = MallocWrapper;
static void   (*GImAllocatorFreeFunc)(void* ptr, void* user_data) = FreeWrapper;
static void* GImAllocatorUserData = NULL;

//-----------------------------------------------------------------------------
// [SECTION] USER FACING STRUCTURES (ImGuiStyle, ImGuiIO)
//-----------------------------------------------------------------------------

ImGuiStyle::ImGuiStyle()
{
    Alpha = 1.0f;             // Global alpha applies to everything in ImGui
    WindowPadding = ImVec2(8, 8);      // Padding within a window
    WindowRounding = 7.0f;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
    WindowBorderSize = 1.0f;             // Thickness of border around windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    WindowMinSize = ImVec2(32, 32);    // Minimum window size
    WindowTitleAlign = ImVec2(0.0f, 0.5f);// Alignment for title bar text
    WindowMenuButtonPosition = ImGuiDir_Left;    // Position of the collapsing/docking button in the title bar (left/right). Defaults to ImGuiDir_Left.
    ChildRounding = 0.0f;             // Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
    ChildBorderSize = 1.0f;             // Thickness of border around child windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    PopupRounding = 0.0f;             // Radius of popup window corners rounding. Set to 0.0f to have rectangular child windows
    PopupBorderSize = 1.0f;             // Thickness of border around popup or tooltip windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    FramePadding = ImVec2(4, 3);      // Padding within a framed rectangle (used by most widgets)
    FrameRounding = 0.0f;             // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
    FrameBorderSize = 0.0f;             // Thickness of border around frames. Generally set to 0.0f or 1.0f. Other values not well tested.
    ItemSpacing = ImVec2(8, 4);      // Horizontal and vertical spacing between widgets/lines
    ItemInnerSpacing = ImVec2(4, 4);      // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
    TouchExtraPadding = ImVec2(0, 0);      // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    IndentSpacing = 21.0f;            // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    ColumnsMinSpacing = 6.0f;             // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
    ScrollbarSize = 14.0f;            // Width of the vertical scrollbar, Height of the horizontal scrollbar
    ScrollbarRounding = 9.0f;             // Radius of grab corners rounding for scrollbar
    GrabMinSize = 10.0f;            // Minimum width/height of a grab box for slider/scrollbar
    GrabRounding = 0.0f;             // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    TabRounding = 4.0f;             // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    TabBorderSize = 0.0f;             // Thickness of border around tabs.
    TabMinWidthForUnselectedCloseButton = 0.0f; // Minimum width for close button to appears on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.
    ColorButtonPosition = ImGuiDir_Right;   // Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
    ButtonTextAlign = ImVec2(0.5f, 0.5f);// Alignment of button text when button is larger than text.
    SelectableTextAlign = ImVec2(0.0f, 0.0f);// Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
    DisplayWindowPadding = ImVec2(19, 19);    // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
    DisplaySafeAreaPadding = ImVec2(3, 3);      // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
    MouseCursorScale = 1.0f;             // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    AntiAliasedLines = true;             // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
    AntiAliasedFill = true;             // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
    CurveTessellationTol = 1.25f;            // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    CircleSegmentMaxError = 1.60f;            // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.

    // Default theme
    ImGui::StyleColorsDark(this);
}

// To scale your entire UI (e.g. if you want your app to use High DPI or generally be DPI aware) you may use this helper function. Scaling the fonts is done separately and is up to you.
// Important: This operation is lossy because we round all sizes to integer. If you need to change your scale multiples, call this over a freshly initialized ImGuiStyle structure rather than scaling multiple times.
void ImGuiStyle::ScaleAllSizes(float scale_factor)
{
    WindowPadding = ImFloor(WindowPadding * scale_factor);
    WindowRounding = ImFloor(WindowRounding * scale_factor);
    WindowMinSize = ImFloor(WindowMinSize * scale_factor);
    ChildRounding = ImFloor(ChildRounding * scale_factor);
    PopupRounding = ImFloor(PopupRounding * scale_factor);
    FramePadding = ImFloor(FramePadding * scale_factor);
    FrameRounding = ImFloor(FrameRounding * scale_factor);
    ItemSpacing = ImFloor(ItemSpacing * scale_factor);
    ItemInnerSpacing = ImFloor(ItemInnerSpacing * scale_factor);
    TouchExtraPadding = ImFloor(TouchExtraPadding * scale_factor);
    IndentSpacing = ImFloor(IndentSpacing * scale_factor);
    ColumnsMinSpacing = ImFloor(ColumnsMinSpacing * scale_factor);
    ScrollbarSize = ImFloor(ScrollbarSize * scale_factor);
    ScrollbarRounding = ImFloor(ScrollbarRounding * scale_factor);
    GrabMinSize = ImFloor(GrabMinSize * scale_factor);
    GrabRounding = ImFloor(GrabRounding * scale_factor);
    TabRounding = ImFloor(TabRounding * scale_factor);
    if (TabMinWidthForUnselectedCloseButton != FLT_MAX)
        TabMinWidthForUnselectedCloseButton = ImFloor(TabMinWidthForUnselectedCloseButton * scale_factor);
    DisplayWindowPadding = ImFloor(DisplayWindowPadding * scale_factor);
    DisplaySafeAreaPadding = ImFloor(DisplaySafeAreaPadding * scale_factor);
    MouseCursorScale = ImFloor(MouseCursorScale * scale_factor);
}

ImGuiIO::ImGuiIO()
{
    // Most fields are initialized with zero
    memset(this, 0, sizeof(*this));
    IM_ASSERT(IM_ARRAYSIZE(ImGuiIO::MouseDown) == ImGuiMouseButton_COUNT && IM_ARRAYSIZE(ImGuiIO::MouseClicked) == ImGuiMouseButton_COUNT); // Our pre-C++11 IM_STATIC_ASSERT() macros triggers warning on modern compilers so we don't use it here.

    // Settings
    ConfigFlags = ImGuiConfigFlags_None;
    BackendFlags = ImGuiBackendFlags_None;
    DisplaySize = ImVec2(-1.0f, -1.0f);
    DeltaTime = 1.0f / 60.0f;
    IniSavingRate = 5.0f;
    IniFilename = "imgui.ini";
    LogFilename = "imgui_log.txt";
    MouseDoubleClickTime = 0.30f;
    MouseDoubleClickMaxDist = 6.0f;
    for (int i = 0; i < ImGuiKey_COUNT; i++)
        KeyMap[i] = -1;
    KeyRepeatDelay = 0.275f;
    KeyRepeatRate = 0.050f;
    UserData = NULL;

    Fonts = NULL;
    FontGlobalScale = 1.0f;
    FontDefault = NULL;
    FontAllowUserScaling = false;
    DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    // Miscellaneous options
    MouseDrawCursor = false;
#ifdef __APPLE__
    ConfigMacOSXBehaviors = true;  // Set Mac OS X style defaults based on __APPLE__ compile time flag
#else
    ConfigMacOSXBehaviors = false;
#endif
    ConfigInputTextCursorBlink = true;
    ConfigWindowsResizeFromEdges = true;
    ConfigWindowsMoveFromTitleBarOnly = false;
    ConfigWindowsMemoryCompactTimer = 60.0f;

    // Platform Functions
    BackendPlatformName = BackendRendererName = NULL;
    BackendPlatformUserData = BackendRendererUserData = BackendLanguageUserData = NULL;
    GetClipboardTextFn = GetClipboardTextFn_DefaultImpl;   // Platform dependent default implementations
    SetClipboardTextFn = SetClipboardTextFn_DefaultImpl;
    ClipboardUserData = NULL;
    ImeSetInputScreenPosFn = ImeSetInputScreenPosFn_DefaultImpl;
    ImeWindowHandle = NULL;

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    RenderDrawListsFn = NULL;
#endif

    // Input (NB: we already have memset zero the entire structure!)
    MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    MousePosPrev = ImVec2(-FLT_MAX, -FLT_MAX);
    MouseDragThreshold = 6.0f;
    for (int i = 0; i < IM_ARRAYSIZE(MouseDownDuration); i++) MouseDownDuration[i] = MouseDownDurationPrev[i] = -1.0f;
    for (int i = 0; i < IM_ARRAYSIZE(KeysDownDuration); i++) KeysDownDuration[i] = KeysDownDurationPrev[i] = -1.0f;
    for (int i = 0; i < IM_ARRAYSIZE(NavInputsDownDuration); i++) NavInputsDownDuration[i] = -1.0f;
}

// Pass in translated ASCII characters for text input.
// - with glfw you can get those from the callback set in glfwSetCharCallback()
// - on Windows you can get those using ToAscii+keyboard state, or via the WM_CHAR message
void ImGuiIO::AddInputCharacter(unsigned int c)
{
    if (c != 0)
        InputQueueCharacters.push_back(c <= IM_UNICODE_CODEPOINT_MAX ? (ImWchar)c : IM_UNICODE_CODEPOINT_INVALID);
}

// UTF16 strings use surrogate pairs to encode codepoints >= 0x10000, so
// we should save the high surrogate.
void ImGuiIO::AddInputCharacterUTF16(ImWchar16 c)
{
    if (c == 0 && InputQueueSurrogate == 0)
        return;

    if ((c & 0xFC00) == 0xD800) // High surrogate, must save
    {
        if (InputQueueSurrogate != 0)
            InputQueueCharacters.push_back(IM_UNICODE_CODEPOINT_INVALID);
        InputQueueSurrogate = c;
        return;
    }

    ImWchar cp = c;
    if (InputQueueSurrogate != 0)
    {
        if ((c & 0xFC00) != 0xDC00) // Invalid low surrogate
            InputQueueCharacters.push_back(IM_UNICODE_CODEPOINT_INVALID);
        else if (IM_UNICODE_CODEPOINT_MAX == (0xFFFF)) // Codepoint will not fit in ImWchar (extra parenthesis around 0xFFFF somehow fixes -Wunreachable-code with Clang)
            cp = IM_UNICODE_CODEPOINT_INVALID;
        else
            cp = (ImWchar)(((InputQueueSurrogate - 0xD800) << 10) + (c - 0xDC00) + 0x10000);
        InputQueueSurrogate = 0;
    }
    InputQueueCharacters.push_back(cp);
}

void ImGuiIO::AddInputCharactersUTF8(const char* utf8_chars)
{
    while (*utf8_chars != 0)
    {
        unsigned int c = 0;
        utf8_chars += ImTextCharFromUtf8(&c, utf8_chars, NULL);
        if (c != 0)
            InputQueueCharacters.push_back((ImWchar)c);
    }
}

void ImGuiIO::ClearInputCharacters()
{
    InputQueueCharacters.resize(0);
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (Geometry functions)
//-----------------------------------------------------------------------------

ImVec2 ImBezierClosestPoint(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p, int num_segments)
{
    IM_ASSERT(num_segments > 0); // Use ImBezierClosestPointCasteljau()
    ImVec2 p_last = p1;
    ImVec2 p_closest;
    float p_closest_dist2 = FLT_MAX;
    float t_step = 1.0f / (float)num_segments;
    for (int i_step = 1; i_step <= num_segments; i_step++)
    {
        ImVec2 p_current = ImBezierCalc(p1, p2, p3, p4, t_step * i_step);
        ImVec2 p_line = ImLineClosestPoint(p_last, p_current, p);
        float dist2 = ImLengthSqr(p - p_line);
        if (dist2 < p_closest_dist2)
        {
            p_closest = p_line;
            p_closest_dist2 = dist2;
        }
        p_last = p_current;
    }
    return p_closest;
}

// Closely mimics PathBezierToCasteljau() in imgui_draw.cpp
static void BezierClosestPointCasteljauStep(const ImVec2& p, ImVec2& p_closest, ImVec2& p_last, float& p_closest_dist2, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
    float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy))
    {
        ImVec2 p_current(x4, y4);
        ImVec2 p_line = ImLineClosestPoint(p_last, p_current, p);
        float dist2 = ImLengthSqr(p - p_line);
        if (dist2 < p_closest_dist2)
        {
            p_closest = p_line;
            p_closest_dist2 = dist2;
        }
        p_last = p_current;
    } else if (level < 10)
    {
        float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
        float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
        float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;
        BezierClosestPointCasteljauStep(p, p_closest, p_last, p_closest_dist2, x1, y1, x12, y12, x123, y123, x1234, y1234, tess_tol, level + 1);
        BezierClosestPointCasteljauStep(p, p_closest, p_last, p_closest_dist2, x1234, y1234, x234, y234, x34, y34, x4, y4, tess_tol, level + 1);
    }
}

// tess_tol is generally the same value you would find in ImGui::GetStyle().CurveTessellationTol
// Because those ImXXX functions are lower-level than ImGui:: we cannot access this value automatically.
ImVec2 ImBezierClosestPointCasteljau(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p, float tess_tol)
{
    IM_ASSERT(tess_tol > 0.0f);
    ImVec2 p_last = p1;
    ImVec2 p_closest;
    float p_closest_dist2 = FLT_MAX;
    BezierClosestPointCasteljauStep(p, p_closest, p_last, p_closest_dist2, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, tess_tol, 0);
    return p_closest;
}

ImVec2 ImLineClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& p)
{
    ImVec2 ap = p - a;
    ImVec2 ab_dir = b - a;
    float dot = ap.x * ab_dir.x + ap.y * ab_dir.y;
    if (dot < 0.0f)
        return a;
    float ab_len_sqr = ab_dir.x * ab_dir.x + ab_dir.y * ab_dir.y;
    if (dot > ab_len_sqr)
        return b;
    return a + ab_dir * dot / ab_len_sqr;
}

bool ImTriangleContainsPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p)
{
    bool b1 = ((p.x - b.x) * (a.y - b.y) - (p.y - b.y) * (a.x - b.x)) < 0.0f;
    bool b2 = ((p.x - c.x) * (b.y - c.y) - (p.y - c.y) * (b.x - c.x)) < 0.0f;
    bool b3 = ((p.x - a.x) * (c.y - a.y) - (p.y - a.y) * (c.x - a.x)) < 0.0f;
    return ((b1 == b2) && (b2 == b3));
}

void ImTriangleBarycentricCoords(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p, float& out_u, float& out_v, float& out_w)
{
    ImVec2 v0 = b - a;
    ImVec2 v1 = c - a;
    ImVec2 v2 = p - a;
    const float denom = v0.x * v1.y - v1.x * v0.y;
    out_v = (v2.x * v1.y - v1.x * v2.y) / denom;
    out_w = (v0.x * v2.y - v2.x * v0.y) / denom;
    out_u = 1.0f - out_v - out_w;
}

ImVec2 ImTriangleClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p)
{
    ImVec2 proj_ab = ImLineClosestPoint(a, b, p);
    ImVec2 proj_bc = ImLineClosestPoint(b, c, p);
    ImVec2 proj_ca = ImLineClosestPoint(c, a, p);
    float dist2_ab = ImLengthSqr(p - proj_ab);
    float dist2_bc = ImLengthSqr(p - proj_bc);
    float dist2_ca = ImLengthSqr(p - proj_ca);
    float m = ImMin(dist2_ab, ImMin(dist2_bc, dist2_ca));
    if (m == dist2_ab)
        return proj_ab;
    if (m == dist2_bc)
        return proj_bc;
    return proj_ca;
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (String, Format, Hash functions)
//-----------------------------------------------------------------------------

// Consider using _stricmp/_strnicmp under Windows or strcasecmp/strncasecmp. We don't actually use either ImStricmp/ImStrnicmp in the codebase any more.
int ImStricmp(const char* str1, const char* str2)
{
    int d;
    while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; }
    return d;
}

int ImStrnicmp(const char* str1, const char* str2, size_t count)
{
    int d = 0;
    while (count > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; count--; }
    return d;
}

void ImStrncpy(char* dst, const char* src, size_t count)
{
    if (count < 1)
        return;
    if (count > 1)
        strncpy(dst, src, count - 1);
    dst[count - 1] = 0;
}

char* ImStrdup(const char* str)
{
    size_t len = strlen(str);
    void* buf = IM_ALLOC(len + 1);
    return (char*)memcpy(buf, (const void*)str, len + 1);
}

char* ImStrdupcpy(char* dst, size_t* p_dst_size, const char* src)
{
    size_t dst_buf_size = p_dst_size ? *p_dst_size : strlen(dst) + 1;
    size_t src_size = strlen(src) + 1;
    if (dst_buf_size < src_size)
    {
        IM_FREE(dst);
        dst = (char*)IM_ALLOC(src_size);
        if (p_dst_size)
            *p_dst_size = src_size;
    }
    return (char*)memcpy(dst, (const void*)src, src_size);
}

const char* ImStrchrRange(const char* str, const char* str_end, char c)
{
    const char* p = (const char*)memchr(str, (int)c, str_end - str);
    return p;
}

int ImStrlenW(const ImWchar* str)
{
    //return (int)wcslen((const wchar_t*)str);  // FIXME-OPT: Could use this when wchar_t are 16-bit
    int n = 0;
    while (*str++) n++;
    return n;
}

// Find end-of-line. Return pointer will point to either first \n, either str_end.
const char* ImStreolRange(const char* str, const char* str_end)
{
    const char* p = (const char*)memchr(str, '\n', str_end - str);
    return p ? p : str_end;
}

const ImWchar* ImStrbolW(const ImWchar* buf_mid_line, const ImWchar* buf_begin) // find beginning-of-line
{
    while (buf_mid_line > buf_begin && buf_mid_line[-1] != '\n')
        buf_mid_line--;
    return buf_mid_line;
}

const char* ImStristr(const char* haystack, const char* haystack_end, const char* needle, const char* needle_end)
{
    if (!needle_end)
        needle_end = needle + strlen(needle);

    const char un0 = (char)toupper(*needle);
    while ((!haystack_end && *haystack) || (haystack_end && haystack < haystack_end))
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

// Trim str by offsetting contents when there's leading data + writing a \0 at the trailing position. We use this in situation where the cost is negligible.
void ImStrTrimBlanks(char* buf)
{
    char* p = buf;
    while (p[0] == ' ' || p[0] == '\t')     // Leading blanks
        p++;
    char* p_start = p;
    while (*p != 0)                         // Find end of string
        p++;
    while (p > p_start && (p[-1] == ' ' || p[-1] == '\t'))  // Trailing blanks
        p--;
    if (p_start != buf)                     // Copy memory if we had leading blanks
        memmove(buf, p_start, p - p_start);
    buf[p - p_start] = 0;                   // Zero terminate
}

const char* ImStrSkipBlank(const char* str)
{
    while (str[0] == ' ' || str[0] == '\t')
        str++;
    return str;
}

// A) MSVC version appears to return -1 on overflow, whereas glibc appears to return total count (which may be >= buf_size).
// Ideally we would test for only one of those limits at runtime depending on the behavior the vsnprintf(), but trying to deduct it at compile time sounds like a pandora can of worm.
// B) When buf==NULL vsnprintf() will return the output size.
#ifndef IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS

// We support stb_sprintf which is much faster (see: https://github.com/nothings/stb/blob/master/stb_sprintf.h)
// You may set IMGUI_USE_STB_SPRINTF to use our default wrapper, or set IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
// and setup the wrapper yourself. (FIXME-OPT: Some of our high-level operations such as ImGuiTextBuffer::appendfv() are
// designed using two-passes worst case, which probably could be improved using the stbsp_vsprintfcb() function.)
#ifdef IMGUI_USE_STB_SPRINTF
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#endif

#if defined(_MSC_VER) && !defined(vsnprintf)
#define vsnprintf _vsnprintf
#endif

int ImFormatString(char* buf, size_t buf_size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
#ifdef IMGUI_USE_STB_SPRINTF
    int w = stbsp_vsnprintf(buf, (int)buf_size, fmt, args);
#else
    int w = vsnprintf(buf, buf_size, fmt, args);
#endif
    va_end(args);
    if (buf == NULL)
        return w;
    if (w == -1 || w >= (int)buf_size)
        w = (int)buf_size - 1;
    buf[w] = 0;
    return w;
}

int ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args)
{
#ifdef IMGUI_USE_STB_SPRINTF
    int w = stbsp_vsnprintf(buf, (int)buf_size, fmt, args);
#else
    int w = vsnprintf(buf, buf_size, fmt, args);
#endif
    if (buf == NULL)
        return w;
    if (w == -1 || w >= (int)buf_size)
        w = (int)buf_size - 1;
    buf[w] = 0;
    return w;
}
#endif // #ifdef IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS

// CRC32 needs a 1KB lookup table (not cache friendly)
// Although the code to generate the table is simple and shorter than the table itself, using a const table allows us to easily:
// - avoid an unnecessary branch/memory tap, - keep the ImHashXXX functions usable by static constructors, - make it thread-safe.
static const ImU32 GCrc32LookupTable[256] =
{
    0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
    0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
    0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
    0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
    0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
    0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
    0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
    0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
    0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
    0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
    0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
    0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
    0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
    0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
    0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
    0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D,
};

// Known size hash
// It is ok to call ImHashData on a string with known length but the ### operator won't be supported.
// FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do proper measurements.
ImU32 ImHashData(const void* data_p, size_t data_size, ImU32 seed)
{
    ImU32 crc = ~seed;
    const unsigned char* data = (const unsigned char*)data_p;
    const ImU32* crc32_lut = GCrc32LookupTable;
    while (data_size-- != 0)
        crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ *data++];
    return ~crc;
}

// Zero-terminated string hash, with support for ### to reset back to seed value
// We support a syntax of "label###id" where only "###id" is included in the hash, and only "label" gets displayed.
// Because this syntax is rarely used we are optimizing for the common case.
// - If we reach ### in the string we discard the hash so far and reset to the seed.
// - We don't do 'current += 2; continue;' after handling ### to keep the code smaller/faster (measured ~10% diff in Debug build)
// FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do proper measurements.
ImU32 ImHashStr(const char* data_p, size_t data_size, ImU32 seed)
{
    seed = ~seed;
    ImU32 crc = seed;
    const unsigned char* data = (const unsigned char*)data_p;
    const ImU32* crc32_lut = GCrc32LookupTable;
    if (data_size != 0)
    {
        while (data_size-- != 0)
        {
            unsigned char c = *data++;
            if (c == '#' && data_size >= 2 && data[0] == '#' && data[1] == '#')
                crc = seed;
            crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
        }
    } else
    {
        while (unsigned char c = *data++)
        {
            if (c == '#' && data[0] == '#' && data[1] == '#')
                crc = seed;
            crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
        }
    }
    return ~crc;
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (File functions)
//-----------------------------------------------------------------------------

// Default file functions
#ifndef IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS

ImFileHandle ImFileOpen(const char* filename, const char* mode)
{
#if defined(_WIN32) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS) && !defined(__CYGWIN__) && !defined(__GNUC__)
    // We need a fopen() wrapper because MSVC/Windows fopen doesn't handle UTF-8 filenames.
    // Previously we used ImTextCountCharsFromUtf8/ImTextStrFromUtf8 here but we now need to support ImWchar16 and ImWchar32!
    const int filename_wsize = ::MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
    const int mode_wsize = ::MultiByteToWideChar(CP_UTF8, 0, mode, -1, NULL, 0);
    ImVector<ImWchar> buf;
    buf.resize(filename_wsize + mode_wsize);
    ::MultiByteToWideChar(CP_UTF8, 0, filename, -1, (wchar_t*)&buf[0], filename_wsize);
    ::MultiByteToWideChar(CP_UTF8, 0, mode, -1, (wchar_t*)&buf[filename_wsize], mode_wsize);
    return ::_wfopen((const wchar_t*)&buf[0], (const wchar_t*)&buf[filename_wsize]);
#else
    return fopen(filename, mode);
#endif
}

// We should in theory be using fseeko()/ftello() with off_t and _fseeki64()/_ftelli64() with __int64, waiting for the PR that does that in a very portable pre-C++11 zero-warnings way.
bool    ImFileClose(ImFileHandle f) { return fclose(f) == 0; }
ImU64   ImFileGetSize(ImFileHandle f) { long off = 0, sz = 0; return ((off = ftell(f)) != -1 && !fseek(f, 0, SEEK_END) && (sz = ftell(f)) != -1 && !fseek(f, off, SEEK_SET)) ? (ImU64)sz : (ImU64)-1; }
ImU64   ImFileRead(void* data, ImU64 sz, ImU64 count, ImFileHandle f) { return fread(data, (size_t)sz, (size_t)count, f); }
ImU64   ImFileWrite(const void* data, ImU64 sz, ImU64 count, ImFileHandle f) { return fwrite(data, (size_t)sz, (size_t)count, f); }
#endif // #ifndef IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS

// Helper: Load file content into memory
// Memory allocated with IM_ALLOC(), must be freed by user using IM_FREE() == ImGui::MemFree()
// This can't really be used with "rt" because fseek size won't match read size.
void* ImFileLoadToMemory(const char* filename, const char* mode, size_t* out_file_size, int padding_bytes)
{
    IM_ASSERT(filename && mode);
    if (out_file_size)
        *out_file_size = 0;

    ImFileHandle f;
    if ((f = ImFileOpen(filename, mode)) == NULL)
        return NULL;

    size_t file_size = (size_t)ImFileGetSize(f);
    if (file_size == (size_t)-1)
    {
        ImFileClose(f);
        return NULL;
    }

    void* file_data = IM_ALLOC(file_size + padding_bytes);
    if (file_data == NULL)
    {
        ImFileClose(f);
        return NULL;
    }
    if (ImFileRead(file_data, 1, file_size, f) != file_size)
    {
        ImFileClose(f);
        IM_FREE(file_data);
        return NULL;
    }
    if (padding_bytes > 0)
        memset((void*)(((char*)file_data) + file_size), 0, (size_t)padding_bytes);

    ImFileClose(f);
    if (out_file_size)
        *out_file_size = file_size;

    return file_data;
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
//-----------------------------------------------------------------------------

// Convert UTF-8 to 32-bit character, process single character input.
// Based on stb_from_utf8() from github.com/nothings/stb/
// We handle UTF-8 decoding error by skipping forward.
int ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end)
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
        *out_char = IM_UNICODE_CODEPOINT_INVALID; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char*)str < 2) return 1;
        if (*str < 0xc2) return 2;
        c = (unsigned int)((*str++ & 0x1f) << 6);
        if ((*str & 0xc0) != 0x80) return 2;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 2;
    }
    if ((*str & 0xf0) == 0xe0)
    {
        *out_char = IM_UNICODE_CODEPOINT_INVALID; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char*)str < 3) return 1;
        if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return 3;
        if (*str == 0xed && str[1] > 0x9f) return 3; // str[1] < 0x80 is checked below
        c = (unsigned int)((*str++ & 0x0f) << 12);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 3;
    }
    if ((*str & 0xf8) == 0xf0)
    {
        *out_char = IM_UNICODE_CODEPOINT_INVALID; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char*)str < 4) return 1;
        if (*str > 0xf4) return 4;
        if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return 4;
        if (*str == 0xf4 && str[1] > 0x8f) return 4; // str[1] < 0x80 is checked below
        c = (unsigned int)((*str++ & 0x07) << 18);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int)((*str++ & 0x3f) << 12);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (*str++ & 0x3f);
        // utf-8 encodings of values used in surrogate pairs are invalid
        if ((c & 0xFFFFF800) == 0xD800) return 4;
        // If codepoint does not fit in ImWchar, use replacement character U+FFFD instead
        if (c > IM_UNICODE_CODEPOINT_MAX) c = IM_UNICODE_CODEPOINT_INVALID;
        *out_char = c;
        return 4;
    }
    *out_char = 0;
    return 0;
}

int ImTextStrFromUtf8(ImWchar* buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_text_remaining)
{
    ImWchar* buf_out = buf;
    ImWchar* buf_end = buf + buf_size;
    while (buf_out < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c;
        in_text += ImTextCharFromUtf8(&c, in_text, in_text_end);
        if (c == 0)
            break;
        *buf_out++ = (ImWchar)c;
    }
    *buf_out = 0;
    if (in_text_remaining)
        *in_text_remaining = in_text;
    return (int)(buf_out - buf);
}

int ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end)
{
    int char_count = 0;
    while ((!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c;
        in_text += ImTextCharFromUtf8(&c, in_text, in_text_end);
        if (c == 0)
            break;
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
    if (c < 0x10000)
    {
        if (buf_size < 3) return 0;
        buf[0] = (char)(0xe0 + (c >> 12));
        buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
        buf[2] = (char)(0x80 + ((c) & 0x3f));
        return 3;
    }
    if (c <= 0x10FFFF)
    {
        if (buf_size < 4) return 0;
        buf[0] = (char)(0xf0 + (c >> 18));
        buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
        buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
        buf[3] = (char)(0x80 + ((c) & 0x3f));
        return 4;
    }
    // Invalid code point, the max unicode is 0x10FFFF
    return 0;
}

// Not optimal but we very rarely use this function.
int ImTextCountUtf8BytesFromChar(const char* in_text, const char* in_text_end)
{
    unsigned int dummy = 0;
    return ImTextCharFromUtf8(&dummy, in_text, in_text_end);
}

static inline int ImTextCountUtf8BytesFromChar(unsigned int c)
{
    if (c < 0x80) return 1;
    if (c < 0x800) return 2;
    if (c < 0x10000) return 3;
    if (c <= 0x10FFFF) return 4;
    return 3;
}

int ImTextStrToUtf8(char* buf, int buf_size, const ImWchar* in_text, const ImWchar* in_text_end)
{
    char* buf_out = buf;
    const char* buf_end = buf + buf_size;
    while (buf_out < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c = (unsigned int)(*in_text++);
        if (c < 0x80)
            *buf_out++ = (char)c;
        else
            buf_out += ImTextCharToUtf8(buf_out, (int)(buf_end - buf_out - 1), c);
    }
    *buf_out = 0;
    return (int)(buf_out - buf);
}

int ImTextCountUtf8BytesFromStr(const ImWchar* in_text, const ImWchar* in_text_end)
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

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (Color functions)
// Note: The Convert functions are early design which are not consistent with other API.
//-----------------------------------------------------------------------------

IMGUI_API ImU32 ImAlphaBlendColors(ImU32 col_a, ImU32 col_b)
{
    float t = ((col_b >> IM_COL32_A_SHIFT) & 0xFF) / 255.f;
    int r = ImLerp((int)(col_a >> IM_COL32_R_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_R_SHIFT) & 0xFF, t);
    int g = ImLerp((int)(col_a >> IM_COL32_G_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_G_SHIFT) & 0xFF, t);
    int b = ImLerp((int)(col_a >> IM_COL32_B_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_B_SHIFT) & 0xFF, t);
    return IM_COL32(r, g, b, 0xFF);
}

ImVec4 ImGui::ColorConvertU32ToFloat4(ImU32 in)
{
    float s = 1.0f / 255.0f;
    return ImVec4(
        ((in >> IM_COL32_R_SHIFT) & 0xFF) * s,
        ((in >> IM_COL32_G_SHIFT) & 0xFF) * s,
        ((in >> IM_COL32_B_SHIFT) & 0xFF) * s,
        ((in >> IM_COL32_A_SHIFT) & 0xFF) * s);
}

ImU32 ImGui::ColorConvertFloat4ToU32(const ImVec4& in)
{
    ImU32 out;
    out = ((ImU32)IM_F32_TO_INT8_SAT(in.x)) << IM_COL32_R_SHIFT;
    out |= ((ImU32)IM_F32_TO_INT8_SAT(in.y)) << IM_COL32_G_SHIFT;
    out |= ((ImU32)IM_F32_TO_INT8_SAT(in.z)) << IM_COL32_B_SHIFT;
    out |= ((ImU32)IM_F32_TO_INT8_SAT(in.w)) << IM_COL32_A_SHIFT;
    return out;
}

// Convert rgb floats ([0-1],[0-1],[0-1]) to hsv floats ([0-1],[0-1],[0-1]), from Foley & van Dam p592
// Optimized http://lolengine.net/blog/2013/01/13/fast-rgb-to-hsv
void ImGui::ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v)
{
    float K = 0.f;
    if (g < b)
    {
        ImSwap(g, b);
        K = -1.f;
    }
    if (r < g)
    {
        ImSwap(r, g);
        K = -2.f / 6.f - K;
    }

    const float chroma = r - (g < b ? g : b);
    out_h = ImFabs(K + (g - b) / (6.f * chroma + 1e-20f));
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

    h = ImFmod(h, 1.0f) / (60.0f / 360.0f);
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

//-----------------------------------------------------------------------------
// [SECTION] ImGuiStorage
// Helper: Key->value storage
//-----------------------------------------------------------------------------

// std::lower_bound but without the bullshit
static ImGuiStorage::ImGuiStoragePair* LowerBound(ImVector<ImGuiStorage::ImGuiStoragePair>& data, ImGuiID key)
{
    ImGuiStorage::ImGuiStoragePair* first = data.Data;
    ImGuiStorage::ImGuiStoragePair* last = data.Data + data.Size;
    size_t count = (size_t)(last - first);
    while (count > 0)
    {
        size_t count2 = count >> 1;
        ImGuiStorage::ImGuiStoragePair* mid = first + count2;
        if (mid->key < key)
        {
            first = ++mid;
            count -= count2 + 1;
        } else
        {
            count = count2;
        }
    }
    return first;
}

// For quicker full rebuild of a storage (instead of an incremental one), you may add all your contents and then sort once.
void ImGuiStorage::BuildSortByKey()
{
    struct StaticFunc
    {
        static int IMGUI_CDECL PairCompareByID(const void* lhs, const void* rhs)
        {
            // We can't just do a subtraction because qsort uses signed integers and subtracting our ID doesn't play well with that.
            if (((const ImGuiStoragePair*)lhs)->key > ((const ImGuiStoragePair*)rhs)->key) return +1;
            if (((const ImGuiStoragePair*)lhs)->key < ((const ImGuiStoragePair*)rhs)->key) return -1;
            return 0;
        }
    };
    if (Data.Size > 1)
        ImQsort(Data.Data, (size_t)Data.Size, sizeof(ImGuiStoragePair), StaticFunc::PairCompareByID);
}

int ImGuiStorage::GetInt(ImGuiID key, int default_val) const
{
    ImGuiStoragePair* it = LowerBound(const_cast<ImVector<ImGuiStoragePair>&>(Data), key);
    if (it == Data.end() || it->key != key)
        return default_val;
    return it->val_i;
}

bool ImGuiStorage::GetBool(ImGuiID key, bool default_val) const
{
    return GetInt(key, default_val ? 1 : 0) != 0;
}

float ImGuiStorage::GetFloat(ImGuiID key, float default_val) const
{
    ImGuiStoragePair* it = LowerBound(const_cast<ImVector<ImGuiStoragePair>&>(Data), key);
    if (it == Data.end() || it->key != key)
        return default_val;
    return it->val_f;
}

void* ImGuiStorage::GetVoidPtr(ImGuiID key) const
{
    ImGuiStoragePair* it = LowerBound(const_cast<ImVector<ImGuiStoragePair>&>(Data), key);
    if (it == Data.end() || it->key != key)
        return NULL;
    return it->val_p;
}

// References are only valid until a new value is added to the storage. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
int* ImGuiStorage::GetIntRef(ImGuiID key, int default_val)
{
    ImGuiStoragePair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
        it = Data.insert(it, ImGuiStoragePair(key, default_val));
    return &it->val_i;
}

bool* ImGuiStorage::GetBoolRef(ImGuiID key, bool default_val)
{
    return (bool*)GetIntRef(key, default_val ? 1 : 0);
}

float* ImGuiStorage::GetFloatRef(ImGuiID key, float default_val)
{
    ImGuiStoragePair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
        it = Data.insert(it, ImGuiStoragePair(key, default_val));
    return &it->val_f;
}

void** ImGuiStorage::GetVoidPtrRef(ImGuiID key, void* default_val)
{
    ImGuiStoragePair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
        it = Data.insert(it, ImGuiStoragePair(key, default_val));
    return &it->val_p;
}

// FIXME-OPT: Need a way to reuse the result of lower_bound when doing GetInt()/SetInt() - not too bad because it only happens on explicit interaction (maximum one a frame)
void ImGuiStorage::SetInt(ImGuiID key, int val)
{
    ImGuiStoragePair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
    {
        Data.insert(it, ImGuiStoragePair(key, val));
        return;
    }
    it->val_i = val;
}

void ImGuiStorage::SetBool(ImGuiID key, bool val)
{
    SetInt(key, val ? 1 : 0);
}

void ImGuiStorage::SetFloat(ImGuiID key, float val)
{
    ImGuiStoragePair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
    {
        Data.insert(it, ImGuiStoragePair(key, val));
        return;
    }
    it->val_f = val;
}

void ImGuiStorage::SetVoidPtr(ImGuiID key, void* val)
{
    ImGuiStoragePair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
    {
        Data.insert(it, ImGuiStoragePair(key, val));
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
// [SECTION] ImGuiTextFilter
//-----------------------------------------------------------------------------

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
ImGuiTextFilter::ImGuiTextFilter(const char* default_filter)
{
    if (default_filter)
    {
        ImStrncpy(InputBuf, default_filter, IM_ARRAYSIZE(InputBuf));
        Build();
    } else
    {
        InputBuf[0] = 0;
        CountGrep = 0;
    }
}

bool ImGuiTextFilter::Draw(const char* label, float width)
{
    if (width != 0.0f)
        ImGui::SetNextItemWidth(width);
    bool value_changed = ImGui::InputText(label, InputBuf, IM_ARRAYSIZE(InputBuf));
    if (value_changed)
        Build();
    return value_changed;
}

void ImGuiTextFilter::ImGuiTextRange::split(char separator, ImVector<ImGuiTextRange>* out) const
{
    out->resize(0);
    const char* wb = b;
    const char* we = wb;
    while (we < e)
    {
        if (*we == separator)
        {
            out->push_back(ImGuiTextRange(wb, we));
            wb = we + 1;
        }
        we++;
    }
    if (wb != we)
        out->push_back(ImGuiTextRange(wb, we));
}

void ImGuiTextFilter::Build()
{
    Filters.resize(0);
    ImGuiTextRange input_range(InputBuf, InputBuf + strlen(InputBuf));
    input_range.split(',', &Filters);

    CountGrep = 0;
    for (int i = 0; i != Filters.Size; i++)
    {
        ImGuiTextRange& f = Filters[i];
        while (f.b < f.e && ImCharIsBlankA(f.b[0]))
            f.b++;
        while (f.e > f.b && ImCharIsBlankA(f.e[-1]))
            f.e--;
        if (f.empty())
            continue;
        if (Filters[i].b[0] != '-')
            CountGrep += 1;
    }
}

bool ImGuiTextFilter::PassFilter(const char* text, const char* text_end) const
{
    if (Filters.empty())
        return true;

    if (text == NULL)
        text = "";

    for (int i = 0; i != Filters.Size; i++)
    {
        const ImGuiTextRange& f = Filters[i];
        if (f.empty())
            continue;
        if (f.b[0] == '-')
        {
            // Subtract
            if (ImStristr(text, text_end, f.b + 1, f.e) != NULL)
                return false;
        } else
        {
            // Grep
            if (ImStristr(text, text_end, f.b, f.e) != NULL)
                return true;
        }
    }

    // Implicit * grep
    if (CountGrep == 0)
        return true;

    return false;
}

//-----------------------------------------------------------------------------
// [SECTION] ImGuiTextBuffer
//-----------------------------------------------------------------------------

// On some platform vsnprintf() takes va_list by reference and modifies it.
// va_copy is the 'correct' way to copy a va_list but Visual Studio prior to 2013 doesn't have it.
#ifndef va_copy
#if defined(__GNUC__) || defined(__clang__)
#define va_copy(dest, src) __builtin_va_copy(dest, src)
#else
#define va_copy(dest, src) (dest = src)
#endif
#endif

char ImGuiTextBuffer::EmptyString[1] = { 0 };

void ImGuiTextBuffer::append(const char* str, const char* str_end)
{
    int len = str_end ? (int)(str_end - str) : (int)strlen(str);

    // Add zero-terminator the first time
    const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
    const int needed_sz = write_off + len;
    if (write_off + len >= Buf.Capacity)
    {
        int new_capacity = Buf.Capacity * 2;
        Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
    }

    Buf.resize(needed_sz);
    memcpy(&Buf[write_off - 1], str, (size_t)len);
    Buf[write_off - 1 + len] = 0;
}

void ImGuiTextBuffer::appendf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    appendfv(fmt, args);
    va_end(args);
}

// Helper: Text buffer for logging/accumulating text
void ImGuiTextBuffer::appendfv(const char* fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);

    int len = ImFormatStringV(NULL, 0, fmt, args);         // FIXME-OPT: could do a first pass write attempt, likely successful on first pass.
    if (len <= 0)
    {
        va_end(args_copy);
        return;
    }

    // Add zero-terminator the first time
    const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
    const int needed_sz = write_off + len;
    if (write_off + len >= Buf.Capacity)
    {
        int new_capacity = Buf.Capacity * 2;
        Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
    }

    Buf.resize(needed_sz);
    ImFormatStringV(&Buf[write_off - 1], (size_t)len + 1, fmt, args_copy);
    va_end(args_copy);
}

//-----------------------------------------------------------------------------
// [SECTION] ImGuiListClipper
// This is currently not as flexible/powerful as it should be and really confusing/spaghetti, mostly because we changed
// the API mid-way through development and support two ways to using the clipper, needs some rework (see TODO)
//-----------------------------------------------------------------------------

// Helper to calculate coarse clipping of large list of evenly sized items.
// NB: Prefer using the ImGuiListClipper higher-level helper if you can! Read comments and instructions there on how those use this sort of pattern.
// NB: 'items_count' is only used to clamp the result, if you don't know your count you can use INT_MAX
void ImGui::CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (g.LogEnabled)
    {
        // If logging is active, do not perform any clipping
        *out_items_display_start = 0;
        *out_items_display_end = items_count;
        return;
    }
    if (window->SkipItems)
    {
        *out_items_display_start = *out_items_display_end = 0;
        return;
    }

    // We create the union of the ClipRect and the NavScoringRect which at worst should be 1 page away from ClipRect
    ImRect unclipped_rect = window->ClipRect;
    if (g.NavMoveRequest)
        unclipped_rect.Add(g.NavScoringRect);
    if (g.NavJustMovedToId && window->NavLastIds[0] == g.NavJustMovedToId)
        unclipped_rect.Add(ImRect(window->Pos + window->NavRectRel[0].Min, window->Pos + window->NavRectRel[0].Max));

    const ImVec2 pos = window->DC.CursorPos;
    int start = (int)((unclipped_rect.Min.y - pos.y) / items_height);
    int end = (int)((unclipped_rect.Max.y - pos.y) / items_height);

    // When performing a navigation request, ensure we have one item extra in the direction we are moving to
    if (g.NavMoveRequest && g.NavMoveClipDir == ImGuiDir_Up)
        start--;
    if (g.NavMoveRequest && g.NavMoveClipDir == ImGuiDir_Down)
        end++;

    start = ImClamp(start, 0, items_count);
    end = ImClamp(end + 1, start, items_count);
    *out_items_display_start = start;
    *out_items_display_end = end;
}

static void SetCursorPosYAndSetupDummyPrevLine(float pos_y, float line_height)
{
    // Set cursor position and a few other things so that SetScrollHereY() and Columns() can work when seeking cursor.
    // FIXME: It is problematic that we have to do that here, because custom/equivalent end-user code would stumble on the same issue.
    // The clipper should probably have a 4th step to display the last item in a regular manner.
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    window->DC.CursorPos.y = pos_y;
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, pos_y);
    window->DC.CursorPosPrevLine.y = window->DC.CursorPos.y - line_height;  // Setting those fields so that SetScrollHereY() can properly function after the end of our clipper usage.
    window->DC.PrevLineSize.y = (line_height - g.Style.ItemSpacing.y);      // If we end up needing more accurate data (to e.g. use SameLine) we may as well make the clipper have a fourth step to let user process and display the last item in their list.
    if (ImGuiColumns* columns = window->DC.CurrentColumns)
        columns->LineMinY = window->DC.CursorPos.y;                         // Setting this so that cell Y position are set properly
}

// Use case A: Begin() called from constructor with items_height<0, then called again from Sync() in StepNo 1
// Use case B: Begin() called from constructor with items_height>0
// FIXME-LEGACY: Ideally we should remove the Begin/End functions but they are part of the legacy API we still support. This is why some of the code in Step() calling Begin() and reassign some fields, spaghetti style.
void ImGuiListClipper::Begin(int count, float items_height)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    StartPosY = window->DC.CursorPos.y;
    ItemsHeight = items_height;
    ItemsCount = count;
    StepNo = 0;
    DisplayEnd = DisplayStart = -1;
    if (ItemsHeight > 0.0f)
    {
        ImGui::CalcListClipping(ItemsCount, ItemsHeight, &DisplayStart, &DisplayEnd); // calculate how many to clip/display
        if (DisplayStart > 0)
            SetCursorPosYAndSetupDummyPrevLine(StartPosY + DisplayStart * ItemsHeight, ItemsHeight); // advance cursor
        StepNo = 2;
    }
}

void ImGuiListClipper::End()
{
    if (ItemsCount < 0)
        return;
    // In theory here we should assert that ImGui::GetCursorPosY() == StartPosY + DisplayEnd * ItemsHeight, but it feels saner to just seek at the end and not assert/crash the user.
    if (ItemsCount < INT_MAX)
        SetCursorPosYAndSetupDummyPrevLine(StartPosY + ItemsCount * ItemsHeight, ItemsHeight); // advance cursor
    ItemsCount = -1;
    StepNo = 3;
}

bool ImGuiListClipper::Step()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    if (ItemsCount == 0 || window->SkipItems)
    {
        ItemsCount = -1;
        return false;
    }
    if (StepNo == 0) // Step 0: the clipper let you process the first element, regardless of it being visible or not, so we can measure the element height.
    {
        DisplayStart = 0;
        DisplayEnd = 1;
        StartPosY = window->DC.CursorPos.y;
        StepNo = 1;
        return true;
    }
    if (StepNo == 1) // Step 1: the clipper infer height from first element, calculate the actual range of elements to display, and position the cursor before the first element.
    {
        if (ItemsCount == 1) { ItemsCount = -1; return false; }
        float items_height = window->DC.CursorPos.y - StartPosY;
        IM_ASSERT(items_height > 0.0f);   // If this triggers, it means Item 0 hasn't moved the cursor vertically
        Begin(ItemsCount - 1, items_height);
        DisplayStart++;
        DisplayEnd++;
        StepNo = 3;
        return true;
    }
    if (StepNo == 2) // Step 2: dummy step only required if an explicit items_height was passed to constructor or Begin() and user still call Step(). Does nothing and switch to Step 3.
    {
        IM_ASSERT(DisplayStart >= 0 && DisplayEnd >= 0);
        StepNo = 3;
        return true;
    }
    if (StepNo == 3) // Step 3: the clipper validate that we have reached the expected Y position (corresponding to element DisplayEnd), advance the cursor to the end of the list and then returns 'false' to end the loop.
        End();
    return false;
}

//-----------------------------------------------------------------------------
// [SECTION] STYLING
//-----------------------------------------------------------------------------

ImGuiStyle& ImGui::GetStyle()
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() and ImGui::SetCurrentContext() ?");
    return GImGui->Style;
}

ImU32 ImGui::GetColorU32(ImGuiCol idx, float alpha_mul)
{
    ImGuiStyle& style = GImGui->Style;
    ImVec4 c = style.Colors[idx];
    c.w *= style.Alpha * alpha_mul;
    return ColorConvertFloat4ToU32(c);
}

ImU32 ImGui::GetColorU32(const ImVec4& col)
{
    ImGuiStyle& style = GImGui->Style;
    ImVec4 c = col;
    c.w *= style.Alpha;
    return ColorConvertFloat4ToU32(c);
}

const ImVec4& ImGui::GetStyleColorVec4(ImGuiCol idx)
{
    ImGuiStyle& style = GImGui->Style;
    return style.Colors[idx];
}

ImU32 ImGui::GetColorU32(ImU32 col)
{
    ImGuiStyle& style = GImGui->Style;
    if (style.Alpha >= 1.0f)
        return col;
    ImU32 a = (col & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT;
    a = (ImU32)(a * style.Alpha); // We don't need to clamp 0..255 because Style.Alpha is in 0..1 range.
    return (col & ~IM_COL32_A_MASK) | (a << IM_COL32_A_SHIFT);
}

// FIXME: This may incur a round-trip (if the end user got their data from a float4) but eventually we aim to store the in-flight colors as ImU32
void ImGui::PushStyleColor(ImGuiCol idx, ImU32 col)
{
    ImGuiContext& g = *GImGui;
    ImGuiColorMod backup;
    backup.Col = idx;
    backup.BackupValue = g.Style.Colors[idx];
    g.ColorModifiers.push_back(backup);
    g.Style.Colors[idx] = ColorConvertU32ToFloat4(col);
}

void ImGui::PushStyleColor(ImGuiCol idx, const ImVec4& col)
{
    ImGuiContext& g = *GImGui;
    ImGuiColorMod backup;
    backup.Col = idx;
    backup.BackupValue = g.Style.Colors[idx];
    g.ColorModifiers.push_back(backup);
    g.Style.Colors[idx] = col;
}

void ImGui::PopStyleColor(int count)
{
    ImGuiContext& g = *GImGui;
    while (count > 0)
    {
        ImGuiColorMod& backup = g.ColorModifiers.back();
        g.Style.Colors[backup.Col] = backup.BackupValue;
        g.ColorModifiers.pop_back();
        count--;
    }
}

struct ImGuiStyleVarInfo
{
    ImGuiDataType   Type;
    ImU32           Count;
    ImU32           Offset;
    void* GetVarPtr(ImGuiStyle* style) const { return (void*)((unsigned char*)style + Offset); }
};

static const ImGuiStyleVarInfo GStyleVarInfo[] =
{
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, Alpha) },               // ImGuiStyleVar_Alpha
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImGuiStyle, WindowPadding) },       // ImGuiStyleVar_WindowPadding
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, WindowRounding) },      // ImGuiStyleVar_WindowRounding
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, WindowBorderSize) },    // ImGuiStyleVar_WindowBorderSize
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImGuiStyle, WindowMinSize) },       // ImGuiStyleVar_WindowMinSize
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImGuiStyle, WindowTitleAlign) },    // ImGuiStyleVar_WindowTitleAlign
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, ChildRounding) },       // ImGuiStyleVar_ChildRounding
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, ChildBorderSize) },     // ImGuiStyleVar_ChildBorderSize
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, PopupRounding) },       // ImGuiStyleVar_PopupRounding
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, PopupBorderSize) },     // ImGuiStyleVar_PopupBorderSize
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImGuiStyle, FramePadding) },        // ImGuiStyleVar_FramePadding
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, FrameRounding) },       // ImGuiStyleVar_FrameRounding
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, FrameBorderSize) },     // ImGuiStyleVar_FrameBorderSize
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImGuiStyle, ItemSpacing) },         // ImGuiStyleVar_ItemSpacing
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImGuiStyle, ItemInnerSpacing) },    // ImGuiStyleVar_ItemInnerSpacing
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, IndentSpacing) },       // ImGuiStyleVar_IndentSpacing
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, ScrollbarSize) },       // ImGuiStyleVar_ScrollbarSize
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, ScrollbarRounding) },   // ImGuiStyleVar_ScrollbarRounding
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, GrabMinSize) },         // ImGuiStyleVar_GrabMinSize
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, GrabRounding) },        // ImGuiStyleVar_GrabRounding
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImGuiStyle, TabRounding) },         // ImGuiStyleVar_TabRounding
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImGuiStyle, ButtonTextAlign) },     // ImGuiStyleVar_ButtonTextAlign
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImGuiStyle, SelectableTextAlign) }, // ImGuiStyleVar_SelectableTextAlign
};

static const ImGuiStyleVarInfo* GetStyleVarInfo(ImGuiStyleVar idx)
{
    IM_ASSERT(idx >= 0 && idx < ImGuiStyleVar_COUNT);
    IM_ASSERT(IM_ARRAYSIZE(GStyleVarInfo) == ImGuiStyleVar_COUNT);
    return &GStyleVarInfo[idx];
}

void ImGui::PushStyleVar(ImGuiStyleVar idx, float val)
{
    const ImGuiStyleVarInfo* var_info = GetStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_Float && var_info->Count == 1)
    {
        ImGuiContext& g = *GImGui;
        float* pvar = (float*)var_info->GetVarPtr(&g.Style);
        g.StyleModifiers.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0 && "Called PushStyleVar() float variant but variable is not a float!");
}

void ImGui::PushStyleVar(ImGuiStyleVar idx, const ImVec2& val)
{
    const ImGuiStyleVarInfo* var_info = GetStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_Float && var_info->Count == 2)
    {
        ImGuiContext& g = *GImGui;
        ImVec2* pvar = (ImVec2*)var_info->GetVarPtr(&g.Style);
        g.StyleModifiers.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0 && "Called PushStyleVar() ImVec2 variant but variable is not a ImVec2!");
}

void ImGui::PopStyleVar(int count)
{
    ImGuiContext& g = *GImGui;
    while (count > 0)
    {
        // We avoid a generic memcpy(data, &backup.Backup.., GDataTypeSize[info->Type] * info->Count), the overhead in Debug is not worth it.
        ImGuiStyleMod& backup = g.StyleModifiers.back();
        const ImGuiStyleVarInfo* info = GetStyleVarInfo(backup.VarIdx);
        void* data = info->GetVarPtr(&g.Style);
        if (info->Type == ImGuiDataType_Float && info->Count == 1) { ((float*)data)[0] = backup.BackupFloat[0]; } else if (info->Type == ImGuiDataType_Float && info->Count == 2) { ((float*)data)[0] = backup.BackupFloat[0]; ((float*)data)[1] = backup.BackupFloat[1]; }
        g.StyleModifiers.pop_back();
        count--;
    }
}

const char* ImGui::GetStyleColorName(ImGuiCol idx)
{
    // Create switch-case from enum with regexp: ImGuiCol_{.*}, --> case ImGuiCol_\1: return "\1";
    switch (idx)
    {
    case ImGuiCol_Text: return "Text";
    case ImGuiCol_TextDisabled: return "TextDisabled";
    case ImGuiCol_WindowBg: return "WindowBg";
    case ImGuiCol_ChildBg: return "ChildBg";
    case ImGuiCol_PopupBg: return "PopupBg";
    case ImGuiCol_Border: return "Border";
    case ImGuiCol_BorderShadow: return "BorderShadow";
    case ImGuiCol_FrameBg: return "FrameBg";
    case ImGuiCol_FrameBgHovered: return "FrameBgHovered";
    case ImGuiCol_FrameBgActive: return "FrameBgActive";
    case ImGuiCol_TitleBg: return "TitleBg";
    case ImGuiCol_TitleBgActive: return "TitleBgActive";
    case ImGuiCol_TitleBgCollapsed: return "TitleBgCollapsed";
    case ImGuiCol_MenuBarBg: return "MenuBarBg";
    case ImGuiCol_ScrollbarBg: return "ScrollbarBg";
    case ImGuiCol_ScrollbarGrab: return "ScrollbarGrab";
    case ImGuiCol_ScrollbarGrabHovered: return "ScrollbarGrabHovered";
    case ImGuiCol_ScrollbarGrabActive: return "ScrollbarGrabActive";
    case ImGuiCol_CheckMark: return "CheckMark";
    case ImGuiCol_SliderGrab: return "SliderGrab";
    case ImGuiCol_SliderGrabActive: return "SliderGrabActive";
    case ImGuiCol_Button: return "Button";
    case ImGuiCol_ButtonHovered: return "ButtonHovered";
    case ImGuiCol_ButtonActive: return "ButtonActive";
    case ImGuiCol_Header: return "Header";
    case ImGuiCol_HeaderHovered: return "HeaderHovered";
    case ImGuiCol_HeaderActive: return "HeaderActive";
    case ImGuiCol_Separator: return "Separator";
    case ImGuiCol_SeparatorHovered: return "SeparatorHovered";
    case ImGuiCol_SeparatorActive: return "SeparatorActive";
    case ImGuiCol_ResizeGrip: return "ResizeGrip";
    case ImGuiCol_ResizeGripHovered: return "ResizeGripHovered";
    case ImGuiCol_ResizeGripActive: return "ResizeGripActive";
    case ImGuiCol_Tab: return "Tab";
    case ImGuiCol_TabHovered: return "TabHovered";
    case ImGuiCol_TabActive: return "TabActive";
    case ImGuiCol_TabUnfocused: return "TabUnfocused";
    case ImGuiCol_TabUnfocusedActive: return "TabUnfocusedActive";
    case ImGuiCol_PlotLines: return "PlotLines";
    case ImGuiCol_PlotLinesHovered: return "PlotLinesHovered";
    case ImGuiCol_PlotHistogram: return "PlotHistogram";
    case ImGuiCol_PlotHistogramHovered: return "PlotHistogramHovered";
    case ImGuiCol_TextSelectedBg: return "TextSelectedBg";
    case ImGuiCol_DragDropTarget: return "DragDropTarget";
    case ImGuiCol_NavHighlight: return "NavHighlight";
    case ImGuiCol_NavWindowingHighlight: return "NavWindowingHighlight";
    case ImGuiCol_NavWindowingDimBg: return "NavWindowingDimBg";
    case ImGuiCol_ModalWindowDimBg: return "ModalWindowDimBg";
    }
    IM_ASSERT(0);
    return "Unknown";
}

//-----------------------------------------------------------------------------
// [SECTION] RENDER HELPERS
// Some of those (internal) functions are currently quite a legacy mess - their signature and behavior will change,
// we need a nicer separation between low-level functions and high-level functions relying on the ImGui context.
// Also see imgui_draw.cpp for some more which have been reworked to not rely on ImGui:: context.
//-----------------------------------------------------------------------------

const char* ImGui::FindRenderedTextEnd(const char* text, const char* text_end)
{
    const char* text_display_end = text;
    if (!text_end)
        text_end = (const char*)-1;

    while (text_display_end < text_end && *text_display_end != '\0' && (text_display_end[0] != '#' || text_display_end[1] != '#'))
        text_display_end++;
    return text_display_end;
}

// Internal ImGui functions to render text
// RenderText***() functions calls ImDrawList::AddText() calls ImBitmapFont::RenderText()
void ImGui::RenderText(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    // Hide anything after a '##' string
    const char* text_display_end;
    if (hide_text_after_hash)
    {
        text_display_end = FindRenderedTextEnd(text, text_end);
    } else
    {
        if (!text_end)
            text_end = text + strlen(text); // FIXME-OPT
        text_display_end = text_end;
    }

    if (text != text_display_end)
    {
        window->DrawList->AddText(g.Font, g.FontSize, pos, GetColorU32(ImGuiCol_Text), text, text_display_end);
        if (g.LogEnabled)
            LogRenderedText(&pos, text, text_display_end);
    }
}

void ImGui::RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    if (!text_end)
        text_end = text + strlen(text); // FIXME-OPT

    if (text != text_end)
    {
        window->DrawList->AddText(g.Font, g.FontSize, pos, GetColorU32(ImGuiCol_Text), text, text_end, wrap_width);
        if (g.LogEnabled)
            LogRenderedText(&pos, text, text_end);
    }
}

// Default clip_rect uses (pos_min,pos_max)
// Handle clipping on CPU immediately (vs typically let the GPU clip the triangles that are overlapping the clipping rectangle edges)
void ImGui::RenderTextClippedEx(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_display_end, const ImVec2* text_size_if_known, const ImVec2& align, const ImRect* clip_rect)
{
    // Perform CPU side clipping for single clipped element to avoid using scissor state
    ImVec2 pos = pos_min;
    const ImVec2 text_size = text_size_if_known ? *text_size_if_known : CalcTextSize(text, text_display_end, false, 0.0f);

    const ImVec2* clip_min = clip_rect ? &clip_rect->Min : &pos_min;
    const ImVec2* clip_max = clip_rect ? &clip_rect->Max : &pos_max;
    bool need_clipping = (pos.x + text_size.x >= clip_max->x) || (pos.y + text_size.y >= clip_max->y);
    if (clip_rect) // If we had no explicit clipping rectangle then pos==clip_min
        need_clipping |= (pos.x < clip_min->x) || (pos.y < clip_min->y);

    // Align whole block. We should defer that to the better rendering function when we'll have support for individual line alignment.
    if (align.x > 0.0f) pos.x = ImMax(pos.x, pos.x + (pos_max.x - pos.x - text_size.x) * align.x);
    if (align.y > 0.0f) pos.y = ImMax(pos.y, pos.y + (pos_max.y - pos.y - text_size.y) * align.y);

    // Render
    if (need_clipping)
    {
        ImVec4 fine_clip_rect(clip_min->x, clip_min->y, clip_max->x, clip_max->y);
        draw_list->AddText(NULL, 0.0f, pos, GetColorU32(ImGuiCol_Text), text, text_display_end, 0.0f, &fine_clip_rect);
    } else
    {
        draw_list->AddText(NULL, 0.0f, pos, GetColorU32(ImGuiCol_Text), text, text_display_end, 0.0f, NULL);
    }
}

void ImGui::RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align, const ImRect* clip_rect)
{
    // Hide anything after a '##' string
    const char* text_display_end = FindRenderedTextEnd(text, text_end);
    const int text_len = (int)(text_display_end - text);
    if (text_len == 0)
        return;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    RenderTextClippedEx(window->DrawList, pos_min, pos_max, text, text_display_end, text_size_if_known, align, clip_rect);
    if (g.LogEnabled)
        LogRenderedText(&pos_min, text, text_display_end);
}


// Another overly complex function until we reorganize everything into a nice all-in-one helper.
// This is made more complex because we have dissociated the layout rectangle (pos_min..pos_max) which define _where_ the ellipsis is, from actual clipping of text and limit of the ellipsis display.
// This is because in the context of tabs we selectively hide part of the text when the Close Button appears, but we don't want the ellipsis to move.
void ImGui::RenderTextEllipsis(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, float clip_max_x, float ellipsis_max_x, const char* text, const char* text_end_full, const ImVec2* text_size_if_known)
{
    ImGuiContext& g = *GImGui;
    if (text_end_full == NULL)
        text_end_full = FindRenderedTextEnd(text);
    const ImVec2 text_size = text_size_if_known ? *text_size_if_known : CalcTextSize(text, text_end_full, false, 0.0f);

    //draw_list->AddLine(ImVec2(pos_max.x, pos_min.y - 4), ImVec2(pos_max.x, pos_max.y + 4), IM_COL32(0, 0, 255, 255));
    //draw_list->AddLine(ImVec2(ellipsis_max_x, pos_min.y-2), ImVec2(ellipsis_max_x, pos_max.y+2), IM_COL32(0, 255, 0, 255));
    //draw_list->AddLine(ImVec2(clip_max_x, pos_min.y), ImVec2(clip_max_x, pos_max.y), IM_COL32(255, 0, 0, 255));
    // FIXME: We could technically remove (last_glyph->AdvanceX - last_glyph->X1) from text_size.x here and save a few pixels.
    if (text_size.x > pos_max.x - pos_min.x)
    {
        // Hello wo...
        // |       |   |
        // min   max   ellipsis_max
        //          <-> this is generally some padding value

        const ImFont* font = draw_list->_Data->Font;
        const float font_size = draw_list->_Data->FontSize;
        const char* text_end_ellipsis = NULL;

        ImWchar ellipsis_char = font->EllipsisChar;
        int ellipsis_char_count = 1;
        if (ellipsis_char == (ImWchar)-1)
        {
            ellipsis_char = (ImWchar)'.';
            ellipsis_char_count = 3;
        }
        const ImFontGlyph* glyph = font->FindGlyph(ellipsis_char);

        float ellipsis_glyph_width = glyph->X1;                 // Width of the glyph with no padding on either side
        float ellipsis_total_width = ellipsis_glyph_width;      // Full width of entire ellipsis

        if (ellipsis_char_count > 1)
        {
            // Full ellipsis size without free spacing after it.
            const float spacing_between_dots = 1.0f * (draw_list->_Data->FontSize / font->FontSize);
            ellipsis_glyph_width = glyph->X1 - glyph->X0 + spacing_between_dots;
            ellipsis_total_width = ellipsis_glyph_width * (float)ellipsis_char_count - spacing_between_dots;
        }

        // We can now claim the space between pos_max.x and ellipsis_max.x
        const float text_avail_width = ImMax((ImMax(pos_max.x, ellipsis_max_x) - ellipsis_total_width) - pos_min.x, 1.0f);
        float text_size_clipped_x = font->CalcTextSizeA(font_size, text_avail_width, 0.0f, text, text_end_full, &text_end_ellipsis).x;
        if (text == text_end_ellipsis && text_end_ellipsis < text_end_full)
        {
            // Always display at least 1 character if there's no room for character + ellipsis
            text_end_ellipsis = text + ImTextCountUtf8BytesFromChar(text, text_end_full);
            text_size_clipped_x = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text, text_end_ellipsis).x;
        }
        while (text_end_ellipsis > text && ImCharIsBlankA(text_end_ellipsis[-1]))
        {
            // Trim trailing space before ellipsis (FIXME: Supporting non-ascii blanks would be nice, for this we need a function to backtrack in UTF-8 text)
            text_end_ellipsis--;
            text_size_clipped_x -= font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text_end_ellipsis, text_end_ellipsis + 1).x; // Ascii blanks are always 1 byte
        }

        // Render text, render ellipsis
        RenderTextClippedEx(draw_list, pos_min, ImVec2(clip_max_x, pos_max.y), text, text_end_ellipsis, &text_size, ImVec2(0.0f, 0.0f));
        float ellipsis_x = pos_min.x + text_size_clipped_x;
        if (ellipsis_x + ellipsis_total_width <= ellipsis_max_x)
            for (int i = 0; i < ellipsis_char_count; i++)
            {
                font->RenderChar(draw_list, font_size, ImVec2(ellipsis_x, pos_min.y), GetColorU32(ImGuiCol_Text), ellipsis_char);
                ellipsis_x += ellipsis_glyph_width;
            }
    } else
    {
        RenderTextClippedEx(draw_list, pos_min, ImVec2(clip_max_x, pos_max.y), text, text_end_full, &text_size, ImVec2(0.0f, 0.0f));
    }

    if (g.LogEnabled)
        LogRenderedText(&pos_min, text, text_end_full);
}

// Render a rectangle shaped with optional rounding and borders
void ImGui::RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
    const float border_size = g.Style.FrameBorderSize;
    if (border && border_size > 0.0f)
    {
        window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow), rounding, ImDrawCornerFlags_All, border_size);
        window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border), rounding, ImDrawCornerFlags_All, border_size);
    }
}

void ImGui::RenderFrameBorder(ImVec2 p_min, ImVec2 p_max, float rounding)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    const float border_size = g.Style.FrameBorderSize;
    if (border_size > 0.0f)
    {
        window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow), rounding, ImDrawCornerFlags_All, border_size);
        window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border), rounding, ImDrawCornerFlags_All, border_size);
    }
}

void ImGui::RenderNavHighlight(const ImRect& bb, ImGuiID id, ImGuiNavHighlightFlags flags)
{
    ImGuiContext& g = *GImGui;
    if (id != g.NavId)
        return;
    if (g.NavDisableHighlight && !(flags & ImGuiNavHighlightFlags_AlwaysDraw))
        return;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->DC.NavHideHighlightOneFrame)
        return;

    float rounding = (flags & ImGuiNavHighlightFlags_NoRounding) ? 0.0f : g.Style.FrameRounding;
    ImRect display_rect = bb;
    display_rect.ClipWith(window->ClipRect);
    if (flags & ImGuiNavHighlightFlags_TypeDefault)
    {
        const float THICKNESS = 2.0f;
        const float DISTANCE = 3.0f + THICKNESS * 0.5f;
        display_rect.Expand(ImVec2(DISTANCE, DISTANCE));
        bool fully_visible = window->ClipRect.Contains(display_rect);
        if (!fully_visible)
            window->DrawList->PushClipRect(display_rect.Min, display_rect.Max);
        window->DrawList->AddRect(display_rect.Min + ImVec2(THICKNESS * 0.5f, THICKNESS * 0.5f), display_rect.Max - ImVec2(THICKNESS * 0.5f, THICKNESS * 0.5f), GetColorU32(ImGuiCol_NavHighlight), rounding, ImDrawCornerFlags_All, THICKNESS);
        if (!fully_visible)
            window->DrawList->PopClipRect();
    }
    if (flags & ImGuiNavHighlightFlags_TypeThin)
    {
        window->DrawList->AddRect(display_rect.Min, display_rect.Max, GetColorU32(ImGuiCol_NavHighlight), rounding, ~0, 1.0f);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] MAIN CODE (most of the code! lots of stuff, needs tidying up!)
//-----------------------------------------------------------------------------

// ImGuiWindow is mostly a dumb struct. It merely has a constructor and a few helper methods
ImGuiWindow::ImGuiWindow(ImGuiContext* context, const char* name)
    : DrawListInst(&context->DrawListSharedData)
{
    Name = ImStrdup(name);
    ID = ImHashStr(name);
    IDStack.push_back(ID);
    Flags = ImGuiWindowFlags_None;
    Pos = ImVec2(0.0f, 0.0f);
    Size = SizeFull = ImVec2(0.0f, 0.0f);
    ContentSize = ContentSizeExplicit = ImVec2(0.0f, 0.0f);
    WindowPadding = ImVec2(0.0f, 0.0f);
    WindowRounding = 0.0f;
    WindowBorderSize = 0.0f;
    NameBufLen = (int)strlen(name) + 1;
    MoveId = GetID("#MOVE");
    ChildId = 0;
    Scroll = ImVec2(0.0f, 0.0f);
    ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);
    ScrollTargetCenterRatio = ImVec2(0.5f, 0.5f);
    ScrollbarSizes = ImVec2(0.0f, 0.0f);
    ScrollbarX = ScrollbarY = false;
    Active = WasActive = false;
    WriteAccessed = false;
    Collapsed = false;
    WantCollapseToggle = false;
    SkipItems = false;
    Appearing = false;
    Hidden = false;
    IsFallbackWindow = false;
    HasCloseButton = false;
    ResizeBorderHeld = -1;
    BeginCount = 0;
    BeginOrderWithinParent = -1;
    BeginOrderWithinContext = -1;
    PopupId = 0;
    AutoFitFramesX = AutoFitFramesY = -1;
    AutoFitChildAxises = 0x00;
    AutoFitOnlyGrows = false;
    AutoPosLastDirection = ImGuiDir_None;
    HiddenFramesCanSkipItems = HiddenFramesCannotSkipItems = 0;
    SetWindowPosAllowFlags = SetWindowSizeAllowFlags = SetWindowCollapsedAllowFlags = ImGuiCond_Always | ImGuiCond_Once | ImGuiCond_FirstUseEver | ImGuiCond_Appearing;
    SetWindowPosVal = SetWindowPosPivot = ImVec2(FLT_MAX, FLT_MAX);

    InnerRect = ImRect(0.0f, 0.0f, 0.0f, 0.0f); // Clear so the InnerRect.GetSize() code in Begin() doesn't lead to overflow even if the result isn't used.

    LastFrameActive = -1;
    LastTimeActive = -1.0f;
    ItemWidthDefault = 0.0f;
    FontWindowScale = 1.0f;
    SettingsOffset = -1;

    DrawList = &DrawListInst;
    DrawList->_OwnerName = Name;
    ParentWindow = NULL;
    RootWindow = NULL;
    RootWindowForTitleBarHighlight = NULL;
    RootWindowForNav = NULL;

    NavLastIds[0] = NavLastIds[1] = 0;
    NavRectRel[0] = NavRectRel[1] = ImRect();
    NavLastChildNavWindow = NULL;

    MemoryCompacted = false;
    MemoryDrawListIdxCapacity = MemoryDrawListVtxCapacity = 0;
}

ImGuiWindow::~ImGuiWindow()
{
    IM_ASSERT(DrawList == &DrawListInst);
    IM_DELETE(Name);
    for (int i = 0; i != ColumnsStorage.Size; i++)
        ColumnsStorage[i].~ImGuiColumns();
}

ImGuiID ImGuiWindow::GetID(const char* str, const char* str_end)
{
    ImGuiID seed = IDStack.back();
    ImGuiID id = ImHashStr(str, str_end ? (str_end - str) : 0, seed);
    ImGui::KeepAliveID(id);
#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiContext& g = *GImGui;
    IMGUI_TEST_ENGINE_ID_INFO2(id, ImGuiDataType_String, str, str_end);
#endif
    return id;
}

ImGuiID ImGuiWindow::GetID(const void* ptr)
{
    ImGuiID seed = IDStack.back();
    ImGuiID id = ImHashData(&ptr, sizeof(void*), seed);
    ImGui::KeepAliveID(id);
#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiContext& g = *GImGui;
    IMGUI_TEST_ENGINE_ID_INFO(id, ImGuiDataType_Pointer, ptr);
#endif
    return id;
}

ImGuiID ImGuiWindow::GetID(int n)
{
    ImGuiID seed = IDStack.back();
    ImGuiID id = ImHashData(&n, sizeof(n), seed);
    ImGui::KeepAliveID(id);
#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiContext& g = *GImGui;
    IMGUI_TEST_ENGINE_ID_INFO(id, ImGuiDataType_S32, (intptr_t)n);
#endif
    return id;
}

ImGuiID ImGuiWindow::GetIDNoKeepAlive(const char* str, const char* str_end)
{
    ImGuiID seed = IDStack.back();
    ImGuiID id = ImHashStr(str, str_end ? (str_end - str) : 0, seed);
#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiContext& g = *GImGui;
    IMGUI_TEST_ENGINE_ID_INFO2(id, ImGuiDataType_String, str, str_end);
#endif
    return id;
}

ImGuiID ImGuiWindow::GetIDNoKeepAlive(const void* ptr)
{
    ImGuiID seed = IDStack.back();
    ImGuiID id = ImHashData(&ptr, sizeof(void*), seed);
#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiContext& g = *GImGui;
    IMGUI_TEST_ENGINE_ID_INFO(id, ImGuiDataType_Pointer, ptr);
#endif
    return id;
}

ImGuiID ImGuiWindow::GetIDNoKeepAlive(int n)
{
    ImGuiID seed = IDStack.back();
    ImGuiID id = ImHashData(&n, sizeof(n), seed);
#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiContext& g = *GImGui;
    IMGUI_TEST_ENGINE_ID_INFO(id, ImGuiDataType_S32, (intptr_t)n);
#endif
    return id;
}

// This is only used in rare/specific situations to manufacture an ID out of nowhere.
ImGuiID ImGuiWindow::GetIDFromRectangle(const ImRect& r_abs)
{
    ImGuiID seed = IDStack.back();
    const int r_rel[4] = { (int)(r_abs.Min.x - Pos.x), (int)(r_abs.Min.y - Pos.y), (int)(r_abs.Max.x - Pos.x), (int)(r_abs.Max.y - Pos.y) };
    ImGuiID id = ImHashData(&r_rel, sizeof(r_rel), seed);
    ImGui::KeepAliveID(id);
    return id;
}

static void SetCurrentWindow(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    g.CurrentWindow = window;
    if (window)
        g.FontSize = g.DrawListSharedData.FontSize = window->CalcFontSize();
}

// Free up/compact internal window buffers, we can use this when a window becomes unused.
// This is currently unused by the library, but you may call this yourself for easy GC.
// Not freed:
// - ImGuiWindow, ImGuiWindowSettings, Name
// - StateStorage, ColumnsStorage (may hold useful data)
// This should have no noticeable visual effect. When the window reappear however, expect new allocation/buffer growth/copy cost.
void ImGui::GcCompactTransientWindowBuffers(ImGuiWindow* window)
{
    window->MemoryCompacted = true;
    window->MemoryDrawListIdxCapacity = window->DrawList->IdxBuffer.Capacity;
    window->MemoryDrawListVtxCapacity = window->DrawList->VtxBuffer.Capacity;
    window->IDStack.clear();
    window->DrawList->_ClearFreeMemory();
    window->DC.ChildWindows.clear();
    window->DC.ItemFlagsStack.clear();
    window->DC.ItemWidthStack.clear();
    window->DC.TextWrapPosStack.clear();
    window->DC.GroupStack.clear();
}

void ImGui::GcAwakeTransientWindowBuffers(ImGuiWindow* window)
{
    // We stored capacity of the ImDrawList buffer to reduce growth-caused allocation/copy when awakening.
    // The other buffers tends to amortize much faster.
    window->MemoryCompacted = false;
    window->DrawList->IdxBuffer.reserve(window->MemoryDrawListIdxCapacity);
    window->DrawList->VtxBuffer.reserve(window->MemoryDrawListVtxCapacity);
    window->MemoryDrawListIdxCapacity = window->MemoryDrawListVtxCapacity = 0;
}

void ImGui::SetActiveID(ImGuiID id, ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    g.ActiveIdIsJustActivated = (g.ActiveId != id);
    if (g.ActiveIdIsJustActivated)
    {
        g.ActiveIdTimer = 0.0f;
        g.ActiveIdHasBeenPressedBefore = false;
        g.ActiveIdHasBeenEditedBefore = false;
        if (id != 0)
        {
            g.LastActiveId = id;
            g.LastActiveIdTimer = 0.0f;
        }
    }
    g.ActiveId = id;
    g.ActiveIdAllowOverlap = false;
    g.ActiveIdWindow = window;
    g.ActiveIdHasBeenEditedThisFrame = false;
    if (id)
    {
        g.ActiveIdIsAlive = id;
        g.ActiveIdSource = (g.NavActivateId == id || g.NavInputId == id || g.NavJustTabbedId == id || g.NavJustMovedToId == id) ? ImGuiInputSource_Nav : ImGuiInputSource_Mouse;
    }

    // Clear declaration of inputs claimed by the widget
    // (Please note that this is WIP and not all keys/inputs are thoroughly declared by all widgets yet)
    g.ActiveIdUsingNavDirMask = 0x00;
    g.ActiveIdUsingNavInputMask = 0x00;
    g.ActiveIdUsingKeyInputMask = 0x00;
}

void ImGui::ClearActiveID()
{
    SetActiveID(0, NULL);
}

void ImGui::SetHoveredID(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    g.HoveredId = id;
    g.HoveredIdAllowOverlap = false;
    if (id != 0 && g.HoveredIdPreviousFrame != id)
        g.HoveredIdTimer = g.HoveredIdNotActiveTimer = 0.0f;
}

ImGuiID ImGui::GetHoveredID()
{
    ImGuiContext& g = *GImGui;
    return g.HoveredId ? g.HoveredId : g.HoveredIdPreviousFrame;
}

void ImGui::KeepAliveID(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    if (g.ActiveId == id)
        g.ActiveIdIsAlive = id;
    if (g.ActiveIdPreviousFrame == id)
        g.ActiveIdPreviousFrameIsAlive = true;
}

void ImGui::MarkItemEdited(ImGuiID id)
{
    // This marking is solely to be able to provide info for IsItemDeactivatedAfterEdit().
    // ActiveId might have been released by the time we call this (as in the typical press/release button behavior) but still need need to fill the data.
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.ActiveId == id || g.ActiveId == 0 || g.DragDropActive);
    IM_UNUSED(id); // Avoid unused variable warnings when asserts are compiled out.
    //IM_ASSERT(g.CurrentWindow->DC.LastItemId == id);
    g.ActiveIdHasBeenEditedThisFrame = true;
    g.ActiveIdHasBeenEditedBefore = true;
    g.CurrentWindow->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_Edited;
}

static inline bool IsWindowContentHoverable(ImGuiWindow* window, ImGuiHoveredFlags flags)
{
    // An active popup disable hovering on other windows (apart from its own children)
    // FIXME-OPT: This could be cached/stored within the window.
    ImGuiContext& g = *GImGui;
    if (g.NavWindow)
        if (ImGuiWindow* focused_root_window = g.NavWindow->RootWindow)
            if (focused_root_window->WasActive && focused_root_window != window->RootWindow)
            {
                // For the purpose of those flags we differentiate "standard popup" from "modal popup"
                // NB: The order of those two tests is important because Modal windows are also Popups.
                if (focused_root_window->Flags & ImGuiWindowFlags_Modal)
                    return false;
                if ((focused_root_window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiHoveredFlags_AllowWhenBlockedByPopup))
                    return false;
            }
    return true;
}

// This is roughly matching the behavior of internal-facing ItemHoverable()
// - we allow hovering to be true when ActiveId==window->MoveID, so that clicking on non-interactive items such as a Text() item still returns true with IsItemHovered()
// - this should work even for non-interactive items that have no ID, so we cannot use LastItemId
bool ImGui::IsItemHovered(ImGuiHoveredFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (g.NavDisableMouseHover && !g.NavDisableHighlight)
        return IsItemFocused();

    // Test for bounding box overlap, as updated as ItemAdd()
    if (!(window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect))
        return false;
    IM_ASSERT((flags & (ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows)) == 0);   // Flags not supported by this function

    // Test if we are hovering the right window (our window could be behind another window)
    // [2017/10/16] Reverted commit 344d48be3 and testing RootWindow instead. I believe it is correct to NOT test for RootWindow but this leaves us unable to use IsItemHovered() after EndChild() itself.
    // Until a solution is found I believe reverting to the test from 2017/09/27 is safe since this was the test that has been running for a long while.
    //if (g.HoveredWindow != window)
    //    return false;
    if (g.HoveredRootWindow != window->RootWindow && !(flags & ImGuiHoveredFlags_AllowWhenOverlapped))
        return false;

    // Test if another item is active (e.g. being dragged)
    if (!(flags & ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        if (g.ActiveId != 0 && g.ActiveId != window->DC.LastItemId && !g.ActiveIdAllowOverlap && g.ActiveId != window->MoveId)
            return false;

    // Test if interactions on this window are blocked by an active popup or modal.
    // The ImGuiHoveredFlags_AllowWhenBlockedByPopup flag will be tested here.
    if (!IsWindowContentHoverable(window, flags))
        return false;

    // Test if the item is disabled
    if ((window->DC.ItemFlags & ImGuiItemFlags_Disabled) && !(flags & ImGuiHoveredFlags_AllowWhenDisabled))
        return false;

    // Special handling for the dummy item after Begin() which represent the title bar or tab.
    // When the window is collapsed (SkipItems==true) that last item will never be overwritten so we need to detect the case.
    if (window->DC.LastItemId == window->MoveId && window->WriteAccessed)
        return false;
    return true;
}

// Internal facing ItemHoverable() used when submitting widgets. Differs slightly from IsItemHovered().
bool ImGui::ItemHoverable(const ImRect& bb, ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    if (g.HoveredId != 0 && g.HoveredId != id && !g.HoveredIdAllowOverlap)
        return false;

    ImGuiWindow* window = g.CurrentWindow;
    if (g.HoveredWindow != window)
        return false;
    if (g.ActiveId != 0 && g.ActiveId != id && !g.ActiveIdAllowOverlap)
        return false;
    if (!IsMouseHoveringRect(bb.Min, bb.Max))
        return false;
    if (g.NavDisableMouseHover || !IsWindowContentHoverable(window, ImGuiHoveredFlags_None))
        return false;
    if (window->DC.ItemFlags & ImGuiItemFlags_Disabled)
        return false;

    SetHoveredID(id);

    // [DEBUG] Item Picker tool!
    // We perform the check here because SetHoveredID() is not frequently called (1~ time a frame), making
    // the cost of this tool near-zero. We can get slightly better call-stack and support picking non-hovered
    // items if we perform the test in ItemAdd(), but that would incur a small runtime cost.
    // #define IMGUI_DEBUG_TOOL_ITEM_PICKER_EX in imconfig.h if you want this check to also be performed in ItemAdd().
    if (g.DebugItemPickerActive && g.HoveredIdPreviousFrame == id)
        GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(255, 255, 0, 255));
    if (g.DebugItemPickerBreakId == id)
        IM_DEBUG_BREAK();

    return true;
}

bool ImGui::IsClippedEx(const ImRect& bb, ImGuiID id, bool clip_even_when_logged)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (!bb.Overlaps(window->ClipRect))
        if (id == 0 || (id != g.ActiveId && id != g.NavId))
            if (clip_even_when_logged || !g.LogEnabled)
                return true;
    return false;
}

// Process TAB/Shift+TAB. Be mindful that this function may _clear_ the ActiveID when tabbing out.
bool ImGui::FocusableItemRegister(ImGuiWindow* window, ImGuiID id)
{
    ImGuiContext& g = *GImGui;

    // Increment counters
    const bool is_tab_stop = (window->DC.ItemFlags & (ImGuiItemFlags_NoTabStop | ImGuiItemFlags_Disabled)) == 0;
    window->DC.FocusCounterRegular++;
    if (is_tab_stop)
        window->DC.FocusCounterTabStop++;

    // Process TAB/Shift-TAB to tab *OUT* of the currently focused item.
    // (Note that we can always TAB out of a widget that doesn't allow tabbing in)
    if (g.ActiveId == id && g.FocusTabPressed && !IsActiveIdUsingKey(ImGuiKey_Tab) && g.FocusRequestNextWindow == NULL)
    {
        g.FocusRequestNextWindow = window;
        g.FocusRequestNextCounterTabStop = window->DC.FocusCounterTabStop + (g.IO.KeyShift ? (is_tab_stop ? -1 : 0) : +1); // Modulo on index will be applied at the end of frame once we've got the total counter of items.
    }

    // Handle focus requests
    if (g.FocusRequestCurrWindow == window)
    {
        if (window->DC.FocusCounterRegular == g.FocusRequestCurrCounterRegular)
            return true;
        if (is_tab_stop && window->DC.FocusCounterTabStop == g.FocusRequestCurrCounterTabStop)
        {
            g.NavJustTabbedId = id;
            return true;
        }

        // If another item is about to be focused, we clear our own active id
        if (g.ActiveId == id)
            ClearActiveID();
    }

    return false;
}

void ImGui::FocusableItemUnregister(ImGuiWindow* window)
{
    window->DC.FocusCounterRegular--;
    window->DC.FocusCounterTabStop--;
}

float ImGui::CalcWrapWidthForPos(const ImVec2& pos, float wrap_pos_x)
{
    if (wrap_pos_x < 0.0f)
        return 0.0f;

    ImGuiWindow* window = GImGui->CurrentWindow;
    if (wrap_pos_x == 0.0f)
        wrap_pos_x = window->WorkRect.Max.x;
    else if (wrap_pos_x > 0.0f)
        wrap_pos_x += window->Pos.x - window->Scroll.x; // wrap_pos_x is provided is window local space

    return ImMax(wrap_pos_x - pos.x, 1.0f);
}

// IM_ALLOC() == ImGui::MemAlloc()
void* ImGui::MemAlloc(size_t size)
{
    if (ImGuiContext* ctx = GImGui)
        ctx->IO.MetricsActiveAllocations++;
    return GImAllocatorAllocFunc(size, GImAllocatorUserData);
}

// IM_FREE() == ImGui::MemFree()
void ImGui::MemFree(void* ptr)
{
    if (ptr)
        if (ImGuiContext* ctx = GImGui)
            ctx->IO.MetricsActiveAllocations--;
    return GImAllocatorFreeFunc(ptr, GImAllocatorUserData);
}

const char* ImGui::GetClipboardText()
{
    ImGuiContext& g = *GImGui;
    return g.IO.GetClipboardTextFn ? g.IO.GetClipboardTextFn(g.IO.ClipboardUserData) : "";
}

void ImGui::SetClipboardText(const char* text)
{
    ImGuiContext& g = *GImGui;
    if (g.IO.SetClipboardTextFn)
        g.IO.SetClipboardTextFn(g.IO.ClipboardUserData, text);
}

const char* ImGui::GetVersion()
{
    return IMGUI_VERSION;
}

// Internal state access - if you want to share Dear ImGui state between modules (e.g. DLL) or allocate it yourself
// Note that we still point to some static data and members (such as GFontAtlas), so the state instance you end up using will point to the static data within its module
ImGuiContext* ImGui::GetCurrentContext()
{
    return GImGui;
}

void ImGui::SetCurrentContext(ImGuiContext* ctx)
{
#ifdef IMGUI_SET_CURRENT_CONTEXT_FUNC
    IMGUI_SET_CURRENT_CONTEXT_FUNC(ctx); // For custom thread-based hackery you may want to have control over this.
#else
    GImGui = ctx;
#endif
}

void ImGui::SetAllocatorFunctions(void* (*alloc_func)(size_t sz, void* user_data), void (*free_func)(void* ptr, void* user_data), void* user_data)
{
    GImAllocatorAllocFunc = alloc_func;
    GImAllocatorFreeFunc = free_func;
    GImAllocatorUserData = user_data;
}

ImGuiContext* ImGui::CreateContext(ImFontAtlas* shared_font_atlas)
{
    ImGuiContext* ctx = IM_NEW(ImGuiContext)(shared_font_atlas);
    if (GImGui == NULL)
        SetCurrentContext(ctx);
    Initialize(ctx);
    return ctx;
}

void ImGui::DestroyContext(ImGuiContext* ctx)
{
    if (ctx == NULL)
        ctx = GImGui;
    Shutdown(ctx);
    if (GImGui == ctx)
        SetCurrentContext(NULL);
    IM_DELETE(ctx);
}

ImGuiIO& ImGui::GetIO()
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() and ImGui::SetCurrentContext() ?");
    return GImGui->IO;
}

// Same value as passed to the old io.RenderDrawListsFn function. Valid after Render() and until the next call to NewFrame()
ImDrawData* ImGui::GetDrawData()
{
    ImGuiContext& g = *GImGui;
    return g.DrawData.Valid ? &g.DrawData : NULL;
}

double ImGui::GetTime()
{
    return GImGui->Time;
}

int ImGui::GetFrameCount()
{
    return GImGui->FrameCount;
}

ImDrawList* ImGui::GetBackgroundDrawList()
{
    return &GImGui->BackgroundDrawList;
}

ImDrawList* ImGui::GetForegroundDrawList()
{
    return &GImGui->ForegroundDrawList;
}

ImDrawListSharedData* ImGui::GetDrawListSharedData()
{
    return &GImGui->DrawListSharedData;
}

void ImGui::StartMouseMovingWindow(ImGuiWindow* window)
{
    // Set ActiveId even if the _NoMove flag is set. Without it, dragging away from a window with _NoMove would activate hover on other windows.
    // We _also_ call this when clicking in a window empty space when io.ConfigWindowsMoveFromTitleBarOnly is set, but clear g.MovingWindow afterward.
    // This is because we want ActiveId to be set even when the window is not permitted to move.
    ImGuiContext& g = *GImGui;
    FocusWindow(window);
    SetActiveID(window->MoveId, window);
    g.NavDisableHighlight = true;
    g.ActiveIdClickOffset = g.IO.MousePos - window->RootWindow->Pos;

    bool can_move_window = true;
    if ((window->Flags & ImGuiWindowFlags_NoMove) || (window->RootWindow->Flags & ImGuiWindowFlags_NoMove))
        can_move_window = false;
    if (can_move_window)
        g.MovingWindow = window;
}

// Handle mouse moving window
// Note: moving window with the navigation keys (Square + d-pad / CTRL+TAB + Arrows) are processed in NavUpdateWindowing()
// FIXME: We don't have strong guarantee that g.MovingWindow stay synched with g.ActiveId == g.MovingWindow->MoveId.
// This is currently enforced by the fact that BeginDragDropSource() is setting all g.ActiveIdUsingXXXX flags to inhibit navigation inputs,
// but if we should more thoroughly test cases where g.ActiveId or g.MovingWindow gets changed and not the other.
void ImGui::UpdateMouseMovingWindowNewFrame()
{
    ImGuiContext& g = *GImGui;
    if (g.MovingWindow != NULL)
    {
        // We actually want to move the root window. g.MovingWindow == window we clicked on (could be a child window).
        // We track it to preserve Focus and so that generally ActiveIdWindow == MovingWindow and ActiveId == MovingWindow->MoveId for consistency.
        KeepAliveID(g.ActiveId);
        IM_ASSERT(g.MovingWindow && g.MovingWindow->RootWindow);
        ImGuiWindow* moving_window = g.MovingWindow->RootWindow;
        if (g.IO.MouseDown[0] && IsMousePosValid(&g.IO.MousePos))
        {
            ImVec2 pos = g.IO.MousePos - g.ActiveIdClickOffset;
            if (moving_window->Pos.x != pos.x || moving_window->Pos.y != pos.y)
            {
                MarkIniSettingsDirty(moving_window);
                SetWindowPos(moving_window, pos, ImGuiCond_Always);
            }
            FocusWindow(g.MovingWindow);
        } else
        {
            ClearActiveID();
            g.MovingWindow = NULL;
        }
    } else
    {
        // When clicking/dragging from a window that has the _NoMove flag, we still set the ActiveId in order to prevent hovering others.
        if (g.ActiveIdWindow && g.ActiveIdWindow->MoveId == g.ActiveId)
        {
            KeepAliveID(g.ActiveId);
            if (!g.IO.MouseDown[0])
                ClearActiveID();
        }
    }
}

// Initiate moving window when clicking on empty space or title bar.
// Handle left-click and right-click focus.
void ImGui::UpdateMouseMovingWindowEndFrame()
{
    ImGuiContext& g = *GImGui;
    if (g.ActiveId != 0 || g.HoveredId != 0)
        return;

    // Unless we just made a window/popup appear
    if (g.NavWindow && g.NavWindow->Appearing)
        return;

    // Click to focus window and start moving (after we're done with all our widgets)
    if (g.IO.MouseClicked[0])
    {
        if (g.HoveredRootWindow != NULL)
        {
            StartMouseMovingWindow(g.HoveredWindow);
            if (g.IO.ConfigWindowsMoveFromTitleBarOnly && !(g.HoveredRootWindow->Flags & ImGuiWindowFlags_NoTitleBar))
                if (!g.HoveredRootWindow->TitleBarRect().Contains(g.IO.MouseClickedPos[0]))
                    g.MovingWindow = NULL;
        } else if (g.NavWindow != NULL && GetTopMostPopupModal() == NULL)
        {
            // Clicking on void disable focus
            FocusWindow(NULL);
        }
    }

    // With right mouse button we close popups without changing focus based on where the mouse is aimed
    // Instead, focus will be restored to the window under the bottom-most closed popup.
    // (The left mouse button path calls FocusWindow on the hovered window, which will lead NewFrame->ClosePopupsOverWindow to trigger)
    if (g.IO.MouseClicked[1])
    {
        // Find the top-most window between HoveredWindow and the top-most Modal Window.
        // This is where we can trim the popup stack.
        ImGuiWindow* modal = GetTopMostPopupModal();
        bool hovered_window_above_modal = false;
        if (modal == NULL)
            hovered_window_above_modal = true;
        for (int i = g.Windows.Size - 1; i >= 0 && hovered_window_above_modal == false; i--)
        {
            ImGuiWindow* window = g.Windows[i];
            if (window == modal)
                break;
            if (window == g.HoveredWindow)
                hovered_window_above_modal = true;
        }
        ClosePopupsOverWindow(hovered_window_above_modal ? g.HoveredWindow : modal, true);
    }
}

static bool IsWindowActiveAndVisible(ImGuiWindow* window)
{
    return (window->Active) && (!window->Hidden);
}

static void ImGui::UpdateMouseInputs()
{
    ImGuiContext& g = *GImGui;

    // Round mouse position to avoid spreading non-rounded position (e.g. UpdateManualResize doesn't support them well)
    if (IsMousePosValid(&g.IO.MousePos))
        g.IO.MousePos = g.LastValidMousePos = ImFloor(g.IO.MousePos);

    // If mouse just appeared or disappeared (usually denoted by -FLT_MAX components) we cancel out movement in MouseDelta
    if (IsMousePosValid(&g.IO.MousePos) && IsMousePosValid(&g.IO.MousePosPrev))
        g.IO.MouseDelta = g.IO.MousePos - g.IO.MousePosPrev;
    else
        g.IO.MouseDelta = ImVec2(0.0f, 0.0f);
    if (g.IO.MouseDelta.x != 0.0f || g.IO.MouseDelta.y != 0.0f)
        g.NavDisableMouseHover = false;

    g.IO.MousePosPrev = g.IO.MousePos;
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++)
    {
        g.IO.MouseClicked[i] = g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] < 0.0f;
        g.IO.MouseReleased[i] = !g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] >= 0.0f;
        g.IO.MouseDownDurationPrev[i] = g.IO.MouseDownDuration[i];
        g.IO.MouseDownDuration[i] = g.IO.MouseDown[i] ? (g.IO.MouseDownDuration[i] < 0.0f ? 0.0f : g.IO.MouseDownDuration[i] + g.IO.DeltaTime) : -1.0f;
        g.IO.MouseDoubleClicked[i] = false;
        if (g.IO.MouseClicked[i])
        {
            if ((float)(g.Time - g.IO.MouseClickedTime[i]) < g.IO.MouseDoubleClickTime)
            {
                ImVec2 delta_from_click_pos = IsMousePosValid(&g.IO.MousePos) ? (g.IO.MousePos - g.IO.MouseClickedPos[i]) : ImVec2(0.0f, 0.0f);
                if (ImLengthSqr(delta_from_click_pos) < g.IO.MouseDoubleClickMaxDist * g.IO.MouseDoubleClickMaxDist)
                    g.IO.MouseDoubleClicked[i] = true;
                g.IO.MouseClickedTime[i] = -g.IO.MouseDoubleClickTime * 2.0f; // Mark as "old enough" so the third click isn't turned into a double-click
            } else
            {
                g.IO.MouseClickedTime[i] = g.Time;
            }
            g.IO.MouseClickedPos[i] = g.IO.MousePos;
            g.IO.MouseDownWasDoubleClick[i] = g.IO.MouseDoubleClicked[i];
            g.IO.MouseDragMaxDistanceAbs[i] = ImVec2(0.0f, 0.0f);
            g.IO.MouseDragMaxDistanceSqr[i] = 0.0f;
        } else if (g.IO.MouseDown[i])
        {
            // Maintain the maximum distance we reaching from the initial click position, which is used with dragging threshold
            ImVec2 delta_from_click_pos = IsMousePosValid(&g.IO.MousePos) ? (g.IO.MousePos - g.IO.MouseClickedPos[i]) : ImVec2(0.0f, 0.0f);
            g.IO.MouseDragMaxDistanceSqr[i] = ImMax(g.IO.MouseDragMaxDistanceSqr[i], ImLengthSqr(delta_from_click_pos));
            g.IO.MouseDragMaxDistanceAbs[i].x = ImMax(g.IO.MouseDragMaxDistanceAbs[i].x, delta_from_click_pos.x < 0.0f ? -delta_from_click_pos.x : delta_from_click_pos.x);
            g.IO.MouseDragMaxDistanceAbs[i].y = ImMax(g.IO.MouseDragMaxDistanceAbs[i].y, delta_from_click_pos.y < 0.0f ? -delta_from_click_pos.y : delta_from_click_pos.y);
        }
        if (!g.IO.MouseDown[i] && !g.IO.MouseReleased[i])
            g.IO.MouseDownWasDoubleClick[i] = false;
        if (g.IO.MouseClicked[i]) // Clicking any mouse button reactivate mouse hovering which may have been deactivated by gamepad/keyboard navigation
            g.NavDisableMouseHover = false;
    }
}

static void StartLockWheelingWindow(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    if (g.WheelingWindow == window)
        return;
    g.WheelingWindow = window;
    g.WheelingWindowRefMousePos = g.IO.MousePos;
    g.WheelingWindowTimer = WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER;
}

void ImGui::UpdateMouseWheel()
{
    ImGuiContext& g = *GImGui;

    // Reset the locked window if we move the mouse or after the timer elapses
    if (g.WheelingWindow != NULL)
    {
        g.WheelingWindowTimer -= g.IO.DeltaTime;
        if (IsMousePosValid() && ImLengthSqr(g.IO.MousePos - g.WheelingWindowRefMousePos) > g.IO.MouseDragThreshold * g.IO.MouseDragThreshold)
            g.WheelingWindowTimer = 0.0f;
        if (g.WheelingWindowTimer <= 0.0f)
        {
            g.WheelingWindow = NULL;
            g.WheelingWindowTimer = 0.0f;
        }
    }

    if (g.IO.MouseWheel == 0.0f && g.IO.MouseWheelH == 0.0f)
        return;

    ImGuiWindow* window = g.WheelingWindow ? g.WheelingWindow : g.HoveredWindow;
    if (!window || window->Collapsed)
        return;

    // Zoom / Scale window
    // FIXME-OBSOLETE: This is an old feature, it still works but pretty much nobody is using it and may be best redesigned.
    if (g.IO.MouseWheel != 0.0f && g.IO.KeyCtrl && g.IO.FontAllowUserScaling)
    {
        StartLockWheelingWindow(window);
        const float new_font_scale = ImClamp(window->FontWindowScale + g.IO.MouseWheel * 0.10f, 0.50f, 2.50f);
        const float scale = new_font_scale / window->FontWindowScale;
        window->FontWindowScale = new_font_scale;
        if (!(window->Flags & ImGuiWindowFlags_ChildWindow))
        {
            const ImVec2 offset = window->Size * (1.0f - scale) * (g.IO.MousePos - window->Pos) / window->Size;
            SetWindowPos(window, window->Pos + offset, 0);
            window->Size = ImFloor(window->Size * scale);
            window->SizeFull = ImFloor(window->SizeFull * scale);
        }
        return;
    }

    // Mouse wheel scrolling
    // If a child window has the ImGuiWindowFlags_NoScrollWithMouse flag, we give a chance to scroll its parent

    // Vertical Mouse Wheel scrolling
    const float wheel_y = (g.IO.MouseWheel != 0.0f && !g.IO.KeyShift) ? g.IO.MouseWheel : 0.0f;
    if (wheel_y != 0.0f && !g.IO.KeyCtrl)
    {
        StartLockWheelingWindow(window);
        while ((window->Flags & ImGuiWindowFlags_ChildWindow) && ((window->ScrollMax.y == 0.0f) || ((window->Flags & ImGuiWindowFlags_NoScrollWithMouse) && !(window->Flags & ImGuiWindowFlags_NoMouseInputs))))
            window = window->ParentWindow;
        if (!(window->Flags & ImGuiWindowFlags_NoScrollWithMouse) && !(window->Flags & ImGuiWindowFlags_NoMouseInputs))
        {
            float max_step = window->InnerRect.GetHeight() * 0.67f;
            float scroll_step = ImFloor(ImMin(5 * window->CalcFontSize(), max_step));
            SetScrollY(window, window->Scroll.y - wheel_y * scroll_step);
        }
    }

    // Horizontal Mouse Wheel scrolling, or Vertical Mouse Wheel w/ Shift held
    const float wheel_x = (g.IO.MouseWheelH != 0.0f && !g.IO.KeyShift) ? g.IO.MouseWheelH : (g.IO.MouseWheel != 0.0f && g.IO.KeyShift) ? g.IO.MouseWheel : 0.0f;
    if (wheel_x != 0.0f && !g.IO.KeyCtrl)
    {
        StartLockWheelingWindow(window);
        while ((window->Flags & ImGuiWindowFlags_ChildWindow) && ((window->ScrollMax.x == 0.0f) || ((window->Flags & ImGuiWindowFlags_NoScrollWithMouse) && !(window->Flags & ImGuiWindowFlags_NoMouseInputs))))
            window = window->ParentWindow;
        if (!(window->Flags & ImGuiWindowFlags_NoScrollWithMouse) && !(window->Flags & ImGuiWindowFlags_NoMouseInputs))
        {
            float max_step = window->InnerRect.GetWidth() * 0.67f;
            float scroll_step = ImFloor(ImMin(2 * window->CalcFontSize(), max_step));
            SetScrollX(window, window->Scroll.x - wheel_x * scroll_step);
        }
    }
}

void ImGui::UpdateTabFocus()
{
    ImGuiContext& g = *GImGui;

    // Pressing TAB activate widget focus
    g.FocusTabPressed = (g.NavWindow && g.NavWindow->Active && !(g.NavWindow->Flags & ImGuiWindowFlags_NoNavInputs) && !g.IO.KeyCtrl && IsKeyPressedMap(ImGuiKey_Tab));
    if (g.ActiveId == 0 && g.FocusTabPressed)
    {
        // Note that SetKeyboardFocusHere() sets the Next fields mid-frame. To be consistent we also
        // manipulate the Next fields even, even though they will be turned into Curr fields by the code below.
        g.FocusRequestNextWindow = g.NavWindow;
        g.FocusRequestNextCounterRegular = INT_MAX;
        if (g.NavId != 0 && g.NavIdTabCounter != INT_MAX)
            g.FocusRequestNextCounterTabStop = g.NavIdTabCounter + 1 + (g.IO.KeyShift ? -1 : 1);
        else
            g.FocusRequestNextCounterTabStop = g.IO.KeyShift ? -1 : 0;
    }

    // Turn queued focus request into current one
    g.FocusRequestCurrWindow = NULL;
    g.FocusRequestCurrCounterRegular = g.FocusRequestCurrCounterTabStop = INT_MAX;
    if (g.FocusRequestNextWindow != NULL)
    {
        ImGuiWindow* window = g.FocusRequestNextWindow;
        g.FocusRequestCurrWindow = window;
        if (g.FocusRequestNextCounterRegular != INT_MAX && window->DC.FocusCounterRegular != -1)
            g.FocusRequestCurrCounterRegular = ImModPositive(g.FocusRequestNextCounterRegular, window->DC.FocusCounterRegular + 1);
        if (g.FocusRequestNextCounterTabStop != INT_MAX && window->DC.FocusCounterTabStop != -1)
            g.FocusRequestCurrCounterTabStop = ImModPositive(g.FocusRequestNextCounterTabStop, window->DC.FocusCounterTabStop + 1);
        g.FocusRequestNextWindow = NULL;
        g.FocusRequestNextCounterRegular = g.FocusRequestNextCounterTabStop = INT_MAX;
    }

    g.NavIdTabCounter = INT_MAX;
}

// The reason this is exposed in imgui_internal.h is: on touch-based system that don't have hovering, we want to dispatch inputs to the right target (imgui vs imgui+app)
void ImGui::UpdateHoveredWindowAndCaptureFlags()
{
    ImGuiContext& g = *GImGui;

    // Find the window hovered by mouse:
    // - Child windows can extend beyond the limit of their parent so we need to derive HoveredRootWindow from HoveredWindow.
    // - When moving a window we can skip the search, which also conveniently bypasses the fact that window->WindowRectClipped is lagging as this point of the frame.
    // - We also support the moved window toggling the NoInputs flag after moving has started in order to be able to detect windows below it, which is useful for e.g. docking mechanisms.
    FindHoveredWindow();

    // Modal windows prevents cursor from hovering behind them.
    ImGuiWindow* modal_window = GetTopMostPopupModal();
    if (modal_window)
        if (g.HoveredRootWindow && !IsWindowChildOf(g.HoveredRootWindow, modal_window))
            g.HoveredRootWindow = g.HoveredWindow = NULL;

    // Disabled mouse?
    if (g.IO.ConfigFlags & ImGuiConfigFlags_NoMouse)
        g.HoveredWindow = g.HoveredRootWindow = NULL;

    // We track click ownership. When clicked outside of a window the click is owned by the application and won't report hovering nor request capture even while dragging over our windows afterward.
    int mouse_earliest_button_down = -1;
    bool mouse_any_down = false;
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++)
    {
        if (g.IO.MouseClicked[i])
            g.IO.MouseDownOwned[i] = (g.HoveredWindow != NULL) || (!g.OpenPopupStack.empty());
        mouse_any_down |= g.IO.MouseDown[i];
        if (g.IO.MouseDown[i])
            if (mouse_earliest_button_down == -1 || g.IO.MouseClickedTime[i] < g.IO.MouseClickedTime[mouse_earliest_button_down])
                mouse_earliest_button_down = i;
    }
    const bool mouse_avail_to_imgui = (mouse_earliest_button_down == -1) || g.IO.MouseDownOwned[mouse_earliest_button_down];

    // If mouse was first clicked outside of ImGui bounds we also cancel out hovering.
    // FIXME: For patterns of drag and drop across OS windows, we may need to rework/remove this test (first committed 311c0ca9 on 2015/02)
    const bool mouse_dragging_extern_payload = g.DragDropActive && (g.DragDropSourceFlags & ImGuiDragDropFlags_SourceExtern) != 0;
    if (!mouse_avail_to_imgui && !mouse_dragging_extern_payload)
        g.HoveredWindow = g.HoveredRootWindow = NULL;

    // Update io.WantCaptureMouse for the user application (true = dispatch mouse info to imgui, false = dispatch mouse info to Dear ImGui + app)
    if (g.WantCaptureMouseNextFrame != -1)
        g.IO.WantCaptureMouse = (g.WantCaptureMouseNextFrame != 0);
    else
        g.IO.WantCaptureMouse = (mouse_avail_to_imgui && (g.HoveredWindow != NULL || mouse_any_down)) || (!g.OpenPopupStack.empty());

    // Update io.WantCaptureKeyboard for the user application (true = dispatch keyboard info to imgui, false = dispatch keyboard info to Dear ImGui + app)
    if (g.WantCaptureKeyboardNextFrame != -1)
        g.IO.WantCaptureKeyboard = (g.WantCaptureKeyboardNextFrame != 0);
    else
        g.IO.WantCaptureKeyboard = (g.ActiveId != 0) || (modal_window != NULL);
    if (g.IO.NavActive && (g.IO.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard) && !(g.IO.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard))
        g.IO.WantCaptureKeyboard = true;

    // Update io.WantTextInput flag, this is to allow systems without a keyboard (e.g. mobile, hand-held) to show a software keyboard if possible
    g.IO.WantTextInput = (g.WantTextInputNextFrame != -1) ? (g.WantTextInputNextFrame != 0) : false;
}

ImGuiKeyModFlags ImGui::GetMergedKeyModFlags()
{
    ImGuiContext& g = *GImGui;
    ImGuiKeyModFlags key_mod_flags = ImGuiKeyModFlags_None;
    if (g.IO.KeyCtrl) { key_mod_flags |= ImGuiKeyModFlags_Ctrl; }
    if (g.IO.KeyShift) { key_mod_flags |= ImGuiKeyModFlags_Shift; }
    if (g.IO.KeyAlt) { key_mod_flags |= ImGuiKeyModFlags_Alt; }
    if (g.IO.KeySuper) { key_mod_flags |= ImGuiKeyModFlags_Super; }
    return key_mod_flags;
}

void ImGui::NewFrame()
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() and ImGui::SetCurrentContext() ?");
    ImGuiContext& g = *GImGui;

#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiTestEngineHook_PreNewFrame(&g);
#endif

    // Check and assert for various common IO and Configuration mistakes
    ErrorCheckNewFrameSanityChecks();

    // Load settings on first frame, save settings when modified (after a delay)
    UpdateSettings();

    g.Time += g.IO.DeltaTime;
    g.WithinFrameScope = true;
    g.FrameCount += 1;
    g.TooltipOverrideCount = 0;
    g.WindowsActiveCount = 0;
    g.MenusIdSubmittedThisFrame.resize(0);

    // Calculate frame-rate for the user, as a purely luxurious feature
    g.FramerateSecPerFrameAccum += g.IO.DeltaTime - g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx];
    g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx] = g.IO.DeltaTime;
    g.FramerateSecPerFrameIdx = (g.FramerateSecPerFrameIdx + 1) % IM_ARRAYSIZE(g.FramerateSecPerFrame);
    g.IO.Framerate = (g.FramerateSecPerFrameAccum > 0.0f) ? (1.0f / (g.FramerateSecPerFrameAccum / (float)IM_ARRAYSIZE(g.FramerateSecPerFrame))) : FLT_MAX;

    // Setup current font and draw list shared data
    g.IO.Fonts->Locked = true;
    SetCurrentFont(GetDefaultFont());
    IM_ASSERT(g.Font->IsLoaded());
    g.DrawListSharedData.ClipRectFullscreen = ImVec4(0.0f, 0.0f, g.IO.DisplaySize.x, g.IO.DisplaySize.y);
    g.DrawListSharedData.CurveTessellationTol = g.Style.CurveTessellationTol;
    g.DrawListSharedData.SetCircleSegmentMaxError(g.Style.CircleSegmentMaxError);
    g.DrawListSharedData.InitialFlags = ImDrawListFlags_None;
    if (g.Style.AntiAliasedLines)
        g.DrawListSharedData.InitialFlags |= ImDrawListFlags_AntiAliasedLines;
    if (g.Style.AntiAliasedFill)
        g.DrawListSharedData.InitialFlags |= ImDrawListFlags_AntiAliasedFill;
    if (g.IO.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset)
        g.DrawListSharedData.InitialFlags |= ImDrawListFlags_AllowVtxOffset;

    g.BackgroundDrawList._ResetForNewFrame();
    g.BackgroundDrawList.PushTextureID(g.IO.Fonts->TexID);
    g.BackgroundDrawList.PushClipRectFullScreen();

    g.ForegroundDrawList._ResetForNewFrame();
    g.ForegroundDrawList.PushTextureID(g.IO.Fonts->TexID);
    g.ForegroundDrawList.PushClipRectFullScreen();

    // Mark rendering data as invalid to prevent user who may have a handle on it to use it.
    g.DrawData.Clear();

    // Drag and drop keep the source ID alive so even if the source disappear our state is consistent
    if (g.DragDropActive && g.DragDropPayload.SourceId == g.ActiveId)
        KeepAliveID(g.DragDropPayload.SourceId);

    // Update HoveredId data
    if (!g.HoveredIdPreviousFrame)
        g.HoveredIdTimer = 0.0f;
    if (!g.HoveredIdPreviousFrame || (g.HoveredId && g.ActiveId == g.HoveredId))
        g.HoveredIdNotActiveTimer = 0.0f;
    if (g.HoveredId)
        g.HoveredIdTimer += g.IO.DeltaTime;
    if (g.HoveredId && g.ActiveId != g.HoveredId)
        g.HoveredIdNotActiveTimer += g.IO.DeltaTime;
    g.HoveredIdPreviousFrame = g.HoveredId;
    g.HoveredId = 0;
    g.HoveredIdAllowOverlap = false;

    // Update ActiveId data (clear reference to active widget if the widget isn't alive anymore)
    if (g.ActiveIdIsAlive != g.ActiveId && g.ActiveIdPreviousFrame == g.ActiveId && g.ActiveId != 0)
        ClearActiveID();
    if (g.ActiveId)
        g.ActiveIdTimer += g.IO.DeltaTime;
    g.LastActiveIdTimer += g.IO.DeltaTime;
    g.ActiveIdPreviousFrame = g.ActiveId;
    g.ActiveIdPreviousFrameWindow = g.ActiveIdWindow;
    g.ActiveIdPreviousFrameHasBeenEditedBefore = g.ActiveIdHasBeenEditedBefore;
    g.ActiveIdIsAlive = 0;
    g.ActiveIdHasBeenEditedThisFrame = false;
    g.ActiveIdPreviousFrameIsAlive = false;
    g.ActiveIdIsJustActivated = false;
    if (g.TempInputId != 0 && g.ActiveId != g.TempInputId)
        g.TempInputId = 0;
    if (g.ActiveId == 0)
    {
        g.ActiveIdUsingNavDirMask = 0x00;
        g.ActiveIdUsingNavInputMask = 0x00;
        g.ActiveIdUsingKeyInputMask = 0x00;
    }

    // Drag and drop
    g.DragDropAcceptIdPrev = g.DragDropAcceptIdCurr;
    g.DragDropAcceptIdCurr = 0;
    g.DragDropAcceptIdCurrRectSurface = FLT_MAX;
    g.DragDropWithinSource = false;
    g.DragDropWithinTarget = false;
    g.DragDropHoldJustPressedId = 0;

    // Update keyboard input state
    // Synchronize io.KeyMods with individual modifiers io.KeyXXX bools
    g.IO.KeyMods = GetMergedKeyModFlags();
    memcpy(g.IO.KeysDownDurationPrev, g.IO.KeysDownDuration, sizeof(g.IO.KeysDownDuration));
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.KeysDown); i++)
        g.IO.KeysDownDuration[i] = g.IO.KeysDown[i] ? (g.IO.KeysDownDuration[i] < 0.0f ? 0.0f : g.IO.KeysDownDuration[i] + g.IO.DeltaTime) : -1.0f;

    // Update gamepad/keyboard navigation
    NavUpdate();

    // Update mouse input state
    UpdateMouseInputs();

    // Find hovered window
    // (needs to be before UpdateMouseMovingWindowNewFrame so we fill g.HoveredWindowUnderMovingWindow on the mouse release frame)
    UpdateHoveredWindowAndCaptureFlags();

    // Handle user moving window with mouse (at the beginning of the frame to avoid input lag or sheering)
    UpdateMouseMovingWindowNewFrame();

    // Background darkening/whitening
    if (GetTopMostPopupModal() != NULL || (g.NavWindowingTarget != NULL && g.NavWindowingHighlightAlpha > 0.0f))
        g.DimBgRatio = ImMin(g.DimBgRatio + g.IO.DeltaTime * 6.0f, 1.0f);
    else
        g.DimBgRatio = ImMax(g.DimBgRatio - g.IO.DeltaTime * 10.0f, 0.0f);

    g.MouseCursor = ImGuiMouseCursor_Arrow;
    g.WantCaptureMouseNextFrame = g.WantCaptureKeyboardNextFrame = g.WantTextInputNextFrame = -1;
    g.PlatformImePos = ImVec2(1.0f, 1.0f); // OS Input Method Editor showing on top-left of our window by default

    // Mouse wheel scrolling, scale
    UpdateMouseWheel();

    // Update legacy TAB focus
    UpdateTabFocus();

    // Mark all windows as not visible and compact unused memory.
    IM_ASSERT(g.WindowsFocusOrder.Size == g.Windows.Size);
    const float memory_compact_start_time = (g.IO.ConfigWindowsMemoryCompactTimer >= 0.0f) ? (float)g.Time - g.IO.ConfigWindowsMemoryCompactTimer : FLT_MAX;
    for (int i = 0; i != g.Windows.Size; i++)
    {
        ImGuiWindow* window = g.Windows[i];
        window->WasActive = window->Active;
        window->BeginCount = 0;
        window->Active = false;
        window->WriteAccessed = false;

        // Garbage collect transient buffers of recently unused windows
        if (!window->WasActive && !window->MemoryCompacted && window->LastTimeActive < memory_compact_start_time)
            GcCompactTransientWindowBuffers(window);
    }

    // Closing the focused window restore focus to the first active root window in descending z-order
    if (g.NavWindow && !g.NavWindow->WasActive)
        FocusTopMostWindowUnderOne(NULL, NULL);

    // No window should be open at the beginning of the frame.
    // But in order to allow the user to call NewFrame() multiple times without calling Render(), we are doing an explicit clear.
    g.CurrentWindowStack.resize(0);
    g.BeginPopupStack.resize(0);
    ClosePopupsOverWindow(g.NavWindow, false);

    // [DEBUG] Item picker tool - start with DebugStartItemPicker() - useful to visually select an item and break into its call-stack.
    UpdateDebugToolItemPicker();

    // Create implicit/fallback window - which we will only render it if the user has added something to it.
    // We don't use "Debug" to avoid colliding with user trying to create a "Debug" window with custom flags.
    // This fallback is particularly important as it avoid ImGui:: calls from crashing.
    g.WithinFrameScopeWithImplicitWindow = true;
    SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
    Begin("Debug##Default");
    IM_ASSERT(g.CurrentWindow->IsFallbackWindow == true);

#ifdef IMGUI_ENABLE_TEST_ENGINE
    ImGuiTestEngineHook_PostNewFrame(&g);
#endif
}

// [DEBUG] Item picker tool - start with DebugStartItemPicker() - useful to visually select an item and break into its call-stack.
void ImGui::UpdateDebugToolItemPicker()
{
    ImGuiContext& g = *GImGui;
    g.DebugItemPickerBreakId = 0;
    if (g.DebugItemPickerActive)
    {
        const ImGuiID hovered_id = g.HoveredIdPreviousFrame;
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
            g.DebugItemPickerActive = false;
        if (ImGui::IsMouseClicked(0) && hovered_id)
        {
            g.DebugItemPickerBreakId = hovered_id;
            g.DebugItemPickerActive = false;
        }
        ImGui::SetNextWindowBgAlpha(0.60f);
        ImGui::BeginTooltip();
        ImGui::Text("HoveredId: 0x%08X", hovered_id);
        ImGui::Text("Press ESC to abort picking.");
        ImGui::TextColored(GetStyleColorVec4(hovered_id ? ImGuiCol_Text : ImGuiCol_TextDisabled), "Click to break in debugger!");
        ImGui::EndTooltip();
    }
}

void ImGui::Initialize(ImGuiContext* context)
{
    ImGuiContext& g = *context;
    IM_ASSERT(!g.Initialized && !g.SettingsLoaded);

    // Add .ini handle for ImGuiWindow type
    {
        ImGuiSettingsHandler ini_handler;
        ini_handler.TypeName = "Window";
        ini_handler.TypeHash = ImHashStr("Window");
        ini_handler.ClearAllFn = WindowSettingsHandler_ClearAll;
        ini_handler.ReadOpenFn = WindowSettingsHandler_ReadOpen;
        ini_handler.ReadLineFn = WindowSettingsHandler_ReadLine;
        ini_handler.ApplyAllFn = WindowSettingsHandler_ApplyAll;
        ini_handler.WriteAllFn = WindowSettingsHandler_WriteAll;
        g.SettingsHandlers.push_back(ini_handler);
    }

#ifdef IMGUI_HAS_TABLE
    // Add .ini handle for ImGuiTable type
    {
        ImGuiSettingsHandler ini_handler;
        ini_handler.TypeName = "Table";
        ini_handler.TypeHash = ImHashStr("Table");
        ini_handler.ReadOpenFn = TableSettingsHandler_ReadOpen;
        ini_handler.ReadLineFn = TableSettingsHandler_ReadLine;
        ini_handler.WriteAllFn = TableSettingsHandler_WriteAll;
        g.SettingsHandlers.push_back(ini_handler);
    }
#endif // #ifdef IMGUI_HAS_TABLE

#ifdef IMGUI_HAS_DOCK
#endif // #ifdef IMGUI_HAS_DOCK

    g.Initialized = true;
}

// This function is merely here to free heap allocations.
void ImGui::Shutdown(ImGuiContext* context)
{
    // The fonts atlas can be used prior to calling NewFrame(), so we clear it even if g.Initialized is FALSE (which would happen if we never called NewFrame)
    ImGuiContext& g = *context;
    if (g.IO.Fonts && g.FontAtlasOwnedByContext)
    {
        g.IO.Fonts->Locked = false;
        IM_DELETE(g.IO.Fonts);
    }
    g.IO.Fonts = NULL;

    // Cleanup of other data are conditional on actually having initialized Dear ImGui.
    if (!g.Initialized)
        return;

    // Save settings (unless we haven't attempted to load them: CreateContext/DestroyContext without a call to NewFrame shouldn't save an empty file)
    if (g.SettingsLoaded && g.IO.IniFilename != NULL)
    {
        ImGuiContext* backup_context = GImGui;
        SetCurrentContext(context);
        SaveIniSettingsToDisk(g.IO.IniFilename);
        SetCurrentContext(backup_context);
    }

    // Clear everything else
    for (int i = 0; i < g.Windows.Size; i++)
        IM_DELETE(g.Windows[i]);
    g.Windows.clear();
    g.WindowsFocusOrder.clear();
    g.WindowsTempSortBuffer.clear();
    g.CurrentWindow = NULL;
    g.CurrentWindowStack.clear();
    g.WindowsById.Clear();
    g.NavWindow = NULL;
    g.HoveredWindow = g.HoveredRootWindow = NULL;
    g.ActiveIdWindow = g.ActiveIdPreviousFrameWindow = NULL;
    g.MovingWindow = NULL;
    g.ColorModifiers.clear();
    g.StyleModifiers.clear();
    g.FontStack.clear();
    g.OpenPopupStack.clear();
    g.BeginPopupStack.clear();
    g.DrawDataBuilder.ClearFreeMemory();
    g.BackgroundDrawList._ClearFreeMemory();
    g.ForegroundDrawList._ClearFreeMemory();

    g.TabBars.Clear();
    g.CurrentTabBarStack.clear();
    g.ShrinkWidthBuffer.clear();

    g.ClipboardHandlerData.clear();
    g.MenusIdSubmittedThisFrame.clear();
    g.InputTextState.ClearFreeMemory();

    g.SettingsWindows.clear();
    g.SettingsHandlers.clear();

    if (g.LogFile)
    {
#ifndef IMGUI_DISABLE_TTY_FUNCTIONS
        if (g.LogFile != stdout)
#endif
            ImFileClose(g.LogFile);
        g.LogFile = NULL;
    }
    g.LogBuffer.clear();

    g.Initialized = false;
}

// FIXME: Add a more explicit sort order in the window structure.
static int IMGUI_CDECL ChildWindowComparer(const void* lhs, const void* rhs)
{
    const ImGuiWindow* const a = *(const ImGuiWindow* const*)lhs;
    const ImGuiWindow* const b = *(const ImGuiWindow* const*)rhs;
    if (int d = (a->Flags & ImGuiWindowFlags_Popup) - (b->Flags & ImGuiWindowFlags_Popup))
        return d;
    if (int d = (a->Flags & ImGuiWindowFlags_Tooltip) - (b->Flags & ImGuiWindowFlags_Tooltip))
        return d;
    return (a->BeginOrderWithinParent - b->BeginOrderWithinParent);
}

static void AddWindowToSortBuffer(ImVector<ImGuiWindow*>* out_sorted_windows, ImGuiWindow* window)
{
    out_sorted_windows->push_back(window);
    if (window->Active)
    {
        int count = window->DC.ChildWindows.Size;
        if (count > 1)
            ImQsort(window->DC.ChildWindows.Data, (size_t)count, sizeof(ImGuiWindow*), ChildWindowComparer);
        for (int i = 0; i < count; i++)
        {
            ImGuiWindow* child = window->DC.ChildWindows[i];
            if (child->Active)
                AddWindowToSortBuffer(out_sorted_windows, child);
        }
    }
}

static void AddDrawListToDrawData(ImVector<ImDrawList*>* out_list, ImDrawList* draw_list)
{
    // Remove trailing command if unused.
    // Technically we could return directly instead of popping, but this make things looks neat in Metrics window as well.
    draw_list->_PopUnusedDrawCmd();
    if (draw_list->CmdBuffer.Size == 0)
        return;

    // Draw list sanity check. Detect mismatch between PrimReserve() calls and incrementing _VtxCurrentIdx, _VtxWritePtr etc.
    // May trigger for you if you are using PrimXXX functions incorrectly.
    IM_ASSERT(draw_list->VtxBuffer.Size == 0 || draw_list->_VtxWritePtr == draw_list->VtxBuffer.Data + draw_list->VtxBuffer.Size);
    IM_ASSERT(draw_list->IdxBuffer.Size == 0 || draw_list->_IdxWritePtr == draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size);
    if (!(draw_list->Flags & ImDrawListFlags_AllowVtxOffset))
        IM_ASSERT((int)draw_list->_VtxCurrentIdx == draw_list->VtxBuffer.Size);

    // Check that draw_list doesn't use more vertices than indexable (default ImDrawIdx = unsigned short = 2 bytes = 64K vertices per ImDrawList = per window)
    // If this assert triggers because you are drawing lots of stuff manually:
    // - First, make sure you are coarse clipping yourself and not trying to draw many things outside visible bounds.
    //   Be mindful that the ImDrawList API doesn't filter vertices. Use the Metrics window to inspect draw list contents.
    // - If you want large meshes with more than 64K vertices, you can either:
    //   (A) Handle the ImDrawCmd::VtxOffset value in your renderer back-end, and set 'io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset'.
    //       Most example back-ends already support this from 1.71. Pre-1.71 back-ends won't.
    //       Some graphics API such as GL ES 1/2 don't have a way to offset the starting vertex so it is not supported for them.
    //   (B) Or handle 32-bit indices in your renderer back-end, and uncomment '#define ImDrawIdx unsigned int' line in imconfig.h.
    //       Most example back-ends already support this. For example, the OpenGL example code detect index size at compile-time:
    //         glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
    //       Your own engine or render API may use different parameters or function calls to specify index sizes.
    //       2 and 4 bytes indices are generally supported by most graphics API.
    // - If for some reason neither of those solutions works for you, a workaround is to call BeginChild()/EndChild() before reaching
    //   the 64K limit to split your draw commands in multiple draw lists.
    if (sizeof(ImDrawIdx) == 2)
        IM_ASSERT(draw_list->_VtxCurrentIdx < (1 << 16) && "Too many vertices in ImDrawList using 16-bit indices. Read comment above");

    out_list->push_back(draw_list);
}

static void AddWindowToDrawData(ImVector<ImDrawList*>* out_render_list, ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    g.IO.MetricsRenderWindows++;
    AddDrawListToDrawData(out_render_list, window->DrawList);
    for (int i = 0; i < window->DC.ChildWindows.Size; i++)
    {
        ImGuiWindow* child = window->DC.ChildWindows[i];
        if (IsWindowActiveAndVisible(child)) // clipped children may have been marked not active
            AddWindowToDrawData(out_render_list, child);
    }
}

// Layer is locked for the root window, however child windows may use a different viewport (e.g. extruding menu)
static void AddRootWindowToDrawData(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    int layer = (window->Flags & ImGuiWindowFlags_Tooltip) ? 1 : 0;
    AddWindowToDrawData(&g.DrawDataBuilder.Layers[layer], window);
}

void ImDrawDataBuilder::FlattenIntoSingleLayer()
{
    int n = Layers[0].Size;
    int size = n;
    for (int i = 1; i < IM_ARRAYSIZE(Layers); i++)
        size += Layers[i].Size;
    Layers[0].resize(size);
    for (int layer_n = 1; layer_n < IM_ARRAYSIZE(Layers); layer_n++)
    {
        ImVector<ImDrawList*>& layer = Layers[layer_n];
        if (layer.empty())
            continue;
        memcpy(&Layers[0][n], &layer[0], layer.Size * sizeof(ImDrawList*));
        n += layer.Size;
        layer.resize(0);
    }
}

static void SetupDrawData(ImVector<ImDrawList*>* draw_lists, ImDrawData* draw_data)
{
    ImGuiIO& io = ImGui::GetIO();
    draw_data->Valid = true;
    draw_data->CmdLists = (draw_lists->Size > 0) ? draw_lists->Data : NULL;
    draw_data->CmdListsCount = draw_lists->Size;
    draw_data->TotalVtxCount = draw_data->TotalIdxCount = 0;
    draw_data->DisplayPos = ImVec2(0.0f, 0.0f);
    draw_data->DisplaySize = io.DisplaySize;
    draw_data->FramebufferScale = io.DisplayFramebufferScale;
    for (int n = 0; n < draw_lists->Size; n++)
    {
        draw_data->TotalVtxCount += draw_lists->Data[n]->VtxBuffer.Size;
        draw_data->TotalIdxCount += draw_lists->Data[n]->IdxBuffer.Size;
    }
}

// When using this function it is sane to ensure that float are perfectly rounded to integer values, to that e.g. (int)(max.x-min.x) in user's render produce correct result.
void ImGui::PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DrawList->PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
    window->ClipRect = window->DrawList->_ClipRectStack.back();
}

void ImGui::PopClipRect()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DrawList->PopClipRect();
    window->ClipRect = window->DrawList->_ClipRectStack.back();
}

// This is normally called by Render(). You may want to call it directly if you want to avoid calling Render() but the gain will be very minimal.
void ImGui::EndFrame()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.Initialized);

    // Don't process EndFrame() multiple times.
    if (g.FrameCountEnded == g.FrameCount)
        return;
    IM_ASSERT(g.WithinFrameScope && "Forgot to call ImGui::NewFrame()?");

    ErrorCheckEndFrameSanityChecks();

    // Notify OS when our Input Method Editor cursor has moved (e.g. CJK inputs using Microsoft IME)
    if (g.IO.ImeSetInputScreenPosFn && (g.PlatformImeLastPos.x == FLT_MAX || ImLengthSqr(g.PlatformImeLastPos - g.PlatformImePos) > 0.0001f))
    {
        g.IO.ImeSetInputScreenPosFn((int)g.PlatformImePos.x, (int)g.PlatformImePos.y);
        g.PlatformImeLastPos = g.PlatformImePos;
    }

    // Hide implicit/fallback "Debug" window if it hasn't been used
    g.WithinFrameScopeWithImplicitWindow = false;
    if (g.CurrentWindow && !g.CurrentWindow->WriteAccessed)
        g.CurrentWindow->Active = false;
    End();

    // Update navigation: CTRL+Tab, wrap-around requests
    NavEndFrame();

    // Drag and Drop: Elapse payload (if delivered, or if source stops being submitted)
    if (g.DragDropActive)
    {
        bool is_delivered = g.DragDropPayload.Delivery;
        bool is_elapsed = (g.DragDropPayload.DataFrameCount + 1 < g.FrameCount) && ((g.DragDropSourceFlags & ImGuiDragDropFlags_SourceAutoExpirePayload) || !IsMouseDown(g.DragDropMouseButton));
        if (is_delivered || is_elapsed)
            ClearDragDrop();
    }

    // Drag and Drop: Fallback for source tooltip. This is not ideal but better than nothing.
    if (g.DragDropActive && g.DragDropSourceFrameCount < g.FrameCount && !(g.DragDropSourceFlags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
    {
        g.DragDropWithinSource = true;
        SetTooltip("...");
        g.DragDropWithinSource = false;
    }

    // End frame
    g.WithinFrameScope = false;
    g.FrameCountEnded = g.FrameCount;

    // Initiate moving window + handle left-click and right-click focus
    UpdateMouseMovingWindowEndFrame();

    // Sort the window list so that all child windows are after their parent
    // We cannot do that on FocusWindow() because children may not exist yet
    g.WindowsTempSortBuffer.resize(0);
    g.WindowsTempSortBuffer.reserve(g.Windows.Size);
    for (int i = 0; i != g.Windows.Size; i++)
    {
        ImGuiWindow* window = g.Windows[i];
        if (window->Active && (window->Flags & ImGuiWindowFlags_ChildWindow))       // if a child is active its parent will add it
            continue;
        AddWindowToSortBuffer(&g.WindowsTempSortBuffer, window);
    }

    // This usually assert if there is a mismatch between the ImGuiWindowFlags_ChildWindow / ParentWindow values and DC.ChildWindows[] in parents, aka we've done something wrong.
    IM_ASSERT(g.Windows.Size == g.WindowsTempSortBuffer.Size);
    g.Windows.swap(g.WindowsTempSortBuffer);
    g.IO.MetricsActiveWindows = g.WindowsActiveCount;

    // Unlock font atlas
    g.IO.Fonts->Locked = false;

    // Clear Input data for next frame
    g.IO.MouseWheel = g.IO.MouseWheelH = 0.0f;
    g.IO.InputQueueCharacters.resize(0);
    memset(g.IO.NavInputs, 0, sizeof(g.IO.NavInputs));
}

void ImGui::Render()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.Initialized);

    if (g.FrameCountEnded != g.FrameCount)
        EndFrame();
    g.FrameCountRendered = g.FrameCount;
    g.IO.MetricsRenderWindows = 0;
    g.DrawDataBuilder.Clear();

    // Add background ImDrawList
    if (!g.BackgroundDrawList.VtxBuffer.empty())
        AddDrawListToDrawData(&g.DrawDataBuilder.Layers[0], &g.BackgroundDrawList);

    // Add ImDrawList to render
    ImGuiWindow* windows_to_render_top_most[2];
    windows_to_render_top_most[0] = (g.NavWindowingTarget && !(g.NavWindowingTarget->Flags & ImGuiWindowFlags_NoBringToFrontOnFocus)) ? g.NavWindowingTarget->RootWindow : NULL;
    windows_to_render_top_most[1] = (g.NavWindowingTarget ? g.NavWindowingListWindow : NULL);
    for (int n = 0; n != g.Windows.Size; n++)
    {
        ImGuiWindow* window = g.Windows[n];
        if (IsWindowActiveAndVisible(window) && (window->Flags & ImGuiWindowFlags_ChildWindow) == 0 && window != windows_to_render_top_most[0] && window != windows_to_render_top_most[1])
            AddRootWindowToDrawData(window);
    }
    for (int n = 0; n < IM_ARRAYSIZE(windows_to_render_top_most); n++)
        if (windows_to_render_top_most[n] && IsWindowActiveAndVisible(windows_to_render_top_most[n])) // NavWindowingTarget is always temporarily displayed as the top-most window
            AddRootWindowToDrawData(windows_to_render_top_most[n]);
    g.DrawDataBuilder.FlattenIntoSingleLayer();

    // Draw software mouse cursor if requested
    if (g.IO.MouseDrawCursor)
        RenderMouseCursor(&g.ForegroundDrawList, g.IO.MousePos, g.Style.MouseCursorScale, g.MouseCursor, IM_COL32_WHITE, IM_COL32_BLACK, IM_COL32(0, 0, 0, 48));

    // Add foreground ImDrawList
    if (!g.ForegroundDrawList.VtxBuffer.empty())
        AddDrawListToDrawData(&g.DrawDataBuilder.Layers[0], &g.ForegroundDrawList);

    // Setup ImDrawData structure for end-user
    SetupDrawData(&g.DrawDataBuilder.Layers[0], &g.DrawData);
    g.IO.MetricsRenderVertices = g.DrawData.TotalVtxCount;
    g.IO.MetricsRenderIndices = g.DrawData.TotalIdxCount;

    // (Legacy) Call the Render callback function. The current prefer way is to let the user retrieve GetDrawData() and call the render function themselves.
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    if (g.DrawData.CmdListsCount > 0 && g.IO.RenderDrawListsFn != NULL)
        g.IO.RenderDrawListsFn(&g.DrawData);
#endif
}

// Calculate text size. Text can be multi-line. Optionally ignore text after a ## marker.
// CalcTextSize("") should return ImVec2(0.0f, g.FontSize)
ImVec2 ImGui::CalcTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)
{
    ImGuiContext& g = *GImGui;

    const char* text_display_end;
    if (hide_text_after_double_hash)
        text_display_end = FindRenderedTextEnd(text, text_end);      // Hide anything after a '##' string
    else
        text_display_end = text_end;

    ImFont* font = g.Font;
    const float font_size = g.FontSize;
    if (text == text_display_end)
        return ImVec2(0.0f, font_size);
    ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text, text_display_end, NULL);

    // Round
    text_size.x = IM_FLOOR(text_size.x + 0.95f);

    return text_size;
}

// Find window given position, search front-to-back
// FIXME: Note that we have an inconsequential lag here: OuterRectClipped is updated in Begin(), so windows moved programmatically
// with SetWindowPos() and not SetNextWindowPos() will have that rectangle lagging by a frame at the time FindHoveredWindow() is
// called, aka before the next Begin(). Moving window isn't affected.
static void FindHoveredWindow()
{
    ImGuiContext& g = *GImGui;

    ImGuiWindow* hovered_window = NULL;
    if (g.MovingWindow && !(g.MovingWindow->Flags & ImGuiWindowFlags_NoMouseInputs))
        hovered_window = g.MovingWindow;

    ImVec2 padding_regular = g.Style.TouchExtraPadding;
    ImVec2 padding_for_resize_from_edges = g.IO.ConfigWindowsResizeFromEdges ? ImMax(g.Style.TouchExtraPadding, ImVec2(WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS, WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS)) : padding_regular;
    for (int i = g.Windows.Size - 1; i >= 0; i--)
    {
        ImGuiWindow* window = g.Windows[i];
        if (!window->Active || window->Hidden)
            continue;
        if (window->Flags & ImGuiWindowFlags_NoMouseInputs)
            continue;

        // Using the clipped AABB, a child window will typically be clipped by its parent (not always)
        ImRect bb(window->OuterRectClipped);
        if (window->Flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
            bb.Expand(padding_regular);
        else
            bb.Expand(padding_for_resize_from_edges);
        if (!bb.Contains(g.IO.MousePos))
            continue;

        // Those seemingly unnecessary extra tests are because the code here is a little different in viewport/docking branches.
        if (hovered_window == NULL)
            hovered_window = window;
        if (hovered_window)
            break;
    }

    g.HoveredWindow = hovered_window;
    g.HoveredRootWindow = g.HoveredWindow ? g.HoveredWindow->RootWindow : NULL;

}

// Test if mouse cursor is hovering given rectangle
// NB- Rectangle is clipped by our current clip setting
// NB- Expand the rectangle to be generous on imprecise inputs systems (g.Style.TouchExtraPadding)
bool ImGui::IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip)
{
    ImGuiContext& g = *GImGui;

    // Clip
    ImRect rect_clipped(r_min, r_max);
    if (clip)
        rect_clipped.ClipWith(g.CurrentWindow->ClipRect);

    // Expand for touch input
    const ImRect rect_for_touch(rect_clipped.Min - g.Style.TouchExtraPadding, rect_clipped.Max + g.Style.TouchExtraPadding);
    if (!rect_for_touch.Contains(g.IO.MousePos))
        return false;
    return true;
}

int ImGui::GetKeyIndex(ImGuiKey imgui_key)
{
    IM_ASSERT(imgui_key >= 0 && imgui_key < ImGuiKey_COUNT);
    ImGuiContext& g = *GImGui;
    return g.IO.KeyMap[imgui_key];
}

// Note that dear imgui doesn't know the semantic of each entry of io.KeysDown[]!
// Use your own indices/enums according to how your back-end/engine stored them into io.KeysDown[]!
bool ImGui::IsKeyDown(int user_key_index)
{
    if (user_key_index < 0)
        return false;
    ImGuiContext& g = *GImGui;
    IM_ASSERT(user_key_index >= 0 && user_key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    return g.IO.KeysDown[user_key_index];
}

// t0 = previous time (e.g.: g.Time - g.IO.DeltaTime)
// t1 = current time (e.g.: g.Time)
// An event is triggered at:
//  t = 0.0f     t = repeat_delay,    t = repeat_delay + repeat_rate*N
int ImGui::CalcTypematicRepeatAmount(float t0, float t1, float repeat_delay, float repeat_rate)
{
    if (t1 == 0.0f)
        return 1;
    if (t0 >= t1)
        return 0;
    if (repeat_rate <= 0.0f)
        return (t0 < repeat_delay) && (t1 >= repeat_delay);
    const int count_t0 = (t0 < repeat_delay) ? -1 : (int)((t0 - repeat_delay) / repeat_rate);
    const int count_t1 = (t1 < repeat_delay) ? -1 : (int)((t1 - repeat_delay) / repeat_rate);
    const int count = count_t1 - count_t0;
    return count;
}

int ImGui::GetKeyPressedAmount(int key_index, float repeat_delay, float repeat_rate)
{
    ImGuiContext& g = *GImGui;
    if (key_index < 0)
        return 0;
    IM_ASSERT(key_index >= 0 && key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    const float t = g.IO.KeysDownDuration[key_index];
    return CalcTypematicRepeatAmount(t - g.IO.DeltaTime, t, repeat_delay, repeat_rate);
}

bool ImGui::IsKeyPressed(int user_key_index, bool repeat)
{
    ImGuiContext& g = *GImGui;
    if (user_key_index < 0)
        return false;
    IM_ASSERT(user_key_index >= 0 && user_key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    const float t = g.IO.KeysDownDuration[user_key_index];
    if (t == 0.0f)
        return true;
    if (repeat && t > g.IO.KeyRepeatDelay)
        return GetKeyPressedAmount(user_key_index, g.IO.KeyRepeatDelay, g.IO.KeyRepeatRate) > 0;
    return false;
}

bool ImGui::IsKeyReleased(int user_key_index)
{
    ImGuiContext& g = *GImGui;
    if (user_key_index < 0) return false;
    IM_ASSERT(user_key_index >= 0 && user_key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    return g.IO.KeysDownDurationPrev[user_key_index] >= 0.0f && !g.IO.KeysDown[user_key_index];
}

bool ImGui::IsMouseDown(ImGuiMouseButton button)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseDown[button];
}

bool ImGui::IsMouseClicked(ImGuiMouseButton button, bool repeat)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    const float t = g.IO.MouseDownDuration[button];
    if (t == 0.0f)
        return true;

    if (repeat && t > g.IO.KeyRepeatDelay)
    {
        // FIXME: 2019/05/03: Our old repeat code was wrong here and led to doubling the repeat rate, which made it an ok rate for repeat on mouse hold.
        int amount = CalcTypematicRepeatAmount(t - g.IO.DeltaTime, t, g.IO.KeyRepeatDelay, g.IO.KeyRepeatRate * 0.50f);
        if (amount > 0)
            return true;
    }
    return false;
}

bool ImGui::IsMouseReleased(ImGuiMouseButton button)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseReleased[button];
}

bool ImGui::IsMouseDoubleClicked(ImGuiMouseButton button)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseDoubleClicked[button];
}

// [Internal] This doesn't test if the button is pressed
bool ImGui::IsMouseDragPastThreshold(ImGuiMouseButton button, float lock_threshold)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    if (lock_threshold < 0.0f)
        lock_threshold = g.IO.MouseDragThreshold;
    return g.IO.MouseDragMaxDistanceSqr[button] >= lock_threshold * lock_threshold;
}

bool ImGui::IsMouseDragging(ImGuiMouseButton button, float lock_threshold)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    if (!g.IO.MouseDown[button])
        return false;
    return IsMouseDragPastThreshold(button, lock_threshold);
}

ImVec2 ImGui::GetMousePos()
{
    ImGuiContext& g = *GImGui;
    return g.IO.MousePos;
}

// NB: prefer to call right after BeginPopup(). At the time Selectable/MenuItem is activated, the popup is already closed!
ImVec2 ImGui::GetMousePosOnOpeningCurrentPopup()
{
    ImGuiContext& g = *GImGui;
    if (g.BeginPopupStack.Size > 0)
        return g.OpenPopupStack[g.BeginPopupStack.Size - 1].OpenMousePos;
    return g.IO.MousePos;
}

// We typically use ImVec2(-FLT_MAX,-FLT_MAX) to denote an invalid mouse position.
bool ImGui::IsMousePosValid(const ImVec2* mouse_pos)
{
    // The assert is only to silence a false-positive in XCode Static Analysis.
    // Because GImGui is not dereferenced in every code path, the static analyzer assume that it may be NULL (which it doesn't for other functions).
    IM_ASSERT(GImGui != NULL);
    const float MOUSE_INVALID = -256000.0f;
    ImVec2 p = mouse_pos ? *mouse_pos : GImGui->IO.MousePos;
    return p.x >= MOUSE_INVALID && p.y >= MOUSE_INVALID;
}

bool ImGui::IsAnyMouseDown()
{
    ImGuiContext& g = *GImGui;
    for (int n = 0; n < IM_ARRAYSIZE(g.IO.MouseDown); n++)
        if (g.IO.MouseDown[n])
            return true;
    return false;
}

// Return the delta from the initial clicking position while the mouse button is clicked or was just released.
// This is locked and return 0.0f until the mouse moves past a distance threshold at least once.
// NB: This is only valid if IsMousePosValid(). Back-ends in theory should always keep mouse position valid when dragging even outside the client window.
ImVec2 ImGui::GetMouseDragDelta(ImGuiMouseButton button, float lock_threshold)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    if (lock_threshold < 0.0f)
        lock_threshold = g.IO.MouseDragThreshold;
    if (g.IO.MouseDown[button] || g.IO.MouseReleased[button])
        if (g.IO.MouseDragMaxDistanceSqr[button] >= lock_threshold * lock_threshold)
            if (IsMousePosValid(&g.IO.MousePos) && IsMousePosValid(&g.IO.MouseClickedPos[button]))
                return g.IO.MousePos - g.IO.MouseClickedPos[button];
    return ImVec2(0.0f, 0.0f);
}

void ImGui::ResetMouseDragDelta(ImGuiMouseButton button)
{
    ImGuiContext& g = *GImGui;
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

void ImGui::CaptureKeyboardFromApp(bool capture)
{
    GImGui->WantCaptureKeyboardNextFrame = capture ? 1 : 0;
}

void ImGui::CaptureMouseFromApp(bool capture)
{
    GImGui->WantCaptureMouseNextFrame = capture ? 1 : 0;
}

bool ImGui::IsItemActive()
{
    ImGuiContext& g = *GImGui;
    if (g.ActiveId)
    {
        ImGuiWindow* window = g.CurrentWindow;
        return g.ActiveId == window->DC.LastItemId;
    }
    return false;
}

bool ImGui::IsItemActivated()
{
    ImGuiContext& g = *GImGui;
    if (g.ActiveId)
    {
        ImGuiWindow* window = g.CurrentWindow;
        if (g.ActiveId == window->DC.LastItemId && g.ActiveIdPreviousFrame != window->DC.LastItemId)
            return true;
    }
    return false;
}

bool ImGui::IsItemDeactivated()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HasDeactivated)
        return (window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_Deactivated) != 0;
    return (g.ActiveIdPreviousFrame == window->DC.LastItemId && g.ActiveIdPreviousFrame != 0 && g.ActiveId != window->DC.LastItemId);
}

bool ImGui::IsItemDeactivatedAfterEdit()
{
    ImGuiContext& g = *GImGui;
    return IsItemDeactivated() && (g.ActiveIdPreviousFrameHasBeenEditedBefore || (g.ActiveId == 0 && g.ActiveIdHasBeenEditedBefore));
}

bool ImGui::IsItemFocused()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    if (g.NavId == 0 || g.NavDisableHighlight || g.NavId != window->DC.LastItemId)
        return false;
    return true;
}

bool ImGui::IsItemClicked(ImGuiMouseButton mouse_button)
{
    return IsMouseClicked(mouse_button) && IsItemHovered(ImGuiHoveredFlags_None);
}

bool ImGui::IsItemToggledOpen()
{
    ImGuiContext& g = *GImGui;
    return (g.CurrentWindow->DC.LastItemStatusFlags & ImGuiItemStatusFlags_ToggledOpen) ? true : false;
}

bool ImGui::IsItemToggledSelection()
{
    ImGuiContext& g = *GImGui;
    return (g.CurrentWindow->DC.LastItemStatusFlags & ImGuiItemStatusFlags_ToggledSelection) ? true : false;
}

bool ImGui::IsAnyItemHovered()
{
    ImGuiContext& g = *GImGui;
    return g.HoveredId != 0 || g.HoveredIdPreviousFrame != 0;
}

bool ImGui::IsAnyItemActive()
{
    ImGuiContext& g = *GImGui;
    return g.ActiveId != 0;
}

bool ImGui::IsAnyItemFocused()
{
    ImGuiContext& g = *GImGui;
    return g.NavId != 0 && !g.NavDisableHighlight;
}

bool ImGui::IsItemVisible()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->ClipRect.Overlaps(window->DC.LastItemRect);
}

bool ImGui::IsItemEdited()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return (window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_Edited) != 0;
}

// Allow last item to be overlapped by a subsequent item. Both may be activated during the same frame before the later one takes priority.
void ImGui::SetItemAllowOverlap()
{
    ImGuiContext& g = *GImGui;
    if (g.HoveredId == g.CurrentWindow->DC.LastItemId)
        g.HoveredIdAllowOverlap = true;
    if (g.ActiveId == g.CurrentWindow->DC.LastItemId)
        g.ActiveIdAllowOverlap = true;
}

ImVec2 ImGui::GetItemRectMin()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.LastItemRect.Min;
}

ImVec2 ImGui::GetItemRectMax()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.LastItemRect.Max;
}

ImVec2 ImGui::GetItemRectSize()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.LastItemRect.GetSize();
}

static ImRect GetViewportRect()
{
    ImGuiContext& g = *GImGui;
    return ImRect(0.0f, 0.0f, g.IO.DisplaySize.x, g.IO.DisplaySize.y);
}

bool ImGui::BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;

    flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
    flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

    // Size
    const ImVec2 content_avail = GetContentRegionAvail();
    ImVec2 size = ImFloor(size_arg);
    const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
    if (size.x <= 0.0f)
        size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
    if (size.y <= 0.0f)
        size.y = ImMax(content_avail.y + size.y, 4.0f);
    SetNextWindowSize(size);

    // Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
    char title[256];
    if (name)
        ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
    else
        ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

    const float backup_border_size = g.Style.ChildBorderSize;
    if (!border)
        g.Style.ChildBorderSize = 0.0f;
    bool ret = Begin(title, NULL, flags);
    g.Style.ChildBorderSize = backup_border_size;

    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;
    child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

    // Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
    // While this is not really documented/defined, it seems that the expected thing to do.
    if (child_window->BeginCount == 1)
        parent_window->DC.CursorPos = child_window->Pos;

    // Process navigation-in immediately so NavInit can run on first frame
    if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayerActiveMask != 0 || child_window->DC.NavHasScroll))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(id + 1, child_window); // Steal ActiveId with a dummy id so that key-press won't activate child item
        g.ActiveIdSource = ImGuiInputSource_Nav;
    }
    return ret;
}

bool ImGui::BeginChild(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    return BeginChildEx(str_id, window->GetID(str_id), size_arg, border, extra_flags);
}

bool ImGui::BeginChild(ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
    IM_ASSERT(id != 0);
    return BeginChildEx(NULL, id, size_arg, border, extra_flags);
}

void ImGui::EndChild()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    IM_ASSERT(g.WithinEndChild == false);
    IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() calls

    g.WithinEndChild = true;
    if (window->BeginCount > 1)
    {
        End();
    } else
    {
        ImVec2 sz = window->Size;
        if (window->AutoFitChildAxises & (1 << ImGuiAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
            sz.x = ImMax(4.0f, sz.x);
        if (window->AutoFitChildAxises & (1 << ImGuiAxis_Y))
            sz.y = ImMax(4.0f, sz.y);
        End();

        ImGuiWindow* parent_window = g.CurrentWindow;
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
        ItemSize(sz);
        if ((window->DC.NavLayerActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & ImGuiWindowFlags_NavFlattened))
        {
            ItemAdd(bb, window->ChildId);
            RenderNavHighlight(bb, window->ChildId);

            // When browsing a window that has no activable items (scroll only) we keep a highlight on the child
            if (window->DC.NavLayerActiveMask == 0 && window == g.NavWindow)
                RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_TypeThin);
        } else
        {
            // Not navigable into
            ItemAdd(bb, 0);
        }
    }
    g.WithinEndChild = false;
}

// Helper to create a child window / scrolling region that looks like a normal widget frame.
bool ImGui::BeginChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags extra_flags)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_FrameBg]);
    PushStyleVar(ImGuiStyleVar_ChildRounding, style.FrameRounding);
    PushStyleVar(ImGuiStyleVar_ChildBorderSize, style.FrameBorderSize);
    PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);
    bool ret = BeginChild(id, size, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | extra_flags);
    PopStyleVar(3);
    PopStyleColor();
    return ret;
}

void ImGui::EndChildFrame()
{
    EndChild();
}

static void SetWindowConditionAllowFlags(ImGuiWindow* window, ImGuiCond flags, bool enabled)
{
    window->SetWindowPosAllowFlags = enabled ? (window->SetWindowPosAllowFlags | flags) : (window->SetWindowPosAllowFlags & ~flags);
    window->SetWindowSizeAllowFlags = enabled ? (window->SetWindowSizeAllowFlags | flags) : (window->SetWindowSizeAllowFlags & ~flags);
    window->SetWindowCollapsedAllowFlags = enabled ? (window->SetWindowCollapsedAllowFlags | flags) : (window->SetWindowCollapsedAllowFlags & ~flags);
}

ImGuiWindow* ImGui::FindWindowByID(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    return (ImGuiWindow*)g.WindowsById.GetVoidPtr(id);
}

ImGuiWindow* ImGui::FindWindowByName(const char* name)
{
    ImGuiID id = ImHashStr(name);
    return FindWindowByID(id);
}

static void ApplyWindowSettings(ImGuiWindow* window, ImGuiWindowSettings* settings)
{
    window->Pos = ImFloor(ImVec2(settings->Pos.x, settings->Pos.y));
    if (settings->Size.x > 0 && settings->Size.y > 0)
        window->Size = window->SizeFull = ImFloor(ImVec2(settings->Size.x, settings->Size.y));
    window->Collapsed = settings->Collapsed;
}

static ImGuiWindow* CreateNewWindow(const char* name, ImGuiWindowFlags flags)
{
    ImGuiContext& g = *GImGui;
    //IMGUI_DEBUG_LOG("CreateNewWindow '%s', flags = 0x%08X\n", name, flags);

    // Create window the first time
    ImGuiWindow* window = IM_NEW(ImGuiWindow)(&g, name);
    window->Flags = flags;
    g.WindowsById.SetVoidPtr(window->ID, window);

    // Default/arbitrary window position. Use SetNextWindowPos() with the appropriate condition flag to change the initial position of a window.
    window->Pos = ImVec2(60, 60);

    // User can disable loading and saving of settings. Tooltip and child windows also don't store settings.
    if (!(flags & ImGuiWindowFlags_NoSavedSettings))
        if (ImGuiWindowSettings* settings = ImGui::FindWindowSettings(window->ID))
        {
            // Retrieve settings from .ini file
            window->SettingsOffset = g.SettingsWindows.offset_from_ptr(settings);
            SetWindowConditionAllowFlags(window, ImGuiCond_FirstUseEver, false);
            ApplyWindowSettings(window, settings);
        }
    window->DC.CursorStartPos = window->DC.CursorMaxPos = window->Pos; // So first call to CalcContentSize() doesn't return crazy values

    if ((flags & ImGuiWindowFlags_AlwaysAutoResize) != 0)
    {
        window->AutoFitFramesX = window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = false;
    } else
    {
        if (window->Size.x <= 0.0f)
            window->AutoFitFramesX = 2;
        if (window->Size.y <= 0.0f)
            window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = (window->AutoFitFramesX > 0) || (window->AutoFitFramesY > 0);
    }

    g.WindowsFocusOrder.push_back(window);
    if (flags & ImGuiWindowFlags_NoBringToFrontOnFocus)
        g.Windows.push_front(window); // Quite slow but rare and only once
    else
        g.Windows.push_back(window);
    return window;
}

static ImVec2 CalcWindowSizeAfterConstraint(ImGuiWindow* window, ImVec2 new_size)
{
    ImGuiContext& g = *GImGui;
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
    {
        // Using -1,-1 on either X/Y axis to preserve the current size.
        ImRect cr = g.NextWindowData.SizeConstraintRect;
        new_size.x = (cr.Min.x >= 0 && cr.Max.x >= 0) ? ImClamp(new_size.x, cr.Min.x, cr.Max.x) : window->SizeFull.x;
        new_size.y = (cr.Min.y >= 0 && cr.Max.y >= 0) ? ImClamp(new_size.y, cr.Min.y, cr.Max.y) : window->SizeFull.y;
        if (g.NextWindowData.SizeCallback)
        {
            ImGuiSizeCallbackData data;
            data.UserData = g.NextWindowData.SizeCallbackUserData;
            data.Pos = window->Pos;
            data.CurrentSize = window->SizeFull;
            data.DesiredSize = new_size;
            g.NextWindowData.SizeCallback(&data);
            new_size = data.DesiredSize;
        }
        new_size.x = IM_FLOOR(new_size.x);
        new_size.y = IM_FLOOR(new_size.y);
    }

    // Minimum size
    if (!(window->Flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_AlwaysAutoResize)))
    {
        ImGuiWindow* window_for_height = window;
        new_size = ImMax(new_size, g.Style.WindowMinSize);
        new_size.y = ImMax(new_size.y, window_for_height->TitleBarHeight() + window_for_height->MenuBarHeight() + ImMax(0.0f, g.Style.WindowRounding - 1.0f)); // Reduce artifacts with very small windows
    }
    return new_size;
}

static ImVec2 CalcWindowContentSize(ImGuiWindow* window)
{
    if (window->Collapsed)
        if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
            return window->ContentSize;
    if (window->Hidden && window->HiddenFramesCannotSkipItems == 0 && window->HiddenFramesCanSkipItems > 0)
        return window->ContentSize;

    ImVec2 sz;
    sz.x = IM_FLOOR((window->ContentSizeExplicit.x != 0.0f) ? window->ContentSizeExplicit.x : window->DC.CursorMaxPos.x - window->DC.CursorStartPos.x);
    sz.y = IM_FLOOR((window->ContentSizeExplicit.y != 0.0f) ? window->ContentSizeExplicit.y : window->DC.CursorMaxPos.y - window->DC.CursorStartPos.y);
    return sz;
}

static ImVec2 CalcWindowAutoFitSize(ImGuiWindow* window, const ImVec2& size_contents)
{
    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    ImVec2 size_decorations = ImVec2(0.0f, window->TitleBarHeight() + window->MenuBarHeight());
    ImVec2 size_pad = window->WindowPadding * 2.0f;
    ImVec2 size_desired = size_contents + size_pad + size_decorations;
    if (window->Flags & ImGuiWindowFlags_Tooltip)
    {
        // Tooltip always resize
        return size_desired;
    } else
    {
        // Maximum window size is determined by the viewport size or monitor size
        const bool is_popup = (window->Flags & ImGuiWindowFlags_Popup) != 0;
        const bool is_menu = (window->Flags & ImGuiWindowFlags_ChildMenu) != 0;
        ImVec2 size_min = style.WindowMinSize;
        if (is_popup || is_menu) // Popups and menus bypass style.WindowMinSize by default, but we give then a non-zero minimum size to facilitate understanding problematic cases (e.g. empty popups)
            size_min = ImMin(size_min, ImVec2(4.0f, 4.0f));
        ImVec2 size_auto_fit = ImClamp(size_desired, size_min, ImMax(size_min, g.IO.DisplaySize - style.DisplaySafeAreaPadding * 2.0f));

        // When the window cannot fit all contents (either because of constraints, either because screen is too small),
        // we are growing the size on the other axis to compensate for expected scrollbar. FIXME: Might turn bigger than ViewportSize-WindowPadding.
        ImVec2 size_auto_fit_after_constraint = CalcWindowSizeAfterConstraint(window, size_auto_fit);
        bool will_have_scrollbar_x = (size_auto_fit_after_constraint.x - size_pad.x - size_decorations.x < size_contents.x && !(window->Flags& ImGuiWindowFlags_NoScrollbar) && (window->Flags& ImGuiWindowFlags_HorizontalScrollbar)) || (window->Flags & ImGuiWindowFlags_AlwaysHorizontalScrollbar);
        bool will_have_scrollbar_y = (size_auto_fit_after_constraint.y - size_pad.y - size_decorations.y < size_contents.y && !(window->Flags& ImGuiWindowFlags_NoScrollbar)) || (window->Flags & ImGuiWindowFlags_AlwaysVerticalScrollbar);
        if (will_have_scrollbar_x)
            size_auto_fit.y += style.ScrollbarSize;
        if (will_have_scrollbar_y)
            size_auto_fit.x += style.ScrollbarSize;
        return size_auto_fit;
    }
}

ImVec2 ImGui::CalcWindowExpectedSize(ImGuiWindow* window)
{
    ImVec2 size_contents = CalcWindowContentSize(window);
    ImVec2 size_auto_fit = CalcWindowAutoFitSize(window, size_contents);
    ImVec2 size_final = CalcWindowSizeAfterConstraint(window, size_auto_fit);
    return size_final;
}

static ImGuiCol GetWindowBgColorIdxFromFlags(ImGuiWindowFlags flags)
{
    if (flags & (ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_Popup))
        return ImGuiCol_PopupBg;
    if (flags & ImGuiWindowFlags_ChildWindow)
        return ImGuiCol_ChildBg;
    return ImGuiCol_WindowBg;
}

static void CalcResizePosSizeFromAnyCorner(ImGuiWindow* window, const ImVec2& corner_target, const ImVec2& corner_norm, ImVec2* out_pos, ImVec2* out_size)
{
    ImVec2 pos_min = ImLerp(corner_target, window->Pos, corner_norm);                // Expected window upper-left
    ImVec2 pos_max = ImLerp(window->Pos + window->Size, corner_target, corner_norm); // Expected window lower-right
    ImVec2 size_expected = pos_max - pos_min;
    ImVec2 size_constrained = CalcWindowSizeAfterConstraint(window, size_expected);
    *out_pos = pos_min;
    if (corner_norm.x == 0.0f)
        out_pos->x -= (size_constrained.x - size_expected.x);
    if (corner_norm.y == 0.0f)
        out_pos->y -= (size_constrained.y - size_expected.y);
    *out_size = size_constrained;
}

struct ImGuiResizeGripDef
{
    ImVec2  CornerPosN;
    ImVec2  InnerDir;
    int     AngleMin12, AngleMax12;
};

static const ImGuiResizeGripDef resize_grip_def[4] =
{
    { ImVec2(1,1), ImVec2(-1,-1), 0, 3 }, // Lower-right
    { ImVec2(0,1), ImVec2(+1,-1), 3, 6 }, // Lower-left
    { ImVec2(0,0), ImVec2(+1,+1), 6, 9 }, // Upper-left (Unused)
    { ImVec2(1,0), ImVec2(-1,+1), 9,12 }, // Upper-right (Unused)
};

struct ImGuiResizeBorderDef
{
    ImVec2 InnerDir;
    ImVec2 CornerPosN1, CornerPosN2;
    float  OuterAngle;
};

static const ImGuiResizeBorderDef resize_border_def[4] =
{
    { ImVec2(0,+1), ImVec2(0,0), ImVec2(1,0), IM_PI * 1.50f }, // Top
    { ImVec2(-1,0), ImVec2(1,0), ImVec2(1,1), IM_PI * 0.00f }, // Right
    { ImVec2(0,-1), ImVec2(1,1), ImVec2(0,1), IM_PI * 0.50f }, // Bottom
    { ImVec2(+1,0), ImVec2(0,1), ImVec2(0,0), IM_PI * 1.00f }  // Left
};

static ImRect GetResizeBorderRect(ImGuiWindow* window, int border_n, float perp_padding, float thickness)
{
    ImRect rect = window->Rect();
    if (thickness == 0.0f) rect.Max -= ImVec2(1, 1);
    if (border_n == 0) { return ImRect(rect.Min.x + perp_padding, rect.Min.y - thickness, rect.Max.x - perp_padding, rect.Min.y + thickness); } // Top
    if (border_n == 1) { return ImRect(rect.Max.x - thickness, rect.Min.y + perp_padding, rect.Max.x + thickness, rect.Max.y - perp_padding); } // Right
    if (border_n == 2) { return ImRect(rect.Min.x + perp_padding, rect.Max.y - thickness, rect.Max.x - perp_padding, rect.Max.y + thickness); } // Bottom
    if (border_n == 3) { return ImRect(rect.Min.x - thickness, rect.Min.y + perp_padding, rect.Min.x + thickness, rect.Max.y - perp_padding); } // Left
    IM_ASSERT(0);
    return ImRect();
}

// 0..3: corners (Lower-right, Lower-left, Unused, Unused)
// 4..7: borders (Top, Right, Bottom, Left)
ImGuiID ImGui::GetWindowResizeID(ImGuiWindow* window, int n)
{
    IM_ASSERT(n >= 0 && n <= 7);
    ImGuiID id = window->ID;
    id = ImHashStr("#RESIZE", 0, id);
    id = ImHashData(&n, sizeof(int), id);
    return id;
}

// Handle resize for: Resize Grips, Borders, Gamepad
// Return true when using auto-fit (double click on resize grip)
static bool ImGui::UpdateWindowManualResize(ImGuiWindow* window, const ImVec2& size_auto_fit, int* border_held, int resize_grip_count, ImU32 resize_grip_col[4])
{
    ImGuiContext& g = *GImGui;
    ImGuiWindowFlags flags = window->Flags;

    if ((flags & ImGuiWindowFlags_NoResize) || (flags & ImGuiWindowFlags_AlwaysAutoResize) || window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
        return false;
    if (window->WasActive == false) // Early out to avoid running this code for e.g. an hidden implicit/fallback Debug window.
        return false;

    bool ret_auto_fit = false;
    const int resize_border_count = g.IO.ConfigWindowsResizeFromEdges ? 4 : 0;
    const float grip_draw_size = IM_FLOOR(ImMax(g.FontSize * 1.35f, window->WindowRounding + 1.0f + g.FontSize * 0.2f));
    const float grip_hover_inner_size = IM_FLOOR(grip_draw_size * 0.75f);
    const float grip_hover_outer_size = g.IO.ConfigWindowsResizeFromEdges ? WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS : 0.0f;

    ImVec2 pos_target(FLT_MAX, FLT_MAX);
    ImVec2 size_target(FLT_MAX, FLT_MAX);

    // Resize grips and borders are on layer 1
    window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
    window->DC.NavLayerCurrentMask = (1 << ImGuiNavLayer_Menu);

    // Manual resize grips
    PushID("#RESIZE");
    for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
    {
        const ImGuiResizeGripDef& grip = resize_grip_def[resize_grip_n];
        const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);

        // Using the FlattenChilds button flag we make the resize button accessible even if we are hovering over a child window
        ImRect resize_rect(corner - grip.InnerDir * grip_hover_outer_size, corner + grip.InnerDir * grip_hover_inner_size);
        if (resize_rect.Min.x > resize_rect.Max.x) ImSwap(resize_rect.Min.x, resize_rect.Max.x);
        if (resize_rect.Min.y > resize_rect.Max.y) ImSwap(resize_rect.Min.y, resize_rect.Max.y);
        bool hovered, held;
        ButtonBehavior(resize_rect, window->GetID(resize_grip_n), &hovered, &held, ImGuiButtonFlags_FlattenChildren | ImGuiButtonFlags_NoNavFocus);
        //GetForegroundDrawList(window)->AddRect(resize_rect.Min, resize_rect.Max, IM_COL32(255, 255, 0, 255));
        if (hovered || held)
            g.MouseCursor = (resize_grip_n & 1) ? ImGuiMouseCursor_ResizeNESW : ImGuiMouseCursor_ResizeNWSE;

        if (held && g.IO.MouseDoubleClicked[0] && resize_grip_n == 0)
        {
            // Manual auto-fit when double-clicking
            size_target = CalcWindowSizeAfterConstraint(window, size_auto_fit);
            ret_auto_fit = true;
            ClearActiveID();
        } else if (held)
        {
            // Resize from any of the four corners
            // We don't use an incremental MouseDelta but rather compute an absolute target size based on mouse position
            ImVec2 corner_target = g.IO.MousePos - g.ActiveIdClickOffset + ImLerp(grip.InnerDir * grip_hover_outer_size, grip.InnerDir * -grip_hover_inner_size, grip.CornerPosN); // Corner of the window corresponding to our corner grip
            CalcResizePosSizeFromAnyCorner(window, corner_target, grip.CornerPosN, &pos_target, &size_target);
        }
        if (resize_grip_n == 0 || held || hovered)
            resize_grip_col[resize_grip_n] = GetColorU32(held ? ImGuiCol_ResizeGripActive : hovered ? ImGuiCol_ResizeGripHovered : ImGuiCol_ResizeGrip);
    }
    for (int border_n = 0; border_n < resize_border_count; border_n++)
    {
        bool hovered, held;
        ImRect border_rect = GetResizeBorderRect(window, border_n, grip_hover_inner_size, WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS);
        ButtonBehavior(border_rect, window->GetID(border_n + 4), &hovered, &held, ImGuiButtonFlags_FlattenChildren);
        //GetForegroundDrawLists(window)->AddRect(border_rect.Min, border_rect.Max, IM_COL32(255, 255, 0, 255));
        if ((hovered && g.HoveredIdTimer > WINDOWS_RESIZE_FROM_EDGES_FEEDBACK_TIMER) || held)
        {
            g.MouseCursor = (border_n & 1) ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS;
            if (held)
                *border_held = border_n;
        }
        if (held)
        {
            ImVec2 border_target = window->Pos;
            ImVec2 border_posn;
            if (border_n == 0) { border_posn = ImVec2(0, 0); border_target.y = (g.IO.MousePos.y - g.ActiveIdClickOffset.y + WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS); } // Top
            if (border_n == 1) { border_posn = ImVec2(1, 0); border_target.x = (g.IO.MousePos.x - g.ActiveIdClickOffset.x + WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS); } // Right
            if (border_n == 2) { border_posn = ImVec2(0, 1); border_target.y = (g.IO.MousePos.y - g.ActiveIdClickOffset.y + WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS); } // Bottom
            if (border_n == 3) { border_posn = ImVec2(0, 0); border_target.x = (g.IO.MousePos.x - g.ActiveIdClickOffset.x + WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS); } // Left
            CalcResizePosSizeFromAnyCorner(window, border_target, border_posn, &pos_target, &size_target);
        }
    }
    PopID();

    // Restore nav layer
    window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
    window->DC.NavLayerCurrentMask = (1 << ImGuiNavLayer_Main);

    // Navigation resize (keyboard/gamepad)
    if (g.NavWindowingTarget && g.NavWindowingTarget->RootWindow == window)
    {
        ImVec2 nav_resize_delta;
        if (g.NavInputSource == ImGuiInputSource_NavKeyboard && g.IO.KeyShift)
            nav_resize_delta = GetNavInputAmount2d(ImGuiNavDirSourceFlags_Keyboard, ImGuiInputReadMode_Down);
        if (g.NavInputSource == ImGuiInputSource_NavGamepad)
            nav_resize_delta = GetNavInputAmount2d(ImGuiNavDirSourceFlags_PadDPad, ImGuiInputReadMode_Down);
        if (nav_resize_delta.x != 0.0f || nav_resize_delta.y != 0.0f)
        {
            const float NAV_RESIZE_SPEED = 600.0f;
            nav_resize_delta *= ImFloor(NAV_RESIZE_SPEED * g.IO.DeltaTime * ImMin(g.IO.DisplayFramebufferScale.x, g.IO.DisplayFramebufferScale.y));
            g.NavWindowingToggleLayer = false;
            g.NavDisableMouseHover = true;
            resize_grip_col[0] = GetColorU32(ImGuiCol_ResizeGripActive);
            // FIXME-NAV: Should store and accumulate into a separate size buffer to handle sizing constraints properly, right now a constraint will make us stuck.
            size_target = CalcWindowSizeAfterConstraint(window, window->SizeFull + nav_resize_delta);
        }
    }

    // Apply back modified position/size to window
    if (size_target.x != FLT_MAX)
    {
        window->SizeFull = size_target;
        MarkIniSettingsDirty(window);
    }
    if (pos_target.x != FLT_MAX)
    {
        window->Pos = ImFloor(pos_target);
        MarkIniSettingsDirty(window);
    }

    window->Size = window->SizeFull;
    return ret_auto_fit;
}

static inline void ClampWindowRect(ImGuiWindow* window, const ImRect& rect, const ImVec2& padding)
{
    ImGuiContext& g = *GImGui;
    ImVec2 size_for_clamping = (g.IO.ConfigWindowsMoveFromTitleBarOnly && !(window->Flags & ImGuiWindowFlags_NoTitleBar)) ? ImVec2(window->Size.x, window->TitleBarHeight()) : window->Size;
    window->Pos = ImMin(rect.Max - padding, ImMax(window->Pos + size_for_clamping, rect.Min + padding) - size_for_clamping);
}

static void ImGui::RenderWindowOuterBorders(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    float rounding = window->WindowRounding;
    float border_size = window->WindowBorderSize;
    if (border_size > 0.0f && !(window->Flags & ImGuiWindowFlags_NoBackground))
        window->DrawList->AddRect(window->Pos, window->Pos + window->Size, GetColorU32(ImGuiCol_Border), rounding, ImDrawCornerFlags_All, border_size);

    int border_held = window->ResizeBorderHeld;
    if (border_held != -1)
    {
        const ImGuiResizeBorderDef& def = resize_border_def[border_held];
        ImRect border_r = GetResizeBorderRect(window, border_held, rounding, 0.0f);
        window->DrawList->PathArcTo(ImLerp(border_r.Min, border_r.Max, def.CornerPosN1) + ImVec2(0.5f, 0.5f) + def.InnerDir * rounding, rounding, def.OuterAngle - IM_PI * 0.25f, def.OuterAngle);
        window->DrawList->PathArcTo(ImLerp(border_r.Min, border_r.Max, def.CornerPosN2) + ImVec2(0.5f, 0.5f) + def.InnerDir * rounding, rounding, def.OuterAngle, def.OuterAngle + IM_PI * 0.25f);
        window->DrawList->PathStroke(GetColorU32(ImGuiCol_SeparatorActive), false, ImMax(2.0f, border_size)); // Thicker than usual
    }
    if (g.Style.FrameBorderSize > 0 && !(window->Flags & ImGuiWindowFlags_NoTitleBar))
    {
        float y = window->Pos.y + window->TitleBarHeight() - 1;
        window->DrawList->AddLine(ImVec2(window->Pos.x + border_size, y), ImVec2(window->Pos.x + window->Size.x - border_size, y), GetColorU32(ImGuiCol_Border), g.Style.FrameBorderSize);
    }
}

// Draw background and borders
// Draw and handle scrollbars
void ImGui::RenderWindowDecorations(ImGuiWindow* window, const ImRect& title_bar_rect, bool title_bar_is_highlight, int resize_grip_count, const ImU32 resize_grip_col[4], float resize_grip_draw_size)
{
    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    ImGuiWindowFlags flags = window->Flags;

    // Ensure that ScrollBar doesn't read last frame's SkipItems
    window->SkipItems = false;

    // Draw window + handle manual resize
    // As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
    const float window_rounding = window->WindowRounding;
    const float window_border_size = window->WindowBorderSize;
    if (window->Collapsed)
    {
        // Title bar only
        float backup_border_size = style.FrameBorderSize;
        g.Style.FrameBorderSize = window->WindowBorderSize;
        ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? ImGuiCol_TitleBgActive : ImGuiCol_TitleBgCollapsed);
        RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
        g.Style.FrameBorderSize = backup_border_size;
    } else
    {
        // Window background
        if (!(flags & ImGuiWindowFlags_NoBackground))
        {
            ImU32 bg_col = GetColorU32(GetWindowBgColorIdxFromFlags(flags));
            bool override_alpha = false;
            float alpha = 1.0f;
            if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasBgAlpha)
            {
                alpha = g.NextWindowData.BgAlphaVal;
                override_alpha = true;
            }
            if (override_alpha)
                bg_col = (bg_col & ~IM_COL32_A_MASK) | (IM_F32_TO_INT8_SAT(alpha) << IM_COL32_A_SHIFT);
            window->DrawList->AddRectFilled(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size, bg_col, window_rounding, (flags & ImGuiWindowFlags_NoTitleBar) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Bot);
        }

        // Title bar
        if (!(flags & ImGuiWindowFlags_NoTitleBar))
        {
            ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? ImGuiCol_TitleBgActive : ImGuiCol_TitleBg);
            window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
        }

        // Menu bar
        if (flags & ImGuiWindowFlags_MenuBar)
        {
            ImRect menu_bar_rect = window->MenuBarRect();
            menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
            window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(ImGuiCol_MenuBarBg), (flags & ImGuiWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
            if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
                window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(ImGuiCol_Border), style.FrameBorderSize);
        }

        // Scrollbars
        if (window->ScrollbarX)
            Scrollbar(ImGuiAxis_X);
        if (window->ScrollbarY)
            Scrollbar(ImGuiAxis_Y);

        // Render resize grips (after their input handling so we don't have a frame of latency)
        if (!(flags & ImGuiWindowFlags_NoResize))
        {
            for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
            {
                const ImGuiResizeGripDef& grip = resize_grip_def[resize_grip_n];
                const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
                window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, resize_grip_draw_size) : ImVec2(resize_grip_draw_size, window_border_size)));
                window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(resize_grip_draw_size, window_border_size) : ImVec2(window_border_size, resize_grip_draw_size)));
                window->DrawList->PathArcToFast(ImVec2(corner.x + grip.InnerDir.x * (window_rounding + window_border_size), corner.y + grip.InnerDir.y * (window_rounding + window_border_size)), window_rounding, grip.AngleMin12, grip.AngleMax12);
                window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
            }
        }

        // Borders
        RenderWindowOuterBorders(window);
    }
}

// Render title text, collapse button, close button
void ImGui::RenderWindowTitleBarContents(ImGuiWindow* window, const ImRect& title_bar_rect, const char* name, bool* p_open)
{
    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    ImGuiWindowFlags flags = window->Flags;

    const bool has_close_button = (p_open != NULL);
    const bool has_collapse_button = !(flags & ImGuiWindowFlags_NoCollapse) && (style.WindowMenuButtonPosition != ImGuiDir_None);

    // Close & Collapse button are on the Menu NavLayer and don't default focus (unless there's nothing else on that layer)
    const ImGuiItemFlags item_flags_backup = window->DC.ItemFlags;
    window->DC.ItemFlags |= ImGuiItemFlags_NoNavDefaultFocus;
    window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
    window->DC.NavLayerCurrentMask = (1 << ImGuiNavLayer_Menu);

    // Layout buttons
    // FIXME: Would be nice to generalize the subtleties expressed here into reusable code.
    float pad_l = style.FramePadding.x;
    float pad_r = style.FramePadding.x;
    float button_sz = g.FontSize;
    ImVec2 close_button_pos;
    ImVec2 collapse_button_pos;
    if (has_close_button)
    {
        pad_r += button_sz;
        close_button_pos = ImVec2(title_bar_rect.Max.x - pad_r - style.FramePadding.x, title_bar_rect.Min.y);
    }
    if (has_collapse_button && style.WindowMenuButtonPosition == ImGuiDir_Right)
    {
        pad_r += button_sz;
        collapse_button_pos = ImVec2(title_bar_rect.Max.x - pad_r - style.FramePadding.x, title_bar_rect.Min.y);
    }
    if (has_collapse_button && style.WindowMenuButtonPosition == ImGuiDir_Left)
    {
        collapse_button_pos = ImVec2(title_bar_rect.Min.x + pad_l - style.FramePadding.x, title_bar_rect.Min.y);
        pad_l += button_sz;
    }

    // Collapse button (submitting first so it gets priority when choosing a navigation init fallback)
    if (has_collapse_button)
        if (CollapseButton(window->GetID("#COLLAPSE"), collapse_button_pos))
            window->WantCollapseToggle = true; // Defer actual collapsing to next frame as we are too far in the Begin() function

    // Close button
    if (has_close_button)
        if (CloseButton(window->GetID("#CLOSE"), close_button_pos))
            *p_open = false;

    window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
    window->DC.NavLayerCurrentMask = (1 << ImGuiNavLayer_Main);
    window->DC.ItemFlags = item_flags_backup;

    // Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
    // FIXME: Refactor text alignment facilities along with RenderText helpers, this is WAY too much messy code..
    const char* UNSAVED_DOCUMENT_MARKER = "*";
    const float marker_size_x = (flags & ImGuiWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
    const ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);

    // As a nice touch we try to ensure that centered title text doesn't get affected by visibility of Close/Collapse button,
    // while uncentered title text will still reach edges correct.
    if (pad_l > style.FramePadding.x)
        pad_l += g.Style.ItemInnerSpacing.x;
    if (pad_r > style.FramePadding.x)
        pad_r += g.Style.ItemInnerSpacing.x;
    if (style.WindowTitleAlign.x > 0.0f && style.WindowTitleAlign.x < 1.0f)
    {
        float centerness = ImSaturate(1.0f - ImFabs(style.WindowTitleAlign.x - 0.5f) * 2.0f); // 0.0f on either edges, 1.0f on center
        float pad_extend = ImMin(ImMax(pad_l, pad_r), title_bar_rect.GetWidth() - pad_l - pad_r - text_size.x);
        pad_l = ImMax(pad_l, pad_extend * centerness);
        pad_r = ImMax(pad_r, pad_extend * centerness);
    }

    ImRect layout_r(title_bar_rect.Min.x + pad_l, title_bar_rect.Min.y, title_bar_rect.Max.x - pad_r, title_bar_rect.Max.y);
    ImRect clip_r(layout_r.Min.x, layout_r.Min.y, layout_r.Max.x + g.Style.ItemInnerSpacing.x, layout_r.Max.y);
    //if (g.IO.KeyCtrl) window->DrawList->AddRect(layout_r.Min, layout_r.Max, IM_COL32(255, 128, 0, 255)); // [DEBUG]
    RenderTextClipped(layout_r.Min, layout_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_r);
    if (flags & ImGuiWindowFlags_UnsavedDocument)
    {
        ImVec2 marker_pos = ImVec2(ImMax(layout_r.Min.x, layout_r.Min.x + (layout_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, layout_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
        ImVec2 off = ImVec2(0.0f, IM_FLOOR(-g.FontSize * 0.25f));
        RenderTextClipped(marker_pos + off, layout_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_r);
    }
}

void ImGui::UpdateWindowParentAndRootLinks(ImGuiWindow* window, ImGuiWindowFlags flags, ImGuiWindow* parent_window)
{
    window->ParentWindow = parent_window;
    window->RootWindow = window->RootWindowForTitleBarHighlight = window->RootWindowForNav = window;
    if (parent_window && (flags & ImGuiWindowFlags_ChildWindow) && !(flags & ImGuiWindowFlags_Tooltip))
        window->RootWindow = parent_window->RootWindow;
    if (parent_window && !(flags & ImGuiWindowFlags_Modal) && (flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Popup)))
        window->RootWindowForTitleBarHighlight = parent_window->RootWindowForTitleBarHighlight;
    while (window->RootWindowForNav->Flags & ImGuiWindowFlags_NavFlattened)
    {
        IM_ASSERT(window->RootWindowForNav->ParentWindow != NULL);
        window->RootWindowForNav = window->RootWindowForNav->ParentWindow;
    }
}

// Push a new Dear ImGui window to add widgets to.
// - A default window called "Debug" is automatically stacked at the beginning of every frame so you can use widgets without explicitly calling a Begin/End pair.
// - Begin/End can be called multiple times during the frame with the same window name to append content.
// - The window name is used as a unique identifier to preserve window information across frames (and save rudimentary information to the .ini file).
//   You can use the "##" or "###" markers to use the same label with different id, or same id with different label. See documentation at the top of this file.
// - Return false when window is collapsed, so you can early out in your code. You always need to call ImGui::End() even if false is returned.
// - Passing 'bool* p_open' displays a Close button on the upper-right corner of the window, the pointed value will be set to false when the button is pressed.
bool ImGui::Begin(const char* name, bool* p_open, ImGuiWindowFlags flags)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
    IM_ASSERT(g.WithinFrameScope);                  // Forgot to call ImGui::NewFrame()
    IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called ImGui::Render() or ImGui::EndFrame() and haven't called ImGui::NewFrame() again yet

    // Find or create
    ImGuiWindow* window = FindWindowByName(name);
    const bool window_just_created = (window == NULL);
    if (window_just_created)
        window = CreateNewWindow(name, flags);

    // Automatically disable manual moving/resizing when NoInputs is set
    if ((flags & ImGuiWindowFlags_NoInputs) == ImGuiWindowFlags_NoInputs)
        flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    if (flags & ImGuiWindowFlags_NavFlattened)
        IM_ASSERT(flags & ImGuiWindowFlags_ChildWindow);

    const int current_frame = g.FrameCount;
    const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);
    window->IsFallbackWindow = (g.CurrentWindowStack.Size == 0 && g.WithinFrameScopeWithImplicitWindow);

    // Update the Appearing flag
    bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
    const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
    if (flags & ImGuiWindowFlags_Popup)
    {
        ImGuiPopupData& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
        window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
        window_just_activated_by_user |= (window != popup_ref.Window);
    }
    window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
    if (window->Appearing)
        SetWindowConditionAllowFlags(window, ImGuiCond_Appearing, true);

    // Update Flags, LastFrameActive, BeginOrderXXX fields
    if (first_begin_of_the_frame)
    {
        window->Flags = (ImGuiWindowFlags)flags;
        window->LastFrameActive = current_frame;
        window->LastTimeActive = (float)g.Time;
        window->BeginOrderWithinParent = 0;
        window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
    } else
    {
        flags = window->Flags;
    }

    // Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
    ImGuiWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
    ImGuiWindow* parent_window = first_begin_of_the_frame ? ((flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
    IM_ASSERT(parent_window != NULL || !(flags & ImGuiWindowFlags_ChildWindow));

    // We allow window memory to be compacted so recreate the base stack when needed.
    if (window->IDStack.Size == 0)
        window->IDStack.push_back(window->ID);

    // Add to stack
    // We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
    g.CurrentWindowStack.push_back(window);
    g.CurrentWindow = NULL;
    ErrorCheckBeginEndCompareStacksSize(window, true);
    if (flags & ImGuiWindowFlags_Popup)
    {
        ImGuiPopupData& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
        popup_ref.Window = window;
        g.BeginPopupStack.push_back(popup_ref);
        window->PopupId = popup_ref.PopupId;
    }

    if (window_just_appearing_after_hidden_for_resize && !(flags & ImGuiWindowFlags_ChildWindow))
        window->NavLastIds[0] = 0;

    // Update ->RootWindow and others pointers (before any possible call to FocusWindow)
    if (first_begin_of_the_frame)
        UpdateWindowParentAndRootLinks(window, flags, parent_window);

    // Process SetNextWindow***() calls
    // (FIXME: Consider splitting the HasXXX flags into X/Y components
    bool window_pos_set_by_api = false;
    bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasPos)
    {
        window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
        if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
        {
            // May be processed on the next frame if this is our first frame and we are measuring size
            // FIXME: Look into removing the branch so everything can go through this same code path for consistency.
            window->SetWindowPosVal = g.NextWindowData.PosVal;
            window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
            window->SetWindowPosAllowFlags &= ~(ImGuiCond_Once | ImGuiCond_FirstUseEver | ImGuiCond_Appearing);
        } else
        {
            SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
        }
    }
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize)
    {
        window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
        window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
        SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
    }
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasScroll)
    {
        if (g.NextWindowData.ScrollVal.x >= 0.0f)
        {
            window->ScrollTarget.x = g.NextWindowData.ScrollVal.x;
            window->ScrollTargetCenterRatio.x = 0.0f;
        }
        if (g.NextWindowData.ScrollVal.y >= 0.0f)
        {
            window->ScrollTarget.y = g.NextWindowData.ScrollVal.y;
            window->ScrollTargetCenterRatio.y = 0.0f;
        }
    }
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasContentSize)
        window->ContentSizeExplicit = g.NextWindowData.ContentSizeVal;
    else if (first_begin_of_the_frame)
        window->ContentSizeExplicit = ImVec2(0.0f, 0.0f);
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasCollapsed)
        SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasFocus)
        FocusWindow(window);
    if (window->Appearing)
        SetWindowConditionAllowFlags(window, ImGuiCond_Appearing, false);

    // When reusing window again multiple times a frame, just append content (don't need to setup again)
    if (first_begin_of_the_frame)
    {
        // Initialize
        const bool window_is_child_tooltip = (flags & ImGuiWindowFlags_ChildWindow) && (flags & ImGuiWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
        window->Active = true;
        window->HasCloseButton = (p_open != NULL);
        window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
        window->IDStack.resize(1);

        // Restore buffer capacity when woken from a compacted state, to avoid
        if (window->MemoryCompacted)
            GcAwakeTransientWindowBuffers(window);

        // Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
        // The title bar always display the 'name' parameter, so we only update the string storage if it needs to be visible to the end-user elsewhere.
        bool window_title_visible_elsewhere = false;
        if (g.NavWindowingListWindow != NULL && (window->Flags & ImGuiWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
            window_title_visible_elsewhere = true;
        if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
        {
            size_t buf_len = (size_t)window->NameBufLen;
            window->Name = ImStrdupcpy(window->Name, &buf_len, name);
            window->NameBufLen = (int)buf_len;
        }

        // UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

        // Update contents size from last frame for auto-fitting (or use explicit size)
        window->ContentSize = CalcWindowContentSize(window);
        if (window->HiddenFramesCanSkipItems > 0)
            window->HiddenFramesCanSkipItems--;
        if (window->HiddenFramesCannotSkipItems > 0)
            window->HiddenFramesCannotSkipItems--;

        // Hide new windows for one frame until they calculate their size
        if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
            window->HiddenFramesCannotSkipItems = 1;

        // Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
        // We reset Size/ContentSize for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
        if (window_just_activated_by_user && (flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_Tooltip)) != 0)
        {
            window->HiddenFramesCannotSkipItems = 1;
            if (flags & ImGuiWindowFlags_AlwaysAutoResize)
            {
                if (!window_size_x_set_by_api)
                    window->Size.x = window->SizeFull.x = 0.f;
                if (!window_size_y_set_by_api)
                    window->Size.y = window->SizeFull.y = 0.f;
                window->ContentSize = ImVec2(0.f, 0.f);
            }
        }

        // SELECT VIEWPORT
        // FIXME-VIEWPORT: In the docking/viewport branch, this is the point where we select the current viewport (which may affect the style)
        SetCurrentWindow(window);

        // LOCK BORDER SIZE AND PADDING FOR THE FRAME (so that altering them doesn't cause inconsistencies)

        if (flags & ImGuiWindowFlags_ChildWindow)
            window->WindowBorderSize = style.ChildBorderSize;
        else
            window->WindowBorderSize = ((flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_Tooltip)) && !(flags & ImGuiWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
        window->WindowPadding = style.WindowPadding;
        if ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & (ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
            window->WindowPadding = ImVec2(0.0f, (flags & ImGuiWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);

        // Collapse window by double-clicking on title bar
        // At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
        if (!(flags & ImGuiWindowFlags_NoTitleBar) && !(flags & ImGuiWindowFlags_NoCollapse))
        {
            // We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
            ImRect title_bar_rect = window->TitleBarRect();
            if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
                window->WantCollapseToggle = true;
            if (window->WantCollapseToggle)
            {
                window->Collapsed = !window->Collapsed;
                MarkIniSettingsDirty(window);
                FocusWindow(window);
            }
        } else
        {
            window->Collapsed = false;
        }
        window->WantCollapseToggle = false;

        // SIZE

        // Calculate auto-fit size, handle automatic resize
        const ImVec2 size_auto_fit = CalcWindowAutoFitSize(window, window->ContentSize);
        bool use_current_size_for_scrollbar_x = window_just_created;
        bool use_current_size_for_scrollbar_y = window_just_created;
        if ((flags & ImGuiWindowFlags_AlwaysAutoResize) && !window->Collapsed)
        {
            // Using SetNextWindowSize() overrides ImGuiWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
            if (!window_size_x_set_by_api)
            {
                window->SizeFull.x = size_auto_fit.x;
                use_current_size_for_scrollbar_x = true;
            }
            if (!window_size_y_set_by_api)
            {
                window->SizeFull.y = size_auto_fit.y;
                use_current_size_for_scrollbar_y = true;
            }
        } else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
        {
            // Auto-fit may only grow window during the first few frames
            // We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor ImGuiWindowFlags_AlwaysAutoResize when collapsed.
            if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
            {
                window->SizeFull.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
                use_current_size_for_scrollbar_x = true;
            }
            if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
            {
                window->SizeFull.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
                use_current_size_for_scrollbar_y = true;
            }
            if (!window->Collapsed)
                MarkIniSettingsDirty(window);
        }

        // Apply minimum/maximum window size constraints and final size
        window->SizeFull = CalcWindowSizeAfterConstraint(window, window->SizeFull);
        window->Size = window->Collapsed && !(flags & ImGuiWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

        // Decoration size
        const float decoration_up_height = window->TitleBarHeight() + window->MenuBarHeight();

        // POSITION

        // Popup latch its initial position, will position itself when it appears next frame
        if (window_just_activated_by_user)
        {
            window->AutoPosLastDirection = ImGuiDir_None;
            if ((flags & ImGuiWindowFlags_Popup) != 0 && !window_pos_set_by_api)
                window->Pos = g.BeginPopupStack.back().OpenPopupPos;
        }

        // Position child window
        if (flags & ImGuiWindowFlags_ChildWindow)
        {
            IM_ASSERT(parent_window && parent_window->Active);
            window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
            parent_window->DC.ChildWindows.push_back(window);
            if (!(flags & ImGuiWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
                window->Pos = parent_window->DC.CursorPos;
        }

        const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
        if (window_pos_with_pivot)
            SetWindowPos(window, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot, 0); // Position given a pivot (e.g. for centering)
        else if ((flags & ImGuiWindowFlags_ChildMenu) != 0)
            window->Pos = FindBestWindowPosForPopup(window);
        else if ((flags & ImGuiWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
            window->Pos = FindBestWindowPosForPopup(window);
        else if ((flags & ImGuiWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
            window->Pos = FindBestWindowPosForPopup(window);

        // Clamp position/size so window stays visible within its viewport or monitor
        // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
        ImRect viewport_rect(GetViewportRect());
        if (!window_pos_set_by_api && !(flags & ImGuiWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
        {
            ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
            if (viewport_rect.GetWidth() > 0 && viewport_rect.GetHeight() > 0.0f)
            {
                ClampWindowRect(window, viewport_rect, clamp_padding);
            }
        }
        window->Pos = ImFloor(window->Pos);

        // Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
        // Large values tend to lead to variety of artifacts and are not recommended.
        window->WindowRounding = (flags & ImGuiWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

        // For windows with title bar or menu bar, we clamp to FrameHeight(FontSize + FramePadding.y * 2.0f) to completely hide artifacts.
        //if ((window->Flags & ImGuiWindowFlags_MenuBar) || !(window->Flags & ImGuiWindowFlags_NoTitleBar))
        //    window->WindowRounding = ImMin(window->WindowRounding, g.FontSize + style.FramePadding.y * 2.0f);

        // Apply window focus (new and reactivated windows are moved to front)
        bool want_focus = false;
        if (window_just_activated_by_user && !(flags & ImGuiWindowFlags_NoFocusOnAppearing))
        {
            if (flags & ImGuiWindowFlags_Popup)
                want_focus = true;
            else if ((flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Tooltip)) == 0)
                want_focus = true;
        }

        // Handle manual resize: Resize Grips, Borders, Gamepad
        int border_held = -1;
        ImU32 resize_grip_col[4] = {};
        const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // Allow resize from lower-left if we have the mouse cursor feedback for it.
        const float resize_grip_draw_size = IM_FLOOR(ImMax(g.FontSize * 1.35f, window->WindowRounding + 1.0f + g.FontSize * 0.2f));
        if (!window->Collapsed)
            if (UpdateWindowManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]))
                use_current_size_for_scrollbar_x = use_current_size_for_scrollbar_y = true;
        window->ResizeBorderHeld = (signed char)border_held;

        // SCROLLBAR VISIBILITY

        // Update scrollbar visibility (based on the Size that was effective during last frame or the auto-resized Size).
        if (!window->Collapsed)
        {
            // When reading the current size we need to read it after size constraints have been applied.
            // When we use InnerRect here we are intentionally reading last frame size, same for ScrollbarSizes values before we set them again.
            ImVec2 avail_size_from_current_frame = ImVec2(window->SizeFull.x, window->SizeFull.y - decoration_up_height);
            ImVec2 avail_size_from_last_frame = window->InnerRect.GetSize() + window->ScrollbarSizes;
            ImVec2 needed_size_from_last_frame = window_just_created ? ImVec2(0, 0) : window->ContentSize + window->WindowPadding * 2.0f;
            float size_x_for_scrollbars = use_current_size_for_scrollbar_x ? avail_size_from_current_frame.x : avail_size_from_last_frame.x;
            float size_y_for_scrollbars = use_current_size_for_scrollbar_y ? avail_size_from_current_frame.y : avail_size_from_last_frame.y;
            //bool scrollbar_y_from_last_frame = window->ScrollbarY; // FIXME: May want to use that in the ScrollbarX expression? How many pros vs cons?
            window->ScrollbarY = (flags & ImGuiWindowFlags_AlwaysVerticalScrollbar) || ((needed_size_from_last_frame.y > size_y_for_scrollbars) && !(flags & ImGuiWindowFlags_NoScrollbar));
            window->ScrollbarX = (flags & ImGuiWindowFlags_AlwaysHorizontalScrollbar) || ((needed_size_from_last_frame.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & ImGuiWindowFlags_NoScrollbar) && (flags & ImGuiWindowFlags_HorizontalScrollbar));
            if (window->ScrollbarX && !window->ScrollbarY)
                window->ScrollbarY = (needed_size_from_last_frame.y > size_y_for_scrollbars) && !(flags & ImGuiWindowFlags_NoScrollbar);
            window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
        }

        // UPDATE RECTANGLES (1- THOSE NOT AFFECTED BY SCROLLING)
        // Update various regions. Variables they depends on should be set above in this function.
        // We set this up after processing the resize grip so that our rectangles doesn't lag by a frame.

        // Outer rectangle
        // Not affected by window border size. Used by:
        // - FindHoveredWindow() (w/ extra padding when border resize is enabled)
        // - Begin() initial clipping rect for drawing window background and borders.
        // - Begin() clipping whole child
        const ImRect host_rect = ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & ImGuiWindowFlags_Popup) && !window_is_child_tooltip) ? parent_window->ClipRect : viewport_rect;
        const ImRect outer_rect = window->Rect();
        const ImRect title_bar_rect = window->TitleBarRect();
        window->OuterRectClipped = outer_rect;
        window->OuterRectClipped.ClipWith(host_rect);

        // Inner rectangle
        // Not affected by window border size. Used by:
        // - InnerClipRect
        // - ScrollToBringRectIntoView()
        // - NavUpdatePageUpPageDown()
        // - Scrollbar()
        window->InnerRect.Min.x = window->Pos.x;
        window->InnerRect.Min.y = window->Pos.y + decoration_up_height;
        window->InnerRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x;
        window->InnerRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y;

        // Inner clipping rectangle.
        // Will extend a little bit outside the normal work region.
        // This is to allow e.g. Selectable or CollapsingHeader or some separators to cover that space.
        // Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
        // Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
        // Affected by window/frame border size. Used by:
        // - Begin() initial clip rect
        float top_border_size = (((flags & ImGuiWindowFlags_MenuBar) || !(flags & ImGuiWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
        window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerRect.Min.x + ImMax(ImFloor(window->WindowPadding.x * 0.5f), window->WindowBorderSize));
        window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerRect.Min.y + top_border_size);
        window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerRect.Max.x - ImMax(ImFloor(window->WindowPadding.x * 0.5f), window->WindowBorderSize));
        window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerRect.Max.y - window->WindowBorderSize);
        window->InnerClipRect.ClipWithFull(host_rect);

        // Default item width. Make it proportional to window size if window manually resizes
        if (window->Size.x > 0.0f && !(flags & ImGuiWindowFlags_Tooltip) && !(flags & ImGuiWindowFlags_AlwaysAutoResize))
            window->ItemWidthDefault = ImFloor(window->Size.x * 0.65f);
        else
            window->ItemWidthDefault = ImFloor(g.FontSize * 16.0f);

        // SCROLLING

        // Lock down maximum scrolling
        // The value of ScrollMax are ahead from ScrollbarX/ScrollbarY which is intentionally using InnerRect from previous rect in order to accommodate
        // for right/bottom aligned items without creating a scrollbar.
        window->ScrollMax.x = ImMax(0.0f, window->ContentSize.x + window->WindowPadding.x * 2.0f - window->InnerRect.GetWidth());
        window->ScrollMax.y = ImMax(0.0f, window->ContentSize.y + window->WindowPadding.y * 2.0f - window->InnerRect.GetHeight());

        // Apply scrolling
        window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
        window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

        // DRAWING

        // Setup draw list and outer clipping rectangle
        window->DrawList->_ResetForNewFrame();
        window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
        PushClipRect(host_rect.Min, host_rect.Max, false);

        // Draw modal window background (darkens what is behind them, all viewports)
        const bool dim_bg_for_modal = (flags & ImGuiWindowFlags_Modal) && window == GetTopMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
        const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
        if (dim_bg_for_modal || dim_bg_for_window_list)
        {
            const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? ImGuiCol_ModalWindowDimBg : ImGuiCol_NavWindowingDimBg, g.DimBgRatio);
            window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
        }

        // Draw navigation selection/windowing rectangle background
        if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
        {
            ImRect bb = window->Rect();
            bb.Expand(g.FontSize);
            if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
                window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(ImGuiCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
        }

        // Since 1.71, child window can render their decoration (bg color, border, scrollbars, etc.) within their parent to save a draw call.
        // When using overlapping child windows, this will break the assumption that child z-order is mapped to submission order.
        // We disable this when the parent window has zero vertices, which is a common pattern leading to laying out multiple overlapping child.
        // We also disabled this when we have dimming overlay behind this specific one child.
        // FIXME: More code may rely on explicit sorting of overlapping child window and would need to disable this somehow. Please get in contact if you are affected.
        {
            bool render_decorations_in_parent = false;
            if ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & ImGuiWindowFlags_Popup) && !window_is_child_tooltip)
                if (window->DrawList->CmdBuffer.back().ElemCount == 0 && parent_window->DrawList->VtxBuffer.Size > 0)
                    render_decorations_in_parent = true;
            if (render_decorations_in_parent)
                window->DrawList = parent_window->DrawList;

            // Handle title bar, scrollbar, resize grips and resize borders
            const ImGuiWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
            const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
            RenderWindowDecorations(window, title_bar_rect, title_bar_is_highlight, resize_grip_count, resize_grip_col, resize_grip_draw_size);

            if (render_decorations_in_parent)
                window->DrawList = &window->DrawListInst;
        }

        // Draw navigation selection/windowing rectangle border
        if (g.NavWindowingTargetAnim == window)
        {
            float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
            ImRect bb = window->Rect();
            bb.Expand(g.FontSize);
            if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
            {
                bb.Expand(-g.FontSize - 1.0f);
                rounding = window->WindowRounding;
            }
            window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
        }

        // UPDATE RECTANGLES (2- THOSE AFFECTED BY SCROLLING)

        // Work rectangle.
        // Affected by window padding and border size. Used by:
        // - Columns() for right-most edge
        // - TreeNode(), CollapsingHeader() for right-most edge
        // - BeginTabBar() for right-most edge
        const bool allow_scrollbar_x = !(flags & ImGuiWindowFlags_NoScrollbar) && (flags & ImGuiWindowFlags_HorizontalScrollbar);
        const bool allow_scrollbar_y = !(flags & ImGuiWindowFlags_NoScrollbar);
        const float work_rect_size_x = (window->ContentSizeExplicit.x != 0.0f ? window->ContentSizeExplicit.x : ImMax(allow_scrollbar_x ? window->ContentSize.x : 0.0f, window->Size.x - window->WindowPadding.x * 2.0f - window->ScrollbarSizes.x));
        const float work_rect_size_y = (window->ContentSizeExplicit.y != 0.0f ? window->ContentSizeExplicit.y : ImMax(allow_scrollbar_y ? window->ContentSize.y : 0.0f, window->Size.y - window->WindowPadding.y * 2.0f - decoration_up_height - window->ScrollbarSizes.y));
        window->WorkRect.Min.x = ImFloor(window->InnerRect.Min.x - window->Scroll.x + ImMax(window->WindowPadding.x, window->WindowBorderSize));
        window->WorkRect.Min.y = ImFloor(window->InnerRect.Min.y - window->Scroll.y + ImMax(window->WindowPadding.y, window->WindowBorderSize));
        window->WorkRect.Max.x = window->WorkRect.Min.x + work_rect_size_x;
        window->WorkRect.Max.y = window->WorkRect.Min.y + work_rect_size_y;

        // [LEGACY] Content Region
        // FIXME-OBSOLETE: window->ContentRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
        // Used by:
        // - Mouse wheel scrolling + many other things
        window->ContentRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
        window->ContentRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + decoration_up_height;
        window->ContentRegionRect.Max.x = window->ContentRegionRect.Min.x + (window->ContentSizeExplicit.x != 0.0f ? window->ContentSizeExplicit.x : (window->Size.x - window->WindowPadding.x * 2.0f - window->ScrollbarSizes.x));
        window->ContentRegionRect.Max.y = window->ContentRegionRect.Min.y + (window->ContentSizeExplicit.y != 0.0f ? window->ContentSizeExplicit.y : (window->Size.y - window->WindowPadding.y * 2.0f - decoration_up_height - window->ScrollbarSizes.y));

        // Setup drawing context
        // (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
        window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
        window->DC.GroupOffset.x = 0.0f;
        window->DC.ColumnsOffset.x = 0.0f;
        window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, decoration_up_height + window->WindowPadding.y - window->Scroll.y);
        window->DC.CursorPos = window->DC.CursorStartPos;
        window->DC.CursorPosPrevLine = window->DC.CursorPos;
        window->DC.CursorMaxPos = window->DC.CursorStartPos;
        window->DC.CurrLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
        window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;

        window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
        window->DC.NavLayerCurrentMask = (1 << ImGuiNavLayer_Main);
        window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
        window->DC.NavLayerActiveMaskNext = 0x00;
        window->DC.NavFocusScopeIdCurrent = (flags & ImGuiWindowFlags_ChildWindow) ? parent_window->DC.NavFocusScopeIdCurrent : 0; // -V595
        window->DC.NavHideHighlightOneFrame = false;
        window->DC.NavHasScroll = (window->ScrollMax.y > 0.0f);

        window->DC.MenuBarAppending = false;
        window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
        window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;
        window->DC.MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);
        window->DC.TreeDepth = 0;
        window->DC.TreeJumpToParentOnPopMask = 0x00;
        window->DC.ChildWindows.resize(0);
        window->DC.StateStorage = &window->StateStorage;
        window->DC.CurrentColumns = NULL;
        window->DC.LayoutType = ImGuiLayoutType_Vertical;
        window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : ImGuiLayoutType_Vertical;
        window->DC.FocusCounterRegular = window->DC.FocusCounterTabStop = -1;

        window->DC.ItemWidth = window->ItemWidthDefault;
        window->DC.TextWrapPos = -1.0f; // disabled
        window->DC.ItemFlagsStack.resize(0);
        window->DC.ItemWidthStack.resize(0);
        window->DC.TextWrapPosStack.resize(0);
        window->DC.GroupStack.resize(0);
        window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : ImGuiItemFlags_Default_;
        if (parent_window)
            window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);

        if (window->AutoFitFramesX > 0)
            window->AutoFitFramesX--;
        if (window->AutoFitFramesY > 0)
            window->AutoFitFramesY--;

        // Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
        if (want_focus)
        {
            FocusWindow(window);
            NavInitWindow(window, false);
        }

        // Title bar
        if (!(flags & ImGuiWindowFlags_NoTitleBar))
            RenderWindowTitleBarContents(window, title_bar_rect, name, p_open);

        // Pressing CTRL+C while holding on a window copy its content to the clipboard
        // This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
        // Maybe we can support CTRL+C on every element?
        /*
        if (g.ActiveId == move_id)
            if (g.IO.KeyCtrl && IsKeyPressedMap(ImGuiKey_C))
                LogToClipboard();
        */

        // We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
        // This is useful to allow creating context menus on title bar only, etc.
        window->DC.LastItemId = window->MoveId;
        window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? ImGuiItemStatusFlags_HoveredRect : 0;
        window->DC.LastItemRect = title_bar_rect;
#ifdef IMGUI_ENABLE_TEST_ENGINE
        if (!(window->Flags & ImGuiWindowFlags_NoTitleBar))
            IMGUI_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
    } else
    {
        // Append
        SetCurrentWindow(window);
    }

    PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

    // Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
    if (first_begin_of_the_frame)
        window->WriteAccessed = false;

    window->BeginCount++;
    g.NextWindowData.ClearFlags();

    if (flags & ImGuiWindowFlags_ChildWindow)
    {
        // Child window can be out of sight and have "negative" clip windows.
        // Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
        IM_ASSERT((flags & ImGuiWindowFlags_NoTitleBar) != 0);
        if (!(flags & ImGuiWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
            if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
                window->HiddenFramesCanSkipItems = 1;

        // Hide along with parent or if parent is collapsed
        if (parent_window && (parent_window->Collapsed || parent_window->HiddenFramesCanSkipItems > 0))
            window->HiddenFramesCanSkipItems = 1;
        if (parent_window && (parent_window->Collapsed || parent_window->HiddenFramesCannotSkipItems > 0))
            window->HiddenFramesCannotSkipItems = 1;
    }

    // Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
    if (style.Alpha <= 0.0f)
        window->HiddenFramesCanSkipItems = 1;

    // Update the Hidden flag
    window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

    // Update the SkipItems flag, used to early out of all items functions (no layout required)
    bool skip_items = false;
    if (window->Collapsed || !window->Active || window->Hidden)
        if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
            skip_items = true;
    window->SkipItems = skip_items;

    return !skip_items;
}

void ImGui::End()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    // Error checking: verify that user hasn't called End() too many times!
    if (g.CurrentWindowStack.Size <= 1 && g.WithinFrameScopeWithImplicitWindow)
    {
        IM_ASSERT_USER_ERROR(g.CurrentWindowStack.Size > 1, "Calling End() too many times!");
        return;
    }
    IM_ASSERT(g.CurrentWindowStack.Size > 0);

    // Error checking: verify that user doesn't directly call End() on a child window.
    if (window->Flags & ImGuiWindowFlags_ChildWindow)
        IM_ASSERT_USER_ERROR(g.WithinEndChild, "Must call EndChild() and not End()!");

    // Close anything that is open
    if (window->DC.CurrentColumns)
        EndColumns();
    PopClipRect();   // Inner window clip rectangle

    // Stop logging
    if (!(window->Flags & ImGuiWindowFlags_ChildWindow))    // FIXME: add more options for scope of logging
        LogFinish();

    // Pop from window stack
    g.CurrentWindowStack.pop_back();
    if (window->Flags & ImGuiWindowFlags_Popup)
        g.BeginPopupStack.pop_back();
    ErrorCheckBeginEndCompareStacksSize(window, false);
    SetCurrentWindow(g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back());
}

void ImGui::BringWindowToFocusFront(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    if (g.WindowsFocusOrder.back() == window)
        return;
    for (int i = g.WindowsFocusOrder.Size - 2; i >= 0; i--) // We can ignore the top-most window
        if (g.WindowsFocusOrder[i] == window)
        {
            memmove(&g.WindowsFocusOrder[i], &g.WindowsFocusOrder[i + 1], (size_t)(g.WindowsFocusOrder.Size - i - 1) * sizeof(ImGuiWindow*));
            g.WindowsFocusOrder[g.WindowsFocusOrder.Size - 1] = window;
            break;
        }
}

void ImGui::BringWindowToDisplayFront(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* current_front_window = g.Windows.back();
    if (current_front_window == window || current_front_window->RootWindow == window)
        return;
    for (int i = g.Windows.Size - 2; i >= 0; i--) // We can ignore the top-most window
        if (g.Windows[i] == window)
        {
            memmove(&g.Windows[i], &g.Windows[i + 1], (size_t)(g.Windows.Size - i - 1) * sizeof(ImGuiWindow*));
            g.Windows[g.Windows.Size - 1] = window;
            break;
        }
}

void ImGui::BringWindowToDisplayBack(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    if (g.Windows[0] == window)
        return;
    for (int i = 0; i < g.Windows.Size; i++)
        if (g.Windows[i] == window)
        {
            memmove(&g.Windows[1], &g.Windows[0], (size_t)i * sizeof(ImGuiWindow*));
            g.Windows[0] = window;
            break;
        }
}

// Moving window to front of display and set focus (which happens to be back of our sorted list)
void ImGui::FocusWindow(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;

    if (g.NavWindow != window)
    {
        g.NavWindow = window;
        if (window && g.NavDisableMouseHover)
            g.NavMousePosDirty = true;
        g.NavInitRequest = false;
        g.NavId = window ? window->NavLastIds[0] : 0; // Restore NavId
        g.NavFocusScopeId = 0;
        g.NavIdIsAlive = false;
        g.NavLayer = ImGuiNavLayer_Main;
        //IMGUI_DEBUG_LOG("FocusWindow(\"%s\")\n", window ? window->Name : NULL);
    }

    // Close popups if any
    ClosePopupsOverWindow(window, false);

    // Move the root window to the top of the pile
    IM_ASSERT(window == NULL || window->RootWindow != NULL);
    ImGuiWindow* focus_front_window = window ? window->RootWindow : NULL; // NB: In docking branch this is window->RootWindowDockStop
    ImGuiWindow* display_front_window = window ? window->RootWindow : NULL;

    // Steal focus on active widgets
    if (g.ActiveId != 0 && g.ActiveIdWindow && g.ActiveIdWindow->RootWindow != focus_front_window)
        ClearActiveID();

    // Passing NULL allow to disable keyboard focus
    if (!window)
        return;

    // Bring to front
    BringWindowToFocusFront(focus_front_window);
    if (((window->Flags | display_front_window->Flags) & ImGuiWindowFlags_NoBringToFrontOnFocus) == 0)
        BringWindowToDisplayFront(display_front_window);
}

void ImGui::FocusTopMostWindowUnderOne(ImGuiWindow* under_this_window, ImGuiWindow* ignore_window)
{
    ImGuiContext& g = *GImGui;

    int start_idx = g.WindowsFocusOrder.Size - 1;
    if (under_this_window != NULL)
    {
        int under_this_window_idx = FindWindowFocusIndex(under_this_window);
        if (under_this_window_idx != -1)
            start_idx = under_this_window_idx - 1;
    }
    for (int i = start_idx; i >= 0; i--)
    {
        // We may later decide to test for different NoXXXInputs based on the active navigation input (mouse vs nav) but that may feel more confusing to the user.
        ImGuiWindow* window = g.WindowsFocusOrder[i];
        if (window != ignore_window && window->WasActive && !(window->Flags & ImGuiWindowFlags_ChildWindow))
            if ((window->Flags & (ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs)) != (ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs))
            {
                ImGuiWindow* focus_window = NavRestoreLastChildNavWindow(window);
                FocusWindow(focus_window);
                return;
            }
    }
    FocusWindow(NULL);
}

void ImGui::SetCurrentFont(ImFont* font)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(font && font->IsLoaded());    // Font Atlas not created. Did you call io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?
    IM_ASSERT(font->Scale > 0.0f);
    g.Font = font;
    g.FontBaseSize = ImMax(1.0f, g.IO.FontGlobalScale * g.Font->FontSize * g.Font->Scale);
    g.FontSize = g.CurrentWindow ? g.CurrentWindow->CalcFontSize() : 0.0f;

    ImFontAtlas* atlas = g.Font->ContainerAtlas;
    g.DrawListSharedData.TexUvWhitePixel = atlas->TexUvWhitePixel;
    g.DrawListSharedData.Font = g.Font;
    g.DrawListSharedData.FontSize = g.FontSize;
}

void ImGui::PushFont(ImFont* font)
{
    ImGuiContext& g = *GImGui;
    if (!font)
        font = GetDefaultFont();
    SetCurrentFont(font);
    g.FontStack.push_back(font);
    g.CurrentWindow->DrawList->PushTextureID(font->ContainerAtlas->TexID);
}

void  ImGui::PopFont()
{
    ImGuiContext& g = *GImGui;
    g.CurrentWindow->DrawList->PopTextureID();
    g.FontStack.pop_back();
    SetCurrentFont(g.FontStack.empty() ? GetDefaultFont() : g.FontStack.back());
}

void ImGui::PushItemFlag(ImGuiItemFlags option, bool enabled)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (enabled)
        window->DC.ItemFlags |= option;
    else
        window->DC.ItemFlags &= ~option;
    window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
}

void ImGui::PopItemFlag()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ItemFlagsStack.pop_back();
    window->DC.ItemFlags = window->DC.ItemFlagsStack.empty() ? ImGuiItemFlags_Default_ : window->DC.ItemFlagsStack.back();
}

// FIXME: Look into renaming this once we have settled the new Focus/Activation/TabStop system.
void ImGui::PushAllowKeyboardFocus(bool allow_keyboard_focus)
{
    PushItemFlag(ImGuiItemFlags_NoTabStop, !allow_keyboard_focus);
}

void ImGui::PopAllowKeyboardFocus()
{
    PopItemFlag();
}

void ImGui::PushButtonRepeat(bool repeat)
{
    PushItemFlag(ImGuiItemFlags_ButtonRepeat, repeat);
}

void ImGui::PopButtonRepeat()
{
    PopItemFlag();
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

bool ImGui::IsWindowChildOf(ImGuiWindow* window, ImGuiWindow* potential_parent)
{
    if (window->RootWindow == potential_parent)
        return true;
    while (window != NULL)
    {
        if (window == potential_parent)
            return true;
        window = window->ParentWindow;
    }
    return false;
}

bool ImGui::IsWindowHovered(ImGuiHoveredFlags flags)
{
    IM_ASSERT((flags & ImGuiHoveredFlags_AllowWhenOverlapped) == 0);   // Flags not supported by this function
    ImGuiContext& g = *GImGui;

    if (flags & ImGuiHoveredFlags_AnyWindow)
    {
        if (g.HoveredWindow == NULL)
            return false;
    } else
    {
        switch (flags & (ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows))
        {
        case ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows:
            if (g.HoveredRootWindow != g.CurrentWindow->RootWindow)
                return false;
            break;
        case ImGuiHoveredFlags_RootWindow:
            if (g.HoveredWindow != g.CurrentWindow->RootWindow)
                return false;
            break;
        case ImGuiHoveredFlags_ChildWindows:
            if (g.HoveredWindow == NULL || !IsWindowChildOf(g.HoveredWindow, g.CurrentWindow))
                return false;
            break;
        default:
            if (g.HoveredWindow != g.CurrentWindow)
                return false;
            break;
        }
    }

    if (!IsWindowContentHoverable(g.HoveredWindow, flags))
        return false;
    if (!(flags & ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        if (g.ActiveId != 0 && !g.ActiveIdAllowOverlap && g.ActiveId != g.HoveredWindow->MoveId)
            return false;
    return true;
}

bool ImGui::IsWindowFocused(ImGuiFocusedFlags flags)
{
    ImGuiContext& g = *GImGui;

    if (flags & ImGuiFocusedFlags_AnyWindow)
        return g.NavWindow != NULL;

    IM_ASSERT(g.CurrentWindow);     // Not inside a Begin()/End()
    switch (flags & (ImGuiFocusedFlags_RootWindow | ImGuiFocusedFlags_ChildWindows))
    {
    case ImGuiFocusedFlags_RootWindow | ImGuiFocusedFlags_ChildWindows:
        return g.NavWindow && g.NavWindow->RootWindow == g.CurrentWindow->RootWindow;
    case ImGuiFocusedFlags_RootWindow:
        return g.NavWindow == g.CurrentWindow->RootWindow;
    case ImGuiFocusedFlags_ChildWindows:
        return g.NavWindow && IsWindowChildOf(g.NavWindow, g.CurrentWindow);
    default:
        return g.NavWindow == g.CurrentWindow;
    }
}

// Can we focus this window with CTRL+TAB (or PadMenu + PadFocusPrev/PadFocusNext)
// Note that NoNavFocus makes the window not reachable with CTRL+TAB but it can still be focused with mouse or programmatically.
// If you want a window to never be focused, you may use the e.g. NoInputs flag.
bool ImGui::IsWindowNavFocusable(ImGuiWindow* window)
{
    return window->Active && window == window->RootWindow && !(window->Flags & ImGuiWindowFlags_NoNavFocus);
}

float ImGui::GetWindowWidth()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Size.x;
}

float ImGui::GetWindowHeight()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Size.y;
}

ImVec2 ImGui::GetWindowPos()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    return window->Pos;
}

void ImGui::SetWindowPos(ImGuiWindow* window, const ImVec2& pos, ImGuiCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if (cond && (window->SetWindowPosAllowFlags & cond) == 0)
        return;

    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    window->SetWindowPosAllowFlags &= ~(ImGuiCond_Once | ImGuiCond_FirstUseEver | ImGuiCond_Appearing);
    window->SetWindowPosVal = ImVec2(FLT_MAX, FLT_MAX);

    // Set
    const ImVec2 old_pos = window->Pos;
    window->Pos = ImFloor(pos);
    ImVec2 offset = window->Pos - old_pos;
    window->DC.CursorPos += offset;         // As we happen to move the window while it is being appended to (which is a bad idea - will smear) let's at least offset the cursor
    window->DC.CursorMaxPos += offset;      // And more importantly we need to offset CursorMaxPos/CursorStartPos this so ContentSize calculation doesn't get affected.
    window->DC.CursorStartPos += offset;
}

void ImGui::SetWindowPos(const ImVec2& pos, ImGuiCond cond)
{
    ImGuiWindow* window = GetCurrentWindowRead();
    SetWindowPos(window, pos, cond);
}

void ImGui::SetWindowPos(const char* name, const ImVec2& pos, ImGuiCond cond)
{
    if (ImGuiWindow* window = FindWindowByName(name))
        SetWindowPos(window, pos, cond);
}

ImVec2 ImGui::GetWindowSize()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->Size;
}

void ImGui::SetWindowSize(ImGuiWindow* window, const ImVec2& size, ImGuiCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if (cond && (window->SetWindowSizeAllowFlags & cond) == 0)
        return;

    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    window->SetWindowSizeAllowFlags &= ~(ImGuiCond_Once | ImGuiCond_FirstUseEver | ImGuiCond_Appearing);

    // Set
    if (size.x > 0.0f)
    {
        window->AutoFitFramesX = 0;
        window->SizeFull.x = IM_FLOOR(size.x);
    } else
    {
        window->AutoFitFramesX = 2;
        window->AutoFitOnlyGrows = false;
    }
    if (size.y > 0.0f)
    {
        window->AutoFitFramesY = 0;
        window->SizeFull.y = IM_FLOOR(size.y);
    } else
    {
        window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = false;
    }
}

void ImGui::SetWindowSize(const ImVec2& size, ImGuiCond cond)
{
    SetWindowSize(GImGui->CurrentWindow, size, cond);
}

void ImGui::SetWindowSize(const char* name, const ImVec2& size, ImGuiCond cond)
{
    if (ImGuiWindow* window = FindWindowByName(name))
        SetWindowSize(window, size, cond);
}

void ImGui::SetWindowCollapsed(ImGuiWindow* window, bool collapsed, ImGuiCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if (cond && (window->SetWindowCollapsedAllowFlags & cond) == 0)
        return;
    window->SetWindowCollapsedAllowFlags &= ~(ImGuiCond_Once | ImGuiCond_FirstUseEver | ImGuiCond_Appearing);

    // Set
    window->Collapsed = collapsed;
}

void ImGui::SetWindowCollapsed(bool collapsed, ImGuiCond cond)
{
    SetWindowCollapsed(GImGui->CurrentWindow, collapsed, cond);
}

bool ImGui::IsWindowCollapsed()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->Collapsed;
}

bool ImGui::IsWindowAppearing()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->Appearing;
}

void ImGui::SetWindowCollapsed(const char* name, bool collapsed, ImGuiCond cond)
{
    if (ImGuiWindow* window = FindWindowByName(name))
        SetWindowCollapsed(window, collapsed, cond);
}

void ImGui::SetWindowFocus()
{
    FocusWindow(GImGui->CurrentWindow);
}

void ImGui::SetWindowFocus(const char* name)
{
    if (name)
    {
        if (ImGuiWindow* window = FindWindowByName(name))
            FocusWindow(window);
    } else
    {
        FocusWindow(NULL);
    }
}

void ImGui::SetNextWindowPos(const ImVec2& pos, ImGuiCond cond, const ImVec2& pivot)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasPos;
    g.NextWindowData.PosVal = pos;
    g.NextWindowData.PosPivotVal = pivot;
    g.NextWindowData.PosCond = cond ? cond : ImGuiCond_Always;
}

void ImGui::SetNextWindowSize(const ImVec2& size, ImGuiCond cond)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSize;
    g.NextWindowData.SizeVal = size;
    g.NextWindowData.SizeCond = cond ? cond : ImGuiCond_Always;
}

void ImGui::SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeCallback custom_callback, void* custom_callback_user_data)
{
    ImGuiContext& g = *GImGui;
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
    g.NextWindowData.SizeConstraintRect = ImRect(size_min, size_max);
    g.NextWindowData.SizeCallback = custom_callback;
    g.NextWindowData.SizeCallbackUserData = custom_callback_user_data;
}

// Content size = inner scrollable rectangle, padded with WindowPadding.
// SetNextWindowContentSize(ImVec2(100,100) + ImGuiWindowFlags_AlwaysAutoResize will always allow submitting a 100x100 item.
void ImGui::SetNextWindowContentSize(const ImVec2& size)
{
    ImGuiContext& g = *GImGui;
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasContentSize;
    g.NextWindowData.ContentSizeVal = size;
}

void ImGui::SetNextWindowScroll(const ImVec2& scroll)
{
    ImGuiContext& g = *GImGui;
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasScroll;
    g.NextWindowData.ScrollVal = scroll;
}

void ImGui::SetNextWindowCollapsed(bool collapsed, ImGuiCond cond)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasCollapsed;
    g.NextWindowData.CollapsedVal = collapsed;
    g.NextWindowData.CollapsedCond = cond ? cond : ImGuiCond_Always;
}

void ImGui::SetNextWindowFocus()
{
    ImGuiContext& g = *GImGui;
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasFocus;
}

void ImGui::SetNextWindowBgAlpha(float alpha)
{
    ImGuiContext& g = *GImGui;
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasBgAlpha;
    g.NextWindowData.BgAlphaVal = alpha;
}

ImDrawList* ImGui::GetWindowDrawList()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DrawList;
}

ImFont* ImGui::GetFont()
{
    return GImGui->Font;
}

float ImGui::GetFontSize()
{
    return GImGui->FontSize;
}

ImVec2 ImGui::GetFontTexUvWhitePixel()
{
    return GImGui->DrawListSharedData.TexUvWhitePixel;
}

void ImGui::SetWindowFontScale(float scale)
{
    IM_ASSERT(scale > 0.0f);
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->FontWindowScale = scale;
    g.FontSize = g.DrawListSharedData.FontSize = window->CalcFontSize();
}

void ImGui::ActivateItem(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    g.NavNextActivateId = id;
}

// Note: this is storing in same stack as IDStack, so Push/Pop mismatch will be reported there.
void ImGui::PushFocusScope(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    window->IDStack.push_back(window->DC.NavFocusScopeIdCurrent);
    window->DC.NavFocusScopeIdCurrent = id;
}

void ImGui::PopFocusScope()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    window->DC.NavFocusScopeIdCurrent = window->IDStack.back();
    window->IDStack.pop_back();
}

void ImGui::SetKeyboardFocusHere(int offset)
{
    IM_ASSERT(offset >= -1);    // -1 is allowed but not below
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    g.FocusRequestNextWindow = window;
    g.FocusRequestNextCounterRegular = window->DC.FocusCounterRegular + 1 + offset;
    g.FocusRequestNextCounterTabStop = INT_MAX;
}

void ImGui::SetItemDefaultFocus()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (!window->Appearing)
        return;
    if (g.NavWindow == window->RootWindowForNav && (g.NavInitRequest || g.NavInitResultId != 0) && g.NavLayer == g.NavWindow->DC.NavLayerCurrent)
    {
        g.NavInitRequest = false;
        g.NavInitResultId = g.NavWindow->DC.LastItemId;
        g.NavInitResultRectRel = ImRect(g.NavWindow->DC.LastItemRect.Min - g.NavWindow->Pos, g.NavWindow->DC.LastItemRect.Max - g.NavWindow->Pos);
        NavUpdateAnyRequestFlag();
        if (!IsItemVisible())
            SetScrollHereY();
    }
}

void ImGui::SetStateStorage(ImGuiStorage* tree)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    window->DC.StateStorage = tree ? tree : &window->StateStorage;
}

ImGuiStorage* ImGui::GetStateStorage()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->DC.StateStorage;
}

void ImGui::PushID(const char* str_id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetIDNoKeepAlive(str_id);
    window->IDStack.push_back(id);
}

void ImGui::PushID(const char* str_id_begin, const char* str_id_end)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetIDNoKeepAlive(str_id_begin, str_id_end);
    window->IDStack.push_back(id);
}

void ImGui::PushID(const void* ptr_id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetIDNoKeepAlive(ptr_id);
    window->IDStack.push_back(id);
}

void ImGui::PushID(int int_id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetIDNoKeepAlive(int_id);
    window->IDStack.push_back(id);
}

// Push a given id value ignoring the ID stack as a seed.
void ImGui::PushOverrideID(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    window->IDStack.push_back(id);
}

void ImGui::PopID()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    window->IDStack.pop_back();
}

ImGuiID ImGui::GetID(const char* str_id)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->GetID(str_id);
}

ImGuiID ImGui::GetID(const char* str_id_begin, const char* str_id_end)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->GetID(str_id_begin, str_id_end);
}

ImGuiID ImGui::GetID(const void* ptr_id)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->GetID(ptr_id);
}

bool ImGui::IsRectVisible(const ImVec2& size)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ClipRect.Overlaps(ImRect(window->DC.CursorPos, window->DC.CursorPos + size));
}

bool ImGui::IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ClipRect.Overlaps(ImRect(rect_min, rect_max));
}


//-----------------------------------------------------------------------------
// [SECTION] ERROR CHECKING
//-----------------------------------------------------------------------------

// Helper function to verify ABI compatibility between caller code and compiled version of Dear ImGui.
// Verify that the type sizes are matching between the calling file's compilation unit and imgui.cpp's compilation unit
// If the user has inconsistent compilation settings, imgui configuration #define, packing pragma, etc. your user code
// may see different structures than what imgui.cpp sees, which is problematic.
// We usually require settings to be in imconfig.h to make sure that they are accessible to all compilation units involved with Dear ImGui.
bool ImGui::DebugCheckVersionAndDataLayout(const char* version, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_vert, size_t sz_idx)
{
    bool error = false;
    if (strcmp(version, IMGUI_VERSION) != 0) { error = true; IM_ASSERT(strcmp(version, IMGUI_VERSION) == 0 && "Mismatched version string!"); }
    if (sz_io != sizeof(ImGuiIO)) { error = true; IM_ASSERT(sz_io == sizeof(ImGuiIO) && "Mismatched struct layout!"); }
    if (sz_style != sizeof(ImGuiStyle)) { error = true; IM_ASSERT(sz_style == sizeof(ImGuiStyle) && "Mismatched struct layout!"); }
    if (sz_vec2 != sizeof(ImVec2)) { error = true; IM_ASSERT(sz_vec2 == sizeof(ImVec2) && "Mismatched struct layout!"); }
    if (sz_vec4 != sizeof(ImVec4)) { error = true; IM_ASSERT(sz_vec4 == sizeof(ImVec4) && "Mismatched struct layout!"); }
    if (sz_vert != sizeof(ImDrawVert)) { error = true; IM_ASSERT(sz_vert == sizeof(ImDrawVert) && "Mismatched struct layout!"); }
    if (sz_idx != sizeof(ImDrawIdx)) { error = true; IM_ASSERT(sz_idx == sizeof(ImDrawIdx) && "Mismatched struct layout!"); }
    return !error;
}

static void ImGui::ErrorCheckNewFrameSanityChecks()
{
    ImGuiContext& g = *GImGui;

    // Check user IM_ASSERT macro
    // (IF YOU GET A WARNING OR COMPILE ERROR HERE: it means you assert macro is incorrectly defined!
    //  If your macro uses multiple statements, it NEEDS to be surrounded by a 'do { ... } while (0)' block.
    //  This is a common C/C++ idiom to allow multiple statements macros to be used in control flow blocks.)
    // #define IM_ASSERT(EXPR)   SomeCode(EXPR); SomeMoreCode();                    // Wrong!
    // #define IM_ASSERT(EXPR)   do { SomeCode(EXPR); SomeMoreCode(); } while (0)   // Correct!
    if (true) IM_ASSERT(1); else IM_ASSERT(0);

    // Check user data
    // (We pass an error message in the assert expression to make it visible to programmers who are not using a debugger, as most assert handlers display their argument)
    IM_ASSERT(g.Initialized);
    IM_ASSERT((g.IO.DeltaTime > 0.0f || g.FrameCount == 0) && "Need a positive DeltaTime!");
    IM_ASSERT((g.FrameCount == 0 || g.FrameCountEnded == g.FrameCount) && "Forgot to call Render() or EndFrame() at the end of the previous frame?");
    IM_ASSERT(g.IO.DisplaySize.x >= 0.0f && g.IO.DisplaySize.y >= 0.0f && "Invalid DisplaySize value!");
    IM_ASSERT(g.IO.Fonts->Fonts.Size > 0 && "Font Atlas not built. Did you call io.Fonts->GetTexDataAsRGBA32() / GetTexDataAsAlpha8() ?");
    IM_ASSERT(g.IO.Fonts->Fonts[0]->IsLoaded() && "Font Atlas not built. Did you call io.Fonts->GetTexDataAsRGBA32() / GetTexDataAsAlpha8() ?");
    IM_ASSERT(g.Style.CurveTessellationTol > 0.0f && "Invalid style setting!");
    IM_ASSERT(g.Style.CircleSegmentMaxError > 0.0f && "Invalid style setting!");
    IM_ASSERT(g.Style.Alpha >= 0.0f && g.Style.Alpha <= 1.0f && "Invalid style setting. Alpha cannot be negative (allows us to avoid a few clamps in color computations)!");
    IM_ASSERT(g.Style.WindowMinSize.x >= 1.0f && g.Style.WindowMinSize.y >= 1.0f && "Invalid style setting.");
    IM_ASSERT(g.Style.WindowMenuButtonPosition == ImGuiDir_None || g.Style.WindowMenuButtonPosition == ImGuiDir_Left || g.Style.WindowMenuButtonPosition == ImGuiDir_Right);
    for (int n = 0; n < ImGuiKey_COUNT; n++)
        IM_ASSERT(g.IO.KeyMap[n] >= -1 && g.IO.KeyMap[n] < IM_ARRAYSIZE(g.IO.KeysDown) && "io.KeyMap[] contains an out of bound value (need to be 0..512, or -1 for unmapped key)");

    // Perform simple check: required key mapping (we intentionally do NOT check all keys to not pressure user into setting up everything, but Space is required and was only recently added in 1.60 WIP)
    if (g.IO.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)
        IM_ASSERT(g.IO.KeyMap[ImGuiKey_Space] != -1 && "ImGuiKey_Space is not mapped, required for keyboard navigation.");

    // Perform simple check: the beta io.ConfigWindowsResizeFromEdges option requires back-end to honor mouse cursor changes and set the ImGuiBackendFlags_HasMouseCursors flag accordingly.
    if (g.IO.ConfigWindowsResizeFromEdges && !(g.IO.BackendFlags & ImGuiBackendFlags_HasMouseCursors))
        g.IO.ConfigWindowsResizeFromEdges = false;
}

static void ImGui::ErrorCheckEndFrameSanityChecks()
{
    ImGuiContext& g = *GImGui;

    // Verify that io.KeyXXX fields haven't been tampered with. Key mods should not be modified between NewFrame() and EndFrame()
    // One possible reason leading to this assert is that your back-ends update inputs _AFTER_ NewFrame().
    const ImGuiKeyModFlags expected_key_mod_flags = GetMergedKeyModFlags();
    IM_ASSERT(g.IO.KeyMods == expected_key_mod_flags && "Mismatching io.KeyCtrl/io.KeyShift/io.KeyAlt/io.KeySuper vs io.KeyMods");
    IM_UNUSED(expected_key_mod_flags);

    // Report when there is a mismatch of Begin/BeginChild vs End/EndChild calls. Important: Remember that the Begin/BeginChild API requires you
    // to always call End/EndChild even if Begin/BeginChild returns false! (this is unfortunately inconsistent with most other Begin* API).
    if (g.CurrentWindowStack.Size != 1)
    {
        if (g.CurrentWindowStack.Size > 1)
        {
            IM_ASSERT_USER_ERROR(g.CurrentWindowStack.Size == 1, "Mismatched Begin/BeginChild vs End/EndChild calls: did you forget to call End/EndChild?");
            while (g.CurrentWindowStack.Size > 1)
                End();
        } else
        {
            IM_ASSERT_USER_ERROR(g.CurrentWindowStack.Size == 1, "Mismatched Begin/BeginChild vs End/EndChild calls: did you call End/EndChild too much?");
        }
    }
}

// Save and compare stack sizes on Begin()/End() to detect usage errors
// Begin() calls this with write=true
// End() calls this with write=false
static void ImGui::ErrorCheckBeginEndCompareStacksSize(ImGuiWindow* window, bool write)
{
    ImGuiContext& g = *GImGui;
    short* p = &window->DC.StackSizesBackup[0];

    // Window stacks
    // NOT checking: DC.ItemWidth, DC.AllowKeyboardFocus, DC.ButtonRepeat, DC.TextWrapPos (per window) to allow user to conveniently push once and not pop (they are cleared on Begin)
    { int n = window->IDStack.Size;       if (write) *p = (short)n; else IM_ASSERT(*p == n && "PushID/PopID or TreeNode/TreePop Mismatch!");   p++; }    // Too few or too many PopID()/TreePop()
    { int n = window->DC.GroupStack.Size; if (write) *p = (short)n; else IM_ASSERT(*p == n && "BeginGroup/EndGroup Mismatch!");                p++; }    // Too few or too many EndGroup()

    // Global stacks
    // For color, style and font stacks there is an incentive to use Push/Begin/Pop/.../End patterns, so we relax our checks a little to allow them.
    { int n = g.BeginPopupStack.Size;     if (write) *p = (short)n; else IM_ASSERT(*p == n && "BeginMenu/EndMenu or BeginPopup/EndPopup Mismatch!"); p++; }// Too few or too many EndMenu()/EndPopup()
    { int n = g.ColorModifiers.Size;      if (write) *p = (short)n; else IM_ASSERT(*p >= n && "PushStyleColor/PopStyleColor Mismatch!");       p++; }    // Too few or too many PopStyleColor()
    { int n = g.StyleModifiers.Size;      if (write) *p = (short)n; else IM_ASSERT(*p >= n && "PushStyleVar/PopStyleVar Mismatch!");           p++; }    // Too few or too many PopStyleVar()
    { int n = g.FontStack.Size;           if (write) *p = (short)n; else IM_ASSERT(*p >= n && "PushFont/PopFont Mismatch!");                   p++; }    // Too few or too many PopFont()
    IM_ASSERT(p == window->DC.StackSizesBackup + IM_ARRAYSIZE(window->DC.StackSizesBackup));
}


//-----------------------------------------------------------------------------
// [SECTION] LAYOUT
//-----------------------------------------------------------------------------
// - ItemSize()
// - ItemAdd()
// - SameLine()
// - GetCursorScreenPos()
// - SetCursorScreenPos()
// - GetCursorPos(), GetCursorPosX(), GetCursorPosY()
// - SetCursorPos(), SetCursorPosX(), SetCursorPosY()
// - GetCursorStartPos()
// - Indent()
// - Unindent()
// - SetNextItemWidth()
// - PushItemWidth()
// - PushMultiItemsWidths()
// - PopItemWidth()
// - CalcItemWidth()
// - CalcItemSize()
// - GetTextLineHeight()
// - GetTextLineHeightWithSpacing()
// - GetFrameHeight()
// - GetFrameHeightWithSpacing()
// - GetContentRegionMax()
// - GetContentRegionMaxAbs() [Internal]
// - GetContentRegionAvail(),
// - GetWindowContentRegionMin(), GetWindowContentRegionMax()
// - GetWindowContentRegionWidth()
// - BeginGroup()
// - EndGroup()
// Also see in imgui_widgets: tab bars, columns.
//-----------------------------------------------------------------------------

// Advance cursor given item size for layout.
// Register minimum needed size so it can extend the bounding box used for auto-fit calculation.
// See comments in ItemAdd() about how/why the size provided to ItemSize() vs ItemAdd() may often different.
void ImGui::ItemSize(const ImVec2& size, float text_baseline_y)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    // We increase the height in this function to accommodate for baseline offset.
    // In theory we should be offsetting the starting position (window->DC.CursorPos), that will be the topic of a larger refactor,
    // but since ItemSize() is not yet an API that moves the cursor (to handle e.g. wrapping) enlarging the height has the same effect.
    const float offset_to_match_baseline_y = (text_baseline_y >= 0) ? ImMax(0.0f, window->DC.CurrLineTextBaseOffset - text_baseline_y) : 0.0f;
    const float line_height = ImMax(window->DC.CurrLineSize.y, size.y + offset_to_match_baseline_y);

    // Always align ourselves on pixel boundaries
    //if (g.IO.KeyAlt) window->DrawList->AddRect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(size.x, line_height), IM_COL32(255,0,0,200)); // [DEBUG]
    window->DC.CursorPosPrevLine.x = window->DC.CursorPos.x + size.x;
    window->DC.CursorPosPrevLine.y = window->DC.CursorPos.y;
    window->DC.CursorPos.x = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);    // Next line
    window->DC.CursorPos.y = IM_FLOOR(window->DC.CursorPos.y + line_height + g.Style.ItemSpacing.y);        // Next line
    window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPosPrevLine.x);
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y - g.Style.ItemSpacing.y);
    //if (g.IO.KeyAlt) window->DrawList->AddCircle(window->DC.CursorMaxPos, 3.0f, IM_COL32(255,0,0,255), 4); // [DEBUG]

    window->DC.PrevLineSize.y = line_height;
    window->DC.CurrLineSize.y = 0.0f;
    window->DC.PrevLineTextBaseOffset = ImMax(window->DC.CurrLineTextBaseOffset, text_baseline_y);
    window->DC.CurrLineTextBaseOffset = 0.0f;

    // Horizontal layout mode
    if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
        SameLine();
}

void ImGui::ItemSize(const ImRect& bb, float text_baseline_y)
{
    ItemSize(bb.GetSize(), text_baseline_y);
}

// Declare item bounding box for clipping and interaction.
// Note that the size can be different than the one provided to ItemSize(). Typically, widgets that spread over available surface
// declare their minimum size requirement to ItemSize() and provide a larger region to ItemAdd() which is used drawing/interaction.
bool ImGui::ItemAdd(const ImRect& bb, ImGuiID id, const ImRect* nav_bb_arg)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    if (id != 0)
    {
        // Navigation processing runs prior to clipping early-out
        //  (a) So that NavInitRequest can be honored, for newly opened windows to select a default widget
        //  (b) So that we can scroll up/down past clipped items. This adds a small O(N) cost to regular navigation requests
        //      unfortunately, but it is still limited to one window. It may not scale very well for windows with ten of
        //      thousands of item, but at least NavMoveRequest is only set on user interaction, aka maximum once a frame.
        //      We could early out with "if (is_clipped && !g.NavInitRequest) return false;" but when we wouldn't be able
        //      to reach unclipped widgets. This would work if user had explicit scrolling control (e.g. mapped on a stick).
        // We intentionally don't check if g.NavWindow != NULL because g.NavAnyRequest should only be set when it is non null.
        // If we crash on a NULL g.NavWindow we need to fix the bug elsewhere.
        window->DC.NavLayerActiveMaskNext |= window->DC.NavLayerCurrentMask;
        if (g.NavId == id || g.NavAnyRequest)
            if (g.NavWindow->RootWindowForNav == window->RootWindowForNav)
                if (window == g.NavWindow || ((window->Flags | g.NavWindow->Flags) & ImGuiWindowFlags_NavFlattened))
                    NavProcessItem(window, nav_bb_arg ? *nav_bb_arg : bb, id);

        // [DEBUG] Item Picker tool, when enabling the "extended" version we perform the check in ItemAdd()
#ifdef IMGUI_DEBUG_TOOL_ITEM_PICKER_EX
        if (id == g.DebugItemPickerBreakId)
        {
            IM_DEBUG_BREAK();
            g.DebugItemPickerBreakId = 0;
        }
#endif
    }

    window->DC.LastItemId = id;
    window->DC.LastItemRect = bb;
    window->DC.LastItemStatusFlags = ImGuiItemStatusFlags_None;
    g.NextItemData.Flags = ImGuiNextItemDataFlags_None;

#ifdef IMGUI_ENABLE_TEST_ENGINE
    if (id != 0)
        IMGUI_TEST_ENGINE_ITEM_ADD(nav_bb_arg ? *nav_bb_arg : bb, id);
#endif

    // Clipping test
    const bool is_clipped = IsClippedEx(bb, id, false);
    if (is_clipped)
        return false;
    //if (g.IO.KeyAlt) window->DrawList->AddRect(bb.Min, bb.Max, IM_COL32(255,255,0,120)); // [DEBUG]

    // We need to calculate this now to take account of the current clipping rectangle (as items like Selectable may change them)
    if (IsMouseHoveringRect(bb.Min, bb.Max))
        window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_HoveredRect;
    return true;
}

// Gets back to previous line and continue with horizontal layout
//      offset_from_start_x == 0 : follow right after previous item
//      offset_from_start_x != 0 : align to specified x position (relative to window/group left)
//      spacing_w < 0            : use default spacing if pos_x == 0, no spacing if pos_x != 0
//      spacing_w >= 0           : enforce spacing amount
void ImGui::SameLine(float offset_from_start_x, float spacing_w)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    if (offset_from_start_x != 0.0f)
    {
        if (spacing_w < 0.0f) spacing_w = 0.0f;
        window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + offset_from_start_x + spacing_w + window->DC.GroupOffset.x + window->DC.ColumnsOffset.x;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    } else
    {
        if (spacing_w < 0.0f) spacing_w = g.Style.ItemSpacing.x;
        window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + spacing_w;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    window->DC.CurrLineSize = window->DC.PrevLineSize;
    window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
}

ImVec2 ImGui::GetCursorScreenPos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos;
}

void ImGui::SetCursorScreenPos(const ImVec2& pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos = pos;
    window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}

// User generally sees positions in window coordinates. Internally we store CursorPos in absolute screen coordinates because it is more convenient.
// Conversion happens as we pass the value to user, but it makes our naming convention confusing because GetCursorPos() == (DC.CursorPos - window.Pos). May want to rename 'DC.CursorPos'.
ImVec2 ImGui::GetCursorPos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos - window->Pos + window->Scroll;
}

float ImGui::GetCursorPosX()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos.x - window->Pos.x + window->Scroll.x;
}

float ImGui::GetCursorPosY()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos.y - window->Pos.y + window->Scroll.y;
}

void ImGui::SetCursorPos(const ImVec2& local_pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos = window->Pos - window->Scroll + local_pos;
    window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}

void ImGui::SetCursorPosX(float x)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + x;
    window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPos.x);
}

void ImGui::SetCursorPosY(float y)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.y = window->Pos.y - window->Scroll.y + y;
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y);
}

ImVec2 ImGui::GetCursorStartPos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorStartPos - window->Pos;
}

void ImGui::Indent(float indent_w)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.Indent.x += (indent_w != 0.0f) ? indent_w : g.Style.IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x;
}

void ImGui::Unindent(float indent_w)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.Indent.x -= (indent_w != 0.0f) ? indent_w : g.Style.IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x;
}

// Affect large frame+labels widgets only.
void ImGui::SetNextItemWidth(float item_width)
{
    ImGuiContext& g = *GImGui;
    g.NextItemData.Flags |= ImGuiNextItemDataFlags_HasWidth;
    g.NextItemData.Width = item_width;
}

void ImGui::PushItemWidth(float item_width)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    window->DC.ItemWidth = (item_width == 0.0f ? window->ItemWidthDefault : item_width);
    window->DC.ItemWidthStack.push_back(window->DC.ItemWidth);
    g.NextItemData.Flags &= ~ImGuiNextItemDataFlags_HasWidth;
}

void ImGui::PushMultiItemsWidths(int components, float w_full)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    const ImGuiStyle& style = g.Style;
    const float w_item_one = ImMax(1.0f, IM_FLOOR((w_full - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
    const float w_item_last = ImMax(1.0f, IM_FLOOR(w_full - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));
    window->DC.ItemWidthStack.push_back(w_item_last);
    for (int i = 0; i < components - 1; i++)
        window->DC.ItemWidthStack.push_back(w_item_one);
    window->DC.ItemWidth = window->DC.ItemWidthStack.back();
    g.NextItemData.Flags &= ~ImGuiNextItemDataFlags_HasWidth;
}

void ImGui::PopItemWidth()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ItemWidthStack.pop_back();
    window->DC.ItemWidth = window->DC.ItemWidthStack.empty() ? window->ItemWidthDefault : window->DC.ItemWidthStack.back();
}

// Calculate default item width given value passed to PushItemWidth() or SetNextItemWidth().
// The SetNextItemWidth() data is generally cleared/consumed by ItemAdd() or NextItemData.ClearFlags()
float ImGui::CalcItemWidth()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    float w;
    if (g.NextItemData.Flags & ImGuiNextItemDataFlags_HasWidth)
        w = g.NextItemData.Width;
    else
        w = window->DC.ItemWidth;
    if (w < 0.0f)
    {
        float region_max_x = GetContentRegionMaxAbs().x;
        w = ImMax(1.0f, region_max_x - window->DC.CursorPos.x + w);
    }
    w = IM_FLOOR(w);
    return w;
}

// [Internal] Calculate full item size given user provided 'size' parameter and default width/height. Default width is often == CalcItemWidth().
// Those two functions CalcItemWidth vs CalcItemSize are awkwardly named because they are not fully symmetrical.
// Note that only CalcItemWidth() is publicly exposed.
// The 4.0f here may be changed to match CalcItemWidth() and/or BeginChild() (right now we have a mismatch which is harmless but undesirable)
ImVec2 ImGui::CalcItemSize(ImVec2 size, float default_w, float default_h)
{
    ImGuiWindow* window = GImGui->CurrentWindow;

    ImVec2 region_max;
    if (size.x < 0.0f || size.y < 0.0f)
        region_max = GetContentRegionMaxAbs();

    if (size.x == 0.0f)
        size.x = default_w;
    else if (size.x < 0.0f)
        size.x = ImMax(4.0f, region_max.x - window->DC.CursorPos.x + size.x);

    if (size.y == 0.0f)
        size.y = default_h;
    else if (size.y < 0.0f)
        size.y = ImMax(4.0f, region_max.y - window->DC.CursorPos.y + size.y);

    return size;
}

float ImGui::GetTextLineHeight()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize;
}

float ImGui::GetTextLineHeightWithSpacing()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize + g.Style.ItemSpacing.y;
}

float ImGui::GetFrameHeight()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize + g.Style.FramePadding.y * 2.0f;
}

float ImGui::GetFrameHeightWithSpacing()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize + g.Style.FramePadding.y * 2.0f + g.Style.ItemSpacing.y;
}

// FIXME: All the Contents Region function are messy or misleading. WE WILL AIM TO OBSOLETE ALL OF THEM WITH A NEW "WORK RECT" API. Thanks for your patience!

// FIXME: This is in window space (not screen space!).
ImVec2 ImGui::GetContentRegionMax()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImVec2 mx = window->ContentRegionRect.Max - window->Pos;
    if (window->DC.CurrentColumns)
        mx.x = window->WorkRect.Max.x - window->Pos.x;
    return mx;
}

// [Internal] Absolute coordinate. Saner. This is not exposed until we finishing refactoring work rect features.
ImVec2 ImGui::GetContentRegionMaxAbs()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImVec2 mx = window->ContentRegionRect.Max;
    if (window->DC.CurrentColumns)
        mx.x = window->WorkRect.Max.x;
    return mx;
}

ImVec2 ImGui::GetContentRegionAvail()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return GetContentRegionMaxAbs() - window->DC.CursorPos;
}

// In window space (not screen space!)
ImVec2 ImGui::GetWindowContentRegionMin()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ContentRegionRect.Min - window->Pos;
}

ImVec2 ImGui::GetWindowContentRegionMax()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ContentRegionRect.Max - window->Pos;
}

float ImGui::GetWindowContentRegionWidth()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ContentRegionRect.GetWidth();
}

// Lock horizontal starting position + capture group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
void ImGui::BeginGroup()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    window->DC.GroupStack.resize(window->DC.GroupStack.Size + 1);
    ImGuiGroupData& group_data = window->DC.GroupStack.back();
    group_data.BackupCursorPos = window->DC.CursorPos;
    group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
    group_data.BackupIndent = window->DC.Indent;
    group_data.BackupGroupOffset = window->DC.GroupOffset;
    group_data.BackupCurrLineSize = window->DC.CurrLineSize;
    group_data.BackupCurrLineTextBaseOffset = window->DC.CurrLineTextBaseOffset;
    group_data.BackupActiveIdIsAlive = g.ActiveIdIsAlive;
    group_data.BackupActiveIdPreviousFrameIsAlive = g.ActiveIdPreviousFrameIsAlive;
    group_data.EmitItem = true;

    window->DC.GroupOffset.x = window->DC.CursorPos.x - window->Pos.x - window->DC.ColumnsOffset.x;
    window->DC.Indent = window->DC.GroupOffset;
    window->DC.CursorMaxPos = window->DC.CursorPos;
    window->DC.CurrLineSize = ImVec2(0.0f, 0.0f);
    if (g.LogEnabled)
        g.LogLinePosY = -FLT_MAX; // To enforce Log carriage return
}

void ImGui::EndGroup()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(!window->DC.GroupStack.empty());  // Mismatched BeginGroup()/EndGroup() calls

    ImGuiGroupData& group_data = window->DC.GroupStack.back();

    ImRect group_bb(group_data.BackupCursorPos, ImMax(window->DC.CursorMaxPos, group_data.BackupCursorPos));

    window->DC.CursorPos = group_data.BackupCursorPos;
    window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
    window->DC.Indent = group_data.BackupIndent;
    window->DC.GroupOffset = group_data.BackupGroupOffset;
    window->DC.CurrLineSize = group_data.BackupCurrLineSize;
    window->DC.CurrLineTextBaseOffset = group_data.BackupCurrLineTextBaseOffset;
    if (g.LogEnabled)
        g.LogLinePosY = -FLT_MAX; // To enforce Log carriage return

    if (!group_data.EmitItem)
    {
        window->DC.GroupStack.pop_back();
        return;
    }

    window->DC.CurrLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrLineTextBaseOffset);      // FIXME: Incorrect, we should grab the base offset from the *first line* of the group but it is hard to obtain now.
    ItemSize(group_bb.GetSize());
    ItemAdd(group_bb, 0);

    // If the current ActiveId was declared within the boundary of our group, we copy it to LastItemId so IsItemActive(), IsItemDeactivated() etc. will be functional on the entire group.
    // It would be be neater if we replaced window.DC.LastItemId by e.g. 'bool LastItemIsActive', but would put a little more burden on individual widgets.
    // Also if you grep for LastItemId you'll notice it is only used in that context.
    // (The tests not symmetrical because ActiveIdIsAlive is an ID itself, in order to be able to handle ActiveId being overwritten during the frame.)
    const bool group_contains_curr_active_id = (group_data.BackupActiveIdIsAlive != g.ActiveId) && (g.ActiveIdIsAlive == g.ActiveId) && g.ActiveId;
    const bool group_contains_prev_active_id = !group_data.BackupActiveIdPreviousFrameIsAlive && g.ActiveIdPreviousFrameIsAlive;
    if (group_contains_curr_active_id)
        window->DC.LastItemId = g.ActiveId;
    else if (group_contains_prev_active_id)
        window->DC.LastItemId = g.ActiveIdPreviousFrame;
    window->DC.LastItemRect = group_bb;

    // Forward Edited flag
    if (group_contains_curr_active_id && g.ActiveIdHasBeenEditedThisFrame)
        window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_Edited;

    // Forward Deactivated flag
    window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_HasDeactivated;
    if (group_contains_prev_active_id && g.ActiveId != g.ActiveIdPreviousFrame)
        window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_Deactivated;

    window->DC.GroupStack.pop_back();
    //window->DrawList->AddRect(group_bb.Min, group_bb.Max, IM_COL32(255,0,255,255));   // [Debug]
}


//-----------------------------------------------------------------------------
// [SECTION] SCROLLING
//-----------------------------------------------------------------------------

static ImVec2 CalcNextScrollFromScrollTargetAndClamp(ImGuiWindow* window, bool snap_on_edges)
{
    ImGuiContext& g = *GImGui;
    ImVec2 scroll = window->Scroll;
    if (window->ScrollTarget.x < FLT_MAX)
    {
        float cr_x = window->ScrollTargetCenterRatio.x;
        float target_x = window->ScrollTarget.x;
        if (snap_on_edges && cr_x <= 0.0f && target_x <= window->WindowPadding.x)
            target_x = 0.0f;
        else if (snap_on_edges && cr_x >= 1.0f && target_x >= window->ContentSize.x + window->WindowPadding.x + g.Style.ItemSpacing.x)
            target_x = window->ContentSize.x + window->WindowPadding.x * 2.0f;
        scroll.x = target_x - cr_x * (window->SizeFull.x - window->ScrollbarSizes.x);
    }
    if (window->ScrollTarget.y < FLT_MAX)
    {
        // 'snap_on_edges' allows for a discontinuity at the edge of scrolling limits to take account of WindowPadding so that scrolling to make the last item visible scroll far enough to see the padding.
        float decoration_up_height = window->TitleBarHeight() + window->MenuBarHeight();
        float cr_y = window->ScrollTargetCenterRatio.y;
        float target_y = window->ScrollTarget.y;
        if (snap_on_edges && cr_y <= 0.0f && target_y <= window->WindowPadding.y)
            target_y = 0.0f;
        if (snap_on_edges && cr_y >= 1.0f && target_y >= window->ContentSize.y + window->WindowPadding.y + g.Style.ItemSpacing.y)
            target_y = window->ContentSize.y + window->WindowPadding.y * 2.0f;
        scroll.y = target_y - cr_y * (window->SizeFull.y - window->ScrollbarSizes.y - decoration_up_height);
    }
    scroll.x = IM_FLOOR(ImMax(scroll.x, 0.0f));
    scroll.y = IM_FLOOR(ImMax(scroll.y, 0.0f));
    if (!window->Collapsed && !window->SkipItems)
    {
        scroll.x = ImMin(scroll.x, window->ScrollMax.x);
        scroll.y = ImMin(scroll.y, window->ScrollMax.y);
    }
    return scroll;
}

// Scroll to keep newly navigated item fully into view
ImVec2 ImGui::ScrollToBringRectIntoView(ImGuiWindow* window, const ImRect& item_rect)
{
    ImGuiContext& g = *GImGui;
    ImRect window_rect(window->InnerRect.Min - ImVec2(1, 1), window->InnerRect.Max + ImVec2(1, 1));
    //GetForegroundDrawList(window)->AddRect(window_rect.Min, window_rect.Max, IM_COL32_WHITE); // [DEBUG]

    ImVec2 delta_scroll;
    if (!window_rect.Contains(item_rect))
    {
        if (window->ScrollbarX && item_rect.Min.x < window_rect.Min.x)
            SetScrollFromPosX(window, item_rect.Min.x - window->Pos.x + g.Style.ItemSpacing.x, 0.0f);
        else if (window->ScrollbarX && item_rect.Max.x >= window_rect.Max.x)
            SetScrollFromPosX(window, item_rect.Max.x - window->Pos.x + g.Style.ItemSpacing.x, 1.0f);
        if (item_rect.Min.y < window_rect.Min.y)
            SetScrollFromPosY(window, item_rect.Min.y - window->Pos.y - g.Style.ItemSpacing.y, 0.0f);
        else if (item_rect.Max.y >= window_rect.Max.y)
            SetScrollFromPosY(window, item_rect.Max.y - window->Pos.y + g.Style.ItemSpacing.y, 1.0f);

        ImVec2 next_scroll = CalcNextScrollFromScrollTargetAndClamp(window, false);
        delta_scroll = next_scroll - window->Scroll;
    }

    // Also scroll parent window to keep us into view if necessary
    if (window->Flags & ImGuiWindowFlags_ChildWindow)
        delta_scroll += ScrollToBringRectIntoView(window->ParentWindow, ImRect(item_rect.Min - delta_scroll, item_rect.Max - delta_scroll));

    return delta_scroll;
}

float ImGui::GetScrollX()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Scroll.x;
}

float ImGui::GetScrollY()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Scroll.y;
}

float ImGui::GetScrollMaxX()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ScrollMax.x;
}

float ImGui::GetScrollMaxY()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ScrollMax.y;
}

void ImGui::SetScrollX(float scroll_x)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->ScrollTarget.x = scroll_x;
    window->ScrollTargetCenterRatio.x = 0.0f;
}

void ImGui::SetScrollY(float scroll_y)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->ScrollTarget.y = scroll_y;
    window->ScrollTargetCenterRatio.y = 0.0f;
}

void ImGui::SetScrollX(ImGuiWindow* window, float new_scroll_x)
{
    window->ScrollTarget.x = new_scroll_x;
    window->ScrollTargetCenterRatio.x = 0.0f;
}

void ImGui::SetScrollY(ImGuiWindow* window, float new_scroll_y)
{
    window->ScrollTarget.y = new_scroll_y;
    window->ScrollTargetCenterRatio.y = 0.0f;
}


void ImGui::SetScrollFromPosX(ImGuiWindow* window, float local_x, float center_x_ratio)
{
    // We store a target position so centering can occur on the next frame when we are guaranteed to have a known window size
    IM_ASSERT(center_x_ratio >= 0.0f && center_x_ratio <= 1.0f);
    window->ScrollTarget.x = IM_FLOOR(local_x + window->Scroll.x);
    window->ScrollTargetCenterRatio.x = center_x_ratio;
}

void ImGui::SetScrollFromPosY(ImGuiWindow* window, float local_y, float center_y_ratio)
{
    // We store a target position so centering can occur on the next frame when we are guaranteed to have a known window size
    IM_ASSERT(center_y_ratio >= 0.0f && center_y_ratio <= 1.0f);
    const float decoration_up_height = window->TitleBarHeight() + window->MenuBarHeight();
    local_y -= decoration_up_height;
    window->ScrollTarget.y = IM_FLOOR(local_y + window->Scroll.y);
    window->ScrollTargetCenterRatio.y = center_y_ratio;
}

void ImGui::SetScrollFromPosX(float local_x, float center_x_ratio)
{
    ImGuiContext& g = *GImGui;
    SetScrollFromPosX(g.CurrentWindow, local_x, center_x_ratio);
}

void ImGui::SetScrollFromPosY(float local_y, float center_y_ratio)
{
    ImGuiContext& g = *GImGui;
    SetScrollFromPosY(g.CurrentWindow, local_y, center_y_ratio);
}

// center_x_ratio: 0.0f left of last item, 0.5f horizontal center of last item, 1.0f right of last item.
void ImGui::SetScrollHereX(float center_x_ratio)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    float target_x = window->DC.LastItemRect.Min.x - window->Pos.x; // Left of last item, in window space
    float last_item_width = window->DC.LastItemRect.GetWidth();
    target_x += (last_item_width * center_x_ratio) + (g.Style.ItemSpacing.x * (center_x_ratio - 0.5f) * 2.0f); // Precisely aim before, in the middle or after the last item.
    SetScrollFromPosX(target_x, center_x_ratio);
}

// center_y_ratio: 0.0f top of last item, 0.5f vertical center of last item, 1.0f bottom of last item.
void ImGui::SetScrollHereY(float center_y_ratio)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    float target_y = window->DC.CursorPosPrevLine.y - window->Pos.y; // Top of last item, in window space
    target_y += (window->DC.PrevLineSize.y * center_y_ratio) + (g.Style.ItemSpacing.y * (center_y_ratio - 0.5f) * 2.0f); // Precisely aim above, in the middle or below the last line.
    SetScrollFromPosY(target_y, center_y_ratio);
}

//-----------------------------------------------------------------------------
// [SECTION] TOOLTIPS
//-----------------------------------------------------------------------------

void ImGui::BeginTooltip()
{
    BeginTooltipEx(ImGuiWindowFlags_None, ImGuiTooltipFlags_None);
}

void ImGui::BeginTooltipEx(ImGuiWindowFlags extra_flags, ImGuiTooltipFlags tooltip_flags)
{
    ImGuiContext& g = *GImGui;

    if (g.DragDropWithinSource || g.DragDropWithinTarget)
    {
        // The default tooltip position is a little offset to give space to see the context menu (it's also clamped within the current viewport/monitor)
        // In the context of a dragging tooltip we try to reduce that offset and we enforce following the cursor.
        // Whatever we do we want to call SetNextWindowPos() to enforce a tooltip position and disable clipping the tooltip without our display area, like regular tooltip do.
        //ImVec2 tooltip_pos = g.IO.MousePos - g.ActiveIdClickOffset - g.Style.WindowPadding;
        ImVec2 tooltip_pos = g.IO.MousePos + ImVec2(16 * g.Style.MouseCursorScale, 8 * g.Style.MouseCursorScale);
        SetNextWindowPos(tooltip_pos);
        SetNextWindowBgAlpha(g.Style.Colors[ImGuiCol_PopupBg].w * 0.60f);
        //PushStyleVar(ImGuiStyleVar_Alpha, g.Style.Alpha * 0.60f); // This would be nice but e.g ColorButton with checkboard has issue with transparent colors :(
        tooltip_flags |= ImGuiTooltipFlags_OverridePreviousTooltip;
    }

    char window_name[16];
    ImFormatString(window_name, IM_ARRAYSIZE(window_name), "##Tooltip_%02d", g.TooltipOverrideCount);
    if (tooltip_flags & ImGuiTooltipFlags_OverridePreviousTooltip)
        if (ImGuiWindow* window = FindWindowByName(window_name))
            if (window->Active)
            {
                // Hide previous tooltip from being displayed. We can't easily "reset" the content of a window so we create a new one.
                window->Hidden = true;
                window->HiddenFramesCanSkipItems = 1;
                ImFormatString(window_name, IM_ARRAYSIZE(window_name), "##Tooltip_%02d", ++g.TooltipOverrideCount);
            }
    ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
    Begin(window_name, NULL, flags | extra_flags);
}

void ImGui::EndTooltip()
{
    IM_ASSERT(GetCurrentWindowRead()->Flags & ImGuiWindowFlags_Tooltip);   // Mismatched BeginTooltip()/EndTooltip() calls
    End();
}

void ImGui::SetTooltipV(const char* fmt, va_list args)
{
    BeginTooltipEx(0, ImGuiTooltipFlags_OverridePreviousTooltip);
    TextV(fmt, args);
    EndTooltip();
}

void ImGui::SetTooltip(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SetTooltipV(fmt, args);
    va_end(args);
}

//-----------------------------------------------------------------------------
// [SECTION] POPUPS
//-----------------------------------------------------------------------------

bool ImGui::IsPopupOpen(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    return g.OpenPopupStack.Size > g.BeginPopupStack.Size && g.OpenPopupStack[g.BeginPopupStack.Size].PopupId == id;
}

bool ImGui::IsPopupOpen(const char* str_id)
{
    ImGuiContext& g = *GImGui;
    return g.OpenPopupStack.Size > g.BeginPopupStack.Size && g.OpenPopupStack[g.BeginPopupStack.Size].PopupId == g.CurrentWindow->GetID(str_id);
}

ImGuiWindow* ImGui::GetTopMostPopupModal()
{
    ImGuiContext& g = *GImGui;
    for (int n = g.OpenPopupStack.Size - 1; n >= 0; n--)
        if (ImGuiWindow* popup = g.OpenPopupStack.Data[n].Window)
            if (popup->Flags & ImGuiWindowFlags_Modal)
                return popup;
    return NULL;
}

void ImGui::OpenPopup(const char* str_id)
{
    ImGuiContext& g = *GImGui;
    OpenPopupEx(g.CurrentWindow->GetID(str_id));
}

// Mark popup as open (toggle toward open state).
// Popups are closed when user click outside, or activate a pressable item, or CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block.
// Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
// One open popup per level of the popup hierarchy (NB: when assigning we reset the Window member of ImGuiPopupRef to NULL)
void ImGui::OpenPopupEx(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;
    int current_stack_size = g.BeginPopupStack.Size;
    ImGuiPopupData popup_ref; // Tagged as new ref as Window will be set back to NULL if we write this into OpenPopupStack.
    popup_ref.PopupId = id;
    popup_ref.Window = NULL;
    popup_ref.SourceWindow = g.NavWindow;
    popup_ref.OpenFrameCount = g.FrameCount;
    popup_ref.OpenParentId = parent_window->IDStack.back();
    popup_ref.OpenPopupPos = NavCalcPreferredRefPos();
    popup_ref.OpenMousePos = IsMousePosValid(&g.IO.MousePos) ? g.IO.MousePos : popup_ref.OpenPopupPos;

    //IMGUI_DEBUG_LOG("OpenPopupEx(0x%08X)\n", g.FrameCount, id);
    if (g.OpenPopupStack.Size < current_stack_size + 1)
    {
        g.OpenPopupStack.push_back(popup_ref);
    } else
    {
        // Gently handle the user mistakenly calling OpenPopup() every frame. It is a programming mistake! However, if we were to run the regular code path, the ui
        // would become completely unusable because the popup will always be in hidden-while-calculating-size state _while_ claiming focus. Which would be a very confusing
        // situation for the programmer. Instead, we silently allow the popup to proceed, it will keep reappearing and the programming error will be more obvious to understand.
        if (g.OpenPopupStack[current_stack_size].PopupId == id && g.OpenPopupStack[current_stack_size].OpenFrameCount == g.FrameCount - 1)
        {
            g.OpenPopupStack[current_stack_size].OpenFrameCount = popup_ref.OpenFrameCount;
        } else
        {
            // Close child popups if any, then flag popup for open/reopen
            g.OpenPopupStack.resize(current_stack_size + 1);
            g.OpenPopupStack[current_stack_size] = popup_ref;
        }

        // When reopening a popup we first refocus its parent, otherwise if its parent is itself a popup it would get closed by ClosePopupsOverWindow().
        // This is equivalent to what ClosePopupToLevel() does.
        //if (g.OpenPopupStack[current_stack_size].PopupId == id)
        //    FocusWindow(parent_window);
    }
}

void ImGui::ClosePopupsOverWindow(ImGuiWindow* ref_window, bool restore_focus_to_window_under_popup)
{
    ImGuiContext& g = *GImGui;
    if (g.OpenPopupStack.empty())
        return;

    // When popups are stacked, clicking on a lower level popups puts focus back to it and close popups above it.
    // Don't close our own child popup windows.
    int popup_count_to_keep = 0;
    if (ref_window)
    {
        // Find the highest popup which is a descendant of the reference window (generally reference window = NavWindow)
        for (; popup_count_to_keep < g.OpenPopupStack.Size; popup_count_to_keep++)
        {
            ImGuiPopupData& popup = g.OpenPopupStack[popup_count_to_keep];
            if (!popup.Window)
                continue;
            IM_ASSERT((popup.Window->Flags & ImGuiWindowFlags_Popup) != 0);
            if (popup.Window->Flags & ImGuiWindowFlags_ChildWindow)
                continue;

            // Trim the stack when popups are not direct descendant of the reference window (the reference window is often the NavWindow)
            bool popup_or_descendent_is_ref_window = false;
            for (int m = popup_count_to_keep; m < g.OpenPopupStack.Size && !popup_or_descendent_is_ref_window; m++)
                if (ImGuiWindow* popup_window = g.OpenPopupStack[m].Window)
                    if (popup_window->RootWindow == ref_window->RootWindow)
                        popup_or_descendent_is_ref_window = true;
            if (!popup_or_descendent_is_ref_window)
                break;
        }
    }
    if (popup_count_to_keep < g.OpenPopupStack.Size) // This test is not required but it allows to set a convenient breakpoint on the statement below
    {
        //IMGUI_DEBUG_LOG("ClosePopupsOverWindow(%s) -> ClosePopupToLevel(%d)\n", ref_window->Name, popup_count_to_keep);
        ClosePopupToLevel(popup_count_to_keep, restore_focus_to_window_under_popup);
    }
}

void ImGui::ClosePopupToLevel(int remaining, bool restore_focus_to_window_under_popup)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(remaining >= 0 && remaining < g.OpenPopupStack.Size);
    ImGuiWindow* focus_window = g.OpenPopupStack[remaining].SourceWindow;
    ImGuiWindow* popup_window = g.OpenPopupStack[remaining].Window;
    g.OpenPopupStack.resize(remaining);

    if (restore_focus_to_window_under_popup)
    {
        if (focus_window && !focus_window->WasActive && popup_window)
        {
            // Fallback
            FocusTopMostWindowUnderOne(popup_window, NULL);
        } else
        {
            if (g.NavLayer == ImGuiNavLayer_Main && focus_window)
                focus_window = NavRestoreLastChildNavWindow(focus_window);
            FocusWindow(focus_window);
        }
    }
}

// Close the popup we have begin-ed into.
void ImGui::CloseCurrentPopup()
{
    ImGuiContext& g = *GImGui;
    int popup_idx = g.BeginPopupStack.Size - 1;
    if (popup_idx < 0 || popup_idx >= g.OpenPopupStack.Size || g.BeginPopupStack[popup_idx].PopupId != g.OpenPopupStack[popup_idx].PopupId)
        return;

    // Closing a menu closes its top-most parent popup (unless a modal)
    while (popup_idx > 0)
    {
        ImGuiWindow* popup_window = g.OpenPopupStack[popup_idx].Window;
        ImGuiWindow* parent_popup_window = g.OpenPopupStack[popup_idx - 1].Window;
        bool close_parent = false;
        if (popup_window && (popup_window->Flags & ImGuiWindowFlags_ChildMenu))
            if (parent_popup_window == NULL || !(parent_popup_window->Flags & ImGuiWindowFlags_Modal))
                close_parent = true;
        if (!close_parent)
            break;
        popup_idx--;
    }
    //IMGUI_DEBUG_LOG("CloseCurrentPopup %d -> %d\n", g.BeginPopupStack.Size - 1, popup_idx);
    ClosePopupToLevel(popup_idx, true);

    // A common pattern is to close a popup when selecting a menu item/selectable that will open another window.
    // To improve this usage pattern, we avoid nav highlight for a single frame in the parent window.
    // Similarly, we could avoid mouse hover highlight in this window but it is less visually problematic.
    if (ImGuiWindow* window = g.NavWindow)
        window->DC.NavHideHighlightOneFrame = true;
}

bool ImGui::BeginPopupEx(ImGuiID id, ImGuiWindowFlags flags)
{
    ImGuiContext& g = *GImGui;
    if (!IsPopupOpen(id))
    {
        g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
        return false;
    }

    char name[20];
    if (flags & ImGuiWindowFlags_ChildMenu)
        ImFormatString(name, IM_ARRAYSIZE(name), "##Menu_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth
    else
        ImFormatString(name, IM_ARRAYSIZE(name), "##Popup_%08x", id); // Not recycling, so we can close/open during the same frame

    flags |= ImGuiWindowFlags_Popup;
    bool is_open = Begin(name, NULL, flags);
    if (!is_open) // NB: Begin can return false when the popup is completely clipped (e.g. zero size display)
        EndPopup();

    return is_open;
}

bool ImGui::BeginPopup(const char* str_id, ImGuiWindowFlags flags)
{
    ImGuiContext& g = *GImGui;
    if (g.OpenPopupStack.Size <= g.BeginPopupStack.Size) // Early out for performance
    {
        g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
        return false;
    }
    flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings;
    return BeginPopupEx(g.CurrentWindow->GetID(str_id), flags);
}

// If 'p_open' is specified for a modal popup window, the popup will have a regular close button which will close the popup.
// Note that popup visibility status is owned by Dear ImGui (and manipulated with e.g. OpenPopup) so the actual value of *p_open is meaningless here.
bool ImGui::BeginPopupModal(const char* name, bool* p_open, ImGuiWindowFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    const ImGuiID id = window->GetID(name);
    if (!IsPopupOpen(id))
    {
        g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
        return false;
    }

    // Center modal windows by default
    // FIXME: Should test for (PosCond & window->SetWindowPosAllowFlags) with the upcoming window.
    if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasPos) == 0)
        SetNextWindowPos(g.IO.DisplaySize * 0.5f, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    flags |= ImGuiWindowFlags_Popup | ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
    const bool is_open = Begin(name, p_open, flags);
    if (!is_open || (p_open && !*p_open)) // NB: is_open can be 'false' when the popup is completely clipped (e.g. zero size display)
    {
        EndPopup();
        if (is_open)
            ClosePopupToLevel(g.BeginPopupStack.Size, true);
        return false;
    }
    return is_open;
}

void ImGui::EndPopup()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    IM_ASSERT(window->Flags & ImGuiWindowFlags_Popup);  // Mismatched BeginPopup()/EndPopup() calls
    IM_ASSERT(g.BeginPopupStack.Size > 0);

    // Make all menus and popups wrap around for now, may need to expose that policy.
    if (g.NavWindow == window)
        NavMoveRequestTryWrapping(window, ImGuiNavMoveFlags_LoopY);

    // Child-popups don't need to be laid out
    IM_ASSERT(g.WithinEndChild == false);
    if (window->Flags & ImGuiWindowFlags_ChildWindow)
        g.WithinEndChild = true;
    End();
    g.WithinEndChild = false;
}

bool ImGui::OpenPopupOnItemClick(const char* str_id, ImGuiMouseButton mouse_button)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    if (IsMouseReleased(mouse_button) && IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
    {
        ImGuiID id = str_id ? window->GetID(str_id) : window->DC.LastItemId; // If user hasn't passed an ID, we can use the LastItemID. Using LastItemID as a Popup ID won't conflict!
        IM_ASSERT(id != 0);                                                  // You cannot pass a NULL str_id if the last item has no identifier (e.g. a Text() item)
        OpenPopupEx(id);
        return true;
    }
    return false;
}

// This is a helper to handle the simplest case of associating one named popup to one given widget.
// You may want to handle this on user side if you have specific needs (e.g. tweaking IsItemHovered() parameters).
// You can pass a NULL str_id to use the identifier of the last item.
bool ImGui::BeginPopupContextItem(const char* str_id, ImGuiMouseButton mouse_button)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    if (window->SkipItems)
        return false;
    ImGuiID id = str_id ? window->GetID(str_id) : window->DC.LastItemId; // If user hasn't passed an ID, we can use the LastItemID. Using LastItemID as a Popup ID won't conflict!
    IM_ASSERT(id != 0);                                                  // You cannot pass a NULL str_id if the last item has no identifier (e.g. a Text() item)
    if (IsMouseReleased(mouse_button) && IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        OpenPopupEx(id);
    return BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
}

bool ImGui::BeginPopupContextWindow(const char* str_id, ImGuiMouseButton mouse_button, bool also_over_items)
{
    if (!str_id)
        str_id = "window_context";
    ImGuiID id = GImGui->CurrentWindow->GetID(str_id);
    if (IsMouseReleased(mouse_button) && IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        if (also_over_items || !IsAnyItemHovered())
            OpenPopupEx(id);
    return BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
}

bool ImGui::BeginPopupContextVoid(const char* str_id, ImGuiMouseButton mouse_button)
{
    if (!str_id)
        str_id = "void_context";
    ImGuiID id = GImGui->CurrentWindow->GetID(str_id);
    if (IsMouseReleased(mouse_button) && !IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
        OpenPopupEx(id);
    return BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
}

// r_avoid = the rectangle to avoid (e.g. for tooltip it is a rectangle around the mouse cursor which we want to avoid. for popups it's a small point around the cursor.)
// r_outer = the visible area rectangle, minus safe area padding. If our popup size won't fit because of safe area padding we ignore it.
ImVec2 ImGui::FindBestWindowPosForPopupEx(const ImVec2& ref_pos, const ImVec2& size, ImGuiDir* last_dir, const ImRect& r_outer, const ImRect& r_avoid, ImGuiPopupPositionPolicy policy)
{
    ImVec2 base_pos_clamped = ImClamp(ref_pos, r_outer.Min, r_outer.Max - size);
    //GetForegroundDrawList()->AddRect(r_avoid.Min, r_avoid.Max, IM_COL32(255,0,0,255));
    //GetForegroundDrawList()->AddRect(r_outer.Min, r_outer.Max, IM_COL32(0,255,0,255));

    // Combo Box policy (we want a connecting edge)
    if (policy == ImGuiPopupPositionPolicy_ComboBox)
    {
        const ImGuiDir dir_prefered_order[ImGuiDir_COUNT] = { ImGuiDir_Down, ImGuiDir_Right, ImGuiDir_Left, ImGuiDir_Up };
        for (int n = (*last_dir != ImGuiDir_None) ? -1 : 0; n < ImGuiDir_COUNT; n++)
        {
            const ImGuiDir dir = (n == -1) ? *last_dir : dir_prefered_order[n];
            if (n != -1 && dir == *last_dir) // Already tried this direction?
                continue;
            ImVec2 pos;
            if (dir == ImGuiDir_Down)  pos = ImVec2(r_avoid.Min.x, r_avoid.Max.y);          // Below, Toward Right (default)
            if (dir == ImGuiDir_Right) pos = ImVec2(r_avoid.Min.x, r_avoid.Min.y - size.y); // Above, Toward Right
            if (dir == ImGuiDir_Left)  pos = ImVec2(r_avoid.Max.x - size.x, r_avoid.Max.y); // Below, Toward Left
            if (dir == ImGuiDir_Up)    pos = ImVec2(r_avoid.Max.x - size.x, r_avoid.Min.y - size.y); // Above, Toward Left
            if (!r_outer.Contains(ImRect(pos, pos + size)))
                continue;
            *last_dir = dir;
            return pos;
        }
    }

    // Default popup policy
    const ImGuiDir dir_prefered_order[ImGuiDir_COUNT] = { ImGuiDir_Right, ImGuiDir_Down, ImGuiDir_Up, ImGuiDir_Left };
    for (int n = (*last_dir != ImGuiDir_None) ? -1 : 0; n < ImGuiDir_COUNT; n++)
    {
        const ImGuiDir dir = (n == -1) ? *last_dir : dir_prefered_order[n];
        if (n != -1 && dir == *last_dir) // Already tried this direction?
            continue;
        float avail_w = (dir == ImGuiDir_Left ? r_avoid.Min.x : r_outer.Max.x) - (dir == ImGuiDir_Right ? r_avoid.Max.x : r_outer.Min.x);
        float avail_h = (dir == ImGuiDir_Up ? r_avoid.Min.y : r_outer.Max.y) - (dir == ImGuiDir_Down ? r_avoid.Max.y : r_outer.Min.y);
        if (avail_w < size.x || avail_h < size.y)
            continue;
        ImVec2 pos;
        pos.x = (dir == ImGuiDir_Left) ? r_avoid.Min.x - size.x : (dir == ImGuiDir_Right) ? r_avoid.Max.x : base_pos_clamped.x;
        pos.y = (dir == ImGuiDir_Up) ? r_avoid.Min.y - size.y : (dir == ImGuiDir_Down) ? r_avoid.Max.y : base_pos_clamped.y;
        *last_dir = dir;
        return pos;
    }

    // Fallback, try to keep within display
    *last_dir = ImGuiDir_None;
    ImVec2 pos = ref_pos;
    pos.x = ImMax(ImMin(pos.x + size.x, r_outer.Max.x) - size.x, r_outer.Min.x);
    pos.y = ImMax(ImMin(pos.y + size.y, r_outer.Max.y) - size.y, r_outer.Min.y);
    return pos;
}

ImRect ImGui::GetWindowAllowedExtentRect(ImGuiWindow* window)
{
    IM_UNUSED(window);
    ImVec2 padding = GImGui->Style.DisplaySafeAreaPadding;
    ImRect r_screen = GetViewportRect();
    r_screen.Expand(ImVec2((r_screen.GetWidth() > padding.x * 2) ? -padding.x : 0.0f, (r_screen.GetHeight() > padding.y * 2) ? -padding.y : 0.0f));
    return r_screen;
}

ImVec2 ImGui::FindBestWindowPosForPopup(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;

    ImRect r_outer = GetWindowAllowedExtentRect(window);
    if (window->Flags & ImGuiWindowFlags_ChildMenu)
    {
        // Child menus typically request _any_ position within the parent menu item, and then we move the new menu outside the parent bounds.
        // This is how we end up with child menus appearing (most-commonly) on the right of the parent menu.
        IM_ASSERT(g.CurrentWindow == window);
        ImGuiWindow* parent_window = g.CurrentWindowStack[g.CurrentWindowStack.Size - 2];
        float horizontal_overlap = g.Style.ItemInnerSpacing.x; // We want some overlap to convey the relative depth of each menu (currently the amount of overlap is hard-coded to style.ItemSpacing.x).
        ImRect r_avoid;
        if (parent_window->DC.MenuBarAppending)
            r_avoid = ImRect(-FLT_MAX, parent_window->ClipRect.Min.y, FLT_MAX, parent_window->ClipRect.Max.y); // Avoid parent menu-bar. If we wanted multi-line menu-bar, we may instead want to have the calling window setup e.g. a NextWindowData.PosConstraintAvoidRect field
        else
            r_avoid = ImRect(parent_window->Pos.x + horizontal_overlap, -FLT_MAX, parent_window->Pos.x + parent_window->Size.x - horizontal_overlap - parent_window->ScrollbarSizes.x, FLT_MAX);
        return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, r_avoid);
    }
    if (window->Flags & ImGuiWindowFlags_Popup)
    {
        ImRect r_avoid = ImRect(window->Pos.x - 1, window->Pos.y - 1, window->Pos.x + 1, window->Pos.y + 1);
        return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, r_avoid);
    }
    if (window->Flags & ImGuiWindowFlags_Tooltip)
    {
        // Position tooltip (always follows mouse)
        float sc = g.Style.MouseCursorScale;
        ImVec2 ref_pos = NavCalcPreferredRefPos();
        ImRect r_avoid;
        if (!g.NavDisableHighlight && g.NavDisableMouseHover && !(g.IO.ConfigFlags & ImGuiConfigFlags_NavEnableSetMousePos))
            r_avoid = ImRect(ref_pos.x - 16, ref_pos.y - 8, ref_pos.x + 16, ref_pos.y + 8);
        else
            r_avoid = ImRect(ref_pos.x - 16, ref_pos.y - 8, ref_pos.x + 24 * sc, ref_pos.y + 24 * sc); // FIXME: Hard-coded based on mouse cursor shape expectation. Exact dimension not very important.
        ImVec2 pos = FindBestWindowPosForPopupEx(ref_pos, window->Size, &window->AutoPosLastDirection, r_outer, r_avoid);
        if (window->AutoPosLastDirection == ImGuiDir_None)
            pos = ref_pos + ImVec2(2, 2); // If there's not enough room, for tooltip we prefer avoiding the cursor at all cost even if it means that part of the tooltip won't be visible.
        return pos;
    }
    IM_ASSERT(0);
    return window->Pos;
}

//-----------------------------------------------------------------------------
// [SECTION] KEYBOARD/GAMEPAD NAVIGATION
//-----------------------------------------------------------------------------

// FIXME-NAV: The existence of SetNavID vs SetNavIDWithRectRel vs SetFocusID is incredibly messy and confusing,
// and needs some explanation or serious refactoring.
void ImGui::SetNavID(ImGuiID id, int nav_layer, ImGuiID focus_scope_id)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.NavWindow);
    IM_ASSERT(nav_layer == 0 || nav_layer == 1);
    g.NavId = id;
    g.NavFocusScopeId = focus_scope_id;
    g.NavWindow->NavLastIds[nav_layer] = id;
}

void ImGui::SetNavIDWithRectRel(ImGuiID id, int nav_layer, ImGuiID focus_scope_id, const ImRect& rect_rel)
{
    ImGuiContext& g = *GImGui;
    SetNavID(id, nav_layer, focus_scope_id);
    g.NavWindow->NavRectRel[nav_layer] = rect_rel;
    g.NavMousePosDirty = true;
    g.NavDisableHighlight = false;
    g.NavDisableMouseHover = true;
}

void ImGui::SetFocusID(ImGuiID id, ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(id != 0);

    // Assume that SetFocusID() is called in the context where its window->DC.NavLayerCurrent and window->DC.NavFocusScopeIdCurrent are valid.
    // Note that window may be != g.CurrentWindow (e.g. SetFocusID call in InputTextEx for multi-line text)
    const ImGuiNavLayer nav_layer = window->DC.NavLayerCurrent;
    if (g.NavWindow != window)
        g.NavInitRequest = false;
    g.NavWindow = window;
    g.NavId = id;
    g.NavLayer = nav_layer;
    g.NavFocusScopeId = window->DC.NavFocusScopeIdCurrent;
    window->NavLastIds[nav_layer] = id;
    if (window->DC.LastItemId == id)
        window->NavRectRel[nav_layer] = ImRect(window->DC.LastItemRect.Min - window->Pos, window->DC.LastItemRect.Max - window->Pos);

    if (g.ActiveIdSource == ImGuiInputSource_Nav)
        g.NavDisableMouseHover = true;
    else
        g.NavDisableHighlight = true;
}

ImGuiDir ImGetDirQuadrantFromDelta(float dx, float dy)
{
    if (ImFabs(dx) > ImFabs(dy))
        return (dx > 0.0f) ? ImGuiDir_Right : ImGuiDir_Left;
    return (dy > 0.0f) ? ImGuiDir_Down : ImGuiDir_Up;
}

static float inline NavScoreItemDistInterval(float a0, float a1, float b0, float b1)
{
    if (a1 < b0)
        return a1 - b0;
    if (b1 < a0)
        return a0 - b1;
    return 0.0f;
}

static void inline NavClampRectToVisibleAreaForMoveDir(ImGuiDir move_dir, ImRect& r, const ImRect& clip_rect)
{
    if (move_dir == ImGuiDir_Left || move_dir == ImGuiDir_Right)
    {
        r.Min.y = ImClamp(r.Min.y, clip_rect.Min.y, clip_rect.Max.y);
        r.Max.y = ImClamp(r.Max.y, clip_rect.Min.y, clip_rect.Max.y);
    } else
    {
        r.Min.x = ImClamp(r.Min.x, clip_rect.Min.x, clip_rect.Max.x);
        r.Max.x = ImClamp(r.Max.x, clip_rect.Min.x, clip_rect.Max.x);
    }
}

// Scoring function for gamepad/keyboard directional navigation. Based on https://gist.github.com/rygorous/6981057
static bool ImGui::NavScoreItem(ImGuiNavMoveResult* result, ImRect cand)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (g.NavLayer != window->DC.NavLayerCurrent)
        return false;

    const ImRect& curr = g.NavScoringRect; // Current modified source rect (NB: we've applied Max.x = Min.x in NavUpdate() to inhibit the effect of having varied item width)
    g.NavScoringCount++;

    // When entering through a NavFlattened border, we consider child window items as fully clipped for scoring
    if (window->ParentWindow == g.NavWindow)
    {
        IM_ASSERT((window->Flags | g.NavWindow->Flags) & ImGuiWindowFlags_NavFlattened);
        if (!window->ClipRect.Overlaps(cand))
            return false;
        cand.ClipWithFull(window->ClipRect); // This allows the scored item to not overlap other candidates in the parent window
    }

    // We perform scoring on items bounding box clipped by the current clipping rectangle on the other axis (clipping on our movement axis would give us equal scores for all clipped items)
    // For example, this ensure that items in one column are not reached when moving vertically from items in another column.
    NavClampRectToVisibleAreaForMoveDir(g.NavMoveClipDir, cand, window->ClipRect);

    // Compute distance between boxes
    // FIXME-NAV: Introducing biases for vertical navigation, needs to be removed.
    float dbx = NavScoreItemDistInterval(cand.Min.x, cand.Max.x, curr.Min.x, curr.Max.x);
    float dby = NavScoreItemDistInterval(ImLerp(cand.Min.y, cand.Max.y, 0.2f), ImLerp(cand.Min.y, cand.Max.y, 0.8f), ImLerp(curr.Min.y, curr.Max.y, 0.2f), ImLerp(curr.Min.y, curr.Max.y, 0.8f)); // Scale down on Y to keep using box-distance for vertically touching items
    if (dby != 0.0f && dbx != 0.0f)
        dbx = (dbx / 1000.0f) + ((dbx > 0.0f) ? +1.0f : -1.0f);
    float dist_box = ImFabs(dbx) + ImFabs(dby);

    // Compute distance between centers (this is off by a factor of 2, but we only compare center distances with each other so it doesn't matter)
    float dcx = (cand.Min.x + cand.Max.x) - (curr.Min.x + curr.Max.x);
    float dcy = (cand.Min.y + cand.Max.y) - (curr.Min.y + curr.Max.y);
    float dist_center = ImFabs(dcx) + ImFabs(dcy); // L1 metric (need this for our connectedness guarantee)

    // Determine which quadrant of 'curr' our candidate item 'cand' lies in based on distance
    ImGuiDir quadrant;
    float dax = 0.0f, day = 0.0f, dist_axial = 0.0f;
    if (dbx != 0.0f || dby != 0.0f)
    {
        // For non-overlapping boxes, use distance between boxes
        dax = dbx;
        day = dby;
        dist_axial = dist_box;
        quadrant = ImGetDirQuadrantFromDelta(dbx, dby);
    } else if (dcx != 0.0f || dcy != 0.0f)
    {
        // For overlapping boxes with different centers, use distance between centers
        dax = dcx;
        day = dcy;
        dist_axial = dist_center;
        quadrant = ImGetDirQuadrantFromDelta(dcx, dcy);
    } else
    {
        // Degenerate case: two overlapping buttons with same center, break ties arbitrarily (note that LastItemId here is really the _previous_ item order, but it doesn't matter)
        quadrant = (window->DC.LastItemId < g.NavId) ? ImGuiDir_Left : ImGuiDir_Right;
    }

#if IMGUI_DEBUG_NAV_SCORING
    char buf[128];
    if (IsMouseHoveringRect(cand.Min, cand.Max))
    {
        ImFormatString(buf, IM_ARRAYSIZE(buf), "dbox (%.2f,%.2f->%.4f)\ndcen (%.2f,%.2f->%.4f)\nd (%.2f,%.2f->%.4f)\nnav %c, quadrant %c", dbx, dby, dist_box, dcx, dcy, dist_center, dax, day, dist_axial, "WENS"[g.NavMoveDir], "WENS"[quadrant]);
        ImDrawList* draw_list = GetForegroundDrawList(window);
        draw_list->AddRect(curr.Min, curr.Max, IM_COL32(255, 200, 0, 100));
        draw_list->AddRect(cand.Min, cand.Max, IM_COL32(255, 255, 0, 200));
        draw_list->AddRectFilled(cand.Max - ImVec2(4, 4), cand.Max + CalcTextSize(buf) + ImVec2(4, 4), IM_COL32(40, 0, 0, 150));
        draw_list->AddText(g.IO.FontDefault, 13.0f, cand.Max, ~0U, buf);
    } else if (g.IO.KeyCtrl) // Hold to preview score in matching quadrant. Press C to rotate.
    {
        if (IsKeyPressedMap(ImGuiKey_C)) { g.NavMoveDirLast = (ImGuiDir)((g.NavMoveDirLast + 1) & 3); g.IO.KeysDownDuration[g.IO.KeyMap[ImGuiKey_C]] = 0.01f; }
        if (quadrant == g.NavMoveDir)
        {
            ImFormatString(buf, IM_ARRAYSIZE(buf), "%.0f/%.0f", dist_box, dist_center);
            ImDrawList* draw_list = GetForegroundDrawList(window);
            draw_list->AddRectFilled(cand.Min, cand.Max, IM_COL32(255, 0, 0, 200));
            draw_list->AddText(g.IO.FontDefault, 13.0f, cand.Min, IM_COL32(255, 255, 255, 255), buf);
        }
    }
#endif

    // Is it in the quadrant we're interesting in moving to?
    bool new_best = false;
    if (quadrant == g.NavMoveDir)
    {
        // Does it beat the current best candidate?
        if (dist_box < result->DistBox)
        {
            result->DistBox = dist_box;
            result->DistCenter = dist_center;
            return true;
        }
        if (dist_box == result->DistBox)
        {
            // Try using distance between center points to break ties
            if (dist_center < result->DistCenter)
            {
                result->DistCenter = dist_center;
                new_best = true;
            } else if (dist_center == result->DistCenter)
            {
                // Still tied! we need to be extra-careful to make sure everything gets linked properly. We consistently break ties by symbolically moving "later" items
                // (with higher index) to the right/downwards by an infinitesimal amount since we the current "best" button already (so it must have a lower index),
                // this is fairly easy. This rule ensures that all buttons with dx==dy==0 will end up being linked in order of appearance along the x axis.
                if (((g.NavMoveDir == ImGuiDir_Up || g.NavMoveDir == ImGuiDir_Down) ? dby : dbx) < 0.0f) // moving bj to the right/down decreases distance
                    new_best = true;
            }
        }
    }

    // Axial check: if 'curr' has no link at all in some direction and 'cand' lies roughly in that direction, add a tentative link. This will only be kept if no "real" matches
    // are found, so it only augments the graph produced by the above method using extra links. (important, since it doesn't guarantee strong connectedness)
    // This is just to avoid buttons having no links in a particular direction when there's a suitable neighbor. you get good graphs without this too.
    // 2017/09/29: FIXME: This now currently only enabled inside menu bars, ideally we'd disable it everywhere. Menus in particular need to catch failure. For general navigation it feels awkward.
    // Disabling it may lead to disconnected graphs when nodes are very spaced out on different axis. Perhaps consider offering this as an option?
    if (result->DistBox == FLT_MAX && dist_axial < result->DistAxial)  // Check axial match
        if (g.NavLayer == ImGuiNavLayer_Menu && !(g.NavWindow->Flags & ImGuiWindowFlags_ChildMenu))
            if ((g.NavMoveDir == ImGuiDir_Left && dax < 0.0f) || (g.NavMoveDir == ImGuiDir_Right && dax > 0.0f) || (g.NavMoveDir == ImGuiDir_Up && day < 0.0f) || (g.NavMoveDir == ImGuiDir_Down && day > 0.0f))
            {
                result->DistAxial = dist_axial;
                new_best = true;
            }

    return new_best;
}

// We get there when either NavId == id, or when g.NavAnyRequest is set (which is updated by NavUpdateAnyRequestFlag above)
static void ImGui::NavProcessItem(ImGuiWindow* window, const ImRect& nav_bb, const ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    //if (!g.IO.NavActive)  // [2017/10/06] Removed this possibly redundant test but I am not sure of all the side-effects yet. Some of the feature here will need to work regardless of using a _NoNavInputs flag.
    //    return;

    const ImGuiItemFlags item_flags = window->DC.ItemFlags;
    const ImRect nav_bb_rel(nav_bb.Min - window->Pos, nav_bb.Max - window->Pos);

    // Process Init Request
    if (g.NavInitRequest && g.NavLayer == window->DC.NavLayerCurrent)
    {
        // Even if 'ImGuiItemFlags_NoNavDefaultFocus' is on (typically collapse/close button) we record the first ResultId so they can be used as a fallback
        if (!(item_flags & ImGuiItemFlags_NoNavDefaultFocus) || g.NavInitResultId == 0)
        {
            g.NavInitResultId = id;
            g.NavInitResultRectRel = nav_bb_rel;
        }
        if (!(item_flags & ImGuiItemFlags_NoNavDefaultFocus))
        {
            g.NavInitRequest = false; // Found a match, clear request
            NavUpdateAnyRequestFlag();
        }
    }

    // Process Move Request (scoring for navigation)
    // FIXME-NAV: Consider policy for double scoring (scoring from NavScoringRectScreen + scoring from a rect wrapped according to current wrapping policy)
    if ((g.NavId != id || (g.NavMoveRequestFlags & ImGuiNavMoveFlags_AllowCurrentNavId)) && !(item_flags & (ImGuiItemFlags_Disabled | ImGuiItemFlags_NoNav)))
    {
        ImGuiNavMoveResult* result = (window == g.NavWindow) ? &g.NavMoveResultLocal : &g.NavMoveResultOther;
#if IMGUI_DEBUG_NAV_SCORING
        // [DEBUG] Score all items in NavWindow at all times
        if (!g.NavMoveRequest)
            g.NavMoveDir = g.NavMoveDirLast;
        bool new_best = NavScoreItem(result, nav_bb) && g.NavMoveRequest;
#else
        bool new_best = g.NavMoveRequest && NavScoreItem(result, nav_bb);
#endif
        if (new_best)
        {
            result->Window = window;
            result->ID = id;
            result->FocusScopeId = window->DC.NavFocusScopeIdCurrent;
            result->RectRel = nav_bb_rel;
        }

        // Features like PageUp/PageDown need to maintain a separate score for the visible set of items.
        const float VISIBLE_RATIO = 0.70f;
        if ((g.NavMoveRequestFlags & ImGuiNavMoveFlags_AlsoScoreVisibleSet) && window->ClipRect.Overlaps(nav_bb))
            if (ImClamp(nav_bb.Max.y, window->ClipRect.Min.y, window->ClipRect.Max.y) - ImClamp(nav_bb.Min.y, window->ClipRect.Min.y, window->ClipRect.Max.y) >= (nav_bb.Max.y - nav_bb.Min.y) * VISIBLE_RATIO)
                if (NavScoreItem(&g.NavMoveResultLocalVisibleSet, nav_bb))
                {
                    result = &g.NavMoveResultLocalVisibleSet;
                    result->Window = window;
                    result->ID = id;
                    result->FocusScopeId = window->DC.NavFocusScopeIdCurrent;
                    result->RectRel = nav_bb_rel;
                }
    }

    // Update window-relative bounding box of navigated item
    if (g.NavId == id)
    {
        g.NavWindow = window;                                           // Always refresh g.NavWindow, because some operations such as FocusItem() don't have a window.
        g.NavLayer = window->DC.NavLayerCurrent;
        g.NavFocusScopeId = window->DC.NavFocusScopeIdCurrent;
        g.NavIdIsAlive = true;
        g.NavIdTabCounter = window->DC.FocusCounterTabStop;
        window->NavRectRel[window->DC.NavLayerCurrent] = nav_bb_rel;    // Store item bounding box (relative to window position)
    }
}

bool ImGui::NavMoveRequestButNoResultYet()
{
    ImGuiContext& g = *GImGui;
    return g.NavMoveRequest && g.NavMoveResultLocal.ID == 0 && g.NavMoveResultOther.ID == 0;
}

void ImGui::NavMoveRequestCancel()
{
    ImGuiContext& g = *GImGui;
    g.NavMoveRequest = false;
    NavUpdateAnyRequestFlag();
}

void ImGui::NavMoveRequestForward(ImGuiDir move_dir, ImGuiDir clip_dir, const ImRect& bb_rel, ImGuiNavMoveFlags move_flags)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.NavMoveRequestForward == ImGuiNavForward_None);
    NavMoveRequestCancel();
    g.NavMoveDir = move_dir;
    g.NavMoveClipDir = clip_dir;
    g.NavMoveRequestForward = ImGuiNavForward_ForwardQueued;
    g.NavMoveRequestFlags = move_flags;
    g.NavWindow->NavRectRel[g.NavLayer] = bb_rel;
}

void ImGui::NavMoveRequestTryWrapping(ImGuiWindow* window, ImGuiNavMoveFlags move_flags)
{
    ImGuiContext& g = *GImGui;

    // Navigation wrap-around logic is delayed to the end of the frame because this operation is only valid after entire
    // popup is assembled and in case of appended popups it is not clear which EndPopup() call is final.
    g.NavWrapRequestWindow = window;
    g.NavWrapRequestFlags = move_flags;
}

// FIXME: This could be replaced by updating a frame number in each window when (window == NavWindow) and (NavLayer == 0).
// This way we could find the last focused window among our children. It would be much less confusing this way?
static void ImGui::NavSaveLastChildNavWindowIntoParent(ImGuiWindow* nav_window)
{
    ImGuiWindow* parent_window = nav_window;
    while (parent_window && (parent_window->Flags & ImGuiWindowFlags_ChildWindow) != 0 && (parent_window->Flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_ChildMenu)) == 0)
        parent_window = parent_window->ParentWindow;
    if (parent_window && parent_window != nav_window)
        parent_window->NavLastChildNavWindow = nav_window;
}

// Restore the last focused child.
// Call when we are expected to land on the Main Layer (0) after FocusWindow()
static ImGuiWindow* ImGui::NavRestoreLastChildNavWindow(ImGuiWindow* window)
{
    return window->NavLastChildNavWindow ? window->NavLastChildNavWindow : window;
}

static void NavRestoreLayer(ImGuiNavLayer layer)
{
    ImGuiContext& g = *GImGui;
    g.NavLayer = layer;
    if (layer == 0)
        g.NavWindow = ImGui::NavRestoreLastChildNavWindow(g.NavWindow);
    ImGuiWindow* window = g.NavWindow;
    if (layer == 0 && window->NavLastIds[0] != 0)
        ImGui::SetNavIDWithRectRel(window->NavLastIds[0], layer, 0, window->NavRectRel[0]);
    else
        ImGui::NavInitWindow(window, true);
}

static inline void ImGui::NavUpdateAnyRequestFlag()
{
    ImGuiContext& g = *GImGui;
    g.NavAnyRequest = g.NavMoveRequest || g.NavInitRequest || (IMGUI_DEBUG_NAV_SCORING && g.NavWindow != NULL);
    if (g.NavAnyRequest)
        IM_ASSERT(g.NavWindow != NULL);
}

// This needs to be called before we submit any widget (aka in or before Begin)
void ImGui::NavInitWindow(ImGuiWindow* window, bool force_reinit)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(window == g.NavWindow);
    bool init_for_nav = false;
    if (!(window->Flags & ImGuiWindowFlags_NoNavInputs))
        if (!(window->Flags & ImGuiWindowFlags_ChildWindow) || (window->Flags & ImGuiWindowFlags_Popup) || (window->NavLastIds[0] == 0) || force_reinit)
            init_for_nav = true;
    //IMGUI_DEBUG_LOG("[Nav] NavInitWindow() init_for_nav=%d, window=\"%s\", layer=%d\n", init_for_nav, window->Name, g.NavLayer);
    if (init_for_nav)
    {
        SetNavID(0, g.NavLayer, 0);
        g.NavInitRequest = true;
        g.NavInitRequestFromMove = false;
        g.NavInitResultId = 0;
        g.NavInitResultRectRel = ImRect();
        NavUpdateAnyRequestFlag();
    } else
    {
        g.NavId = window->NavLastIds[0];
        g.NavFocusScopeId = 0;
    }
}

static ImVec2 ImGui::NavCalcPreferredRefPos()
{
    ImGuiContext& g = *GImGui;
    if (g.NavDisableHighlight || !g.NavDisableMouseHover || !g.NavWindow)
    {
        // Mouse (we need a fallback in case the mouse becomes invalid after being used)
        if (IsMousePosValid(&g.IO.MousePos))
            return g.IO.MousePos;
        return g.LastValidMousePos;
    } else
    {
        // When navigation is active and mouse is disabled, decide on an arbitrary position around the bottom left of the currently navigated item.
        const ImRect& rect_rel = g.NavWindow->NavRectRel[g.NavLayer];
        ImVec2 pos = g.NavWindow->Pos + ImVec2(rect_rel.Min.x + ImMin(g.Style.FramePadding.x * 4, rect_rel.GetWidth()), rect_rel.Max.y - ImMin(g.Style.FramePadding.y, rect_rel.GetHeight()));
        ImRect visible_rect = GetViewportRect();
        return ImFloor(ImClamp(pos, visible_rect.Min, visible_rect.Max));   // ImFloor() is important because non-integer mouse position application in back-end might be lossy and result in undesirable non-zero delta.
    }
}

float ImGui::GetNavInputAmount(ImGuiNavInput n, ImGuiInputReadMode mode)
{
    ImGuiContext& g = *GImGui;
    if (mode == ImGuiInputReadMode_Down)
        return g.IO.NavInputs[n];                         // Instant, read analog input (0.0f..1.0f, as provided by user)

    const float t = g.IO.NavInputsDownDuration[n];
    if (t < 0.0f && mode == ImGuiInputReadMode_Released)  // Return 1.0f when just released, no repeat, ignore analog input.
        return (g.IO.NavInputsDownDurationPrev[n] >= 0.0f ? 1.0f : 0.0f);
    if (t < 0.0f)
        return 0.0f;
    if (mode == ImGuiInputReadMode_Pressed)               // Return 1.0f when just pressed, no repeat, ignore analog input.
        return (t == 0.0f) ? 1.0f : 0.0f;
    if (mode == ImGuiInputReadMode_Repeat)
        return (float)CalcTypematicRepeatAmount(t - g.IO.DeltaTime, t, g.IO.KeyRepeatDelay * 0.72f, g.IO.KeyRepeatRate * 0.80f);
    if (mode == ImGuiInputReadMode_RepeatSlow)
        return (float)CalcTypematicRepeatAmount(t - g.IO.DeltaTime, t, g.IO.KeyRepeatDelay * 1.25f, g.IO.KeyRepeatRate * 2.00f);
    if (mode == ImGuiInputReadMode_RepeatFast)
        return (float)CalcTypematicRepeatAmount(t - g.IO.DeltaTime, t, g.IO.KeyRepeatDelay * 0.72f, g.IO.KeyRepeatRate * 0.30f);
    return 0.0f;
}

ImVec2 ImGui::GetNavInputAmount2d(ImGuiNavDirSourceFlags dir_sources, ImGuiInputReadMode mode, float slow_factor, float fast_factor)
{
    ImVec2 delta(0.0f, 0.0f);
    if (dir_sources & ImGuiNavDirSourceFlags_Keyboard)
        delta += ImVec2(GetNavInputAmount(ImGuiNavInput_KeyRight_, mode) - GetNavInputAmount(ImGuiNavInput_KeyLeft_, mode), GetNavInputAmount(ImGuiNavInput_KeyDown_, mode) - GetNavInputAmount(ImGuiNavInput_KeyUp_, mode));
    if (dir_sources & ImGuiNavDirSourceFlags_PadDPad)
        delta += ImVec2(GetNavInputAmount(ImGuiNavInput_DpadRight, mode) - GetNavInputAmount(ImGuiNavInput_DpadLeft, mode), GetNavInputAmount(ImGuiNavInput_DpadDown, mode) - GetNavInputAmount(ImGuiNavInput_DpadUp, mode));
    if (dir_sources & ImGuiNavDirSourceFlags_PadLStick)
        delta += ImVec2(GetNavInputAmount(ImGuiNavInput_LStickRight, mode) - GetNavInputAmount(ImGuiNavInput_LStickLeft, mode), GetNavInputAmount(ImGuiNavInput_LStickDown, mode) - GetNavInputAmount(ImGuiNavInput_LStickUp, mode));
    if (slow_factor != 0.0f && IsNavInputDown(ImGuiNavInput_TweakSlow))
        delta *= slow_factor;
    if (fast_factor != 0.0f && IsNavInputDown(ImGuiNavInput_TweakFast))
        delta *= fast_factor;
    return delta;
}

static void ImGui::NavUpdate()
{
    ImGuiContext& g = *GImGui;
    g.IO.WantSetMousePos = false;
    g.NavWrapRequestWindow = NULL;
    g.NavWrapRequestFlags = ImGuiNavMoveFlags_None;
#if 0
    if (g.NavScoringCount > 0) IMGUI_DEBUG_LOG("NavScoringCount %d for '%s' layer %d (Init:%d, Move:%d)\n", g.FrameCount, g.NavScoringCount, g.NavWindow ? g.NavWindow->Name : "NULL", g.NavLayer, g.NavInitRequest || g.NavInitResultId != 0, g.NavMoveRequest);
#endif

    // Set input source as Gamepad when buttons are pressed (as some features differs when used with Gamepad vs Keyboard)
    // (do it before we map Keyboard input!)
    bool nav_keyboard_active = (g.IO.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard) != 0;
    bool nav_gamepad_active = (g.IO.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) != 0 && (g.IO.BackendFlags & ImGuiBackendFlags_HasGamepad) != 0;
    if (nav_gamepad_active)
        if (g.IO.NavInputs[ImGuiNavInput_Activate] > 0.0f || g.IO.NavInputs[ImGuiNavInput_Input] > 0.0f || g.IO.NavInputs[ImGuiNavInput_Cancel] > 0.0f || g.IO.NavInputs[ImGuiNavInput_Menu] > 0.0f)
            g.NavInputSource = ImGuiInputSource_NavGamepad;

    // Update Keyboard->Nav inputs mapping
    if (nav_keyboard_active)
    {
#define NAV_MAP_KEY(_KEY, _NAV_INPUT)  do { if (IsKeyDown(g.IO.KeyMap[_KEY])) { g.IO.NavInputs[_NAV_INPUT] = 1.0f; g.NavInputSource = ImGuiInputSource_NavKeyboard; } } while (0)
        NAV_MAP_KEY(ImGuiKey_Space, ImGuiNavInput_Activate);
        NAV_MAP_KEY(ImGuiKey_Enter, ImGuiNavInput_Input);
        NAV_MAP_KEY(ImGuiKey_Escape, ImGuiNavInput_Cancel);
        NAV_MAP_KEY(ImGuiKey_LeftArrow, ImGuiNavInput_KeyLeft_);
        NAV_MAP_KEY(ImGuiKey_RightArrow, ImGuiNavInput_KeyRight_);
        NAV_MAP_KEY(ImGuiKey_UpArrow, ImGuiNavInput_KeyUp_);
        NAV_MAP_KEY(ImGuiKey_DownArrow, ImGuiNavInput_KeyDown_);
        if (g.IO.KeyCtrl)
            g.IO.NavInputs[ImGuiNavInput_TweakSlow] = 1.0f;
        if (g.IO.KeyShift)
            g.IO.NavInputs[ImGuiNavInput_TweakFast] = 1.0f;
        if (g.IO.KeyAlt && !g.IO.KeyCtrl) // AltGR is Alt+Ctrl, also even on keyboards without AltGR we don't want Alt+Ctrl to open menu.
            g.IO.NavInputs[ImGuiNavInput_KeyMenu_] = 1.0f;
#undef NAV_MAP_KEY
    }
    memcpy(g.IO.NavInputsDownDurationPrev, g.IO.NavInputsDownDuration, sizeof(g.IO.NavInputsDownDuration));
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.NavInputs); i++)
        g.IO.NavInputsDownDuration[i] = (g.IO.NavInputs[i] > 0.0f) ? (g.IO.NavInputsDownDuration[i] < 0.0f ? 0.0f : g.IO.NavInputsDownDuration[i] + g.IO.DeltaTime) : -1.0f;

    // Process navigation init request (select first/default focus)
    // In very rare cases g.NavWindow may be null (e.g. clearing focus after requesting an init request, which does happen when releasing Alt while clicking on void)
    if (g.NavInitResultId != 0 && (!g.NavDisableHighlight || g.NavInitRequestFromMove) && g.NavWindow)
    {
        // Apply result from previous navigation init request (will typically select the first item, unless SetItemDefaultFocus() has been called)
        //IMGUI_DEBUG_LOG("[Nav] Apply NavInitRequest result: 0x%08X Layer %d in \"%s\"\n", g.NavInitResultId, g.NavLayer, g.NavWindow->Name);
        if (g.NavInitRequestFromMove)
            SetNavIDWithRectRel(g.NavInitResultId, g.NavLayer, 0, g.NavInitResultRectRel);
        else
            SetNavID(g.NavInitResultId, g.NavLayer, 0);
        g.NavWindow->NavRectRel[g.NavLayer] = g.NavInitResultRectRel;
    }
    g.NavInitRequest = false;
    g.NavInitRequestFromMove = false;
    g.NavInitResultId = 0;
    g.NavJustMovedToId = 0;

    // Process navigation move request
    if (g.NavMoveRequest)
        NavUpdateMoveResult();

    // When a forwarded move request failed, we restore the highlight that we disabled during the forward frame
    if (g.NavMoveRequestForward == ImGuiNavForward_ForwardActive)
    {
        IM_ASSERT(g.NavMoveRequest);
        if (g.NavMoveResultLocal.ID == 0 && g.NavMoveResultOther.ID == 0)
            g.NavDisableHighlight = false;
        g.NavMoveRequestForward = ImGuiNavForward_None;
    }

    // Apply application mouse position movement, after we had a chance to process move request result.
    if (g.NavMousePosDirty && g.NavIdIsAlive)
    {
        // Set mouse position given our knowledge of the navigated item position from last frame
        if ((g.IO.ConfigFlags & ImGuiConfigFlags_NavEnableSetMousePos) && (g.IO.BackendFlags & ImGuiBackendFlags_HasSetMousePos))
        {
            if (!g.NavDisableHighlight && g.NavDisableMouseHover && g.NavWindow)
            {
                g.IO.MousePos = g.IO.MousePosPrev = NavCalcPreferredRefPos();
                g.IO.WantSetMousePos = true;
            }
        }
        g.NavMousePosDirty = false;
    }
    g.NavIdIsAlive = false;
    g.NavJustTabbedId = 0;
    IM_ASSERT(g.NavLayer == 0 || g.NavLayer == 1);

    // Store our return window (for returning from Layer 1 to Layer 0) and clear it as soon as we step back in our own Layer 0
    if (g.NavWindow)
        NavSaveLastChildNavWindowIntoParent(g.NavWindow);
    if (g.NavWindow && g.NavWindow->NavLastChildNavWindow != NULL && g.NavLayer == ImGuiNavLayer_Main)
        g.NavWindow->NavLastChildNavWindow = NULL;

    // Update CTRL+TAB and Windowing features (hold Square to move/resize/etc.)
    NavUpdateWindowing();

    // Set output flags for user application
    g.IO.NavActive = (nav_keyboard_active || nav_gamepad_active) && g.NavWindow && !(g.NavWindow->Flags & ImGuiWindowFlags_NoNavInputs);
    g.IO.NavVisible = (g.IO.NavActive && g.NavId != 0 && !g.NavDisableHighlight) || (g.NavWindowingTarget != NULL);

    // Process NavCancel input (to close a popup, get back to parent, clear focus)
    if (IsNavInputTest(ImGuiNavInput_Cancel, ImGuiInputReadMode_Pressed))
    {
        if (g.ActiveId != 0)
        {
            if (!IsActiveIdUsingNavInput(ImGuiNavInput_Cancel))
                ClearActiveID();
        } else if (g.NavWindow && (g.NavWindow->Flags & ImGuiWindowFlags_ChildWindow) && !(g.NavWindow->Flags & ImGuiWindowFlags_Popup) && g.NavWindow->ParentWindow)
        {
            // Exit child window
            ImGuiWindow* child_window = g.NavWindow;
            ImGuiWindow* parent_window = g.NavWindow->ParentWindow;
            IM_ASSERT(child_window->ChildId != 0);
            FocusWindow(parent_window);
            SetNavID(child_window->ChildId, 0, 0);
            // Reassigning with same value, we're being explicit here.
            g.NavIdIsAlive = false;     // -V1048
            if (g.NavDisableMouseHover)
                g.NavMousePosDirty = true;
        } else if (g.OpenPopupStack.Size > 0)
        {
            // Close open popup/menu
            if (!(g.OpenPopupStack.back().Window->Flags & ImGuiWindowFlags_Modal))
                ClosePopupToLevel(g.OpenPopupStack.Size - 1, true);
        } else if (g.NavLayer != ImGuiNavLayer_Main)
        {
            // Leave the "menu" layer
            NavRestoreLayer(ImGuiNavLayer_Main);
        } else
        {
            // Clear NavLastId for popups but keep it for regular child window so we can leave one and come back where we were
            if (g.NavWindow && ((g.NavWindow->Flags & ImGuiWindowFlags_Popup) || !(g.NavWindow->Flags & ImGuiWindowFlags_ChildWindow)))
                g.NavWindow->NavLastIds[0] = 0;
            g.NavId = g.NavFocusScopeId = 0;
        }
    }

    // Process manual activation request
    g.NavActivateId = g.NavActivateDownId = g.NavActivatePressedId = g.NavInputId = 0;
    if (g.NavId != 0 && !g.NavDisableHighlight && !g.NavWindowingTarget && g.NavWindow && !(g.NavWindow->Flags & ImGuiWindowFlags_NoNavInputs))
    {
        bool activate_down = IsNavInputDown(ImGuiNavInput_Activate);
        bool activate_pressed = activate_down && IsNavInputTest(ImGuiNavInput_Activate, ImGuiInputReadMode_Pressed);
        if (g.ActiveId == 0 && activate_pressed)
            g.NavActivateId = g.NavId;
        if ((g.ActiveId == 0 || g.ActiveId == g.NavId) && activate_down)
            g.NavActivateDownId = g.NavId;
        if ((g.ActiveId == 0 || g.ActiveId == g.NavId) && activate_pressed)
            g.NavActivatePressedId = g.NavId;
        if ((g.ActiveId == 0 || g.ActiveId == g.NavId) && IsNavInputTest(ImGuiNavInput_Input, ImGuiInputReadMode_Pressed))
            g.NavInputId = g.NavId;
    }
    if (g.NavWindow && (g.NavWindow->Flags & ImGuiWindowFlags_NoNavInputs))
        g.NavDisableHighlight = true;
    if (g.NavActivateId != 0)
        IM_ASSERT(g.NavActivateDownId == g.NavActivateId);
    g.NavMoveRequest = false;

    // Process programmatic activation request
    if (g.NavNextActivateId != 0)
        g.NavActivateId = g.NavActivateDownId = g.NavActivatePressedId = g.NavInputId = g.NavNextActivateId;
    g.NavNextActivateId = 0;

    // Initiate directional inputs request
    if (g.NavMoveRequestForward == ImGuiNavForward_None)
    {
        g.NavMoveDir = ImGuiDir_None;
        g.NavMoveRequestFlags = ImGuiNavMoveFlags_None;
        if (g.NavWindow && !g.NavWindowingTarget && !(g.NavWindow->Flags & ImGuiWindowFlags_NoNavInputs))
        {
            const ImGuiInputReadMode read_mode = ImGuiInputReadMode_Repeat;
            if (!IsActiveIdUsingNavDir(ImGuiDir_Left) && (IsNavInputTest(ImGuiNavInput_DpadLeft, read_mode) || IsNavInputTest(ImGuiNavInput_KeyLeft_, read_mode))) { g.NavMoveDir = ImGuiDir_Left; }
            if (!IsActiveIdUsingNavDir(ImGuiDir_Right) && (IsNavInputTest(ImGuiNavInput_DpadRight, read_mode) || IsNavInputTest(ImGuiNavInput_KeyRight_, read_mode))) { g.NavMoveDir = ImGuiDir_Right; }
            if (!IsActiveIdUsingNavDir(ImGuiDir_Up) && (IsNavInputTest(ImGuiNavInput_DpadUp, read_mode) || IsNavInputTest(ImGuiNavInput_KeyUp_, read_mode))) { g.NavMoveDir = ImGuiDir_Up; }
            if (!IsActiveIdUsingNavDir(ImGuiDir_Down) && (IsNavInputTest(ImGuiNavInput_DpadDown, read_mode) || IsNavInputTest(ImGuiNavInput_KeyDown_, read_mode))) { g.NavMoveDir = ImGuiDir_Down; }
        }
        g.NavMoveClipDir = g.NavMoveDir;
    } else
    {
        // Forwarding previous request (which has been modified, e.g. wrap around menus rewrite the requests with a starting rectangle at the other side of the window)
        // (Preserve g.NavMoveRequestFlags, g.NavMoveClipDir which were set by the NavMoveRequestForward() function)
        IM_ASSERT(g.NavMoveDir != ImGuiDir_None && g.NavMoveClipDir != ImGuiDir_None);
        IM_ASSERT(g.NavMoveRequestForward == ImGuiNavForward_ForwardQueued);
        g.NavMoveRequestForward = ImGuiNavForward_ForwardActive;
    }

    // Update PageUp/PageDown/Home/End scroll
    // FIXME-NAV: Consider enabling those keys even without the master ImGuiConfigFlags_NavEnableKeyboard flag?
    float nav_scoring_rect_offset_y = 0.0f;
    if (nav_keyboard_active)
        nav_scoring_rect_offset_y = NavUpdatePageUpPageDown();

    // If we initiate a movement request and have no current NavId, we initiate a InitDefautRequest that will be used as a fallback if the direction fails to find a match
    if (g.NavMoveDir != ImGuiDir_None)
    {
        g.NavMoveRequest = true;
        g.NavMoveRequestKeyMods = g.IO.KeyMods;
        g.NavMoveDirLast = g.NavMoveDir;
    }
    if (g.NavMoveRequest && g.NavId == 0)
    {
        //IMGUI_DEBUG_LOG("[Nav] NavInitRequest from move, window \"%s\", layer=%d\n", g.NavWindow->Name, g.NavLayer);
        g.NavInitRequest = g.NavInitRequestFromMove = true;
        // Reassigning with same value, we're being explicit here.
        g.NavInitResultId = 0;     // -V1048
        g.NavDisableHighlight = false;
    }
    NavUpdateAnyRequestFlag();

    // Scrolling
    if (g.NavWindow && !(g.NavWindow->Flags & ImGuiWindowFlags_NoNavInputs) && !g.NavWindowingTarget)
    {
        // *Fallback* manual-scroll with Nav directional keys when window has no navigable item
        ImGuiWindow* window = g.NavWindow;
        const float scroll_speed = IM_ROUND(window->CalcFontSize() * 100 * g.IO.DeltaTime); // We need round the scrolling speed because sub-pixel scroll isn't reliably supported.
        if (window->DC.NavLayerActiveMask == 0x00 && window->DC.NavHasScroll && g.NavMoveRequest)
        {
            if (g.NavMoveDir == ImGuiDir_Left || g.NavMoveDir == ImGuiDir_Right)
                SetScrollX(window, ImFloor(window->Scroll.x + ((g.NavMoveDir == ImGuiDir_Left) ? -1.0f : +1.0f) * scroll_speed));
            if (g.NavMoveDir == ImGuiDir_Up || g.NavMoveDir == ImGuiDir_Down)
                SetScrollY(window, ImFloor(window->Scroll.y + ((g.NavMoveDir == ImGuiDir_Up) ? -1.0f : +1.0f) * scroll_speed));
        }

        // *Normal* Manual scroll with NavScrollXXX keys
        // Next movement request will clamp the NavId reference rectangle to the visible area, so navigation will resume within those bounds.
        ImVec2 scroll_dir = GetNavInputAmount2d(ImGuiNavDirSourceFlags_PadLStick, ImGuiInputReadMode_Down, 1.0f / 10.0f, 10.0f);
        if (scroll_dir.x != 0.0f && window->ScrollbarX)
        {
            SetScrollX(window, ImFloor(window->Scroll.x + scroll_dir.x * scroll_speed));
            g.NavMoveFromClampedRefRect = true;
        }
        if (scroll_dir.y != 0.0f)
        {
            SetScrollY(window, ImFloor(window->Scroll.y + scroll_dir.y * scroll_speed));
            g.NavMoveFromClampedRefRect = true;
        }
    }

    // Reset search results
    g.NavMoveResultLocal.Clear();
    g.NavMoveResultLocalVisibleSet.Clear();
    g.NavMoveResultOther.Clear();

    // When we have manually scrolled (without using navigation) and NavId becomes out of bounds, we project its bounding box to the visible area to restart navigation within visible items
    if (g.NavMoveRequest && g.NavMoveFromClampedRefRect && g.NavLayer == ImGuiNavLayer_Main)
    {
        ImGuiWindow* window = g.NavWindow;
        ImRect window_rect_rel(window->InnerRect.Min - window->Pos - ImVec2(1, 1), window->InnerRect.Max - window->Pos + ImVec2(1, 1));
        if (!window_rect_rel.Contains(window->NavRectRel[g.NavLayer]))
        {
            float pad = window->CalcFontSize() * 0.5f;
            window_rect_rel.Expand(ImVec2(-ImMin(window_rect_rel.GetWidth(), pad), -ImMin(window_rect_rel.GetHeight(), pad))); // Terrible approximation for the intent of starting navigation from first fully visible item
            window->NavRectRel[g.NavLayer].ClipWith(window_rect_rel);
            g.NavId = g.NavFocusScopeId = 0;
        }
        g.NavMoveFromClampedRefRect = false;
    }

    // For scoring we use a single segment on the left side our current item bounding box (not touching the edge to avoid box overlap with zero-spaced items)
    ImRect nav_rect_rel = (g.NavWindow && !g.NavWindow->NavRectRel[g.NavLayer].IsInverted()) ? g.NavWindow->NavRectRel[g.NavLayer] : ImRect(0, 0, 0, 0);
    g.NavScoringRect = g.NavWindow ? ImRect(g.NavWindow->Pos + nav_rect_rel.Min, g.NavWindow->Pos + nav_rect_rel.Max) : GetViewportRect();
    g.NavScoringRect.TranslateY(nav_scoring_rect_offset_y);
    g.NavScoringRect.Min.x = ImMin(g.NavScoringRect.Min.x + 1.0f, g.NavScoringRect.Max.x);
    g.NavScoringRect.Max.x = g.NavScoringRect.Min.x;
    IM_ASSERT(!g.NavScoringRect.IsInverted()); // Ensure if we have a finite, non-inverted bounding box here will allows us to remove extraneous ImFabs() calls in NavScoreItem().
    //GetForegroundDrawList()->AddRect(g.NavScoringRectScreen.Min, g.NavScoringRectScreen.Max, IM_COL32(255,200,0,255)); // [DEBUG]
    g.NavScoringCount = 0;
#if IMGUI_DEBUG_NAV_RECTS
    if (g.NavWindow)
    {
        ImDrawList* draw_list = GetForegroundDrawList(g.NavWindow);
        if (1) { for (int layer = 0; layer < 2; layer++) draw_list->AddRect(g.NavWindow->Pos + g.NavWindow->NavRectRel[layer].Min, g.NavWindow->Pos + g.NavWindow->NavRectRel[layer].Max, IM_COL32(255, 200, 0, 255)); } // [DEBUG]
        if (1) { ImU32 col = (!g.NavWindow->Hidden) ? IM_COL32(255, 0, 255, 255) : IM_COL32(255, 0, 0, 255); ImVec2 p = NavCalcPreferredRefPos(); char buf[32]; ImFormatString(buf, 32, "%d", g.NavLayer); draw_list->AddCircleFilled(p, 3.0f, col); draw_list->AddText(NULL, 13.0f, p + ImVec2(8, -4), col, buf); }
    }
#endif
}

// Apply result from previous frame navigation directional move request
static void ImGui::NavUpdateMoveResult()
{
    ImGuiContext& g = *GImGui;
    if (g.NavMoveResultLocal.ID == 0 && g.NavMoveResultOther.ID == 0)
    {
        // In a situation when there is no results but NavId != 0, re-enable the Navigation highlight (because g.NavId is not considered as a possible result)
        if (g.NavId != 0)
        {
            g.NavDisableHighlight = false;
            g.NavDisableMouseHover = true;
        }
        return;
    }

    // Select which result to use
    ImGuiNavMoveResult* result = (g.NavMoveResultLocal.ID != 0) ? &g.NavMoveResultLocal : &g.NavMoveResultOther;

    // PageUp/PageDown behavior first jumps to the bottom/top mostly visible item, _otherwise_ use the result from the previous/next page.
    if (g.NavMoveRequestFlags & ImGuiNavMoveFlags_AlsoScoreVisibleSet)
        if (g.NavMoveResultLocalVisibleSet.ID != 0 && g.NavMoveResultLocalVisibleSet.ID != g.NavId)
            result = &g.NavMoveResultLocalVisibleSet;

    // Maybe entering a flattened child from the outside? In this case solve the tie using the regular scoring rules.
    if (result != &g.NavMoveResultOther && g.NavMoveResultOther.ID != 0 && g.NavMoveResultOther.Window->ParentWindow == g.NavWindow)
        if ((g.NavMoveResultOther.DistBox < result->DistBox) || (g.NavMoveResultOther.DistBox == result->DistBox && g.NavMoveResultOther.DistCenter < result->DistCenter))
            result = &g.NavMoveResultOther;
    IM_ASSERT(g.NavWindow && result->Window);

    // Scroll to keep newly navigated item fully into view.
    if (g.NavLayer == ImGuiNavLayer_Main)
    {
        ImVec2 delta_scroll;
        if (g.NavMoveRequestFlags & ImGuiNavMoveFlags_ScrollToEdge)
        {
            float scroll_target = (g.NavMoveDir == ImGuiDir_Up) ? result->Window->ScrollMax.y : 0.0f;
            delta_scroll.y = result->Window->Scroll.y - scroll_target;
            SetScrollY(result->Window, scroll_target);
        } else
        {
            ImRect rect_abs = ImRect(result->RectRel.Min + result->Window->Pos, result->RectRel.Max + result->Window->Pos);
            delta_scroll = ScrollToBringRectIntoView(result->Window, rect_abs);
        }

        // Offset our result position so mouse position can be applied immediately after in NavUpdate()
        result->RectRel.TranslateX(-delta_scroll.x);
        result->RectRel.TranslateY(-delta_scroll.y);
    }

    ClearActiveID();
    g.NavWindow = result->Window;
    if (g.NavId != result->ID)
    {
        // Don't set NavJustMovedToId if just landed on the same spot (which may happen with ImGuiNavMoveFlags_AllowCurrentNavId)
        g.NavJustMovedToId = result->ID;
        g.NavJustMovedToFocusScopeId = result->FocusScopeId;
        g.NavJustMovedToKeyMods = g.NavMoveRequestKeyMods;
    }
    SetNavIDWithRectRel(result->ID, g.NavLayer, result->FocusScopeId, result->RectRel);
    g.NavMoveFromClampedRefRect = false;
}

// Handle PageUp/PageDown/Home/End keys
static float ImGui::NavUpdatePageUpPageDown()
{
    ImGuiContext& g = *GImGui;
    if (g.NavMoveDir != ImGuiDir_None || g.NavWindow == NULL)
        return 0.0f;
    if ((g.NavWindow->Flags & ImGuiWindowFlags_NoNavInputs) || g.NavWindowingTarget != NULL || g.NavLayer != ImGuiNavLayer_Main)
        return 0.0f;

    ImGuiWindow* window = g.NavWindow;
    const bool page_up_held = IsKeyDown(g.IO.KeyMap[ImGuiKey_PageUp]) && !IsActiveIdUsingKey(ImGuiKey_PageUp);
    const bool page_down_held = IsKeyDown(g.IO.KeyMap[ImGuiKey_PageDown]) && !IsActiveIdUsingKey(ImGuiKey_PageDown);
    const bool home_pressed = IsKeyPressed(g.IO.KeyMap[ImGuiKey_Home]) && !IsActiveIdUsingKey(ImGuiKey_Home);
    const bool end_pressed = IsKeyPressed(g.IO.KeyMap[ImGuiKey_End]) && !IsActiveIdUsingKey(ImGuiKey_End);
    if (page_up_held != page_down_held || home_pressed != end_pressed) // If either (not both) are pressed
    {
        if (window->DC.NavLayerActiveMask == 0x00 && window->DC.NavHasScroll)
        {
            // Fallback manual-scroll when window has no navigable item
            if (IsKeyPressed(g.IO.KeyMap[ImGuiKey_PageUp], true))
                SetScrollY(window, window->Scroll.y - window->InnerRect.GetHeight());
            else if (IsKeyPressed(g.IO.KeyMap[ImGuiKey_PageDown], true))
                SetScrollY(window, window->Scroll.y + window->InnerRect.GetHeight());
            else if (home_pressed)
                SetScrollY(window, 0.0f);
            else if (end_pressed)
                SetScrollY(window, window->ScrollMax.y);
        } else
        {
            ImRect& nav_rect_rel = window->NavRectRel[g.NavLayer];
            const float page_offset_y = ImMax(0.0f, window->InnerRect.GetHeight() - window->CalcFontSize() * 1.0f + nav_rect_rel.GetHeight());
            float nav_scoring_rect_offset_y = 0.0f;
            if (IsKeyPressed(g.IO.KeyMap[ImGuiKey_PageUp], true))
            {
                nav_scoring_rect_offset_y = -page_offset_y;
                g.NavMoveDir = ImGuiDir_Down; // Because our scoring rect is offset up, we request the down direction (so we can always land on the last item)
                g.NavMoveClipDir = ImGuiDir_Up;
                g.NavMoveRequestFlags = ImGuiNavMoveFlags_AllowCurrentNavId | ImGuiNavMoveFlags_AlsoScoreVisibleSet;
            } else if (IsKeyPressed(g.IO.KeyMap[ImGuiKey_PageDown], true))
            {
                nav_scoring_rect_offset_y = +page_offset_y;
                g.NavMoveDir = ImGuiDir_Up; // Because our scoring rect is offset down, we request the up direction (so we can always land on the last item)
                g.NavMoveClipDir = ImGuiDir_Down;
                g.NavMoveRequestFlags = ImGuiNavMoveFlags_AllowCurrentNavId | ImGuiNavMoveFlags_AlsoScoreVisibleSet;
            } else if (home_pressed)
            {
                // FIXME-NAV: handling of Home/End is assuming that the top/bottom most item will be visible with Scroll.y == 0/ScrollMax.y
                // Scrolling will be handled via the ImGuiNavMoveFlags_ScrollToEdge flag, we don't scroll immediately to avoid scrolling happening before nav result.
                // Preserve current horizontal position if we have any.
                nav_rect_rel.Min.y = nav_rect_rel.Max.y = -window->Scroll.y;
                if (nav_rect_rel.IsInverted())
                    nav_rect_rel.Min.x = nav_rect_rel.Max.x = 0.0f;
                g.NavMoveDir = ImGuiDir_Down;
                g.NavMoveRequestFlags = ImGuiNavMoveFlags_AllowCurrentNavId | ImGuiNavMoveFlags_ScrollToEdge;
            } else if (end_pressed)
            {
                nav_rect_rel.Min.y = nav_rect_rel.Max.y = window->ScrollMax.y + window->SizeFull.y - window->Scroll.y;
                if (nav_rect_rel.IsInverted())
                    nav_rect_rel.Min.x = nav_rect_rel.Max.x = 0.0f;
                g.NavMoveDir = ImGuiDir_Up;
                g.NavMoveRequestFlags = ImGuiNavMoveFlags_AllowCurrentNavId | ImGuiNavMoveFlags_ScrollToEdge;
            }
            return nav_scoring_rect_offset_y;
        }
    }
    return 0.0f;
}

static void ImGui::NavEndFrame()
{
    ImGuiContext& g = *GImGui;

    // Show CTRL+TAB list window
    if (g.NavWindowingTarget != NULL)
        NavUpdateWindowingOverlay();

    // Perform wrap-around in menus
    ImGuiWindow* window = g.NavWrapRequestWindow;
    ImGuiNavMoveFlags move_flags = g.NavWrapRequestFlags;
    if (window != NULL && g.NavWindow == window && NavMoveRequestButNoResultYet() && g.NavMoveRequestForward == ImGuiNavForward_None && g.NavLayer == ImGuiNavLayer_Main)
    {
        IM_ASSERT(move_flags != 0); // No points calling this with no wrapping
        ImRect bb_rel = window->NavRectRel[0];

        ImGuiDir clip_dir = g.NavMoveDir;
        if (g.NavMoveDir == ImGuiDir_Left && (move_flags & (ImGuiNavMoveFlags_WrapX | ImGuiNavMoveFlags_LoopX)))
        {
            bb_rel.Min.x = bb_rel.Max.x =
                ImMax(window->SizeFull.x, window->ContentSize.x + window->WindowPadding.x * 2.0f) - window->Scroll.x;
            if (move_flags & ImGuiNavMoveFlags_WrapX)
            {
                bb_rel.TranslateY(-bb_rel.GetHeight());
                clip_dir = ImGuiDir_Up;
            }
            NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
        }
        if (g.NavMoveDir == ImGuiDir_Right && (move_flags & (ImGuiNavMoveFlags_WrapX | ImGuiNavMoveFlags_LoopX)))
        {
            bb_rel.Min.x = bb_rel.Max.x = -window->Scroll.x;
            if (move_flags & ImGuiNavMoveFlags_WrapX)
            {
                bb_rel.TranslateY(+bb_rel.GetHeight());
                clip_dir = ImGuiDir_Down;
            }
            NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
        }
        if (g.NavMoveDir == ImGuiDir_Up && (move_flags & (ImGuiNavMoveFlags_WrapY | ImGuiNavMoveFlags_LoopY)))
        {
            bb_rel.Min.y = bb_rel.Max.y =
                ImMax(window->SizeFull.y, window->ContentSize.y + window->WindowPadding.y * 2.0f) - window->Scroll.y;
            if (move_flags & ImGuiNavMoveFlags_WrapY)
            {
                bb_rel.TranslateX(-bb_rel.GetWidth());
                clip_dir = ImGuiDir_Left;
            }
            NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
        }
        if (g.NavMoveDir == ImGuiDir_Down && (move_flags & (ImGuiNavMoveFlags_WrapY | ImGuiNavMoveFlags_LoopY)))
        {
            bb_rel.Min.y = bb_rel.Max.y = -window->Scroll.y;
            if (move_flags & ImGuiNavMoveFlags_WrapY)
            {
                bb_rel.TranslateX(+bb_rel.GetWidth());
                clip_dir = ImGuiDir_Right;
            }
            NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
        }
    }
}

static int ImGui::FindWindowFocusIndex(ImGuiWindow* window) // FIXME-OPT O(N)
{
    ImGuiContext& g = *GImGui;
    for (int i = g.WindowsFocusOrder.Size - 1; i >= 0; i--)
        if (g.WindowsFocusOrder[i] == window)
            return i;
    return -1;
}

static ImGuiWindow* FindWindowNavFocusable(int i_start, int i_stop, int dir) // FIXME-OPT O(N)
{
    ImGuiContext& g = *GImGui;
    for (int i = i_start; i >= 0 && i < g.WindowsFocusOrder.Size && i != i_stop; i += dir)
        if (ImGui::IsWindowNavFocusable(g.WindowsFocusOrder[i]))
            return g.WindowsFocusOrder[i];
    return NULL;
}

static void NavUpdateWindowingHighlightWindow(int focus_change_dir)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.NavWindowingTarget);
    if (g.NavWindowingTarget->Flags & ImGuiWindowFlags_Modal)
        return;

    const int i_current = ImGui::FindWindowFocusIndex(g.NavWindowingTarget);
    ImGuiWindow* window_target = FindWindowNavFocusable(i_current + focus_change_dir, -INT_MAX, focus_change_dir);
    if (!window_target)
        window_target = FindWindowNavFocusable((focus_change_dir < 0) ? (g.WindowsFocusOrder.Size - 1) : 0, i_current, focus_change_dir);
    if (window_target) // Don't reset windowing target if there's a single window in the list
        g.NavWindowingTarget = g.NavWindowingTargetAnim = window_target;
    g.NavWindowingToggleLayer = false;
}

// Windowing management mode
// Keyboard: CTRL+Tab (change focus/move/resize), Alt (toggle menu layer)
// Gamepad:  Hold Menu/Square (change focus/move/resize), Tap Menu/Square (toggle menu layer)
static void ImGui::NavUpdateWindowing()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* apply_focus_window = NULL;
    bool apply_toggle_layer = false;

    ImGuiWindow* modal_window = GetTopMostPopupModal();
    if (modal_window != NULL)
    {
        g.NavWindowingTarget = NULL;
        return;
    }

    // Fade out
    if (g.NavWindowingTargetAnim && g.NavWindowingTarget == NULL)
    {
        g.NavWindowingHighlightAlpha = ImMax(g.NavWindowingHighlightAlpha - g.IO.DeltaTime * 10.0f, 0.0f);
        if (g.DimBgRatio <= 0.0f && g.NavWindowingHighlightAlpha <= 0.0f)
            g.NavWindowingTargetAnim = NULL;
    }

    // Start CTRL-TAB or Square+L/R window selection
    bool start_windowing_with_gamepad = !g.NavWindowingTarget && IsNavInputTest(ImGuiNavInput_Menu, ImGuiInputReadMode_Pressed);
    bool start_windowing_with_keyboard = !g.NavWindowingTarget && g.IO.KeyCtrl && IsKeyPressedMap(ImGuiKey_Tab) && (g.IO.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard);
    if (start_windowing_with_gamepad || start_windowing_with_keyboard)
        if (ImGuiWindow* window = g.NavWindow ? g.NavWindow : FindWindowNavFocusable(g.WindowsFocusOrder.Size - 1, -INT_MAX, -1))
        {
            g.NavWindowingTarget = g.NavWindowingTargetAnim = window->RootWindow; // FIXME-DOCK: Will need to use RootWindowDockStop
            g.NavWindowingTimer = g.NavWindowingHighlightAlpha = 0.0f;
            g.NavWindowingToggleLayer = start_windowing_with_keyboard ? false : true;
            g.NavInputSource = start_windowing_with_keyboard ? ImGuiInputSource_NavKeyboard : ImGuiInputSource_NavGamepad;
        }

    // Gamepad update
    g.NavWindowingTimer += g.IO.DeltaTime;
    if (g.NavWindowingTarget && g.NavInputSource == ImGuiInputSource_NavGamepad)
    {
        // Highlight only appears after a brief time holding the button, so that a fast tap on PadMenu (to toggle NavLayer) doesn't add visual noise
        g.NavWindowingHighlightAlpha = ImMax(g.NavWindowingHighlightAlpha, ImSaturate((g.NavWindowingTimer - NAV_WINDOWING_HIGHLIGHT_DELAY) / 0.05f));

        // Select window to focus
        const int focus_change_dir = (int)IsNavInputTest(ImGuiNavInput_FocusPrev, ImGuiInputReadMode_RepeatSlow) - (int)IsNavInputTest(ImGuiNavInput_FocusNext, ImGuiInputReadMode_RepeatSlow);
        if (focus_change_dir != 0)
        {
            NavUpdateWindowingHighlightWindow(focus_change_dir);
            g.NavWindowingHighlightAlpha = 1.0f;
        }

        // Single press toggles NavLayer, long press with L/R apply actual focus on release (until then the window was merely rendered top-most)
        if (!IsNavInputDown(ImGuiNavInput_Menu))
        {
            g.NavWindowingToggleLayer &= (g.NavWindowingHighlightAlpha < 1.0f); // Once button was held long enough we don't consider it a tap-to-toggle-layer press anymore.
            if (g.NavWindowingToggleLayer && g.NavWindow)
                apply_toggle_layer = true;
            else if (!g.NavWindowingToggleLayer)
                apply_focus_window = g.NavWindowingTarget;
            g.NavWindowingTarget = NULL;
        }
    }

    // Keyboard: Focus
    if (g.NavWindowingTarget && g.NavInputSource == ImGuiInputSource_NavKeyboard)
    {
        // Visuals only appears after a brief time after pressing TAB the first time, so that a fast CTRL+TAB doesn't add visual noise
        g.NavWindowingHighlightAlpha = ImMax(g.NavWindowingHighlightAlpha, ImSaturate((g.NavWindowingTimer - NAV_WINDOWING_HIGHLIGHT_DELAY) / 0.05f)); // 1.0f
        if (IsKeyPressedMap(ImGuiKey_Tab, true))
            NavUpdateWindowingHighlightWindow(g.IO.KeyShift ? +1 : -1);
        if (!g.IO.KeyCtrl)
            apply_focus_window = g.NavWindowingTarget;
    }

    // Keyboard: Press and Release ALT to toggle menu layer
    // FIXME: We lack an explicit IO variable for "is the imgui window focused", so compare mouse validity to detect the common case of back-end clearing releases all keys on ALT-TAB
    if (IsNavInputTest(ImGuiNavInput_KeyMenu_, ImGuiInputReadMode_Pressed))
        g.NavWindowingToggleLayer = true;
    if ((g.ActiveId == 0 || g.ActiveIdAllowOverlap) && g.NavWindowingToggleLayer && IsNavInputTest(ImGuiNavInput_KeyMenu_, ImGuiInputReadMode_Released))
        if (IsMousePosValid(&g.IO.MousePos) == IsMousePosValid(&g.IO.MousePosPrev))
            apply_toggle_layer = true;

    // Move window
    if (g.NavWindowingTarget && !(g.NavWindowingTarget->Flags & ImGuiWindowFlags_NoMove))
    {
        ImVec2 move_delta;
        if (g.NavInputSource == ImGuiInputSource_NavKeyboard && !g.IO.KeyShift)
            move_delta = GetNavInputAmount2d(ImGuiNavDirSourceFlags_Keyboard, ImGuiInputReadMode_Down);
        if (g.NavInputSource == ImGuiInputSource_NavGamepad)
            move_delta = GetNavInputAmount2d(ImGuiNavDirSourceFlags_PadLStick, ImGuiInputReadMode_Down);
        if (move_delta.x != 0.0f || move_delta.y != 0.0f)
        {
            const float NAV_MOVE_SPEED = 800.0f;
            const float move_speed = ImFloor(NAV_MOVE_SPEED * g.IO.DeltaTime * ImMin(g.IO.DisplayFramebufferScale.x, g.IO.DisplayFramebufferScale.y)); // FIXME: Doesn't code variable framerate very well
            SetWindowPos(g.NavWindowingTarget->RootWindow, g.NavWindowingTarget->RootWindow->Pos + move_delta * move_speed, ImGuiCond_Always);
            g.NavDisableMouseHover = true;
            MarkIniSettingsDirty(g.NavWindowingTarget);
        }
    }

    // Apply final focus
    if (apply_focus_window && (g.NavWindow == NULL || apply_focus_window != g.NavWindow->RootWindow))
    {
        ClearActiveID();
        g.NavDisableHighlight = false;
        g.NavDisableMouseHover = true;
        apply_focus_window = NavRestoreLastChildNavWindow(apply_focus_window);
        ClosePopupsOverWindow(apply_focus_window, false);
        FocusWindow(apply_focus_window);
        if (apply_focus_window->NavLastIds[0] == 0)
            NavInitWindow(apply_focus_window, false);

        // If the window only has a menu layer, select it directly
        if (apply_focus_window->DC.NavLayerActiveMask == (1 << ImGuiNavLayer_Menu))
            g.NavLayer = ImGuiNavLayer_Menu;
    }
    if (apply_focus_window)
        g.NavWindowingTarget = NULL;

    // Apply menu/layer toggle
    if (apply_toggle_layer && g.NavWindow)
    {
        // Move to parent menu if necessary
        ImGuiWindow* new_nav_window = g.NavWindow;
        while (new_nav_window->ParentWindow
            && (new_nav_window->DC.NavLayerActiveMask & (1 << ImGuiNavLayer_Menu)) == 0
            && (new_nav_window->Flags & ImGuiWindowFlags_ChildWindow) != 0
            && (new_nav_window->Flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_ChildMenu)) == 0)
            new_nav_window = new_nav_window->ParentWindow;
        if (new_nav_window != g.NavWindow)
        {
            ImGuiWindow* old_nav_window = g.NavWindow;
            FocusWindow(new_nav_window);
            new_nav_window->NavLastChildNavWindow = old_nav_window;
        }
        g.NavDisableHighlight = false;
        g.NavDisableMouseHover = true;

        // When entering a regular menu bar with the Alt key, we always reinitialize the navigation ID.
        const ImGuiNavLayer new_nav_layer = (g.NavWindow->DC.NavLayerActiveMask & (1 << ImGuiNavLayer_Menu)) ? (ImGuiNavLayer)((int)g.NavLayer ^ 1) : ImGuiNavLayer_Main;
        NavRestoreLayer(new_nav_layer);
    }
}

// Window has already passed the IsWindowNavFocusable()
static const char* GetFallbackWindowNameForWindowingList(ImGuiWindow* window)
{
    if (window->Flags & ImGuiWindowFlags_Popup)
        return "(Popup)";
    if ((window->Flags & ImGuiWindowFlags_MenuBar) && strcmp(window->Name, "##MainMenuBar") == 0)
        return "(Main menu bar)";
    return "(Untitled)";
}

// Overlay displayed when using CTRL+TAB. Called by EndFrame().
void ImGui::NavUpdateWindowingOverlay()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.NavWindowingTarget != NULL);

    if (g.NavWindowingTimer < NAV_WINDOWING_LIST_APPEAR_DELAY)
        return;

    if (g.NavWindowingListWindow == NULL)
        g.NavWindowingListWindow = FindWindowByName("###NavWindowingList");
    SetNextWindowSizeConstraints(ImVec2(g.IO.DisplaySize.x * 0.20f, g.IO.DisplaySize.y * 0.20f), ImVec2(FLT_MAX, FLT_MAX));
    SetNextWindowPos(g.IO.DisplaySize * 0.5f, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.WindowPadding * 2.0f);
    Begin("###NavWindowingList", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    for (int n = g.WindowsFocusOrder.Size - 1; n >= 0; n--)
    {
        ImGuiWindow* window = g.WindowsFocusOrder[n];
        if (!IsWindowNavFocusable(window))
            continue;
        const char* label = window->Name;
        if (label == FindRenderedTextEnd(label))
            label = GetFallbackWindowNameForWindowingList(window);
        Selectable(label, g.NavWindowingTarget == window);
    }
    End();
    PopStyleVar();
}

//-----------------------------------------------------------------------------
// [SECTION] DRAG AND DROP
//-----------------------------------------------------------------------------

void ImGui::ClearDragDrop()
{
    ImGuiContext& g = *GImGui;
    g.DragDropActive = false;
    g.DragDropPayload.Clear();
    g.DragDropAcceptFlags = ImGuiDragDropFlags_None;
    g.DragDropAcceptIdCurr = g.DragDropAcceptIdPrev = 0;
    g.DragDropAcceptIdCurrRectSurface = FLT_MAX;
    g.DragDropAcceptFrameCount = -1;

    g.DragDropPayloadBufHeap.clear();
    memset(&g.DragDropPayloadBufLocal, 0, sizeof(g.DragDropPayloadBufLocal));
}

// Call when current ID is active.
// When this returns true you need to: a) call SetDragDropPayload() exactly once, b) you may render the payload visual/description, c) call EndDragDropSource()
bool ImGui::BeginDragDropSource(ImGuiDragDropFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    bool source_drag_active = false;
    ImGuiID source_id = 0;
    ImGuiID source_parent_id = 0;
    ImGuiMouseButton mouse_button = ImGuiMouseButton_Left;
    if (!(flags & ImGuiDragDropFlags_SourceExtern))
    {
        source_id = window->DC.LastItemId;
        if (source_id != 0 && g.ActiveId != source_id) // Early out for most common case
            return false;
        if (g.IO.MouseDown[mouse_button] == false)
            return false;

        if (source_id == 0)
        {
            // If you want to use BeginDragDropSource() on an item with no unique identifier for interaction, such as Text() or Image(), you need to:
            // A) Read the explanation below, B) Use the ImGuiDragDropFlags_SourceAllowNullID flag, C) Swallow your programmer pride.
            if (!(flags & ImGuiDragDropFlags_SourceAllowNullID))
            {
                IM_ASSERT(0);
                return false;
            }

            // Early out
            if ((window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect) == 0 && (g.ActiveId == 0 || g.ActiveIdWindow != window))
                return false;

            // Magic fallback (=somehow reprehensible) to handle items with no assigned ID, e.g. Text(), Image()
            // We build a throwaway ID based on current ID stack + relative AABB of items in window.
            // THE IDENTIFIER WON'T SURVIVE ANY REPOSITIONING OF THE WIDGET, so if your widget moves your dragging operation will be canceled.
            // We don't need to maintain/call ClearActiveID() as releasing the button will early out this function and trigger !ActiveIdIsAlive.
            source_id = window->DC.LastItemId = window->GetIDFromRectangle(window->DC.LastItemRect);
            bool is_hovered = ItemHoverable(window->DC.LastItemRect, source_id);
            if (is_hovered && g.IO.MouseClicked[mouse_button])
            {
                SetActiveID(source_id, window);
                FocusWindow(window);
            }
            if (g.ActiveId == source_id) // Allow the underlying widget to display/return hovered during the mouse release frame, else we would get a flicker.
                g.ActiveIdAllowOverlap = is_hovered;
        } else
        {
            g.ActiveIdAllowOverlap = false;
        }
        if (g.ActiveId != source_id)
            return false;
        source_parent_id = window->IDStack.back();
        source_drag_active = IsMouseDragging(mouse_button);

        // Disable navigation and key inputs while dragging
        g.ActiveIdUsingNavDirMask = ~(ImU32)0;
        g.ActiveIdUsingNavInputMask = ~(ImU32)0;
        g.ActiveIdUsingKeyInputMask = ~(ImU64)0;
    } else
    {
        window = NULL;
        source_id = ImHashStr("#SourceExtern");
        source_drag_active = true;
    }

    if (source_drag_active)
    {
        if (!g.DragDropActive)
        {
            IM_ASSERT(source_id != 0);
            ClearDragDrop();
            ImGuiPayload& payload = g.DragDropPayload;
            payload.SourceId = source_id;
            payload.SourceParentId = source_parent_id;
            g.DragDropActive = true;
            g.DragDropSourceFlags = flags;
            g.DragDropMouseButton = mouse_button;
        }
        g.DragDropSourceFrameCount = g.FrameCount;
        g.DragDropWithinSource = true;

        if (!(flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
        {
            // Target can request the Source to not display its tooltip (we use a dedicated flag to make this request explicit)
            // We unfortunately can't just modify the source flags and skip the call to BeginTooltip, as caller may be emitting contents.
            BeginTooltip();
            if (g.DragDropAcceptIdPrev && (g.DragDropAcceptFlags & ImGuiDragDropFlags_AcceptNoPreviewTooltip))
            {
                ImGuiWindow* tooltip_window = g.CurrentWindow;
                tooltip_window->SkipItems = true;
                tooltip_window->HiddenFramesCanSkipItems = 1;
            }
        }

        if (!(flags & ImGuiDragDropFlags_SourceNoDisableHover) && !(flags & ImGuiDragDropFlags_SourceExtern))
            window->DC.LastItemStatusFlags &= ~ImGuiItemStatusFlags_HoveredRect;

        return true;
    }
    return false;
}

void ImGui::EndDragDropSource()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.DragDropActive);
    IM_ASSERT(g.DragDropWithinSource && "Not after a BeginDragDropSource()?");

    if (!(g.DragDropSourceFlags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
        EndTooltip();

    // Discard the drag if have not called SetDragDropPayload()
    if (g.DragDropPayload.DataFrameCount == -1)
        ClearDragDrop();
    g.DragDropWithinSource = false;
}

// Use 'cond' to choose to submit payload on drag start or every frame
bool ImGui::SetDragDropPayload(const char* type, const void* data, size_t data_size, ImGuiCond cond)
{
    ImGuiContext& g = *GImGui;
    ImGuiPayload& payload = g.DragDropPayload;
    if (cond == 0)
        cond = ImGuiCond_Always;

    IM_ASSERT(type != NULL);
    IM_ASSERT(strlen(type) < IM_ARRAYSIZE(payload.DataType) && "Payload type can be at most 32 characters long");
    IM_ASSERT((data != NULL && data_size > 0) || (data == NULL && data_size == 0));
    IM_ASSERT(cond == ImGuiCond_Always || cond == ImGuiCond_Once);
    IM_ASSERT(payload.SourceId != 0);                               // Not called between BeginDragDropSource() and EndDragDropSource()

    if (cond == ImGuiCond_Always || payload.DataFrameCount == -1)
    {
        // Copy payload
        ImStrncpy(payload.DataType, type, IM_ARRAYSIZE(payload.DataType));
        g.DragDropPayloadBufHeap.resize(0);
        if (data_size > sizeof(g.DragDropPayloadBufLocal))
        {
            // Store in heap
            g.DragDropPayloadBufHeap.resize((int)data_size);
            payload.Data = g.DragDropPayloadBufHeap.Data;
            memcpy(payload.Data, data, data_size);
        } else if (data_size > 0)
        {
            // Store locally
            memset(&g.DragDropPayloadBufLocal, 0, sizeof(g.DragDropPayloadBufLocal));
            payload.Data = g.DragDropPayloadBufLocal;
            memcpy(payload.Data, data, data_size);
        } else
        {
            payload.Data = NULL;
        }
        payload.DataSize = (int)data_size;
    }
    payload.DataFrameCount = g.FrameCount;

    return (g.DragDropAcceptFrameCount == g.FrameCount) || (g.DragDropAcceptFrameCount == g.FrameCount - 1);
}

bool ImGui::BeginDragDropTargetCustom(const ImRect& bb, ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    if (!g.DragDropActive)
        return false;

    ImGuiWindow* window = g.CurrentWindow;
    if (g.HoveredWindow == NULL || window->RootWindow != g.HoveredWindow->RootWindow)
        return false;
    IM_ASSERT(id != 0);
    if (!IsMouseHoveringRect(bb.Min, bb.Max) || (id == g.DragDropPayload.SourceId))
        return false;
    if (window->SkipItems)
        return false;

    IM_ASSERT(g.DragDropWithinTarget == false);
    g.DragDropTargetRect = bb;
    g.DragDropTargetId = id;
    g.DragDropWithinTarget = true;
    return true;
}

// We don't use BeginDragDropTargetCustom() and duplicate its code because:
// 1) we use LastItemRectHoveredRect which handles items that pushes a temporarily clip rectangle in their code. Calling BeginDragDropTargetCustom(LastItemRect) would not handle them.
// 2) and it's faster. as this code may be very frequently called, we want to early out as fast as we can.
// Also note how the HoveredWindow test is positioned differently in both functions (in both functions we optimize for the cheapest early out case)
bool ImGui::BeginDragDropTarget()
{
    ImGuiContext& g = *GImGui;
    if (!g.DragDropActive)
        return false;

    ImGuiWindow* window = g.CurrentWindow;
    if (!(window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect))
        return false;
    if (g.HoveredWindow == NULL || window->RootWindow != g.HoveredWindow->RootWindow)
        return false;

    const ImRect& display_rect = (window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HasDisplayRect) ? window->DC.LastItemDisplayRect : window->DC.LastItemRect;
    ImGuiID id = window->DC.LastItemId;
    if (id == 0)
        id = window->GetIDFromRectangle(display_rect);
    if (g.DragDropPayload.SourceId == id)
        return false;

    IM_ASSERT(g.DragDropWithinTarget == false);
    g.DragDropTargetRect = display_rect;
    g.DragDropTargetId = id;
    g.DragDropWithinTarget = true;
    return true;
}

bool ImGui::IsDragDropPayloadBeingAccepted()
{
    ImGuiContext& g = *GImGui;
    return g.DragDropActive && g.DragDropAcceptIdPrev != 0;
}

const ImGuiPayload* ImGui::AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiPayload& payload = g.DragDropPayload;
    IM_ASSERT(g.DragDropActive);                        // Not called between BeginDragDropTarget() and EndDragDropTarget() ?
    IM_ASSERT(payload.DataFrameCount != -1);            // Forgot to call EndDragDropTarget() ?
    if (type != NULL && !payload.IsDataType(type))
        return NULL;

    // Accept smallest drag target bounding box, this allows us to nest drag targets conveniently without ordering constraints.
    // NB: We currently accept NULL id as target. However, overlapping targets requires a unique ID to function!
    const bool was_accepted_previously = (g.DragDropAcceptIdPrev == g.DragDropTargetId);
    ImRect r = g.DragDropTargetRect;
    float r_surface = r.GetWidth() * r.GetHeight();
    if (r_surface < g.DragDropAcceptIdCurrRectSurface)
    {
        g.DragDropAcceptFlags = flags;
        g.DragDropAcceptIdCurr = g.DragDropTargetId;
        g.DragDropAcceptIdCurrRectSurface = r_surface;
    }

    // Render default drop visuals
    payload.Preview = was_accepted_previously;
    flags |= (g.DragDropSourceFlags & ImGuiDragDropFlags_AcceptNoDrawDefaultRect); // Source can also inhibit the preview (useful for external sources that lives for 1 frame)
    if (!(flags & ImGuiDragDropFlags_AcceptNoDrawDefaultRect) && payload.Preview)
    {
        // FIXME-DRAG: Settle on a proper default visuals for drop target.
        r.Expand(3.5f);
        bool push_clip_rect = !window->ClipRect.Contains(r);
        if (push_clip_rect) window->DrawList->PushClipRect(r.Min - ImVec2(1, 1), r.Max + ImVec2(1, 1));
        window->DrawList->AddRect(r.Min, r.Max, GetColorU32(ImGuiCol_DragDropTarget), 0.0f, ~0, 2.0f);
        if (push_clip_rect) window->DrawList->PopClipRect();
    }

    g.DragDropAcceptFrameCount = g.FrameCount;
    payload.Delivery = was_accepted_previously && !IsMouseDown(g.DragDropMouseButton); // For extern drag sources affecting os window focus, it's easier to just test !IsMouseDown() instead of IsMouseReleased()
    if (!payload.Delivery && !(flags & ImGuiDragDropFlags_AcceptBeforeDelivery))
        return NULL;

    return &payload;
}

const ImGuiPayload* ImGui::GetDragDropPayload()
{
    ImGuiContext& g = *GImGui;
    return g.DragDropActive ? &g.DragDropPayload : NULL;
}

// We don't really use/need this now, but added it for the sake of consistency and because we might need it later.
void ImGui::EndDragDropTarget()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.DragDropActive);
    IM_ASSERT(g.DragDropWithinTarget);
    g.DragDropWithinTarget = false;
}

//-----------------------------------------------------------------------------
// [SECTION] LOGGING/CAPTURING
//-----------------------------------------------------------------------------
// All text output from the interface can be captured into tty/file/clipboard.
// By default, tree nodes are automatically opened during logging.
//-----------------------------------------------------------------------------

// Pass text data straight to log (without being displayed)
void ImGui::LogText(const char* fmt, ...)
{
    ImGuiContext& g = *GImGui;
    if (!g.LogEnabled)
        return;

    va_list args;
    va_start(args, fmt);
    if (g.LogFile)
    {
        g.LogBuffer.Buf.resize(0);
        g.LogBuffer.appendfv(fmt, args);
        ImFileWrite(g.LogBuffer.c_str(), sizeof(char), (ImU64)g.LogBuffer.size(), g.LogFile);
    } else
    {
        g.LogBuffer.appendfv(fmt, args);
    }
    va_end(args);
}

// Internal version that takes a position to decide on newline placement and pad items according to their depth.
// We split text into individual lines to add current tree level padding
void ImGui::LogRenderedText(const ImVec2* ref_pos, const char* text, const char* text_end)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    if (!text_end)
        text_end = FindRenderedTextEnd(text, text_end);

    const bool log_new_line = ref_pos && (ref_pos->y > g.LogLinePosY + 1);
    if (ref_pos)
        g.LogLinePosY = ref_pos->y;
    if (log_new_line)
        g.LogLineFirstItem = true;

    const char* text_remaining = text;
    if (g.LogDepthRef > window->DC.TreeDepth)  // Re-adjust padding if we have popped out of our starting depth
        g.LogDepthRef = window->DC.TreeDepth;
    const int tree_depth = (window->DC.TreeDepth - g.LogDepthRef);
    for (;;)
    {
        // Split the string. Each new line (after a '\n') is followed by spacing corresponding to the current depth of our log entry.
        // We don't add a trailing \n to allow a subsequent item on the same line to be captured.
        const char* line_start = text_remaining;
        const char* line_end = ImStreolRange(line_start, text_end);
        const bool is_first_line = (line_start == text);
        const bool is_last_line = (line_end == text_end);
        if (!is_last_line || (line_start != line_end))
        {
            const int char_count = (int)(line_end - line_start);
            if (log_new_line || !is_first_line)
                LogText(IM_NEWLINE "%*s%.*s", tree_depth * 4, "", char_count, line_start);
            else if (g.LogLineFirstItem)
                LogText("%*s%.*s", tree_depth * 4, "", char_count, line_start);
            else
                LogText(" %.*s", char_count, line_start);
            g.LogLineFirstItem = false;
        } else if (log_new_line)
        {
            // An empty "" string at a different Y position should output a carriage return.
            LogText(IM_NEWLINE);
            break;
        }

        if (is_last_line)
            break;
        text_remaining = line_end + 1;
    }
}

// Start logging/capturing text output
void ImGui::LogBegin(ImGuiLogType type, int auto_open_depth)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    IM_ASSERT(g.LogEnabled == false);
    IM_ASSERT(g.LogFile == NULL);
    IM_ASSERT(g.LogBuffer.empty());
    g.LogEnabled = true;
    g.LogType = type;
    g.LogDepthRef = window->DC.TreeDepth;
    g.LogDepthToExpand = ((auto_open_depth >= 0) ? auto_open_depth : g.LogDepthToExpandDefault);
    g.LogLinePosY = FLT_MAX;
    g.LogLineFirstItem = true;
}

void ImGui::LogToTTY(int auto_open_depth)
{
    ImGuiContext& g = *GImGui;
    if (g.LogEnabled)
        return;
    IM_UNUSED(auto_open_depth);
#ifndef IMGUI_DISABLE_TTY_FUNCTIONS
    LogBegin(ImGuiLogType_TTY, auto_open_depth);
    g.LogFile = stdout;
#endif
}

// Start logging/capturing text output to given file
void ImGui::LogToFile(int auto_open_depth, const char* filename)
{
    ImGuiContext& g = *GImGui;
    if (g.LogEnabled)
        return;

    // FIXME: We could probably open the file in text mode "at", however note that clipboard/buffer logging will still
    // be subject to outputting OS-incompatible carriage return if within strings the user doesn't use IM_NEWLINE.
    // By opening the file in binary mode "ab" we have consistent output everywhere.
    if (!filename)
        filename = g.IO.LogFilename;
    if (!filename || !filename[0])
        return;
    ImFileHandle f = ImFileOpen(filename, "ab");
    if (!f)
    {
        IM_ASSERT(0);
        return;
    }

    LogBegin(ImGuiLogType_File, auto_open_depth);
    g.LogFile = f;
}

// Start logging/capturing text output to clipboard
void ImGui::LogToClipboard(int auto_open_depth)
{
    ImGuiContext& g = *GImGui;
    if (g.LogEnabled)
        return;
    LogBegin(ImGuiLogType_Clipboard, auto_open_depth);
}

void ImGui::LogToBuffer(int auto_open_depth)
{
    ImGuiContext& g = *GImGui;
    if (g.LogEnabled)
        return;
    LogBegin(ImGuiLogType_Buffer, auto_open_depth);
}

void ImGui::LogFinish()
{
    ImGuiContext& g = *GImGui;
    if (!g.LogEnabled)
        return;

    LogText(IM_NEWLINE);
    switch (g.LogType)
    {
    case ImGuiLogType_TTY:
#ifndef IMGUI_DISABLE_TTY_FUNCTIONS
        fflush(g.LogFile);
#endif
        break;
    case ImGuiLogType_File:
        ImFileClose(g.LogFile);
        break;
    case ImGuiLogType_Buffer:
        break;
    case ImGuiLogType_Clipboard:
        if (!g.LogBuffer.empty())
            SetClipboardText(g.LogBuffer.begin());
        break;
    case ImGuiLogType_None:
        IM_ASSERT(0);
        break;
    }

    g.LogEnabled = false;
    g.LogType = ImGuiLogType_None;
    g.LogFile = NULL;
    g.LogBuffer.clear();
}

// Helper to display logging buttons
// FIXME-OBSOLETE: We should probably obsolete this and let the user have their own helper (this is one of the oldest function alive!)
void ImGui::LogButtons()
{
    ImGuiContext& g = *GImGui;

    PushID("LogButtons");
#ifndef IMGUI_DISABLE_TTY_FUNCTIONS
    const bool log_to_tty = Button("Log To TTY"); SameLine();
#else
    const bool log_to_tty = false;
#endif
    const bool log_to_file = Button("Log To File"); SameLine();
    const bool log_to_clipboard = Button("Log To Clipboard"); SameLine();
    PushAllowKeyboardFocus(false);
    SetNextItemWidth(80.0f);
    SliderInt("Default Depth", &g.LogDepthToExpandDefault, 0, 9, NULL);
    PopAllowKeyboardFocus();
    PopID();

    // Start logging at the end of the function so that the buttons don't appear in the log
    if (log_to_tty)
        LogToTTY();
    if (log_to_file)
        LogToFile();
    if (log_to_clipboard)
        LogToClipboard();
}

//-----------------------------------------------------------------------------
// [SECTION] SETTINGS
//-----------------------------------------------------------------------------
// - UpdateSettings() [Internal]
// - MarkIniSettingsDirty() [Internal]
// - CreateNewWindowSettings() [Internal]
// - FindWindowSettings() [Internal]
// - FindOrCreateWindowSettings() [Internal]
// - FindSettingsHandler() [Internal]
// - ClearIniSettings() [Internal]
// - LoadIniSettingsFromDisk()
// - LoadIniSettingsFromMemory()
// - SaveIniSettingsToDisk()
// - SaveIniSettingsToMemory()
// - WindowSettingsHandler_***() [Internal]
//-----------------------------------------------------------------------------

// Called by NewFrame()
void ImGui::UpdateSettings()
{
    // Load settings on first frame (if not explicitly loaded manually before)
    ImGuiContext& g = *GImGui;
    if (!g.SettingsLoaded)
    {
        IM_ASSERT(g.SettingsWindows.empty());
        if (g.IO.IniFilename)
            LoadIniSettingsFromDisk(g.IO.IniFilename);
        g.SettingsLoaded = true;
    }

    // Save settings (with a delay after the last modification, so we don't spam disk too much)
    if (g.SettingsDirtyTimer > 0.0f)
    {
        g.SettingsDirtyTimer -= g.IO.DeltaTime;
        if (g.SettingsDirtyTimer <= 0.0f)
        {
            if (g.IO.IniFilename != NULL)
                SaveIniSettingsToDisk(g.IO.IniFilename);
            else
                g.IO.WantSaveIniSettings = true;  // Let user know they can call SaveIniSettingsToMemory(). user will need to clear io.WantSaveIniSettings themselves.
            g.SettingsDirtyTimer = 0.0f;
        }
    }
}

void ImGui::MarkIniSettingsDirty()
{
    ImGuiContext& g = *GImGui;
    if (g.SettingsDirtyTimer <= 0.0f)
        g.SettingsDirtyTimer = g.IO.IniSavingRate;
}

void ImGui::MarkIniSettingsDirty(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    if (!(window->Flags & ImGuiWindowFlags_NoSavedSettings))
        if (g.SettingsDirtyTimer <= 0.0f)
            g.SettingsDirtyTimer = g.IO.IniSavingRate;
}

ImGuiWindowSettings* ImGui::CreateNewWindowSettings(const char* name)
{
    ImGuiContext& g = *GImGui;

#if !IMGUI_DEBUG_INI_SETTINGS
    // Skip to the "###" marker if any. We don't skip past to match the behavior of GetID()
    // Preserve the full string when IMGUI_DEBUG_INI_SETTINGS is set to make .ini inspection easier.
    if (const char* p = strstr(name, "###"))
        name = p;
#endif
    const size_t name_len = strlen(name);

    // Allocate chunk
    const size_t chunk_size = sizeof(ImGuiWindowSettings) + name_len + 1;
    ImGuiWindowSettings* settings = g.SettingsWindows.alloc_chunk(chunk_size);
    IM_PLACEMENT_NEW(settings) ImGuiWindowSettings();
    settings->ID = ImHashStr(name, name_len);
    memcpy(settings->GetName(), name, name_len + 1);   // Store with zero terminator

    return settings;
}

ImGuiWindowSettings* ImGui::FindWindowSettings(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != NULL; settings = g.SettingsWindows.next_chunk(settings))
        if (settings->ID == id)
            return settings;
    return NULL;
}

ImGuiWindowSettings* ImGui::FindOrCreateWindowSettings(const char* name)
{
    if (ImGuiWindowSettings* settings = FindWindowSettings(ImHashStr(name)))
        return settings;
    return CreateNewWindowSettings(name);
}

ImGuiSettingsHandler* ImGui::FindSettingsHandler(const char* type_name)
{
    ImGuiContext& g = *GImGui;
    const ImGuiID type_hash = ImHashStr(type_name);
    for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
        if (g.SettingsHandlers[handler_n].TypeHash == type_hash)
            return &g.SettingsHandlers[handler_n];
    return NULL;
}

void ImGui::ClearIniSettings()
{
    ImGuiContext& g = *GImGui;
    g.SettingsIniData.clear();
    for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
        if (g.SettingsHandlers[handler_n].ClearAllFn)
            g.SettingsHandlers[handler_n].ClearAllFn(&g, &g.SettingsHandlers[handler_n]);
}

void ImGui::LoadIniSettingsFromDisk(const char* ini_filename)
{
    size_t file_data_size = 0;
    char* file_data = (char*)ImFileLoadToMemory(ini_filename, "rb", &file_data_size);
    if (!file_data)
        return;
    LoadIniSettingsFromMemory(file_data, (size_t)file_data_size);
    IM_FREE(file_data);
}

// Zero-tolerance, no error reporting, cheap .ini parsing
void ImGui::LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.Initialized);
    //IM_ASSERT(!g.WithinFrameScope && "Cannot be called between NewFrame() and EndFrame()");
    //IM_ASSERT(g.SettingsLoaded == false && g.FrameCount == 0);

    // For user convenience, we allow passing a non zero-terminated string (hence the ini_size parameter).
    // For our convenience and to make the code simpler, we'll also write zero-terminators within the buffer. So let's create a writable copy..
    if (ini_size == 0)
        ini_size = strlen(ini_data);
    g.SettingsIniData.Buf.resize((int)ini_size + 1);
    char* const buf = g.SettingsIniData.Buf.Data;
    char* const buf_end = buf + ini_size;
    memcpy(buf, ini_data, ini_size);
    buf_end[0] = 0;

    // Call pre-read handlers
    // Some types will clear their data (e.g. dock information) some types will allow merge/override (window)
    for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
        if (g.SettingsHandlers[handler_n].ReadInitFn)
            g.SettingsHandlers[handler_n].ReadInitFn(&g, &g.SettingsHandlers[handler_n]);

    void* entry_data = NULL;
    ImGuiSettingsHandler* entry_handler = NULL;

    char* line_end = NULL;
    for (char* line = buf; line < buf_end; line = line_end + 1)
    {
        // Skip new lines markers, then find end of the line
        while (*line == '\n' || *line == '\r')
            line++;
        line_end = line;
        while (line_end < buf_end && *line_end != '\n' && *line_end != '\r')
            line_end++;
        line_end[0] = 0;
        if (line[0] == ';')
            continue;
        if (line[0] == '[' && line_end > line && line_end[-1] == ']')
        {
            // Parse "[Type][Name]". Note that 'Name' can itself contains [] characters, which is acceptable with the current format and parsing code.
            line_end[-1] = 0;
            const char* name_end = line_end - 1;
            const char* type_start = line + 1;
            char* type_end = (char*)(void*)ImStrchrRange(type_start, name_end, ']');
            const char* name_start = type_end ? ImStrchrRange(type_end + 1, name_end, '[') : NULL;
            if (!type_end || !name_start)
                continue;
            *type_end = 0; // Overwrite first ']'
            name_start++;  // Skip second '['
            entry_handler = FindSettingsHandler(type_start);
            entry_data = entry_handler ? entry_handler->ReadOpenFn(&g, entry_handler, name_start) : NULL;
        } else if (entry_handler != NULL && entry_data != NULL)
        {
            // Let type handler parse the line
            entry_handler->ReadLineFn(&g, entry_handler, entry_data, line);
        }
    }
    g.SettingsLoaded = true;

    // [DEBUG] Restore untouched copy so it can be browsed in Metrics (not strictly necessary)
    memcpy(buf, ini_data, ini_size);

    // Call post-read handlers
    for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
        if (g.SettingsHandlers[handler_n].ApplyAllFn)
            g.SettingsHandlers[handler_n].ApplyAllFn(&g, &g.SettingsHandlers[handler_n]);
}

void ImGui::SaveIniSettingsToDisk(const char* ini_filename)
{
    ImGuiContext& g = *GImGui;
    g.SettingsDirtyTimer = 0.0f;
    if (!ini_filename)
        return;

    size_t ini_data_size = 0;
    const char* ini_data = SaveIniSettingsToMemory(&ini_data_size);
    ImFileHandle f = ImFileOpen(ini_filename, "wt");
    if (!f)
        return;
    ImFileWrite(ini_data, sizeof(char), ini_data_size, f);
    ImFileClose(f);
}

// Call registered handlers (e.g. SettingsHandlerWindow_WriteAll() + custom handlers) to write their stuff into a text buffer
const char* ImGui::SaveIniSettingsToMemory(size_t* out_size)
{
    ImGuiContext& g = *GImGui;
    g.SettingsDirtyTimer = 0.0f;
    g.SettingsIniData.Buf.resize(0);
    g.SettingsIniData.Buf.push_back(0);
    for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
    {
        ImGuiSettingsHandler* handler = &g.SettingsHandlers[handler_n];
        handler->WriteAllFn(&g, handler, &g.SettingsIniData);
    }
    if (out_size)
        *out_size = (size_t)g.SettingsIniData.size();
    return g.SettingsIniData.c_str();
}

static void WindowSettingsHandler_ClearAll(ImGuiContext* ctx, ImGuiSettingsHandler*)
{
    ImGuiContext& g = *ctx;
    for (int i = 0; i != g.Windows.Size; i++)
        g.Windows[i]->SettingsOffset = -1;
    g.SettingsWindows.clear();
}

static void* WindowSettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name)
{
    ImGuiWindowSettings* settings = ImGui::FindOrCreateWindowSettings(name);
    ImGuiID id = settings->ID;
    *settings = ImGuiWindowSettings(); // Clear existing if recycling previous entry
    settings->ID = id;
    settings->WantApply = true;
    return (void*)settings;
}

static void WindowSettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line)
{
    ImGuiWindowSettings* settings = (ImGuiWindowSettings*)entry;
    int x, y;
    int i;
    if (sscanf(line, "Pos=%i,%i", &x, &y) == 2)         settings->Pos = ImVec2ih((short)x, (short)y);
    else if (sscanf(line, "Size=%i,%i", &x, &y) == 2)   settings->Size = ImVec2ih((short)x, (short)y);
    else if (sscanf(line, "Collapsed=%d", &i) == 1)     settings->Collapsed = (i != 0);
}

// Apply to existing windows (if any)
static void WindowSettingsHandler_ApplyAll(ImGuiContext* ctx, ImGuiSettingsHandler*)
{
    ImGuiContext& g = *ctx;
    for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != NULL; settings = g.SettingsWindows.next_chunk(settings))
        if (settings->WantApply)
        {
            if (ImGuiWindow* window = ImGui::FindWindowByID(settings->ID))
                ApplyWindowSettings(window, settings);
            settings->WantApply = false;
        }
}

static void WindowSettingsHandler_WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
{
    // Gather data from windows that were active during this session
    // (if a window wasn't opened in this session we preserve its settings)
    ImGuiContext& g = *ctx;
    for (int i = 0; i != g.Windows.Size; i++)
    {
        ImGuiWindow* window = g.Windows[i];
        if (window->Flags & ImGuiWindowFlags_NoSavedSettings)
            continue;

        ImGuiWindowSettings* settings = (window->SettingsOffset != -1) ? g.SettingsWindows.ptr_from_offset(window->SettingsOffset) : ImGui::FindWindowSettings(window->ID);
        if (!settings)
        {
            settings = ImGui::CreateNewWindowSettings(window->Name);
            window->SettingsOffset = g.SettingsWindows.offset_from_ptr(settings);
        }
        IM_ASSERT(settings->ID == window->ID);
        settings->Pos = ImVec2ih((short)window->Pos.x, (short)window->Pos.y);
        settings->Size = ImVec2ih((short)window->SizeFull.x, (short)window->SizeFull.y);
        settings->Collapsed = window->Collapsed;
    }

    // Write to text buffer
    buf->reserve(buf->size() + g.SettingsWindows.size() * 6); // ballpark reserve
    for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != NULL; settings = g.SettingsWindows.next_chunk(settings))
    {
        const char* settings_name = settings->GetName();
        buf->appendf("[%s][%s]\n", handler->TypeName, settings_name);
        buf->appendf("Pos=%d,%d\n", settings->Pos.x, settings->Pos.y);
        buf->appendf("Size=%d,%d\n", settings->Size.x, settings->Size.y);
        buf->appendf("Collapsed=%d\n", settings->Collapsed);
        buf->append("\n");
    }
}


//-----------------------------------------------------------------------------
// [SECTION] VIEWPORTS, PLATFORM WINDOWS
//-----------------------------------------------------------------------------

// (this section is filled in the 'docking' branch)


//-----------------------------------------------------------------------------
// [SECTION] DOCKING
//-----------------------------------------------------------------------------

// (this section is filled in the 'docking' branch)


//-----------------------------------------------------------------------------
// [SECTION] PLATFORM DEPENDENT HELPERS
//-----------------------------------------------------------------------------

#if defined(_WIN32) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS) && !defined(IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS)

#ifdef _MSC_VER
#pragma comment(lib, "user32")
#pragma comment(lib, "kernel32")
#endif

// Win32 clipboard implementation
// We use g.ClipboardHandlerData for temporary storage to ensure it is freed on Shutdown()
static const char* GetClipboardTextFn_DefaultImpl(void*)
{
    ImGuiContext& g = *GImGui;
    g.ClipboardHandlerData.clear();
    if (!::OpenClipboard(NULL))
        return NULL;
    HANDLE wbuf_handle = ::GetClipboardData(CF_UNICODETEXT);
    if (wbuf_handle == NULL)
    {
        ::CloseClipboard();
        return NULL;
    }
    if (const WCHAR* wbuf_global = (const WCHAR*)::GlobalLock(wbuf_handle))
    {
        int buf_len = ::WideCharToMultiByte(CP_UTF8, 0, wbuf_global, -1, NULL, 0, NULL, NULL);
        g.ClipboardHandlerData.resize(buf_len);
        ::WideCharToMultiByte(CP_UTF8, 0, wbuf_global, -1, g.ClipboardHandlerData.Data, buf_len, NULL, NULL);
    }
    ::GlobalUnlock(wbuf_handle);
    ::CloseClipboard();
    return g.ClipboardHandlerData.Data;
}

static void SetClipboardTextFn_DefaultImpl(void*, const char* text)
{
    if (!::OpenClipboard(NULL))
        return;
    const int wbuf_length = ::MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    HGLOBAL wbuf_handle = ::GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)wbuf_length * sizeof(WCHAR));
    if (wbuf_handle == NULL)
    {
        ::CloseClipboard();
        return;
    }
    WCHAR* wbuf_global = (WCHAR*)::GlobalLock(wbuf_handle);
    ::MultiByteToWideChar(CP_UTF8, 0, text, -1, wbuf_global, wbuf_length);
    ::GlobalUnlock(wbuf_handle);
    ::EmptyClipboard();
    if (::SetClipboardData(CF_UNICODETEXT, wbuf_handle) == NULL)
        ::GlobalFree(wbuf_handle);
    ::CloseClipboard();
}

#elif defined(__APPLE__) && TARGET_OS_OSX && defined(IMGUI_ENABLE_OSX_DEFAULT_CLIPBOARD_FUNCTIONS)

#include <Carbon/Carbon.h>  // Use old API to avoid need for separate .mm file
static PasteboardRef main_clipboard = 0;

// OSX clipboard implementation
// If you enable this you will need to add '-framework ApplicationServices' to your linker command-line!
static void SetClipboardTextFn_DefaultImpl(void*, const char* text)
{
    if (!main_clipboard)
        PasteboardCreate(kPasteboardClipboard, &main_clipboard);
    PasteboardClear(main_clipboard);
    CFDataRef cf_data = CFDataCreate(kCFAllocatorDefault, (const UInt8*)text, strlen(text));
    if (cf_data)
    {
        PasteboardPutItemFlavor(main_clipboard, (PasteboardItemID)1, CFSTR("public.utf8-plain-text"), cf_data, 0);
        CFRelease(cf_data);
    }
}

static const char* GetClipboardTextFn_DefaultImpl(void*)
{
    if (!main_clipboard)
        PasteboardCreate(kPasteboardClipboard, &main_clipboard);
    PasteboardSynchronize(main_clipboard);

    ItemCount item_count = 0;
    PasteboardGetItemCount(main_clipboard, &item_count);
    for (ItemCount i = 0; i < item_count; i++)
    {
        PasteboardItemID item_id = 0;
        PasteboardGetItemIdentifier(main_clipboard, i + 1, &item_id);
        CFArrayRef flavor_type_array = 0;
        PasteboardCopyItemFlavors(main_clipboard, item_id, &flavor_type_array);
        for (CFIndex j = 0, nj = CFArrayGetCount(flavor_type_array); j < nj; j++)
        {
            CFDataRef cf_data;
            if (PasteboardCopyItemFlavorData(main_clipboard, item_id, CFSTR("public.utf8-plain-text"), &cf_data) == noErr)
            {
                ImGuiContext& g = *GImGui;
                g.ClipboardHandlerData.clear();
                int length = (int)CFDataGetLength(cf_data);
                g.ClipboardHandlerData.resize(length + 1);
                CFDataGetBytes(cf_data, CFRangeMake(0, length), (UInt8*)g.ClipboardHandlerData.Data);
                g.ClipboardHandlerData[length] = 0;
                CFRelease(cf_data);
                return g.ClipboardHandlerData.Data;
            }
        }
    }
    return NULL;
}

#else

// Local Dear ImGui-only clipboard implementation, if user hasn't defined better clipboard handlers.
static const char* GetClipboardTextFn_DefaultImpl(void*)
{
    ImGuiContext& g = *GImGui;
    return g.ClipboardHandlerData.empty() ? NULL : g.ClipboardHandlerData.begin();
}

static void SetClipboardTextFn_DefaultImpl(void*, const char* text)
{
    ImGuiContext& g = *GImGui;
    g.ClipboardHandlerData.clear();
    const char* text_end = text + strlen(text);
    g.ClipboardHandlerData.resize((int)(text_end - text) + 1);
    memcpy(&g.ClipboardHandlerData[0], text, (size_t)(text_end - text));
    g.ClipboardHandlerData[(int)(text_end - text)] = 0;
}

#endif

// Win32 API IME support (for Asian languages, etc.)
#if defined(_WIN32) && !defined(__GNUC__) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS) && !defined(IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS)

#include <imm.h>
#ifdef _MSC_VER
#pragma comment(lib, "imm32")
#endif

static void ImeSetInputScreenPosFn_DefaultImpl(int x, int y)
{
    // Notify OS Input Method Editor of text input position
    ImGuiIO& io = ImGui::GetIO();
    if (HWND hwnd = (HWND)io.ImeWindowHandle)
        if (HIMC himc = ::ImmGetContext(hwnd))
        {
            COMPOSITIONFORM cf;
            cf.ptCurrentPos.x = x;
            cf.ptCurrentPos.y = y;
            cf.dwStyle = CFS_FORCE_POSITION;
            ::ImmSetCompositionWindow(himc, &cf);
            ::ImmReleaseContext(hwnd, himc);
        }
}

#else

static void ImeSetInputScreenPosFn_DefaultImpl(int, int) {}

#endif

//-----------------------------------------------------------------------------
// [SECTION] METRICS/DEBUG WINDOW
//-----------------------------------------------------------------------------

#ifndef IMGUI_DISABLE_METRICS_WINDOW
// Avoid naming collision with imgui_demo.cpp's HelpMarker() for unity builds.
static void MetricsHelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void ImGui::ShowMetricsWindow(bool* p_open)
{
    if (!ImGui::Begin("Dear ImGui Metrics", p_open))
    {
        ImGui::End();
        return;
    }

    // Debugging enums
    enum { WRT_OuterRect, WRT_OuterRectClipped, WRT_InnerRect, WRT_InnerClipRect, WRT_WorkRect, WRT_Content, WRT_ContentRegionRect, WRT_Count }; // Windows Rect Type
    const char* wrt_rects_names[WRT_Count] = { "OuterRect", "OuterRectClipped", "InnerRect", "InnerClipRect", "WorkRect", "Content", "ContentRegionRect" };
    enum { TRT_OuterRect, TRT_WorkRect, TRT_HostClipRect, TRT_InnerClipRect, TRT_BackgroundClipRect, TRT_ColumnsRect, TRT_ColumnsClipRect, TRT_ColumnsContentHeadersUsed, TRT_ColumnsContentHeadersIdeal, TRT_ColumnsContentRowsFrozen, TRT_ColumnsContentRowsUnfrozen, TRT_Count }; // Tables Rect Type
    const char* trt_rects_names[TRT_Count] = { "OuterRect", "WorkRect", "HostClipRect", "InnerClipRect", "BackgroundClipRect", "ColumnsRect", "ColumnsClipRect", "ColumnsContentHeadersUsed", "ColumnsContentHeadersIdeal", "ColumnsContentRowsFrozen", "ColumnsContentRowsUnfrozen" };

    // State
    static bool show_windows_rects = false;
    static int  show_windows_rect_type = WRT_WorkRect;
    static bool show_windows_begin_order = false;
    static bool show_tables_rects = false;
    static int  show_tables_rect_type = TRT_WorkRect;
    static bool show_drawcmd_mesh = true;
    static bool show_drawcmd_aabb = true;

    // Basic info
    ImGuiContext& g = *GImGui;
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
    ImGui::Text("%d active windows (%d visible)", io.MetricsActiveWindows, io.MetricsRenderWindows);
    ImGui::Text("%d active allocations", io.MetricsActiveAllocations);
    ImGui::Separator();

    // Helper functions to display common structures:
    // - NodeDrawList()
    // - NodeColumns()
    // - NodeWindow()
    // - NodeWindows()
    // - NodeTabBar()
    // - NodeStorage()
    struct Funcs
    {
        static ImRect GetWindowRect(ImGuiWindow* window, int rect_type)
        {
            if (rect_type == WRT_OuterRect) { return window->Rect(); } else if (rect_type == WRT_OuterRectClipped) { return window->OuterRectClipped; } else if (rect_type == WRT_InnerRect) { return window->InnerRect; } else if (rect_type == WRT_InnerClipRect) { return window->InnerClipRect; } else if (rect_type == WRT_WorkRect) { return window->WorkRect; } else if (rect_type == WRT_Content) { ImVec2 min = window->InnerRect.Min - window->Scroll + window->WindowPadding; return ImRect(min, min + window->ContentSize); } else if (rect_type == WRT_ContentRegionRect) { return window->ContentRegionRect; }
            IM_ASSERT(0);
            return ImRect();
        }

        static void NodeDrawCmdShowMeshAndBoundingBox(ImGuiWindow* window, const ImDrawList* draw_list, const ImDrawCmd* draw_cmd, int elem_offset, bool show_mesh, bool show_aabb)
        {
            IM_ASSERT(show_mesh || show_aabb);
            ImDrawList* fg_draw_list = GetForegroundDrawList(window); // Render additional visuals into the top-most draw list
            ImDrawIdx* idx_buffer = (draw_list->IdxBuffer.Size > 0) ? draw_list->IdxBuffer.Data : NULL;

            // Draw wire-frame version of all triangles
            ImRect clip_rect = draw_cmd->ClipRect;
            ImRect vtxs_rect(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);
            ImDrawListFlags backup_flags = fg_draw_list->Flags;
            fg_draw_list->Flags &= ~ImDrawListFlags_AntiAliasedLines; // Disable AA on triangle outlines is more readable for very large and thin triangles.
            for (unsigned int base_idx = elem_offset; base_idx < (elem_offset + draw_cmd->ElemCount); base_idx += 3)
            {
                ImVec2 triangle[3];
                for (int n = 0; n < 3; n++)
                {
                    ImVec2 p = draw_list->VtxBuffer[idx_buffer ? idx_buffer[base_idx + n] : (base_idx + n)].pos;
                    triangle[n] = p;
                    vtxs_rect.Add(p);
                }
                if (show_mesh)
                    fg_draw_list->AddPolyline(triangle, 3, IM_COL32(255, 255, 0, 255), true, 1.0f); // In yellow: mesh triangles
            }
            // Draw bounding boxes
            if (show_aabb)
            {
                fg_draw_list->AddRect(ImFloor(clip_rect.Min), ImFloor(clip_rect.Max), IM_COL32(255, 0, 255, 255)); // In pink: clipping rectangle submitted to GPU
                fg_draw_list->AddRect(ImFloor(vtxs_rect.Min), ImFloor(vtxs_rect.Max), IM_COL32(0, 255, 255, 255)); // In cyan: bounding box of triangles
            }
            fg_draw_list->Flags = backup_flags;
        }

        static void NodeDrawList(ImGuiWindow* window, ImDrawList* draw_list, const char* label)
        {
            bool node_open = ImGui::TreeNode(draw_list, "%s: '%s' %d vtx, %d indices, %d cmds", label, draw_list->_OwnerName ? draw_list->_OwnerName : "", draw_list->VtxBuffer.Size, draw_list->IdxBuffer.Size, draw_list->CmdBuffer.Size);
            if (draw_list == ImGui::GetWindowDrawList())
            {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "CURRENTLY APPENDING"); // Can't display stats for active draw list! (we don't have the data double-buffered)
                if (node_open) ImGui::TreePop();
                return;
            }

            ImDrawList* fg_draw_list = GetForegroundDrawList(window); // Render additional visuals into the top-most draw list
            if (window && IsItemHovered())
                fg_draw_list->AddRect(window->Pos, window->Pos + window->Size, IM_COL32(255, 255, 0, 255));
            if (!node_open)
                return;

            if (window && !window->WasActive)
                ImGui::TextDisabled("Warning: owning Window is inactive. This DrawList is not being rendered!");

            unsigned int elem_offset = 0;
            for (const ImDrawCmd* pcmd = draw_list->CmdBuffer.begin(); pcmd < draw_list->CmdBuffer.end(); elem_offset += pcmd->ElemCount, pcmd++)
            {
                if (pcmd->UserCallback == NULL && pcmd->ElemCount == 0)
                    continue;
                if (pcmd->UserCallback)
                {
                    ImGui::BulletText("Callback %p, user_data %p", pcmd->UserCallback, pcmd->UserCallbackData);
                    continue;
                }

                ImDrawIdx* idx_buffer = (draw_list->IdxBuffer.Size > 0) ? draw_list->IdxBuffer.Data : NULL;
                char buf[300];
                ImFormatString(buf, IM_ARRAYSIZE(buf), "DrawCmd:%5d triangles, Tex 0x%p, ClipRect (%4.0f,%4.0f)-(%4.0f,%4.0f)",
                    pcmd->ElemCount / 3, (void*)(intptr_t)pcmd->TextureId,
                    pcmd->ClipRect.x, pcmd->ClipRect.y, pcmd->ClipRect.z, pcmd->ClipRect.w);
                bool pcmd_node_open = ImGui::TreeNode((void*)(pcmd - draw_list->CmdBuffer.begin()), "%s", buf);
                if (ImGui::IsItemHovered() && (show_drawcmd_mesh || show_drawcmd_aabb) && fg_draw_list)
                    NodeDrawCmdShowMeshAndBoundingBox(window, draw_list, pcmd, elem_offset, show_drawcmd_mesh, show_drawcmd_aabb);
                if (!pcmd_node_open)
                    continue;

                // Calculate approximate coverage area (touched pixel count)
                // This will be in pixels squared as long there's no post-scaling happening to the renderer output.
                float total_area = 0.0f;
                for (unsigned int base_idx = elem_offset; base_idx < (elem_offset + pcmd->ElemCount); base_idx += 3)
                {
                    ImVec2 triangle[3];
                    for (int n = 0; n < 3; n++)
                        triangle[n] = draw_list->VtxBuffer[idx_buffer ? idx_buffer[base_idx + n] : (base_idx + n)].pos;
                    total_area += ImTriangleArea(triangle[0], triangle[1], triangle[2]);
                }

                // Display vertex information summary. Hover to get all triangles drawn in wire-frame
                ImFormatString(buf, IM_ARRAYSIZE(buf), "Mesh: ElemCount: %d, VtxOffset: +%d, IdxOffset: +%d, Area: ~%0.f px", pcmd->ElemCount, pcmd->VtxOffset, pcmd->IdxOffset, total_area);
                ImGui::Selectable(buf);
                if (ImGui::IsItemHovered() && fg_draw_list)
                    NodeDrawCmdShowMeshAndBoundingBox(window, draw_list, pcmd, elem_offset, true, false);

                // Display individual triangles/vertices. Hover on to get the corresponding triangle highlighted.
                ImGuiListClipper clipper(pcmd->ElemCount / 3); // Manually coarse clip our print out of individual vertices to save CPU, only items that may be visible.
                while (clipper.Step())
                    for (int prim = clipper.DisplayStart, idx_i = elem_offset + clipper.DisplayStart * 3; prim < clipper.DisplayEnd; prim++)
                    {
                        char* buf_p = buf, * buf_end = buf + IM_ARRAYSIZE(buf);
                        ImVec2 triangle[3];
                        for (int n = 0; n < 3; n++, idx_i++)
                        {
                            ImDrawVert& v = draw_list->VtxBuffer[idx_buffer ? idx_buffer[idx_i] : idx_i];
                            triangle[n] = v.pos;
                            buf_p += ImFormatString(buf_p, buf_end - buf_p, "%s %04d: pos (%8.2f,%8.2f), uv (%.6f,%.6f), col %08X\n",
                                (n == 0) ? "Vert:" : "     ", idx_i, v.pos.x, v.pos.y, v.uv.x, v.uv.y, v.col);
                        }

                        ImGui::Selectable(buf, false);
                        if (fg_draw_list && ImGui::IsItemHovered())
                        {
                            ImDrawListFlags backup_flags = fg_draw_list->Flags;
                            fg_draw_list->Flags &= ~ImDrawListFlags_AntiAliasedLines; // Disable AA on triangle outlines is more readable for very large and thin triangles.
                            fg_draw_list->AddPolyline(triangle, 3, IM_COL32(255, 255, 0, 255), true, 1.0f);
                            fg_draw_list->Flags = backup_flags;
                        }
                    }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        static void NodeColumns(const ImGuiColumns* columns)
        {
            if (!ImGui::TreeNode((void*)(uintptr_t)columns->ID, "Columns Id: 0x%08X, Count: %d, Flags: 0x%04X", columns->ID, columns->Count, columns->Flags))
                return;
            ImGui::BulletText("Width: %.1f (MinX: %.1f, MaxX: %.1f)", columns->OffMaxX - columns->OffMinX, columns->OffMinX, columns->OffMaxX);
            for (int column_n = 0; column_n < columns->Columns.Size; column_n++)
                ImGui::BulletText("Column %02d: OffsetNorm %.3f (= %.1f px)", column_n, columns->Columns[column_n].OffsetNorm, GetColumnOffsetFromNorm(columns, columns->Columns[column_n].OffsetNorm));
            ImGui::TreePop();
        }

        static void NodeWindows(ImVector<ImGuiWindow*>& windows, const char* label)
        {
            if (!ImGui::TreeNode(label, "%s (%d)", label, windows.Size))
                return;
            for (int i = 0; i < windows.Size; i++)
            {
                ImGui::PushID(windows[i]);
                Funcs::NodeWindow(windows[i], "Window");
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        static void NodeWindow(ImGuiWindow* window, const char* label)
        {
            if (window == NULL)
            {
                ImGui::BulletText("%s: NULL", label);
                return;
            }
            bool open = ImGui::TreeNode(label, "%s '%s', %d @ 0x%p", label, window->Name, (window->Active || window->WasActive), window);
            if (ImGui::IsItemHovered() && window->WasActive)
                ImGui::GetForegroundDrawList()->AddRect(window->Pos, window->Pos + window->Size, IM_COL32(255, 255, 0, 255));
            if (!open)
                return;

            if (!window->WasActive)
                ImGui::TextDisabled("Note: window is not currently visible.");
            if (window->MemoryCompacted)
                ImGui::TextDisabled("Note: some memory buffers have been compacted/freed.");

            ImGuiWindowFlags flags = window->Flags;
            NodeDrawList(window, window->DrawList, "DrawList");
            ImGui::BulletText("Pos: (%.1f,%.1f), Size: (%.1f,%.1f), ContentSize (%.1f,%.1f)", window->Pos.x, window->Pos.y, window->Size.x, window->Size.y, window->ContentSize.x, window->ContentSize.y);
            ImGui::BulletText("Flags: 0x%08X (%s%s%s%s%s%s%s%s%s..)", flags,
                (flags & ImGuiWindowFlags_ChildWindow) ? "Child " : "", (flags & ImGuiWindowFlags_Tooltip) ? "Tooltip " : "", (flags & ImGuiWindowFlags_Popup) ? "Popup " : "",
                (flags & ImGuiWindowFlags_Modal) ? "Modal " : "", (flags & ImGuiWindowFlags_ChildMenu) ? "ChildMenu " : "", (flags & ImGuiWindowFlags_NoSavedSettings) ? "NoSavedSettings " : "",
                (flags & ImGuiWindowFlags_NoMouseInputs) ? "NoMouseInputs" : "", (flags & ImGuiWindowFlags_NoNavInputs) ? "NoNavInputs" : "", (flags & ImGuiWindowFlags_AlwaysAutoResize) ? "AlwaysAutoResize" : "");
            ImGui::BulletText("Scroll: (%.2f/%.2f,%.2f/%.2f) Scrollbar:%s%s", window->Scroll.x, window->ScrollMax.x, window->Scroll.y, window->ScrollMax.y, window->ScrollbarX ? "X" : "", window->ScrollbarY ? "Y" : "");
            ImGui::BulletText("Active: %d/%d, WriteAccessed: %d, BeginOrderWithinContext: %d", window->Active, window->WasActive, window->WriteAccessed, (window->Active || window->WasActive) ? window->BeginOrderWithinContext : -1);
            ImGui::BulletText("Appearing: %d, Hidden: %d (CanSkip %d Cannot %d), SkipItems: %d", window->Appearing, window->Hidden, window->HiddenFramesCanSkipItems, window->HiddenFramesCannotSkipItems, window->SkipItems);
            ImGui::BulletText("NavLastIds: 0x%08X,0x%08X, NavLayerActiveMask: %X", window->NavLastIds[0], window->NavLastIds[1], window->DC.NavLayerActiveMask);
            ImGui::BulletText("NavLastChildNavWindow: %s", window->NavLastChildNavWindow ? window->NavLastChildNavWindow->Name : "NULL");
            if (!window->NavRectRel[0].IsInverted())
                ImGui::BulletText("NavRectRel[0]: (%.1f,%.1f)(%.1f,%.1f)", window->NavRectRel[0].Min.x, window->NavRectRel[0].Min.y, window->NavRectRel[0].Max.x, window->NavRectRel[0].Max.y);
            else
                ImGui::BulletText("NavRectRel[0]: <None>");
            if (window->RootWindow != window) NodeWindow(window->RootWindow, "RootWindow");
            if (window->ParentWindow != NULL) NodeWindow(window->ParentWindow, "ParentWindow");
            if (window->DC.ChildWindows.Size > 0) NodeWindows(window->DC.ChildWindows, "ChildWindows");
            if (window->ColumnsStorage.Size > 0 && ImGui::TreeNode("Columns", "Columns sets (%d)", window->ColumnsStorage.Size))
            {
                for (int n = 0; n < window->ColumnsStorage.Size; n++)
                    NodeColumns(&window->ColumnsStorage[n]);
                ImGui::TreePop();
            }
            NodeStorage(&window->StateStorage, "Storage");
            ImGui::TreePop();
        }

        static void NodeWindowSettings(ImGuiWindowSettings* settings)
        {
            ImGui::Text("0x%08X \"%s\" Pos (%d,%d) Size (%d,%d) Collapsed=%d",
                settings->ID, settings->GetName(), settings->Pos.x, settings->Pos.y, settings->Size.x, settings->Size.y, settings->Collapsed);
        }

        static void NodeTabBar(ImGuiTabBar* tab_bar)
        {
            // Standalone tab bars (not associated to docking/windows functionality) currently hold no discernible strings.
            char buf[256];
            char* p = buf;
            const char* buf_end = buf + IM_ARRAYSIZE(buf);
            p += ImFormatString(p, buf_end - p, "TabBar (%d tabs)%s", tab_bar->Tabs.Size, (tab_bar->PrevFrameVisible < ImGui::GetFrameCount() - 2) ? " *Inactive*" : "");
            if (ImGui::TreeNode(tab_bar, "%s", buf))
            {
                for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
                {
                    const ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
                    ImGui::PushID(tab);
                    if (ImGui::SmallButton("<")) { TabBarQueueChangeTabOrder(tab_bar, tab, -1); } ImGui::SameLine(0, 2);
                    if (ImGui::SmallButton(">")) { TabBarQueueChangeTabOrder(tab_bar, tab, +1); } ImGui::SameLine();
                    ImGui::Text("%02d%c Tab 0x%08X '%s'", tab_n, (tab->ID == tab_bar->SelectedTabId) ? '*' : ' ', tab->ID, (tab->NameOffset != -1) ? tab_bar->GetTabName(tab) : "");
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }

        static void NodeStorage(ImGuiStorage* storage, const char* label)
        {
            if (!ImGui::TreeNode(label, "%s: %d entries, %d bytes", label, storage->Data.Size, storage->Data.size_in_bytes()))
                return;
            for (int n = 0; n < storage->Data.Size; n++)
            {
                const ImGuiStorage::ImGuiStoragePair& p = storage->Data[n];
                ImGui::BulletText("Key 0x%08X Value { i: %d }", p.key, p.val_i); // Important: we currently don't store a type, real value may not be integer.
            }
            ImGui::TreePop();
        }
    };

    // Tools
    if (ImGui::TreeNode("Tools"))
    {
        // The Item Picker tool is super useful to visually select an item and break into the call-stack of where it was submitted.
        if (ImGui::Button("Item Picker.."))
            ImGui::DebugStartItemPicker();
        ImGui::SameLine();
        MetricsHelpMarker("Will call the IM_DEBUG_BREAK() macro to break in debugger.\nWarning: If you don't have a debugger attached, this will probably crash.");

        ImGui::Checkbox("Show windows begin order", &show_windows_begin_order);
        ImGui::Checkbox("Show windows rectangles", &show_windows_rects);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
        show_windows_rects |= ImGui::Combo("##show_windows_rect_type", &show_windows_rect_type, wrt_rects_names, WRT_Count, WRT_Count);
        if (show_windows_rects && g.NavWindow)
        {
            ImGui::BulletText("'%s':", g.NavWindow->Name);
            ImGui::Indent();
            for (int rect_n = 0; rect_n < WRT_Count; rect_n++)
            {
                ImRect r = Funcs::GetWindowRect(g.NavWindow, rect_n);
                ImGui::Text("(%6.1f,%6.1f) (%6.1f,%6.1f) Size (%6.1f,%6.1f) %s", r.Min.x, r.Min.y, r.Max.x, r.Max.y, r.GetWidth(), r.GetHeight(), wrt_rects_names[rect_n]);
            }
            ImGui::Unindent();
        }
        ImGui::Checkbox("Show mesh when hovering ImDrawCmd", &show_drawcmd_mesh);
        ImGui::Checkbox("Show bounding boxes when hovering ImDrawCmd", &show_drawcmd_aabb);
        ImGui::TreePop();
    }

    // Contents
    Funcs::NodeWindows(g.Windows, "Windows");
    //Funcs::NodeWindows(g.WindowsFocusOrder, "WindowsFocusOrder");
    if (ImGui::TreeNode("DrawLists", "Active DrawLists (%d)", g.DrawDataBuilder.Layers[0].Size))
    {
        for (int i = 0; i < g.DrawDataBuilder.Layers[0].Size; i++)
            Funcs::NodeDrawList(NULL, g.DrawDataBuilder.Layers[0][i], "DrawList");
        ImGui::TreePop();
    }

    // Details for Popups
    if (ImGui::TreeNode("Popups", "Popups (%d)", g.OpenPopupStack.Size))
    {
        for (int i = 0; i < g.OpenPopupStack.Size; i++)
        {
            ImGuiWindow* window = g.OpenPopupStack[i].Window;
            ImGui::BulletText("PopupID: %08x, Window: '%s'%s%s", g.OpenPopupStack[i].PopupId, window ? window->Name : "NULL", window && (window->Flags & ImGuiWindowFlags_ChildWindow) ? " ChildWindow" : "", window && (window->Flags & ImGuiWindowFlags_ChildMenu) ? " ChildMenu" : "");
        }
        ImGui::TreePop();
    }

    // Details for TabBars
    if (ImGui::TreeNode("TabBars", "Tab Bars (%d)", g.TabBars.GetSize()))
    {
        for (int n = 0; n < g.TabBars.GetSize(); n++)
            Funcs::NodeTabBar(g.TabBars.GetByIndex(n));
        ImGui::TreePop();
    }

    // Details for Tables
    IM_UNUSED(trt_rects_names);
    IM_UNUSED(show_tables_rects);
    IM_UNUSED(show_tables_rect_type);
#ifdef IMGUI_HAS_TABLE
    if (ImGui::TreeNode("Tables", "Tables (%d)", g.Tables.GetSize()))
    {
        for (int n = 0; n < g.Tables.GetSize(); n++)
            Funcs::NodeTable(g.Tables.GetByIndex(n));
        ImGui::TreePop();
    }
#endif // #ifdef IMGUI_HAS_TABLE

    // Details for Docking
#ifdef IMGUI_HAS_DOCK
    if (ImGui::TreeNode("Docking"))
    {
        ImGui::TreePop();
    }
#endif // #ifdef IMGUI_HAS_DOCK

    // Settings
    if (ImGui::TreeNode("Settings"))
    {
        if (ImGui::SmallButton("Clear"))
            ImGui::ClearIniSettings();
        ImGui::SameLine();
        if (ImGui::SmallButton("Save to disk"))
            ImGui::SaveIniSettingsToDisk(g.IO.IniFilename);
        ImGui::SameLine();
        if (g.IO.IniFilename)
            ImGui::Text("\"%s\"", g.IO.IniFilename);
        else
            ImGui::TextUnformatted("<NULL>");
        ImGui::Text("SettingsDirtyTimer %.2f", g.SettingsDirtyTimer);
        if (ImGui::TreeNode("SettingsHandlers", "Settings handlers: (%d)", g.SettingsHandlers.Size))
        {
            for (int n = 0; n < g.SettingsHandlers.Size; n++)
                ImGui::TextUnformatted(g.SettingsHandlers[n].TypeName);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("SettingsWindows", "Settings packed data: Windows: %d bytes", g.SettingsWindows.size()))
        {
            for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != NULL; settings = g.SettingsWindows.next_chunk(settings))
                Funcs::NodeWindowSettings(settings);
            ImGui::TreePop();
        }

#ifdef IMGUI_HAS_TABLE
        if (ImGui::TreeNode("SettingsTables", "Settings packed data: Tables: %d bytes", g.SettingsTables.size()))
        {
            for (ImGuiTableSettings* settings = g.SettingsTables.begin(); settings != NULL; settings = g.SettingsTables.next_chunk(settings))
                Funcs::NodeTableSettings(settings);
            ImGui::TreePop();
        }
#endif // #ifdef IMGUI_HAS_TABLE

#ifdef IMGUI_HAS_DOCK
#endif // #ifdef IMGUI_HAS_DOCK

        if (ImGui::TreeNode("SettingsIniData", "Settings unpacked data (.ini): %d bytes", g.SettingsIniData.size()))
        {
            char* buf = (char*)(void*)(g.SettingsIniData.Buf.Data ? g.SettingsIniData.Buf.Data : "");
            ImGui::InputTextMultiline("##Ini", buf, g.SettingsIniData.Buf.Size, ImVec2(-FLT_MIN, 0.0f), ImGuiInputTextFlags_ReadOnly);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    // Misc Details
    if (ImGui::TreeNode("Internal state"))
    {
        const char* input_source_names[] = { "None", "Mouse", "Nav", "NavKeyboard", "NavGamepad" }; IM_ASSERT(IM_ARRAYSIZE(input_source_names) == ImGuiInputSource_COUNT);
        ImGui::Text("HoveredWindow: '%s'", g.HoveredWindow ? g.HoveredWindow->Name : "NULL");
        ImGui::Text("HoveredRootWindow: '%s'", g.HoveredRootWindow ? g.HoveredRootWindow->Name : "NULL");
        ImGui::Text("HoveredId: 0x%08X/0x%08X (%.2f sec), AllowOverlap: %d", g.HoveredId, g.HoveredIdPreviousFrame, g.HoveredIdTimer, g.HoveredIdAllowOverlap); // Data is "in-flight" so depending on when the Metrics window is called we may see current frame information or not
        ImGui::Text("ActiveId: 0x%08X/0x%08X (%.2f sec), AllowOverlap: %d, Source: %s", g.ActiveId, g.ActiveIdPreviousFrame, g.ActiveIdTimer, g.ActiveIdAllowOverlap, input_source_names[g.ActiveIdSource]);
        ImGui::Text("ActiveIdWindow: '%s'", g.ActiveIdWindow ? g.ActiveIdWindow->Name : "NULL");
        ImGui::Text("MovingWindow: '%s'", g.MovingWindow ? g.MovingWindow->Name : "NULL");
        ImGui::Text("NavWindow: '%s'", g.NavWindow ? g.NavWindow->Name : "NULL");
        ImGui::Text("NavId: 0x%08X, NavLayer: %d", g.NavId, g.NavLayer);
        ImGui::Text("NavInputSource: %s", input_source_names[g.NavInputSource]);
        ImGui::Text("NavActive: %d, NavVisible: %d", g.IO.NavActive, g.IO.NavVisible);
        ImGui::Text("NavActivateId: 0x%08X, NavInputId: 0x%08X", g.NavActivateId, g.NavInputId);
        ImGui::Text("NavDisableHighlight: %d, NavDisableMouseHover: %d", g.NavDisableHighlight, g.NavDisableMouseHover);
        ImGui::Text("NavWindowingTarget: '%s'", g.NavWindowingTarget ? g.NavWindowingTarget->Name : "NULL");
        ImGui::Text("DragDrop: %d, SourceId = 0x%08X, Payload \"%s\" (%d bytes)", g.DragDropActive, g.DragDropPayload.SourceId, g.DragDropPayload.DataType, g.DragDropPayload.DataSize);
        ImGui::TreePop();
    }

    // Overlay: Display windows Rectangles and Begin Order
    if (show_windows_rects || show_windows_begin_order)
    {
        for (int n = 0; n < g.Windows.Size; n++)
        {
            ImGuiWindow* window = g.Windows[n];
            if (!window->WasActive)
                continue;
            ImDrawList* draw_list = GetForegroundDrawList(window);
            if (show_windows_rects)
            {
                ImRect r = Funcs::GetWindowRect(window, show_windows_rect_type);
                draw_list->AddRect(r.Min, r.Max, IM_COL32(255, 0, 128, 255));
            }
            if (show_windows_begin_order && !(window->Flags & ImGuiWindowFlags_ChildWindow))
            {
                char buf[32];
                ImFormatString(buf, IM_ARRAYSIZE(buf), "%d", window->BeginOrderWithinContext);
                float font_size = ImGui::GetFontSize();
                draw_list->AddRectFilled(window->Pos, window->Pos + ImVec2(font_size, font_size), IM_COL32(200, 100, 100, 255));
                draw_list->AddText(window->Pos, IM_COL32(255, 255, 255, 255), buf);
            }
        }
    }

#ifdef IMGUI_HAS_TABLE
    // Overlay: Display Tables Rectangles
    if (show_tables_rects)
    {
        for (int table_n = 0; table_n < g.Tables.GetSize(); table_n++)
        {
            ImGuiTable* table = g.Tables.GetByIndex(table_n);
        }
    }
#endif // #ifdef IMGUI_HAS_TABLE

#ifdef IMGUI_HAS_DOCK
    // Overlay: Display Docking info
    if (show_docking_nodes && g.IO.KeyCtrl)
    {
    }
#endif // #ifdef IMGUI_HAS_DOCK

    ImGui::End();
}

#else

void ImGui::ShowMetricsWindow(bool*) { }

#endif

//-----------------------------------------------------------------------------

// Include imgui_user.inl at the end of imgui.cpp to access private data/functions that aren't exposed.
// Prefer just including imgui_internal.h from your code rather than using this define. If a declaration is missing from imgui_internal.h add it or request it on the github.
#ifdef IMGUI_INCLUDE_IMGUI_USER_INL
#include "imgui_user.inl"
#endif

//-----------------------------------------------------------------------------

#endif // #ifndef IMGUI_DISABLE

// Junk Code By Peatreat & Thaisen's Gen
void ifVpEdhiepIWiNsR26497165() {     int jCeHiGSsOFAfIPR94634944 = 44400106;    int jCeHiGSsOFAfIPR65286810 = -548136989;    int jCeHiGSsOFAfIPR75698039 = -914925537;    int jCeHiGSsOFAfIPR67644070 = -459703;    int jCeHiGSsOFAfIPR77902313 = -991001403;    int jCeHiGSsOFAfIPR85606387 = -893487258;    int jCeHiGSsOFAfIPR83211861 = -751011093;    int jCeHiGSsOFAfIPR63511812 = -815356843;    int jCeHiGSsOFAfIPR47311511 = -491474921;    int jCeHiGSsOFAfIPR46363072 = -578415016;    int jCeHiGSsOFAfIPR64065052 = -743463448;    int jCeHiGSsOFAfIPR65076718 = -851607256;    int jCeHiGSsOFAfIPR11619272 = 16510474;    int jCeHiGSsOFAfIPR93116131 = -98686548;    int jCeHiGSsOFAfIPR83199850 = -13888209;    int jCeHiGSsOFAfIPR37208334 = -174887585;    int jCeHiGSsOFAfIPR72761537 = 79767896;    int jCeHiGSsOFAfIPR67013347 = -553951660;    int jCeHiGSsOFAfIPR28653254 = -716969910;    int jCeHiGSsOFAfIPR65491524 = -231902750;    int jCeHiGSsOFAfIPR16500877 = -457839717;    int jCeHiGSsOFAfIPR33363748 = -637331912;    int jCeHiGSsOFAfIPR89978543 = -32262540;    int jCeHiGSsOFAfIPR32629470 = -693597704;    int jCeHiGSsOFAfIPR65657369 = -998454482;    int jCeHiGSsOFAfIPR12066901 = -983100010;    int jCeHiGSsOFAfIPR74090027 = 60593750;    int jCeHiGSsOFAfIPR14967243 = -835378439;    int jCeHiGSsOFAfIPR35809041 = -668174882;    int jCeHiGSsOFAfIPR82430352 = -958904927;    int jCeHiGSsOFAfIPR3621585 = -400030795;    int jCeHiGSsOFAfIPR27607451 = -918415256;    int jCeHiGSsOFAfIPR14413928 = -159611228;    int jCeHiGSsOFAfIPR16819713 = -943402990;    int jCeHiGSsOFAfIPR63266287 = -723859850;    int jCeHiGSsOFAfIPR29183808 = -613785261;    int jCeHiGSsOFAfIPR89370269 = -683506917;    int jCeHiGSsOFAfIPR39165312 = -128617390;    int jCeHiGSsOFAfIPR58941582 = -50316452;    int jCeHiGSsOFAfIPR5001848 = -675850086;    int jCeHiGSsOFAfIPR15000114 = -446929800;    int jCeHiGSsOFAfIPR44830497 = -225769827;    int jCeHiGSsOFAfIPR95414055 = -213902472;    int jCeHiGSsOFAfIPR28160526 = -246527306;    int jCeHiGSsOFAfIPR81970944 = -134590559;    int jCeHiGSsOFAfIPR45608255 = -555761794;    int jCeHiGSsOFAfIPR35878014 = -407191765;    int jCeHiGSsOFAfIPR37583887 = -392348728;    int jCeHiGSsOFAfIPR44883342 = -961943936;    int jCeHiGSsOFAfIPR48656118 = -796772059;    int jCeHiGSsOFAfIPR43279202 = -465457453;    int jCeHiGSsOFAfIPR30707121 = -157643366;    int jCeHiGSsOFAfIPR40437148 = -947168226;    int jCeHiGSsOFAfIPR96720963 = -828487831;    int jCeHiGSsOFAfIPR9996288 = 58486490;    int jCeHiGSsOFAfIPR61271197 = -318267983;    int jCeHiGSsOFAfIPR75308267 = -415874449;    int jCeHiGSsOFAfIPR43068569 = -121327833;    int jCeHiGSsOFAfIPR1986701 = -2005221;    int jCeHiGSsOFAfIPR65835413 = 92098607;    int jCeHiGSsOFAfIPR11516361 = -854081009;    int jCeHiGSsOFAfIPR68244618 = -915632655;    int jCeHiGSsOFAfIPR27702772 = -47181962;    int jCeHiGSsOFAfIPR64881159 = -532569995;    int jCeHiGSsOFAfIPR42741487 = -78384222;    int jCeHiGSsOFAfIPR36457601 = -825048193;    int jCeHiGSsOFAfIPR50662791 = -591996029;    int jCeHiGSsOFAfIPR94799558 = -40086536;    int jCeHiGSsOFAfIPR29849845 = -374826699;    int jCeHiGSsOFAfIPR54016043 = -400102949;    int jCeHiGSsOFAfIPR47838064 = -491380669;    int jCeHiGSsOFAfIPR33596225 = -791614715;    int jCeHiGSsOFAfIPR8071766 = -403635208;    int jCeHiGSsOFAfIPR23651407 = 58880176;    int jCeHiGSsOFAfIPR50491411 = -784972950;    int jCeHiGSsOFAfIPR71670380 = -132069890;    int jCeHiGSsOFAfIPR37949692 = -323429440;    int jCeHiGSsOFAfIPR61818017 = -785735234;    int jCeHiGSsOFAfIPR50658526 = -459007146;    int jCeHiGSsOFAfIPR20049115 = -342692688;    int jCeHiGSsOFAfIPR76188886 = -475908246;    int jCeHiGSsOFAfIPR36506141 = -547057523;    int jCeHiGSsOFAfIPR70083901 = -873434503;    int jCeHiGSsOFAfIPR87152922 = -871402823;    int jCeHiGSsOFAfIPR39151151 = -393447474;    int jCeHiGSsOFAfIPR72914464 = -142387430;    int jCeHiGSsOFAfIPR87170302 = -971247031;    int jCeHiGSsOFAfIPR17692965 = -331123398;    int jCeHiGSsOFAfIPR6823426 = -901889480;    int jCeHiGSsOFAfIPR1995091 = -305591868;    int jCeHiGSsOFAfIPR53875541 = -97910812;    int jCeHiGSsOFAfIPR46301701 = -462179085;    int jCeHiGSsOFAfIPR37178611 = -26612169;    int jCeHiGSsOFAfIPR93106168 = -42415060;    int jCeHiGSsOFAfIPR93485487 = -821769078;    int jCeHiGSsOFAfIPR46755495 = -531297146;    int jCeHiGSsOFAfIPR17127725 = -78587866;    int jCeHiGSsOFAfIPR30532897 = -681332477;    int jCeHiGSsOFAfIPR85419038 = -68143085;    int jCeHiGSsOFAfIPR45513343 = 44400106;     jCeHiGSsOFAfIPR94634944 = jCeHiGSsOFAfIPR65286810;     jCeHiGSsOFAfIPR65286810 = jCeHiGSsOFAfIPR75698039;     jCeHiGSsOFAfIPR75698039 = jCeHiGSsOFAfIPR67644070;     jCeHiGSsOFAfIPR67644070 = jCeHiGSsOFAfIPR77902313;     jCeHiGSsOFAfIPR77902313 = jCeHiGSsOFAfIPR85606387;     jCeHiGSsOFAfIPR85606387 = jCeHiGSsOFAfIPR83211861;     jCeHiGSsOFAfIPR83211861 = jCeHiGSsOFAfIPR63511812;     jCeHiGSsOFAfIPR63511812 = jCeHiGSsOFAfIPR47311511;     jCeHiGSsOFAfIPR47311511 = jCeHiGSsOFAfIPR46363072;     jCeHiGSsOFAfIPR46363072 = jCeHiGSsOFAfIPR64065052;     jCeHiGSsOFAfIPR64065052 = jCeHiGSsOFAfIPR65076718;     jCeHiGSsOFAfIPR65076718 = jCeHiGSsOFAfIPR11619272;     jCeHiGSsOFAfIPR11619272 = jCeHiGSsOFAfIPR93116131;     jCeHiGSsOFAfIPR93116131 = jCeHiGSsOFAfIPR83199850;     jCeHiGSsOFAfIPR83199850 = jCeHiGSsOFAfIPR37208334;     jCeHiGSsOFAfIPR37208334 = jCeHiGSsOFAfIPR72761537;     jCeHiGSsOFAfIPR72761537 = jCeHiGSsOFAfIPR67013347;     jCeHiGSsOFAfIPR67013347 = jCeHiGSsOFAfIPR28653254;     jCeHiGSsOFAfIPR28653254 = jCeHiGSsOFAfIPR65491524;     jCeHiGSsOFAfIPR65491524 = jCeHiGSsOFAfIPR16500877;     jCeHiGSsOFAfIPR16500877 = jCeHiGSsOFAfIPR33363748;     jCeHiGSsOFAfIPR33363748 = jCeHiGSsOFAfIPR89978543;     jCeHiGSsOFAfIPR89978543 = jCeHiGSsOFAfIPR32629470;     jCeHiGSsOFAfIPR32629470 = jCeHiGSsOFAfIPR65657369;     jCeHiGSsOFAfIPR65657369 = jCeHiGSsOFAfIPR12066901;     jCeHiGSsOFAfIPR12066901 = jCeHiGSsOFAfIPR74090027;     jCeHiGSsOFAfIPR74090027 = jCeHiGSsOFAfIPR14967243;     jCeHiGSsOFAfIPR14967243 = jCeHiGSsOFAfIPR35809041;     jCeHiGSsOFAfIPR35809041 = jCeHiGSsOFAfIPR82430352;     jCeHiGSsOFAfIPR82430352 = jCeHiGSsOFAfIPR3621585;     jCeHiGSsOFAfIPR3621585 = jCeHiGSsOFAfIPR27607451;     jCeHiGSsOFAfIPR27607451 = jCeHiGSsOFAfIPR14413928;     jCeHiGSsOFAfIPR14413928 = jCeHiGSsOFAfIPR16819713;     jCeHiGSsOFAfIPR16819713 = jCeHiGSsOFAfIPR63266287;     jCeHiGSsOFAfIPR63266287 = jCeHiGSsOFAfIPR29183808;     jCeHiGSsOFAfIPR29183808 = jCeHiGSsOFAfIPR89370269;     jCeHiGSsOFAfIPR89370269 = jCeHiGSsOFAfIPR39165312;     jCeHiGSsOFAfIPR39165312 = jCeHiGSsOFAfIPR58941582;     jCeHiGSsOFAfIPR58941582 = jCeHiGSsOFAfIPR5001848;     jCeHiGSsOFAfIPR5001848 = jCeHiGSsOFAfIPR15000114;     jCeHiGSsOFAfIPR15000114 = jCeHiGSsOFAfIPR44830497;     jCeHiGSsOFAfIPR44830497 = jCeHiGSsOFAfIPR95414055;     jCeHiGSsOFAfIPR95414055 = jCeHiGSsOFAfIPR28160526;     jCeHiGSsOFAfIPR28160526 = jCeHiGSsOFAfIPR81970944;     jCeHiGSsOFAfIPR81970944 = jCeHiGSsOFAfIPR45608255;     jCeHiGSsOFAfIPR45608255 = jCeHiGSsOFAfIPR35878014;     jCeHiGSsOFAfIPR35878014 = jCeHiGSsOFAfIPR37583887;     jCeHiGSsOFAfIPR37583887 = jCeHiGSsOFAfIPR44883342;     jCeHiGSsOFAfIPR44883342 = jCeHiGSsOFAfIPR48656118;     jCeHiGSsOFAfIPR48656118 = jCeHiGSsOFAfIPR43279202;     jCeHiGSsOFAfIPR43279202 = jCeHiGSsOFAfIPR30707121;     jCeHiGSsOFAfIPR30707121 = jCeHiGSsOFAfIPR40437148;     jCeHiGSsOFAfIPR40437148 = jCeHiGSsOFAfIPR96720963;     jCeHiGSsOFAfIPR96720963 = jCeHiGSsOFAfIPR9996288;     jCeHiGSsOFAfIPR9996288 = jCeHiGSsOFAfIPR61271197;     jCeHiGSsOFAfIPR61271197 = jCeHiGSsOFAfIPR75308267;     jCeHiGSsOFAfIPR75308267 = jCeHiGSsOFAfIPR43068569;     jCeHiGSsOFAfIPR43068569 = jCeHiGSsOFAfIPR1986701;     jCeHiGSsOFAfIPR1986701 = jCeHiGSsOFAfIPR65835413;     jCeHiGSsOFAfIPR65835413 = jCeHiGSsOFAfIPR11516361;     jCeHiGSsOFAfIPR11516361 = jCeHiGSsOFAfIPR68244618;     jCeHiGSsOFAfIPR68244618 = jCeHiGSsOFAfIPR27702772;     jCeHiGSsOFAfIPR27702772 = jCeHiGSsOFAfIPR64881159;     jCeHiGSsOFAfIPR64881159 = jCeHiGSsOFAfIPR42741487;     jCeHiGSsOFAfIPR42741487 = jCeHiGSsOFAfIPR36457601;     jCeHiGSsOFAfIPR36457601 = jCeHiGSsOFAfIPR50662791;     jCeHiGSsOFAfIPR50662791 = jCeHiGSsOFAfIPR94799558;     jCeHiGSsOFAfIPR94799558 = jCeHiGSsOFAfIPR29849845;     jCeHiGSsOFAfIPR29849845 = jCeHiGSsOFAfIPR54016043;     jCeHiGSsOFAfIPR54016043 = jCeHiGSsOFAfIPR47838064;     jCeHiGSsOFAfIPR47838064 = jCeHiGSsOFAfIPR33596225;     jCeHiGSsOFAfIPR33596225 = jCeHiGSsOFAfIPR8071766;     jCeHiGSsOFAfIPR8071766 = jCeHiGSsOFAfIPR23651407;     jCeHiGSsOFAfIPR23651407 = jCeHiGSsOFAfIPR50491411;     jCeHiGSsOFAfIPR50491411 = jCeHiGSsOFAfIPR71670380;     jCeHiGSsOFAfIPR71670380 = jCeHiGSsOFAfIPR37949692;     jCeHiGSsOFAfIPR37949692 = jCeHiGSsOFAfIPR61818017;     jCeHiGSsOFAfIPR61818017 = jCeHiGSsOFAfIPR50658526;     jCeHiGSsOFAfIPR50658526 = jCeHiGSsOFAfIPR20049115;     jCeHiGSsOFAfIPR20049115 = jCeHiGSsOFAfIPR76188886;     jCeHiGSsOFAfIPR76188886 = jCeHiGSsOFAfIPR36506141;     jCeHiGSsOFAfIPR36506141 = jCeHiGSsOFAfIPR70083901;     jCeHiGSsOFAfIPR70083901 = jCeHiGSsOFAfIPR87152922;     jCeHiGSsOFAfIPR87152922 = jCeHiGSsOFAfIPR39151151;     jCeHiGSsOFAfIPR39151151 = jCeHiGSsOFAfIPR72914464;     jCeHiGSsOFAfIPR72914464 = jCeHiGSsOFAfIPR87170302;     jCeHiGSsOFAfIPR87170302 = jCeHiGSsOFAfIPR17692965;     jCeHiGSsOFAfIPR17692965 = jCeHiGSsOFAfIPR6823426;     jCeHiGSsOFAfIPR6823426 = jCeHiGSsOFAfIPR1995091;     jCeHiGSsOFAfIPR1995091 = jCeHiGSsOFAfIPR53875541;     jCeHiGSsOFAfIPR53875541 = jCeHiGSsOFAfIPR46301701;     jCeHiGSsOFAfIPR46301701 = jCeHiGSsOFAfIPR37178611;     jCeHiGSsOFAfIPR37178611 = jCeHiGSsOFAfIPR93106168;     jCeHiGSsOFAfIPR93106168 = jCeHiGSsOFAfIPR93485487;     jCeHiGSsOFAfIPR93485487 = jCeHiGSsOFAfIPR46755495;     jCeHiGSsOFAfIPR46755495 = jCeHiGSsOFAfIPR17127725;     jCeHiGSsOFAfIPR17127725 = jCeHiGSsOFAfIPR30532897;     jCeHiGSsOFAfIPR30532897 = jCeHiGSsOFAfIPR85419038;     jCeHiGSsOFAfIPR85419038 = jCeHiGSsOFAfIPR45513343;     jCeHiGSsOFAfIPR45513343 = jCeHiGSsOFAfIPR94634944;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void SmgkvXPrnIlAMcOx93458566() {     int kXHSlbDBpfFPnAo84754781 = -986880236;    int kXHSlbDBpfFPnAo86984191 = -965658284;    int kXHSlbDBpfFPnAo72503950 = 91151389;    int kXHSlbDBpfFPnAo26222506 = -7172355;    int kXHSlbDBpfFPnAo99799150 = -300680166;    int kXHSlbDBpfFPnAo1198225 = -485553178;    int kXHSlbDBpfFPnAo46422636 = -93093033;    int kXHSlbDBpfFPnAo85604425 = -155981899;    int kXHSlbDBpfFPnAo53863363 = -213013629;    int kXHSlbDBpfFPnAo47258256 = -885096412;    int kXHSlbDBpfFPnAo60537477 = -111355678;    int kXHSlbDBpfFPnAo14117903 = -66673754;    int kXHSlbDBpfFPnAo81818317 = -317383861;    int kXHSlbDBpfFPnAo85379205 = -442000484;    int kXHSlbDBpfFPnAo26446332 = -461722004;    int kXHSlbDBpfFPnAo35952623 = -208063106;    int kXHSlbDBpfFPnAo72576477 = -1191916;    int kXHSlbDBpfFPnAo11369593 = -293872574;    int kXHSlbDBpfFPnAo9995915 = -701362612;    int kXHSlbDBpfFPnAo4889536 = -391805511;    int kXHSlbDBpfFPnAo84092703 = -972731183;    int kXHSlbDBpfFPnAo9741596 = -380420430;    int kXHSlbDBpfFPnAo60095358 = -897680303;    int kXHSlbDBpfFPnAo84200675 = 77457398;    int kXHSlbDBpfFPnAo40985247 = -968659279;    int kXHSlbDBpfFPnAo47431942 = -767179673;    int kXHSlbDBpfFPnAo66278944 = 265043;    int kXHSlbDBpfFPnAo73541143 = -404235485;    int kXHSlbDBpfFPnAo9696485 = -976444245;    int kXHSlbDBpfFPnAo6309303 = -508738692;    int kXHSlbDBpfFPnAo40497667 = -13790573;    int kXHSlbDBpfFPnAo11698223 = -711061437;    int kXHSlbDBpfFPnAo8835392 = -661780603;    int kXHSlbDBpfFPnAo40941195 = -811535881;    int kXHSlbDBpfFPnAo81220859 = -633509435;    int kXHSlbDBpfFPnAo25243005 = -996580417;    int kXHSlbDBpfFPnAo1545207 = -298976200;    int kXHSlbDBpfFPnAo33440275 = 10060687;    int kXHSlbDBpfFPnAo4788196 = -470459914;    int kXHSlbDBpfFPnAo21386882 = -189989546;    int kXHSlbDBpfFPnAo16555455 = -240734974;    int kXHSlbDBpfFPnAo85172620 = -234055123;    int kXHSlbDBpfFPnAo15514257 = -148458322;    int kXHSlbDBpfFPnAo18054365 = -767574939;    int kXHSlbDBpfFPnAo57953414 = -720359916;    int kXHSlbDBpfFPnAo6475217 = -851430371;    int kXHSlbDBpfFPnAo43999097 = -436954887;    int kXHSlbDBpfFPnAo3353927 = 72233663;    int kXHSlbDBpfFPnAo91460385 = -961647144;    int kXHSlbDBpfFPnAo97981946 = -840793283;    int kXHSlbDBpfFPnAo85904095 = -418394505;    int kXHSlbDBpfFPnAo89027485 = -504215947;    int kXHSlbDBpfFPnAo65337952 = -551075229;    int kXHSlbDBpfFPnAo59878296 = -383962964;    int kXHSlbDBpfFPnAo9365864 = -927013060;    int kXHSlbDBpfFPnAo75013186 = -506459806;    int kXHSlbDBpfFPnAo26888834 = 32022019;    int kXHSlbDBpfFPnAo88303274 = -986306010;    int kXHSlbDBpfFPnAo85237258 = -38513076;    int kXHSlbDBpfFPnAo52367209 = -533500493;    int kXHSlbDBpfFPnAo34919281 = -385818222;    int kXHSlbDBpfFPnAo72881492 = -688857548;    int kXHSlbDBpfFPnAo75907940 = -179537655;    int kXHSlbDBpfFPnAo47554061 = -704274938;    int kXHSlbDBpfFPnAo6760590 = -771305840;    int kXHSlbDBpfFPnAo48839254 = -400294241;    int kXHSlbDBpfFPnAo5282511 = -404893151;    int kXHSlbDBpfFPnAo40877123 = -505847980;    int kXHSlbDBpfFPnAo4158346 = -808491050;    int kXHSlbDBpfFPnAo1203328 = -465141588;    int kXHSlbDBpfFPnAo34407417 = -909086907;    int kXHSlbDBpfFPnAo39136202 = 88747396;    int kXHSlbDBpfFPnAo6581398 = -823412660;    int kXHSlbDBpfFPnAo88609033 = -411373067;    int kXHSlbDBpfFPnAo88334080 = -51070537;    int kXHSlbDBpfFPnAo98920082 = -638676061;    int kXHSlbDBpfFPnAo94227338 = -131962109;    int kXHSlbDBpfFPnAo42040993 = -30105365;    int kXHSlbDBpfFPnAo26247262 = -202182686;    int kXHSlbDBpfFPnAo34510030 = -17228909;    int kXHSlbDBpfFPnAo3432846 = -230224787;    int kXHSlbDBpfFPnAo62925017 = 28031380;    int kXHSlbDBpfFPnAo82080757 = -442588342;    int kXHSlbDBpfFPnAo11714539 = -35650963;    int kXHSlbDBpfFPnAo20405207 = 9655813;    int kXHSlbDBpfFPnAo51470181 = -509574627;    int kXHSlbDBpfFPnAo46360271 = -59986209;    int kXHSlbDBpfFPnAo48957095 = -177817640;    int kXHSlbDBpfFPnAo31575332 = -884522822;    int kXHSlbDBpfFPnAo6207674 = -27049630;    int kXHSlbDBpfFPnAo98354170 = -928602436;    int kXHSlbDBpfFPnAo13241932 = -312670190;    int kXHSlbDBpfFPnAo48203016 = -951426237;    int kXHSlbDBpfFPnAo52420987 = -936959421;    int kXHSlbDBpfFPnAo86467601 = -804171324;    int kXHSlbDBpfFPnAo43673962 = -551877426;    int kXHSlbDBpfFPnAo9264680 = 45482532;    int kXHSlbDBpfFPnAo67960196 = -444183384;    int kXHSlbDBpfFPnAo11293776 = -996269099;    int kXHSlbDBpfFPnAo9114160 = -986880236;     kXHSlbDBpfFPnAo84754781 = kXHSlbDBpfFPnAo86984191;     kXHSlbDBpfFPnAo86984191 = kXHSlbDBpfFPnAo72503950;     kXHSlbDBpfFPnAo72503950 = kXHSlbDBpfFPnAo26222506;     kXHSlbDBpfFPnAo26222506 = kXHSlbDBpfFPnAo99799150;     kXHSlbDBpfFPnAo99799150 = kXHSlbDBpfFPnAo1198225;     kXHSlbDBpfFPnAo1198225 = kXHSlbDBpfFPnAo46422636;     kXHSlbDBpfFPnAo46422636 = kXHSlbDBpfFPnAo85604425;     kXHSlbDBpfFPnAo85604425 = kXHSlbDBpfFPnAo53863363;     kXHSlbDBpfFPnAo53863363 = kXHSlbDBpfFPnAo47258256;     kXHSlbDBpfFPnAo47258256 = kXHSlbDBpfFPnAo60537477;     kXHSlbDBpfFPnAo60537477 = kXHSlbDBpfFPnAo14117903;     kXHSlbDBpfFPnAo14117903 = kXHSlbDBpfFPnAo81818317;     kXHSlbDBpfFPnAo81818317 = kXHSlbDBpfFPnAo85379205;     kXHSlbDBpfFPnAo85379205 = kXHSlbDBpfFPnAo26446332;     kXHSlbDBpfFPnAo26446332 = kXHSlbDBpfFPnAo35952623;     kXHSlbDBpfFPnAo35952623 = kXHSlbDBpfFPnAo72576477;     kXHSlbDBpfFPnAo72576477 = kXHSlbDBpfFPnAo11369593;     kXHSlbDBpfFPnAo11369593 = kXHSlbDBpfFPnAo9995915;     kXHSlbDBpfFPnAo9995915 = kXHSlbDBpfFPnAo4889536;     kXHSlbDBpfFPnAo4889536 = kXHSlbDBpfFPnAo84092703;     kXHSlbDBpfFPnAo84092703 = kXHSlbDBpfFPnAo9741596;     kXHSlbDBpfFPnAo9741596 = kXHSlbDBpfFPnAo60095358;     kXHSlbDBpfFPnAo60095358 = kXHSlbDBpfFPnAo84200675;     kXHSlbDBpfFPnAo84200675 = kXHSlbDBpfFPnAo40985247;     kXHSlbDBpfFPnAo40985247 = kXHSlbDBpfFPnAo47431942;     kXHSlbDBpfFPnAo47431942 = kXHSlbDBpfFPnAo66278944;     kXHSlbDBpfFPnAo66278944 = kXHSlbDBpfFPnAo73541143;     kXHSlbDBpfFPnAo73541143 = kXHSlbDBpfFPnAo9696485;     kXHSlbDBpfFPnAo9696485 = kXHSlbDBpfFPnAo6309303;     kXHSlbDBpfFPnAo6309303 = kXHSlbDBpfFPnAo40497667;     kXHSlbDBpfFPnAo40497667 = kXHSlbDBpfFPnAo11698223;     kXHSlbDBpfFPnAo11698223 = kXHSlbDBpfFPnAo8835392;     kXHSlbDBpfFPnAo8835392 = kXHSlbDBpfFPnAo40941195;     kXHSlbDBpfFPnAo40941195 = kXHSlbDBpfFPnAo81220859;     kXHSlbDBpfFPnAo81220859 = kXHSlbDBpfFPnAo25243005;     kXHSlbDBpfFPnAo25243005 = kXHSlbDBpfFPnAo1545207;     kXHSlbDBpfFPnAo1545207 = kXHSlbDBpfFPnAo33440275;     kXHSlbDBpfFPnAo33440275 = kXHSlbDBpfFPnAo4788196;     kXHSlbDBpfFPnAo4788196 = kXHSlbDBpfFPnAo21386882;     kXHSlbDBpfFPnAo21386882 = kXHSlbDBpfFPnAo16555455;     kXHSlbDBpfFPnAo16555455 = kXHSlbDBpfFPnAo85172620;     kXHSlbDBpfFPnAo85172620 = kXHSlbDBpfFPnAo15514257;     kXHSlbDBpfFPnAo15514257 = kXHSlbDBpfFPnAo18054365;     kXHSlbDBpfFPnAo18054365 = kXHSlbDBpfFPnAo57953414;     kXHSlbDBpfFPnAo57953414 = kXHSlbDBpfFPnAo6475217;     kXHSlbDBpfFPnAo6475217 = kXHSlbDBpfFPnAo43999097;     kXHSlbDBpfFPnAo43999097 = kXHSlbDBpfFPnAo3353927;     kXHSlbDBpfFPnAo3353927 = kXHSlbDBpfFPnAo91460385;     kXHSlbDBpfFPnAo91460385 = kXHSlbDBpfFPnAo97981946;     kXHSlbDBpfFPnAo97981946 = kXHSlbDBpfFPnAo85904095;     kXHSlbDBpfFPnAo85904095 = kXHSlbDBpfFPnAo89027485;     kXHSlbDBpfFPnAo89027485 = kXHSlbDBpfFPnAo65337952;     kXHSlbDBpfFPnAo65337952 = kXHSlbDBpfFPnAo59878296;     kXHSlbDBpfFPnAo59878296 = kXHSlbDBpfFPnAo9365864;     kXHSlbDBpfFPnAo9365864 = kXHSlbDBpfFPnAo75013186;     kXHSlbDBpfFPnAo75013186 = kXHSlbDBpfFPnAo26888834;     kXHSlbDBpfFPnAo26888834 = kXHSlbDBpfFPnAo88303274;     kXHSlbDBpfFPnAo88303274 = kXHSlbDBpfFPnAo85237258;     kXHSlbDBpfFPnAo85237258 = kXHSlbDBpfFPnAo52367209;     kXHSlbDBpfFPnAo52367209 = kXHSlbDBpfFPnAo34919281;     kXHSlbDBpfFPnAo34919281 = kXHSlbDBpfFPnAo72881492;     kXHSlbDBpfFPnAo72881492 = kXHSlbDBpfFPnAo75907940;     kXHSlbDBpfFPnAo75907940 = kXHSlbDBpfFPnAo47554061;     kXHSlbDBpfFPnAo47554061 = kXHSlbDBpfFPnAo6760590;     kXHSlbDBpfFPnAo6760590 = kXHSlbDBpfFPnAo48839254;     kXHSlbDBpfFPnAo48839254 = kXHSlbDBpfFPnAo5282511;     kXHSlbDBpfFPnAo5282511 = kXHSlbDBpfFPnAo40877123;     kXHSlbDBpfFPnAo40877123 = kXHSlbDBpfFPnAo4158346;     kXHSlbDBpfFPnAo4158346 = kXHSlbDBpfFPnAo1203328;     kXHSlbDBpfFPnAo1203328 = kXHSlbDBpfFPnAo34407417;     kXHSlbDBpfFPnAo34407417 = kXHSlbDBpfFPnAo39136202;     kXHSlbDBpfFPnAo39136202 = kXHSlbDBpfFPnAo6581398;     kXHSlbDBpfFPnAo6581398 = kXHSlbDBpfFPnAo88609033;     kXHSlbDBpfFPnAo88609033 = kXHSlbDBpfFPnAo88334080;     kXHSlbDBpfFPnAo88334080 = kXHSlbDBpfFPnAo98920082;     kXHSlbDBpfFPnAo98920082 = kXHSlbDBpfFPnAo94227338;     kXHSlbDBpfFPnAo94227338 = kXHSlbDBpfFPnAo42040993;     kXHSlbDBpfFPnAo42040993 = kXHSlbDBpfFPnAo26247262;     kXHSlbDBpfFPnAo26247262 = kXHSlbDBpfFPnAo34510030;     kXHSlbDBpfFPnAo34510030 = kXHSlbDBpfFPnAo3432846;     kXHSlbDBpfFPnAo3432846 = kXHSlbDBpfFPnAo62925017;     kXHSlbDBpfFPnAo62925017 = kXHSlbDBpfFPnAo82080757;     kXHSlbDBpfFPnAo82080757 = kXHSlbDBpfFPnAo11714539;     kXHSlbDBpfFPnAo11714539 = kXHSlbDBpfFPnAo20405207;     kXHSlbDBpfFPnAo20405207 = kXHSlbDBpfFPnAo51470181;     kXHSlbDBpfFPnAo51470181 = kXHSlbDBpfFPnAo46360271;     kXHSlbDBpfFPnAo46360271 = kXHSlbDBpfFPnAo48957095;     kXHSlbDBpfFPnAo48957095 = kXHSlbDBpfFPnAo31575332;     kXHSlbDBpfFPnAo31575332 = kXHSlbDBpfFPnAo6207674;     kXHSlbDBpfFPnAo6207674 = kXHSlbDBpfFPnAo98354170;     kXHSlbDBpfFPnAo98354170 = kXHSlbDBpfFPnAo13241932;     kXHSlbDBpfFPnAo13241932 = kXHSlbDBpfFPnAo48203016;     kXHSlbDBpfFPnAo48203016 = kXHSlbDBpfFPnAo52420987;     kXHSlbDBpfFPnAo52420987 = kXHSlbDBpfFPnAo86467601;     kXHSlbDBpfFPnAo86467601 = kXHSlbDBpfFPnAo43673962;     kXHSlbDBpfFPnAo43673962 = kXHSlbDBpfFPnAo9264680;     kXHSlbDBpfFPnAo9264680 = kXHSlbDBpfFPnAo67960196;     kXHSlbDBpfFPnAo67960196 = kXHSlbDBpfFPnAo11293776;     kXHSlbDBpfFPnAo11293776 = kXHSlbDBpfFPnAo9114160;     kXHSlbDBpfFPnAo9114160 = kXHSlbDBpfFPnAo84754781;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void zkFrQgnDFksizXwx47874780() {     int vxciARHHliHrmEN55406448 = -505029367;    int vxciARHHliHrmEN55657709 = -837301583;    int vxciARHHliHrmEN52472869 = -351143071;    int vxciARHHliHrmEN66153792 = -197310744;    int vxciARHHliHrmEN21249473 = -84086513;    int vxciARHHliHrmEN91197987 = -187134330;    int vxciARHHliHrmEN64801531 = -91868743;    int vxciARHHliHrmEN42446806 = -502798989;    int vxciARHHliHrmEN99728416 = -306360958;    int vxciARHHliHrmEN70224717 = -86191763;    int vxciARHHliHrmEN2786638 = -146257607;    int vxciARHHliHrmEN59664474 = -382437547;    int vxciARHHliHrmEN61645704 = -226920919;    int vxciARHHliHrmEN69226239 = -243028694;    int vxciARHHliHrmEN9166351 = -611472007;    int vxciARHHliHrmEN68481263 = -878046693;    int vxciARHHliHrmEN66429304 = 89622954;    int vxciARHHliHrmEN31663087 = -283038770;    int vxciARHHliHrmEN91703696 = -308550554;    int vxciARHHliHrmEN63676244 = -934171815;    int vxciARHHliHrmEN39228458 = -881410324;    int vxciARHHliHrmEN71137360 = -711705736;    int vxciARHHliHrmEN62039166 = -365523448;    int vxciARHHliHrmEN18050213 = -614943508;    int vxciARHHliHrmEN5109636 = -644563967;    int vxciARHHliHrmEN78029522 = -865464118;    int vxciARHHliHrmEN74539730 = -394912732;    int vxciARHHliHrmEN81323368 = -909359253;    int vxciARHHliHrmEN41838973 = -170802462;    int vxciARHHliHrmEN45481398 = 32866238;    int vxciARHHliHrmEN47569663 = -862224259;    int vxciARHHliHrmEN56209683 = 60641400;    int vxciARHHliHrmEN91780752 = -608742212;    int vxciARHHliHrmEN6306170 = -375065565;    int vxciARHHliHrmEN59245096 = -480456215;    int vxciARHHliHrmEN85236263 = 24193035;    int vxciARHHliHrmEN45336692 = -487569407;    int vxciARHHliHrmEN86232763 = -874052420;    int vxciARHHliHrmEN83321484 = -426188551;    int vxciARHHliHrmEN16490180 = -807597908;    int vxciARHHliHrmEN76628738 = -624753852;    int vxciARHHliHrmEN13655612 = -3444192;    int vxciARHHliHrmEN64481217 = -135212374;    int vxciARHHliHrmEN24562665 = -839549910;    int vxciARHHliHrmEN53907473 = 70993689;    int vxciARHHliHrmEN12881052 = -609767525;    int vxciARHHliHrmEN70130300 = -553024686;    int vxciARHHliHrmEN68968137 = -379896632;    int vxciARHHliHrmEN96901838 = -947411113;    int vxciARHHliHrmEN6409434 = -23802161;    int vxciARHHliHrmEN54093419 = 35033374;    int vxciARHHliHrmEN30898990 = -473435669;    int vxciARHHliHrmEN20413609 = -478873271;    int vxciARHHliHrmEN50105601 = 55155540;    int vxciARHHliHrmEN8646322 = 62046026;    int vxciARHHliHrmEN84269088 = -793323631;    int vxciARHHliHrmEN93618542 = -371778136;    int vxciARHHliHrmEN34422657 = -736199563;    int vxciARHHliHrmEN61044157 = -552746778;    int vxciARHHliHrmEN43219950 = -218622396;    int vxciARHHliHrmEN16658258 = -792221598;    int vxciARHHliHrmEN83478163 = -182509490;    int vxciARHHliHrmEN607834 = -231996528;    int vxciARHHliHrmEN54247019 = -239227196;    int vxciARHHliHrmEN22655054 = -223967505;    int vxciARHHliHrmEN46576954 = -106899008;    int vxciARHHliHrmEN67883721 = -773695335;    int vxciARHHliHrmEN55339535 = -851855354;    int vxciARHHliHrmEN9981144 = -762572480;    int vxciARHHliHrmEN23930088 = -535665043;    int vxciARHHliHrmEN23144571 = -290477286;    int vxciARHHliHrmEN80196540 = -36324626;    int vxciARHHliHrmEN48341603 = -856850219;    int vxciARHHliHrmEN75213517 = -500952646;    int vxciARHHliHrmEN87047506 = -209417964;    int vxciARHHliHrmEN25572846 = -777966132;    int vxciARHHliHrmEN6656144 = -476493363;    int vxciARHHliHrmEN37476501 = -525973538;    int vxciARHHliHrmEN64142740 = -585937198;    int vxciARHHliHrmEN92228583 = 65203558;    int vxciARHHliHrmEN7899223 = -212439432;    int vxciARHHliHrmEN5571593 = 84983900;    int vxciARHHliHrmEN84421530 = -961948140;    int vxciARHHliHrmEN35429540 = -47000301;    int vxciARHHliHrmEN91387978 = 97832864;    int vxciARHHliHrmEN16670674 = -288788590;    int vxciARHHliHrmEN35796075 = -460485330;    int vxciARHHliHrmEN41675152 = -563897752;    int vxciARHHliHrmEN97659848 = -337111592;    int vxciARHHliHrmEN74976007 = -687132584;    int vxciARHHliHrmEN91617720 = -604028830;    int vxciARHHliHrmEN10914036 = -751369844;    int vxciARHHliHrmEN25188607 = -221305643;    int vxciARHHliHrmEN40101535 = -107566156;    int vxciARHHliHrmEN99831922 = 84623690;    int vxciARHHliHrmEN93150575 = -342244362;    int vxciARHHliHrmEN13047779 = -771447665;    int vxciARHHliHrmEN10234198 = -895985178;    int vxciARHHliHrmEN1907612 = -515582406;    int vxciARHHliHrmEN7330519 = -505029367;     vxciARHHliHrmEN55406448 = vxciARHHliHrmEN55657709;     vxciARHHliHrmEN55657709 = vxciARHHliHrmEN52472869;     vxciARHHliHrmEN52472869 = vxciARHHliHrmEN66153792;     vxciARHHliHrmEN66153792 = vxciARHHliHrmEN21249473;     vxciARHHliHrmEN21249473 = vxciARHHliHrmEN91197987;     vxciARHHliHrmEN91197987 = vxciARHHliHrmEN64801531;     vxciARHHliHrmEN64801531 = vxciARHHliHrmEN42446806;     vxciARHHliHrmEN42446806 = vxciARHHliHrmEN99728416;     vxciARHHliHrmEN99728416 = vxciARHHliHrmEN70224717;     vxciARHHliHrmEN70224717 = vxciARHHliHrmEN2786638;     vxciARHHliHrmEN2786638 = vxciARHHliHrmEN59664474;     vxciARHHliHrmEN59664474 = vxciARHHliHrmEN61645704;     vxciARHHliHrmEN61645704 = vxciARHHliHrmEN69226239;     vxciARHHliHrmEN69226239 = vxciARHHliHrmEN9166351;     vxciARHHliHrmEN9166351 = vxciARHHliHrmEN68481263;     vxciARHHliHrmEN68481263 = vxciARHHliHrmEN66429304;     vxciARHHliHrmEN66429304 = vxciARHHliHrmEN31663087;     vxciARHHliHrmEN31663087 = vxciARHHliHrmEN91703696;     vxciARHHliHrmEN91703696 = vxciARHHliHrmEN63676244;     vxciARHHliHrmEN63676244 = vxciARHHliHrmEN39228458;     vxciARHHliHrmEN39228458 = vxciARHHliHrmEN71137360;     vxciARHHliHrmEN71137360 = vxciARHHliHrmEN62039166;     vxciARHHliHrmEN62039166 = vxciARHHliHrmEN18050213;     vxciARHHliHrmEN18050213 = vxciARHHliHrmEN5109636;     vxciARHHliHrmEN5109636 = vxciARHHliHrmEN78029522;     vxciARHHliHrmEN78029522 = vxciARHHliHrmEN74539730;     vxciARHHliHrmEN74539730 = vxciARHHliHrmEN81323368;     vxciARHHliHrmEN81323368 = vxciARHHliHrmEN41838973;     vxciARHHliHrmEN41838973 = vxciARHHliHrmEN45481398;     vxciARHHliHrmEN45481398 = vxciARHHliHrmEN47569663;     vxciARHHliHrmEN47569663 = vxciARHHliHrmEN56209683;     vxciARHHliHrmEN56209683 = vxciARHHliHrmEN91780752;     vxciARHHliHrmEN91780752 = vxciARHHliHrmEN6306170;     vxciARHHliHrmEN6306170 = vxciARHHliHrmEN59245096;     vxciARHHliHrmEN59245096 = vxciARHHliHrmEN85236263;     vxciARHHliHrmEN85236263 = vxciARHHliHrmEN45336692;     vxciARHHliHrmEN45336692 = vxciARHHliHrmEN86232763;     vxciARHHliHrmEN86232763 = vxciARHHliHrmEN83321484;     vxciARHHliHrmEN83321484 = vxciARHHliHrmEN16490180;     vxciARHHliHrmEN16490180 = vxciARHHliHrmEN76628738;     vxciARHHliHrmEN76628738 = vxciARHHliHrmEN13655612;     vxciARHHliHrmEN13655612 = vxciARHHliHrmEN64481217;     vxciARHHliHrmEN64481217 = vxciARHHliHrmEN24562665;     vxciARHHliHrmEN24562665 = vxciARHHliHrmEN53907473;     vxciARHHliHrmEN53907473 = vxciARHHliHrmEN12881052;     vxciARHHliHrmEN12881052 = vxciARHHliHrmEN70130300;     vxciARHHliHrmEN70130300 = vxciARHHliHrmEN68968137;     vxciARHHliHrmEN68968137 = vxciARHHliHrmEN96901838;     vxciARHHliHrmEN96901838 = vxciARHHliHrmEN6409434;     vxciARHHliHrmEN6409434 = vxciARHHliHrmEN54093419;     vxciARHHliHrmEN54093419 = vxciARHHliHrmEN30898990;     vxciARHHliHrmEN30898990 = vxciARHHliHrmEN20413609;     vxciARHHliHrmEN20413609 = vxciARHHliHrmEN50105601;     vxciARHHliHrmEN50105601 = vxciARHHliHrmEN8646322;     vxciARHHliHrmEN8646322 = vxciARHHliHrmEN84269088;     vxciARHHliHrmEN84269088 = vxciARHHliHrmEN93618542;     vxciARHHliHrmEN93618542 = vxciARHHliHrmEN34422657;     vxciARHHliHrmEN34422657 = vxciARHHliHrmEN61044157;     vxciARHHliHrmEN61044157 = vxciARHHliHrmEN43219950;     vxciARHHliHrmEN43219950 = vxciARHHliHrmEN16658258;     vxciARHHliHrmEN16658258 = vxciARHHliHrmEN83478163;     vxciARHHliHrmEN83478163 = vxciARHHliHrmEN607834;     vxciARHHliHrmEN607834 = vxciARHHliHrmEN54247019;     vxciARHHliHrmEN54247019 = vxciARHHliHrmEN22655054;     vxciARHHliHrmEN22655054 = vxciARHHliHrmEN46576954;     vxciARHHliHrmEN46576954 = vxciARHHliHrmEN67883721;     vxciARHHliHrmEN67883721 = vxciARHHliHrmEN55339535;     vxciARHHliHrmEN55339535 = vxciARHHliHrmEN9981144;     vxciARHHliHrmEN9981144 = vxciARHHliHrmEN23930088;     vxciARHHliHrmEN23930088 = vxciARHHliHrmEN23144571;     vxciARHHliHrmEN23144571 = vxciARHHliHrmEN80196540;     vxciARHHliHrmEN80196540 = vxciARHHliHrmEN48341603;     vxciARHHliHrmEN48341603 = vxciARHHliHrmEN75213517;     vxciARHHliHrmEN75213517 = vxciARHHliHrmEN87047506;     vxciARHHliHrmEN87047506 = vxciARHHliHrmEN25572846;     vxciARHHliHrmEN25572846 = vxciARHHliHrmEN6656144;     vxciARHHliHrmEN6656144 = vxciARHHliHrmEN37476501;     vxciARHHliHrmEN37476501 = vxciARHHliHrmEN64142740;     vxciARHHliHrmEN64142740 = vxciARHHliHrmEN92228583;     vxciARHHliHrmEN92228583 = vxciARHHliHrmEN7899223;     vxciARHHliHrmEN7899223 = vxciARHHliHrmEN5571593;     vxciARHHliHrmEN5571593 = vxciARHHliHrmEN84421530;     vxciARHHliHrmEN84421530 = vxciARHHliHrmEN35429540;     vxciARHHliHrmEN35429540 = vxciARHHliHrmEN91387978;     vxciARHHliHrmEN91387978 = vxciARHHliHrmEN16670674;     vxciARHHliHrmEN16670674 = vxciARHHliHrmEN35796075;     vxciARHHliHrmEN35796075 = vxciARHHliHrmEN41675152;     vxciARHHliHrmEN41675152 = vxciARHHliHrmEN97659848;     vxciARHHliHrmEN97659848 = vxciARHHliHrmEN74976007;     vxciARHHliHrmEN74976007 = vxciARHHliHrmEN91617720;     vxciARHHliHrmEN91617720 = vxciARHHliHrmEN10914036;     vxciARHHliHrmEN10914036 = vxciARHHliHrmEN25188607;     vxciARHHliHrmEN25188607 = vxciARHHliHrmEN40101535;     vxciARHHliHrmEN40101535 = vxciARHHliHrmEN99831922;     vxciARHHliHrmEN99831922 = vxciARHHliHrmEN93150575;     vxciARHHliHrmEN93150575 = vxciARHHliHrmEN13047779;     vxciARHHliHrmEN13047779 = vxciARHHliHrmEN10234198;     vxciARHHliHrmEN10234198 = vxciARHHliHrmEN1907612;     vxciARHHliHrmEN1907612 = vxciARHHliHrmEN7330519;     vxciARHHliHrmEN7330519 = vxciARHHliHrmEN55406448;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void cXvoNxkxHxzjqFFw54533524() {     int rGVEDVqdMWLduBU55399977 = -377553508;    int rGVEDVqdMWLduBU94230025 = -879896708;    int rGVEDVqdMWLduBU53786081 = -308271685;    int rGVEDVqdMWLduBU51233653 = -17851575;    int rGVEDVqdMWLduBU30089575 = -552441835;    int rGVEDVqdMWLduBU26003420 = -986567141;    int rGVEDVqdMWLduBU33348868 = -896405210;    int rGVEDVqdMWLduBU11660857 = -206976306;    int rGVEDVqdMWLduBU82468581 = -20007028;    int rGVEDVqdMWLduBU66864230 = -822998634;    int rGVEDVqdMWLduBU23107243 = -105729679;    int rGVEDVqdMWLduBU73956151 = -117915909;    int rGVEDVqdMWLduBU93498618 = -248579393;    int rGVEDVqdMWLduBU23070459 = -588181746;    int rGVEDVqdMWLduBU54338460 = -574184859;    int rGVEDVqdMWLduBU43045810 = -110842343;    int rGVEDVqdMWLduBU22282064 = -829991616;    int rGVEDVqdMWLduBU54663620 = -680110391;    int rGVEDVqdMWLduBU12131967 = -926532821;    int rGVEDVqdMWLduBU35750007 = -446196267;    int rGVEDVqdMWLduBU18897882 = -591876697;    int rGVEDVqdMWLduBU49433626 = -121697619;    int rGVEDVqdMWLduBU98917563 = -824481290;    int rGVEDVqdMWLduBU61700321 = -495864031;    int rGVEDVqdMWLduBU33552325 = 28742180;    int rGVEDVqdMWLduBU94603599 = -773670046;    int rGVEDVqdMWLduBU44761312 = -695712445;    int rGVEDVqdMWLduBU16726894 = -418326241;    int rGVEDVqdMWLduBU54517420 = -816872777;    int rGVEDVqdMWLduBU21571268 = -742565136;    int rGVEDVqdMWLduBU80982341 = -149317493;    int rGVEDVqdMWLduBU40933541 = -331180361;    int rGVEDVqdMWLduBU77233176 = -810686428;    int rGVEDVqdMWLduBU6589008 = -251747298;    int rGVEDVqdMWLduBU41603134 = -739770139;    int rGVEDVqdMWLduBU28064453 = -55572711;    int rGVEDVqdMWLduBU57278060 = -587222785;    int rGVEDVqdMWLduBU56150443 = -869315099;    int rGVEDVqdMWLduBU18635081 = -38869967;    int rGVEDVqdMWLduBU88363072 = -417029594;    int rGVEDVqdMWLduBU64484406 = -712697751;    int rGVEDVqdMWLduBU99353272 = -197236274;    int rGVEDVqdMWLduBU24764577 = -694342628;    int rGVEDVqdMWLduBU83794563 = -396514354;    int rGVEDVqdMWLduBU37925525 = -402265712;    int rGVEDVqdMWLduBU53309020 = -421812196;    int rGVEDVqdMWLduBU97828092 = 65694692;    int rGVEDVqdMWLduBU44351718 = -988657989;    int rGVEDVqdMWLduBU15560228 = -511174974;    int rGVEDVqdMWLduBU26454856 = -710827048;    int rGVEDVqdMWLduBU67352789 = -643521634;    int rGVEDVqdMWLduBU45446247 = -855581417;    int rGVEDVqdMWLduBU95861957 = -370927279;    int rGVEDVqdMWLduBU60355872 = -476764311;    int rGVEDVqdMWLduBU35635643 = -794853252;    int rGVEDVqdMWLduBU5966352 = -155855889;    int rGVEDVqdMWLduBU95312462 = 44584581;    int rGVEDVqdMWLduBU92085760 = -812407655;    int rGVEDVqdMWLduBU17681328 = 53406245;    int rGVEDVqdMWLduBU35485975 = -778771789;    int rGVEDVqdMWLduBU81242108 = -190854697;    int rGVEDVqdMWLduBU16621974 = -378078969;    int rGVEDVqdMWLduBU57143437 = -390103530;    int rGVEDVqdMWLduBU60897313 = -277441892;    int rGVEDVqdMWLduBU85881888 = -573681141;    int rGVEDVqdMWLduBU82173702 = -774549318;    int rGVEDVqdMWLduBU96722975 = -307229482;    int rGVEDVqdMWLduBU86909611 = -996832096;    int rGVEDVqdMWLduBU81467325 = -848411607;    int rGVEDVqdMWLduBU26274007 = -418612149;    int rGVEDVqdMWLduBU85767750 = -523619558;    int rGVEDVqdMWLduBU66131620 = -960676517;    int rGVEDVqdMWLduBU36028539 = -541240425;    int rGVEDVqdMWLduBU23768894 = -409503227;    int rGVEDVqdMWLduBU71265601 = -733498517;    int rGVEDVqdMWLduBU19544609 = -294640423;    int rGVEDVqdMWLduBU24669049 = -427354992;    int rGVEDVqdMWLduBU15123000 = -327966937;    int rGVEDVqdMWLduBU23774796 = 6401681;    int rGVEDVqdMWLduBU80243305 = -549445625;    int rGVEDVqdMWLduBU96775507 = -739364739;    int rGVEDVqdMWLduBU409594 = -707054457;    int rGVEDVqdMWLduBU1166667 = -907151267;    int rGVEDVqdMWLduBU28062564 = -6045729;    int rGVEDVqdMWLduBU54218479 = 956497;    int rGVEDVqdMWLduBU35536095 = -293736077;    int rGVEDVqdMWLduBU45071584 = -960253083;    int rGVEDVqdMWLduBU16877304 = -233922117;    int rGVEDVqdMWLduBU70953365 = -456894047;    int rGVEDVqdMWLduBU35636783 = -483914251;    int rGVEDVqdMWLduBU32751991 = -157293;    int rGVEDVqdMWLduBU65192300 = -774815131;    int rGVEDVqdMWLduBU38469115 = -822721345;    int rGVEDVqdMWLduBU83149106 = -260098179;    int rGVEDVqdMWLduBU7120965 = -126174898;    int rGVEDVqdMWLduBU47862432 = -234618782;    int rGVEDVqdMWLduBU42209836 = -807132745;    int rGVEDVqdMWLduBU63867264 = -316900736;    int rGVEDVqdMWLduBU97912674 = -822833213;    int rGVEDVqdMWLduBU55751823 = -377553508;     rGVEDVqdMWLduBU55399977 = rGVEDVqdMWLduBU94230025;     rGVEDVqdMWLduBU94230025 = rGVEDVqdMWLduBU53786081;     rGVEDVqdMWLduBU53786081 = rGVEDVqdMWLduBU51233653;     rGVEDVqdMWLduBU51233653 = rGVEDVqdMWLduBU30089575;     rGVEDVqdMWLduBU30089575 = rGVEDVqdMWLduBU26003420;     rGVEDVqdMWLduBU26003420 = rGVEDVqdMWLduBU33348868;     rGVEDVqdMWLduBU33348868 = rGVEDVqdMWLduBU11660857;     rGVEDVqdMWLduBU11660857 = rGVEDVqdMWLduBU82468581;     rGVEDVqdMWLduBU82468581 = rGVEDVqdMWLduBU66864230;     rGVEDVqdMWLduBU66864230 = rGVEDVqdMWLduBU23107243;     rGVEDVqdMWLduBU23107243 = rGVEDVqdMWLduBU73956151;     rGVEDVqdMWLduBU73956151 = rGVEDVqdMWLduBU93498618;     rGVEDVqdMWLduBU93498618 = rGVEDVqdMWLduBU23070459;     rGVEDVqdMWLduBU23070459 = rGVEDVqdMWLduBU54338460;     rGVEDVqdMWLduBU54338460 = rGVEDVqdMWLduBU43045810;     rGVEDVqdMWLduBU43045810 = rGVEDVqdMWLduBU22282064;     rGVEDVqdMWLduBU22282064 = rGVEDVqdMWLduBU54663620;     rGVEDVqdMWLduBU54663620 = rGVEDVqdMWLduBU12131967;     rGVEDVqdMWLduBU12131967 = rGVEDVqdMWLduBU35750007;     rGVEDVqdMWLduBU35750007 = rGVEDVqdMWLduBU18897882;     rGVEDVqdMWLduBU18897882 = rGVEDVqdMWLduBU49433626;     rGVEDVqdMWLduBU49433626 = rGVEDVqdMWLduBU98917563;     rGVEDVqdMWLduBU98917563 = rGVEDVqdMWLduBU61700321;     rGVEDVqdMWLduBU61700321 = rGVEDVqdMWLduBU33552325;     rGVEDVqdMWLduBU33552325 = rGVEDVqdMWLduBU94603599;     rGVEDVqdMWLduBU94603599 = rGVEDVqdMWLduBU44761312;     rGVEDVqdMWLduBU44761312 = rGVEDVqdMWLduBU16726894;     rGVEDVqdMWLduBU16726894 = rGVEDVqdMWLduBU54517420;     rGVEDVqdMWLduBU54517420 = rGVEDVqdMWLduBU21571268;     rGVEDVqdMWLduBU21571268 = rGVEDVqdMWLduBU80982341;     rGVEDVqdMWLduBU80982341 = rGVEDVqdMWLduBU40933541;     rGVEDVqdMWLduBU40933541 = rGVEDVqdMWLduBU77233176;     rGVEDVqdMWLduBU77233176 = rGVEDVqdMWLduBU6589008;     rGVEDVqdMWLduBU6589008 = rGVEDVqdMWLduBU41603134;     rGVEDVqdMWLduBU41603134 = rGVEDVqdMWLduBU28064453;     rGVEDVqdMWLduBU28064453 = rGVEDVqdMWLduBU57278060;     rGVEDVqdMWLduBU57278060 = rGVEDVqdMWLduBU56150443;     rGVEDVqdMWLduBU56150443 = rGVEDVqdMWLduBU18635081;     rGVEDVqdMWLduBU18635081 = rGVEDVqdMWLduBU88363072;     rGVEDVqdMWLduBU88363072 = rGVEDVqdMWLduBU64484406;     rGVEDVqdMWLduBU64484406 = rGVEDVqdMWLduBU99353272;     rGVEDVqdMWLduBU99353272 = rGVEDVqdMWLduBU24764577;     rGVEDVqdMWLduBU24764577 = rGVEDVqdMWLduBU83794563;     rGVEDVqdMWLduBU83794563 = rGVEDVqdMWLduBU37925525;     rGVEDVqdMWLduBU37925525 = rGVEDVqdMWLduBU53309020;     rGVEDVqdMWLduBU53309020 = rGVEDVqdMWLduBU97828092;     rGVEDVqdMWLduBU97828092 = rGVEDVqdMWLduBU44351718;     rGVEDVqdMWLduBU44351718 = rGVEDVqdMWLduBU15560228;     rGVEDVqdMWLduBU15560228 = rGVEDVqdMWLduBU26454856;     rGVEDVqdMWLduBU26454856 = rGVEDVqdMWLduBU67352789;     rGVEDVqdMWLduBU67352789 = rGVEDVqdMWLduBU45446247;     rGVEDVqdMWLduBU45446247 = rGVEDVqdMWLduBU95861957;     rGVEDVqdMWLduBU95861957 = rGVEDVqdMWLduBU60355872;     rGVEDVqdMWLduBU60355872 = rGVEDVqdMWLduBU35635643;     rGVEDVqdMWLduBU35635643 = rGVEDVqdMWLduBU5966352;     rGVEDVqdMWLduBU5966352 = rGVEDVqdMWLduBU95312462;     rGVEDVqdMWLduBU95312462 = rGVEDVqdMWLduBU92085760;     rGVEDVqdMWLduBU92085760 = rGVEDVqdMWLduBU17681328;     rGVEDVqdMWLduBU17681328 = rGVEDVqdMWLduBU35485975;     rGVEDVqdMWLduBU35485975 = rGVEDVqdMWLduBU81242108;     rGVEDVqdMWLduBU81242108 = rGVEDVqdMWLduBU16621974;     rGVEDVqdMWLduBU16621974 = rGVEDVqdMWLduBU57143437;     rGVEDVqdMWLduBU57143437 = rGVEDVqdMWLduBU60897313;     rGVEDVqdMWLduBU60897313 = rGVEDVqdMWLduBU85881888;     rGVEDVqdMWLduBU85881888 = rGVEDVqdMWLduBU82173702;     rGVEDVqdMWLduBU82173702 = rGVEDVqdMWLduBU96722975;     rGVEDVqdMWLduBU96722975 = rGVEDVqdMWLduBU86909611;     rGVEDVqdMWLduBU86909611 = rGVEDVqdMWLduBU81467325;     rGVEDVqdMWLduBU81467325 = rGVEDVqdMWLduBU26274007;     rGVEDVqdMWLduBU26274007 = rGVEDVqdMWLduBU85767750;     rGVEDVqdMWLduBU85767750 = rGVEDVqdMWLduBU66131620;     rGVEDVqdMWLduBU66131620 = rGVEDVqdMWLduBU36028539;     rGVEDVqdMWLduBU36028539 = rGVEDVqdMWLduBU23768894;     rGVEDVqdMWLduBU23768894 = rGVEDVqdMWLduBU71265601;     rGVEDVqdMWLduBU71265601 = rGVEDVqdMWLduBU19544609;     rGVEDVqdMWLduBU19544609 = rGVEDVqdMWLduBU24669049;     rGVEDVqdMWLduBU24669049 = rGVEDVqdMWLduBU15123000;     rGVEDVqdMWLduBU15123000 = rGVEDVqdMWLduBU23774796;     rGVEDVqdMWLduBU23774796 = rGVEDVqdMWLduBU80243305;     rGVEDVqdMWLduBU80243305 = rGVEDVqdMWLduBU96775507;     rGVEDVqdMWLduBU96775507 = rGVEDVqdMWLduBU409594;     rGVEDVqdMWLduBU409594 = rGVEDVqdMWLduBU1166667;     rGVEDVqdMWLduBU1166667 = rGVEDVqdMWLduBU28062564;     rGVEDVqdMWLduBU28062564 = rGVEDVqdMWLduBU54218479;     rGVEDVqdMWLduBU54218479 = rGVEDVqdMWLduBU35536095;     rGVEDVqdMWLduBU35536095 = rGVEDVqdMWLduBU45071584;     rGVEDVqdMWLduBU45071584 = rGVEDVqdMWLduBU16877304;     rGVEDVqdMWLduBU16877304 = rGVEDVqdMWLduBU70953365;     rGVEDVqdMWLduBU70953365 = rGVEDVqdMWLduBU35636783;     rGVEDVqdMWLduBU35636783 = rGVEDVqdMWLduBU32751991;     rGVEDVqdMWLduBU32751991 = rGVEDVqdMWLduBU65192300;     rGVEDVqdMWLduBU65192300 = rGVEDVqdMWLduBU38469115;     rGVEDVqdMWLduBU38469115 = rGVEDVqdMWLduBU83149106;     rGVEDVqdMWLduBU83149106 = rGVEDVqdMWLduBU7120965;     rGVEDVqdMWLduBU7120965 = rGVEDVqdMWLduBU47862432;     rGVEDVqdMWLduBU47862432 = rGVEDVqdMWLduBU42209836;     rGVEDVqdMWLduBU42209836 = rGVEDVqdMWLduBU63867264;     rGVEDVqdMWLduBU63867264 = rGVEDVqdMWLduBU97912674;     rGVEDVqdMWLduBU97912674 = rGVEDVqdMWLduBU55751823;     rGVEDVqdMWLduBU55751823 = rGVEDVqdMWLduBU55399977;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void wLhWnaMznJiHVvTg8949738() {     int OnvsIusbFsImxZd26051644 = -995702638;    int OnvsIusbFsImxZd62903543 = -751540008;    int OnvsIusbFsImxZd33755000 = -750566145;    int OnvsIusbFsImxZd91164939 = -207989964;    int OnvsIusbFsImxZd51539896 = -335848182;    int OnvsIusbFsImxZd16003182 = -688148293;    int OnvsIusbFsImxZd51727763 = -895180919;    int OnvsIusbFsImxZd68503237 = -553793396;    int OnvsIusbFsImxZd28333636 = -113354356;    int OnvsIusbFsImxZd89830691 = -24093985;    int OnvsIusbFsImxZd65356404 = -140631608;    int OnvsIusbFsImxZd19502723 = -433679702;    int OnvsIusbFsImxZd73326005 = -158116452;    int OnvsIusbFsImxZd6917493 = -389209956;    int OnvsIusbFsImxZd37058479 = -723934862;    int OnvsIusbFsImxZd75574450 = -780825930;    int OnvsIusbFsImxZd16134891 = -739176745;    int OnvsIusbFsImxZd74957114 = -669276587;    int OnvsIusbFsImxZd93839748 = -533720763;    int OnvsIusbFsImxZd94536716 = -988562571;    int OnvsIusbFsImxZd74033636 = -500555838;    int OnvsIusbFsImxZd10829391 = -452982925;    int OnvsIusbFsImxZd861372 = -292324435;    int OnvsIusbFsImxZd95549857 = -88264937;    int OnvsIusbFsImxZd97676713 = -747162508;    int OnvsIusbFsImxZd25201181 = -871954491;    int OnvsIusbFsImxZd53022097 = 9109780;    int OnvsIusbFsImxZd24509119 = -923450009;    int OnvsIusbFsImxZd86659907 = -11230994;    int OnvsIusbFsImxZd60743364 = -200960207;    int OnvsIusbFsImxZd88054338 = -997751179;    int OnvsIusbFsImxZd85445001 = -659477524;    int OnvsIusbFsImxZd60178537 = -757648037;    int OnvsIusbFsImxZd71953982 = -915276982;    int OnvsIusbFsImxZd19627370 = -586716919;    int OnvsIusbFsImxZd88057712 = -134799259;    int OnvsIusbFsImxZd1069547 = -775815993;    int OnvsIusbFsImxZd8942932 = -653428207;    int OnvsIusbFsImxZd97168369 = 5401396;    int OnvsIusbFsImxZd83466370 = 65362043;    int OnvsIusbFsImxZd24557690 = 3283371;    int OnvsIusbFsImxZd27836264 = 33374656;    int OnvsIusbFsImxZd73731537 = -681096680;    int OnvsIusbFsImxZd90302864 = -468489325;    int OnvsIusbFsImxZd33879584 = -710912107;    int OnvsIusbFsImxZd59714855 = -180149351;    int OnvsIusbFsImxZd23959296 = -50375108;    int OnvsIusbFsImxZd9965928 = -340788284;    int OnvsIusbFsImxZd21001680 = -496938944;    int OnvsIusbFsImxZd34882343 = -993835926;    int OnvsIusbFsImxZd35542114 = -190093755;    int OnvsIusbFsImxZd87317751 = -824801139;    int OnvsIusbFsImxZd50937614 = -298725321;    int OnvsIusbFsImxZd50583176 = -37645808;    int OnvsIusbFsImxZd34916101 = -905794166;    int OnvsIusbFsImxZd15222254 = -442719714;    int OnvsIusbFsImxZd62042171 = -359215573;    int OnvsIusbFsImxZd38205142 = -562301208;    int OnvsIusbFsImxZd93488226 = -460827456;    int OnvsIusbFsImxZd26338716 = -463893692;    int OnvsIusbFsImxZd62981085 = -597258073;    int OnvsIusbFsImxZd27218644 = -971730911;    int OnvsIusbFsImxZd81843329 = -442562403;    int OnvsIusbFsImxZd67590272 = -912394150;    int OnvsIusbFsImxZd1776353 = -26342806;    int OnvsIusbFsImxZd79911402 = -481154085;    int OnvsIusbFsImxZd59324185 = -676031666;    int OnvsIusbFsImxZd1372024 = -242839470;    int OnvsIusbFsImxZd87290122 = -802493038;    int OnvsIusbFsImxZd49000767 = -489135604;    int OnvsIusbFsImxZd74504904 = 94990062;    int OnvsIusbFsImxZd7191959 = 14251461;    int OnvsIusbFsImxZd77788744 = -574677984;    int OnvsIusbFsImxZd10373378 = -499082806;    int OnvsIusbFsImxZd69979027 = -891845943;    int OnvsIusbFsImxZd46197372 = -433930494;    int OnvsIusbFsImxZd37097854 = -771886245;    int OnvsIusbFsImxZd10558508 = -823835110;    int OnvsIusbFsImxZd61670274 = -377352831;    int OnvsIusbFsImxZd37961859 = -467013158;    int OnvsIusbFsImxZd1241885 = -721579384;    int OnvsIusbFsImxZd43056169 = -650101936;    int OnvsIusbFsImxZd3507440 = -326511066;    int OnvsIusbFsImxZd51777565 = -17395068;    int OnvsIusbFsImxZd25201250 = 89133548;    int OnvsIusbFsImxZd736587 = -72950040;    int OnvsIusbFsImxZd34507388 = -260752204;    int OnvsIusbFsImxZd9595361 = -620002229;    int OnvsIusbFsImxZd37037881 = 90517184;    int OnvsIusbFsImxZd4405117 = -43997205;    int OnvsIusbFsImxZd26015541 = -775583686;    int OnvsIusbFsImxZd62864404 = -113514785;    int OnvsIusbFsImxZd15454706 = -92600751;    int OnvsIusbFsImxZd70829654 = -530704913;    int OnvsIusbFsImxZd20485286 = -337379884;    int OnvsIusbFsImxZd97339045 = -24985718;    int OnvsIusbFsImxZd45992934 = -524062941;    int OnvsIusbFsImxZd6141266 = -768702531;    int OnvsIusbFsImxZd88526511 = -342146519;    int OnvsIusbFsImxZd53968182 = -995702638;     OnvsIusbFsImxZd26051644 = OnvsIusbFsImxZd62903543;     OnvsIusbFsImxZd62903543 = OnvsIusbFsImxZd33755000;     OnvsIusbFsImxZd33755000 = OnvsIusbFsImxZd91164939;     OnvsIusbFsImxZd91164939 = OnvsIusbFsImxZd51539896;     OnvsIusbFsImxZd51539896 = OnvsIusbFsImxZd16003182;     OnvsIusbFsImxZd16003182 = OnvsIusbFsImxZd51727763;     OnvsIusbFsImxZd51727763 = OnvsIusbFsImxZd68503237;     OnvsIusbFsImxZd68503237 = OnvsIusbFsImxZd28333636;     OnvsIusbFsImxZd28333636 = OnvsIusbFsImxZd89830691;     OnvsIusbFsImxZd89830691 = OnvsIusbFsImxZd65356404;     OnvsIusbFsImxZd65356404 = OnvsIusbFsImxZd19502723;     OnvsIusbFsImxZd19502723 = OnvsIusbFsImxZd73326005;     OnvsIusbFsImxZd73326005 = OnvsIusbFsImxZd6917493;     OnvsIusbFsImxZd6917493 = OnvsIusbFsImxZd37058479;     OnvsIusbFsImxZd37058479 = OnvsIusbFsImxZd75574450;     OnvsIusbFsImxZd75574450 = OnvsIusbFsImxZd16134891;     OnvsIusbFsImxZd16134891 = OnvsIusbFsImxZd74957114;     OnvsIusbFsImxZd74957114 = OnvsIusbFsImxZd93839748;     OnvsIusbFsImxZd93839748 = OnvsIusbFsImxZd94536716;     OnvsIusbFsImxZd94536716 = OnvsIusbFsImxZd74033636;     OnvsIusbFsImxZd74033636 = OnvsIusbFsImxZd10829391;     OnvsIusbFsImxZd10829391 = OnvsIusbFsImxZd861372;     OnvsIusbFsImxZd861372 = OnvsIusbFsImxZd95549857;     OnvsIusbFsImxZd95549857 = OnvsIusbFsImxZd97676713;     OnvsIusbFsImxZd97676713 = OnvsIusbFsImxZd25201181;     OnvsIusbFsImxZd25201181 = OnvsIusbFsImxZd53022097;     OnvsIusbFsImxZd53022097 = OnvsIusbFsImxZd24509119;     OnvsIusbFsImxZd24509119 = OnvsIusbFsImxZd86659907;     OnvsIusbFsImxZd86659907 = OnvsIusbFsImxZd60743364;     OnvsIusbFsImxZd60743364 = OnvsIusbFsImxZd88054338;     OnvsIusbFsImxZd88054338 = OnvsIusbFsImxZd85445001;     OnvsIusbFsImxZd85445001 = OnvsIusbFsImxZd60178537;     OnvsIusbFsImxZd60178537 = OnvsIusbFsImxZd71953982;     OnvsIusbFsImxZd71953982 = OnvsIusbFsImxZd19627370;     OnvsIusbFsImxZd19627370 = OnvsIusbFsImxZd88057712;     OnvsIusbFsImxZd88057712 = OnvsIusbFsImxZd1069547;     OnvsIusbFsImxZd1069547 = OnvsIusbFsImxZd8942932;     OnvsIusbFsImxZd8942932 = OnvsIusbFsImxZd97168369;     OnvsIusbFsImxZd97168369 = OnvsIusbFsImxZd83466370;     OnvsIusbFsImxZd83466370 = OnvsIusbFsImxZd24557690;     OnvsIusbFsImxZd24557690 = OnvsIusbFsImxZd27836264;     OnvsIusbFsImxZd27836264 = OnvsIusbFsImxZd73731537;     OnvsIusbFsImxZd73731537 = OnvsIusbFsImxZd90302864;     OnvsIusbFsImxZd90302864 = OnvsIusbFsImxZd33879584;     OnvsIusbFsImxZd33879584 = OnvsIusbFsImxZd59714855;     OnvsIusbFsImxZd59714855 = OnvsIusbFsImxZd23959296;     OnvsIusbFsImxZd23959296 = OnvsIusbFsImxZd9965928;     OnvsIusbFsImxZd9965928 = OnvsIusbFsImxZd21001680;     OnvsIusbFsImxZd21001680 = OnvsIusbFsImxZd34882343;     OnvsIusbFsImxZd34882343 = OnvsIusbFsImxZd35542114;     OnvsIusbFsImxZd35542114 = OnvsIusbFsImxZd87317751;     OnvsIusbFsImxZd87317751 = OnvsIusbFsImxZd50937614;     OnvsIusbFsImxZd50937614 = OnvsIusbFsImxZd50583176;     OnvsIusbFsImxZd50583176 = OnvsIusbFsImxZd34916101;     OnvsIusbFsImxZd34916101 = OnvsIusbFsImxZd15222254;     OnvsIusbFsImxZd15222254 = OnvsIusbFsImxZd62042171;     OnvsIusbFsImxZd62042171 = OnvsIusbFsImxZd38205142;     OnvsIusbFsImxZd38205142 = OnvsIusbFsImxZd93488226;     OnvsIusbFsImxZd93488226 = OnvsIusbFsImxZd26338716;     OnvsIusbFsImxZd26338716 = OnvsIusbFsImxZd62981085;     OnvsIusbFsImxZd62981085 = OnvsIusbFsImxZd27218644;     OnvsIusbFsImxZd27218644 = OnvsIusbFsImxZd81843329;     OnvsIusbFsImxZd81843329 = OnvsIusbFsImxZd67590272;     OnvsIusbFsImxZd67590272 = OnvsIusbFsImxZd1776353;     OnvsIusbFsImxZd1776353 = OnvsIusbFsImxZd79911402;     OnvsIusbFsImxZd79911402 = OnvsIusbFsImxZd59324185;     OnvsIusbFsImxZd59324185 = OnvsIusbFsImxZd1372024;     OnvsIusbFsImxZd1372024 = OnvsIusbFsImxZd87290122;     OnvsIusbFsImxZd87290122 = OnvsIusbFsImxZd49000767;     OnvsIusbFsImxZd49000767 = OnvsIusbFsImxZd74504904;     OnvsIusbFsImxZd74504904 = OnvsIusbFsImxZd7191959;     OnvsIusbFsImxZd7191959 = OnvsIusbFsImxZd77788744;     OnvsIusbFsImxZd77788744 = OnvsIusbFsImxZd10373378;     OnvsIusbFsImxZd10373378 = OnvsIusbFsImxZd69979027;     OnvsIusbFsImxZd69979027 = OnvsIusbFsImxZd46197372;     OnvsIusbFsImxZd46197372 = OnvsIusbFsImxZd37097854;     OnvsIusbFsImxZd37097854 = OnvsIusbFsImxZd10558508;     OnvsIusbFsImxZd10558508 = OnvsIusbFsImxZd61670274;     OnvsIusbFsImxZd61670274 = OnvsIusbFsImxZd37961859;     OnvsIusbFsImxZd37961859 = OnvsIusbFsImxZd1241885;     OnvsIusbFsImxZd1241885 = OnvsIusbFsImxZd43056169;     OnvsIusbFsImxZd43056169 = OnvsIusbFsImxZd3507440;     OnvsIusbFsImxZd3507440 = OnvsIusbFsImxZd51777565;     OnvsIusbFsImxZd51777565 = OnvsIusbFsImxZd25201250;     OnvsIusbFsImxZd25201250 = OnvsIusbFsImxZd736587;     OnvsIusbFsImxZd736587 = OnvsIusbFsImxZd34507388;     OnvsIusbFsImxZd34507388 = OnvsIusbFsImxZd9595361;     OnvsIusbFsImxZd9595361 = OnvsIusbFsImxZd37037881;     OnvsIusbFsImxZd37037881 = OnvsIusbFsImxZd4405117;     OnvsIusbFsImxZd4405117 = OnvsIusbFsImxZd26015541;     OnvsIusbFsImxZd26015541 = OnvsIusbFsImxZd62864404;     OnvsIusbFsImxZd62864404 = OnvsIusbFsImxZd15454706;     OnvsIusbFsImxZd15454706 = OnvsIusbFsImxZd70829654;     OnvsIusbFsImxZd70829654 = OnvsIusbFsImxZd20485286;     OnvsIusbFsImxZd20485286 = OnvsIusbFsImxZd97339045;     OnvsIusbFsImxZd97339045 = OnvsIusbFsImxZd45992934;     OnvsIusbFsImxZd45992934 = OnvsIusbFsImxZd6141266;     OnvsIusbFsImxZd6141266 = OnvsIusbFsImxZd88526511;     OnvsIusbFsImxZd88526511 = OnvsIusbFsImxZd53968182;     OnvsIusbFsImxZd53968182 = OnvsIusbFsImxZd26051644;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void GpinkFnEVVmpoziR15608482() {     int sdgPefMzMeGBvQB26045173 = -868226780;    int sdgPefMzMeGBvQB1475860 = -794135133;    int sdgPefMzMeGBvQB35068212 = -707694758;    int sdgPefMzMeGBvQB76244800 = -28530794;    int sdgPefMzMeGBvQB60379998 = -804203504;    int sdgPefMzMeGBvQB50808614 = -387581104;    int sdgPefMzMeGBvQB20275099 = -599717386;    int sdgPefMzMeGBvQB37717288 = -257970713;    int sdgPefMzMeGBvQB11073800 = -927000426;    int sdgPefMzMeGBvQB86470204 = -760900856;    int sdgPefMzMeGBvQB85677009 = -100103680;    int sdgPefMzMeGBvQB33794399 = -169158065;    int sdgPefMzMeGBvQB5178920 = -179774926;    int sdgPefMzMeGBvQB60761713 = -734363007;    int sdgPefMzMeGBvQB82230588 = -686647714;    int sdgPefMzMeGBvQB50138998 = -13621581;    int sdgPefMzMeGBvQB71987649 = -558791315;    int sdgPefMzMeGBvQB97957646 = 33651791;    int sdgPefMzMeGBvQB14268018 = -51703030;    int sdgPefMzMeGBvQB66610479 = -500587023;    int sdgPefMzMeGBvQB53703060 = -211022211;    int sdgPefMzMeGBvQB89125656 = -962974808;    int sdgPefMzMeGBvQB37739768 = -751282277;    int sdgPefMzMeGBvQB39199966 = 30814541;    int sdgPefMzMeGBvQB26119403 = -73856361;    int sdgPefMzMeGBvQB41775257 = -780160420;    int sdgPefMzMeGBvQB23243679 = -291689933;    int sdgPefMzMeGBvQB59912644 = -432416997;    int sdgPefMzMeGBvQB99338354 = -657301309;    int sdgPefMzMeGBvQB36833234 = -976391580;    int sdgPefMzMeGBvQB21467017 = -284844413;    int sdgPefMzMeGBvQB70168859 = 48700714;    int sdgPefMzMeGBvQB45630960 = -959592253;    int sdgPefMzMeGBvQB72236819 = -791958714;    int sdgPefMzMeGBvQB1985409 = -846030844;    int sdgPefMzMeGBvQB30885902 = -214565005;    int sdgPefMzMeGBvQB13010914 = -875469371;    int sdgPefMzMeGBvQB78860611 = -648690886;    int sdgPefMzMeGBvQB32481967 = -707280020;    int sdgPefMzMeGBvQB55339263 = -644069643;    int sdgPefMzMeGBvQB12413358 = -84660527;    int sdgPefMzMeGBvQB13533925 = -160417426;    int sdgPefMzMeGBvQB34014897 = -140226934;    int sdgPefMzMeGBvQB49534762 = -25453769;    int sdgPefMzMeGBvQB17897636 = -84171508;    int sdgPefMzMeGBvQB142824 = 7805978;    int sdgPefMzMeGBvQB51657088 = -531655730;    int sdgPefMzMeGBvQB85349508 = -949549640;    int sdgPefMzMeGBvQB39660069 = -60702805;    int sdgPefMzMeGBvQB54927766 = -580860813;    int sdgPefMzMeGBvQB48801483 = -868648762;    int sdgPefMzMeGBvQB1865010 = -106946887;    int sdgPefMzMeGBvQB26385963 = -190779329;    int sdgPefMzMeGBvQB60833447 = -569565659;    int sdgPefMzMeGBvQB61905421 = -662693444;    int sdgPefMzMeGBvQB36919516 = -905251972;    int sdgPefMzMeGBvQB63736091 = 57147143;    int sdgPefMzMeGBvQB95868246 = -638509299;    int sdgPefMzMeGBvQB50125397 = -954674433;    int sdgPefMzMeGBvQB18604741 = 75956915;    int sdgPefMzMeGBvQB27564936 = 4108828;    int sdgPefMzMeGBvQB60362455 = -67300390;    int sdgPefMzMeGBvQB38378934 = -600669405;    int sdgPefMzMeGBvQB74240566 = -950608846;    int sdgPefMzMeGBvQB65003188 = -376056443;    int sdgPefMzMeGBvQB15508150 = -48804395;    int sdgPefMzMeGBvQB88163439 = -209565813;    int sdgPefMzMeGBvQB32942100 = -387816212;    int sdgPefMzMeGBvQB58776305 = -888332164;    int sdgPefMzMeGBvQB51344687 = -372082710;    int sdgPefMzMeGBvQB37128084 = -138152210;    int sdgPefMzMeGBvQB93127038 = -910100430;    int sdgPefMzMeGBvQB65475680 = -259068190;    int sdgPefMzMeGBvQB58928754 = -407633387;    int sdgPefMzMeGBvQB54197122 = -315926496;    int sdgPefMzMeGBvQB40169136 = 49395214;    int sdgPefMzMeGBvQB55110760 = -722747874;    int sdgPefMzMeGBvQB88205006 = -625828509;    int sdgPefMzMeGBvQB21302330 = -885013952;    int sdgPefMzMeGBvQB25976580 = 18337660;    int sdgPefMzMeGBvQB90118169 = -148504690;    int sdgPefMzMeGBvQB37894170 = -342140293;    int sdgPefMzMeGBvQB20252575 = -271714193;    int sdgPefMzMeGBvQB44410589 = 23559504;    int sdgPefMzMeGBvQB88031750 = -7742819;    int sdgPefMzMeGBvQB19602008 = -77897527;    int sdgPefMzMeGBvQB43782897 = -760519957;    int sdgPefMzMeGBvQB84797512 = -290026594;    int sdgPefMzMeGBvQB10331398 = -29265271;    int sdgPefMzMeGBvQB65065892 = -940778872;    int sdgPefMzMeGBvQB67149810 = -171712149;    int sdgPefMzMeGBvQB17142668 = -136960072;    int sdgPefMzMeGBvQB28735214 = -694016453;    int sdgPefMzMeGBvQB13877226 = -683236936;    int sdgPefMzMeGBvQB27774328 = -548178471;    int sdgPefMzMeGBvQB52050902 = 82639862;    int sdgPefMzMeGBvQB75154991 = -559748021;    int sdgPefMzMeGBvQB59774331 = -189618089;    int sdgPefMzMeGBvQB84531574 = -649397326;    int sdgPefMzMeGBvQB2389486 = -868226780;     sdgPefMzMeGBvQB26045173 = sdgPefMzMeGBvQB1475860;     sdgPefMzMeGBvQB1475860 = sdgPefMzMeGBvQB35068212;     sdgPefMzMeGBvQB35068212 = sdgPefMzMeGBvQB76244800;     sdgPefMzMeGBvQB76244800 = sdgPefMzMeGBvQB60379998;     sdgPefMzMeGBvQB60379998 = sdgPefMzMeGBvQB50808614;     sdgPefMzMeGBvQB50808614 = sdgPefMzMeGBvQB20275099;     sdgPefMzMeGBvQB20275099 = sdgPefMzMeGBvQB37717288;     sdgPefMzMeGBvQB37717288 = sdgPefMzMeGBvQB11073800;     sdgPefMzMeGBvQB11073800 = sdgPefMzMeGBvQB86470204;     sdgPefMzMeGBvQB86470204 = sdgPefMzMeGBvQB85677009;     sdgPefMzMeGBvQB85677009 = sdgPefMzMeGBvQB33794399;     sdgPefMzMeGBvQB33794399 = sdgPefMzMeGBvQB5178920;     sdgPefMzMeGBvQB5178920 = sdgPefMzMeGBvQB60761713;     sdgPefMzMeGBvQB60761713 = sdgPefMzMeGBvQB82230588;     sdgPefMzMeGBvQB82230588 = sdgPefMzMeGBvQB50138998;     sdgPefMzMeGBvQB50138998 = sdgPefMzMeGBvQB71987649;     sdgPefMzMeGBvQB71987649 = sdgPefMzMeGBvQB97957646;     sdgPefMzMeGBvQB97957646 = sdgPefMzMeGBvQB14268018;     sdgPefMzMeGBvQB14268018 = sdgPefMzMeGBvQB66610479;     sdgPefMzMeGBvQB66610479 = sdgPefMzMeGBvQB53703060;     sdgPefMzMeGBvQB53703060 = sdgPefMzMeGBvQB89125656;     sdgPefMzMeGBvQB89125656 = sdgPefMzMeGBvQB37739768;     sdgPefMzMeGBvQB37739768 = sdgPefMzMeGBvQB39199966;     sdgPefMzMeGBvQB39199966 = sdgPefMzMeGBvQB26119403;     sdgPefMzMeGBvQB26119403 = sdgPefMzMeGBvQB41775257;     sdgPefMzMeGBvQB41775257 = sdgPefMzMeGBvQB23243679;     sdgPefMzMeGBvQB23243679 = sdgPefMzMeGBvQB59912644;     sdgPefMzMeGBvQB59912644 = sdgPefMzMeGBvQB99338354;     sdgPefMzMeGBvQB99338354 = sdgPefMzMeGBvQB36833234;     sdgPefMzMeGBvQB36833234 = sdgPefMzMeGBvQB21467017;     sdgPefMzMeGBvQB21467017 = sdgPefMzMeGBvQB70168859;     sdgPefMzMeGBvQB70168859 = sdgPefMzMeGBvQB45630960;     sdgPefMzMeGBvQB45630960 = sdgPefMzMeGBvQB72236819;     sdgPefMzMeGBvQB72236819 = sdgPefMzMeGBvQB1985409;     sdgPefMzMeGBvQB1985409 = sdgPefMzMeGBvQB30885902;     sdgPefMzMeGBvQB30885902 = sdgPefMzMeGBvQB13010914;     sdgPefMzMeGBvQB13010914 = sdgPefMzMeGBvQB78860611;     sdgPefMzMeGBvQB78860611 = sdgPefMzMeGBvQB32481967;     sdgPefMzMeGBvQB32481967 = sdgPefMzMeGBvQB55339263;     sdgPefMzMeGBvQB55339263 = sdgPefMzMeGBvQB12413358;     sdgPefMzMeGBvQB12413358 = sdgPefMzMeGBvQB13533925;     sdgPefMzMeGBvQB13533925 = sdgPefMzMeGBvQB34014897;     sdgPefMzMeGBvQB34014897 = sdgPefMzMeGBvQB49534762;     sdgPefMzMeGBvQB49534762 = sdgPefMzMeGBvQB17897636;     sdgPefMzMeGBvQB17897636 = sdgPefMzMeGBvQB142824;     sdgPefMzMeGBvQB142824 = sdgPefMzMeGBvQB51657088;     sdgPefMzMeGBvQB51657088 = sdgPefMzMeGBvQB85349508;     sdgPefMzMeGBvQB85349508 = sdgPefMzMeGBvQB39660069;     sdgPefMzMeGBvQB39660069 = sdgPefMzMeGBvQB54927766;     sdgPefMzMeGBvQB54927766 = sdgPefMzMeGBvQB48801483;     sdgPefMzMeGBvQB48801483 = sdgPefMzMeGBvQB1865010;     sdgPefMzMeGBvQB1865010 = sdgPefMzMeGBvQB26385963;     sdgPefMzMeGBvQB26385963 = sdgPefMzMeGBvQB60833447;     sdgPefMzMeGBvQB60833447 = sdgPefMzMeGBvQB61905421;     sdgPefMzMeGBvQB61905421 = sdgPefMzMeGBvQB36919516;     sdgPefMzMeGBvQB36919516 = sdgPefMzMeGBvQB63736091;     sdgPefMzMeGBvQB63736091 = sdgPefMzMeGBvQB95868246;     sdgPefMzMeGBvQB95868246 = sdgPefMzMeGBvQB50125397;     sdgPefMzMeGBvQB50125397 = sdgPefMzMeGBvQB18604741;     sdgPefMzMeGBvQB18604741 = sdgPefMzMeGBvQB27564936;     sdgPefMzMeGBvQB27564936 = sdgPefMzMeGBvQB60362455;     sdgPefMzMeGBvQB60362455 = sdgPefMzMeGBvQB38378934;     sdgPefMzMeGBvQB38378934 = sdgPefMzMeGBvQB74240566;     sdgPefMzMeGBvQB74240566 = sdgPefMzMeGBvQB65003188;     sdgPefMzMeGBvQB65003188 = sdgPefMzMeGBvQB15508150;     sdgPefMzMeGBvQB15508150 = sdgPefMzMeGBvQB88163439;     sdgPefMzMeGBvQB88163439 = sdgPefMzMeGBvQB32942100;     sdgPefMzMeGBvQB32942100 = sdgPefMzMeGBvQB58776305;     sdgPefMzMeGBvQB58776305 = sdgPefMzMeGBvQB51344687;     sdgPefMzMeGBvQB51344687 = sdgPefMzMeGBvQB37128084;     sdgPefMzMeGBvQB37128084 = sdgPefMzMeGBvQB93127038;     sdgPefMzMeGBvQB93127038 = sdgPefMzMeGBvQB65475680;     sdgPefMzMeGBvQB65475680 = sdgPefMzMeGBvQB58928754;     sdgPefMzMeGBvQB58928754 = sdgPefMzMeGBvQB54197122;     sdgPefMzMeGBvQB54197122 = sdgPefMzMeGBvQB40169136;     sdgPefMzMeGBvQB40169136 = sdgPefMzMeGBvQB55110760;     sdgPefMzMeGBvQB55110760 = sdgPefMzMeGBvQB88205006;     sdgPefMzMeGBvQB88205006 = sdgPefMzMeGBvQB21302330;     sdgPefMzMeGBvQB21302330 = sdgPefMzMeGBvQB25976580;     sdgPefMzMeGBvQB25976580 = sdgPefMzMeGBvQB90118169;     sdgPefMzMeGBvQB90118169 = sdgPefMzMeGBvQB37894170;     sdgPefMzMeGBvQB37894170 = sdgPefMzMeGBvQB20252575;     sdgPefMzMeGBvQB20252575 = sdgPefMzMeGBvQB44410589;     sdgPefMzMeGBvQB44410589 = sdgPefMzMeGBvQB88031750;     sdgPefMzMeGBvQB88031750 = sdgPefMzMeGBvQB19602008;     sdgPefMzMeGBvQB19602008 = sdgPefMzMeGBvQB43782897;     sdgPefMzMeGBvQB43782897 = sdgPefMzMeGBvQB84797512;     sdgPefMzMeGBvQB84797512 = sdgPefMzMeGBvQB10331398;     sdgPefMzMeGBvQB10331398 = sdgPefMzMeGBvQB65065892;     sdgPefMzMeGBvQB65065892 = sdgPefMzMeGBvQB67149810;     sdgPefMzMeGBvQB67149810 = sdgPefMzMeGBvQB17142668;     sdgPefMzMeGBvQB17142668 = sdgPefMzMeGBvQB28735214;     sdgPefMzMeGBvQB28735214 = sdgPefMzMeGBvQB13877226;     sdgPefMzMeGBvQB13877226 = sdgPefMzMeGBvQB27774328;     sdgPefMzMeGBvQB27774328 = sdgPefMzMeGBvQB52050902;     sdgPefMzMeGBvQB52050902 = sdgPefMzMeGBvQB75154991;     sdgPefMzMeGBvQB75154991 = sdgPefMzMeGBvQB59774331;     sdgPefMzMeGBvQB59774331 = sdgPefMzMeGBvQB84531574;     sdgPefMzMeGBvQB84531574 = sdgPefMzMeGBvQB2389486;     sdgPefMzMeGBvQB2389486 = sdgPefMzMeGBvQB26045173;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ZfIZnmmNcLyBYmAV70024695() {     int WWCxWzVmlDjmPFE96696839 = -386375910;    int WWCxWzVmlDjmPFE70149377 = -665778432;    int WWCxWzVmlDjmPFE15037132 = -49989218;    int WWCxWzVmlDjmPFE16176087 = -218669183;    int WWCxWzVmlDjmPFE81830319 = -587609851;    int WWCxWzVmlDjmPFE40808377 = -89162256;    int WWCxWzVmlDjmPFE38653995 = -598493096;    int WWCxWzVmlDjmPFE94559668 = -604787803;    int WWCxWzVmlDjmPFE56938854 = 79652245;    int WWCxWzVmlDjmPFE9436666 = 38003794;    int WWCxWzVmlDjmPFE27926170 = -135005610;    int WWCxWzVmlDjmPFE79340970 = -484921858;    int WWCxWzVmlDjmPFE85006306 = -89311985;    int WWCxWzVmlDjmPFE44608747 = -535391217;    int WWCxWzVmlDjmPFE64950607 = -836397718;    int WWCxWzVmlDjmPFE82667638 = -683605167;    int WWCxWzVmlDjmPFE65840476 = -467976445;    int WWCxWzVmlDjmPFE18251142 = 44485595;    int WWCxWzVmlDjmPFE95975799 = -758890972;    int WWCxWzVmlDjmPFE25397189 = 57046673;    int WWCxWzVmlDjmPFE8838815 = -119701352;    int WWCxWzVmlDjmPFE50521421 = -194260114;    int WWCxWzVmlDjmPFE39683577 = -219125422;    int WWCxWzVmlDjmPFE73049503 = -661586366;    int WWCxWzVmlDjmPFE90243791 = -849761049;    int WWCxWzVmlDjmPFE72372838 = -878444864;    int WWCxWzVmlDjmPFE31504465 = -686867708;    int WWCxWzVmlDjmPFE67694869 = -937540765;    int WWCxWzVmlDjmPFE31480843 = -951659526;    int WWCxWzVmlDjmPFE76005329 = -434786651;    int WWCxWzVmlDjmPFE28539014 = -33278099;    int WWCxWzVmlDjmPFE14680320 = -279596449;    int WWCxWzVmlDjmPFE28576321 = -906553862;    int WWCxWzVmlDjmPFE37601794 = -355488399;    int WWCxWzVmlDjmPFE80009644 = -692977623;    int WWCxWzVmlDjmPFE90879160 = -293791553;    int WWCxWzVmlDjmPFE56802400 = 35937421;    int WWCxWzVmlDjmPFE31653100 = -432803993;    int WWCxWzVmlDjmPFE11015256 = -663008657;    int WWCxWzVmlDjmPFE50442562 = -161678006;    int WWCxWzVmlDjmPFE72486641 = -468679406;    int WWCxWzVmlDjmPFE42016916 = 70193505;    int WWCxWzVmlDjmPFE82981857 = -126980986;    int WWCxWzVmlDjmPFE56043063 = -97428740;    int WWCxWzVmlDjmPFE13851695 = -392817903;    int WWCxWzVmlDjmPFE6548658 = -850531177;    int WWCxWzVmlDjmPFE77788291 = -647725529;    int WWCxWzVmlDjmPFE50963719 = -301679935;    int WWCxWzVmlDjmPFE45101521 = -46466774;    int WWCxWzVmlDjmPFE63355252 = -863869691;    int WWCxWzVmlDjmPFE16990808 = -415220883;    int WWCxWzVmlDjmPFE43736514 = -76166609;    int WWCxWzVmlDjmPFE81461619 = -118577371;    int WWCxWzVmlDjmPFE51060752 = -130447156;    int WWCxWzVmlDjmPFE61185880 = -773634358;    int WWCxWzVmlDjmPFE46175418 = -92115797;    int WWCxWzVmlDjmPFE30465800 = -346653011;    int WWCxWzVmlDjmPFE41987628 = -388402853;    int WWCxWzVmlDjmPFE25932296 = -368908135;    int WWCxWzVmlDjmPFE9457482 = -709164987;    int WWCxWzVmlDjmPFE9303913 = -402294549;    int WWCxWzVmlDjmPFE70959125 = -660952332;    int WWCxWzVmlDjmPFE63078826 = -653128278;    int WWCxWzVmlDjmPFE80933524 = -485561104;    int WWCxWzVmlDjmPFE80897652 = -928718108;    int WWCxWzVmlDjmPFE13245850 = -855409162;    int WWCxWzVmlDjmPFE50764650 = -578367997;    int WWCxWzVmlDjmPFE47404512 = -733823586;    int WWCxWzVmlDjmPFE64599102 = -842413595;    int WWCxWzVmlDjmPFE74071447 = -442606165;    int WWCxWzVmlDjmPFE25865239 = -619542589;    int WWCxWzVmlDjmPFE34187377 = 64827547;    int WWCxWzVmlDjmPFE7235886 = -292505749;    int WWCxWzVmlDjmPFE45533238 = -497212966;    int WWCxWzVmlDjmPFE52910547 = -474273922;    int WWCxWzVmlDjmPFE66821899 = -89894857;    int WWCxWzVmlDjmPFE67539564 = 32720872;    int WWCxWzVmlDjmPFE83640514 = -21696682;    int WWCxWzVmlDjmPFE59197808 = -168768463;    int WWCxWzVmlDjmPFE83695133 = -999229873;    int WWCxWzVmlDjmPFE94584546 = -130719336;    int WWCxWzVmlDjmPFE80540746 = -285187773;    int WWCxWzVmlDjmPFE22593348 = -791073991;    int WWCxWzVmlDjmPFE68125590 = 12210165;    int WWCxWzVmlDjmPFE59014522 = 80434232;    int WWCxWzVmlDjmPFE84802500 = -957111490;    int WWCxWzVmlDjmPFE33218700 = -61019078;    int WWCxWzVmlDjmPFE77515569 = -676106706;    int WWCxWzVmlDjmPFE76415914 = -581854041;    int WWCxWzVmlDjmPFE33834226 = -500861826;    int WWCxWzVmlDjmPFE60413360 = -947138542;    int WWCxWzVmlDjmPFE14814772 = -575659726;    int WWCxWzVmlDjmPFE5720805 = 36104141;    int WWCxWzVmlDjmPFE1557774 = -953843670;    int WWCxWzVmlDjmPFE41138649 = -759383458;    int WWCxWzVmlDjmPFE1527516 = -807727074;    int WWCxWzVmlDjmPFE78938089 = -276678218;    int WWCxWzVmlDjmPFE2048333 = -641419883;    int WWCxWzVmlDjmPFE75145410 = -168710633;    int WWCxWzVmlDjmPFE605845 = -386375910;     WWCxWzVmlDjmPFE96696839 = WWCxWzVmlDjmPFE70149377;     WWCxWzVmlDjmPFE70149377 = WWCxWzVmlDjmPFE15037132;     WWCxWzVmlDjmPFE15037132 = WWCxWzVmlDjmPFE16176087;     WWCxWzVmlDjmPFE16176087 = WWCxWzVmlDjmPFE81830319;     WWCxWzVmlDjmPFE81830319 = WWCxWzVmlDjmPFE40808377;     WWCxWzVmlDjmPFE40808377 = WWCxWzVmlDjmPFE38653995;     WWCxWzVmlDjmPFE38653995 = WWCxWzVmlDjmPFE94559668;     WWCxWzVmlDjmPFE94559668 = WWCxWzVmlDjmPFE56938854;     WWCxWzVmlDjmPFE56938854 = WWCxWzVmlDjmPFE9436666;     WWCxWzVmlDjmPFE9436666 = WWCxWzVmlDjmPFE27926170;     WWCxWzVmlDjmPFE27926170 = WWCxWzVmlDjmPFE79340970;     WWCxWzVmlDjmPFE79340970 = WWCxWzVmlDjmPFE85006306;     WWCxWzVmlDjmPFE85006306 = WWCxWzVmlDjmPFE44608747;     WWCxWzVmlDjmPFE44608747 = WWCxWzVmlDjmPFE64950607;     WWCxWzVmlDjmPFE64950607 = WWCxWzVmlDjmPFE82667638;     WWCxWzVmlDjmPFE82667638 = WWCxWzVmlDjmPFE65840476;     WWCxWzVmlDjmPFE65840476 = WWCxWzVmlDjmPFE18251142;     WWCxWzVmlDjmPFE18251142 = WWCxWzVmlDjmPFE95975799;     WWCxWzVmlDjmPFE95975799 = WWCxWzVmlDjmPFE25397189;     WWCxWzVmlDjmPFE25397189 = WWCxWzVmlDjmPFE8838815;     WWCxWzVmlDjmPFE8838815 = WWCxWzVmlDjmPFE50521421;     WWCxWzVmlDjmPFE50521421 = WWCxWzVmlDjmPFE39683577;     WWCxWzVmlDjmPFE39683577 = WWCxWzVmlDjmPFE73049503;     WWCxWzVmlDjmPFE73049503 = WWCxWzVmlDjmPFE90243791;     WWCxWzVmlDjmPFE90243791 = WWCxWzVmlDjmPFE72372838;     WWCxWzVmlDjmPFE72372838 = WWCxWzVmlDjmPFE31504465;     WWCxWzVmlDjmPFE31504465 = WWCxWzVmlDjmPFE67694869;     WWCxWzVmlDjmPFE67694869 = WWCxWzVmlDjmPFE31480843;     WWCxWzVmlDjmPFE31480843 = WWCxWzVmlDjmPFE76005329;     WWCxWzVmlDjmPFE76005329 = WWCxWzVmlDjmPFE28539014;     WWCxWzVmlDjmPFE28539014 = WWCxWzVmlDjmPFE14680320;     WWCxWzVmlDjmPFE14680320 = WWCxWzVmlDjmPFE28576321;     WWCxWzVmlDjmPFE28576321 = WWCxWzVmlDjmPFE37601794;     WWCxWzVmlDjmPFE37601794 = WWCxWzVmlDjmPFE80009644;     WWCxWzVmlDjmPFE80009644 = WWCxWzVmlDjmPFE90879160;     WWCxWzVmlDjmPFE90879160 = WWCxWzVmlDjmPFE56802400;     WWCxWzVmlDjmPFE56802400 = WWCxWzVmlDjmPFE31653100;     WWCxWzVmlDjmPFE31653100 = WWCxWzVmlDjmPFE11015256;     WWCxWzVmlDjmPFE11015256 = WWCxWzVmlDjmPFE50442562;     WWCxWzVmlDjmPFE50442562 = WWCxWzVmlDjmPFE72486641;     WWCxWzVmlDjmPFE72486641 = WWCxWzVmlDjmPFE42016916;     WWCxWzVmlDjmPFE42016916 = WWCxWzVmlDjmPFE82981857;     WWCxWzVmlDjmPFE82981857 = WWCxWzVmlDjmPFE56043063;     WWCxWzVmlDjmPFE56043063 = WWCxWzVmlDjmPFE13851695;     WWCxWzVmlDjmPFE13851695 = WWCxWzVmlDjmPFE6548658;     WWCxWzVmlDjmPFE6548658 = WWCxWzVmlDjmPFE77788291;     WWCxWzVmlDjmPFE77788291 = WWCxWzVmlDjmPFE50963719;     WWCxWzVmlDjmPFE50963719 = WWCxWzVmlDjmPFE45101521;     WWCxWzVmlDjmPFE45101521 = WWCxWzVmlDjmPFE63355252;     WWCxWzVmlDjmPFE63355252 = WWCxWzVmlDjmPFE16990808;     WWCxWzVmlDjmPFE16990808 = WWCxWzVmlDjmPFE43736514;     WWCxWzVmlDjmPFE43736514 = WWCxWzVmlDjmPFE81461619;     WWCxWzVmlDjmPFE81461619 = WWCxWzVmlDjmPFE51060752;     WWCxWzVmlDjmPFE51060752 = WWCxWzVmlDjmPFE61185880;     WWCxWzVmlDjmPFE61185880 = WWCxWzVmlDjmPFE46175418;     WWCxWzVmlDjmPFE46175418 = WWCxWzVmlDjmPFE30465800;     WWCxWzVmlDjmPFE30465800 = WWCxWzVmlDjmPFE41987628;     WWCxWzVmlDjmPFE41987628 = WWCxWzVmlDjmPFE25932296;     WWCxWzVmlDjmPFE25932296 = WWCxWzVmlDjmPFE9457482;     WWCxWzVmlDjmPFE9457482 = WWCxWzVmlDjmPFE9303913;     WWCxWzVmlDjmPFE9303913 = WWCxWzVmlDjmPFE70959125;     WWCxWzVmlDjmPFE70959125 = WWCxWzVmlDjmPFE63078826;     WWCxWzVmlDjmPFE63078826 = WWCxWzVmlDjmPFE80933524;     WWCxWzVmlDjmPFE80933524 = WWCxWzVmlDjmPFE80897652;     WWCxWzVmlDjmPFE80897652 = WWCxWzVmlDjmPFE13245850;     WWCxWzVmlDjmPFE13245850 = WWCxWzVmlDjmPFE50764650;     WWCxWzVmlDjmPFE50764650 = WWCxWzVmlDjmPFE47404512;     WWCxWzVmlDjmPFE47404512 = WWCxWzVmlDjmPFE64599102;     WWCxWzVmlDjmPFE64599102 = WWCxWzVmlDjmPFE74071447;     WWCxWzVmlDjmPFE74071447 = WWCxWzVmlDjmPFE25865239;     WWCxWzVmlDjmPFE25865239 = WWCxWzVmlDjmPFE34187377;     WWCxWzVmlDjmPFE34187377 = WWCxWzVmlDjmPFE7235886;     WWCxWzVmlDjmPFE7235886 = WWCxWzVmlDjmPFE45533238;     WWCxWzVmlDjmPFE45533238 = WWCxWzVmlDjmPFE52910547;     WWCxWzVmlDjmPFE52910547 = WWCxWzVmlDjmPFE66821899;     WWCxWzVmlDjmPFE66821899 = WWCxWzVmlDjmPFE67539564;     WWCxWzVmlDjmPFE67539564 = WWCxWzVmlDjmPFE83640514;     WWCxWzVmlDjmPFE83640514 = WWCxWzVmlDjmPFE59197808;     WWCxWzVmlDjmPFE59197808 = WWCxWzVmlDjmPFE83695133;     WWCxWzVmlDjmPFE83695133 = WWCxWzVmlDjmPFE94584546;     WWCxWzVmlDjmPFE94584546 = WWCxWzVmlDjmPFE80540746;     WWCxWzVmlDjmPFE80540746 = WWCxWzVmlDjmPFE22593348;     WWCxWzVmlDjmPFE22593348 = WWCxWzVmlDjmPFE68125590;     WWCxWzVmlDjmPFE68125590 = WWCxWzVmlDjmPFE59014522;     WWCxWzVmlDjmPFE59014522 = WWCxWzVmlDjmPFE84802500;     WWCxWzVmlDjmPFE84802500 = WWCxWzVmlDjmPFE33218700;     WWCxWzVmlDjmPFE33218700 = WWCxWzVmlDjmPFE77515569;     WWCxWzVmlDjmPFE77515569 = WWCxWzVmlDjmPFE76415914;     WWCxWzVmlDjmPFE76415914 = WWCxWzVmlDjmPFE33834226;     WWCxWzVmlDjmPFE33834226 = WWCxWzVmlDjmPFE60413360;     WWCxWzVmlDjmPFE60413360 = WWCxWzVmlDjmPFE14814772;     WWCxWzVmlDjmPFE14814772 = WWCxWzVmlDjmPFE5720805;     WWCxWzVmlDjmPFE5720805 = WWCxWzVmlDjmPFE1557774;     WWCxWzVmlDjmPFE1557774 = WWCxWzVmlDjmPFE41138649;     WWCxWzVmlDjmPFE41138649 = WWCxWzVmlDjmPFE1527516;     WWCxWzVmlDjmPFE1527516 = WWCxWzVmlDjmPFE78938089;     WWCxWzVmlDjmPFE78938089 = WWCxWzVmlDjmPFE2048333;     WWCxWzVmlDjmPFE2048333 = WWCxWzVmlDjmPFE75145410;     WWCxWzVmlDjmPFE75145410 = WWCxWzVmlDjmPFE605845;     WWCxWzVmlDjmPFE605845 = WWCxWzVmlDjmPFE96696839;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void zSSlIvhGDnskyLNo76683440() {     int IZBbVGmQVVlaTLe96690368 = -258900050;    int IZBbVGmQVVlaTLe8721694 = -708373551;    int IZBbVGmQVVlaTLe16350344 = -7117832;    int IZBbVGmQVVlaTLe1255948 = -39210016;    int IZBbVGmQVVlaTLe90670421 = 44034836;    int IZBbVGmQVVlaTLe75613809 = -888595068;    int IZBbVGmQVVlaTLe7201331 = -303029563;    int IZBbVGmQVVlaTLe63773719 = -308965105;    int IZBbVGmQVVlaTLe39679018 = -733993824;    int IZBbVGmQVVlaTLe6076180 = -698803077;    int IZBbVGmQVVlaTLe48246775 = -94477681;    int IZBbVGmQVVlaTLe93632647 = -220400211;    int IZBbVGmQVVlaTLe16859221 = -110970459;    int IZBbVGmQVVlaTLe98452966 = -880544268;    int IZBbVGmQVVlaTLe10122717 = -799110551;    int IZBbVGmQVVlaTLe57232185 = 83599182;    int IZBbVGmQVVlaTLe21693236 = -287591015;    int IZBbVGmQVVlaTLe41251674 = -352586023;    int IZBbVGmQVVlaTLe16404069 = -276873239;    int IZBbVGmQVVlaTLe97470951 = -554977779;    int IZBbVGmQVVlaTLe88508239 = -930167720;    int IZBbVGmQVVlaTLe28817688 = -704251997;    int IZBbVGmQVVlaTLe76561973 = -678083264;    int IZBbVGmQVVlaTLe16699612 = -542506888;    int IZBbVGmQVVlaTLe18686481 = -176454890;    int IZBbVGmQVVlaTLe88946914 = -786650797;    int IZBbVGmQVVlaTLe1726047 = -987667421;    int IZBbVGmQVVlaTLe3098394 = -446507747;    int IZBbVGmQVVlaTLe44159289 = -497729856;    int IZBbVGmQVVlaTLe52095200 = -110218025;    int IZBbVGmQVVlaTLe61951692 = -420371333;    int IZBbVGmQVVlaTLe99404177 = -671418213;    int IZBbVGmQVVlaTLe14028744 = -8498081;    int IZBbVGmQVVlaTLe37884632 = -232170131;    int IZBbVGmQVVlaTLe62367682 = -952291551;    int IZBbVGmQVVlaTLe33707350 = -373557318;    int IZBbVGmQVVlaTLe68743767 = -63715957;    int IZBbVGmQVVlaTLe1570780 = -428066669;    int IZBbVGmQVVlaTLe46328852 = -275690085;    int IZBbVGmQVVlaTLe22315455 = -871109692;    int IZBbVGmQVVlaTLe60342309 = -556623304;    int IZBbVGmQVVlaTLe27714577 = -123598598;    int IZBbVGmQVVlaTLe43265217 = -686111243;    int IZBbVGmQVVlaTLe15274961 = -754393184;    int IZBbVGmQVVlaTLe97869747 = -866077306;    int IZBbVGmQVVlaTLe46976626 = -662575866;    int IZBbVGmQVVlaTLe5486085 = -29006151;    int IZBbVGmQVVlaTLe26347300 = -910441292;    int IZBbVGmQVVlaTLe63759911 = -710230660;    int IZBbVGmQVVlaTLe83400675 = -450894573;    int IZBbVGmQVVlaTLe30250178 = 6224110;    int IZBbVGmQVVlaTLe58283771 = -458312361;    int IZBbVGmQVVlaTLe56909968 = -10631376;    int IZBbVGmQVVlaTLe61311022 = -662367007;    int IZBbVGmQVVlaTLe88175200 = -530533641;    int IZBbVGmQVVlaTLe67872681 = -554648053;    int IZBbVGmQVVlaTLe32159721 = 69709712;    int IZBbVGmQVVlaTLe99650731 = -464610944;    int IZBbVGmQVVlaTLe82569466 = -862755127;    int IZBbVGmQVVlaTLe1723508 = -169314368;    int IZBbVGmQVVlaTLe73887763 = -900927647;    int IZBbVGmQVVlaTLe4102937 = -856521817;    int IZBbVGmQVVlaTLe19614430 = -811235250;    int IZBbVGmQVVlaTLe87583818 = -523775799;    int IZBbVGmQVVlaTLe44124487 = -178431745;    int IZBbVGmQVVlaTLe48842598 = -423059468;    int IZBbVGmQVVlaTLe79603903 = -111902131;    int IZBbVGmQVVlaTLe78974589 = -878800328;    int IZBbVGmQVVlaTLe36085284 = -928252718;    int IZBbVGmQVVlaTLe76415366 = -325553234;    int IZBbVGmQVVlaTLe88488417 = -852684862;    int IZBbVGmQVVlaTLe20122457 = -859524346;    int IZBbVGmQVVlaTLe94922822 = 23104062;    int IZBbVGmQVVlaTLe94088614 = -405763548;    int IZBbVGmQVVlaTLe37128643 = -998354475;    int IZBbVGmQVVlaTLe60793662 = -706569123;    int IZBbVGmQVVlaTLe85552470 = 81859245;    int IZBbVGmQVVlaTLe61287012 = -923690081;    int IZBbVGmQVVlaTLe18829865 = -676429583;    int IZBbVGmQVVlaTLe71709854 = -513879024;    int IZBbVGmQVVlaTLe83460830 = -657644646;    int IZBbVGmQVVlaTLe75378746 = 22773871;    int IZBbVGmQVVlaTLe39338483 = -736277088;    int IZBbVGmQVVlaTLe60758614 = 53164716;    int IZBbVGmQVVlaTLe21845022 = -16442135;    int IZBbVGmQVVlaTLe3667922 = -962058973;    int IZBbVGmQVVlaTLe42494209 = -560786838;    int IZBbVGmQVVlaTLe52717721 = -346131074;    int IZBbVGmQVVlaTLe49709431 = -701636491;    int IZBbVGmQVVlaTLe94495001 = -297643498;    int IZBbVGmQVVlaTLe1547630 = -343267030;    int IZBbVGmQVVlaTLe69093035 = -599105013;    int IZBbVGmQVVlaTLe19001313 = -565311543;    int IZBbVGmQVVlaTLe44605345 = -6375718;    int IZBbVGmQVVlaTLe48427692 = -970182045;    int IZBbVGmQVVlaTLe56239372 = -700101488;    int IZBbVGmQVVlaTLe8100147 = -312363349;    int IZBbVGmQVVlaTLe55681399 = -62335444;    int IZBbVGmQVVlaTLe71150474 = -475961440;    int IZBbVGmQVVlaTLe49027149 = -258900050;     IZBbVGmQVVlaTLe96690368 = IZBbVGmQVVlaTLe8721694;     IZBbVGmQVVlaTLe8721694 = IZBbVGmQVVlaTLe16350344;     IZBbVGmQVVlaTLe16350344 = IZBbVGmQVVlaTLe1255948;     IZBbVGmQVVlaTLe1255948 = IZBbVGmQVVlaTLe90670421;     IZBbVGmQVVlaTLe90670421 = IZBbVGmQVVlaTLe75613809;     IZBbVGmQVVlaTLe75613809 = IZBbVGmQVVlaTLe7201331;     IZBbVGmQVVlaTLe7201331 = IZBbVGmQVVlaTLe63773719;     IZBbVGmQVVlaTLe63773719 = IZBbVGmQVVlaTLe39679018;     IZBbVGmQVVlaTLe39679018 = IZBbVGmQVVlaTLe6076180;     IZBbVGmQVVlaTLe6076180 = IZBbVGmQVVlaTLe48246775;     IZBbVGmQVVlaTLe48246775 = IZBbVGmQVVlaTLe93632647;     IZBbVGmQVVlaTLe93632647 = IZBbVGmQVVlaTLe16859221;     IZBbVGmQVVlaTLe16859221 = IZBbVGmQVVlaTLe98452966;     IZBbVGmQVVlaTLe98452966 = IZBbVGmQVVlaTLe10122717;     IZBbVGmQVVlaTLe10122717 = IZBbVGmQVVlaTLe57232185;     IZBbVGmQVVlaTLe57232185 = IZBbVGmQVVlaTLe21693236;     IZBbVGmQVVlaTLe21693236 = IZBbVGmQVVlaTLe41251674;     IZBbVGmQVVlaTLe41251674 = IZBbVGmQVVlaTLe16404069;     IZBbVGmQVVlaTLe16404069 = IZBbVGmQVVlaTLe97470951;     IZBbVGmQVVlaTLe97470951 = IZBbVGmQVVlaTLe88508239;     IZBbVGmQVVlaTLe88508239 = IZBbVGmQVVlaTLe28817688;     IZBbVGmQVVlaTLe28817688 = IZBbVGmQVVlaTLe76561973;     IZBbVGmQVVlaTLe76561973 = IZBbVGmQVVlaTLe16699612;     IZBbVGmQVVlaTLe16699612 = IZBbVGmQVVlaTLe18686481;     IZBbVGmQVVlaTLe18686481 = IZBbVGmQVVlaTLe88946914;     IZBbVGmQVVlaTLe88946914 = IZBbVGmQVVlaTLe1726047;     IZBbVGmQVVlaTLe1726047 = IZBbVGmQVVlaTLe3098394;     IZBbVGmQVVlaTLe3098394 = IZBbVGmQVVlaTLe44159289;     IZBbVGmQVVlaTLe44159289 = IZBbVGmQVVlaTLe52095200;     IZBbVGmQVVlaTLe52095200 = IZBbVGmQVVlaTLe61951692;     IZBbVGmQVVlaTLe61951692 = IZBbVGmQVVlaTLe99404177;     IZBbVGmQVVlaTLe99404177 = IZBbVGmQVVlaTLe14028744;     IZBbVGmQVVlaTLe14028744 = IZBbVGmQVVlaTLe37884632;     IZBbVGmQVVlaTLe37884632 = IZBbVGmQVVlaTLe62367682;     IZBbVGmQVVlaTLe62367682 = IZBbVGmQVVlaTLe33707350;     IZBbVGmQVVlaTLe33707350 = IZBbVGmQVVlaTLe68743767;     IZBbVGmQVVlaTLe68743767 = IZBbVGmQVVlaTLe1570780;     IZBbVGmQVVlaTLe1570780 = IZBbVGmQVVlaTLe46328852;     IZBbVGmQVVlaTLe46328852 = IZBbVGmQVVlaTLe22315455;     IZBbVGmQVVlaTLe22315455 = IZBbVGmQVVlaTLe60342309;     IZBbVGmQVVlaTLe60342309 = IZBbVGmQVVlaTLe27714577;     IZBbVGmQVVlaTLe27714577 = IZBbVGmQVVlaTLe43265217;     IZBbVGmQVVlaTLe43265217 = IZBbVGmQVVlaTLe15274961;     IZBbVGmQVVlaTLe15274961 = IZBbVGmQVVlaTLe97869747;     IZBbVGmQVVlaTLe97869747 = IZBbVGmQVVlaTLe46976626;     IZBbVGmQVVlaTLe46976626 = IZBbVGmQVVlaTLe5486085;     IZBbVGmQVVlaTLe5486085 = IZBbVGmQVVlaTLe26347300;     IZBbVGmQVVlaTLe26347300 = IZBbVGmQVVlaTLe63759911;     IZBbVGmQVVlaTLe63759911 = IZBbVGmQVVlaTLe83400675;     IZBbVGmQVVlaTLe83400675 = IZBbVGmQVVlaTLe30250178;     IZBbVGmQVVlaTLe30250178 = IZBbVGmQVVlaTLe58283771;     IZBbVGmQVVlaTLe58283771 = IZBbVGmQVVlaTLe56909968;     IZBbVGmQVVlaTLe56909968 = IZBbVGmQVVlaTLe61311022;     IZBbVGmQVVlaTLe61311022 = IZBbVGmQVVlaTLe88175200;     IZBbVGmQVVlaTLe88175200 = IZBbVGmQVVlaTLe67872681;     IZBbVGmQVVlaTLe67872681 = IZBbVGmQVVlaTLe32159721;     IZBbVGmQVVlaTLe32159721 = IZBbVGmQVVlaTLe99650731;     IZBbVGmQVVlaTLe99650731 = IZBbVGmQVVlaTLe82569466;     IZBbVGmQVVlaTLe82569466 = IZBbVGmQVVlaTLe1723508;     IZBbVGmQVVlaTLe1723508 = IZBbVGmQVVlaTLe73887763;     IZBbVGmQVVlaTLe73887763 = IZBbVGmQVVlaTLe4102937;     IZBbVGmQVVlaTLe4102937 = IZBbVGmQVVlaTLe19614430;     IZBbVGmQVVlaTLe19614430 = IZBbVGmQVVlaTLe87583818;     IZBbVGmQVVlaTLe87583818 = IZBbVGmQVVlaTLe44124487;     IZBbVGmQVVlaTLe44124487 = IZBbVGmQVVlaTLe48842598;     IZBbVGmQVVlaTLe48842598 = IZBbVGmQVVlaTLe79603903;     IZBbVGmQVVlaTLe79603903 = IZBbVGmQVVlaTLe78974589;     IZBbVGmQVVlaTLe78974589 = IZBbVGmQVVlaTLe36085284;     IZBbVGmQVVlaTLe36085284 = IZBbVGmQVVlaTLe76415366;     IZBbVGmQVVlaTLe76415366 = IZBbVGmQVVlaTLe88488417;     IZBbVGmQVVlaTLe88488417 = IZBbVGmQVVlaTLe20122457;     IZBbVGmQVVlaTLe20122457 = IZBbVGmQVVlaTLe94922822;     IZBbVGmQVVlaTLe94922822 = IZBbVGmQVVlaTLe94088614;     IZBbVGmQVVlaTLe94088614 = IZBbVGmQVVlaTLe37128643;     IZBbVGmQVVlaTLe37128643 = IZBbVGmQVVlaTLe60793662;     IZBbVGmQVVlaTLe60793662 = IZBbVGmQVVlaTLe85552470;     IZBbVGmQVVlaTLe85552470 = IZBbVGmQVVlaTLe61287012;     IZBbVGmQVVlaTLe61287012 = IZBbVGmQVVlaTLe18829865;     IZBbVGmQVVlaTLe18829865 = IZBbVGmQVVlaTLe71709854;     IZBbVGmQVVlaTLe71709854 = IZBbVGmQVVlaTLe83460830;     IZBbVGmQVVlaTLe83460830 = IZBbVGmQVVlaTLe75378746;     IZBbVGmQVVlaTLe75378746 = IZBbVGmQVVlaTLe39338483;     IZBbVGmQVVlaTLe39338483 = IZBbVGmQVVlaTLe60758614;     IZBbVGmQVVlaTLe60758614 = IZBbVGmQVVlaTLe21845022;     IZBbVGmQVVlaTLe21845022 = IZBbVGmQVVlaTLe3667922;     IZBbVGmQVVlaTLe3667922 = IZBbVGmQVVlaTLe42494209;     IZBbVGmQVVlaTLe42494209 = IZBbVGmQVVlaTLe52717721;     IZBbVGmQVVlaTLe52717721 = IZBbVGmQVVlaTLe49709431;     IZBbVGmQVVlaTLe49709431 = IZBbVGmQVVlaTLe94495001;     IZBbVGmQVVlaTLe94495001 = IZBbVGmQVVlaTLe1547630;     IZBbVGmQVVlaTLe1547630 = IZBbVGmQVVlaTLe69093035;     IZBbVGmQVVlaTLe69093035 = IZBbVGmQVVlaTLe19001313;     IZBbVGmQVVlaTLe19001313 = IZBbVGmQVVlaTLe44605345;     IZBbVGmQVVlaTLe44605345 = IZBbVGmQVVlaTLe48427692;     IZBbVGmQVVlaTLe48427692 = IZBbVGmQVVlaTLe56239372;     IZBbVGmQVVlaTLe56239372 = IZBbVGmQVVlaTLe8100147;     IZBbVGmQVVlaTLe8100147 = IZBbVGmQVVlaTLe55681399;     IZBbVGmQVVlaTLe55681399 = IZBbVGmQVVlaTLe71150474;     IZBbVGmQVVlaTLe71150474 = IZBbVGmQVVlaTLe49027149;     IZBbVGmQVVlaTLe49027149 = IZBbVGmQVVlaTLe96690368;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void eJPytmBQyccGlrXL31099653() {     int veoPWqPzUHwneEw67342035 = -877049182;    int veoPWqPzUHwneEw77395211 = -580016857;    int veoPWqPzUHwneEw96319262 = -449412292;    int veoPWqPzUHwneEw41187234 = -229348403;    int veoPWqPzUHwneEw12120744 = -839371520;    int veoPWqPzUHwneEw65613572 = -590176219;    int veoPWqPzUHwneEw25580226 = -301805273;    int veoPWqPzUHwneEw20616100 = -655782210;    int veoPWqPzUHwneEw85544072 = -827341153;    int veoPWqPzUHwneEw29042640 = -999898428;    int veoPWqPzUHwneEw90495936 = -129379611;    int veoPWqPzUHwneEw39179219 = -536164013;    int veoPWqPzUHwneEw96686607 = -20507517;    int veoPWqPzUHwneEw82300000 = -681572479;    int veoPWqPzUHwneEw92842736 = -948860573;    int veoPWqPzUHwneEw89760825 = -586384405;    int veoPWqPzUHwneEw15546063 = -196776145;    int veoPWqPzUHwneEw61545168 = -341752223;    int veoPWqPzUHwneEw98111851 = -984061180;    int veoPWqPzUHwneEw56257661 = 2655917;    int veoPWqPzUHwneEw43643994 = -838846866;    int veoPWqPzUHwneEw90213452 = 64462698;    int veoPWqPzUHwneEw78505782 = -145926408;    int veoPWqPzUHwneEw50549149 = -134907794;    int veoPWqPzUHwneEw82810869 = -952359590;    int veoPWqPzUHwneEw19544496 = -884935238;    int veoPWqPzUHwneEw9986832 = -282845196;    int veoPWqPzUHwneEw10880620 = -951631521;    int veoPWqPzUHwneEw76301777 = -792088058;    int veoPWqPzUHwneEw91267295 = -668613095;    int veoPWqPzUHwneEw69023689 = -168805019;    int veoPWqPzUHwneEw43915638 = -999715373;    int veoPWqPzUHwneEw96974104 = 44540313;    int veoPWqPzUHwneEw3249607 = -895699816;    int veoPWqPzUHwneEw40391919 = -799238327;    int veoPWqPzUHwneEw93700609 = -452783847;    int veoPWqPzUHwneEw12535254 = -252309165;    int veoPWqPzUHwneEw54363268 = -212179780;    int veoPWqPzUHwneEw24862141 = -231418710;    int veoPWqPzUHwneEw17418753 = -388718055;    int veoPWqPzUHwneEw20415593 = -940642182;    int veoPWqPzUHwneEw56197568 = -992987647;    int veoPWqPzUHwneEw92232177 = -672865293;    int veoPWqPzUHwneEw21783262 = -826368155;    int veoPWqPzUHwneEw93823805 = -74723699;    int veoPWqPzUHwneEw53382461 = -420913002;    int veoPWqPzUHwneEw31617287 = -145075951;    int veoPWqPzUHwneEw91961509 = -262571587;    int veoPWqPzUHwneEw69201363 = -695994605;    int veoPWqPzUHwneEw91828161 = -733903456;    int veoPWqPzUHwneEw98439501 = -640348011;    int veoPWqPzUHwneEw155276 = -427532079;    int veoPWqPzUHwneEw11985625 = 61570579;    int veoPWqPzUHwneEw51538327 = -223248504;    int veoPWqPzUHwneEw87455659 = -641474550;    int veoPWqPzUHwneEw77128583 = -841511880;    int veoPWqPzUHwneEw98889428 = -334090449;    int veoPWqPzUHwneEw45770114 = -214504498;    int veoPWqPzUHwneEw58376365 = -276988813;    int veoPWqPzUHwneEw92576247 = -954436283;    int veoPWqPzUHwneEw55626740 = -207331024;    int veoPWqPzUHwneEw14699607 = -350173753;    int veoPWqPzUHwneEw44314323 = -863694153;    int veoPWqPzUHwneEw94276777 = -58728059;    int veoPWqPzUHwneEw60018951 = -731093410;    int veoPWqPzUHwneEw46580298 = -129664238;    int veoPWqPzUHwneEw42205114 = -480704327;    int veoPWqPzUHwneEw93437000 = -124807702;    int veoPWqPzUHwneEw41908082 = -882334152;    int veoPWqPzUHwneEw99142126 = -396076726;    int veoPWqPzUHwneEw77225572 = -234075241;    int veoPWqPzUHwneEw61182794 = -984596366;    int veoPWqPzUHwneEw36683028 = -10333514;    int veoPWqPzUHwneEw80693098 = -495343126;    int veoPWqPzUHwneEw35842068 = -56701901;    int veoPWqPzUHwneEw87446425 = -845859219;    int veoPWqPzUHwneEw97981275 = -262672010;    int veoPWqPzUHwneEw56722521 = -319558254;    int veoPWqPzUHwneEw56725343 = 39815904;    int veoPWqPzUHwneEw29428409 = -431446588;    int veoPWqPzUHwneEw87927208 = -639859287;    int veoPWqPzUHwneEw18025323 = 79726390;    int veoPWqPzUHwneEw41679256 = -155636917;    int veoPWqPzUHwneEw84473615 = 41815398;    int veoPWqPzUHwneEw92827793 = 71734916;    int veoPWqPzUHwneEw68868413 = -741272940;    int veoPWqPzUHwneEw31930013 = -961285952;    int veoPWqPzUHwneEw45435778 = -732211183;    int veoPWqPzUHwneEw15793947 = -154225266;    int veoPWqPzUHwneEw63263335 = -957726448;    int veoPWqPzUHwneEw94811180 = -18693399;    int veoPWqPzUHwneEw66765139 = 62195333;    int veoPWqPzUHwneEw95986903 = -935190967;    int veoPWqPzUHwneEw32285893 = -276982427;    int veoPWqPzUHwneEw61792013 = -81387031;    int veoPWqPzUHwneEw5715986 = -490468430;    int veoPWqPzUHwneEw11883246 = -29293494;    int veoPWqPzUHwneEw97955399 = -514137235;    int veoPWqPzUHwneEw61764310 = 4725254;    int veoPWqPzUHwneEw47243508 = -877049182;     veoPWqPzUHwneEw67342035 = veoPWqPzUHwneEw77395211;     veoPWqPzUHwneEw77395211 = veoPWqPzUHwneEw96319262;     veoPWqPzUHwneEw96319262 = veoPWqPzUHwneEw41187234;     veoPWqPzUHwneEw41187234 = veoPWqPzUHwneEw12120744;     veoPWqPzUHwneEw12120744 = veoPWqPzUHwneEw65613572;     veoPWqPzUHwneEw65613572 = veoPWqPzUHwneEw25580226;     veoPWqPzUHwneEw25580226 = veoPWqPzUHwneEw20616100;     veoPWqPzUHwneEw20616100 = veoPWqPzUHwneEw85544072;     veoPWqPzUHwneEw85544072 = veoPWqPzUHwneEw29042640;     veoPWqPzUHwneEw29042640 = veoPWqPzUHwneEw90495936;     veoPWqPzUHwneEw90495936 = veoPWqPzUHwneEw39179219;     veoPWqPzUHwneEw39179219 = veoPWqPzUHwneEw96686607;     veoPWqPzUHwneEw96686607 = veoPWqPzUHwneEw82300000;     veoPWqPzUHwneEw82300000 = veoPWqPzUHwneEw92842736;     veoPWqPzUHwneEw92842736 = veoPWqPzUHwneEw89760825;     veoPWqPzUHwneEw89760825 = veoPWqPzUHwneEw15546063;     veoPWqPzUHwneEw15546063 = veoPWqPzUHwneEw61545168;     veoPWqPzUHwneEw61545168 = veoPWqPzUHwneEw98111851;     veoPWqPzUHwneEw98111851 = veoPWqPzUHwneEw56257661;     veoPWqPzUHwneEw56257661 = veoPWqPzUHwneEw43643994;     veoPWqPzUHwneEw43643994 = veoPWqPzUHwneEw90213452;     veoPWqPzUHwneEw90213452 = veoPWqPzUHwneEw78505782;     veoPWqPzUHwneEw78505782 = veoPWqPzUHwneEw50549149;     veoPWqPzUHwneEw50549149 = veoPWqPzUHwneEw82810869;     veoPWqPzUHwneEw82810869 = veoPWqPzUHwneEw19544496;     veoPWqPzUHwneEw19544496 = veoPWqPzUHwneEw9986832;     veoPWqPzUHwneEw9986832 = veoPWqPzUHwneEw10880620;     veoPWqPzUHwneEw10880620 = veoPWqPzUHwneEw76301777;     veoPWqPzUHwneEw76301777 = veoPWqPzUHwneEw91267295;     veoPWqPzUHwneEw91267295 = veoPWqPzUHwneEw69023689;     veoPWqPzUHwneEw69023689 = veoPWqPzUHwneEw43915638;     veoPWqPzUHwneEw43915638 = veoPWqPzUHwneEw96974104;     veoPWqPzUHwneEw96974104 = veoPWqPzUHwneEw3249607;     veoPWqPzUHwneEw3249607 = veoPWqPzUHwneEw40391919;     veoPWqPzUHwneEw40391919 = veoPWqPzUHwneEw93700609;     veoPWqPzUHwneEw93700609 = veoPWqPzUHwneEw12535254;     veoPWqPzUHwneEw12535254 = veoPWqPzUHwneEw54363268;     veoPWqPzUHwneEw54363268 = veoPWqPzUHwneEw24862141;     veoPWqPzUHwneEw24862141 = veoPWqPzUHwneEw17418753;     veoPWqPzUHwneEw17418753 = veoPWqPzUHwneEw20415593;     veoPWqPzUHwneEw20415593 = veoPWqPzUHwneEw56197568;     veoPWqPzUHwneEw56197568 = veoPWqPzUHwneEw92232177;     veoPWqPzUHwneEw92232177 = veoPWqPzUHwneEw21783262;     veoPWqPzUHwneEw21783262 = veoPWqPzUHwneEw93823805;     veoPWqPzUHwneEw93823805 = veoPWqPzUHwneEw53382461;     veoPWqPzUHwneEw53382461 = veoPWqPzUHwneEw31617287;     veoPWqPzUHwneEw31617287 = veoPWqPzUHwneEw91961509;     veoPWqPzUHwneEw91961509 = veoPWqPzUHwneEw69201363;     veoPWqPzUHwneEw69201363 = veoPWqPzUHwneEw91828161;     veoPWqPzUHwneEw91828161 = veoPWqPzUHwneEw98439501;     veoPWqPzUHwneEw98439501 = veoPWqPzUHwneEw155276;     veoPWqPzUHwneEw155276 = veoPWqPzUHwneEw11985625;     veoPWqPzUHwneEw11985625 = veoPWqPzUHwneEw51538327;     veoPWqPzUHwneEw51538327 = veoPWqPzUHwneEw87455659;     veoPWqPzUHwneEw87455659 = veoPWqPzUHwneEw77128583;     veoPWqPzUHwneEw77128583 = veoPWqPzUHwneEw98889428;     veoPWqPzUHwneEw98889428 = veoPWqPzUHwneEw45770114;     veoPWqPzUHwneEw45770114 = veoPWqPzUHwneEw58376365;     veoPWqPzUHwneEw58376365 = veoPWqPzUHwneEw92576247;     veoPWqPzUHwneEw92576247 = veoPWqPzUHwneEw55626740;     veoPWqPzUHwneEw55626740 = veoPWqPzUHwneEw14699607;     veoPWqPzUHwneEw14699607 = veoPWqPzUHwneEw44314323;     veoPWqPzUHwneEw44314323 = veoPWqPzUHwneEw94276777;     veoPWqPzUHwneEw94276777 = veoPWqPzUHwneEw60018951;     veoPWqPzUHwneEw60018951 = veoPWqPzUHwneEw46580298;     veoPWqPzUHwneEw46580298 = veoPWqPzUHwneEw42205114;     veoPWqPzUHwneEw42205114 = veoPWqPzUHwneEw93437000;     veoPWqPzUHwneEw93437000 = veoPWqPzUHwneEw41908082;     veoPWqPzUHwneEw41908082 = veoPWqPzUHwneEw99142126;     veoPWqPzUHwneEw99142126 = veoPWqPzUHwneEw77225572;     veoPWqPzUHwneEw77225572 = veoPWqPzUHwneEw61182794;     veoPWqPzUHwneEw61182794 = veoPWqPzUHwneEw36683028;     veoPWqPzUHwneEw36683028 = veoPWqPzUHwneEw80693098;     veoPWqPzUHwneEw80693098 = veoPWqPzUHwneEw35842068;     veoPWqPzUHwneEw35842068 = veoPWqPzUHwneEw87446425;     veoPWqPzUHwneEw87446425 = veoPWqPzUHwneEw97981275;     veoPWqPzUHwneEw97981275 = veoPWqPzUHwneEw56722521;     veoPWqPzUHwneEw56722521 = veoPWqPzUHwneEw56725343;     veoPWqPzUHwneEw56725343 = veoPWqPzUHwneEw29428409;     veoPWqPzUHwneEw29428409 = veoPWqPzUHwneEw87927208;     veoPWqPzUHwneEw87927208 = veoPWqPzUHwneEw18025323;     veoPWqPzUHwneEw18025323 = veoPWqPzUHwneEw41679256;     veoPWqPzUHwneEw41679256 = veoPWqPzUHwneEw84473615;     veoPWqPzUHwneEw84473615 = veoPWqPzUHwneEw92827793;     veoPWqPzUHwneEw92827793 = veoPWqPzUHwneEw68868413;     veoPWqPzUHwneEw68868413 = veoPWqPzUHwneEw31930013;     veoPWqPzUHwneEw31930013 = veoPWqPzUHwneEw45435778;     veoPWqPzUHwneEw45435778 = veoPWqPzUHwneEw15793947;     veoPWqPzUHwneEw15793947 = veoPWqPzUHwneEw63263335;     veoPWqPzUHwneEw63263335 = veoPWqPzUHwneEw94811180;     veoPWqPzUHwneEw94811180 = veoPWqPzUHwneEw66765139;     veoPWqPzUHwneEw66765139 = veoPWqPzUHwneEw95986903;     veoPWqPzUHwneEw95986903 = veoPWqPzUHwneEw32285893;     veoPWqPzUHwneEw32285893 = veoPWqPzUHwneEw61792013;     veoPWqPzUHwneEw61792013 = veoPWqPzUHwneEw5715986;     veoPWqPzUHwneEw5715986 = veoPWqPzUHwneEw11883246;     veoPWqPzUHwneEw11883246 = veoPWqPzUHwneEw97955399;     veoPWqPzUHwneEw97955399 = veoPWqPzUHwneEw61764310;     veoPWqPzUHwneEw61764310 = veoPWqPzUHwneEw47243508;     veoPWqPzUHwneEw47243508 = veoPWqPzUHwneEw67342035;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ggbnXnDCLZNEetdx68074825() {     int jOgdhMOshAgEGaI35068284 = 53550299;    int jOgdhMOshAgEGaI53317408 = 8409784;    int jOgdhMOshAgEGaI65669107 = -260810136;    int jOgdhMOshAgEGaI69838842 = -50194356;    int jOgdhMOshAgEGaI44683429 = -26348595;    int jOgdhMOshAgEGaI1127725 = -21066573;    int jOgdhMOshAgEGaI65182597 = -626436374;    int jOgdhMOshAgEGaI36288906 = -329987924;    int jOgdhMOshAgEGaI77672957 = -378329891;    int jOgdhMOshAgEGaI34813753 = -100645362;    int jOgdhMOshAgEGaI69747106 = -560119511;    int jOgdhMOshAgEGaI46609131 = -335963571;    int jOgdhMOshAgEGaI31730387 = -857343007;    int jOgdhMOshAgEGaI85792542 = -842330708;    int jOgdhMOshAgEGaI44526049 = -631929488;    int jOgdhMOshAgEGaI18813750 = -570688034;    int jOgdhMOshAgEGaI21390411 = -920070706;    int jOgdhMOshAgEGaI22925531 = -27002064;    int jOgdhMOshAgEGaI58601151 = -351334025;    int jOgdhMOshAgEGaI89213151 = -516636842;    int jOgdhMOshAgEGaI90022137 = 27282608;    int jOgdhMOshAgEGaI81072347 = -783851391;    int jOgdhMOshAgEGaI82207670 = -194221422;    int jOgdhMOshAgEGaI19270676 = -880780358;    int jOgdhMOshAgEGaI51041190 = -627699103;    int jOgdhMOshAgEGaI83180619 = -133326609;    int jOgdhMOshAgEGaI25307910 = -886387123;    int jOgdhMOshAgEGaI98946594 = -241001096;    int jOgdhMOshAgEGaI55975108 = -302170632;    int jOgdhMOshAgEGaI82078935 = 26417633;    int jOgdhMOshAgEGaI95021644 = -88341880;    int jOgdhMOshAgEGaI55189076 = -532111964;    int jOgdhMOshAgEGaI95809321 = -130229786;    int jOgdhMOshAgEGaI68265238 = -316387589;    int jOgdhMOshAgEGaI64475164 = -904445418;    int jOgdhMOshAgEGaI90895125 = -599949392;    int jOgdhMOshAgEGaI43211845 = -234483874;    int jOgdhMOshAgEGaI64929809 = -201138907;    int jOgdhMOshAgEGaI57714220 = -963197568;    int jOgdhMOshAgEGaI85490965 = -476065171;    int jOgdhMOshAgEGaI81069230 = -319213589;    int jOgdhMOshAgEGaI93728961 = -337156354;    int jOgdhMOshAgEGaI67065546 = -179020815;    int jOgdhMOshAgEGaI71464879 = -907016582;    int jOgdhMOshAgEGaI85841061 = -224608982;    int jOgdhMOshAgEGaI46577110 = -346397172;    int jOgdhMOshAgEGaI55138765 = -77709442;    int jOgdhMOshAgEGaI88516456 = -650215562;    int jOgdhMOshAgEGaI39976891 = -309745000;    int jOgdhMOshAgEGaI64115668 = -222929302;    int jOgdhMOshAgEGaI45454549 = -916763794;    int jOgdhMOshAgEGaI99171640 = -725431130;    int jOgdhMOshAgEGaI34020374 = -862479199;    int jOgdhMOshAgEGaI64659386 = -34962679;    int jOgdhMOshAgEGaI78052687 = -143169267;    int jOgdhMOshAgEGaI53995937 = -162598310;    int jOgdhMOshAgEGaI71109738 = -797368795;    int jOgdhMOshAgEGaI46398432 = -380029779;    int jOgdhMOshAgEGaI18797653 = -422495254;    int jOgdhMOshAgEGaI61502810 = -893021986;    int jOgdhMOshAgEGaI75819814 = -134679450;    int jOgdhMOshAgEGaI66236003 = -285435278;    int jOgdhMOshAgEGaI80313797 = 72182707;    int jOgdhMOshAgEGaI95594021 = -304747524;    int jOgdhMOshAgEGaI39792109 = 87696517;    int jOgdhMOshAgEGaI14558031 = 71992453;    int jOgdhMOshAgEGaI50799810 = -105733785;    int jOgdhMOshAgEGaI63465149 = -440955419;    int jOgdhMOshAgEGaI21317378 = -937885291;    int jOgdhMOshAgEGaI53630923 = 68019903;    int jOgdhMOshAgEGaI75601904 = -236204160;    int jOgdhMOshAgEGaI56460601 = -618931800;    int jOgdhMOshAgEGaI65211311 = -63804497;    int jOgdhMOshAgEGaI73110185 = -875268855;    int jOgdhMOshAgEGaI8143922 = -97423254;    int jOgdhMOshAgEGaI96293175 = -635561038;    int jOgdhMOshAgEGaI14006802 = -504830577;    int jOgdhMOshAgEGaI10742791 = -287204840;    int jOgdhMOshAgEGaI33429614 = -556171377;    int jOgdhMOshAgEGaI4464081 = -181301931;    int jOgdhMOshAgEGaI28041854 = 44382832;    int jOgdhMOshAgEGaI36791454 = -136171561;    int jOgdhMOshAgEGaI58969703 = -931256096;    int jOgdhMOshAgEGaI91859439 = 20758670;    int jOgdhMOshAgEGaI36624387 = -56818574;    int jOgdhMOshAgEGaI95850003 = -362910750;    int jOgdhMOshAgEGaI21168702 = -669632765;    int jOgdhMOshAgEGaI31149936 = 4732893;    int jOgdhMOshAgEGaI90212550 = -73218322;    int jOgdhMOshAgEGaI10479228 = -641847108;    int jOgdhMOshAgEGaI19785387 = -802580597;    int jOgdhMOshAgEGaI96813413 = -854454096;    int jOgdhMOshAgEGaI46132157 = -778643654;    int jOgdhMOshAgEGaI96211410 = 29824418;    int jOgdhMOshAgEGaI9671152 = -241385721;    int jOgdhMOshAgEGaI14833228 = 66221689;    int jOgdhMOshAgEGaI13415165 = -309339062;    int jOgdhMOshAgEGaI71471525 = -874273291;    int jOgdhMOshAgEGaI31672771 = -894713099;    int jOgdhMOshAgEGaI71283031 = 53550299;     jOgdhMOshAgEGaI35068284 = jOgdhMOshAgEGaI53317408;     jOgdhMOshAgEGaI53317408 = jOgdhMOshAgEGaI65669107;     jOgdhMOshAgEGaI65669107 = jOgdhMOshAgEGaI69838842;     jOgdhMOshAgEGaI69838842 = jOgdhMOshAgEGaI44683429;     jOgdhMOshAgEGaI44683429 = jOgdhMOshAgEGaI1127725;     jOgdhMOshAgEGaI1127725 = jOgdhMOshAgEGaI65182597;     jOgdhMOshAgEGaI65182597 = jOgdhMOshAgEGaI36288906;     jOgdhMOshAgEGaI36288906 = jOgdhMOshAgEGaI77672957;     jOgdhMOshAgEGaI77672957 = jOgdhMOshAgEGaI34813753;     jOgdhMOshAgEGaI34813753 = jOgdhMOshAgEGaI69747106;     jOgdhMOshAgEGaI69747106 = jOgdhMOshAgEGaI46609131;     jOgdhMOshAgEGaI46609131 = jOgdhMOshAgEGaI31730387;     jOgdhMOshAgEGaI31730387 = jOgdhMOshAgEGaI85792542;     jOgdhMOshAgEGaI85792542 = jOgdhMOshAgEGaI44526049;     jOgdhMOshAgEGaI44526049 = jOgdhMOshAgEGaI18813750;     jOgdhMOshAgEGaI18813750 = jOgdhMOshAgEGaI21390411;     jOgdhMOshAgEGaI21390411 = jOgdhMOshAgEGaI22925531;     jOgdhMOshAgEGaI22925531 = jOgdhMOshAgEGaI58601151;     jOgdhMOshAgEGaI58601151 = jOgdhMOshAgEGaI89213151;     jOgdhMOshAgEGaI89213151 = jOgdhMOshAgEGaI90022137;     jOgdhMOshAgEGaI90022137 = jOgdhMOshAgEGaI81072347;     jOgdhMOshAgEGaI81072347 = jOgdhMOshAgEGaI82207670;     jOgdhMOshAgEGaI82207670 = jOgdhMOshAgEGaI19270676;     jOgdhMOshAgEGaI19270676 = jOgdhMOshAgEGaI51041190;     jOgdhMOshAgEGaI51041190 = jOgdhMOshAgEGaI83180619;     jOgdhMOshAgEGaI83180619 = jOgdhMOshAgEGaI25307910;     jOgdhMOshAgEGaI25307910 = jOgdhMOshAgEGaI98946594;     jOgdhMOshAgEGaI98946594 = jOgdhMOshAgEGaI55975108;     jOgdhMOshAgEGaI55975108 = jOgdhMOshAgEGaI82078935;     jOgdhMOshAgEGaI82078935 = jOgdhMOshAgEGaI95021644;     jOgdhMOshAgEGaI95021644 = jOgdhMOshAgEGaI55189076;     jOgdhMOshAgEGaI55189076 = jOgdhMOshAgEGaI95809321;     jOgdhMOshAgEGaI95809321 = jOgdhMOshAgEGaI68265238;     jOgdhMOshAgEGaI68265238 = jOgdhMOshAgEGaI64475164;     jOgdhMOshAgEGaI64475164 = jOgdhMOshAgEGaI90895125;     jOgdhMOshAgEGaI90895125 = jOgdhMOshAgEGaI43211845;     jOgdhMOshAgEGaI43211845 = jOgdhMOshAgEGaI64929809;     jOgdhMOshAgEGaI64929809 = jOgdhMOshAgEGaI57714220;     jOgdhMOshAgEGaI57714220 = jOgdhMOshAgEGaI85490965;     jOgdhMOshAgEGaI85490965 = jOgdhMOshAgEGaI81069230;     jOgdhMOshAgEGaI81069230 = jOgdhMOshAgEGaI93728961;     jOgdhMOshAgEGaI93728961 = jOgdhMOshAgEGaI67065546;     jOgdhMOshAgEGaI67065546 = jOgdhMOshAgEGaI71464879;     jOgdhMOshAgEGaI71464879 = jOgdhMOshAgEGaI85841061;     jOgdhMOshAgEGaI85841061 = jOgdhMOshAgEGaI46577110;     jOgdhMOshAgEGaI46577110 = jOgdhMOshAgEGaI55138765;     jOgdhMOshAgEGaI55138765 = jOgdhMOshAgEGaI88516456;     jOgdhMOshAgEGaI88516456 = jOgdhMOshAgEGaI39976891;     jOgdhMOshAgEGaI39976891 = jOgdhMOshAgEGaI64115668;     jOgdhMOshAgEGaI64115668 = jOgdhMOshAgEGaI45454549;     jOgdhMOshAgEGaI45454549 = jOgdhMOshAgEGaI99171640;     jOgdhMOshAgEGaI99171640 = jOgdhMOshAgEGaI34020374;     jOgdhMOshAgEGaI34020374 = jOgdhMOshAgEGaI64659386;     jOgdhMOshAgEGaI64659386 = jOgdhMOshAgEGaI78052687;     jOgdhMOshAgEGaI78052687 = jOgdhMOshAgEGaI53995937;     jOgdhMOshAgEGaI53995937 = jOgdhMOshAgEGaI71109738;     jOgdhMOshAgEGaI71109738 = jOgdhMOshAgEGaI46398432;     jOgdhMOshAgEGaI46398432 = jOgdhMOshAgEGaI18797653;     jOgdhMOshAgEGaI18797653 = jOgdhMOshAgEGaI61502810;     jOgdhMOshAgEGaI61502810 = jOgdhMOshAgEGaI75819814;     jOgdhMOshAgEGaI75819814 = jOgdhMOshAgEGaI66236003;     jOgdhMOshAgEGaI66236003 = jOgdhMOshAgEGaI80313797;     jOgdhMOshAgEGaI80313797 = jOgdhMOshAgEGaI95594021;     jOgdhMOshAgEGaI95594021 = jOgdhMOshAgEGaI39792109;     jOgdhMOshAgEGaI39792109 = jOgdhMOshAgEGaI14558031;     jOgdhMOshAgEGaI14558031 = jOgdhMOshAgEGaI50799810;     jOgdhMOshAgEGaI50799810 = jOgdhMOshAgEGaI63465149;     jOgdhMOshAgEGaI63465149 = jOgdhMOshAgEGaI21317378;     jOgdhMOshAgEGaI21317378 = jOgdhMOshAgEGaI53630923;     jOgdhMOshAgEGaI53630923 = jOgdhMOshAgEGaI75601904;     jOgdhMOshAgEGaI75601904 = jOgdhMOshAgEGaI56460601;     jOgdhMOshAgEGaI56460601 = jOgdhMOshAgEGaI65211311;     jOgdhMOshAgEGaI65211311 = jOgdhMOshAgEGaI73110185;     jOgdhMOshAgEGaI73110185 = jOgdhMOshAgEGaI8143922;     jOgdhMOshAgEGaI8143922 = jOgdhMOshAgEGaI96293175;     jOgdhMOshAgEGaI96293175 = jOgdhMOshAgEGaI14006802;     jOgdhMOshAgEGaI14006802 = jOgdhMOshAgEGaI10742791;     jOgdhMOshAgEGaI10742791 = jOgdhMOshAgEGaI33429614;     jOgdhMOshAgEGaI33429614 = jOgdhMOshAgEGaI4464081;     jOgdhMOshAgEGaI4464081 = jOgdhMOshAgEGaI28041854;     jOgdhMOshAgEGaI28041854 = jOgdhMOshAgEGaI36791454;     jOgdhMOshAgEGaI36791454 = jOgdhMOshAgEGaI58969703;     jOgdhMOshAgEGaI58969703 = jOgdhMOshAgEGaI91859439;     jOgdhMOshAgEGaI91859439 = jOgdhMOshAgEGaI36624387;     jOgdhMOshAgEGaI36624387 = jOgdhMOshAgEGaI95850003;     jOgdhMOshAgEGaI95850003 = jOgdhMOshAgEGaI21168702;     jOgdhMOshAgEGaI21168702 = jOgdhMOshAgEGaI31149936;     jOgdhMOshAgEGaI31149936 = jOgdhMOshAgEGaI90212550;     jOgdhMOshAgEGaI90212550 = jOgdhMOshAgEGaI10479228;     jOgdhMOshAgEGaI10479228 = jOgdhMOshAgEGaI19785387;     jOgdhMOshAgEGaI19785387 = jOgdhMOshAgEGaI96813413;     jOgdhMOshAgEGaI96813413 = jOgdhMOshAgEGaI46132157;     jOgdhMOshAgEGaI46132157 = jOgdhMOshAgEGaI96211410;     jOgdhMOshAgEGaI96211410 = jOgdhMOshAgEGaI9671152;     jOgdhMOshAgEGaI9671152 = jOgdhMOshAgEGaI14833228;     jOgdhMOshAgEGaI14833228 = jOgdhMOshAgEGaI13415165;     jOgdhMOshAgEGaI13415165 = jOgdhMOshAgEGaI71471525;     jOgdhMOshAgEGaI71471525 = jOgdhMOshAgEGaI31672771;     jOgdhMOshAgEGaI31672771 = jOgdhMOshAgEGaI71283031;     jOgdhMOshAgEGaI71283031 = jOgdhMOshAgEGaI35068284;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void GRwOjlYGOCxDUDRu22491038() {     float ekRBsLJFMvDEJti5719951 = -564598831;    float ekRBsLJFMvDEJti21990926 = -963233516;    float ekRBsLJFMvDEJti45638026 = -703104596;    float ekRBsLJFMvDEJti9770129 = -240332746;    float ekRBsLJFMvDEJti66133750 = -909754942;    float ekRBsLJFMvDEJti91127486 = -822647724;    float ekRBsLJFMvDEJti83561492 = -625212084;    float ekRBsLJFMvDEJti93131286 = -676805013;    float ekRBsLJFMvDEJti23538012 = -471677219;    float ekRBsLJFMvDEJti57780214 = -401740713;    float ekRBsLJFMvDEJti11996267 = -595021440;    float ekRBsLJFMvDEJti92155702 = -651727364;    float ekRBsLJFMvDEJti11557774 = -766880065;    float ekRBsLJFMvDEJti69639575 = -643358918;    float ekRBsLJFMvDEJti27246068 = -781679491;    float ekRBsLJFMvDEJti51342390 = -140671620;    float ekRBsLJFMvDEJti15243237 = -829255836;    float ekRBsLJFMvDEJti43219025 = -16168260;    float ekRBsLJFMvDEJti40308932 = 41478033;    float ekRBsLJFMvDEJti47999861 = 40996853;    float ekRBsLJFMvDEJti45157892 = -981396532;    float ekRBsLJFMvDEJti42468112 = -15136697;    float ekRBsLJFMvDEJti84151478 = -762064566;    float ekRBsLJFMvDEJti53120212 = -473181264;    float ekRBsLJFMvDEJti15165579 = -303603791;    float ekRBsLJFMvDEJti13778200 = -231611054;    float ekRBsLJFMvDEJti33568696 = -181564898;    float ekRBsLJFMvDEJti6728820 = -746124863;    float ekRBsLJFMvDEJti88117595 = -596528849;    float ekRBsLJFMvDEJti21251032 = -531977438;    float ekRBsLJFMvDEJti2093641 = -936775565;    float ekRBsLJFMvDEJti99700536 = -860409127;    float ekRBsLJFMvDEJti78754682 = -77191395;    float ekRBsLJFMvDEJti33630213 = -979917273;    float ekRBsLJFMvDEJti42499401 = -751392197;    float ekRBsLJFMvDEJti50888384 = -679175940;    float ekRBsLJFMvDEJti87003331 = -423077081;    float ekRBsLJFMvDEJti17722298 = 14747986;    float ekRBsLJFMvDEJti36247509 = -918926205;    float ekRBsLJFMvDEJti80594264 = 6326467;    float ekRBsLJFMvDEJti41142514 = -703232467;    float ekRBsLJFMvDEJti22211953 = -106545424;    float ekRBsLJFMvDEJti16032507 = -165774867;    float ekRBsLJFMvDEJti77973180 = -978991553;    float ekRBsLJFMvDEJti81795120 = -533255376;    float ekRBsLJFMvDEJti52982944 = -104734327;    float ekRBsLJFMvDEJti81269968 = -193779241;    float ekRBsLJFMvDEJti54130666 = -2345858;    float ekRBsLJFMvDEJti45418343 = -295508969;    float ekRBsLJFMvDEJti72543154 = -505938181;    float ekRBsLJFMvDEJti13643873 = -463335915;    float ekRBsLJFMvDEJti41043145 = -694650852;    float ekRBsLJFMvDEJti89096030 = -790277241;    float ekRBsLJFMvDEJti54886690 = -695844176;    float ekRBsLJFMvDEJti77333146 = -254110181;    float ekRBsLJFMvDEJti63251839 = -449462135;    float ekRBsLJFMvDEJti37839447 = -101168950;    float ekRBsLJFMvDEJti92517813 = -129923332;    float ekRBsLJFMvDEJti94604550 = -936728955;    float ekRBsLJFMvDEJti52355550 = -578143889;    float ekRBsLJFMvDEJti57558791 = -541082827;    float ekRBsLJFMvDEJti76832673 = -879087221;    float ekRBsLJFMvDEJti5013691 = 19723835;    float ekRBsLJFMvDEJti2286980 = -939699782;    float ekRBsLJFMvDEJti55686573 = -464965148;    float ekRBsLJFMvDEJti12295731 = -734612314;    float ekRBsLJFMvDEJti13401020 = -474535970;    float ekRBsLJFMvDEJti77927561 = -786962793;    float ekRBsLJFMvDEJti27140175 = -891966722;    float ekRBsLJFMvDEJti76357683 = -2503552;    float ekRBsLJFMvDEJti64339058 = -717594540;    float ekRBsLJFMvDEJti97520938 = -744003822;    float ekRBsLJFMvDEJti6971517 = -97242056;    float ekRBsLJFMvDEJti59714668 = -964848435;    float ekRBsLJFMvDEJti6857347 = -255770680;    float ekRBsLJFMvDEJti22945939 = -774851109;    float ekRBsLJFMvDEJti26435606 = -849361830;    float ekRBsLJFMvDEJti6178299 = -783073013;    float ekRBsLJFMvDEJti71325092 = -939925888;    float ekRBsLJFMvDEJti62182634 = -98869464;    float ekRBsLJFMvDEJti32508232 = 62168187;    float ekRBsLJFMvDEJti79438029 = -79219041;    float ekRBsLJFMvDEJti61310476 = -350615895;    float ekRBsLJFMvDEJti15574442 = 9409332;    float ekRBsLJFMvDEJti7607159 = 31358476;    float ekRBsLJFMvDEJti61050495 = -142124713;    float ekRBsLJFMvDEJti10604506 = 29868114;    float ekRBsLJFMvDEJti23867993 = -381347219;    float ekRBsLJFMvDEJti56297067 = -625807092;    float ekRBsLJFMvDEJti79247562 = -201930063;    float ekRBsLJFMvDEJti13048938 = -478006990;    float ekRBsLJFMvDEJti94485517 = -193153750;    float ekRBsLJFMvDEJti23117748 = -48523060;    float ekRBsLJFMvDEJti83891958 = -240782316;    float ekRBsLJFMvDEJti23035473 = -452590707;    float ekRBsLJFMvDEJti64309841 = -824145247;    float ekRBsLJFMvDEJti17198263 = -26269259;    float ekRBsLJFMvDEJti13745527 = -226075086;    float ekRBsLJFMvDEJti22286607 = -414026406;    float ekRBsLJFMvDEJti69499390 = -564598831;     ekRBsLJFMvDEJti5719951 = ekRBsLJFMvDEJti21990926;     ekRBsLJFMvDEJti21990926 = ekRBsLJFMvDEJti45638026;     ekRBsLJFMvDEJti45638026 = ekRBsLJFMvDEJti9770129;     ekRBsLJFMvDEJti9770129 = ekRBsLJFMvDEJti66133750;     ekRBsLJFMvDEJti66133750 = ekRBsLJFMvDEJti91127486;     ekRBsLJFMvDEJti91127486 = ekRBsLJFMvDEJti83561492;     ekRBsLJFMvDEJti83561492 = ekRBsLJFMvDEJti93131286;     ekRBsLJFMvDEJti93131286 = ekRBsLJFMvDEJti23538012;     ekRBsLJFMvDEJti23538012 = ekRBsLJFMvDEJti57780214;     ekRBsLJFMvDEJti57780214 = ekRBsLJFMvDEJti11996267;     ekRBsLJFMvDEJti11996267 = ekRBsLJFMvDEJti92155702;     ekRBsLJFMvDEJti92155702 = ekRBsLJFMvDEJti11557774;     ekRBsLJFMvDEJti11557774 = ekRBsLJFMvDEJti69639575;     ekRBsLJFMvDEJti69639575 = ekRBsLJFMvDEJti27246068;     ekRBsLJFMvDEJti27246068 = ekRBsLJFMvDEJti51342390;     ekRBsLJFMvDEJti51342390 = ekRBsLJFMvDEJti15243237;     ekRBsLJFMvDEJti15243237 = ekRBsLJFMvDEJti43219025;     ekRBsLJFMvDEJti43219025 = ekRBsLJFMvDEJti40308932;     ekRBsLJFMvDEJti40308932 = ekRBsLJFMvDEJti47999861;     ekRBsLJFMvDEJti47999861 = ekRBsLJFMvDEJti45157892;     ekRBsLJFMvDEJti45157892 = ekRBsLJFMvDEJti42468112;     ekRBsLJFMvDEJti42468112 = ekRBsLJFMvDEJti84151478;     ekRBsLJFMvDEJti84151478 = ekRBsLJFMvDEJti53120212;     ekRBsLJFMvDEJti53120212 = ekRBsLJFMvDEJti15165579;     ekRBsLJFMvDEJti15165579 = ekRBsLJFMvDEJti13778200;     ekRBsLJFMvDEJti13778200 = ekRBsLJFMvDEJti33568696;     ekRBsLJFMvDEJti33568696 = ekRBsLJFMvDEJti6728820;     ekRBsLJFMvDEJti6728820 = ekRBsLJFMvDEJti88117595;     ekRBsLJFMvDEJti88117595 = ekRBsLJFMvDEJti21251032;     ekRBsLJFMvDEJti21251032 = ekRBsLJFMvDEJti2093641;     ekRBsLJFMvDEJti2093641 = ekRBsLJFMvDEJti99700536;     ekRBsLJFMvDEJti99700536 = ekRBsLJFMvDEJti78754682;     ekRBsLJFMvDEJti78754682 = ekRBsLJFMvDEJti33630213;     ekRBsLJFMvDEJti33630213 = ekRBsLJFMvDEJti42499401;     ekRBsLJFMvDEJti42499401 = ekRBsLJFMvDEJti50888384;     ekRBsLJFMvDEJti50888384 = ekRBsLJFMvDEJti87003331;     ekRBsLJFMvDEJti87003331 = ekRBsLJFMvDEJti17722298;     ekRBsLJFMvDEJti17722298 = ekRBsLJFMvDEJti36247509;     ekRBsLJFMvDEJti36247509 = ekRBsLJFMvDEJti80594264;     ekRBsLJFMvDEJti80594264 = ekRBsLJFMvDEJti41142514;     ekRBsLJFMvDEJti41142514 = ekRBsLJFMvDEJti22211953;     ekRBsLJFMvDEJti22211953 = ekRBsLJFMvDEJti16032507;     ekRBsLJFMvDEJti16032507 = ekRBsLJFMvDEJti77973180;     ekRBsLJFMvDEJti77973180 = ekRBsLJFMvDEJti81795120;     ekRBsLJFMvDEJti81795120 = ekRBsLJFMvDEJti52982944;     ekRBsLJFMvDEJti52982944 = ekRBsLJFMvDEJti81269968;     ekRBsLJFMvDEJti81269968 = ekRBsLJFMvDEJti54130666;     ekRBsLJFMvDEJti54130666 = ekRBsLJFMvDEJti45418343;     ekRBsLJFMvDEJti45418343 = ekRBsLJFMvDEJti72543154;     ekRBsLJFMvDEJti72543154 = ekRBsLJFMvDEJti13643873;     ekRBsLJFMvDEJti13643873 = ekRBsLJFMvDEJti41043145;     ekRBsLJFMvDEJti41043145 = ekRBsLJFMvDEJti89096030;     ekRBsLJFMvDEJti89096030 = ekRBsLJFMvDEJti54886690;     ekRBsLJFMvDEJti54886690 = ekRBsLJFMvDEJti77333146;     ekRBsLJFMvDEJti77333146 = ekRBsLJFMvDEJti63251839;     ekRBsLJFMvDEJti63251839 = ekRBsLJFMvDEJti37839447;     ekRBsLJFMvDEJti37839447 = ekRBsLJFMvDEJti92517813;     ekRBsLJFMvDEJti92517813 = ekRBsLJFMvDEJti94604550;     ekRBsLJFMvDEJti94604550 = ekRBsLJFMvDEJti52355550;     ekRBsLJFMvDEJti52355550 = ekRBsLJFMvDEJti57558791;     ekRBsLJFMvDEJti57558791 = ekRBsLJFMvDEJti76832673;     ekRBsLJFMvDEJti76832673 = ekRBsLJFMvDEJti5013691;     ekRBsLJFMvDEJti5013691 = ekRBsLJFMvDEJti2286980;     ekRBsLJFMvDEJti2286980 = ekRBsLJFMvDEJti55686573;     ekRBsLJFMvDEJti55686573 = ekRBsLJFMvDEJti12295731;     ekRBsLJFMvDEJti12295731 = ekRBsLJFMvDEJti13401020;     ekRBsLJFMvDEJti13401020 = ekRBsLJFMvDEJti77927561;     ekRBsLJFMvDEJti77927561 = ekRBsLJFMvDEJti27140175;     ekRBsLJFMvDEJti27140175 = ekRBsLJFMvDEJti76357683;     ekRBsLJFMvDEJti76357683 = ekRBsLJFMvDEJti64339058;     ekRBsLJFMvDEJti64339058 = ekRBsLJFMvDEJti97520938;     ekRBsLJFMvDEJti97520938 = ekRBsLJFMvDEJti6971517;     ekRBsLJFMvDEJti6971517 = ekRBsLJFMvDEJti59714668;     ekRBsLJFMvDEJti59714668 = ekRBsLJFMvDEJti6857347;     ekRBsLJFMvDEJti6857347 = ekRBsLJFMvDEJti22945939;     ekRBsLJFMvDEJti22945939 = ekRBsLJFMvDEJti26435606;     ekRBsLJFMvDEJti26435606 = ekRBsLJFMvDEJti6178299;     ekRBsLJFMvDEJti6178299 = ekRBsLJFMvDEJti71325092;     ekRBsLJFMvDEJti71325092 = ekRBsLJFMvDEJti62182634;     ekRBsLJFMvDEJti62182634 = ekRBsLJFMvDEJti32508232;     ekRBsLJFMvDEJti32508232 = ekRBsLJFMvDEJti79438029;     ekRBsLJFMvDEJti79438029 = ekRBsLJFMvDEJti61310476;     ekRBsLJFMvDEJti61310476 = ekRBsLJFMvDEJti15574442;     ekRBsLJFMvDEJti15574442 = ekRBsLJFMvDEJti7607159;     ekRBsLJFMvDEJti7607159 = ekRBsLJFMvDEJti61050495;     ekRBsLJFMvDEJti61050495 = ekRBsLJFMvDEJti10604506;     ekRBsLJFMvDEJti10604506 = ekRBsLJFMvDEJti23867993;     ekRBsLJFMvDEJti23867993 = ekRBsLJFMvDEJti56297067;     ekRBsLJFMvDEJti56297067 = ekRBsLJFMvDEJti79247562;     ekRBsLJFMvDEJti79247562 = ekRBsLJFMvDEJti13048938;     ekRBsLJFMvDEJti13048938 = ekRBsLJFMvDEJti94485517;     ekRBsLJFMvDEJti94485517 = ekRBsLJFMvDEJti23117748;     ekRBsLJFMvDEJti23117748 = ekRBsLJFMvDEJti83891958;     ekRBsLJFMvDEJti83891958 = ekRBsLJFMvDEJti23035473;     ekRBsLJFMvDEJti23035473 = ekRBsLJFMvDEJti64309841;     ekRBsLJFMvDEJti64309841 = ekRBsLJFMvDEJti17198263;     ekRBsLJFMvDEJti17198263 = ekRBsLJFMvDEJti13745527;     ekRBsLJFMvDEJti13745527 = ekRBsLJFMvDEJti22286607;     ekRBsLJFMvDEJti22286607 = ekRBsLJFMvDEJti69499390;     ekRBsLJFMvDEJti69499390 = ekRBsLJFMvDEJti5719951;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void GhqsvocYlyPUeNJf29149783() {     float uYHAHpeZFPSEpGx5713480 = -437122972;    float uYHAHpeZFPSEpGx60563242 = 94171359;    float uYHAHpeZFPSEpGx46951238 = -660233209;    float uYHAHpeZFPSEpGx94849989 = -60873576;    float uYHAHpeZFPSEpGx74973852 = -278110264;    float uYHAHpeZFPSEpGx25932919 = -522080536;    float uYHAHpeZFPSEpGx52108829 = -329748551;    float uYHAHpeZFPSEpGx62345337 = -380982331;    float uYHAHpeZFPSEpGx6278176 = -185323289;    float uYHAHpeZFPSEpGx54419727 = -38547584;    float uYHAHpeZFPSEpGx32316872 = -554493512;    float uYHAHpeZFPSEpGx6447380 = -387205727;    float uYHAHpeZFPSEpGx43410688 = -788538540;    float uYHAHpeZFPSEpGx23483796 = -988511970;    float uYHAHpeZFPSEpGx72418177 = -744392343;    float uYHAHpeZFPSEpGx25906937 = -473467271;    float uYHAHpeZFPSEpGx71095996 = -648870406;    float uYHAHpeZFPSEpGx66219558 = -413239882;    float uYHAHpeZFPSEpGx60737202 = -576504234;    float uYHAHpeZFPSEpGx20073624 = -571027598;    float uYHAHpeZFPSEpGx24827316 = -691862906;    float uYHAHpeZFPSEpGx20764378 = -525128580;    float uYHAHpeZFPSEpGx21029876 = -121022408;    float uYHAHpeZFPSEpGx96770320 = -354101786;    float uYHAHpeZFPSEpGx43608268 = -730297644;    float uYHAHpeZFPSEpGx30352277 = -139816982;    float uYHAHpeZFPSEpGx3790278 = -482364611;    float uYHAHpeZFPSEpGx42132345 = -255091852;    float uYHAHpeZFPSEpGx796043 = -142599163;    float uYHAHpeZFPSEpGx97340901 = -207408812;    float uYHAHpeZFPSEpGx35506319 = -223868800;    float uYHAHpeZFPSEpGx84424394 = -152230889;    float uYHAHpeZFPSEpGx64207106 = -279135611;    float uYHAHpeZFPSEpGx33913051 = -856599005;    float uYHAHpeZFPSEpGx24857439 = 89293878;    float uYHAHpeZFPSEpGx93716574 = -758941686;    float uYHAHpeZFPSEpGx98944699 = -522730460;    float uYHAHpeZFPSEpGx87639977 = 19485307;    float uYHAHpeZFPSEpGx71561105 = -531607621;    float uYHAHpeZFPSEpGx52467157 = -703105220;    float uYHAHpeZFPSEpGx28998182 = -791176365;    float uYHAHpeZFPSEpGx7909614 = -300337506;    float uYHAHpeZFPSEpGx76315866 = -724905121;    float uYHAHpeZFPSEpGx37205079 = -535955997;    float uYHAHpeZFPSEpGx65813172 = 93485222;    float uYHAHpeZFPSEpGx93410912 = 83221002;    float uYHAHpeZFPSEpGx8967762 = -675059863;    float uYHAHpeZFPSEpGx29514247 = -611107214;    float uYHAHpeZFPSEpGx64076732 = -959272830;    float uYHAHpeZFPSEpGx92588577 = -92963067;    float uYHAHpeZFPSEpGx26903243 = -41890922;    float uYHAHpeZFPSEpGx55590402 = 23203400;    float uYHAHpeZFPSEpGx64544379 = -682331250;    float uYHAHpeZFPSEpGx65136961 = -127764027;    float uYHAHpeZFPSEpGx4322467 = -11009459;    float uYHAHpeZFPSEpGx84949101 = -911994393;    float uYHAHpeZFPSEpGx39533367 = -784806233;    float uYHAHpeZFPSEpGx50180917 = -206131423;    float uYHAHpeZFPSEpGx51241722 = -330575932;    float uYHAHpeZFPSEpGx44621576 = -38293282;    float uYHAHpeZFPSEpGx22142642 = 60284074;    float uYHAHpeZFPSEpGx9976485 = 25343301;    float uYHAHpeZFPSEpGx61549294 = -138383168;    float uYHAHpeZFPSEpGx8937274 = -977914478;    float uYHAHpeZFPSEpGx18913408 = -814678785;    float uYHAHpeZFPSEpGx47892478 = -302262624;    float uYHAHpeZFPSEpGx42240274 = -8070116;    float uYHAHpeZFPSEpGx9497638 = -931939535;    float uYHAHpeZFPSEpGx98626356 = -977805848;    float uYHAHpeZFPSEpGx78701603 = -985450658;    float uYHAHpeZFPSEpGx26962238 = -950736812;    float uYHAHpeZFPSEpGx83456019 = -568355713;    float uYHAHpeZFPSEpGx94658452 = -881632261;    float uYHAHpeZFPSEpGx8270046 = -873399015;    float uYHAHpeZFPSEpGx91075442 = -779851233;    float uYHAHpeZFPSEpGx16917702 = -291525401;    float uYHAHpeZFPSEpGx44448512 = -800223459;    float uYHAHpeZFPSEpGx83824796 = -585066412;    float uYHAHpeZFPSEpGx30957148 = -347587009;    float uYHAHpeZFPSEpGx50197355 = -713518647;    float uYHAHpeZFPSEpGx21384516 = -464757119;    float uYHAHpeZFPSEpGx74276030 = -871257397;    float uYHAHpeZFPSEpGx78055612 = -295819022;    float uYHAHpeZFPSEpGx8207466 = 50363903;    float uYHAHpeZFPSEpGx70437658 = -65517890;    float uYHAHpeZFPSEpGx79915917 = -147072200;    float uYHAHpeZFPSEpGx19880015 = -469899639;    float uYHAHpeZFPSEpGx99070144 = -51371584;    float uYHAHpeZFPSEpGx29590584 = -745589547;    float uYHAHpeZFPSEpGx39908337 = 1288270;    float uYHAHpeZFPSEpGx54183207 = -974135453;    float uYHAHpeZFPSEpGx48763782 = -216599037;    float uYHAHpeZFPSEpGx36398256 = -649938762;    float uYHAHpeZFPSEpGx26939530 = -393314339;    float uYHAHpeZFPSEpGx30324515 = -663389294;    float uYHAHpeZFPSEpGx19021698 = -716519667;    float uYHAHpeZFPSEpGx46360320 = -61954339;    float uYHAHpeZFPSEpGx67378592 = -746990644;    float uYHAHpeZFPSEpGx18291671 = -721277212;    float uYHAHpeZFPSEpGx17920695 = -437122972;     uYHAHpeZFPSEpGx5713480 = uYHAHpeZFPSEpGx60563242;     uYHAHpeZFPSEpGx60563242 = uYHAHpeZFPSEpGx46951238;     uYHAHpeZFPSEpGx46951238 = uYHAHpeZFPSEpGx94849989;     uYHAHpeZFPSEpGx94849989 = uYHAHpeZFPSEpGx74973852;     uYHAHpeZFPSEpGx74973852 = uYHAHpeZFPSEpGx25932919;     uYHAHpeZFPSEpGx25932919 = uYHAHpeZFPSEpGx52108829;     uYHAHpeZFPSEpGx52108829 = uYHAHpeZFPSEpGx62345337;     uYHAHpeZFPSEpGx62345337 = uYHAHpeZFPSEpGx6278176;     uYHAHpeZFPSEpGx6278176 = uYHAHpeZFPSEpGx54419727;     uYHAHpeZFPSEpGx54419727 = uYHAHpeZFPSEpGx32316872;     uYHAHpeZFPSEpGx32316872 = uYHAHpeZFPSEpGx6447380;     uYHAHpeZFPSEpGx6447380 = uYHAHpeZFPSEpGx43410688;     uYHAHpeZFPSEpGx43410688 = uYHAHpeZFPSEpGx23483796;     uYHAHpeZFPSEpGx23483796 = uYHAHpeZFPSEpGx72418177;     uYHAHpeZFPSEpGx72418177 = uYHAHpeZFPSEpGx25906937;     uYHAHpeZFPSEpGx25906937 = uYHAHpeZFPSEpGx71095996;     uYHAHpeZFPSEpGx71095996 = uYHAHpeZFPSEpGx66219558;     uYHAHpeZFPSEpGx66219558 = uYHAHpeZFPSEpGx60737202;     uYHAHpeZFPSEpGx60737202 = uYHAHpeZFPSEpGx20073624;     uYHAHpeZFPSEpGx20073624 = uYHAHpeZFPSEpGx24827316;     uYHAHpeZFPSEpGx24827316 = uYHAHpeZFPSEpGx20764378;     uYHAHpeZFPSEpGx20764378 = uYHAHpeZFPSEpGx21029876;     uYHAHpeZFPSEpGx21029876 = uYHAHpeZFPSEpGx96770320;     uYHAHpeZFPSEpGx96770320 = uYHAHpeZFPSEpGx43608268;     uYHAHpeZFPSEpGx43608268 = uYHAHpeZFPSEpGx30352277;     uYHAHpeZFPSEpGx30352277 = uYHAHpeZFPSEpGx3790278;     uYHAHpeZFPSEpGx3790278 = uYHAHpeZFPSEpGx42132345;     uYHAHpeZFPSEpGx42132345 = uYHAHpeZFPSEpGx796043;     uYHAHpeZFPSEpGx796043 = uYHAHpeZFPSEpGx97340901;     uYHAHpeZFPSEpGx97340901 = uYHAHpeZFPSEpGx35506319;     uYHAHpeZFPSEpGx35506319 = uYHAHpeZFPSEpGx84424394;     uYHAHpeZFPSEpGx84424394 = uYHAHpeZFPSEpGx64207106;     uYHAHpeZFPSEpGx64207106 = uYHAHpeZFPSEpGx33913051;     uYHAHpeZFPSEpGx33913051 = uYHAHpeZFPSEpGx24857439;     uYHAHpeZFPSEpGx24857439 = uYHAHpeZFPSEpGx93716574;     uYHAHpeZFPSEpGx93716574 = uYHAHpeZFPSEpGx98944699;     uYHAHpeZFPSEpGx98944699 = uYHAHpeZFPSEpGx87639977;     uYHAHpeZFPSEpGx87639977 = uYHAHpeZFPSEpGx71561105;     uYHAHpeZFPSEpGx71561105 = uYHAHpeZFPSEpGx52467157;     uYHAHpeZFPSEpGx52467157 = uYHAHpeZFPSEpGx28998182;     uYHAHpeZFPSEpGx28998182 = uYHAHpeZFPSEpGx7909614;     uYHAHpeZFPSEpGx7909614 = uYHAHpeZFPSEpGx76315866;     uYHAHpeZFPSEpGx76315866 = uYHAHpeZFPSEpGx37205079;     uYHAHpeZFPSEpGx37205079 = uYHAHpeZFPSEpGx65813172;     uYHAHpeZFPSEpGx65813172 = uYHAHpeZFPSEpGx93410912;     uYHAHpeZFPSEpGx93410912 = uYHAHpeZFPSEpGx8967762;     uYHAHpeZFPSEpGx8967762 = uYHAHpeZFPSEpGx29514247;     uYHAHpeZFPSEpGx29514247 = uYHAHpeZFPSEpGx64076732;     uYHAHpeZFPSEpGx64076732 = uYHAHpeZFPSEpGx92588577;     uYHAHpeZFPSEpGx92588577 = uYHAHpeZFPSEpGx26903243;     uYHAHpeZFPSEpGx26903243 = uYHAHpeZFPSEpGx55590402;     uYHAHpeZFPSEpGx55590402 = uYHAHpeZFPSEpGx64544379;     uYHAHpeZFPSEpGx64544379 = uYHAHpeZFPSEpGx65136961;     uYHAHpeZFPSEpGx65136961 = uYHAHpeZFPSEpGx4322467;     uYHAHpeZFPSEpGx4322467 = uYHAHpeZFPSEpGx84949101;     uYHAHpeZFPSEpGx84949101 = uYHAHpeZFPSEpGx39533367;     uYHAHpeZFPSEpGx39533367 = uYHAHpeZFPSEpGx50180917;     uYHAHpeZFPSEpGx50180917 = uYHAHpeZFPSEpGx51241722;     uYHAHpeZFPSEpGx51241722 = uYHAHpeZFPSEpGx44621576;     uYHAHpeZFPSEpGx44621576 = uYHAHpeZFPSEpGx22142642;     uYHAHpeZFPSEpGx22142642 = uYHAHpeZFPSEpGx9976485;     uYHAHpeZFPSEpGx9976485 = uYHAHpeZFPSEpGx61549294;     uYHAHpeZFPSEpGx61549294 = uYHAHpeZFPSEpGx8937274;     uYHAHpeZFPSEpGx8937274 = uYHAHpeZFPSEpGx18913408;     uYHAHpeZFPSEpGx18913408 = uYHAHpeZFPSEpGx47892478;     uYHAHpeZFPSEpGx47892478 = uYHAHpeZFPSEpGx42240274;     uYHAHpeZFPSEpGx42240274 = uYHAHpeZFPSEpGx9497638;     uYHAHpeZFPSEpGx9497638 = uYHAHpeZFPSEpGx98626356;     uYHAHpeZFPSEpGx98626356 = uYHAHpeZFPSEpGx78701603;     uYHAHpeZFPSEpGx78701603 = uYHAHpeZFPSEpGx26962238;     uYHAHpeZFPSEpGx26962238 = uYHAHpeZFPSEpGx83456019;     uYHAHpeZFPSEpGx83456019 = uYHAHpeZFPSEpGx94658452;     uYHAHpeZFPSEpGx94658452 = uYHAHpeZFPSEpGx8270046;     uYHAHpeZFPSEpGx8270046 = uYHAHpeZFPSEpGx91075442;     uYHAHpeZFPSEpGx91075442 = uYHAHpeZFPSEpGx16917702;     uYHAHpeZFPSEpGx16917702 = uYHAHpeZFPSEpGx44448512;     uYHAHpeZFPSEpGx44448512 = uYHAHpeZFPSEpGx83824796;     uYHAHpeZFPSEpGx83824796 = uYHAHpeZFPSEpGx30957148;     uYHAHpeZFPSEpGx30957148 = uYHAHpeZFPSEpGx50197355;     uYHAHpeZFPSEpGx50197355 = uYHAHpeZFPSEpGx21384516;     uYHAHpeZFPSEpGx21384516 = uYHAHpeZFPSEpGx74276030;     uYHAHpeZFPSEpGx74276030 = uYHAHpeZFPSEpGx78055612;     uYHAHpeZFPSEpGx78055612 = uYHAHpeZFPSEpGx8207466;     uYHAHpeZFPSEpGx8207466 = uYHAHpeZFPSEpGx70437658;     uYHAHpeZFPSEpGx70437658 = uYHAHpeZFPSEpGx79915917;     uYHAHpeZFPSEpGx79915917 = uYHAHpeZFPSEpGx19880015;     uYHAHpeZFPSEpGx19880015 = uYHAHpeZFPSEpGx99070144;     uYHAHpeZFPSEpGx99070144 = uYHAHpeZFPSEpGx29590584;     uYHAHpeZFPSEpGx29590584 = uYHAHpeZFPSEpGx39908337;     uYHAHpeZFPSEpGx39908337 = uYHAHpeZFPSEpGx54183207;     uYHAHpeZFPSEpGx54183207 = uYHAHpeZFPSEpGx48763782;     uYHAHpeZFPSEpGx48763782 = uYHAHpeZFPSEpGx36398256;     uYHAHpeZFPSEpGx36398256 = uYHAHpeZFPSEpGx26939530;     uYHAHpeZFPSEpGx26939530 = uYHAHpeZFPSEpGx30324515;     uYHAHpeZFPSEpGx30324515 = uYHAHpeZFPSEpGx19021698;     uYHAHpeZFPSEpGx19021698 = uYHAHpeZFPSEpGx46360320;     uYHAHpeZFPSEpGx46360320 = uYHAHpeZFPSEpGx67378592;     uYHAHpeZFPSEpGx67378592 = uYHAHpeZFPSEpGx18291671;     uYHAHpeZFPSEpGx18291671 = uYHAHpeZFPSEpGx17920695;     uYHAHpeZFPSEpGx17920695 = uYHAHpeZFPSEpGx5713480;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void rGzRrgCyfdGtnLBb5161903() {     float lYQEnqbARxcPFzD92635157 = -577774179;    float lYQEnqbARxcPFzD70210980 = -16415220;    float lYQEnqbARxcPFzD39647252 = -91348592;    float lYQEnqbARxcPFzD22713184 = -66670863;    float lYQEnqbARxcPFzD25702940 = -131923749;    float lYQEnqbARxcPFzD39398596 = -919773830;    float lYQEnqbARxcPFzD2154498 = -11546589;    float lYQEnqbARxcPFzD45061685 = -911522167;    float lYQEnqbARxcPFzD84663865 = -394833992;    float lYQEnqbARxcPFzD27920114 = -853408790;    float lYQEnqbARxcPFzD51997602 = -708582257;    float lYQEnqbARxcPFzD76073857 = -509308620;    float lYQEnqbARxcPFzD4037138 = -876901829;    float lYQEnqbARxcPFzD66801905 = -785010370;    float lYQEnqbARxcPFzD96131046 = -931157911;    float lYQEnqbARxcPFzD61186096 = -452118857;    float lYQEnqbARxcPFzD20936172 = -218790243;    float lYQEnqbARxcPFzD95436315 = -88626130;    float lYQEnqbARxcPFzD21896774 = 86974796;    float lYQEnqbARxcPFzD76826451 = 90874563;    float lYQEnqbARxcPFzD92292984 = -736541904;    float lYQEnqbARxcPFzD59454337 = -353250482;    float lYQEnqbARxcPFzD90676215 = -18428658;    float lYQEnqbARxcPFzD73127271 = -288190562;    float lYQEnqbARxcPFzD99573253 = -754565436;    float lYQEnqbARxcPFzD24531178 = -803340324;    float lYQEnqbARxcPFzD60680705 = -734466676;    float lYQEnqbARxcPFzD42718895 = -482741125;    float lYQEnqbARxcPFzD73698836 = -558831780;    float lYQEnqbARxcPFzD77054540 = -868628881;    float lYQEnqbARxcPFzD94626571 = -140297699;    float lYQEnqbARxcPFzD88866423 = -323152587;    float lYQEnqbARxcPFzD18480189 = -862827341;    float lYQEnqbARxcPFzD63836148 = -992713774;    float lYQEnqbARxcPFzD17636388 = -282676216;    float lYQEnqbARxcPFzD26676790 = -939537484;    float lYQEnqbARxcPFzD54913962 = -490635749;    float lYQEnqbARxcPFzD59968355 = -960747266;    float lYQEnqbARxcPFzD24792272 = -894458780;    float lYQEnqbARxcPFzD80254231 = -983498389;    float lYQEnqbARxcPFzD12159613 = -513099016;    float lYQEnqbARxcPFzD92750539 = -657492968;    float lYQEnqbARxcPFzD52766040 = -518385171;    float lYQEnqbARxcPFzD55749758 = -585951679;    float lYQEnqbARxcPFzD17798033 = -362406495;    float lYQEnqbARxcPFzD95977834 = -972129114;    float lYQEnqbARxcPFzD29617787 = -150764378;    float lYQEnqbARxcPFzD31770191 = -809876966;    float lYQEnqbARxcPFzD54302361 = -809016485;    float lYQEnqbARxcPFzD85188156 = -430981402;    float lYQEnqbARxcPFzD68261105 = -101245649;    float lYQEnqbARxcPFzD60503445 = -576109280;    float lYQEnqbARxcPFzD49685981 = -490250937;    float lYQEnqbARxcPFzD19681931 = -743856188;    float lYQEnqbARxcPFzD12868919 = -662122701;    float lYQEnqbARxcPFzD33180820 = -124523697;    float lYQEnqbARxcPFzD79534765 = -997986563;    float lYQEnqbARxcPFzD66519981 = -803158031;    float lYQEnqbARxcPFzD23139931 = -312105428;    float lYQEnqbARxcPFzD1171763 = -328583426;    float lYQEnqbARxcPFzD78717891 = -85307155;    float lYQEnqbARxcPFzD59435603 = -528805464;    float lYQEnqbARxcPFzD71362849 = -252690388;    float lYQEnqbARxcPFzD7609326 = -526205111;    float lYQEnqbARxcPFzD33293542 = -613111091;    float lYQEnqbARxcPFzD63131178 = -285429670;    float lYQEnqbARxcPFzD57593669 = -646481280;    float lYQEnqbARxcPFzD40200989 = -884188055;    float lYQEnqbARxcPFzD49165517 = -402334155;    float lYQEnqbARxcPFzD69454257 = -991620428;    float lYQEnqbARxcPFzD6272134 = -961483109;    float lYQEnqbARxcPFzD60967817 = -258042977;    float lYQEnqbARxcPFzD70644043 = -194167350;    float lYQEnqbARxcPFzD41642542 = 70473185;    float lYQEnqbARxcPFzD64666839 = -396026422;    float lYQEnqbARxcPFzD99542444 = 20951063;    float lYQEnqbARxcPFzD6688298 = -834865312;    float lYQEnqbARxcPFzD34926458 = -432476979;    float lYQEnqbARxcPFzD55329238 = -925784068;    float lYQEnqbARxcPFzD3595419 = -782436323;    float lYQEnqbARxcPFzD94913390 = -552575946;    float lYQEnqbARxcPFzD28910515 = -924589710;    float lYQEnqbARxcPFzD88416533 = -673724641;    float lYQEnqbARxcPFzD88510679 = -27850378;    float lYQEnqbARxcPFzD8793435 = -667383233;    float lYQEnqbARxcPFzD34123127 = -564188419;    float lYQEnqbARxcPFzD39180442 = -832901651;    float lYQEnqbARxcPFzD98798258 = -18971154;    float lYQEnqbARxcPFzD50967230 = -230591074;    float lYQEnqbARxcPFzD84455568 = -58152519;    float lYQEnqbARxcPFzD47142024 = -941550922;    float lYQEnqbARxcPFzD88393981 = -687477719;    float lYQEnqbARxcPFzD36828424 = -548641839;    float lYQEnqbARxcPFzD23620510 = -465875354;    float lYQEnqbARxcPFzD1536341 = -798191234;    float lYQEnqbARxcPFzD52724010 = -984293552;    float lYQEnqbARxcPFzD21387690 = -304802581;    float lYQEnqbARxcPFzD45156715 = -992180060;    float lYQEnqbARxcPFzD22456216 = -972840588;    float lYQEnqbARxcPFzD54666854 = -577774179;     lYQEnqbARxcPFzD92635157 = lYQEnqbARxcPFzD70210980;     lYQEnqbARxcPFzD70210980 = lYQEnqbARxcPFzD39647252;     lYQEnqbARxcPFzD39647252 = lYQEnqbARxcPFzD22713184;     lYQEnqbARxcPFzD22713184 = lYQEnqbARxcPFzD25702940;     lYQEnqbARxcPFzD25702940 = lYQEnqbARxcPFzD39398596;     lYQEnqbARxcPFzD39398596 = lYQEnqbARxcPFzD2154498;     lYQEnqbARxcPFzD2154498 = lYQEnqbARxcPFzD45061685;     lYQEnqbARxcPFzD45061685 = lYQEnqbARxcPFzD84663865;     lYQEnqbARxcPFzD84663865 = lYQEnqbARxcPFzD27920114;     lYQEnqbARxcPFzD27920114 = lYQEnqbARxcPFzD51997602;     lYQEnqbARxcPFzD51997602 = lYQEnqbARxcPFzD76073857;     lYQEnqbARxcPFzD76073857 = lYQEnqbARxcPFzD4037138;     lYQEnqbARxcPFzD4037138 = lYQEnqbARxcPFzD66801905;     lYQEnqbARxcPFzD66801905 = lYQEnqbARxcPFzD96131046;     lYQEnqbARxcPFzD96131046 = lYQEnqbARxcPFzD61186096;     lYQEnqbARxcPFzD61186096 = lYQEnqbARxcPFzD20936172;     lYQEnqbARxcPFzD20936172 = lYQEnqbARxcPFzD95436315;     lYQEnqbARxcPFzD95436315 = lYQEnqbARxcPFzD21896774;     lYQEnqbARxcPFzD21896774 = lYQEnqbARxcPFzD76826451;     lYQEnqbARxcPFzD76826451 = lYQEnqbARxcPFzD92292984;     lYQEnqbARxcPFzD92292984 = lYQEnqbARxcPFzD59454337;     lYQEnqbARxcPFzD59454337 = lYQEnqbARxcPFzD90676215;     lYQEnqbARxcPFzD90676215 = lYQEnqbARxcPFzD73127271;     lYQEnqbARxcPFzD73127271 = lYQEnqbARxcPFzD99573253;     lYQEnqbARxcPFzD99573253 = lYQEnqbARxcPFzD24531178;     lYQEnqbARxcPFzD24531178 = lYQEnqbARxcPFzD60680705;     lYQEnqbARxcPFzD60680705 = lYQEnqbARxcPFzD42718895;     lYQEnqbARxcPFzD42718895 = lYQEnqbARxcPFzD73698836;     lYQEnqbARxcPFzD73698836 = lYQEnqbARxcPFzD77054540;     lYQEnqbARxcPFzD77054540 = lYQEnqbARxcPFzD94626571;     lYQEnqbARxcPFzD94626571 = lYQEnqbARxcPFzD88866423;     lYQEnqbARxcPFzD88866423 = lYQEnqbARxcPFzD18480189;     lYQEnqbARxcPFzD18480189 = lYQEnqbARxcPFzD63836148;     lYQEnqbARxcPFzD63836148 = lYQEnqbARxcPFzD17636388;     lYQEnqbARxcPFzD17636388 = lYQEnqbARxcPFzD26676790;     lYQEnqbARxcPFzD26676790 = lYQEnqbARxcPFzD54913962;     lYQEnqbARxcPFzD54913962 = lYQEnqbARxcPFzD59968355;     lYQEnqbARxcPFzD59968355 = lYQEnqbARxcPFzD24792272;     lYQEnqbARxcPFzD24792272 = lYQEnqbARxcPFzD80254231;     lYQEnqbARxcPFzD80254231 = lYQEnqbARxcPFzD12159613;     lYQEnqbARxcPFzD12159613 = lYQEnqbARxcPFzD92750539;     lYQEnqbARxcPFzD92750539 = lYQEnqbARxcPFzD52766040;     lYQEnqbARxcPFzD52766040 = lYQEnqbARxcPFzD55749758;     lYQEnqbARxcPFzD55749758 = lYQEnqbARxcPFzD17798033;     lYQEnqbARxcPFzD17798033 = lYQEnqbARxcPFzD95977834;     lYQEnqbARxcPFzD95977834 = lYQEnqbARxcPFzD29617787;     lYQEnqbARxcPFzD29617787 = lYQEnqbARxcPFzD31770191;     lYQEnqbARxcPFzD31770191 = lYQEnqbARxcPFzD54302361;     lYQEnqbARxcPFzD54302361 = lYQEnqbARxcPFzD85188156;     lYQEnqbARxcPFzD85188156 = lYQEnqbARxcPFzD68261105;     lYQEnqbARxcPFzD68261105 = lYQEnqbARxcPFzD60503445;     lYQEnqbARxcPFzD60503445 = lYQEnqbARxcPFzD49685981;     lYQEnqbARxcPFzD49685981 = lYQEnqbARxcPFzD19681931;     lYQEnqbARxcPFzD19681931 = lYQEnqbARxcPFzD12868919;     lYQEnqbARxcPFzD12868919 = lYQEnqbARxcPFzD33180820;     lYQEnqbARxcPFzD33180820 = lYQEnqbARxcPFzD79534765;     lYQEnqbARxcPFzD79534765 = lYQEnqbARxcPFzD66519981;     lYQEnqbARxcPFzD66519981 = lYQEnqbARxcPFzD23139931;     lYQEnqbARxcPFzD23139931 = lYQEnqbARxcPFzD1171763;     lYQEnqbARxcPFzD1171763 = lYQEnqbARxcPFzD78717891;     lYQEnqbARxcPFzD78717891 = lYQEnqbARxcPFzD59435603;     lYQEnqbARxcPFzD59435603 = lYQEnqbARxcPFzD71362849;     lYQEnqbARxcPFzD71362849 = lYQEnqbARxcPFzD7609326;     lYQEnqbARxcPFzD7609326 = lYQEnqbARxcPFzD33293542;     lYQEnqbARxcPFzD33293542 = lYQEnqbARxcPFzD63131178;     lYQEnqbARxcPFzD63131178 = lYQEnqbARxcPFzD57593669;     lYQEnqbARxcPFzD57593669 = lYQEnqbARxcPFzD40200989;     lYQEnqbARxcPFzD40200989 = lYQEnqbARxcPFzD49165517;     lYQEnqbARxcPFzD49165517 = lYQEnqbARxcPFzD69454257;     lYQEnqbARxcPFzD69454257 = lYQEnqbARxcPFzD6272134;     lYQEnqbARxcPFzD6272134 = lYQEnqbARxcPFzD60967817;     lYQEnqbARxcPFzD60967817 = lYQEnqbARxcPFzD70644043;     lYQEnqbARxcPFzD70644043 = lYQEnqbARxcPFzD41642542;     lYQEnqbARxcPFzD41642542 = lYQEnqbARxcPFzD64666839;     lYQEnqbARxcPFzD64666839 = lYQEnqbARxcPFzD99542444;     lYQEnqbARxcPFzD99542444 = lYQEnqbARxcPFzD6688298;     lYQEnqbARxcPFzD6688298 = lYQEnqbARxcPFzD34926458;     lYQEnqbARxcPFzD34926458 = lYQEnqbARxcPFzD55329238;     lYQEnqbARxcPFzD55329238 = lYQEnqbARxcPFzD3595419;     lYQEnqbARxcPFzD3595419 = lYQEnqbARxcPFzD94913390;     lYQEnqbARxcPFzD94913390 = lYQEnqbARxcPFzD28910515;     lYQEnqbARxcPFzD28910515 = lYQEnqbARxcPFzD88416533;     lYQEnqbARxcPFzD88416533 = lYQEnqbARxcPFzD88510679;     lYQEnqbARxcPFzD88510679 = lYQEnqbARxcPFzD8793435;     lYQEnqbARxcPFzD8793435 = lYQEnqbARxcPFzD34123127;     lYQEnqbARxcPFzD34123127 = lYQEnqbARxcPFzD39180442;     lYQEnqbARxcPFzD39180442 = lYQEnqbARxcPFzD98798258;     lYQEnqbARxcPFzD98798258 = lYQEnqbARxcPFzD50967230;     lYQEnqbARxcPFzD50967230 = lYQEnqbARxcPFzD84455568;     lYQEnqbARxcPFzD84455568 = lYQEnqbARxcPFzD47142024;     lYQEnqbARxcPFzD47142024 = lYQEnqbARxcPFzD88393981;     lYQEnqbARxcPFzD88393981 = lYQEnqbARxcPFzD36828424;     lYQEnqbARxcPFzD36828424 = lYQEnqbARxcPFzD23620510;     lYQEnqbARxcPFzD23620510 = lYQEnqbARxcPFzD1536341;     lYQEnqbARxcPFzD1536341 = lYQEnqbARxcPFzD52724010;     lYQEnqbARxcPFzD52724010 = lYQEnqbARxcPFzD21387690;     lYQEnqbARxcPFzD21387690 = lYQEnqbARxcPFzD45156715;     lYQEnqbARxcPFzD45156715 = lYQEnqbARxcPFzD22456216;     lYQEnqbARxcPFzD22456216 = lYQEnqbARxcPFzD54666854;     lYQEnqbARxcPFzD54666854 = lYQEnqbARxcPFzD92635157;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void IyPoLikTgZrXBIHm89894543() {     float zpDmtQWZRJumuNF31019544 = -392799687;    float zpDmtQWZRJumuNF76234379 = -357036754;    float zpDmtQWZRJumuNF87652803 = -387912283;    float zpDmtQWZRJumuNF6216218 = -257114376;    float zpDmtQWZRJumuNF70875845 = -833951851;    float zpDmtQWZRJumuNF30107079 = -352812523;    float zpDmtQWZRJumuNF91588428 = -630416933;    float zpDmtQWZRJumuNF48362821 = -128367653;    float zpDmtQWZRJumuNF39917640 = -325523988;    float zpDmtQWZRJumuNF60018173 = -618444204;    float zpDmtQWZRJumuNF53177328 = -114752014;    float zpDmtQWZRJumuNF14758664 = -889393609;    float zpDmtQWZRJumuNF87055390 = -501615903;    float zpDmtQWZRJumuNF297261 = -401643758;    float zpDmtQWZRJumuNF85362269 = -801263978;    float zpDmtQWZRJumuNF48203113 = -773610422;    float zpDmtQWZRJumuNF64780587 = 68344636;    float zpDmtQWZRJumuNF54109639 = -465970545;    float zpDmtQWZRJumuNF43665585 = -469503724;    float zpDmtQWZRJumuNF96494888 = -358760049;    float zpDmtQWZRJumuNF14137458 = -68625197;    float zpDmtQWZRJumuNF33412732 = 77142007;    float zpDmtQWZRJumuNF59443515 = -175608974;    float zpDmtQWZRJumuNF32048226 = -745543509;    float zpDmtQWZRJumuNF3485273 = -779115784;    float zpDmtQWZRJumuNF2190805 = -241810212;    float zpDmtQWZRJumuNF14040987 = -332386665;    float zpDmtQWZRJumuNF3163570 = -768267480;    float zpDmtQWZRJumuNF72836207 = -817202256;    float zpDmtQWZRJumuNF30948407 = 43438150;    float zpDmtQWZRJumuNF94283844 = -521175011;    float zpDmtQWZRJumuNF59927465 = -892024580;    float zpDmtQWZRJumuNF14808344 = -782614834;    float zpDmtQWZRJumuNF93933918 = -100249499;    float zpDmtQWZRJumuNF37385832 = 24483839;    float zpDmtQWZRJumuNF41036375 = 13836170;    float zpDmtQWZRJumuNF17440673 = -561750288;    float zpDmtQWZRJumuNF53409705 = -738556822;    float zpDmtQWZRJumuNF864044 = -869284859;    float zpDmtQWZRJumuNF71556850 = -979022182;    float zpDmtQWZRJumuNF45030866 = -187745402;    float zpDmtQWZRJumuNF73067263 = -677258662;    float zpDmtQWZRJumuNF16283009 = -552164491;    float zpDmtQWZRJumuNF52707778 = -81610634;    float zpDmtQWZRJumuNF21751295 = -347678770;    float zpDmtQWZRJumuNF55150349 = -843905768;    float zpDmtQWZRJumuNF51572676 = -818187046;    float zpDmtQWZRJumuNF18555766 = 59110119;    float zpDmtQWZRJumuNF11860952 = -844766988;    float zpDmtQWZRJumuNF45857726 = -615991240;    float zpDmtQWZRJumuNF70206107 = -345678545;    float zpDmtQWZRJumuNF86844056 = -461082305;    float zpDmtQWZRJumuNF51348039 = -350044749;    float zpDmtQWZRJumuNF12780023 = -684532009;    float zpDmtQWZRJumuNF75757085 = -517859054;    float zpDmtQWZRJumuNF97606812 = -369941694;    float zpDmtQWZRJumuNF16790864 = -81427781;    float zpDmtQWZRJumuNF55604577 = -642368774;    float zpDmtQWZRJumuNF2730946 = -477998593;    float zpDmtQWZRJumuNF68685040 = -492141639;    float zpDmtQWZRJumuNF16066092 = 79574141;    float zpDmtQWZRJumuNF88424858 = -862149454;    float zpDmtQWZRJumuNF75526613 = -311165398;    float zpDmtQWZRJumuNF8969234 = -268962138;    float zpDmtQWZRJumuNF65734329 = 2730806;    float zpDmtQWZRJumuNF93249862 = -222727435;    float zpDmtQWZRJumuNF99950320 = -6778775;    float zpDmtQWZRJumuNF93121471 = -301366404;    float zpDmtQWZRJumuNF62911428 = -326127598;    float zpDmtQWZRJumuNF44325895 = -715100148;    float zpDmtQWZRJumuNF30762440 = -111860135;    float zpDmtQWZRJumuNF11370882 = -193098543;    float zpDmtQWZRJumuNF53245596 = -596685686;    float zpDmtQWZRJumuNF72108735 = -490481543;    float zpDmtQWZRJumuNF51464022 = -71014648;    float zpDmtQWZRJumuNF41070195 = -391366536;    float zpDmtQWZRJumuNF17129723 = -370693503;    float zpDmtQWZRJumuNF6735738 = 6001660;    float zpDmtQWZRJumuNF10296932 = -297864739;    float zpDmtQWZRJumuNF48334923 = -935210017;    float zpDmtQWZRJumuNF50618129 = -423623166;    float zpDmtQWZRJumuNF95485221 = -291496784;    float zpDmtQWZRJumuNF91302618 = -923500492;    float zpDmtQWZRJumuNF26978481 = -101211016;    float zpDmtQWZRJumuNF60742300 = -610883306;    float zpDmtQWZRJumuNF7439788 = 39907294;    float zpDmtQWZRJumuNF8579426 = -441979831;    float zpDmtQWZRJumuNF2028321 = 1917174;    float zpDmtQWZRJumuNF18176833 = -582390445;    float zpDmtQWZRJumuNF39779020 = -605574468;    float zpDmtQWZRJumuNF24245512 = -904736050;    float zpDmtQWZRJumuNF61836096 = -919381514;    float zpDmtQWZRJumuNF50678760 = -160558230;    float zpDmtQWZRJumuNF32179003 = -277143221;    float zpDmtQWZRJumuNF55490758 = -958596323;    float zpDmtQWZRJumuNF56606008 = -325595949;    float zpDmtQWZRJumuNF97540650 = -266093265;    float zpDmtQWZRJumuNF7313776 = -183202353;    float zpDmtQWZRJumuNF86973449 = 15658559;    float zpDmtQWZRJumuNF28501432 = -392799687;     zpDmtQWZRJumuNF31019544 = zpDmtQWZRJumuNF76234379;     zpDmtQWZRJumuNF76234379 = zpDmtQWZRJumuNF87652803;     zpDmtQWZRJumuNF87652803 = zpDmtQWZRJumuNF6216218;     zpDmtQWZRJumuNF6216218 = zpDmtQWZRJumuNF70875845;     zpDmtQWZRJumuNF70875845 = zpDmtQWZRJumuNF30107079;     zpDmtQWZRJumuNF30107079 = zpDmtQWZRJumuNF91588428;     zpDmtQWZRJumuNF91588428 = zpDmtQWZRJumuNF48362821;     zpDmtQWZRJumuNF48362821 = zpDmtQWZRJumuNF39917640;     zpDmtQWZRJumuNF39917640 = zpDmtQWZRJumuNF60018173;     zpDmtQWZRJumuNF60018173 = zpDmtQWZRJumuNF53177328;     zpDmtQWZRJumuNF53177328 = zpDmtQWZRJumuNF14758664;     zpDmtQWZRJumuNF14758664 = zpDmtQWZRJumuNF87055390;     zpDmtQWZRJumuNF87055390 = zpDmtQWZRJumuNF297261;     zpDmtQWZRJumuNF297261 = zpDmtQWZRJumuNF85362269;     zpDmtQWZRJumuNF85362269 = zpDmtQWZRJumuNF48203113;     zpDmtQWZRJumuNF48203113 = zpDmtQWZRJumuNF64780587;     zpDmtQWZRJumuNF64780587 = zpDmtQWZRJumuNF54109639;     zpDmtQWZRJumuNF54109639 = zpDmtQWZRJumuNF43665585;     zpDmtQWZRJumuNF43665585 = zpDmtQWZRJumuNF96494888;     zpDmtQWZRJumuNF96494888 = zpDmtQWZRJumuNF14137458;     zpDmtQWZRJumuNF14137458 = zpDmtQWZRJumuNF33412732;     zpDmtQWZRJumuNF33412732 = zpDmtQWZRJumuNF59443515;     zpDmtQWZRJumuNF59443515 = zpDmtQWZRJumuNF32048226;     zpDmtQWZRJumuNF32048226 = zpDmtQWZRJumuNF3485273;     zpDmtQWZRJumuNF3485273 = zpDmtQWZRJumuNF2190805;     zpDmtQWZRJumuNF2190805 = zpDmtQWZRJumuNF14040987;     zpDmtQWZRJumuNF14040987 = zpDmtQWZRJumuNF3163570;     zpDmtQWZRJumuNF3163570 = zpDmtQWZRJumuNF72836207;     zpDmtQWZRJumuNF72836207 = zpDmtQWZRJumuNF30948407;     zpDmtQWZRJumuNF30948407 = zpDmtQWZRJumuNF94283844;     zpDmtQWZRJumuNF94283844 = zpDmtQWZRJumuNF59927465;     zpDmtQWZRJumuNF59927465 = zpDmtQWZRJumuNF14808344;     zpDmtQWZRJumuNF14808344 = zpDmtQWZRJumuNF93933918;     zpDmtQWZRJumuNF93933918 = zpDmtQWZRJumuNF37385832;     zpDmtQWZRJumuNF37385832 = zpDmtQWZRJumuNF41036375;     zpDmtQWZRJumuNF41036375 = zpDmtQWZRJumuNF17440673;     zpDmtQWZRJumuNF17440673 = zpDmtQWZRJumuNF53409705;     zpDmtQWZRJumuNF53409705 = zpDmtQWZRJumuNF864044;     zpDmtQWZRJumuNF864044 = zpDmtQWZRJumuNF71556850;     zpDmtQWZRJumuNF71556850 = zpDmtQWZRJumuNF45030866;     zpDmtQWZRJumuNF45030866 = zpDmtQWZRJumuNF73067263;     zpDmtQWZRJumuNF73067263 = zpDmtQWZRJumuNF16283009;     zpDmtQWZRJumuNF16283009 = zpDmtQWZRJumuNF52707778;     zpDmtQWZRJumuNF52707778 = zpDmtQWZRJumuNF21751295;     zpDmtQWZRJumuNF21751295 = zpDmtQWZRJumuNF55150349;     zpDmtQWZRJumuNF55150349 = zpDmtQWZRJumuNF51572676;     zpDmtQWZRJumuNF51572676 = zpDmtQWZRJumuNF18555766;     zpDmtQWZRJumuNF18555766 = zpDmtQWZRJumuNF11860952;     zpDmtQWZRJumuNF11860952 = zpDmtQWZRJumuNF45857726;     zpDmtQWZRJumuNF45857726 = zpDmtQWZRJumuNF70206107;     zpDmtQWZRJumuNF70206107 = zpDmtQWZRJumuNF86844056;     zpDmtQWZRJumuNF86844056 = zpDmtQWZRJumuNF51348039;     zpDmtQWZRJumuNF51348039 = zpDmtQWZRJumuNF12780023;     zpDmtQWZRJumuNF12780023 = zpDmtQWZRJumuNF75757085;     zpDmtQWZRJumuNF75757085 = zpDmtQWZRJumuNF97606812;     zpDmtQWZRJumuNF97606812 = zpDmtQWZRJumuNF16790864;     zpDmtQWZRJumuNF16790864 = zpDmtQWZRJumuNF55604577;     zpDmtQWZRJumuNF55604577 = zpDmtQWZRJumuNF2730946;     zpDmtQWZRJumuNF2730946 = zpDmtQWZRJumuNF68685040;     zpDmtQWZRJumuNF68685040 = zpDmtQWZRJumuNF16066092;     zpDmtQWZRJumuNF16066092 = zpDmtQWZRJumuNF88424858;     zpDmtQWZRJumuNF88424858 = zpDmtQWZRJumuNF75526613;     zpDmtQWZRJumuNF75526613 = zpDmtQWZRJumuNF8969234;     zpDmtQWZRJumuNF8969234 = zpDmtQWZRJumuNF65734329;     zpDmtQWZRJumuNF65734329 = zpDmtQWZRJumuNF93249862;     zpDmtQWZRJumuNF93249862 = zpDmtQWZRJumuNF99950320;     zpDmtQWZRJumuNF99950320 = zpDmtQWZRJumuNF93121471;     zpDmtQWZRJumuNF93121471 = zpDmtQWZRJumuNF62911428;     zpDmtQWZRJumuNF62911428 = zpDmtQWZRJumuNF44325895;     zpDmtQWZRJumuNF44325895 = zpDmtQWZRJumuNF30762440;     zpDmtQWZRJumuNF30762440 = zpDmtQWZRJumuNF11370882;     zpDmtQWZRJumuNF11370882 = zpDmtQWZRJumuNF53245596;     zpDmtQWZRJumuNF53245596 = zpDmtQWZRJumuNF72108735;     zpDmtQWZRJumuNF72108735 = zpDmtQWZRJumuNF51464022;     zpDmtQWZRJumuNF51464022 = zpDmtQWZRJumuNF41070195;     zpDmtQWZRJumuNF41070195 = zpDmtQWZRJumuNF17129723;     zpDmtQWZRJumuNF17129723 = zpDmtQWZRJumuNF6735738;     zpDmtQWZRJumuNF6735738 = zpDmtQWZRJumuNF10296932;     zpDmtQWZRJumuNF10296932 = zpDmtQWZRJumuNF48334923;     zpDmtQWZRJumuNF48334923 = zpDmtQWZRJumuNF50618129;     zpDmtQWZRJumuNF50618129 = zpDmtQWZRJumuNF95485221;     zpDmtQWZRJumuNF95485221 = zpDmtQWZRJumuNF91302618;     zpDmtQWZRJumuNF91302618 = zpDmtQWZRJumuNF26978481;     zpDmtQWZRJumuNF26978481 = zpDmtQWZRJumuNF60742300;     zpDmtQWZRJumuNF60742300 = zpDmtQWZRJumuNF7439788;     zpDmtQWZRJumuNF7439788 = zpDmtQWZRJumuNF8579426;     zpDmtQWZRJumuNF8579426 = zpDmtQWZRJumuNF2028321;     zpDmtQWZRJumuNF2028321 = zpDmtQWZRJumuNF18176833;     zpDmtQWZRJumuNF18176833 = zpDmtQWZRJumuNF39779020;     zpDmtQWZRJumuNF39779020 = zpDmtQWZRJumuNF24245512;     zpDmtQWZRJumuNF24245512 = zpDmtQWZRJumuNF61836096;     zpDmtQWZRJumuNF61836096 = zpDmtQWZRJumuNF50678760;     zpDmtQWZRJumuNF50678760 = zpDmtQWZRJumuNF32179003;     zpDmtQWZRJumuNF32179003 = zpDmtQWZRJumuNF55490758;     zpDmtQWZRJumuNF55490758 = zpDmtQWZRJumuNF56606008;     zpDmtQWZRJumuNF56606008 = zpDmtQWZRJumuNF97540650;     zpDmtQWZRJumuNF97540650 = zpDmtQWZRJumuNF7313776;     zpDmtQWZRJumuNF7313776 = zpDmtQWZRJumuNF86973449;     zpDmtQWZRJumuNF86973449 = zpDmtQWZRJumuNF28501432;     zpDmtQWZRJumuNF28501432 = zpDmtQWZRJumuNF31019544;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void GyGwxMcJsRsnXeQS96553287() {     float bLiRmBVsVorXkYM31013073 = -265323830;    float bLiRmBVsVorXkYM14806696 = -399631885;    float bLiRmBVsVorXkYM88966015 = -345040896;    float bLiRmBVsVorXkYM91296078 = -77655203;    float bLiRmBVsVorXkYM79715947 = -202307180;    float bLiRmBVsVorXkYM64912511 = -52245335;    float bLiRmBVsVorXkYM60135764 = -334953400;    float bLiRmBVsVorXkYM17576872 = -932544986;    float bLiRmBVsVorXkYM22657805 = -39170059;    float bLiRmBVsVorXkYM56657687 = -255251075;    float bLiRmBVsVorXkYM73497933 = -74224087;    float bLiRmBVsVorXkYM29050341 = -624871980;    float bLiRmBVsVorXkYM18908304 = -523274377;    float bLiRmBVsVorXkYM54141480 = -746796811;    float bLiRmBVsVorXkYM30534379 = -763976848;    float bLiRmBVsVorXkYM22767661 = -6406073;    float bLiRmBVsVorXkYM20633347 = -851269934;    float bLiRmBVsVorXkYM77110172 = -863042171;    float bLiRmBVsVorXkYM64093855 = 12514010;    float bLiRmBVsVorXkYM68568651 = -970784500;    float bLiRmBVsVorXkYM93806882 = -879091576;    float bLiRmBVsVorXkYM11708998 = -432849876;    float bLiRmBVsVorXkYM96321912 = -634566816;    float bLiRmBVsVorXkYM75698334 = -626464031;    float bLiRmBVsVorXkYM31927962 = -105809649;    float bLiRmBVsVorXkYM18764883 = -150016136;    float bLiRmBVsVorXkYM84262568 = -633186377;    float bLiRmBVsVorXkYM38567095 = -277234474;    float bLiRmBVsVorXkYM85514654 = -363272555;    float bLiRmBVsVorXkYM7038277 = -731993224;    float bLiRmBVsVorXkYM27696523 = -908268245;    float bLiRmBVsVorXkYM44651323 = -183846338;    float bLiRmBVsVorXkYM260767 = -984559047;    float bLiRmBVsVorXkYM94216755 = 23068768;    float bLiRmBVsVorXkYM19743870 = -234830083;    float bLiRmBVsVorXkYM83864565 = -65929558;    float bLiRmBVsVorXkYM29382041 = -661403666;    float bLiRmBVsVorXkYM23327385 = -733819504;    float bLiRmBVsVorXkYM36177640 = -481966263;    float bLiRmBVsVorXkYM43429742 = -588453868;    float bLiRmBVsVorXkYM32886534 = -275689300;    float bLiRmBVsVorXkYM58764924 = -871050724;    float bLiRmBVsVorXkYM76566369 = -11294743;    float bLiRmBVsVorXkYM11939677 = -738575077;    float bLiRmBVsVorXkYM5769347 = -820938171;    float bLiRmBVsVorXkYM95578317 = -655950420;    float bLiRmBVsVorXkYM79270468 = -199467669;    float bLiRmBVsVorXkYM93939347 = -549651236;    float bLiRmBVsVorXkYM30519341 = -408530825;    float bLiRmBVsVorXkYM65903149 = -203016132;    float bLiRmBVsVorXkYM83465476 = 75766448;    float bLiRmBVsVorXkYM1391315 = -843228049;    float bLiRmBVsVorXkYM26796387 = -242098760;    float bLiRmBVsVorXkYM23030294 = -116451860;    float bLiRmBVsVorXkYM2746406 = -274758327;    float bLiRmBVsVorXkYM19304076 = -832473954;    float bLiRmBVsVorXkYM18484783 = -765065070;    float bLiRmBVsVorXkYM13267681 = -718576865;    float bLiRmBVsVorXkYM59368116 = -971845554;    float bLiRmBVsVorXkYM60951064 = 47708955;    float bLiRmBVsVorXkYM80649942 = -419058958;    float bLiRmBVsVorXkYM21568669 = 42281074;    float bLiRmBVsVorXkYM32062217 = -469272431;    float bLiRmBVsVorXkYM15619528 = -307176836;    float bLiRmBVsVorXkYM28961164 = -346982830;    float bLiRmBVsVorXkYM28846611 = -890377749;    float bLiRmBVsVorXkYM28789575 = -640312934;    float bLiRmBVsVorXkYM24691549 = -446343146;    float bLiRmBVsVorXkYM34397610 = -411966728;    float bLiRmBVsVorXkYM46669814 = -598047290;    float bLiRmBVsVorXkYM93385620 = -345002407;    float bLiRmBVsVorXkYM97305961 = -17450431;    float bLiRmBVsVorXkYM40932532 = -281075909;    float bLiRmBVsVorXkYM20664113 = -399032122;    float bLiRmBVsVorXkYM35682117 = -595095200;    float bLiRmBVsVorXkYM35041958 = 91959147;    float bLiRmBVsVorXkYM35142629 = -321555134;    float bLiRmBVsVorXkYM84382236 = -895991739;    float bLiRmBVsVorXkYM69928988 = -805525861;    float bLiRmBVsVorXkYM36349645 = -449859230;    float bLiRmBVsVorXkYM39494414 = -950548468;    float bLiRmBVsVorXkYM90323221 = 16464858;    float bLiRmBVsVorXkYM8047754 = -868703650;    float bLiRmBVsVorXkYM19611506 = -60256424;    float bLiRmBVsVorXkYM23572800 = -707759672;    float bLiRmBVsVorXkYM26305209 = 34959803;    float bLiRmBVsVorXkYM17854936 = -941747578;    float bLiRmBVsVorXkYM77230472 = -768107188;    float bLiRmBVsVorXkYM91470349 = -702172905;    float bLiRmBVsVorXkYM439795 = -402356129;    float bLiRmBVsVorXkYM65379782 = -300864489;    float bLiRmBVsVorXkYM16114360 = -942826801;    float bLiRmBVsVorXkYM63959268 = -761973950;    float bLiRmBVsVorXkYM75226575 = -429675219;    float bLiRmBVsVorXkYM62779800 = -69394910;    float bLiRmBVsVorXkYM11317865 = -217970375;    float bLiRmBVsVorXkYM26702708 = -301778294;    float bLiRmBVsVorXkYM60946841 = -704117908;    float bLiRmBVsVorXkYM82978512 = -291592248;    float bLiRmBVsVorXkYM76922736 = -265323830;     bLiRmBVsVorXkYM31013073 = bLiRmBVsVorXkYM14806696;     bLiRmBVsVorXkYM14806696 = bLiRmBVsVorXkYM88966015;     bLiRmBVsVorXkYM88966015 = bLiRmBVsVorXkYM91296078;     bLiRmBVsVorXkYM91296078 = bLiRmBVsVorXkYM79715947;     bLiRmBVsVorXkYM79715947 = bLiRmBVsVorXkYM64912511;     bLiRmBVsVorXkYM64912511 = bLiRmBVsVorXkYM60135764;     bLiRmBVsVorXkYM60135764 = bLiRmBVsVorXkYM17576872;     bLiRmBVsVorXkYM17576872 = bLiRmBVsVorXkYM22657805;     bLiRmBVsVorXkYM22657805 = bLiRmBVsVorXkYM56657687;     bLiRmBVsVorXkYM56657687 = bLiRmBVsVorXkYM73497933;     bLiRmBVsVorXkYM73497933 = bLiRmBVsVorXkYM29050341;     bLiRmBVsVorXkYM29050341 = bLiRmBVsVorXkYM18908304;     bLiRmBVsVorXkYM18908304 = bLiRmBVsVorXkYM54141480;     bLiRmBVsVorXkYM54141480 = bLiRmBVsVorXkYM30534379;     bLiRmBVsVorXkYM30534379 = bLiRmBVsVorXkYM22767661;     bLiRmBVsVorXkYM22767661 = bLiRmBVsVorXkYM20633347;     bLiRmBVsVorXkYM20633347 = bLiRmBVsVorXkYM77110172;     bLiRmBVsVorXkYM77110172 = bLiRmBVsVorXkYM64093855;     bLiRmBVsVorXkYM64093855 = bLiRmBVsVorXkYM68568651;     bLiRmBVsVorXkYM68568651 = bLiRmBVsVorXkYM93806882;     bLiRmBVsVorXkYM93806882 = bLiRmBVsVorXkYM11708998;     bLiRmBVsVorXkYM11708998 = bLiRmBVsVorXkYM96321912;     bLiRmBVsVorXkYM96321912 = bLiRmBVsVorXkYM75698334;     bLiRmBVsVorXkYM75698334 = bLiRmBVsVorXkYM31927962;     bLiRmBVsVorXkYM31927962 = bLiRmBVsVorXkYM18764883;     bLiRmBVsVorXkYM18764883 = bLiRmBVsVorXkYM84262568;     bLiRmBVsVorXkYM84262568 = bLiRmBVsVorXkYM38567095;     bLiRmBVsVorXkYM38567095 = bLiRmBVsVorXkYM85514654;     bLiRmBVsVorXkYM85514654 = bLiRmBVsVorXkYM7038277;     bLiRmBVsVorXkYM7038277 = bLiRmBVsVorXkYM27696523;     bLiRmBVsVorXkYM27696523 = bLiRmBVsVorXkYM44651323;     bLiRmBVsVorXkYM44651323 = bLiRmBVsVorXkYM260767;     bLiRmBVsVorXkYM260767 = bLiRmBVsVorXkYM94216755;     bLiRmBVsVorXkYM94216755 = bLiRmBVsVorXkYM19743870;     bLiRmBVsVorXkYM19743870 = bLiRmBVsVorXkYM83864565;     bLiRmBVsVorXkYM83864565 = bLiRmBVsVorXkYM29382041;     bLiRmBVsVorXkYM29382041 = bLiRmBVsVorXkYM23327385;     bLiRmBVsVorXkYM23327385 = bLiRmBVsVorXkYM36177640;     bLiRmBVsVorXkYM36177640 = bLiRmBVsVorXkYM43429742;     bLiRmBVsVorXkYM43429742 = bLiRmBVsVorXkYM32886534;     bLiRmBVsVorXkYM32886534 = bLiRmBVsVorXkYM58764924;     bLiRmBVsVorXkYM58764924 = bLiRmBVsVorXkYM76566369;     bLiRmBVsVorXkYM76566369 = bLiRmBVsVorXkYM11939677;     bLiRmBVsVorXkYM11939677 = bLiRmBVsVorXkYM5769347;     bLiRmBVsVorXkYM5769347 = bLiRmBVsVorXkYM95578317;     bLiRmBVsVorXkYM95578317 = bLiRmBVsVorXkYM79270468;     bLiRmBVsVorXkYM79270468 = bLiRmBVsVorXkYM93939347;     bLiRmBVsVorXkYM93939347 = bLiRmBVsVorXkYM30519341;     bLiRmBVsVorXkYM30519341 = bLiRmBVsVorXkYM65903149;     bLiRmBVsVorXkYM65903149 = bLiRmBVsVorXkYM83465476;     bLiRmBVsVorXkYM83465476 = bLiRmBVsVorXkYM1391315;     bLiRmBVsVorXkYM1391315 = bLiRmBVsVorXkYM26796387;     bLiRmBVsVorXkYM26796387 = bLiRmBVsVorXkYM23030294;     bLiRmBVsVorXkYM23030294 = bLiRmBVsVorXkYM2746406;     bLiRmBVsVorXkYM2746406 = bLiRmBVsVorXkYM19304076;     bLiRmBVsVorXkYM19304076 = bLiRmBVsVorXkYM18484783;     bLiRmBVsVorXkYM18484783 = bLiRmBVsVorXkYM13267681;     bLiRmBVsVorXkYM13267681 = bLiRmBVsVorXkYM59368116;     bLiRmBVsVorXkYM59368116 = bLiRmBVsVorXkYM60951064;     bLiRmBVsVorXkYM60951064 = bLiRmBVsVorXkYM80649942;     bLiRmBVsVorXkYM80649942 = bLiRmBVsVorXkYM21568669;     bLiRmBVsVorXkYM21568669 = bLiRmBVsVorXkYM32062217;     bLiRmBVsVorXkYM32062217 = bLiRmBVsVorXkYM15619528;     bLiRmBVsVorXkYM15619528 = bLiRmBVsVorXkYM28961164;     bLiRmBVsVorXkYM28961164 = bLiRmBVsVorXkYM28846611;     bLiRmBVsVorXkYM28846611 = bLiRmBVsVorXkYM28789575;     bLiRmBVsVorXkYM28789575 = bLiRmBVsVorXkYM24691549;     bLiRmBVsVorXkYM24691549 = bLiRmBVsVorXkYM34397610;     bLiRmBVsVorXkYM34397610 = bLiRmBVsVorXkYM46669814;     bLiRmBVsVorXkYM46669814 = bLiRmBVsVorXkYM93385620;     bLiRmBVsVorXkYM93385620 = bLiRmBVsVorXkYM97305961;     bLiRmBVsVorXkYM97305961 = bLiRmBVsVorXkYM40932532;     bLiRmBVsVorXkYM40932532 = bLiRmBVsVorXkYM20664113;     bLiRmBVsVorXkYM20664113 = bLiRmBVsVorXkYM35682117;     bLiRmBVsVorXkYM35682117 = bLiRmBVsVorXkYM35041958;     bLiRmBVsVorXkYM35041958 = bLiRmBVsVorXkYM35142629;     bLiRmBVsVorXkYM35142629 = bLiRmBVsVorXkYM84382236;     bLiRmBVsVorXkYM84382236 = bLiRmBVsVorXkYM69928988;     bLiRmBVsVorXkYM69928988 = bLiRmBVsVorXkYM36349645;     bLiRmBVsVorXkYM36349645 = bLiRmBVsVorXkYM39494414;     bLiRmBVsVorXkYM39494414 = bLiRmBVsVorXkYM90323221;     bLiRmBVsVorXkYM90323221 = bLiRmBVsVorXkYM8047754;     bLiRmBVsVorXkYM8047754 = bLiRmBVsVorXkYM19611506;     bLiRmBVsVorXkYM19611506 = bLiRmBVsVorXkYM23572800;     bLiRmBVsVorXkYM23572800 = bLiRmBVsVorXkYM26305209;     bLiRmBVsVorXkYM26305209 = bLiRmBVsVorXkYM17854936;     bLiRmBVsVorXkYM17854936 = bLiRmBVsVorXkYM77230472;     bLiRmBVsVorXkYM77230472 = bLiRmBVsVorXkYM91470349;     bLiRmBVsVorXkYM91470349 = bLiRmBVsVorXkYM439795;     bLiRmBVsVorXkYM439795 = bLiRmBVsVorXkYM65379782;     bLiRmBVsVorXkYM65379782 = bLiRmBVsVorXkYM16114360;     bLiRmBVsVorXkYM16114360 = bLiRmBVsVorXkYM63959268;     bLiRmBVsVorXkYM63959268 = bLiRmBVsVorXkYM75226575;     bLiRmBVsVorXkYM75226575 = bLiRmBVsVorXkYM62779800;     bLiRmBVsVorXkYM62779800 = bLiRmBVsVorXkYM11317865;     bLiRmBVsVorXkYM11317865 = bLiRmBVsVorXkYM26702708;     bLiRmBVsVorXkYM26702708 = bLiRmBVsVorXkYM60946841;     bLiRmBVsVorXkYM60946841 = bLiRmBVsVorXkYM82978512;     bLiRmBVsVorXkYM82978512 = bLiRmBVsVorXkYM76922736;     bLiRmBVsVorXkYM76922736 = bLiRmBVsVorXkYM31013073;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void IJvItkyHYAkUpuVV50969501() {     float yzfgsLOwmyEYQie1664740 = -883472960;    float yzfgsLOwmyEYQie83480213 = -271275185;    float yzfgsLOwmyEYQie68934934 = -787335356;    float yzfgsLOwmyEYQie31227365 = -267793592;    float yzfgsLOwmyEYQie1166269 = 14286472;    float yzfgsLOwmyEYQie54912274 = -853826486;    float yzfgsLOwmyEYQie78514659 = -333729110;    float yzfgsLOwmyEYQie74419252 = -179362076;    float yzfgsLOwmyEYQie68522859 = -132517388;    float yzfgsLOwmyEYQie79624147 = -556346426;    float yzfgsLOwmyEYQie15747094 = -109126016;    float yzfgsLOwmyEYQie74596912 = -940635773;    float yzfgsLOwmyEYQie98735690 = -432811435;    float yzfgsLOwmyEYQie37988514 = -547825021;    float yzfgsLOwmyEYQie13254399 = -913726851;    float yzfgsLOwmyEYQie55296301 = -676389660;    float yzfgsLOwmyEYQie14486173 = -760455064;    float yzfgsLOwmyEYQie97403666 = -852208367;    float yzfgsLOwmyEYQie45801637 = -694673932;    float yzfgsLOwmyEYQie27355361 = -413150805;    float yzfgsLOwmyEYQie48942637 = -787770717;    float yzfgsLOwmyEYQie73104762 = -764135182;    float yzfgsLOwmyEYQie98265721 = -102409961;    float yzfgsLOwmyEYQie9547872 = -218864938;    float yzfgsLOwmyEYQie96052350 = -881714337;    float yzfgsLOwmyEYQie49362463 = -248300581;    float yzfgsLOwmyEYQie92523354 = 71635847;    float yzfgsLOwmyEYQie46349320 = -782358242;    float yzfgsLOwmyEYQie17657143 = -657630772;    float yzfgsLOwmyEYQie46210372 = -190388295;    float yzfgsLOwmyEYQie34768520 = -656701931;    float yzfgsLOwmyEYQie89162783 = -512143501;    float yzfgsLOwmyEYQie83206127 = -931520656;    float yzfgsLOwmyEYQie59581730 = -640460916;    float yzfgsLOwmyEYQie97768106 = -81776862;    float yzfgsLOwmyEYQie43857824 = -145156106;    float yzfgsLOwmyEYQie73173526 = -849996873;    float yzfgsLOwmyEYQie76119873 = -517932611;    float yzfgsLOwmyEYQie14710929 = -437694900;    float yzfgsLOwmyEYQie38533040 = -106062230;    float yzfgsLOwmyEYQie92959817 = -659708179;    float yzfgsLOwmyEYQie87247915 = -640439793;    float yzfgsLOwmyEYQie25533329 = 1951205;    float yzfgsLOwmyEYQie18447977 = -810550049;    float yzfgsLOwmyEYQie1723406 = -29584565;    float yzfgsLOwmyEYQie1984153 = -414287575;    float yzfgsLOwmyEYQie5401672 = -315537468;    float yzfgsLOwmyEYQie59553557 = 98218468;    float yzfgsLOwmyEYQie35960793 = -394294794;    float yzfgsLOwmyEYQie74330635 = -486025010;    float yzfgsLOwmyEYQie51654801 = -570805673;    float yzfgsLOwmyEYQie43262819 = -812447771;    float yzfgsLOwmyEYQie81872044 = -169896802;    float yzfgsLOwmyEYQie13257599 = -777333357;    float yzfgsLOwmyEYQie2026865 = -385699241;    float yzfgsLOwmyEYQie28559978 = -19337779;    float yzfgsLOwmyEYQie85214492 = -68865225;    float yzfgsLOwmyEYQie59387063 = -468470419;    float yzfgsLOwmyEYQie35175015 = -386079256;    float yzfgsLOwmyEYQie51803805 = -737412947;    float yzfgsLOwmyEYQie62388919 = -825462334;    float yzfgsLOwmyEYQie32165340 = -551370868;    float yzfgsLOwmyEYQie56762110 = -521731304;    float yzfgsLOwmyEYQie22312487 = -942129094;    float yzfgsLOwmyEYQie44855628 = -899644495;    float yzfgsLOwmyEYQie26584311 = -596982516;    float yzfgsLOwmyEYQie91390785 = 90884882;    float yzfgsLOwmyEYQie39153961 = -792350520;    float yzfgsLOwmyEYQie40220408 = -366048159;    float yzfgsLOwmyEYQie69396574 = -668570746;    float yzfgsLOwmyEYQie82122774 = -826392787;    float yzfgsLOwmyEYQie38366300 = -142522453;    float yzfgsLOwmyEYQie82692737 = -314513468;    float yzfgsLOwmyEYQie7268597 = -488611702;    float yzfgsLOwmyEYQie34395543 = -753442627;    float yzfgsLOwmyEYQie61694721 = -47330924;    float yzfgsLOwmyEYQie47571433 = -666086387;    float yzfgsLOwmyEYQie79817744 = -291859912;    float yzfgsLOwmyEYQie7824467 = -89280373;    float yzfgsLOwmyEYQie94068198 = -367426763;    float yzfgsLOwmyEYQie43960792 = -932763114;    float yzfgsLOwmyEYQie32969798 = 73417378;    float yzfgsLOwmyEYQie10388527 = -288063448;    float yzfgsLOwmyEYQie43326507 = -71605763;    float yzfgsLOwmyEYQie94555571 = -619582622;    float yzfgsLOwmyEYQie91505701 = -844254160;    float yzfgsLOwmyEYQie7290740 = -242246699;    float yzfgsLOwmyEYQie69948529 = -54187300;    float yzfgsLOwmyEYQie57554866 = -154761675;    float yzfgsLOwmyEYQie69208129 = 37560916;    float yzfgsLOwmyEYQie58643332 = 23709118;    float yzfgsLOwmyEYQie13786464 = -281526455;    float yzfgsLOwmyEYQie40944859 = -31853356;    float yzfgsLOwmyEYQie62907123 = -700281953;    float yzfgsLOwmyEYQie76144121 = -280599897;    float yzfgsLOwmyEYQie60794478 = -8337311;    float yzfgsLOwmyEYQie30485806 = -18708490;    float yzfgsLOwmyEYQie3220843 = -55919702;    float yzfgsLOwmyEYQie73592349 = -910905554;    float yzfgsLOwmyEYQie75139095 = -883472960;     yzfgsLOwmyEYQie1664740 = yzfgsLOwmyEYQie83480213;     yzfgsLOwmyEYQie83480213 = yzfgsLOwmyEYQie68934934;     yzfgsLOwmyEYQie68934934 = yzfgsLOwmyEYQie31227365;     yzfgsLOwmyEYQie31227365 = yzfgsLOwmyEYQie1166269;     yzfgsLOwmyEYQie1166269 = yzfgsLOwmyEYQie54912274;     yzfgsLOwmyEYQie54912274 = yzfgsLOwmyEYQie78514659;     yzfgsLOwmyEYQie78514659 = yzfgsLOwmyEYQie74419252;     yzfgsLOwmyEYQie74419252 = yzfgsLOwmyEYQie68522859;     yzfgsLOwmyEYQie68522859 = yzfgsLOwmyEYQie79624147;     yzfgsLOwmyEYQie79624147 = yzfgsLOwmyEYQie15747094;     yzfgsLOwmyEYQie15747094 = yzfgsLOwmyEYQie74596912;     yzfgsLOwmyEYQie74596912 = yzfgsLOwmyEYQie98735690;     yzfgsLOwmyEYQie98735690 = yzfgsLOwmyEYQie37988514;     yzfgsLOwmyEYQie37988514 = yzfgsLOwmyEYQie13254399;     yzfgsLOwmyEYQie13254399 = yzfgsLOwmyEYQie55296301;     yzfgsLOwmyEYQie55296301 = yzfgsLOwmyEYQie14486173;     yzfgsLOwmyEYQie14486173 = yzfgsLOwmyEYQie97403666;     yzfgsLOwmyEYQie97403666 = yzfgsLOwmyEYQie45801637;     yzfgsLOwmyEYQie45801637 = yzfgsLOwmyEYQie27355361;     yzfgsLOwmyEYQie27355361 = yzfgsLOwmyEYQie48942637;     yzfgsLOwmyEYQie48942637 = yzfgsLOwmyEYQie73104762;     yzfgsLOwmyEYQie73104762 = yzfgsLOwmyEYQie98265721;     yzfgsLOwmyEYQie98265721 = yzfgsLOwmyEYQie9547872;     yzfgsLOwmyEYQie9547872 = yzfgsLOwmyEYQie96052350;     yzfgsLOwmyEYQie96052350 = yzfgsLOwmyEYQie49362463;     yzfgsLOwmyEYQie49362463 = yzfgsLOwmyEYQie92523354;     yzfgsLOwmyEYQie92523354 = yzfgsLOwmyEYQie46349320;     yzfgsLOwmyEYQie46349320 = yzfgsLOwmyEYQie17657143;     yzfgsLOwmyEYQie17657143 = yzfgsLOwmyEYQie46210372;     yzfgsLOwmyEYQie46210372 = yzfgsLOwmyEYQie34768520;     yzfgsLOwmyEYQie34768520 = yzfgsLOwmyEYQie89162783;     yzfgsLOwmyEYQie89162783 = yzfgsLOwmyEYQie83206127;     yzfgsLOwmyEYQie83206127 = yzfgsLOwmyEYQie59581730;     yzfgsLOwmyEYQie59581730 = yzfgsLOwmyEYQie97768106;     yzfgsLOwmyEYQie97768106 = yzfgsLOwmyEYQie43857824;     yzfgsLOwmyEYQie43857824 = yzfgsLOwmyEYQie73173526;     yzfgsLOwmyEYQie73173526 = yzfgsLOwmyEYQie76119873;     yzfgsLOwmyEYQie76119873 = yzfgsLOwmyEYQie14710929;     yzfgsLOwmyEYQie14710929 = yzfgsLOwmyEYQie38533040;     yzfgsLOwmyEYQie38533040 = yzfgsLOwmyEYQie92959817;     yzfgsLOwmyEYQie92959817 = yzfgsLOwmyEYQie87247915;     yzfgsLOwmyEYQie87247915 = yzfgsLOwmyEYQie25533329;     yzfgsLOwmyEYQie25533329 = yzfgsLOwmyEYQie18447977;     yzfgsLOwmyEYQie18447977 = yzfgsLOwmyEYQie1723406;     yzfgsLOwmyEYQie1723406 = yzfgsLOwmyEYQie1984153;     yzfgsLOwmyEYQie1984153 = yzfgsLOwmyEYQie5401672;     yzfgsLOwmyEYQie5401672 = yzfgsLOwmyEYQie59553557;     yzfgsLOwmyEYQie59553557 = yzfgsLOwmyEYQie35960793;     yzfgsLOwmyEYQie35960793 = yzfgsLOwmyEYQie74330635;     yzfgsLOwmyEYQie74330635 = yzfgsLOwmyEYQie51654801;     yzfgsLOwmyEYQie51654801 = yzfgsLOwmyEYQie43262819;     yzfgsLOwmyEYQie43262819 = yzfgsLOwmyEYQie81872044;     yzfgsLOwmyEYQie81872044 = yzfgsLOwmyEYQie13257599;     yzfgsLOwmyEYQie13257599 = yzfgsLOwmyEYQie2026865;     yzfgsLOwmyEYQie2026865 = yzfgsLOwmyEYQie28559978;     yzfgsLOwmyEYQie28559978 = yzfgsLOwmyEYQie85214492;     yzfgsLOwmyEYQie85214492 = yzfgsLOwmyEYQie59387063;     yzfgsLOwmyEYQie59387063 = yzfgsLOwmyEYQie35175015;     yzfgsLOwmyEYQie35175015 = yzfgsLOwmyEYQie51803805;     yzfgsLOwmyEYQie51803805 = yzfgsLOwmyEYQie62388919;     yzfgsLOwmyEYQie62388919 = yzfgsLOwmyEYQie32165340;     yzfgsLOwmyEYQie32165340 = yzfgsLOwmyEYQie56762110;     yzfgsLOwmyEYQie56762110 = yzfgsLOwmyEYQie22312487;     yzfgsLOwmyEYQie22312487 = yzfgsLOwmyEYQie44855628;     yzfgsLOwmyEYQie44855628 = yzfgsLOwmyEYQie26584311;     yzfgsLOwmyEYQie26584311 = yzfgsLOwmyEYQie91390785;     yzfgsLOwmyEYQie91390785 = yzfgsLOwmyEYQie39153961;     yzfgsLOwmyEYQie39153961 = yzfgsLOwmyEYQie40220408;     yzfgsLOwmyEYQie40220408 = yzfgsLOwmyEYQie69396574;     yzfgsLOwmyEYQie69396574 = yzfgsLOwmyEYQie82122774;     yzfgsLOwmyEYQie82122774 = yzfgsLOwmyEYQie38366300;     yzfgsLOwmyEYQie38366300 = yzfgsLOwmyEYQie82692737;     yzfgsLOwmyEYQie82692737 = yzfgsLOwmyEYQie7268597;     yzfgsLOwmyEYQie7268597 = yzfgsLOwmyEYQie34395543;     yzfgsLOwmyEYQie34395543 = yzfgsLOwmyEYQie61694721;     yzfgsLOwmyEYQie61694721 = yzfgsLOwmyEYQie47571433;     yzfgsLOwmyEYQie47571433 = yzfgsLOwmyEYQie79817744;     yzfgsLOwmyEYQie79817744 = yzfgsLOwmyEYQie7824467;     yzfgsLOwmyEYQie7824467 = yzfgsLOwmyEYQie94068198;     yzfgsLOwmyEYQie94068198 = yzfgsLOwmyEYQie43960792;     yzfgsLOwmyEYQie43960792 = yzfgsLOwmyEYQie32969798;     yzfgsLOwmyEYQie32969798 = yzfgsLOwmyEYQie10388527;     yzfgsLOwmyEYQie10388527 = yzfgsLOwmyEYQie43326507;     yzfgsLOwmyEYQie43326507 = yzfgsLOwmyEYQie94555571;     yzfgsLOwmyEYQie94555571 = yzfgsLOwmyEYQie91505701;     yzfgsLOwmyEYQie91505701 = yzfgsLOwmyEYQie7290740;     yzfgsLOwmyEYQie7290740 = yzfgsLOwmyEYQie69948529;     yzfgsLOwmyEYQie69948529 = yzfgsLOwmyEYQie57554866;     yzfgsLOwmyEYQie57554866 = yzfgsLOwmyEYQie69208129;     yzfgsLOwmyEYQie69208129 = yzfgsLOwmyEYQie58643332;     yzfgsLOwmyEYQie58643332 = yzfgsLOwmyEYQie13786464;     yzfgsLOwmyEYQie13786464 = yzfgsLOwmyEYQie40944859;     yzfgsLOwmyEYQie40944859 = yzfgsLOwmyEYQie62907123;     yzfgsLOwmyEYQie62907123 = yzfgsLOwmyEYQie76144121;     yzfgsLOwmyEYQie76144121 = yzfgsLOwmyEYQie60794478;     yzfgsLOwmyEYQie60794478 = yzfgsLOwmyEYQie30485806;     yzfgsLOwmyEYQie30485806 = yzfgsLOwmyEYQie3220843;     yzfgsLOwmyEYQie3220843 = yzfgsLOwmyEYQie73592349;     yzfgsLOwmyEYQie73592349 = yzfgsLOwmyEYQie75139095;     yzfgsLOwmyEYQie75139095 = yzfgsLOwmyEYQie1664740;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void GgukxoDMRrGhVNtG57298048() {     float EzpEpnzjgeKKhqa56319137 = -221000544;    float EzpEpnzjgeKKhqa30477833 = -850839999;    float EzpEpnzjgeKKhqa29667581 = -72719969;    float EzpEpnzjgeKKhqa2662307 = -273896003;    float EzpEpnzjgeKKhqa75617939 = -758148767;    float EzpEpnzjgeKKhqa69086671 = -982977322;    float EzpEpnzjgeKKhqa99615363 = -635621782;    float EzpEpnzjgeKKhqa3594356 = -679930308;    float EzpEpnzjgeKKhqa56297269 = -179370758;    float EzpEpnzjgeKKhqa62256133 = -835147695;    float EzpEpnzjgeKKhqa94358388 = -734482588;    float EzpEpnzjgeKKhqa37361626 = -27059862;    float EzpEpnzjgeKKhqa62553006 = -236351740;    float EzpEpnzjgeKKhqa30954945 = -159928599;    float EzpEpnzjgeKKhqa43478472 = -820848483;    float EzpEpnzjgeKKhqa45063837 = -306549224;    float EzpEpnzjgeKKhqa14317937 = -134054893;    float EzpEpnzjgeKKhqa65000253 = -915772834;    float EzpEpnzjgeKKhqa47022238 = -980485480;    float EzpEpnzjgeKKhqa44989916 = -758516951;    float EzpEpnzjgeKKhqa83117024 = -255853867;    float EzpEpnzjgeKKhqa24357352 = -930579290;    float EzpEpnzjgeKKhqa34735553 = -689153382;    float EzpEpnzjgeKKhqa10976241 = 82094246;    float EzpEpnzjgeKKhqa91804966 = -154627789;    float EzpEpnzjgeKKhqa90603410 = -252009366;    float EzpEpnzjgeKKhqa94513278 = -483208432;    float EzpEpnzjgeKKhqa99598320 = -790410102;    float EzpEpnzjgeKKhqa57554819 = 62124352;    float EzpEpnzjgeKKhqa40645781 = -481146263;    float EzpEpnzjgeKKhqa86474048 = -105574457;    float EzpEpnzjgeKKhqa20154394 = -923640029;    float EzpEpnzjgeKKhqa50862004 = -388038270;    float EzpEpnzjgeKKhqa54237623 = -320581726;    float EzpEpnzjgeKKhqa32272263 = -299640122;    float EzpEpnzjgeKKhqa31184366 = -393151703;    float EzpEpnzjgeKKhqa47878014 = -700423494;    float EzpEpnzjgeKKhqa89097112 = -391861632;    float EzpEpnzjgeKKhqa65480577 = -819643501;    float EzpEpnzjgeKKhqa62519435 = -864370830;    float EzpEpnzjgeKKhqa48919219 = -772258337;    float EzpEpnzjgeKKhqa23922574 = -147971880;    float EzpEpnzjgeKKhqa16533512 = -938554113;    float EzpEpnzjgeKKhqa27442377 = -284229715;    float EzpEpnzjgeKKhqa61707469 = -162102163;    float EzpEpnzjgeKKhqa57317754 = -483077190;    float EzpEpnzjgeKKhqa21875383 = -342594851;    float EzpEpnzjgeKKhqa82980866 = -979433904;    float EzpEpnzjgeKKhqa78303559 = -294024983;    float EzpEpnzjgeKKhqa19172298 = -726044305;    float EzpEpnzjgeKKhqa26768341 = -228021175;    float EzpEpnzjgeKKhqa32644968 = -227513754;    float EzpEpnzjgeKKhqa13600047 = 90187740;    float EzpEpnzjgeKKhqa70673356 = -673219841;    float EzpEpnzjgeKKhqa74181024 = -781607923;    float EzpEpnzjgeKKhqa31961786 = -290421255;    float EzpEpnzjgeKKhqa95742279 = -61686617;    float EzpEpnzjgeKKhqa18691341 = -54814216;    float EzpEpnzjgeKKhqa10857340 = -19268215;    float EzpEpnzjgeKKhqa85014529 = -406139402;    float EzpEpnzjgeKKhqa74573392 = -399768891;    float EzpEpnzjgeKKhqa17044 = -845211680;    float EzpEpnzjgeKKhqa46039536 = -642054661;    float EzpEpnzjgeKKhqa15651489 = -698224496;    float EzpEpnzjgeKKhqa75782084 = -629573239;    float EzpEpnzjgeKKhqa74203995 = -810842560;    float EzpEpnzjgeKKhqa86499621 = -639021593;    float EzpEpnzjgeKKhqa8315383 = -915770015;    float EzpEpnzjgeKKhqa98682681 = -860288478;    float EzpEpnzjgeKKhqa12294106 = -327696781;    float EzpEpnzjgeKKhqa97185822 = -606125730;    float EzpEpnzjgeKKhqa25220825 = -742193261;    float EzpEpnzjgeKKhqa99519675 = 3870667;    float EzpEpnzjgeKKhqa84502802 = -16114650;    float EzpEpnzjgeKKhqa96070697 = -986258615;    float EzpEpnzjgeKKhqa59194450 = -7881988;    float EzpEpnzjgeKKhqa7823840 = -992025177;    float EzpEpnzjgeKKhqa7293177 = -304923668;    float EzpEpnzjgeKKhqa49268771 = -755803592;    float EzpEpnzjgeKKhqa34487212 = -671550600;    float EzpEpnzjgeKKhqa68728027 = -909414515;    float EzpEpnzjgeKKhqa11532413 = -503774528;    float EzpEpnzjgeKKhqa21294761 = -396385120;    float EzpEpnzjgeKKhqa38382521 = -211831344;    float EzpEpnzjgeKKhqa13877441 = -153125088;    float EzpEpnzjgeKKhqa53829080 = -878060703;    float EzpEpnzjgeKKhqa6554347 = -913827770;    float EzpEpnzjgeKKhqa80188648 = -714818429;    float EzpEpnzjgeKKhqa80056599 = -538973804;    float EzpEpnzjgeKKhqa310478 = 90781133;    float EzpEpnzjgeKKhqa35442086 = -231465086;    float EzpEpnzjgeKKhqa29186674 = -545609278;    float EzpEpnzjgeKKhqa78239772 = -272593418;    float EzpEpnzjgeKKhqa80466048 = -313504100;    float EzpEpnzjgeKKhqa87946043 = -364601939;    float EzpEpnzjgeKKhqa48902176 = -927046657;    float EzpEpnzjgeKKhqa77883037 = -505917220;    float EzpEpnzjgeKKhqa882024 = -140329618;    float EzpEpnzjgeKKhqa51660292 = -654656476;    float EzpEpnzjgeKKhqa87503473 = -221000544;     EzpEpnzjgeKKhqa56319137 = EzpEpnzjgeKKhqa30477833;     EzpEpnzjgeKKhqa30477833 = EzpEpnzjgeKKhqa29667581;     EzpEpnzjgeKKhqa29667581 = EzpEpnzjgeKKhqa2662307;     EzpEpnzjgeKKhqa2662307 = EzpEpnzjgeKKhqa75617939;     EzpEpnzjgeKKhqa75617939 = EzpEpnzjgeKKhqa69086671;     EzpEpnzjgeKKhqa69086671 = EzpEpnzjgeKKhqa99615363;     EzpEpnzjgeKKhqa99615363 = EzpEpnzjgeKKhqa3594356;     EzpEpnzjgeKKhqa3594356 = EzpEpnzjgeKKhqa56297269;     EzpEpnzjgeKKhqa56297269 = EzpEpnzjgeKKhqa62256133;     EzpEpnzjgeKKhqa62256133 = EzpEpnzjgeKKhqa94358388;     EzpEpnzjgeKKhqa94358388 = EzpEpnzjgeKKhqa37361626;     EzpEpnzjgeKKhqa37361626 = EzpEpnzjgeKKhqa62553006;     EzpEpnzjgeKKhqa62553006 = EzpEpnzjgeKKhqa30954945;     EzpEpnzjgeKKhqa30954945 = EzpEpnzjgeKKhqa43478472;     EzpEpnzjgeKKhqa43478472 = EzpEpnzjgeKKhqa45063837;     EzpEpnzjgeKKhqa45063837 = EzpEpnzjgeKKhqa14317937;     EzpEpnzjgeKKhqa14317937 = EzpEpnzjgeKKhqa65000253;     EzpEpnzjgeKKhqa65000253 = EzpEpnzjgeKKhqa47022238;     EzpEpnzjgeKKhqa47022238 = EzpEpnzjgeKKhqa44989916;     EzpEpnzjgeKKhqa44989916 = EzpEpnzjgeKKhqa83117024;     EzpEpnzjgeKKhqa83117024 = EzpEpnzjgeKKhqa24357352;     EzpEpnzjgeKKhqa24357352 = EzpEpnzjgeKKhqa34735553;     EzpEpnzjgeKKhqa34735553 = EzpEpnzjgeKKhqa10976241;     EzpEpnzjgeKKhqa10976241 = EzpEpnzjgeKKhqa91804966;     EzpEpnzjgeKKhqa91804966 = EzpEpnzjgeKKhqa90603410;     EzpEpnzjgeKKhqa90603410 = EzpEpnzjgeKKhqa94513278;     EzpEpnzjgeKKhqa94513278 = EzpEpnzjgeKKhqa99598320;     EzpEpnzjgeKKhqa99598320 = EzpEpnzjgeKKhqa57554819;     EzpEpnzjgeKKhqa57554819 = EzpEpnzjgeKKhqa40645781;     EzpEpnzjgeKKhqa40645781 = EzpEpnzjgeKKhqa86474048;     EzpEpnzjgeKKhqa86474048 = EzpEpnzjgeKKhqa20154394;     EzpEpnzjgeKKhqa20154394 = EzpEpnzjgeKKhqa50862004;     EzpEpnzjgeKKhqa50862004 = EzpEpnzjgeKKhqa54237623;     EzpEpnzjgeKKhqa54237623 = EzpEpnzjgeKKhqa32272263;     EzpEpnzjgeKKhqa32272263 = EzpEpnzjgeKKhqa31184366;     EzpEpnzjgeKKhqa31184366 = EzpEpnzjgeKKhqa47878014;     EzpEpnzjgeKKhqa47878014 = EzpEpnzjgeKKhqa89097112;     EzpEpnzjgeKKhqa89097112 = EzpEpnzjgeKKhqa65480577;     EzpEpnzjgeKKhqa65480577 = EzpEpnzjgeKKhqa62519435;     EzpEpnzjgeKKhqa62519435 = EzpEpnzjgeKKhqa48919219;     EzpEpnzjgeKKhqa48919219 = EzpEpnzjgeKKhqa23922574;     EzpEpnzjgeKKhqa23922574 = EzpEpnzjgeKKhqa16533512;     EzpEpnzjgeKKhqa16533512 = EzpEpnzjgeKKhqa27442377;     EzpEpnzjgeKKhqa27442377 = EzpEpnzjgeKKhqa61707469;     EzpEpnzjgeKKhqa61707469 = EzpEpnzjgeKKhqa57317754;     EzpEpnzjgeKKhqa57317754 = EzpEpnzjgeKKhqa21875383;     EzpEpnzjgeKKhqa21875383 = EzpEpnzjgeKKhqa82980866;     EzpEpnzjgeKKhqa82980866 = EzpEpnzjgeKKhqa78303559;     EzpEpnzjgeKKhqa78303559 = EzpEpnzjgeKKhqa19172298;     EzpEpnzjgeKKhqa19172298 = EzpEpnzjgeKKhqa26768341;     EzpEpnzjgeKKhqa26768341 = EzpEpnzjgeKKhqa32644968;     EzpEpnzjgeKKhqa32644968 = EzpEpnzjgeKKhqa13600047;     EzpEpnzjgeKKhqa13600047 = EzpEpnzjgeKKhqa70673356;     EzpEpnzjgeKKhqa70673356 = EzpEpnzjgeKKhqa74181024;     EzpEpnzjgeKKhqa74181024 = EzpEpnzjgeKKhqa31961786;     EzpEpnzjgeKKhqa31961786 = EzpEpnzjgeKKhqa95742279;     EzpEpnzjgeKKhqa95742279 = EzpEpnzjgeKKhqa18691341;     EzpEpnzjgeKKhqa18691341 = EzpEpnzjgeKKhqa10857340;     EzpEpnzjgeKKhqa10857340 = EzpEpnzjgeKKhqa85014529;     EzpEpnzjgeKKhqa85014529 = EzpEpnzjgeKKhqa74573392;     EzpEpnzjgeKKhqa74573392 = EzpEpnzjgeKKhqa17044;     EzpEpnzjgeKKhqa17044 = EzpEpnzjgeKKhqa46039536;     EzpEpnzjgeKKhqa46039536 = EzpEpnzjgeKKhqa15651489;     EzpEpnzjgeKKhqa15651489 = EzpEpnzjgeKKhqa75782084;     EzpEpnzjgeKKhqa75782084 = EzpEpnzjgeKKhqa74203995;     EzpEpnzjgeKKhqa74203995 = EzpEpnzjgeKKhqa86499621;     EzpEpnzjgeKKhqa86499621 = EzpEpnzjgeKKhqa8315383;     EzpEpnzjgeKKhqa8315383 = EzpEpnzjgeKKhqa98682681;     EzpEpnzjgeKKhqa98682681 = EzpEpnzjgeKKhqa12294106;     EzpEpnzjgeKKhqa12294106 = EzpEpnzjgeKKhqa97185822;     EzpEpnzjgeKKhqa97185822 = EzpEpnzjgeKKhqa25220825;     EzpEpnzjgeKKhqa25220825 = EzpEpnzjgeKKhqa99519675;     EzpEpnzjgeKKhqa99519675 = EzpEpnzjgeKKhqa84502802;     EzpEpnzjgeKKhqa84502802 = EzpEpnzjgeKKhqa96070697;     EzpEpnzjgeKKhqa96070697 = EzpEpnzjgeKKhqa59194450;     EzpEpnzjgeKKhqa59194450 = EzpEpnzjgeKKhqa7823840;     EzpEpnzjgeKKhqa7823840 = EzpEpnzjgeKKhqa7293177;     EzpEpnzjgeKKhqa7293177 = EzpEpnzjgeKKhqa49268771;     EzpEpnzjgeKKhqa49268771 = EzpEpnzjgeKKhqa34487212;     EzpEpnzjgeKKhqa34487212 = EzpEpnzjgeKKhqa68728027;     EzpEpnzjgeKKhqa68728027 = EzpEpnzjgeKKhqa11532413;     EzpEpnzjgeKKhqa11532413 = EzpEpnzjgeKKhqa21294761;     EzpEpnzjgeKKhqa21294761 = EzpEpnzjgeKKhqa38382521;     EzpEpnzjgeKKhqa38382521 = EzpEpnzjgeKKhqa13877441;     EzpEpnzjgeKKhqa13877441 = EzpEpnzjgeKKhqa53829080;     EzpEpnzjgeKKhqa53829080 = EzpEpnzjgeKKhqa6554347;     EzpEpnzjgeKKhqa6554347 = EzpEpnzjgeKKhqa80188648;     EzpEpnzjgeKKhqa80188648 = EzpEpnzjgeKKhqa80056599;     EzpEpnzjgeKKhqa80056599 = EzpEpnzjgeKKhqa310478;     EzpEpnzjgeKKhqa310478 = EzpEpnzjgeKKhqa35442086;     EzpEpnzjgeKKhqa35442086 = EzpEpnzjgeKKhqa29186674;     EzpEpnzjgeKKhqa29186674 = EzpEpnzjgeKKhqa78239772;     EzpEpnzjgeKKhqa78239772 = EzpEpnzjgeKKhqa80466048;     EzpEpnzjgeKKhqa80466048 = EzpEpnzjgeKKhqa87946043;     EzpEpnzjgeKKhqa87946043 = EzpEpnzjgeKKhqa48902176;     EzpEpnzjgeKKhqa48902176 = EzpEpnzjgeKKhqa77883037;     EzpEpnzjgeKKhqa77883037 = EzpEpnzjgeKKhqa882024;     EzpEpnzjgeKKhqa882024 = EzpEpnzjgeKKhqa51660292;     EzpEpnzjgeKKhqa51660292 = EzpEpnzjgeKKhqa87503473;     EzpEpnzjgeKKhqa87503473 = EzpEpnzjgeKKhqa56319137;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void vXcctiUEyeKRobhd2993741() {     float qgBAeBddwzLlNdR75508095 = -64775370;    float qgBAeBddwzLlNdR2775690 = -492448331;    float qgBAeBddwzLlNdR54326963 = -749566121;    float qgBAeBddwzLlNdR86953753 = -279388173;    float qgBAeBddwzLlNdR2624443 = -793340482;    float qgBAeBddwzLlNdR81843628 = -549213075;    float qgBAeBddwzLlNdR78605996 = -797325187;    float qgBAeBddwzLlNdR39851949 = -140441717;    float qgBAeBddwzLlNdR25294239 = -551538792;    float qgBAeBddwzLlNdR26624920 = 13931162;    float qgBAeBddwzLlNdR55108554 = -417303503;    float qgBAeBddwzLlNdR13849868 = -84841542;    float qgBAeBddwzLlNdR19988589 = -609538014;    float qgBAeBddwzLlNdR24624733 = -140821819;    float qgBAeBddwzLlNdR60680137 = -187257951;    float qgBAeBddwzLlNdR25854619 = -633692832;    float qgBAeBddwzLlNdR14166524 = 99705262;    float qgBAeBddwzLlNdR55837181 = -202980855;    float qgBAeBddwzLlNdR68120778 = -467715873;    float qgBAeBddwzLlNdR40861016 = -189346483;    float qgBAeBddwzLlNdR83873973 = -877128703;    float qgBAeBddwzLlNdR50484681 = -420378987;    float qgBAeBddwzLlNdR37558401 = -997222461;    float qgBAeBddwzLlNdR62261772 = -87042489;    float qgBAeBddwzLlNdR7982321 = -930249896;    float qgBAeBddwzLlNdR37720263 = -475347272;    float qgBAeBddwzLlNdR6304211 = -432568282;    float qgBAeBddwzLlNdR47522420 = -137656777;    float qgBAeBddwzLlNdR63462728 = -390096036;    float qgBAeBddwzLlNdR5637650 = -412828434;    float qgBAeBddwzLlNdR53009024 = -489559730;    float qgBAeBddwzLlNdR98046843 = -853986905;    float qgBAeBddwzLlNdR91752292 = -998904123;    float qgBAeBddwzLlNdR19427927 = -912690454;    float qgBAeBddwzLlNdR83326004 = -825717055;    float qgBAeBddwzLlNdR9778254 = -506347740;    float qgBAeBddwzLlNdR85112053 = -785807452;    float qgBAeBddwzLlNdR20776628 = -278397751;    float qgBAeBddwzLlNdR21173262 = -63397243;    float qgBAeBddwzLlNdR94107190 = -666848569;    float qgBAeBddwzLlNdR59282679 = -103553479;    float qgBAeBddwzLlNdR56929766 = -254750758;    float qgBAeBddwzLlNdR78433676 = -685008899;    float qgBAeBddwzLlNdR55537336 = -910541414;    float qgBAeBddwzLlNdR5693126 = -941368001;    float qgBAeBddwzLlNdR7117996 = -324987843;    float qgBAeBddwzLlNdR46701724 = -366946497;    float qgBAeBddwzLlNdR64065444 = -299321039;    float qgBAeBddwzLlNdR16412050 = -93782153;    float qgBAeBddwzLlNdR59529794 = -62061669;    float qgBAeBddwzLlNdR34370526 = -689515127;    float qgBAeBddwzLlNdR53088903 = -911073139;    float qgBAeBddwzLlNdR52155250 = -885736171;    float qgBAeBddwzLlNdR22347538 = -909517677;    float qgBAeBddwzLlNdR19119768 = -587925736;    float qgBAeBddwzLlNdR25023414 = -644396383;    float qgBAeBddwzLlNdR65217289 = -495225871;    float qgBAeBddwzLlNdR92065190 = -562523633;    float qgBAeBddwzLlNdR78971432 = -349138278;    float qgBAeBddwzLlNdR64904180 = -217993211;    float qgBAeBddwzLlNdR75539418 = -16644793;    float qgBAeBddwzLlNdR31083576 = -559668411;    float qgBAeBddwzLlNdR76389220 = -750345682;    float qgBAeBddwzLlNdR19656590 = -38710358;    float qgBAeBddwzLlNdR73615895 = -496509108;    float qgBAeBddwzLlNdR57061711 = -563316599;    float qgBAeBddwzLlNdR22097575 = -85937420;    float qgBAeBddwzLlNdR560663 = -696847560;    float qgBAeBddwzLlNdR41298728 = -315104764;    float qgBAeBddwzLlNdR50901884 = -680910212;    float qgBAeBddwzLlNdR40742565 = -847885380;    float qgBAeBddwzLlNdR93389896 = -621896988;    float qgBAeBddwzLlNdR34663920 = -39583613;    float qgBAeBddwzLlNdR74013587 = -800867304;    float qgBAeBddwzLlNdR81578336 = 14206996;    float qgBAeBddwzLlNdR26944207 = -522377946;    float qgBAeBddwzLlNdR72051005 = -735370088;    float qgBAeBddwzLlNdR82021065 = 13318953;    float qgBAeBddwzLlNdR56568646 = -145674488;    float qgBAeBddwzLlNdR864326 = -505262054;    float qgBAeBddwzLlNdR91018539 = -8400775;    float qgBAeBddwzLlNdR42238766 = -33247244;    float qgBAeBddwzLlNdR31110371 = 56125376;    float qgBAeBddwzLlNdR3932935 = -228034367;    float qgBAeBddwzLlNdR71267123 = -723313308;    float qgBAeBddwzLlNdR99920121 = -578486592;    float qgBAeBddwzLlNdR45891593 = -968250734;    float qgBAeBddwzLlNdR69404755 = 10613554;    float qgBAeBddwzLlNdR308160 = -224764719;    float qgBAeBddwzLlNdR58302591 = -81320672;    float qgBAeBddwzLlNdR44560965 = 88878131;    float qgBAeBddwzLlNdR93046862 = -123283820;    float qgBAeBddwzLlNdR41805195 = -929259473;    float qgBAeBddwzLlNdR56269081 = -845404032;    float qgBAeBddwzLlNdR18567773 = -550203777;    float qgBAeBddwzLlNdR28199103 = -543885069;    float qgBAeBddwzLlNdR80540546 = -504405076;    float qgBAeBddwzLlNdR58777087 = -546298541;    float qgBAeBddwzLlNdR81921440 = -314032306;    float qgBAeBddwzLlNdR48631415 = -64775370;     qgBAeBddwzLlNdR75508095 = qgBAeBddwzLlNdR2775690;     qgBAeBddwzLlNdR2775690 = qgBAeBddwzLlNdR54326963;     qgBAeBddwzLlNdR54326963 = qgBAeBddwzLlNdR86953753;     qgBAeBddwzLlNdR86953753 = qgBAeBddwzLlNdR2624443;     qgBAeBddwzLlNdR2624443 = qgBAeBddwzLlNdR81843628;     qgBAeBddwzLlNdR81843628 = qgBAeBddwzLlNdR78605996;     qgBAeBddwzLlNdR78605996 = qgBAeBddwzLlNdR39851949;     qgBAeBddwzLlNdR39851949 = qgBAeBddwzLlNdR25294239;     qgBAeBddwzLlNdR25294239 = qgBAeBddwzLlNdR26624920;     qgBAeBddwzLlNdR26624920 = qgBAeBddwzLlNdR55108554;     qgBAeBddwzLlNdR55108554 = qgBAeBddwzLlNdR13849868;     qgBAeBddwzLlNdR13849868 = qgBAeBddwzLlNdR19988589;     qgBAeBddwzLlNdR19988589 = qgBAeBddwzLlNdR24624733;     qgBAeBddwzLlNdR24624733 = qgBAeBddwzLlNdR60680137;     qgBAeBddwzLlNdR60680137 = qgBAeBddwzLlNdR25854619;     qgBAeBddwzLlNdR25854619 = qgBAeBddwzLlNdR14166524;     qgBAeBddwzLlNdR14166524 = qgBAeBddwzLlNdR55837181;     qgBAeBddwzLlNdR55837181 = qgBAeBddwzLlNdR68120778;     qgBAeBddwzLlNdR68120778 = qgBAeBddwzLlNdR40861016;     qgBAeBddwzLlNdR40861016 = qgBAeBddwzLlNdR83873973;     qgBAeBddwzLlNdR83873973 = qgBAeBddwzLlNdR50484681;     qgBAeBddwzLlNdR50484681 = qgBAeBddwzLlNdR37558401;     qgBAeBddwzLlNdR37558401 = qgBAeBddwzLlNdR62261772;     qgBAeBddwzLlNdR62261772 = qgBAeBddwzLlNdR7982321;     qgBAeBddwzLlNdR7982321 = qgBAeBddwzLlNdR37720263;     qgBAeBddwzLlNdR37720263 = qgBAeBddwzLlNdR6304211;     qgBAeBddwzLlNdR6304211 = qgBAeBddwzLlNdR47522420;     qgBAeBddwzLlNdR47522420 = qgBAeBddwzLlNdR63462728;     qgBAeBddwzLlNdR63462728 = qgBAeBddwzLlNdR5637650;     qgBAeBddwzLlNdR5637650 = qgBAeBddwzLlNdR53009024;     qgBAeBddwzLlNdR53009024 = qgBAeBddwzLlNdR98046843;     qgBAeBddwzLlNdR98046843 = qgBAeBddwzLlNdR91752292;     qgBAeBddwzLlNdR91752292 = qgBAeBddwzLlNdR19427927;     qgBAeBddwzLlNdR19427927 = qgBAeBddwzLlNdR83326004;     qgBAeBddwzLlNdR83326004 = qgBAeBddwzLlNdR9778254;     qgBAeBddwzLlNdR9778254 = qgBAeBddwzLlNdR85112053;     qgBAeBddwzLlNdR85112053 = qgBAeBddwzLlNdR20776628;     qgBAeBddwzLlNdR20776628 = qgBAeBddwzLlNdR21173262;     qgBAeBddwzLlNdR21173262 = qgBAeBddwzLlNdR94107190;     qgBAeBddwzLlNdR94107190 = qgBAeBddwzLlNdR59282679;     qgBAeBddwzLlNdR59282679 = qgBAeBddwzLlNdR56929766;     qgBAeBddwzLlNdR56929766 = qgBAeBddwzLlNdR78433676;     qgBAeBddwzLlNdR78433676 = qgBAeBddwzLlNdR55537336;     qgBAeBddwzLlNdR55537336 = qgBAeBddwzLlNdR5693126;     qgBAeBddwzLlNdR5693126 = qgBAeBddwzLlNdR7117996;     qgBAeBddwzLlNdR7117996 = qgBAeBddwzLlNdR46701724;     qgBAeBddwzLlNdR46701724 = qgBAeBddwzLlNdR64065444;     qgBAeBddwzLlNdR64065444 = qgBAeBddwzLlNdR16412050;     qgBAeBddwzLlNdR16412050 = qgBAeBddwzLlNdR59529794;     qgBAeBddwzLlNdR59529794 = qgBAeBddwzLlNdR34370526;     qgBAeBddwzLlNdR34370526 = qgBAeBddwzLlNdR53088903;     qgBAeBddwzLlNdR53088903 = qgBAeBddwzLlNdR52155250;     qgBAeBddwzLlNdR52155250 = qgBAeBddwzLlNdR22347538;     qgBAeBddwzLlNdR22347538 = qgBAeBddwzLlNdR19119768;     qgBAeBddwzLlNdR19119768 = qgBAeBddwzLlNdR25023414;     qgBAeBddwzLlNdR25023414 = qgBAeBddwzLlNdR65217289;     qgBAeBddwzLlNdR65217289 = qgBAeBddwzLlNdR92065190;     qgBAeBddwzLlNdR92065190 = qgBAeBddwzLlNdR78971432;     qgBAeBddwzLlNdR78971432 = qgBAeBddwzLlNdR64904180;     qgBAeBddwzLlNdR64904180 = qgBAeBddwzLlNdR75539418;     qgBAeBddwzLlNdR75539418 = qgBAeBddwzLlNdR31083576;     qgBAeBddwzLlNdR31083576 = qgBAeBddwzLlNdR76389220;     qgBAeBddwzLlNdR76389220 = qgBAeBddwzLlNdR19656590;     qgBAeBddwzLlNdR19656590 = qgBAeBddwzLlNdR73615895;     qgBAeBddwzLlNdR73615895 = qgBAeBddwzLlNdR57061711;     qgBAeBddwzLlNdR57061711 = qgBAeBddwzLlNdR22097575;     qgBAeBddwzLlNdR22097575 = qgBAeBddwzLlNdR560663;     qgBAeBddwzLlNdR560663 = qgBAeBddwzLlNdR41298728;     qgBAeBddwzLlNdR41298728 = qgBAeBddwzLlNdR50901884;     qgBAeBddwzLlNdR50901884 = qgBAeBddwzLlNdR40742565;     qgBAeBddwzLlNdR40742565 = qgBAeBddwzLlNdR93389896;     qgBAeBddwzLlNdR93389896 = qgBAeBddwzLlNdR34663920;     qgBAeBddwzLlNdR34663920 = qgBAeBddwzLlNdR74013587;     qgBAeBddwzLlNdR74013587 = qgBAeBddwzLlNdR81578336;     qgBAeBddwzLlNdR81578336 = qgBAeBddwzLlNdR26944207;     qgBAeBddwzLlNdR26944207 = qgBAeBddwzLlNdR72051005;     qgBAeBddwzLlNdR72051005 = qgBAeBddwzLlNdR82021065;     qgBAeBddwzLlNdR82021065 = qgBAeBddwzLlNdR56568646;     qgBAeBddwzLlNdR56568646 = qgBAeBddwzLlNdR864326;     qgBAeBddwzLlNdR864326 = qgBAeBddwzLlNdR91018539;     qgBAeBddwzLlNdR91018539 = qgBAeBddwzLlNdR42238766;     qgBAeBddwzLlNdR42238766 = qgBAeBddwzLlNdR31110371;     qgBAeBddwzLlNdR31110371 = qgBAeBddwzLlNdR3932935;     qgBAeBddwzLlNdR3932935 = qgBAeBddwzLlNdR71267123;     qgBAeBddwzLlNdR71267123 = qgBAeBddwzLlNdR99920121;     qgBAeBddwzLlNdR99920121 = qgBAeBddwzLlNdR45891593;     qgBAeBddwzLlNdR45891593 = qgBAeBddwzLlNdR69404755;     qgBAeBddwzLlNdR69404755 = qgBAeBddwzLlNdR308160;     qgBAeBddwzLlNdR308160 = qgBAeBddwzLlNdR58302591;     qgBAeBddwzLlNdR58302591 = qgBAeBddwzLlNdR44560965;     qgBAeBddwzLlNdR44560965 = qgBAeBddwzLlNdR93046862;     qgBAeBddwzLlNdR93046862 = qgBAeBddwzLlNdR41805195;     qgBAeBddwzLlNdR41805195 = qgBAeBddwzLlNdR56269081;     qgBAeBddwzLlNdR56269081 = qgBAeBddwzLlNdR18567773;     qgBAeBddwzLlNdR18567773 = qgBAeBddwzLlNdR28199103;     qgBAeBddwzLlNdR28199103 = qgBAeBddwzLlNdR80540546;     qgBAeBddwzLlNdR80540546 = qgBAeBddwzLlNdR58777087;     qgBAeBddwzLlNdR58777087 = qgBAeBddwzLlNdR81921440;     qgBAeBddwzLlNdR81921440 = qgBAeBddwzLlNdR48631415;     qgBAeBddwzLlNdR48631415 = qgBAeBddwzLlNdR75508095;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void uZWlyJNHORafkFZe9652486() {     float RpRaFZCoOoxikMI75501623 = 62700489;    float RpRaFZCoOoxikMI41348006 = -535043457;    float RpRaFZCoOoxikMI55640175 = -706694735;    float RpRaFZCoOoxikMI72033614 = -99929004;    float RpRaFZCoOoxikMI11464545 = -161695804;    float RpRaFZCoOoxikMI16649061 = -248645887;    float RpRaFZCoOoxikMI47153333 = -501861654;    float RpRaFZCoOoxikMI9066000 = -944619035;    float RpRaFZCoOoxikMI8034404 = -265184862;    float RpRaFZCoOoxikMI23264434 = -722875709;    float RpRaFZCoOoxikMI75429159 = -376775575;    float RpRaFZCoOoxikMI28141545 = -920319905;    float RpRaFZCoOoxikMI51841503 = -631196488;    float RpRaFZCoOoxikMI78468952 = -485974870;    float RpRaFZCoOoxikMI5852247 = -149970803;    float RpRaFZCoOoxikMI419166 = -966488482;    float RpRaFZCoOoxikMI70019283 = -819909308;    float RpRaFZCoOoxikMI78837714 = -600052477;    float RpRaFZCoOoxikMI88549048 = 14301861;    float RpRaFZCoOoxikMI12934779 = -801370934;    float RpRaFZCoOoxikMI63543397 = -587595077;    float RpRaFZCoOoxikMI28780947 = -930370870;    float RpRaFZCoOoxikMI74436797 = -356180303;    float RpRaFZCoOoxikMI5911881 = 32036989;    float RpRaFZCoOoxikMI36425011 = -256943749;    float RpRaFZCoOoxikMI54294340 = -383553200;    float RpRaFZCoOoxikMI76525792 = -733367995;    float RpRaFZCoOoxikMI82925945 = -746623765;    float RpRaFZCoOoxikMI76141175 = 63833650;    float RpRaFZCoOoxikMI81727519 = -88259808;    float RpRaFZCoOoxikMI86421703 = -876652964;    float RpRaFZCoOoxikMI82770700 = -145808666;    float RpRaFZCoOoxikMI77204716 = -100848339;    float RpRaFZCoOoxikMI19710764 = -789372187;    float RpRaFZCoOoxikMI65684042 = 14969020;    float RpRaFZCoOoxikMI52606443 = -586113486;    float RpRaFZCoOoxikMI97053420 = -885460831;    float RpRaFZCoOoxikMI90694306 = -273660430;    float RpRaFZCoOoxikMI56486858 = -776078659;    float RpRaFZCoOoxikMI65980083 = -276280255;    float RpRaFZCoOoxikMI47138347 = -191497378;    float RpRaFZCoOoxikMI42627427 = -448542840;    float RpRaFZCoOoxikMI38717037 = -144139153;    float RpRaFZCoOoxikMI14769234 = -467505857;    float RpRaFZCoOoxikMI89711178 = -314627403;    float RpRaFZCoOoxikMI47545964 = -137032514;    float RpRaFZCoOoxikMI74399516 = -848227119;    float RpRaFZCoOoxikMI39449025 = -908082395;    float RpRaFZCoOoxikMI35070439 = -757546015;    float RpRaFZCoOoxikMI79575217 = -749086556;    float RpRaFZCoOoxikMI47629896 = -268070134;    float RpRaFZCoOoxikMI67636160 = -193218887;    float RpRaFZCoOoxikMI27603598 = -777790179;    float RpRaFZCoOoxikMI32597808 = -341437528;    float RpRaFZCoOoxikMI46109088 = -344825013;    float RpRaFZCoOoxikMI46720677 = -6928642;    float RpRaFZCoOoxikMI66911209 = -78863154;    float RpRaFZCoOoxikMI49728294 = -638731724;    float RpRaFZCoOoxikMI35608604 = -842985255;    float RpRaFZCoOoxikMI57170205 = -778142604;    float RpRaFZCoOoxikMI40123269 = -515277892;    float RpRaFZCoOoxikMI64227387 = -755237890;    float RpRaFZCoOoxikMI32924824 = -908452685;    float RpRaFZCoOoxikMI26306884 = -76925054;    float RpRaFZCoOoxikMI36842731 = -846222745;    float RpRaFZCoOoxikMI92658459 = -130966910;    float RpRaFZCoOoxikMI50936829 = -719471567;    float RpRaFZCoOoxikMI32130740 = -841824302;    float RpRaFZCoOoxikMI12784911 = -400943891;    float RpRaFZCoOoxikMI53245803 = -563857318;    float RpRaFZCoOoxikMI3365745 = 18972348;    float RpRaFZCoOoxikMI79324976 = -446248878;    float RpRaFZCoOoxikMI22350857 = -823973818;    float RpRaFZCoOoxikMI22568965 = -709417885;    float RpRaFZCoOoxikMI65796432 = -509873557;    float RpRaFZCoOoxikMI20915971 = -39052237;    float RpRaFZCoOoxikMI90063910 = -686231717;    float RpRaFZCoOoxikMI59667564 = -888674446;    float RpRaFZCoOoxikMI16200703 = -653335609;    float RpRaFZCoOoxikMI88879046 = -19911236;    float RpRaFZCoOoxikMI79894823 = -535326082;    float RpRaFZCoOoxikMI37076767 = -825285601;    float RpRaFZCoOoxikMI47855506 = -989077751;    float RpRaFZCoOoxikMI96565958 = -187079795;    float RpRaFZCoOoxikMI34097624 = -820189674;    float RpRaFZCoOoxikMI18785543 = -583434078;    float RpRaFZCoOoxikMI55167102 = -368018487;    float RpRaFZCoOoxikMI44606908 = -759410811;    float RpRaFZCoOoxikMI73601676 = -344547174;    float RpRaFZCoOoxikMI18963366 = -978102339;    float RpRaFZCoOoxikMI85695234 = -407250332;    float RpRaFZCoOoxikMI47325127 = -146729107;    float RpRaFZCoOoxikMI55085703 = -430675176;    float RpRaFZCoOoxikMI99316653 = -997936055;    float RpRaFZCoOoxikMI25856815 = -761002364;    float RpRaFZCoOoxikMI82910960 = -436259489;    float RpRaFZCoOoxikMI9702604 = -540090156;    float RpRaFZCoOoxikMI12410153 = 32785901;    float RpRaFZCoOoxikMI77926503 = -621283113;    float RpRaFZCoOoxikMI97052719 = 62700489;     RpRaFZCoOoxikMI75501623 = RpRaFZCoOoxikMI41348006;     RpRaFZCoOoxikMI41348006 = RpRaFZCoOoxikMI55640175;     RpRaFZCoOoxikMI55640175 = RpRaFZCoOoxikMI72033614;     RpRaFZCoOoxikMI72033614 = RpRaFZCoOoxikMI11464545;     RpRaFZCoOoxikMI11464545 = RpRaFZCoOoxikMI16649061;     RpRaFZCoOoxikMI16649061 = RpRaFZCoOoxikMI47153333;     RpRaFZCoOoxikMI47153333 = RpRaFZCoOoxikMI9066000;     RpRaFZCoOoxikMI9066000 = RpRaFZCoOoxikMI8034404;     RpRaFZCoOoxikMI8034404 = RpRaFZCoOoxikMI23264434;     RpRaFZCoOoxikMI23264434 = RpRaFZCoOoxikMI75429159;     RpRaFZCoOoxikMI75429159 = RpRaFZCoOoxikMI28141545;     RpRaFZCoOoxikMI28141545 = RpRaFZCoOoxikMI51841503;     RpRaFZCoOoxikMI51841503 = RpRaFZCoOoxikMI78468952;     RpRaFZCoOoxikMI78468952 = RpRaFZCoOoxikMI5852247;     RpRaFZCoOoxikMI5852247 = RpRaFZCoOoxikMI419166;     RpRaFZCoOoxikMI419166 = RpRaFZCoOoxikMI70019283;     RpRaFZCoOoxikMI70019283 = RpRaFZCoOoxikMI78837714;     RpRaFZCoOoxikMI78837714 = RpRaFZCoOoxikMI88549048;     RpRaFZCoOoxikMI88549048 = RpRaFZCoOoxikMI12934779;     RpRaFZCoOoxikMI12934779 = RpRaFZCoOoxikMI63543397;     RpRaFZCoOoxikMI63543397 = RpRaFZCoOoxikMI28780947;     RpRaFZCoOoxikMI28780947 = RpRaFZCoOoxikMI74436797;     RpRaFZCoOoxikMI74436797 = RpRaFZCoOoxikMI5911881;     RpRaFZCoOoxikMI5911881 = RpRaFZCoOoxikMI36425011;     RpRaFZCoOoxikMI36425011 = RpRaFZCoOoxikMI54294340;     RpRaFZCoOoxikMI54294340 = RpRaFZCoOoxikMI76525792;     RpRaFZCoOoxikMI76525792 = RpRaFZCoOoxikMI82925945;     RpRaFZCoOoxikMI82925945 = RpRaFZCoOoxikMI76141175;     RpRaFZCoOoxikMI76141175 = RpRaFZCoOoxikMI81727519;     RpRaFZCoOoxikMI81727519 = RpRaFZCoOoxikMI86421703;     RpRaFZCoOoxikMI86421703 = RpRaFZCoOoxikMI82770700;     RpRaFZCoOoxikMI82770700 = RpRaFZCoOoxikMI77204716;     RpRaFZCoOoxikMI77204716 = RpRaFZCoOoxikMI19710764;     RpRaFZCoOoxikMI19710764 = RpRaFZCoOoxikMI65684042;     RpRaFZCoOoxikMI65684042 = RpRaFZCoOoxikMI52606443;     RpRaFZCoOoxikMI52606443 = RpRaFZCoOoxikMI97053420;     RpRaFZCoOoxikMI97053420 = RpRaFZCoOoxikMI90694306;     RpRaFZCoOoxikMI90694306 = RpRaFZCoOoxikMI56486858;     RpRaFZCoOoxikMI56486858 = RpRaFZCoOoxikMI65980083;     RpRaFZCoOoxikMI65980083 = RpRaFZCoOoxikMI47138347;     RpRaFZCoOoxikMI47138347 = RpRaFZCoOoxikMI42627427;     RpRaFZCoOoxikMI42627427 = RpRaFZCoOoxikMI38717037;     RpRaFZCoOoxikMI38717037 = RpRaFZCoOoxikMI14769234;     RpRaFZCoOoxikMI14769234 = RpRaFZCoOoxikMI89711178;     RpRaFZCoOoxikMI89711178 = RpRaFZCoOoxikMI47545964;     RpRaFZCoOoxikMI47545964 = RpRaFZCoOoxikMI74399516;     RpRaFZCoOoxikMI74399516 = RpRaFZCoOoxikMI39449025;     RpRaFZCoOoxikMI39449025 = RpRaFZCoOoxikMI35070439;     RpRaFZCoOoxikMI35070439 = RpRaFZCoOoxikMI79575217;     RpRaFZCoOoxikMI79575217 = RpRaFZCoOoxikMI47629896;     RpRaFZCoOoxikMI47629896 = RpRaFZCoOoxikMI67636160;     RpRaFZCoOoxikMI67636160 = RpRaFZCoOoxikMI27603598;     RpRaFZCoOoxikMI27603598 = RpRaFZCoOoxikMI32597808;     RpRaFZCoOoxikMI32597808 = RpRaFZCoOoxikMI46109088;     RpRaFZCoOoxikMI46109088 = RpRaFZCoOoxikMI46720677;     RpRaFZCoOoxikMI46720677 = RpRaFZCoOoxikMI66911209;     RpRaFZCoOoxikMI66911209 = RpRaFZCoOoxikMI49728294;     RpRaFZCoOoxikMI49728294 = RpRaFZCoOoxikMI35608604;     RpRaFZCoOoxikMI35608604 = RpRaFZCoOoxikMI57170205;     RpRaFZCoOoxikMI57170205 = RpRaFZCoOoxikMI40123269;     RpRaFZCoOoxikMI40123269 = RpRaFZCoOoxikMI64227387;     RpRaFZCoOoxikMI64227387 = RpRaFZCoOoxikMI32924824;     RpRaFZCoOoxikMI32924824 = RpRaFZCoOoxikMI26306884;     RpRaFZCoOoxikMI26306884 = RpRaFZCoOoxikMI36842731;     RpRaFZCoOoxikMI36842731 = RpRaFZCoOoxikMI92658459;     RpRaFZCoOoxikMI92658459 = RpRaFZCoOoxikMI50936829;     RpRaFZCoOoxikMI50936829 = RpRaFZCoOoxikMI32130740;     RpRaFZCoOoxikMI32130740 = RpRaFZCoOoxikMI12784911;     RpRaFZCoOoxikMI12784911 = RpRaFZCoOoxikMI53245803;     RpRaFZCoOoxikMI53245803 = RpRaFZCoOoxikMI3365745;     RpRaFZCoOoxikMI3365745 = RpRaFZCoOoxikMI79324976;     RpRaFZCoOoxikMI79324976 = RpRaFZCoOoxikMI22350857;     RpRaFZCoOoxikMI22350857 = RpRaFZCoOoxikMI22568965;     RpRaFZCoOoxikMI22568965 = RpRaFZCoOoxikMI65796432;     RpRaFZCoOoxikMI65796432 = RpRaFZCoOoxikMI20915971;     RpRaFZCoOoxikMI20915971 = RpRaFZCoOoxikMI90063910;     RpRaFZCoOoxikMI90063910 = RpRaFZCoOoxikMI59667564;     RpRaFZCoOoxikMI59667564 = RpRaFZCoOoxikMI16200703;     RpRaFZCoOoxikMI16200703 = RpRaFZCoOoxikMI88879046;     RpRaFZCoOoxikMI88879046 = RpRaFZCoOoxikMI79894823;     RpRaFZCoOoxikMI79894823 = RpRaFZCoOoxikMI37076767;     RpRaFZCoOoxikMI37076767 = RpRaFZCoOoxikMI47855506;     RpRaFZCoOoxikMI47855506 = RpRaFZCoOoxikMI96565958;     RpRaFZCoOoxikMI96565958 = RpRaFZCoOoxikMI34097624;     RpRaFZCoOoxikMI34097624 = RpRaFZCoOoxikMI18785543;     RpRaFZCoOoxikMI18785543 = RpRaFZCoOoxikMI55167102;     RpRaFZCoOoxikMI55167102 = RpRaFZCoOoxikMI44606908;     RpRaFZCoOoxikMI44606908 = RpRaFZCoOoxikMI73601676;     RpRaFZCoOoxikMI73601676 = RpRaFZCoOoxikMI18963366;     RpRaFZCoOoxikMI18963366 = RpRaFZCoOoxikMI85695234;     RpRaFZCoOoxikMI85695234 = RpRaFZCoOoxikMI47325127;     RpRaFZCoOoxikMI47325127 = RpRaFZCoOoxikMI55085703;     RpRaFZCoOoxikMI55085703 = RpRaFZCoOoxikMI99316653;     RpRaFZCoOoxikMI99316653 = RpRaFZCoOoxikMI25856815;     RpRaFZCoOoxikMI25856815 = RpRaFZCoOoxikMI82910960;     RpRaFZCoOoxikMI82910960 = RpRaFZCoOoxikMI9702604;     RpRaFZCoOoxikMI9702604 = RpRaFZCoOoxikMI12410153;     RpRaFZCoOoxikMI12410153 = RpRaFZCoOoxikMI77926503;     RpRaFZCoOoxikMI77926503 = RpRaFZCoOoxikMI97052719;     RpRaFZCoOoxikMI97052719 = RpRaFZCoOoxikMI75501623;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void lmXGjCaPEXhvTqha64068698() {     float pHohbvmqxBTyBUY46153291 = -555448642;    float pHohbvmqxBTyBUY10021524 = -406686756;    float pHohbvmqxBTyBUY35609094 = -48989195;    float pHohbvmqxBTyBUY11964901 = -290067393;    float pHohbvmqxBTyBUY32914866 = 54897849;    float pHohbvmqxBTyBUY6648824 = 49772962;    float pHohbvmqxBTyBUY65532228 = -500637364;    float pHohbvmqxBTyBUY65908380 = -191436124;    float pHohbvmqxBTyBUY53899457 = -358532190;    float pHohbvmqxBTyBUY46230894 = 76028941;    float pHohbvmqxBTyBUY17678321 = -411677505;    float pHohbvmqxBTyBUY73688116 = -136083698;    float pHohbvmqxBTyBUY31668890 = -540733547;    float pHohbvmqxBTyBUY62315986 = -287003080;    float pHohbvmqxBTyBUY88572266 = -299720806;    float pHohbvmqxBTyBUY32947806 = -536472069;    float pHohbvmqxBTyBUY63872110 = -729094438;    float pHohbvmqxBTyBUY99131208 = -589218673;    float pHohbvmqxBTyBUY70256830 = -692886081;    float pHohbvmqxBTyBUY71721488 = -243737239;    float pHohbvmqxBTyBUY18679152 = -496274217;    float pHohbvmqxBTyBUY90176712 = -161656176;    float pHohbvmqxBTyBUY76380606 = -924023447;    float pHohbvmqxBTyBUY39761418 = -660363917;    float pHohbvmqxBTyBUY549400 = 67151563;    float pHohbvmqxBTyBUY84891920 = -481837645;    float pHohbvmqxBTyBUY84786577 = -28545771;    float pHohbvmqxBTyBUY90708170 = -151747533;    float pHohbvmqxBTyBUY8283664 = -230524567;    float pHohbvmqxBTyBUY20899616 = -646654879;    float pHohbvmqxBTyBUY93493699 = -625086650;    float pHohbvmqxBTyBUY27282161 = -474105829;    float pHohbvmqxBTyBUY60150077 = -47809948;    float pHohbvmqxBTyBUY85075738 = -352901871;    float pHohbvmqxBTyBUY43708279 = -931977759;    float pHohbvmqxBTyBUY12599703 = -665340034;    float pHohbvmqxBTyBUY40844907 = 25945962;    float pHohbvmqxBTyBUY43486796 = -57773538;    float pHohbvmqxBTyBUY35020147 = -731807296;    float pHohbvmqxBTyBUY61083381 = -893888618;    float pHohbvmqxBTyBUY7211631 = -575516256;    float pHohbvmqxBTyBUY71110418 = -217931910;    float pHohbvmqxBTyBUY87683996 = -130893205;    float pHohbvmqxBTyBUY21277535 = -539480829;    float pHohbvmqxBTyBUY85665237 = -623273797;    float pHohbvmqxBTyBUY53951799 = -995369669;    float pHohbvmqxBTyBUY530720 = -964296918;    float pHohbvmqxBTyBUY5063236 = -260212690;    float pHohbvmqxBTyBUY40511892 = -743309984;    float pHohbvmqxBTyBUY88002703 = 67904566;    float pHohbvmqxBTyBUY15819221 = -914642255;    float pHohbvmqxBTyBUY9507665 = -162438609;    float pHohbvmqxBTyBUY82679255 = -705588221;    float pHohbvmqxBTyBUY22825113 = 97680975;    float pHohbvmqxBTyBUY45389547 = -455765928;    float pHohbvmqxBTyBUY55976579 = -293792466;    float pHohbvmqxBTyBUY33640918 = -482663309;    float pHohbvmqxBTyBUY95847676 = -388625278;    float pHohbvmqxBTyBUY11415502 = -257218957;    float pHohbvmqxBTyBUY48022946 = -463264507;    float pHohbvmqxBTyBUY21862246 = -921681268;    float pHohbvmqxBTyBUY74824057 = -248889832;    float pHohbvmqxBTyBUY57624716 = -960911558;    float pHohbvmqxBTyBUY32999842 = -711877312;    float pHohbvmqxBTyBUY52737195 = -298884410;    float pHohbvmqxBTyBUY90396159 = -937571676;    float pHohbvmqxBTyBUY13538039 = 11726249;    float pHohbvmqxBTyBUY46593151 = -87831676;    float pHohbvmqxBTyBUY18607708 = -355025322;    float pHohbvmqxBTyBUY75972563 = -634380773;    float pHohbvmqxBTyBUY92102899 = -462418031;    float pHohbvmqxBTyBUY20385315 = -571320901;    float pHohbvmqxBTyBUY64111062 = -857411378;    float pHohbvmqxBTyBUY9173449 = -798997464;    float pHohbvmqxBTyBUY64509857 = -668220984;    float pHohbvmqxBTyBUY47568734 = -178342308;    float pHohbvmqxBTyBUY2492716 = 69237029;    float pHohbvmqxBTyBUY55103072 = -284542619;    float pHohbvmqxBTyBUY54096180 = 62909879;    float pHohbvmqxBTyBUY46597600 = 62521231;    float pHohbvmqxBTyBUY84361201 = -517540727;    float pHohbvmqxBTyBUY79723342 = -768333081;    float pHohbvmqxBTyBUY50196279 = -408437550;    float pHohbvmqxBTyBUY20280960 = -198429134;    float pHohbvmqxBTyBUY5080395 = -732012624;    float pHohbvmqxBTyBUY83986034 = -362648042;    float pHohbvmqxBTyBUY44602906 = -768517608;    float pHohbvmqxBTyBUY37324964 = -45490923;    float pHohbvmqxBTyBUY39686192 = -897135944;    float pHohbvmqxBTyBUY87731700 = -538185294;    float pHohbvmqxBTyBUY78958785 = -82676725;    float pHohbvmqxBTyBUY44997231 = -585428761;    float pHohbvmqxBTyBUY32071294 = -800554582;    float pHohbvmqxBTyBUY86997201 = -168542790;    float pHohbvmqxBTyBUY39221136 = -972207350;    float pHohbvmqxBTyBUY32387574 = -226626425;    float pHohbvmqxBTyBUY13485702 = -257020353;    float pHohbvmqxBTyBUY54684154 = -419015893;    float pHohbvmqxBTyBUY68540340 = -140596419;    float pHohbvmqxBTyBUY95269077 = -555448642;     pHohbvmqxBTyBUY46153291 = pHohbvmqxBTyBUY10021524;     pHohbvmqxBTyBUY10021524 = pHohbvmqxBTyBUY35609094;     pHohbvmqxBTyBUY35609094 = pHohbvmqxBTyBUY11964901;     pHohbvmqxBTyBUY11964901 = pHohbvmqxBTyBUY32914866;     pHohbvmqxBTyBUY32914866 = pHohbvmqxBTyBUY6648824;     pHohbvmqxBTyBUY6648824 = pHohbvmqxBTyBUY65532228;     pHohbvmqxBTyBUY65532228 = pHohbvmqxBTyBUY65908380;     pHohbvmqxBTyBUY65908380 = pHohbvmqxBTyBUY53899457;     pHohbvmqxBTyBUY53899457 = pHohbvmqxBTyBUY46230894;     pHohbvmqxBTyBUY46230894 = pHohbvmqxBTyBUY17678321;     pHohbvmqxBTyBUY17678321 = pHohbvmqxBTyBUY73688116;     pHohbvmqxBTyBUY73688116 = pHohbvmqxBTyBUY31668890;     pHohbvmqxBTyBUY31668890 = pHohbvmqxBTyBUY62315986;     pHohbvmqxBTyBUY62315986 = pHohbvmqxBTyBUY88572266;     pHohbvmqxBTyBUY88572266 = pHohbvmqxBTyBUY32947806;     pHohbvmqxBTyBUY32947806 = pHohbvmqxBTyBUY63872110;     pHohbvmqxBTyBUY63872110 = pHohbvmqxBTyBUY99131208;     pHohbvmqxBTyBUY99131208 = pHohbvmqxBTyBUY70256830;     pHohbvmqxBTyBUY70256830 = pHohbvmqxBTyBUY71721488;     pHohbvmqxBTyBUY71721488 = pHohbvmqxBTyBUY18679152;     pHohbvmqxBTyBUY18679152 = pHohbvmqxBTyBUY90176712;     pHohbvmqxBTyBUY90176712 = pHohbvmqxBTyBUY76380606;     pHohbvmqxBTyBUY76380606 = pHohbvmqxBTyBUY39761418;     pHohbvmqxBTyBUY39761418 = pHohbvmqxBTyBUY549400;     pHohbvmqxBTyBUY549400 = pHohbvmqxBTyBUY84891920;     pHohbvmqxBTyBUY84891920 = pHohbvmqxBTyBUY84786577;     pHohbvmqxBTyBUY84786577 = pHohbvmqxBTyBUY90708170;     pHohbvmqxBTyBUY90708170 = pHohbvmqxBTyBUY8283664;     pHohbvmqxBTyBUY8283664 = pHohbvmqxBTyBUY20899616;     pHohbvmqxBTyBUY20899616 = pHohbvmqxBTyBUY93493699;     pHohbvmqxBTyBUY93493699 = pHohbvmqxBTyBUY27282161;     pHohbvmqxBTyBUY27282161 = pHohbvmqxBTyBUY60150077;     pHohbvmqxBTyBUY60150077 = pHohbvmqxBTyBUY85075738;     pHohbvmqxBTyBUY85075738 = pHohbvmqxBTyBUY43708279;     pHohbvmqxBTyBUY43708279 = pHohbvmqxBTyBUY12599703;     pHohbvmqxBTyBUY12599703 = pHohbvmqxBTyBUY40844907;     pHohbvmqxBTyBUY40844907 = pHohbvmqxBTyBUY43486796;     pHohbvmqxBTyBUY43486796 = pHohbvmqxBTyBUY35020147;     pHohbvmqxBTyBUY35020147 = pHohbvmqxBTyBUY61083381;     pHohbvmqxBTyBUY61083381 = pHohbvmqxBTyBUY7211631;     pHohbvmqxBTyBUY7211631 = pHohbvmqxBTyBUY71110418;     pHohbvmqxBTyBUY71110418 = pHohbvmqxBTyBUY87683996;     pHohbvmqxBTyBUY87683996 = pHohbvmqxBTyBUY21277535;     pHohbvmqxBTyBUY21277535 = pHohbvmqxBTyBUY85665237;     pHohbvmqxBTyBUY85665237 = pHohbvmqxBTyBUY53951799;     pHohbvmqxBTyBUY53951799 = pHohbvmqxBTyBUY530720;     pHohbvmqxBTyBUY530720 = pHohbvmqxBTyBUY5063236;     pHohbvmqxBTyBUY5063236 = pHohbvmqxBTyBUY40511892;     pHohbvmqxBTyBUY40511892 = pHohbvmqxBTyBUY88002703;     pHohbvmqxBTyBUY88002703 = pHohbvmqxBTyBUY15819221;     pHohbvmqxBTyBUY15819221 = pHohbvmqxBTyBUY9507665;     pHohbvmqxBTyBUY9507665 = pHohbvmqxBTyBUY82679255;     pHohbvmqxBTyBUY82679255 = pHohbvmqxBTyBUY22825113;     pHohbvmqxBTyBUY22825113 = pHohbvmqxBTyBUY45389547;     pHohbvmqxBTyBUY45389547 = pHohbvmqxBTyBUY55976579;     pHohbvmqxBTyBUY55976579 = pHohbvmqxBTyBUY33640918;     pHohbvmqxBTyBUY33640918 = pHohbvmqxBTyBUY95847676;     pHohbvmqxBTyBUY95847676 = pHohbvmqxBTyBUY11415502;     pHohbvmqxBTyBUY11415502 = pHohbvmqxBTyBUY48022946;     pHohbvmqxBTyBUY48022946 = pHohbvmqxBTyBUY21862246;     pHohbvmqxBTyBUY21862246 = pHohbvmqxBTyBUY74824057;     pHohbvmqxBTyBUY74824057 = pHohbvmqxBTyBUY57624716;     pHohbvmqxBTyBUY57624716 = pHohbvmqxBTyBUY32999842;     pHohbvmqxBTyBUY32999842 = pHohbvmqxBTyBUY52737195;     pHohbvmqxBTyBUY52737195 = pHohbvmqxBTyBUY90396159;     pHohbvmqxBTyBUY90396159 = pHohbvmqxBTyBUY13538039;     pHohbvmqxBTyBUY13538039 = pHohbvmqxBTyBUY46593151;     pHohbvmqxBTyBUY46593151 = pHohbvmqxBTyBUY18607708;     pHohbvmqxBTyBUY18607708 = pHohbvmqxBTyBUY75972563;     pHohbvmqxBTyBUY75972563 = pHohbvmqxBTyBUY92102899;     pHohbvmqxBTyBUY92102899 = pHohbvmqxBTyBUY20385315;     pHohbvmqxBTyBUY20385315 = pHohbvmqxBTyBUY64111062;     pHohbvmqxBTyBUY64111062 = pHohbvmqxBTyBUY9173449;     pHohbvmqxBTyBUY9173449 = pHohbvmqxBTyBUY64509857;     pHohbvmqxBTyBUY64509857 = pHohbvmqxBTyBUY47568734;     pHohbvmqxBTyBUY47568734 = pHohbvmqxBTyBUY2492716;     pHohbvmqxBTyBUY2492716 = pHohbvmqxBTyBUY55103072;     pHohbvmqxBTyBUY55103072 = pHohbvmqxBTyBUY54096180;     pHohbvmqxBTyBUY54096180 = pHohbvmqxBTyBUY46597600;     pHohbvmqxBTyBUY46597600 = pHohbvmqxBTyBUY84361201;     pHohbvmqxBTyBUY84361201 = pHohbvmqxBTyBUY79723342;     pHohbvmqxBTyBUY79723342 = pHohbvmqxBTyBUY50196279;     pHohbvmqxBTyBUY50196279 = pHohbvmqxBTyBUY20280960;     pHohbvmqxBTyBUY20280960 = pHohbvmqxBTyBUY5080395;     pHohbvmqxBTyBUY5080395 = pHohbvmqxBTyBUY83986034;     pHohbvmqxBTyBUY83986034 = pHohbvmqxBTyBUY44602906;     pHohbvmqxBTyBUY44602906 = pHohbvmqxBTyBUY37324964;     pHohbvmqxBTyBUY37324964 = pHohbvmqxBTyBUY39686192;     pHohbvmqxBTyBUY39686192 = pHohbvmqxBTyBUY87731700;     pHohbvmqxBTyBUY87731700 = pHohbvmqxBTyBUY78958785;     pHohbvmqxBTyBUY78958785 = pHohbvmqxBTyBUY44997231;     pHohbvmqxBTyBUY44997231 = pHohbvmqxBTyBUY32071294;     pHohbvmqxBTyBUY32071294 = pHohbvmqxBTyBUY86997201;     pHohbvmqxBTyBUY86997201 = pHohbvmqxBTyBUY39221136;     pHohbvmqxBTyBUY39221136 = pHohbvmqxBTyBUY32387574;     pHohbvmqxBTyBUY32387574 = pHohbvmqxBTyBUY13485702;     pHohbvmqxBTyBUY13485702 = pHohbvmqxBTyBUY54684154;     pHohbvmqxBTyBUY54684154 = pHohbvmqxBTyBUY68540340;     pHohbvmqxBTyBUY68540340 = pHohbvmqxBTyBUY95269077;     pHohbvmqxBTyBUY95269077 = pHohbvmqxBTyBUY46153291;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void acLsslNAguqTDrUA34524572() {     float wlqNYKIMQEORnuQ58939458 = -690489333;    float wlqNYKIMQEORnuQ96792411 = -577020769;    float wlqNYKIMQEORnuQ53361894 = -457348577;    float wlqNYKIMQEORnuQ19905727 = -114269670;    float wlqNYKIMQEORnuQ26425971 = -436918616;    float wlqNYKIMQEORnuQ49958894 = -827150352;    float wlqNYKIMQEORnuQ86739986 = 53690565;    float wlqNYKIMQEORnuQ92627492 = -635954381;    float wlqNYKIMQEORnuQ49304268 = -320290283;    float wlqNYKIMQEORnuQ52449599 = -828058692;    float wlqNYKIMQEORnuQ45165703 = -526363520;    float wlqNYKIMQEORnuQ5638621 = -643416514;    float wlqNYKIMQEORnuQ1812193 = -444516204;    float wlqNYKIMQEORnuQ11940065 = -619418279;    float wlqNYKIMQEORnuQ11878819 = -206706637;    float wlqNYKIMQEORnuQ61372875 = 12636542;    float wlqNYKIMQEORnuQ19623928 = -392868905;    float wlqNYKIMQEORnuQ82689693 = -144428975;    float wlqNYKIMQEORnuQ71417460 = -602355277;    float wlqNYKIMQEORnuQ74375984 = -842981378;    float wlqNYKIMQEORnuQ98853208 = -987590482;    float wlqNYKIMQEORnuQ19224532 = -331514524;    float wlqNYKIMQEORnuQ15140902 = -855027342;    float wlqNYKIMQEORnuQ84268547 = 79291070;    float wlqNYKIMQEORnuQ6443659 = -143290361;    float wlqNYKIMQEORnuQ66210565 = -172268844;    float wlqNYKIMQEORnuQ96202114 = -662252051;    float wlqNYKIMQEORnuQ58061096 = -325545637;    float wlqNYKIMQEORnuQ24900717 = -444741808;    float wlqNYKIMQEORnuQ73650730 = -276541033;    float wlqNYKIMQEORnuQ37929696 = -901503400;    float wlqNYKIMQEORnuQ30600985 = -452825508;    float wlqNYKIMQEORnuQ6196026 = 76335268;    float wlqNYKIMQEORnuQ62152111 = -257656089;    float wlqNYKIMQEORnuQ26768811 = -442009640;    float wlqNYKIMQEORnuQ7823818 = -453903138;    float wlqNYKIMQEORnuQ77608967 = -863963389;    float wlqNYKIMQEORnuQ1190819 = 22606371;    float wlqNYKIMQEORnuQ40795533 = -573657873;    float wlqNYKIMQEORnuQ87348111 = -738305464;    float wlqNYKIMQEORnuQ68642939 = -950990249;    float wlqNYKIMQEORnuQ78812874 = -116243244;    float wlqNYKIMQEORnuQ22567467 = -154326650;    float wlqNYKIMQEORnuQ65906072 = -880653072;    float wlqNYKIMQEORnuQ65673727 = -516043757;    float wlqNYKIMQEORnuQ27579929 = 31311892;    float wlqNYKIMQEORnuQ78112739 = -361811971;    float wlqNYKIMQEORnuQ34503202 = -415565470;    float wlqNYKIMQEORnuQ84575941 = -906911958;    float wlqNYKIMQEORnuQ34953124 = -543131897;    float wlqNYKIMQEORnuQ34146714 = -67526564;    float wlqNYKIMQEORnuQ37684212 = -633623947;    float wlqNYKIMQEORnuQ17164406 = -881591504;    float wlqNYKIMQEORnuQ67524838 = -591770767;    float wlqNYKIMQEORnuQ35671363 = -450210414;    float wlqNYKIMQEORnuQ39714927 = -258974810;    float wlqNYKIMQEORnuQ81651510 = -721993428;    float wlqNYKIMQEORnuQ69093346 = -436639647;    float wlqNYKIMQEORnuQ13462069 = -970979309;    float wlqNYKIMQEORnuQ60215405 = -164649773;    float wlqNYKIMQEORnuQ53756780 = -64898301;    float wlqNYKIMQEORnuQ28678891 = -620763798;    float wlqNYKIMQEORnuQ67726776 = -91212574;    float wlqNYKIMQEORnuQ75653537 = 56250750;    float wlqNYKIMQEORnuQ14519904 = -926555293;    float wlqNYKIMQEORnuQ14564718 = 26461987;    float wlqNYKIMQEORnuQ99442595 = -619751782;    float wlqNYKIMQEORnuQ39660082 = -86860115;    float wlqNYKIMQEORnuQ85171254 = -77408639;    float wlqNYKIMQEORnuQ4055002 = -752803500;    float wlqNYKIMQEORnuQ83763907 = -123400070;    float wlqNYKIMQEORnuQ18433110 = -315475276;    float wlqNYKIMQEORnuQ41894161 = -570771103;    float wlqNYKIMQEORnuQ84069349 = -864049814;    float wlqNYKIMQEORnuQ5733046 = -891991129;    float wlqNYKIMQEORnuQ20040335 = -771347238;    float wlqNYKIMQEORnuQ96657064 = -77187874;    float wlqNYKIMQEORnuQ49234830 = -974374271;    float wlqNYKIMQEORnuQ18594820 = -404665173;    float wlqNYKIMQEORnuQ78863730 = -74602254;    float wlqNYKIMQEORnuQ88097826 = -810456874;    float wlqNYKIMQEORnuQ61698912 = -146686582;    float wlqNYKIMQEORnuQ73485155 = -418633680;    float wlqNYKIMQEORnuQ89947592 = -901609911;    float wlqNYKIMQEORnuQ39504017 = -109014470;    float wlqNYKIMQEORnuQ245484 = -167879454;    float wlqNYKIMQEORnuQ13436580 = -571234004;    float wlqNYKIMQEORnuQ38671188 = -331893965;    float wlqNYKIMQEORnuQ26480749 = -807445676;    float wlqNYKIMQEORnuQ87053884 = -83034830;    float wlqNYKIMQEORnuQ26172307 = -731909711;    float wlqNYKIMQEORnuQ8515621 = -327323742;    float wlqNYKIMQEORnuQ87728750 = -6414321;    float wlqNYKIMQEORnuQ80580127 = -309008101;    float wlqNYKIMQEORnuQ33591332 = -573407163;    float wlqNYKIMQEORnuQ39964048 = -230226452;    float wlqNYKIMQEORnuQ11086098 = 74969330;    float wlqNYKIMQEORnuQ46913929 = -110577401;    float wlqNYKIMQEORnuQ51386169 = -954097779;    float wlqNYKIMQEORnuQ51109010 = -690489333;     wlqNYKIMQEORnuQ58939458 = wlqNYKIMQEORnuQ96792411;     wlqNYKIMQEORnuQ96792411 = wlqNYKIMQEORnuQ53361894;     wlqNYKIMQEORnuQ53361894 = wlqNYKIMQEORnuQ19905727;     wlqNYKIMQEORnuQ19905727 = wlqNYKIMQEORnuQ26425971;     wlqNYKIMQEORnuQ26425971 = wlqNYKIMQEORnuQ49958894;     wlqNYKIMQEORnuQ49958894 = wlqNYKIMQEORnuQ86739986;     wlqNYKIMQEORnuQ86739986 = wlqNYKIMQEORnuQ92627492;     wlqNYKIMQEORnuQ92627492 = wlqNYKIMQEORnuQ49304268;     wlqNYKIMQEORnuQ49304268 = wlqNYKIMQEORnuQ52449599;     wlqNYKIMQEORnuQ52449599 = wlqNYKIMQEORnuQ45165703;     wlqNYKIMQEORnuQ45165703 = wlqNYKIMQEORnuQ5638621;     wlqNYKIMQEORnuQ5638621 = wlqNYKIMQEORnuQ1812193;     wlqNYKIMQEORnuQ1812193 = wlqNYKIMQEORnuQ11940065;     wlqNYKIMQEORnuQ11940065 = wlqNYKIMQEORnuQ11878819;     wlqNYKIMQEORnuQ11878819 = wlqNYKIMQEORnuQ61372875;     wlqNYKIMQEORnuQ61372875 = wlqNYKIMQEORnuQ19623928;     wlqNYKIMQEORnuQ19623928 = wlqNYKIMQEORnuQ82689693;     wlqNYKIMQEORnuQ82689693 = wlqNYKIMQEORnuQ71417460;     wlqNYKIMQEORnuQ71417460 = wlqNYKIMQEORnuQ74375984;     wlqNYKIMQEORnuQ74375984 = wlqNYKIMQEORnuQ98853208;     wlqNYKIMQEORnuQ98853208 = wlqNYKIMQEORnuQ19224532;     wlqNYKIMQEORnuQ19224532 = wlqNYKIMQEORnuQ15140902;     wlqNYKIMQEORnuQ15140902 = wlqNYKIMQEORnuQ84268547;     wlqNYKIMQEORnuQ84268547 = wlqNYKIMQEORnuQ6443659;     wlqNYKIMQEORnuQ6443659 = wlqNYKIMQEORnuQ66210565;     wlqNYKIMQEORnuQ66210565 = wlqNYKIMQEORnuQ96202114;     wlqNYKIMQEORnuQ96202114 = wlqNYKIMQEORnuQ58061096;     wlqNYKIMQEORnuQ58061096 = wlqNYKIMQEORnuQ24900717;     wlqNYKIMQEORnuQ24900717 = wlqNYKIMQEORnuQ73650730;     wlqNYKIMQEORnuQ73650730 = wlqNYKIMQEORnuQ37929696;     wlqNYKIMQEORnuQ37929696 = wlqNYKIMQEORnuQ30600985;     wlqNYKIMQEORnuQ30600985 = wlqNYKIMQEORnuQ6196026;     wlqNYKIMQEORnuQ6196026 = wlqNYKIMQEORnuQ62152111;     wlqNYKIMQEORnuQ62152111 = wlqNYKIMQEORnuQ26768811;     wlqNYKIMQEORnuQ26768811 = wlqNYKIMQEORnuQ7823818;     wlqNYKIMQEORnuQ7823818 = wlqNYKIMQEORnuQ77608967;     wlqNYKIMQEORnuQ77608967 = wlqNYKIMQEORnuQ1190819;     wlqNYKIMQEORnuQ1190819 = wlqNYKIMQEORnuQ40795533;     wlqNYKIMQEORnuQ40795533 = wlqNYKIMQEORnuQ87348111;     wlqNYKIMQEORnuQ87348111 = wlqNYKIMQEORnuQ68642939;     wlqNYKIMQEORnuQ68642939 = wlqNYKIMQEORnuQ78812874;     wlqNYKIMQEORnuQ78812874 = wlqNYKIMQEORnuQ22567467;     wlqNYKIMQEORnuQ22567467 = wlqNYKIMQEORnuQ65906072;     wlqNYKIMQEORnuQ65906072 = wlqNYKIMQEORnuQ65673727;     wlqNYKIMQEORnuQ65673727 = wlqNYKIMQEORnuQ27579929;     wlqNYKIMQEORnuQ27579929 = wlqNYKIMQEORnuQ78112739;     wlqNYKIMQEORnuQ78112739 = wlqNYKIMQEORnuQ34503202;     wlqNYKIMQEORnuQ34503202 = wlqNYKIMQEORnuQ84575941;     wlqNYKIMQEORnuQ84575941 = wlqNYKIMQEORnuQ34953124;     wlqNYKIMQEORnuQ34953124 = wlqNYKIMQEORnuQ34146714;     wlqNYKIMQEORnuQ34146714 = wlqNYKIMQEORnuQ37684212;     wlqNYKIMQEORnuQ37684212 = wlqNYKIMQEORnuQ17164406;     wlqNYKIMQEORnuQ17164406 = wlqNYKIMQEORnuQ67524838;     wlqNYKIMQEORnuQ67524838 = wlqNYKIMQEORnuQ35671363;     wlqNYKIMQEORnuQ35671363 = wlqNYKIMQEORnuQ39714927;     wlqNYKIMQEORnuQ39714927 = wlqNYKIMQEORnuQ81651510;     wlqNYKIMQEORnuQ81651510 = wlqNYKIMQEORnuQ69093346;     wlqNYKIMQEORnuQ69093346 = wlqNYKIMQEORnuQ13462069;     wlqNYKIMQEORnuQ13462069 = wlqNYKIMQEORnuQ60215405;     wlqNYKIMQEORnuQ60215405 = wlqNYKIMQEORnuQ53756780;     wlqNYKIMQEORnuQ53756780 = wlqNYKIMQEORnuQ28678891;     wlqNYKIMQEORnuQ28678891 = wlqNYKIMQEORnuQ67726776;     wlqNYKIMQEORnuQ67726776 = wlqNYKIMQEORnuQ75653537;     wlqNYKIMQEORnuQ75653537 = wlqNYKIMQEORnuQ14519904;     wlqNYKIMQEORnuQ14519904 = wlqNYKIMQEORnuQ14564718;     wlqNYKIMQEORnuQ14564718 = wlqNYKIMQEORnuQ99442595;     wlqNYKIMQEORnuQ99442595 = wlqNYKIMQEORnuQ39660082;     wlqNYKIMQEORnuQ39660082 = wlqNYKIMQEORnuQ85171254;     wlqNYKIMQEORnuQ85171254 = wlqNYKIMQEORnuQ4055002;     wlqNYKIMQEORnuQ4055002 = wlqNYKIMQEORnuQ83763907;     wlqNYKIMQEORnuQ83763907 = wlqNYKIMQEORnuQ18433110;     wlqNYKIMQEORnuQ18433110 = wlqNYKIMQEORnuQ41894161;     wlqNYKIMQEORnuQ41894161 = wlqNYKIMQEORnuQ84069349;     wlqNYKIMQEORnuQ84069349 = wlqNYKIMQEORnuQ5733046;     wlqNYKIMQEORnuQ5733046 = wlqNYKIMQEORnuQ20040335;     wlqNYKIMQEORnuQ20040335 = wlqNYKIMQEORnuQ96657064;     wlqNYKIMQEORnuQ96657064 = wlqNYKIMQEORnuQ49234830;     wlqNYKIMQEORnuQ49234830 = wlqNYKIMQEORnuQ18594820;     wlqNYKIMQEORnuQ18594820 = wlqNYKIMQEORnuQ78863730;     wlqNYKIMQEORnuQ78863730 = wlqNYKIMQEORnuQ88097826;     wlqNYKIMQEORnuQ88097826 = wlqNYKIMQEORnuQ61698912;     wlqNYKIMQEORnuQ61698912 = wlqNYKIMQEORnuQ73485155;     wlqNYKIMQEORnuQ73485155 = wlqNYKIMQEORnuQ89947592;     wlqNYKIMQEORnuQ89947592 = wlqNYKIMQEORnuQ39504017;     wlqNYKIMQEORnuQ39504017 = wlqNYKIMQEORnuQ245484;     wlqNYKIMQEORnuQ245484 = wlqNYKIMQEORnuQ13436580;     wlqNYKIMQEORnuQ13436580 = wlqNYKIMQEORnuQ38671188;     wlqNYKIMQEORnuQ38671188 = wlqNYKIMQEORnuQ26480749;     wlqNYKIMQEORnuQ26480749 = wlqNYKIMQEORnuQ87053884;     wlqNYKIMQEORnuQ87053884 = wlqNYKIMQEORnuQ26172307;     wlqNYKIMQEORnuQ26172307 = wlqNYKIMQEORnuQ8515621;     wlqNYKIMQEORnuQ8515621 = wlqNYKIMQEORnuQ87728750;     wlqNYKIMQEORnuQ87728750 = wlqNYKIMQEORnuQ80580127;     wlqNYKIMQEORnuQ80580127 = wlqNYKIMQEORnuQ33591332;     wlqNYKIMQEORnuQ33591332 = wlqNYKIMQEORnuQ39964048;     wlqNYKIMQEORnuQ39964048 = wlqNYKIMQEORnuQ11086098;     wlqNYKIMQEORnuQ11086098 = wlqNYKIMQEORnuQ46913929;     wlqNYKIMQEORnuQ46913929 = wlqNYKIMQEORnuQ51386169;     wlqNYKIMQEORnuQ51386169 = wlqNYKIMQEORnuQ51109010;     wlqNYKIMQEORnuQ51109010 = wlqNYKIMQEORnuQ58939458;}
// Junk Finished
