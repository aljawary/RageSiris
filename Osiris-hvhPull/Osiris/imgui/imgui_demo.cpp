// dear imgui, v1.77 WIP
// (demo code)

// Help:
// - Read FAQ at http://dearimgui.org/faq
// - Newcomers, read 'Programmer guide' in imgui.cpp for notes on how to setup Dear ImGui in your codebase.
// - Call and read ImGui::ShowDemoWindow() in imgui_demo.cpp. All applications in examples/ are doing that.
// Read imgui.cpp for more details, documentation and comments.
// Get latest version at https://github.com/ocornut/imgui

// Message to the person tempted to delete this file when integrating Dear ImGui into their code base:
// Do NOT remove this file from your project! Think again! It is the most useful reference code that you and other
// coders will want to refer to and call. Have the ImGui::ShowDemoWindow() function wired in an always-available
// debug menu of your game/app! Removing this file from your project is hindering access to documentation for everyone
// in your team, likely leading you to poorer usage of the library.
// Everything in this file will be stripped out by the linker if you don't call ImGui::ShowDemoWindow().
// If you want to link core Dear ImGui in your shipped builds but want a thorough guarantee that the demo will not be
// linked, you can setup your imconfig.h with #define IMGUI_DISABLE_DEMO_WINDOWS and those functions will be empty.
// In other situation, whenever you have Dear ImGui available you probably want this to be available for reference.
// Thank you,
// -Your beloved friend, imgui_demo.cpp (which you won't delete)

// Message to beginner C/C++ programmers about the meaning of the 'static' keyword:
// In this demo code, we frequently we use 'static' variables inside functions. A static variable persist across calls,
// so it is essentially like a global variable but declared inside the scope of the function. We do this as a way to
// gather code and data in the same place, to make the demo source code faster to read, faster to write, and smaller
// in size. It also happens to be a convenient way of storing simple UI related information as long as your function
// doesn't need to be reentrant or used in multiple threads. This might be a pattern you will want to use in your code,
// but most of the real data you would be editing is likely going to be stored outside your functions.

// The Demo code in this file is designed to be easy to copy-and-paste in into your application!
// Because of this:
// - We never omit the ImGui:: prefix when calling functions, even though most code here is in the same namespace.
// - We try to declare static variables in the local scope, as close as possible to the code using them.
// - We never use any of the helpers/facilities used internally by Dear ImGui, unless available in the public API.
// - We never use maths operators on ImVec2/ImVec4. For our other sources files we use them, and they are provided
//   by imgui_internal.h using the IMGUI_DEFINE_MATH_OPERATORS define. For your own sources file they are optional
//   and require you either enable those, either provide your own via IM_VEC2_CLASS_EXTRA in imconfig.h.
//   Because we can't assume anything about your support of maths operators, we cannot use them in imgui_demo.cpp.

/*

Index of this file:

// [SECTION] Forward Declarations, Helpers
// [SECTION] Demo Window / ShowDemoWindow()
// [SECTION] About Window / ShowAboutWindow()
// [SECTION] Style Editor / ShowStyleEditor()
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
// [SECTION] Example App: Simple Overlay / ShowExampleAppSimpleOverlay()
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DISABLE

#include <ctype.h>          // toupper
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>         // intptr_t
#else
#include <stdint.h>         // intptr_t
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wold-style-cast"           // warning: use of old-style cast                          // yes, they are more terse.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"  // warning: 'xx' is deprecated: The POSIX name for this..  // for strdup used in demo code (so user can copy & paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast" // warning: cast to 'void *' from smaller integer type
#pragma clang diagnostic ignored "-Wformat-security"          // warning: format string is not a string literal
#pragma clang diagnostic ignored "-Wexit-time-destructors"    // warning: declaration requires an exit-time destructor   // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. ImGui coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wunused-macros"            // warning: macro is not used                              // we define snprintf/vsnprintf on Windows so they are available, but not always used.
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant" // warning: zero as null pointer constant             // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"        // warning: macro name is a reserved identifier
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                    // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"        // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat-security"            // warning: format string is not a string literal (potentially insecure)
#pragma GCC diagnostic ignored "-Wdouble-promotion"           // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"                 // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wmisleading-indentation"     // [__GNUC__ >= 6] warning: this 'if' clause does not guard this statement      // GCC 6.0+ only. See #883 on GitHub.
#endif

// Play it nice with Windows users (Update: May 2018, Notepad now supports Unix-style carriage returns!)
#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#else
#define IM_NEWLINE  "\n"
#endif

// Helpers
#if defined(_MSC_VER) && !defined(snprintf)
#define snprintf    _snprintf
#endif
#if defined(_MSC_VER) && !defined(vsnprintf)
#define vsnprintf   _vsnprintf
#endif

// Helpers macros
// We normally try to not use many helpers in imgui_demo.cpp in order to make code easier to copy and paste,
// but making an exception here as those are largely simplifying code...
// In other imgui sources we can use nicer internal functions from imgui_internal.h (ImMin/ImMax) but not in the demo.
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

//-----------------------------------------------------------------------------
// [SECTION] Forward Declarations, Helpers
//-----------------------------------------------------------------------------

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

// Forward Declarations
static void ShowExampleAppDocuments(bool* p_open);
static void ShowExampleAppMainMenuBar();
static void ShowExampleAppConsole(bool* p_open);
static void ShowExampleAppLog(bool* p_open);
static void ShowExampleAppLayout(bool* p_open);
static void ShowExampleAppPropertyEditor(bool* p_open);
static void ShowExampleAppLongText(bool* p_open);
static void ShowExampleAppAutoResize(bool* p_open);
static void ShowExampleAppConstrainedResize(bool* p_open);
static void ShowExampleAppSimpleOverlay(bool* p_open);
static void ShowExampleAppWindowTitles(bool* p_open);
static void ShowExampleAppCustomRendering(bool* p_open);
static void ShowExampleMenuFile();

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
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

// Helper to display basic user controls.
void ImGui::ShowUserGuide()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::BulletText("Double-click on title bar to collapse window.");
    ImGui::BulletText(
        "Click and drag on lower corner to resize window\n"
        "(double-click to auto fit window to its contents).");
    ImGui::BulletText("CTRL+Click on a slider or drag box to input value as text.");
    ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
    if (io.FontAllowUserScaling)
        ImGui::BulletText("CTRL+Mouse Wheel to zoom window contents.");
    ImGui::BulletText("While inputing text:\n");
    ImGui::Indent();
    ImGui::BulletText("CTRL+Left/Right to word jump.");
    ImGui::BulletText("CTRL+A or double-click to select all.");
    ImGui::BulletText("CTRL+X/C/V to use clipboard cut/copy/paste.");
    ImGui::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
    ImGui::BulletText("ESCAPE to revert.");
    ImGui::BulletText("You can apply arithmetic operators +,*,/ on numerical values.\nUse +- to subtract.");
    ImGui::Unindent();
    ImGui::BulletText("With keyboard navigation enabled:");
    ImGui::Indent();
    ImGui::BulletText("Arrow keys to navigate.");
    ImGui::BulletText("Space to activate a widget.");
    ImGui::BulletText("Return to input text into a widget.");
    ImGui::BulletText("Escape to deactivate a widget, close popup, exit child window.");
    ImGui::BulletText("Alt to jump to the menu layer of a window.");
    ImGui::BulletText("CTRL+Tab to select a window.");
    ImGui::Unindent();
}

//-----------------------------------------------------------------------------
// [SECTION] Demo Window / ShowDemoWindow()
//-----------------------------------------------------------------------------
// - ShowDemoWindowWidgets()
// - ShowDemoWindowLayout()
// - ShowDemoWindowPopups()
// - ShowDemoWindowColumns()
// - ShowDemoWindowMisc()
//-----------------------------------------------------------------------------

// We split the contents of the big ShowDemoWindow() function into smaller functions
// (because the link time of very large functions grow non-linearly)
static void ShowDemoWindowWidgets();
static void ShowDemoWindowLayout();
static void ShowDemoWindowPopups();
static void ShowDemoWindowColumns();
static void ShowDemoWindowMisc();

// Demonstrate most Dear ImGui features (this is big function!)
// You may execute this function to experiment with the UI and understand what it does.
// You may then search for keywords in the code when you are interested by a specific feature.
void ImGui::ShowDemoWindow(bool* p_open)
{
    // Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
    // Most ImGui functions would normally just crash if the context is missing.
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

    // Examples Apps (accessible from the "Examples" menu)
    static bool show_app_documents = false;
    static bool show_app_main_menu_bar = false;
    static bool show_app_console = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_window_titles = false;
    static bool show_app_custom_rendering = false;

    if (show_app_documents)           ShowExampleAppDocuments(&show_app_documents);
    if (show_app_main_menu_bar)       ShowExampleAppMainMenuBar();
    if (show_app_console)             ShowExampleAppConsole(&show_app_console);
    if (show_app_log)                 ShowExampleAppLog(&show_app_log);
    if (show_app_layout)              ShowExampleAppLayout(&show_app_layout);
    if (show_app_property_editor)     ShowExampleAppPropertyEditor(&show_app_property_editor);
    if (show_app_long_text)           ShowExampleAppLongText(&show_app_long_text);
    if (show_app_auto_resize)         ShowExampleAppAutoResize(&show_app_auto_resize);
    if (show_app_constrained_resize)  ShowExampleAppConstrainedResize(&show_app_constrained_resize);
    if (show_app_simple_overlay)      ShowExampleAppSimpleOverlay(&show_app_simple_overlay);
    if (show_app_window_titles)       ShowExampleAppWindowTitles(&show_app_window_titles);
    if (show_app_custom_rendering)    ShowExampleAppCustomRendering(&show_app_custom_rendering);

    // Dear ImGui Apps (accessible from the "Tools" menu)
    static bool show_app_metrics = false;
    static bool show_app_style_editor = false;
    static bool show_app_about = false;

    if (show_app_metrics) { ImGui::ShowMetricsWindow(&show_app_metrics); }
    if (show_app_about) { ImGui::ShowAboutWindow(&show_app_about); }
    if (show_app_style_editor)
    {
        ImGui::Begin("Dear ImGui Style Editor", &show_app_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }

    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!ImGui::Begin("Dear ImGui Demo", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    // Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.

    // e.g. Use 2/3 of the space for widgets and 1/3 for labels (default)
    //ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);

    // e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

    // Menu Bar
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
            ImGui::MenuItem("Log", NULL, &show_app_log);
            ImGui::MenuItem("Simple layout", NULL, &show_app_layout);
            ImGui::MenuItem("Property editor", NULL, &show_app_property_editor);
            ImGui::MenuItem("Long text display", NULL, &show_app_long_text);
            ImGui::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            ImGui::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
            ImGui::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            ImGui::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
            ImGui::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            ImGui::MenuItem("Documents", NULL, &show_app_documents);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
            ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
            ImGui::MenuItem("About Dear ImGui", NULL, &show_app_about);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("dear imgui says hello. (%s)", IMGUI_VERSION);
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::Text("ABOUT THIS DEMO:");
        ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
        ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
        ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
            "and Metrics (general purpose Dear ImGui debugging tool).");
        ImGui::Separator();

        ImGui::Text("PROGRAMMER GUIDE:");
        ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
        ImGui::BulletText("See comments in imgui.cpp.");
        ImGui::BulletText("See example applications in the examples/ folder.");
        ImGui::BulletText("Read the FAQ at http://www.dearimgui.org/faq/");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
        ImGui::Separator();

        ImGui::Text("USER GUIDE:");
        ImGui::ShowUserGuide();
    }

    if (ImGui::CollapsingHeader("Configuration"))
    {
        ImGuiIO& io = ImGui::GetIO();

        if (ImGui::TreeNode("Configuration##2"))
        {
            ImGui::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard", (unsigned int*)&io.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
            ImGui::CheckboxFlags("io.ConfigFlags: NavEnableGamepad", (unsigned int*)&io.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad);
            ImGui::SameLine(); HelpMarker("Required back-end to feed in gamepad inputs in io.NavInputs[] and set io.BackendFlags |= ImGuiBackendFlags_HasGamepad.\n\nRead instructions in imgui.cpp for details.");
            ImGui::CheckboxFlags("io.ConfigFlags: NavEnableSetMousePos", (unsigned int*)&io.ConfigFlags, ImGuiConfigFlags_NavEnableSetMousePos);
            ImGui::SameLine(); HelpMarker("Instruct navigation to move the mouse cursor. See comment for ImGuiConfigFlags_NavEnableSetMousePos.");
            ImGui::CheckboxFlags("io.ConfigFlags: NoMouse", (unsigned int*)&io.ConfigFlags, ImGuiConfigFlags_NoMouse);

            // The "NoMouse" option above can get us stuck with a disable mouse! Provide an alternative way to fix it:
            if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)
            {
                if (fmodf((float)ImGui::GetTime(), 0.40f) < 0.20f)
                {
                    ImGui::SameLine();
                    ImGui::Text("<<PRESS SPACE TO DISABLE>>");
                }
                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
                    io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            }
            ImGui::CheckboxFlags("io.ConfigFlags: NoMouseCursorChange", (unsigned int*)&io.ConfigFlags, ImGuiConfigFlags_NoMouseCursorChange);
            ImGui::SameLine(); HelpMarker("Instruct back-end to not alter mouse cursor shape and visibility.");
            ImGui::Checkbox("io.ConfigInputTextCursorBlink", &io.ConfigInputTextCursorBlink);
            ImGui::SameLine(); HelpMarker("Set to false to disable blinking cursor, for users who consider it distracting");
            ImGui::Checkbox("io.ConfigWindowsResizeFromEdges", &io.ConfigWindowsResizeFromEdges);
            ImGui::SameLine(); HelpMarker("Enable resizing of windows from their edges and from the lower-left corner.\nThis requires (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors) because it needs mouse cursor feedback.");
            ImGui::Checkbox("io.ConfigWindowsMoveFromTitleBarOnly", &io.ConfigWindowsMoveFromTitleBarOnly);
            ImGui::Checkbox("io.MouseDrawCursor", &io.MouseDrawCursor);
            ImGui::SameLine(); HelpMarker("Instruct Dear ImGui to render a mouse cursor itself. Note that a mouse cursor rendered via your application GPU rendering path will feel more laggy than hardware cursor, but will be more in sync with your other visuals.\n\nSome desktop applications may use both kinds of cursors (e.g. enable software cursor only when resizing/dragging something).");
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (ImGui::TreeNode("Backend Flags"))
        {
            HelpMarker(
                "Those flags are set by the back-ends (imgui_impl_xxx files) to specify their capabilities.\n"
                "Here we expose then as read-only fields to avoid breaking interactions with your back-end.");

            // Make a local copy to avoid modifying actual back-end flags.
            ImGuiBackendFlags backend_flags = io.BackendFlags;
            ImGui::CheckboxFlags("io.BackendFlags: HasGamepad", (unsigned int*)&backend_flags, ImGuiBackendFlags_HasGamepad);
            ImGui::CheckboxFlags("io.BackendFlags: HasMouseCursors", (unsigned int*)&backend_flags, ImGuiBackendFlags_HasMouseCursors);
            ImGui::CheckboxFlags("io.BackendFlags: HasSetMousePos", (unsigned int*)&backend_flags, ImGuiBackendFlags_HasSetMousePos);
            ImGui::CheckboxFlags("io.BackendFlags: RendererHasVtxOffset", (unsigned int*)&backend_flags, ImGuiBackendFlags_RendererHasVtxOffset);
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (ImGui::TreeNode("Style"))
        {
            HelpMarker("The same contents can be accessed in 'Tools->Style Editor' or by calling the ShowStyleEditor() function.");
            ImGui::ShowStyleEditor();
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (ImGui::TreeNode("Capture/Logging"))
        {
            HelpMarker(
                "The logging API redirects all text output so you can easily capture the content of "
                "a window or a block. Tree nodes can be automatically expanded.\n"
                "Try opening any of the contents below in this window and then click one of the \"Log To\" button.");
            ImGui::LogButtons();

            HelpMarker("You can also call ImGui::LogText() to output directly to the log without a visual output.");
            if (ImGui::Button("Copy \"Hello, world!\" to clipboard"))
            {
                ImGui::LogToClipboard();
                ImGui::LogText("Hello, world!");
                ImGui::LogFinish();
            }
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Window options"))
    {
        ImGui::Checkbox("No titlebar", &no_titlebar); ImGui::SameLine(150);
        ImGui::Checkbox("No scrollbar", &no_scrollbar); ImGui::SameLine(300);
        ImGui::Checkbox("No menu", &no_menu);
        ImGui::Checkbox("No move", &no_move); ImGui::SameLine(150);
        ImGui::Checkbox("No resize", &no_resize); ImGui::SameLine(300);
        ImGui::Checkbox("No collapse", &no_collapse);
        ImGui::Checkbox("No close", &no_close); ImGui::SameLine(150);
        ImGui::Checkbox("No nav", &no_nav); ImGui::SameLine(300);
        ImGui::Checkbox("No background", &no_background);
        ImGui::Checkbox("No bring to front", &no_bring_to_front);
    }

    // All demo contents
    ShowDemoWindowWidgets();
    ShowDemoWindowLayout();
    ShowDemoWindowPopups();
    ShowDemoWindowColumns();
    ShowDemoWindowMisc();

    // End of ShowDemoWindow()
    ImGui::End();
}

static void ShowDemoWindowWidgets()
{
    if (!ImGui::CollapsingHeader("Widgets"))
        return;

    if (ImGui::TreeNode("Basic"))
    {
        static int clicked = 0;
        if (ImGui::Button("Button"))
            clicked++;
        if (clicked & 1)
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
            if (i > 0)
                ImGui::SameLine();
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
            ImGui::Button("Click");
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed widgets elements
        // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default!)
        // See 'Demo->Layout->Text Baseline Alignment' for details.
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Hold to repeat:");
        ImGui::SameLine();

        // Arrow buttons with Repeater
        static int counter = 0;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::PushButtonRepeat(true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
        ImGui::PopButtonRepeat();
        ImGui::SameLine();
        ImGui::Text("%d", counter);

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

        ImGui::Separator();

        ImGui::LabelText("label", "Value");

        {
            // Using the _simplified_ one-liner Combo() api here
            // See "Combo" section for examples of how to use the more complete BeginCombo()/EndCombo() api.
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
            static int item_current = 0;
            ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
            ImGui::SameLine(); HelpMarker("Refer to the \"Combo\" section below for an explanation of the full BeginCombo/EndCombo API, and demonstration of various flags.\n");
        }

        {
            // To wire InputText() with std::string or any other custom string type,
            // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/imgui_stdlib.h file.
            static char str0[128] = "Hello, world!";
            ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));
            ImGui::SameLine(); HelpMarker(
                "USER:\n"
                "Hold SHIFT or use mouse to select text.\n"
                "CTRL+Left/Right to word jump.\n"
                "CTRL+A or double-click to select all.\n"
                "CTRL+X,CTRL+C,CTRL+V clipboard.\n"
                "CTRL+Z,CTRL+Y undo/redo.\n"
                "ESCAPE to revert.\n\n"
                "PROGRAMMER:\n"
                "You can use the ImGuiInputTextFlags_CallbackResize facility if you need to wire InputText() "
                "to a dynamic string type. See misc/cpp/imgui_stdlib.h for an example (this is not demonstrated "
                "in imgui_demo.cpp).");

            static char str1[128] = "";
            ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            static int i0 = 123;
            ImGui::InputInt("input int", &i0);
            ImGui::SameLine(); HelpMarker(
                "You can apply arithmetic operators +,*,/ on numerical values.\n"
                "  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\n"
                "Use +- to subtract.");

            static float f0 = 0.001f;
            ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static double d0 = 999999.00000001;
            ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            ImGui::SameLine(); HelpMarker(
                "You can input value using the scientific notation,\n"
                "  e.g. \"1e+8\" becomes \"100000000\".");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            ImGui::InputFloat3("input float3", vec4a);
        }

        {
            static int i1 = 50, i2 = 42;
            ImGui::DragInt("drag int", &i1, 1);
            ImGui::SameLine(); HelpMarker(
                "Click and drag to edit value.\n"
                "Hold SHIFT/ALT for faster/slower edit.\n"
                "Double-click or CTRL+click to input value.");

            ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%");

            static float f1 = 1.00f, f2 = 0.0067f;
            ImGui::DragFloat("drag float", &f1, 0.005f);
            ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
        }

        {
            static int i1 = 0;
            ImGui::SliderInt("slider int", &i1, -1, 3);
            ImGui::SameLine(); HelpMarker("CTRL+click to input value.");

            static float f1 = 0.123f, f2 = 0.0f;
            ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            ImGui::SliderFloat("slider float (curve)", &f2, -10.0f, 10.0f, "%.4f", 2.0f);

            static float angle = 0.0f;
            ImGui::SliderAngle("slider angle", &angle);

            // Using the format string to display a name instead of an integer.
            // Here we completely omit '%d' from the format string, so it'll only display a name.
            // This technique can also be used with DragInt().
            enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };
            static int elem = Element_Fire;
            const char* elems_names[Element_COUNT] = { "Fire", "Earth", "Air", "Water" };
            const char* elem_name = (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
            ImGui::SliderInt("slider enum", &elem, 0, Element_COUNT - 1, elem_name);
            ImGui::SameLine(); HelpMarker("Using the format string parameter to display a name instead of the underlying integer.");
        }

        {
            static float col1[3] = { 1.0f,0.0f,0.2f };
            static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
            ImGui::ColorEdit3("color 1", col1);
            ImGui::SameLine(); HelpMarker(
                "Click on the colored square to open a color picker.\n"
                "Click and hold to use drag and drop.\n"
                "Right-click on the colored square to show options.\n"
                "CTRL+click on individual component to input value.\n");

            ImGui::ColorEdit4("color 2", col2);
        }

        {
            // List box
            const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
            static int item_current = 1;
            ImGui::ListBox("listbox\n(single select)", &item_current, items, IM_ARRAYSIZE(items), 4);

            //static int listbox_item_current2 = 2;
            //ImGui::SetNextItemWidth(-1);
            //ImGui::ListBox("##listbox2", &listbox_item_current2, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
        }

        ImGui::TreePop();
    }

    // Testing ImGuiOnceUponAFrame helper.
    //static ImGuiOnceUponAFrame once;
    //for (int i = 0; i < 5; i++)
    //    if (once)
    //        ImGui::Text("This will be displayed only once.");

    if (ImGui::TreeNode("Trees"))
    {
        if (ImGui::TreeNode("Basic trees"))
        {
            for (int i = 0; i < 5; i++)
            {
                // Use SetNextItemOpen() so set the default state of a node to be open. We could
                // also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
                if (i == 0)
                    ImGui::SetNextItemOpen(true, ImGuiCond_Once);

                if (ImGui::TreeNode((void*)(intptr_t)i, "Child %d", i))
                {
                    ImGui::Text("blah blah");
                    ImGui::SameLine();
                    if (ImGui::SmallButton("button")) {}
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Advanced, with Selectable nodes"))
        {
            HelpMarker(
                "This is a more typical looking tree with selectable nodes.\n"
                "Click to select, CTRL+Click to toggle, click on arrows or double-click to open.");
            static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            static bool align_label_with_current_x_position = false;
            static bool test_drag_and_drop = false;
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_OpenOnArrow);
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_SpanAvailWidth); ImGui::SameLine(); HelpMarker("Extend hit area to all available width instead of allowing more items to be laid out after the node.");
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_SpanFullWidth);
            ImGui::Checkbox("Align label with current X position", &align_label_with_current_x_position);
            ImGui::Checkbox("Test tree node as drag source", &test_drag_and_drop);
            ImGui::Text("Hello!");
            if (align_label_with_current_x_position)
                ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

            // 'selection_mask' is dumb representation of what may be user-side selection state.
            //  You may retain selection state inside or outside your objects in whatever format you see fit.
            // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
            /// of the loop. May be a pointer to your own node type, etc.
            static int selection_mask = (1 << 2);
            int node_clicked = -1;
            for (int i = 0; i < 6; i++)
            {
                // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
                ImGuiTreeNodeFlags node_flags = base_flags;
                const bool is_selected = (selection_mask & (1 << i)) != 0;
                if (is_selected)
                    node_flags |= ImGuiTreeNodeFlags_Selected;
                if (i < 3)
                {
                    // Items 0..2 are Tree Node
                    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                    if (ImGui::IsItemClicked())
                        node_clicked = i;
                    if (test_drag_and_drop && ImGui::BeginDragDropSource())
                    {
                        ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                        ImGui::Text("This is a drag and drop source");
                        ImGui::EndDragDropSource();
                    }
                    if (node_open)
                    {
                        ImGui::BulletText("Blah blah\nBlah Blah");
                        ImGui::TreePop();
                    }
                } else
                {
                    // Items 3..5 are Tree Leaves
                    // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
                    // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
                    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                    ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
                    if (ImGui::IsItemClicked())
                        node_clicked = i;
                    if (test_drag_and_drop && ImGui::BeginDragDropSource())
                    {
                        ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                        ImGui::Text("This is a drag and drop source");
                        ImGui::EndDragDropSource();
                    }
                }
            }
            if (node_clicked != -1)
            {
                // Update selection state
                // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                if (ImGui::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select
            }
            if (align_label_with_current_x_position)
                ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Collapsing Headers"))
    {
        static bool closable_group = true;
        ImGui::Checkbox("Show 2nd header", &closable_group);
        if (ImGui::CollapsingHeader("Header", ImGuiTreeNodeFlags_None))
        {
            ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
            for (int i = 0; i < 5; i++)
                ImGui::Text("Some content %d", i);
        }
        if (ImGui::CollapsingHeader("Header with a close button", &closable_group))
        {
            ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
            for (int i = 0; i < 5; i++)
                ImGui::Text("More content %d", i);
        }
        /*
        if (ImGui::CollapsingHeader("Header with a bullet", ImGuiTreeNodeFlags_Bullet))
            ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
        */
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Bullets"))
    {
        ImGui::BulletText("Bullet point 1");
        ImGui::BulletText("Bullet point 2\nOn multiple lines");
        if (ImGui::TreeNode("Tree node"))
        {
            ImGui::BulletText("Another bullet point");
            ImGui::TreePop();
        }
        ImGui::Bullet(); ImGui::Text("Bullet point 3 (two calls)");
        ImGui::Bullet(); ImGui::SmallButton("Button");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Text"))
    {
        if (ImGui::TreeNode("Colored Text"))
        {
            // Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Pink");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Yellow");
            ImGui::TextDisabled("Disabled");
            ImGui::SameLine(); HelpMarker("The TextDisabled color is stored in ImGuiStyle.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Word Wrapping"))
        {
            // Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
            ImGui::TextWrapped("This text should automatically wrap on the edge of the window. The current implementation for text wrapping follows simple rules suitable for English and possibly other languages.");
            ImGui::Spacing();

            static float wrap_width = 200.0f;
            ImGui::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            for (int n = 0; n < 2; n++)
            {
                ImGui::Text("Test paragraph %d:", n);
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec2 marker_min = ImVec2(pos.x + wrap_width, pos.y);
                ImVec2 marker_max = ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight());
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
                if (n == 0)
                    ImGui::Text("The lazy dog is a good dog. This paragraph should fit within %.0f pixels. Testing a 1 character word. The quick brown fox jumps over the lazy dog.", wrap_width);
                if (n == 1)
                    ImGui::Text("aaaaaaaa bbbbbbbb, c cccccccc,dddddddd. d eeeeeeee   ffffffff. gggggggg!hhhhhhhh");

                // Draw actual text bounding box, following by marker of our expected limit (should not overlap!)
                draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
                draw_list->AddRectFilled(marker_min, marker_max, IM_COL32(255, 0, 255, 255));
                ImGui::PopTextWrapPos();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("UTF-8 Text"))
        {
            // UTF-8 test with Japanese characters
            // (Needs a suitable font? Try "Google Noto" or "Arial Unicode". See docs/FONTS.md for details.)
            // - From C++11 you can use the u8"my text" syntax to encode literal strings as UTF-8
            // - For earlier compiler, you may be able to encode your sources as UTF-8 (e.g. in Visual Studio, you
            //   can save your source files as 'UTF-8 without signature').
            // - FOR THIS DEMO FILE ONLY, BECAUSE WE WANT TO SUPPORT OLD COMPILERS, WE ARE *NOT* INCLUDING RAW UTF-8
            //   CHARACTERS IN THIS SOURCE FILE. Instead we are encoding a few strings with hexadecimal constants.
            //   Don't do this in your application! Please use u8"text in any language" in your application!
            // Note that characters values are preserved even by InputText() if the font cannot be displayed,
            // so you can safely copy & paste garbled characters into another application.
            ImGui::TextWrapped(
                "CJK text will only appears if the font was loaded with the appropriate CJK character ranges. "
                "Call io.Font->AddFontFromFileTTF() manually to load extra character ranges. "
                "Read docs/FONTS.md for details.");
            ImGui::Text("Hiragana: \xe3\x81\x8b\xe3\x81\x8d\xe3\x81\x8f\xe3\x81\x91\xe3\x81\x93 (kakikukeko)"); // Normally we would use u8"blah blah" with the proper characters directly in the string.
            ImGui::Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
            static char buf[32] = "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
            //static char buf[32] = u8"NIHONGO"; // <- this is how you would write it with C++11, using real kanjis
            ImGui::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Images"))
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::TextWrapped("Below we are displaying the font texture (which is the only texture we have access to in this demo). Use the 'ImTextureID' type as storage to pass pointers or identifier to your own texture data. Hover the texture for a zoomed view!");

        // Below we are displaying the font texture because it is the only texture we have access to inside the demo!
        // Remember that ImTextureID is just storage for whatever you want it to be. It is essentially a value that
        // will be passed to the rendering back-end via the ImDrawCmd structure.
        // If you use one of the default imgui_impl_XXXX.cpp rendering back-end, they all have comments at the top
        // of their respective source file to specify what they expect to be stored in ImTextureID, for example:
        // - The imgui_impl_dx11.cpp renderer expect a 'ID3D11ShaderResourceView*' pointer
        // - The imgui_impl_opengl3.cpp renderer expect a GLuint OpenGL texture identifier, etc.
        // More:
        // - If you decided that ImTextureID = MyEngineTexture*, then you can pass your MyEngineTexture* pointers
        //   to ImGui::Image(), and gather width/height through your own functions, etc.
        // - You can use ShowMetricsWindow() to inspect the draw data that are being passed to your renderer,
        //   it will help you debug issues if you are confused about it.
        // - Consider using the lower-level ImDrawList::AddImage() API, via ImGui::GetWindowDrawList()->AddImage().
        // - Read https://github.com/ocornut/imgui/blob/master/docs/FAQ.md
        // - Read https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
        ImTextureID my_tex_id = io.Fonts->TexID;
        float my_tex_w = (float)io.Fonts->TexWidth;
        float my_tex_h = (float)io.Fonts->TexHeight;
        {
            ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
            ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
            ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                float region_sz = 32.0f;
                float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
                float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
                float zoom = 4.0f;
                if (region_x < 0.0f) { region_x = 0.0f; } else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
                if (region_y < 0.0f) { region_y = 0.0f; } else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
                ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
                ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
                ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
                ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
                ImGui::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
                ImGui::EndTooltip();
            }
        }
        ImGui::TextWrapped("And now some textured buttons..");
        static int pressed_count = 0;
        for (int i = 0; i < 8; i++)
        {
            ImGui::PushID(i);
            int frame_padding = -1 + i;                             // -1 == uses default padding (style.FramePadding)
            ImVec2 size = ImVec2(32.0f, 32.0f);                     // Size of the image we want to make visible
            ImVec2 uv0 = ImVec2(0.0f, 0.0f);                        // UV coordinates for lower-left
            ImVec2 uv1 = ImVec2(32.0f / my_tex_w, 32 / my_tex_h);   // UV coordinates for (32,32) in our texture
            ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint
            if (ImGui::ImageButton(my_tex_id, size, uv0, uv1, frame_padding, bg_col, tint_col))
                pressed_count += 1;
            ImGui::PopID();
            ImGui::SameLine();
        }
        ImGui::NewLine();
        ImGui::Text("Pressed %d times.", pressed_count);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Combo"))
    {
        // Expose flags as checkbox for the demo
        static ImGuiComboFlags flags = 0;
        ImGui::CheckboxFlags("ImGuiComboFlags_PopupAlignLeft", (unsigned int*)&flags, ImGuiComboFlags_PopupAlignLeft);
        ImGui::SameLine(); HelpMarker("Only makes a difference if the popup is larger than the combo");
        if (ImGui::CheckboxFlags("ImGuiComboFlags_NoArrowButton", (unsigned int*)&flags, ImGuiComboFlags_NoArrowButton))
            flags &= ~ImGuiComboFlags_NoPreview;     // Clear the other flag, as we cannot combine both
        if (ImGui::CheckboxFlags("ImGuiComboFlags_NoPreview", (unsigned int*)&flags, ImGuiComboFlags_NoPreview))
            flags &= ~ImGuiComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both

        // Using the generic BeginCombo() API, you have full control over how to display the combo contents.
        // (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
        // stored in the object itself, etc.)
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static int item_current_idx = 0;                    // Here our selection data is an index.
        const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically could be anything)(
        if (ImGui::BeginCombo("combo 1", combo_label, flags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(items[n], is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // Simplified one-liner Combo() API, using values packed in a single constant string
        static int item_current_2 = 0;
        ImGui::Combo("combo 2 (one-liner)", &item_current_2, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

        // Simplified one-liner Combo() using an array of const char*
        static int item_current_3 = -1; // If the selection isn't within 0..count, Combo won't display a preview
        ImGui::Combo("combo 3 (array)", &item_current_3, items, IM_ARRAYSIZE(items));

        // Simplified one-liner Combo() using an accessor function
        struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };
        static int item_current_4 = 0;
        ImGui::Combo("combo 4 (function)", &item_current_4, &FuncHolder::ItemGetter, items, IM_ARRAYSIZE(items));

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Selectables"))
    {
        // Selectable() has 2 overloads:
        // - The one taking "bool selected" as a read-only selection information.
        //   When Selectable() has been clicked it returns true and you can alter selection state accordingly.
        // - The one taking "bool* p_selected" as a read-write selection information (convenient in some cases)
        // The earlier is more flexible, as in real application your selection may be stored in many different ways
        // and not necessarily inside a bool value (e.g. in flags within objects, as an external list, etc).
        if (ImGui::TreeNode("Basic"))
        {
            static bool selection[5] = { false, true, false, false, false };
            ImGui::Selectable("1. I am selectable", &selection[0]);
            ImGui::Selectable("2. I am selectable", &selection[1]);
            ImGui::Text("3. I am not selectable");
            ImGui::Selectable("4. I am selectable", &selection[3]);
            if (ImGui::Selectable("5. I am double clickable", selection[4], ImGuiSelectableFlags_AllowDoubleClick))
                if (ImGui::IsMouseDoubleClicked(0))
                    selection[4] = !selection[4];
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Selection State: Single Selection"))
        {
            static int selected = -1;
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (ImGui::Selectable(buf, selected == n))
                    selected = n;
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Selection State: Multiple Selection"))
        {
            HelpMarker("Hold CTRL and click to select multiple items.");
            static bool selection[5] = { false, false, false, false, false };
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (ImGui::Selectable(buf, selection[n]))
                {
                    if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                        memset(selection, 0, sizeof(selection));
                    selection[n] ^= 1;
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Rendering more text into the same line"))
        {
            // Using the Selectable() override that takes "bool* p_selected" parameter,
            // this function toggle your bool value automatically.
            static bool selected[3] = { false, false, false };
            ImGui::Selectable("main.c", &selected[0]); ImGui::SameLine(300); ImGui::Text(" 2,345 bytes");
            ImGui::Selectable("Hello.cpp", &selected[1]); ImGui::SameLine(300); ImGui::Text("12,345 bytes");
            ImGui::Selectable("Hello.h", &selected[2]); ImGui::SameLine(300); ImGui::Text(" 2,345 bytes");
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("In columns"))
        {
            ImGui::Columns(3, NULL, false);
            static bool selected[16] = {};
            for (int i = 0; i < 16; i++)
            {
                char label[32]; sprintf(label, "Item %d", i);
                if (ImGui::Selectable(label, &selected[i])) {}
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Grid"))
        {
            static int selected[4 * 4] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
            for (int i = 0; i < 4 * 4; i++)
            {
                ImGui::PushID(i);
                if (ImGui::Selectable("Sailor", selected[i] != 0, 0, ImVec2(50, 50)))
                {
                    // Toggle
                    selected[i] = !selected[i];

                    // Note: We _unnecessarily_ test for both x/y and i here only to silence some static analyzer.
                    // The second part of each test is unnecessary.
                    int x = i % 4;
                    int y = i / 4;
                    if (x > 0) { selected[i - 1] ^= 1; }
                    if (x < 3 && i < 15) { selected[i + 1] ^= 1; }
                    if (y > 0 && i > 3) { selected[i - 4] ^= 1; }
                    if (y < 3 && i < 12) { selected[i + 4] ^= 1; }
                }
                if ((i % 4) < 3) ImGui::SameLine();
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Alignment"))
        {
            HelpMarker(
                "By default, Selectables uses style.SelectableTextAlign but it can be overridden on a per-item "
                "basis using PushStyleVar(). You'll probably want to always keep your default situation to "
                "left-align otherwise it becomes difficult to layout multiple items on a same line");
            static bool selected[3 * 3] = { true, false, true, false, true, false, true, false, true };
            for (int y = 0; y < 3; y++)
            {
                for (int x = 0; x < 3; x++)
                {
                    ImVec2 alignment = ImVec2((float)x / 2.0f, (float)y / 2.0f);
                    char name[32];
                    sprintf(name, "(%.1f,%.1f)", alignment.x, alignment.y);
                    if (x > 0) ImGui::SameLine();
                    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, alignment);
                    ImGui::Selectable(name, &selected[3 * y + x], ImGuiSelectableFlags_None, ImVec2(80, 80));
                    ImGui::PopStyleVar();
                }
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    // To wire InputText() with std::string or any other custom string type,
    // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/imgui_stdlib.h file.
    if (ImGui::TreeNode("Text Input"))
    {
        if (ImGui::TreeNode("Multi-line Text Input"))
        {
            // Note: we are using a fixed-sized buffer for simplicity here. See ImGuiInputTextFlags_CallbackResize
            // and the code in misc/cpp/imgui_stdlib.h for how to setup InputText() for dynamically resizing strings.
            static char text[1024 * 16] =
                "/*\n"
                " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
                " the hexadecimal encoding of one offending instruction,\n"
                " more formally, the invalid operand with locked CMPXCHG8B\n"
                " instruction bug, is a design flaw in the majority of\n"
                " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
                " processors (all in the P5 microarchitecture).\n"
                "*/\n\n"
                "label:\n"
                "\tlock cmpxchg8b eax\n";

            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            HelpMarker("You can use the ImGuiInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/imgui_stdlib.h for an example. (This is not demonstrated in imgui_demo.cpp because we don't want to include <string> in here)");
            ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", (unsigned int*)&flags, ImGuiInputTextFlags_ReadOnly);
            ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", (unsigned int*)&flags, ImGuiInputTextFlags_AllowTabInput);
            ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine", (unsigned int*)&flags, ImGuiInputTextFlags_CtrlEnterForNewLine);
            ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Filtered Text Input"))
        {
            struct TextFilters
            {
                // Return 0 (pass) if the character is 'i' or 'm' or 'g' or 'u' or 'i'
                static int FilterImGuiLetters(ImGuiInputTextCallbackData* data)
                {
                    if (data->EventChar < 256 && strchr("imgui", (char)data->EventChar))
                        return 0;
                    return 1;
                }
            };

            static char buf1[64] = ""; ImGui::InputText("default", buf1, 64);
            static char buf2[64] = ""; ImGui::InputText("decimal", buf2, 64, ImGuiInputTextFlags_CharsDecimal);
            static char buf3[64] = ""; ImGui::InputText("hexadecimal", buf3, 64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
            static char buf4[64] = ""; ImGui::InputText("uppercase", buf4, 64, ImGuiInputTextFlags_CharsUppercase);
            static char buf5[64] = ""; ImGui::InputText("no blank", buf5, 64, ImGuiInputTextFlags_CharsNoBlank);
            static char buf6[64] = ""; ImGui::InputText("\"imgui\" letters", buf6, 64, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterImGuiLetters);

            ImGui::Text("Password input");
            static char password[64] = "password123";
            ImGui::InputText("password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);
            ImGui::SameLine(); HelpMarker("Display all characters as '*'.\nDisable clipboard cut and copy.\nDisable logging.\n");
            ImGui::InputTextWithHint("password (w/ hint)", "<password>", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);
            ImGui::InputText("password (clear)", password, IM_ARRAYSIZE(password));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Resize Callback"))
        {
            // To wire InputText() with std::string or any other custom string type,
            // you can use the ImGuiInputTextFlags_CallbackResize flag + create a custom ImGui::InputText() wrapper
            // using your preferred type. See misc/cpp/imgui_stdlib.h for an implementation of this using std::string.
            HelpMarker(
                "Using ImGuiInputTextFlags_CallbackResize to wire your custom string type to InputText().\n\n"
                "See misc/cpp/imgui_stdlib.h for an implementation of this for std::string.");
            struct Funcs
            {
                static int MyResizeCallback(ImGuiInputTextCallbackData* data)
                {
                    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                    {
                        ImVector<char>* my_str = (ImVector<char>*)data->UserData;
                        IM_ASSERT(my_str->begin() == data->Buf);
                        my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
                        data->Buf = my_str->begin();
                    }
                    return 0;
                }

                // Note: Because ImGui:: is a namespace you would typically add your own function into the namespace.
                // For example, you code may declare a function 'ImGui::InputText(const char* label, MyString* my_str)'
                static bool MyInputTextMultiline(const char* label, ImVector<char>* my_str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0)
                {
                    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
                    return ImGui::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, Funcs::MyResizeCallback, (void*)my_str);
                }
            };

            // For this demo we are using ImVector as a string container.
            // Note that because we need to store a terminating zero character, our size/capacity are 1 more
            // than usually reported by a typical string class.
            static ImVector<char> my_str;
            if (my_str.empty())
                my_str.push_back(0);
            Funcs::MyInputTextMultiline("##MyStr", &my_str, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
            ImGui::Text("Data: %p\nSize: %d\nCapacity: %d", (void*)my_str.begin(), my_str.size(), my_str.capacity());
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    // Plot/Graph widgets are currently fairly limited.
    // Consider writing your own plotting widget, or using a third-party one
    // (for third-party Plot widgets, see 'Wiki->Useful Widgets' or https://github.com/ocornut/imgui/labels/plot%2Fgraph)
    if (ImGui::TreeNode("Plots Widgets"))
    {
        static bool animate = true;
        ImGui::Checkbox("Animate", &animate);

        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

        // Create a dummy array of contiguous float values to plot
        // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float
        // and the sizeof() of your structure in the "stride" parameter.
        static float values[90] = {};
        static int values_offset = 0;
        static double refresh_time = 0.0;
        if (!animate || refresh_time == 0.0)
            refresh_time = ImGui::GetTime();
        while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 60 Hz rate for the demo
        {
            static float phase = 0.0f;
            values[values_offset] = cosf(phase);
            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            phase += 0.10f * values_offset;
            refresh_time += 1.0f / 60.0f;
        }

        // Plots can display overlay texts
        // (in this example, we will display an average value)
        {
            float average = 0.0f;
            for (int n = 0; n < IM_ARRAYSIZE(values); n++)
                average += values[n];
            average /= (float)IM_ARRAYSIZE(values);
            char overlay[32];
            sprintf(overlay, "avg %f", average);
            ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        }
        ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));

        // Use functions to generate output
        // FIXME: This is rather awkward because current plot API only pass in indices.
        // We probably want an API passing floats and user provide sample rate/count.
        struct Funcs
        {
            static float Sin(void*, int i) { return sinf(i * 0.1f); }
            static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
        };
        static int func_type = 0, display_count = 70;
        ImGui::Separator();
        ImGui::SetNextItemWidth(100);
        ImGui::Combo("func", &func_type, "Sin\0Saw\0");
        ImGui::SameLine();
        ImGui::SliderInt("Sample count", &display_count, 1, 400);
        float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
        ImGui::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        ImGui::Separator();

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        if (animate)
        {
            progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
            if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
            if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
        }

        // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
        // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Progress Bar");

        float progress_saturated = IM_CLAMP(progress, 0.0f, 1.0f);
        char buf[32];
        sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
        ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Color/Picker Widgets"))
    {
        static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        ImGui::Checkbox("With Alpha Preview", &alpha_preview);
        ImGui::Checkbox("With Half Alpha Preview", &alpha_half_preview);
        ImGui::Checkbox("With Drag and Drop", &drag_and_drop);
        ImGui::Checkbox("With Options Menu", &options_menu); ImGui::SameLine(); HelpMarker("Right-click on the individual color widget to show options.");
        ImGui::Checkbox("With HDR", &hdr); ImGui::SameLine(); HelpMarker("Currently all this does is to lift the 0..1 limits on dragging widgets.");
        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

        ImGui::Text("Color widget:");
        ImGui::SameLine(); HelpMarker(
            "Click on the colored square to open a color picker.\n"
            "CTRL+click on individual component to input value.\n");
        ImGui::ColorEdit3("MyColor##1", (float*)&color, misc_flags);

        ImGui::Text("Color widget HSV with Alpha:");
        ImGui::ColorEdit4("MyColor##2", (float*)&color, ImGuiColorEditFlags_DisplayHSV | misc_flags);

        ImGui::Text("Color widget with Float Display:");
        ImGui::ColorEdit4("MyColor##2f", (float*)&color, ImGuiColorEditFlags_Float | misc_flags);

        ImGui::Text("Color button with Picker:");
        ImGui::SameLine(); HelpMarker(
            "With the ImGuiColorEditFlags_NoInputs flag you can hide all the slider/text inputs.\n"
            "With the ImGuiColorEditFlags_NoLabel flag you can pass a non-empty label which will only "
            "be used for the tooltip and picker popup.");
        ImGui::ColorEdit4("MyColor##3", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);

        ImGui::Text("Color button with Custom Picker Popup:");

        // Generate a dummy default palette. The palette will persist and can be edited.
        static bool saved_palette_init = true;
        static ImVec4 saved_palette[32] = {};
        if (saved_palette_init)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                    saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
            saved_palette_init = false;
        }

        static ImVec4 backup_color;
        bool open_popup = ImGui::ColorButton("MyColor##3b", color, misc_flags);
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        open_popup |= ImGui::Button("Palette");
        if (open_popup)
        {
            ImGui::OpenPopup("mypicker");
            backup_color = color;
        }
        if (ImGui::BeginPopup("mypicker"))
        {
            ImGui::Text("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
            ImGui::Separator();
            ImGui::ColorPicker4("##picker", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
            ImGui::SameLine();

            ImGui::BeginGroup(); // Lock X position
            ImGui::Text("Current");
            ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
            ImGui::Text("Previous");
            if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                color = backup_color;
            ImGui::Separator();
            ImGui::Text("Palette");
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                ImGui::PushID(n);
                if ((n % 8) != 0)
                    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

                ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
                if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
                    color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

                // Allow user to drop colors into each palette entry. Note that ColorButton() is already a
                // drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                    ImGui::EndDragDropTarget();
                }

                ImGui::PopID();
            }
            ImGui::EndGroup();
            ImGui::EndPopup();
        }

        ImGui::Text("Color button only:");
        static bool no_border = false;
        ImGui::Checkbox("ImGuiColorEditFlags_NoBorder", &no_border);
        ImGui::ColorButton("MyColor##3c", *(ImVec4*)&color, misc_flags | (no_border ? ImGuiColorEditFlags_NoBorder : 0), ImVec2(80, 80));

        ImGui::Text("Color picker:");
        static bool alpha = true;
        static bool alpha_bar = true;
        static bool side_preview = true;
        static bool ref_color = false;
        static ImVec4 ref_color_v(1.0f, 0.0f, 1.0f, 0.5f);
        static int display_mode = 0;
        static int picker_mode = 0;
        ImGui::Checkbox("With Alpha", &alpha);
        ImGui::Checkbox("With Alpha Bar", &alpha_bar);
        ImGui::Checkbox("With Side Preview", &side_preview);
        if (side_preview)
        {
            ImGui::SameLine();
            ImGui::Checkbox("With Ref Color", &ref_color);
            if (ref_color)
            {
                ImGui::SameLine();
                ImGui::ColorEdit4("##RefColor", &ref_color_v.x, ImGuiColorEditFlags_NoInputs | misc_flags);
            }
        }
        ImGui::Combo("Display Mode", &display_mode, "Auto/Current\0None\0RGB Only\0HSV Only\0Hex Only\0");
        ImGui::SameLine(); HelpMarker(
            "ColorEdit defaults to displaying RGB inputs if you don't specify a display mode, "
            "but the user can change it with a right-click.\n\nColorPicker defaults to displaying RGB+HSV+Hex "
            "if you don't specify a display mode.\n\nYou can change the defaults using SetColorEditOptions().");
        ImGui::Combo("Picker Mode", &picker_mode, "Auto/Current\0Hue bar + SV rect\0Hue wheel + SV triangle\0");
        ImGui::SameLine(); HelpMarker("User can right-click the picker to change mode.");
        ImGuiColorEditFlags flags = misc_flags;
        if (!alpha)            flags |= ImGuiColorEditFlags_NoAlpha;        // This is by default if you call ColorPicker3() instead of ColorPicker4()
        if (alpha_bar)         flags |= ImGuiColorEditFlags_AlphaBar;
        if (!side_preview)     flags |= ImGuiColorEditFlags_NoSidePreview;
        if (picker_mode == 1)  flags |= ImGuiColorEditFlags_PickerHueBar;
        if (picker_mode == 2)  flags |= ImGuiColorEditFlags_PickerHueWheel;
        if (display_mode == 1) flags |= ImGuiColorEditFlags_NoInputs;       // Disable all RGB/HSV/Hex displays
        if (display_mode == 2) flags |= ImGuiColorEditFlags_DisplayRGB;     // Override display mode
        if (display_mode == 3) flags |= ImGuiColorEditFlags_DisplayHSV;
        if (display_mode == 4) flags |= ImGuiColorEditFlags_DisplayHex;
        ImGui::ColorPicker4("MyColor##4", (float*)&color, flags, ref_color ? &ref_color_v.x : NULL);

        ImGui::Text("Set defaults in code:");
        ImGui::SameLine(); HelpMarker(
            "SetColorEditOptions() is designed to allow you to set boot-time default.\n"
            "We don't have Push/Pop functions because you can force options on a per-widget basis if needed,"
            "and the user can change non-forced ones with the options menu.\nWe don't have a getter to avoid"
            "encouraging you to persistently save values that aren't forward-compatible.");
        if (ImGui::Button("Default: Uint8 + HSV + Hue Bar"))
            ImGui::SetColorEditOptions(ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_PickerHueBar);
        if (ImGui::Button("Default: Float + HDR + Hue Wheel"))
            ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);

        // HSV encoded support (to avoid RGB<>HSV round trips and singularities when S==0 or V==0)
        static ImVec4 color_hsv(0.23f, 1.0f, 1.0f, 1.0f); // Stored as HSV!
        ImGui::Spacing();
        ImGui::Text("HSV encoded colors");
        ImGui::SameLine(); HelpMarker(
            "By default, colors are given to ColorEdit and ColorPicker in RGB, but ImGuiColorEditFlags_InputHSV"
            "allows you to store colors as HSV and pass them to ColorEdit and ColorPicker as HSV. This comes with the"
            "added benefit that you can manipulate hue values with the picker even when saturation or value are zero.");
        ImGui::Text("Color widget with InputHSV:");
        ImGui::ColorEdit4("HSV shown as RGB##1", (float*)&color_hsv, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
        ImGui::ColorEdit4("HSV shown as HSV##1", (float*)&color_hsv, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
        ImGui::DragFloat4("Raw HSV values", (float*)&color_hsv, 0.01f, 0.0f, 1.0f);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Range Widgets"))
    {
        static float begin = 10, end = 90;
        static int begin_i = 100, end_i = 1000;
        ImGui::DragFloatRange2("range", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");
        ImGui::DragIntRange2("range int (no bounds)", &begin_i, &end_i, 5, 0, 0, "Min: %d units", "Max: %d units");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Data Types"))
    {
        // DragScalar/InputScalar/SliderScalar functions allow various data types
        // - signed/unsigned
        // - 8/16/32/64-bits
        // - integer/float/double
        // To avoid polluting the public API with all possible combinations, we use the ImGuiDataType enum
        // to pass the type, and passing all arguments by pointer.
        // This is the reason the test code below creates local variables to hold "zero" "one" etc. for each types.
        // In practice, if you frequently use a given type that is not covered by the normal API entry points,
        // you can wrap it yourself inside a 1 line function which can take typed argument as value instead of void*,
        // and then pass their address to the generic function. For example:
        //   bool MySliderU64(const char *label, u64* value, u64 min = 0, u64 max = 0, const char* format = "%lld")
        //   {
        //      return SliderScalar(label, ImGuiDataType_U64, value, &min, &max, format);
        //   }

        // Setup limits (as helper variables so we can take their address, as explained above)
        // Note: SliderScalar() functions have a maximum usable range of half the natural type maximum, hence the /2.
#ifndef LLONG_MIN
        ImS64 LLONG_MIN = -9223372036854775807LL - 1;
        ImS64 LLONG_MAX = 9223372036854775807LL;
        ImU64 ULLONG_MAX = (2ULL * 9223372036854775807LL + 1);
#endif
        const char    s8_zero = 0, s8_one = 1, s8_fifty = 50, s8_min = -128, s8_max = 127;
        const ImU8    u8_zero = 0, u8_one = 1, u8_fifty = 50, u8_min = 0, u8_max = 255;
        const short   s16_zero = 0, s16_one = 1, s16_fifty = 50, s16_min = -32768, s16_max = 32767;
        const ImU16   u16_zero = 0, u16_one = 1, u16_fifty = 50, u16_min = 0, u16_max = 65535;
        const ImS32   s32_zero = 0, s32_one = 1, s32_fifty = 50, s32_min = INT_MIN / 2, s32_max = INT_MAX / 2, s32_hi_a = INT_MAX / 2 - 100, s32_hi_b = INT_MAX / 2;
        const ImU32   u32_zero = 0, u32_one = 1, u32_fifty = 50, u32_min = 0, u32_max = UINT_MAX / 2, u32_hi_a = UINT_MAX / 2 - 100, u32_hi_b = UINT_MAX / 2;
        const ImS64   s64_zero = 0, s64_one = 1, s64_fifty = 50, s64_min = LLONG_MIN / 2, s64_max = LLONG_MAX / 2, s64_hi_a = LLONG_MAX / 2 - 100, s64_hi_b = LLONG_MAX / 2;
        const ImU64   u64_zero = 0, u64_one = 1, u64_fifty = 50, u64_min = 0, u64_max = ULLONG_MAX / 2, u64_hi_a = ULLONG_MAX / 2 - 100, u64_hi_b = ULLONG_MAX / 2;
        const float   f32_zero = 0.f, f32_one = 1.f, f32_lo_a = -10000000000.0f, f32_hi_a = +10000000000.0f;
        const double  f64_zero = 0., f64_one = 1., f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

        // State
        static char   s8_v = 127;
        static ImU8   u8_v = 255;
        static short  s16_v = 32767;
        static ImU16  u16_v = 65535;
        static ImS32  s32_v = -1;
        static ImU32  u32_v = (ImU32)-1;
        static ImS64  s64_v = -1;
        static ImU64  u64_v = (ImU64)-1;
        static float  f32_v = 0.123f;
        static double f64_v = 90000.01234567890123456789;

        const float drag_speed = 0.2f;
        static bool drag_clamp = false;
        ImGui::Text("Drags:");
        ImGui::Checkbox("Clamp integers to 0..50", &drag_clamp); ImGui::SameLine(); HelpMarker("As with every widgets in dear imgui, we never modify values unless there is a user interaction.\nYou can override the clamping limits by using CTRL+Click to input a value.");
        ImGui::DragScalar("drag s8", ImGuiDataType_S8, &s8_v, drag_speed, drag_clamp ? &s8_zero : NULL, drag_clamp ? &s8_fifty : NULL);
        ImGui::DragScalar("drag u8", ImGuiDataType_U8, &u8_v, drag_speed, drag_clamp ? &u8_zero : NULL, drag_clamp ? &u8_fifty : NULL, "%u ms");
        ImGui::DragScalar("drag s16", ImGuiDataType_S16, &s16_v, drag_speed, drag_clamp ? &s16_zero : NULL, drag_clamp ? &s16_fifty : NULL);
        ImGui::DragScalar("drag u16", ImGuiDataType_U16, &u16_v, drag_speed, drag_clamp ? &u16_zero : NULL, drag_clamp ? &u16_fifty : NULL, "%u ms");
        ImGui::DragScalar("drag s32", ImGuiDataType_S32, &s32_v, drag_speed, drag_clamp ? &s32_zero : NULL, drag_clamp ? &s32_fifty : NULL);
        ImGui::DragScalar("drag u32", ImGuiDataType_U32, &u32_v, drag_speed, drag_clamp ? &u32_zero : NULL, drag_clamp ? &u32_fifty : NULL, "%u ms");
        ImGui::DragScalar("drag s64", ImGuiDataType_S64, &s64_v, drag_speed, drag_clamp ? &s64_zero : NULL, drag_clamp ? &s64_fifty : NULL);
        ImGui::DragScalar("drag u64", ImGuiDataType_U64, &u64_v, drag_speed, drag_clamp ? &u64_zero : NULL, drag_clamp ? &u64_fifty : NULL);
        ImGui::DragScalar("drag float", ImGuiDataType_Float, &f32_v, 0.005f, &f32_zero, &f32_one, "%f", 1.0f);
        ImGui::DragScalar("drag float ^2", ImGuiDataType_Float, &f32_v, 0.005f, &f32_zero, &f32_one, "%f", 2.0f); ImGui::SameLine(); HelpMarker("You can use the 'power' parameter to increase tweaking precision on one side of the range.");
        ImGui::DragScalar("drag double", ImGuiDataType_Double, &f64_v, 0.0005f, &f64_zero, NULL, "%.10f grams", 1.0f);
        ImGui::DragScalar("drag double ^2", ImGuiDataType_Double, &f64_v, 0.0005f, &f64_zero, &f64_one, "0 < %.10f < 1", 2.0f);

        ImGui::Text("Sliders");
        ImGui::SliderScalar("slider s8 full", ImGuiDataType_S8, &s8_v, &s8_min, &s8_max, "%d");
        ImGui::SliderScalar("slider u8 full", ImGuiDataType_U8, &u8_v, &u8_min, &u8_max, "%u");
        ImGui::SliderScalar("slider s16 full", ImGuiDataType_S16, &s16_v, &s16_min, &s16_max, "%d");
        ImGui::SliderScalar("slider u16 full", ImGuiDataType_U16, &u16_v, &u16_min, &u16_max, "%u");
        ImGui::SliderScalar("slider s32 low", ImGuiDataType_S32, &s32_v, &s32_zero, &s32_fifty, "%d");
        ImGui::SliderScalar("slider s32 high", ImGuiDataType_S32, &s32_v, &s32_hi_a, &s32_hi_b, "%d");
        ImGui::SliderScalar("slider s32 full", ImGuiDataType_S32, &s32_v, &s32_min, &s32_max, "%d");
        ImGui::SliderScalar("slider u32 low", ImGuiDataType_U32, &u32_v, &u32_zero, &u32_fifty, "%u");
        ImGui::SliderScalar("slider u32 high", ImGuiDataType_U32, &u32_v, &u32_hi_a, &u32_hi_b, "%u");
        ImGui::SliderScalar("slider u32 full", ImGuiDataType_U32, &u32_v, &u32_min, &u32_max, "%u");
        ImGui::SliderScalar("slider s64 low", ImGuiDataType_S64, &s64_v, &s64_zero, &s64_fifty, "%I64d");
        ImGui::SliderScalar("slider s64 high", ImGuiDataType_S64, &s64_v, &s64_hi_a, &s64_hi_b, "%I64d");
        ImGui::SliderScalar("slider s64 full", ImGuiDataType_S64, &s64_v, &s64_min, &s64_max, "%I64d");
        ImGui::SliderScalar("slider u64 low", ImGuiDataType_U64, &u64_v, &u64_zero, &u64_fifty, "%I64u ms");
        ImGui::SliderScalar("slider u64 high", ImGuiDataType_U64, &u64_v, &u64_hi_a, &u64_hi_b, "%I64u ms");
        ImGui::SliderScalar("slider u64 full", ImGuiDataType_U64, &u64_v, &u64_min, &u64_max, "%I64u ms");
        ImGui::SliderScalar("slider float low", ImGuiDataType_Float, &f32_v, &f32_zero, &f32_one);
        ImGui::SliderScalar("slider float low^2", ImGuiDataType_Float, &f32_v, &f32_zero, &f32_one, "%.10f", 2.0f);
        ImGui::SliderScalar("slider float high", ImGuiDataType_Float, &f32_v, &f32_lo_a, &f32_hi_a, "%e");
        ImGui::SliderScalar("slider double low", ImGuiDataType_Double, &f64_v, &f64_zero, &f64_one, "%.10f grams", 1.0f);
        ImGui::SliderScalar("slider double low^2", ImGuiDataType_Double, &f64_v, &f64_zero, &f64_one, "%.10f", 2.0f);
        ImGui::SliderScalar("slider double high", ImGuiDataType_Double, &f64_v, &f64_lo_a, &f64_hi_a, "%e grams", 1.0f);

        static bool inputs_step = true;
        ImGui::Text("Inputs");
        ImGui::Checkbox("Show step buttons", &inputs_step);
        ImGui::InputScalar("input s8", ImGuiDataType_S8, &s8_v, inputs_step ? &s8_one : NULL, NULL, "%d");
        ImGui::InputScalar("input u8", ImGuiDataType_U8, &u8_v, inputs_step ? &u8_one : NULL, NULL, "%u");
        ImGui::InputScalar("input s16", ImGuiDataType_S16, &s16_v, inputs_step ? &s16_one : NULL, NULL, "%d");
        ImGui::InputScalar("input u16", ImGuiDataType_U16, &u16_v, inputs_step ? &u16_one : NULL, NULL, "%u");
        ImGui::InputScalar("input s32", ImGuiDataType_S32, &s32_v, inputs_step ? &s32_one : NULL, NULL, "%d");
        ImGui::InputScalar("input s32 hex", ImGuiDataType_S32, &s32_v, inputs_step ? &s32_one : NULL, NULL, "%08X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("input u32", ImGuiDataType_U32, &u32_v, inputs_step ? &u32_one : NULL, NULL, "%u");
        ImGui::InputScalar("input u32 hex", ImGuiDataType_U32, &u32_v, inputs_step ? &u32_one : NULL, NULL, "%08X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("input s64", ImGuiDataType_S64, &s64_v, inputs_step ? &s64_one : NULL);
        ImGui::InputScalar("input u64", ImGuiDataType_U64, &u64_v, inputs_step ? &u64_one : NULL);
        ImGui::InputScalar("input float", ImGuiDataType_Float, &f32_v, inputs_step ? &f32_one : NULL);
        ImGui::InputScalar("input double", ImGuiDataType_Double, &f64_v, inputs_step ? &f64_one : NULL);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Multi-component Widgets"))
    {
        static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
        static int vec4i[4] = { 1, 5, 100, 255 };

        ImGui::InputFloat2("input float2", vec4f);
        ImGui::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
        ImGui::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
        ImGui::InputInt2("input int2", vec4i);
        ImGui::DragInt2("drag int2", vec4i, 1, 0, 255);
        ImGui::SliderInt2("slider int2", vec4i, 0, 255);
        ImGui::Spacing();

        ImGui::InputFloat3("input float3", vec4f);
        ImGui::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
        ImGui::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
        ImGui::InputInt3("input int3", vec4i);
        ImGui::DragInt3("drag int3", vec4i, 1, 0, 255);
        ImGui::SliderInt3("slider int3", vec4i, 0, 255);
        ImGui::Spacing();

        ImGui::InputFloat4("input float4", vec4f);
        ImGui::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
        ImGui::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
        ImGui::InputInt4("input int4", vec4i);
        ImGui::DragInt4("drag int4", vec4i, 1, 0, 255);
        ImGui::SliderInt4("slider int4", vec4i, 0, 255);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Vertical Sliders"))
    {
        const float spacing = 4;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

        static int int_value = 0;
        ImGui::VSliderInt("##int", ImVec2(18, 160), &int_value, 0, 5);
        ImGui::SameLine();

        static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
        ImGui::PushID("set1");
        for (int i = 0; i < 7; i++)
        {
            if (i > 0) ImGui::SameLine();
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(i / 7.0f, 0.5f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(i / 7.0f, 0.9f, 0.9f));
            ImGui::VSliderFloat("##v", ImVec2(18, 160), &values[i], 0.0f, 1.0f, "");
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
        const ImVec2 small_slider_size(18, (float)(int)((160.0f - (rows - 1) * spacing) / rows));
        for (int nx = 0; nx < 4; nx++)
        {
            if (nx > 0) ImGui::SameLine();
            ImGui::BeginGroup();
            for (int ny = 0; ny < rows; ny++)
            {
                ImGui::PushID(nx * rows + ny);
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
            ImGui::VSliderFloat("##v", ImVec2(40, 160), &values[i], 0.0f, 1.0f, "%.2f\nsec");
            ImGui::PopStyleVar();
            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Drag and Drop"))
    {
        if (ImGui::TreeNode("Drag and drop in standard widgets"))
        {
            // ColorEdit widgets automatically act as drag source and drag target.
            // They are using standardized payload strings IMGUI_PAYLOAD_TYPE_COLOR_3F and IMGUI_PAYLOAD_TYPE_COLOR_4F
            // to allow your own widgets to use colors in their drag and drop interaction.
            // Also see 'Demo->Widgets->Color/Picker Widgets->Palette' demo.
            HelpMarker("You can drag from the colored squares.");
            static float col1[3] = { 1.0f, 0.0f, 0.2f };
            static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            ImGui::ColorEdit3("color 1", col1);
            ImGui::ColorEdit4("color 2", col2);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Drag and drop to copy/swap items"))
        {
            enum Mode
            {
                Mode_Copy,
                Mode_Move,
                Mode_Swap
            };
            static int mode = 0;
            if (ImGui::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } ImGui::SameLine();
            if (ImGui::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } ImGui::SameLine();
            if (ImGui::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; }
            const char* names[9] =
            {
                "Bobby", "Beatrice", "Betty",
                "Brianna", "Barry", "Bernard",
                "Bibi", "Blaine", "Bryn"
            };
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                ImGui::PushID(n);
                if ((n % 3) != 0)
                    ImGui::SameLine();
                ImGui::Button(names[n], ImVec2(60, 60));

                // Our buttons are both drag sources and drag targets here!
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    // Set payload to carry the index of our item (could be anything)
                    ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

                    // Display preview (could be anything, e.g. when dragging an image we could decide to display
                    // the filename and a small preview of the image, etc.)
                    if (mode == Mode_Copy) { ImGui::Text("Copy %s", names[n]); }
                    if (mode == Mode_Move) { ImGui::Text("Move %s", names[n]); }
                    if (mode == Mode_Swap) { ImGui::Text("Swap %s", names[n]); }
                    ImGui::EndDragDropSource();
                }
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        if (mode == Mode_Copy)
                        {
                            names[n] = names[payload_n];
                        }
                        if (mode == Mode_Move)
                        {
                            names[n] = names[payload_n];
                            names[payload_n] = "";
                        }
                        if (mode == Mode_Swap)
                        {
                            const char* tmp = names[n];
                            names[n] = names[payload_n];
                            names[payload_n] = tmp;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Drag to reorder items (simple)"))
        {
            // Simple reordering
            HelpMarker(
                "We don't use the drag and drop api at all here! "
                "Instead we query when the item is held but not hovered, and order items accordingly.");
            static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
            for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
            {
                const char* item = item_names[n];
                ImGui::Selectable(item);

                if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
                {
                    int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (n_next >= 0 && n_next < IM_ARRAYSIZE(item_names))
                    {
                        item_names[n] = item_names[n_next];
                        item_names[n_next] = item;
                        ImGui::ResetMouseDragDelta();
                    }
                }
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Querying Status (Active/Focused/Hovered etc.)"))
    {
        // Select an item type
        const char* item_names[] =
        {
            "Text", "Button", "Button (w/ repeat)", "Checkbox", "SliderFloat", "InputText", "InputFloat",
            "InputFloat3", "ColorEdit4", "MenuItem", "TreeNode", "TreeNode (w/ double-click)", "ListBox"
        };
        static int item_type = 1;
        ImGui::Combo("Item Type", &item_type, item_names, IM_ARRAYSIZE(item_names), IM_ARRAYSIZE(item_names));
        ImGui::SameLine();
        HelpMarker("Testing how various types of items are interacting with the IsItemXXX functions.");

        // Submit selected item item so we can query their status in the code following it.
        bool ret = false;
        static bool b = false;
        static float col4f[4] = { 1.0f, 0.5, 0.0f, 1.0f };
        static char str[16] = {};
        if (item_type == 0) { ImGui::Text("ITEM: Text"); }                                              // Testing text items with no identifier/interaction
        if (item_type == 1) { ret = ImGui::Button("ITEM: Button"); }                                    // Testing button
        if (item_type == 2) { ImGui::PushButtonRepeat(true); ret = ImGui::Button("ITEM: Button"); ImGui::PopButtonRepeat(); } // Testing button (with repeater)
        if (item_type == 3) { ret = ImGui::Checkbox("ITEM: Checkbox", &b); }                            // Testing checkbox
        if (item_type == 4) { ret = ImGui::SliderFloat("ITEM: SliderFloat", &col4f[0], 0.0f, 1.0f); }   // Testing basic item
        if (item_type == 5) { ret = ImGui::InputText("ITEM: InputText", &str[0], IM_ARRAYSIZE(str)); }  // Testing input text (which handles tabbing)
        if (item_type == 6) { ret = ImGui::InputFloat("ITEM: InputFloat", col4f, 1.0f); }               // Testing +/- buttons on scalar input
        if (item_type == 7) { ret = ImGui::InputFloat3("ITEM: InputFloat3", col4f); }                   // Testing multi-component items (IsItemXXX flags are reported merged)
        if (item_type == 8) { ret = ImGui::ColorEdit4("ITEM: ColorEdit4", col4f); }                     // Testing multi-component items (IsItemXXX flags are reported merged)
        if (item_type == 9) { ret = ImGui::MenuItem("ITEM: MenuItem"); }                                // Testing menu item (they use ImGuiButtonFlags_PressedOnRelease button policy)
        if (item_type == 10) { ret = ImGui::TreeNode("ITEM: TreeNode"); if (ret) ImGui::TreePop(); }     // Testing tree node
        if (item_type == 11) { ret = ImGui::TreeNodeEx("ITEM: TreeNode w/ ImGuiTreeNodeFlags_OpenOnDoubleClick", ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_NoTreePushOnOpen); } // Testing tree node with ImGuiButtonFlags_PressedOnDoubleClick button policy.
        if (item_type == 12) { const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi" }; static int current = 1; ret = ImGui::ListBox("ITEM: ListBox", &current, items, IM_ARRAYSIZE(items), IM_ARRAYSIZE(items)); }

        // Display the values of IsItemHovered() and other common item state functions.
        // Note that the ImGuiHoveredFlags_XXX flags can be combined.
        // Because BulletText is an item itself and that would affect the output of IsItemXXX functions,
        // we query every state in a single call to avoid storing them and to simplify the code.
        ImGui::BulletText(
            "Return value = %d\n"
            "IsItemFocused() = %d\n"
            "IsItemHovered() = %d\n"
            "IsItemHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsItemHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsItemHovered(_AllowWhenOverlapped) = %d\n"
            "IsItemHovered(_RectOnly) = %d\n"
            "IsItemActive() = %d\n"
            "IsItemEdited() = %d\n"
            "IsItemActivated() = %d\n"
            "IsItemDeactivated() = %d\n"
            "IsItemDeactivatedAfterEdit() = %d\n"
            "IsItemVisible() = %d\n"
            "IsItemClicked() = %d\n"
            "IsItemToggledOpen() = %d\n"
            "GetItemRectMin() = (%.1f, %.1f)\n"
            "GetItemRectMax() = (%.1f, %.1f)\n"
            "GetItemRectSize() = (%.1f, %.1f)",
            ret,
            ImGui::IsItemFocused(),
            ImGui::IsItemHovered(),
            ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup),
            ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem),
            ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped),
            ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly),
            ImGui::IsItemActive(),
            ImGui::IsItemEdited(),
            ImGui::IsItemActivated(),
            ImGui::IsItemDeactivated(),
            ImGui::IsItemDeactivatedAfterEdit(),
            ImGui::IsItemVisible(),
            ImGui::IsItemClicked(),
            ImGui::IsItemToggledOpen(),
            ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y,
            ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y,
            ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y
        );

        static bool embed_all_inside_a_child_window = false;
        ImGui::Checkbox("Embed everything inside a child window (for additional testing)", &embed_all_inside_a_child_window);
        if (embed_all_inside_a_child_window)
            ImGui::BeginChild("outer_child", ImVec2(0, ImGui::GetFontSize() * 20.0f), true);

        // Testing IsWindowFocused() function with its various flags.
        // Note that the ImGuiFocusedFlags_XXX flags can be combined.
        ImGui::BulletText(
            "IsWindowFocused() = %d\n"
            "IsWindowFocused(_ChildWindows) = %d\n"
            "IsWindowFocused(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowFocused(_RootWindow) = %d\n"
            "IsWindowFocused(_AnyWindow) = %d\n",
            ImGui::IsWindowFocused(),
            ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows),
            ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_RootWindow),
            ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow),
            ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow));

        // Testing IsWindowHovered() function with its various flags.
        // Note that the ImGuiHoveredFlags_XXX flags can be combined.
        ImGui::BulletText(
            "IsWindowHovered() = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsWindowHovered(_ChildWindows) = %d\n"
            "IsWindowHovered(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowHovered(_ChildWindows|_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_RootWindow) = %d\n"
            "IsWindowHovered(_AnyWindow) = %d\n",
            ImGui::IsWindowHovered(),
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup),
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem),
            ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows),
            ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_RootWindow),
            ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByPopup),
            ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow),
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow));

        ImGui::BeginChild("child", ImVec2(0, 50), true);
        ImGui::Text("This is another child window for testing the _ChildWindows flag.");
        ImGui::EndChild();
        if (embed_all_inside_a_child_window)
            ImGui::EndChild();

        static char dummy_str[] = "This is a dummy field to be able to tab-out of the widgets above.";
        ImGui::InputText("dummy", dummy_str, IM_ARRAYSIZE(dummy_str), ImGuiInputTextFlags_ReadOnly);

        // Calling IsItemHovered() after begin returns the hovered status of the title bar.
        // This is useful in particular if you want to create a context menu associated to the title bar of a window.
        static bool test_window = false;
        ImGui::Checkbox("Hovered/Active tests after Begin() for title bar testing", &test_window);
        if (test_window)
        {
            ImGui::Begin("Title bar Hovered/Active tests", &test_window);
            if (ImGui::BeginPopupContextItem()) // <-- This is using IsItemHovered()
            {
                if (ImGui::MenuItem("Close")) { test_window = false; }
                ImGui::EndPopup();
            }
            ImGui::Text(
                "IsItemHovered() after begin = %d (== is title bar hovered)\n"
                "IsItemActive() after begin = %d (== is window being clicked/moved)\n",
                ImGui::IsItemHovered(), ImGui::IsItemActive());
            ImGui::End();
        }

        ImGui::TreePop();
    }
}

static void ShowDemoWindowLayout()
{
    if (!ImGui::CollapsingHeader("Layout"))
        return;

    if (ImGui::TreeNode("Child windows"))
    {
        HelpMarker("Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window.");
        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;
        ImGui::Checkbox("Disable Mouse Wheel", &disable_mouse_wheel);
        ImGui::Checkbox("Disable Menu", &disable_menu);

        static int line = 50;
        bool goto_line = ImGui::Button("Goto");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        goto_line |= ImGui::InputInt("##Line", &line, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);

        // Child 1: no border, enable horizontal scrollbar
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            if (disable_mouse_wheel)
                window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
            ImGui::BeginChild("ChildL", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 260), false, window_flags);
            for (int i = 0; i < 100; i++)
            {
                ImGui::Text("%04d: scrollable region", i);
                if (goto_line && line == i)
                    ImGui::SetScrollHereY();
            }
            if (goto_line && line >= 100)
                ImGui::SetScrollHereY();
            ImGui::EndChild();
        }

        ImGui::SameLine();

        // Child 2: rounded border
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
            if (disable_mouse_wheel)
                window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
            if (!disable_menu)
                window_flags |= ImGuiWindowFlags_MenuBar;
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild("ChildR", ImVec2(0, 260), true, window_flags);
            if (!disable_menu && ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    ShowExampleMenuFile();
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::Columns(2);
            for (int i = 0; i < 100; i++)
            {
                char buf[32];
                sprintf(buf, "%03d", i);
                ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                ImGui::NextColumn();
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();
        }

        ImGui::Separator();

        // Demonstrate a few extra things
        // - Changing ImGuiCol_ChildBg (which is transparent black in default styles)
        // - Using SetCursorPos() to position child window (the child window is an item from the POV of parent window)
        //   You can also call SetNextWindowPos() to position the child window. The parent window will effectively
        //   layout from this position.
        // - Using ImGui::GetItemRectMin/Max() to query the "item" state (because the child window is an item from
        //   the POV of the parent window). See 'Demo->Querying Status (Active/Focused/Hovered etc.)' for details.
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
            ImGui::BeginChild("Red", ImVec2(200, 100), true, ImGuiWindowFlags_None);
            for (int n = 0; n < 50; n++)
                ImGui::Text("Some test %d", n);
            ImGui::EndChild();
            ImVec2 child_rect_min = ImGui::GetItemRectMin();
            ImVec2 child_rect_max = ImGui::GetItemRectMax();
            ImGui::PopStyleColor();
            ImGui::Text("Rect of child window is: (%.0f,%.0f) (%.0f,%.0f)", child_rect_min.x, child_rect_min.y, child_rect_max.x, child_rect_max.y);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Widgets Width"))
    {
        // Use SetNextItemWidth() to set the width of a single upcoming item.
        // Use PushItemWidth()/PopItemWidth() to set the width of a group of items.
        // In real code use you'll probably want to choose width values that are proportional to your font size
        // e.g. Using '20.0f * GetFontSize()' as width instead of '200.0f', etc.

        static float f = 0.0f;
        ImGui::Text("SetNextItemWidth/PushItemWidth(100)");
        ImGui::SameLine(); HelpMarker("Fixed width.");
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("float##1", &f);

        ImGui::Text("SetNextItemWidth/PushItemWidth(GetWindowWidth() * 0.5f)");
        ImGui::SameLine(); HelpMarker("Half of window width.");
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
        ImGui::DragFloat("float##2", &f);

        ImGui::Text("SetNextItemWidth/PushItemWidth(GetContentRegionAvail().x * 0.5f)");
        ImGui::SameLine(); HelpMarker("Half of available width.\n(~ right-cursor_pos)\n(works within a column set)");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::DragFloat("float##3", &f);

        ImGui::Text("SetNextItemWidth/PushItemWidth(-100)");
        ImGui::SameLine(); HelpMarker("Align to right edge minus 100");
        ImGui::SetNextItemWidth(-100);
        ImGui::DragFloat("float##4", &f);

        // Demonstrate using PushItemWidth to surround three items.
        // Calling SetNextItemWidth() before each of them would have the same effect.
        ImGui::Text("SetNextItemWidth/PushItemWidth(-1)");
        ImGui::SameLine(); HelpMarker("Align to right edge");
        ImGui::PushItemWidth(-1);
        ImGui::DragFloat("##float5a", &f);
        ImGui::DragFloat("##float5b", &f);
        ImGui::DragFloat("##float5c", &f);
        ImGui::PopItemWidth();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Basic Horizontal Layout"))
    {
        ImGui::TextWrapped("(Use ImGui::SameLine() to keep adding items to the right of the preceding item)");

        // Text
        ImGui::Text("Two items: Hello"); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

        // Adjust spacing
        ImGui::Text("More spacing: Hello"); ImGui::SameLine(0, 20);
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

        // Button
        ImGui::AlignTextToFramePadding();
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
        static bool c1 = false, c2 = false, c3 = false, c4 = false;
        ImGui::Checkbox("My", &c1); ImGui::SameLine();
        ImGui::Checkbox("Tailor", &c2); ImGui::SameLine();
        ImGui::Checkbox("Is", &c3); ImGui::SameLine();
        ImGui::Checkbox("Rich", &c4);

        // Various
        static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
        ImGui::PushItemWidth(80);
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
        static int item = -1;
        ImGui::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); ImGui::SameLine();
        ImGui::SliderFloat("X", &f0, 0.0f, 5.0f); ImGui::SameLine();
        ImGui::SliderFloat("Y", &f1, 0.0f, 5.0f); ImGui::SameLine();
        ImGui::SliderFloat("Z", &f2, 0.0f, 5.0f);
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
        ImVec2 button_sz(40, 40);
        ImGui::Button("A", button_sz); ImGui::SameLine();
        ImGui::Dummy(button_sz); ImGui::SameLine();
        ImGui::Button("B", button_sz);

        // Manually wrapping
        // (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
        ImGui::Text("Manually wrapping:");
        ImGuiStyle& style = ImGui::GetStyle();
        int buttons_count = 20;
        float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        for (int n = 0; n < buttons_count; n++)
        {
            ImGui::PushID(n);
            ImGui::Button("Box", button_sz);
            float last_button_x2 = ImGui::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
            if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
                ImGui::SameLine();
            ImGui::PopID();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Tabs"))
    {
        if (ImGui::TreeNode("Basic"))
        {
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("Avocado"))
                {
                    ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Broccoli"))
                {
                    ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Cucumber"))
                {
                    ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::Separator();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Advanced & Close Button"))
        {
            // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
            static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
            ImGui::CheckboxFlags("ImGuiTabBarFlags_Reorderable", (unsigned int*)&tab_bar_flags, ImGuiTabBarFlags_Reorderable);
            ImGui::CheckboxFlags("ImGuiTabBarFlags_AutoSelectNewTabs", (unsigned int*)&tab_bar_flags, ImGuiTabBarFlags_AutoSelectNewTabs);
            ImGui::CheckboxFlags("ImGuiTabBarFlags_TabListPopupButton", (unsigned int*)&tab_bar_flags, ImGuiTabBarFlags_TabListPopupButton);
            ImGui::CheckboxFlags("ImGuiTabBarFlags_NoCloseWithMiddleMouseButton", (unsigned int*)&tab_bar_flags, ImGuiTabBarFlags_NoCloseWithMiddleMouseButton);
            if ((tab_bar_flags & ImGuiTabBarFlags_FittingPolicyMask_) == 0)
                tab_bar_flags |= ImGuiTabBarFlags_FittingPolicyDefault_;
            if (ImGui::CheckboxFlags("ImGuiTabBarFlags_FittingPolicyResizeDown", (unsigned int*)&tab_bar_flags, ImGuiTabBarFlags_FittingPolicyResizeDown))
                tab_bar_flags &= ~(ImGuiTabBarFlags_FittingPolicyMask_ ^ ImGuiTabBarFlags_FittingPolicyResizeDown);
            if (ImGui::CheckboxFlags("ImGuiTabBarFlags_FittingPolicyScroll", (unsigned int*)&tab_bar_flags, ImGuiTabBarFlags_FittingPolicyScroll))
                tab_bar_flags &= ~(ImGuiTabBarFlags_FittingPolicyMask_ ^ ImGuiTabBarFlags_FittingPolicyScroll);

            // Tab Bar
            const char* names[4] = { "Artichoke", "Beetroot", "Celery", "Daikon" };
            static bool opened[4] = { true, true, true, true }; // Persistent user state
            for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
            {
                if (n > 0) { ImGui::SameLine(); }
                ImGui::Checkbox(names[n], &opened[n]);
            }

            // Passing a bool* to BeginTabItem() is similar to passing one to Begin():
            // the underlying bool will be set to false when the tab is closed.
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                    if (opened[n] && ImGui::BeginTabItem(names[n], &opened[n], ImGuiTabItemFlags_None))
                    {
                        ImGui::Text("This is the %s tab!", names[n]);
                        if (n & 1)
                            ImGui::Text("I am an odd tab.");
                        ImGui::EndTabItem();
                    }
                ImGui::EndTabBar();
            }
            ImGui::Separator();
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Groups"))
    {
        HelpMarker(
            "BeginGroup() basically locks the horizontal position for new line. "
            "EndGroup() bundles the whole group so that you can use \"item\" functions such as "
            "IsItemHovered()/IsItemActive() or SameLine() etc. on the whole group.");
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
            ImGui::SameLine();
            ImGui::Button("EEE");
            ImGui::EndGroup();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("First group hovered");
        }
        // Capture the group size and create widgets using the same size
        ImVec2 size = ImGui::GetItemRectSize();
        const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
        ImGui::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

        ImGui::Button("ACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        ImGui::SameLine();
        ImGui::Button("REACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        ImGui::EndGroup();
        ImGui::SameLine();

        ImGui::Button("LEVERAGE\nBUZZWORD", size);
        ImGui::SameLine();

        if (ImGui::ListBoxHeader("List", size))
        {
            ImGui::Selectable("Selected", true);
            ImGui::Selectable("Not Selected", false);
            ImGui::ListBoxFooter();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Text Baseline Alignment"))
    {
        {
            ImGui::BulletText("Text baseline:");
            ImGui::SameLine(); HelpMarker(
                "This is testing the vertical alignment that gets applied on text to keep it aligned with widgets. "
                "Lines only composed of text or \"small\" widgets use less vertical space than lines with framed widgets.");
            ImGui::Indent();

            ImGui::Text("KO Blahblah"); ImGui::SameLine();
            ImGui::Button("Some framed item"); ImGui::SameLine();
            HelpMarker("Baseline of button will look misaligned with text..");

            // If your line starts with text, call AlignTextToFramePadding() to align text to upcoming widgets.
            // (because we don't know what's coming after the Text() statement, we need to move the text baseline
            // down by FramePadding.y ahead of time)
            ImGui::AlignTextToFramePadding();
            ImGui::Text("OK Blahblah"); ImGui::SameLine();
            ImGui::Button("Some framed item"); ImGui::SameLine();
            HelpMarker("We call AlignTextToFramePadding() to vertically align the text baseline by +FramePadding.y");

            // SmallButton() uses the same vertical padding as Text
            ImGui::Button("TEST##1"); ImGui::SameLine();
            ImGui::Text("TEST"); ImGui::SameLine();
            ImGui::SmallButton("TEST##2");

            // If your line starts with text, call AlignTextToFramePadding() to align text to upcoming widgets.
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Text aligned to framed item"); ImGui::SameLine();
            ImGui::Button("Item##1"); ImGui::SameLine();
            ImGui::Text("Item"); ImGui::SameLine();
            ImGui::SmallButton("Item##2"); ImGui::SameLine();
            ImGui::Button("Item##3");

            ImGui::Unindent();
        }

        ImGui::Spacing();

        {
            ImGui::BulletText("Multi-line text:");
            ImGui::Indent();
            ImGui::Text("One\nTwo\nThree"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Text("Banana"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("One\nTwo\nThree");

            ImGui::Button("HOP##1"); ImGui::SameLine();
            ImGui::Text("Banana"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Button("HOP##2"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");
            ImGui::Unindent();
        }

        ImGui::Spacing();

        {
            ImGui::BulletText("Misc items:");
            ImGui::Indent();

            // SmallButton() sets FramePadding to zero. Text baseline is aligned to match baseline of previous Button.
            ImGui::Button("80x80", ImVec2(80, 80));
            ImGui::SameLine();
            ImGui::Button("50x50", ImVec2(50, 50));
            ImGui::SameLine();
            ImGui::Button("Button()");
            ImGui::SameLine();
            ImGui::SmallButton("SmallButton()");

            // Tree
            const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
            ImGui::Button("Button##1");
            ImGui::SameLine(0.0f, spacing);
            if (ImGui::TreeNode("Node##1"))
            {
                // Dummy tree data
                for (int i = 0; i < 6; i++)
                    ImGui::BulletText("Item %d..", i);
                ImGui::TreePop();
            }

            // Vertically align text node a bit lower so it'll be vertically centered with upcoming widget.
            // Otherwise you can use SmallButton() (smaller fit).
            ImGui::AlignTextToFramePadding();

            // Common mistake to avoid: if we want to SameLine after TreeNode we need to do it before we add
            // other contents below the node.
            bool node_open = ImGui::TreeNode("Node##2");
            ImGui::SameLine(0.0f, spacing); ImGui::Button("Button##2");
            if (node_open)
            {
                // Dummy tree data
                for (int i = 0; i < 6; i++)
                    ImGui::BulletText("Item %d..", i);
                ImGui::TreePop();
            }

            // Bullet
            ImGui::Button("Button##3");
            ImGui::SameLine(0.0f, spacing);
            ImGui::BulletText("Bullet text");

            ImGui::AlignTextToFramePadding();
            ImGui::BulletText("Node");
            ImGui::SameLine(0.0f, spacing); ImGui::Button("Button##4");
            ImGui::Unindent();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Scrolling"))
    {
        // Vertical scroll functions
        HelpMarker("Use SetScrollHereY() or SetScrollFromPosY() to scroll to a given vertical position.");

        static int track_item = 50;
        static bool enable_track = true;
        static bool enable_extra_decorations = false;
        static float scroll_to_off_px = 0.0f;
        static float scroll_to_pos_px = 200.0f;

        ImGui::Checkbox("Decoration", &enable_extra_decorations);
        ImGui::SameLine();
        HelpMarker("We expose this for testing because scrolling sometimes had issues with window decoration such as menu-bars.");

        ImGui::Checkbox("Track", &enable_track);
        ImGui::PushItemWidth(100);
        ImGui::SameLine(140); enable_track |= ImGui::DragInt("##item", &track_item, 0.25f, 0, 99, "Item = %d");

        bool scroll_to_off = ImGui::Button("Scroll Offset");
        ImGui::SameLine(140); scroll_to_off |= ImGui::DragFloat("##off", &scroll_to_off_px, 1.00f, 0, FLT_MAX, "+%.0f px");

        bool scroll_to_pos = ImGui::Button("Scroll To Pos");
        ImGui::SameLine(140); scroll_to_pos |= ImGui::DragFloat("##pos", &scroll_to_pos_px, 1.00f, -10, FLT_MAX, "X/Y = %.0f px");
        ImGui::PopItemWidth();

        if (scroll_to_off || scroll_to_pos)
            enable_track = false;

        ImGuiStyle& style = ImGui::GetStyle();
        float child_w = (ImGui::GetContentRegionAvail().x - 4 * style.ItemSpacing.x) / 5;
        if (child_w < 1.0f)
            child_w = 1.0f;
        ImGui::PushID("##VerticalScrolling");
        for (int i = 0; i < 5; i++)
        {
            if (i > 0) ImGui::SameLine();
            ImGui::BeginGroup();
            const char* names[] = { "Top", "25%", "Center", "75%", "Bottom" };
            ImGui::TextUnformatted(names[i]);

            const ImGuiWindowFlags child_flags = enable_extra_decorations ? ImGuiWindowFlags_MenuBar : 0;
            const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)i);
            const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(child_w, 200.0f), true, child_flags);
            if (ImGui::BeginMenuBar())
            {
                ImGui::TextUnformatted("abc");
                ImGui::EndMenuBar();
            }
            if (scroll_to_off)
                ImGui::SetScrollY(scroll_to_off_px);
            if (scroll_to_pos)
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + scroll_to_pos_px, i * 0.25f);
            if (child_is_visible) // Avoid calling SetScrollHereY when running with culled items
            {
                for (int item = 0; item < 100; item++)
                {
                    if (enable_track && item == track_item)
                    {
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Item %d", item);
                        ImGui::SetScrollHereY(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
                    } else
                    {
                        ImGui::Text("Item %d", item);
                    }
                }
            }
            float scroll_y = ImGui::GetScrollY();
            float scroll_max_y = ImGui::GetScrollMaxY();
            ImGui::EndChild();
            ImGui::Text("%.0f/%.0f", scroll_y, scroll_max_y);
            ImGui::EndGroup();
        }
        ImGui::PopID();

        // Horizontal scroll functions
        ImGui::Spacing();
        HelpMarker(
            "Use SetScrollHereX() or SetScrollFromPosX() to scroll to a given horizontal position.\n\n"
            "Using the \"Scroll To Pos\" button above will make the discontinuity at edges visible: "
            "scrolling to the top/bottom/left/right-most item will add an additional WindowPadding to reflect "
            "on reaching the edge of the list.\n\nBecause the clipping rectangle of most window hides half "
            "worth of WindowPadding on the left/right, using SetScrollFromPosX(+1) will usually result in "
            "clipped text whereas the equivalent SetScrollFromPosY(+1) wouldn't.");
        ImGui::PushID("##HorizontalScrolling");
        for (int i = 0; i < 5; i++)
        {
            float child_height = ImGui::GetTextLineHeight() + style.ScrollbarSize + style.WindowPadding.y * 2.0f;
            ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar | (enable_extra_decorations ? ImGuiWindowFlags_AlwaysVerticalScrollbar : 0);
            ImGuiID child_id = ImGui::GetID((void*)(intptr_t)i);
            bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(-100, child_height), true, child_flags);
            if (scroll_to_off)
                ImGui::SetScrollX(scroll_to_off_px);
            if (scroll_to_pos)
                ImGui::SetScrollFromPosX(ImGui::GetCursorStartPos().x + scroll_to_pos_px, i * 0.25f);
            if (child_is_visible) // Avoid calling SetScrollHereY when running with culled items
            {
                for (int item = 0; item < 100; item++)
                {
                    if (enable_track && item == track_item)
                    {
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Item %d", item);
                        ImGui::SetScrollHereX(i * 0.25f); // 0.0f:left, 0.5f:center, 1.0f:right
                    } else
                    {
                        ImGui::Text("Item %d", item);
                    }
                    ImGui::SameLine();
                }
            }
            float scroll_x = ImGui::GetScrollX();
            float scroll_max_x = ImGui::GetScrollMaxX();
            ImGui::EndChild();
            ImGui::SameLine();
            const char* names[] = { "Left", "25%", "Center", "75%", "Right" };
            ImGui::Text("%s\n%.0f/%.0f", names[i], scroll_x, scroll_max_x);
            ImGui::Spacing();
        }
        ImGui::PopID();

        // Miscellaneous Horizontal Scrolling Demo
        HelpMarker(
            "Horizontal scrolling for a window is enabled via the ImGuiWindowFlags_HorizontalScrollbar flag.\n\n"
            "You may want to also explicitly specify content width by using SetNextWindowContentWidth() before Begin().");
        static int lines = 7;
        ImGui::SliderInt("Lines", &lines, 1, 15);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        ImVec2 scrolling_child_size = ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30);
        ImGui::BeginChild("scrolling", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar);
        for (int line = 0; line < lines; line++)
        {
            // Display random stuff. For the sake of this trivial demo we are using basic Button() + SameLine()
            // If you want to create your own time line for a real application you may be better off manipulating
            // the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position the widgets
            // yourself. You may also want to use the lower-level ImDrawList API.
            int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
            for (int n = 0; n < num_buttons; n++)
            {
                if (n > 0) ImGui::SameLine();
                ImGui::PushID(n + line * 1000);
                char num_buf[16];
                sprintf(num_buf, "%d", n);
                const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : num_buf;
                float hue = n * 0.05f;
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
                ImGui::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f));
                ImGui::PopStyleColor(3);
                ImGui::PopID();
            }
        }
        float scroll_x = ImGui::GetScrollX();
        float scroll_max_x = ImGui::GetScrollMaxX();
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        float scroll_x_delta = 0.0f;
        ImGui::SmallButton("<<");
        if (ImGui::IsItemActive())
            scroll_x_delta = -ImGui::GetIO().DeltaTime * 1000.0f;
        ImGui::SameLine();
        ImGui::Text("Scroll from code"); ImGui::SameLine();
        ImGui::SmallButton(">>");
        if (ImGui::IsItemActive())
            scroll_x_delta = +ImGui::GetIO().DeltaTime * 1000.0f;
        ImGui::SameLine();
        ImGui::Text("%.0f/%.0f", scroll_x, scroll_max_x);
        if (scroll_x_delta != 0.0f)
        {
            // Demonstrate a trick: you can use Begin to set yourself in the context of another window
            // (here we are already out of your child window)
            ImGui::BeginChild("scrolling");
            ImGui::SetScrollX(ImGui::GetScrollX() + scroll_x_delta);
            ImGui::EndChild();
        }
        ImGui::Spacing();

        static bool show_horizontal_contents_size_demo_window = false;
        ImGui::Checkbox("Show Horizontal contents size demo window", &show_horizontal_contents_size_demo_window);

        if (show_horizontal_contents_size_demo_window)
        {
            static bool show_h_scrollbar = true;
            static bool show_button = true;
            static bool show_tree_nodes = true;
            static bool show_text_wrapped = false;
            static bool show_columns = true;
            static bool show_tab_bar = true;
            static bool show_child = false;
            static bool explicit_content_size = false;
            static float contents_size_x = 300.0f;
            if (explicit_content_size)
                ImGui::SetNextWindowContentSize(ImVec2(contents_size_x, 0.0f));
            ImGui::Begin("Horizontal contents size demo window", &show_horizontal_contents_size_demo_window, show_h_scrollbar ? ImGuiWindowFlags_HorizontalScrollbar : 0);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 0));
            HelpMarker("Test of different widgets react and impact the work rectangle growing when horizontal scrolling is enabled.\n\nUse 'Metrics->Tools->Show windows rectangles' to visualize rectangles.");
            ImGui::Checkbox("H-scrollbar", &show_h_scrollbar);
            ImGui::Checkbox("Button", &show_button);            // Will grow contents size (unless explicitly overwritten)
            ImGui::Checkbox("Tree nodes", &show_tree_nodes);    // Will grow contents size and display highlight over full width
            ImGui::Checkbox("Text wrapped", &show_text_wrapped);// Will grow and use contents size
            ImGui::Checkbox("Columns", &show_columns);          // Will use contents size
            ImGui::Checkbox("Tab bar", &show_tab_bar);          // Will use contents size
            ImGui::Checkbox("Child", &show_child);              // Will grow and use contents size
            ImGui::Checkbox("Explicit content size", &explicit_content_size);
            ImGui::Text("Scroll %.1f/%.1f %.1f/%.1f", ImGui::GetScrollX(), ImGui::GetScrollMaxX(), ImGui::GetScrollY(), ImGui::GetScrollMaxY());
            if (explicit_content_size)
            {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::DragFloat("##csx", &contents_size_x);
                ImVec2 p = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 10, p.y + 10), IM_COL32_WHITE);
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x + contents_size_x - 10, p.y), ImVec2(p.x + contents_size_x, p.y + 10), IM_COL32_WHITE);
                ImGui::Dummy(ImVec2(0, 10));
            }
            ImGui::PopStyleVar(2);
            ImGui::Separator();
            if (show_button)
            {
                ImGui::Button("this is a 300-wide button", ImVec2(300, 0));
            }
            if (show_tree_nodes)
            {
                bool open = true;
                if (ImGui::TreeNode("this is a tree node"))
                {
                    if (ImGui::TreeNode("another one of those tree node..."))
                    {
                        ImGui::Text("Some tree contents");
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
                ImGui::CollapsingHeader("CollapsingHeader", &open);
            }
            if (show_text_wrapped)
            {
                ImGui::TextWrapped("This text should automatically wrap on the edge of the work rectangle.");
            }
            if (show_columns)
            {
                ImGui::Columns(4);
                for (int n = 0; n < 4; n++)
                {
                    ImGui::Text("Width %.2f", ImGui::GetColumnWidth());
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
            }
            if (show_tab_bar && ImGui::BeginTabBar("Hello"))
            {
                if (ImGui::BeginTabItem("OneOneOne")) { ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("TwoTwoTwo")) { ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("ThreeThreeThree")) { ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("FourFourFour")) { ImGui::EndTabItem(); }
                ImGui::EndTabBar();
            }
            if (show_child)
            {
                ImGui::BeginChild("child", ImVec2(0, 0), true);
                ImGui::EndChild();
            }
            ImGui::End();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Clipping"))
    {
        static ImVec2 size(100, 100), offset(50, 20);
        ImGui::TextWrapped(
            "On a per-widget basis we are occasionally clipping text CPU-side if it won't fit in its frame. "
            "Otherwise we are doing coarser clipping + passing a scissor rectangle to the renderer. "
            "The system is designed to try minimizing both execution and CPU/GPU rendering cost.");
        ImGui::DragFloat2("size", (float*)&size, 0.5f, 1.0f, 200.0f, "%.0f");
        ImGui::TextWrapped("(Click and drag)");
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec4 clip_rect(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
        ImGui::InvisibleButton("##dummy", size);
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
        {
            offset.x += ImGui::GetIO().MouseDelta.x;
            offset.y += ImGui::GetIO().MouseDelta.y;
        }
        ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(90, 90, 120, 255));
        ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize() * 2.0f, ImVec2(pos.x + offset.x, pos.y + offset.y), IM_COL32_WHITE, "Line 1 hello\nLine 2 clip me!", NULL, 0.0f, &clip_rect);
        ImGui::TreePop();
    }
}

static void ShowDemoWindowPopups()
{
    if (!ImGui::CollapsingHeader("Popups & Modal windows"))
        return;

    // The properties of popups windows are:
    // - They block normal mouse hovering detection outside them. (*)
    // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    // - Their visibility state (~bool) is held internally by Dear ImGui instead of being held by the programmer as
    //   we are used to with regular Begin() calls. User can manipulate the visibility state by calling OpenPopup().
    // (*) One can use IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even
    //     when normally blocked by a popup.
    // Those three properties are connected. The library needs to hold their visibility state BECAUSE it can close
    // popups at any time.

    // Typical use for regular windows:
    //   bool my_tool_is_active = false; if (ImGui::Button("Open")) my_tool_is_active = true; [...] if (my_tool_is_active) Begin("My Tool", &my_tool_is_active) { [...] } End();
    // Typical use for popups:
    //   if (ImGui::Button("Open")) ImGui::OpenPopup("MyPopup"); if (ImGui::BeginPopup("MyPopup") { [...] EndPopup(); }

    // With popups we have to go through a library call (here OpenPopup) to manipulate the visibility state.
    // This may be a bit confusing at first but it should quickly make sense. Follow on the examples below.

    if (ImGui::TreeNode("Popups"))
    {
        ImGui::TextWrapped(
            "When a popup is active, it inhibits interacting with windows that are behind the popup. "
            "Clicking outside the popup closes it.");

        static int selected_fish = -1;
        const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
        static bool toggles[] = { true, false, false, false, false };

        // Simple selection popup (if you want to show the current selection inside the Button itself,
        // you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
        if (ImGui::Button("Select.."))
            ImGui::OpenPopup("my_select_popup");
        ImGui::SameLine();
        ImGui::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
        if (ImGui::BeginPopup("my_select_popup"))
        {
            ImGui::Text("Aquarium");
            ImGui::Separator();
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                if (ImGui::Selectable(names[i]))
                    selected_fish = i;
            ImGui::EndPopup();
        }

        // Showing a menu with toggles
        if (ImGui::Button("Toggle.."))
            ImGui::OpenPopup("my_toggle_popup");
        if (ImGui::BeginPopup("my_toggle_popup"))
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
                    if (ImGui::Button("Stacked Popup"))
                        ImGui::OpenPopup("another popup");
                    if (ImGui::BeginPopup("another popup"))
                    {
                        ImGui::Text("I am the last one here.");
                        ImGui::EndPopup();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
            ImGui::EndPopup();
        }

        // Call the more complete ShowExampleMenuFile which we use in various places of this demo
        if (ImGui::Button("File Menu.."))
            ImGui::OpenPopup("my_file_popup");
        if (ImGui::BeginPopup("my_file_popup"))
        {
            ShowExampleMenuFile();
            ImGui::EndPopup();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Context menus"))
    {
        // BeginPopupContextItem() is a helper to provide common/simple popup behavior of essentially doing:
        //    if (IsItemHovered() && IsMouseReleased(0))
        //       OpenPopup(id);
        //    return BeginPopup(id);
        // For more advanced uses you may want to replicate and customize this code.
        // See details in BeginPopupContextItem().
        static float value = 0.5f;
        ImGui::Text("Value = %.3f (<-- right-click here)", value);
        if (ImGui::BeginPopupContextItem("item context menu"))
        {
            if (ImGui::Selectable("Set to zero")) value = 0.0f;
            if (ImGui::Selectable("Set to PI")) value = 3.1415f;
            ImGui::SetNextItemWidth(-1);
            ImGui::DragFloat("##Value", &value, 0.1f, 0.0f, 0.0f);
            ImGui::EndPopup();
        }

        // We can also use OpenPopupOnItemClick() which is the same as BeginPopupContextItem() but without the
        // Begin() call. So here we will make it that clicking on the text field with the right mouse button (1)
        // will toggle the visibility of the popup above.
        ImGui::Text("(You can also right-click me to open the same popup as above.)");
        ImGui::OpenPopupOnItemClick("item context menu", 1);

        // When used after an item that has an ID (e.g.Button), we can skip providing an ID to BeginPopupContextItem().
        // BeginPopupContextItem() will use the last item ID as the popup ID.
        // In addition here, we want to include your editable label inside the button label.
        // We use the ### operator to override the ID (read FAQ about ID for details)
        static char name[32] = "Label1";
        char buf[64];
        sprintf(buf, "Button: %s###Button", name); // ### operator override ID ignoring the preceding label
        ImGui::Button(buf);
        if (ImGui::BeginPopupContextItem())
        {
            ImGui::Text("Edit name:");
            ImGui::InputText("##edit", name, IM_ARRAYSIZE(name));
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        ImGui::SameLine(); ImGui::Text("(<-- right-click here)");

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Modals"))
    {
        ImGui::TextWrapped("Modal windows are like popups but the user cannot close them by clicking outside.");

        if (ImGui::Button("Delete.."))
            ImGui::OpenPopup("Delete?");

        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
            ImGui::Separator();

            //static int dummy_i = 0;
            //ImGui::Combo("Combo", &dummy_i, "Delete\0Delete harder\0");

            static bool dont_ask_me_next_time = false;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            ImGui::PopStyleVar();

            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        if (ImGui::Button("Stacked modals.."))
            ImGui::OpenPopup("Stacked 1");
        if (ImGui::BeginPopupModal("Stacked 1", NULL, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Dummy menu item")) {}
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::Text("Hello from Stacked The First\nUsing style.Colors[ImGuiCol_ModalWindowDimBg] behind it.");

            // Testing behavior of widgets stacking their own regular popups over the modal.
            static int item = 1;
            static float color[4] = { 0.4f,0.7f,0.0f,0.5f };
            ImGui::Combo("Combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
            ImGui::ColorEdit4("color", color);

            if (ImGui::Button("Add another modal.."))
                ImGui::OpenPopup("Stacked 2");

            // Also demonstrate passing a bool* to BeginPopupModal(), this will create a regular close button which
            // will close the popup. Note that the visibility state of popups is owned by imgui, so the input value
            // of the bool actually doesn't matter here.
            bool dummy_open = true;
            if (ImGui::BeginPopupModal("Stacked 2", &dummy_open))
            {
                ImGui::Text("Hello from Stacked The Second!");
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

    if (ImGui::TreeNode("Menus inside a regular window"))
    {
        ImGui::TextWrapped("Below we are testing adding menu items to a regular window. It's rather unusual but should work!");
        ImGui::Separator();

        // Note: As a quirk in this very specific example, we want to differentiate the parent of this menu from the
        // parent of the various popup menus above. To do so we are encloding the items in a PushID()/PopID() block
        // to make them two different menusets. If we don't, opening any popup above and hovering our menu here would
        // open it. This is because once a menu is active, we allow to switch to a sibling menu by just hovering on it,
        // which is the desired behavior for regular menus.
        ImGui::PushID("foo");
        ImGui::MenuItem("Menu item", "CTRL+M");
        if (ImGui::BeginMenu("Menu inside a regular window"))
        {
            ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        ImGui::PopID();
        ImGui::Separator();
        ImGui::TreePop();
    }
}

static void ShowDemoWindowColumns()
{
    if (!ImGui::CollapsingHeader("Columns"))
        return;

    ImGui::PushID("Columns");

    static bool disable_indent = false;
    ImGui::Checkbox("Disable tree indentation", &disable_indent);
    ImGui::SameLine();
    HelpMarker("Disable the indenting of tree nodes so demo columns can use the full window width.");
    if (disable_indent)
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);

    // Basic columns
    if (ImGui::TreeNode("Basic"))
    {
        ImGui::Text("Without border:");
        ImGui::Columns(3, "mycolumns3", false);  // 3-ways, no border
        ImGui::Separator();
        for (int n = 0; n < 14; n++)
        {
            char label[32];
            sprintf(label, "Item %d", n);
            if (ImGui::Selectable(label)) {}
            //if (ImGui::Button(label, ImVec2(-FLT_MIN,0.0f))) {}
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::Text("With border:");
        ImGui::Columns(4, "mycolumns"); // 4-ways, with border
        ImGui::Separator();
        ImGui::Text("ID"); ImGui::NextColumn();
        ImGui::Text("Name"); ImGui::NextColumn();
        ImGui::Text("Path"); ImGui::NextColumn();
        ImGui::Text("Hovered"); ImGui::NextColumn();
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
            bool hovered = ImGui::IsItemHovered();
            ImGui::NextColumn();
            ImGui::Text(names[i]); ImGui::NextColumn();
            ImGui::Text(paths[i]); ImGui::NextColumn();
            ImGui::Text("%d", hovered); ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Borders"))
    {
        // NB: Future columns API should allow automatic horizontal borders.
        static bool h_borders = true;
        static bool v_borders = true;
        static int columns_count = 4;
        const int lines_count = 3;
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
        ImGui::DragInt("##columns_count", &columns_count, 0.1f, 2, 10, "%d columns");
        if (columns_count < 2)
            columns_count = 2;
        ImGui::SameLine();
        ImGui::Checkbox("horizontal", &h_borders);
        ImGui::SameLine();
        ImGui::Checkbox("vertical", &v_borders);
        ImGui::Columns(columns_count, NULL, v_borders);
        for (int i = 0; i < columns_count * lines_count; i++)
        {
            if (h_borders && ImGui::GetColumnIndex() == 0)
                ImGui::Separator();
            ImGui::Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
            ImGui::Text("Width %.2f", ImGui::GetColumnWidth());
            ImGui::Text("Avail %.2f", ImGui::GetContentRegionAvail().x);
            ImGui::Text("Offset %.2f", ImGui::GetColumnOffset());
            ImGui::Text("Long text that is likely to clip");
            ImGui::Button("Button", ImVec2(-FLT_MIN, 0.0f));
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        if (h_borders)
            ImGui::Separator();
        ImGui::TreePop();
    }

    // Create multiple items in a same cell before switching to next column
    if (ImGui::TreeNode("Mixed items"))
    {
        ImGui::Columns(3, "mixed");
        ImGui::Separator();

        ImGui::Text("Hello");
        ImGui::Button("Banana");
        ImGui::NextColumn();

        ImGui::Text("ImGui");
        ImGui::Button("Apple");
        static float foo = 1.0f;
        ImGui::InputFloat("red", &foo, 0.05f, 0, "%.3f");
        ImGui::Text("An extra line here.");
        ImGui::NextColumn();

        ImGui::Text("Sailor");
        ImGui::Button("Corniflower");
        static float bar = 1.0f;
        ImGui::InputFloat("blue", &bar, 0.05f, 0, "%.3f");
        ImGui::NextColumn();

        if (ImGui::CollapsingHeader("Category A")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
        if (ImGui::CollapsingHeader("Category B")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
        if (ImGui::CollapsingHeader("Category C")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::TreePop();
    }

    // Word wrapping
    if (ImGui::TreeNode("Word-wrapping"))
    {
        ImGui::Columns(2, "word-wrapping");
        ImGui::Separator();
        ImGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
        ImGui::TextWrapped("Hello Left");
        ImGui::NextColumn();
        ImGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
        ImGui::TextWrapped("Hello Right");
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::TreePop();
    }

    // Scrolling columns
    /*
    if (ImGui::TreeNode("Vertical Scrolling"))
    {
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
        ImGui::TreePop();
    }
    */

    if (ImGui::TreeNode("Horizontal Scrolling"))
    {
        ImGui::SetNextWindowContentSize(ImVec2(1500.0f, 0.0f));
        ImVec2 child_size = ImVec2(0, ImGui::GetFontSize() * 20.0f);
        ImGui::BeginChild("##ScrollingRegion", child_size, false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Columns(10);
        int ITEMS_COUNT = 2000;
        ImGuiListClipper clipper(ITEMS_COUNT);  // Also demonstrate using the clipper for large list
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                for (int j = 0; j < 10; j++)
                {
                    ImGui::Text("Line %d Column %d...", i, j);
                    ImGui::NextColumn();
                }
        }
        ImGui::Columns(1);
        ImGui::EndChild();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Tree"))
    {
        ImGui::Columns(2, "tree", true);
        for (int x = 0; x < 3; x++)
        {
            bool open1 = ImGui::TreeNode((void*)(intptr_t)x, "Node%d", x);
            ImGui::NextColumn();
            ImGui::Text("Node contents");
            ImGui::NextColumn();
            if (open1)
            {
                for (int y = 0; y < 3; y++)
                {
                    bool open2 = ImGui::TreeNode((void*)(intptr_t)y, "Node%d.%d", x, y);
                    ImGui::NextColumn();
                    ImGui::Text("Node contents");
                    if (open2)
                    {
                        ImGui::Text("Even more contents");
                        if (ImGui::TreeNode("Tree in column"))
                        {
                            ImGui::Text("The quick brown fox jumps over the lazy dog");
                            ImGui::TreePop();
                        }
                    }
                    ImGui::NextColumn();
                    if (open2)
                        ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }
        ImGui::Columns(1);
        ImGui::TreePop();
    }

    if (disable_indent)
        ImGui::PopStyleVar();
    ImGui::PopID();
}

static void ShowDemoWindowMisc()
{
    if (ImGui::CollapsingHeader("Filtering"))
    {
        // Helper class to easy setup a text filter.
        // You may want to implement a more feature-full filtering scheme in your own application.
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

    if (ImGui::CollapsingHeader("Inputs, Navigation & Focus"))
    {
        ImGuiIO& io = ImGui::GetIO();

        // Display ImGuiIO output flags
        ImGui::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
        ImGui::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
        ImGui::Text("WantTextInput: %d", io.WantTextInput);
        ImGui::Text("WantSetMousePos: %d", io.WantSetMousePos);
        ImGui::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

        // Display Keyboard/Mouse state
        if (ImGui::TreeNode("Keyboard, Mouse & Navigation State"))
        {
            if (ImGui::IsMousePosValid())
                ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                ImGui::Text("Mouse pos: <INVALID>");
            ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            ImGui::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            ImGui::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("Mouse dblclick:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDoubleClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseReleased(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);

            ImGui::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
            ImGui::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyPressed(i)) { ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i); }
            ImGui::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyReleased(i)) { ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i); }
            ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
            ImGui::Text("Chars queue:");    for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; ImGui::SameLine();  ImGui::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

            ImGui::Text("NavInputs down:");     for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputs[i] > 0.0f) { ImGui::SameLine(); ImGui::Text("[%d] %.2f", i, io.NavInputs[i]); }
            ImGui::Text("NavInputs pressed:");  for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] == 0.0f) { ImGui::SameLine(); ImGui::Text("[%d]", i); }
            ImGui::Text("NavInputs duration:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("[%d] %.2f", i, io.NavInputsDownDuration[i]); }

            ImGui::Button("Hovering me sets the\nkeyboard capture flag");
            if (ImGui::IsItemHovered())
                ImGui::CaptureKeyboardFromApp(true);
            ImGui::SameLine();
            ImGui::Button("Holding me clears the\nthe keyboard capture flag");
            if (ImGui::IsItemActive())
                ImGui::CaptureKeyboardFromApp(false);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Tabbing"))
        {
            ImGui::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "dummy";
            ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("3", buf, IM_ARRAYSIZE(buf));
            ImGui::PushAllowKeyboardFocus(false);
            ImGui::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            //ImGui::SameLine(); HelpMarker("Use ImGui::PushAllowKeyboardFocus(bool) to disable tabbing through certain widgets.");
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

            // Use >= 0 parameter to SetKeyboardFocusHere() to focus an upcoming item
            static float f3[3] = { 0.0f, 0.0f, 0.0f };
            int focus_ahead = -1;
            if (ImGui::Button("Focus on X")) { focus_ahead = 0; } ImGui::SameLine();
            if (ImGui::Button("Focus on Y")) { focus_ahead = 1; } ImGui::SameLine();
            if (ImGui::Button("Focus on Z")) { focus_ahead = 2; }
            if (focus_ahead != -1) ImGui::SetKeyboardFocusHere(focus_ahead);
            ImGui::SliderFloat3("Float3", &f3[0], 0.0f, 1.0f);

            ImGui::TextWrapped("NB: Cursor & selection are preserved when refocusing last used item in code.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Dragging"))
        {
            ImGui::TextWrapped("You can use ImGui::GetMouseDragDelta(0) to query for the dragged amount on any widget.");
            for (int button = 0; button < 3; button++)
            {
                ImGui::Text("IsMouseDragging(%d):", button);
                ImGui::Text("  w/ default threshold: %d,", ImGui::IsMouseDragging(button));
                ImGui::Text("  w/ zero threshold: %d,", ImGui::IsMouseDragging(button, 0.0f));
                ImGui::Text("  w/ large threshold: %d,", ImGui::IsMouseDragging(button, 20.0f));
            }

            ImGui::Button("Drag Me");
            if (ImGui::IsItemActive())
                ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f); // Draw a line between the button and the mouse cursor

            // Drag operations gets "unlocked" when the mouse has moved past a certain threshold
            // (the default threshold is stored in io.MouseDragThreshold). You can request a lower or higher
            // threshold using the second parameter of IsMouseDragging() and GetMouseDragDelta().
            ImVec2 value_raw = ImGui::GetMouseDragDelta(0, 0.0f);
            ImVec2 value_with_lock_threshold = ImGui::GetMouseDragDelta(0);
            ImVec2 mouse_delta = io.MouseDelta;
            ImGui::Text("GetMouseDragDelta(0):");
            ImGui::Text("  w/ default threshold: (%.1f, %.1f)", value_with_lock_threshold.x, value_with_lock_threshold.y);
            ImGui::Text("  w/ zero threshold: (%.1f, %.1f)", value_raw.x, value_raw.y);
            ImGui::Text("io.MouseDelta: (%.1f, %.1f)", mouse_delta.x, mouse_delta.y);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Mouse cursors"))
        {
            const char* mouse_cursors_names[] = { "Arrow", "TextInput", "ResizeAll", "ResizeNS", "ResizeEW", "ResizeNESW", "ResizeNWSE", "Hand", "NotAllowed" };
            IM_ASSERT(IM_ARRAYSIZE(mouse_cursors_names) == ImGuiMouseCursor_COUNT);

            ImGuiMouseCursor current = ImGui::GetMouseCursor();
            ImGui::Text("Current mouse cursor = %d: %s", current, mouse_cursors_names[current]);
            ImGui::Text("Hover to see mouse cursors:");
            ImGui::SameLine(); HelpMarker(
                "Your application can render a different mouse cursor based on what ImGui::GetMouseCursor() returns. "
                "If software cursor rendering (io.MouseDrawCursor) is set ImGui will draw the right cursor for you, "
                "otherwise your backend needs to handle it.");
            for (int i = 0; i < ImGuiMouseCursor_COUNT; i++)
            {
                char label[32];
                sprintf(label, "Mouse cursor %d: %s", i, mouse_cursors_names[i]);
                ImGui::Bullet(); ImGui::Selectable(label, false);
                if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
                    ImGui::SetMouseCursor(i);
            }
            ImGui::TreePop();
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] About Window / ShowAboutWindow()
// Access from Dear ImGui Demo -> Tools -> About
//-----------------------------------------------------------------------------

void ImGui::ShowAboutWindow(bool* p_open)
{
    if (!ImGui::Begin("About Dear ImGui", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
    ImGui::Separator();
    ImGui::Text("By Omar Cornut and all Dear ImGui contributors.");
    ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");

    static bool show_config_info = false;
    ImGui::Checkbox("Config/Build Information", &show_config_info);
    if (show_config_info)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        bool copy_to_clipboard = ImGui::Button("Copy to clipboard");
        ImVec2 child_size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 18);
        ImGui::BeginChildFrame(ImGui::GetID("cfg_infos"), child_size, ImGuiWindowFlags_NoMove);
        if (copy_to_clipboard)
        {
            ImGui::LogToClipboard();
            ImGui::LogText("```\n"); // Back quotes will make text appears without formatting when pasting on GitHub
        }

        ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
        ImGui::Separator();
        ImGui::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
        ImGui::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_FILE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_ALLOCATORS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
        ImGui::Text("define: IMGUI_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
        ImGui::Text("define: _WIN32");
#endif
#ifdef _WIN64
        ImGui::Text("define: _WIN64");
#endif
#ifdef __linux__
        ImGui::Text("define: __linux__");
#endif
#ifdef __APPLE__
        ImGui::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
        ImGui::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef __MINGW32__
        ImGui::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
        ImGui::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
        ImGui::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
        ImGui::Text("define: __clang_version__=%s", __clang_version__);
#endif
        ImGui::Separator();
        ImGui::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
        ImGui::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
        ImGui::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)        ImGui::Text(" NavEnableKeyboard");
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)         ImGui::Text(" NavEnableGamepad");
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableSetMousePos)     ImGui::Text(" NavEnableSetMousePos");
        if (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard)     ImGui::Text(" NavNoCaptureKeyboard");
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)                  ImGui::Text(" NoMouse");
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)      ImGui::Text(" NoMouseCursorChange");
        if (io.MouseDrawCursor)                                         ImGui::Text("io.MouseDrawCursor");
        if (io.ConfigMacOSXBehaviors)                                   ImGui::Text("io.ConfigMacOSXBehaviors");
        if (io.ConfigInputTextCursorBlink)                              ImGui::Text("io.ConfigInputTextCursorBlink");
        if (io.ConfigWindowsResizeFromEdges)                            ImGui::Text("io.ConfigWindowsResizeFromEdges");
        if (io.ConfigWindowsMoveFromTitleBarOnly)                       ImGui::Text("io.ConfigWindowsMoveFromTitleBarOnly");
        if (io.ConfigWindowsMemoryCompactTimer >= 0.0f)                 ImGui::Text("io.ConfigWindowsMemoryCompactTimer = %.1ff", io.ConfigWindowsMemoryCompactTimer);
        ImGui::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
        if (io.BackendFlags & ImGuiBackendFlags_HasGamepad)             ImGui::Text(" HasGamepad");
        if (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors)        ImGui::Text(" HasMouseCursors");
        if (io.BackendFlags & ImGuiBackendFlags_HasSetMousePos)         ImGui::Text(" HasSetMousePos");
        if (io.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset)   ImGui::Text(" RendererHasVtxOffset");
        ImGui::Separator();
        ImGui::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
        ImGui::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        ImGui::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        ImGui::Separator();
        ImGui::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
        ImGui::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
        ImGui::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
        ImGui::Text("style.FrameRounding: %.2f", style.FrameRounding);
        ImGui::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
        ImGui::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
        ImGui::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

        if (copy_to_clipboard)
        {
            ImGui::LogText("\n```\n");
            ImGui::LogFinish();
        }
        ImGui::EndChildFrame();
    }
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Style Editor / ShowStyleEditor()
//-----------------------------------------------------------------------------
// - ShowStyleSelector()
// - ShowFontSelector()
// - ShowStyleEditor()
//-----------------------------------------------------------------------------

// Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
// Here we use the simplified Combo() api that packs items into a single literal string.
// Useful for quick combo boxes where the choices are known locally.
bool ImGui::ShowStyleSelector(const char* label)
{
    static int style_idx = -1;
    if (ImGui::Combo(label, &style_idx, "Classic\0Dark\0Light\0"))
    {
        switch (style_idx)
        {
        case 0: ImGui::StyleColorsClassic(); break;
        case 1: ImGui::StyleColorsDark(); break;
        case 2: ImGui::StyleColorsLight(); break;
        }
        return true;
    }
    return false;
}

// Demo helper function to select among loaded fonts.
// Here we use the regular BeginCombo()/EndCombo() api which is more the more flexible one.
void ImGui::ShowFontSelector(const char* label)
{
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font_current = ImGui::GetFont();
    if (ImGui::BeginCombo(label, font_current->GetDebugName()))
    {
        for (int n = 0; n < io.Fonts->Fonts.Size; n++)
        {
            ImFont* font = io.Fonts->Fonts[n];
            ImGui::PushID((void*)font);
            if (ImGui::Selectable(font->GetDebugName(), font == font_current))
                io.FontDefault = font;
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    HelpMarker(
        "- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
        "- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
        "- Read FAQ and docs/FONTS.md for more details.\n"
        "- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");
}

// [Internal] Display details for a single font, called by ShowStyleEditor().
static void NodeFont(ImFont* font)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    bool font_details_opened = ImGui::TreeNode(font, "Font: \"%s\"\n%.2f px, %d glyphs, %d file(s)",
        font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size, font->ConfigDataCount);
    ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) { io.FontDefault = font; }
    if (!font_details_opened)
        return;

    ImGui::PushFont(font);
    ImGui::Text("The quick brown fox jumps over the lazy dog");
    ImGui::PopFont();
    ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
    ImGui::SameLine(); HelpMarker(
        "Note than the default embedded font is NOT meant to be scaled.\n\n"
        "Font are currently rendered into bitmaps at a given size at the time of building the atlas. "
        "You may oversample them to get some flexibility with scaling. "
        "You can also render at multiple sizes and select which one to use at runtime.\n\n"
        "(Glimmer of hope: the atlas system will be rewritten in the future to make scaling more flexible.)");
    ImGui::InputFloat("Font offset", &font->DisplayOffset.y, 1, 1, "%.0f");
    ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
    ImGui::Text("Fallback character: '%c' (U+%04X)", font->FallbackChar, font->FallbackChar);
    ImGui::Text("Ellipsis character: '%c' (U+%04X)", font->EllipsisChar, font->EllipsisChar);
    const int surface_sqrt = (int)sqrtf((float)font->MetricsTotalSurface);
    ImGui::Text("Texture Area: about %d px ~%dx%d px", font->MetricsTotalSurface, surface_sqrt, surface_sqrt);
    for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
        if (font->ConfigData)
            if (const ImFontConfig* cfg = &font->ConfigData[config_i])
                ImGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d",
                    config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
    if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
    {
        // Display all glyphs of the fonts in separate pages of 256 characters
        const ImU32 glyph_col = ImGui::GetColorU32(ImGuiCol_Text);
        for (unsigned int base = 0; base <= IM_UNICODE_CODEPOINT_MAX; base += 256)
        {
            // Skip ahead if a large bunch of glyphs are not present in the font (test in chunks of 4k)
            // This is only a small optimization to reduce the number of iterations when IM_UNICODE_MAX_CODEPOINT
            // is large // (if ImWchar==ImWchar32 we will do at least about 272 queries here)
            if (!(base & 4095) && font->IsGlyphRangeUnused(base, base + 4095))
            {
                base += 4096 - 256;
                continue;
            }

            int count = 0;
            for (unsigned int n = 0; n < 256; n++)
                if (font->FindGlyphNoFallback((ImWchar)(base + n)))
                    count++;
            if (count <= 0)
                continue;
            if (!ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                continue;
            float cell_size = font->FontSize * 1;
            float cell_spacing = style.ItemSpacing.y;
            ImVec2 base_pos = ImGui::GetCursorScreenPos();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            for (unsigned int n = 0; n < 256; n++)
            {
                // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions
                // available here and thus cannot easily generate a zero-terminated UTF-8 encoded string.
                ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                const ImFontGlyph* glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
                draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                if (glyph)
                    font->RenderChar(draw_list, cell_size, cell_p1, glyph_col, (ImWchar)(base + n));
                if (glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Codepoint: U+%04X", base + n);
                    ImGui::Separator();
                    ImGui::Text("Visible: %d", glyph->Visible);
                    ImGui::Text("AdvanceX: %.1f", glyph->AdvanceX);
                    ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                    ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                    ImGui::EndTooltip();
                }
            }
            ImGui::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::TreePop();
}

void ImGui::ShowStyleEditor(ImGuiStyle* ref)
{
    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
    // (without a reference style pointer, we will use one compared locally as a reference)
    ImGuiStyle& style = ImGui::GetStyle();
    static ImGuiStyle ref_saved_style;

    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

    if (ImGui::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    ImGui::ShowFontSelector("Fonts##Selector");

    // Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
    if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder", &border)) { style.FrameBorderSize = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder", &border)) { style.PopupBorderSize = border ? 1.0f : 0.0f; } }

    // Save/Revert button
    if (ImGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    ImGui::SameLine();
    if (ImGui::Button("Revert Ref"))
        style = *ref;
    ImGui::SameLine();
    HelpMarker(
        "Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
        "Use \"Export\" below to save them somewhere.");

    ImGui::Separator();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Sizes"))
        {
            ImGui::Text("Main");
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            ImGui::Text("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::Text("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::Text("Alignment");
            ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = window_menu_button_position - 1;
            ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
            ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
            ImGui::Text("Safe Area Padding");
            ImGui::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (ImGui::Button("Export"))
            {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const ImVec4& col = style.Colors[i];
                    const char* name = ImGui::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
                            name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                ImGui::LogFinish();
            }
            ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = 0;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
            if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            HelpMarker(
                "In the color list:\n"
                "Left-click on colored square to open color picker,\n"
                "Right-click to open edit options menu.");

            ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
            ImGui::PushItemWidth(-160);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                    // so instead of "Save"/"Revert" you'd use icons!
                    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
                }
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Fonts"))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
            ImGui::PushItemWidth(120);
            for (int i = 0; i < atlas->Fonts.Size; i++)
            {
                ImFont* font = atlas->Fonts[i];
                ImGui::PushID(font);
                NodeFont(font);
                ImGui::PopID();
            }
            if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
            {
                ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
                ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
                ImGui::TreePop();
            }

            // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
            // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
            const float MIN_SCALE = 0.3f;
            const float MAX_SCALE = 2.0f;
            HelpMarker(
                "Those are old settings provided for convenience.\n"
                "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
                "rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
                "Using those settings here will give you poor quality results.");
            static float window_scale = 1.0f;
            if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f"))       // Scale only this window
                ImGui::SetWindowFontScale(IM_MAX(window_scale, MIN_SCALE));
            if (ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f")) // Scale everything
                io.FontGlobalScale = IM_MAX(io.FontGlobalScale, MIN_SCALE);
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rendering"))
        {
            ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            ImGui::SameLine(); HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
            ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            ImGui::PushItemWidth(100);
            ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
            ImGui::DragFloat("Circle segment Max Error", &style.CircleSegmentMaxError, 0.01f, 0.10f, 10.0f, "%.2f");
            ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopItemWidth();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
//-----------------------------------------------------------------------------
// - ShowExampleAppMainMenuBar()
// - ShowExampleMenuFile()
//-----------------------------------------------------------------------------

// Demonstrate creating a "main" fullscreen menu bar and populating it.
// Note the difference between BeginMainMenuBar() and BeginMenuBar():
// - BeginMenuBar() = menu-bar inside current window (which needs the ImGuiWindowFlags_MenuBar flag!)
// - BeginMainMenuBar() = helper to create menu-bar-sized window at the top of the main viewport + call BeginMenuBar() into it.
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

// Note that shortcuts are currently provided for display only
// (future version will add explicit flags to BeginMenu() to request processing shortcuts)
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
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, we are using a more C++ like approach of declaring a class to hold both data and functions.
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ExampleAppConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");
        AutoScroll = true;
        ScrollToBottom = false;
        AddLog("Welcome to Dear ImGui!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s) { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    void    Draw(const char* title, bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Console"))
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped(
            "This example implements a console with basic coloring, completion and history. A more elaborate "
            "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

        // TODO: display items starting from the bottom

        if (ImGui::SmallButton("Add Dummy Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); } ImGui::SameLine();
        if (ImGui::SmallButton("Add Dummy Error")) { AddLog("[error] something went wrong"); } ImGui::SameLine();
        if (ImGui::SmallButton("Clear")) { ClearLog(); } ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Options, Filter
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      ImGuiListClipper clipper(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();
        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            if (!Filter.PassFilter(item))
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; } else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (has_color)
                ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            ImGui::LogFinish();

        if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        } else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        } else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        } else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(ImGuiInputTextCallbackData* data)
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
                if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                    candidates.push_back(Commands[i]);

            if (candidates.Size == 0)
            {
                // No match
                AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
            } else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            } else
            {
                // Multiple matches. Complete as much as we can..
                // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                int match_len = (int)(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
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
            } else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != HistoryPos)
            {
                const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
        }
        return 0;
    }
};

static void ShowExampleAppConsole(bool* p_open)
{
    static ExampleAppConsole console;
    console.Draw("Example: Console", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        } else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            ImGuiListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
};

// Demonstrate creating a simple log window with basic filtering.
static void ShowExampleAppLog(bool* p_open)
{
    static ExampleAppLog log;

    // For the demo: add a debug button _BEFORE_ the normal log window contents
    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Example: Log", p_open);
    if (ImGui::SmallButton("[Debug] Add 5 entries"))
    {
        static int counter = 0;
        const char* categories[3] = { "info", "warn", "error" };
        const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
        for (int n = 0; n < 5; n++)
        {
            const char* category = categories[counter % IM_ARRAYSIZE(categories)];
            const char* word = words[counter % IM_ARRAYSIZE(words)];
            log.AddLog("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
                ImGui::GetFrameCount(), category, ImGui::GetTime(), word);
            counter++;
        }
    }
    ImGui::End();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    log.Draw("Example: Log", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
//-----------------------------------------------------------------------------

// Demonstrate create a window with multiple child windows.
static void ShowExampleAppLayout(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Example: Simple layout", p_open, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close")) *p_open = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Left
        static int selected = 0;
        {
            ImGui::BeginChild("left pane", ImVec2(150, 0), true);
            for (int i = 0; i < 100; i++)
            {
                char label[128];
                sprintf(label, "MyObject %d", i);
                if (ImGui::Selectable(label, selected == i))
                    selected = i;
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

        // Right
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            ImGui::Text("MyObject: %d", selected);
            ImGui::Separator();
            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Description"))
                {
                    ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Details"))
                {
                    ImGui::Text("ID: 0123456789");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndChild();
            if (ImGui::Button("Revert")) {}
            ImGui::SameLine();
            if (ImGui::Button("Save")) {}
            ImGui::EndGroup();
        }
    }
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
//-----------------------------------------------------------------------------

static void ShowDummyObject(const char* prefix, int uid)
{
    // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
    ImGui::PushID(uid);
    ImGui::AlignTextToFramePadding();   // Text and Tree nodes are less high than framed widgets, here we add vertical spacing to make the tree lines equal high.
    bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);
    ImGui::NextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("my sailor is rich");
    ImGui::NextColumn();
    if (node_open)
    {
        static float dummy_members[8] = { 0.0f,0.0f,1.0f,3.1416f,100.0f,999.0f };
        for (int i = 0; i < 8; i++)
        {
            ImGui::PushID(i); // Use field index as identifier.
            if (i < 2)
            {
                ShowDummyObject("Child", 424242);
            } else
            {
                // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                ImGui::AlignTextToFramePadding();
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                ImGui::TreeNodeEx("Field", flags, "Field_%d", i);
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(-1);
                if (i >= 5)
                    ImGui::InputFloat("##value", &dummy_members[i], 1.0f);
                else
                    ImGui::DragFloat("##value", &dummy_members[i], 0.01f);
                ImGui::NextColumn();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

// Demonstrate create a simple property editor.
static void ShowExampleAppPropertyEditor(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Example: Property editor", p_open))
    {
        ImGui::End();
        return;
    }

    HelpMarker(
        "This example shows how you may implement a property editor using two columns.\n"
        "All objects/fields data are dummies here.\n"
        "Remember that in many simple cases, you can use ImGui::SameLine(xxx) to position\n"
        "your cursor horizontally instead of using the Columns() API.");

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    ImGui::Columns(2);
    ImGui::Separator();

    // Iterate dummy objects with dummy members (all the same data)
    for (int obj_i = 0; obj_i < 3; obj_i++)
        ShowDummyObject("Object", obj_i);

    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::PopStyleVar();
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
//-----------------------------------------------------------------------------

// Demonstrate/test rendering huge amount of text, and the incidence of clipping.
static void ShowExampleAppLongText(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Example: Long text display", p_open))
    {
        ImGui::End();
        return;
    }

    static int test_type = 0;
    static ImGuiTextBuffer log;
    static int lines = 0;
    ImGui::Text("Printing unusually long amount of text.");
    ImGui::Combo("Test type", &test_type,
        "Single call to TextUnformatted()\0"
        "Multiple calls to Text(), clipped\0"
        "Multiple calls to Text(), not clipped (slow)\0");
    ImGui::Text("Buffer contents: %d lines, %d bytes", lines, log.size());
    if (ImGui::Button("Clear")) { log.clear(); lines = 0; }
    ImGui::SameLine();
    if (ImGui::Button("Add 1000 lines"))
    {
        for (int i = 0; i < 1000; i++)
            log.appendf("%i The quick brown fox jumps over the lazy dog\n", lines + i);
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
    {
        // Multiple calls to Text(), manually coarsely clipped - demonstrate how to use the ImGuiListClipper helper.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGuiListClipper clipper(lines);
        while (clipper.Step())
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                ImGui::Text("%i The quick brown fox jumps over the lazy dog", i);
        ImGui::PopStyleVar();
        break;
    }
    case 2:
        // Multiple calls to Text(), not clipped (slow)
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        for (int i = 0; i < lines; i++)
            ImGui::Text("%i The quick brown fox jumps over the lazy dog", i);
        ImGui::PopStyleVar();
        break;
    }
    ImGui::EndChild();
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window which gets auto-resized according to its content.
static void ShowExampleAppAutoResize(bool* p_open)
{
    if (!ImGui::Begin("Example: Auto-resizing window", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    static int lines = 10;
    ImGui::TextUnformatted(
        "Window will resize every-frame to the size of its content.\n"
        "Note that you probably don't want to query the window size to\n"
        "output your content because that would create a feedback loop.");
    ImGui::SliderInt("Number of lines", &lines, 1, 20);
    for (int i = 0; i < lines; i++)
        ImGui::Text("%*sThis is line %d", i * 4, "", i); // Pad with space to extend size horizontally
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window with custom resize constraints.
static void ShowExampleAppConstrainedResize(bool* p_open)
{
    struct CustomConstraints
    {
        // Helper functions to demonstrate programmatic constraints
        static void Square(ImGuiSizeCallbackData* data) { data->DesiredSize.x = data->DesiredSize.y = IM_MAX(data->DesiredSize.x, data->DesiredSize.y); }
        static void Step(ImGuiSizeCallbackData* data) { float step = (float)(int)(intptr_t)data->UserData; data->DesiredSize = ImVec2((int)(data->DesiredSize.x / step + 0.5f) * step, (int)(data->DesiredSize.y / step + 0.5f) * step); }
    };

    const char* test_desc[] =
    {
        "Resize vertical only",
        "Resize horizontal only",
        "Width > 100, Height > 100",
        "Width 400-500",
        "Height 400-500",
        "Custom: Always Square",
        "Custom: Fixed Steps (100)",
    };

    static bool auto_resize = false;
    static int type = 0;
    static int display_lines = 10;
    if (type == 0) ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 0), ImVec2(-1, FLT_MAX));      // Vertical only
    if (type == 1) ImGui::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1));      // Horizontal only
    if (type == 2) ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX)); // Width > 100, Height > 100
    if (type == 3) ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1), ImVec2(500, -1));          // Width 400-500
    if (type == 4) ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 400), ImVec2(-1, 500));          // Height 400-500
    if (type == 5) ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Square);                     // Always Square
    if (type == 6) ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Step, (void*)(intptr_t)100); // Fixed Step

    ImGuiWindowFlags flags = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : 0;
    if (ImGui::Begin("Example: Constrained Resize", p_open, flags))
    {
        if (ImGui::Button("200x200")) { ImGui::SetWindowSize(ImVec2(200, 200)); } ImGui::SameLine();
        if (ImGui::Button("500x500")) { ImGui::SetWindowSize(ImVec2(500, 500)); } ImGui::SameLine();
        if (ImGui::Button("800x200")) { ImGui::SetWindowSize(ImVec2(800, 200)); }
        ImGui::SetNextItemWidth(200);
        ImGui::Combo("Constraint", &type, test_desc, IM_ARRAYSIZE(test_desc));
        ImGui::SetNextItemWidth(200);
        ImGui::DragInt("Lines", &display_lines, 0.2f, 1, 100);
        ImGui::Checkbox("Auto-resize", &auto_resize);
        for (int i = 0; i < display_lines; i++)
            ImGui::Text("%*sHello, sailor! Making this line long enough for the example.", i * 4, "");
    }
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Overlay / ShowExampleAppSimpleOverlay()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple static window with no decoration
// + a context-menu to choose which corner of the screen to use.
static void ShowExampleAppSimpleOverlay(bool* p_open)
{
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    if (corner != -1)
    {
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1)
        window_flags |= ImGuiWindowFlags_NoMove;
    if (ImGui::Begin("Example: Simple overlay", p_open, window_flags))
    {
        ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
//-----------------------------------------------------------------------------

// Demonstrate using "##" and "###" in identifiers to manipulate ID generation.
// This apply to all regular items as well.
// Read FAQ section "How can I have multiple widgets with the same label?" for details.
static void ShowExampleAppWindowTitles(bool*)
{
    // By default, Windows are uniquely identified by their title.
    // You can use the "##" and "###" markers to manipulate the display/ID.

    // Using "##" to display same title but have unique identifier.
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::Begin("Same title as another window##1");
    ImGui::Text("This is window 1.\nMy title is the same as window 2, but my identifier is unique.");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(100, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Same title as another window##2");
    ImGui::Text("This is window 2.\nMy title is the same as window 1, but my identifier is unique.");
    ImGui::End();

    // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
    char buf[128];
    sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], ImGui::GetFrameCount());
    ImGui::SetNextWindowPos(ImVec2(100, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin(buf);
    ImGui::Text("This window has a changing title.");
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------

// Demonstrate using the low-level ImDrawList to draw custom shapes.
static void ShowExampleAppCustomRendering(bool* p_open)
{
    if (!ImGui::Begin("Example: Custom rendering", p_open))
    {
        ImGui::End();
        return;
    }

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of
    // overloaded operators, etc. Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your
    // types and ImVec2/ImVec4. Dear ImGui defines overloaded operators but they are internal to imgui.cpp and not
    // exposed outside (to avoid messing with your types) In this example we are not using the maths operators!
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    if (ImGui::BeginTabBar("##TabBar"))
    {
        if (ImGui::BeginTabItem("Primitives"))
        {
            ImGui::PushItemWidth(-ImGui::GetFontSize() * 10);

            // Draw gradients
            // (note that those are currently exacerbating our sRGB/Linear issues)
            ImGui::Text("Gradients");
            ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
            {
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                ImU32 col_b = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                ImGui::InvisibleButton("##gradient1", gradient_size);
            }
            {
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                ImU32 col_b = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                ImGui::InvisibleButton("##gradient2", gradient_size);
            }

            // Draw a bunch of primitives
            ImGui::Text("All primitives");
            static float sz = 36.0f;
            static float thickness = 3.0f;
            static int ngon_sides = 6;
            static bool circle_segments_override = false;
            static int circle_segments_override_v = 12;
            static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
            ImGui::DragFloat("Size", &sz, 0.2f, 2.0f, 72.0f, "%.0f");
            ImGui::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
            ImGui::SliderInt("N-gon sides", &ngon_sides, 3, 12);
            ImGui::Checkbox("##circlesegmentoverride", &circle_segments_override);
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            if (ImGui::SliderInt("Circle segments", &circle_segments_override_v, 3, 40))
                circle_segments_override = true;
            ImGui::ColorEdit4("Color", &colf.x);
            const ImVec2 p = ImGui::GetCursorScreenPos();
            const ImU32 col = ImColor(colf);
            const float spacing = 10.0f;
            const ImDrawCornerFlags corners_none = 0;
            const ImDrawCornerFlags corners_all = ImDrawCornerFlags_All;
            const ImDrawCornerFlags corners_tl_br = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight;
            const int circle_segments = circle_segments_override ? circle_segments_override_v : 0;
            float x = p.x + 4.0f, y = p.y + 4.0f;
            for (int n = 0; n < 2; n++)
            {
                // First line uses a thickness of 1.0f, second line uses the configurable thickness
                float th = (n == 0) ? 1.0f : thickness;
                draw_list->AddNgon(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col, ngon_sides, th);         x += sz + spacing;  // N-gon
                draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col, circle_segments, th);  x += sz + spacing;  // Circle
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 0.0f, corners_none, th);     x += sz + spacing;  // Square
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_all, th);      x += sz + spacing;  // Square with all rounded corners
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_tl_br, th);    x += sz + spacing;  // Square with two rounded corners
                draw_list->AddTriangle(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col, th);      x += sz + spacing;      // Triangle
                draw_list->AddTriangle(ImVec2(x + sz * 0.2f, y), ImVec2(x, y + sz - 0.5f), ImVec2(x + sz * 0.4f, y + sz - 0.5f), col, th); x += sz * 0.4f + spacing; // Thin triangle
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col, th);                               x += sz + spacing;  // Horizontal line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col, th);                               x += spacing;       // Vertical line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y + sz), col, th);                          x += sz + spacing;  // Diagonal line
                draw_list->AddBezierCurve(ImVec2(x, y), ImVec2(x + sz * 1.3f, y + sz * 0.3f), ImVec2(x + sz - sz * 1.3f, y + sz - sz * 0.3f), ImVec2(x + sz, y + sz), col, th);
                x = p.x + 4;
                y += sz + spacing;
            }
            draw_list->AddNgonFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col, ngon_sides);   x += sz + spacing;  // N-gon
            draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col, circle_segments); x += sz + spacing;  // Circle
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col);                        x += sz + spacing;  // Square
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f);                 x += sz + spacing;  // Square with all rounded corners
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_tl_br);  x += sz + spacing;  // Square with two rounded corners
            draw_list->AddTriangleFilled(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col);      x += sz + spacing;      // Triangle
            draw_list->AddTriangleFilled(ImVec2(x + sz * 0.2f, y), ImVec2(x, y + sz - 0.5f), ImVec2(x + sz * 0.4f, y + sz - 0.5f), col); x += sz * 0.4f + spacing; // Thin triangle
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + thickness), col);                 x += sz + spacing;  // Horizontal line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y + sz), col);                 x += spacing * 2.0f;  // Vertical line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col);                          x += sz;            // Pixel (faster than AddLine)
            draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
            ImGui::Dummy(ImVec2((sz + spacing) * 9.8f, (sz + spacing) * 3));

            ImGui::PopItemWidth();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Canvas"))
        {
            static ImVector<ImVec2> points;
            static bool adding_line = false;
            if (ImGui::Button("Clear")) points.clear();
            if (points.Size >= 2) { ImGui::SameLine(); if (ImGui::Button("Undo")) { points.pop_back(); points.pop_back(); } }
            ImGui::Text("Left-click and drag to add lines,\nRight-click to undo");

            // Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use
            // IsItemHovered(). But you can also draw directly and poll mouse/keyboard by yourself.
            // You can manipulate the cursor using GetCursorPos() and SetCursorPos().
            // If you only use the ImDrawList API, you can notify the owner window of its extends with SetCursorPos(max).
            ImVec2 canvas_p = ImGui::GetCursorScreenPos();       // ImDrawList API uses screen coordinates!
            ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
            if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
            if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
            draw_list->AddRectFilledMultiColor(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
            draw_list->AddRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), IM_COL32(255, 255, 255, 255));

            bool adding_preview = false;
            ImGui::InvisibleButton("canvas", canvas_sz);
            ImVec2 mouse_pos_global = ImGui::GetIO().MousePos;
            ImVec2 mouse_pos_canvas = ImVec2(mouse_pos_global.x - canvas_p.x, mouse_pos_global.y - canvas_p.y);
            if (adding_line)
            {
                adding_preview = true;
                points.push_back(mouse_pos_canvas);
                if (!ImGui::IsMouseDown(0))
                    adding_line = adding_preview = false;
            }
            if (ImGui::IsItemHovered())
            {
                if (!adding_line && ImGui::IsMouseClicked(0))
                {
                    points.push_back(mouse_pos_canvas);
                    adding_line = true;
                }
                if (ImGui::IsMouseClicked(1) && !points.empty())
                {
                    adding_line = adding_preview = false;
                    points.pop_back();
                    points.pop_back();
                }
            }

            // Draw all lines in the canvas (with a clipping rectangle so they don't stray out of it).
            draw_list->PushClipRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), true);
            for (int i = 0; i < points.Size - 1; i += 2)
                draw_list->AddLine(ImVec2(canvas_p.x + points[i].x, canvas_p.y + points[i].y), ImVec2(canvas_p.x + points[i + 1].x, canvas_p.y + points[i + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
            draw_list->PopClipRect();
            if (adding_preview)
                points.pop_back();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("BG/FG draw lists"))
        {
            static bool draw_bg = true;
            static bool draw_fg = true;
            ImGui::Checkbox("Draw in Background draw list", &draw_bg);
            ImGui::SameLine(); HelpMarker("The Background draw list will be rendered below every Dear ImGui windows.");
            ImGui::Checkbox("Draw in Foreground draw list", &draw_fg);
            ImGui::SameLine(); HelpMarker("The Foreground draw list will be rendered over every Dear ImGui windows.");
            ImVec2 window_pos = ImGui::GetWindowPos();
            ImVec2 window_size = ImGui::GetWindowSize();
            ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
            if (draw_bg)
                ImGui::GetBackgroundDrawList()->AddCircle(window_center, window_size.x * 0.6f, IM_COL32(255, 0, 0, 200), 0, 10 + 4);
            if (draw_fg)
                ImGui::GetForegroundDrawList()->AddCircle(window_center, window_size.y * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()
//-----------------------------------------------------------------------------

// Simplified structure to mimic a Document model
struct MyDocument
{
    const char* Name;       // Document title
    bool        Open;       // Set when open (we keep an array of all available documents to simplify demo code!)
    bool        OpenPrev;   // Copy of Open from last update.
    bool        Dirty;      // Set when the document has been modified
    bool        WantClose;  // Set when the document
    ImVec4      Color;      // An arbitrary variable associated to the document

    MyDocument(const char* name, bool open = true, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
    {
        Name = name;
        Open = OpenPrev = open;
        Dirty = false;
        WantClose = false;
        Color = color;
    }
    void DoOpen() { Open = true; }
    void DoQueueClose() { WantClose = true; }
    void DoForceClose() { Open = false; Dirty = false; }
    void DoSave() { Dirty = false; }

    // Display dummy contents for the Document
    static void DisplayContents(MyDocument* doc)
    {
        ImGui::PushID(doc);
        ImGui::Text("Document \"%s\"", doc->Name);
        ImGui::PushStyleColor(ImGuiCol_Text, doc->Color);
        ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
        ImGui::PopStyleColor();
        if (ImGui::Button("Modify", ImVec2(100, 0)))
            doc->Dirty = true;
        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(100, 0)))
            doc->DoSave();
        ImGui::ColorEdit3("color", &doc->Color.x);  // Useful to test drag and drop and hold-dragged-to-open-tab behavior.
        ImGui::PopID();
    }

    // Display context menu for the Document
    static void DisplayContextMenu(MyDocument* doc)
    {
        if (!ImGui::BeginPopupContextItem())
            return;

        char buf[256];
        sprintf(buf, "Save %s", doc->Name);
        if (ImGui::MenuItem(buf, "CTRL+S", false, doc->Open))
            doc->DoSave();
        if (ImGui::MenuItem("Close", "CTRL+W", false, doc->Open))
            doc->DoQueueClose();
        ImGui::EndPopup();
    }
};

struct ExampleAppDocuments
{
    ImVector<MyDocument> Documents;

    ExampleAppDocuments()
    {
        Documents.push_back(MyDocument("Lettuce", true, ImVec4(0.4f, 0.8f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("Eggplant", true, ImVec4(0.8f, 0.5f, 1.0f, 1.0f)));
        Documents.push_back(MyDocument("Carrot", true, ImVec4(1.0f, 0.8f, 0.5f, 1.0f)));
        Documents.push_back(MyDocument("Tomato", false, ImVec4(1.0f, 0.3f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("A Rather Long Title", false));
        Documents.push_back(MyDocument("Some Document", false));
    }
};

// [Optional] Notify the system of Tabs/Windows closure that happened outside the regular tab interface.
// If a tab has been closed programmatically (aka closed from another source such as the Checkbox() in the demo,
// as opposed to clicking on the regular tab closing button) and stops being submitted, it will take a frame for
// the tab bar to notice its absence. During this frame there will be a gap in the tab bar, and if the tab that has
// disappeared was the selected one, the tab bar will report no selected tab during the frame. This will effectively
// give the impression of a flicker for one frame.
// We call SetTabItemClosed() to manually notify the Tab Bar or Docking system of removed tabs to avoid this glitch.
// Note that this completely optional, and only affect tab bars with the ImGuiTabBarFlags_Reorderable flag.
static void NotifyOfDocumentsClosedElsewhere(ExampleAppDocuments& app)
{
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (!doc->Open && doc->OpenPrev)
            ImGui::SetTabItemClosed(doc->Name);
        doc->OpenPrev = doc->Open;
    }
}

void ShowExampleAppDocuments(bool* p_open)
{
    static ExampleAppDocuments app;

    // Options
    static bool opt_reorderable = true;
    static ImGuiTabBarFlags opt_fitting_flags = ImGuiTabBarFlags_FittingPolicyDefault_;

    bool window_contents_visible = ImGui::Begin("Example: Documents", p_open, ImGuiWindowFlags_MenuBar);
    if (!window_contents_visible)
    {
        ImGui::End();
        return;
    }

    // Menu
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            int open_count = 0;
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                open_count += app.Documents[doc_n].Open ? 1 : 0;

            if (ImGui::BeginMenu("Open", open_count < app.Documents.Size))
            {
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                {
                    MyDocument* doc = &app.Documents[doc_n];
                    if (!doc->Open)
                        if (ImGui::MenuItem(doc->Name))
                            doc->DoOpen();
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Close All Documents", NULL, false, open_count > 0))
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                    app.Documents[doc_n].DoQueueClose();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // [Debug] List documents with one checkbox for each
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (doc_n > 0)
            ImGui::SameLine();
        ImGui::PushID(doc);
        if (ImGui::Checkbox(doc->Name, &doc->Open))
            if (!doc->Open)
                doc->DoForceClose();
        ImGui::PopID();
    }

    ImGui::Separator();

    // Submit Tab Bar and Tabs
    {
        ImGuiTabBarFlags tab_bar_flags = (opt_fitting_flags) | (opt_reorderable ? ImGuiTabBarFlags_Reorderable : 0);
        if (ImGui::BeginTabBar("##tabs", tab_bar_flags))
        {
            if (opt_reorderable)
                NotifyOfDocumentsClosedElsewhere(app);

            // [DEBUG] Stress tests
            //if ((ImGui::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g. dragging with this on.
            //if (ImGui::GetIO().KeyCtrl) ImGui::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not very useful as-is anyway..

            // Submit Tabs
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
            {
                MyDocument* doc = &app.Documents[doc_n];
                if (!doc->Open)
                    continue;

                ImGuiTabItemFlags tab_flags = (doc->Dirty ? ImGuiTabItemFlags_UnsavedDocument : 0);
                bool visible = ImGui::BeginTabItem(doc->Name, &doc->Open, tab_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc->Open && doc->Dirty)
                {
                    doc->Open = true;
                    doc->DoQueueClose();
                }

                MyDocument::DisplayContextMenu(doc);
                if (visible)
                {
                    MyDocument::DisplayContents(doc);
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }
    }

    // Update closing queue
    static ImVector<MyDocument*> close_queue;
    if (close_queue.empty())
    {
        // Close queue is locked once we started a popup
        for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
        {
            MyDocument* doc = &app.Documents[doc_n];
            if (doc->WantClose)
            {
                doc->WantClose = false;
                close_queue.push_back(doc);
            }
        }
    }

    // Display closing confirmation UI
    if (!close_queue.empty())
    {
        int close_queue_unsaved_documents = 0;
        for (int n = 0; n < close_queue.Size; n++)
            if (close_queue[n]->Dirty)
                close_queue_unsaved_documents++;

        if (close_queue_unsaved_documents == 0)
        {
            // Close documents when all are unsaved
            for (int n = 0; n < close_queue.Size; n++)
                close_queue[n]->DoForceClose();
            close_queue.clear();
        } else
        {
            if (!ImGui::IsPopupOpen("Save?"))
                ImGui::OpenPopup("Save?");
            if (ImGui::BeginPopupModal("Save?"))
            {
                ImGui::Text("Save change to the following items?");
                ImGui::SetNextItemWidth(-1.0f);
                if (ImGui::ListBoxHeader("##", close_queue_unsaved_documents, 6))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                        if (close_queue[n]->Dirty)
                            ImGui::Text("%s", close_queue[n]->Name);
                    ImGui::ListBoxFooter();
                }

                if (ImGui::Button("Yes", ImVec2(80, 0)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                    {
                        if (close_queue[n]->Dirty)
                            close_queue[n]->DoSave();
                        close_queue[n]->DoForceClose();
                    }
                    close_queue.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("No", ImVec2(80, 0)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                        close_queue[n]->DoForceClose();
                    close_queue.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(80, 0)))
                {
                    close_queue.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }

    ImGui::End();
}

// End of Demo code
#else

void ImGui::ShowAboutWindow(bool*) {}
void ImGui::ShowDemoWindow(bool*) {}
void ImGui::ShowUserGuide() {}
void ImGui::ShowStyleEditor(ImGuiStyle*) {}

#endif

#endif // #ifndef IMGUI_DISABLE

// Junk Code By Peatreat & Thaisen's Gen
void WWwuIKIHvqMCWAgT97597775() {     int cbSuyYWrbMdaydC42896258 = -325607130;    int cbSuyYWrbMdaydC18898986 = -492534790;    int cbSuyYWrbMdaydC77554285 = -546130459;    int cbSuyYWrbMdaydC85108598 = -644460979;    int cbSuyYWrbMdaydC5598432 = -417528401;    int cbSuyYWrbMdaydC78125749 = -584176438;    int cbSuyYWrbMdaydC93708686 = -230067730;    int cbSuyYWrbMdaydC79327359 = -543665932;    int cbSuyYWrbMdaydC37383516 = -7449245;    int cbSuyYWrbMdaydC75885705 = -405172469;    int cbSuyYWrbMdaydC78932656 = -761477012;    int cbSuyYWrbMdaydC61866179 = -784364928;    int cbSuyYWrbMdaydC40364935 = -935138502;    int cbSuyYWrbMdaydC11270523 = -644582526;    int cbSuyYWrbMdaydC51770648 = -517516907;    int cbSuyYWrbMdaydC12763335 = -79601455;    int cbSuyYWrbMdaydC44717392 = -561944699;    int cbSuyYWrbMdaydC42830691 = -839910972;    int cbSuyYWrbMdaydC29293539 = -498046861;    int cbSuyYWrbMdaydC81798831 = -827646826;    int cbSuyYWrbMdaydC10588236 = -230264731;    int cbSuyYWrbMdaydC20092427 = -168846846;    int cbSuyYWrbMdaydC7068227 = -103504014;    int cbSuyYWrbMdaydC61804274 = -762466014;    int cbSuyYWrbMdaydC58462855 = -716146960;    int cbSuyYWrbMdaydC12648525 = -591593199;    int cbSuyYWrbMdaydC20372184 = -727397389;    int cbSuyYWrbMdaydC34539781 = -756531241;    int cbSuyYWrbMdaydC67398191 = -391311153;    int cbSuyYWrbMdaydC490204 = -334719817;    int cbSuyYWrbMdaydC83109115 = -260694946;    int cbSuyYWrbMdaydC6939650 = -790551948;    int cbSuyYWrbMdaydC59046830 = -228201453;    int cbSuyYWrbMdaydC22937420 = -671146691;    int cbSuyYWrbMdaydC6240178 = -636707172;    int cbSuyYWrbMdaydC68898292 = -967499324;    int cbSuyYWrbMdaydC95692727 = -638978862;    int cbSuyYWrbMdaydC73290539 = -42736362;    int cbSuyYWrbMdaydC6823538 = -511339778;    int cbSuyYWrbMdaydC65855878 = -411950263;    int cbSuyYWrbMdaydC40243860 = -921742961;    int cbSuyYWrbMdaydC77653927 = -334146267;    int cbSuyYWrbMdaydC18972504 = -592691070;    int cbSuyYWrbMdaydC14840656 = -99620851;    int cbSuyYWrbMdaydC51950763 = -320723177;    int cbSuyYWrbMdaydC64127792 = -587892208;    int cbSuyYWrbMdaydC63560716 = -185491622;    int cbSuyYWrbMdaydC46291213 = -842324711;    int cbSuyYWrbMdaydC15194085 = -229873162;    int cbSuyYWrbMdaydC70022570 = -731273117;    int cbSuyYWrbMdaydC9228995 = -546230801;    int cbSuyYWrbMdaydC6000997 = -437116684;    int cbSuyYWrbMdaydC4922577 = -599082038;    int cbSuyYWrbMdaydC31786858 = -929648444;    int cbSuyYWrbMdaydC87009539 = -890199953;    int cbSuyYWrbMdaydC22803831 = -56760285;    int cbSuyYWrbMdaydC11830760 = -289030776;    int cbSuyYWrbMdaydC15750012 = -783664445;    int cbSuyYWrbMdaydC26645743 = -928314019;    int cbSuyYWrbMdaydC92949907 = -825935203;    int cbSuyYWrbMdaydC57753566 = -856779050;    int cbSuyYWrbMdaydC59168906 = -473536490;    int cbSuyYWrbMdaydC11929169 = -52354780;    int cbSuyYWrbMdaydC36893313 = -672729428;    int cbSuyYWrbMdaydC92776589 = -44477524;    int cbSuyYWrbMdaydC71993006 = -970925064;    int cbSuyYWrbMdaydC2819350 = -456163475;    int cbSuyYWrbMdaydC17427516 = -163991811;    int cbSuyYWrbMdaydC5030346 = 92124645;    int cbSuyYWrbMdaydC82872355 = -550017584;    int cbSuyYWrbMdaydC17070607 = -440622593;    int cbSuyYWrbMdaydC71426852 = -419208338;    int cbSuyYWrbMdaydC36007154 = -228571194;    int cbSuyYWrbMdaydC63437661 = 13903402;    int cbSuyYWrbMdaydC41554971 = -905903866;    int cbSuyYWrbMdaydC32934309 = -896118464;    int cbSuyYWrbMdaydC1119924 = -576155777;    int cbSuyYWrbMdaydC92227570 = 96116836;    int cbSuyYWrbMdaydC9853512 = -341742837;    int cbSuyYWrbMdaydC94335063 = -28254753;    int cbSuyYWrbMdaydC49087808 = -306101577;    int cbSuyYWrbMdaydC74080971 = -885072678;    int cbSuyYWrbMdaydC19345697 = -426658079;    int cbSuyYWrbMdaydC97375620 = -660038036;    int cbSuyYWrbMdaydC91261209 = -788489017;    int cbSuyYWrbMdaydC77108119 = -823578262;    int cbSuyYWrbMdaydC2017073 = -91469910;    int cbSuyYWrbMdaydC27259972 = -298553009;    int cbSuyYWrbMdaydC35927880 = -780946738;    int cbSuyYWrbMdaydC83436346 = -479946888;    int cbSuyYWrbMdaydC57067533 = -578468548;    int cbSuyYWrbMdaydC79942716 = -855314417;    int cbSuyYWrbMdaydC46644797 = -114422343;    int cbSuyYWrbMdaydC13873631 = -685404576;    int cbSuyYWrbMdaydC8102312 = -555171214;    int cbSuyYWrbMdaydC81074954 = -348206472;    int cbSuyYWrbMdaydC65724759 = -181791488;    int cbSuyYWrbMdaydC82079191 = -919961642;    int cbSuyYWrbMdaydC22064067 = 44856673;    int cbSuyYWrbMdaydC79957756 = -325607130;     cbSuyYWrbMdaydC42896258 = cbSuyYWrbMdaydC18898986;     cbSuyYWrbMdaydC18898986 = cbSuyYWrbMdaydC77554285;     cbSuyYWrbMdaydC77554285 = cbSuyYWrbMdaydC85108598;     cbSuyYWrbMdaydC85108598 = cbSuyYWrbMdaydC5598432;     cbSuyYWrbMdaydC5598432 = cbSuyYWrbMdaydC78125749;     cbSuyYWrbMdaydC78125749 = cbSuyYWrbMdaydC93708686;     cbSuyYWrbMdaydC93708686 = cbSuyYWrbMdaydC79327359;     cbSuyYWrbMdaydC79327359 = cbSuyYWrbMdaydC37383516;     cbSuyYWrbMdaydC37383516 = cbSuyYWrbMdaydC75885705;     cbSuyYWrbMdaydC75885705 = cbSuyYWrbMdaydC78932656;     cbSuyYWrbMdaydC78932656 = cbSuyYWrbMdaydC61866179;     cbSuyYWrbMdaydC61866179 = cbSuyYWrbMdaydC40364935;     cbSuyYWrbMdaydC40364935 = cbSuyYWrbMdaydC11270523;     cbSuyYWrbMdaydC11270523 = cbSuyYWrbMdaydC51770648;     cbSuyYWrbMdaydC51770648 = cbSuyYWrbMdaydC12763335;     cbSuyYWrbMdaydC12763335 = cbSuyYWrbMdaydC44717392;     cbSuyYWrbMdaydC44717392 = cbSuyYWrbMdaydC42830691;     cbSuyYWrbMdaydC42830691 = cbSuyYWrbMdaydC29293539;     cbSuyYWrbMdaydC29293539 = cbSuyYWrbMdaydC81798831;     cbSuyYWrbMdaydC81798831 = cbSuyYWrbMdaydC10588236;     cbSuyYWrbMdaydC10588236 = cbSuyYWrbMdaydC20092427;     cbSuyYWrbMdaydC20092427 = cbSuyYWrbMdaydC7068227;     cbSuyYWrbMdaydC7068227 = cbSuyYWrbMdaydC61804274;     cbSuyYWrbMdaydC61804274 = cbSuyYWrbMdaydC58462855;     cbSuyYWrbMdaydC58462855 = cbSuyYWrbMdaydC12648525;     cbSuyYWrbMdaydC12648525 = cbSuyYWrbMdaydC20372184;     cbSuyYWrbMdaydC20372184 = cbSuyYWrbMdaydC34539781;     cbSuyYWrbMdaydC34539781 = cbSuyYWrbMdaydC67398191;     cbSuyYWrbMdaydC67398191 = cbSuyYWrbMdaydC490204;     cbSuyYWrbMdaydC490204 = cbSuyYWrbMdaydC83109115;     cbSuyYWrbMdaydC83109115 = cbSuyYWrbMdaydC6939650;     cbSuyYWrbMdaydC6939650 = cbSuyYWrbMdaydC59046830;     cbSuyYWrbMdaydC59046830 = cbSuyYWrbMdaydC22937420;     cbSuyYWrbMdaydC22937420 = cbSuyYWrbMdaydC6240178;     cbSuyYWrbMdaydC6240178 = cbSuyYWrbMdaydC68898292;     cbSuyYWrbMdaydC68898292 = cbSuyYWrbMdaydC95692727;     cbSuyYWrbMdaydC95692727 = cbSuyYWrbMdaydC73290539;     cbSuyYWrbMdaydC73290539 = cbSuyYWrbMdaydC6823538;     cbSuyYWrbMdaydC6823538 = cbSuyYWrbMdaydC65855878;     cbSuyYWrbMdaydC65855878 = cbSuyYWrbMdaydC40243860;     cbSuyYWrbMdaydC40243860 = cbSuyYWrbMdaydC77653927;     cbSuyYWrbMdaydC77653927 = cbSuyYWrbMdaydC18972504;     cbSuyYWrbMdaydC18972504 = cbSuyYWrbMdaydC14840656;     cbSuyYWrbMdaydC14840656 = cbSuyYWrbMdaydC51950763;     cbSuyYWrbMdaydC51950763 = cbSuyYWrbMdaydC64127792;     cbSuyYWrbMdaydC64127792 = cbSuyYWrbMdaydC63560716;     cbSuyYWrbMdaydC63560716 = cbSuyYWrbMdaydC46291213;     cbSuyYWrbMdaydC46291213 = cbSuyYWrbMdaydC15194085;     cbSuyYWrbMdaydC15194085 = cbSuyYWrbMdaydC70022570;     cbSuyYWrbMdaydC70022570 = cbSuyYWrbMdaydC9228995;     cbSuyYWrbMdaydC9228995 = cbSuyYWrbMdaydC6000997;     cbSuyYWrbMdaydC6000997 = cbSuyYWrbMdaydC4922577;     cbSuyYWrbMdaydC4922577 = cbSuyYWrbMdaydC31786858;     cbSuyYWrbMdaydC31786858 = cbSuyYWrbMdaydC87009539;     cbSuyYWrbMdaydC87009539 = cbSuyYWrbMdaydC22803831;     cbSuyYWrbMdaydC22803831 = cbSuyYWrbMdaydC11830760;     cbSuyYWrbMdaydC11830760 = cbSuyYWrbMdaydC15750012;     cbSuyYWrbMdaydC15750012 = cbSuyYWrbMdaydC26645743;     cbSuyYWrbMdaydC26645743 = cbSuyYWrbMdaydC92949907;     cbSuyYWrbMdaydC92949907 = cbSuyYWrbMdaydC57753566;     cbSuyYWrbMdaydC57753566 = cbSuyYWrbMdaydC59168906;     cbSuyYWrbMdaydC59168906 = cbSuyYWrbMdaydC11929169;     cbSuyYWrbMdaydC11929169 = cbSuyYWrbMdaydC36893313;     cbSuyYWrbMdaydC36893313 = cbSuyYWrbMdaydC92776589;     cbSuyYWrbMdaydC92776589 = cbSuyYWrbMdaydC71993006;     cbSuyYWrbMdaydC71993006 = cbSuyYWrbMdaydC2819350;     cbSuyYWrbMdaydC2819350 = cbSuyYWrbMdaydC17427516;     cbSuyYWrbMdaydC17427516 = cbSuyYWrbMdaydC5030346;     cbSuyYWrbMdaydC5030346 = cbSuyYWrbMdaydC82872355;     cbSuyYWrbMdaydC82872355 = cbSuyYWrbMdaydC17070607;     cbSuyYWrbMdaydC17070607 = cbSuyYWrbMdaydC71426852;     cbSuyYWrbMdaydC71426852 = cbSuyYWrbMdaydC36007154;     cbSuyYWrbMdaydC36007154 = cbSuyYWrbMdaydC63437661;     cbSuyYWrbMdaydC63437661 = cbSuyYWrbMdaydC41554971;     cbSuyYWrbMdaydC41554971 = cbSuyYWrbMdaydC32934309;     cbSuyYWrbMdaydC32934309 = cbSuyYWrbMdaydC1119924;     cbSuyYWrbMdaydC1119924 = cbSuyYWrbMdaydC92227570;     cbSuyYWrbMdaydC92227570 = cbSuyYWrbMdaydC9853512;     cbSuyYWrbMdaydC9853512 = cbSuyYWrbMdaydC94335063;     cbSuyYWrbMdaydC94335063 = cbSuyYWrbMdaydC49087808;     cbSuyYWrbMdaydC49087808 = cbSuyYWrbMdaydC74080971;     cbSuyYWrbMdaydC74080971 = cbSuyYWrbMdaydC19345697;     cbSuyYWrbMdaydC19345697 = cbSuyYWrbMdaydC97375620;     cbSuyYWrbMdaydC97375620 = cbSuyYWrbMdaydC91261209;     cbSuyYWrbMdaydC91261209 = cbSuyYWrbMdaydC77108119;     cbSuyYWrbMdaydC77108119 = cbSuyYWrbMdaydC2017073;     cbSuyYWrbMdaydC2017073 = cbSuyYWrbMdaydC27259972;     cbSuyYWrbMdaydC27259972 = cbSuyYWrbMdaydC35927880;     cbSuyYWrbMdaydC35927880 = cbSuyYWrbMdaydC83436346;     cbSuyYWrbMdaydC83436346 = cbSuyYWrbMdaydC57067533;     cbSuyYWrbMdaydC57067533 = cbSuyYWrbMdaydC79942716;     cbSuyYWrbMdaydC79942716 = cbSuyYWrbMdaydC46644797;     cbSuyYWrbMdaydC46644797 = cbSuyYWrbMdaydC13873631;     cbSuyYWrbMdaydC13873631 = cbSuyYWrbMdaydC8102312;     cbSuyYWrbMdaydC8102312 = cbSuyYWrbMdaydC81074954;     cbSuyYWrbMdaydC81074954 = cbSuyYWrbMdaydC65724759;     cbSuyYWrbMdaydC65724759 = cbSuyYWrbMdaydC82079191;     cbSuyYWrbMdaydC82079191 = cbSuyYWrbMdaydC22064067;     cbSuyYWrbMdaydC22064067 = cbSuyYWrbMdaydC79957756;     cbSuyYWrbMdaydC79957756 = cbSuyYWrbMdaydC42896258;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void umSuQnyAuZsdXqPY32495515() {     int TInEwbzLrieuPRA3950211 = -958130920;    int TInEwbzLrieuPRA20709733 = -564671491;    int TInEwbzLrieuPRA95850119 = -628566150;    int TInEwbzLrieuPRA25434092 = -192123695;    int TInEwbzLrieuPRA17965553 = -967516560;    int TInEwbzLrieuPRA79149749 = -352356119;    int TInEwbzLrieuPRA56865933 = -550259971;    int TInEwbzLrieuPRA52647968 = 87684009;    int TInEwbzLrieuPRA40120168 = -871535593;    int TInEwbzLrieuPRA14987530 = -399210684;    int TInEwbzLrieuPRA967037 = -934704521;    int TInEwbzLrieuPRA76314467 = -388977071;    int TInEwbzLrieuPRA7400987 = -668911660;    int TInEwbzLrieuPRA25204773 = -77740653;    int TInEwbzLrieuPRA98475888 = -965418620;    int TInEwbzLrieuPRA42178858 = -202411063;    int TInEwbzLrieuPRA16572305 = 52182809;    int TInEwbzLrieuPRA79205988 = -284008972;    int TInEwbzLrieuPRA10666186 = -670610739;    int TInEwbzLrieuPRA28686872 = -310610591;    int TInEwbzLrieuPRA5180229 = -783539645;    int TInEwbzLrieuPRA57572660 = -460228244;    int TInEwbzLrieuPRA26039810 = -746791540;    int TInEwbzLrieuPRA91836099 = -210758814;    int TInEwbzLrieuPRA28719912 = -217587533;    int TInEwbzLrieuPRA77974717 = 17688349;    int TInEwbzLrieuPRA7848295 = -748295095;    int TInEwbzLrieuPRA86061718 = -242515171;    int TInEwbzLrieuPRA2925948 = -782594318;    int TInEwbzLrieuPRA95211300 = -764989489;    int TInEwbzLrieuPRA73619963 = -10682612;    int TInEwbzLrieuPRA4866815 = -249586551;    int TInEwbzLrieuPRA64273257 = 29297760;    int TInEwbzLrieuPRA5848662 = -426962877;    int TInEwbzLrieuPRA49916563 = -900272444;    int TInEwbzLrieuPRA61008702 = 69989292;    int TInEwbzLrieuPRA26837878 = -284706780;    int TInEwbzLrieuPRA95202110 = -981212752;    int TInEwbzLrieuPRA25167283 = -101532240;    int TInEwbzLrieuPRA81101744 = -383035599;    int TInEwbzLrieuPRA39063248 = -584086218;    int TInEwbzLrieuPRA32482152 = -147041918;    int TInEwbzLrieuPRA17131062 = -435782854;    int TInEwbzLrieuPRA86917425 = -736922194;    int TInEwbzLrieuPRA17921019 = 73633647;    int TInEwbzLrieuPRA15847490 = -881296353;    int TInEwbzLrieuPRA41127645 = 19974090;    int TInEwbzLrieuPRA9054924 = -838892116;    int TInEwbzLrieuPRA10910487 = -97640452;    int TInEwbzLrieuPRA18294020 = -589785761;    int TInEwbzLrieuPRA80246911 = -201333450;    int TInEwbzLrieuPRA94924162 = -805629583;    int TInEwbzLrieuPRA28444806 = -534945132;    int TInEwbzLrieuPRA1302208 = -88340948;    int TInEwbzLrieuPRA27315287 = -976431595;    int TInEwbzLrieuPRA46377551 = -397902676;    int TInEwbzLrieuPRA94669922 = -817879952;    int TInEwbzLrieuPRA4014021 = -317807336;    int TInEwbzLrieuPRA96714180 = -974536162;    int TInEwbzLrieuPRA39990835 = -885204909;    int TInEwbzLrieuPRA71301455 = -604061025;    int TInEwbzLrieuPRA70804215 = -207744800;    int TInEwbzLrieuPRA49722021 = -129721674;    int TInEwbzLrieuPRA44908868 = -6546104;    int TInEwbzLrieuPRA41367566 = -288528073;    int TInEwbzLrieuPRA96100222 = -585117970;    int TInEwbzLrieuPRA12041210 = -318274832;    int TInEwbzLrieuPRA1552326 = -141948784;    int TInEwbzLrieuPRA75288210 = -177468209;    int TInEwbzLrieuPRA37467186 = -935407913;    int TInEwbzLrieuPRA15340980 = -917704284;    int TInEwbzLrieuPRA21370195 = 33395560;    int TInEwbzLrieuPRA54038705 = -82476733;    int TInEwbzLrieuPRA29564442 = -187575140;    int TInEwbzLrieuPRA89623624 = -726524374;    int TInEwbzLrieuPRA72698076 = -536497728;    int TInEwbzLrieuPRA40441598 = 75554609;    int TInEwbzLrieuPRA39122384 = 90130654;    int TInEwbzLrieuPRA73915080 = -184392462;    int TInEwbzLrieuPRA12872422 = -336291181;    int TInEwbzLrieuPRA36847073 = 97714258;    int TInEwbzLrieuPRA98793370 = -909402979;    int TInEwbzLrieuPRA75151232 = -44874719;    int TInEwbzLrieuPRA84631927 = -92808557;    int TInEwbzLrieuPRA14964390 = -463656040;    int TInEwbzLrieuPRA78695801 = -205053029;    int TInEwbzLrieuPRA76422008 = -714641419;    int TInEwbzLrieuPRA62971050 = -882361292;    int TInEwbzLrieuPRA78533375 = -450531283;    int TInEwbzLrieuPRA3539012 = -402369768;    int TInEwbzLrieuPRA66338780 = -112130757;    int TInEwbzLrieuPRA22823858 = -966899444;    int TInEwbzLrieuPRA98487930 = 93323410;    int TInEwbzLrieuPRA85176448 = -216327331;    int TInEwbzLrieuPRA9800289 = -778974575;    int TInEwbzLrieuPRA68259033 = -276341418;    int TInEwbzLrieuPRA82760131 = 82679755;    int TInEwbzLrieuPRA72222193 = -329236750;    int TInEwbzLrieuPRA45549860 = -348394122;    int TInEwbzLrieuPRA21820797 = -958130920;     TInEwbzLrieuPRA3950211 = TInEwbzLrieuPRA20709733;     TInEwbzLrieuPRA20709733 = TInEwbzLrieuPRA95850119;     TInEwbzLrieuPRA95850119 = TInEwbzLrieuPRA25434092;     TInEwbzLrieuPRA25434092 = TInEwbzLrieuPRA17965553;     TInEwbzLrieuPRA17965553 = TInEwbzLrieuPRA79149749;     TInEwbzLrieuPRA79149749 = TInEwbzLrieuPRA56865933;     TInEwbzLrieuPRA56865933 = TInEwbzLrieuPRA52647968;     TInEwbzLrieuPRA52647968 = TInEwbzLrieuPRA40120168;     TInEwbzLrieuPRA40120168 = TInEwbzLrieuPRA14987530;     TInEwbzLrieuPRA14987530 = TInEwbzLrieuPRA967037;     TInEwbzLrieuPRA967037 = TInEwbzLrieuPRA76314467;     TInEwbzLrieuPRA76314467 = TInEwbzLrieuPRA7400987;     TInEwbzLrieuPRA7400987 = TInEwbzLrieuPRA25204773;     TInEwbzLrieuPRA25204773 = TInEwbzLrieuPRA98475888;     TInEwbzLrieuPRA98475888 = TInEwbzLrieuPRA42178858;     TInEwbzLrieuPRA42178858 = TInEwbzLrieuPRA16572305;     TInEwbzLrieuPRA16572305 = TInEwbzLrieuPRA79205988;     TInEwbzLrieuPRA79205988 = TInEwbzLrieuPRA10666186;     TInEwbzLrieuPRA10666186 = TInEwbzLrieuPRA28686872;     TInEwbzLrieuPRA28686872 = TInEwbzLrieuPRA5180229;     TInEwbzLrieuPRA5180229 = TInEwbzLrieuPRA57572660;     TInEwbzLrieuPRA57572660 = TInEwbzLrieuPRA26039810;     TInEwbzLrieuPRA26039810 = TInEwbzLrieuPRA91836099;     TInEwbzLrieuPRA91836099 = TInEwbzLrieuPRA28719912;     TInEwbzLrieuPRA28719912 = TInEwbzLrieuPRA77974717;     TInEwbzLrieuPRA77974717 = TInEwbzLrieuPRA7848295;     TInEwbzLrieuPRA7848295 = TInEwbzLrieuPRA86061718;     TInEwbzLrieuPRA86061718 = TInEwbzLrieuPRA2925948;     TInEwbzLrieuPRA2925948 = TInEwbzLrieuPRA95211300;     TInEwbzLrieuPRA95211300 = TInEwbzLrieuPRA73619963;     TInEwbzLrieuPRA73619963 = TInEwbzLrieuPRA4866815;     TInEwbzLrieuPRA4866815 = TInEwbzLrieuPRA64273257;     TInEwbzLrieuPRA64273257 = TInEwbzLrieuPRA5848662;     TInEwbzLrieuPRA5848662 = TInEwbzLrieuPRA49916563;     TInEwbzLrieuPRA49916563 = TInEwbzLrieuPRA61008702;     TInEwbzLrieuPRA61008702 = TInEwbzLrieuPRA26837878;     TInEwbzLrieuPRA26837878 = TInEwbzLrieuPRA95202110;     TInEwbzLrieuPRA95202110 = TInEwbzLrieuPRA25167283;     TInEwbzLrieuPRA25167283 = TInEwbzLrieuPRA81101744;     TInEwbzLrieuPRA81101744 = TInEwbzLrieuPRA39063248;     TInEwbzLrieuPRA39063248 = TInEwbzLrieuPRA32482152;     TInEwbzLrieuPRA32482152 = TInEwbzLrieuPRA17131062;     TInEwbzLrieuPRA17131062 = TInEwbzLrieuPRA86917425;     TInEwbzLrieuPRA86917425 = TInEwbzLrieuPRA17921019;     TInEwbzLrieuPRA17921019 = TInEwbzLrieuPRA15847490;     TInEwbzLrieuPRA15847490 = TInEwbzLrieuPRA41127645;     TInEwbzLrieuPRA41127645 = TInEwbzLrieuPRA9054924;     TInEwbzLrieuPRA9054924 = TInEwbzLrieuPRA10910487;     TInEwbzLrieuPRA10910487 = TInEwbzLrieuPRA18294020;     TInEwbzLrieuPRA18294020 = TInEwbzLrieuPRA80246911;     TInEwbzLrieuPRA80246911 = TInEwbzLrieuPRA94924162;     TInEwbzLrieuPRA94924162 = TInEwbzLrieuPRA28444806;     TInEwbzLrieuPRA28444806 = TInEwbzLrieuPRA1302208;     TInEwbzLrieuPRA1302208 = TInEwbzLrieuPRA27315287;     TInEwbzLrieuPRA27315287 = TInEwbzLrieuPRA46377551;     TInEwbzLrieuPRA46377551 = TInEwbzLrieuPRA94669922;     TInEwbzLrieuPRA94669922 = TInEwbzLrieuPRA4014021;     TInEwbzLrieuPRA4014021 = TInEwbzLrieuPRA96714180;     TInEwbzLrieuPRA96714180 = TInEwbzLrieuPRA39990835;     TInEwbzLrieuPRA39990835 = TInEwbzLrieuPRA71301455;     TInEwbzLrieuPRA71301455 = TInEwbzLrieuPRA70804215;     TInEwbzLrieuPRA70804215 = TInEwbzLrieuPRA49722021;     TInEwbzLrieuPRA49722021 = TInEwbzLrieuPRA44908868;     TInEwbzLrieuPRA44908868 = TInEwbzLrieuPRA41367566;     TInEwbzLrieuPRA41367566 = TInEwbzLrieuPRA96100222;     TInEwbzLrieuPRA96100222 = TInEwbzLrieuPRA12041210;     TInEwbzLrieuPRA12041210 = TInEwbzLrieuPRA1552326;     TInEwbzLrieuPRA1552326 = TInEwbzLrieuPRA75288210;     TInEwbzLrieuPRA75288210 = TInEwbzLrieuPRA37467186;     TInEwbzLrieuPRA37467186 = TInEwbzLrieuPRA15340980;     TInEwbzLrieuPRA15340980 = TInEwbzLrieuPRA21370195;     TInEwbzLrieuPRA21370195 = TInEwbzLrieuPRA54038705;     TInEwbzLrieuPRA54038705 = TInEwbzLrieuPRA29564442;     TInEwbzLrieuPRA29564442 = TInEwbzLrieuPRA89623624;     TInEwbzLrieuPRA89623624 = TInEwbzLrieuPRA72698076;     TInEwbzLrieuPRA72698076 = TInEwbzLrieuPRA40441598;     TInEwbzLrieuPRA40441598 = TInEwbzLrieuPRA39122384;     TInEwbzLrieuPRA39122384 = TInEwbzLrieuPRA73915080;     TInEwbzLrieuPRA73915080 = TInEwbzLrieuPRA12872422;     TInEwbzLrieuPRA12872422 = TInEwbzLrieuPRA36847073;     TInEwbzLrieuPRA36847073 = TInEwbzLrieuPRA98793370;     TInEwbzLrieuPRA98793370 = TInEwbzLrieuPRA75151232;     TInEwbzLrieuPRA75151232 = TInEwbzLrieuPRA84631927;     TInEwbzLrieuPRA84631927 = TInEwbzLrieuPRA14964390;     TInEwbzLrieuPRA14964390 = TInEwbzLrieuPRA78695801;     TInEwbzLrieuPRA78695801 = TInEwbzLrieuPRA76422008;     TInEwbzLrieuPRA76422008 = TInEwbzLrieuPRA62971050;     TInEwbzLrieuPRA62971050 = TInEwbzLrieuPRA78533375;     TInEwbzLrieuPRA78533375 = TInEwbzLrieuPRA3539012;     TInEwbzLrieuPRA3539012 = TInEwbzLrieuPRA66338780;     TInEwbzLrieuPRA66338780 = TInEwbzLrieuPRA22823858;     TInEwbzLrieuPRA22823858 = TInEwbzLrieuPRA98487930;     TInEwbzLrieuPRA98487930 = TInEwbzLrieuPRA85176448;     TInEwbzLrieuPRA85176448 = TInEwbzLrieuPRA9800289;     TInEwbzLrieuPRA9800289 = TInEwbzLrieuPRA68259033;     TInEwbzLrieuPRA68259033 = TInEwbzLrieuPRA82760131;     TInEwbzLrieuPRA82760131 = TInEwbzLrieuPRA72222193;     TInEwbzLrieuPRA72222193 = TInEwbzLrieuPRA45549860;     TInEwbzLrieuPRA45549860 = TInEwbzLrieuPRA21820797;     TInEwbzLrieuPRA21820797 = TInEwbzLrieuPRA3950211;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void LboVqEEeYYtMSARE39154259() {     int toNQFpjJicyFiZw3943740 = -830655062;    int toNQFpjJicyFiZw59282049 = -607266617;    int toNQFpjJicyFiZw97163331 = -585694763;    int toNQFpjJicyFiZw10513953 = -12664525;    int toNQFpjJicyFiZw26805655 = -335871881;    int toNQFpjJicyFiZw13955182 = -51788931;    int toNQFpjJicyFiZw25413269 = -254796438;    int toNQFpjJicyFiZw21862019 = -716493309;    int toNQFpjJicyFiZw22860332 = -585181663;    int toNQFpjJicyFiZw11627043 = -36017555;    int toNQFpjJicyFiZw21287643 = -894176593;    int toNQFpjJicyFiZw90606144 = -124455434;    int toNQFpjJicyFiZw39253901 = -690570135;    int toNQFpjJicyFiZw79048993 = -422893704;    int toNQFpjJicyFiZw43647997 = -928131472;    int toNQFpjJicyFiZw16743405 = -535206714;    int toNQFpjJicyFiZw72425064 = -867431761;    int toNQFpjJicyFiZw2206521 = -681080594;    int toNQFpjJicyFiZw31094456 = -188593005;    int toNQFpjJicyFiZw760636 = -922635042;    int toNQFpjJicyFiZw84849652 = -494006019;    int toNQFpjJicyFiZw35868926 = -970220128;    int toNQFpjJicyFiZw62918206 = -105749382;    int toNQFpjJicyFiZw35486208 = -91679337;    int toNQFpjJicyFiZw57162601 = -644281386;    int toNQFpjJicyFiZw94548794 = -990517579;    int toNQFpjJicyFiZw78069876 = 50905192;    int toNQFpjJicyFiZw21465244 = -851482160;    int toNQFpjJicyFiZw15604395 = -328664633;    int toNQFpjJicyFiZw71301170 = -440420863;    int toNQFpjJicyFiZw7032643 = -397775846;    int toNQFpjJicyFiZw89590672 = -641408312;    int toNQFpjJicyFiZw49725681 = -172646456;    int toNQFpjJicyFiZw6131499 = -303644610;    int toNQFpjJicyFiZw32274601 = -59586369;    int toNQFpjJicyFiZw3836893 = -9776454;    int toNQFpjJicyFiZw38779246 = -384360158;    int toNQFpjJicyFiZw65119790 = -976475431;    int toNQFpjJicyFiZw60480879 = -814213656;    int toNQFpjJicyFiZw52974637 = 7532715;    int toNQFpjJicyFiZw26918915 = -672030116;    int toNQFpjJicyFiZw18179814 = -340834001;    int toNQFpjJicyFiZw77414421 = -994913108;    int toNQFpjJicyFiZw46149324 = -293886638;    int toNQFpjJicyFiZw1939072 = -399625754;    int toNQFpjJicyFiZw56275458 = -693341024;    int toNQFpjJicyFiZw68825437 = -461306532;    int toNQFpjJicyFiZw84438505 = -347653472;    int toNQFpjJicyFiZw29568876 = -761404314;    int toNQFpjJicyFiZw38339443 = -176810648;    int toNQFpjJicyFiZw93506280 = -879888457;    int toNQFpjJicyFiZw9471420 = -87775331;    int toNQFpjJicyFiZw3893155 = -426999140;    int toNQFpjJicyFiZw11552478 = -620260800;    int toNQFpjJicyFiZw54304607 = -733330873;    int toNQFpjJicyFiZw68074814 = -860434935;    int toNQFpjJicyFiZw96363842 = -401517235;    int toNQFpjJicyFiZw61677124 = -394015427;    int toNQFpjJicyFiZw53351351 = -368383139;    int toNQFpjJicyFiZw32256860 = -345354303;    int toNQFpjJicyFiZw35885306 = -2694124;    int toNQFpjJicyFiZw3948026 = -403314279;    int toNQFpjJicyFiZw6257625 = -287828676;    int toNQFpjJicyFiZw51559162 = -44760800;    int toNQFpjJicyFiZw4594401 = -638241709;    int toNQFpjJicyFiZw31696970 = -152768281;    int toNQFpjJicyFiZw40880464 = -951808978;    int toNQFpjJicyFiZw33122403 = -286925526;    int toNQFpjJicyFiZw46774393 = -263307336;    int toNQFpjJicyFiZw39811105 = -818355019;    int toNQFpjJicyFiZw77964159 = -50846556;    int toNQFpjJicyFiZw7305275 = -890956330;    int toNQFpjJicyFiZw41725641 = -866866939;    int toNQFpjJicyFiZw78119819 = -96125721;    int toNQFpjJicyFiZw73841720 = -150604927;    int toNQFpjJicyFiZw66669839 = -53172019;    int toNQFpjJicyFiZw58454504 = -975307020;    int toNQFpjJicyFiZw16768882 = -811862745;    int toNQFpjJicyFiZw33547137 = -692053583;    int toNQFpjJicyFiZw887143 = -950940363;    int toNQFpjJicyFiZw25723358 = -429211048;    int toNQFpjJicyFiZw93631370 = -601441336;    int toNQFpjJicyFiZw91896367 = 9922153;    int toNQFpjJicyFiZw77264951 = -51853986;    int toNQFpjJicyFiZw77794889 = -560532407;    int toNQFpjJicyFiZw97561222 = -210000515;    int toNQFpjJicyFiZw85697517 = -114409173;    int toNQFpjJicyFiZw38173203 = -552385657;    int toNQFpjJicyFiZw51826892 = -570313737;    int toNQFpjJicyFiZw64199787 = -199151435;    int toNQFpjJicyFiZw7473050 = -608259220;    int toNQFpjJicyFiZw77102121 = -990344731;    int toNQFpjJicyFiZw11768439 = -508092292;    int toNQFpjJicyFiZw28224020 = -368859354;    int toNQFpjJicyFiZw17089331 = -989773162;    int toNQFpjJicyFiZw22970890 = -168715838;    int toNQFpjJicyFiZw11922189 = 46994675;    int toNQFpjJicyFiZw25855260 = -850152308;    int toNQFpjJicyFiZw41554924 = -655644929;    int toNQFpjJicyFiZw70242101 = -830655062;     toNQFpjJicyFiZw3943740 = toNQFpjJicyFiZw59282049;     toNQFpjJicyFiZw59282049 = toNQFpjJicyFiZw97163331;     toNQFpjJicyFiZw97163331 = toNQFpjJicyFiZw10513953;     toNQFpjJicyFiZw10513953 = toNQFpjJicyFiZw26805655;     toNQFpjJicyFiZw26805655 = toNQFpjJicyFiZw13955182;     toNQFpjJicyFiZw13955182 = toNQFpjJicyFiZw25413269;     toNQFpjJicyFiZw25413269 = toNQFpjJicyFiZw21862019;     toNQFpjJicyFiZw21862019 = toNQFpjJicyFiZw22860332;     toNQFpjJicyFiZw22860332 = toNQFpjJicyFiZw11627043;     toNQFpjJicyFiZw11627043 = toNQFpjJicyFiZw21287643;     toNQFpjJicyFiZw21287643 = toNQFpjJicyFiZw90606144;     toNQFpjJicyFiZw90606144 = toNQFpjJicyFiZw39253901;     toNQFpjJicyFiZw39253901 = toNQFpjJicyFiZw79048993;     toNQFpjJicyFiZw79048993 = toNQFpjJicyFiZw43647997;     toNQFpjJicyFiZw43647997 = toNQFpjJicyFiZw16743405;     toNQFpjJicyFiZw16743405 = toNQFpjJicyFiZw72425064;     toNQFpjJicyFiZw72425064 = toNQFpjJicyFiZw2206521;     toNQFpjJicyFiZw2206521 = toNQFpjJicyFiZw31094456;     toNQFpjJicyFiZw31094456 = toNQFpjJicyFiZw760636;     toNQFpjJicyFiZw760636 = toNQFpjJicyFiZw84849652;     toNQFpjJicyFiZw84849652 = toNQFpjJicyFiZw35868926;     toNQFpjJicyFiZw35868926 = toNQFpjJicyFiZw62918206;     toNQFpjJicyFiZw62918206 = toNQFpjJicyFiZw35486208;     toNQFpjJicyFiZw35486208 = toNQFpjJicyFiZw57162601;     toNQFpjJicyFiZw57162601 = toNQFpjJicyFiZw94548794;     toNQFpjJicyFiZw94548794 = toNQFpjJicyFiZw78069876;     toNQFpjJicyFiZw78069876 = toNQFpjJicyFiZw21465244;     toNQFpjJicyFiZw21465244 = toNQFpjJicyFiZw15604395;     toNQFpjJicyFiZw15604395 = toNQFpjJicyFiZw71301170;     toNQFpjJicyFiZw71301170 = toNQFpjJicyFiZw7032643;     toNQFpjJicyFiZw7032643 = toNQFpjJicyFiZw89590672;     toNQFpjJicyFiZw89590672 = toNQFpjJicyFiZw49725681;     toNQFpjJicyFiZw49725681 = toNQFpjJicyFiZw6131499;     toNQFpjJicyFiZw6131499 = toNQFpjJicyFiZw32274601;     toNQFpjJicyFiZw32274601 = toNQFpjJicyFiZw3836893;     toNQFpjJicyFiZw3836893 = toNQFpjJicyFiZw38779246;     toNQFpjJicyFiZw38779246 = toNQFpjJicyFiZw65119790;     toNQFpjJicyFiZw65119790 = toNQFpjJicyFiZw60480879;     toNQFpjJicyFiZw60480879 = toNQFpjJicyFiZw52974637;     toNQFpjJicyFiZw52974637 = toNQFpjJicyFiZw26918915;     toNQFpjJicyFiZw26918915 = toNQFpjJicyFiZw18179814;     toNQFpjJicyFiZw18179814 = toNQFpjJicyFiZw77414421;     toNQFpjJicyFiZw77414421 = toNQFpjJicyFiZw46149324;     toNQFpjJicyFiZw46149324 = toNQFpjJicyFiZw1939072;     toNQFpjJicyFiZw1939072 = toNQFpjJicyFiZw56275458;     toNQFpjJicyFiZw56275458 = toNQFpjJicyFiZw68825437;     toNQFpjJicyFiZw68825437 = toNQFpjJicyFiZw84438505;     toNQFpjJicyFiZw84438505 = toNQFpjJicyFiZw29568876;     toNQFpjJicyFiZw29568876 = toNQFpjJicyFiZw38339443;     toNQFpjJicyFiZw38339443 = toNQFpjJicyFiZw93506280;     toNQFpjJicyFiZw93506280 = toNQFpjJicyFiZw9471420;     toNQFpjJicyFiZw9471420 = toNQFpjJicyFiZw3893155;     toNQFpjJicyFiZw3893155 = toNQFpjJicyFiZw11552478;     toNQFpjJicyFiZw11552478 = toNQFpjJicyFiZw54304607;     toNQFpjJicyFiZw54304607 = toNQFpjJicyFiZw68074814;     toNQFpjJicyFiZw68074814 = toNQFpjJicyFiZw96363842;     toNQFpjJicyFiZw96363842 = toNQFpjJicyFiZw61677124;     toNQFpjJicyFiZw61677124 = toNQFpjJicyFiZw53351351;     toNQFpjJicyFiZw53351351 = toNQFpjJicyFiZw32256860;     toNQFpjJicyFiZw32256860 = toNQFpjJicyFiZw35885306;     toNQFpjJicyFiZw35885306 = toNQFpjJicyFiZw3948026;     toNQFpjJicyFiZw3948026 = toNQFpjJicyFiZw6257625;     toNQFpjJicyFiZw6257625 = toNQFpjJicyFiZw51559162;     toNQFpjJicyFiZw51559162 = toNQFpjJicyFiZw4594401;     toNQFpjJicyFiZw4594401 = toNQFpjJicyFiZw31696970;     toNQFpjJicyFiZw31696970 = toNQFpjJicyFiZw40880464;     toNQFpjJicyFiZw40880464 = toNQFpjJicyFiZw33122403;     toNQFpjJicyFiZw33122403 = toNQFpjJicyFiZw46774393;     toNQFpjJicyFiZw46774393 = toNQFpjJicyFiZw39811105;     toNQFpjJicyFiZw39811105 = toNQFpjJicyFiZw77964159;     toNQFpjJicyFiZw77964159 = toNQFpjJicyFiZw7305275;     toNQFpjJicyFiZw7305275 = toNQFpjJicyFiZw41725641;     toNQFpjJicyFiZw41725641 = toNQFpjJicyFiZw78119819;     toNQFpjJicyFiZw78119819 = toNQFpjJicyFiZw73841720;     toNQFpjJicyFiZw73841720 = toNQFpjJicyFiZw66669839;     toNQFpjJicyFiZw66669839 = toNQFpjJicyFiZw58454504;     toNQFpjJicyFiZw58454504 = toNQFpjJicyFiZw16768882;     toNQFpjJicyFiZw16768882 = toNQFpjJicyFiZw33547137;     toNQFpjJicyFiZw33547137 = toNQFpjJicyFiZw887143;     toNQFpjJicyFiZw887143 = toNQFpjJicyFiZw25723358;     toNQFpjJicyFiZw25723358 = toNQFpjJicyFiZw93631370;     toNQFpjJicyFiZw93631370 = toNQFpjJicyFiZw91896367;     toNQFpjJicyFiZw91896367 = toNQFpjJicyFiZw77264951;     toNQFpjJicyFiZw77264951 = toNQFpjJicyFiZw77794889;     toNQFpjJicyFiZw77794889 = toNQFpjJicyFiZw97561222;     toNQFpjJicyFiZw97561222 = toNQFpjJicyFiZw85697517;     toNQFpjJicyFiZw85697517 = toNQFpjJicyFiZw38173203;     toNQFpjJicyFiZw38173203 = toNQFpjJicyFiZw51826892;     toNQFpjJicyFiZw51826892 = toNQFpjJicyFiZw64199787;     toNQFpjJicyFiZw64199787 = toNQFpjJicyFiZw7473050;     toNQFpjJicyFiZw7473050 = toNQFpjJicyFiZw77102121;     toNQFpjJicyFiZw77102121 = toNQFpjJicyFiZw11768439;     toNQFpjJicyFiZw11768439 = toNQFpjJicyFiZw28224020;     toNQFpjJicyFiZw28224020 = toNQFpjJicyFiZw17089331;     toNQFpjJicyFiZw17089331 = toNQFpjJicyFiZw22970890;     toNQFpjJicyFiZw22970890 = toNQFpjJicyFiZw11922189;     toNQFpjJicyFiZw11922189 = toNQFpjJicyFiZw25855260;     toNQFpjJicyFiZw25855260 = toNQFpjJicyFiZw41554924;     toNQFpjJicyFiZw41554924 = toNQFpjJicyFiZw70242101;     toNQFpjJicyFiZw70242101 = toNQFpjJicyFiZw3943740;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void zwyvCjtyAFCchoos93570472() {     int mqfulAikHgSbrmH74595406 = -348804192;    int mqfulAikHgSbrmH27955567 = -478909916;    int mqfulAikHgSbrmH77132251 = 72010777;    int mqfulAikHgSbrmH50445239 = -202802914;    int mqfulAikHgSbrmH48255976 = -119278229;    int mqfulAikHgSbrmH3954945 = -853370082;    int mqfulAikHgSbrmH43792165 = -253572148;    int mqfulAikHgSbrmH78704399 = 36689602;    int mqfulAikHgSbrmH68725386 = -678528991;    int mqfulAikHgSbrmH34593504 = -337112906;    int mqfulAikHgSbrmH63536803 = -929078522;    int mqfulAikHgSbrmH36152716 = -440219227;    int mqfulAikHgSbrmH19081288 = -600107193;    int mqfulAikHgSbrmH62896027 = -223921915;    int mqfulAikHgSbrmH26368017 = 22118524;    int mqfulAikHgSbrmH49272045 = -105190300;    int mqfulAikHgSbrmH66277891 = -776616891;    int mqfulAikHgSbrmH22500015 = -670246790;    int mqfulAikHgSbrmH12802238 = -895780947;    int mqfulAikHgSbrmH59547344 = -365001347;    int mqfulAikHgSbrmH39985407 = -402685159;    int mqfulAikHgSbrmH97264690 = -201505433;    int mqfulAikHgSbrmH64862015 = -673592527;    int mqfulAikHgSbrmH69335744 = -784080243;    int mqfulAikHgSbrmH21286990 = -320186074;    int mqfulAikHgSbrmH25146375 = 11197976;    int mqfulAikHgSbrmH86330661 = -344272583;    int mqfulAikHgSbrmH29247469 = -256605927;    int mqfulAikHgSbrmH47746882 = -623022850;    int mqfulAikHgSbrmH10473266 = -998815934;    int mqfulAikHgSbrmH14104639 = -146209532;    int mqfulAikHgSbrmH34102133 = -969705475;    int mqfulAikHgSbrmH32671042 = -119608065;    int mqfulAikHgSbrmH71496473 = -967174294;    int mqfulAikHgSbrmH10298837 = 93466852;    int mqfulAikHgSbrmH63830151 = -89003002;    int mqfulAikHgSbrmH82570731 = -572953366;    int mqfulAikHgSbrmH17912279 = -760588539;    int mqfulAikHgSbrmH39014168 = -769942292;    int mqfulAikHgSbrmH48077935 = -610075648;    int mqfulAikHgSbrmH86992199 = 43951006;    int mqfulAikHgSbrmH46662804 = -110223070;    int mqfulAikHgSbrmH26381382 = -981667160;    int mqfulAikHgSbrmH52657625 = -365861609;    int mqfulAikHgSbrmH97893129 = -708272149;    int mqfulAikHgSbrmH62681293 = -451678179;    int mqfulAikHgSbrmH94956640 = -577376332;    int mqfulAikHgSbrmH50052715 = -799783767;    int mqfulAikHgSbrmH35010328 = -747168283;    int mqfulAikHgSbrmH46766930 = -459819526;    int mqfulAikHgSbrmH61695605 = -426460578;    int mqfulAikHgSbrmH51342924 = -56995053;    int mqfulAikHgSbrmH58968811 = -354797182;    int mqfulAikHgSbrmH1779783 = -181142296;    int mqfulAikHgSbrmH53585066 = -844271787;    int mqfulAikHgSbrmH77330716 = -47298759;    int mqfulAikHgSbrmH63093551 = -805317389;    int mqfulAikHgSbrmH7796507 = -143908981;    int mqfulAikHgSbrmH29158250 = -882616841;    int mqfulAikHgSbrmH23109601 = -30476205;    int mqfulAikHgSbrmH17624283 = -409097500;    int mqfulAikHgSbrmH14544696 = -996966221;    int mqfulAikHgSbrmH30957517 = -340287549;    int mqfulAikHgSbrmH58252120 = -679713058;    int mqfulAikHgSbrmH20488865 = -90903374;    int mqfulAikHgSbrmH29434671 = -959373047;    int mqfulAikHgSbrmH3481675 = -220611162;    int mqfulAikHgSbrmH47584815 = -632932900;    int mqfulAikHgSbrmH52597190 = -217388767;    int mqfulAikHgSbrmH62537866 = -888878474;    int mqfulAikHgSbrmH66701314 = -532236935;    int mqfulAikHgSbrmH48365612 = 83971647;    int mqfulAikHgSbrmH83485847 = -900304498;    int mqfulAikHgSbrmH64724302 = -185705300;    int mqfulAikHgSbrmH72555145 = -308952353;    int mqfulAikHgSbrmH93322602 = -192462090;    int mqfulAikHgSbrmH70883309 = -219838274;    int mqfulAikHgSbrmH12204391 = -207730918;    int mqfulAikHgSbrmH71442614 = 24191905;    int mqfulAikHgSbrmH58605696 = -868507896;    int mqfulAikHgSbrmH30189735 = -411425693;    int mqfulAikHgSbrmH36277947 = -544488816;    int mqfulAikHgSbrmH94237140 = -509437645;    int mqfulAikHgSbrmH979953 = -63203324;    int mqfulAikHgSbrmH48777661 = -472355356;    int mqfulAikHgSbrmH62761714 = 10785521;    int mqfulAikHgSbrmH75133321 = -514908294;    int mqfulAikHgSbrmH30891259 = -938465769;    int mqfulAikHgSbrmH17911408 = -22902507;    int mqfulAikHgSbrmH32968121 = -859234389;    int mqfulAikHgSbrmH736600 = -283685613;    int mqfulAikHgSbrmH74774225 = -329044385;    int mqfulAikHgSbrmH88754029 = -877971698;    int mqfulAikHgSbrmH15904568 = -639466088;    int mqfulAikHgSbrmH30453652 = -100978148;    int mqfulAikHgSbrmH72447503 = 40917226;    int mqfulAikHgSbrmH15705287 = -769935521;    int mqfulAikHgSbrmH68129261 = -201954102;    int mqfulAikHgSbrmH32168760 = -174958236;    int mqfulAikHgSbrmH68458459 = -348804192;     mqfulAikHgSbrmH74595406 = mqfulAikHgSbrmH27955567;     mqfulAikHgSbrmH27955567 = mqfulAikHgSbrmH77132251;     mqfulAikHgSbrmH77132251 = mqfulAikHgSbrmH50445239;     mqfulAikHgSbrmH50445239 = mqfulAikHgSbrmH48255976;     mqfulAikHgSbrmH48255976 = mqfulAikHgSbrmH3954945;     mqfulAikHgSbrmH3954945 = mqfulAikHgSbrmH43792165;     mqfulAikHgSbrmH43792165 = mqfulAikHgSbrmH78704399;     mqfulAikHgSbrmH78704399 = mqfulAikHgSbrmH68725386;     mqfulAikHgSbrmH68725386 = mqfulAikHgSbrmH34593504;     mqfulAikHgSbrmH34593504 = mqfulAikHgSbrmH63536803;     mqfulAikHgSbrmH63536803 = mqfulAikHgSbrmH36152716;     mqfulAikHgSbrmH36152716 = mqfulAikHgSbrmH19081288;     mqfulAikHgSbrmH19081288 = mqfulAikHgSbrmH62896027;     mqfulAikHgSbrmH62896027 = mqfulAikHgSbrmH26368017;     mqfulAikHgSbrmH26368017 = mqfulAikHgSbrmH49272045;     mqfulAikHgSbrmH49272045 = mqfulAikHgSbrmH66277891;     mqfulAikHgSbrmH66277891 = mqfulAikHgSbrmH22500015;     mqfulAikHgSbrmH22500015 = mqfulAikHgSbrmH12802238;     mqfulAikHgSbrmH12802238 = mqfulAikHgSbrmH59547344;     mqfulAikHgSbrmH59547344 = mqfulAikHgSbrmH39985407;     mqfulAikHgSbrmH39985407 = mqfulAikHgSbrmH97264690;     mqfulAikHgSbrmH97264690 = mqfulAikHgSbrmH64862015;     mqfulAikHgSbrmH64862015 = mqfulAikHgSbrmH69335744;     mqfulAikHgSbrmH69335744 = mqfulAikHgSbrmH21286990;     mqfulAikHgSbrmH21286990 = mqfulAikHgSbrmH25146375;     mqfulAikHgSbrmH25146375 = mqfulAikHgSbrmH86330661;     mqfulAikHgSbrmH86330661 = mqfulAikHgSbrmH29247469;     mqfulAikHgSbrmH29247469 = mqfulAikHgSbrmH47746882;     mqfulAikHgSbrmH47746882 = mqfulAikHgSbrmH10473266;     mqfulAikHgSbrmH10473266 = mqfulAikHgSbrmH14104639;     mqfulAikHgSbrmH14104639 = mqfulAikHgSbrmH34102133;     mqfulAikHgSbrmH34102133 = mqfulAikHgSbrmH32671042;     mqfulAikHgSbrmH32671042 = mqfulAikHgSbrmH71496473;     mqfulAikHgSbrmH71496473 = mqfulAikHgSbrmH10298837;     mqfulAikHgSbrmH10298837 = mqfulAikHgSbrmH63830151;     mqfulAikHgSbrmH63830151 = mqfulAikHgSbrmH82570731;     mqfulAikHgSbrmH82570731 = mqfulAikHgSbrmH17912279;     mqfulAikHgSbrmH17912279 = mqfulAikHgSbrmH39014168;     mqfulAikHgSbrmH39014168 = mqfulAikHgSbrmH48077935;     mqfulAikHgSbrmH48077935 = mqfulAikHgSbrmH86992199;     mqfulAikHgSbrmH86992199 = mqfulAikHgSbrmH46662804;     mqfulAikHgSbrmH46662804 = mqfulAikHgSbrmH26381382;     mqfulAikHgSbrmH26381382 = mqfulAikHgSbrmH52657625;     mqfulAikHgSbrmH52657625 = mqfulAikHgSbrmH97893129;     mqfulAikHgSbrmH97893129 = mqfulAikHgSbrmH62681293;     mqfulAikHgSbrmH62681293 = mqfulAikHgSbrmH94956640;     mqfulAikHgSbrmH94956640 = mqfulAikHgSbrmH50052715;     mqfulAikHgSbrmH50052715 = mqfulAikHgSbrmH35010328;     mqfulAikHgSbrmH35010328 = mqfulAikHgSbrmH46766930;     mqfulAikHgSbrmH46766930 = mqfulAikHgSbrmH61695605;     mqfulAikHgSbrmH61695605 = mqfulAikHgSbrmH51342924;     mqfulAikHgSbrmH51342924 = mqfulAikHgSbrmH58968811;     mqfulAikHgSbrmH58968811 = mqfulAikHgSbrmH1779783;     mqfulAikHgSbrmH1779783 = mqfulAikHgSbrmH53585066;     mqfulAikHgSbrmH53585066 = mqfulAikHgSbrmH77330716;     mqfulAikHgSbrmH77330716 = mqfulAikHgSbrmH63093551;     mqfulAikHgSbrmH63093551 = mqfulAikHgSbrmH7796507;     mqfulAikHgSbrmH7796507 = mqfulAikHgSbrmH29158250;     mqfulAikHgSbrmH29158250 = mqfulAikHgSbrmH23109601;     mqfulAikHgSbrmH23109601 = mqfulAikHgSbrmH17624283;     mqfulAikHgSbrmH17624283 = mqfulAikHgSbrmH14544696;     mqfulAikHgSbrmH14544696 = mqfulAikHgSbrmH30957517;     mqfulAikHgSbrmH30957517 = mqfulAikHgSbrmH58252120;     mqfulAikHgSbrmH58252120 = mqfulAikHgSbrmH20488865;     mqfulAikHgSbrmH20488865 = mqfulAikHgSbrmH29434671;     mqfulAikHgSbrmH29434671 = mqfulAikHgSbrmH3481675;     mqfulAikHgSbrmH3481675 = mqfulAikHgSbrmH47584815;     mqfulAikHgSbrmH47584815 = mqfulAikHgSbrmH52597190;     mqfulAikHgSbrmH52597190 = mqfulAikHgSbrmH62537866;     mqfulAikHgSbrmH62537866 = mqfulAikHgSbrmH66701314;     mqfulAikHgSbrmH66701314 = mqfulAikHgSbrmH48365612;     mqfulAikHgSbrmH48365612 = mqfulAikHgSbrmH83485847;     mqfulAikHgSbrmH83485847 = mqfulAikHgSbrmH64724302;     mqfulAikHgSbrmH64724302 = mqfulAikHgSbrmH72555145;     mqfulAikHgSbrmH72555145 = mqfulAikHgSbrmH93322602;     mqfulAikHgSbrmH93322602 = mqfulAikHgSbrmH70883309;     mqfulAikHgSbrmH70883309 = mqfulAikHgSbrmH12204391;     mqfulAikHgSbrmH12204391 = mqfulAikHgSbrmH71442614;     mqfulAikHgSbrmH71442614 = mqfulAikHgSbrmH58605696;     mqfulAikHgSbrmH58605696 = mqfulAikHgSbrmH30189735;     mqfulAikHgSbrmH30189735 = mqfulAikHgSbrmH36277947;     mqfulAikHgSbrmH36277947 = mqfulAikHgSbrmH94237140;     mqfulAikHgSbrmH94237140 = mqfulAikHgSbrmH979953;     mqfulAikHgSbrmH979953 = mqfulAikHgSbrmH48777661;     mqfulAikHgSbrmH48777661 = mqfulAikHgSbrmH62761714;     mqfulAikHgSbrmH62761714 = mqfulAikHgSbrmH75133321;     mqfulAikHgSbrmH75133321 = mqfulAikHgSbrmH30891259;     mqfulAikHgSbrmH30891259 = mqfulAikHgSbrmH17911408;     mqfulAikHgSbrmH17911408 = mqfulAikHgSbrmH32968121;     mqfulAikHgSbrmH32968121 = mqfulAikHgSbrmH736600;     mqfulAikHgSbrmH736600 = mqfulAikHgSbrmH74774225;     mqfulAikHgSbrmH74774225 = mqfulAikHgSbrmH88754029;     mqfulAikHgSbrmH88754029 = mqfulAikHgSbrmH15904568;     mqfulAikHgSbrmH15904568 = mqfulAikHgSbrmH30453652;     mqfulAikHgSbrmH30453652 = mqfulAikHgSbrmH72447503;     mqfulAikHgSbrmH72447503 = mqfulAikHgSbrmH15705287;     mqfulAikHgSbrmH15705287 = mqfulAikHgSbrmH68129261;     mqfulAikHgSbrmH68129261 = mqfulAikHgSbrmH32168760;     mqfulAikHgSbrmH32168760 = mqfulAikHgSbrmH68458459;     mqfulAikHgSbrmH68458459 = mqfulAikHgSbrmH74595406;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ufiRckfPuFCWXJaA229217() {     int PRYDnakABTFRvSa74588935 = -221328333;    int PRYDnakABTFRvSa66527883 = -521505041;    int PRYDnakABTFRvSa78445463 = -985117837;    int PRYDnakABTFRvSa35525100 = -23343745;    int PRYDnakABTFRvSa57096078 = -587633550;    int PRYDnakABTFRvSa38760377 = -552802894;    int PRYDnakABTFRvSa12339501 = 41891385;    int PRYDnakABTFRvSa47918450 = -767487716;    int PRYDnakABTFRvSa51465551 = -392175061;    int PRYDnakABTFRvSa31233017 = 26080223;    int PRYDnakABTFRvSa83857408 = -888550594;    int PRYDnakABTFRvSa50444393 = -175697589;    int PRYDnakABTFRvSa50934202 = -621765667;    int PRYDnakABTFRvSa16740247 = -569074966;    int PRYDnakABTFRvSa71540126 = 59405672;    int PRYDnakABTFRvSa23836593 = -437985951;    int PRYDnakABTFRvSa22130651 = -596231461;    int PRYDnakABTFRvSa45500548 = 32681588;    int PRYDnakABTFRvSa33230507 = -413763214;    int PRYDnakABTFRvSa31621107 = -977025798;    int PRYDnakABTFRvSa19654831 = -113151533;    int PRYDnakABTFRvSa75560956 = -711497316;    int PRYDnakABTFRvSa1740412 = -32550369;    int PRYDnakABTFRvSa12985853 = -665000765;    int PRYDnakABTFRvSa49729679 = -746879927;    int PRYDnakABTFRvSa41720452 = -997007953;    int PRYDnakABTFRvSa56552243 = -645072296;    int PRYDnakABTFRvSa64650993 = -865572916;    int PRYDnakABTFRvSa60425329 = -169093165;    int PRYDnakABTFRvSa86563136 = -674247307;    int PRYDnakABTFRvSa47517318 = -533302766;    int PRYDnakABTFRvSa18825990 = -261527237;    int PRYDnakABTFRvSa18123465 = -321552281;    int PRYDnakABTFRvSa71779310 = -843856026;    int PRYDnakABTFRvSa92656874 = -165847073;    int PRYDnakABTFRvSa6658341 = -168768748;    int PRYDnakABTFRvSa94512099 = -672606744;    int PRYDnakABTFRvSa87829958 = -755851218;    int PRYDnakABTFRvSa74327765 = -382623709;    int PRYDnakABTFRvSa19950828 = -219507334;    int PRYDnakABTFRvSa74847866 = -43992893;    int PRYDnakABTFRvSa32360465 = -304015152;    int PRYDnakABTFRvSa86664741 = -440797414;    int PRYDnakABTFRvSa11889523 = 77173947;    int PRYDnakABTFRvSa81911182 = -81531550;    int PRYDnakABTFRvSa3109262 = -263722849;    int PRYDnakABTFRvSa22654433 = 41343046;    int PRYDnakABTFRvSa25436296 = -308545124;    int PRYDnakABTFRvSa53668717 = -310932144;    int PRYDnakABTFRvSa66812352 = -46844413;    int PRYDnakABTFRvSa74954975 = -5015585;    int PRYDnakABTFRvSa65890182 = -439140802;    int PRYDnakABTFRvSa34417160 = -246851190;    int PRYDnakABTFRvSa12030054 = -713062148;    int PRYDnakABTFRvSa80574386 = -601171065;    int PRYDnakABTFRvSa99027978 = -509831018;    int PRYDnakABTFRvSa64787471 = -388954672;    int PRYDnakABTFRvSa65459610 = -220117072;    int PRYDnakABTFRvSa85795420 = -276463818;    int PRYDnakABTFRvSa15375626 = -590625598;    int PRYDnakABTFRvSa82208133 = -907730599;    int PRYDnakABTFRvSa47688507 = -92535700;    int PRYDnakABTFRvSa87493121 = -498394551;    int PRYDnakABTFRvSa64902414 = -717927754;    int PRYDnakABTFRvSa83715699 = -440617011;    int PRYDnakABTFRvSa65031418 = -527023358;    int PRYDnakABTFRvSa32320928 = -854145309;    int PRYDnakABTFRvSa79154891 = -777909642;    int PRYDnakABTFRvSa24083372 = -303227894;    int PRYDnakABTFRvSa64881785 = -771825580;    int PRYDnakABTFRvSa29324494 = -765379208;    int PRYDnakABTFRvSa34300693 = -840380244;    int PRYDnakABTFRvSa71172783 = -584694704;    int PRYDnakABTFRvSa13279680 = -94255881;    int PRYDnakABTFRvSa56773241 = -833032906;    int PRYDnakABTFRvSa87294365 = -809136381;    int PRYDnakABTFRvSa88896214 = -170699903;    int PRYDnakABTFRvSa89850888 = -9724317;    int PRYDnakABTFRvSa31074671 = -483469216;    int PRYDnakABTFRvSa46620418 = -383157078;    int PRYDnakABTFRvSa19066020 = -938350999;    int PRYDnakABTFRvSa31115947 = -236527173;    int PRYDnakABTFRvSa10982277 = -454640772;    int PRYDnakABTFRvSa93612976 = -22248752;    int PRYDnakABTFRvSa11608162 = -569231722;    int PRYDnakABTFRvSa81627135 = 5838035;    int PRYDnakABTFRvSa84408830 = 85323953;    int PRYDnakABTFRvSa6093412 = -608490134;    int PRYDnakABTFRvSa91204924 = -142684962;    int PRYDnakABTFRvSa93628896 = -656016056;    int PRYDnakABTFRvSa41870869 = -779814076;    int PRYDnakABTFRvSa29052489 = -352489672;    int PRYDnakABTFRvSa2034538 = -379387400;    int PRYDnakABTFRvSa58952139 = -791998111;    int PRYDnakABTFRvSa37742694 = -311776736;    int PRYDnakABTFRvSa27159360 = -951457194;    int PRYDnakABTFRvSa44867344 = -805620601;    int PRYDnakABTFRvSa21762327 = -722869660;    int PRYDnakABTFRvSa28173823 = -482209043;    int PRYDnakABTFRvSa16879764 = -221328333;     PRYDnakABTFRvSa74588935 = PRYDnakABTFRvSa66527883;     PRYDnakABTFRvSa66527883 = PRYDnakABTFRvSa78445463;     PRYDnakABTFRvSa78445463 = PRYDnakABTFRvSa35525100;     PRYDnakABTFRvSa35525100 = PRYDnakABTFRvSa57096078;     PRYDnakABTFRvSa57096078 = PRYDnakABTFRvSa38760377;     PRYDnakABTFRvSa38760377 = PRYDnakABTFRvSa12339501;     PRYDnakABTFRvSa12339501 = PRYDnakABTFRvSa47918450;     PRYDnakABTFRvSa47918450 = PRYDnakABTFRvSa51465551;     PRYDnakABTFRvSa51465551 = PRYDnakABTFRvSa31233017;     PRYDnakABTFRvSa31233017 = PRYDnakABTFRvSa83857408;     PRYDnakABTFRvSa83857408 = PRYDnakABTFRvSa50444393;     PRYDnakABTFRvSa50444393 = PRYDnakABTFRvSa50934202;     PRYDnakABTFRvSa50934202 = PRYDnakABTFRvSa16740247;     PRYDnakABTFRvSa16740247 = PRYDnakABTFRvSa71540126;     PRYDnakABTFRvSa71540126 = PRYDnakABTFRvSa23836593;     PRYDnakABTFRvSa23836593 = PRYDnakABTFRvSa22130651;     PRYDnakABTFRvSa22130651 = PRYDnakABTFRvSa45500548;     PRYDnakABTFRvSa45500548 = PRYDnakABTFRvSa33230507;     PRYDnakABTFRvSa33230507 = PRYDnakABTFRvSa31621107;     PRYDnakABTFRvSa31621107 = PRYDnakABTFRvSa19654831;     PRYDnakABTFRvSa19654831 = PRYDnakABTFRvSa75560956;     PRYDnakABTFRvSa75560956 = PRYDnakABTFRvSa1740412;     PRYDnakABTFRvSa1740412 = PRYDnakABTFRvSa12985853;     PRYDnakABTFRvSa12985853 = PRYDnakABTFRvSa49729679;     PRYDnakABTFRvSa49729679 = PRYDnakABTFRvSa41720452;     PRYDnakABTFRvSa41720452 = PRYDnakABTFRvSa56552243;     PRYDnakABTFRvSa56552243 = PRYDnakABTFRvSa64650993;     PRYDnakABTFRvSa64650993 = PRYDnakABTFRvSa60425329;     PRYDnakABTFRvSa60425329 = PRYDnakABTFRvSa86563136;     PRYDnakABTFRvSa86563136 = PRYDnakABTFRvSa47517318;     PRYDnakABTFRvSa47517318 = PRYDnakABTFRvSa18825990;     PRYDnakABTFRvSa18825990 = PRYDnakABTFRvSa18123465;     PRYDnakABTFRvSa18123465 = PRYDnakABTFRvSa71779310;     PRYDnakABTFRvSa71779310 = PRYDnakABTFRvSa92656874;     PRYDnakABTFRvSa92656874 = PRYDnakABTFRvSa6658341;     PRYDnakABTFRvSa6658341 = PRYDnakABTFRvSa94512099;     PRYDnakABTFRvSa94512099 = PRYDnakABTFRvSa87829958;     PRYDnakABTFRvSa87829958 = PRYDnakABTFRvSa74327765;     PRYDnakABTFRvSa74327765 = PRYDnakABTFRvSa19950828;     PRYDnakABTFRvSa19950828 = PRYDnakABTFRvSa74847866;     PRYDnakABTFRvSa74847866 = PRYDnakABTFRvSa32360465;     PRYDnakABTFRvSa32360465 = PRYDnakABTFRvSa86664741;     PRYDnakABTFRvSa86664741 = PRYDnakABTFRvSa11889523;     PRYDnakABTFRvSa11889523 = PRYDnakABTFRvSa81911182;     PRYDnakABTFRvSa81911182 = PRYDnakABTFRvSa3109262;     PRYDnakABTFRvSa3109262 = PRYDnakABTFRvSa22654433;     PRYDnakABTFRvSa22654433 = PRYDnakABTFRvSa25436296;     PRYDnakABTFRvSa25436296 = PRYDnakABTFRvSa53668717;     PRYDnakABTFRvSa53668717 = PRYDnakABTFRvSa66812352;     PRYDnakABTFRvSa66812352 = PRYDnakABTFRvSa74954975;     PRYDnakABTFRvSa74954975 = PRYDnakABTFRvSa65890182;     PRYDnakABTFRvSa65890182 = PRYDnakABTFRvSa34417160;     PRYDnakABTFRvSa34417160 = PRYDnakABTFRvSa12030054;     PRYDnakABTFRvSa12030054 = PRYDnakABTFRvSa80574386;     PRYDnakABTFRvSa80574386 = PRYDnakABTFRvSa99027978;     PRYDnakABTFRvSa99027978 = PRYDnakABTFRvSa64787471;     PRYDnakABTFRvSa64787471 = PRYDnakABTFRvSa65459610;     PRYDnakABTFRvSa65459610 = PRYDnakABTFRvSa85795420;     PRYDnakABTFRvSa85795420 = PRYDnakABTFRvSa15375626;     PRYDnakABTFRvSa15375626 = PRYDnakABTFRvSa82208133;     PRYDnakABTFRvSa82208133 = PRYDnakABTFRvSa47688507;     PRYDnakABTFRvSa47688507 = PRYDnakABTFRvSa87493121;     PRYDnakABTFRvSa87493121 = PRYDnakABTFRvSa64902414;     PRYDnakABTFRvSa64902414 = PRYDnakABTFRvSa83715699;     PRYDnakABTFRvSa83715699 = PRYDnakABTFRvSa65031418;     PRYDnakABTFRvSa65031418 = PRYDnakABTFRvSa32320928;     PRYDnakABTFRvSa32320928 = PRYDnakABTFRvSa79154891;     PRYDnakABTFRvSa79154891 = PRYDnakABTFRvSa24083372;     PRYDnakABTFRvSa24083372 = PRYDnakABTFRvSa64881785;     PRYDnakABTFRvSa64881785 = PRYDnakABTFRvSa29324494;     PRYDnakABTFRvSa29324494 = PRYDnakABTFRvSa34300693;     PRYDnakABTFRvSa34300693 = PRYDnakABTFRvSa71172783;     PRYDnakABTFRvSa71172783 = PRYDnakABTFRvSa13279680;     PRYDnakABTFRvSa13279680 = PRYDnakABTFRvSa56773241;     PRYDnakABTFRvSa56773241 = PRYDnakABTFRvSa87294365;     PRYDnakABTFRvSa87294365 = PRYDnakABTFRvSa88896214;     PRYDnakABTFRvSa88896214 = PRYDnakABTFRvSa89850888;     PRYDnakABTFRvSa89850888 = PRYDnakABTFRvSa31074671;     PRYDnakABTFRvSa31074671 = PRYDnakABTFRvSa46620418;     PRYDnakABTFRvSa46620418 = PRYDnakABTFRvSa19066020;     PRYDnakABTFRvSa19066020 = PRYDnakABTFRvSa31115947;     PRYDnakABTFRvSa31115947 = PRYDnakABTFRvSa10982277;     PRYDnakABTFRvSa10982277 = PRYDnakABTFRvSa93612976;     PRYDnakABTFRvSa93612976 = PRYDnakABTFRvSa11608162;     PRYDnakABTFRvSa11608162 = PRYDnakABTFRvSa81627135;     PRYDnakABTFRvSa81627135 = PRYDnakABTFRvSa84408830;     PRYDnakABTFRvSa84408830 = PRYDnakABTFRvSa6093412;     PRYDnakABTFRvSa6093412 = PRYDnakABTFRvSa91204924;     PRYDnakABTFRvSa91204924 = PRYDnakABTFRvSa93628896;     PRYDnakABTFRvSa93628896 = PRYDnakABTFRvSa41870869;     PRYDnakABTFRvSa41870869 = PRYDnakABTFRvSa29052489;     PRYDnakABTFRvSa29052489 = PRYDnakABTFRvSa2034538;     PRYDnakABTFRvSa2034538 = PRYDnakABTFRvSa58952139;     PRYDnakABTFRvSa58952139 = PRYDnakABTFRvSa37742694;     PRYDnakABTFRvSa37742694 = PRYDnakABTFRvSa27159360;     PRYDnakABTFRvSa27159360 = PRYDnakABTFRvSa44867344;     PRYDnakABTFRvSa44867344 = PRYDnakABTFRvSa21762327;     PRYDnakABTFRvSa21762327 = PRYDnakABTFRvSa28173823;     PRYDnakABTFRvSa28173823 = PRYDnakABTFRvSa16879764;     PRYDnakABTFRvSa16879764 = PRYDnakABTFRvSa74588935;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void CvJKocvGkFduiWwx54645430() {     int IvrYquMJbdiaGyl45240602 = -839477464;    int IvrYquMJbdiaGyl35201400 = -393148340;    int IvrYquMJbdiaGyl58414382 = -327412297;    int IvrYquMJbdiaGyl75456386 = -213482134;    int IvrYquMJbdiaGyl78546399 = -371039898;    int IvrYquMJbdiaGyl28760140 = -254384045;    int IvrYquMJbdiaGyl30718397 = 43115675;    int IvrYquMJbdiaGyl4760831 = -14304805;    int IvrYquMJbdiaGyl97330604 = -485522390;    int IvrYquMJbdiaGyl54199478 = -275015127;    int IvrYquMJbdiaGyl26106569 = -923452523;    int IvrYquMJbdiaGyl95990964 = -491461382;    int IvrYquMJbdiaGyl30761589 = -531302726;    int IvrYquMJbdiaGyl587281 = -370103176;    int IvrYquMJbdiaGyl54260145 = -90344331;    int IvrYquMJbdiaGyl56365233 = -7969538;    int IvrYquMJbdiaGyl15983478 = -505416591;    int IvrYquMJbdiaGyl65794042 = 43515392;    int IvrYquMJbdiaGyl14938289 = -20951156;    int IvrYquMJbdiaGyl90407816 = -419392103;    int IvrYquMJbdiaGyl74790585 = -21830673;    int IvrYquMJbdiaGyl36956721 = 57217378;    int IvrYquMJbdiaGyl3684221 = -600393514;    int IvrYquMJbdiaGyl46835390 = -257401672;    int IvrYquMJbdiaGyl13854068 = -422784615;    int IvrYquMJbdiaGyl72318033 = 4707603;    int IvrYquMJbdiaGyl64813029 = 59749929;    int IvrYquMJbdiaGyl72433219 = -270696683;    int IvrYquMJbdiaGyl92567817 = -463451382;    int IvrYquMJbdiaGyl25735232 = -132642378;    int IvrYquMJbdiaGyl54589314 = -281736452;    int IvrYquMJbdiaGyl63337451 = -589824400;    int IvrYquMJbdiaGyl1068826 = -268513890;    int IvrYquMJbdiaGyl37144286 = -407385711;    int IvrYquMJbdiaGyl70681111 = -12793852;    int IvrYquMJbdiaGyl66651600 = -247995296;    int IvrYquMJbdiaGyl38303585 = -861199951;    int IvrYquMJbdiaGyl40622447 = -539964325;    int IvrYquMJbdiaGyl52861054 = -338352345;    int IvrYquMJbdiaGyl15054126 = -837115696;    int IvrYquMJbdiaGyl34921151 = -428011771;    int IvrYquMJbdiaGyl60843456 = -73404222;    int IvrYquMJbdiaGyl35631702 = -427551466;    int IvrYquMJbdiaGyl18397824 = 5198976;    int IvrYquMJbdiaGyl77865241 = -390177945;    int IvrYquMJbdiaGyl9515097 = -22060004;    int IvrYquMJbdiaGyl48785636 = -74726753;    int IvrYquMJbdiaGyl91050506 = -760675419;    int IvrYquMJbdiaGyl59110170 = -296696114;    int IvrYquMJbdiaGyl75239839 = -329853291;    int IvrYquMJbdiaGyl43144299 = -651587706;    int IvrYquMJbdiaGyl7761687 = -408360523;    int IvrYquMJbdiaGyl89492816 = -174649232;    int IvrYquMJbdiaGyl2257358 = -273943644;    int IvrYquMJbdiaGyl79854844 = -712111979;    int IvrYquMJbdiaGyl8283881 = -796694842;    int IvrYquMJbdiaGyl31517180 = -792754827;    int IvrYquMJbdiaGyl11578992 = 29989375;    int IvrYquMJbdiaGyl61602319 = -790697519;    int IvrYquMJbdiaGyl6228367 = -275747501;    int IvrYquMJbdiaGyl63947110 = -214133975;    int IvrYquMJbdiaGyl58285177 = -686187642;    int IvrYquMJbdiaGyl12193014 = -550853424;    int IvrYquMJbdiaGyl71595373 = -252880012;    int IvrYquMJbdiaGyl99610163 = -993278676;    int IvrYquMJbdiaGyl62769118 = -233628124;    int IvrYquMJbdiaGyl94922138 = -122947493;    int IvrYquMJbdiaGyl93617303 = -23917016;    int IvrYquMJbdiaGyl29906169 = -257309324;    int IvrYquMJbdiaGyl87608545 = -842349035;    int IvrYquMJbdiaGyl18061648 = -146769587;    int IvrYquMJbdiaGyl75361030 = -965452266;    int IvrYquMJbdiaGyl12932989 = -618132263;    int IvrYquMJbdiaGyl99884162 = -183835460;    int IvrYquMJbdiaGyl55486666 = -991380332;    int IvrYquMJbdiaGyl13947129 = -948426452;    int IvrYquMJbdiaGyl1325020 = -515231156;    int IvrYquMJbdiaGyl85286396 = -505592490;    int IvrYquMJbdiaGyl68970149 = -867223727;    int IvrYquMJbdiaGyl4338972 = -300724611;    int IvrYquMJbdiaGyl23532397 = -920565645;    int IvrYquMJbdiaGyl73762523 = -179574652;    int IvrYquMJbdiaGyl13323050 = -974000570;    int IvrYquMJbdiaGyl17327978 = -33598091;    int IvrYquMJbdiaGyl82590932 = -481054672;    int IvrYquMJbdiaGyl46827628 = -873375929;    int IvrYquMJbdiaGyl73844634 = -315175168;    int IvrYquMJbdiaGyl98811467 = -994570246;    int IvrYquMJbdiaGyl57289441 = -695273732;    int IvrYquMJbdiaGyl62397230 = -216099010;    int IvrYquMJbdiaGyl35134420 = -455240469;    int IvrYquMJbdiaGyl26724593 = -791189326;    int IvrYquMJbdiaGyl79020128 = -749266806;    int IvrYquMJbdiaGyl46632687 = 37395155;    int IvrYquMJbdiaGyl51107016 = -522981722;    int IvrYquMJbdiaGyl76635973 = -741824130;    int IvrYquMJbdiaGyl48650443 = -522550798;    int IvrYquMJbdiaGyl64036328 = -74671454;    int IvrYquMJbdiaGyl18787660 = -1522349;    int IvrYquMJbdiaGyl15096123 = -839477464;     IvrYquMJbdiaGyl45240602 = IvrYquMJbdiaGyl35201400;     IvrYquMJbdiaGyl35201400 = IvrYquMJbdiaGyl58414382;     IvrYquMJbdiaGyl58414382 = IvrYquMJbdiaGyl75456386;     IvrYquMJbdiaGyl75456386 = IvrYquMJbdiaGyl78546399;     IvrYquMJbdiaGyl78546399 = IvrYquMJbdiaGyl28760140;     IvrYquMJbdiaGyl28760140 = IvrYquMJbdiaGyl30718397;     IvrYquMJbdiaGyl30718397 = IvrYquMJbdiaGyl4760831;     IvrYquMJbdiaGyl4760831 = IvrYquMJbdiaGyl97330604;     IvrYquMJbdiaGyl97330604 = IvrYquMJbdiaGyl54199478;     IvrYquMJbdiaGyl54199478 = IvrYquMJbdiaGyl26106569;     IvrYquMJbdiaGyl26106569 = IvrYquMJbdiaGyl95990964;     IvrYquMJbdiaGyl95990964 = IvrYquMJbdiaGyl30761589;     IvrYquMJbdiaGyl30761589 = IvrYquMJbdiaGyl587281;     IvrYquMJbdiaGyl587281 = IvrYquMJbdiaGyl54260145;     IvrYquMJbdiaGyl54260145 = IvrYquMJbdiaGyl56365233;     IvrYquMJbdiaGyl56365233 = IvrYquMJbdiaGyl15983478;     IvrYquMJbdiaGyl15983478 = IvrYquMJbdiaGyl65794042;     IvrYquMJbdiaGyl65794042 = IvrYquMJbdiaGyl14938289;     IvrYquMJbdiaGyl14938289 = IvrYquMJbdiaGyl90407816;     IvrYquMJbdiaGyl90407816 = IvrYquMJbdiaGyl74790585;     IvrYquMJbdiaGyl74790585 = IvrYquMJbdiaGyl36956721;     IvrYquMJbdiaGyl36956721 = IvrYquMJbdiaGyl3684221;     IvrYquMJbdiaGyl3684221 = IvrYquMJbdiaGyl46835390;     IvrYquMJbdiaGyl46835390 = IvrYquMJbdiaGyl13854068;     IvrYquMJbdiaGyl13854068 = IvrYquMJbdiaGyl72318033;     IvrYquMJbdiaGyl72318033 = IvrYquMJbdiaGyl64813029;     IvrYquMJbdiaGyl64813029 = IvrYquMJbdiaGyl72433219;     IvrYquMJbdiaGyl72433219 = IvrYquMJbdiaGyl92567817;     IvrYquMJbdiaGyl92567817 = IvrYquMJbdiaGyl25735232;     IvrYquMJbdiaGyl25735232 = IvrYquMJbdiaGyl54589314;     IvrYquMJbdiaGyl54589314 = IvrYquMJbdiaGyl63337451;     IvrYquMJbdiaGyl63337451 = IvrYquMJbdiaGyl1068826;     IvrYquMJbdiaGyl1068826 = IvrYquMJbdiaGyl37144286;     IvrYquMJbdiaGyl37144286 = IvrYquMJbdiaGyl70681111;     IvrYquMJbdiaGyl70681111 = IvrYquMJbdiaGyl66651600;     IvrYquMJbdiaGyl66651600 = IvrYquMJbdiaGyl38303585;     IvrYquMJbdiaGyl38303585 = IvrYquMJbdiaGyl40622447;     IvrYquMJbdiaGyl40622447 = IvrYquMJbdiaGyl52861054;     IvrYquMJbdiaGyl52861054 = IvrYquMJbdiaGyl15054126;     IvrYquMJbdiaGyl15054126 = IvrYquMJbdiaGyl34921151;     IvrYquMJbdiaGyl34921151 = IvrYquMJbdiaGyl60843456;     IvrYquMJbdiaGyl60843456 = IvrYquMJbdiaGyl35631702;     IvrYquMJbdiaGyl35631702 = IvrYquMJbdiaGyl18397824;     IvrYquMJbdiaGyl18397824 = IvrYquMJbdiaGyl77865241;     IvrYquMJbdiaGyl77865241 = IvrYquMJbdiaGyl9515097;     IvrYquMJbdiaGyl9515097 = IvrYquMJbdiaGyl48785636;     IvrYquMJbdiaGyl48785636 = IvrYquMJbdiaGyl91050506;     IvrYquMJbdiaGyl91050506 = IvrYquMJbdiaGyl59110170;     IvrYquMJbdiaGyl59110170 = IvrYquMJbdiaGyl75239839;     IvrYquMJbdiaGyl75239839 = IvrYquMJbdiaGyl43144299;     IvrYquMJbdiaGyl43144299 = IvrYquMJbdiaGyl7761687;     IvrYquMJbdiaGyl7761687 = IvrYquMJbdiaGyl89492816;     IvrYquMJbdiaGyl89492816 = IvrYquMJbdiaGyl2257358;     IvrYquMJbdiaGyl2257358 = IvrYquMJbdiaGyl79854844;     IvrYquMJbdiaGyl79854844 = IvrYquMJbdiaGyl8283881;     IvrYquMJbdiaGyl8283881 = IvrYquMJbdiaGyl31517180;     IvrYquMJbdiaGyl31517180 = IvrYquMJbdiaGyl11578992;     IvrYquMJbdiaGyl11578992 = IvrYquMJbdiaGyl61602319;     IvrYquMJbdiaGyl61602319 = IvrYquMJbdiaGyl6228367;     IvrYquMJbdiaGyl6228367 = IvrYquMJbdiaGyl63947110;     IvrYquMJbdiaGyl63947110 = IvrYquMJbdiaGyl58285177;     IvrYquMJbdiaGyl58285177 = IvrYquMJbdiaGyl12193014;     IvrYquMJbdiaGyl12193014 = IvrYquMJbdiaGyl71595373;     IvrYquMJbdiaGyl71595373 = IvrYquMJbdiaGyl99610163;     IvrYquMJbdiaGyl99610163 = IvrYquMJbdiaGyl62769118;     IvrYquMJbdiaGyl62769118 = IvrYquMJbdiaGyl94922138;     IvrYquMJbdiaGyl94922138 = IvrYquMJbdiaGyl93617303;     IvrYquMJbdiaGyl93617303 = IvrYquMJbdiaGyl29906169;     IvrYquMJbdiaGyl29906169 = IvrYquMJbdiaGyl87608545;     IvrYquMJbdiaGyl87608545 = IvrYquMJbdiaGyl18061648;     IvrYquMJbdiaGyl18061648 = IvrYquMJbdiaGyl75361030;     IvrYquMJbdiaGyl75361030 = IvrYquMJbdiaGyl12932989;     IvrYquMJbdiaGyl12932989 = IvrYquMJbdiaGyl99884162;     IvrYquMJbdiaGyl99884162 = IvrYquMJbdiaGyl55486666;     IvrYquMJbdiaGyl55486666 = IvrYquMJbdiaGyl13947129;     IvrYquMJbdiaGyl13947129 = IvrYquMJbdiaGyl1325020;     IvrYquMJbdiaGyl1325020 = IvrYquMJbdiaGyl85286396;     IvrYquMJbdiaGyl85286396 = IvrYquMJbdiaGyl68970149;     IvrYquMJbdiaGyl68970149 = IvrYquMJbdiaGyl4338972;     IvrYquMJbdiaGyl4338972 = IvrYquMJbdiaGyl23532397;     IvrYquMJbdiaGyl23532397 = IvrYquMJbdiaGyl73762523;     IvrYquMJbdiaGyl73762523 = IvrYquMJbdiaGyl13323050;     IvrYquMJbdiaGyl13323050 = IvrYquMJbdiaGyl17327978;     IvrYquMJbdiaGyl17327978 = IvrYquMJbdiaGyl82590932;     IvrYquMJbdiaGyl82590932 = IvrYquMJbdiaGyl46827628;     IvrYquMJbdiaGyl46827628 = IvrYquMJbdiaGyl73844634;     IvrYquMJbdiaGyl73844634 = IvrYquMJbdiaGyl98811467;     IvrYquMJbdiaGyl98811467 = IvrYquMJbdiaGyl57289441;     IvrYquMJbdiaGyl57289441 = IvrYquMJbdiaGyl62397230;     IvrYquMJbdiaGyl62397230 = IvrYquMJbdiaGyl35134420;     IvrYquMJbdiaGyl35134420 = IvrYquMJbdiaGyl26724593;     IvrYquMJbdiaGyl26724593 = IvrYquMJbdiaGyl79020128;     IvrYquMJbdiaGyl79020128 = IvrYquMJbdiaGyl46632687;     IvrYquMJbdiaGyl46632687 = IvrYquMJbdiaGyl51107016;     IvrYquMJbdiaGyl51107016 = IvrYquMJbdiaGyl76635973;     IvrYquMJbdiaGyl76635973 = IvrYquMJbdiaGyl48650443;     IvrYquMJbdiaGyl48650443 = IvrYquMJbdiaGyl64036328;     IvrYquMJbdiaGyl64036328 = IvrYquMJbdiaGyl18787660;     IvrYquMJbdiaGyl18787660 = IvrYquMJbdiaGyl15096123;     IvrYquMJbdiaGyl15096123 = IvrYquMJbdiaGyl45240602;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void CPYwraizzGXCBLNd61304174() {     int VrbGvQSZaKWnkDA45234131 = -712001603;    int VrbGvQSZaKWnkDA73773717 = -435743459;    int VrbGvQSZaKWnkDA59727594 = -284540910;    int VrbGvQSZaKWnkDA60536247 = -34022967;    int VrbGvQSZaKWnkDA87386501 = -839395211;    int VrbGvQSZaKWnkDA63565572 = 46183143;    int VrbGvQSZaKWnkDA99265732 = -761420792;    int VrbGvQSZaKWnkDA73974881 = -818482107;    int VrbGvQSZaKWnkDA80070769 = -199168459;    int VrbGvQSZaKWnkDA50838992 = 88178002;    int VrbGvQSZaKWnkDA46427174 = -882924594;    int VrbGvQSZaKWnkDA10282642 = -226939736;    int VrbGvQSZaKWnkDA62614503 = -552961200;    int VrbGvQSZaKWnkDA54431501 = -715256227;    int VrbGvQSZaKWnkDA99432254 = -53057164;    int VrbGvQSZaKWnkDA30929780 = -340765188;    int VrbGvQSZaKWnkDA71836237 = -325031161;    int VrbGvQSZaKWnkDA88794575 = -353556226;    int VrbGvQSZaKWnkDA35366558 = -638933424;    int VrbGvQSZaKWnkDA62481579 = 68583446;    int VrbGvQSZaKWnkDA54460009 = -832297042;    int VrbGvQSZaKWnkDA15252987 = -452774505;    int VrbGvQSZaKWnkDA40562616 = 40648644;    int VrbGvQSZaKWnkDA90485498 = -138322194;    int VrbGvQSZaKWnkDA42296757 = -849478456;    int VrbGvQSZaKWnkDA88892109 = 96501670;    int VrbGvQSZaKWnkDA35034611 = -241049784;    int VrbGvQSZaKWnkDA7836744 = -879663665;    int VrbGvQSZaKWnkDA5246264 = -9521712;    int VrbGvQSZaKWnkDA1825102 = -908073752;    int VrbGvQSZaKWnkDA88001992 = -668829686;    int VrbGvQSZaKWnkDA48061308 = -981646164;    int VrbGvQSZaKWnkDA86521248 = -470458109;    int VrbGvQSZaKWnkDA37427123 = -284067443;    int VrbGvQSZaKWnkDA53039149 = -272107780;    int VrbGvQSZaKWnkDA9479789 = -327761061;    int VrbGvQSZaKWnkDA50244953 = -960853330;    int VrbGvQSZaKWnkDA10540127 = -535227001;    int VrbGvQSZaKWnkDA88174650 = 48966226;    int VrbGvQSZaKWnkDA86927019 = -446547383;    int VrbGvQSZaKWnkDA22776818 = -515955670;    int VrbGvQSZaKWnkDA46541117 = -267196324;    int VrbGvQSZaKWnkDA95915061 = -986681722;    int VrbGvQSZaKWnkDA77629721 = -651765468;    int VrbGvQSZaKWnkDA61883293 = -863437347;    int VrbGvQSZaKWnkDA49943065 = -934104694;    int VrbGvQSZaKWnkDA76483429 = -556007375;    int VrbGvQSZaKWnkDA66434087 = -269436776;    int VrbGvQSZaKWnkDA77768559 = -960459999;    int VrbGvQSZaKWnkDA95285262 = 83121827;    int VrbGvQSZaKWnkDA56403669 = -230142714;    int VrbGvQSZaKWnkDA22308944 = -790506276;    int VrbGvQSZaKWnkDA64941165 = -66703238;    int VrbGvQSZaKWnkDA12507629 = -805863495;    int VrbGvQSZaKWnkDA6844166 = -469011262;    int VrbGvQSZaKWnkDA29981144 = -159227099;    int VrbGvQSZaKWnkDA33211101 = -376392104;    int VrbGvQSZaKWnkDA69242096 = -46218717;    int VrbGvQSZaKWnkDA18239490 = -184544512;    int VrbGvQSZaKWnkDA98494392 = -835896882;    int VrbGvQSZaKWnkDA28530961 = -712767074;    int VrbGvQSZaKWnkDA91428988 = -881757127;    int VrbGvQSZaKWnkDA68728617 = -708960396;    int VrbGvQSZaKWnkDA78245667 = -291094707;    int VrbGvQSZaKWnkDA62836999 = -242992312;    int VrbGvQSZaKWnkDA98365866 = -901278431;    int VrbGvQSZaKWnkDA23761393 = -756481627;    int VrbGvQSZaKWnkDA25187380 = -168893758;    int VrbGvQSZaKWnkDA1392352 = -343148447;    int VrbGvQSZaKWnkDA89952465 = -725296104;    int VrbGvQSZaKWnkDA80684827 = -379911859;    int VrbGvQSZaKWnkDA61296110 = -789804160;    int VrbGvQSZaKWnkDA619925 = -302522452;    int VrbGvQSZaKWnkDA48439539 = -92386042;    int VrbGvQSZaKWnkDA39704761 = -415460885;    int VrbGvQSZaKWnkDA7918893 = -465100718;    int VrbGvQSZaKWnkDA19337925 = -466092783;    int VrbGvQSZaKWnkDA62932895 = -307585889;    int VrbGvQSZaKWnkDA28602205 = -274884847;    int VrbGvQSZaKWnkDA92353692 = -915373763;    int VrbGvQSZaKWnkDA12408680 = -347490955;    int VrbGvQSZaKWnkDA68600524 = -971613008;    int VrbGvQSZaKWnkDA30068185 = -919203667;    int VrbGvQSZaKWnkDA9961002 = 7356460;    int VrbGvQSZaKWnkDA45421433 = -577931038;    int VrbGvQSZaKWnkDA65693049 = -878323411;    int VrbGvQSZaKWnkDA83120142 = -814942927;    int VrbGvQSZaKWnkDA74013620 = -664594614;    int VrbGvQSZaKWnkDA30582958 = -815056182;    int VrbGvQSZaKWnkDA23058006 = -12880682;    int VrbGvQSZaKWnkDA76268688 = -951368957;    int VrbGvQSZaKWnkDA81002857 = -814634613;    int VrbGvQSZaKWnkDA92300636 = -250682490;    int VrbGvQSZaKWnkDA89680258 = -115136893;    int VrbGvQSZaKWnkDA58396058 = -733780309;    int VrbGvQSZaKWnkDA31347830 = -634198543;    int VrbGvQSZaKWnkDA77812500 = -558235929;    int VrbGvQSZaKWnkDA17669395 = -595587016;    int VrbGvQSZaKWnkDA14792723 = -308773156;    int VrbGvQSZaKWnkDA63517427 = -712001603;     VrbGvQSZaKWnkDA45234131 = VrbGvQSZaKWnkDA73773717;     VrbGvQSZaKWnkDA73773717 = VrbGvQSZaKWnkDA59727594;     VrbGvQSZaKWnkDA59727594 = VrbGvQSZaKWnkDA60536247;     VrbGvQSZaKWnkDA60536247 = VrbGvQSZaKWnkDA87386501;     VrbGvQSZaKWnkDA87386501 = VrbGvQSZaKWnkDA63565572;     VrbGvQSZaKWnkDA63565572 = VrbGvQSZaKWnkDA99265732;     VrbGvQSZaKWnkDA99265732 = VrbGvQSZaKWnkDA73974881;     VrbGvQSZaKWnkDA73974881 = VrbGvQSZaKWnkDA80070769;     VrbGvQSZaKWnkDA80070769 = VrbGvQSZaKWnkDA50838992;     VrbGvQSZaKWnkDA50838992 = VrbGvQSZaKWnkDA46427174;     VrbGvQSZaKWnkDA46427174 = VrbGvQSZaKWnkDA10282642;     VrbGvQSZaKWnkDA10282642 = VrbGvQSZaKWnkDA62614503;     VrbGvQSZaKWnkDA62614503 = VrbGvQSZaKWnkDA54431501;     VrbGvQSZaKWnkDA54431501 = VrbGvQSZaKWnkDA99432254;     VrbGvQSZaKWnkDA99432254 = VrbGvQSZaKWnkDA30929780;     VrbGvQSZaKWnkDA30929780 = VrbGvQSZaKWnkDA71836237;     VrbGvQSZaKWnkDA71836237 = VrbGvQSZaKWnkDA88794575;     VrbGvQSZaKWnkDA88794575 = VrbGvQSZaKWnkDA35366558;     VrbGvQSZaKWnkDA35366558 = VrbGvQSZaKWnkDA62481579;     VrbGvQSZaKWnkDA62481579 = VrbGvQSZaKWnkDA54460009;     VrbGvQSZaKWnkDA54460009 = VrbGvQSZaKWnkDA15252987;     VrbGvQSZaKWnkDA15252987 = VrbGvQSZaKWnkDA40562616;     VrbGvQSZaKWnkDA40562616 = VrbGvQSZaKWnkDA90485498;     VrbGvQSZaKWnkDA90485498 = VrbGvQSZaKWnkDA42296757;     VrbGvQSZaKWnkDA42296757 = VrbGvQSZaKWnkDA88892109;     VrbGvQSZaKWnkDA88892109 = VrbGvQSZaKWnkDA35034611;     VrbGvQSZaKWnkDA35034611 = VrbGvQSZaKWnkDA7836744;     VrbGvQSZaKWnkDA7836744 = VrbGvQSZaKWnkDA5246264;     VrbGvQSZaKWnkDA5246264 = VrbGvQSZaKWnkDA1825102;     VrbGvQSZaKWnkDA1825102 = VrbGvQSZaKWnkDA88001992;     VrbGvQSZaKWnkDA88001992 = VrbGvQSZaKWnkDA48061308;     VrbGvQSZaKWnkDA48061308 = VrbGvQSZaKWnkDA86521248;     VrbGvQSZaKWnkDA86521248 = VrbGvQSZaKWnkDA37427123;     VrbGvQSZaKWnkDA37427123 = VrbGvQSZaKWnkDA53039149;     VrbGvQSZaKWnkDA53039149 = VrbGvQSZaKWnkDA9479789;     VrbGvQSZaKWnkDA9479789 = VrbGvQSZaKWnkDA50244953;     VrbGvQSZaKWnkDA50244953 = VrbGvQSZaKWnkDA10540127;     VrbGvQSZaKWnkDA10540127 = VrbGvQSZaKWnkDA88174650;     VrbGvQSZaKWnkDA88174650 = VrbGvQSZaKWnkDA86927019;     VrbGvQSZaKWnkDA86927019 = VrbGvQSZaKWnkDA22776818;     VrbGvQSZaKWnkDA22776818 = VrbGvQSZaKWnkDA46541117;     VrbGvQSZaKWnkDA46541117 = VrbGvQSZaKWnkDA95915061;     VrbGvQSZaKWnkDA95915061 = VrbGvQSZaKWnkDA77629721;     VrbGvQSZaKWnkDA77629721 = VrbGvQSZaKWnkDA61883293;     VrbGvQSZaKWnkDA61883293 = VrbGvQSZaKWnkDA49943065;     VrbGvQSZaKWnkDA49943065 = VrbGvQSZaKWnkDA76483429;     VrbGvQSZaKWnkDA76483429 = VrbGvQSZaKWnkDA66434087;     VrbGvQSZaKWnkDA66434087 = VrbGvQSZaKWnkDA77768559;     VrbGvQSZaKWnkDA77768559 = VrbGvQSZaKWnkDA95285262;     VrbGvQSZaKWnkDA95285262 = VrbGvQSZaKWnkDA56403669;     VrbGvQSZaKWnkDA56403669 = VrbGvQSZaKWnkDA22308944;     VrbGvQSZaKWnkDA22308944 = VrbGvQSZaKWnkDA64941165;     VrbGvQSZaKWnkDA64941165 = VrbGvQSZaKWnkDA12507629;     VrbGvQSZaKWnkDA12507629 = VrbGvQSZaKWnkDA6844166;     VrbGvQSZaKWnkDA6844166 = VrbGvQSZaKWnkDA29981144;     VrbGvQSZaKWnkDA29981144 = VrbGvQSZaKWnkDA33211101;     VrbGvQSZaKWnkDA33211101 = VrbGvQSZaKWnkDA69242096;     VrbGvQSZaKWnkDA69242096 = VrbGvQSZaKWnkDA18239490;     VrbGvQSZaKWnkDA18239490 = VrbGvQSZaKWnkDA98494392;     VrbGvQSZaKWnkDA98494392 = VrbGvQSZaKWnkDA28530961;     VrbGvQSZaKWnkDA28530961 = VrbGvQSZaKWnkDA91428988;     VrbGvQSZaKWnkDA91428988 = VrbGvQSZaKWnkDA68728617;     VrbGvQSZaKWnkDA68728617 = VrbGvQSZaKWnkDA78245667;     VrbGvQSZaKWnkDA78245667 = VrbGvQSZaKWnkDA62836999;     VrbGvQSZaKWnkDA62836999 = VrbGvQSZaKWnkDA98365866;     VrbGvQSZaKWnkDA98365866 = VrbGvQSZaKWnkDA23761393;     VrbGvQSZaKWnkDA23761393 = VrbGvQSZaKWnkDA25187380;     VrbGvQSZaKWnkDA25187380 = VrbGvQSZaKWnkDA1392352;     VrbGvQSZaKWnkDA1392352 = VrbGvQSZaKWnkDA89952465;     VrbGvQSZaKWnkDA89952465 = VrbGvQSZaKWnkDA80684827;     VrbGvQSZaKWnkDA80684827 = VrbGvQSZaKWnkDA61296110;     VrbGvQSZaKWnkDA61296110 = VrbGvQSZaKWnkDA619925;     VrbGvQSZaKWnkDA619925 = VrbGvQSZaKWnkDA48439539;     VrbGvQSZaKWnkDA48439539 = VrbGvQSZaKWnkDA39704761;     VrbGvQSZaKWnkDA39704761 = VrbGvQSZaKWnkDA7918893;     VrbGvQSZaKWnkDA7918893 = VrbGvQSZaKWnkDA19337925;     VrbGvQSZaKWnkDA19337925 = VrbGvQSZaKWnkDA62932895;     VrbGvQSZaKWnkDA62932895 = VrbGvQSZaKWnkDA28602205;     VrbGvQSZaKWnkDA28602205 = VrbGvQSZaKWnkDA92353692;     VrbGvQSZaKWnkDA92353692 = VrbGvQSZaKWnkDA12408680;     VrbGvQSZaKWnkDA12408680 = VrbGvQSZaKWnkDA68600524;     VrbGvQSZaKWnkDA68600524 = VrbGvQSZaKWnkDA30068185;     VrbGvQSZaKWnkDA30068185 = VrbGvQSZaKWnkDA9961002;     VrbGvQSZaKWnkDA9961002 = VrbGvQSZaKWnkDA45421433;     VrbGvQSZaKWnkDA45421433 = VrbGvQSZaKWnkDA65693049;     VrbGvQSZaKWnkDA65693049 = VrbGvQSZaKWnkDA83120142;     VrbGvQSZaKWnkDA83120142 = VrbGvQSZaKWnkDA74013620;     VrbGvQSZaKWnkDA74013620 = VrbGvQSZaKWnkDA30582958;     VrbGvQSZaKWnkDA30582958 = VrbGvQSZaKWnkDA23058006;     VrbGvQSZaKWnkDA23058006 = VrbGvQSZaKWnkDA76268688;     VrbGvQSZaKWnkDA76268688 = VrbGvQSZaKWnkDA81002857;     VrbGvQSZaKWnkDA81002857 = VrbGvQSZaKWnkDA92300636;     VrbGvQSZaKWnkDA92300636 = VrbGvQSZaKWnkDA89680258;     VrbGvQSZaKWnkDA89680258 = VrbGvQSZaKWnkDA58396058;     VrbGvQSZaKWnkDA58396058 = VrbGvQSZaKWnkDA31347830;     VrbGvQSZaKWnkDA31347830 = VrbGvQSZaKWnkDA77812500;     VrbGvQSZaKWnkDA77812500 = VrbGvQSZaKWnkDA17669395;     VrbGvQSZaKWnkDA17669395 = VrbGvQSZaKWnkDA14792723;     VrbGvQSZaKWnkDA14792723 = VrbGvQSZaKWnkDA63517427;     VrbGvQSZaKWnkDA63517427 = VrbGvQSZaKWnkDA45234131;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void EcrtHEVSWpwTWfLS15720388() {     int chncllyAtfBPmJG15885798 = -230150736;    int chncllyAtfBPmJG42447234 = -307386765;    int chncllyAtfBPmJG39696513 = -726835370;    int chncllyAtfBPmJG467535 = -224161353;    int chncllyAtfBPmJG8836824 = -622801566;    int chncllyAtfBPmJG53565334 = -755398008;    int chncllyAtfBPmJG17644628 = -760196501;    int chncllyAtfBPmJG30817262 = -65299212;    int chncllyAtfBPmJG25935824 = -292515788;    int chncllyAtfBPmJG73805452 = -212917349;    int chncllyAtfBPmJG88676335 = -917826525;    int chncllyAtfBPmJG55829212 = -542703538;    int chncllyAtfBPmJG42441890 = -462498259;    int chncllyAtfBPmJG38278534 = -516284438;    int chncllyAtfBPmJG82152273 = -202807186;    int chncllyAtfBPmJG63458420 = 89251225;    int chncllyAtfBPmJG65689063 = -234216291;    int chncllyAtfBPmJG9088070 = -342722426;    int chncllyAtfBPmJG17074341 = -246121365;    int chncllyAtfBPmJG21268289 = -473782859;    int chncllyAtfBPmJG9595764 = -740976188;    int chncllyAtfBPmJG76648751 = -784059811;    int chncllyAtfBPmJG42506425 = -527194501;    int chncllyAtfBPmJG24335035 = -830723100;    int chncllyAtfBPmJG6421146 = -525383156;    int chncllyAtfBPmJG19489691 = -1782771;    int chncllyAtfBPmJG43295396 = -636227559;    int chncllyAtfBPmJG15618970 = -284787439;    int chncllyAtfBPmJG37388752 = -303879913;    int chncllyAtfBPmJG40997198 = -366468822;    int chncllyAtfBPmJG95073989 = -417263372;    int chncllyAtfBPmJG92572769 = -209943324;    int chncllyAtfBPmJG69466609 = -417419714;    int chncllyAtfBPmJG2792098 = -947597127;    int chncllyAtfBPmJG31063386 = -119054557;    int chncllyAtfBPmJG69473048 = -406987590;    int chncllyAtfBPmJG94036438 = -49446537;    int chncllyAtfBPmJG63332615 = -319340112;    int chncllyAtfBPmJG66707939 = 93237602;    int chncllyAtfBPmJG82030317 = 35844255;    int chncllyAtfBPmJG82850102 = -899974548;    int chncllyAtfBPmJG75024108 = -36585373;    int chncllyAtfBPmJG44882022 = -973435772;    int chncllyAtfBPmJG84138022 = -723740439;    int chncllyAtfBPmJG57837352 = -72083741;    int chncllyAtfBPmJG56348899 = -692441830;    int chncllyAtfBPmJG2614632 = -672077174;    int chncllyAtfBPmJG32048297 = -721567070;    int chncllyAtfBPmJG83210011 = -946223944;    int chncllyAtfBPmJG3712749 = -199887056;    int chncllyAtfBPmJG24592994 = -876714835;    int chncllyAtfBPmJG64180448 = -759725993;    int chncllyAtfBPmJG20016823 = 5498717;    int chncllyAtfBPmJG2734934 = -366744992;    int chncllyAtfBPmJG6124624 = -579952171;    int chncllyAtfBPmJG39237046 = -446090925;    int chncllyAtfBPmJG99940808 = -780192265;    int chncllyAtfBPmJG15361478 = -896112270;    int chncllyAtfBPmJG94046388 = -698778198;    int chncllyAtfBPmJG89347132 = -521018796;    int chncllyAtfBPmJG10269938 = -19170450;    int chncllyAtfBPmJG2025659 = -375409063;    int chncllyAtfBPmJG93428510 = -761419299;    int chncllyAtfBPmJG84938625 = -926046966;    int chncllyAtfBPmJG78731463 = -795653977;    int chncllyAtfBPmJG96103566 = -607883201;    int chncllyAtfBPmJG86362602 = -25283824;    int chncllyAtfBPmJG39649792 = -514901132;    int chncllyAtfBPmJG7215149 = -297229882;    int chncllyAtfBPmJG12679226 = -795819596;    int chncllyAtfBPmJG69421981 = -861302239;    int chncllyAtfBPmJG2356449 = -914876179;    int chncllyAtfBPmJG42380130 = -335960028;    int chncllyAtfBPmJG35044023 = -181965620;    int chncllyAtfBPmJG38418187 = -573808312;    int chncllyAtfBPmJG34571656 = -604390815;    int chncllyAtfBPmJG31766730 = -810624039;    int chncllyAtfBPmJG58368403 = -803454062;    int chncllyAtfBPmJG66497683 = -658639360;    int chncllyAtfBPmJG50072246 = -832941326;    int chncllyAtfBPmJG16875059 = -329705597;    int chncllyAtfBPmJG11247100 = -914660489;    int chncllyAtfBPmJG32408958 = -338563496;    int chncllyAtfBPmJG33676003 = -3992858;    int chncllyAtfBPmJG16404205 = -489753988;    int chncllyAtfBPmJG30893541 = -657537379;    int chncllyAtfBPmJG72555947 = -115442042;    int chncllyAtfBPmJG66731676 = 49325277;    int chncllyAtfBPmJG96667473 = -267644957;    int chncllyAtfBPmJG91826339 = -672963632;    int chncllyAtfBPmJG69532239 = -626795326;    int chncllyAtfBPmJG78674960 = -153334267;    int chncllyAtfBPmJG69286226 = -620561915;    int chncllyAtfBPmJG77360806 = -385743602;    int chncllyAtfBPmJG71760379 = -944985296;    int chncllyAtfBPmJG80824443 = -424565486;    int chncllyAtfBPmJG81595598 = -275166074;    int chncllyAtfBPmJG59943396 = 52611194;    int chncllyAtfBPmJG5406560 = -928086462;    int chncllyAtfBPmJG61733786 = -230150736;     chncllyAtfBPmJG15885798 = chncllyAtfBPmJG42447234;     chncllyAtfBPmJG42447234 = chncllyAtfBPmJG39696513;     chncllyAtfBPmJG39696513 = chncllyAtfBPmJG467535;     chncllyAtfBPmJG467535 = chncllyAtfBPmJG8836824;     chncllyAtfBPmJG8836824 = chncllyAtfBPmJG53565334;     chncllyAtfBPmJG53565334 = chncllyAtfBPmJG17644628;     chncllyAtfBPmJG17644628 = chncllyAtfBPmJG30817262;     chncllyAtfBPmJG30817262 = chncllyAtfBPmJG25935824;     chncllyAtfBPmJG25935824 = chncllyAtfBPmJG73805452;     chncllyAtfBPmJG73805452 = chncllyAtfBPmJG88676335;     chncllyAtfBPmJG88676335 = chncllyAtfBPmJG55829212;     chncllyAtfBPmJG55829212 = chncllyAtfBPmJG42441890;     chncllyAtfBPmJG42441890 = chncllyAtfBPmJG38278534;     chncllyAtfBPmJG38278534 = chncllyAtfBPmJG82152273;     chncllyAtfBPmJG82152273 = chncllyAtfBPmJG63458420;     chncllyAtfBPmJG63458420 = chncllyAtfBPmJG65689063;     chncllyAtfBPmJG65689063 = chncllyAtfBPmJG9088070;     chncllyAtfBPmJG9088070 = chncllyAtfBPmJG17074341;     chncllyAtfBPmJG17074341 = chncllyAtfBPmJG21268289;     chncllyAtfBPmJG21268289 = chncllyAtfBPmJG9595764;     chncllyAtfBPmJG9595764 = chncllyAtfBPmJG76648751;     chncllyAtfBPmJG76648751 = chncllyAtfBPmJG42506425;     chncllyAtfBPmJG42506425 = chncllyAtfBPmJG24335035;     chncllyAtfBPmJG24335035 = chncllyAtfBPmJG6421146;     chncllyAtfBPmJG6421146 = chncllyAtfBPmJG19489691;     chncllyAtfBPmJG19489691 = chncllyAtfBPmJG43295396;     chncllyAtfBPmJG43295396 = chncllyAtfBPmJG15618970;     chncllyAtfBPmJG15618970 = chncllyAtfBPmJG37388752;     chncllyAtfBPmJG37388752 = chncllyAtfBPmJG40997198;     chncllyAtfBPmJG40997198 = chncllyAtfBPmJG95073989;     chncllyAtfBPmJG95073989 = chncllyAtfBPmJG92572769;     chncllyAtfBPmJG92572769 = chncllyAtfBPmJG69466609;     chncllyAtfBPmJG69466609 = chncllyAtfBPmJG2792098;     chncllyAtfBPmJG2792098 = chncllyAtfBPmJG31063386;     chncllyAtfBPmJG31063386 = chncllyAtfBPmJG69473048;     chncllyAtfBPmJG69473048 = chncllyAtfBPmJG94036438;     chncllyAtfBPmJG94036438 = chncllyAtfBPmJG63332615;     chncllyAtfBPmJG63332615 = chncllyAtfBPmJG66707939;     chncllyAtfBPmJG66707939 = chncllyAtfBPmJG82030317;     chncllyAtfBPmJG82030317 = chncllyAtfBPmJG82850102;     chncllyAtfBPmJG82850102 = chncllyAtfBPmJG75024108;     chncllyAtfBPmJG75024108 = chncllyAtfBPmJG44882022;     chncllyAtfBPmJG44882022 = chncllyAtfBPmJG84138022;     chncllyAtfBPmJG84138022 = chncllyAtfBPmJG57837352;     chncllyAtfBPmJG57837352 = chncllyAtfBPmJG56348899;     chncllyAtfBPmJG56348899 = chncllyAtfBPmJG2614632;     chncllyAtfBPmJG2614632 = chncllyAtfBPmJG32048297;     chncllyAtfBPmJG32048297 = chncllyAtfBPmJG83210011;     chncllyAtfBPmJG83210011 = chncllyAtfBPmJG3712749;     chncllyAtfBPmJG3712749 = chncllyAtfBPmJG24592994;     chncllyAtfBPmJG24592994 = chncllyAtfBPmJG64180448;     chncllyAtfBPmJG64180448 = chncllyAtfBPmJG20016823;     chncllyAtfBPmJG20016823 = chncllyAtfBPmJG2734934;     chncllyAtfBPmJG2734934 = chncllyAtfBPmJG6124624;     chncllyAtfBPmJG6124624 = chncllyAtfBPmJG39237046;     chncllyAtfBPmJG39237046 = chncllyAtfBPmJG99940808;     chncllyAtfBPmJG99940808 = chncllyAtfBPmJG15361478;     chncllyAtfBPmJG15361478 = chncllyAtfBPmJG94046388;     chncllyAtfBPmJG94046388 = chncllyAtfBPmJG89347132;     chncllyAtfBPmJG89347132 = chncllyAtfBPmJG10269938;     chncllyAtfBPmJG10269938 = chncllyAtfBPmJG2025659;     chncllyAtfBPmJG2025659 = chncllyAtfBPmJG93428510;     chncllyAtfBPmJG93428510 = chncllyAtfBPmJG84938625;     chncllyAtfBPmJG84938625 = chncllyAtfBPmJG78731463;     chncllyAtfBPmJG78731463 = chncllyAtfBPmJG96103566;     chncllyAtfBPmJG96103566 = chncllyAtfBPmJG86362602;     chncllyAtfBPmJG86362602 = chncllyAtfBPmJG39649792;     chncllyAtfBPmJG39649792 = chncllyAtfBPmJG7215149;     chncllyAtfBPmJG7215149 = chncllyAtfBPmJG12679226;     chncllyAtfBPmJG12679226 = chncllyAtfBPmJG69421981;     chncllyAtfBPmJG69421981 = chncllyAtfBPmJG2356449;     chncllyAtfBPmJG2356449 = chncllyAtfBPmJG42380130;     chncllyAtfBPmJG42380130 = chncllyAtfBPmJG35044023;     chncllyAtfBPmJG35044023 = chncllyAtfBPmJG38418187;     chncllyAtfBPmJG38418187 = chncllyAtfBPmJG34571656;     chncllyAtfBPmJG34571656 = chncllyAtfBPmJG31766730;     chncllyAtfBPmJG31766730 = chncllyAtfBPmJG58368403;     chncllyAtfBPmJG58368403 = chncllyAtfBPmJG66497683;     chncllyAtfBPmJG66497683 = chncllyAtfBPmJG50072246;     chncllyAtfBPmJG50072246 = chncllyAtfBPmJG16875059;     chncllyAtfBPmJG16875059 = chncllyAtfBPmJG11247100;     chncllyAtfBPmJG11247100 = chncllyAtfBPmJG32408958;     chncllyAtfBPmJG32408958 = chncllyAtfBPmJG33676003;     chncllyAtfBPmJG33676003 = chncllyAtfBPmJG16404205;     chncllyAtfBPmJG16404205 = chncllyAtfBPmJG30893541;     chncllyAtfBPmJG30893541 = chncllyAtfBPmJG72555947;     chncllyAtfBPmJG72555947 = chncllyAtfBPmJG66731676;     chncllyAtfBPmJG66731676 = chncllyAtfBPmJG96667473;     chncllyAtfBPmJG96667473 = chncllyAtfBPmJG91826339;     chncllyAtfBPmJG91826339 = chncllyAtfBPmJG69532239;     chncllyAtfBPmJG69532239 = chncllyAtfBPmJG78674960;     chncllyAtfBPmJG78674960 = chncllyAtfBPmJG69286226;     chncllyAtfBPmJG69286226 = chncllyAtfBPmJG77360806;     chncllyAtfBPmJG77360806 = chncllyAtfBPmJG71760379;     chncllyAtfBPmJG71760379 = chncllyAtfBPmJG80824443;     chncllyAtfBPmJG80824443 = chncllyAtfBPmJG81595598;     chncllyAtfBPmJG81595598 = chncllyAtfBPmJG59943396;     chncllyAtfBPmJG59943396 = chncllyAtfBPmJG5406560;     chncllyAtfBPmJG5406560 = chncllyAtfBPmJG61733786;     chncllyAtfBPmJG61733786 = chncllyAtfBPmJG15885798;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void hEOKgVDFbeVYKlLq22379133() {     int GrHEGKjWouPelqO15879327 = -102674875;    int GrHEGKjWouPelqO81019551 = -349981884;    int GrHEGKjWouPelqO41009725 = -683963984;    int GrHEGKjWouPelqO85547394 = -44702186;    int GrHEGKjWouPelqO17676926 = 8843120;    int GrHEGKjWouPelqO88370766 = -454830820;    int GrHEGKjWouPelqO86191963 = -464732968;    int GrHEGKjWouPelqO31313 = -869476514;    int GrHEGKjWouPelqO8675988 = -6161857;    int GrHEGKjWouPelqO70444966 = -949724220;    int GrHEGKjWouPelqO8996941 = -877298596;    int GrHEGKjWouPelqO70120889 = -278181891;    int GrHEGKjWouPelqO74294804 = -484156733;    int GrHEGKjWouPelqO92122754 = -861437488;    int GrHEGKjWouPelqO27324383 = -165520019;    int GrHEGKjWouPelqO38022967 = -243544426;    int GrHEGKjWouPelqO21541823 = -53830861;    int GrHEGKjWouPelqO32088603 = -739794044;    int GrHEGKjWouPelqO37502610 = -864103632;    int GrHEGKjWouPelqO93342051 = 14192690;    int GrHEGKjWouPelqO89265188 = -451442556;    int GrHEGKjWouPelqO54945017 = -194051694;    int GrHEGKjWouPelqO79384821 = -986152343;    int GrHEGKjWouPelqO67985143 = -711643623;    int GrHEGKjWouPelqO34863836 = -952076997;    int GrHEGKjWouPelqO36063767 = 90011297;    int GrHEGKjWouPelqO13516978 = -937027272;    int GrHEGKjWouPelqO51022494 = -893754421;    int GrHEGKjWouPelqO50067199 = -949950244;    int GrHEGKjWouPelqO17087068 = -41900196;    int GrHEGKjWouPelqO28486668 = -804356606;    int GrHEGKjWouPelqO77296626 = -601765089;    int GrHEGKjWouPelqO54919033 = -619363933;    int GrHEGKjWouPelqO3074935 = -824278860;    int GrHEGKjWouPelqO13421424 = -378368484;    int GrHEGKjWouPelqO12301238 = -486753355;    int GrHEGKjWouPelqO5977807 = -149099915;    int GrHEGKjWouPelqO33250295 = -314602788;    int GrHEGKjWouPelqO2021536 = -619443827;    int GrHEGKjWouPelqO53903210 = -673587431;    int GrHEGKjWouPelqO70705769 = -987918446;    int GrHEGKjWouPelqO60721769 = -230377476;    int GrHEGKjWouPelqO5165382 = -432566029;    int GrHEGKjWouPelqO43369920 = -280704883;    int GrHEGKjWouPelqO41855404 = -545343144;    int GrHEGKjWouPelqO96776867 = -504486519;    int GrHEGKjWouPelqO30312425 = -53357797;    int GrHEGKjWouPelqO7431878 = -230328427;    int GrHEGKjWouPelqO1868401 = -509987830;    int GrHEGKjWouPelqO23758172 = -886911937;    int GrHEGKjWouPelqO37852363 = -455269842;    int GrHEGKjWouPelqO78727705 = -41871746;    int GrHEGKjWouPelqO95465171 = -986555288;    int GrHEGKjWouPelqO12985205 = -898664843;    int GrHEGKjWouPelqO33113944 = -336851454;    int GrHEGKjWouPelqO60934309 = -908623182;    int GrHEGKjWouPelqO1634730 = -363829542;    int GrHEGKjWouPelqO73024582 = -972320361;    int GrHEGKjWouPelqO50683559 = -92625190;    int GrHEGKjWouPelqO81613158 = 18831823;    int GrHEGKjWouPelqO74853789 = -517803549;    int GrHEGKjWouPelqO35169470 = -570978548;    int GrHEGKjWouPelqO49964114 = -919526271;    int GrHEGKjWouPelqO91588919 = -964261662;    int GrHEGKjWouPelqO41958298 = -45367614;    int GrHEGKjWouPelqO31700314 = -175533508;    int GrHEGKjWouPelqO15201857 = -658817958;    int GrHEGKjWouPelqO71219869 = -659877874;    int GrHEGKjWouPelqO78701331 = -383069004;    int GrHEGKjWouPelqO15023145 = -678766665;    int GrHEGKjWouPelqO32045161 = 5555489;    int GrHEGKjWouPelqO88291528 = -739228073;    int GrHEGKjWouPelqO30067067 = -20350217;    int GrHEGKjWouPelqO83599399 = -90516201;    int GrHEGKjWouPelqO22636282 = 2111136;    int GrHEGKjWouPelqO28543419 = -121065080;    int GrHEGKjWouPelqO49779636 = -761485666;    int GrHEGKjWouPelqO36014901 = -605447460;    int GrHEGKjWouPelqO26129739 = -66300480;    int GrHEGKjWouPelqO38086968 = -347590478;    int GrHEGKjWouPelqO5751342 = -856630907;    int GrHEGKjWouPelqO6085101 = -606698845;    int GrHEGKjWouPelqO49154093 = -283766592;    int GrHEGKjWouPelqO26309027 = 36961693;    int GrHEGKjWouPelqO79234704 = -586630354;    int GrHEGKjWouPelqO49758962 = -662484861;    int GrHEGKjWouPelqO81831455 = -615209801;    int GrHEGKjWouPelqO41933829 = -720699091;    int GrHEGKjWouPelqO69960991 = -387427407;    int GrHEGKjWouPelqO52487115 = -469745303;    int GrHEGKjWouPelqO10666509 = -22923813;    int GrHEGKjWouPelqO32953225 = -176779554;    int GrHEGKjWouPelqO82566735 = -121977598;    int GrHEGKjWouPelqO20408378 = -538275650;    int GrHEGKjWouPelqO79049421 = -55783883;    int GrHEGKjWouPelqO35536300 = -316939899;    int GrHEGKjWouPelqO10757656 = -310851206;    int GrHEGKjWouPelqO13576462 = -468304368;    int GrHEGKjWouPelqO1411623 = -135337269;    int GrHEGKjWouPelqO10155090 = -102674875;     GrHEGKjWouPelqO15879327 = GrHEGKjWouPelqO81019551;     GrHEGKjWouPelqO81019551 = GrHEGKjWouPelqO41009725;     GrHEGKjWouPelqO41009725 = GrHEGKjWouPelqO85547394;     GrHEGKjWouPelqO85547394 = GrHEGKjWouPelqO17676926;     GrHEGKjWouPelqO17676926 = GrHEGKjWouPelqO88370766;     GrHEGKjWouPelqO88370766 = GrHEGKjWouPelqO86191963;     GrHEGKjWouPelqO86191963 = GrHEGKjWouPelqO31313;     GrHEGKjWouPelqO31313 = GrHEGKjWouPelqO8675988;     GrHEGKjWouPelqO8675988 = GrHEGKjWouPelqO70444966;     GrHEGKjWouPelqO70444966 = GrHEGKjWouPelqO8996941;     GrHEGKjWouPelqO8996941 = GrHEGKjWouPelqO70120889;     GrHEGKjWouPelqO70120889 = GrHEGKjWouPelqO74294804;     GrHEGKjWouPelqO74294804 = GrHEGKjWouPelqO92122754;     GrHEGKjWouPelqO92122754 = GrHEGKjWouPelqO27324383;     GrHEGKjWouPelqO27324383 = GrHEGKjWouPelqO38022967;     GrHEGKjWouPelqO38022967 = GrHEGKjWouPelqO21541823;     GrHEGKjWouPelqO21541823 = GrHEGKjWouPelqO32088603;     GrHEGKjWouPelqO32088603 = GrHEGKjWouPelqO37502610;     GrHEGKjWouPelqO37502610 = GrHEGKjWouPelqO93342051;     GrHEGKjWouPelqO93342051 = GrHEGKjWouPelqO89265188;     GrHEGKjWouPelqO89265188 = GrHEGKjWouPelqO54945017;     GrHEGKjWouPelqO54945017 = GrHEGKjWouPelqO79384821;     GrHEGKjWouPelqO79384821 = GrHEGKjWouPelqO67985143;     GrHEGKjWouPelqO67985143 = GrHEGKjWouPelqO34863836;     GrHEGKjWouPelqO34863836 = GrHEGKjWouPelqO36063767;     GrHEGKjWouPelqO36063767 = GrHEGKjWouPelqO13516978;     GrHEGKjWouPelqO13516978 = GrHEGKjWouPelqO51022494;     GrHEGKjWouPelqO51022494 = GrHEGKjWouPelqO50067199;     GrHEGKjWouPelqO50067199 = GrHEGKjWouPelqO17087068;     GrHEGKjWouPelqO17087068 = GrHEGKjWouPelqO28486668;     GrHEGKjWouPelqO28486668 = GrHEGKjWouPelqO77296626;     GrHEGKjWouPelqO77296626 = GrHEGKjWouPelqO54919033;     GrHEGKjWouPelqO54919033 = GrHEGKjWouPelqO3074935;     GrHEGKjWouPelqO3074935 = GrHEGKjWouPelqO13421424;     GrHEGKjWouPelqO13421424 = GrHEGKjWouPelqO12301238;     GrHEGKjWouPelqO12301238 = GrHEGKjWouPelqO5977807;     GrHEGKjWouPelqO5977807 = GrHEGKjWouPelqO33250295;     GrHEGKjWouPelqO33250295 = GrHEGKjWouPelqO2021536;     GrHEGKjWouPelqO2021536 = GrHEGKjWouPelqO53903210;     GrHEGKjWouPelqO53903210 = GrHEGKjWouPelqO70705769;     GrHEGKjWouPelqO70705769 = GrHEGKjWouPelqO60721769;     GrHEGKjWouPelqO60721769 = GrHEGKjWouPelqO5165382;     GrHEGKjWouPelqO5165382 = GrHEGKjWouPelqO43369920;     GrHEGKjWouPelqO43369920 = GrHEGKjWouPelqO41855404;     GrHEGKjWouPelqO41855404 = GrHEGKjWouPelqO96776867;     GrHEGKjWouPelqO96776867 = GrHEGKjWouPelqO30312425;     GrHEGKjWouPelqO30312425 = GrHEGKjWouPelqO7431878;     GrHEGKjWouPelqO7431878 = GrHEGKjWouPelqO1868401;     GrHEGKjWouPelqO1868401 = GrHEGKjWouPelqO23758172;     GrHEGKjWouPelqO23758172 = GrHEGKjWouPelqO37852363;     GrHEGKjWouPelqO37852363 = GrHEGKjWouPelqO78727705;     GrHEGKjWouPelqO78727705 = GrHEGKjWouPelqO95465171;     GrHEGKjWouPelqO95465171 = GrHEGKjWouPelqO12985205;     GrHEGKjWouPelqO12985205 = GrHEGKjWouPelqO33113944;     GrHEGKjWouPelqO33113944 = GrHEGKjWouPelqO60934309;     GrHEGKjWouPelqO60934309 = GrHEGKjWouPelqO1634730;     GrHEGKjWouPelqO1634730 = GrHEGKjWouPelqO73024582;     GrHEGKjWouPelqO73024582 = GrHEGKjWouPelqO50683559;     GrHEGKjWouPelqO50683559 = GrHEGKjWouPelqO81613158;     GrHEGKjWouPelqO81613158 = GrHEGKjWouPelqO74853789;     GrHEGKjWouPelqO74853789 = GrHEGKjWouPelqO35169470;     GrHEGKjWouPelqO35169470 = GrHEGKjWouPelqO49964114;     GrHEGKjWouPelqO49964114 = GrHEGKjWouPelqO91588919;     GrHEGKjWouPelqO91588919 = GrHEGKjWouPelqO41958298;     GrHEGKjWouPelqO41958298 = GrHEGKjWouPelqO31700314;     GrHEGKjWouPelqO31700314 = GrHEGKjWouPelqO15201857;     GrHEGKjWouPelqO15201857 = GrHEGKjWouPelqO71219869;     GrHEGKjWouPelqO71219869 = GrHEGKjWouPelqO78701331;     GrHEGKjWouPelqO78701331 = GrHEGKjWouPelqO15023145;     GrHEGKjWouPelqO15023145 = GrHEGKjWouPelqO32045161;     GrHEGKjWouPelqO32045161 = GrHEGKjWouPelqO88291528;     GrHEGKjWouPelqO88291528 = GrHEGKjWouPelqO30067067;     GrHEGKjWouPelqO30067067 = GrHEGKjWouPelqO83599399;     GrHEGKjWouPelqO83599399 = GrHEGKjWouPelqO22636282;     GrHEGKjWouPelqO22636282 = GrHEGKjWouPelqO28543419;     GrHEGKjWouPelqO28543419 = GrHEGKjWouPelqO49779636;     GrHEGKjWouPelqO49779636 = GrHEGKjWouPelqO36014901;     GrHEGKjWouPelqO36014901 = GrHEGKjWouPelqO26129739;     GrHEGKjWouPelqO26129739 = GrHEGKjWouPelqO38086968;     GrHEGKjWouPelqO38086968 = GrHEGKjWouPelqO5751342;     GrHEGKjWouPelqO5751342 = GrHEGKjWouPelqO6085101;     GrHEGKjWouPelqO6085101 = GrHEGKjWouPelqO49154093;     GrHEGKjWouPelqO49154093 = GrHEGKjWouPelqO26309027;     GrHEGKjWouPelqO26309027 = GrHEGKjWouPelqO79234704;     GrHEGKjWouPelqO79234704 = GrHEGKjWouPelqO49758962;     GrHEGKjWouPelqO49758962 = GrHEGKjWouPelqO81831455;     GrHEGKjWouPelqO81831455 = GrHEGKjWouPelqO41933829;     GrHEGKjWouPelqO41933829 = GrHEGKjWouPelqO69960991;     GrHEGKjWouPelqO69960991 = GrHEGKjWouPelqO52487115;     GrHEGKjWouPelqO52487115 = GrHEGKjWouPelqO10666509;     GrHEGKjWouPelqO10666509 = GrHEGKjWouPelqO32953225;     GrHEGKjWouPelqO32953225 = GrHEGKjWouPelqO82566735;     GrHEGKjWouPelqO82566735 = GrHEGKjWouPelqO20408378;     GrHEGKjWouPelqO20408378 = GrHEGKjWouPelqO79049421;     GrHEGKjWouPelqO79049421 = GrHEGKjWouPelqO35536300;     GrHEGKjWouPelqO35536300 = GrHEGKjWouPelqO10757656;     GrHEGKjWouPelqO10757656 = GrHEGKjWouPelqO13576462;     GrHEGKjWouPelqO13576462 = GrHEGKjWouPelqO1411623;     GrHEGKjWouPelqO1411623 = GrHEGKjWouPelqO10155090;     GrHEGKjWouPelqO10155090 = GrHEGKjWouPelqO15879327;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void BbJuTpYFLSraWsRF7111773() {     int OBZbTPGYURjFXHz54263713 = 82299613;    int OBZbTPGYURjFXHz87042949 = -690603430;    int OBZbTPGYURjFXHz89015276 = -980527674;    int OBZbTPGYURjFXHz69050428 = -235145693;    int OBZbTPGYURjFXHz62849830 = -693184997;    int OBZbTPGYURjFXHz79079249 = -987869513;    int OBZbTPGYURjFXHz75625894 = 16396688;    int OBZbTPGYURjFXHz3332449 = -86322031;    int OBZbTPGYURjFXHz63929762 = 63148145;    int OBZbTPGYURjFXHz2543026 = -714759634;    int OBZbTPGYURjFXHz10176666 = -283468355;    int OBZbTPGYURjFXHz8805697 = -658266898;    int OBZbTPGYURjFXHz57313056 = -108870807;    int OBZbTPGYURjFXHz25618110 = -478070878;    int OBZbTPGYURjFXHz16555606 = -35626123;    int OBZbTPGYURjFXHz25039985 = -565035991;    int OBZbTPGYURjFXHz65386238 = -866695982;    int OBZbTPGYURjFXHz90761926 = -17138467;    int OBZbTPGYURjFXHz59271422 = -320582151;    int OBZbTPGYURjFXHz13010489 = -435441922;    int OBZbTPGYURjFXHz11109662 = -883525859;    int OBZbTPGYURjFXHz28903412 = -863659205;    int OBZbTPGYURjFXHz48152122 = -43332658;    int OBZbTPGYURjFXHz26906099 = -68996570;    int OBZbTPGYURjFXHz38775855 = -976627369;    int OBZbTPGYURjFXHz13723396 = -448458583;    int OBZbTPGYURjFXHz66877260 = -534947261;    int OBZbTPGYURjFXHz11467170 = -79280788;    int OBZbTPGYURjFXHz49204570 = -108320689;    int OBZbTPGYURjFXHz70980934 = -229833165;    int OBZbTPGYURjFXHz28143941 = -85233918;    int OBZbTPGYURjFXHz48357668 = -70637075;    int OBZbTPGYURjFXHz51247188 = -539151420;    int OBZbTPGYURjFXHz33172705 = 68185415;    int OBZbTPGYURjFXHz33170868 = -71208424;    int OBZbTPGYURjFXHz26660824 = -633379664;    int OBZbTPGYURjFXHz68504517 = -220214454;    int OBZbTPGYURjFXHz26691645 = -92412350;    int OBZbTPGYURjFXHz78093307 = -594269881;    int OBZbTPGYURjFXHz45205828 = -669111224;    int OBZbTPGYURjFXHz3577024 = -662564833;    int OBZbTPGYURjFXHz41038494 = -250143129;    int OBZbTPGYURjFXHz68682351 = -466345345;    int OBZbTPGYURjFXHz40327941 = -876363838;    int OBZbTPGYURjFXHz45808666 = -530615417;    int OBZbTPGYURjFXHz55949383 = -376263136;    int OBZbTPGYURjFXHz52267313 = -720780465;    int OBZbTPGYURjFXHz94217453 = -461341340;    int OBZbTPGYURjFXHz59426991 = -545738284;    int OBZbTPGYURjFXHz84427741 = 28078214;    int OBZbTPGYURjFXHz39797365 = -699702738;    int OBZbTPGYURjFXHz5068318 = 73155237;    int OBZbTPGYURjFXHz97127227 = -846349106;    int OBZbTPGYURjFXHz6083297 = -839340664;    int OBZbTPGYURjFXHz96002110 = -192587797;    int OBZbTPGYURjFXHz25360302 = -54041182;    int OBZbTPGYURjFXHz38890827 = -547270772;    int OBZbTPGYURjFXHz62109178 = -811531105;    int OBZbTPGYURjFXHz30274574 = -258518324;    int OBZbTPGYURjFXHz49126435 = -144726415;    int OBZbTPGYURjFXHz12201990 = -352922253;    int OBZbTPGYURjFXHz64158725 = -904322524;    int OBZbTPGYURjFXHz54127878 = -978001343;    int OBZbTPGYURjFXHz92948828 = -707018691;    int OBZbTPGYURjFXHz74399085 = -529525716;    int OBZbTPGYURjFXHz61818998 = -112831280;    int OBZbTPGYURjFXHz57558509 = -19115478;    int OBZbTPGYURjFXHz24140352 = -77056222;    int OBZbTPGYURjFXHz92447241 = -306862455;    int OBZbTPGYURjFXHz89894781 = -402246459;    int OBZbTPGYURjFXHz56535468 = -244821537;    int OBZbTPGYURjFXHz38694593 = -674283633;    int OBZbTPGYURjFXHz12668619 = -422868586;    int OBZbTPGYURjFXHz14065594 = -651470927;    int OBZbTPGYURjFXHz9433466 = -772877090;    int OBZbTPGYURjFXHz70071168 = -533382730;    int OBZbTPGYURjFXHz60221061 = -297313861;    int OBZbTPGYURjFXHz7824182 = -166968821;    int OBZbTPGYURjFXHz81097432 = -538381153;    int OBZbTPGYURjFXHz82826472 = -500364233;    int OBZbTPGYURjFXHz61456082 = -727678118;    int OBZbTPGYURjFXHz72659806 = 26394079;    int OBZbTPGYURjFXHz52040178 = -533542505;    int OBZbTPGYURjFXHz64776829 = -36398904;    int OBZbTPGYURjFXHz31183570 = -530130427;    int OBZbTPGYURjFXHz23075624 = -58389156;    int OBZbTPGYURjFXHz51230440 = -224287970;    int OBZbTPGYURjFXHz45163891 = -699810756;    int OBZbTPGYURjFXHz37170594 = -739226788;    int OBZbTPGYURjFXHz7810567 = 82832758;    int OBZbTPGYURjFXHz87769997 = 13891108;    int OBZbTPGYURjFXHz6395339 = -408683350;    int OBZbTPGYURjFXHz96417071 = -833894026;    int OBZbTPGYURjFXHz28966872 = -349543467;    int OBZbTPGYURjFXHz33003839 = -216188971;    int OBZbTPGYURjFXHz39418298 = -758242309;    int OBZbTPGYURjFXHz86910615 = -272141787;    int OBZbTPGYURjFXHz75733522 = -759326654;    int OBZbTPGYURjFXHz65928856 = -246838122;    int OBZbTPGYURjFXHz83989667 = 82299613;     OBZbTPGYURjFXHz54263713 = OBZbTPGYURjFXHz87042949;     OBZbTPGYURjFXHz87042949 = OBZbTPGYURjFXHz89015276;     OBZbTPGYURjFXHz89015276 = OBZbTPGYURjFXHz69050428;     OBZbTPGYURjFXHz69050428 = OBZbTPGYURjFXHz62849830;     OBZbTPGYURjFXHz62849830 = OBZbTPGYURjFXHz79079249;     OBZbTPGYURjFXHz79079249 = OBZbTPGYURjFXHz75625894;     OBZbTPGYURjFXHz75625894 = OBZbTPGYURjFXHz3332449;     OBZbTPGYURjFXHz3332449 = OBZbTPGYURjFXHz63929762;     OBZbTPGYURjFXHz63929762 = OBZbTPGYURjFXHz2543026;     OBZbTPGYURjFXHz2543026 = OBZbTPGYURjFXHz10176666;     OBZbTPGYURjFXHz10176666 = OBZbTPGYURjFXHz8805697;     OBZbTPGYURjFXHz8805697 = OBZbTPGYURjFXHz57313056;     OBZbTPGYURjFXHz57313056 = OBZbTPGYURjFXHz25618110;     OBZbTPGYURjFXHz25618110 = OBZbTPGYURjFXHz16555606;     OBZbTPGYURjFXHz16555606 = OBZbTPGYURjFXHz25039985;     OBZbTPGYURjFXHz25039985 = OBZbTPGYURjFXHz65386238;     OBZbTPGYURjFXHz65386238 = OBZbTPGYURjFXHz90761926;     OBZbTPGYURjFXHz90761926 = OBZbTPGYURjFXHz59271422;     OBZbTPGYURjFXHz59271422 = OBZbTPGYURjFXHz13010489;     OBZbTPGYURjFXHz13010489 = OBZbTPGYURjFXHz11109662;     OBZbTPGYURjFXHz11109662 = OBZbTPGYURjFXHz28903412;     OBZbTPGYURjFXHz28903412 = OBZbTPGYURjFXHz48152122;     OBZbTPGYURjFXHz48152122 = OBZbTPGYURjFXHz26906099;     OBZbTPGYURjFXHz26906099 = OBZbTPGYURjFXHz38775855;     OBZbTPGYURjFXHz38775855 = OBZbTPGYURjFXHz13723396;     OBZbTPGYURjFXHz13723396 = OBZbTPGYURjFXHz66877260;     OBZbTPGYURjFXHz66877260 = OBZbTPGYURjFXHz11467170;     OBZbTPGYURjFXHz11467170 = OBZbTPGYURjFXHz49204570;     OBZbTPGYURjFXHz49204570 = OBZbTPGYURjFXHz70980934;     OBZbTPGYURjFXHz70980934 = OBZbTPGYURjFXHz28143941;     OBZbTPGYURjFXHz28143941 = OBZbTPGYURjFXHz48357668;     OBZbTPGYURjFXHz48357668 = OBZbTPGYURjFXHz51247188;     OBZbTPGYURjFXHz51247188 = OBZbTPGYURjFXHz33172705;     OBZbTPGYURjFXHz33172705 = OBZbTPGYURjFXHz33170868;     OBZbTPGYURjFXHz33170868 = OBZbTPGYURjFXHz26660824;     OBZbTPGYURjFXHz26660824 = OBZbTPGYURjFXHz68504517;     OBZbTPGYURjFXHz68504517 = OBZbTPGYURjFXHz26691645;     OBZbTPGYURjFXHz26691645 = OBZbTPGYURjFXHz78093307;     OBZbTPGYURjFXHz78093307 = OBZbTPGYURjFXHz45205828;     OBZbTPGYURjFXHz45205828 = OBZbTPGYURjFXHz3577024;     OBZbTPGYURjFXHz3577024 = OBZbTPGYURjFXHz41038494;     OBZbTPGYURjFXHz41038494 = OBZbTPGYURjFXHz68682351;     OBZbTPGYURjFXHz68682351 = OBZbTPGYURjFXHz40327941;     OBZbTPGYURjFXHz40327941 = OBZbTPGYURjFXHz45808666;     OBZbTPGYURjFXHz45808666 = OBZbTPGYURjFXHz55949383;     OBZbTPGYURjFXHz55949383 = OBZbTPGYURjFXHz52267313;     OBZbTPGYURjFXHz52267313 = OBZbTPGYURjFXHz94217453;     OBZbTPGYURjFXHz94217453 = OBZbTPGYURjFXHz59426991;     OBZbTPGYURjFXHz59426991 = OBZbTPGYURjFXHz84427741;     OBZbTPGYURjFXHz84427741 = OBZbTPGYURjFXHz39797365;     OBZbTPGYURjFXHz39797365 = OBZbTPGYURjFXHz5068318;     OBZbTPGYURjFXHz5068318 = OBZbTPGYURjFXHz97127227;     OBZbTPGYURjFXHz97127227 = OBZbTPGYURjFXHz6083297;     OBZbTPGYURjFXHz6083297 = OBZbTPGYURjFXHz96002110;     OBZbTPGYURjFXHz96002110 = OBZbTPGYURjFXHz25360302;     OBZbTPGYURjFXHz25360302 = OBZbTPGYURjFXHz38890827;     OBZbTPGYURjFXHz38890827 = OBZbTPGYURjFXHz62109178;     OBZbTPGYURjFXHz62109178 = OBZbTPGYURjFXHz30274574;     OBZbTPGYURjFXHz30274574 = OBZbTPGYURjFXHz49126435;     OBZbTPGYURjFXHz49126435 = OBZbTPGYURjFXHz12201990;     OBZbTPGYURjFXHz12201990 = OBZbTPGYURjFXHz64158725;     OBZbTPGYURjFXHz64158725 = OBZbTPGYURjFXHz54127878;     OBZbTPGYURjFXHz54127878 = OBZbTPGYURjFXHz92948828;     OBZbTPGYURjFXHz92948828 = OBZbTPGYURjFXHz74399085;     OBZbTPGYURjFXHz74399085 = OBZbTPGYURjFXHz61818998;     OBZbTPGYURjFXHz61818998 = OBZbTPGYURjFXHz57558509;     OBZbTPGYURjFXHz57558509 = OBZbTPGYURjFXHz24140352;     OBZbTPGYURjFXHz24140352 = OBZbTPGYURjFXHz92447241;     OBZbTPGYURjFXHz92447241 = OBZbTPGYURjFXHz89894781;     OBZbTPGYURjFXHz89894781 = OBZbTPGYURjFXHz56535468;     OBZbTPGYURjFXHz56535468 = OBZbTPGYURjFXHz38694593;     OBZbTPGYURjFXHz38694593 = OBZbTPGYURjFXHz12668619;     OBZbTPGYURjFXHz12668619 = OBZbTPGYURjFXHz14065594;     OBZbTPGYURjFXHz14065594 = OBZbTPGYURjFXHz9433466;     OBZbTPGYURjFXHz9433466 = OBZbTPGYURjFXHz70071168;     OBZbTPGYURjFXHz70071168 = OBZbTPGYURjFXHz60221061;     OBZbTPGYURjFXHz60221061 = OBZbTPGYURjFXHz7824182;     OBZbTPGYURjFXHz7824182 = OBZbTPGYURjFXHz81097432;     OBZbTPGYURjFXHz81097432 = OBZbTPGYURjFXHz82826472;     OBZbTPGYURjFXHz82826472 = OBZbTPGYURjFXHz61456082;     OBZbTPGYURjFXHz61456082 = OBZbTPGYURjFXHz72659806;     OBZbTPGYURjFXHz72659806 = OBZbTPGYURjFXHz52040178;     OBZbTPGYURjFXHz52040178 = OBZbTPGYURjFXHz64776829;     OBZbTPGYURjFXHz64776829 = OBZbTPGYURjFXHz31183570;     OBZbTPGYURjFXHz31183570 = OBZbTPGYURjFXHz23075624;     OBZbTPGYURjFXHz23075624 = OBZbTPGYURjFXHz51230440;     OBZbTPGYURjFXHz51230440 = OBZbTPGYURjFXHz45163891;     OBZbTPGYURjFXHz45163891 = OBZbTPGYURjFXHz37170594;     OBZbTPGYURjFXHz37170594 = OBZbTPGYURjFXHz7810567;     OBZbTPGYURjFXHz7810567 = OBZbTPGYURjFXHz87769997;     OBZbTPGYURjFXHz87769997 = OBZbTPGYURjFXHz6395339;     OBZbTPGYURjFXHz6395339 = OBZbTPGYURjFXHz96417071;     OBZbTPGYURjFXHz96417071 = OBZbTPGYURjFXHz28966872;     OBZbTPGYURjFXHz28966872 = OBZbTPGYURjFXHz33003839;     OBZbTPGYURjFXHz33003839 = OBZbTPGYURjFXHz39418298;     OBZbTPGYURjFXHz39418298 = OBZbTPGYURjFXHz86910615;     OBZbTPGYURjFXHz86910615 = OBZbTPGYURjFXHz75733522;     OBZbTPGYURjFXHz75733522 = OBZbTPGYURjFXHz65928856;     OBZbTPGYURjFXHz65928856 = OBZbTPGYURjFXHz83989667;     OBZbTPGYURjFXHz83989667 = OBZbTPGYURjFXHz54263713;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void RgZuoXuRrRaivPyE13770518() {     float lEfsQaskvHqBohT54257242 = -890224526;    float lEfsQaskvHqBohT25615266 = -733198549;    float lEfsQaskvHqBohT90328488 = -937656288;    float lEfsQaskvHqBohT54130289 = -55686526;    float lEfsQaskvHqBohT71689932 = -61540310;    float lEfsQaskvHqBohT13884682 = -687302325;    float lEfsQaskvHqBohT44173231 = -788139779;    float lEfsQaskvHqBohT72546498 = -890499333;    float lEfsQaskvHqBohT46669927 = -750497924;    float lEfsQaskvHqBohT99182539 = -351566505;    float lEfsQaskvHqBohT30497272 = -242940426;    float lEfsQaskvHqBohT23097374 = -393745251;    float lEfsQaskvHqBohT89165970 = -130529281;    float lEfsQaskvHqBohT79462329 = -823223929;    float lEfsQaskvHqBohT61727715 = 1661044;    float lEfsQaskvHqBohT99604531 = -897831641;    float lEfsQaskvHqBohT21238998 = -686310552;    float lEfsQaskvHqBohT13762459 = -414210085;    float lEfsQaskvHqBohT79699691 = -938564418;    float lEfsQaskvHqBohT85084251 = 52533626;    float lEfsQaskvHqBohT90779086 = -593992228;    float lEfsQaskvHqBohT7199678 = -273651088;    float lEfsQaskvHqBohT85030518 = -502290500;    float lEfsQaskvHqBohT70556207 = 50082908;    float lEfsQaskvHqBohT67218544 = -303321210;    float lEfsQaskvHqBohT30297472 = -356664515;    float lEfsQaskvHqBohT37098842 = -835746974;    float lEfsQaskvHqBohT46870694 = -688247770;    float lEfsQaskvHqBohT61883017 = -754391019;    float lEfsQaskvHqBohT47070804 = 94735461;    float lEfsQaskvHqBohT61556620 = -472327153;    float lEfsQaskvHqBohT33081525 = -462458840;    float lEfsQaskvHqBohT36699611 = -741095639;    float lEfsQaskvHqBohT33455542 = -908496317;    float lEfsQaskvHqBohT15528906 = -330522351;    float lEfsQaskvHqBohT69489013 = -713145429;    float lEfsQaskvHqBohT80445884 = -319867832;    float lEfsQaskvHqBohT96609324 = -87675026;    float lEfsQaskvHqBohT13406904 = -206951310;    float lEfsQaskvHqBohT17078721 = -278542910;    float lEfsQaskvHqBohT91432691 = -750508731;    float lEfsQaskvHqBohT26736155 = -443935232;    float lEfsQaskvHqBohT28965711 = 74524399;    float lEfsQaskvHqBohT99559839 = -433328281;    float lEfsQaskvHqBohT29826719 = 96125180;    float lEfsQaskvHqBohT96377351 = -188307826;    float lEfsQaskvHqBohT79965106 = -102061087;    float lEfsQaskvHqBohT69601034 = 29897303;    float lEfsQaskvHqBohT78085381 = -109502170;    float lEfsQaskvHqBohT4473165 = -658946667;    float lEfsQaskvHqBohT53056734 = -278257745;    float lEfsQaskvHqBohT19615575 = -308990515;    float lEfsQaskvHqBohT72575576 = -738403111;    float lEfsQaskvHqBohT16333568 = -271260516;    float lEfsQaskvHqBohT22991432 = 50512920;    float lEfsQaskvHqBohT47057564 = -516573438;    float lEfsQaskvHqBohT40584748 = -130908049;    float lEfsQaskvHqBohT19772282 = -887739196;    float lEfsQaskvHqBohT86911745 = -752365317;    float lEfsQaskvHqBohT41392461 = -704875796;    float lEfsQaskvHqBohT76785840 = -851555352;    float lEfsQaskvHqBohT97302536 = 107991;    float lEfsQaskvHqBohT10663482 = -36108314;    float lEfsQaskvHqBohT99599122 = -745233386;    float lEfsQaskvHqBohT37625920 = -879239353;    float lEfsQaskvHqBohT97415746 = -780481587;    float lEfsQaskvHqBohT86397762 = -652649613;    float lEfsQaskvHqBohT55710429 = -222032964;    float lEfsQaskvHqBohT63933424 = -392701578;    float lEfsQaskvHqBohT92238701 = -285193528;    float lEfsQaskvHqBohT19158647 = -477963810;    float lEfsQaskvHqBohT24629673 = -498635527;    float lEfsQaskvHqBohT355556 = -107258776;    float lEfsQaskvHqBohT62620970 = -560021509;    float lEfsQaskvHqBohT93651560 = -196957643;    float lEfsQaskvHqBohT64042932 = -50056996;    float lEfsQaskvHqBohT78233966 = -248175488;    float lEfsQaskvHqBohT85470679 = 31037780;    float lEfsQaskvHqBohT40729489 = 53957727;    float lEfsQaskvHqBohT70841193 = -15013385;    float lEfsQaskvHqBohT50332366 = -154603429;    float lEfsQaskvHqBohT67497807 = -765644277;    float lEfsQaskvHqBohT68785313 = -478745601;    float lEfsQaskvHqBohT57409853 = 4555647;    float lEfsQaskvHqBohT94014069 = -627006794;    float lEfsQaskvHqBohT41941045 = -63336638;    float lEfsQaskvHqBohT60505949 = -724055729;    float lEfsQaskvHqBohT20366044 = -369835124;    float lEfsQaskvHqBohT10464111 = -859009238;    float lEfsQaskvHqBohT68471341 = -813948914;    float lEfsQaskvHqBohT28904266 = -482237380;    float lEfsQaskvHqBohT60673603 = -432128637;    float lEfsQaskvHqBohT9697580 = -335309709;    float lEfsQaskvHqBohT72014443 = -502075514;    float lEfsQaskvHqBohT40292881 = -426987559;    float lEfsQaskvHqBohT94130154 = -650616722;    float lEfsQaskvHqBohT16072673 = -307826919;    float lEfsQaskvHqBohT29366589 = -180242215;    float lEfsQaskvHqBohT61933919 = -554088929;    float lEfsQaskvHqBohT32410972 = -890224526;     lEfsQaskvHqBohT54257242 = lEfsQaskvHqBohT25615266;     lEfsQaskvHqBohT25615266 = lEfsQaskvHqBohT90328488;     lEfsQaskvHqBohT90328488 = lEfsQaskvHqBohT54130289;     lEfsQaskvHqBohT54130289 = lEfsQaskvHqBohT71689932;     lEfsQaskvHqBohT71689932 = lEfsQaskvHqBohT13884682;     lEfsQaskvHqBohT13884682 = lEfsQaskvHqBohT44173231;     lEfsQaskvHqBohT44173231 = lEfsQaskvHqBohT72546498;     lEfsQaskvHqBohT72546498 = lEfsQaskvHqBohT46669927;     lEfsQaskvHqBohT46669927 = lEfsQaskvHqBohT99182539;     lEfsQaskvHqBohT99182539 = lEfsQaskvHqBohT30497272;     lEfsQaskvHqBohT30497272 = lEfsQaskvHqBohT23097374;     lEfsQaskvHqBohT23097374 = lEfsQaskvHqBohT89165970;     lEfsQaskvHqBohT89165970 = lEfsQaskvHqBohT79462329;     lEfsQaskvHqBohT79462329 = lEfsQaskvHqBohT61727715;     lEfsQaskvHqBohT61727715 = lEfsQaskvHqBohT99604531;     lEfsQaskvHqBohT99604531 = lEfsQaskvHqBohT21238998;     lEfsQaskvHqBohT21238998 = lEfsQaskvHqBohT13762459;     lEfsQaskvHqBohT13762459 = lEfsQaskvHqBohT79699691;     lEfsQaskvHqBohT79699691 = lEfsQaskvHqBohT85084251;     lEfsQaskvHqBohT85084251 = lEfsQaskvHqBohT90779086;     lEfsQaskvHqBohT90779086 = lEfsQaskvHqBohT7199678;     lEfsQaskvHqBohT7199678 = lEfsQaskvHqBohT85030518;     lEfsQaskvHqBohT85030518 = lEfsQaskvHqBohT70556207;     lEfsQaskvHqBohT70556207 = lEfsQaskvHqBohT67218544;     lEfsQaskvHqBohT67218544 = lEfsQaskvHqBohT30297472;     lEfsQaskvHqBohT30297472 = lEfsQaskvHqBohT37098842;     lEfsQaskvHqBohT37098842 = lEfsQaskvHqBohT46870694;     lEfsQaskvHqBohT46870694 = lEfsQaskvHqBohT61883017;     lEfsQaskvHqBohT61883017 = lEfsQaskvHqBohT47070804;     lEfsQaskvHqBohT47070804 = lEfsQaskvHqBohT61556620;     lEfsQaskvHqBohT61556620 = lEfsQaskvHqBohT33081525;     lEfsQaskvHqBohT33081525 = lEfsQaskvHqBohT36699611;     lEfsQaskvHqBohT36699611 = lEfsQaskvHqBohT33455542;     lEfsQaskvHqBohT33455542 = lEfsQaskvHqBohT15528906;     lEfsQaskvHqBohT15528906 = lEfsQaskvHqBohT69489013;     lEfsQaskvHqBohT69489013 = lEfsQaskvHqBohT80445884;     lEfsQaskvHqBohT80445884 = lEfsQaskvHqBohT96609324;     lEfsQaskvHqBohT96609324 = lEfsQaskvHqBohT13406904;     lEfsQaskvHqBohT13406904 = lEfsQaskvHqBohT17078721;     lEfsQaskvHqBohT17078721 = lEfsQaskvHqBohT91432691;     lEfsQaskvHqBohT91432691 = lEfsQaskvHqBohT26736155;     lEfsQaskvHqBohT26736155 = lEfsQaskvHqBohT28965711;     lEfsQaskvHqBohT28965711 = lEfsQaskvHqBohT99559839;     lEfsQaskvHqBohT99559839 = lEfsQaskvHqBohT29826719;     lEfsQaskvHqBohT29826719 = lEfsQaskvHqBohT96377351;     lEfsQaskvHqBohT96377351 = lEfsQaskvHqBohT79965106;     lEfsQaskvHqBohT79965106 = lEfsQaskvHqBohT69601034;     lEfsQaskvHqBohT69601034 = lEfsQaskvHqBohT78085381;     lEfsQaskvHqBohT78085381 = lEfsQaskvHqBohT4473165;     lEfsQaskvHqBohT4473165 = lEfsQaskvHqBohT53056734;     lEfsQaskvHqBohT53056734 = lEfsQaskvHqBohT19615575;     lEfsQaskvHqBohT19615575 = lEfsQaskvHqBohT72575576;     lEfsQaskvHqBohT72575576 = lEfsQaskvHqBohT16333568;     lEfsQaskvHqBohT16333568 = lEfsQaskvHqBohT22991432;     lEfsQaskvHqBohT22991432 = lEfsQaskvHqBohT47057564;     lEfsQaskvHqBohT47057564 = lEfsQaskvHqBohT40584748;     lEfsQaskvHqBohT40584748 = lEfsQaskvHqBohT19772282;     lEfsQaskvHqBohT19772282 = lEfsQaskvHqBohT86911745;     lEfsQaskvHqBohT86911745 = lEfsQaskvHqBohT41392461;     lEfsQaskvHqBohT41392461 = lEfsQaskvHqBohT76785840;     lEfsQaskvHqBohT76785840 = lEfsQaskvHqBohT97302536;     lEfsQaskvHqBohT97302536 = lEfsQaskvHqBohT10663482;     lEfsQaskvHqBohT10663482 = lEfsQaskvHqBohT99599122;     lEfsQaskvHqBohT99599122 = lEfsQaskvHqBohT37625920;     lEfsQaskvHqBohT37625920 = lEfsQaskvHqBohT97415746;     lEfsQaskvHqBohT97415746 = lEfsQaskvHqBohT86397762;     lEfsQaskvHqBohT86397762 = lEfsQaskvHqBohT55710429;     lEfsQaskvHqBohT55710429 = lEfsQaskvHqBohT63933424;     lEfsQaskvHqBohT63933424 = lEfsQaskvHqBohT92238701;     lEfsQaskvHqBohT92238701 = lEfsQaskvHqBohT19158647;     lEfsQaskvHqBohT19158647 = lEfsQaskvHqBohT24629673;     lEfsQaskvHqBohT24629673 = lEfsQaskvHqBohT355556;     lEfsQaskvHqBohT355556 = lEfsQaskvHqBohT62620970;     lEfsQaskvHqBohT62620970 = lEfsQaskvHqBohT93651560;     lEfsQaskvHqBohT93651560 = lEfsQaskvHqBohT64042932;     lEfsQaskvHqBohT64042932 = lEfsQaskvHqBohT78233966;     lEfsQaskvHqBohT78233966 = lEfsQaskvHqBohT85470679;     lEfsQaskvHqBohT85470679 = lEfsQaskvHqBohT40729489;     lEfsQaskvHqBohT40729489 = lEfsQaskvHqBohT70841193;     lEfsQaskvHqBohT70841193 = lEfsQaskvHqBohT50332366;     lEfsQaskvHqBohT50332366 = lEfsQaskvHqBohT67497807;     lEfsQaskvHqBohT67497807 = lEfsQaskvHqBohT68785313;     lEfsQaskvHqBohT68785313 = lEfsQaskvHqBohT57409853;     lEfsQaskvHqBohT57409853 = lEfsQaskvHqBohT94014069;     lEfsQaskvHqBohT94014069 = lEfsQaskvHqBohT41941045;     lEfsQaskvHqBohT41941045 = lEfsQaskvHqBohT60505949;     lEfsQaskvHqBohT60505949 = lEfsQaskvHqBohT20366044;     lEfsQaskvHqBohT20366044 = lEfsQaskvHqBohT10464111;     lEfsQaskvHqBohT10464111 = lEfsQaskvHqBohT68471341;     lEfsQaskvHqBohT68471341 = lEfsQaskvHqBohT28904266;     lEfsQaskvHqBohT28904266 = lEfsQaskvHqBohT60673603;     lEfsQaskvHqBohT60673603 = lEfsQaskvHqBohT9697580;     lEfsQaskvHqBohT9697580 = lEfsQaskvHqBohT72014443;     lEfsQaskvHqBohT72014443 = lEfsQaskvHqBohT40292881;     lEfsQaskvHqBohT40292881 = lEfsQaskvHqBohT94130154;     lEfsQaskvHqBohT94130154 = lEfsQaskvHqBohT16072673;     lEfsQaskvHqBohT16072673 = lEfsQaskvHqBohT29366589;     lEfsQaskvHqBohT29366589 = lEfsQaskvHqBohT61933919;     lEfsQaskvHqBohT61933919 = lEfsQaskvHqBohT32410972;     lEfsQaskvHqBohT32410972 = lEfsQaskvHqBohT54257242;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void DrfyiAxnHDIZTaQb89782637() {     float ZkxxTTGSRDRnfVX41178919 = 69124267;    float ZkxxTTGSRDRnfVX35263004 = -843785128;    float ZkxxTTGSRDRnfVX83024502 = -368771671;    float ZkxxTTGSRDRnfVX81993483 = -61483814;    float ZkxxTTGSRDRnfVX22419020 = 84646204;    float ZkxxTTGSRDRnfVX27350359 = 15004381;    float ZkxxTTGSRDRnfVX94218899 = -469937818;    float ZkxxTTGSRDRnfVX55262847 = -321039169;    float ZkxxTTGSRDRnfVX25055617 = -960008627;    float ZkxxTTGSRDRnfVX72682925 = -66427711;    float ZkxxTTGSRDRnfVX50178002 = -397029170;    float ZkxxTTGSRDRnfVX92723851 = -515848145;    float ZkxxTTGSRDRnfVX49792420 = -218892570;    float ZkxxTTGSRDRnfVX22780439 = -619722329;    float ZkxxTTGSRDRnfVX85440584 = -185104524;    float ZkxxTTGSRDRnfVX34883691 = -876483228;    float ZkxxTTGSRDRnfVX71079173 = -256230389;    float ZkxxTTGSRDRnfVX42979217 = -89596333;    float ZkxxTTGSRDRnfVX40859263 = -275085388;    float ZkxxTTGSRDRnfVX41837079 = -385564213;    float ZkxxTTGSRDRnfVX58244754 = -638671226;    float ZkxxTTGSRDRnfVX45889637 = -101772991;    float ZkxxTTGSRDRnfVX54676859 = -399696750;    float ZkxxTTGSRDRnfVX46913157 = -984005868;    float ZkxxTTGSRDRnfVX23183530 = -327589002;    float ZkxxTTGSRDRnfVX24476372 = 79812143;    float ZkxxTTGSRDRnfVX93989269 = 12150962;    float ZkxxTTGSRDRnfVX47457244 = -915897044;    float ZkxxTTGSRDRnfVX34785810 = -70623636;    float ZkxxTTGSRDRnfVX26784443 = -566484608;    float ZkxxTTGSRDRnfVX20676872 = -388756052;    float ZkxxTTGSRDRnfVX37523555 = -633380538;    float ZkxxTTGSRDRnfVX90972693 = -224787369;    float ZkxxTTGSRDRnfVX63378640 = 55388914;    float ZkxxTTGSRDRnfVX8307855 = -702492445;    float ZkxxTTGSRDRnfVX2449229 = -893741227;    float ZkxxTTGSRDRnfVX36415148 = -287773122;    float ZkxxTTGSRDRnfVX68937701 = 32092402;    float ZkxxTTGSRDRnfVX66638070 = -569802469;    float ZkxxTTGSRDRnfVX44865796 = -558936079;    float ZkxxTTGSRDRnfVX74594122 = -472431381;    float ZkxxTTGSRDRnfVX11577080 = -801090694;    float ZkxxTTGSRDRnfVX5415885 = -818955651;    float ZkxxTTGSRDRnfVX18104519 = -483323963;    float ZkxxTTGSRDRnfVX81811578 = -359766536;    float ZkxxTTGSRDRnfVX98944272 = -143657941;    float ZkxxTTGSRDRnfVX615132 = -677765602;    float ZkxxTTGSRDRnfVX71856978 = -168872450;    float ZkxxTTGSRDRnfVX68311009 = 40754175;    float ZkxxTTGSRDRnfVX97072743 = -996965002;    float ZkxxTTGSRDRnfVX94414596 = -337612472;    float ZkxxTTGSRDRnfVX24528618 = -908303195;    float ZkxxTTGSRDRnfVX57717179 = -546322798;    float ZkxxTTGSRDRnfVX70878537 = -887352676;    float ZkxxTTGSRDRnfVX31537883 = -600600322;    float ZkxxTTGSRDRnfVX95289282 = -829102743;    float ZkxxTTGSRDRnfVX80586145 = -344088379;    float ZkxxTTGSRDRnfVX36111345 = -384765803;    float ZkxxTTGSRDRnfVX58809954 = -733894813;    float ZkxxTTGSRDRnfVX97942647 = -995165940;    float ZkxxTTGSRDRnfVX33361089 = -997146581;    float ZkxxTTGSRDRnfVX46761655 = -554040774;    float ZkxxTTGSRDRnfVX20477037 = -150415534;    float ZkxxTTGSRDRnfVX98271174 = -293524019;    float ZkxxTTGSRDRnfVX52006054 = -677671659;    float ZkxxTTGSRDRnfVX12654447 = -763648633;    float ZkxxTTGSRDRnfVX1751158 = -191060776;    float ZkxxTTGSRDRnfVX86413779 = -174281484;    float ZkxxTTGSRDRnfVX14472585 = -917229884;    float ZkxxTTGSRDRnfVX82991355 = -291363298;    float ZkxxTTGSRDRnfVX98468542 = -488710106;    float ZkxxTTGSRDRnfVX2141472 = -188322791;    float ZkxxTTGSRDRnfVX76341146 = -519793864;    float ZkxxTTGSRDRnfVX95993466 = -716149309;    float ZkxxTTGSRDRnfVX67242957 = -913132832;    float ZkxxTTGSRDRnfVX46667675 = -837580533;    float ZkxxTTGSRDRnfVX40473753 = -282817340;    float ZkxxTTGSRDRnfVX36572340 = -916372787;    float ZkxxTTGSRDRnfVX65101579 = -524239332;    float ZkxxTTGSRDRnfVX24239257 = -83931061;    float ZkxxTTGSRDRnfVX23861241 = -242422255;    float ZkxxTTGSRDRnfVX22132292 = -818976589;    float ZkxxTTGSRDRnfVX79146235 = -856651220;    float ZkxxTTGSRDRnfVX37713067 = -73658634;    float ZkxxTTGSRDRnfVX32369846 = -128872137;    float ZkxxTTGSRDRnfVX96148254 = -480452858;    float ZkxxTTGSRDRnfVX79806376 = 12942260;    float ZkxxTTGSRDRnfVX20094157 = -337434694;    float ZkxxTTGSRDRnfVX31840757 = -344010765;    float ZkxxTTGSRDRnfVX13018573 = -873389703;    float ZkxxTTGSRDRnfVX21863083 = -449652849;    float ZkxxTTGSRDRnfVX303803 = -903007319;    float ZkxxTTGSRDRnfVX10127748 = -234012786;    float ZkxxTTGSRDRnfVX68695423 = -574636530;    float ZkxxTTGSRDRnfVX11504707 = -561789499;    float ZkxxTTGSRDRnfVX27832467 = -918390608;    float ZkxxTTGSRDRnfVX91100043 = -550675160;    float ZkxxTTGSRDRnfVX7144711 = -425431632;    float ZkxxTTGSRDRnfVX66098465 = -805652305;    float ZkxxTTGSRDRnfVX69157132 = 69124267;     ZkxxTTGSRDRnfVX41178919 = ZkxxTTGSRDRnfVX35263004;     ZkxxTTGSRDRnfVX35263004 = ZkxxTTGSRDRnfVX83024502;     ZkxxTTGSRDRnfVX83024502 = ZkxxTTGSRDRnfVX81993483;     ZkxxTTGSRDRnfVX81993483 = ZkxxTTGSRDRnfVX22419020;     ZkxxTTGSRDRnfVX22419020 = ZkxxTTGSRDRnfVX27350359;     ZkxxTTGSRDRnfVX27350359 = ZkxxTTGSRDRnfVX94218899;     ZkxxTTGSRDRnfVX94218899 = ZkxxTTGSRDRnfVX55262847;     ZkxxTTGSRDRnfVX55262847 = ZkxxTTGSRDRnfVX25055617;     ZkxxTTGSRDRnfVX25055617 = ZkxxTTGSRDRnfVX72682925;     ZkxxTTGSRDRnfVX72682925 = ZkxxTTGSRDRnfVX50178002;     ZkxxTTGSRDRnfVX50178002 = ZkxxTTGSRDRnfVX92723851;     ZkxxTTGSRDRnfVX92723851 = ZkxxTTGSRDRnfVX49792420;     ZkxxTTGSRDRnfVX49792420 = ZkxxTTGSRDRnfVX22780439;     ZkxxTTGSRDRnfVX22780439 = ZkxxTTGSRDRnfVX85440584;     ZkxxTTGSRDRnfVX85440584 = ZkxxTTGSRDRnfVX34883691;     ZkxxTTGSRDRnfVX34883691 = ZkxxTTGSRDRnfVX71079173;     ZkxxTTGSRDRnfVX71079173 = ZkxxTTGSRDRnfVX42979217;     ZkxxTTGSRDRnfVX42979217 = ZkxxTTGSRDRnfVX40859263;     ZkxxTTGSRDRnfVX40859263 = ZkxxTTGSRDRnfVX41837079;     ZkxxTTGSRDRnfVX41837079 = ZkxxTTGSRDRnfVX58244754;     ZkxxTTGSRDRnfVX58244754 = ZkxxTTGSRDRnfVX45889637;     ZkxxTTGSRDRnfVX45889637 = ZkxxTTGSRDRnfVX54676859;     ZkxxTTGSRDRnfVX54676859 = ZkxxTTGSRDRnfVX46913157;     ZkxxTTGSRDRnfVX46913157 = ZkxxTTGSRDRnfVX23183530;     ZkxxTTGSRDRnfVX23183530 = ZkxxTTGSRDRnfVX24476372;     ZkxxTTGSRDRnfVX24476372 = ZkxxTTGSRDRnfVX93989269;     ZkxxTTGSRDRnfVX93989269 = ZkxxTTGSRDRnfVX47457244;     ZkxxTTGSRDRnfVX47457244 = ZkxxTTGSRDRnfVX34785810;     ZkxxTTGSRDRnfVX34785810 = ZkxxTTGSRDRnfVX26784443;     ZkxxTTGSRDRnfVX26784443 = ZkxxTTGSRDRnfVX20676872;     ZkxxTTGSRDRnfVX20676872 = ZkxxTTGSRDRnfVX37523555;     ZkxxTTGSRDRnfVX37523555 = ZkxxTTGSRDRnfVX90972693;     ZkxxTTGSRDRnfVX90972693 = ZkxxTTGSRDRnfVX63378640;     ZkxxTTGSRDRnfVX63378640 = ZkxxTTGSRDRnfVX8307855;     ZkxxTTGSRDRnfVX8307855 = ZkxxTTGSRDRnfVX2449229;     ZkxxTTGSRDRnfVX2449229 = ZkxxTTGSRDRnfVX36415148;     ZkxxTTGSRDRnfVX36415148 = ZkxxTTGSRDRnfVX68937701;     ZkxxTTGSRDRnfVX68937701 = ZkxxTTGSRDRnfVX66638070;     ZkxxTTGSRDRnfVX66638070 = ZkxxTTGSRDRnfVX44865796;     ZkxxTTGSRDRnfVX44865796 = ZkxxTTGSRDRnfVX74594122;     ZkxxTTGSRDRnfVX74594122 = ZkxxTTGSRDRnfVX11577080;     ZkxxTTGSRDRnfVX11577080 = ZkxxTTGSRDRnfVX5415885;     ZkxxTTGSRDRnfVX5415885 = ZkxxTTGSRDRnfVX18104519;     ZkxxTTGSRDRnfVX18104519 = ZkxxTTGSRDRnfVX81811578;     ZkxxTTGSRDRnfVX81811578 = ZkxxTTGSRDRnfVX98944272;     ZkxxTTGSRDRnfVX98944272 = ZkxxTTGSRDRnfVX615132;     ZkxxTTGSRDRnfVX615132 = ZkxxTTGSRDRnfVX71856978;     ZkxxTTGSRDRnfVX71856978 = ZkxxTTGSRDRnfVX68311009;     ZkxxTTGSRDRnfVX68311009 = ZkxxTTGSRDRnfVX97072743;     ZkxxTTGSRDRnfVX97072743 = ZkxxTTGSRDRnfVX94414596;     ZkxxTTGSRDRnfVX94414596 = ZkxxTTGSRDRnfVX24528618;     ZkxxTTGSRDRnfVX24528618 = ZkxxTTGSRDRnfVX57717179;     ZkxxTTGSRDRnfVX57717179 = ZkxxTTGSRDRnfVX70878537;     ZkxxTTGSRDRnfVX70878537 = ZkxxTTGSRDRnfVX31537883;     ZkxxTTGSRDRnfVX31537883 = ZkxxTTGSRDRnfVX95289282;     ZkxxTTGSRDRnfVX95289282 = ZkxxTTGSRDRnfVX80586145;     ZkxxTTGSRDRnfVX80586145 = ZkxxTTGSRDRnfVX36111345;     ZkxxTTGSRDRnfVX36111345 = ZkxxTTGSRDRnfVX58809954;     ZkxxTTGSRDRnfVX58809954 = ZkxxTTGSRDRnfVX97942647;     ZkxxTTGSRDRnfVX97942647 = ZkxxTTGSRDRnfVX33361089;     ZkxxTTGSRDRnfVX33361089 = ZkxxTTGSRDRnfVX46761655;     ZkxxTTGSRDRnfVX46761655 = ZkxxTTGSRDRnfVX20477037;     ZkxxTTGSRDRnfVX20477037 = ZkxxTTGSRDRnfVX98271174;     ZkxxTTGSRDRnfVX98271174 = ZkxxTTGSRDRnfVX52006054;     ZkxxTTGSRDRnfVX52006054 = ZkxxTTGSRDRnfVX12654447;     ZkxxTTGSRDRnfVX12654447 = ZkxxTTGSRDRnfVX1751158;     ZkxxTTGSRDRnfVX1751158 = ZkxxTTGSRDRnfVX86413779;     ZkxxTTGSRDRnfVX86413779 = ZkxxTTGSRDRnfVX14472585;     ZkxxTTGSRDRnfVX14472585 = ZkxxTTGSRDRnfVX82991355;     ZkxxTTGSRDRnfVX82991355 = ZkxxTTGSRDRnfVX98468542;     ZkxxTTGSRDRnfVX98468542 = ZkxxTTGSRDRnfVX2141472;     ZkxxTTGSRDRnfVX2141472 = ZkxxTTGSRDRnfVX76341146;     ZkxxTTGSRDRnfVX76341146 = ZkxxTTGSRDRnfVX95993466;     ZkxxTTGSRDRnfVX95993466 = ZkxxTTGSRDRnfVX67242957;     ZkxxTTGSRDRnfVX67242957 = ZkxxTTGSRDRnfVX46667675;     ZkxxTTGSRDRnfVX46667675 = ZkxxTTGSRDRnfVX40473753;     ZkxxTTGSRDRnfVX40473753 = ZkxxTTGSRDRnfVX36572340;     ZkxxTTGSRDRnfVX36572340 = ZkxxTTGSRDRnfVX65101579;     ZkxxTTGSRDRnfVX65101579 = ZkxxTTGSRDRnfVX24239257;     ZkxxTTGSRDRnfVX24239257 = ZkxxTTGSRDRnfVX23861241;     ZkxxTTGSRDRnfVX23861241 = ZkxxTTGSRDRnfVX22132292;     ZkxxTTGSRDRnfVX22132292 = ZkxxTTGSRDRnfVX79146235;     ZkxxTTGSRDRnfVX79146235 = ZkxxTTGSRDRnfVX37713067;     ZkxxTTGSRDRnfVX37713067 = ZkxxTTGSRDRnfVX32369846;     ZkxxTTGSRDRnfVX32369846 = ZkxxTTGSRDRnfVX96148254;     ZkxxTTGSRDRnfVX96148254 = ZkxxTTGSRDRnfVX79806376;     ZkxxTTGSRDRnfVX79806376 = ZkxxTTGSRDRnfVX20094157;     ZkxxTTGSRDRnfVX20094157 = ZkxxTTGSRDRnfVX31840757;     ZkxxTTGSRDRnfVX31840757 = ZkxxTTGSRDRnfVX13018573;     ZkxxTTGSRDRnfVX13018573 = ZkxxTTGSRDRnfVX21863083;     ZkxxTTGSRDRnfVX21863083 = ZkxxTTGSRDRnfVX303803;     ZkxxTTGSRDRnfVX303803 = ZkxxTTGSRDRnfVX10127748;     ZkxxTTGSRDRnfVX10127748 = ZkxxTTGSRDRnfVX68695423;     ZkxxTTGSRDRnfVX68695423 = ZkxxTTGSRDRnfVX11504707;     ZkxxTTGSRDRnfVX11504707 = ZkxxTTGSRDRnfVX27832467;     ZkxxTTGSRDRnfVX27832467 = ZkxxTTGSRDRnfVX91100043;     ZkxxTTGSRDRnfVX91100043 = ZkxxTTGSRDRnfVX7144711;     ZkxxTTGSRDRnfVX7144711 = ZkxxTTGSRDRnfVX66098465;     ZkxxTTGSRDRnfVX66098465 = ZkxxTTGSRDRnfVX69157132;     ZkxxTTGSRDRnfVX69157132 = ZkxxTTGSRDRnfVX41178919;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void gQnKePcRNfOwYwpv44198851() {     float ROPaPYOrBeyAuDn11830587 = -549024861;    float ROPaPYOrBeyAuDn3936522 = -715428422;    float ROPaPYOrBeyAuDn62993422 = -811066130;    float ROPaPYOrBeyAuDn21924771 = -251622206;    float ROPaPYOrBeyAuDn43869341 = -798760135;    float ROPaPYOrBeyAuDn17350122 = -786576771;    float ROPaPYOrBeyAuDn12597795 = -468713528;    float ROPaPYOrBeyAuDn12105228 = -667856244;    float ROPaPYOrBeyAuDn70920671 = 46644045;    float ROPaPYOrBeyAuDn95649386 = -367523062;    float ROPaPYOrBeyAuDn92427162 = -431931099;    float ROPaPYOrBeyAuDn38270422 = -831611929;    float ROPaPYOrBeyAuDn29619807 = -128429629;    float ROPaPYOrBeyAuDn6627473 = -420750538;    float ROPaPYOrBeyAuDn68160604 = -334854509;    float ROPaPYOrBeyAuDn67412331 = -446466814;    float ROPaPYOrBeyAuDn64931999 = -165415519;    float ROPaPYOrBeyAuDn63272711 = -78762525;    float ROPaPYOrBeyAuDn22567044 = -982273331;    float ROPaPYOrBeyAuDn623789 = -927930517;    float ROPaPYOrBeyAuDn13380509 = -547350361;    float ROPaPYOrBeyAuDn7285402 = -433058296;    float ROPaPYOrBeyAuDn56620667 = -967539895;    float ROPaPYOrBeyAuDn80762694 = -576406774;    float ROPaPYOrBeyAuDn87307918 = -3493677;    float ROPaPYOrBeyAuDn55073952 = -18472306;    float ROPaPYOrBeyAuDn2250056 = -383026814;    float ROPaPYOrBeyAuDn55239470 = -321020805;    float ROPaPYOrBeyAuDn66928298 = -364981868;    float ROPaPYOrBeyAuDn65956538 = -24879679;    float ROPaPYOrBeyAuDn27748869 = -137189738;    float ROPaPYOrBeyAuDn82035015 = -961677704;    float ROPaPYOrBeyAuDn73918054 = -171748981;    float ROPaPYOrBeyAuDn28743615 = -608140771;    float ROPaPYOrBeyAuDn86332091 = -549439227;    float ROPaPYOrBeyAuDn62442487 = -972967793;    float ROPaPYOrBeyAuDn80206633 = -476366329;    float ROPaPYOrBeyAuDn21730190 = -852020703;    float ROPaPYOrBeyAuDn45171359 = -525531118;    float ROPaPYOrBeyAuDn39969095 = -76544442;    float ROPaPYOrBeyAuDn34667406 = -856450260;    float ROPaPYOrBeyAuDn40060071 = -570479784;    float ROPaPYOrBeyAuDn54382845 = -805709705;    float ROPaPYOrBeyAuDn24612819 = -555298935;    float ROPaPYOrBeyAuDn77765637 = -668412932;    float ROPaPYOrBeyAuDn5350108 = 98004885;    float ROPaPYOrBeyAuDn26746336 = -793835401;    float ROPaPYOrBeyAuDn37471188 = -621002746;    float ROPaPYOrBeyAuDn73752461 = 54990182;    float ROPaPYOrBeyAuDn5500231 = -179973875;    float ROPaPYOrBeyAuDn62603921 = -984184593;    float ROPaPYOrBeyAuDn66400122 = -877522921;    float ROPaPYOrBeyAuDn12792837 = -474120837;    float ROPaPYOrBeyAuDn61105841 = -448234173;    float ROPaPYOrBeyAuDn30818342 = -711541241;    float ROPaPYOrBeyAuDn4545185 = -15966565;    float ROPaPYOrBeyAuDn47315855 = -747888527;    float ROPaPYOrBeyAuDn82230727 = -134659357;    float ROPaPYOrBeyAuDn34616852 = -148128529;    float ROPaPYOrBeyAuDn88795388 = -680287830;    float ROPaPYOrBeyAuDn15100066 = -303549958;    float ROPaPYOrBeyAuDn57358325 = -47692723;    float ROPaPYOrBeyAuDn45176929 = -202874376;    float ROPaPYOrBeyAuDn4964133 = -928476276;    float ROPaPYOrBeyAuDn67900518 = -130333324;    float ROPaPYOrBeyAuDn10392147 = -470253395;    float ROPaPYOrBeyAuDn64352368 = -559862948;    float ROPaPYOrBeyAuDn876192 = -520288858;    float ROPaPYOrBeyAuDn20295382 = -871311311;    float ROPaPYOrBeyAuDn5718117 = -361886716;    float ROPaPYOrBeyAuDn87205697 = -970100486;    float ROPaPYOrBeyAuDn43201810 = -313394816;    float ROPaPYOrBeyAuDn18101352 = -553231407;    float ROPaPYOrBeyAuDn82597949 = -805728889;    float ROPaPYOrBeyAuDn65956383 = 28519742;    float ROPaPYOrBeyAuDn73320438 = -976870578;    float ROPaPYOrBeyAuDn52902557 = -627348592;    float ROPaPYOrBeyAuDn32007848 = -312240961;    float ROPaPYOrBeyAuDn2997058 = -907993843;    float ROPaPYOrBeyAuDn81957810 = -1498563;    float ROPaPYOrBeyAuDn28327617 = -224636905;    float ROPaPYOrBeyAuDn64778867 = -762024068;    float ROPaPYOrBeyAuDn81487008 = -276010987;    float ROPaPYOrBeyAuDn61428068 = -85007993;    float ROPaPYOrBeyAuDn3352618 = -40695086;    float ROPaPYOrBeyAuDn61348747 = -259666818;    float ROPaPYOrBeyAuDn69242179 = -387556867;    float ROPaPYOrBeyAuDn12812213 = -723514809;    float ROPaPYOrBeyAuDn97925273 = -896599530;    float ROPaPYOrBeyAuDn81786906 = -433472663;    float ROPaPYOrBeyAuDn15126633 = -125079267;    float ROPaPYOrBeyAuDn97975906 = -241706973;    float ROPaPYOrBeyAuDn87113338 = -603892174;    float ROPaPYOrBeyAuDn56375970 = -845243288;    float ROPaPYOrBeyAuDn24869029 = -772994485;    float ROPaPYOrBeyAuDn77309081 = -708757537;    float ROPaPYOrBeyAuDn94883141 = -267605408;    float ROPaPYOrBeyAuDn49418712 = -877233429;    float ROPaPYOrBeyAuDn56712301 = -324965611;    float ROPaPYOrBeyAuDn67373491 = -549024861;     ROPaPYOrBeyAuDn11830587 = ROPaPYOrBeyAuDn3936522;     ROPaPYOrBeyAuDn3936522 = ROPaPYOrBeyAuDn62993422;     ROPaPYOrBeyAuDn62993422 = ROPaPYOrBeyAuDn21924771;     ROPaPYOrBeyAuDn21924771 = ROPaPYOrBeyAuDn43869341;     ROPaPYOrBeyAuDn43869341 = ROPaPYOrBeyAuDn17350122;     ROPaPYOrBeyAuDn17350122 = ROPaPYOrBeyAuDn12597795;     ROPaPYOrBeyAuDn12597795 = ROPaPYOrBeyAuDn12105228;     ROPaPYOrBeyAuDn12105228 = ROPaPYOrBeyAuDn70920671;     ROPaPYOrBeyAuDn70920671 = ROPaPYOrBeyAuDn95649386;     ROPaPYOrBeyAuDn95649386 = ROPaPYOrBeyAuDn92427162;     ROPaPYOrBeyAuDn92427162 = ROPaPYOrBeyAuDn38270422;     ROPaPYOrBeyAuDn38270422 = ROPaPYOrBeyAuDn29619807;     ROPaPYOrBeyAuDn29619807 = ROPaPYOrBeyAuDn6627473;     ROPaPYOrBeyAuDn6627473 = ROPaPYOrBeyAuDn68160604;     ROPaPYOrBeyAuDn68160604 = ROPaPYOrBeyAuDn67412331;     ROPaPYOrBeyAuDn67412331 = ROPaPYOrBeyAuDn64931999;     ROPaPYOrBeyAuDn64931999 = ROPaPYOrBeyAuDn63272711;     ROPaPYOrBeyAuDn63272711 = ROPaPYOrBeyAuDn22567044;     ROPaPYOrBeyAuDn22567044 = ROPaPYOrBeyAuDn623789;     ROPaPYOrBeyAuDn623789 = ROPaPYOrBeyAuDn13380509;     ROPaPYOrBeyAuDn13380509 = ROPaPYOrBeyAuDn7285402;     ROPaPYOrBeyAuDn7285402 = ROPaPYOrBeyAuDn56620667;     ROPaPYOrBeyAuDn56620667 = ROPaPYOrBeyAuDn80762694;     ROPaPYOrBeyAuDn80762694 = ROPaPYOrBeyAuDn87307918;     ROPaPYOrBeyAuDn87307918 = ROPaPYOrBeyAuDn55073952;     ROPaPYOrBeyAuDn55073952 = ROPaPYOrBeyAuDn2250056;     ROPaPYOrBeyAuDn2250056 = ROPaPYOrBeyAuDn55239470;     ROPaPYOrBeyAuDn55239470 = ROPaPYOrBeyAuDn66928298;     ROPaPYOrBeyAuDn66928298 = ROPaPYOrBeyAuDn65956538;     ROPaPYOrBeyAuDn65956538 = ROPaPYOrBeyAuDn27748869;     ROPaPYOrBeyAuDn27748869 = ROPaPYOrBeyAuDn82035015;     ROPaPYOrBeyAuDn82035015 = ROPaPYOrBeyAuDn73918054;     ROPaPYOrBeyAuDn73918054 = ROPaPYOrBeyAuDn28743615;     ROPaPYOrBeyAuDn28743615 = ROPaPYOrBeyAuDn86332091;     ROPaPYOrBeyAuDn86332091 = ROPaPYOrBeyAuDn62442487;     ROPaPYOrBeyAuDn62442487 = ROPaPYOrBeyAuDn80206633;     ROPaPYOrBeyAuDn80206633 = ROPaPYOrBeyAuDn21730190;     ROPaPYOrBeyAuDn21730190 = ROPaPYOrBeyAuDn45171359;     ROPaPYOrBeyAuDn45171359 = ROPaPYOrBeyAuDn39969095;     ROPaPYOrBeyAuDn39969095 = ROPaPYOrBeyAuDn34667406;     ROPaPYOrBeyAuDn34667406 = ROPaPYOrBeyAuDn40060071;     ROPaPYOrBeyAuDn40060071 = ROPaPYOrBeyAuDn54382845;     ROPaPYOrBeyAuDn54382845 = ROPaPYOrBeyAuDn24612819;     ROPaPYOrBeyAuDn24612819 = ROPaPYOrBeyAuDn77765637;     ROPaPYOrBeyAuDn77765637 = ROPaPYOrBeyAuDn5350108;     ROPaPYOrBeyAuDn5350108 = ROPaPYOrBeyAuDn26746336;     ROPaPYOrBeyAuDn26746336 = ROPaPYOrBeyAuDn37471188;     ROPaPYOrBeyAuDn37471188 = ROPaPYOrBeyAuDn73752461;     ROPaPYOrBeyAuDn73752461 = ROPaPYOrBeyAuDn5500231;     ROPaPYOrBeyAuDn5500231 = ROPaPYOrBeyAuDn62603921;     ROPaPYOrBeyAuDn62603921 = ROPaPYOrBeyAuDn66400122;     ROPaPYOrBeyAuDn66400122 = ROPaPYOrBeyAuDn12792837;     ROPaPYOrBeyAuDn12792837 = ROPaPYOrBeyAuDn61105841;     ROPaPYOrBeyAuDn61105841 = ROPaPYOrBeyAuDn30818342;     ROPaPYOrBeyAuDn30818342 = ROPaPYOrBeyAuDn4545185;     ROPaPYOrBeyAuDn4545185 = ROPaPYOrBeyAuDn47315855;     ROPaPYOrBeyAuDn47315855 = ROPaPYOrBeyAuDn82230727;     ROPaPYOrBeyAuDn82230727 = ROPaPYOrBeyAuDn34616852;     ROPaPYOrBeyAuDn34616852 = ROPaPYOrBeyAuDn88795388;     ROPaPYOrBeyAuDn88795388 = ROPaPYOrBeyAuDn15100066;     ROPaPYOrBeyAuDn15100066 = ROPaPYOrBeyAuDn57358325;     ROPaPYOrBeyAuDn57358325 = ROPaPYOrBeyAuDn45176929;     ROPaPYOrBeyAuDn45176929 = ROPaPYOrBeyAuDn4964133;     ROPaPYOrBeyAuDn4964133 = ROPaPYOrBeyAuDn67900518;     ROPaPYOrBeyAuDn67900518 = ROPaPYOrBeyAuDn10392147;     ROPaPYOrBeyAuDn10392147 = ROPaPYOrBeyAuDn64352368;     ROPaPYOrBeyAuDn64352368 = ROPaPYOrBeyAuDn876192;     ROPaPYOrBeyAuDn876192 = ROPaPYOrBeyAuDn20295382;     ROPaPYOrBeyAuDn20295382 = ROPaPYOrBeyAuDn5718117;     ROPaPYOrBeyAuDn5718117 = ROPaPYOrBeyAuDn87205697;     ROPaPYOrBeyAuDn87205697 = ROPaPYOrBeyAuDn43201810;     ROPaPYOrBeyAuDn43201810 = ROPaPYOrBeyAuDn18101352;     ROPaPYOrBeyAuDn18101352 = ROPaPYOrBeyAuDn82597949;     ROPaPYOrBeyAuDn82597949 = ROPaPYOrBeyAuDn65956383;     ROPaPYOrBeyAuDn65956383 = ROPaPYOrBeyAuDn73320438;     ROPaPYOrBeyAuDn73320438 = ROPaPYOrBeyAuDn52902557;     ROPaPYOrBeyAuDn52902557 = ROPaPYOrBeyAuDn32007848;     ROPaPYOrBeyAuDn32007848 = ROPaPYOrBeyAuDn2997058;     ROPaPYOrBeyAuDn2997058 = ROPaPYOrBeyAuDn81957810;     ROPaPYOrBeyAuDn81957810 = ROPaPYOrBeyAuDn28327617;     ROPaPYOrBeyAuDn28327617 = ROPaPYOrBeyAuDn64778867;     ROPaPYOrBeyAuDn64778867 = ROPaPYOrBeyAuDn81487008;     ROPaPYOrBeyAuDn81487008 = ROPaPYOrBeyAuDn61428068;     ROPaPYOrBeyAuDn61428068 = ROPaPYOrBeyAuDn3352618;     ROPaPYOrBeyAuDn3352618 = ROPaPYOrBeyAuDn61348747;     ROPaPYOrBeyAuDn61348747 = ROPaPYOrBeyAuDn69242179;     ROPaPYOrBeyAuDn69242179 = ROPaPYOrBeyAuDn12812213;     ROPaPYOrBeyAuDn12812213 = ROPaPYOrBeyAuDn97925273;     ROPaPYOrBeyAuDn97925273 = ROPaPYOrBeyAuDn81786906;     ROPaPYOrBeyAuDn81786906 = ROPaPYOrBeyAuDn15126633;     ROPaPYOrBeyAuDn15126633 = ROPaPYOrBeyAuDn97975906;     ROPaPYOrBeyAuDn97975906 = ROPaPYOrBeyAuDn87113338;     ROPaPYOrBeyAuDn87113338 = ROPaPYOrBeyAuDn56375970;     ROPaPYOrBeyAuDn56375970 = ROPaPYOrBeyAuDn24869029;     ROPaPYOrBeyAuDn24869029 = ROPaPYOrBeyAuDn77309081;     ROPaPYOrBeyAuDn77309081 = ROPaPYOrBeyAuDn94883141;     ROPaPYOrBeyAuDn94883141 = ROPaPYOrBeyAuDn49418712;     ROPaPYOrBeyAuDn49418712 = ROPaPYOrBeyAuDn56712301;     ROPaPYOrBeyAuDn56712301 = ROPaPYOrBeyAuDn67373491;     ROPaPYOrBeyAuDn67373491 = ROPaPYOrBeyAuDn11830587;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void HJMEbsFhQIxZyWPj81174022() {     float KfhuPIkurcpXCny79556835 = -718425384;    float KfhuPIkurcpXCny79858719 = -127001794;    float KfhuPIkurcpXCny32343266 = -622463975;    float KfhuPIkurcpXCny50576378 = -72468154;    float KfhuPIkurcpXCny76432027 = 14262773;    float KfhuPIkurcpXCny52864274 = -217467124;    float KfhuPIkurcpXCny52200166 = -793344628;    float KfhuPIkurcpXCny27778034 = -342061988;    float KfhuPIkurcpXCny63049555 = -604344694;    float KfhuPIkurcpXCny1420500 = -568269996;    float KfhuPIkurcpXCny71678332 = -862671000;    float KfhuPIkurcpXCny45700335 = -631411505;    float KfhuPIkurcpXCny64663586 = -965265118;    float KfhuPIkurcpXCny10120015 = -581508769;    float KfhuPIkurcpXCny19843917 = -17923461;    float KfhuPIkurcpXCny96465255 = -430770443;    float KfhuPIkurcpXCny70776347 = -888710080;    float KfhuPIkurcpXCny24653073 = -864012374;    float KfhuPIkurcpXCny83056344 = -349546174;    float KfhuPIkurcpXCny33579279 = -347223276;    float KfhuPIkurcpXCny59758652 = -781220898;    float KfhuPIkurcpXCny98144297 = -181372385;    float KfhuPIkurcpXCny60322555 = 84165092;    float KfhuPIkurcpXCny49484221 = -222279337;    float KfhuPIkurcpXCny55538238 = -778833215;    float KfhuPIkurcpXCny18710078 = -366863669;    float KfhuPIkurcpXCny17571133 = -986568740;    float KfhuPIkurcpXCny43305445 = -710390393;    float KfhuPIkurcpXCny46601629 = -975064411;    float KfhuPIkurcpXCny56768179 = -429848951;    float KfhuPIkurcpXCny53746824 = -56726599;    float KfhuPIkurcpXCny93308453 = -494074289;    float KfhuPIkurcpXCny72753271 = -346519075;    float KfhuPIkurcpXCny93759246 = -28828544;    float KfhuPIkurcpXCny10415337 = -654646312;    float KfhuPIkurcpXCny59637004 = -20133301;    float KfhuPIkurcpXCny10883226 = -458541038;    float KfhuPIkurcpXCny32296732 = -840979836;    float KfhuPIkurcpXCny78023438 = -157309952;    float KfhuPIkurcpXCny8041307 = -163891558;    float KfhuPIkurcpXCny95321043 = -235021666;    float KfhuPIkurcpXCny77591464 = 85351550;    float KfhuPIkurcpXCny29216214 = -311865223;    float KfhuPIkurcpXCny74294437 = -635947362;    float KfhuPIkurcpXCny69782893 = -818298212;    float KfhuPIkurcpXCny98544756 = -927479248;    float KfhuPIkurcpXCny50267813 = -726468892;    float KfhuPIkurcpXCny34026134 = 91353280;    float KfhuPIkurcpXCny44527989 = -658760165;    float KfhuPIkurcpXCny77787736 = -768999732;    float KfhuPIkurcpXCny9618968 = -160600376;    float KfhuPIkurcpXCny65416487 = -75421964;    float KfhuPIkurcpXCny34827585 = -298170622;    float KfhuPIkurcpXCny74226900 = -259948348;    float KfhuPIkurcpXCny21415370 = -213235948;    float KfhuPIkurcpXCny81412538 = -437052999;    float KfhuPIkurcpXCny19536164 = -111166886;    float KfhuPIkurcpXCny82859045 = -300184638;    float KfhuPIkurcpXCny95038139 = -293634939;    float KfhuPIkurcpXCny57721950 = -618873558;    float KfhuPIkurcpXCny35293141 = -230898384;    float KfhuPIkurcpXCny8894721 = 17045764;    float KfhuPIkurcpXCny81176404 = -366997577;    float KfhuPIkurcpXCny6281377 = -74495744;    float KfhuPIkurcpXCny47673676 = -411543398;    float KfhuPIkurcpXCny78369878 = -268596712;    float KfhuPIkurcpXCny72947063 = -184892431;    float KfhuPIkurcpXCny70904339 = -836436575;    float KfhuPIkurcpXCny99704677 = -926862458;    float KfhuPIkurcpXCny60206912 = -997790161;    float KfhuPIkurcpXCny85582029 = -972229405;    float KfhuPIkurcpXCny38479616 = 52269756;    float KfhuPIkurcpXCny46629635 = -606702423;    float KfhuPIkurcpXCny75015038 = -85654616;    float KfhuPIkurcpXCny38258236 = -12201610;    float KfhuPIkurcpXCny82167187 = -766572448;    float KfhuPIkurcpXCny68928083 = -869507162;    float KfhuPIkurcpXCny86028118 = -279887547;    float KfhuPIkurcpXCny79701328 = -403981125;    float KfhuPIkurcpXCny56993482 = -851353968;    float KfhuPIkurcpXCny68442264 = -640394777;    float KfhuPIkurcpXCny83544999 = -977922021;    float KfhuPIkurcpXCny98777455 = 48369771;    float KfhuPIkurcpXCny68813893 = -106064680;    float KfhuPIkurcpXCny47149211 = -169248576;    float KfhuPIkurcpXCny88330337 = -981304635;    float KfhuPIkurcpXCny58480869 = -95903668;    float KfhuPIkurcpXCny98526371 = 13429273;    float KfhuPIkurcpXCny72343876 = -815592596;    float KfhuPIkurcpXCny29002799 = -117593313;    float KfhuPIkurcpXCny40100841 = -908966415;    float KfhuPIkurcpXCny28024181 = -58356401;    float KfhuPIkurcpXCny37258592 = -447344898;    float KfhuPIkurcpXCny20301489 = -538436394;    float KfhuPIkurcpXCny72748166 = -932993174;    float KfhuPIkurcpXCny86426322 = -152067431;    float KfhuPIkurcpXCny96415060 = -547650874;    float KfhuPIkurcpXCny22934837 = -137369480;    float KfhuPIkurcpXCny26620762 = -124403964;    float KfhuPIkurcpXCny91413014 = -718425384;     KfhuPIkurcpXCny79556835 = KfhuPIkurcpXCny79858719;     KfhuPIkurcpXCny79858719 = KfhuPIkurcpXCny32343266;     KfhuPIkurcpXCny32343266 = KfhuPIkurcpXCny50576378;     KfhuPIkurcpXCny50576378 = KfhuPIkurcpXCny76432027;     KfhuPIkurcpXCny76432027 = KfhuPIkurcpXCny52864274;     KfhuPIkurcpXCny52864274 = KfhuPIkurcpXCny52200166;     KfhuPIkurcpXCny52200166 = KfhuPIkurcpXCny27778034;     KfhuPIkurcpXCny27778034 = KfhuPIkurcpXCny63049555;     KfhuPIkurcpXCny63049555 = KfhuPIkurcpXCny1420500;     KfhuPIkurcpXCny1420500 = KfhuPIkurcpXCny71678332;     KfhuPIkurcpXCny71678332 = KfhuPIkurcpXCny45700335;     KfhuPIkurcpXCny45700335 = KfhuPIkurcpXCny64663586;     KfhuPIkurcpXCny64663586 = KfhuPIkurcpXCny10120015;     KfhuPIkurcpXCny10120015 = KfhuPIkurcpXCny19843917;     KfhuPIkurcpXCny19843917 = KfhuPIkurcpXCny96465255;     KfhuPIkurcpXCny96465255 = KfhuPIkurcpXCny70776347;     KfhuPIkurcpXCny70776347 = KfhuPIkurcpXCny24653073;     KfhuPIkurcpXCny24653073 = KfhuPIkurcpXCny83056344;     KfhuPIkurcpXCny83056344 = KfhuPIkurcpXCny33579279;     KfhuPIkurcpXCny33579279 = KfhuPIkurcpXCny59758652;     KfhuPIkurcpXCny59758652 = KfhuPIkurcpXCny98144297;     KfhuPIkurcpXCny98144297 = KfhuPIkurcpXCny60322555;     KfhuPIkurcpXCny60322555 = KfhuPIkurcpXCny49484221;     KfhuPIkurcpXCny49484221 = KfhuPIkurcpXCny55538238;     KfhuPIkurcpXCny55538238 = KfhuPIkurcpXCny18710078;     KfhuPIkurcpXCny18710078 = KfhuPIkurcpXCny17571133;     KfhuPIkurcpXCny17571133 = KfhuPIkurcpXCny43305445;     KfhuPIkurcpXCny43305445 = KfhuPIkurcpXCny46601629;     KfhuPIkurcpXCny46601629 = KfhuPIkurcpXCny56768179;     KfhuPIkurcpXCny56768179 = KfhuPIkurcpXCny53746824;     KfhuPIkurcpXCny53746824 = KfhuPIkurcpXCny93308453;     KfhuPIkurcpXCny93308453 = KfhuPIkurcpXCny72753271;     KfhuPIkurcpXCny72753271 = KfhuPIkurcpXCny93759246;     KfhuPIkurcpXCny93759246 = KfhuPIkurcpXCny10415337;     KfhuPIkurcpXCny10415337 = KfhuPIkurcpXCny59637004;     KfhuPIkurcpXCny59637004 = KfhuPIkurcpXCny10883226;     KfhuPIkurcpXCny10883226 = KfhuPIkurcpXCny32296732;     KfhuPIkurcpXCny32296732 = KfhuPIkurcpXCny78023438;     KfhuPIkurcpXCny78023438 = KfhuPIkurcpXCny8041307;     KfhuPIkurcpXCny8041307 = KfhuPIkurcpXCny95321043;     KfhuPIkurcpXCny95321043 = KfhuPIkurcpXCny77591464;     KfhuPIkurcpXCny77591464 = KfhuPIkurcpXCny29216214;     KfhuPIkurcpXCny29216214 = KfhuPIkurcpXCny74294437;     KfhuPIkurcpXCny74294437 = KfhuPIkurcpXCny69782893;     KfhuPIkurcpXCny69782893 = KfhuPIkurcpXCny98544756;     KfhuPIkurcpXCny98544756 = KfhuPIkurcpXCny50267813;     KfhuPIkurcpXCny50267813 = KfhuPIkurcpXCny34026134;     KfhuPIkurcpXCny34026134 = KfhuPIkurcpXCny44527989;     KfhuPIkurcpXCny44527989 = KfhuPIkurcpXCny77787736;     KfhuPIkurcpXCny77787736 = KfhuPIkurcpXCny9618968;     KfhuPIkurcpXCny9618968 = KfhuPIkurcpXCny65416487;     KfhuPIkurcpXCny65416487 = KfhuPIkurcpXCny34827585;     KfhuPIkurcpXCny34827585 = KfhuPIkurcpXCny74226900;     KfhuPIkurcpXCny74226900 = KfhuPIkurcpXCny21415370;     KfhuPIkurcpXCny21415370 = KfhuPIkurcpXCny81412538;     KfhuPIkurcpXCny81412538 = KfhuPIkurcpXCny19536164;     KfhuPIkurcpXCny19536164 = KfhuPIkurcpXCny82859045;     KfhuPIkurcpXCny82859045 = KfhuPIkurcpXCny95038139;     KfhuPIkurcpXCny95038139 = KfhuPIkurcpXCny57721950;     KfhuPIkurcpXCny57721950 = KfhuPIkurcpXCny35293141;     KfhuPIkurcpXCny35293141 = KfhuPIkurcpXCny8894721;     KfhuPIkurcpXCny8894721 = KfhuPIkurcpXCny81176404;     KfhuPIkurcpXCny81176404 = KfhuPIkurcpXCny6281377;     KfhuPIkurcpXCny6281377 = KfhuPIkurcpXCny47673676;     KfhuPIkurcpXCny47673676 = KfhuPIkurcpXCny78369878;     KfhuPIkurcpXCny78369878 = KfhuPIkurcpXCny72947063;     KfhuPIkurcpXCny72947063 = KfhuPIkurcpXCny70904339;     KfhuPIkurcpXCny70904339 = KfhuPIkurcpXCny99704677;     KfhuPIkurcpXCny99704677 = KfhuPIkurcpXCny60206912;     KfhuPIkurcpXCny60206912 = KfhuPIkurcpXCny85582029;     KfhuPIkurcpXCny85582029 = KfhuPIkurcpXCny38479616;     KfhuPIkurcpXCny38479616 = KfhuPIkurcpXCny46629635;     KfhuPIkurcpXCny46629635 = KfhuPIkurcpXCny75015038;     KfhuPIkurcpXCny75015038 = KfhuPIkurcpXCny38258236;     KfhuPIkurcpXCny38258236 = KfhuPIkurcpXCny82167187;     KfhuPIkurcpXCny82167187 = KfhuPIkurcpXCny68928083;     KfhuPIkurcpXCny68928083 = KfhuPIkurcpXCny86028118;     KfhuPIkurcpXCny86028118 = KfhuPIkurcpXCny79701328;     KfhuPIkurcpXCny79701328 = KfhuPIkurcpXCny56993482;     KfhuPIkurcpXCny56993482 = KfhuPIkurcpXCny68442264;     KfhuPIkurcpXCny68442264 = KfhuPIkurcpXCny83544999;     KfhuPIkurcpXCny83544999 = KfhuPIkurcpXCny98777455;     KfhuPIkurcpXCny98777455 = KfhuPIkurcpXCny68813893;     KfhuPIkurcpXCny68813893 = KfhuPIkurcpXCny47149211;     KfhuPIkurcpXCny47149211 = KfhuPIkurcpXCny88330337;     KfhuPIkurcpXCny88330337 = KfhuPIkurcpXCny58480869;     KfhuPIkurcpXCny58480869 = KfhuPIkurcpXCny98526371;     KfhuPIkurcpXCny98526371 = KfhuPIkurcpXCny72343876;     KfhuPIkurcpXCny72343876 = KfhuPIkurcpXCny29002799;     KfhuPIkurcpXCny29002799 = KfhuPIkurcpXCny40100841;     KfhuPIkurcpXCny40100841 = KfhuPIkurcpXCny28024181;     KfhuPIkurcpXCny28024181 = KfhuPIkurcpXCny37258592;     KfhuPIkurcpXCny37258592 = KfhuPIkurcpXCny20301489;     KfhuPIkurcpXCny20301489 = KfhuPIkurcpXCny72748166;     KfhuPIkurcpXCny72748166 = KfhuPIkurcpXCny86426322;     KfhuPIkurcpXCny86426322 = KfhuPIkurcpXCny96415060;     KfhuPIkurcpXCny96415060 = KfhuPIkurcpXCny22934837;     KfhuPIkurcpXCny22934837 = KfhuPIkurcpXCny26620762;     KfhuPIkurcpXCny26620762 = KfhuPIkurcpXCny91413014;     KfhuPIkurcpXCny91413014 = KfhuPIkurcpXCny79556835;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void sZlfMxSmgGwHbZuQ35590236() {     float pxRPQQJLFdNaIVF50208502 = -236574514;    float pxRPQQJLFdNaIVF48532236 = 1354907;    float pxRPQQJLFdNaIVF12312185 = 35241565;    float pxRPQQJLFdNaIVF90507664 = -262606543;    float pxRPQQJLFdNaIVF97882348 = -869143574;    float pxRPQQJLFdNaIVF42864036 = 80951724;    float pxRPQQJLFdNaIVF70579061 = -792120338;    float pxRPQQJLFdNaIVF84620414 = -688879078;    float pxRPQQJLFdNaIVF8914610 = -697692023;    float pxRPQQJLFdNaIVF24386960 = -869365347;    float pxRPQQJLFdNaIVF13927494 = -897572930;    float pxRPQQJLFdNaIVF91246906 = -947175298;    float pxRPQQJLFdNaIVF44490973 = -874802177;    float pxRPQQJLFdNaIVF93967047 = -382536979;    float pxRPQQJLFdNaIVF2563936 = -167673464;    float pxRPQQJLFdNaIVF28993896 = -754030;    float pxRPQQJLFdNaIVF64629174 = -797895210;    float pxRPQQJLFdNaIVF44946567 = -853178570;    float pxRPQQJLFdNaIVF64764126 = 43265884;    float pxRPQQJLFdNaIVF92365988 = -889589581;    float pxRPQQJLFdNaIVF14894407 = -689900038;    float pxRPQQJLFdNaIVF59540062 = -512657690;    float pxRPQQJLFdNaIVF62266364 = -483678053;    float pxRPQQJLFdNaIVF83333758 = -914680244;    float pxRPQQJLFdNaIVF19662627 = -454737903;    float pxRPQQJLFdNaIVF49307658 = -465148114;    float pxRPQQJLFdNaIVF25831919 = -281746516;    float pxRPQQJLFdNaIVF51087670 = -115514161;    float pxRPQQJLFdNaIVF78744116 = -169422628;    float pxRPQQJLFdNaIVF95940274 = -988244022;    float pxRPQQJLFdNaIVF60818820 = -905160284;    float pxRPQQJLFdNaIVF37819915 = -822371452;    float pxRPQQJLFdNaIVF55698632 = -293480684;    float pxRPQQJLFdNaIVF59124221 = -692358228;    float pxRPQQJLFdNaIVF88439573 = -501593092;    float pxRPQQJLFdNaIVF19630264 = -99359849;    float pxRPQQJLFdNaIVF54674712 = -647134246;    float pxRPQQJLFdNaIVF85089220 = -625092944;    float pxRPQQJLFdNaIVF56556727 = -113038588;    float pxRPQQJLFdNaIVF3144605 = -781499921;    float pxRPQQJLFdNaIVF55394327 = -619040544;    float pxRPQQJLFdNaIVF6074456 = -784037520;    float pxRPQQJLFdNaIVF78183173 = -298619275;    float pxRPQQJLFdNaIVF80802737 = -707922333;    float pxRPQQJLFdNaIVF65736951 = -26944607;    float pxRPQQJLFdNaIVF4950591 = -685816402;    float pxRPQQJLFdNaIVF76399016 = -842538692;    float pxRPQQJLFdNaIVF99640344 = -360777015;    float pxRPQQJLFdNaIVF49969441 = -644524134;    float pxRPQQJLFdNaIVF86215222 = 47991390;    float pxRPQQJLFdNaIVF77808292 = -807172497;    float pxRPQQJLFdNaIVF7287992 = -44641686;    float pxRPQQJLFdNaIVF89903241 = -225968664;    float pxRPQQJLFdNaIVF64454204 = -920829845;    float pxRPQQJLFdNaIVF20695829 = -324176862;    float pxRPQQJLFdNaIVF90668440 = -723916824;    float pxRPQQJLFdNaIVF86265872 = -514967040;    float pxRPQQJLFdNaIVF28978427 = -50078191;    float pxRPQQJLFdNaIVF70845038 = -807868640;    float pxRPQQJLFdNaIVF48574691 = -303995461;    float pxRPQQJLFdNaIVF17032118 = -637301761;    float pxRPQQJLFdNaIVF19491392 = -576606178;    float pxRPQQJLFdNaIVF5876298 = -419456450;    float pxRPQQJLFdNaIVF12974336 = -709448002;    float pxRPQQJLFdNaIVF63568140 = -964205063;    float pxRPQQJLFdNaIVF76107578 = 24798522;    float pxRPQQJLFdNaIVF35548274 = -553694615;    float pxRPQQJLFdNaIVF85366751 = -82443949;    float pxRPQQJLFdNaIVF5527475 = -880943888;    float pxRPQQJLFdNaIVF82933672 = 31686384;    float pxRPQQJLFdNaIVF74319183 = -353619784;    float pxRPQQJLFdNaIVF79539953 = -72802267;    float pxRPQQJLFdNaIVF88389840 = -640139982;    float pxRPQQJLFdNaIVF61619521 = -175234196;    float pxRPQQJLFdNaIVF36971661 = -170549037;    float pxRPQQJLFdNaIVF8819951 = -905862519;    float pxRPQQJLFdNaIVF81356888 = -114038416;    float pxRPQQJLFdNaIVF81463626 = -775755720;    float pxRPQQJLFdNaIVF17596807 = -787735637;    float pxRPQQJLFdNaIVF14712036 = -768921501;    float pxRPQQJLFdNaIVF72908642 = -622609423;    float pxRPQQJLFdNaIVF26191575 = -920969501;    float pxRPQQJLFdNaIVF1118229 = -470990027;    float pxRPQQJLFdNaIVF92528894 = -117414019;    float pxRPQQJLFdNaIVF18131983 = -81071525;    float pxRPQQJLFdNaIVF53530829 = -760518599;    float pxRPQQJLFdNaIVF47916673 = -496402789;    float pxRPQQJLFdNaIVF91244427 = -372650840;    float pxRPQQJLFdNaIVF38428393 = -268181366;    float pxRPQQJLFdNaIVF97771133 = -777676268;    float pxRPQQJLFdNaIVF33364391 = -584392809;    float pxRPQQJLFdNaIVF25696285 = -497056055;    float pxRPQQJLFdNaIVF14244183 = -817224304;    float pxRPQQJLFdNaIVF7982037 = -809043128;    float pxRPQQJLFdNaIVF86112487 = -44198161;    float pxRPQQJLFdNaIVF35902936 = 57565633;    float pxRPQQJLFdNaIVF198159 = -264581070;    float pxRPQQJLFdNaIVF65208838 = -589171274;    float pxRPQQJLFdNaIVF17234598 = -743717271;    float pxRPQQJLFdNaIVF89629372 = -236574514;     pxRPQQJLFdNaIVF50208502 = pxRPQQJLFdNaIVF48532236;     pxRPQQJLFdNaIVF48532236 = pxRPQQJLFdNaIVF12312185;     pxRPQQJLFdNaIVF12312185 = pxRPQQJLFdNaIVF90507664;     pxRPQQJLFdNaIVF90507664 = pxRPQQJLFdNaIVF97882348;     pxRPQQJLFdNaIVF97882348 = pxRPQQJLFdNaIVF42864036;     pxRPQQJLFdNaIVF42864036 = pxRPQQJLFdNaIVF70579061;     pxRPQQJLFdNaIVF70579061 = pxRPQQJLFdNaIVF84620414;     pxRPQQJLFdNaIVF84620414 = pxRPQQJLFdNaIVF8914610;     pxRPQQJLFdNaIVF8914610 = pxRPQQJLFdNaIVF24386960;     pxRPQQJLFdNaIVF24386960 = pxRPQQJLFdNaIVF13927494;     pxRPQQJLFdNaIVF13927494 = pxRPQQJLFdNaIVF91246906;     pxRPQQJLFdNaIVF91246906 = pxRPQQJLFdNaIVF44490973;     pxRPQQJLFdNaIVF44490973 = pxRPQQJLFdNaIVF93967047;     pxRPQQJLFdNaIVF93967047 = pxRPQQJLFdNaIVF2563936;     pxRPQQJLFdNaIVF2563936 = pxRPQQJLFdNaIVF28993896;     pxRPQQJLFdNaIVF28993896 = pxRPQQJLFdNaIVF64629174;     pxRPQQJLFdNaIVF64629174 = pxRPQQJLFdNaIVF44946567;     pxRPQQJLFdNaIVF44946567 = pxRPQQJLFdNaIVF64764126;     pxRPQQJLFdNaIVF64764126 = pxRPQQJLFdNaIVF92365988;     pxRPQQJLFdNaIVF92365988 = pxRPQQJLFdNaIVF14894407;     pxRPQQJLFdNaIVF14894407 = pxRPQQJLFdNaIVF59540062;     pxRPQQJLFdNaIVF59540062 = pxRPQQJLFdNaIVF62266364;     pxRPQQJLFdNaIVF62266364 = pxRPQQJLFdNaIVF83333758;     pxRPQQJLFdNaIVF83333758 = pxRPQQJLFdNaIVF19662627;     pxRPQQJLFdNaIVF19662627 = pxRPQQJLFdNaIVF49307658;     pxRPQQJLFdNaIVF49307658 = pxRPQQJLFdNaIVF25831919;     pxRPQQJLFdNaIVF25831919 = pxRPQQJLFdNaIVF51087670;     pxRPQQJLFdNaIVF51087670 = pxRPQQJLFdNaIVF78744116;     pxRPQQJLFdNaIVF78744116 = pxRPQQJLFdNaIVF95940274;     pxRPQQJLFdNaIVF95940274 = pxRPQQJLFdNaIVF60818820;     pxRPQQJLFdNaIVF60818820 = pxRPQQJLFdNaIVF37819915;     pxRPQQJLFdNaIVF37819915 = pxRPQQJLFdNaIVF55698632;     pxRPQQJLFdNaIVF55698632 = pxRPQQJLFdNaIVF59124221;     pxRPQQJLFdNaIVF59124221 = pxRPQQJLFdNaIVF88439573;     pxRPQQJLFdNaIVF88439573 = pxRPQQJLFdNaIVF19630264;     pxRPQQJLFdNaIVF19630264 = pxRPQQJLFdNaIVF54674712;     pxRPQQJLFdNaIVF54674712 = pxRPQQJLFdNaIVF85089220;     pxRPQQJLFdNaIVF85089220 = pxRPQQJLFdNaIVF56556727;     pxRPQQJLFdNaIVF56556727 = pxRPQQJLFdNaIVF3144605;     pxRPQQJLFdNaIVF3144605 = pxRPQQJLFdNaIVF55394327;     pxRPQQJLFdNaIVF55394327 = pxRPQQJLFdNaIVF6074456;     pxRPQQJLFdNaIVF6074456 = pxRPQQJLFdNaIVF78183173;     pxRPQQJLFdNaIVF78183173 = pxRPQQJLFdNaIVF80802737;     pxRPQQJLFdNaIVF80802737 = pxRPQQJLFdNaIVF65736951;     pxRPQQJLFdNaIVF65736951 = pxRPQQJLFdNaIVF4950591;     pxRPQQJLFdNaIVF4950591 = pxRPQQJLFdNaIVF76399016;     pxRPQQJLFdNaIVF76399016 = pxRPQQJLFdNaIVF99640344;     pxRPQQJLFdNaIVF99640344 = pxRPQQJLFdNaIVF49969441;     pxRPQQJLFdNaIVF49969441 = pxRPQQJLFdNaIVF86215222;     pxRPQQJLFdNaIVF86215222 = pxRPQQJLFdNaIVF77808292;     pxRPQQJLFdNaIVF77808292 = pxRPQQJLFdNaIVF7287992;     pxRPQQJLFdNaIVF7287992 = pxRPQQJLFdNaIVF89903241;     pxRPQQJLFdNaIVF89903241 = pxRPQQJLFdNaIVF64454204;     pxRPQQJLFdNaIVF64454204 = pxRPQQJLFdNaIVF20695829;     pxRPQQJLFdNaIVF20695829 = pxRPQQJLFdNaIVF90668440;     pxRPQQJLFdNaIVF90668440 = pxRPQQJLFdNaIVF86265872;     pxRPQQJLFdNaIVF86265872 = pxRPQQJLFdNaIVF28978427;     pxRPQQJLFdNaIVF28978427 = pxRPQQJLFdNaIVF70845038;     pxRPQQJLFdNaIVF70845038 = pxRPQQJLFdNaIVF48574691;     pxRPQQJLFdNaIVF48574691 = pxRPQQJLFdNaIVF17032118;     pxRPQQJLFdNaIVF17032118 = pxRPQQJLFdNaIVF19491392;     pxRPQQJLFdNaIVF19491392 = pxRPQQJLFdNaIVF5876298;     pxRPQQJLFdNaIVF5876298 = pxRPQQJLFdNaIVF12974336;     pxRPQQJLFdNaIVF12974336 = pxRPQQJLFdNaIVF63568140;     pxRPQQJLFdNaIVF63568140 = pxRPQQJLFdNaIVF76107578;     pxRPQQJLFdNaIVF76107578 = pxRPQQJLFdNaIVF35548274;     pxRPQQJLFdNaIVF35548274 = pxRPQQJLFdNaIVF85366751;     pxRPQQJLFdNaIVF85366751 = pxRPQQJLFdNaIVF5527475;     pxRPQQJLFdNaIVF5527475 = pxRPQQJLFdNaIVF82933672;     pxRPQQJLFdNaIVF82933672 = pxRPQQJLFdNaIVF74319183;     pxRPQQJLFdNaIVF74319183 = pxRPQQJLFdNaIVF79539953;     pxRPQQJLFdNaIVF79539953 = pxRPQQJLFdNaIVF88389840;     pxRPQQJLFdNaIVF88389840 = pxRPQQJLFdNaIVF61619521;     pxRPQQJLFdNaIVF61619521 = pxRPQQJLFdNaIVF36971661;     pxRPQQJLFdNaIVF36971661 = pxRPQQJLFdNaIVF8819951;     pxRPQQJLFdNaIVF8819951 = pxRPQQJLFdNaIVF81356888;     pxRPQQJLFdNaIVF81356888 = pxRPQQJLFdNaIVF81463626;     pxRPQQJLFdNaIVF81463626 = pxRPQQJLFdNaIVF17596807;     pxRPQQJLFdNaIVF17596807 = pxRPQQJLFdNaIVF14712036;     pxRPQQJLFdNaIVF14712036 = pxRPQQJLFdNaIVF72908642;     pxRPQQJLFdNaIVF72908642 = pxRPQQJLFdNaIVF26191575;     pxRPQQJLFdNaIVF26191575 = pxRPQQJLFdNaIVF1118229;     pxRPQQJLFdNaIVF1118229 = pxRPQQJLFdNaIVF92528894;     pxRPQQJLFdNaIVF92528894 = pxRPQQJLFdNaIVF18131983;     pxRPQQJLFdNaIVF18131983 = pxRPQQJLFdNaIVF53530829;     pxRPQQJLFdNaIVF53530829 = pxRPQQJLFdNaIVF47916673;     pxRPQQJLFdNaIVF47916673 = pxRPQQJLFdNaIVF91244427;     pxRPQQJLFdNaIVF91244427 = pxRPQQJLFdNaIVF38428393;     pxRPQQJLFdNaIVF38428393 = pxRPQQJLFdNaIVF97771133;     pxRPQQJLFdNaIVF97771133 = pxRPQQJLFdNaIVF33364391;     pxRPQQJLFdNaIVF33364391 = pxRPQQJLFdNaIVF25696285;     pxRPQQJLFdNaIVF25696285 = pxRPQQJLFdNaIVF14244183;     pxRPQQJLFdNaIVF14244183 = pxRPQQJLFdNaIVF7982037;     pxRPQQJLFdNaIVF7982037 = pxRPQQJLFdNaIVF86112487;     pxRPQQJLFdNaIVF86112487 = pxRPQQJLFdNaIVF35902936;     pxRPQQJLFdNaIVF35902936 = pxRPQQJLFdNaIVF198159;     pxRPQQJLFdNaIVF198159 = pxRPQQJLFdNaIVF65208838;     pxRPQQJLFdNaIVF65208838 = pxRPQQJLFdNaIVF17234598;     pxRPQQJLFdNaIVF17234598 = pxRPQQJLFdNaIVF89629372;     pxRPQQJLFdNaIVF89629372 = pxRPQQJLFdNaIVF50208502;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void wqUkQklTHpTvKBCv42248980() {     float oIxjQncjxvzWvfZ50202031 = -109098656;    float oIxjQncjxvzWvfZ87104553 = -41240218;    float oIxjQncjxvzWvfZ13625398 = 78112952;    float oIxjQncjxvzWvfZ75587525 = -83147373;    float oIxjQncjxvzWvfZ6722451 = -237498896;    float oIxjQncjxvzWvfZ77669468 = -718481087;    float oIxjQncjxvzWvfZ39126397 = -496656805;    float oIxjQncjxvzWvfZ53834464 = -393056395;    float oIxjQncjxvzWvfZ91654774 = -411338093;    float oIxjQncjxvzWvfZ21026474 = -506172218;    float oIxjQncjxvzWvfZ34248099 = -857045002;    float oIxjQncjxvzWvfZ5538584 = -682653660;    float oIxjQncjxvzWvfZ76343887 = -896460651;    float oIxjQncjxvzWvfZ47811268 = -727690031;    float oIxjQncjxvzWvfZ47736045 = -130386316;    float oIxjQncjxvzWvfZ3558443 = -333549681;    float oIxjQncjxvzWvfZ20481934 = -617509780;    float oIxjQncjxvzWvfZ67947100 = -150250192;    float oIxjQncjxvzWvfZ85192395 = -574716383;    float oIxjQncjxvzWvfZ64439751 = -401614032;    float oIxjQncjxvzWvfZ94563831 = -400366412;    float oIxjQncjxvzWvfZ37836328 = 77350426;    float oIxjQncjxvzWvfZ99144760 = -942635895;    float oIxjQncjxvzWvfZ26983867 = -795600766;    float oIxjQncjxvzWvfZ48105316 = -881431756;    float oIxjQncjxvzWvfZ65881735 = -373354042;    float oIxjQncjxvzWvfZ96053500 = -582546228;    float oIxjQncjxvzWvfZ86491195 = -724481149;    float oIxjQncjxvzWvfZ91422563 = -815492943;    float oIxjQncjxvzWvfZ72030144 = -663675395;    float oIxjQncjxvzWvfZ94231499 = -192253519;    float oIxjQncjxvzWvfZ22543772 = -114193214;    float oIxjQncjxvzWvfZ41151055 = -495424900;    float oIxjQncjxvzWvfZ59407059 = -569039960;    float oIxjQncjxvzWvfZ70797611 = -760907016;    float oIxjQncjxvzWvfZ62458453 = -179125595;    float oIxjQncjxvzWvfZ66616079 = -746787624;    float oIxjQncjxvzWvfZ55006900 = -620355623;    float oIxjQncjxvzWvfZ91870323 = -825720005;    float oIxjQncjxvzWvfZ75017497 = -390931607;    float oIxjQncjxvzWvfZ43249995 = -706984443;    float oIxjQncjxvzWvfZ91772116 = -977829602;    float oIxjQncjxvzWvfZ38466534 = -857749529;    float oIxjQncjxvzWvfZ40034636 = -264886777;    float oIxjQncjxvzWvfZ49755004 = -500204009;    float oIxjQncjxvzWvfZ45378559 = -497861073;    float oIxjQncjxvzWvfZ4096809 = -223819314;    float oIxjQncjxvzWvfZ75023925 = -969538371;    float oIxjQncjxvzWvfZ68627831 = -208287995;    float oIxjQncjxvzWvfZ6260646 = -639033496;    float oIxjQncjxvzWvfZ91067662 = -385727504;    float oIxjQncjxvzWvfZ21835249 = -426787434;    float oIxjQncjxvzWvfZ65351590 = -118022672;    float oIxjQncjxvzWvfZ74704475 = -352749696;    float oIxjQncjxvzWvfZ47685149 = -81076140;    float oIxjQncjxvzWvfZ12365703 = -86449083;    float oIxjQncjxvzWvfZ87959792 = -98604324;    float oIxjQncjxvzWvfZ86641530 = -126286283;    float oIxjQncjxvzWvfZ27482209 = -201715617;    float oIxjQncjxvzWvfZ40840716 = -864144854;    float oIxjQncjxvzWvfZ81615968 = -35934859;    float oIxjQncjxvzWvfZ52635202 = -772175657;    float oIxjQncjxvzWvfZ62411901 = -577563452;    float oIxjQncjxvzWvfZ19624630 = -747662698;    float oIxjQncjxvzWvfZ26794975 = -213918700;    float oIxjQncjxvzWvfZ11704327 = -642851789;    float oIxjQncjxvzWvfZ64387528 = -87228761;    float oIxjQncjxvzWvfZ16936829 = -227420691;    float oIxjQncjxvzWvfZ77013657 = -966783015;    float oIxjQncjxvzWvfZ85277592 = -951260722;    float oIxjQncjxvzWvfZ36942363 = -586762057;    float oIxjQncjxvzWvfZ65475034 = -997154158;    float oIxjQncjxvzWvfZ76076776 = -324530188;    float oIxjQncjxvzWvfZ10174899 = -83784776;    float oIxjQncjxvzWvfZ21189757 = -694629590;    float oIxjQncjxvzWvfZ2791715 = -422536811;    float oIxjQncjxvzWvfZ99369793 = -64900045;    float oIxjQncjxvzWvfZ59110125 = -577749119;    float oIxjQncjxvzWvfZ77228862 = -195396758;    float oIxjQncjxvzWvfZ2726758 = -283570683;    float oIxjQncjxvzWvfZ61784926 = -49534729;    float oIxjQncjxvzWvfZ21029576 = -613007858;    float oIxjQncjxvzWvfZ17863364 = -416193154;    float oIxjQncjxvzWvfZ85161918 = -76459447;    float oIxjQncjxvzWvfZ80962482 = -177947892;    float oIxjQncjxvzWvfZ72396250 = -765466085;    float oIxjQncjxvzWvfZ57192182 = -996170542;    float oIxjQncjxvzWvfZ66446580 = -42675204;    float oIxjQncjxvzWvfZ11721910 = -387963821;    float oIxjQncjxvzWvfZ58431908 = -574457934;    float oIxjQncjxvzWvfZ74498660 = 19478728;    float oIxjQncjxvzWvfZ79974548 = -520501342;    float oIxjQncjxvzWvfZ27524691 = -318640006;    float oIxjQncjxvzWvfZ51029608 = -961575151;    float oIxjQncjxvzWvfZ93401529 = -254996748;    float oIxjQncjxvzWvfZ90614792 = -934808786;    float oIxjQncjxvzWvfZ29360216 = -300266150;    float oIxjQncjxvzWvfZ18841905 = -10086832;    float oIxjQncjxvzWvfZ13239662 = 49031922;    float oIxjQncjxvzWvfZ38050677 = -109098656;     oIxjQncjxvzWvfZ50202031 = oIxjQncjxvzWvfZ87104553;     oIxjQncjxvzWvfZ87104553 = oIxjQncjxvzWvfZ13625398;     oIxjQncjxvzWvfZ13625398 = oIxjQncjxvzWvfZ75587525;     oIxjQncjxvzWvfZ75587525 = oIxjQncjxvzWvfZ6722451;     oIxjQncjxvzWvfZ6722451 = oIxjQncjxvzWvfZ77669468;     oIxjQncjxvzWvfZ77669468 = oIxjQncjxvzWvfZ39126397;     oIxjQncjxvzWvfZ39126397 = oIxjQncjxvzWvfZ53834464;     oIxjQncjxvzWvfZ53834464 = oIxjQncjxvzWvfZ91654774;     oIxjQncjxvzWvfZ91654774 = oIxjQncjxvzWvfZ21026474;     oIxjQncjxvzWvfZ21026474 = oIxjQncjxvzWvfZ34248099;     oIxjQncjxvzWvfZ34248099 = oIxjQncjxvzWvfZ5538584;     oIxjQncjxvzWvfZ5538584 = oIxjQncjxvzWvfZ76343887;     oIxjQncjxvzWvfZ76343887 = oIxjQncjxvzWvfZ47811268;     oIxjQncjxvzWvfZ47811268 = oIxjQncjxvzWvfZ47736045;     oIxjQncjxvzWvfZ47736045 = oIxjQncjxvzWvfZ3558443;     oIxjQncjxvzWvfZ3558443 = oIxjQncjxvzWvfZ20481934;     oIxjQncjxvzWvfZ20481934 = oIxjQncjxvzWvfZ67947100;     oIxjQncjxvzWvfZ67947100 = oIxjQncjxvzWvfZ85192395;     oIxjQncjxvzWvfZ85192395 = oIxjQncjxvzWvfZ64439751;     oIxjQncjxvzWvfZ64439751 = oIxjQncjxvzWvfZ94563831;     oIxjQncjxvzWvfZ94563831 = oIxjQncjxvzWvfZ37836328;     oIxjQncjxvzWvfZ37836328 = oIxjQncjxvzWvfZ99144760;     oIxjQncjxvzWvfZ99144760 = oIxjQncjxvzWvfZ26983867;     oIxjQncjxvzWvfZ26983867 = oIxjQncjxvzWvfZ48105316;     oIxjQncjxvzWvfZ48105316 = oIxjQncjxvzWvfZ65881735;     oIxjQncjxvzWvfZ65881735 = oIxjQncjxvzWvfZ96053500;     oIxjQncjxvzWvfZ96053500 = oIxjQncjxvzWvfZ86491195;     oIxjQncjxvzWvfZ86491195 = oIxjQncjxvzWvfZ91422563;     oIxjQncjxvzWvfZ91422563 = oIxjQncjxvzWvfZ72030144;     oIxjQncjxvzWvfZ72030144 = oIxjQncjxvzWvfZ94231499;     oIxjQncjxvzWvfZ94231499 = oIxjQncjxvzWvfZ22543772;     oIxjQncjxvzWvfZ22543772 = oIxjQncjxvzWvfZ41151055;     oIxjQncjxvzWvfZ41151055 = oIxjQncjxvzWvfZ59407059;     oIxjQncjxvzWvfZ59407059 = oIxjQncjxvzWvfZ70797611;     oIxjQncjxvzWvfZ70797611 = oIxjQncjxvzWvfZ62458453;     oIxjQncjxvzWvfZ62458453 = oIxjQncjxvzWvfZ66616079;     oIxjQncjxvzWvfZ66616079 = oIxjQncjxvzWvfZ55006900;     oIxjQncjxvzWvfZ55006900 = oIxjQncjxvzWvfZ91870323;     oIxjQncjxvzWvfZ91870323 = oIxjQncjxvzWvfZ75017497;     oIxjQncjxvzWvfZ75017497 = oIxjQncjxvzWvfZ43249995;     oIxjQncjxvzWvfZ43249995 = oIxjQncjxvzWvfZ91772116;     oIxjQncjxvzWvfZ91772116 = oIxjQncjxvzWvfZ38466534;     oIxjQncjxvzWvfZ38466534 = oIxjQncjxvzWvfZ40034636;     oIxjQncjxvzWvfZ40034636 = oIxjQncjxvzWvfZ49755004;     oIxjQncjxvzWvfZ49755004 = oIxjQncjxvzWvfZ45378559;     oIxjQncjxvzWvfZ45378559 = oIxjQncjxvzWvfZ4096809;     oIxjQncjxvzWvfZ4096809 = oIxjQncjxvzWvfZ75023925;     oIxjQncjxvzWvfZ75023925 = oIxjQncjxvzWvfZ68627831;     oIxjQncjxvzWvfZ68627831 = oIxjQncjxvzWvfZ6260646;     oIxjQncjxvzWvfZ6260646 = oIxjQncjxvzWvfZ91067662;     oIxjQncjxvzWvfZ91067662 = oIxjQncjxvzWvfZ21835249;     oIxjQncjxvzWvfZ21835249 = oIxjQncjxvzWvfZ65351590;     oIxjQncjxvzWvfZ65351590 = oIxjQncjxvzWvfZ74704475;     oIxjQncjxvzWvfZ74704475 = oIxjQncjxvzWvfZ47685149;     oIxjQncjxvzWvfZ47685149 = oIxjQncjxvzWvfZ12365703;     oIxjQncjxvzWvfZ12365703 = oIxjQncjxvzWvfZ87959792;     oIxjQncjxvzWvfZ87959792 = oIxjQncjxvzWvfZ86641530;     oIxjQncjxvzWvfZ86641530 = oIxjQncjxvzWvfZ27482209;     oIxjQncjxvzWvfZ27482209 = oIxjQncjxvzWvfZ40840716;     oIxjQncjxvzWvfZ40840716 = oIxjQncjxvzWvfZ81615968;     oIxjQncjxvzWvfZ81615968 = oIxjQncjxvzWvfZ52635202;     oIxjQncjxvzWvfZ52635202 = oIxjQncjxvzWvfZ62411901;     oIxjQncjxvzWvfZ62411901 = oIxjQncjxvzWvfZ19624630;     oIxjQncjxvzWvfZ19624630 = oIxjQncjxvzWvfZ26794975;     oIxjQncjxvzWvfZ26794975 = oIxjQncjxvzWvfZ11704327;     oIxjQncjxvzWvfZ11704327 = oIxjQncjxvzWvfZ64387528;     oIxjQncjxvzWvfZ64387528 = oIxjQncjxvzWvfZ16936829;     oIxjQncjxvzWvfZ16936829 = oIxjQncjxvzWvfZ77013657;     oIxjQncjxvzWvfZ77013657 = oIxjQncjxvzWvfZ85277592;     oIxjQncjxvzWvfZ85277592 = oIxjQncjxvzWvfZ36942363;     oIxjQncjxvzWvfZ36942363 = oIxjQncjxvzWvfZ65475034;     oIxjQncjxvzWvfZ65475034 = oIxjQncjxvzWvfZ76076776;     oIxjQncjxvzWvfZ76076776 = oIxjQncjxvzWvfZ10174899;     oIxjQncjxvzWvfZ10174899 = oIxjQncjxvzWvfZ21189757;     oIxjQncjxvzWvfZ21189757 = oIxjQncjxvzWvfZ2791715;     oIxjQncjxvzWvfZ2791715 = oIxjQncjxvzWvfZ99369793;     oIxjQncjxvzWvfZ99369793 = oIxjQncjxvzWvfZ59110125;     oIxjQncjxvzWvfZ59110125 = oIxjQncjxvzWvfZ77228862;     oIxjQncjxvzWvfZ77228862 = oIxjQncjxvzWvfZ2726758;     oIxjQncjxvzWvfZ2726758 = oIxjQncjxvzWvfZ61784926;     oIxjQncjxvzWvfZ61784926 = oIxjQncjxvzWvfZ21029576;     oIxjQncjxvzWvfZ21029576 = oIxjQncjxvzWvfZ17863364;     oIxjQncjxvzWvfZ17863364 = oIxjQncjxvzWvfZ85161918;     oIxjQncjxvzWvfZ85161918 = oIxjQncjxvzWvfZ80962482;     oIxjQncjxvzWvfZ80962482 = oIxjQncjxvzWvfZ72396250;     oIxjQncjxvzWvfZ72396250 = oIxjQncjxvzWvfZ57192182;     oIxjQncjxvzWvfZ57192182 = oIxjQncjxvzWvfZ66446580;     oIxjQncjxvzWvfZ66446580 = oIxjQncjxvzWvfZ11721910;     oIxjQncjxvzWvfZ11721910 = oIxjQncjxvzWvfZ58431908;     oIxjQncjxvzWvfZ58431908 = oIxjQncjxvzWvfZ74498660;     oIxjQncjxvzWvfZ74498660 = oIxjQncjxvzWvfZ79974548;     oIxjQncjxvzWvfZ79974548 = oIxjQncjxvzWvfZ27524691;     oIxjQncjxvzWvfZ27524691 = oIxjQncjxvzWvfZ51029608;     oIxjQncjxvzWvfZ51029608 = oIxjQncjxvzWvfZ93401529;     oIxjQncjxvzWvfZ93401529 = oIxjQncjxvzWvfZ90614792;     oIxjQncjxvzWvfZ90614792 = oIxjQncjxvzWvfZ29360216;     oIxjQncjxvzWvfZ29360216 = oIxjQncjxvzWvfZ18841905;     oIxjQncjxvzWvfZ18841905 = oIxjQncjxvzWvfZ13239662;     oIxjQncjxvzWvfZ13239662 = oIxjQncjxvzWvfZ38050677;     oIxjQncjxvzWvfZ38050677 = oIxjQncjxvzWvfZ50202031;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void SpSgDENbpQtQBpqW48577528() {     float LpCqVzHOSgLDSdt4856429 = -546626239;    float LpCqVzHOSgLDSdt34102173 = -620805032;    float LpCqVzHOSgLDSdt74358043 = -307271662;    float LpCqVzHOSgLDSdt47022467 = -89249784;    float LpCqVzHOSgLDSdt81174121 = 90065865;    float LpCqVzHOSgLDSdt91843865 = -847631924;    float LpCqVzHOSgLDSdt60227101 = -798549478;    float LpCqVzHOSgLDSdt83009568 = -893624628;    float LpCqVzHOSgLDSdt79429184 = -458191463;    float LpCqVzHOSgLDSdt3658460 = -784973487;    float LpCqVzHOSgLDSdt12859394 = -382401574;    float LpCqVzHOSgLDSdt68303296 = -869077749;    float LpCqVzHOSgLDSdt40161202 = -700000955;    float LpCqVzHOSgLDSdt40777699 = -339793609;    float LpCqVzHOSgLDSdt77960118 = -37507948;    float LpCqVzHOSgLDSdt93325978 = 36290755;    float LpCqVzHOSgLDSdt20313697 = 8890392;    float LpCqVzHOSgLDSdt35543687 = -213814659;    float LpCqVzHOSgLDSdt86412996 = -860527931;    float LpCqVzHOSgLDSdt82074307 = -746980178;    float LpCqVzHOSgLDSdt28738219 = -968449563;    float LpCqVzHOSgLDSdt89088916 = -89093681;    float LpCqVzHOSgLDSdt35614593 = -429379316;    float LpCqVzHOSgLDSdt28412235 = -494641583;    float LpCqVzHOSgLDSdt43857932 = -154345208;    float LpCqVzHOSgLDSdt7122682 = -377062827;    float LpCqVzHOSgLDSdt98043424 = -37390507;    float LpCqVzHOSgLDSdt39740195 = -732533009;    float LpCqVzHOSgLDSdt31320241 = -95737819;    float LpCqVzHOSgLDSdt66465553 = -954433364;    float LpCqVzHOSgLDSdt45937028 = -741126044;    float LpCqVzHOSgLDSdt53535382 = -525689742;    float LpCqVzHOSgLDSdt8806932 = 48057486;    float LpCqVzHOSgLDSdt54062951 = -249160770;    float LpCqVzHOSgLDSdt5301768 = -978770276;    float LpCqVzHOSgLDSdt49784995 = -427121192;    float LpCqVzHOSgLDSdt41320567 = -597214245;    float LpCqVzHOSgLDSdt67984139 = -494284644;    float LpCqVzHOSgLDSdt42639973 = -107668606;    float LpCqVzHOSgLDSdt99003892 = -49240206;    float LpCqVzHOSgLDSdt99209395 = -819534601;    float LpCqVzHOSgLDSdt28446775 = -485361689;    float LpCqVzHOSgLDSdt29466717 = -698254847;    float LpCqVzHOSgLDSdt49029035 = -838566442;    float LpCqVzHOSgLDSdt9739068 = -632721606;    float LpCqVzHOSgLDSdt712161 = -566650688;    float LpCqVzHOSgLDSdt20570521 = -250876698;    float LpCqVzHOSgLDSdt98451234 = -947190744;    float LpCqVzHOSgLDSdt10970598 = -108018184;    float LpCqVzHOSgLDSdt51102308 = -879052791;    float LpCqVzHOSgLDSdt66181202 = -42943006;    float LpCqVzHOSgLDSdt11217399 = -941853417;    float LpCqVzHOSgLDSdt97079592 = -957938129;    float LpCqVzHOSgLDSdt32120233 = -248636180;    float LpCqVzHOSgLDSdt19839309 = -476984821;    float LpCqVzHOSgLDSdt15767512 = -357532559;    float LpCqVzHOSgLDSdt98487580 = -91425717;    float LpCqVzHOSgLDSdt45945808 = -812630080;    float LpCqVzHOSgLDSdt3164535 = -934904577;    float LpCqVzHOSgLDSdt74051439 = -532871309;    float LpCqVzHOSgLDSdt93800441 = -710241417;    float LpCqVzHOSgLDSdt20486906 = 33983531;    float LpCqVzHOSgLDSdt51689327 = -697886810;    float LpCqVzHOSgLDSdt12963631 = -503758100;    float LpCqVzHOSgLDSdt57721431 = 56152557;    float LpCqVzHOSgLDSdt59324011 = -856711833;    float LpCqVzHOSgLDSdt59496364 = -817135236;    float LpCqVzHOSgLDSdt86098250 = -350840186;    float LpCqVzHOSgLDSdt35475931 = -361023334;    float LpCqVzHOSgLDSdt28175124 = -610386757;    float LpCqVzHOSgLDSdt52005411 = -366495001;    float LpCqVzHOSgLDSdt52329558 = -496824965;    float LpCqVzHOSgLDSdt92903714 = -6146053;    float LpCqVzHOSgLDSdt87409104 = -711287725;    float LpCqVzHOSgLDSdt82864911 = -927445578;    float LpCqVzHOSgLDSdt291444 = -383087875;    float LpCqVzHOSgLDSdt59622200 = -390838835;    float LpCqVzHOSgLDSdt86585557 = -590812874;    float LpCqVzHOSgLDSdt18673168 = -861919977;    float LpCqVzHOSgLDSdt43145772 = -587694521;    float LpCqVzHOSgLDSdt86552161 = -26186130;    float LpCqVzHOSgLDSdt99592190 = -90199764;    float LpCqVzHOSgLDSdt28769598 = -524514826;    float LpCqVzHOSgLDSdt80217932 = -216685028;    float LpCqVzHOSgLDSdt284352 = -811490358;    float LpCqVzHOSgLDSdt34719629 = -799272628;    float LpCqVzHOSgLDSdt56455789 = -567751613;    float LpCqVzHOSgLDSdt76686699 = -703306334;    float LpCqVzHOSgLDSdt34223643 = -772175949;    float LpCqVzHOSgLDSdt89534256 = -521237718;    float LpCqVzHOSgLDSdt51297415 = -235695476;    float LpCqVzHOSgLDSdt95374758 = -784584166;    float LpCqVzHOSgLDSdt64819604 = -559380067;    float LpCqVzHOSgLDSdt68588533 = -574797298;    float LpCqVzHOSgLDSdt5203452 = -338998790;    float LpCqVzHOSgLDSdt78722489 = -753518133;    float LpCqVzHOSgLDSdt76757447 = -787474880;    float LpCqVzHOSgLDSdt16503086 = -94496747;    float LpCqVzHOSgLDSdt91307603 = -794718999;    float LpCqVzHOSgLDSdt50415056 = -546626239;     LpCqVzHOSgLDSdt4856429 = LpCqVzHOSgLDSdt34102173;     LpCqVzHOSgLDSdt34102173 = LpCqVzHOSgLDSdt74358043;     LpCqVzHOSgLDSdt74358043 = LpCqVzHOSgLDSdt47022467;     LpCqVzHOSgLDSdt47022467 = LpCqVzHOSgLDSdt81174121;     LpCqVzHOSgLDSdt81174121 = LpCqVzHOSgLDSdt91843865;     LpCqVzHOSgLDSdt91843865 = LpCqVzHOSgLDSdt60227101;     LpCqVzHOSgLDSdt60227101 = LpCqVzHOSgLDSdt83009568;     LpCqVzHOSgLDSdt83009568 = LpCqVzHOSgLDSdt79429184;     LpCqVzHOSgLDSdt79429184 = LpCqVzHOSgLDSdt3658460;     LpCqVzHOSgLDSdt3658460 = LpCqVzHOSgLDSdt12859394;     LpCqVzHOSgLDSdt12859394 = LpCqVzHOSgLDSdt68303296;     LpCqVzHOSgLDSdt68303296 = LpCqVzHOSgLDSdt40161202;     LpCqVzHOSgLDSdt40161202 = LpCqVzHOSgLDSdt40777699;     LpCqVzHOSgLDSdt40777699 = LpCqVzHOSgLDSdt77960118;     LpCqVzHOSgLDSdt77960118 = LpCqVzHOSgLDSdt93325978;     LpCqVzHOSgLDSdt93325978 = LpCqVzHOSgLDSdt20313697;     LpCqVzHOSgLDSdt20313697 = LpCqVzHOSgLDSdt35543687;     LpCqVzHOSgLDSdt35543687 = LpCqVzHOSgLDSdt86412996;     LpCqVzHOSgLDSdt86412996 = LpCqVzHOSgLDSdt82074307;     LpCqVzHOSgLDSdt82074307 = LpCqVzHOSgLDSdt28738219;     LpCqVzHOSgLDSdt28738219 = LpCqVzHOSgLDSdt89088916;     LpCqVzHOSgLDSdt89088916 = LpCqVzHOSgLDSdt35614593;     LpCqVzHOSgLDSdt35614593 = LpCqVzHOSgLDSdt28412235;     LpCqVzHOSgLDSdt28412235 = LpCqVzHOSgLDSdt43857932;     LpCqVzHOSgLDSdt43857932 = LpCqVzHOSgLDSdt7122682;     LpCqVzHOSgLDSdt7122682 = LpCqVzHOSgLDSdt98043424;     LpCqVzHOSgLDSdt98043424 = LpCqVzHOSgLDSdt39740195;     LpCqVzHOSgLDSdt39740195 = LpCqVzHOSgLDSdt31320241;     LpCqVzHOSgLDSdt31320241 = LpCqVzHOSgLDSdt66465553;     LpCqVzHOSgLDSdt66465553 = LpCqVzHOSgLDSdt45937028;     LpCqVzHOSgLDSdt45937028 = LpCqVzHOSgLDSdt53535382;     LpCqVzHOSgLDSdt53535382 = LpCqVzHOSgLDSdt8806932;     LpCqVzHOSgLDSdt8806932 = LpCqVzHOSgLDSdt54062951;     LpCqVzHOSgLDSdt54062951 = LpCqVzHOSgLDSdt5301768;     LpCqVzHOSgLDSdt5301768 = LpCqVzHOSgLDSdt49784995;     LpCqVzHOSgLDSdt49784995 = LpCqVzHOSgLDSdt41320567;     LpCqVzHOSgLDSdt41320567 = LpCqVzHOSgLDSdt67984139;     LpCqVzHOSgLDSdt67984139 = LpCqVzHOSgLDSdt42639973;     LpCqVzHOSgLDSdt42639973 = LpCqVzHOSgLDSdt99003892;     LpCqVzHOSgLDSdt99003892 = LpCqVzHOSgLDSdt99209395;     LpCqVzHOSgLDSdt99209395 = LpCqVzHOSgLDSdt28446775;     LpCqVzHOSgLDSdt28446775 = LpCqVzHOSgLDSdt29466717;     LpCqVzHOSgLDSdt29466717 = LpCqVzHOSgLDSdt49029035;     LpCqVzHOSgLDSdt49029035 = LpCqVzHOSgLDSdt9739068;     LpCqVzHOSgLDSdt9739068 = LpCqVzHOSgLDSdt712161;     LpCqVzHOSgLDSdt712161 = LpCqVzHOSgLDSdt20570521;     LpCqVzHOSgLDSdt20570521 = LpCqVzHOSgLDSdt98451234;     LpCqVzHOSgLDSdt98451234 = LpCqVzHOSgLDSdt10970598;     LpCqVzHOSgLDSdt10970598 = LpCqVzHOSgLDSdt51102308;     LpCqVzHOSgLDSdt51102308 = LpCqVzHOSgLDSdt66181202;     LpCqVzHOSgLDSdt66181202 = LpCqVzHOSgLDSdt11217399;     LpCqVzHOSgLDSdt11217399 = LpCqVzHOSgLDSdt97079592;     LpCqVzHOSgLDSdt97079592 = LpCqVzHOSgLDSdt32120233;     LpCqVzHOSgLDSdt32120233 = LpCqVzHOSgLDSdt19839309;     LpCqVzHOSgLDSdt19839309 = LpCqVzHOSgLDSdt15767512;     LpCqVzHOSgLDSdt15767512 = LpCqVzHOSgLDSdt98487580;     LpCqVzHOSgLDSdt98487580 = LpCqVzHOSgLDSdt45945808;     LpCqVzHOSgLDSdt45945808 = LpCqVzHOSgLDSdt3164535;     LpCqVzHOSgLDSdt3164535 = LpCqVzHOSgLDSdt74051439;     LpCqVzHOSgLDSdt74051439 = LpCqVzHOSgLDSdt93800441;     LpCqVzHOSgLDSdt93800441 = LpCqVzHOSgLDSdt20486906;     LpCqVzHOSgLDSdt20486906 = LpCqVzHOSgLDSdt51689327;     LpCqVzHOSgLDSdt51689327 = LpCqVzHOSgLDSdt12963631;     LpCqVzHOSgLDSdt12963631 = LpCqVzHOSgLDSdt57721431;     LpCqVzHOSgLDSdt57721431 = LpCqVzHOSgLDSdt59324011;     LpCqVzHOSgLDSdt59324011 = LpCqVzHOSgLDSdt59496364;     LpCqVzHOSgLDSdt59496364 = LpCqVzHOSgLDSdt86098250;     LpCqVzHOSgLDSdt86098250 = LpCqVzHOSgLDSdt35475931;     LpCqVzHOSgLDSdt35475931 = LpCqVzHOSgLDSdt28175124;     LpCqVzHOSgLDSdt28175124 = LpCqVzHOSgLDSdt52005411;     LpCqVzHOSgLDSdt52005411 = LpCqVzHOSgLDSdt52329558;     LpCqVzHOSgLDSdt52329558 = LpCqVzHOSgLDSdt92903714;     LpCqVzHOSgLDSdt92903714 = LpCqVzHOSgLDSdt87409104;     LpCqVzHOSgLDSdt87409104 = LpCqVzHOSgLDSdt82864911;     LpCqVzHOSgLDSdt82864911 = LpCqVzHOSgLDSdt291444;     LpCqVzHOSgLDSdt291444 = LpCqVzHOSgLDSdt59622200;     LpCqVzHOSgLDSdt59622200 = LpCqVzHOSgLDSdt86585557;     LpCqVzHOSgLDSdt86585557 = LpCqVzHOSgLDSdt18673168;     LpCqVzHOSgLDSdt18673168 = LpCqVzHOSgLDSdt43145772;     LpCqVzHOSgLDSdt43145772 = LpCqVzHOSgLDSdt86552161;     LpCqVzHOSgLDSdt86552161 = LpCqVzHOSgLDSdt99592190;     LpCqVzHOSgLDSdt99592190 = LpCqVzHOSgLDSdt28769598;     LpCqVzHOSgLDSdt28769598 = LpCqVzHOSgLDSdt80217932;     LpCqVzHOSgLDSdt80217932 = LpCqVzHOSgLDSdt284352;     LpCqVzHOSgLDSdt284352 = LpCqVzHOSgLDSdt34719629;     LpCqVzHOSgLDSdt34719629 = LpCqVzHOSgLDSdt56455789;     LpCqVzHOSgLDSdt56455789 = LpCqVzHOSgLDSdt76686699;     LpCqVzHOSgLDSdt76686699 = LpCqVzHOSgLDSdt34223643;     LpCqVzHOSgLDSdt34223643 = LpCqVzHOSgLDSdt89534256;     LpCqVzHOSgLDSdt89534256 = LpCqVzHOSgLDSdt51297415;     LpCqVzHOSgLDSdt51297415 = LpCqVzHOSgLDSdt95374758;     LpCqVzHOSgLDSdt95374758 = LpCqVzHOSgLDSdt64819604;     LpCqVzHOSgLDSdt64819604 = LpCqVzHOSgLDSdt68588533;     LpCqVzHOSgLDSdt68588533 = LpCqVzHOSgLDSdt5203452;     LpCqVzHOSgLDSdt5203452 = LpCqVzHOSgLDSdt78722489;     LpCqVzHOSgLDSdt78722489 = LpCqVzHOSgLDSdt76757447;     LpCqVzHOSgLDSdt76757447 = LpCqVzHOSgLDSdt16503086;     LpCqVzHOSgLDSdt16503086 = LpCqVzHOSgLDSdt91307603;     LpCqVzHOSgLDSdt91307603 = LpCqVzHOSgLDSdt50415056;     LpCqVzHOSgLDSdt50415056 = LpCqVzHOSgLDSdt4856429;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void UqHGFhBmkaCTHBhN94273220() {     float VHlEbdActuyguWj24045386 = -390401065;    float VHlEbdActuyguWj6400030 = -262413365;    float VHlEbdActuyguWj99017425 = -984117814;    float VHlEbdActuyguWj31313914 = -94741954;    float VHlEbdActuyguWj8180625 = 54874150;    float VHlEbdActuyguWj4600824 = -413867676;    float VHlEbdActuyguWj39217734 = -960252883;    float VHlEbdActuyguWj19267162 = -354136037;    float VHlEbdActuyguWj48426154 = -830359497;    float VHlEbdActuyguWj68027246 = 64105370;    float VHlEbdActuyguWj73609559 = -65222489;    float VHlEbdActuyguWj44791539 = -926859429;    float VHlEbdActuyguWj97596785 = 26812771;    float VHlEbdActuyguWj34447486 = -320686829;    float VHlEbdActuyguWj95161784 = -503917416;    float VHlEbdActuyguWj74116760 = -290852853;    float VHlEbdActuyguWj20162285 = -857349454;    float VHlEbdActuyguWj26380616 = -601022680;    float VHlEbdActuyguWj7511538 = -347758324;    float VHlEbdActuyguWj77945407 = -177809710;    float VHlEbdActuyguWj29495168 = -489724399;    float VHlEbdActuyguWj15216247 = -678893378;    float VHlEbdActuyguWj38437441 = -737448395;    float VHlEbdActuyguWj79697767 = -663778317;    float VHlEbdActuyguWj60035287 = -929967315;    float VHlEbdActuyguWj54239534 = -600400733;    float VHlEbdActuyguWj9834357 = 13249642;    float VHlEbdActuyguWj87664295 = -79779684;    float VHlEbdActuyguWj37228150 = -547958206;    float VHlEbdActuyguWj31457422 = -886115535;    float VHlEbdActuyguWj12472004 = -25111317;    float VHlEbdActuyguWj31427832 = -456036617;    float VHlEbdActuyguWj49697221 = -562808367;    float VHlEbdActuyguWj19253255 = -841269499;    float VHlEbdActuyguWj56355509 = -404847209;    float VHlEbdActuyguWj28378883 = -540317229;    float VHlEbdActuyguWj78554606 = -682598203;    float VHlEbdActuyguWj99663653 = -380820762;    float VHlEbdActuyguWj98332656 = -451422348;    float VHlEbdActuyguWj30591648 = -951717946;    float VHlEbdActuyguWj9572857 = -150829743;    float VHlEbdActuyguWj61453968 = -592140567;    float VHlEbdActuyguWj91366881 = -444709633;    float VHlEbdActuyguWj77123994 = -364878141;    float VHlEbdActuyguWj53724724 = -311987444;    float VHlEbdActuyguWj50512403 = -408561341;    float VHlEbdActuyguWj45396861 = -275228343;    float VHlEbdActuyguWj79535812 = -267077879;    float VHlEbdActuyguWj49079088 = 92224646;    float VHlEbdActuyguWj91459804 = -215070156;    float VHlEbdActuyguWj73783387 = -504436958;    float VHlEbdActuyguWj31661333 = -525412801;    float VHlEbdActuyguWj35634796 = -833862041;    float VHlEbdActuyguWj83794414 = -484934017;    float VHlEbdActuyguWj64778052 = -283302634;    float VHlEbdActuyguWj8829140 = -711507687;    float VHlEbdActuyguWj67962589 = -524964970;    float VHlEbdActuyguWj19319659 = -220339497;    float VHlEbdActuyguWj71278627 = -164774640;    float VHlEbdActuyguWj53941090 = -344725118;    float VHlEbdActuyguWj94766466 = -327117318;    float VHlEbdActuyguWj51553439 = -780473199;    float VHlEbdActuyguWj82039011 = -806177831;    float VHlEbdActuyguWj16968733 = -944243962;    float VHlEbdActuyguWj55555242 = -910783313;    float VHlEbdActuyguWj42181727 = -609185872;    float VHlEbdActuyguWj95094317 = -264051063;    float VHlEbdActuyguWj78343530 = -131917731;    float VHlEbdActuyguWj78091977 = -915839620;    float VHlEbdActuyguWj66782901 = -963600188;    float VHlEbdActuyguWj95562154 = -608254650;    float VHlEbdActuyguWj20498631 = -376528692;    float VHlEbdActuyguWj28047959 = -49600332;    float VHlEbdActuyguWj76919889 = -396040378;    float VHlEbdActuyguWj68372550 = 73020033;    float VHlEbdActuyguWj68041200 = -897583833;    float VHlEbdActuyguWj23849366 = -134183746;    float VHlEbdActuyguWj61313446 = -272570254;    float VHlEbdActuyguWj25973043 = -251790873;    float VHlEbdActuyguWj9522885 = -421405974;    float VHlEbdActuyguWj8842674 = -225172391;    float VHlEbdActuyguWj30298544 = -719672480;    float VHlEbdActuyguWj38585208 = -72004330;    float VHlEbdActuyguWj45768346 = -232888051;    float VHlEbdActuyguWj57674034 = -281678578;    float VHlEbdActuyguWj80810670 = -499698516;    float VHlEbdActuyguWj95793036 = -622174577;    float VHlEbdActuyguWj65902806 = 22125649;    float VHlEbdActuyguWj54475203 = -457966865;    float VHlEbdActuyguWj47526370 = -693339523;    float VHlEbdActuyguWj60416294 = 84647741;    float VHlEbdActuyguWj59234948 = -362258707;    float VHlEbdActuyguWj28385027 = -116046123;    float VHlEbdActuyguWj44391566 = -6697230;    float VHlEbdActuyguWj35825181 = -524600628;    float VHlEbdActuyguWj58019417 = -370356544;    float VHlEbdActuyguWj79414956 = -785962736;    float VHlEbdActuyguWj74398149 = -500465671;    float VHlEbdActuyguWj21568753 = -454094829;    float VHlEbdActuyguWj11542997 = -390401065;     VHlEbdActuyguWj24045386 = VHlEbdActuyguWj6400030;     VHlEbdActuyguWj6400030 = VHlEbdActuyguWj99017425;     VHlEbdActuyguWj99017425 = VHlEbdActuyguWj31313914;     VHlEbdActuyguWj31313914 = VHlEbdActuyguWj8180625;     VHlEbdActuyguWj8180625 = VHlEbdActuyguWj4600824;     VHlEbdActuyguWj4600824 = VHlEbdActuyguWj39217734;     VHlEbdActuyguWj39217734 = VHlEbdActuyguWj19267162;     VHlEbdActuyguWj19267162 = VHlEbdActuyguWj48426154;     VHlEbdActuyguWj48426154 = VHlEbdActuyguWj68027246;     VHlEbdActuyguWj68027246 = VHlEbdActuyguWj73609559;     VHlEbdActuyguWj73609559 = VHlEbdActuyguWj44791539;     VHlEbdActuyguWj44791539 = VHlEbdActuyguWj97596785;     VHlEbdActuyguWj97596785 = VHlEbdActuyguWj34447486;     VHlEbdActuyguWj34447486 = VHlEbdActuyguWj95161784;     VHlEbdActuyguWj95161784 = VHlEbdActuyguWj74116760;     VHlEbdActuyguWj74116760 = VHlEbdActuyguWj20162285;     VHlEbdActuyguWj20162285 = VHlEbdActuyguWj26380616;     VHlEbdActuyguWj26380616 = VHlEbdActuyguWj7511538;     VHlEbdActuyguWj7511538 = VHlEbdActuyguWj77945407;     VHlEbdActuyguWj77945407 = VHlEbdActuyguWj29495168;     VHlEbdActuyguWj29495168 = VHlEbdActuyguWj15216247;     VHlEbdActuyguWj15216247 = VHlEbdActuyguWj38437441;     VHlEbdActuyguWj38437441 = VHlEbdActuyguWj79697767;     VHlEbdActuyguWj79697767 = VHlEbdActuyguWj60035287;     VHlEbdActuyguWj60035287 = VHlEbdActuyguWj54239534;     VHlEbdActuyguWj54239534 = VHlEbdActuyguWj9834357;     VHlEbdActuyguWj9834357 = VHlEbdActuyguWj87664295;     VHlEbdActuyguWj87664295 = VHlEbdActuyguWj37228150;     VHlEbdActuyguWj37228150 = VHlEbdActuyguWj31457422;     VHlEbdActuyguWj31457422 = VHlEbdActuyguWj12472004;     VHlEbdActuyguWj12472004 = VHlEbdActuyguWj31427832;     VHlEbdActuyguWj31427832 = VHlEbdActuyguWj49697221;     VHlEbdActuyguWj49697221 = VHlEbdActuyguWj19253255;     VHlEbdActuyguWj19253255 = VHlEbdActuyguWj56355509;     VHlEbdActuyguWj56355509 = VHlEbdActuyguWj28378883;     VHlEbdActuyguWj28378883 = VHlEbdActuyguWj78554606;     VHlEbdActuyguWj78554606 = VHlEbdActuyguWj99663653;     VHlEbdActuyguWj99663653 = VHlEbdActuyguWj98332656;     VHlEbdActuyguWj98332656 = VHlEbdActuyguWj30591648;     VHlEbdActuyguWj30591648 = VHlEbdActuyguWj9572857;     VHlEbdActuyguWj9572857 = VHlEbdActuyguWj61453968;     VHlEbdActuyguWj61453968 = VHlEbdActuyguWj91366881;     VHlEbdActuyguWj91366881 = VHlEbdActuyguWj77123994;     VHlEbdActuyguWj77123994 = VHlEbdActuyguWj53724724;     VHlEbdActuyguWj53724724 = VHlEbdActuyguWj50512403;     VHlEbdActuyguWj50512403 = VHlEbdActuyguWj45396861;     VHlEbdActuyguWj45396861 = VHlEbdActuyguWj79535812;     VHlEbdActuyguWj79535812 = VHlEbdActuyguWj49079088;     VHlEbdActuyguWj49079088 = VHlEbdActuyguWj91459804;     VHlEbdActuyguWj91459804 = VHlEbdActuyguWj73783387;     VHlEbdActuyguWj73783387 = VHlEbdActuyguWj31661333;     VHlEbdActuyguWj31661333 = VHlEbdActuyguWj35634796;     VHlEbdActuyguWj35634796 = VHlEbdActuyguWj83794414;     VHlEbdActuyguWj83794414 = VHlEbdActuyguWj64778052;     VHlEbdActuyguWj64778052 = VHlEbdActuyguWj8829140;     VHlEbdActuyguWj8829140 = VHlEbdActuyguWj67962589;     VHlEbdActuyguWj67962589 = VHlEbdActuyguWj19319659;     VHlEbdActuyguWj19319659 = VHlEbdActuyguWj71278627;     VHlEbdActuyguWj71278627 = VHlEbdActuyguWj53941090;     VHlEbdActuyguWj53941090 = VHlEbdActuyguWj94766466;     VHlEbdActuyguWj94766466 = VHlEbdActuyguWj51553439;     VHlEbdActuyguWj51553439 = VHlEbdActuyguWj82039011;     VHlEbdActuyguWj82039011 = VHlEbdActuyguWj16968733;     VHlEbdActuyguWj16968733 = VHlEbdActuyguWj55555242;     VHlEbdActuyguWj55555242 = VHlEbdActuyguWj42181727;     VHlEbdActuyguWj42181727 = VHlEbdActuyguWj95094317;     VHlEbdActuyguWj95094317 = VHlEbdActuyguWj78343530;     VHlEbdActuyguWj78343530 = VHlEbdActuyguWj78091977;     VHlEbdActuyguWj78091977 = VHlEbdActuyguWj66782901;     VHlEbdActuyguWj66782901 = VHlEbdActuyguWj95562154;     VHlEbdActuyguWj95562154 = VHlEbdActuyguWj20498631;     VHlEbdActuyguWj20498631 = VHlEbdActuyguWj28047959;     VHlEbdActuyguWj28047959 = VHlEbdActuyguWj76919889;     VHlEbdActuyguWj76919889 = VHlEbdActuyguWj68372550;     VHlEbdActuyguWj68372550 = VHlEbdActuyguWj68041200;     VHlEbdActuyguWj68041200 = VHlEbdActuyguWj23849366;     VHlEbdActuyguWj23849366 = VHlEbdActuyguWj61313446;     VHlEbdActuyguWj61313446 = VHlEbdActuyguWj25973043;     VHlEbdActuyguWj25973043 = VHlEbdActuyguWj9522885;     VHlEbdActuyguWj9522885 = VHlEbdActuyguWj8842674;     VHlEbdActuyguWj8842674 = VHlEbdActuyguWj30298544;     VHlEbdActuyguWj30298544 = VHlEbdActuyguWj38585208;     VHlEbdActuyguWj38585208 = VHlEbdActuyguWj45768346;     VHlEbdActuyguWj45768346 = VHlEbdActuyguWj57674034;     VHlEbdActuyguWj57674034 = VHlEbdActuyguWj80810670;     VHlEbdActuyguWj80810670 = VHlEbdActuyguWj95793036;     VHlEbdActuyguWj95793036 = VHlEbdActuyguWj65902806;     VHlEbdActuyguWj65902806 = VHlEbdActuyguWj54475203;     VHlEbdActuyguWj54475203 = VHlEbdActuyguWj47526370;     VHlEbdActuyguWj47526370 = VHlEbdActuyguWj60416294;     VHlEbdActuyguWj60416294 = VHlEbdActuyguWj59234948;     VHlEbdActuyguWj59234948 = VHlEbdActuyguWj28385027;     VHlEbdActuyguWj28385027 = VHlEbdActuyguWj44391566;     VHlEbdActuyguWj44391566 = VHlEbdActuyguWj35825181;     VHlEbdActuyguWj35825181 = VHlEbdActuyguWj58019417;     VHlEbdActuyguWj58019417 = VHlEbdActuyguWj79414956;     VHlEbdActuyguWj79414956 = VHlEbdActuyguWj74398149;     VHlEbdActuyguWj74398149 = VHlEbdActuyguWj21568753;     VHlEbdActuyguWj21568753 = VHlEbdActuyguWj11542997;     VHlEbdActuyguWj11542997 = VHlEbdActuyguWj24045386;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void pnERFRTdgJxeoVUu48689433() {     float QPoXovDgVWhWJMf94697052 = 91449805;    float QPoXovDgVWhWJMf75073547 = -134056664;    float QPoXovDgVWhWJMf78986344 = -326412274;    float QPoXovDgVWhWJMf71245200 = -284880343;    float QPoXovDgVWhWJMf29630946 = -828532198;    float QPoXovDgVWhWJMf94600585 = -115448827;    float QPoXovDgVWhWJMf57596630 = -959028593;    float QPoXovDgVWhWJMf76109541 = -700953127;    float QPoXovDgVWhWJMf94291208 = -923706825;    float QPoXovDgVWhWJMf90993706 = -236989980;    float QPoXovDgVWhWJMf15858720 = -100124418;    float QPoXovDgVWhWJMf90338109 = -142623222;    float QPoXovDgVWhWJMf77424172 = -982724288;    float QPoXovDgVWhWJMf18294520 = -121715039;    float QPoXovDgVWhWJMf77881803 = -653667419;    float QPoXovDgVWhWJMf6645401 = -960836439;    float QPoXovDgVWhWJMf14015111 = -766534584;    float QPoXovDgVWhWJMf46674110 = -590188876;    float QPoXovDgVWhWJMf89219319 = 45053734;    float QPoXovDgVWhWJMf36732116 = -720176015;    float QPoXovDgVWhWJMf84630922 = -398403539;    float QPoXovDgVWhWJMf76612011 = 89821316;    float QPoXovDgVWhWJMf40381250 = -205291540;    float QPoXovDgVWhWJMf13547304 = -256179224;    float QPoXovDgVWhWJMf24159676 = -605872003;    float QPoXovDgVWhWJMf84837115 = -698685178;    float QPoXovDgVWhWJMf18095143 = -381928133;    float QPoXovDgVWhWJMf95446520 = -584903451;    float QPoXovDgVWhWJMf69370638 = -842316423;    float QPoXovDgVWhWJMf70629517 = -344510606;    float QPoXovDgVWhWJMf19544000 = -873545003;    float QPoXovDgVWhWJMf75939292 = -784333780;    float QPoXovDgVWhWJMf32642582 = -509769975;    float QPoXovDgVWhWJMf84618229 = -404799183;    float QPoXovDgVWhWJMf34379745 = -251793989;    float QPoXovDgVWhWJMf88372141 = -619543777;    float QPoXovDgVWhWJMf22346093 = -871191411;    float QPoXovDgVWhWJMf52456142 = -164933870;    float QPoXovDgVWhWJMf76865945 = -407150984;    float QPoXovDgVWhWJMf25694946 = -469326308;    float QPoXovDgVWhWJMf69646140 = -534848622;    float QPoXovDgVWhWJMf89936958 = -361529636;    float QPoXovDgVWhWJMf40333841 = -431463685;    float QPoXovDgVWhWJMf83632295 = -436853113;    float QPoXovDgVWhWJMf49678783 = -620633839;    float QPoXovDgVWhWJMf56918237 = -166898496;    float QPoXovDgVWhWJMf71528064 = -391298142;    float QPoXovDgVWhWJMf45150023 = -719208174;    float QPoXovDgVWhWJMf54520540 = -993539323;    float QPoXovDgVWhWJMf99887290 = -498079034;    float QPoXovDgVWhWJMf41972712 = -51009079;    float QPoXovDgVWhWJMf73532838 = -494632523;    float QPoXovDgVWhWJMf90710452 = -761660083;    float QPoXovDgVWhWJMf74021719 = -45815513;    float QPoXovDgVWhWJMf64058511 = -394243549;    float QPoXovDgVWhWJMf18085042 = -998371512;    float QPoXovDgVWhWJMf34692298 = -928765125;    float QPoXovDgVWhWJMf65439040 = 29766949;    float QPoXovDgVWhWJMf47085525 = -679008341;    float QPoXovDgVWhWJMf44793831 = -29847020;    float QPoXovDgVWhWJMf76505443 = -733520694;    float QPoXovDgVWhWJMf62150109 = -274125142;    float QPoXovDgVWhWJMf6738904 = -858636704;    float QPoXovDgVWhWJMf23661691 = -479196220;    float QPoXovDgVWhWJMf71449706 = -363444978;    float QPoXovDgVWhWJMf39919427 = -315790639;    float QPoXovDgVWhWJMf57695528 = -632853247;    float QPoXovDgVWhWJMf92805942 = -477925105;    float QPoXovDgVWhWJMf83914774 = -869921051;    float QPoXovDgVWhWJMf89509661 = 65876357;    float QPoXovDgVWhWJMf84299308 = 10354971;    float QPoXovDgVWhWJMf61558969 = -501600714;    float QPoXovDgVWhWJMf69808164 = -83037892;    float QPoXovDgVWhWJMf63524373 = -485619958;    float QPoXovDgVWhWJMf67085976 = -85327394;    float QPoXovDgVWhWJMf94693963 = 63126096;    float QPoXovDgVWhWJMf36278170 = -478714999;    float QPoXovDgVWhWJMf56748954 = -768438427;    float QPoXovDgVWhWJMf63868521 = -635545385;    float QPoXovDgVWhWJMf67241438 = -338973507;    float QPoXovDgVWhWJMf13309051 = -207387036;    float QPoXovDgVWhWJMf72945119 = -662719960;    float QPoXovDgVWhWJMf40925981 = -591364129;    float QPoXovDgVWhWJMf69483347 = -244237390;    float QPoXovDgVWhWJMf28656806 = -193501527;    float QPoXovDgVWhWJMf46011162 = -278912480;    float QPoXovDgVWhWJMf85228839 = 77326302;    float QPoXovDgVWhWJMf58620863 = -363954463;    float QPoXovDgVWhWJMf20559719 = 89444365;    float QPoXovDgVWhWJMf16294704 = -253422478;    float QPoXovDgVWhWJMf53679844 = -690778652;    float QPoXovDgVWhWJMf56907052 = -800958361;    float QPoXovDgVWhWJMf5370618 = -485925529;    float QPoXovDgVWhWJMf32072115 = -277303965;    float QPoXovDgVWhWJMf49189502 = -735805614;    float QPoXovDgVWhWJMf7496031 = -160723480;    float QPoXovDgVWhWJMf83198054 = -502892933;    float QPoXovDgVWhWJMf16672151 = -952267465;    float QPoXovDgVWhWJMf12182589 = 26591864;    float QPoXovDgVWhWJMf9759356 = 91449805;     QPoXovDgVWhWJMf94697052 = QPoXovDgVWhWJMf75073547;     QPoXovDgVWhWJMf75073547 = QPoXovDgVWhWJMf78986344;     QPoXovDgVWhWJMf78986344 = QPoXovDgVWhWJMf71245200;     QPoXovDgVWhWJMf71245200 = QPoXovDgVWhWJMf29630946;     QPoXovDgVWhWJMf29630946 = QPoXovDgVWhWJMf94600585;     QPoXovDgVWhWJMf94600585 = QPoXovDgVWhWJMf57596630;     QPoXovDgVWhWJMf57596630 = QPoXovDgVWhWJMf76109541;     QPoXovDgVWhWJMf76109541 = QPoXovDgVWhWJMf94291208;     QPoXovDgVWhWJMf94291208 = QPoXovDgVWhWJMf90993706;     QPoXovDgVWhWJMf90993706 = QPoXovDgVWhWJMf15858720;     QPoXovDgVWhWJMf15858720 = QPoXovDgVWhWJMf90338109;     QPoXovDgVWhWJMf90338109 = QPoXovDgVWhWJMf77424172;     QPoXovDgVWhWJMf77424172 = QPoXovDgVWhWJMf18294520;     QPoXovDgVWhWJMf18294520 = QPoXovDgVWhWJMf77881803;     QPoXovDgVWhWJMf77881803 = QPoXovDgVWhWJMf6645401;     QPoXovDgVWhWJMf6645401 = QPoXovDgVWhWJMf14015111;     QPoXovDgVWhWJMf14015111 = QPoXovDgVWhWJMf46674110;     QPoXovDgVWhWJMf46674110 = QPoXovDgVWhWJMf89219319;     QPoXovDgVWhWJMf89219319 = QPoXovDgVWhWJMf36732116;     QPoXovDgVWhWJMf36732116 = QPoXovDgVWhWJMf84630922;     QPoXovDgVWhWJMf84630922 = QPoXovDgVWhWJMf76612011;     QPoXovDgVWhWJMf76612011 = QPoXovDgVWhWJMf40381250;     QPoXovDgVWhWJMf40381250 = QPoXovDgVWhWJMf13547304;     QPoXovDgVWhWJMf13547304 = QPoXovDgVWhWJMf24159676;     QPoXovDgVWhWJMf24159676 = QPoXovDgVWhWJMf84837115;     QPoXovDgVWhWJMf84837115 = QPoXovDgVWhWJMf18095143;     QPoXovDgVWhWJMf18095143 = QPoXovDgVWhWJMf95446520;     QPoXovDgVWhWJMf95446520 = QPoXovDgVWhWJMf69370638;     QPoXovDgVWhWJMf69370638 = QPoXovDgVWhWJMf70629517;     QPoXovDgVWhWJMf70629517 = QPoXovDgVWhWJMf19544000;     QPoXovDgVWhWJMf19544000 = QPoXovDgVWhWJMf75939292;     QPoXovDgVWhWJMf75939292 = QPoXovDgVWhWJMf32642582;     QPoXovDgVWhWJMf32642582 = QPoXovDgVWhWJMf84618229;     QPoXovDgVWhWJMf84618229 = QPoXovDgVWhWJMf34379745;     QPoXovDgVWhWJMf34379745 = QPoXovDgVWhWJMf88372141;     QPoXovDgVWhWJMf88372141 = QPoXovDgVWhWJMf22346093;     QPoXovDgVWhWJMf22346093 = QPoXovDgVWhWJMf52456142;     QPoXovDgVWhWJMf52456142 = QPoXovDgVWhWJMf76865945;     QPoXovDgVWhWJMf76865945 = QPoXovDgVWhWJMf25694946;     QPoXovDgVWhWJMf25694946 = QPoXovDgVWhWJMf69646140;     QPoXovDgVWhWJMf69646140 = QPoXovDgVWhWJMf89936958;     QPoXovDgVWhWJMf89936958 = QPoXovDgVWhWJMf40333841;     QPoXovDgVWhWJMf40333841 = QPoXovDgVWhWJMf83632295;     QPoXovDgVWhWJMf83632295 = QPoXovDgVWhWJMf49678783;     QPoXovDgVWhWJMf49678783 = QPoXovDgVWhWJMf56918237;     QPoXovDgVWhWJMf56918237 = QPoXovDgVWhWJMf71528064;     QPoXovDgVWhWJMf71528064 = QPoXovDgVWhWJMf45150023;     QPoXovDgVWhWJMf45150023 = QPoXovDgVWhWJMf54520540;     QPoXovDgVWhWJMf54520540 = QPoXovDgVWhWJMf99887290;     QPoXovDgVWhWJMf99887290 = QPoXovDgVWhWJMf41972712;     QPoXovDgVWhWJMf41972712 = QPoXovDgVWhWJMf73532838;     QPoXovDgVWhWJMf73532838 = QPoXovDgVWhWJMf90710452;     QPoXovDgVWhWJMf90710452 = QPoXovDgVWhWJMf74021719;     QPoXovDgVWhWJMf74021719 = QPoXovDgVWhWJMf64058511;     QPoXovDgVWhWJMf64058511 = QPoXovDgVWhWJMf18085042;     QPoXovDgVWhWJMf18085042 = QPoXovDgVWhWJMf34692298;     QPoXovDgVWhWJMf34692298 = QPoXovDgVWhWJMf65439040;     QPoXovDgVWhWJMf65439040 = QPoXovDgVWhWJMf47085525;     QPoXovDgVWhWJMf47085525 = QPoXovDgVWhWJMf44793831;     QPoXovDgVWhWJMf44793831 = QPoXovDgVWhWJMf76505443;     QPoXovDgVWhWJMf76505443 = QPoXovDgVWhWJMf62150109;     QPoXovDgVWhWJMf62150109 = QPoXovDgVWhWJMf6738904;     QPoXovDgVWhWJMf6738904 = QPoXovDgVWhWJMf23661691;     QPoXovDgVWhWJMf23661691 = QPoXovDgVWhWJMf71449706;     QPoXovDgVWhWJMf71449706 = QPoXovDgVWhWJMf39919427;     QPoXovDgVWhWJMf39919427 = QPoXovDgVWhWJMf57695528;     QPoXovDgVWhWJMf57695528 = QPoXovDgVWhWJMf92805942;     QPoXovDgVWhWJMf92805942 = QPoXovDgVWhWJMf83914774;     QPoXovDgVWhWJMf83914774 = QPoXovDgVWhWJMf89509661;     QPoXovDgVWhWJMf89509661 = QPoXovDgVWhWJMf84299308;     QPoXovDgVWhWJMf84299308 = QPoXovDgVWhWJMf61558969;     QPoXovDgVWhWJMf61558969 = QPoXovDgVWhWJMf69808164;     QPoXovDgVWhWJMf69808164 = QPoXovDgVWhWJMf63524373;     QPoXovDgVWhWJMf63524373 = QPoXovDgVWhWJMf67085976;     QPoXovDgVWhWJMf67085976 = QPoXovDgVWhWJMf94693963;     QPoXovDgVWhWJMf94693963 = QPoXovDgVWhWJMf36278170;     QPoXovDgVWhWJMf36278170 = QPoXovDgVWhWJMf56748954;     QPoXovDgVWhWJMf56748954 = QPoXovDgVWhWJMf63868521;     QPoXovDgVWhWJMf63868521 = QPoXovDgVWhWJMf67241438;     QPoXovDgVWhWJMf67241438 = QPoXovDgVWhWJMf13309051;     QPoXovDgVWhWJMf13309051 = QPoXovDgVWhWJMf72945119;     QPoXovDgVWhWJMf72945119 = QPoXovDgVWhWJMf40925981;     QPoXovDgVWhWJMf40925981 = QPoXovDgVWhWJMf69483347;     QPoXovDgVWhWJMf69483347 = QPoXovDgVWhWJMf28656806;     QPoXovDgVWhWJMf28656806 = QPoXovDgVWhWJMf46011162;     QPoXovDgVWhWJMf46011162 = QPoXovDgVWhWJMf85228839;     QPoXovDgVWhWJMf85228839 = QPoXovDgVWhWJMf58620863;     QPoXovDgVWhWJMf58620863 = QPoXovDgVWhWJMf20559719;     QPoXovDgVWhWJMf20559719 = QPoXovDgVWhWJMf16294704;     QPoXovDgVWhWJMf16294704 = QPoXovDgVWhWJMf53679844;     QPoXovDgVWhWJMf53679844 = QPoXovDgVWhWJMf56907052;     QPoXovDgVWhWJMf56907052 = QPoXovDgVWhWJMf5370618;     QPoXovDgVWhWJMf5370618 = QPoXovDgVWhWJMf32072115;     QPoXovDgVWhWJMf32072115 = QPoXovDgVWhWJMf49189502;     QPoXovDgVWhWJMf49189502 = QPoXovDgVWhWJMf7496031;     QPoXovDgVWhWJMf7496031 = QPoXovDgVWhWJMf83198054;     QPoXovDgVWhWJMf83198054 = QPoXovDgVWhWJMf16672151;     QPoXovDgVWhWJMf16672151 = QPoXovDgVWhWJMf12182589;     QPoXovDgVWhWJMf12182589 = QPoXovDgVWhWJMf9759356;     QPoXovDgVWhWJMf9759356 = QPoXovDgVWhWJMf94697052;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void qZFfDecfhcHKQuNO55348178() {     float FtoGopPupAgIhjc94690581 = -881074337;    float FtoGopPupAgIhjc13645864 = -176651789;    float FtoGopPupAgIhjc80299556 = -283540887;    float FtoGopPupAgIhjc56325061 = -105421174;    float FtoGopPupAgIhjc38471048 = -196887519;    float FtoGopPupAgIhjc29406018 = -914881639;    float FtoGopPupAgIhjc26143966 = -663565060;    float FtoGopPupAgIhjc45323592 = -405130444;    float FtoGopPupAgIhjc77031372 = -637352895;    float FtoGopPupAgIhjc87633220 = -973796851;    float FtoGopPupAgIhjc36179325 = -59596490;    float FtoGopPupAgIhjc4629787 = -978101585;    float FtoGopPupAgIhjc9277087 = 95617238;    float FtoGopPupAgIhjc72138740 = -466868091;    float FtoGopPupAgIhjc23053913 = -616380271;    float FtoGopPupAgIhjc81209948 = -193632090;    float FtoGopPupAgIhjc69867870 = -586149154;    float FtoGopPupAgIhjc69674642 = -987260497;    float FtoGopPupAgIhjc9647589 = -572928532;    float FtoGopPupAgIhjc8805879 = -232200466;    float FtoGopPupAgIhjc64300346 = -108869913;    float FtoGopPupAgIhjc54908277 = -420170567;    float FtoGopPupAgIhjc77259646 = -664249382;    float FtoGopPupAgIhjc57197412 = -137099746;    float FtoGopPupAgIhjc52602365 = 67434144;    float FtoGopPupAgIhjc1411193 = -606891106;    float FtoGopPupAgIhjc88316724 = -682727846;    float FtoGopPupAgIhjc30850046 = -93870440;    float FtoGopPupAgIhjc82049084 = -388386738;    float FtoGopPupAgIhjc46719388 = -19941979;    float FtoGopPupAgIhjc52956679 = -160638237;    float FtoGopPupAgIhjc60663150 = -76155542;    float FtoGopPupAgIhjc18095005 = -711714191;    float FtoGopPupAgIhjc84901067 = -281480915;    float FtoGopPupAgIhjc16737784 = -511107913;    float FtoGopPupAgIhjc31200332 = -699309523;    float FtoGopPupAgIhjc34287460 = -970844789;    float FtoGopPupAgIhjc22373822 = -160196549;    float FtoGopPupAgIhjc12179543 = -19832401;    float FtoGopPupAgIhjc97567838 = -78757995;    float FtoGopPupAgIhjc57501808 = -622792520;    float FtoGopPupAgIhjc75634619 = -555321718;    float FtoGopPupAgIhjc617202 = -990593939;    float FtoGopPupAgIhjc42864193 = 6182444;    float FtoGopPupAgIhjc33696835 = 6106759;    float FtoGopPupAgIhjc97346205 = 21056833;    float FtoGopPupAgIhjc99225856 = -872578764;    float FtoGopPupAgIhjc20533604 = -227969530;    float FtoGopPupAgIhjc73178929 = -557303185;    float FtoGopPupAgIhjc19932714 = -85103921;    float FtoGopPupAgIhjc55232081 = -729564086;    float FtoGopPupAgIhjc88080095 = -876778272;    float FtoGopPupAgIhjc66158801 = -653714091;    float FtoGopPupAgIhjc84271989 = -577735365;    float FtoGopPupAgIhjc91047831 = -151142826;    float FtoGopPupAgIhjc39782304 = -360903770;    float FtoGopPupAgIhjc36386218 = -512402408;    float FtoGopPupAgIhjc23102144 = -46441142;    float FtoGopPupAgIhjc3722697 = -72855318;    float FtoGopPupAgIhjc37059856 = -589996413;    float FtoGopPupAgIhjc41089294 = -132153793;    float FtoGopPupAgIhjc95293920 = -469694620;    float FtoGopPupAgIhjc63274508 = 83256294;    float FtoGopPupAgIhjc30311985 = -517410916;    float FtoGopPupAgIhjc34676542 = -713158614;    float FtoGopPupAgIhjc75516175 = -983440949;    float FtoGopPupAgIhjc86534781 = -166387394;    float FtoGopPupAgIhjc24376020 = -622901847;    float FtoGopPupAgIhjc55400957 = -955760178;    float FtoGopPupAgIhjc91853581 = -917070749;    float FtoGopPupAgIhjc46922488 = -222787302;    float FtoGopPupAgIhjc47494049 = -325952605;    float FtoGopPupAgIhjc57495100 = -867428097;    float FtoGopPupAgIhjc12079751 = -394170538;    float FtoGopPupAgIhjc51304071 = -609407946;    float FtoGopPupAgIhjc88665726 = -553548195;    float FtoGopPupAgIhjc54291076 = -429576629;    float FtoGopPupAgIhjc34395453 = -570431826;    float FtoGopPupAgIhjc23500577 = -43206506;    float FtoGopPupAgIhjc55256159 = -953622689;    float FtoGopPupAgIhjc2185336 = -734312342;    float FtoGopPupAgIhjc67783120 = -354758317;    float FtoGopPupAgIhjc57671116 = -536567256;    float FtoGopPupAgIhjc62116371 = -203282818;    float FtoGopPupAgIhjc91487306 = -290377894;    float FtoGopPupAgIhjc64876584 = -283859966;    float FtoGopPupAgIhjc94504349 = -422441451;    float FtoGopPupAgIhjc33823015 = -33978827;    float FtoGopPupAgIhjc93853235 = -30338090;    float FtoGopPupAgIhjc76955479 = -50204144;    float FtoGopPupAgIhjc94814113 = -86907115;    float FtoGopPupAgIhjc11185316 = -824403648;    float FtoGopPupAgIhjc18651126 = 12658769;    float FtoGopPupAgIhjc75119686 = -429835988;    float FtoGopPupAgIhjc56478545 = -946604202;    float FtoGopPupAgIhjc62207887 = -53097900;    float FtoGopPupAgIhjc12360112 = -538578013;    float FtoGopPupAgIhjc70305216 = -373183023;    float FtoGopPupAgIhjc8187652 = -280658943;    float FtoGopPupAgIhjc58180660 = -881074337;     FtoGopPupAgIhjc94690581 = FtoGopPupAgIhjc13645864;     FtoGopPupAgIhjc13645864 = FtoGopPupAgIhjc80299556;     FtoGopPupAgIhjc80299556 = FtoGopPupAgIhjc56325061;     FtoGopPupAgIhjc56325061 = FtoGopPupAgIhjc38471048;     FtoGopPupAgIhjc38471048 = FtoGopPupAgIhjc29406018;     FtoGopPupAgIhjc29406018 = FtoGopPupAgIhjc26143966;     FtoGopPupAgIhjc26143966 = FtoGopPupAgIhjc45323592;     FtoGopPupAgIhjc45323592 = FtoGopPupAgIhjc77031372;     FtoGopPupAgIhjc77031372 = FtoGopPupAgIhjc87633220;     FtoGopPupAgIhjc87633220 = FtoGopPupAgIhjc36179325;     FtoGopPupAgIhjc36179325 = FtoGopPupAgIhjc4629787;     FtoGopPupAgIhjc4629787 = FtoGopPupAgIhjc9277087;     FtoGopPupAgIhjc9277087 = FtoGopPupAgIhjc72138740;     FtoGopPupAgIhjc72138740 = FtoGopPupAgIhjc23053913;     FtoGopPupAgIhjc23053913 = FtoGopPupAgIhjc81209948;     FtoGopPupAgIhjc81209948 = FtoGopPupAgIhjc69867870;     FtoGopPupAgIhjc69867870 = FtoGopPupAgIhjc69674642;     FtoGopPupAgIhjc69674642 = FtoGopPupAgIhjc9647589;     FtoGopPupAgIhjc9647589 = FtoGopPupAgIhjc8805879;     FtoGopPupAgIhjc8805879 = FtoGopPupAgIhjc64300346;     FtoGopPupAgIhjc64300346 = FtoGopPupAgIhjc54908277;     FtoGopPupAgIhjc54908277 = FtoGopPupAgIhjc77259646;     FtoGopPupAgIhjc77259646 = FtoGopPupAgIhjc57197412;     FtoGopPupAgIhjc57197412 = FtoGopPupAgIhjc52602365;     FtoGopPupAgIhjc52602365 = FtoGopPupAgIhjc1411193;     FtoGopPupAgIhjc1411193 = FtoGopPupAgIhjc88316724;     FtoGopPupAgIhjc88316724 = FtoGopPupAgIhjc30850046;     FtoGopPupAgIhjc30850046 = FtoGopPupAgIhjc82049084;     FtoGopPupAgIhjc82049084 = FtoGopPupAgIhjc46719388;     FtoGopPupAgIhjc46719388 = FtoGopPupAgIhjc52956679;     FtoGopPupAgIhjc52956679 = FtoGopPupAgIhjc60663150;     FtoGopPupAgIhjc60663150 = FtoGopPupAgIhjc18095005;     FtoGopPupAgIhjc18095005 = FtoGopPupAgIhjc84901067;     FtoGopPupAgIhjc84901067 = FtoGopPupAgIhjc16737784;     FtoGopPupAgIhjc16737784 = FtoGopPupAgIhjc31200332;     FtoGopPupAgIhjc31200332 = FtoGopPupAgIhjc34287460;     FtoGopPupAgIhjc34287460 = FtoGopPupAgIhjc22373822;     FtoGopPupAgIhjc22373822 = FtoGopPupAgIhjc12179543;     FtoGopPupAgIhjc12179543 = FtoGopPupAgIhjc97567838;     FtoGopPupAgIhjc97567838 = FtoGopPupAgIhjc57501808;     FtoGopPupAgIhjc57501808 = FtoGopPupAgIhjc75634619;     FtoGopPupAgIhjc75634619 = FtoGopPupAgIhjc617202;     FtoGopPupAgIhjc617202 = FtoGopPupAgIhjc42864193;     FtoGopPupAgIhjc42864193 = FtoGopPupAgIhjc33696835;     FtoGopPupAgIhjc33696835 = FtoGopPupAgIhjc97346205;     FtoGopPupAgIhjc97346205 = FtoGopPupAgIhjc99225856;     FtoGopPupAgIhjc99225856 = FtoGopPupAgIhjc20533604;     FtoGopPupAgIhjc20533604 = FtoGopPupAgIhjc73178929;     FtoGopPupAgIhjc73178929 = FtoGopPupAgIhjc19932714;     FtoGopPupAgIhjc19932714 = FtoGopPupAgIhjc55232081;     FtoGopPupAgIhjc55232081 = FtoGopPupAgIhjc88080095;     FtoGopPupAgIhjc88080095 = FtoGopPupAgIhjc66158801;     FtoGopPupAgIhjc66158801 = FtoGopPupAgIhjc84271989;     FtoGopPupAgIhjc84271989 = FtoGopPupAgIhjc91047831;     FtoGopPupAgIhjc91047831 = FtoGopPupAgIhjc39782304;     FtoGopPupAgIhjc39782304 = FtoGopPupAgIhjc36386218;     FtoGopPupAgIhjc36386218 = FtoGopPupAgIhjc23102144;     FtoGopPupAgIhjc23102144 = FtoGopPupAgIhjc3722697;     FtoGopPupAgIhjc3722697 = FtoGopPupAgIhjc37059856;     FtoGopPupAgIhjc37059856 = FtoGopPupAgIhjc41089294;     FtoGopPupAgIhjc41089294 = FtoGopPupAgIhjc95293920;     FtoGopPupAgIhjc95293920 = FtoGopPupAgIhjc63274508;     FtoGopPupAgIhjc63274508 = FtoGopPupAgIhjc30311985;     FtoGopPupAgIhjc30311985 = FtoGopPupAgIhjc34676542;     FtoGopPupAgIhjc34676542 = FtoGopPupAgIhjc75516175;     FtoGopPupAgIhjc75516175 = FtoGopPupAgIhjc86534781;     FtoGopPupAgIhjc86534781 = FtoGopPupAgIhjc24376020;     FtoGopPupAgIhjc24376020 = FtoGopPupAgIhjc55400957;     FtoGopPupAgIhjc55400957 = FtoGopPupAgIhjc91853581;     FtoGopPupAgIhjc91853581 = FtoGopPupAgIhjc46922488;     FtoGopPupAgIhjc46922488 = FtoGopPupAgIhjc47494049;     FtoGopPupAgIhjc47494049 = FtoGopPupAgIhjc57495100;     FtoGopPupAgIhjc57495100 = FtoGopPupAgIhjc12079751;     FtoGopPupAgIhjc12079751 = FtoGopPupAgIhjc51304071;     FtoGopPupAgIhjc51304071 = FtoGopPupAgIhjc88665726;     FtoGopPupAgIhjc88665726 = FtoGopPupAgIhjc54291076;     FtoGopPupAgIhjc54291076 = FtoGopPupAgIhjc34395453;     FtoGopPupAgIhjc34395453 = FtoGopPupAgIhjc23500577;     FtoGopPupAgIhjc23500577 = FtoGopPupAgIhjc55256159;     FtoGopPupAgIhjc55256159 = FtoGopPupAgIhjc2185336;     FtoGopPupAgIhjc2185336 = FtoGopPupAgIhjc67783120;     FtoGopPupAgIhjc67783120 = FtoGopPupAgIhjc57671116;     FtoGopPupAgIhjc57671116 = FtoGopPupAgIhjc62116371;     FtoGopPupAgIhjc62116371 = FtoGopPupAgIhjc91487306;     FtoGopPupAgIhjc91487306 = FtoGopPupAgIhjc64876584;     FtoGopPupAgIhjc64876584 = FtoGopPupAgIhjc94504349;     FtoGopPupAgIhjc94504349 = FtoGopPupAgIhjc33823015;     FtoGopPupAgIhjc33823015 = FtoGopPupAgIhjc93853235;     FtoGopPupAgIhjc93853235 = FtoGopPupAgIhjc76955479;     FtoGopPupAgIhjc76955479 = FtoGopPupAgIhjc94814113;     FtoGopPupAgIhjc94814113 = FtoGopPupAgIhjc11185316;     FtoGopPupAgIhjc11185316 = FtoGopPupAgIhjc18651126;     FtoGopPupAgIhjc18651126 = FtoGopPupAgIhjc75119686;     FtoGopPupAgIhjc75119686 = FtoGopPupAgIhjc56478545;     FtoGopPupAgIhjc56478545 = FtoGopPupAgIhjc62207887;     FtoGopPupAgIhjc62207887 = FtoGopPupAgIhjc12360112;     FtoGopPupAgIhjc12360112 = FtoGopPupAgIhjc70305216;     FtoGopPupAgIhjc70305216 = FtoGopPupAgIhjc8187652;     FtoGopPupAgIhjc8187652 = FtoGopPupAgIhjc58180660;     FtoGopPupAgIhjc58180660 = FtoGopPupAgIhjc94690581;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void PHGeCfIBSASgqwGj73561519() {     float PEwrTwpqQUzcoXv78134887 = -661740017;    float PEwrTwpqQUzcoXv30517953 = -176033977;    float PEwrTwpqQUzcoXv76708063 = -77066115;    float PEwrTwpqQUzcoXv19117313 = -299221009;    float PEwrTwpqQUzcoXv44592372 = -3755010;    float PEwrTwpqQUzcoXv27910419 = -693953292;    float PEwrTwpqQUzcoXv97183283 = -403476373;    float PEwrTwpqQUzcoXv59671035 = -392288473;    float PEwrTwpqQUzcoXv35561073 = -978812246;    float PEwrTwpqQUzcoXv20178873 = -342172964;    float PEwrTwpqQUzcoXv85595262 = -249712363;    float PEwrTwpqQUzcoXv67835186 = -965719831;    float PEwrTwpqQUzcoXv27394862 = -796044003;    float PEwrTwpqQUzcoXv51765632 = -255158447;    float PEwrTwpqQUzcoXv83908376 = -710403253;    float PEwrTwpqQUzcoXv67599110 = 18288585;    float PEwrTwpqQUzcoXv63619755 = -339494181;    float PEwrTwpqQUzcoXv50526089 = -134565374;    float PEwrTwpqQUzcoXv72087731 = -571603403;    float PEwrTwpqQUzcoXv98173321 = -761786458;    float PEwrTwpqQUzcoXv19940734 = -798398944;    float PEwrTwpqQUzcoXv67055595 = -411322338;    float PEwrTwpqQUzcoXv81085353 = -704138579;    float PEwrTwpqQUzcoXv91903970 = -208925142;    float PEwrTwpqQUzcoXv94178323 = -492218615;    float PEwrTwpqQUzcoXv96753341 = -487400822;    float PEwrTwpqQUzcoXv37771464 = -310812189;    float PEwrTwpqQUzcoXv70581671 = -163825324;    float PEwrTwpqQUzcoXv18130179 = -250891880;    float PEwrTwpqQUzcoXv62552729 = -532791831;    float PEwrTwpqQUzcoXv71051992 = -898395438;    float PEwrTwpqQUzcoXv23769577 = 8649378;    float PEwrTwpqQUzcoXv61633892 = -332586369;    float PEwrTwpqQUzcoXv27059578 = -973083085;    float PEwrTwpqQUzcoXv95464513 = -708772649;    float PEwrTwpqQUzcoXv43589516 = -487333429;    float PEwrTwpqQUzcoXv2901639 = -849693969;    float PEwrTwpqQUzcoXv62952654 = -968667069;    float PEwrTwpqQUzcoXv61174620 = -204730198;    float PEwrTwpqQUzcoXv47062974 = -931351517;    float PEwrTwpqQUzcoXv91150732 = -194341493;    float PEwrTwpqQUzcoXv26122406 = -29230040;    float PEwrTwpqQUzcoXv24184271 = -441651182;    float PEwrTwpqQUzcoXv34769134 = -850000327;    float PEwrTwpqQUzcoXv25641332 = -822050194;    float PEwrTwpqQUzcoXv36952202 = 1445909;    float PEwrTwpqQUzcoXv75241287 = 95117006;    float PEwrTwpqQUzcoXv40204199 = -226691249;    float PEwrTwpqQUzcoXv4026042 = -42905267;    float PEwrTwpqQUzcoXv55265198 = -292124376;    float PEwrTwpqQUzcoXv28489530 = -950465508;    float PEwrTwpqQUzcoXv43580890 = -935037583;    float PEwrTwpqQUzcoXv80271259 = -865461407;    float PEwrTwpqQUzcoXv8948749 = -296148752;    float PEwrTwpqQUzcoXv53620786 = -499628949;    float PEwrTwpqQUzcoXv11079292 = -150417680;    float PEwrTwpqQUzcoXv49432599 = -471895398;    float PEwrTwpqQUzcoXv84804092 = -868140973;    float PEwrTwpqQUzcoXv24938990 = -807002395;    float PEwrTwpqQUzcoXv47839031 = -516354189;    float PEwrTwpqQUzcoXv90138954 = -283141104;    float PEwrTwpqQUzcoXv26601613 = -139651050;    float PEwrTwpqQUzcoXv41540856 = -41396593;    float PEwrTwpqQUzcoXv73008344 = -346020416;    float PEwrTwpqQUzcoXv49126880 = -443777526;    float PEwrTwpqQUzcoXv61825686 = -158361742;    float PEwrTwpqQUzcoXv6201295 = -533133463;    float PEwrTwpqQUzcoXv335285 = -822960918;    float PEwrTwpqQUzcoXv56301118 = -546385799;    float PEwrTwpqQUzcoXv40318861 = -123069825;    float PEwrTwpqQUzcoXv64697471 = -132017447;    float PEwrTwpqQUzcoXv667102 = -370827112;    float PEwrTwpqQUzcoXv89351468 = -929835176;    float PEwrTwpqQUzcoXv25024758 = -640251887;    float PEwrTwpqQUzcoXv7022590 = -467444966;    float PEwrTwpqQUzcoXv93818327 = -669168904;    float PEwrTwpqQUzcoXv42871325 = -969671156;    float PEwrTwpqQUzcoXv46316220 = -854138252;    float PEwrTwpqQUzcoXv66262638 = -386874949;    float PEwrTwpqQUzcoXv57226121 = -393664525;    float PEwrTwpqQUzcoXv21512054 = -482517829;    float PEwrTwpqQUzcoXv97567265 = 15879059;    float PEwrTwpqQUzcoXv66555629 = -20920057;    float PEwrTwpqQUzcoXv62864981 = -958767505;    float PEwrTwpqQUzcoXv34063199 = -582326323;    float PEwrTwpqQUzcoXv27471103 = -963357856;    float PEwrTwpqQUzcoXv43498317 = -125889215;    float PEwrTwpqQUzcoXv52685143 = 63562383;    float PEwrTwpqQUzcoXv73438792 = -373454137;    float PEwrTwpqQUzcoXv84385222 = -458354969;    float PEwrTwpqQUzcoXv94156916 = 84561969;    float PEwrTwpqQUzcoXv18097546 = -981552996;    float PEwrTwpqQUzcoXv38013664 = -61664674;    float PEwrTwpqQUzcoXv13335590 = -688376010;    float PEwrTwpqQUzcoXv56924019 = -548210413;    float PEwrTwpqQUzcoXv64549119 = 45309556;    float PEwrTwpqQUzcoXv84581549 = -987833447;    float PEwrTwpqQUzcoXv51175926 = 4369233;    float PEwrTwpqQUzcoXv85642253 = -306222802;    float PEwrTwpqQUzcoXv63815646 = -661740017;     PEwrTwpqQUzcoXv78134887 = PEwrTwpqQUzcoXv30517953;     PEwrTwpqQUzcoXv30517953 = PEwrTwpqQUzcoXv76708063;     PEwrTwpqQUzcoXv76708063 = PEwrTwpqQUzcoXv19117313;     PEwrTwpqQUzcoXv19117313 = PEwrTwpqQUzcoXv44592372;     PEwrTwpqQUzcoXv44592372 = PEwrTwpqQUzcoXv27910419;     PEwrTwpqQUzcoXv27910419 = PEwrTwpqQUzcoXv97183283;     PEwrTwpqQUzcoXv97183283 = PEwrTwpqQUzcoXv59671035;     PEwrTwpqQUzcoXv59671035 = PEwrTwpqQUzcoXv35561073;     PEwrTwpqQUzcoXv35561073 = PEwrTwpqQUzcoXv20178873;     PEwrTwpqQUzcoXv20178873 = PEwrTwpqQUzcoXv85595262;     PEwrTwpqQUzcoXv85595262 = PEwrTwpqQUzcoXv67835186;     PEwrTwpqQUzcoXv67835186 = PEwrTwpqQUzcoXv27394862;     PEwrTwpqQUzcoXv27394862 = PEwrTwpqQUzcoXv51765632;     PEwrTwpqQUzcoXv51765632 = PEwrTwpqQUzcoXv83908376;     PEwrTwpqQUzcoXv83908376 = PEwrTwpqQUzcoXv67599110;     PEwrTwpqQUzcoXv67599110 = PEwrTwpqQUzcoXv63619755;     PEwrTwpqQUzcoXv63619755 = PEwrTwpqQUzcoXv50526089;     PEwrTwpqQUzcoXv50526089 = PEwrTwpqQUzcoXv72087731;     PEwrTwpqQUzcoXv72087731 = PEwrTwpqQUzcoXv98173321;     PEwrTwpqQUzcoXv98173321 = PEwrTwpqQUzcoXv19940734;     PEwrTwpqQUzcoXv19940734 = PEwrTwpqQUzcoXv67055595;     PEwrTwpqQUzcoXv67055595 = PEwrTwpqQUzcoXv81085353;     PEwrTwpqQUzcoXv81085353 = PEwrTwpqQUzcoXv91903970;     PEwrTwpqQUzcoXv91903970 = PEwrTwpqQUzcoXv94178323;     PEwrTwpqQUzcoXv94178323 = PEwrTwpqQUzcoXv96753341;     PEwrTwpqQUzcoXv96753341 = PEwrTwpqQUzcoXv37771464;     PEwrTwpqQUzcoXv37771464 = PEwrTwpqQUzcoXv70581671;     PEwrTwpqQUzcoXv70581671 = PEwrTwpqQUzcoXv18130179;     PEwrTwpqQUzcoXv18130179 = PEwrTwpqQUzcoXv62552729;     PEwrTwpqQUzcoXv62552729 = PEwrTwpqQUzcoXv71051992;     PEwrTwpqQUzcoXv71051992 = PEwrTwpqQUzcoXv23769577;     PEwrTwpqQUzcoXv23769577 = PEwrTwpqQUzcoXv61633892;     PEwrTwpqQUzcoXv61633892 = PEwrTwpqQUzcoXv27059578;     PEwrTwpqQUzcoXv27059578 = PEwrTwpqQUzcoXv95464513;     PEwrTwpqQUzcoXv95464513 = PEwrTwpqQUzcoXv43589516;     PEwrTwpqQUzcoXv43589516 = PEwrTwpqQUzcoXv2901639;     PEwrTwpqQUzcoXv2901639 = PEwrTwpqQUzcoXv62952654;     PEwrTwpqQUzcoXv62952654 = PEwrTwpqQUzcoXv61174620;     PEwrTwpqQUzcoXv61174620 = PEwrTwpqQUzcoXv47062974;     PEwrTwpqQUzcoXv47062974 = PEwrTwpqQUzcoXv91150732;     PEwrTwpqQUzcoXv91150732 = PEwrTwpqQUzcoXv26122406;     PEwrTwpqQUzcoXv26122406 = PEwrTwpqQUzcoXv24184271;     PEwrTwpqQUzcoXv24184271 = PEwrTwpqQUzcoXv34769134;     PEwrTwpqQUzcoXv34769134 = PEwrTwpqQUzcoXv25641332;     PEwrTwpqQUzcoXv25641332 = PEwrTwpqQUzcoXv36952202;     PEwrTwpqQUzcoXv36952202 = PEwrTwpqQUzcoXv75241287;     PEwrTwpqQUzcoXv75241287 = PEwrTwpqQUzcoXv40204199;     PEwrTwpqQUzcoXv40204199 = PEwrTwpqQUzcoXv4026042;     PEwrTwpqQUzcoXv4026042 = PEwrTwpqQUzcoXv55265198;     PEwrTwpqQUzcoXv55265198 = PEwrTwpqQUzcoXv28489530;     PEwrTwpqQUzcoXv28489530 = PEwrTwpqQUzcoXv43580890;     PEwrTwpqQUzcoXv43580890 = PEwrTwpqQUzcoXv80271259;     PEwrTwpqQUzcoXv80271259 = PEwrTwpqQUzcoXv8948749;     PEwrTwpqQUzcoXv8948749 = PEwrTwpqQUzcoXv53620786;     PEwrTwpqQUzcoXv53620786 = PEwrTwpqQUzcoXv11079292;     PEwrTwpqQUzcoXv11079292 = PEwrTwpqQUzcoXv49432599;     PEwrTwpqQUzcoXv49432599 = PEwrTwpqQUzcoXv84804092;     PEwrTwpqQUzcoXv84804092 = PEwrTwpqQUzcoXv24938990;     PEwrTwpqQUzcoXv24938990 = PEwrTwpqQUzcoXv47839031;     PEwrTwpqQUzcoXv47839031 = PEwrTwpqQUzcoXv90138954;     PEwrTwpqQUzcoXv90138954 = PEwrTwpqQUzcoXv26601613;     PEwrTwpqQUzcoXv26601613 = PEwrTwpqQUzcoXv41540856;     PEwrTwpqQUzcoXv41540856 = PEwrTwpqQUzcoXv73008344;     PEwrTwpqQUzcoXv73008344 = PEwrTwpqQUzcoXv49126880;     PEwrTwpqQUzcoXv49126880 = PEwrTwpqQUzcoXv61825686;     PEwrTwpqQUzcoXv61825686 = PEwrTwpqQUzcoXv6201295;     PEwrTwpqQUzcoXv6201295 = PEwrTwpqQUzcoXv335285;     PEwrTwpqQUzcoXv335285 = PEwrTwpqQUzcoXv56301118;     PEwrTwpqQUzcoXv56301118 = PEwrTwpqQUzcoXv40318861;     PEwrTwpqQUzcoXv40318861 = PEwrTwpqQUzcoXv64697471;     PEwrTwpqQUzcoXv64697471 = PEwrTwpqQUzcoXv667102;     PEwrTwpqQUzcoXv667102 = PEwrTwpqQUzcoXv89351468;     PEwrTwpqQUzcoXv89351468 = PEwrTwpqQUzcoXv25024758;     PEwrTwpqQUzcoXv25024758 = PEwrTwpqQUzcoXv7022590;     PEwrTwpqQUzcoXv7022590 = PEwrTwpqQUzcoXv93818327;     PEwrTwpqQUzcoXv93818327 = PEwrTwpqQUzcoXv42871325;     PEwrTwpqQUzcoXv42871325 = PEwrTwpqQUzcoXv46316220;     PEwrTwpqQUzcoXv46316220 = PEwrTwpqQUzcoXv66262638;     PEwrTwpqQUzcoXv66262638 = PEwrTwpqQUzcoXv57226121;     PEwrTwpqQUzcoXv57226121 = PEwrTwpqQUzcoXv21512054;     PEwrTwpqQUzcoXv21512054 = PEwrTwpqQUzcoXv97567265;     PEwrTwpqQUzcoXv97567265 = PEwrTwpqQUzcoXv66555629;     PEwrTwpqQUzcoXv66555629 = PEwrTwpqQUzcoXv62864981;     PEwrTwpqQUzcoXv62864981 = PEwrTwpqQUzcoXv34063199;     PEwrTwpqQUzcoXv34063199 = PEwrTwpqQUzcoXv27471103;     PEwrTwpqQUzcoXv27471103 = PEwrTwpqQUzcoXv43498317;     PEwrTwpqQUzcoXv43498317 = PEwrTwpqQUzcoXv52685143;     PEwrTwpqQUzcoXv52685143 = PEwrTwpqQUzcoXv73438792;     PEwrTwpqQUzcoXv73438792 = PEwrTwpqQUzcoXv84385222;     PEwrTwpqQUzcoXv84385222 = PEwrTwpqQUzcoXv94156916;     PEwrTwpqQUzcoXv94156916 = PEwrTwpqQUzcoXv18097546;     PEwrTwpqQUzcoXv18097546 = PEwrTwpqQUzcoXv38013664;     PEwrTwpqQUzcoXv38013664 = PEwrTwpqQUzcoXv13335590;     PEwrTwpqQUzcoXv13335590 = PEwrTwpqQUzcoXv56924019;     PEwrTwpqQUzcoXv56924019 = PEwrTwpqQUzcoXv64549119;     PEwrTwpqQUzcoXv64549119 = PEwrTwpqQUzcoXv84581549;     PEwrTwpqQUzcoXv84581549 = PEwrTwpqQUzcoXv51175926;     PEwrTwpqQUzcoXv51175926 = PEwrTwpqQUzcoXv85642253;     PEwrTwpqQUzcoXv85642253 = PEwrTwpqQUzcoXv63815646;     PEwrTwpqQUzcoXv63815646 = PEwrTwpqQUzcoXv78134887;}
// Junk Finished
