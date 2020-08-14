// dear imgui, v1.77 WIP
// (widgets code)

/*

Index of this file:

// [SECTION] Forward Declarations
// [SECTION] Widgets: Text, etc.
// [SECTION] Widgets: Main (Button, Image, Checkbox, RadioButton, ProgressBar, Bullet, etc.)
// [SECTION] Widgets: Low-level Layout helpers (Spacing, Dummy, NewLine, Separator, etc.)
// [SECTION] Widgets: ComboBox
// [SECTION] Data Type and Data Formatting Helpers
// [SECTION] Widgets: DragScalar, DragFloat, DragInt, etc.
// [SECTION] Widgets: SliderScalar, SliderFloat, SliderInt, etc.
// [SECTION] Widgets: InputScalar, InputFloat, InputInt, etc.
// [SECTION] Widgets: InputText, InputTextMultiline
// [SECTION] Widgets: ColorEdit, ColorPicker, ColorButton, etc.
// [SECTION] Widgets: TreeNode, CollapsingHeader, etc.
// [SECTION] Widgets: Selectable
// [SECTION] Widgets: ListBox
// [SECTION] Widgets: PlotLines, PlotHistogram
// [SECTION] Widgets: Value helpers
// [SECTION] Widgets: MenuItem, BeginMenu, EndMenu, etc.
// [SECTION] Widgets: BeginTabBar, EndTabBar, etc.
// [SECTION] Widgets: BeginTabItem, EndTabItem, etc.
// [SECTION] Widgets: Columns, BeginColumns, EndColumns, etc.

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DISABLE

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"

#include <ctype.h>      // toupper
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127)     // condition expression is constant
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#if defined(_MSC_VER) && _MSC_VER >= 1922 // MSVC 2019 16.2 or later
#pragma warning (disable: 5054)     // operator '|': deprecated between enumerations of different types
#endif
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants (typically 0.0f) is ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning : format string is not a string literal              // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant              // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"       // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#if __has_warning("-Wdeprecated-enum-enum-conversion")
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion" // warning: bitwise operation between different enumeration types ('XXXFlags_' and 'XXXFlagsPrivate_') is deprecated
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wformat-nonliteral"        // warning: format not a string literal, format string not checked
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-------------------------------------------------------------------------
// Data
//-------------------------------------------------------------------------

// Those MIN/MAX values are not define because we need to point to them
static const signed char    IM_S8_MIN = -128;
static const signed char    IM_S8_MAX = 127;
static const unsigned char  IM_U8_MIN = 0;
static const unsigned char  IM_U8_MAX = 0xFF;
static const signed short   IM_S16_MIN = -32768;
static const signed short   IM_S16_MAX = 32767;
static const unsigned short IM_U16_MIN = 0;
static const unsigned short IM_U16_MAX = 0xFFFF;
static const ImS32          IM_S32_MIN = INT_MIN;    // (-2147483647 - 1), (0x80000000);
static const ImS32          IM_S32_MAX = INT_MAX;    // (2147483647), (0x7FFFFFFF)
static const ImU32          IM_U32_MIN = 0;
static const ImU32          IM_U32_MAX = UINT_MAX;   // (0xFFFFFFFF)
#ifdef LLONG_MIN
static const ImS64          IM_S64_MIN = LLONG_MIN;  // (-9223372036854775807ll - 1ll);
static const ImS64          IM_S64_MAX = LLONG_MAX;  // (9223372036854775807ll);
#else
static const ImS64          IM_S64_MIN = -9223372036854775807LL - 1;
static const ImS64          IM_S64_MAX = 9223372036854775807LL;
#endif
static const ImU64          IM_U64_MIN = 0;
#ifdef ULLONG_MAX
static const ImU64          IM_U64_MAX = ULLONG_MAX; // (0xFFFFFFFFFFFFFFFFull);
#else
static const ImU64          IM_U64_MAX = (2ULL * 9223372036854775807LL + 1);
#endif

//-------------------------------------------------------------------------
// [SECTION] Forward Declarations
//-------------------------------------------------------------------------

// For InputTextEx()
static bool             InputTextFilterCharacter(unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
static int              InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end);
static ImVec2           InputTextCalcTextSizeW(const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining = NULL, ImVec2* out_offset = NULL, bool stop_on_new_line = false);

//-------------------------------------------------------------------------
// [SECTION] Widgets: Text, etc.
//-------------------------------------------------------------------------
// - TextEx() [Internal]
// - TextUnformatted()
// - Text()
// - TextV()
// - TextColored()
// - TextColoredV()
// - TextDisabled()
// - TextDisabledV()
// - TextWrapped()
// - TextWrappedV()
// - LabelText()
// - LabelTextV()
// - BulletText()
// - BulletTextV()
//-------------------------------------------------------------------------

void ImGui::TextEx(const char* text, const char* text_end, ImGuiTextFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    IM_ASSERT(text != NULL);
    const char* text_begin = text;
    if (text_end == NULL)
        text_end = text + strlen(text); // FIXME-OPT

    const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
    const float wrap_pos_x = window->DC.TextWrapPos;
    const bool wrap_enabled = (wrap_pos_x >= 0.0f);
    if (text_end - text > 2000 && !wrap_enabled)
    {
        // Long text!
        // Perform manual coarse clipping to optimize for long multi-line text
        // - From this point we will only compute the width of lines that are visible. Optimization only available when word-wrapping is disabled.
        // - We also don't vertically center the text within the line full height, which is unlikely to matter because we are likely the biggest and only item on the line.
        // - We use memchr(), pay attention that well optimized versions of those str/mem functions are much faster than a casually written loop.
        const char* line = text;
        const float line_height = GetTextLineHeight();
        ImVec2 text_size(0, 0);

        // Lines to skip (can't skip when logging text)
        ImVec2 pos = text_pos;
        if (!g.LogEnabled)
        {
            int lines_skippable = (int)((window->ClipRect.Min.y - text_pos.y) / line_height);
            if (lines_skippable > 0)
            {
                int lines_skipped = 0;
                while (line < text_end && lines_skipped < lines_skippable)
                {
                    const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                    if (!line_end)
                        line_end = text_end;
                    if ((flags & ImGuiTextFlags_NoWidthForLargeClippedText) == 0)
                        text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                    line = line_end + 1;
                    lines_skipped++;
                }
                pos.y += lines_skipped * line_height;
            }
        }

        // Lines to render
        if (line < text_end)
        {
            ImRect line_rect(pos, pos + ImVec2(FLT_MAX, line_height));
            while (line < text_end)
            {
                if (IsClippedEx(line_rect, 0, false))
                    break;

                const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                if (!line_end)
                    line_end = text_end;
                text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                RenderText(pos, line, line_end, false);
                line = line_end + 1;
                line_rect.Min.y += line_height;
                line_rect.Max.y += line_height;
                pos.y += line_height;
            }

            // Count remaining lines
            int lines_skipped = 0;
            while (line < text_end)
            {
                const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                if (!line_end)
                    line_end = text_end;
                if ((flags & ImGuiTextFlags_NoWidthForLargeClippedText) == 0)
                    text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                line = line_end + 1;
                lines_skipped++;
            }
            pos.y += lines_skipped * line_height;
        }
        text_size.y = (pos - text_pos).y;

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(text_size, 0.0f);
        ItemAdd(bb, 0);
    } else
    {
        const float wrap_width = wrap_enabled ? CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
        const ImVec2 text_size = CalcTextSize(text_begin, text_end, false, wrap_width);

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(text_size, 0.0f);
        if (!ItemAdd(bb, 0))
            return;

        // Render (we don't hide text after ## in this end-user function)
        RenderTextWrapped(bb.Min, text_begin, text_end, wrap_width);
    }
}

void ImGui::TextUnformatted(const char* text, const char* text_end)
{
    TextEx(text, text_end, ImGuiTextFlags_NoWidthForLargeClippedText);
}

void ImGui::Text(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextV(fmt, args);
    va_end(args);
}

void ImGui::TextV(const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const char* text_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    TextEx(g.TempBuffer, text_end, ImGuiTextFlags_NoWidthForLargeClippedText);
}

void ImGui::TextColored(const ImVec4& col, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextColoredV(col, fmt, args);
    va_end(args);
}

void ImGui::TextColoredV(const ImVec4& col, const char* fmt, va_list args)
{
    PushStyleColor(ImGuiCol_Text, col);
    TextV(fmt, args);
    PopStyleColor();
}

void ImGui::TextDisabled(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextDisabledV(fmt, args);
    va_end(args);
}

void ImGui::TextDisabledV(const char* fmt, va_list args)
{
    PushStyleColor(ImGuiCol_Text, GImGui->Style.Colors[ImGuiCol_TextDisabled]);
    TextV(fmt, args);
    PopStyleColor();
}

void ImGui::TextWrapped(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextWrappedV(fmt, args);
    va_end(args);
}

void ImGui::TextWrappedV(const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    bool need_backup = (window->DC.TextWrapPos < 0.0f);  // Keep existing wrap position if one is already set
    if (need_backup)
        PushTextWrapPos(0.0f);
    TextV(fmt, args);
    if (need_backup)
        PopTextWrapPos();
}

void ImGui::LabelText(const char* label, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LabelTextV(label, fmt, args);
    va_end(args);
}

// Add a label+text combo aligned to other label+value widgets
void ImGui::LabelTextV(const char* label, const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float w = CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect value_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2));
    const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + (label_size.x > 0.0f ? style.ItemInnerSpacing.x : 0.0f), style.FramePadding.y * 2) + label_size);
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0))
        return;

    // Render
    const char* value_text_begin = &g.TempBuffer[0];
    const char* value_text_end = value_text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    RenderTextClipped(value_bb.Min, value_bb.Max, value_text_begin, value_text_end, NULL, ImVec2(0.0f, 0.5f));
    if (label_size.x > 0.0f)
        RenderText(ImVec2(value_bb.Max.x + style.ItemInnerSpacing.x, value_bb.Min.y + style.FramePadding.y), label);
}

void ImGui::BulletText(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    BulletTextV(fmt, args);
    va_end(args);
}

// Text with a little bullet aligned to the typical tree node.
void ImGui::BulletTextV(const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const char* text_begin = g.TempBuffer;
    const char* text_end = text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    const ImVec2 label_size = CalcTextSize(text_begin, text_end, false);
    const ImVec2 total_size = ImVec2(g.FontSize + (label_size.x > 0.0f ? (label_size.x + style.FramePadding.x * 2) : 0.0f), label_size.y);  // Empty text doesn't add padding
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrLineTextBaseOffset;
    ItemSize(total_size, 0.0f);
    const ImRect bb(pos, pos + total_size);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    ImU32 text_col = GetColorU32(ImGuiCol_Text);
    RenderBullet(window->DrawList, bb.Min + ImVec2(style.FramePadding.x + g.FontSize * 0.5f, g.FontSize * 0.5f), text_col);
    RenderText(bb.Min + ImVec2(g.FontSize + style.FramePadding.x * 2, 0.0f), text_begin, text_end, false);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Main
//-------------------------------------------------------------------------
// - ButtonBehavior() [Internal]
// - Button()
// - SmallButton()
// - InvisibleButton()
// - ArrowButton()
// - CloseButton() [Internal]
// - CollapseButton() [Internal]
// - GetWindowScrollbarID() [Internal]
// - GetWindowScrollbarRect() [Internal]
// - Scrollbar() [Internal]
// - ScrollbarEx() [Internal]
// - Image()
// - ImageButton()
// - Checkbox()
// - CheckboxFlags()
// - RadioButton()
// - ProgressBar()
// - Bullet()
//-------------------------------------------------------------------------

// The ButtonBehavior() function is key to many interactions and used by many/most widgets.
// Because we handle so many cases (keyboard/gamepad navigation, drag and drop) and many specific behavior (via ImGuiButtonFlags_),
// this code is a little complex.
// By far the most common path is interacting with the Mouse using the default ImGuiButtonFlags_PressedOnClickRelease button behavior.
// See the series of events below and the corresponding state reported by dear imgui:
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnClickRelease:             return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+0 (mouse is outside bb)        -             -                -               -                  -                    -
//   Frame N+1 (mouse moves inside bb)      -             true             -               -                  -                    -
//   Frame N+2 (mouse button is down)       -             true             true            true               -                    true
//   Frame N+3 (mouse button is down)       -             true             true            -                  -                    -
//   Frame N+4 (mouse moves outside bb)     -             -                true            -                  -                    -
//   Frame N+5 (mouse moves inside bb)      -             true             true            -                  -                    -
//   Frame N+6 (mouse button is released)   true          true             -               -                  true                 -
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -
//   Frame N+8 (mouse moves outside bb)     -             -                -               -                  -                    -
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnClick:                    return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+2 (mouse button is down)       true          true             true            true               -                    true
//   Frame N+3 (mouse button is down)       -             true             true            -                  -                    -
//   Frame N+6 (mouse button is released)   -             true             -               -                  true                 -
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnRelease:                  return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+2 (mouse button is down)       -             true             -               -                  -                    true
//   Frame N+3 (mouse button is down)       -             true             -               -                  -                    -
//   Frame N+6 (mouse button is released)   true          true             -               -                  -                    -
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnDoubleClick:              return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+0 (mouse button is down)       -             true             -               -                  -                    true
//   Frame N+1 (mouse button is down)       -             true             -               -                  -                    -
//   Frame N+2 (mouse button is released)   -             true             -               -                  -                    -
//   Frame N+3 (mouse button is released)   -             true             -               -                  -                    -
//   Frame N+4 (mouse button is down)       true          true             true            true               -                    true
//   Frame N+5 (mouse button is down)       -             true             true            -                  -                    -
//   Frame N+6 (mouse button is released)   -             true             -               -                  true                 -
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -
//------------------------------------------------------------------------------------------------------------------------------------------------
// Note that some combinations are supported,
// - PressedOnDragDropHold can generally be associated with any flag.
// - PressedOnDoubleClick can be associated by PressedOnClickRelease/PressedOnRelease, in which case the second release event won't be reported.
//------------------------------------------------------------------------------------------------------------------------------------------------
// The behavior of the return-value changes when ImGuiButtonFlags_Repeat is set:
//                                         Repeat+                  Repeat+           Repeat+             Repeat+
//                                         PressedOnClickRelease    PressedOnClick    PressedOnRelease    PressedOnDoubleClick
//-------------------------------------------------------------------------------------------------------------------------------------------------
//   Frame N+0 (mouse button is down)       -                        true              -                   true
//   ...                                    -                        -                 -                   -
//   Frame N + RepeatDelay                  true                     true              -                   true
//   ...                                    -                        -                 -                   -
//   Frame N + RepeatDelay + RepeatRate*N   true                     true              -                   true
//-------------------------------------------------------------------------------------------------------------------------------------------------

bool ImGui::ButtonBehavior(const ImRect& bb, ImGuiID id, bool* out_hovered, bool* out_held, ImGuiButtonFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    if (flags & ImGuiButtonFlags_Disabled)
    {
        if (out_hovered) *out_hovered = false;
        if (out_held) *out_held = false;
        if (g.ActiveId == id) ClearActiveID();
        return false;
    }

    // Default only reacts to left mouse button
    if ((flags & ImGuiButtonFlags_MouseButtonMask_) == 0)
        flags |= ImGuiButtonFlags_MouseButtonDefault_;

    // Default behavior requires click + release inside bounding box
    if ((flags & ImGuiButtonFlags_PressedOnMask_) == 0)
        flags |= ImGuiButtonFlags_PressedOnDefault_;

    ImGuiWindow* backup_hovered_window = g.HoveredWindow;
    const bool flatten_hovered_children = (flags & ImGuiButtonFlags_FlattenChildren) && g.HoveredRootWindow == window;
    if (flatten_hovered_children)
        g.HoveredWindow = window;

#ifdef IMGUI_ENABLE_TEST_ENGINE
    if (id != 0 && window->DC.LastItemId != id)
        IMGUI_TEST_ENGINE_ITEM_ADD(bb, id);
#endif

    bool pressed = false;
    bool hovered = ItemHoverable(bb, id);

    // Drag source doesn't report as hovered
    if (hovered && g.DragDropActive && g.DragDropPayload.SourceId == id && !(g.DragDropSourceFlags & ImGuiDragDropFlags_SourceNoDisableHover))
        hovered = false;

    // Special mode for Drag and Drop where holding button pressed for a long time while dragging another item triggers the button
    if (g.DragDropActive && (flags & ImGuiButtonFlags_PressedOnDragDropHold) && !(g.DragDropSourceFlags & ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
        if (IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        {
            const float DRAG_DROP_HOLD_TIMER = 0.70f;
            hovered = true;
            SetHoveredID(id);
            if (CalcTypematicRepeatAmount(g.HoveredIdTimer + 0.0001f - g.IO.DeltaTime, g.HoveredIdTimer + 0.0001f, DRAG_DROP_HOLD_TIMER, 0.00f))
            {
                pressed = true;
                g.DragDropHoldJustPressedId = id;
                FocusWindow(window);
            }
        }

    if (flatten_hovered_children)
        g.HoveredWindow = backup_hovered_window;

    // AllowOverlap mode (rarely used) requires previous frame HoveredId to be null or to match. This allows using patterns where a later submitted widget overlaps a previous one.
    if (hovered && (flags & ImGuiButtonFlags_AllowItemOverlap) && (g.HoveredIdPreviousFrame != id && g.HoveredIdPreviousFrame != 0))
        hovered = false;

    // Mouse handling
    if (hovered)
    {
        if (!(flags & ImGuiButtonFlags_NoKeyModifiers) || (!g.IO.KeyCtrl && !g.IO.KeyShift && !g.IO.KeyAlt))
        {
            // Poll buttons
            int mouse_button_clicked = -1;
            int mouse_button_released = -1;
            if ((flags & ImGuiButtonFlags_MouseButtonLeft) && g.IO.MouseClicked[0]) { mouse_button_clicked = 0; } else if ((flags & ImGuiButtonFlags_MouseButtonRight) && g.IO.MouseClicked[1]) { mouse_button_clicked = 1; } else if ((flags & ImGuiButtonFlags_MouseButtonMiddle) && g.IO.MouseClicked[2]) { mouse_button_clicked = 2; }
            if ((flags & ImGuiButtonFlags_MouseButtonLeft) && g.IO.MouseReleased[0]) { mouse_button_released = 0; } else if ((flags & ImGuiButtonFlags_MouseButtonRight) && g.IO.MouseReleased[1]) { mouse_button_released = 1; } else if ((flags & ImGuiButtonFlags_MouseButtonMiddle) && g.IO.MouseReleased[2]) { mouse_button_released = 2; }

            if (mouse_button_clicked != -1 && g.ActiveId != id)
            {
                if (flags & (ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnClickReleaseAnywhere))
                {
                    SetActiveID(id, window);
                    g.ActiveIdMouseButton = mouse_button_clicked;
                    if (!(flags & ImGuiButtonFlags_NoNavFocus))
                        SetFocusID(id, window);
                    FocusWindow(window);
                }
                if ((flags & ImGuiButtonFlags_PressedOnClick) || ((flags & ImGuiButtonFlags_PressedOnDoubleClick) && g.IO.MouseDoubleClicked[mouse_button_clicked]))
                {
                    pressed = true;
                    if (flags & ImGuiButtonFlags_NoHoldingActiveId)
                        ClearActiveID();
                    else
                        SetActiveID(id, window); // Hold on ID
                    g.ActiveIdMouseButton = mouse_button_clicked;
                    FocusWindow(window);
                }
            }
            if ((flags & ImGuiButtonFlags_PressedOnRelease) && mouse_button_released != -1)
            {
                // Repeat mode trumps on release behavior
                if (!((flags & ImGuiButtonFlags_Repeat) && g.IO.MouseDownDurationPrev[mouse_button_released] >= g.IO.KeyRepeatDelay))
                    pressed = true;
                ClearActiveID();
            }

            // 'Repeat' mode acts when held regardless of _PressedOn flags (see table above).
            // Relies on repeat logic of IsMouseClicked() but we may as well do it ourselves if we end up exposing finer RepeatDelay/RepeatRate settings.
            if (g.ActiveId == id && (flags & ImGuiButtonFlags_Repeat))
                if (g.IO.MouseDownDuration[g.ActiveIdMouseButton] > 0.0f && IsMouseClicked(g.ActiveIdMouseButton, true))
                    pressed = true;
        }

        if (pressed)
            g.NavDisableHighlight = true;
    }

    // Gamepad/Keyboard navigation
    // We report navigated item as hovered but we don't set g.HoveredId to not interfere with mouse.
    if (g.NavId == id && !g.NavDisableHighlight && g.NavDisableMouseHover && (g.ActiveId == 0 || g.ActiveId == id || g.ActiveId == window->MoveId))
        if (!(flags & ImGuiButtonFlags_NoHoveredOnFocus))
            hovered = true;
    if (g.NavActivateDownId == id)
    {
        bool nav_activated_by_code = (g.NavActivateId == id);
        bool nav_activated_by_inputs = IsNavInputTest(ImGuiNavInput_Activate, (flags & ImGuiButtonFlags_Repeat) ? ImGuiInputReadMode_Repeat : ImGuiInputReadMode_Pressed);
        if (nav_activated_by_code || nav_activated_by_inputs)
            pressed = true;
        if (nav_activated_by_code || nav_activated_by_inputs || g.ActiveId == id)
        {
            // Set active id so it can be queried by user via IsItemActive(), equivalent of holding the mouse button.
            g.NavActivateId = id; // This is so SetActiveId assign a Nav source
            SetActiveID(id, window);
            if ((nav_activated_by_code || nav_activated_by_inputs) && !(flags & ImGuiButtonFlags_NoNavFocus))
                SetFocusID(id, window);
        }
    }

    bool held = false;
    if (g.ActiveId == id)
    {
        if (g.ActiveIdSource == ImGuiInputSource_Mouse)
        {
            if (g.ActiveIdIsJustActivated)
                g.ActiveIdClickOffset = g.IO.MousePos - bb.Min;

            const int mouse_button = g.ActiveIdMouseButton;
            IM_ASSERT(mouse_button >= 0 && mouse_button < ImGuiMouseButton_COUNT);
            if (g.IO.MouseDown[mouse_button])
            {
                held = true;
            } else
            {
                bool release_in = hovered && (flags & ImGuiButtonFlags_PressedOnClickRelease) != 0;
                bool release_anywhere = (flags & ImGuiButtonFlags_PressedOnClickReleaseAnywhere) != 0;
                if ((release_in || release_anywhere) && !g.DragDropActive)
                {
                    bool is_double_click_release = (flags & ImGuiButtonFlags_PressedOnDoubleClick) && g.IO.MouseDownWasDoubleClick[mouse_button];
                    bool is_repeating_already = (flags & ImGuiButtonFlags_Repeat) && g.IO.MouseDownDurationPrev[mouse_button] >= g.IO.KeyRepeatDelay; // Repeat mode trumps <on release>
                    if (!is_double_click_release && !is_repeating_already)
                        pressed = true;
                }
                ClearActiveID();
            }
            if (!(flags & ImGuiButtonFlags_NoNavFocus))
                g.NavDisableHighlight = true;
        } else if (g.ActiveIdSource == ImGuiInputSource_Nav)
        {
            if (g.NavActivateDownId != id)
                ClearActiveID();
        }
        if (pressed)
            g.ActiveIdHasBeenPressedBefore = true;
    }

    if (out_hovered) *out_hovered = hovered;
    if (out_held) *out_held = held;

    return pressed;
}

bool ImGui::ButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
    RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
    return pressed;
}

bool ImGui::Button(const char* label, const ImVec2& size_arg)
{
    return ButtonEx(label, size_arg, 0);
}

// Small buttons fits within text without additional vertical spacing.
bool ImGui::SmallButton(const char* label)
{
    ImGuiContext& g = *GImGui;
    float backup_padding_y = g.Style.FramePadding.y;
    g.Style.FramePadding.y = 0.0f;
    bool pressed = ButtonEx(label, ImVec2(0, 0), ImGuiButtonFlags_AlignTextBaseLine);
    g.Style.FramePadding.y = backup_padding_y;
    return pressed;
}

// Tip: use ImGui::PushID()/PopID() to push indices or pointers in the ID stack.
// Then you can keep 'str_id' empty or the same for all your buttons (instead of creating a string based on a non-string id)
bool ImGui::InvisibleButton(const char* str_id, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    // Cannot use zero-size for InvisibleButton(). Unlike Button() there is not way to fallback using the label size.
    IM_ASSERT(size_arg.x != 0.0f && size_arg.y != 0.0f);

    const ImGuiID id = window->GetID(str_id);
    ImVec2 size = CalcItemSize(size_arg, 0.0f, 0.0f);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(size);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    return pressed;
}

bool ImGui::ArrowButtonEx(const char* str_id, ImGuiDir dir, ImVec2 size, ImGuiButtonFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(str_id);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const float default_size = GetFrameHeight();
    ItemSize(size, (size.y >= default_size) ? g.Style.FramePadding.y : -1.0f);
    if (!ItemAdd(bb, id))
        return false;

    if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    const ImU32 text_col = GetColorU32(ImGuiCol_Text);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
    RenderArrow(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, dir);

    return pressed;
}

bool ImGui::ArrowButton(const char* str_id, ImGuiDir dir)
{
    float sz = GetFrameHeight();
    return ArrowButtonEx(str_id, dir, ImVec2(sz, sz), ImGuiButtonFlags_None);
}

// Button to close a window
bool ImGui::CloseButton(ImGuiID id, const ImVec2& pos)//, float size)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    // We intentionally allow interaction when clipped so that a mechanical Alt,Right,Validate sequence close a window.
    // (this isn't the regular behavior of buttons, but it doesn't affect the user much because navigation tends to keep items visible).
    const ImRect bb(pos, pos + ImVec2(g.FontSize, g.FontSize) + g.Style.FramePadding * 2.0f);
    bool is_clipped = !ItemAdd(bb, id);

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (is_clipped)
        return pressed;

    // Render
    ImU32 col = GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
    ImVec2 center = bb.GetCenter();
    if (hovered)
        window->DrawList->AddCircleFilled(center, ImMax(2.0f, g.FontSize * 0.5f + 1.0f), col, 12);

    float cross_extent = g.FontSize * 0.5f * 0.7071f - 1.0f;
    ImU32 cross_col = GetColorU32(ImGuiCol_Text);
    center -= ImVec2(0.5f, 0.5f);
    window->DrawList->AddLine(center + ImVec2(+cross_extent, +cross_extent), center + ImVec2(-cross_extent, -cross_extent), cross_col, 1.0f);
    window->DrawList->AddLine(center + ImVec2(+cross_extent, -cross_extent), center + ImVec2(-cross_extent, +cross_extent), cross_col, 1.0f);

    return pressed;
}

bool ImGui::CollapseButton(ImGuiID id, const ImVec2& pos)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    ImRect bb(pos, pos + ImVec2(g.FontSize, g.FontSize) + g.Style.FramePadding * 2.0f);
    ItemAdd(bb, id);
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_None);

    // Render
    ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    ImU32 text_col = GetColorU32(ImGuiCol_Text);
    ImVec2 center = bb.GetCenter();
    if (hovered || held)
        window->DrawList->AddCircleFilled(center/*+ ImVec2(0.0f, -0.5f)*/, g.FontSize * 0.5f + 1.0f, bg_col, 12);
    RenderArrow(window->DrawList, bb.Min + g.Style.FramePadding, text_col, window->Collapsed ? ImGuiDir_Right : ImGuiDir_Down, 1.0f);

    // Switch to moving the window after mouse is moved beyond the initial drag threshold
    if (IsItemActive() && IsMouseDragging(0))
        StartMouseMovingWindow(window);

    return pressed;
}

ImGuiID ImGui::GetWindowScrollbarID(ImGuiWindow* window, ImGuiAxis axis)
{
    return window->GetIDNoKeepAlive(axis == ImGuiAxis_X ? "#SCROLLX" : "#SCROLLY");
}

// Return scrollbar rectangle, must only be called for corresponding axis if window->ScrollbarX/Y is set.
ImRect ImGui::GetWindowScrollbarRect(ImGuiWindow* window, ImGuiAxis axis)
{
    const ImRect outer_rect = window->Rect();
    const ImRect inner_rect = window->InnerRect;
    const float border_size = window->WindowBorderSize;
    const float scrollbar_size = window->ScrollbarSizes[axis ^ 1]; // (ScrollbarSizes.x = width of Y scrollbar; ScrollbarSizes.y = height of X scrollbar)
    IM_ASSERT(scrollbar_size > 0.0f);
    if (axis == ImGuiAxis_X)
        return ImRect(inner_rect.Min.x, ImMax(outer_rect.Min.y, outer_rect.Max.y - border_size - scrollbar_size), inner_rect.Max.x, outer_rect.Max.y);
    else
        return ImRect(ImMax(outer_rect.Min.x, outer_rect.Max.x - border_size - scrollbar_size), inner_rect.Min.y, outer_rect.Max.x, inner_rect.Max.y);
}

void ImGui::Scrollbar(ImGuiAxis axis)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImGuiID id = GetWindowScrollbarID(window, axis);
    KeepAliveID(id);

    // Calculate scrollbar bounding box
    ImRect bb = GetWindowScrollbarRect(window, axis);
    ImDrawCornerFlags rounding_corners = 0;
    if (axis == ImGuiAxis_X)
    {
        rounding_corners |= ImDrawCornerFlags_BotLeft;
        if (!window->ScrollbarY)
            rounding_corners |= ImDrawCornerFlags_BotRight;
    } else
    {
        if ((window->Flags & ImGuiWindowFlags_NoTitleBar) && !(window->Flags & ImGuiWindowFlags_MenuBar))
            rounding_corners |= ImDrawCornerFlags_TopRight;
        if (!window->ScrollbarX)
            rounding_corners |= ImDrawCornerFlags_BotRight;
    }
    float size_avail = window->InnerRect.Max[axis] - window->InnerRect.Min[axis];
    float size_contents = window->ContentSize[axis] + window->WindowPadding[axis] * 2.0f;
    ScrollbarEx(bb, id, axis, &window->Scroll[axis], size_avail, size_contents, rounding_corners);
}

// Vertical/Horizontal scrollbar
// The entire piece of code below is rather confusing because:
// - We handle absolute seeking (when first clicking outside the grab) and relative manipulation (afterward or when clicking inside the grab)
// - We store values as normalized ratio and in a form that allows the window content to change while we are holding on a scrollbar
// - We handle both horizontal and vertical scrollbars, which makes the terminology not ideal.
// Still, the code should probably be made simpler..
bool ImGui::ScrollbarEx(const ImRect& bb_frame, ImGuiID id, ImGuiAxis axis, float* p_scroll_v, float size_avail_v, float size_contents_v, ImDrawCornerFlags rounding_corners)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    const float bb_frame_width = bb_frame.GetWidth();
    const float bb_frame_height = bb_frame.GetHeight();
    if (bb_frame_width <= 0.0f || bb_frame_height <= 0.0f)
        return false;

    // When we are too small, start hiding and disabling the grab (this reduce visual noise on very small window and facilitate using the window resize grab)
    float alpha = 1.0f;
    if ((axis == ImGuiAxis_Y) && bb_frame_height < g.FontSize + g.Style.FramePadding.y * 2.0f)
        alpha = ImSaturate((bb_frame_height - g.FontSize) / (g.Style.FramePadding.y * 2.0f));
    if (alpha <= 0.0f)
        return false;

    const ImGuiStyle& style = g.Style;
    const bool allow_interaction = (alpha >= 1.0f);

    ImRect bb = bb_frame;
    bb.Expand(ImVec2(-ImClamp(IM_FLOOR((bb_frame_width - 2.0f) * 0.5f), 0.0f, 3.0f), -ImClamp(IM_FLOOR((bb_frame_height - 2.0f) * 0.5f), 0.0f, 3.0f)));

    // V denote the main, longer axis of the scrollbar (= height for a vertical scrollbar)
    const float scrollbar_size_v = (axis == ImGuiAxis_X) ? bb.GetWidth() : bb.GetHeight();

    // Calculate the height of our grabbable box. It generally represent the amount visible (vs the total scrollable amount)
    // But we maintain a minimum size in pixel to allow for the user to still aim inside.
    IM_ASSERT(ImMax(size_contents_v, size_avail_v) > 0.0f); // Adding this assert to check if the ImMax(XXX,1.0f) is still needed. PLEASE CONTACT ME if this triggers.
    const float win_size_v = ImMax(ImMax(size_contents_v, size_avail_v), 1.0f);
    const float grab_h_pixels = ImClamp(scrollbar_size_v * (size_avail_v / win_size_v), style.GrabMinSize, scrollbar_size_v);
    const float grab_h_norm = grab_h_pixels / scrollbar_size_v;

    // Handle input right away. None of the code of Begin() is relying on scrolling position before calling Scrollbar().
    bool held = false;
    bool hovered = false;
    ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_NoNavFocus);

    float scroll_max = ImMax(1.0f, size_contents_v - size_avail_v);
    float scroll_ratio = ImSaturate(*p_scroll_v / scroll_max);
    float grab_v_norm = scroll_ratio * (scrollbar_size_v - grab_h_pixels) / scrollbar_size_v; // Grab position in normalized space
    if (held && allow_interaction && grab_h_norm < 1.0f)
    {
        float scrollbar_pos_v = bb.Min[axis];
        float mouse_pos_v = g.IO.MousePos[axis];

        // Click position in scrollbar normalized space (0.0f->1.0f)
        const float clicked_v_norm = ImSaturate((mouse_pos_v - scrollbar_pos_v) / scrollbar_size_v);
        SetHoveredID(id);

        bool seek_absolute = false;
        if (g.ActiveIdIsJustActivated)
        {
            // On initial click calculate the distance between mouse and the center of the grab
            seek_absolute = (clicked_v_norm < grab_v_norm || clicked_v_norm > grab_v_norm + grab_h_norm);
            if (seek_absolute)
                g.ScrollbarClickDeltaToGrabCenter = 0.0f;
            else
                g.ScrollbarClickDeltaToGrabCenter = clicked_v_norm - grab_v_norm - grab_h_norm * 0.5f;
        }

        // Apply scroll (p_scroll_v will generally point on one member of window->Scroll)
        // It is ok to modify Scroll here because we are being called in Begin() after the calculation of ContentSize and before setting up our starting position
        const float scroll_v_norm = ImSaturate((clicked_v_norm - g.ScrollbarClickDeltaToGrabCenter - grab_h_norm * 0.5f) / (1.0f - grab_h_norm));
        *p_scroll_v = IM_ROUND(scroll_v_norm * scroll_max);//(win_size_contents_v - win_size_v));

        // Update values for rendering
        scroll_ratio = ImSaturate(*p_scroll_v / scroll_max);
        grab_v_norm = scroll_ratio * (scrollbar_size_v - grab_h_pixels) / scrollbar_size_v;

        // Update distance to grab now that we have seeked and saturated
        if (seek_absolute)
            g.ScrollbarClickDeltaToGrabCenter = clicked_v_norm - grab_v_norm - grab_h_norm * 0.5f;
    }

    // Render
    const ImU32 bg_col = GetColorU32(ImGuiCol_ScrollbarBg);
    const ImU32 grab_col = GetColorU32(held ? ImGuiCol_ScrollbarGrabActive : hovered ? ImGuiCol_ScrollbarGrabHovered : ImGuiCol_ScrollbarGrab, alpha);
    window->DrawList->AddRectFilled(bb_frame.Min, bb_frame.Max, bg_col, window->WindowRounding, rounding_corners);
    ImRect grab_rect;
    if (axis == ImGuiAxis_X)
        grab_rect = ImRect(ImLerp(bb.Min.x, bb.Max.x, grab_v_norm), bb.Min.y, ImLerp(bb.Min.x, bb.Max.x, grab_v_norm) + grab_h_pixels, bb.Max.y);
    else
        grab_rect = ImRect(bb.Min.x, ImLerp(bb.Min.y, bb.Max.y, grab_v_norm), bb.Max.x, ImLerp(bb.Min.y, bb.Max.y, grab_v_norm) + grab_h_pixels);
    window->DrawList->AddRectFilled(grab_rect.Min, grab_rect.Max, grab_col, style.ScrollbarRounding);

    return held;
}

void ImGui::Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    if (border_col.w > 0.0f)
        bb.Max += ImVec2(2, 2);
    ItemSize(bb);
    if (!ItemAdd(bb, 0))
        return;

    if (border_col.w > 0.0f)
    {
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(border_col), 0.0f);
        window->DrawList->AddImage(user_texture_id, bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), uv0, uv1, GetColorU32(tint_col));
    } else
    {
        window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max, uv0, uv1, GetColorU32(tint_col));
    }
}

// frame_padding < 0: uses FramePadding from style (default)
// frame_padding = 0: no framing
// frame_padding > 0: set framing size
// The color used are the button colors.
bool ImGui::ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    // Default to using texture ID as ID. User can still push string/integer prefixes.
    // We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
    PushID((void*)(intptr_t)user_texture_id);
    const ImGuiID id = window->GetID("#image");
    PopID();

    const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
    const ImRect image_bb(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size);
    ItemSize(bb);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
    if (bg_col.w > 0.0f)
        window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));
    window->DrawList->AddImage(user_texture_id, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

    return pressed;
}

bool ImGui::Checkbox(const char* label, bool* v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float square_sz = GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        MarkItemEdited(id);
    }

    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
    RenderNavHighlight(total_bb, id);
    RenderFrame(check_bb.Min, check_bb.Max, GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
    ImU32 check_col = GetColorU32(ImGuiCol_CheckMark);
    if (window->DC.ItemFlags & ImGuiItemFlags_MixedValue)
    {
        // Undocumented tristate/mixed/indeterminate checkbox (#2644)
        ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
        window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
    }
    else if (*v)
    {
        const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
        //RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
        window->DrawList->AddRectFilledMultiColor(check_bb.Min + ImVec2(pad, pad), check_bb.Max - ImVec2(pad, pad), check_col, check_col, check_col, check_col);
    }

    if (g.LogEnabled)
        LogRenderedText(&total_bb.Min, *v ? "[x]" : "[ ]");
    if (label_size.x > 0.0f)
        RenderText(ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

bool ImGui::CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value)
{
    bool v = ((*flags & flags_value) == flags_value);
    bool pressed = Checkbox(label, &v);
    if (pressed)
    {
        if (v)
            *flags |= flags_value;
        else
            *flags &= ~flags_value;
    }

    return pressed;
}

bool ImGui::RadioButton(const char* label, bool active)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float square_sz = GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
    const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    ImVec2 center = check_bb.GetCenter();
    center.x = IM_ROUND(center.x);
    center.y = IM_ROUND(center.y);
    const float radius = (square_sz - 1.0f) * 0.5f;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
        MarkItemEdited(id);

    RenderNavHighlight(total_bb, id);
    window->DrawList->AddCircleFilled(center, radius, GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 16);
    if (active)
    {
        const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
        window->DrawList->AddCircleFilled(center, radius - pad, GetColorU32(ImGuiCol_CheckMark), 16);
    }

    if (style.FrameBorderSize > 0.0f)
    {
        window->DrawList->AddCircle(center + ImVec2(1, 1), radius, GetColorU32(ImGuiCol_BorderShadow), 16, style.FrameBorderSize);
        window->DrawList->AddCircle(center, radius, GetColorU32(ImGuiCol_Border), 16, style.FrameBorderSize);
    }

    if (g.LogEnabled)
        LogRenderedText(&total_bb.Min, active ? "(x)" : "( )");
    if (label_size.x > 0.0f)
        RenderText(ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return pressed;
}

// FIXME: This would work nicely if it was a public template, e.g. 'template<T> RadioButton(const char* label, T* v, T v_button)', but I'm not sure how we would expose it..
bool ImGui::RadioButton(const char* label, int* v, int v_button)
{
    const bool pressed = RadioButton(label, *v == v_button);
    if (pressed)
        *v = v_button;
    return pressed;
}

// size_arg (for each axis) < 0.0f: align to end, 0.0f: auto, > 0.0f: specified size
void ImGui::ProgressBar(float fraction, const ImVec2& size_arg, const char* overlay)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f);
    ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    fraction = ImSaturate(fraction);
    RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
    const ImVec2 fill_br = ImVec2(ImLerp(bb.Min.x, bb.Max.x, fraction), bb.Max.y);
    RenderRectFilledRangeH(window->DrawList, bb, GetColorU32(ImGuiCol_PlotHistogram), 0.0f, fraction, style.FrameRounding);

    // Default displaying the fraction as percentage string, but user can override it
    char overlay_buf[32];
    if (!overlay)
    {
        ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", fraction * 100 + 0.01f);
        overlay = overlay_buf;
    }

    ImVec2 overlay_size = CalcTextSize(overlay, NULL);
    if (overlay_size.x > 0.0f)
        RenderTextClipped(ImVec2(ImClamp(fill_br.x + style.ItemSpacing.x, bb.Min.x, bb.Max.x - overlay_size.x - style.ItemInnerSpacing.x), bb.Min.y), bb.Max, overlay, NULL, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
}

void ImGui::Bullet()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float line_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + g.Style.FramePadding.y * 2), g.FontSize);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(g.FontSize, line_height));
    ItemSize(bb);
    if (!ItemAdd(bb, 0))
    {
        SameLine(0, style.FramePadding.x * 2);
        return;
    }

    // Render and stay on same line
    ImU32 text_col = GetColorU32(ImGuiCol_Text);
    RenderBullet(window->DrawList, bb.Min + ImVec2(style.FramePadding.x + g.FontSize * 0.5f, line_height * 0.5f), text_col);
    SameLine(0, style.FramePadding.x * 2.0f);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Low-level Layout helpers
//-------------------------------------------------------------------------
// - Spacing()
// - Dummy()
// - NewLine()
// - AlignTextToFramePadding()
// - SeparatorEx() [Internal]
// - Separator()
// - SplitterBehavior() [Internal]
// - ShrinkWidths() [Internal]
//-------------------------------------------------------------------------

void ImGui::Spacing()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    ItemSize(ImVec2(0, 0));
}

void ImGui::Dummy(const ImVec2& size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(size);
    ItemAdd(bb, 0);
}

void ImGui::NewLine()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiLayoutType backup_layout_type = window->DC.LayoutType;
    window->DC.LayoutType = ImGuiLayoutType_Vertical;
    if (window->DC.CurrLineSize.y > 0.0f)     // In the event that we are on a line with items that is smaller that FontSize high, we will preserve its height.
        ItemSize(ImVec2(0, 0));
    else
        ItemSize(ImVec2(0.0f, g.FontSize));
    window->DC.LayoutType = backup_layout_type;
}

void ImGui::AlignTextToFramePadding()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    window->DC.CurrLineSize.y = ImMax(window->DC.CurrLineSize.y, g.FontSize + g.Style.FramePadding.y * 2);
    window->DC.CurrLineTextBaseOffset = ImMax(window->DC.CurrLineTextBaseOffset, g.Style.FramePadding.y);
}

// Horizontal/vertical separating line
void ImGui::SeparatorEx(ImGuiSeparatorFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    IM_ASSERT(ImIsPowerOfTwo(flags & (ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_Vertical)));   // Check that only 1 option is selected

    float thickness_draw = 1.0f;
    float thickness_layout = 0.0f;
    if (flags & ImGuiSeparatorFlags_Vertical)
    {
        // Vertical separator, for menu bars (use current line height). Not exposed because it is misleading and it doesn't have an effect on regular layout.
        float y1 = window->DC.CursorPos.y;
        float y2 = window->DC.CursorPos.y + window->DC.CurrLineSize.y;
        const ImRect bb(ImVec2(window->DC.CursorPos.x, y1), ImVec2(window->DC.CursorPos.x + thickness_draw, y2));
        ItemSize(ImVec2(thickness_layout, 0.0f));
        if (!ItemAdd(bb, 0))
            return;

        // Draw
        window->DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Min.x, bb.Max.y), GetColorU32(ImGuiCol_Separator));
        if (g.LogEnabled)
            LogText(" |");
    } else if (flags & ImGuiSeparatorFlags_Horizontal)
    {
        // Horizontal Separator
        float x1 = window->Pos.x;
        float x2 = window->Pos.x + window->Size.x;
        if (!window->DC.GroupStack.empty())
            x1 += window->DC.Indent.x;

        ImGuiColumns* columns = (flags & ImGuiSeparatorFlags_SpanAllColumns) ? window->DC.CurrentColumns : NULL;
        if (columns)
            PushColumnsBackground();

        // We don't provide our width to the layout so that it doesn't get feed back into AutoFit
        const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y + thickness_draw));
        ItemSize(ImVec2(0.0f, thickness_layout));
        const bool item_visible = ItemAdd(bb, 0);
        if (item_visible)
        {
            // Draw
            window->DrawList->AddLine(bb.Min, ImVec2(bb.Max.x, bb.Min.y), GetColorU32(ImGuiCol_Separator));
            if (g.LogEnabled)
                LogRenderedText(&bb.Min, "--------------------------------");
        }
        if (columns)
        {
            PopColumnsBackground();
            columns->LineMinY = window->DC.CursorPos.y;
        }
    }
}

void ImGui::Separator()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    // Those flags should eventually be overridable by the user
    ImGuiSeparatorFlags flags = (window->DC.LayoutType == ImGuiLayoutType_Horizontal) ? ImGuiSeparatorFlags_Vertical : ImGuiSeparatorFlags_Horizontal;
    flags |= ImGuiSeparatorFlags_SpanAllColumns;
    SeparatorEx(flags);
}

// Using 'hover_visibility_delay' allows us to hide the highlight and mouse cursor for a short time, which can be convenient to reduce visual noise.
bool ImGui::SplitterBehavior(const ImRect& bb, ImGuiID id, ImGuiAxis axis, float* size1, float* size2, float min_size1, float min_size2, float hover_extend, float hover_visibility_delay)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImGuiItemFlags item_flags_backup = window->DC.ItemFlags;
    window->DC.ItemFlags |= ImGuiItemFlags_NoNav | ImGuiItemFlags_NoNavDefaultFocus;
    bool item_add = ItemAdd(bb, id);
    window->DC.ItemFlags = item_flags_backup;
    if (!item_add)
        return false;

    bool hovered, held;
    ImRect bb_interact = bb;
    bb_interact.Expand(axis == ImGuiAxis_Y ? ImVec2(0.0f, hover_extend) : ImVec2(hover_extend, 0.0f));
    ButtonBehavior(bb_interact, id, &hovered, &held, ImGuiButtonFlags_FlattenChildren | ImGuiButtonFlags_AllowItemOverlap);
    if (g.ActiveId != id)
        SetItemAllowOverlap();

    if (held || (g.HoveredId == id && g.HoveredIdPreviousFrame == id && g.HoveredIdTimer >= hover_visibility_delay))
        SetMouseCursor(axis == ImGuiAxis_Y ? ImGuiMouseCursor_ResizeNS : ImGuiMouseCursor_ResizeEW);

    ImRect bb_render = bb;
    if (held)
    {
        ImVec2 mouse_delta_2d = g.IO.MousePos - g.ActiveIdClickOffset - bb_interact.Min;
        float mouse_delta = (axis == ImGuiAxis_Y) ? mouse_delta_2d.y : mouse_delta_2d.x;

        // Minimum pane size
        float size_1_maximum_delta = ImMax(0.0f, *size1 - min_size1);
        float size_2_maximum_delta = ImMax(0.0f, *size2 - min_size2);
        if (mouse_delta < -size_1_maximum_delta)
            mouse_delta = -size_1_maximum_delta;
        if (mouse_delta > size_2_maximum_delta)
            mouse_delta = size_2_maximum_delta;

        // Apply resize
        if (mouse_delta != 0.0f)
        {
            if (mouse_delta < 0.0f)
                IM_ASSERT(*size1 + mouse_delta >= min_size1);
            if (mouse_delta > 0.0f)
                IM_ASSERT(*size2 - mouse_delta >= min_size2);
            *size1 += mouse_delta;
            *size2 -= mouse_delta;
            bb_render.Translate((axis == ImGuiAxis_X) ? ImVec2(mouse_delta, 0.0f) : ImVec2(0.0f, mouse_delta));
            MarkItemEdited(id);
        }
    }

    // Render
    const ImU32 col = GetColorU32(held ? ImGuiCol_SeparatorActive : (hovered && g.HoveredIdTimer >= hover_visibility_delay) ? ImGuiCol_SeparatorHovered : ImGuiCol_Separator);
    window->DrawList->AddRectFilled(bb_render.Min, bb_render.Max, col, 0.0f);

    return held;
}

static int IMGUI_CDECL ShrinkWidthItemComparer(const void* lhs, const void* rhs)
{
    const ImGuiShrinkWidthItem* a = (const ImGuiShrinkWidthItem*)lhs;
    const ImGuiShrinkWidthItem* b = (const ImGuiShrinkWidthItem*)rhs;
    if (int d = (int)(b->Width - a->Width))
        return d;
    return (b->Index - a->Index);
}

// Shrink excess width from a set of item, by removing width from the larger items first.
void ImGui::ShrinkWidths(ImGuiShrinkWidthItem* items, int count, float width_excess)
{
    if (count == 1)
    {
        items[0].Width = ImMax(items[0].Width - width_excess, 1.0f);
        return;
    }
    ImQsort(items, (size_t)count, sizeof(ImGuiShrinkWidthItem), ShrinkWidthItemComparer);
    int count_same_width = 1;
    while (width_excess > 0.0f && count_same_width < count)
    {
        while (count_same_width < count && items[0].Width <= items[count_same_width].Width)
            count_same_width++;
        float max_width_to_remove_per_item = (count_same_width < count) ? (items[0].Width - items[count_same_width].Width) : (items[0].Width - 1.0f);
        float width_to_remove_per_item = ImMin(width_excess / count_same_width, max_width_to_remove_per_item);
        for (int item_n = 0; item_n < count_same_width; item_n++)
            items[item_n].Width -= width_to_remove_per_item;
        width_excess -= width_to_remove_per_item * count_same_width;
    }

    // Round width and redistribute remainder left-to-right (could make it an option of the function?)
    // Ensure that e.g. the right-most tab of a shrunk tab-bar always reaches exactly at the same distance from the right-most edge of the tab bar separator.
    width_excess = 0.0f;
    for (int n = 0; n < count; n++)
    {
        float width_rounded = ImFloor(items[n].Width);
        width_excess += items[n].Width - width_rounded;
        items[n].Width = width_rounded;
    }
    if (width_excess > 0.0f)
        for (int n = 0; n < count; n++)
            if (items[n].Index < (int)(width_excess + 0.01f))
                items[n].Width += 1.0f;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: ComboBox
//-------------------------------------------------------------------------
// - BeginCombo()
// - EndCombo()
// - Combo()
//-------------------------------------------------------------------------

static float CalcMaxPopupHeightFromItemCount(int items_count)
{
    ImGuiContext& g = *GImGui;
    if (items_count <= 0)
        return FLT_MAX;
    return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool ImGui::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
    // Always consume the SetNextWindowSizeConstraint() call in our early return paths
    ImGuiContext& g = *GImGui;
    bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
    g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const float expected_w = CalcItemWidth();
    const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held);
    bool popup_open = IsPopupOpen(id);

    const ImU32 frame_col = GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);
    RenderNavHighlight(frame_bb, id);
    if (!(flags & ImGuiComboFlags_NoPreview))
        window->DrawList->AddRectFilled(frame_bb.Min, ImVec2(value_x2, frame_bb.Max.y), frame_col, style.FrameRounding, (flags & ImGuiComboFlags_NoArrowButton) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Left);
    if (!(flags & ImGuiComboFlags_NoArrowButton))
    {
        ImU32 bg_col = GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        ImU32 text_col = GetColorU32(ImGuiCol_Text);
        window->DrawList->AddRectFilled(ImVec2(value_x2, frame_bb.Min.y), frame_bb.Max, bg_col, style.FrameRounding, (w <= arrow_size) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Right);
        if (value_x2 + arrow_size - style.FramePadding.x <= frame_bb.Max.x)
            RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, frame_bb.Min.y + style.FramePadding.y), text_col, ImGuiDir_Down, 1.0f);
    }
    RenderFrameBorder(frame_bb.Min, frame_bb.Max, style.FrameRounding);
    if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
        RenderTextClipped(frame_bb.Min + style.FramePadding, ImVec2(value_x2, frame_bb.Max.y), preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));
    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    if ((pressed || g.NavActivateId == id) && !popup_open)
    {
        if (window->DC.NavLayerCurrent == 0)
            window->NavLastIds[0] = id;
        OpenPopupEx(id);
        popup_open = true;
    }

    if (!popup_open)
        return false;

    if (has_window_size_constraint)
    {
        g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
    } else
    {
        if ((flags & ImGuiComboFlags_HeightMask_) == 0)
            flags |= ImGuiComboFlags_HeightRegular;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));    // Only one
        int popup_max_height_in_items = -1;
        if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
        else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
        else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
        SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
    }

    char name[16];
    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

    // Peak into expected window size so we can position it
    if (ImGuiWindow* popup_window = FindWindowByName(name))
        if (popup_window->WasActive)
        {
            ImVec2 size_expected = CalcWindowExpectedSize(popup_window);
            if (flags & ImGuiComboFlags_PopupAlignLeft)
                popup_window->AutoPosLastDirection = ImGuiDir_Left;
            ImRect r_outer = GetWindowAllowedExtentRect(popup_window);
            ImVec2 pos = FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
            SetNextWindowPos(pos);
        }

    // We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

    // Horizontally align ourselves with the framed text
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
    bool ret = Begin(name, NULL, window_flags);
    PopStyleVar();
    if (!ret)
    {
        EndPopup();
        IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
        return false;
    }
    return true;
}

void ImGui::EndCombo()
{
    EndPopup();
}

// Getter for the old Combo() API: const char*[]
static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}

// Getter for the old Combo() API: "item1\0item2\0item3\0"
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

// Old API, prefer using BeginCombo() nowadays if you can.
bool ImGui::Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
    ImGuiContext& g = *GImGui;

    // Call the getter to obtain the preview string which is a parameter to BeginCombo()
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        items_getter(data, *current_item, &preview_value);

    // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

    if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
        return false;

    // Display items
    // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        PushID((void*)(intptr_t)i);
        const bool item_selected = (i == *current_item);
        const char* item_text;
        if (!items_getter(data, i, &item_text))
            item_text = "*Unknown item*";
        if (Selectable(item_text, item_selected))
        {
            value_changed = true;
            *current_item = i;
        }
        if (item_selected)
            SetItemDefaultFocus();
        PopID();
    }

    EndCombo();
    return value_changed;
}

// Combo box helper allowing to pass an array of strings.
bool ImGui::Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
    const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
    return value_changed;
}

// Combo box helper allowing to pass all items in a single string literal holding multiple zero-terminated items "item1\0item2\0"
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

//-------------------------------------------------------------------------
// [SECTION] Data Type and Data Formatting Helpers [Internal]
//-------------------------------------------------------------------------
// - PatchFormatStringFloatToInt()
// - DataTypeGetInfo()
// - DataTypeFormatString()
// - DataTypeApplyOp()
// - DataTypeApplyOpFromText()
// - DataTypeClamp()
// - GetMinimumStepAtDecimalPrecision
// - RoundScalarWithFormat<>()
//-------------------------------------------------------------------------

static const ImGuiDataTypeInfo GDataTypeInfo[] =
{
    { sizeof(char),             "%d",   "%d"    },  // ImGuiDataType_S8
    { sizeof(unsigned char),    "%u",   "%u"    },
    { sizeof(short),            "%d",   "%d"    },  // ImGuiDataType_S16
    { sizeof(unsigned short),   "%u",   "%u"    },
    { sizeof(int),              "%d",   "%d"    },  // ImGuiDataType_S32
    { sizeof(unsigned int),     "%u",   "%u"    },
#ifdef _MSC_VER
    { sizeof(ImS64),            "%I64d","%I64d" },  // ImGuiDataType_S64
    { sizeof(ImU64),            "%I64u","%I64u" },
#else
    { sizeof(ImS64),            "%lld", "%lld"  },  // ImGuiDataType_S64
    { sizeof(ImU64),            "%llu", "%llu"  },
#endif
    { sizeof(float),            "%f",   "%f"    },  // ImGuiDataType_Float (float are promoted to double in va_arg)
    { sizeof(double),           "%f",   "%lf"   },  // ImGuiDataType_Double
};
IM_STATIC_ASSERT(IM_ARRAYSIZE(GDataTypeInfo) == ImGuiDataType_COUNT);

// FIXME-LEGACY: Prior to 1.61 our DragInt() function internally used floats and because of this the compile-time default value for format was "%.0f".
// Even though we changed the compile-time default, we expect users to have carried %f around, which would break the display of DragInt() calls.
// To honor backward compatibility we are rewriting the format string, unless IMGUI_DISABLE_OBSOLETE_FUNCTIONS is enabled. What could possibly go wrong?!
static const char* PatchFormatStringFloatToInt(const char* fmt)
{
    if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
        return "%d";
    const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
    if (fmt_end > fmt_start && fmt_end[-1] == 'f')
    {
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        if (fmt_start == fmt && fmt_end[0] == 0)
            return "%d";
        ImGuiContext& g = *GImGui;
        ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
        return g.TempBuffer;
#else
        IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
    }
    return fmt;
}

const ImGuiDataTypeInfo* ImGui::DataTypeGetInfo(ImGuiDataType data_type)
{
    IM_ASSERT(data_type >= 0 && data_type < ImGuiDataType_COUNT);
    return &GDataTypeInfo[data_type];
}

int ImGui::DataTypeFormatString(char* buf, int buf_size, ImGuiDataType data_type, const void* p_data, const char* format)
{
    // Signedness doesn't matter when pushing integer arguments
    if (data_type == ImGuiDataType_S32 || data_type == ImGuiDataType_U32)
        return ImFormatString(buf, buf_size, format, *(const ImU32*)p_data);
    if (data_type == ImGuiDataType_S64 || data_type == ImGuiDataType_U64)
        return ImFormatString(buf, buf_size, format, *(const ImU64*)p_data);
    if (data_type == ImGuiDataType_Float)
        return ImFormatString(buf, buf_size, format, *(const float*)p_data);
    if (data_type == ImGuiDataType_Double)
        return ImFormatString(buf, buf_size, format, *(const double*)p_data);
    if (data_type == ImGuiDataType_S8)
        return ImFormatString(buf, buf_size, format, *(const ImS8*)p_data);
    if (data_type == ImGuiDataType_U8)
        return ImFormatString(buf, buf_size, format, *(const ImU8*)p_data);
    if (data_type == ImGuiDataType_S16)
        return ImFormatString(buf, buf_size, format, *(const ImS16*)p_data);
    if (data_type == ImGuiDataType_U16)
        return ImFormatString(buf, buf_size, format, *(const ImU16*)p_data);
    IM_ASSERT(0);
    return 0;
}

void ImGui::DataTypeApplyOp(ImGuiDataType data_type, int op, void* output, void* arg1, const void* arg2)
{
    IM_ASSERT(op == '+' || op == '-');
    switch (data_type)
    {
    case ImGuiDataType_S8:
        if (op == '+') { *(ImS8*)output = ImAddClampOverflow(*(const ImS8*)arg1, *(const ImS8*)arg2, IM_S8_MIN, IM_S8_MAX); }
        if (op == '-') { *(ImS8*)output = ImSubClampOverflow(*(const ImS8*)arg1, *(const ImS8*)arg2, IM_S8_MIN, IM_S8_MAX); }
        return;
    case ImGuiDataType_U8:
        if (op == '+') { *(ImU8*)output = ImAddClampOverflow(*(const ImU8*)arg1, *(const ImU8*)arg2, IM_U8_MIN, IM_U8_MAX); }
        if (op == '-') { *(ImU8*)output = ImSubClampOverflow(*(const ImU8*)arg1, *(const ImU8*)arg2, IM_U8_MIN, IM_U8_MAX); }
        return;
    case ImGuiDataType_S16:
        if (op == '+') { *(ImS16*)output = ImAddClampOverflow(*(const ImS16*)arg1, *(const ImS16*)arg2, IM_S16_MIN, IM_S16_MAX); }
        if (op == '-') { *(ImS16*)output = ImSubClampOverflow(*(const ImS16*)arg1, *(const ImS16*)arg2, IM_S16_MIN, IM_S16_MAX); }
        return;
    case ImGuiDataType_U16:
        if (op == '+') { *(ImU16*)output = ImAddClampOverflow(*(const ImU16*)arg1, *(const ImU16*)arg2, IM_U16_MIN, IM_U16_MAX); }
        if (op == '-') { *(ImU16*)output = ImSubClampOverflow(*(const ImU16*)arg1, *(const ImU16*)arg2, IM_U16_MIN, IM_U16_MAX); }
        return;
    case ImGuiDataType_S32:
        if (op == '+') { *(ImS32*)output = ImAddClampOverflow(*(const ImS32*)arg1, *(const ImS32*)arg2, IM_S32_MIN, IM_S32_MAX); }
        if (op == '-') { *(ImS32*)output = ImSubClampOverflow(*(const ImS32*)arg1, *(const ImS32*)arg2, IM_S32_MIN, IM_S32_MAX); }
        return;
    case ImGuiDataType_U32:
        if (op == '+') { *(ImU32*)output = ImAddClampOverflow(*(const ImU32*)arg1, *(const ImU32*)arg2, IM_U32_MIN, IM_U32_MAX); }
        if (op == '-') { *(ImU32*)output = ImSubClampOverflow(*(const ImU32*)arg1, *(const ImU32*)arg2, IM_U32_MIN, IM_U32_MAX); }
        return;
    case ImGuiDataType_S64:
        if (op == '+') { *(ImS64*)output = ImAddClampOverflow(*(const ImS64*)arg1, *(const ImS64*)arg2, IM_S64_MIN, IM_S64_MAX); }
        if (op == '-') { *(ImS64*)output = ImSubClampOverflow(*(const ImS64*)arg1, *(const ImS64*)arg2, IM_S64_MIN, IM_S64_MAX); }
        return;
    case ImGuiDataType_U64:
        if (op == '+') { *(ImU64*)output = ImAddClampOverflow(*(const ImU64*)arg1, *(const ImU64*)arg2, IM_U64_MIN, IM_U64_MAX); }
        if (op == '-') { *(ImU64*)output = ImSubClampOverflow(*(const ImU64*)arg1, *(const ImU64*)arg2, IM_U64_MIN, IM_U64_MAX); }
        return;
    case ImGuiDataType_Float:
        if (op == '+') { *(float*)output = *(const float*)arg1 + *(const float*)arg2; }
        if (op == '-') { *(float*)output = *(const float*)arg1 - *(const float*)arg2; }
        return;
    case ImGuiDataType_Double:
        if (op == '+') { *(double*)output = *(const double*)arg1 + *(const double*)arg2; }
        if (op == '-') { *(double*)output = *(const double*)arg1 - *(const double*)arg2; }
        return;
    case ImGuiDataType_COUNT: break;
    }
    IM_ASSERT(0);
}

// User can input math operators (e.g. +100) to edit a numerical values.
// NB: This is _not_ a full expression evaluator. We should probably add one and replace this dumb mess..
bool ImGui::DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, ImGuiDataType data_type, void* p_data, const char* format)
{
    while (ImCharIsBlankA(*buf))
        buf++;

    // We don't support '-' op because it would conflict with inputing negative value.
    // Instead you can use +-100 to subtract from an existing value
    char op = buf[0];
    if (op == '+' || op == '*' || op == '/')
    {
        buf++;
        while (ImCharIsBlankA(*buf))
            buf++;
    } else
    {
        op = 0;
    }
    if (!buf[0])
        return false;

    // Copy the value in an opaque buffer so we can compare at the end of the function if it changed at all.
    const ImGuiDataTypeInfo* type_info = DataTypeGetInfo(data_type);
    ImGuiDataTypeTempStorage data_backup;
    memcpy(&data_backup, p_data, type_info->Size);

    if (format == NULL)
        format = type_info->ScanFmt;

    // FIXME-LEGACY: The aim is to remove those operators and write a proper expression evaluator at some point..
    int arg1i = 0;
    if (data_type == ImGuiDataType_S32)
    {
        int* v = (int*)p_data;
        int arg0i = *v;
        float arg1f = 0.0f;
        if (op && sscanf(initial_value_buf, format, &arg0i) < 1)
            return false;
        // Store operand in a float so we can use fractional value for multipliers (*1.1), but constant always parsed as integer so we can fit big integers (e.g. 2000000003) past float precision
        if (op == '+') { if (sscanf(buf, "%d", &arg1i)) *v = (int)(arg0i + arg1i); }                   // Add (use "+-" to subtract)
        else if (op == '*') { if (sscanf(buf, "%f", &arg1f)) *v = (int)(arg0i * arg1f); }                   // Multiply
        else if (op == '/') { if (sscanf(buf, "%f", &arg1f) && arg1f != 0.0f) *v = (int)(arg0i / arg1f); }  // Divide
        else { if (sscanf(buf, format, &arg1i) == 1) *v = arg1i; }                           // Assign constant
    } else if (data_type == ImGuiDataType_Float)
    {
        // For floats we have to ignore format with precision (e.g. "%.2f") because sscanf doesn't take them in
        format = "%f";
        float* v = (float*)p_data;
        float arg0f = *v, arg1f = 0.0f;
        if (op && sscanf(initial_value_buf, format, &arg0f) < 1)
            return false;
        if (sscanf(buf, format, &arg1f) < 1)
            return false;
        if (op == '+') { *v = arg0f + arg1f; }                    // Add (use "+-" to subtract)
        else if (op == '*') { *v = arg0f * arg1f; }                    // Multiply
        else if (op == '/') { if (arg1f != 0.0f) *v = arg0f / arg1f; } // Divide
        else { *v = arg1f; }                            // Assign constant
    } else if (data_type == ImGuiDataType_Double)
    {
        format = "%lf"; // scanf differentiate float/double unlike printf which forces everything to double because of ellipsis
        double* v = (double*)p_data;
        double arg0f = *v, arg1f = 0.0;
        if (op && sscanf(initial_value_buf, format, &arg0f) < 1)
            return false;
        if (sscanf(buf, format, &arg1f) < 1)
            return false;
        if (op == '+') { *v = arg0f + arg1f; }                    // Add (use "+-" to subtract)
        else if (op == '*') { *v = arg0f * arg1f; }                    // Multiply
        else if (op == '/') { if (arg1f != 0.0f) *v = arg0f / arg1f; } // Divide
        else { *v = arg1f; }                            // Assign constant
    } else if (data_type == ImGuiDataType_U32 || data_type == ImGuiDataType_S64 || data_type == ImGuiDataType_U64)
    {
        // All other types assign constant
        // We don't bother handling support for legacy operators since they are a little too crappy. Instead we will later implement a proper expression evaluator in the future.
        sscanf(buf, format, p_data);
    } else
    {
        // Small types need a 32-bit buffer to receive the result from scanf()
        int v32;
        sscanf(buf, format, &v32);
        if (data_type == ImGuiDataType_S8)
            *(ImS8*)p_data = (ImS8)ImClamp(v32, (int)IM_S8_MIN, (int)IM_S8_MAX);
        else if (data_type == ImGuiDataType_U8)
            *(ImU8*)p_data = (ImU8)ImClamp(v32, (int)IM_U8_MIN, (int)IM_U8_MAX);
        else if (data_type == ImGuiDataType_S16)
            *(ImS16*)p_data = (ImS16)ImClamp(v32, (int)IM_S16_MIN, (int)IM_S16_MAX);
        else if (data_type == ImGuiDataType_U16)
            *(ImU16*)p_data = (ImU16)ImClamp(v32, (int)IM_U16_MIN, (int)IM_U16_MAX);
        else
            IM_ASSERT(0);
    }

    return memcmp(&data_backup, p_data, type_info->Size) != 0;
}

template<typename T>
static bool ClampBehaviorT(T* v, T v_min, T v_max)
{
    if (*v < v_min) { *v = v_min; return true; }
    if (*v > v_max) { *v = v_max; return true; }
    return false;
}

bool ImGui::DataTypeClamp(ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max)
{
    switch (data_type)
    {
    case ImGuiDataType_S8:     return ClampBehaviorT<ImS8  >((ImS8*)p_data, *(const ImS8*)p_min, *(const ImS8*)p_max);
    case ImGuiDataType_U8:     return ClampBehaviorT<ImU8  >((ImU8*)p_data, *(const ImU8*)p_min, *(const ImU8*)p_max);
    case ImGuiDataType_S16:    return ClampBehaviorT<ImS16 >((ImS16*)p_data, *(const ImS16*)p_min, *(const ImS16*)p_max);
    case ImGuiDataType_U16:    return ClampBehaviorT<ImU16 >((ImU16*)p_data, *(const ImU16*)p_min, *(const ImU16*)p_max);
    case ImGuiDataType_S32:    return ClampBehaviorT<ImS32 >((ImS32*)p_data, *(const ImS32*)p_min, *(const ImS32*)p_max);
    case ImGuiDataType_U32:    return ClampBehaviorT<ImU32 >((ImU32*)p_data, *(const ImU32*)p_min, *(const ImU32*)p_max);
    case ImGuiDataType_S64:    return ClampBehaviorT<ImS64 >((ImS64*)p_data, *(const ImS64*)p_min, *(const ImS64*)p_max);
    case ImGuiDataType_U64:    return ClampBehaviorT<ImU64 >((ImU64*)p_data, *(const ImU64*)p_min, *(const ImU64*)p_max);
    case ImGuiDataType_Float:  return ClampBehaviorT<float >((float*)p_data, *(const float*)p_min, *(const float*)p_max);
    case ImGuiDataType_Double: return ClampBehaviorT<double>((double*)p_data, *(const double*)p_min, *(const double*)p_max);
    case ImGuiDataType_COUNT:  break;
    }
    IM_ASSERT(0);
    return false;
}

static float GetMinimumStepAtDecimalPrecision(int decimal_precision)
{
    static const float min_steps[10] = { 1.0f, 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f, 0.000001f, 0.0000001f, 0.00000001f, 0.000000001f };
    if (decimal_precision < 0)
        return FLT_MIN;
    return (decimal_precision < IM_ARRAYSIZE(min_steps)) ? min_steps[decimal_precision] : ImPow(10.0f, (float)-decimal_precision);
}

template<typename TYPE>
static const char* ImAtoi(const char* src, TYPE* output)
{
    int negative = 0;
    if (*src == '-') { negative = 1; src++; }
    if (*src == '+') { src++; }
    TYPE v = 0;
    while (*src >= '0' && *src <= '9')
        v = (v * 10) + (*src++ - '0');
    *output = negative ? -v : v;
    return src;
}

template<typename TYPE, typename SIGNEDTYPE>
TYPE ImGui::RoundScalarWithFormatT(const char* format, ImGuiDataType data_type, TYPE v)
{
    const char* fmt_start = ImParseFormatFindStart(format);
    if (fmt_start[0] != '%' || fmt_start[1] == '%') // Don't apply if the value is not visible in the format string
        return v;
    char v_str[64];
    ImFormatString(v_str, IM_ARRAYSIZE(v_str), fmt_start, v);
    const char* p = v_str;
    while (*p == ' ')
        p++;
    if (data_type == ImGuiDataType_Float || data_type == ImGuiDataType_Double)
        v = (TYPE)ImAtof(p);
    else
        ImAtoi(p, (SIGNEDTYPE*)&v);
    return v;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: DragScalar, DragFloat, DragInt, etc.
//-------------------------------------------------------------------------
// - DragBehaviorT<>() [Internal]
// - DragBehavior() [Internal]
// - DragScalar()
// - DragScalarN()
// - DragFloat()
// - DragFloat2()
// - DragFloat3()
// - DragFloat4()
// - DragFloatRange2()
// - DragInt()
// - DragInt2()
// - DragInt3()
// - DragInt4()
// - DragIntRange2()
//-------------------------------------------------------------------------

// This is called by DragBehavior() when the widget is active (held by mouse or being manipulated with Nav controls)
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool ImGui::DragBehaviorT(ImGuiDataType data_type, TYPE* v, float v_speed, const TYPE v_min, const TYPE v_max, const char* format, float power, ImGuiDragFlags flags)
{
    ImGuiContext& g = *GImGui;
    const ImGuiAxis axis = (flags & ImGuiDragFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;
    const bool is_decimal = (data_type == ImGuiDataType_Float) || (data_type == ImGuiDataType_Double);
    const bool is_clamped = (v_min < v_max);
    const bool is_power = (power != 1.0f && is_decimal && is_clamped && (v_max - v_min < FLT_MAX));
    const bool is_locked = (v_min > v_max);
    if (is_locked)
        return false;

    // Default tweak speed
    if (v_speed == 0.0f && is_clamped && (v_max - v_min < FLT_MAX))
        v_speed = (float)((v_max - v_min) * g.DragSpeedDefaultRatio);

    // Inputs accumulates into g.DragCurrentAccum, which is flushed into the current value as soon as it makes a difference with our precision settings
    float adjust_delta = 0.0f;
    if (g.ActiveIdSource == ImGuiInputSource_Mouse && IsMousePosValid() && g.IO.MouseDragMaxDistanceSqr[0] > 1.0f * 1.0f)
    {
        adjust_delta = g.IO.MouseDelta[axis];
        if (g.IO.KeyAlt)
            adjust_delta *= 1.0f / 100.0f;
        if (g.IO.KeyShift)
            adjust_delta *= 10.0f;
    } else if (g.ActiveIdSource == ImGuiInputSource_Nav)
    {
        int decimal_precision = is_decimal ? ImParseFormatPrecision(format, 3) : 0;
        adjust_delta = GetNavInputAmount2d(ImGuiNavDirSourceFlags_Keyboard | ImGuiNavDirSourceFlags_PadDPad, ImGuiInputReadMode_RepeatFast, 1.0f / 10.0f, 10.0f)[axis];
        v_speed = ImMax(v_speed, GetMinimumStepAtDecimalPrecision(decimal_precision));
    }
    adjust_delta *= v_speed;

    // For vertical drag we currently assume that Up=higher value (like we do with vertical sliders). This may become a parameter.
    if (axis == ImGuiAxis_Y)
        adjust_delta = -adjust_delta;

    // Clear current value on activation
    // Avoid altering values and clamping when we are _already_ past the limits and heading in the same direction, so e.g. if range is 0..255, current value is 300 and we are pushing to the right side, keep the 300.
    bool is_just_activated = g.ActiveIdIsJustActivated;
    bool is_already_past_limits_and_pushing_outward = is_clamped && ((*v >= v_max && adjust_delta > 0.0f) || (*v <= v_min && adjust_delta < 0.0f));
    bool is_drag_direction_change_with_power = is_power && ((adjust_delta < 0 && g.DragCurrentAccum > 0) || (adjust_delta > 0 && g.DragCurrentAccum < 0));
    if (is_just_activated || is_already_past_limits_and_pushing_outward || is_drag_direction_change_with_power)
    {
        g.DragCurrentAccum = 0.0f;
        g.DragCurrentAccumDirty = false;
    } else if (adjust_delta != 0.0f)
    {
        g.DragCurrentAccum += adjust_delta;
        g.DragCurrentAccumDirty = true;
    }

    if (!g.DragCurrentAccumDirty)
        return false;

    TYPE v_cur = *v;
    FLOATTYPE v_old_ref_for_accum_remainder = (FLOATTYPE)0.0f;

    if (is_power)
    {
        // Offset + round to user desired precision, with a curve on the v_min..v_max range to get more precision on one side of the range
        FLOATTYPE v_old_norm_curved = ImPow((FLOATTYPE)(v_cur - v_min) / (FLOATTYPE)(v_max - v_min), (FLOATTYPE)1.0f / power);
        FLOATTYPE v_new_norm_curved = v_old_norm_curved + (g.DragCurrentAccum / (v_max - v_min));
        v_cur = v_min + (SIGNEDTYPE)ImPow(ImSaturate((float)v_new_norm_curved), power) * (v_max - v_min);
        v_old_ref_for_accum_remainder = v_old_norm_curved;
    } else
    {
        v_cur += (SIGNEDTYPE)g.DragCurrentAccum;
    }

    // Round to user desired precision based on format string
    v_cur = RoundScalarWithFormatT<TYPE, SIGNEDTYPE>(format, data_type, v_cur);

    // Preserve remainder after rounding has been applied. This also allow slow tweaking of values.
    g.DragCurrentAccumDirty = false;
    if (is_power)
    {
        FLOATTYPE v_cur_norm_curved = ImPow((FLOATTYPE)(v_cur - v_min) / (FLOATTYPE)(v_max - v_min), (FLOATTYPE)1.0f / power);
        g.DragCurrentAccum -= (float)(v_cur_norm_curved - v_old_ref_for_accum_remainder);
    } else
    {
        g.DragCurrentAccum -= (float)((SIGNEDTYPE)v_cur - (SIGNEDTYPE)*v);
    }

    // Lose zero sign for float/double
    if (v_cur == (TYPE)-0)
        v_cur = (TYPE)0;

    // Clamp values (+ handle overflow/wrap-around for integer types)
    if (*v != v_cur && is_clamped)
    {
        if (v_cur < v_min || (v_cur > * v && adjust_delta < 0.0f && !is_decimal))
            v_cur = v_min;
        if (v_cur > v_max || (v_cur < *v && adjust_delta > 0.0f && !is_decimal))
            v_cur = v_max;
    }

    // Apply result
    if (*v == v_cur)
        return false;
    *v = v_cur;
    return true;
}

bool ImGui::DragBehavior(ImGuiID id, ImGuiDataType data_type, void* p_v, float v_speed, const void* p_min, const void* p_max, const char* format, float power, ImGuiDragFlags flags)
{
    ImGuiContext& g = *GImGui;
    if (g.ActiveId == id)
    {
        if (g.ActiveIdSource == ImGuiInputSource_Mouse && !g.IO.MouseDown[0])
            ClearActiveID();
        else if (g.ActiveIdSource == ImGuiInputSource_Nav && g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
            ClearActiveID();
    }
    if (g.ActiveId != id)
        return false;

    switch (data_type)
    {
    case ImGuiDataType_S8: { ImS32 v32 = (ImS32) * (ImS8*)p_v;  bool r = DragBehaviorT<ImS32, ImS32, float>(ImGuiDataType_S32, &v32, v_speed, p_min ? *(const ImS8*)p_min : IM_S8_MIN, p_max ? *(const ImS8*)p_max : IM_S8_MAX, format, power, flags); if (r) *(ImS8*)p_v = (ImS8)v32; return r; }
    case ImGuiDataType_U8: { ImU32 v32 = (ImU32) * (ImU8*)p_v;  bool r = DragBehaviorT<ImU32, ImS32, float>(ImGuiDataType_U32, &v32, v_speed, p_min ? *(const ImU8*)p_min : IM_U8_MIN, p_max ? *(const ImU8*)p_max : IM_U8_MAX, format, power, flags); if (r) *(ImU8*)p_v = (ImU8)v32; return r; }
    case ImGuiDataType_S16: { ImS32 v32 = (ImS32) * (ImS16*)p_v; bool r = DragBehaviorT<ImS32, ImS32, float>(ImGuiDataType_S32, &v32, v_speed, p_min ? *(const ImS16*)p_min : IM_S16_MIN, p_max ? *(const ImS16*)p_max : IM_S16_MAX, format, power, flags); if (r) *(ImS16*)p_v = (ImS16)v32; return r; }
    case ImGuiDataType_U16: { ImU32 v32 = (ImU32) * (ImU16*)p_v; bool r = DragBehaviorT<ImU32, ImS32, float>(ImGuiDataType_U32, &v32, v_speed, p_min ? *(const ImU16*)p_min : IM_U16_MIN, p_max ? *(const ImU16*)p_max : IM_U16_MAX, format, power, flags); if (r) *(ImU16*)p_v = (ImU16)v32; return r; }
    case ImGuiDataType_S32:    return DragBehaviorT<ImS32, ImS32, float >(data_type, (ImS32*)p_v, v_speed, p_min ? *(const ImS32*)p_min : IM_S32_MIN, p_max ? *(const ImS32*)p_max : IM_S32_MAX, format, power, flags);
    case ImGuiDataType_U32:    return DragBehaviorT<ImU32, ImS32, float >(data_type, (ImU32*)p_v, v_speed, p_min ? *(const ImU32*)p_min : IM_U32_MIN, p_max ? *(const ImU32*)p_max : IM_U32_MAX, format, power, flags);
    case ImGuiDataType_S64:    return DragBehaviorT<ImS64, ImS64, double>(data_type, (ImS64*)p_v, v_speed, p_min ? *(const ImS64*)p_min : IM_S64_MIN, p_max ? *(const ImS64*)p_max : IM_S64_MAX, format, power, flags);
    case ImGuiDataType_U64:    return DragBehaviorT<ImU64, ImS64, double>(data_type, (ImU64*)p_v, v_speed, p_min ? *(const ImU64*)p_min : IM_U64_MIN, p_max ? *(const ImU64*)p_max : IM_U64_MAX, format, power, flags);
    case ImGuiDataType_Float:  return DragBehaviorT<float, float, float >(data_type, (float*)p_v, v_speed, p_min ? *(const float*)p_min : -FLT_MAX, p_max ? *(const float*)p_max : FLT_MAX, format, power, flags);
    case ImGuiDataType_Double: return DragBehaviorT<double, double, double>(data_type, (double*)p_v, v_speed, p_min ? *(const double*)p_min : -DBL_MAX, p_max ? *(const double*)p_max : DBL_MAX, format, power, flags);
    case ImGuiDataType_COUNT:  break;
    }
    IM_ASSERT(0);
    return false;
}

// Note: p_data, p_min and p_max are _pointers_ to a memory address holding the data. For a Drag widget, p_min and p_max are optional.
// Read code of e.g. SliderFloat(), SliderInt() etc. or examples in 'Demo->Widgets->Data Types' to understand how to use this function directly.
bool ImGui::DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, float power)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    if (power != 1.0f)
        IM_ASSERT(p_min != NULL && p_max != NULL); // When using a power curve the drag needs to have known bounds

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = CalcItemWidth();
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
        format = PatchFormatStringFloatToInt(format);

    // Tabbing or CTRL-clicking on Drag turns it into an input box
    const bool hovered = ItemHoverable(frame_bb, id);
    bool temp_input_is_active = TempInputIsActive(id);
    bool temp_input_start = false;
    if (!temp_input_is_active)
    {
        const bool focus_requested = FocusableItemRegister(window, id);
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        const bool double_clicked = (hovered && g.IO.MouseDoubleClicked[0]);
        if (focus_requested || clicked || double_clicked || g.NavActivateId == id || g.NavInputId == id)
        {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            if (focus_requested || (clicked && g.IO.KeyCtrl) || double_clicked || g.NavInputId == id)
            {
                temp_input_start = true;
                FocusableItemUnregister(window);
            }
        }
    }

    // Our current specs do NOT clamp when using CTRL+Click manual input, but we should eventually add a flag for that..
    if (temp_input_is_active || temp_input_start)
        return TempInputScalar(frame_bb, id, label, data_type, p_data, format);// , p_min, p_max);

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

    // Drag behavior
    const bool value_changed = DragBehavior(id, data_type, p_data, v_speed, p_min, p_max, format, power, ImGuiDragFlags_None);
    if (value_changed)
        MarkItemEdited(id);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return value_changed;
}

bool ImGui::DragScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, float power)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components, CalcItemWidth());
    size_t type_size = GDataTypeInfo[data_type].Size;
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        if (i > 0)
            SameLine(0, g.Style.ItemInnerSpacing.x);
        value_changed |= DragScalar("", data_type, p_data, v_speed, p_min, p_max, format, power);
        PopID();
        PopItemWidth();
        p_data = (void*)((char*)p_data + type_size);
    }
    PopID();

    const char* label_end = FindRenderedTextEnd(label);
    if (label != label_end)
    {
        SameLine(0, g.Style.ItemInnerSpacing.x);
        TextEx(label, label_end);
    }

    EndGroup();
    return value_changed;
}

bool ImGui::DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalar(label, ImGuiDataType_Float, v, v_speed, &v_min, &v_max, format, power);
}

bool ImGui::DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalarN(label, ImGuiDataType_Float, v, 2, v_speed, &v_min, &v_max, format, power);
}

bool ImGui::DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalarN(label, ImGuiDataType_Float, v, 3, v_speed, &v_min, &v_max, format, power);
}

bool ImGui::DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalarN(label, ImGuiDataType_Float, v, 4, v_speed, &v_min, &v_max, format, power);
}

bool ImGui::DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* format, const char* format_max, float power)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2, CalcItemWidth());

    bool value_changed = DragFloat("##min", v_current_min, v_speed, (v_min >= v_max) ? -FLT_MAX : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragFloat("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? FLT_MAX : v_max, format_max ? format_max : format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();
    return value_changed;
}

// NB: v_speed is float to allow adjusting the drag speed with more precision
bool ImGui::DragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalar(label, ImGuiDataType_S32, v, v_speed, &v_min, &v_max, format);
}

bool ImGui::DragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalarN(label, ImGuiDataType_S32, v, 2, v_speed, &v_min, &v_max, format);
}

bool ImGui::DragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalarN(label, ImGuiDataType_S32, v, 3, v_speed, &v_min, &v_max, format);
}

bool ImGui::DragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalarN(label, ImGuiDataType_S32, v, 4, v_speed, &v_min, &v_max, format);
}

bool ImGui::DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* format, const char* format_max)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2, CalcItemWidth());

    bool value_changed = DragInt("##min", v_current_min, v_speed, (v_min >= v_max) ? INT_MIN : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), format);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragInt("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? INT_MAX : v_max, format_max ? format_max : format);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();

    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: SliderScalar, SliderFloat, SliderInt, etc.
//-------------------------------------------------------------------------
// - SliderBehaviorT<>() [Internal]
// - SliderBehavior() [Internal]
// - SliderScalar()
// - SliderScalarN()
// - SliderFloat()
// - SliderFloat2()
// - SliderFloat3()
// - SliderFloat4()
// - SliderAngle()
// - SliderInt()
// - SliderInt2()
// - SliderInt3()
// - SliderInt4()
// - VSliderScalar()
// - VSliderFloat()
// - VSliderInt()
//-------------------------------------------------------------------------

template<typename TYPE, typename FLOATTYPE>
float ImGui::SliderCalcRatioFromValueT(ImGuiDataType data_type, TYPE v, TYPE v_min, TYPE v_max, float power, float linear_zero_pos)
{
    if (v_min == v_max)
        return 0.0f;

    const bool is_power = (power != 1.0f) && (data_type == ImGuiDataType_Float || data_type == ImGuiDataType_Double);
    const TYPE v_clamped = (v_min < v_max) ? ImClamp(v, v_min, v_max) : ImClamp(v, v_max, v_min);
    if (is_power)
    {
        if (v_clamped < 0.0f)
        {
            const float f = 1.0f - (float)((v_clamped - v_min) / (ImMin((TYPE)0, v_max) - v_min));
            return (1.0f - ImPow(f, 1.0f / power)) * linear_zero_pos;
        } else
        {
            const float f = (float)((v_clamped - ImMax((TYPE)0, v_min)) / (v_max - ImMax((TYPE)0, v_min)));
            return linear_zero_pos + ImPow(f, 1.0f / power) * (1.0f - linear_zero_pos);
        }
    }

    // Linear slider
    return (float)((FLOATTYPE)(v_clamped - v_min) / (FLOATTYPE)(v_max - v_min));
}

// FIXME: Move some of the code into SliderBehavior(). Current responsibility is larger than what the equivalent DragBehaviorT<> does, we also do some rendering, etc.
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool ImGui::SliderBehaviorT(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, TYPE* v, const TYPE v_min, const TYPE v_max, const char* format, float power, ImGuiSliderFlags flags, ImRect* out_grab_bb)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImGuiAxis axis = (flags & ImGuiSliderFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;
    const bool is_decimal = (data_type == ImGuiDataType_Float) || (data_type == ImGuiDataType_Double);
    const bool is_power = (power != 1.0f) && is_decimal;

    const float grab_padding = 2.0f;
    const float slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
    float grab_sz = style.GrabMinSize;
    SIGNEDTYPE v_range = (v_min < v_max ? v_max - v_min : v_min - v_max);
    if (!is_decimal && v_range >= 0)                                             // v_range < 0 may happen on integer overflows
        grab_sz = ImMax((float)(slider_sz / (v_range + 1)), style.GrabMinSize);  // For integer sliders: if possible have the grab size represent 1 unit
    grab_sz = ImMin(grab_sz, slider_sz);
    const float slider_usable_sz = slider_sz - grab_sz;
    const float slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
    const float slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;

    // For power curve sliders that cross over sign boundary we want the curve to be symmetric around 0.0f
    float linear_zero_pos;   // 0.0->1.0f
    if (is_power && v_min * v_max < 0.0f)
    {
        // Different sign
        const FLOATTYPE linear_dist_min_to_0 = ImPow(v_min >= 0 ? (FLOATTYPE)v_min : -(FLOATTYPE)v_min, (FLOATTYPE)1.0f / power);
        const FLOATTYPE linear_dist_max_to_0 = ImPow(v_max >= 0 ? (FLOATTYPE)v_max : -(FLOATTYPE)v_max, (FLOATTYPE)1.0f / power);
        linear_zero_pos = (float)(linear_dist_min_to_0 / (linear_dist_min_to_0 + linear_dist_max_to_0));
    } else
    {
        // Same sign
        linear_zero_pos = v_min < 0.0f ? 1.0f : 0.0f;
    }

    // Process interacting with the slider
    bool value_changed = false;
    if (g.ActiveId == id)
    {
        bool set_new_value = false;
        float clicked_t = 0.0f;
        if (g.ActiveIdSource == ImGuiInputSource_Mouse)
        {
            if (!g.IO.MouseDown[0])
            {
                ClearActiveID();
            } else
            {
                const float mouse_abs_pos = g.IO.MousePos[axis];
                clicked_t = (slider_usable_sz > 0.0f) ? ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f) : 0.0f;
                if (axis == ImGuiAxis_Y)
                    clicked_t = 1.0f - clicked_t;
                set_new_value = true;
            }
        } else if (g.ActiveIdSource == ImGuiInputSource_Nav)
        {
            const ImVec2 delta2 = GetNavInputAmount2d(ImGuiNavDirSourceFlags_Keyboard | ImGuiNavDirSourceFlags_PadDPad, ImGuiInputReadMode_RepeatFast, 0.0f, 0.0f);
            float delta = (axis == ImGuiAxis_X) ? delta2.x : -delta2.y;
            if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
            {
                ClearActiveID();
            } else if (delta != 0.0f)
            {
                clicked_t = SliderCalcRatioFromValueT<TYPE, FLOATTYPE>(data_type, *v, v_min, v_max, power, linear_zero_pos);
                const int decimal_precision = is_decimal ? ImParseFormatPrecision(format, 3) : 0;
                if ((decimal_precision > 0) || is_power)
                {
                    delta /= 100.0f;    // Gamepad/keyboard tweak speeds in % of slider bounds
                    if (IsNavInputDown(ImGuiNavInput_TweakSlow))
                        delta /= 10.0f;
                } else
                {
                    if ((v_range >= -100.0f && v_range <= 100.0f) || IsNavInputDown(ImGuiNavInput_TweakSlow))
                        delta = ((delta < 0.0f) ? -1.0f : +1.0f) / (float)v_range; // Gamepad/keyboard tweak speeds in integer steps
                    else
                        delta /= 100.0f;
                }
                if (IsNavInputDown(ImGuiNavInput_TweakFast))
                    delta *= 10.0f;
                set_new_value = true;
                if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f)) // This is to avoid applying the saturation when already past the limits
                    set_new_value = false;
                else
                    clicked_t = ImSaturate(clicked_t + delta);
            }
        }

        if (set_new_value)
        {
            TYPE v_new;
            if (is_power)
            {
                // Account for power curve scale on both sides of the zero
                if (clicked_t < linear_zero_pos)
                {
                    // Negative: rescale to the negative range before powering
                    float a = 1.0f - (clicked_t / linear_zero_pos);
                    a = ImPow(a, power);
                    v_new = ImLerp(ImMin(v_max, (TYPE)0), v_min, a);
                } else
                {
                    // Positive: rescale to the positive range before powering
                    float a;
                    if (ImFabs(linear_zero_pos - 1.0f) > 1.e-6f)
                        a = (clicked_t - linear_zero_pos) / (1.0f - linear_zero_pos);
                    else
                        a = clicked_t;
                    a = ImPow(a, power);
                    v_new = ImLerp(ImMax(v_min, (TYPE)0), v_max, a);
                }
            } else
            {
                // Linear slider
                if (is_decimal)
                {
                    v_new = ImLerp(v_min, v_max, clicked_t);
                } else
                {
                    // For integer values we want the clicking position to match the grab box so we round above
                    // This code is carefully tuned to work with large values (e.g. high ranges of U64) while preserving this property..
                    FLOATTYPE v_new_off_f = (v_max - v_min) * clicked_t;
                    TYPE v_new_off_floor = (TYPE)(v_new_off_f);
                    TYPE v_new_off_round = (TYPE)(v_new_off_f + (FLOATTYPE)0.5);
                    if (v_new_off_floor < v_new_off_round)
                        v_new = v_min + v_new_off_round;
                    else
                        v_new = v_min + v_new_off_floor;
                }
            }

            // Round to user desired precision based on format string
            v_new = RoundScalarWithFormatT<TYPE, SIGNEDTYPE>(format, data_type, v_new);

            // Apply result
            if (*v != v_new)
            {
                *v = v_new;
                value_changed = true;
            }
        }
    }

    if (slider_sz < 1.0f)
    {
        *out_grab_bb = ImRect(bb.Min, bb.Min);
    } else
    {
        // Output grab position so it can be displayed by the caller
        float grab_t = SliderCalcRatioFromValueT<TYPE, FLOATTYPE>(data_type, *v, v_min, v_max, power, linear_zero_pos);
        if (axis == ImGuiAxis_Y)
            grab_t = 1.0f - grab_t;
        const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
        if (axis == ImGuiAxis_X)
            *out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
        else
            *out_grab_bb = ImRect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);
    }

    return value_changed;
}

// For 32-bit and larger types, slider bounds are limited to half the natural type range.
// So e.g. an integer Slider between INT_MAX-10 and INT_MAX will fail, but an integer Slider between INT_MAX/2-10 and INT_MAX/2 will be ok.
// It would be possible to lift that limitation with some work but it doesn't seem to be worth it for sliders.
bool ImGui::SliderBehavior(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, void* p_v, const void* p_min, const void* p_max, const char* format, float power, ImGuiSliderFlags flags, ImRect* out_grab_bb)
{
    switch (data_type)
    {
    case ImGuiDataType_S8: { ImS32 v32 = (ImS32) * (ImS8*)p_v;  bool r = SliderBehaviorT<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS8*)p_min, *(const ImS8*)p_max, format, power, flags, out_grab_bb); if (r) *(ImS8*)p_v = (ImS8)v32;  return r; }
    case ImGuiDataType_U8: { ImU32 v32 = (ImU32) * (ImU8*)p_v;  bool r = SliderBehaviorT<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU8*)p_min, *(const ImU8*)p_max, format, power, flags, out_grab_bb); if (r) *(ImU8*)p_v = (ImU8)v32;  return r; }
    case ImGuiDataType_S16: { ImS32 v32 = (ImS32) * (ImS16*)p_v; bool r = SliderBehaviorT<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS16*)p_min, *(const ImS16*)p_max, format, power, flags, out_grab_bb); if (r) *(ImS16*)p_v = (ImS16)v32; return r; }
    case ImGuiDataType_U16: { ImU32 v32 = (ImU32) * (ImU16*)p_v; bool r = SliderBehaviorT<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU16*)p_min, *(const ImU16*)p_max, format, power, flags, out_grab_bb); if (r) *(ImU16*)p_v = (ImU16)v32; return r; }
    case ImGuiDataType_S32:
        IM_ASSERT(*(const ImS32*)p_min >= IM_S32_MIN / 2 && *(const ImS32*)p_max <= IM_S32_MAX / 2);
        return SliderBehaviorT<ImS32, ImS32, float >(bb, id, data_type, (ImS32*)p_v, *(const ImS32*)p_min, *(const ImS32*)p_max, format, power, flags, out_grab_bb);
    case ImGuiDataType_U32:
        IM_ASSERT(*(const ImU32*)p_max <= IM_U32_MAX / 2);
        return SliderBehaviorT<ImU32, ImS32, float >(bb, id, data_type, (ImU32*)p_v, *(const ImU32*)p_min, *(const ImU32*)p_max, format, power, flags, out_grab_bb);
    case ImGuiDataType_S64:
        IM_ASSERT(*(const ImS64*)p_min >= IM_S64_MIN / 2 && *(const ImS64*)p_max <= IM_S64_MAX / 2);
        return SliderBehaviorT<ImS64, ImS64, double>(bb, id, data_type, (ImS64*)p_v, *(const ImS64*)p_min, *(const ImS64*)p_max, format, power, flags, out_grab_bb);
    case ImGuiDataType_U64:
        IM_ASSERT(*(const ImU64*)p_max <= IM_U64_MAX / 2);
        return SliderBehaviorT<ImU64, ImS64, double>(bb, id, data_type, (ImU64*)p_v, *(const ImU64*)p_min, *(const ImU64*)p_max, format, power, flags, out_grab_bb);
    case ImGuiDataType_Float:
        IM_ASSERT(*(const float*)p_min >= -FLT_MAX / 2.0f && *(const float*)p_max <= FLT_MAX / 2.0f);
        return SliderBehaviorT<float, float, float >(bb, id, data_type, (float*)p_v, *(const float*)p_min, *(const float*)p_max, format, power, flags, out_grab_bb);
    case ImGuiDataType_Double:
        IM_ASSERT(*(const double*)p_min >= -DBL_MAX / 2.0f && *(const double*)p_max <= DBL_MAX / 2.0f);
        return SliderBehaviorT<double, double, double>(bb, id, data_type, (double*)p_v, *(const double*)p_min, *(const double*)p_max, format, power, flags, out_grab_bb);
    case ImGuiDataType_COUNT: break;
    }
    IM_ASSERT(0);
    return false;
}

// Note: p_data, p_min and p_max are _pointers_ to a memory address holding the data. For a slider, they are all required.
// Read code of e.g. SliderFloat(), SliderInt() etc. or examples in 'Demo->Widgets->Data Types' to understand how to use this function directly.
bool ImGui::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, float power)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
        format = PatchFormatStringFloatToInt(format);

    // Tabbing or CTRL-clicking on Slider turns it into an input box
    const bool hovered = ItemHoverable(frame_bb, id);
    bool temp_input_is_active = TempInputIsActive(id);
    bool temp_input_start = false;
    if (!temp_input_is_active)
    {
        const bool focus_requested = FocusableItemRegister(window, id);
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
        {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            if (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id)
            {
                temp_input_start = true;
                FocusableItemUnregister(window);
            }
        }
    }

    // Our current specs do NOT clamp when using CTRL+Click manual input, but we should eventually add a flag for that..
    if (temp_input_is_active || temp_input_start)
        return TempInputScalar(frame_bb, id, label, data_type, p_data, format);// , p_min, p_max);

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, power, ImGuiSliderFlags_None, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);

    // Render grab
    if (grab_bb.Max.x > grab_bb.Min.x)
        window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return value_changed;
}

// Add multiple sliders on 1 line for compact edition of multiple components
bool ImGui::SliderScalarN(const char* label, ImGuiDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, float power)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components, CalcItemWidth());
    size_t type_size = GDataTypeInfo[data_type].Size;
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        if (i > 0)
            SameLine(0, g.Style.ItemInnerSpacing.x);
        value_changed |= SliderScalar("", data_type, v, v_min, v_max, format, power);
        PopID();
        PopItemWidth();
        v = (void*)((char*)v + type_size);
    }
    PopID();

    const char* label_end = FindRenderedTextEnd(label);
    if (label != label_end)
    {
        SameLine(0, g.Style.ItemInnerSpacing.x);
        TextEx(label, label_end);
    }

    EndGroup();
    return value_changed;
}

bool ImGui::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, float power)
{
    return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, power);
}

bool ImGui::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, float power)
{
    return SliderScalarN(label, ImGuiDataType_Float, v, 2, &v_min, &v_max, format, power);
}

bool ImGui::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, float power)
{
    return SliderScalarN(label, ImGuiDataType_Float, v, 3, &v_min, &v_max, format, power);
}

bool ImGui::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, float power)
{
    return SliderScalarN(label, ImGuiDataType_Float, v, 4, &v_min, &v_max, format, power);
}

bool ImGui::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format)
{
    if (format == NULL)
        format = "%.0f deg";
    float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
    bool value_changed = SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, 1.0f);
    *v_rad = v_deg * (2 * IM_PI) / 360.0f;
    return value_changed;
}

bool ImGui::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format)
{
    return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format);
}

bool ImGui::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_S32, v, 2, &v_min, &v_max, format);
}

bool ImGui::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_S32, v, 3, &v_min, &v_max, format);
}

bool ImGui::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_S32, v, 4, &v_min, &v_max, format);
}

bool ImGui::VSliderScalar(const char* label, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, float power)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const ImRect bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(frame_bb, id))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
        format = PatchFormatStringFloatToInt(format);

    const bool hovered = ItemHoverable(frame_bb, id);
    if ((hovered && g.IO.MouseClicked[0]) || g.NavActivateId == id || g.NavInputId == id)
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
    }

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, power, ImGuiSliderFlags_Vertical, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);

    // Render grab
    if (grab_bb.Max.y > grab_bb.Min.y)
        window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    // For the vertical slider we allow centered text to overlap the frame padding
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.0f));
    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    return value_changed;
}

bool ImGui::VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format, float power)
{
    return VSliderScalar(label, size, ImGuiDataType_Float, v, &v_min, &v_max, format, power);
}

bool ImGui::VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format)
{
    return VSliderScalar(label, size, ImGuiDataType_S32, v, &v_min, &v_max, format);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: InputScalar, InputFloat, InputInt, etc.
//-------------------------------------------------------------------------
// - ImParseFormatFindStart() [Internal]
// - ImParseFormatFindEnd() [Internal]
// - ImParseFormatTrimDecorations() [Internal]
// - ImParseFormatPrecision() [Internal]
// - TempInputTextScalar() [Internal]
// - InputScalar()
// - InputScalarN()
// - InputFloat()
// - InputFloat2()
// - InputFloat3()
// - InputFloat4()
// - InputInt()
// - InputInt2()
// - InputInt3()
// - InputInt4()
// - InputDouble()
//-------------------------------------------------------------------------

// We don't use strchr() because our strings are usually very short and often start with '%'
const char* ImParseFormatFindStart(const char* fmt)
{
    while (char c = fmt[0])
    {
        if (c == '%' && fmt[1] != '%')
            return fmt;
        else if (c == '%')
            fmt++;
        fmt++;
    }
    return fmt;
}

const char* ImParseFormatFindEnd(const char* fmt)
{
    // Printf/scanf types modifiers: I/L/h/j/l/t/w/z. Other uppercase letters qualify as types aka end of the format.
    if (fmt[0] != '%')
        return fmt;
    const unsigned int ignored_uppercase_mask = (1 << ('I' - 'A')) | (1 << ('L' - 'A'));
    const unsigned int ignored_lowercase_mask = (1 << ('h' - 'a')) | (1 << ('j' - 'a')) | (1 << ('l' - 'a')) | (1 << ('t' - 'a')) | (1 << ('w' - 'a')) | (1 << ('z' - 'a'));
    for (char c; (c = *fmt) != 0; fmt++)
    {
        if (c >= 'A' && c <= 'Z' && ((1 << (c - 'A')) & ignored_uppercase_mask) == 0)
            return fmt + 1;
        if (c >= 'a' && c <= 'z' && ((1 << (c - 'a')) & ignored_lowercase_mask) == 0)
            return fmt + 1;
    }
    return fmt;
}

// Extract the format out of a format string with leading or trailing decorations
//  fmt = "blah blah"  -> return fmt
//  fmt = "%.3f"       -> return fmt
//  fmt = "hello %.3f" -> return fmt + 6
//  fmt = "%.3f hello" -> return buf written with "%.3f"
const char* ImParseFormatTrimDecorations(const char* fmt, char* buf, size_t buf_size)
{
    const char* fmt_start = ImParseFormatFindStart(fmt);
    if (fmt_start[0] != '%')
        return fmt;
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);
    if (fmt_end[0] == 0) // If we only have leading decoration, we don't need to copy the data.
        return fmt_start;
    ImStrncpy(buf, fmt_start, ImMin((size_t)(fmt_end - fmt_start) + 1, buf_size));
    return buf;
}

// Parse display precision back from the display format string
// FIXME: This is still used by some navigation code path to infer a minimum tweak step, but we should aim to rework widgets so it isn't needed.
int ImParseFormatPrecision(const char* fmt, int default_precision)
{
    fmt = ImParseFormatFindStart(fmt);
    if (fmt[0] != '%')
        return default_precision;
    fmt++;
    while (*fmt >= '0' && *fmt <= '9')
        fmt++;
    int precision = INT_MAX;
    if (*fmt == '.')
    {
        fmt = ImAtoi<int>(fmt + 1, &precision);
        if (precision < 0 || precision > 99)
            precision = default_precision;
    }
    if (*fmt == 'e' || *fmt == 'E') // Maximum precision with scientific notation
        precision = -1;
    if ((*fmt == 'g' || *fmt == 'G') && precision == INT_MAX)
        precision = -1;
    return (precision == INT_MAX) ? default_precision : precision;
}

// Create text input in place of another active widget (e.g. used when doing a CTRL+Click on drag/slider widgets)
// FIXME: Facilitate using this in variety of other situations.
bool ImGui::TempInputText(const ImRect& bb, ImGuiID id, const char* label, char* buf, int buf_size, ImGuiInputTextFlags flags)
{
    // On the first frame, g.TempInputTextId == 0, then on subsequent frames it becomes == id.
    // We clear ActiveID on the first frame to allow the InputText() taking it back.
    ImGuiContext& g = *GImGui;
    const bool init = (g.TempInputId != id);
    if (init)
        ClearActiveID();

    g.CurrentWindow->DC.CursorPos = bb.Min;
    bool value_changed = InputTextEx(label, NULL, buf, buf_size, bb.GetSize(), flags);
    if (init)
    {
        // First frame we started displaying the InputText widget, we expect it to take the active id.
        IM_ASSERT(g.ActiveId == id);
        g.TempInputId = g.ActiveId;
    }
    return value_changed;
}

// Note that Drag/Slider functions are currently NOT forwarding the min/max values clamping values!
// This is intended: this way we allow CTRL+Click manual input to set a value out of bounds, for maximum flexibility.
// However this may not be ideal for all uses, as some user code may break on out of bound values.
// In the future we should add flags to Slider/Drag to specify how to enforce min/max values with CTRL+Click.
// See GitHub issues #1829 and #3209
// In the meanwhile, you can easily "wrap" those functions to enforce clamping, using wrapper functions, e.g.
//   bool SliderFloatClamp(const char* label, float* v, float v_min, float v_max)
//   {
//      float v_backup = *v;
//      if (!SliderFloat(label, v, v_min, v_max))
//         return false;
//      *v = ImClamp(*v, v_min, v_max);
//      return v_backup != *v;
//   }
bool ImGui::TempInputScalar(const ImRect& bb, ImGuiID id, const char* label, ImGuiDataType data_type, void* p_data, const char* format, const void* p_clamp_min, const void* p_clamp_max)
{
    ImGuiContext& g = *GImGui;

    char fmt_buf[32];
    char data_buf[32];
    format = ImParseFormatTrimDecorations(format, fmt_buf, IM_ARRAYSIZE(fmt_buf));
    DataTypeFormatString(data_buf, IM_ARRAYSIZE(data_buf), data_type, p_data, format);
    ImStrTrimBlanks(data_buf);

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoMarkEdited;
    flags |= ((data_type == ImGuiDataType_Float || data_type == ImGuiDataType_Double) ? ImGuiInputTextFlags_CharsScientific : ImGuiInputTextFlags_CharsDecimal);
    bool value_changed = false;
    if (TempInputText(bb, id, label, data_buf, IM_ARRAYSIZE(data_buf), flags))
    {
        // Backup old value
        size_t data_type_size = DataTypeGetInfo(data_type)->Size;
        ImGuiDataTypeTempStorage data_backup;
        memcpy(&data_backup, p_data, data_type_size);

        // Apply new value (or operations) then clamp
        DataTypeApplyOpFromText(data_buf, g.InputTextState.InitialTextA.Data, data_type, p_data, NULL);
        if (p_clamp_min && p_clamp_max)
            DataTypeClamp(data_type, p_data, p_clamp_min, p_clamp_max);

        // Only mark as edited if new value is different
        value_changed = memcmp(&data_backup, p_data, data_type_size) != 0;
        if (value_changed)
            MarkItemEdited(id);
    }
    return value_changed;
}

// Note: p_data, p_step, p_step_fast are _pointers_ to a memory address holding the data. For an Input widget, p_step and p_step_fast are optional.
// Read code of e.g. InputFloat(), InputInt() etc. or examples in 'Demo->Widgets->Data Types' to understand how to use this function directly.
bool ImGui::InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;

    char buf[64];
    DataTypeFormatString(buf, IM_ARRAYSIZE(buf), data_type, p_data, format);

    bool value_changed = false;
    if ((flags & (ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsScientific)) == 0)
        flags |= ImGuiInputTextFlags_CharsDecimal;
    flags |= ImGuiInputTextFlags_AutoSelectAll;
    flags |= ImGuiInputTextFlags_NoMarkEdited;  // We call MarkItemEdited() ourselves by comparing the actual data rather than the string.

    if (p_step != NULL)
    {
        const float button_size = GetFrameHeight();

        BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
        PushID(label);
        SetNextItemWidth(ImMax(1.0f, CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
        if (InputText("", buf, IM_ARRAYSIZE(buf), flags)) // PushId(label) + "" gives us the expected ID from outside point of view
            value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, p_data, format);

        // Step buttons
        const ImVec2 backup_frame_padding = style.FramePadding;
        style.FramePadding.x = style.FramePadding.y;
        ImGuiButtonFlags button_flags = ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups;
        if (flags & ImGuiInputTextFlags_ReadOnly)
            button_flags |= ImGuiButtonFlags_Disabled;
        SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("-", ImVec2(button_size, button_size), button_flags))
        {
            DataTypeApplyOp(data_type, '-', p_data, p_data, g.IO.KeyCtrl && p_step_fast ? p_step_fast : p_step);
            value_changed = true;
        }
        SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("+", ImVec2(button_size, button_size), button_flags))
        {
            DataTypeApplyOp(data_type, '+', p_data, p_data, g.IO.KeyCtrl && p_step_fast ? p_step_fast : p_step);
            value_changed = true;
        }

        const char* label_end = FindRenderedTextEnd(label);
        if (label != label_end)
        {
            SameLine(0, style.ItemInnerSpacing.x);
            TextEx(label, label_end);
        }
        style.FramePadding = backup_frame_padding;

        PopID();
        EndGroup();
    } else
    {
        if (InputText(label, buf, IM_ARRAYSIZE(buf), flags))
            value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, p_data, format);
    }
    if (value_changed)
        MarkItemEdited(window->DC.LastItemId);

    return value_changed;
}

bool ImGui::InputScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components, CalcItemWidth());
    size_t type_size = GDataTypeInfo[data_type].Size;
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        if (i > 0)
            SameLine(0, g.Style.ItemInnerSpacing.x);
        value_changed |= InputScalar("", data_type, p_data, p_step, p_step_fast, format, flags);
        PopID();
        PopItemWidth();
        p_data = (void*)((char*)p_data + type_size);
    }
    PopID();

    const char* label_end = FindRenderedTextEnd(label);
    if (label != label_end)
    {
        SameLine(0.0f, g.Style.ItemInnerSpacing.x);
        TextEx(label, label_end);
    }

    EndGroup();
    return value_changed;
}

bool ImGui::InputFloat(const char* label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
{
    flags |= ImGuiInputTextFlags_CharsScientific;
    return InputScalar(label, ImGuiDataType_Float, (void*)v, (void*)(step > 0.0f ? &step : NULL), (void*)(step_fast > 0.0f ? &step_fast : NULL), format, flags);
}

bool ImGui::InputFloat2(const char* label, float v[2], const char* format, ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_Float, v, 2, NULL, NULL, format, flags);
}

bool ImGui::InputFloat3(const char* label, float v[3], const char* format, ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_Float, v, 3, NULL, NULL, format, flags);
}

bool ImGui::InputFloat4(const char* label, float v[4], const char* format, ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_Float, v, 4, NULL, NULL, format, flags);
}

// Prefer using "const char* format" directly, which is more flexible and consistent with other API.
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
bool ImGui::InputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, ImGuiInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputFloat(label, v, step, step_fast, format, flags);
}

bool ImGui::InputFloat2(const char* label, float v[2], int decimal_precision, ImGuiInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputScalarN(label, ImGuiDataType_Float, v, 2, NULL, NULL, format, flags);
}

bool ImGui::InputFloat3(const char* label, float v[3], int decimal_precision, ImGuiInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputScalarN(label, ImGuiDataType_Float, v, 3, NULL, NULL, format, flags);
}

bool ImGui::InputFloat4(const char* label, float v[4], int decimal_precision, ImGuiInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputScalarN(label, ImGuiDataType_Float, v, 4, NULL, NULL, format, flags);
}
#endif // IMGUI_DISABLE_OBSOLETE_FUNCTIONS

bool ImGui::InputInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, ImGuiDataType_S32, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool ImGui::InputInt2(const char* label, int v[2], ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_S32, v, 2, NULL, NULL, "%d", flags);
}

bool ImGui::InputInt3(const char* label, int v[3], ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_S32, v, 3, NULL, NULL, "%d", flags);
}

bool ImGui::InputInt4(const char* label, int v[4], ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_S32, v, 4, NULL, NULL, "%d", flags);
}

bool ImGui::InputDouble(const char* label, double* v, double step, double step_fast, const char* format, ImGuiInputTextFlags flags)
{
    flags |= ImGuiInputTextFlags_CharsScientific;
    return InputScalar(label, ImGuiDataType_Double, (void*)v, (void*)(step > 0.0 ? &step : NULL), (void*)(step_fast > 0.0 ? &step_fast : NULL), format, flags);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: InputText, InputTextMultiline, InputTextWithHint
//-------------------------------------------------------------------------
// - InputText()
// - InputTextWithHint()
// - InputTextMultiline()
// - InputTextEx() [Internal]
//-------------------------------------------------------------------------

bool ImGui::InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()
    return InputTextEx(label, NULL, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
}

bool ImGui::InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    return InputTextEx(label, NULL, buf, (int)buf_size, size, flags | ImGuiInputTextFlags_Multiline, callback, user_data);
}

bool ImGui::InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()
    return InputTextEx(label, hint, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
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

static ImVec2 InputTextCalcTextSizeW(const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining, ImVec2* out_offset, bool stop_on_new_line)
{
    ImGuiContext& g = *GImGui;
    ImFont* font = g.Font;
    const float line_height = g.FontSize;
    const float scale = line_height / font->FontSize;

    ImVec2 text_size = ImVec2(0, 0);
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

        const float char_width = font->GetCharAdvance((ImWchar)c) * scale;
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
namespace ImStb
{

    static int     STB_TEXTEDIT_STRINGLEN(const STB_TEXTEDIT_STRING* obj) { return obj->CurLenW; }
    static ImWchar STB_TEXTEDIT_GETCHAR(const STB_TEXTEDIT_STRING* obj, int idx) { return obj->TextW[idx]; }
    static float   STB_TEXTEDIT_GETWIDTH(STB_TEXTEDIT_STRING* obj, int line_start_idx, int char_idx) { ImWchar c = obj->TextW[line_start_idx + char_idx]; if (c == '\n') return STB_TEXTEDIT_GETWIDTH_NEWLINE; ImGuiContext& g = *GImGui; return g.Font->GetCharAdvance(c) * (g.FontSize / g.Font->FontSize); }
    static int     STB_TEXTEDIT_KEYTOTEXT(int key) { return key >= 0x200000 ? 0 : key; }
    static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
    static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* r, STB_TEXTEDIT_STRING* obj, int line_start_idx)
    {
        const ImWchar* text = obj->TextW.Data;
        const ImWchar* text_remaining = NULL;
        const ImVec2 size = InputTextCalcTextSizeW(text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
        r->x0 = 0.0f;
        r->x1 = size.x;
        r->baseline_y_delta = size.y;
        r->ymin = 0.0f;
        r->ymax = size.y;
        r->num_chars = (int)(text_remaining - (text + line_start_idx));
    }

    static bool is_separator(unsigned int c) { return ImCharIsBlankW(c) || c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '|'; }
    static int  is_word_boundary_from_right(STB_TEXTEDIT_STRING* obj, int idx) { return idx > 0 ? (is_separator(obj->TextW[idx - 1]) && !is_separator(obj->TextW[idx])) : 1; }
    static int  STB_TEXTEDIT_MOVEWORDLEFT_IMPL(STB_TEXTEDIT_STRING* obj, int idx) { idx--; while (idx >= 0 && !is_word_boundary_from_right(obj, idx)) idx--; return idx < 0 ? 0 : idx; }
#ifdef __APPLE__    // FIXME: Move setting to IO structure
    static int  is_word_boundary_from_left(STB_TEXTEDIT_STRING* obj, int idx) { return idx > 0 ? (!is_separator(obj->TextW[idx - 1]) && is_separator(obj->TextW[idx])) : 1; }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(STB_TEXTEDIT_STRING* obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_left(obj, idx)) idx++; return idx > len ? len : idx; }
#else
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(STB_TEXTEDIT_STRING* obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_right(obj, idx)) idx++; return idx > len ? len : idx; }
#endif
#define STB_TEXTEDIT_MOVEWORDLEFT   STB_TEXTEDIT_MOVEWORDLEFT_IMPL    // They need to be #define for stb_textedit.h
#define STB_TEXTEDIT_MOVEWORDRIGHT  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL

    static void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING* obj, int pos, int n)
    {
        ImWchar* dst = obj->TextW.Data + pos;

        // We maintain our buffer length in both UTF-8 and wchar formats
        obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
        obj->CurLenW -= n;

        // Offset remaining text (FIXME-OPT: Use memmove)
        const ImWchar* src = obj->TextW.Data + pos + n;
        while (ImWchar c = *src++)
            *dst++ = c;
        *dst = '\0';
    }

    static bool STB_TEXTEDIT_INSERTCHARS(STB_TEXTEDIT_STRING* obj, int pos, const ImWchar* new_text, int new_text_len)
    {
        const bool is_resizable = (obj->UserFlags & ImGuiInputTextFlags_CallbackResize) != 0;
        const int text_len = obj->CurLenW;
        IM_ASSERT(pos <= text_len);

        const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_text_len);
        if (!is_resizable && (new_text_len_utf8 + obj->CurLenA + 1 > obj->BufCapacityA))
            return false;

        // Grow internal buffer if needed
        if (new_text_len + text_len + 1 > obj->TextW.Size)
        {
            if (!is_resizable)
                return false;
            IM_ASSERT(text_len < obj->TextW.Size);
            obj->TextW.resize(text_len + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1);
        }

        ImWchar* text = obj->TextW.Data;
        if (pos != text_len)
            memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(ImWchar));
        memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(ImWchar));

        obj->CurLenW += new_text_len;
        obj->CurLenA += new_text_len_utf8;
        obj->TextW[obj->CurLenW] = '\0';

        return true;
    }

    // We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define STB_TEXTEDIT_K_LEFT         0x200000 // keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x200001 // keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x200002 // keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x200003 // keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x200004 // keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x200005 // keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x200006 // keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x200007 // keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x200008 // keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x200009 // keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x20000A // keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x20000B // keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x20000C // keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x20000D // keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_SHIFT        0x400000

#define STB_TEXTEDIT_IMPLEMENTATION
#include "imstb_textedit.h"

// stb_textedit internally allows for a single undo record to do addition and deletion, but somehow, calling
// the stb_textedit_paste() function creates two separate records, so we perform it manually. (FIXME: Report to nothings/stb?)
    static void stb_textedit_replace(STB_TEXTEDIT_STRING* str, STB_TexteditState* state, const STB_TEXTEDIT_CHARTYPE* text, int text_len)
    {
        stb_text_makeundo_replace(str, state, 0, str->CurLenW, text_len);
        ImStb::STB_TEXTEDIT_DELETECHARS(str, 0, str->CurLenW);
        if (text_len <= 0)
            return;
        if (ImStb::STB_TEXTEDIT_INSERTCHARS(str, 0, text, text_len))
        {
            state->cursor = text_len;
            state->has_preferred_x = 0;
            return;
        }
        IM_ASSERT(0); // Failed to insert character, normally shouldn't happen because of how we currently use stb_textedit_replace()
    }

} // namespace ImStb

void ImGuiInputTextState::OnKeyPressed(int key)
{
    stb_textedit_key(this, &Stb, key);
    CursorFollow = true;
    CursorAnimReset();
}

ImGuiInputTextCallbackData::ImGuiInputTextCallbackData()
{
    memset(this, 0, sizeof(*this));
}

// Public API to manipulate UTF-8 text
// We expose UTF-8 to the user (unlike the STB_TEXTEDIT_* functions which are manipulating wchar)
// FIXME: The existence of this rarely exercised code path is a bit of a nuisance.
void ImGuiInputTextCallbackData::DeleteChars(int pos, int bytes_count)
{
    IM_ASSERT(pos + bytes_count <= BufTextLen);
    char* dst = Buf + pos;
    const char* src = Buf + pos + bytes_count;
    while (char c = *src++)
        *dst++ = c;
    *dst = '\0';

    if (CursorPos + bytes_count >= pos)
        CursorPos -= bytes_count;
    else if (CursorPos >= pos)
        CursorPos = pos;
    SelectionStart = SelectionEnd = CursorPos;
    BufDirty = true;
    BufTextLen -= bytes_count;
}

void ImGuiInputTextCallbackData::InsertChars(int pos, const char* new_text, const char* new_text_end)
{
    const bool is_resizable = (Flags & ImGuiInputTextFlags_CallbackResize) != 0;
    const int new_text_len = new_text_end ? (int)(new_text_end - new_text) : (int)strlen(new_text);
    if (new_text_len + BufTextLen >= BufSize)
    {
        if (!is_resizable)
            return;

        // Contrary to STB_TEXTEDIT_INSERTCHARS() this is working in the UTF8 buffer, hence the mildly similar code (until we remove the U16 buffer altogether!)
        ImGuiContext& g = *GImGui;
        ImGuiInputTextState* edit_state = &g.InputTextState;
        IM_ASSERT(edit_state->ID != 0 && g.ActiveId == edit_state->ID);
        IM_ASSERT(Buf == edit_state->TextA.Data);
        int new_buf_size = BufTextLen + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1;
        edit_state->TextA.reserve(new_buf_size + 1);
        Buf = edit_state->TextA.Data;
        BufSize = edit_state->BufCapacityA = new_buf_size;
    }

    if (BufTextLen != pos)
        memmove(Buf + pos + new_text_len, Buf + pos, (size_t)(BufTextLen - pos));
    memcpy(Buf + pos, new_text, (size_t)new_text_len * sizeof(char));
    Buf[BufTextLen + new_text_len] = '\0';

    if (CursorPos >= pos)
        CursorPos += new_text_len;
    SelectionStart = SelectionEnd = CursorPos;
    BufDirty = true;
    BufTextLen += new_text_len;
}

// Return false to discard a character.
static bool InputTextFilterCharacter(unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    unsigned int c = *p_char;

    // Filter non-printable (NB: isprint is unreliable! see #2467)
    if (c < 0x20)
    {
        bool pass = false;
        pass |= (c == '\n' && (flags & ImGuiInputTextFlags_Multiline));
        pass |= (c == '\t' && (flags & ImGuiInputTextFlags_AllowTabInput));
        if (!pass)
            return false;
    }

    // We ignore Ascii representation of delete (emitted from Backspace on OSX, see #2578, #2817)
    if (c == 127)
        return false;

    // Filter private Unicode range. GLFW on OSX seems to send private characters for special keys like arrow keys (FIXME)
    if (c >= 0xE000 && c <= 0xF8FF)
        return false;

    // Filter Unicode ranges we are not handling in this build.
    if (c > IM_UNICODE_CODEPOINT_MAX)
        return false;

    // Generic named filters
    if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsScientific))
    {
        if (flags & ImGuiInputTextFlags_CharsDecimal)
            if (!(c >= '0' && c <= '9') && (c != '.') && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
                return false;

        if (flags & ImGuiInputTextFlags_CharsScientific)
            if (!(c >= '0' && c <= '9') && (c != '.') && (c != '-') && (c != '+') && (c != '*') && (c != '/') && (c != 'e') && (c != 'E'))
                return false;

        if (flags & ImGuiInputTextFlags_CharsHexadecimal)
            if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
                return false;

        if (flags & ImGuiInputTextFlags_CharsUppercase)
            if (c >= 'a' && c <= 'z')
                *p_char = (c += (unsigned int)('A' - 'a'));

        if (flags & ImGuiInputTextFlags_CharsNoBlank)
            if (ImCharIsBlankW(c))
                return false;
    }

    // Custom callback filter
    if (flags & ImGuiInputTextFlags_CallbackCharFilter)
    {
        ImGuiInputTextCallbackData callback_data;
        memset(&callback_data, 0, sizeof(ImGuiInputTextCallbackData));
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
// - buf_size account for the zero-terminator, so a buf_size of 6 can hold "Hello" but not "Hello!".
//   This is so we can easily call InputText() on static arrays using ARRAYSIZE() and to match
//   Note that in std::string world, capacity() would omit 1 byte used by the zero-terminator.
// - When active, hold on a privately held copy of the text (and apply back to 'buf'). So changing 'buf' while the InputText is active has no effect.
// - If you want to use ImGui::InputText() with std::string, see misc/cpp/imgui_stdlib.h
// (FIXME: Rather confusing and messy function, among the worse part of our codebase, expecting to rewrite a V2 at some point.. Partly because we are
//  doing UTF8 > U16 > UTF8 conversions on the go to easily interface with stb_textedit. Ideally should stay in UTF-8 all the time. See https://github.com/nothings/stb/issues/188)
bool ImGui::InputTextEx(const char* label, const char* hint, char* buf, int buf_size, const ImVec2& size_arg, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* callback_user_data)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    IM_ASSERT(buf != NULL && buf_size >= 0);
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline)));        // Can't use both together (they both use up/down keys)
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const bool RENDER_SELECTION_WHEN_INACTIVE = false;
    const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;
    const bool is_readonly = (flags & ImGuiInputTextFlags_ReadOnly) != 0;
    const bool is_password = (flags & ImGuiInputTextFlags_Password) != 0;
    const bool is_undoable = (flags & ImGuiInputTextFlags_NoUndoRedo) == 0;
    const bool is_resizable = (flags & ImGuiInputTextFlags_CallbackResize) != 0;
    if (is_resizable)
        IM_ASSERT(callback != NULL); // Must provide a callback if you set the ImGuiInputTextFlags_CallbackResize flag!

    if (is_multiline) // Open group before calling GetID() because groups tracks id created within their scope,
        BeginGroup();
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImVec2 frame_size = CalcItemSize(size_arg, CalcItemWidth(), (is_multiline ? g.FontSize * 8.0f : label_size.y) + style.FramePadding.y * 2.0f); // Arbitrary default of 8 lines high for multi-line
    const ImVec2 total_size = ImVec2(frame_size.x + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), frame_size.y);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const ImRect total_bb(frame_bb.Min, frame_bb.Min + total_size);

    ImGuiWindow* draw_window = window;
    ImVec2 inner_size = frame_size;
    if (is_multiline)
    {
        if (!ItemAdd(total_bb, id, &frame_bb))
        {
            ItemSize(total_bb, style.FramePadding.y);
            EndGroup();
            return false;
        }

        // We reproduce the contents of BeginChildFrame() in order to provide 'label' so our window internal data are easier to read/debug.
        PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_FrameBg]);
        PushStyleVar(ImGuiStyleVar_ChildRounding, style.FrameRounding);
        PushStyleVar(ImGuiStyleVar_ChildBorderSize, style.FrameBorderSize);
        PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);
        bool child_visible = BeginChildEx(label, id, frame_bb.GetSize(), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding);
        PopStyleVar(3);
        PopStyleColor();
        if (!child_visible)
        {
            EndChild();
            EndGroup();
            return false;
        }
        draw_window = g.CurrentWindow; // Child window
        draw_window->DC.NavLayerActiveMaskNext |= draw_window->DC.NavLayerCurrentMask; // This is to ensure that EndChild() will display a navigation highlight so we can "enter" into it.
        inner_size.x -= draw_window->ScrollbarSizes.x;
    } else
    {
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id, &frame_bb))
            return false;
    }
    const bool hovered = ItemHoverable(frame_bb, id);
    if (hovered)
        g.MouseCursor = ImGuiMouseCursor_TextInput;

    // We are only allowed to access the state if we are already the active widget.
    ImGuiInputTextState* state = GetInputTextState(id);

    const bool focus_requested = FocusableItemRegister(window, id);
    const bool focus_requested_by_code = focus_requested && (g.FocusRequestCurrWindow == window && g.FocusRequestCurrCounterRegular == window->DC.FocusCounterRegular);
    const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

    const bool user_clicked = hovered && io.MouseClicked[0];
    const bool user_nav_input_start = (g.ActiveId != id) && ((g.NavInputId == id) || (g.NavActivateId == id && g.NavInputSource == ImGuiInputSource_NavKeyboard));
    const bool user_scroll_finish = is_multiline && state != NULL && g.ActiveId == 0 && g.ActiveIdPreviousFrame == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
    const bool user_scroll_active = is_multiline && state != NULL && g.ActiveId == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);

    bool clear_active_id = false;
    bool select_all = (g.ActiveId != id) && ((flags & ImGuiInputTextFlags_AutoSelectAll) != 0 || user_nav_input_start) && (!is_multiline);

    const bool init_make_active = (focus_requested || user_clicked || user_scroll_finish || user_nav_input_start);
    const bool init_state = (init_make_active || user_scroll_active);
    if (init_state && g.ActiveId != id)
    {
        // Access state even if we don't own it yet.
        state = &g.InputTextState;
        state->CursorAnimReset();

        // Take a copy of the initial buffer value (both in original UTF-8 format and converted to wchar)
        // From the moment we focused we are ignoring the content of 'buf' (unless we are in read-only mode)
        const int buf_len = (int)strlen(buf);
        state->InitialTextA.resize(buf_len + 1);    // UTF-8. we use +1 to make sure that .Data is always pointing to at least an empty string.
        memcpy(state->InitialTextA.Data, buf, buf_len + 1);

        // Start edition
        const char* buf_end = NULL;
        state->TextW.resize(buf_size + 1);          // wchar count <= UTF-8 count. we use +1 to make sure that .Data is always pointing to at least an empty string.
        state->TextA.resize(0);
        state->TextAIsValid = false;                // TextA is not valid yet (we will display buf until then)
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, buf_size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);      // We can't get the result from ImStrncpy() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.

        // Preserve cursor position and undo/redo stack if we come back to same widget
        // FIXME: For non-readonly widgets we might be able to require that TextAIsValid && TextA == buf ? (untested) and discard undo stack if user buffer has changed.
        const bool recycle_state = (state->ID == id);
        if (recycle_state)
        {
            // Recycle existing cursor/selection/undo stack but clamp position
            // Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
            state->CursorClamp();
        } else
        {
            state->ID = id;
            state->ScrollX = 0.0f;
            stb_textedit_initialize_state(&state->Stb, !is_multiline);
            if (!is_multiline && focus_requested_by_code)
                select_all = true;
        }
        if (flags & ImGuiInputTextFlags_AlwaysInsertMode)
            state->Stb.insert_mode = 1;
        if (!is_multiline && (focus_requested_by_tab || (user_clicked && io.KeyCtrl)))
            select_all = true;
    }

    if (g.ActiveId != id && init_make_active)
    {
        IM_ASSERT(state && state->ID == id);
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);

        // Declare our inputs
        IM_ASSERT(ImGuiNavInput_COUNT < 32);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        if (is_multiline || (flags & ImGuiInputTextFlags_CallbackHistory))
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
        g.ActiveIdUsingNavInputMask |= (1 << ImGuiNavInput_Cancel);
        g.ActiveIdUsingKeyInputMask |= ((ImU64)1 << ImGuiKey_Home) | ((ImU64)1 << ImGuiKey_End);
        if (is_multiline)
            g.ActiveIdUsingKeyInputMask |= ((ImU64)1 << ImGuiKey_PageUp) | ((ImU64)1 << ImGuiKey_PageDown); // FIXME-NAV: Page up/down actually not supported yet by widget, but claim them ahead.
        if (flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_AllowTabInput))  // Disable keyboard tabbing out as we will use the \t character.
            g.ActiveIdUsingKeyInputMask |= ((ImU64)1 << ImGuiKey_Tab);
    }

    // We have an edge case if ActiveId was set through another widget (e.g. widget being swapped), clear id immediately (don't wait until the end of the function)
    if (g.ActiveId == id && state == NULL)
        ClearActiveID();

    // Release focus when we click outside
    if (g.ActiveId == id && io.MouseClicked[0] && !init_state && !init_make_active) //-V560
        clear_active_id = true;

    // Lock the decision of whether we are going to take the path displaying the cursor or selection
    const bool render_cursor = (g.ActiveId == id) || (state && user_scroll_active);
    bool render_selection = state && state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    bool value_changed = false;
    bool enter_pressed = false;

    // When read-only we always use the live data passed to the function
    // FIXME-OPT: Because our selection/cursor code currently needs the wide text we need to convert it when active, which is not ideal :(
    if (is_readonly && state != NULL && (render_cursor || render_selection))
    {
        const char* buf_end = NULL;
        state->TextW.resize(buf_size + 1);
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);
        state->CursorClamp();
        render_selection &= state->HasSelection();
    }

    // Select the buffer to render.
    const bool buf_display_from_state = (render_cursor || render_selection || g.ActiveId == id) && !is_readonly && state && state->TextAIsValid;
    const bool is_displaying_hint = (hint != NULL && (buf_display_from_state ? state->TextA.Data : buf)[0] == 0);

    // Password pushes a temporary font with only a fallback glyph
    if (is_password && !is_displaying_hint)
    {
        const ImFontGlyph* glyph = g.Font->FindGlyph('*');
        ImFont* password_font = &g.InputTextPasswordFont;
        password_font->FontSize = g.Font->FontSize;
        password_font->Scale = g.Font->Scale;
        password_font->DisplayOffset = g.Font->DisplayOffset;
        password_font->Ascent = g.Font->Ascent;
        password_font->Descent = g.Font->Descent;
        password_font->ContainerAtlas = g.Font->ContainerAtlas;
        password_font->FallbackGlyph = glyph;
        password_font->FallbackAdvanceX = glyph->AdvanceX;
        IM_ASSERT(password_font->Glyphs.empty() && password_font->IndexAdvanceX.empty() && password_font->IndexLookup.empty());
        PushFont(password_font);
    }

    // Process mouse inputs and character inputs
    int backup_current_text_length = 0;
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        backup_current_text_length = state->CurLenA;
        state->BufCapacityA = buf_size;
        state->UserFlags = flags;
        state->UserCallback = callback;
        state->UserCallbackData = callback_user_data;

        // Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
        // Down the line we should have a cleaner library-wide concept of Selected vs Active.
        g.ActiveIdAllowOverlap = !io.MouseDown[0];
        g.WantTextInputNextFrame = 1;

        // Edit in progress
        const float mouse_x = (io.MousePos.x - frame_bb.Min.x - style.FramePadding.x) + state->ScrollX;
        const float mouse_y = (is_multiline ? (io.MousePos.y - draw_window->DC.CursorPos.y - style.FramePadding.y) : (g.FontSize * 0.5f));

        const bool is_osx = io.ConfigMacOSXBehaviors;
        if (select_all || (hovered && !is_osx && io.MouseDoubleClicked[0]))
        {
            state->SelectAll();
            state->SelectedAllMouseLock = true;
        } else if (hovered && is_osx && io.MouseDoubleClicked[0])
        {
            // Double-click select a word only, OS X style (by simulating keystrokes)
            state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);
            state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
        } else if (io.MouseClicked[0] && !state->SelectedAllMouseLock)
        {
            if (hovered)
            {
                stb_textedit_click(state, &state->Stb, mouse_x, mouse_y);
                state->CursorAnimReset();
            }
        } else if (io.MouseDown[0] && !state->SelectedAllMouseLock && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
        {
            stb_textedit_drag(state, &state->Stb, mouse_x, mouse_y);
            state->CursorAnimReset();
            state->CursorFollow = true;
        }
        if (state->SelectedAllMouseLock && !io.MouseDown[0])
            state->SelectedAllMouseLock = false;

        // It is ill-defined whether the back-end needs to send a \t character when pressing the TAB keys.
        // Win32 and GLFW naturally do it but not SDL.
        const bool ignore_char_inputs = (io.KeyCtrl && !io.KeyAlt) || (is_osx && io.KeySuper);
        if ((flags & ImGuiInputTextFlags_AllowTabInput) && IsKeyPressedMap(ImGuiKey_Tab) && !ignore_char_inputs && !io.KeyShift && !is_readonly)
            if (!io.InputQueueCharacters.contains('\t'))
            {
                unsigned int c = '\t'; // Insert TAB
                if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                    state->OnKeyPressed((int)c);
            }

        // Process regular text input (before we check for Return because using some IME will effectively send a Return?)
        // We ignore CTRL inputs, but need to allow ALT+CTRL as some keyboards (e.g. German) use AltGR (which _is_ Alt+Ctrl) to input certain characters.
        if (io.InputQueueCharacters.Size > 0)
        {
            if (!ignore_char_inputs && !is_readonly && !user_nav_input_start)
                for (int n = 0; n < io.InputQueueCharacters.Size; n++)
                {
                    // Insert character if they pass filtering
                    unsigned int c = (unsigned int)io.InputQueueCharacters[n];
                    if (c == '\t' && io.KeyShift)
                        continue;
                    if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                        state->OnKeyPressed((int)c);
                }

            // Consume characters
            io.InputQueueCharacters.resize(0);
        }
    }

    // Process other shortcuts/key-presses
    bool cancel_edit = false;
    if (g.ActiveId == id && !g.ActiveIdIsJustActivated && !clear_active_id)
    {
        IM_ASSERT(state != NULL);
        IM_ASSERT(io.KeyMods == GetMergedKeyModFlags() && "Mismatching io.KeyCtrl/io.KeyShift/io.KeyAlt/io.KeySuper vs io.KeyMods"); // We rarely do this check, but if anything let's do it here.

        const int k_mask = (io.KeyShift ? STB_TEXTEDIT_K_SHIFT : 0);
        const bool is_osx = io.ConfigMacOSXBehaviors;
        const bool is_osx_shift_shortcut = is_osx && (io.KeyMods == (ImGuiKeyModFlags_Super | ImGuiKeyModFlags_Shift));
        const bool is_wordmove_key_down = is_osx ? io.KeyAlt : io.KeyCtrl;                     // OS X style: Text editing cursor movement using Alt instead of Ctrl
        const bool is_startend_key_down = is_osx && io.KeySuper && !io.KeyCtrl && !io.KeyAlt;  // OS X style: Line/Text Start and End using Cmd+Arrows instead of Home/End
        const bool is_ctrl_key_only = (io.KeyMods == ImGuiKeyModFlags_Ctrl);
        const bool is_shift_key_only = (io.KeyMods == ImGuiKeyModFlags_Shift);
        const bool is_shortcut_key = g.IO.ConfigMacOSXBehaviors ? (io.KeyMods == ImGuiKeyModFlags_Super) : (io.KeyMods == ImGuiKeyModFlags_Ctrl);

        const bool is_cut = ((is_shortcut_key && IsKeyPressedMap(ImGuiKey_X)) || (is_shift_key_only && IsKeyPressedMap(ImGuiKey_Delete))) && !is_readonly && !is_password && (!is_multiline || state->HasSelection());
        const bool is_copy = ((is_shortcut_key && IsKeyPressedMap(ImGuiKey_C)) || (is_ctrl_key_only && IsKeyPressedMap(ImGuiKey_Insert))) && !is_password && (!is_multiline || state->HasSelection());
        const bool is_paste = ((is_shortcut_key && IsKeyPressedMap(ImGuiKey_V)) || (is_shift_key_only && IsKeyPressedMap(ImGuiKey_Insert))) && !is_readonly;
        const bool is_undo = ((is_shortcut_key && IsKeyPressedMap(ImGuiKey_Z)) && !is_readonly && is_undoable);
        const bool is_redo = ((is_shortcut_key && IsKeyPressedMap(ImGuiKey_Y)) || (is_osx_shift_shortcut && IsKeyPressedMap(ImGuiKey_Z))) && !is_readonly && is_undoable;

        if (IsKeyPressedMap(ImGuiKey_LeftArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINESTART : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) | k_mask); } else if (IsKeyPressedMap(ImGuiKey_RightArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINEEND : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDRIGHT : STB_TEXTEDIT_K_RIGHT) | k_mask); } else if (IsKeyPressedMap(ImGuiKey_UpArrow) && is_multiline) { if (io.KeyCtrl) SetScrollY(draw_window, ImMax(draw_window->Scroll.y - g.FontSize, 0.0f)); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_UP) | k_mask); } else if (IsKeyPressedMap(ImGuiKey_DownArrow) && is_multiline) { if (io.KeyCtrl) SetScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, GetScrollMaxY())); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); } else if (IsKeyPressedMap(ImGuiKey_Home)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); } else if (IsKeyPressedMap(ImGuiKey_End)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); } else if (IsKeyPressedMap(ImGuiKey_Delete) && !is_readonly) { state->OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask); } else if (IsKeyPressedMap(ImGuiKey_Backspace) && !is_readonly)
        {
            if (!state->HasSelection())
            {
                if (is_wordmove_key_down)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT | STB_TEXTEDIT_K_SHIFT);
                else if (is_osx && io.KeySuper && !io.KeyAlt && !io.KeyCtrl)
                    state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_SHIFT);
            }
            state->OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
        } else if (IsKeyPressedMap(ImGuiKey_Enter) || IsKeyPressedMap(ImGuiKey_KeyPadEnter))
        {
            bool ctrl_enter_for_new_line = (flags & ImGuiInputTextFlags_CtrlEnterForNewLine) != 0;
            if (!is_multiline || (ctrl_enter_for_new_line && !io.KeyCtrl) || (!ctrl_enter_for_new_line && io.KeyCtrl))
            {
                enter_pressed = clear_active_id = true;
            } else if (!is_readonly)
            {
                unsigned int c = '\n'; // Insert new line
                if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                    state->OnKeyPressed((int)c);
            }
        } else if (IsKeyPressedMap(ImGuiKey_Escape))
        {
            clear_active_id = cancel_edit = true;
        } else if (is_undo || is_redo)
        {
            state->OnKeyPressed(is_undo ? STB_TEXTEDIT_K_UNDO : STB_TEXTEDIT_K_REDO);
            state->ClearSelection();
        } else if (is_shortcut_key && IsKeyPressedMap(ImGuiKey_A))
        {
            state->SelectAll();
            state->CursorFollow = true;
        } else if (is_cut || is_copy)
        {
            // Cut, Copy
            if (io.SetClipboardTextFn)
            {
                const int ib = state->HasSelection() ? ImMin(state->Stb.select_start, state->Stb.select_end) : 0;
                const int ie = state->HasSelection() ? ImMax(state->Stb.select_start, state->Stb.select_end) : state->CurLenW;
                const int clipboard_data_len = ImTextCountUtf8BytesFromStr(state->TextW.Data + ib, state->TextW.Data + ie) + 1;
                char* clipboard_data = (char*)IM_ALLOC(clipboard_data_len * sizeof(char));
                ImTextStrToUtf8(clipboard_data, clipboard_data_len, state->TextW.Data + ib, state->TextW.Data + ie);
                SetClipboardText(clipboard_data);
                MemFree(clipboard_data);
            }
            if (is_cut)
            {
                if (!state->HasSelection())
                    state->SelectAll();
                state->CursorFollow = true;
                stb_textedit_cut(state, &state->Stb);
            }
        } else if (is_paste)
        {
            if (const char* clipboard = GetClipboardText())
            {
                // Filter pasted buffer
                const int clipboard_len = (int)strlen(clipboard);
                ImWchar* clipboard_filtered = (ImWchar*)IM_ALLOC((clipboard_len + 1) * sizeof(ImWchar));
                int clipboard_filtered_len = 0;
                for (const char* s = clipboard; *s; )
                {
                    unsigned int c;
                    s += ImTextCharFromUtf8(&c, s, NULL);
                    if (c == 0)
                        break;
                    if (!InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                        continue;
                    clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
                }
                clipboard_filtered[clipboard_filtered_len] = 0;
                if (clipboard_filtered_len > 0) // If everything was filtered, ignore the pasting operation
                {
                    stb_textedit_paste(state, &state->Stb, clipboard_filtered, clipboard_filtered_len);
                    state->CursorFollow = true;
                }
                MemFree(clipboard_filtered);
            }
        }

        // Update render selection flag after events have been handled, so selection highlight can be displayed during the same frame.
        render_selection |= state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    }

    // Process callbacks and apply result back to user's buffer.
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        const char* apply_new_text = NULL;
        int apply_new_text_length = 0;
        if (cancel_edit)
        {
            // Restore initial value. Only return true if restoring to the initial value changes the current buffer contents.
            if (!is_readonly && strcmp(buf, state->InitialTextA.Data) != 0)
            {
                // Push records into the undo stack so we can CTRL+Z the revert operation itself
                apply_new_text = state->InitialTextA.Data;
                apply_new_text_length = state->InitialTextA.Size - 1;
                ImVector<ImWchar> w_text;
                if (apply_new_text_length > 0)
                {
                    w_text.resize(ImTextCountCharsFromUtf8(apply_new_text, apply_new_text + apply_new_text_length) + 1);
                    ImTextStrFromUtf8(w_text.Data, w_text.Size, apply_new_text, apply_new_text + apply_new_text_length);
                }
                stb_textedit_replace(state, &state->Stb, w_text.Data, (apply_new_text_length > 0) ? (w_text.Size - 1) : 0);
            }
        }

        // When using 'ImGuiInputTextFlags_EnterReturnsTrue' as a special case we reapply the live buffer back to the input buffer before clearing ActiveId, even though strictly speaking it wasn't modified on this frame.
        // If we didn't do that, code like InputInt() with ImGuiInputTextFlags_EnterReturnsTrue would fail.
        // This also allows the user to use InputText() with ImGuiInputTextFlags_EnterReturnsTrue without maintaining any user-side storage (please note that if you use this property along ImGuiInputTextFlags_CallbackResize you can end up with your temporary string object unnecessarily allocating once a frame, either store your string data, either if you don't then don't use ImGuiInputTextFlags_CallbackResize).
        bool apply_edit_back_to_user_buffer = !cancel_edit || (enter_pressed && (flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0);
        if (apply_edit_back_to_user_buffer)
        {
            // Apply new value immediately - copy modified buffer back
            // Note that as soon as the input box is active, the in-widget value gets priority over any underlying modification of the input buffer
            // FIXME: We actually always render 'buf' when calling DrawList->AddText, making the comment above incorrect.
            // FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks.
            if (!is_readonly)
            {
                state->TextAIsValid = true;
                state->TextA.resize(state->TextW.Size * 4 + 1);
                ImTextStrToUtf8(state->TextA.Data, state->TextA.Size, state->TextW.Data, NULL);
            }

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
                } else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_UpArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_UpArrow;
                } else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_DownArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_DownArrow;
                } else if (flags & ImGuiInputTextFlags_CallbackAlways)
                    event_flag = ImGuiInputTextFlags_CallbackAlways;

                if (event_flag)
                {
                    ImGuiInputTextCallbackData callback_data;
                    memset(&callback_data, 0, sizeof(ImGuiInputTextCallbackData));
                    callback_data.EventFlag = event_flag;
                    callback_data.Flags = flags;
                    callback_data.UserData = callback_user_data;

                    callback_data.EventKey = event_key;
                    callback_data.Buf = state->TextA.Data;
                    callback_data.BufTextLen = state->CurLenA;
                    callback_data.BufSize = state->BufCapacityA;
                    callback_data.BufDirty = false;

                    // We have to convert from wchar-positions to UTF-8-positions, which can be pretty slow (an incentive to ditch the ImWchar buffer, see https://github.com/nothings/stb/issues/188)
                    ImWchar* text = state->TextW.Data;
                    const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + state->Stb.cursor);
                    const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_start);
                    const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_end);

                    // Call user code
                    callback(&callback_data);

                    // Read back what user may have modified
                    IM_ASSERT(callback_data.Buf == state->TextA.Data);  // Invalid to modify those fields
                    IM_ASSERT(callback_data.BufSize == state->BufCapacityA);
                    IM_ASSERT(callback_data.Flags == flags);
                    if (callback_data.CursorPos != utf8_cursor_pos) { state->Stb.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos); state->CursorFollow = true; }
                    if (callback_data.SelectionStart != utf8_selection_start) { state->Stb.select_start = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart); }
                    if (callback_data.SelectionEnd != utf8_selection_end) { state->Stb.select_end = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd); }
                    if (callback_data.BufDirty)
                    {
                        IM_ASSERT(callback_data.BufTextLen == (int)strlen(callback_data.Buf)); // You need to maintain BufTextLen if you change the text!
                        if (callback_data.BufTextLen > backup_current_text_length && is_resizable)
                            state->TextW.resize(state->TextW.Size + (callback_data.BufTextLen - backup_current_text_length));
                        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, callback_data.Buf, NULL);
                        state->CurLenA = callback_data.BufTextLen;  // Assume correct length and valid UTF-8 from user, saves us an extra strlen()
                        state->CursorAnimReset();
                    }
                }
            }

            // Will copy result string if modified
            if (!is_readonly && strcmp(state->TextA.Data, buf) != 0)
            {
                apply_new_text = state->TextA.Data;
                apply_new_text_length = state->CurLenA;
            }
        }

        // Copy result to user buffer
        if (apply_new_text)
        {
            // We cannot test for 'backup_current_text_length != apply_new_text_length' here because we have no guarantee that the size
            // of our owned buffer matches the size of the string object held by the user, and by design we allow InputText() to be used
            // without any storage on user's side.
            IM_ASSERT(apply_new_text_length >= 0);
            if (is_resizable)
            {
                ImGuiInputTextCallbackData callback_data;
                callback_data.EventFlag = ImGuiInputTextFlags_CallbackResize;
                callback_data.Flags = flags;
                callback_data.Buf = buf;
                callback_data.BufTextLen = apply_new_text_length;
                callback_data.BufSize = ImMax(buf_size, apply_new_text_length + 1);
                callback_data.UserData = callback_user_data;
                callback(&callback_data);
                buf = callback_data.Buf;
                buf_size = callback_data.BufSize;
                apply_new_text_length = ImMin(callback_data.BufTextLen, buf_size - 1);
                IM_ASSERT(apply_new_text_length <= buf_size);
            }
            //IMGUI_DEBUG_LOG("InputText(\"%s\"): apply_new_text length %d\n", label, apply_new_text_length);

            // If the underlying buffer resize was denied or not carried to the next frame, apply_new_text_length+1 may be >= buf_size.
            ImStrncpy(buf, apply_new_text, ImMin(apply_new_text_length + 1, buf_size));
            value_changed = true;
        }

        // Clear temporary user storage
        state->UserFlags = 0;
        state->UserCallback = NULL;
        state->UserCallbackData = NULL;
    }

    // Release active ID at the end of the function (so e.g. pressing Return still does a final application of the value)
    if (clear_active_id && g.ActiveId == id)
        ClearActiveID();

    // Render frame
    if (!is_multiline)
    {
        RenderNavHighlight(frame_bb, id);
        RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    }

    const ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + inner_size.x, frame_bb.Min.y + inner_size.y); // Not using frame_bb.Max because we have adjusted size
    ImVec2 draw_pos = is_multiline ? draw_window->DC.CursorPos : frame_bb.Min + style.FramePadding;
    ImVec2 text_size(0.0f, 0.0f);

    // Set upper limit of single-line InputTextEx() at 2 million characters strings. The current pathological worst case is a long line
    // without any carriage return, which would makes ImFont::RenderText() reserve too many vertices and probably crash. Avoid it altogether.
    // Note that we only use this limit on single-line InputText(), so a pathologically large line on a InputTextMultiline() would still crash.
    const int buf_display_max_length = 2 * 1024 * 1024;
    const char* buf_display = buf_display_from_state ? state->TextA.Data : buf; //-V595
    const char* buf_display_end = NULL; // We have specialized paths below for setting the length
    if (is_displaying_hint)
    {
        buf_display = hint;
        buf_display_end = hint + strlen(hint);
    }

    // Render text. We currently only render selection when the widget is active or while scrolling.
    // FIXME: We could remove the '&& render_cursor' to keep rendering selection when inactive.
    if (render_cursor || render_selection)
    {
        IM_ASSERT(state != NULL);
        if (!is_displaying_hint)
            buf_display_end = buf_display + state->CurLenA;

        // Render text (with cursor and selection)
        // This is going to be messy. We need to:
        // - Display the text (this alone can be more easily clipped)
        // - Handle scrolling, highlight selection, display cursor (those all requires some form of 1d->2d cursor position calculation)
        // - Measure text height (for scrollbar)
        // We are attempting to do most of that in **one main pass** to minimize the computation cost (non-negligible for large amount of text) + 2nd pass for selection rendering (we could merge them by an extra refactoring effort)
        // FIXME: This should occur on buf_display but we'd need to maintain cursor/select_start/select_end for UTF-8.
        const ImWchar* text_begin = state->TextW.Data;
        ImVec2 cursor_offset, select_start_offset;

        {
            // Find lines numbers straddling 'cursor' (slot 0) and 'select_start' (slot 1) positions.
            const ImWchar* searches_input_ptr[2] = { NULL, NULL };
            int searches_result_line_no[2] = { -1000, -1000 };
            int searches_remaining = 0;
            if (render_cursor)
            {
                searches_input_ptr[0] = text_begin + state->Stb.cursor;
                searches_result_line_no[0] = -1;
                searches_remaining++;
            }
            if (render_selection)
            {
                searches_input_ptr[1] = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
                searches_result_line_no[1] = -1;
                searches_remaining++;
            }

            // Iterate all lines to find our line numbers
            // In multi-line mode, we never exit the loop until all lines are counted, so add one extra to the searches_remaining counter.
            searches_remaining += is_multiline ? 1 : 0;
            int line_count = 0;
            //for (const ImWchar* s = text_begin; (s = (const ImWchar*)wcschr((const wchar_t*)s, (wchar_t)'\n')) != NULL; s++)  // FIXME-OPT: Could use this when wchar_t are 16-bit
            for (const ImWchar* s = text_begin; *s != 0; s++)
                if (*s == '\n')
                {
                    line_count++;
                    if (searches_result_line_no[0] == -1 && s >= searches_input_ptr[0]) { searches_result_line_no[0] = line_count; if (--searches_remaining <= 0) break; }
                    if (searches_result_line_no[1] == -1 && s >= searches_input_ptr[1]) { searches_result_line_no[1] = line_count; if (--searches_remaining <= 0) break; }
                }
            line_count++;
            if (searches_result_line_no[0] == -1)
                searches_result_line_no[0] = line_count;
            if (searches_result_line_no[1] == -1)
                searches_result_line_no[1] = line_count;

            // Calculate 2d position by finding the beginning of the line and measuring distance
            cursor_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[0], text_begin), searches_input_ptr[0]).x;
            cursor_offset.y = searches_result_line_no[0] * g.FontSize;
            if (searches_result_line_no[1] >= 0)
            {
                select_start_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[1], text_begin), searches_input_ptr[1]).x;
                select_start_offset.y = searches_result_line_no[1] * g.FontSize;
            }

            // Store text height (note that we haven't calculated text width at all, see GitHub issues #383, #1224)
            if (is_multiline)
                text_size = ImVec2(inner_size.x, line_count * g.FontSize);
        }

        // Scroll
        if (render_cursor && state->CursorFollow)
        {
            // Horizontal scroll in chunks of quarter width
            if (!(flags & ImGuiInputTextFlags_NoHorizontalScroll))
            {
                const float scroll_increment_x = inner_size.x * 0.25f;
                if (cursor_offset.x < state->ScrollX)
                    state->ScrollX = IM_FLOOR(ImMax(0.0f, cursor_offset.x - scroll_increment_x));
                else if (cursor_offset.x - inner_size.x >= state->ScrollX)
                    state->ScrollX = IM_FLOOR(cursor_offset.x - inner_size.x + scroll_increment_x);
            } else
            {
                state->ScrollX = 0.0f;
            }

            // Vertical scroll
            if (is_multiline)
            {
                float scroll_y = draw_window->Scroll.y;
                if (cursor_offset.y - g.FontSize < scroll_y)
                    scroll_y = ImMax(0.0f, cursor_offset.y - g.FontSize);
                else if (cursor_offset.y - inner_size.y >= scroll_y)
                    scroll_y = cursor_offset.y - inner_size.y;
                draw_pos.y += (draw_window->Scroll.y - scroll_y);   // Manipulate cursor pos immediately avoid a frame of lag
                draw_window->Scroll.y = scroll_y;
            }

            state->CursorFollow = false;
        }

        // Draw selection
        const ImVec2 draw_scroll = ImVec2(state->ScrollX, 0.0f);
        if (render_selection)
        {
            const ImWchar* text_selected_begin = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
            const ImWchar* text_selected_end = text_begin + ImMax(state->Stb.select_start, state->Stb.select_end);

            ImU32 bg_color = GetColorU32(ImGuiCol_TextSelectedBg, render_cursor ? 1.0f : 0.6f); // FIXME: current code flow mandate that render_cursor is always true here, we are leaving the transparent one for tests.
            float bg_offy_up = is_multiline ? 0.0f : -1.0f;    // FIXME: those offsets should be part of the style? they don't play so well with multi-line selection.
            float bg_offy_dn = is_multiline ? 0.0f : 2.0f;
            ImVec2 rect_pos = draw_pos + select_start_offset - draw_scroll;
            for (const ImWchar* p = text_selected_begin; p < text_selected_end; )
            {
                if (rect_pos.y > clip_rect.w + g.FontSize)
                    break;
                if (rect_pos.y < clip_rect.y)
                {
                    //p = (const ImWchar*)wmemchr((const wchar_t*)p, '\n', text_selected_end - p);  // FIXME-OPT: Could use this when wchar_t are 16-bit
                    //p = p ? p + 1 : text_selected_end;
                    while (p < text_selected_end)
                        if (*p++ == '\n')
                            break;
                } else
                {
                    ImVec2 rect_size = InputTextCalcTextSizeW(p, text_selected_end, &p, NULL, true);
                    if (rect_size.x <= 0.0f) rect_size.x = IM_FLOOR(g.Font->GetCharAdvance((ImWchar)' ') * 0.50f); // So we can see selected empty lines
                    ImRect rect(rect_pos + ImVec2(0.0f, bg_offy_up - g.FontSize), rect_pos + ImVec2(rect_size.x, bg_offy_dn));
                    rect.ClipWith(clip_rect);
                    if (rect.Overlaps(clip_rect))
                        draw_window->DrawList->AddRectFilled(rect.Min, rect.Max, bg_color);
                }
                rect_pos.x = draw_pos.x - draw_scroll.x;
                rect_pos.y += g.FontSize;
            }
        }

        // We test for 'buf_display_max_length' as a way to avoid some pathological cases (e.g. single-line 1 MB string) which would make ImDrawList crash.
        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
        {
            ImU32 col = GetColorU32(is_displaying_hint ? ImGuiCol_TextDisabled : ImGuiCol_Text);
            draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos - draw_scroll, col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
        }

        // Draw blinking cursor
        if (render_cursor)
        {
            state->CursorAnim += io.DeltaTime;
            bool cursor_is_visible = (!g.IO.ConfigInputTextCursorBlink) || (state->CursorAnim <= 0.0f) || ImFmod(state->CursorAnim, 1.20f) <= 0.80f;
            ImVec2 cursor_screen_pos = draw_pos + cursor_offset - draw_scroll;
            ImRect cursor_screen_rect(cursor_screen_pos.x, cursor_screen_pos.y - g.FontSize + 0.5f, cursor_screen_pos.x + 1.0f, cursor_screen_pos.y - 1.5f);
            if (cursor_is_visible && cursor_screen_rect.Overlaps(clip_rect))
                draw_window->DrawList->AddLine(cursor_screen_rect.Min, cursor_screen_rect.GetBL(), GetColorU32(ImGuiCol_Text));

            // Notify OS of text input position for advanced IME (-1 x offset so that Windows IME can cover our cursor. Bit of an extra nicety.)
            if (!is_readonly)
                g.PlatformImePos = ImVec2(cursor_screen_pos.x - 1.0f, cursor_screen_pos.y - g.FontSize);
        }
    } else
    {
        // Render text only (no selection, no cursor)
        if (is_multiline)
            text_size = ImVec2(inner_size.x, InputTextCalcTextLenAndLineCount(buf_display, &buf_display_end) * g.FontSize); // We don't need width
        else if (!is_displaying_hint && g.ActiveId == id)
            buf_display_end = buf_display + state->CurLenA;
        else if (!is_displaying_hint)
            buf_display_end = buf_display + strlen(buf_display);

        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
        {
            ImU32 col = GetColorU32(is_displaying_hint ? ImGuiCol_TextDisabled : ImGuiCol_Text);
            draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos, col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
        }
    }

    if (is_multiline)
    {
        Dummy(text_size + ImVec2(0.0f, g.FontSize)); // Always add room to scroll an extra line
        EndChild();
        EndGroup();
    }

    if (is_password && !is_displaying_hint)
        PopFont();

    // Log as text
    if (g.LogEnabled && !(is_password && !is_displaying_hint))
        LogRenderedText(&draw_pos, buf_display, buf_display_end);

    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    if (value_changed && !(flags & ImGuiInputTextFlags_NoMarkEdited))
        MarkItemEdited(id);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    if ((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0)
        return enter_pressed;
    else
        return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: ColorEdit, ColorPicker, ColorButton, etc.
//-------------------------------------------------------------------------
// - ColorEdit3()
// - ColorEdit4()
// - ColorPicker3()
// - RenderColorRectWithAlphaCheckerboard() [Internal]
// - ColorPicker4()
// - ColorButton()
// - SetColorEditOptions()
// - ColorTooltip() [Internal]
// - ColorEditOptionsPopup() [Internal]
// - ColorPickerOptionsPopup() [Internal]
//-------------------------------------------------------------------------

bool ImGui::ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags)
{
    return ColorEdit4(label, col, flags | ImGuiColorEditFlags_NoAlpha);
}

// Edit colors components (each component in 0.0f..1.0f range).
// See enum ImGuiColorEditFlags_ for available options. e.g. Only access 3 floats if ImGuiColorEditFlags_NoAlpha flag is set.
// With typical options: Left-click on colored square to open color picker. Right-click to open option menu. CTRL-Click over input fields to edit them and TAB to go to next item.
bool ImGui::ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float square_sz = GetFrameHeight();
    const float w_full = CalcItemWidth();
    const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
    const float w_inputs = w_full - w_button;
    const char* label_display_end = FindRenderedTextEnd(label);
    g.NextItemData.ClearFlags();

    BeginGroup();
    PushID(label);

    // If we're not showing any slider there's no point in doing any HSV conversions
    const ImGuiColorEditFlags flags_untouched = flags;
    if (flags & ImGuiColorEditFlags_NoInputs)
        flags = (flags & (~ImGuiColorEditFlags__DisplayMask)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

    // Context menu: display and modify options (before defaults are applied)
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        ColorEditOptionsPopup(col, flags);

    // Read stored options
    if (!(flags & ImGuiColorEditFlags__DisplayMask))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DisplayMask);
    if (!(flags & ImGuiColorEditFlags__DataTypeMask))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DataTypeMask);
    if (!(flags & ImGuiColorEditFlags__PickerMask))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags__PickerMask);
    if (!(flags & ImGuiColorEditFlags__InputMask))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags__InputMask);
    flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags__DisplayMask | ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask | ImGuiColorEditFlags__InputMask));
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__DisplayMask)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__InputMask));   // Check that only 1 is selected

    const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
    const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;

    // Convert to the formats we need
    float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
    if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
        ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
    else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
    {
        // Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
        ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
        if (memcmp(g.ColorEditLastColor, col, sizeof(float) * 3) == 0)
        {
            if (f[1] == 0)
                f[0] = g.ColorEditLastHue;
            if (f[2] == 0)
                f[1] = g.ColorEditLastSat;
        }
    }
    int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

    bool value_changed = false;
    bool value_changed_as_float = false;

    const ImVec2 pos = window->DC.CursorPos;
    const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
    window->DC.CursorPos.x = pos.x + inputs_offset_x;

    if ((flags & (ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
    {
        // RGB/HSV 0..255 Sliders
        const float w_item_one = ImMax(1.0f, IM_FLOOR((w_inputs - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
        const float w_item_last = ImMax(1.0f, IM_FLOOR(w_inputs - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

        const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
        static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
        static const char* fmt_table_int[3][4] =
        {
            {   "%3d",   "%3d",   "%3d",   "%3d" }, // Short display
            { "R:%3d", "G:%3d", "B:%3d", "A:%3d" }, // Long display for RGBA
            { "H:%3d", "S:%3d", "V:%3d", "A:%3d" }  // Long display for HSVA
        };
        static const char* fmt_table_float[3][4] =
        {
            {   "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
            { "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
            { "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }  // Long display for HSVA
        };
        const int fmt_idx = hide_prefix ? 0 : (flags & ImGuiColorEditFlags_DisplayHSV) ? 2 : 1;

        for (int n = 0; n < components; n++)
        {
            if (n > 0)
                SameLine(0, style.ItemInnerSpacing.x);
            SetNextItemWidth((n + 1 < components) ? w_item_one : w_item_last);

            // FIXME: When ImGuiColorEditFlags_HDR flag is passed HS values snap in weird ways when SV values go below 0.
            if (flags & ImGuiColorEditFlags_Float)
            {
                value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
                value_changed_as_float |= value_changed;
            } else
            {
                value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);
            }
            if (!(flags & ImGuiColorEditFlags_NoOptions))
                OpenPopupOnItemClick("context");
        }
    } else if ((flags & ImGuiColorEditFlags_DisplayHex) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
    {
        // RGB Hexadecimal Input
        char buf[64];
        if (alpha)
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
        else
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));
        SetNextItemWidth(w_inputs);
        if (InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase))
        {
            value_changed = true;
            char* p = buf;
            while (*p == '#' || ImCharIsBlankA(*p))
                p++;
            i[0] = i[1] = i[2] = i[3] = 0;
            if (alpha)
                sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
            else
                sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context");
    }

    ImGuiWindow* picker_active_window = NULL;
    if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
    {
        const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;
        window->DC.CursorPos = ImVec2(pos.x + button_offset_x, pos.y);

        const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);
        if (ColorButton("##ColorButton", col_v4, flags))
        {
            if (!(flags & ImGuiColorEditFlags_NoPicker))
            {
                // Store current color and open a picker
                g.ColorPickerRef = col_v4;
                OpenPopup("picker");
                SetNextWindowPos(window->DC.LastItemRect.GetBL() + ImVec2(-1, style.ItemSpacing.y));
            }
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context");

        if (BeginPopup("picker"))
        {
            picker_active_window = g.CurrentWindow;
            if (label != label_display_end)
            {
                TextEx(label, label_display_end);
                Spacing();
            }
            ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask | ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
            ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags__DisplayMask | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
            SetNextItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?
            value_changed |= ColorPicker4("##picker", col, picker_flags, &g.ColorPickerRef.x);
            EndPopup();
        }
    }

    if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
    {
        const float text_offset_x = (flags & ImGuiColorEditFlags_NoInputs) ? w_button : w_full + style.ItemInnerSpacing.x;
        window->DC.CursorPos = ImVec2(pos.x + text_offset_x, pos.y + style.FramePadding.y);
        TextEx(label, label_display_end);
    }

    // Convert back
    if (value_changed && picker_active_window == NULL)
    {
        if (!value_changed_as_float)
            for (int n = 0; n < 4; n++)
                f[n] = i[n] / 255.0f;
        if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
        {
            g.ColorEditLastHue = f[0];
            g.ColorEditLastSat = f[1];
            ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
            memcpy(g.ColorEditLastColor, f, sizeof(float) * 3);
        }
        if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
            ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

        col[0] = f[0];
        col[1] = f[1];
        col[2] = f[2];
        if (alpha)
            col[3] = f[3];
    }

    PopID();
    EndGroup();

    // Drag and Drop Target
    // NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
    if ((window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropTarget())
    {
        bool accepted_drag_drop = false;
        if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
        {
            memcpy((float*)col, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512
            value_changed = accepted_drag_drop = true;
        }
        if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
        {
            memcpy((float*)col, payload->Data, sizeof(float) * components);
            value_changed = accepted_drag_drop = true;
        }

        // Drag-drop payloads are always RGB
        if (accepted_drag_drop && (flags & ImGuiColorEditFlags_InputHSV))
            ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);
        EndDragDropTarget();
    }

    // When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
    if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
        window->DC.LastItemId = g.ActiveId;

    if (value_changed)
        MarkItemEdited(window->DC.LastItemId);

    return value_changed;
}

bool ImGui::ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags)
{
    float col4[4] = { col[0], col[1], col[2], 1.0f };
    if (!ColorPicker4(label, col4, flags | ImGuiColorEditFlags_NoAlpha))
        return false;
    col[0] = col4[0]; col[1] = col4[1]; col[2] = col4[2];
    return true;
}

// Helper for ColorPicker4()
static void RenderArrowsForVerticalBar(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w, float alpha)
{
    ImU32 alpha8 = IM_F32_TO_INT8_SAT(alpha);
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x + 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Right, IM_COL32(0, 0, 0, alpha8));
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x, pos.y), half_sz, ImGuiDir_Right, IM_COL32(255, 255, 255, alpha8));
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x - 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Left, IM_COL32(0, 0, 0, alpha8));
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x, pos.y), half_sz, ImGuiDir_Left, IM_COL32(255, 255, 255, alpha8));
}

// Note: ColorPicker4() only accesses 3 floats if ImGuiColorEditFlags_NoAlpha flag is set.
// (In C++ the 'float col[4]' notation for a function argument is equivalent to 'float* col', we only specify a size to facilitate understanding of the code.)
// FIXME: we adjust the big color square height based on item width, which may cause a flickering feedback loop (if automatic height makes a vertical scrollbar appears, affecting automatic width..)
// FIXME: this is trying to be aware of style.Alpha but not fully correct. Also, the color wheel will have overlapping glitches with (style.Alpha < 1.0)
bool ImGui::ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImDrawList* draw_list = window->DrawList;
    ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO;

    const float width = CalcItemWidth();
    g.NextItemData.ClearFlags();

    PushID(label);
    BeginGroup();

    if (!(flags & ImGuiColorEditFlags_NoSidePreview))
        flags |= ImGuiColorEditFlags_NoSmallPreview;

    // Context menu: display and store options.
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        ColorPickerOptionsPopup(col, flags);

    // Read stored options
    if (!(flags & ImGuiColorEditFlags__PickerMask))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags__PickerMask) ? g.ColorEditOptions : ImGuiColorEditFlags__OptionsDefault) & ImGuiColorEditFlags__PickerMask;
    if (!(flags & ImGuiColorEditFlags__InputMask))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags__InputMask) ? g.ColorEditOptions : ImGuiColorEditFlags__OptionsDefault) & ImGuiColorEditFlags__InputMask;
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__PickerMask)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__InputMask));  // Check that only 1 is selected
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

    // Setup
    int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
    bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
    ImVec2 picker_pos = window->DC.CursorPos;
    float square_sz = GetFrameHeight();
    float bars_width = square_sz; // Arbitrary smallish width of Hue/Alpha picking bars
    float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)); // Saturation/Value picking box
    float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
    float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
    float bars_triangles_half_sz = IM_FLOOR(bars_width * 0.20f);

    float backup_initial_col[4];
    memcpy(backup_initial_col, col, components * sizeof(float));

    float wheel_thickness = sv_picker_size * 0.08f;
    float wheel_r_outer = sv_picker_size * 0.50f;
    float wheel_r_inner = wheel_r_outer - wheel_thickness;
    ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

    // Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
    float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
    ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
    ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
    ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

    float H = col[0], S = col[1], V = col[2];
    float R = col[0], G = col[1], B = col[2];
    if (flags & ImGuiColorEditFlags_InputRGB)
    {
        // Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
        ColorConvertRGBtoHSV(R, G, B, H, S, V);
        if (memcmp(g.ColorEditLastColor, col, sizeof(float) * 3) == 0)
        {
            if (S == 0)
                H = g.ColorEditLastHue;
            if (V == 0)
                S = g.ColorEditLastSat;
        }
    } else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        ColorConvertHSVtoRGB(H, S, V, R, G, B);
    }

    bool value_changed = false, value_changed_h = false, value_changed_sv = false;

    PushItemFlag(ImGuiItemFlags_NoNav, true);
    if (flags & ImGuiColorEditFlags_PickerHueWheel)
    {
        // Hue wheel + SV triangle logic
        InvisibleButton("hsv", ImVec2(sv_picker_size + style.ItemInnerSpacing.x + bars_width, sv_picker_size));
        if (IsItemActive())
        {
            ImVec2 initial_off = g.IO.MouseClickedPos[0] - wheel_center;
            ImVec2 current_off = g.IO.MousePos - wheel_center;
            float initial_dist2 = ImLengthSqr(initial_off);
            if (initial_dist2 >= (wheel_r_inner - 1) * (wheel_r_inner - 1) && initial_dist2 <= (wheel_r_outer + 1) * (wheel_r_outer + 1))
            {
                // Interactive with Hue wheel
                H = ImAtan2(current_off.y, current_off.x) / IM_PI * 0.5f;
                if (H < 0.0f)
                    H += 1.0f;
                value_changed = value_changed_h = true;
            }
            float cos_hue_angle = ImCos(-H * 2.0f * IM_PI);
            float sin_hue_angle = ImSin(-H * 2.0f * IM_PI);
            if (ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, ImRotate(initial_off, cos_hue_angle, sin_hue_angle)))
            {
                // Interacting with SV triangle
                ImVec2 current_off_unrotated = ImRotate(current_off, cos_hue_angle, sin_hue_angle);
                if (!ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated))
                    current_off_unrotated = ImTriangleClosestPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated);
                float uu, vv, ww;
                ImTriangleBarycentricCoords(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated, uu, vv, ww);
                V = ImClamp(1.0f - vv, 0.0001f, 1.0f);
                S = ImClamp(uu / V, 0.0001f, 1.0f);
                value_changed = value_changed_sv = true;
            }
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context");
    } else if (flags & ImGuiColorEditFlags_PickerHueBar)
    {
        // SV rectangle logic
        InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
        if (IsItemActive())
        {
            S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
            V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = value_changed_sv = true;
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context");

        // Hue bar logic
        SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
        InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive())
        {
            H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = value_changed_h = true;
        }
    }

    // Alpha bar logic
    if (alpha_bar)
    {
        SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
        InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive())
        {
            col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = true;
        }
    }
    PopItemFlag(); // ImGuiItemFlags_NoNav

    if (!(flags & ImGuiColorEditFlags_NoSidePreview))
    {
        SameLine(0, style.ItemInnerSpacing.x);
        BeginGroup();
    }

    if (!(flags & ImGuiColorEditFlags_NoLabel))
    {
        const char* label_display_end = FindRenderedTextEnd(label);
        if (label != label_display_end)
        {
            if ((flags & ImGuiColorEditFlags_NoSidePreview))
                SameLine(0, style.ItemInnerSpacing.x);
            TextEx(label, label_display_end);
        }
    }

    if (!(flags & ImGuiColorEditFlags_NoSidePreview))
    {
        PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
        ImVec4 col_v4(col[0], col[1], col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : col[3]);
        if ((flags & ImGuiColorEditFlags_NoLabel))
            Text("Current");

        ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoTooltip;
        ColorButton("##current", col_v4, (flags & sub_flags_to_forward), ImVec2(square_sz * 3, square_sz * 2));
        if (ref_col != NULL)
        {
            Text("Original");
            ImVec4 ref_col_v4(ref_col[0], ref_col[1], ref_col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : ref_col[3]);
            if (ColorButton("##original", ref_col_v4, (flags & sub_flags_to_forward), ImVec2(square_sz * 3, square_sz * 2)))
            {
                memcpy(col, ref_col, components * sizeof(float));
                value_changed = true;
            }
        }
        PopItemFlag();
        EndGroup();
    }

    // Convert back color to RGB
    if (value_changed_h || value_changed_sv)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            ColorConvertHSVtoRGB(H >= 1.0f ? H - 10 * 1e-6f : H, S > 0.0f ? S : 10 * 1e-6f, V > 0.0f ? V : 1e-6f, col[0], col[1], col[2]);
            g.ColorEditLastHue = H;
            g.ColorEditLastSat = S;
            memcpy(g.ColorEditLastColor, col, sizeof(float) * 3);
        } else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            col[0] = H;
            col[1] = S;
            col[2] = V;
        }
    }

    // R,G,B and H,S,V slider color editor
    bool value_changed_fix_hue_wrap = false;
    if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
    {
        PushItemWidth((alpha_bar ? bar1_pos_x : bar0_pos_x) + bars_width - picker_pos.x);
        ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
        ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;
        if (flags & ImGuiColorEditFlags_DisplayRGB || (flags & ImGuiColorEditFlags__DisplayMask) == 0)
            if (ColorEdit4("##rgb", col, sub_flags | ImGuiColorEditFlags_DisplayRGB))
            {
                // FIXME: Hackily differentiating using the DragInt (ActiveId != 0 && !ActiveIdAllowOverlap) vs. using the InputText or DropTarget.
                // For the later we don't want to run the hue-wrap canceling code. If you are well versed in HSV picker please provide your input! (See #2050)
                value_changed_fix_hue_wrap = (g.ActiveId != 0 && !g.ActiveIdAllowOverlap);
                value_changed = true;
            }
        if (flags & ImGuiColorEditFlags_DisplayHSV || (flags & ImGuiColorEditFlags__DisplayMask) == 0)
            value_changed |= ColorEdit4("##hsv", col, sub_flags | ImGuiColorEditFlags_DisplayHSV);
        if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags__DisplayMask) == 0)
            value_changed |= ColorEdit4("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);
        PopItemWidth();
    }

    // Try to cancel hue wrap (after ColorEdit4 call), if any
    if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
    {
        float new_H, new_S, new_V;
        ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
        if (new_H <= 0 && H > 0)
        {
            if (new_V <= 0 && V != new_V)
                ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
            else if (new_S <= 0)
                ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
        }
    }

    if (value_changed)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            R = col[0];
            G = col[1];
            B = col[2];
            ColorConvertRGBtoHSV(R, G, B, H, S, V);
            if (memcmp(g.ColorEditLastColor, col, sizeof(float) * 3) == 0) // Fix local Hue as display below will use it immediately.
            {
                if (S == 0)
                    H = g.ColorEditLastHue;
                if (V == 0)
                    S = g.ColorEditLastSat;
            }
        } else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            H = col[0];
            S = col[1];
            V = col[2];
            ColorConvertHSVtoRGB(H, S, V, R, G, B);
        }
    }

    const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
    const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
    const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
    const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
    const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

    ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
    ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
    ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

    ImVec2 sv_cursor_pos;

    if (flags & ImGuiColorEditFlags_PickerHueWheel)
    {
        // Render Hue Wheel
        const float aeps = 0.5f / wheel_r_outer; // Half a pixel arc length in radians (2pi cancels out).
        const int segment_per_arc = ImMax(4, (int)wheel_r_outer / 12);
        for (int n = 0; n < 6; n++)
        {
            const float a0 = (n) / 6.0f * 2.0f * IM_PI - aeps;
            const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + aeps;
            const int vert_start_idx = draw_list->VtxBuffer.Size;
            draw_list->PathArcTo(wheel_center, (wheel_r_inner + wheel_r_outer) * 0.5f, a0, a1, segment_per_arc);
            draw_list->PathStroke(col_white, false, wheel_thickness);
            const int vert_end_idx = draw_list->VtxBuffer.Size;

            // Paint colors over existing vertices
            ImVec2 gradient_p0(wheel_center.x + ImCos(a0) * wheel_r_inner, wheel_center.y + ImSin(a0) * wheel_r_inner);
            ImVec2 gradient_p1(wheel_center.x + ImCos(a1) * wheel_r_inner, wheel_center.y + ImSin(a1) * wheel_r_inner);
            ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, col_hues[n], col_hues[n + 1]);
        }

        // Render Cursor + preview on Hue Wheel
        float cos_hue_angle = ImCos(H * 2.0f * IM_PI);
        float sin_hue_angle = ImSin(H * 2.0f * IM_PI);
        ImVec2 hue_cursor_pos(wheel_center.x + cos_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f, wheel_center.y + sin_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f);
        float hue_cursor_rad = value_changed_h ? wheel_thickness * 0.65f : wheel_thickness * 0.55f;
        int hue_cursor_segments = ImClamp((int)(hue_cursor_rad / 1.4f), 9, 32);
        draw_list->AddCircleFilled(hue_cursor_pos, hue_cursor_rad, hue_color32, hue_cursor_segments);
        draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad + 1, col_midgrey, hue_cursor_segments);
        draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad, col_white, hue_cursor_segments);

        // Render SV triangle (rotated according to hue)
        ImVec2 tra = wheel_center + ImRotate(triangle_pa, cos_hue_angle, sin_hue_angle);
        ImVec2 trb = wheel_center + ImRotate(triangle_pb, cos_hue_angle, sin_hue_angle);
        ImVec2 trc = wheel_center + ImRotate(triangle_pc, cos_hue_angle, sin_hue_angle);
        ImVec2 uv_white = GetFontTexUvWhitePixel();
        draw_list->PrimReserve(6, 6);
        draw_list->PrimVtx(tra, uv_white, hue_color32);
        draw_list->PrimVtx(trb, uv_white, hue_color32);
        draw_list->PrimVtx(trc, uv_white, col_white);
        draw_list->PrimVtx(tra, uv_white, 0);
        draw_list->PrimVtx(trb, uv_white, col_black);
        draw_list->PrimVtx(trc, uv_white, 0);
        draw_list->AddTriangle(tra, trb, trc, col_midgrey, 1.5f);
        sv_cursor_pos = ImLerp(ImLerp(trc, tra, ImSaturate(S)), trb, ImSaturate(1 - V));
    } else if (flags & ImGuiColorEditFlags_PickerHueBar)
    {
        // Render SV Square
        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), col_white, hue_color32, hue_color32, col_white);
        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0, 0, col_black, col_black);
        RenderFrameBorder(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0.0f);
        sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
        sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

        // Render Hue Bar
        for (int i = 0; i < 6; ++i)
            draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1]);
        float bar0_line_y = IM_ROUND(picker_pos.y + H * sv_picker_size);
        RenderFrameBorder(ImVec2(bar0_pos_x, picker_pos.y), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size), 0.0f);
        RenderArrowsForVerticalBar(draw_list, ImVec2(bar0_pos_x - 1, bar0_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
    }

    // Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
    float sv_cursor_rad = value_changed_sv ? 10.0f : 6.0f;
    draw_list->AddCircleFilled(sv_cursor_pos, sv_cursor_rad, user_col32_striped_of_alpha, 12);
    draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad + 1, col_midgrey, 12);
    draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad, col_white, 12);

    // Render alpha bar
    if (alpha_bar)
    {
        float alpha = ImSaturate(col[3]);
        ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);
        RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max, 0, bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f));
        draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);
        float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size);
        RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);
        RenderArrowsForVerticalBar(draw_list, ImVec2(bar1_pos_x - 1, bar1_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
    }

    EndGroup();

    if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0)
        value_changed = false;
    if (value_changed)
        MarkItemEdited(window->DC.LastItemId);

    PopID();

    return value_changed;
}

// A little colored square. Return true when clicked.
// FIXME: May want to display/ignore the alpha component in the color display? Yet show it in the tooltip.
// 'desc_id' is not called 'label' because we don't display it next to the button, but only in the tooltip.
// Note that 'col' may be encoded in HSV if ImGuiColorEditFlags_InputHSV is set.
bool ImGui::ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, ImVec2 size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(desc_id);
    float default_size = GetFrameHeight();
    if (size.x == 0.0f)
        size.x = default_size;
    if (size.y == 0.0f)
        size.y = default_size;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    if (flags & ImGuiColorEditFlags_NoAlpha)
        flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_rgb = col;
    if (flags & ImGuiColorEditFlags_InputHSV)
        ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

    ImVec4 col_rgb_without_alpha(col_rgb.x, col_rgb.y, col_rgb.z, 1.0f);
    float grid_step = ImMin(size.x, size.y) / 2.99f;
    float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);
    ImRect bb_inner = bb;
    float off = 0.0f;
    if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        off = -0.75f; // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
        bb_inner.Expand(off);
    }
    if ((flags & ImGuiColorEditFlags_AlphaPreviewHalf) && col_rgb.w < 1.0f)
    {
        float mid_x = IM_ROUND((bb_inner.Min.x + bb_inner.Max.x) * 0.5f);
        RenderColorRectWithAlphaCheckerboard(window->DrawList, ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y), bb_inner.Max, GetColorU32(col_rgb), grid_step, ImVec2(-grid_step + off, off), rounding, ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight);
        window->DrawList->AddRectFilled(bb_inner.Min, ImVec2(mid_x, bb_inner.Max.y), GetColorU32(col_rgb_without_alpha), rounding, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft);
    } else
    {
        // Because GetColorU32() multiplies by the global style Alpha and we don't want to display a checkerboard if the source code had no alpha
        ImVec4 col_source = (flags & ImGuiColorEditFlags_AlphaPreview) ? col_rgb : col_rgb_without_alpha;
        if (col_source.w < 1.0f)
            RenderColorRectWithAlphaCheckerboard(window->DrawList, bb_inner.Min, bb_inner.Max, GetColorU32(col_source), grid_step, ImVec2(off, off), rounding);
        else
            window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), rounding, ImDrawCornerFlags_All);
    }
    RenderNavHighlight(bb, id);
    if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        if (g.Style.FrameBorderSize > 0.0f)
            RenderFrameBorder(bb.Min, bb.Max, rounding);
        else
            window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), rounding); // Color button are often in need of some sort of border
    }

    // Drag and Drop Source
    // NB: The ActiveId test is merely an optional micro-optimization, BeginDragDropSource() does the same test.
    if (g.ActiveId == id && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropSource())
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
            SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col_rgb, sizeof(float) * 3, ImGuiCond_Once);
        else
            SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col_rgb, sizeof(float) * 4, ImGuiCond_Once);
        ColorButton(desc_id, col, flags);
        SameLine();
        TextEx("Color");
        EndDragDropSource();
    }

    // Tooltip
    if (!(flags & ImGuiColorEditFlags_NoTooltip) && hovered)
        ColorTooltip(desc_id, &col.x, flags & (ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf));

    return pressed;
}

// Initialize/override default color options
void ImGui::SetColorEditOptions(ImGuiColorEditFlags flags)
{
    ImGuiContext& g = *GImGui;
    if ((flags & ImGuiColorEditFlags__DisplayMask) == 0)
        flags |= ImGuiColorEditFlags__OptionsDefault & ImGuiColorEditFlags__DisplayMask;
    if ((flags & ImGuiColorEditFlags__DataTypeMask) == 0)
        flags |= ImGuiColorEditFlags__OptionsDefault & ImGuiColorEditFlags__DataTypeMask;
    if ((flags & ImGuiColorEditFlags__PickerMask) == 0)
        flags |= ImGuiColorEditFlags__OptionsDefault & ImGuiColorEditFlags__PickerMask;
    if ((flags & ImGuiColorEditFlags__InputMask) == 0)
        flags |= ImGuiColorEditFlags__OptionsDefault & ImGuiColorEditFlags__InputMask;
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__DisplayMask));    // Check only 1 option is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__DataTypeMask));   // Check only 1 option is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__PickerMask));     // Check only 1 option is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__InputMask));      // Check only 1 option is selected
    g.ColorEditOptions = flags;
}

// Note: only access 3 floats if ImGuiColorEditFlags_NoAlpha flag is set.
void ImGui::ColorTooltip(const char* text, const float* col, ImGuiColorEditFlags flags)
{
    ImGuiContext& g = *GImGui;

    BeginTooltipEx(0, ImGuiTooltipFlags_OverridePreviousTooltip);
    const char* text_end = text ? FindRenderedTextEnd(text, NULL) : text;
    if (text_end > text)
    {
        TextEx(text, text_end);
        Separator();
    }

    ImVec2 sz(g.FontSize * 3 + g.Style.FramePadding.y * 2, g.FontSize * 3 + g.Style.FramePadding.y * 2);
    ImVec4 cf(col[0], col[1], col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : col[3]);
    int cr = IM_F32_TO_INT8_SAT(col[0]), cg = IM_F32_TO_INT8_SAT(col[1]), cb = IM_F32_TO_INT8_SAT(col[2]), ca = (flags & ImGuiColorEditFlags_NoAlpha) ? 255 : IM_F32_TO_INT8_SAT(col[3]);
    ColorButton("##preview", cf, (flags & (ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf)) | ImGuiColorEditFlags_NoTooltip, sz);
    SameLine();
    if ((flags & ImGuiColorEditFlags_InputRGB) || !(flags & ImGuiColorEditFlags__InputMask))
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
            Text("#%02X%02X%02X\nR: %d, G: %d, B: %d\n(%.3f, %.3f, %.3f)", cr, cg, cb, cr, cg, cb, col[0], col[1], col[2]);
        else
            Text("#%02X%02X%02X%02X\nR:%d, G:%d, B:%d, A:%d\n(%.3f, %.3f, %.3f, %.3f)", cr, cg, cb, ca, cr, cg, cb, ca, col[0], col[1], col[2], col[3]);
    } else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
            Text("H: %.3f, S: %.3f, V: %.3f", col[0], col[1], col[2]);
        else
            Text("H: %.3f, S: %.3f, V: %.3f, A: %.3f", col[0], col[1], col[2], col[3]);
    }
    EndTooltip();
}

void ImGui::ColorEditOptionsPopup(const float* col, ImGuiColorEditFlags flags)
{
    bool allow_opt_inputs = !(flags & ImGuiColorEditFlags__DisplayMask);
    bool allow_opt_datatype = !(flags & ImGuiColorEditFlags__DataTypeMask);
    if ((!allow_opt_inputs && !allow_opt_datatype) || !BeginPopup("context"))
        return;
    ImGuiContext& g = *GImGui;
    ImGuiColorEditFlags opts = g.ColorEditOptions;
    if (allow_opt_inputs)
    {
        if (RadioButton("RGB", (opts & ImGuiColorEditFlags_DisplayRGB) != 0)) opts = (opts & ~ImGuiColorEditFlags__DisplayMask) | ImGuiColorEditFlags_DisplayRGB;
        if (RadioButton("HSV", (opts & ImGuiColorEditFlags_DisplayHSV) != 0)) opts = (opts & ~ImGuiColorEditFlags__DisplayMask) | ImGuiColorEditFlags_DisplayHSV;
        if (RadioButton("Hex", (opts & ImGuiColorEditFlags_DisplayHex) != 0)) opts = (opts & ~ImGuiColorEditFlags__DisplayMask) | ImGuiColorEditFlags_DisplayHex;
    }
    if (allow_opt_datatype)
    {
        if (allow_opt_inputs) Separator();
        if (RadioButton("0..255", (opts & ImGuiColorEditFlags_Uint8) != 0)) opts = (opts & ~ImGuiColorEditFlags__DataTypeMask) | ImGuiColorEditFlags_Uint8;
        if (RadioButton("0.00..1.00", (opts & ImGuiColorEditFlags_Float) != 0)) opts = (opts & ~ImGuiColorEditFlags__DataTypeMask) | ImGuiColorEditFlags_Float;
    }

    if (allow_opt_inputs || allow_opt_datatype)
        Separator();
    if (Button("Copy as..", ImVec2(-1, 0)))
        OpenPopup("Copy");
    if (BeginPopup("Copy"))
    {
        int cr = IM_F32_TO_INT8_SAT(col[0]), cg = IM_F32_TO_INT8_SAT(col[1]), cb = IM_F32_TO_INT8_SAT(col[2]), ca = (flags & ImGuiColorEditFlags_NoAlpha) ? 255 : IM_F32_TO_INT8_SAT(col[3]);
        char buf[64];
        ImFormatString(buf, IM_ARRAYSIZE(buf), "(%.3ff, %.3ff, %.3ff, %.3ff)", col[0], col[1], col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : col[3]);
        if (Selectable(buf))
            SetClipboardText(buf);
        ImFormatString(buf, IM_ARRAYSIZE(buf), "(%d,%d,%d,%d)", cr, cg, cb, ca);
        if (Selectable(buf))
            SetClipboardText(buf);
        ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", cr, cg, cb);
        if (Selectable(buf))
            SetClipboardText(buf);
        if (!(flags & ImGuiColorEditFlags_NoAlpha))
        {
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", cr, cg, cb, ca);
            if (Selectable(buf))
                SetClipboardText(buf);
        }
        EndPopup();
    }

    g.ColorEditOptions = opts;
    EndPopup();
}

void ImGui::ColorPickerOptionsPopup(const float* ref_col, ImGuiColorEditFlags flags)
{
    bool allow_opt_picker = !(flags & ImGuiColorEditFlags__PickerMask);
    bool allow_opt_alpha_bar = !(flags & ImGuiColorEditFlags_NoAlpha) && !(flags & ImGuiColorEditFlags_AlphaBar);
    if ((!allow_opt_picker && !allow_opt_alpha_bar) || !BeginPopup("context"))
        return;
    ImGuiContext& g = *GImGui;
    if (allow_opt_picker)
    {
        ImVec2 picker_size(g.FontSize * 8, ImMax(g.FontSize * 8 - (GetFrameHeight() + g.Style.ItemInnerSpacing.x), 1.0f)); // FIXME: Picker size copied from main picker function
        PushItemWidth(picker_size.x);
        for (int picker_type = 0; picker_type < 2; picker_type++)
        {
            // Draw small/thumbnail version of each picker type (over an invisible button for selection)
            if (picker_type > 0) Separator();
            PushID(picker_type);
            ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | (flags & ImGuiColorEditFlags_NoAlpha);
            if (picker_type == 0) picker_flags |= ImGuiColorEditFlags_PickerHueBar;
            if (picker_type == 1) picker_flags |= ImGuiColorEditFlags_PickerHueWheel;
            ImVec2 backup_pos = GetCursorScreenPos();
            if (Selectable("##selectable", false, 0, picker_size)) // By default, Selectable() is closing popup
                g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags__PickerMask) | (picker_flags & ImGuiColorEditFlags__PickerMask);
            SetCursorScreenPos(backup_pos);
            ImVec4 dummy_ref_col;
            memcpy(&dummy_ref_col, ref_col, sizeof(float) * ((picker_flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4));
            ColorPicker4("##dummypicker", &dummy_ref_col.x, picker_flags);
            PopID();
        }
        PopItemWidth();
    }
    if (allow_opt_alpha_bar)
    {
        if (allow_opt_picker) Separator();
        CheckboxFlags("Alpha Bar", (unsigned int*)&g.ColorEditOptions, ImGuiColorEditFlags_AlphaBar);
    }
    EndPopup();
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: TreeNode, CollapsingHeader, etc.
//-------------------------------------------------------------------------
// - TreeNode()
// - TreeNodeV()
// - TreeNodeEx()
// - TreeNodeExV()
// - TreeNodeBehavior() [Internal]
// - TreePush()
// - TreePop()
// - GetTreeNodeToLabelSpacing()
// - SetNextItemOpen()
// - CollapsingHeader()
//-------------------------------------------------------------------------

bool ImGui::TreeNode(const char* str_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(str_id, 0, fmt, args);
    va_end(args);
    return is_open;
}

bool ImGui::TreeNode(const void* ptr_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(ptr_id, 0, fmt, args);
    va_end(args);
    return is_open;
}

bool ImGui::TreeNode(const char* label)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    return TreeNodeBehavior(window->GetID(label), 0, label, NULL);
}

bool ImGui::TreeNodeV(const char* str_id, const char* fmt, va_list args)
{
    return TreeNodeExV(str_id, 0, fmt, args);
}

bool ImGui::TreeNodeV(const void* ptr_id, const char* fmt, va_list args)
{
    return TreeNodeExV(ptr_id, 0, fmt, args);
}

bool ImGui::TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    return TreeNodeBehavior(window->GetID(label), flags, label, NULL);
}

bool ImGui::TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(str_id, flags, fmt, args);
    va_end(args);
    return is_open;
}

bool ImGui::TreeNodeEx(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(ptr_id, flags, fmt, args);
    va_end(args);
    return is_open;
}

bool ImGui::TreeNodeExV(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const char* label_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    return TreeNodeBehavior(window->GetID(str_id), flags, g.TempBuffer, label_end);
}

bool ImGui::TreeNodeExV(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const char* label_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    return TreeNodeBehavior(window->GetID(ptr_id), flags, g.TempBuffer, label_end);
}

bool ImGui::TreeNodeBehaviorIsOpen(ImGuiID id, ImGuiTreeNodeFlags flags)
{
    if (flags & ImGuiTreeNodeFlags_Leaf)
        return true;

    // We only write to the tree storage if the user clicks (or explicitly use the SetNextItemOpen function)
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiStorage* storage = window->DC.StateStorage;

    bool is_open;
    if (g.NextItemData.Flags & ImGuiNextItemDataFlags_HasOpen)
    {
        if (g.NextItemData.OpenCond & ImGuiCond_Always)
        {
            is_open = g.NextItemData.OpenVal;
            storage->SetInt(id, is_open);
        } else
        {
            // We treat ImGuiCond_Once and ImGuiCond_FirstUseEver the same because tree node state are not saved persistently.
            const int stored_value = storage->GetInt(id, -1);
            if (stored_value == -1)
            {
                is_open = g.NextItemData.OpenVal;
                storage->SetInt(id, is_open);
            } else
            {
                is_open = stored_value != 0;
            }
        }
    } else
    {
        is_open = storage->GetInt(id, (flags & ImGuiTreeNodeFlags_DefaultOpen) ? 1 : 0) != 0;
    }

    // When logging is enabled, we automatically expand tree nodes (but *NOT* collapsing headers.. seems like sensible behavior).
    // NB- If we are above max depth we still allow manually opened nodes to be logged.
    if (g.LogEnabled && !(flags & ImGuiTreeNodeFlags_NoAutoOpenOnLog) && (window->DC.TreeDepth - g.LogDepthRef) < g.LogDepthToExpand)
        is_open = true;

    return is_open;
}

bool ImGui::TreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags, const char* label, const char* label_end)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
    const ImVec2 padding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));

    if (!label_end)
        label_end = FindRenderedTextEnd(label);
    const ImVec2 label_size = CalcTextSize(label, label_end, false);

    // We vertically grow up to current line height up the typical widget height.
    const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
    ImRect frame_bb;
    frame_bb.Min.x = (flags & ImGuiTreeNodeFlags_SpanFullWidth) ? window->WorkRect.Min.x : window->DC.CursorPos.x;
    frame_bb.Min.y = window->DC.CursorPos.y;
    frame_bb.Max.x = window->WorkRect.Max.x;
    frame_bb.Max.y = window->DC.CursorPos.y + frame_height;
    if (display_frame)
    {
        // Framed header expand a little outside the default padding, to the edge of InnerClipRect
        // (FIXME: May remove this at some point and make InnerClipRect align with WindowPadding.x instead of WindowPadding.x*0.5f)
        frame_bb.Min.x -= IM_FLOOR(window->WindowPadding.x * 0.5f - 1.0f);
        frame_bb.Max.x += IM_FLOOR(window->WindowPadding.x * 0.5f);
    }

    const float text_offset_x = g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2);               // Collapser arrow width + Spacing
    const float text_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset);                    // Latch before ItemSize changes it
    const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);    // Include collapser
    ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
    ItemSize(ImVec2(text_width, frame_height), padding.y);

    // For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
    ImRect interact_bb = frame_bb;
    if (!display_frame && (flags & (ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)) == 0)
        interact_bb.Max.x = frame_bb.Min.x + text_width + style.ItemSpacing.x * 2.0f;

    // Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child.
    // For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
    // This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
    const bool is_leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
    bool is_open = TreeNodeBehaviorIsOpen(id, flags);
    if (is_open && !g.NavIdIsAlive && (flags & ImGuiTreeNodeFlags_NavLeftJumpsBackHere) && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
        window->DC.TreeJumpToParentOnPopMask |= (1 << window->DC.TreeDepth);

    bool item_add = ItemAdd(interact_bb, id);
    window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
    window->DC.LastItemDisplayRect = frame_bb;

    if (!item_add)
    {
        if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
            TreePushOverrideID(id);
        IMGUI_TEST_ENGINE_ITEM_INFO(window->DC.LastItemId, label, window->DC.ItemFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
        return is_open;
    }

    ImGuiButtonFlags button_flags = ImGuiTreeNodeFlags_None;
    if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
        button_flags |= ImGuiButtonFlags_AllowItemOverlap;
    if (!is_leaf)
        button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;

    // We allow clicking on the arrow section with keyboard modifiers held, in order to easily
    // allow browsing a tree while preserving selection with code implementing multi-selection patterns.
    // When clicking on the rest of the tree node we always disallow keyboard modifiers.
    const float arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
    const float arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x * 2.0f) + style.TouchExtraPadding.x;
    const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);
    if (window != g.HoveredWindow || !is_mouse_x_over_arrow)
        button_flags |= ImGuiButtonFlags_NoKeyModifiers;

    // Open behaviors can be altered with the _OpenOnArrow and _OnOnDoubleClick flags.
    // Some alteration have subtle effects (e.g. toggle on MouseUp vs MouseDown events) due to requirements for multi-selection and drag and drop support.
    // - Single-click on label = Toggle on MouseUp (default)
    // - Single-click on arrow = Toggle on MouseUp (when _OpenOnArrow=0)
    // - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=1)
    // - Double-click on label = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1)
    // - Double-click on arrow = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1 and _OpenOnArrow=0)
    // This makes _OpenOnArrow have a subtle effect on _OpenOnDoubleClick: arrow click reacts on Down rather than Up.
    // It is rather standard that arrow click react on Down rather than Up and we'd be tempted to make it the default
    // (by removing the _OpenOnArrow test below), however this would have a perhaps surprising effect on CollapsingHeader()?
    // So right now we are making this optional. May evolve later.
    // We set ImGuiButtonFlags_PressedOnClickRelease on OpenOnDoubleClick because we want the item to be active on the initial MouseDown in order for drag and drop to work.
    if (is_mouse_x_over_arrow && (flags & ImGuiTreeNodeFlags_OpenOnArrow))
        button_flags |= ImGuiButtonFlags_PressedOnClick;
    else if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
        button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
    else
        button_flags |= ImGuiButtonFlags_PressedOnClickRelease;

    bool selected = (flags & ImGuiTreeNodeFlags_Selected) != 0;
    const bool was_selected = selected;

    bool hovered, held;
    bool pressed = ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
    bool toggled = false;
    if (!is_leaf)
    {
        if (pressed && g.DragDropHoldJustPressedId != id)
        {
            if ((flags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) == 0 || (g.NavActivateId == id))
                toggled = true;
            if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
                toggled |= is_mouse_x_over_arrow && !g.NavDisableMouseHover; // Lightweight equivalent of IsMouseHoveringRect() since ButtonBehavior() already did the job
            if ((flags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && g.IO.MouseDoubleClicked[0])
                toggled = true;
        } else if (pressed && g.DragDropHoldJustPressedId == id)
        {
            IM_ASSERT(button_flags & ImGuiButtonFlags_PressedOnDragDropHold);
            if (!is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
                toggled = true;
        }

        if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Left && is_open)
        {
            toggled = true;
            NavMoveRequestCancel();
        }
        if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
        {
            toggled = true;
            NavMoveRequestCancel();
        }

        if (toggled)
        {
            is_open = !is_open;
            window->DC.StateStorage->SetInt(id, is_open);
            window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledOpen;
        }
    }
    if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
        SetItemAllowOverlap();

    // In this branch, TreeNodeBehavior() cannot toggle the selection so this will never trigger.
    if (selected != was_selected) //-V547
        window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

    // Render
    const ImU32 text_col = GetColorU32(ImGuiCol_Text);
    ImGuiNavHighlightFlags nav_highlight_flags = ImGuiNavHighlightFlags_TypeThin;
    if (display_frame)
    {
        // Framed type
        const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
        RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, true, style.FrameRounding);
        RenderNavHighlight(frame_bb, id, nav_highlight_flags);
        if (flags & ImGuiTreeNodeFlags_Bullet)
            RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.60f, text_pos.y + g.FontSize * 0.5f), text_col);
        else if (!is_leaf)
            RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y), text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);
        else // Leaf without bullet, left-adjusted text
            text_pos.x -= text_offset_x;
        if (flags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton)
            frame_bb.Max.x -= g.FontSize + style.FramePadding.x;
        if (g.LogEnabled)
        {
            // NB: '##' is normally used to hide text (as a library-wide feature), so we need to specify the text range to make sure the ## aren't stripped out here.
            const char log_prefix[] = "\n##";
            const char log_suffix[] = "##";
            LogRenderedText(&text_pos, log_prefix, log_prefix + 3);
            RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
            LogRenderedText(&text_pos, log_suffix, log_suffix + 2);
        } else
        {
            RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
        }
    } else
    {
        // Unframed typed for tree nodes
        if (hovered || selected)
        {
            const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
            RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, false);
            RenderNavHighlight(frame_bb, id, nav_highlight_flags);
        }
        if (flags & ImGuiTreeNodeFlags_Bullet)
            RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.5f, text_pos.y + g.FontSize * 0.5f), text_col);
        else if (!is_leaf)
            RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y + g.FontSize * 0.15f), text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 0.70f);
        if (g.LogEnabled)
            LogRenderedText(&text_pos, ">");
        RenderText(text_pos, label, label_end, false);
    }

    if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
        TreePushOverrideID(id);
    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
    return is_open;
}

void ImGui::TreePush(const char* str_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    PushID(str_id ? str_id : "#TreePush");
}

void ImGui::TreePush(const void* ptr_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    PushID(ptr_id ? ptr_id : (const void*)"#TreePush");
}

void ImGui::TreePushOverrideID(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    Indent();
    window->DC.TreeDepth++;
    window->IDStack.push_back(id);
}

void ImGui::TreePop()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    Unindent();

    window->DC.TreeDepth--;
    ImU32 tree_depth_mask = (1 << window->DC.TreeDepth);

    // Handle Left arrow to move to parent tree node (when ImGuiTreeNodeFlags_NavLeftJumpsBackHere is enabled)
    if (g.NavMoveDir == ImGuiDir_Left && g.NavWindow == window && NavMoveRequestButNoResultYet())
        if (g.NavIdIsAlive && (window->DC.TreeJumpToParentOnPopMask & tree_depth_mask))
        {
            SetNavID(window->IDStack.back(), g.NavLayer, 0);
            NavMoveRequestCancel();
        }
    window->DC.TreeJumpToParentOnPopMask &= tree_depth_mask - 1;

    IM_ASSERT(window->IDStack.Size > 1); // There should always be 1 element in the IDStack (pushed during window creation). If this triggers you called TreePop/PopID too much.
    PopID();
}

// Horizontal distance preceding label when using TreeNode() or Bullet()
float ImGui::GetTreeNodeToLabelSpacing()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize + (g.Style.FramePadding.x * 2.0f);
}

// Set next TreeNode/CollapsingHeader open state.
void ImGui::SetNextItemOpen(bool is_open, ImGuiCond cond)
{
    ImGuiContext& g = *GImGui;
    if (g.CurrentWindow->SkipItems)
        return;
    g.NextItemData.Flags |= ImGuiNextItemDataFlags_HasOpen;
    g.NextItemData.OpenVal = is_open;
    g.NextItemData.OpenCond = cond ? cond : ImGuiCond_Always;
}

// CollapsingHeader returns true when opened but do not indent nor push into the ID stack (because of the ImGuiTreeNodeFlags_NoTreePushOnOpen flag).
// This is basically the same as calling TreeNodeEx(label, ImGuiTreeNodeFlags_CollapsingHeader). You can remove the _NoTreePushOnOpen flag if you want behavior closer to normal TreeNode().
bool ImGui::CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    return TreeNodeBehavior(window->GetID(label), flags | ImGuiTreeNodeFlags_CollapsingHeader, label);
}

bool ImGui::CollapsingHeader(const char* label, bool* p_open, ImGuiTreeNodeFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    if (p_open && !*p_open)
        return false;

    ImGuiID id = window->GetID(label);
    flags |= ImGuiTreeNodeFlags_CollapsingHeader;
    if (p_open)
        flags |= ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
    bool is_open = TreeNodeBehavior(id, flags, label);
    if (p_open)
    {
        // Create a small overlapping close button
        // FIXME: We can evolve this into user accessible helpers to add extra buttons on title bars, headers, etc.
        // FIXME: CloseButton can overlap into text, need find a way to clip the text somehow.
        ImGuiContext& g = *GImGui;
        ImGuiItemHoveredDataBackup last_item_backup;
        float button_size = g.FontSize;
        float button_x = ImMax(window->DC.LastItemRect.Min.x, window->DC.LastItemRect.Max.x - g.Style.FramePadding.x * 2.0f - button_size);
        float button_y = window->DC.LastItemRect.Min.y;
        if (CloseButton(window->GetID((void*)((intptr_t)id + 1)), ImVec2(button_x, button_y)))
            *p_open = false;
        last_item_backup.Restore();
    }

    return is_open;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Selectable
//-------------------------------------------------------------------------
// - Selectable()
//-------------------------------------------------------------------------

// Tip: pass a non-visible label (e.g. "##dummy") then you can use the space to draw other text or image.
// But you need to make sure the ID is unique, e.g. enclose calls in PushID/PopID or use ##unique_id.
// With this scheme, ImGuiSelectableFlags_SpanAllColumns and ImGuiSelectableFlags_AllowItemOverlap are also frequently used flags.
// FIXME: Selectable() with (size.x == 0.0f) and (SelectableTextAlign.x > 0.0f) followed by SameLine() is currently not supported.
bool ImGui::Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns) // FIXME-OPT: Avoid if vertically clipped.
        PushColumnsBackground();

    // Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
    ImGuiID id = window->GetID(label);
    ImVec2 label_size = CalcTextSize(label, NULL, true);
    ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrLineTextBaseOffset;
    ItemSize(size, 0.0f);

    // Fill horizontal space
    const float min_x = (flags & ImGuiSelectableFlags_SpanAllColumns) ? window->ContentRegionRect.Min.x : pos.x;
    const float max_x = (flags & ImGuiSelectableFlags_SpanAllColumns) ? window->ContentRegionRect.Max.x : GetContentRegionMaxAbs().x;
    if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
        size.x = ImMax(label_size.x, max_x - min_x);

    // Text stays at the submission position, but bounding box may be extended on both sides
    const ImVec2 text_min = pos;
    const ImVec2 text_max(min_x + size.x, pos.y + size.y);

    // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
    ImRect bb_enlarged(min_x, pos.y, text_max.x, text_max.y);
    const float spacing_x = style.ItemSpacing.x;
    const float spacing_y = style.ItemSpacing.y;
    const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
    const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
    bb_enlarged.Min.x -= spacing_L;
    bb_enlarged.Min.y -= spacing_U;
    bb_enlarged.Max.x += (spacing_x - spacing_L);
    bb_enlarged.Max.y += (spacing_y - spacing_U);
    //if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb_align.Min, bb_align.Max, IM_COL32(255, 0, 0, 255)); }
    //if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb_enlarged.Min, bb_enlarged.Max, IM_COL32(0, 255, 0, 255)); }

    bool item_add;
    if (flags & ImGuiSelectableFlags_Disabled)
    {
        ImGuiItemFlags backup_item_flags = window->DC.ItemFlags;
        window->DC.ItemFlags |= ImGuiItemFlags_Disabled | ImGuiItemFlags_NoNavDefaultFocus;
        item_add = ItemAdd(bb_enlarged, id);
        window->DC.ItemFlags = backup_item_flags;
    } else
    {
        item_add = ItemAdd(bb_enlarged, id);
    }
    if (!item_add)
    {
        if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns)
            PopColumnsBackground();
        return false;
    }

    // We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
    ImGuiButtonFlags button_flags = 0;
    if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
    if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
    if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
    if (flags & ImGuiSelectableFlags_Disabled) { button_flags |= ImGuiButtonFlags_Disabled; }
    if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
    if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

    if (flags & ImGuiSelectableFlags_Disabled)
        selected = false;

    const bool was_selected = selected;
    bool hovered, held;
    bool pressed = ButtonBehavior(bb_enlarged, id, &hovered, &held, button_flags);

    // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
    if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
    {
        if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
        {
            g.NavDisableHighlight = true;
            SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent);
        }
    }
    if (pressed)
        MarkItemEdited(id);

    if (flags & ImGuiSelectableFlags_AllowItemOverlap)
        SetItemAllowOverlap();

    // In this branch, Selectable() cannot toggle the selection so this will never trigger.
    if (selected != was_selected) //-V547
        window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

    // Render
    if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
        hovered = true;
    if (hovered || selected)
    {
        const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
        RenderFrame(bb_enlarged.Min, bb_enlarged.Max, col, false, 0.0f);
        RenderNavHighlight(bb_enlarged, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
    }

    if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns)
        PopColumnsBackground();

    if (flags & ImGuiSelectableFlags_Disabled) PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
    RenderTextClipped(text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb_enlarged);
    if (flags & ImGuiSelectableFlags_Disabled) PopStyleColor();

    // Automatically close popups
    if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
        CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return pressed;
}

bool ImGui::Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    if (Selectable(label, *p_selected, flags, size_arg))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: ListBox
//-------------------------------------------------------------------------
// - ListBox()
// - ListBoxHeader()
// - ListBoxFooter()
//-------------------------------------------------------------------------
// FIXME: This is an old API. We should redesign some of it, rename ListBoxHeader->BeginListBox, ListBoxFooter->EndListBox
// and promote using them over existing ListBox() functions, similarly to change with combo boxes.
//-------------------------------------------------------------------------

// FIXME: In principle this function should be called BeginListBox(). We should rename it after re-evaluating if we want to keep the same signature.
// Helper to calculate the size of a listbox and display a label on the right.
// Tip: To have a list filling the entire window width, PushItemWidth(-1) and pass an non-visible label e.g. "##empty"
bool ImGui::ListBoxHeader(const char* label, const ImVec2& size_arg)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    // Size default to hold ~7 items. Fractional number of items helps seeing that we can scroll down/up without looking at scrollbar.
    ImVec2 size = CalcItemSize(size_arg, CalcItemWidth(), GetTextLineHeightWithSpacing() * 7.4f + style.ItemSpacing.y);
    ImVec2 frame_size = ImVec2(size.x, ImMax(size.y, label_size.y));
    ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    ImRect bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    window->DC.LastItemRect = bb; // Forward storage for ListBoxFooter.. dodgy.
    g.NextItemData.ClearFlags();

    if (!IsRectVisible(bb.Min, bb.Max))
    {
        ItemSize(bb.GetSize(), style.FramePadding.y);
        ItemAdd(bb, 0, &frame_bb);
        return false;
    }

    BeginGroup();
    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    BeginChildFrame(id, frame_bb.GetSize());
    return true;
}

// FIXME: In principle this function should be called EndListBox(). We should rename it after re-evaluating if we want to keep the same signature.
bool ImGui::ListBoxHeader(const char* label, int items_count, int height_in_items)
{
    // Size default to hold ~7.25 items.
    // We add +25% worth of item height to allow the user to see at a glance if there are more items up/down, without looking at the scrollbar.
    // We don't add this extra bit if items_count <= height_in_items. It is slightly dodgy, because it means a dynamic list of items will make the widget resize occasionally when it crosses that size.
    // I am expecting that someone will come and complain about this behavior in a remote future, then we can advise on a better solution.
    if (height_in_items < 0)
        height_in_items = ImMin(items_count, 7);
    const ImGuiStyle& style = GetStyle();
    float height_in_items_f = (height_in_items < items_count) ? (height_in_items + 0.25f) : (height_in_items + 0.00f);

    // We include ItemSpacing.y so that a list sized for the exact number of items doesn't make a scrollbar appears. We could also enforce that by passing a flag to BeginChild().
    ImVec2 size;
    size.x = 0.0f;
    size.y = ImFloor(GetTextLineHeightWithSpacing() * height_in_items_f + style.FramePadding.y * 2.0f);
    return ListBoxHeader(label, size);
}

// FIXME: In principle this function should be called EndListBox(). We should rename it after re-evaluating if we want to keep the same signature.
void ImGui::ListBoxFooter()
{
    ImGuiWindow* parent_window = GetCurrentWindow()->ParentWindow;
    const ImRect bb = parent_window->DC.LastItemRect;
    const ImGuiStyle& style = GetStyle();

    EndChildFrame();

    // Redeclare item size so that it includes the label (we have stored the full size in LastItemRect)
    // We call SameLine() to restore DC.CurrentLine* data
    SameLine();
    parent_window->DC.CursorPos = bb.Min;
    ItemSize(bb, style.FramePadding.y);
    EndGroup();
}

bool ImGui::ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_items)
{
    const bool value_changed = ListBox(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_items);
    return value_changed;
}

bool ImGui::ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
{
    if (!ListBoxHeader(label, items_count, height_in_items))
        return false;

    // Assume all items have even height (= 1 line of text). If you need items of different or variable sizes you can create a custom version of ListBox() in your code without using the clipper.
    ImGuiContext& g = *GImGui;
    bool value_changed = false;
    ImGuiListClipper clipper(items_count, GetTextLineHeightWithSpacing()); // We know exactly our line height here so we pass it as a minor optimization, but generally you don't need to.
    while (clipper.Step())
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const bool item_selected = (i == *current_item);
            const char* item_text;
            if (!items_getter(data, i, &item_text))
                item_text = "*Unknown item*";

            PushID(i);
            if (Selectable(item_text, item_selected))
            {
                *current_item = i;
                value_changed = true;
            }
            if (item_selected)
                SetItemDefaultFocus();
            PopID();
        }
    ListBoxFooter();
    if (value_changed)
        MarkItemEdited(g.CurrentWindow->DC.LastItemId);

    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: PlotLines, PlotHistogram
//-------------------------------------------------------------------------
// - PlotEx() [Internal]
// - PlotLines()
// - PlotHistogram()
//-------------------------------------------------------------------------

int ImGui::PlotEx(ImGuiPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return -1;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    if (frame_size.x == 0.0f)
        frame_size.x = CalcItemWidth();
    if (frame_size.y == 0.0f)
        frame_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0, &frame_bb))
        return -1;
    const bool hovered = ItemHoverable(frame_bb, id);

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < values_count; i++)
        {
            const float v = values_getter(data, i);
            if (v != v) // Ignore NaN values
                continue;
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    const int values_count_min = (plot_type == ImGuiPlotType_Lines) ? 2 : 1;
    int idx_hovered = -1;
    if (values_count >= values_count_min)
    {
        int res_w = ImMin((int)frame_size.x, values_count) + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);
        int item_count = values_count + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);

        // Tooltip on hover
        if (hovered && inner_bb.Contains(g.IO.MousePos))
        {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < values_count);

            const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
            const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
            if (plot_type == ImGuiPlotType_Lines)
                SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx + 1, v1);
            else if (plot_type == ImGuiPlotType_Histogram)
                SetTooltip("%d: %8.4g", v_idx, v0);
            idx_hovered = v_idx;
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = values_getter(data, (0 + values_offset) % values_count);
        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale));                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

        const ImU32 col_base = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);

        for (int n = 0; n < res_w; n++)
        {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
            const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale));

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
            if (plot_type == ImGuiPlotType_Lines)
            {
                window->DrawList->AddLine(pos0, pos1, idx_hovered == v1_idx ? col_hovered : col_base);
            } else if (plot_type == ImGuiPlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                window->DrawList->AddRectFilled(pos0, pos1, idx_hovered == v1_idx ? col_hovered : col_base);
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if (overlay_text)
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f, 0.0f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);

    // Return hovered index or -1 if none are hovered.
    // This is currently not exposed in the public API because we need a larger redesign of the whole thing, but in the short-term we are making it available in PlotEx().
    return idx_hovered;
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
    const float v = *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
    return v;
}

void ImGui::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterData data(values, stride);
    PlotEx(ImGuiPlotType_Lines, label, &Plot_ArrayGetter, (void*)&data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotLines(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotEx(ImGuiPlotType_Lines, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterData data(values, stride);
    PlotEx(ImGuiPlotType_Histogram, label, &Plot_ArrayGetter, (void*)&data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotHistogram(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotEx(ImGuiPlotType_Histogram, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Value helpers
// Those is not very useful, legacy API.
//-------------------------------------------------------------------------
// - Value()
//-------------------------------------------------------------------------

void ImGui::Value(const char* prefix, bool b)
{
    Text("%s: %s", prefix, (b ? "true" : "false"));
}

void ImGui::Value(const char* prefix, int v)
{
    Text("%s: %d", prefix, v);
}

void ImGui::Value(const char* prefix, unsigned int v)
{
    Text("%s: %d", prefix, v);
}

void ImGui::Value(const char* prefix, float v, const char* float_format)
{
    if (float_format)
    {
        char fmt[64];
        ImFormatString(fmt, IM_ARRAYSIZE(fmt), "%%s: %s", float_format);
        Text(fmt, prefix, v);
    } else
    {
        Text("%s: %.3f", prefix, v);
    }
}

//-------------------------------------------------------------------------
// [SECTION] MenuItem, BeginMenu, EndMenu, etc.
//-------------------------------------------------------------------------
// - ImGuiMenuColumns [Internal]
// - BeginMenuBar()
// - EndMenuBar()
// - BeginMainMenuBar()
// - EndMainMenuBar()
// - BeginMenu()
// - EndMenu()
// - MenuItem()
//-------------------------------------------------------------------------

// Helpers for internal use
ImGuiMenuColumns::ImGuiMenuColumns()
{
    Spacing = Width = NextWidth = 0.0f;
    memset(Pos, 0, sizeof(Pos));
    memset(NextWidths, 0, sizeof(NextWidths));
}

void ImGuiMenuColumns::Update(int count, float spacing, bool clear)
{
    IM_ASSERT(count == IM_ARRAYSIZE(Pos));
    IM_UNUSED(count);
    Width = NextWidth = 0.0f;
    Spacing = spacing;
    if (clear)
        memset(NextWidths, 0, sizeof(NextWidths));
    for (int i = 0; i < IM_ARRAYSIZE(Pos); i++)
    {
        if (i > 0 && NextWidths[i] > 0.0f)
            Width += Spacing;
        Pos[i] = IM_FLOOR(Width);
        Width += NextWidths[i];
        NextWidths[i] = 0.0f;
    }
}

float ImGuiMenuColumns::DeclColumns(float w0, float w1, float w2) // not using va_arg because they promote float to double
{
    NextWidth = 0.0f;
    NextWidths[0] = ImMax(NextWidths[0], w0);
    NextWidths[1] = ImMax(NextWidths[1], w1);
    NextWidths[2] = ImMax(NextWidths[2], w2);
    for (int i = 0; i < IM_ARRAYSIZE(Pos); i++)
        NextWidth += NextWidths[i] + ((i > 0 && NextWidths[i] > 0.0f) ? Spacing : 0.0f);
    return ImMax(Width, NextWidth);
}

float ImGuiMenuColumns::CalcExtraSpace(float avail_w) const
{
    return ImMax(0.0f, avail_w - Width);
}

// FIXME: Provided a rectangle perhaps e.g. a BeginMenuBarEx() could be used anywhere..
// Currently the main responsibility of this function being to setup clip-rect + horizontal layout + menu navigation layer.
// Ideally we also want this to be responsible for claiming space out of the main window scrolling rectangle, in which case ImGuiWindowFlags_MenuBar will become unnecessary.
// Then later the same system could be used for multiple menu-bars, scrollbars, side-bars.
bool ImGui::BeginMenuBar()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    if (!(window->Flags & ImGuiWindowFlags_MenuBar))
        return false;

    IM_ASSERT(!window->DC.MenuBarAppending);
    BeginGroup(); // Backup position on layer 0 // FIXME: Misleading to use a group for that backup/restore
    PushID("##menubar");

    // We don't clip with current window clipping rectangle as it is already set to the area below. However we clip with window full rect.
    // We remove 1 worth of rounding to Max.x to that text in long menus and small windows don't tend to display over the lower-right rounded area, which looks particularly glitchy.
    ImRect bar_rect = window->MenuBarRect();
    ImRect clip_rect(IM_ROUND(bar_rect.Min.x + window->WindowBorderSize), IM_ROUND(bar_rect.Min.y + window->WindowBorderSize), IM_ROUND(ImMax(bar_rect.Min.x, bar_rect.Max.x - ImMax(window->WindowRounding, window->WindowBorderSize))), IM_ROUND(bar_rect.Max.y));
    clip_rect.ClipWith(window->OuterRectClipped);
    PushClipRect(clip_rect.Min, clip_rect.Max, false);

    window->DC.CursorPos = ImVec2(bar_rect.Min.x + window->DC.MenuBarOffset.x, bar_rect.Min.y + window->DC.MenuBarOffset.y);
    window->DC.LayoutType = ImGuiLayoutType_Horizontal;
    window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
    window->DC.NavLayerCurrentMask = (1 << ImGuiNavLayer_Menu);
    window->DC.MenuBarAppending = true;
    AlignTextToFramePadding();
    return true;
}

void ImGui::EndMenuBar()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    ImGuiContext& g = *GImGui;

    // Nav: When a move request within one of our child menu failed, capture the request to navigate among our siblings.
    if (NavMoveRequestButNoResultYet() && (g.NavMoveDir == ImGuiDir_Left || g.NavMoveDir == ImGuiDir_Right) && (g.NavWindow->Flags & ImGuiWindowFlags_ChildMenu))
    {
        ImGuiWindow* nav_earliest_child = g.NavWindow;
        while (nav_earliest_child->ParentWindow && (nav_earliest_child->ParentWindow->Flags & ImGuiWindowFlags_ChildMenu))
            nav_earliest_child = nav_earliest_child->ParentWindow;
        if (nav_earliest_child->ParentWindow == window && nav_earliest_child->DC.ParentLayoutType == ImGuiLayoutType_Horizontal && g.NavMoveRequestForward == ImGuiNavForward_None)
        {
            // To do so we claim focus back, restore NavId and then process the movement request for yet another frame.
            // This involve a one-frame delay which isn't very problematic in this situation. We could remove it by scoring in advance for multiple window (probably not worth the hassle/cost)
            const ImGuiNavLayer layer = ImGuiNavLayer_Menu;
            IM_ASSERT(window->DC.NavLayerActiveMaskNext & (1 << layer)); // Sanity check
            FocusWindow(window);
            SetNavIDWithRectRel(window->NavLastIds[layer], layer, 0, window->NavRectRel[layer]);
            g.NavLayer = layer;
            g.NavDisableHighlight = true; // Hide highlight for the current frame so we don't see the intermediary selection.
            g.NavMoveRequestForward = ImGuiNavForward_ForwardQueued;
            NavMoveRequestCancel();
        }
    }

    IM_ASSERT(window->Flags & ImGuiWindowFlags_MenuBar);
    IM_ASSERT(window->DC.MenuBarAppending);
    PopClipRect();
    PopID();
    window->DC.MenuBarOffset.x = window->DC.CursorPos.x - window->MenuBarRect().Min.x; // Save horizontal position so next append can reuse it. This is kinda equivalent to a per-layer CursorPos.
    window->DC.GroupStack.back().EmitItem = false;
    EndGroup(); // Restore position on layer 0
    window->DC.LayoutType = ImGuiLayoutType_Vertical;
    window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
    window->DC.NavLayerCurrentMask = (1 << ImGuiNavLayer_Main);
    window->DC.MenuBarAppending = false;
}

// For the main menu bar, which cannot be moved, we honor g.Style.DisplaySafeAreaPadding to ensure text can be visible on a TV set.
bool ImGui::BeginMainMenuBar()
{
    ImGuiContext& g = *GImGui;
    g.NextWindowData.MenuBarOffsetMinVal = ImVec2(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
    SetNextWindowPos(ImVec2(0.0f, 0.0f));
    SetNextWindowSize(ImVec2(g.IO.DisplaySize.x, g.NextWindowData.MenuBarOffsetMinVal.y + g.FontBaseSize + g.Style.FramePadding.y));
    PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
    bool is_open = Begin("##MainMenuBar", NULL, window_flags) && BeginMenuBar();
    PopStyleVar(2);
    g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);
    if (!is_open)
    {
        End();
        return false;
    }
    return true; //-V1020
}

void ImGui::EndMainMenuBar()
{
    EndMenuBar();

    // When the user has left the menu layer (typically: closed menus through activation of an item), we restore focus to the previous window
    // FIXME: With this strategy we won't be able to restore a NULL focus.
    ImGuiContext& g = *GImGui;
    if (g.CurrentWindow == g.NavWindow && g.NavLayer == ImGuiNavLayer_Main && !g.NavAnyRequest)
        FocusTopMostWindowUnderOne(g.NavWindow, NULL);

    End();
}

bool ImGui::BeginMenu(const char* label, bool enabled)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    bool menu_is_open = IsPopupOpen(id);

    // Sub-menus are ChildWindow so that mouse can be hovering across them (otherwise top-most popup menu would steal focus and not allow hovering on parent menu)
    ImGuiWindowFlags flags = ImGuiWindowFlags_ChildMenu | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;
    if (window->Flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_ChildMenu))
        flags |= ImGuiWindowFlags_ChildWindow;

    // If a menu with same the ID was already submitted, we will append to it, matching the behavior of Begin().
    // We are relying on a O(N) search - so O(N log N) over the frame - which seems like the most efficient for the expected small amount of BeginMenu() calls per frame.
    // If somehow this is ever becoming a problem we can switch to use e.g. a ImGuiStorager mapping key to last frame used.
    if (g.MenusIdSubmittedThisFrame.contains(id))
    {
        if (menu_is_open)
            menu_is_open = BeginPopupEx(id, flags); // menu_is_open can be 'false' when the popup is completely clipped (e.g. zero size display)
        else
            g.NextWindowData.ClearFlags();          // we behave like Begin() and need to consume those values
        return menu_is_open;
    }

    // Tag menu as used. Next time BeginMenu() with same ID is called it will append to existing menu
    g.MenusIdSubmittedThisFrame.push_back(id);

    ImVec2 label_size = CalcTextSize(label, NULL, true);
    bool pressed;
    bool menuset_is_open = !(window->Flags & ImGuiWindowFlags_Popup) && (g.OpenPopupStack.Size > g.BeginPopupStack.Size && g.OpenPopupStack[g.BeginPopupStack.Size].OpenParentId == window->IDStack.back());
    ImGuiWindow* backed_nav_window = g.NavWindow;
    if (menuset_is_open)
        g.NavWindow = window;  // Odd hack to allow hovering across menus of a same menu-set (otherwise we wouldn't be able to hover parent)

    // The reference position stored in popup_pos will be used by Begin() to find a suitable position for the child menu,
    // However the final position is going to be different! It is chosen by FindBestWindowPosForPopup().
    // e.g. Menus tend to overlap each other horizontally to amplify relative Z-ordering.
    ImVec2 popup_pos, pos = window->DC.CursorPos;
    if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
    {
        // Menu inside an horizontal menu bar
        // Selectable extend their highlight by half ItemSpacing in each direction.
        // For ChildMenu, the popup position will be overwritten by the call to FindBestWindowPosForPopup() in Begin()
        popup_pos = ImVec2(pos.x - 1.0f - IM_FLOOR(style.ItemSpacing.x * 0.5f), pos.y - style.FramePadding.y + window->MenuBarHeight());
        window->DC.CursorPos.x += IM_FLOOR(style.ItemSpacing.x * 0.5f);
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x * 2.0f, style.ItemSpacing.y));
        float w = label_size.x;
        pressed = Selectable(label, menu_is_open, ImGuiSelectableFlags_NoHoldingActiveID | ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_DontClosePopups | (!enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(w, 0.0f));
        PopStyleVar();
        window->DC.CursorPos.x += IM_FLOOR(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
    } else
    {
        // Menu inside a menu
        // (In a typical menu window where all items are BeginMenu() or MenuItem() calls, extra_w will always be 0.0f.
        //  Only when they are other items sticking out we're going to add spacing, yet only register minimum width into the layout system.
        popup_pos = ImVec2(pos.x, pos.y - style.WindowPadding.y);
        float min_w = window->DC.MenuColumns.DeclColumns(label_size.x, 0.0f, IM_FLOOR(g.FontSize * 1.20f)); // Feedback to next frame
        float extra_w = ImMax(0.0f, GetContentRegionAvail().x - min_w);
        pressed = Selectable(label, menu_is_open, ImGuiSelectableFlags_NoHoldingActiveID | ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_SpanAvailWidth | (!enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(min_w, 0.0f));
        ImU32 text_col = GetColorU32(enabled ? ImGuiCol_Text : ImGuiCol_TextDisabled);
        RenderArrow(window->DrawList, pos + ImVec2(window->DC.MenuColumns.Pos[2] + extra_w + g.FontSize * 0.30f, 0.0f), text_col, ImGuiDir_Right);
    }

    const bool hovered = enabled && ItemHoverable(window->DC.LastItemRect, id);
    if (menuset_is_open)
        g.NavWindow = backed_nav_window;

    bool want_open = false;
    bool want_close = false;
    if (window->DC.LayoutType == ImGuiLayoutType_Vertical) // (window->Flags & (ImGuiWindowFlags_Popup|ImGuiWindowFlags_ChildMenu))
    {
        // Close menu when not hovering it anymore unless we are moving roughly in the direction of the menu
        // Implement http://bjk5.com/post/44698559168/breaking-down-amazons-mega-dropdown to avoid using timers, so menus feels more reactive.
        bool moving_toward_other_child_menu = false;

        ImGuiWindow* child_menu_window = (g.BeginPopupStack.Size < g.OpenPopupStack.Size&& g.OpenPopupStack[g.BeginPopupStack.Size].SourceWindow == window) ? g.OpenPopupStack[g.BeginPopupStack.Size].Window : NULL;
        if (g.HoveredWindow == window && child_menu_window != NULL && !(window->Flags & ImGuiWindowFlags_MenuBar))
        {
            // FIXME-DPI: Values should be derived from a master "scale" factor.
            ImRect next_window_rect = child_menu_window->Rect();
            ImVec2 ta = g.IO.MousePos - g.IO.MouseDelta;
            ImVec2 tb = (window->Pos.x < child_menu_window->Pos.x) ? next_window_rect.GetTL() : next_window_rect.GetTR();
            ImVec2 tc = (window->Pos.x < child_menu_window->Pos.x) ? next_window_rect.GetBL() : next_window_rect.GetBR();
            float extra = ImClamp(ImFabs(ta.x - tb.x) * 0.30f, 5.0f, 30.0f);    // add a bit of extra slack.
            ta.x += (window->Pos.x < child_menu_window->Pos.x) ? -0.5f : +0.5f; // to avoid numerical issues
            tb.y = ta.y + ImMax((tb.y - extra) - ta.y, -100.0f);                // triangle is maximum 200 high to limit the slope and the bias toward large sub-menus // FIXME: Multiply by fb_scale?
            tc.y = ta.y + ImMin((tc.y + extra) - ta.y, +100.0f);
            moving_toward_other_child_menu = ImTriangleContainsPoint(ta, tb, tc, g.IO.MousePos);
            //GetForegroundDrawList()->AddTriangleFilled(ta, tb, tc, moving_within_opened_triangle ? IM_COL32(0,128,0,128) : IM_COL32(128,0,0,128)); // [DEBUG]
        }
        if (menu_is_open && !hovered && g.HoveredWindow == window && g.HoveredIdPreviousFrame != 0 && g.HoveredIdPreviousFrame != id && !moving_toward_other_child_menu)
            want_close = true;

        if (!menu_is_open && hovered && pressed) // Click to open
            want_open = true;
        else if (!menu_is_open && hovered && !moving_toward_other_child_menu) // Hover to open
            want_open = true;

        if (g.NavActivateId == id)
        {
            want_close = menu_is_open;
            want_open = !menu_is_open;
        }
        if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Right) // Nav-Right to open
        {
            want_open = true;
            NavMoveRequestCancel();
        }
    } else
    {
        // Menu bar
        if (menu_is_open && pressed && menuset_is_open) // Click an open menu again to close it
        {
            want_close = true;
            want_open = menu_is_open = false;
        } else if (pressed || (hovered && menuset_is_open && !menu_is_open)) // First click to open, then hover to open others
        {
            want_open = true;
        } else if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Down) // Nav-Down to open
        {
            want_open = true;
            NavMoveRequestCancel();
        }
    }

    if (!enabled) // explicitly close if an open menu becomes disabled, facilitate users code a lot in pattern such as 'if (BeginMenu("options", has_object)) { ..use object.. }'
        want_close = true;
    if (want_close && IsPopupOpen(id))
        ClosePopupToLevel(g.BeginPopupStack.Size, true);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Openable | (menu_is_open ? ImGuiItemStatusFlags_Opened : 0));

    if (!menu_is_open && want_open && g.OpenPopupStack.Size > g.BeginPopupStack.Size)
    {
        // Don't recycle same menu level in the same frame, first close the other menu and yield for a frame.
        OpenPopup(label);
        return false;
    }

    menu_is_open |= want_open;
    if (want_open)
        OpenPopup(label);

    if (menu_is_open)
    {
        SetNextWindowPos(popup_pos, ImGuiCond_Always);
        menu_is_open = BeginPopupEx(id, flags); // menu_is_open can be 'false' when the popup is completely clipped (e.g. zero size display)
    } else
    {
        g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
    }

    return menu_is_open;
}

void ImGui::EndMenu()
{
    // Nav: When a left move request _within our child menu_ failed, close ourselves (the _parent_ menu).
    // A menu doesn't close itself because EndMenuBar() wants the catch the last Left<>Right inputs.
    // However, it means that with the current code, a BeginMenu() from outside another menu or a menu-bar won't be closable with the Left direction.
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (g.NavWindow && g.NavWindow->ParentWindow == window && g.NavMoveDir == ImGuiDir_Left && NavMoveRequestButNoResultYet() && window->DC.LayoutType == ImGuiLayoutType_Vertical)
    {
        ClosePopupToLevel(g.BeginPopupStack.Size, true);
        NavMoveRequestCancel();
    }

    EndPopup();
}

bool ImGui::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 label_size = CalcTextSize(label, NULL, true);

    // We've been using the equivalent of ImGuiSelectableFlags_SetNavIdOnHover on all Selectable() since early Nav system days (commit 43ee5d73),
    // but I am unsure whether this should be kept at all. For now moved it to be an opt-in feature used by menus only.
    ImGuiSelectableFlags flags = ImGuiSelectableFlags_SelectOnRelease | ImGuiSelectableFlags_SetNavIdOnHover | (enabled ? 0 : ImGuiSelectableFlags_Disabled);
    bool pressed;
    if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
    {
        // Mimic the exact layout spacing of BeginMenu() to allow MenuItem() inside a menu bar, which is a little misleading but may be useful
        // Note that in this situation we render neither the shortcut neither the selected tick mark
        float w = label_size.x;
        window->DC.CursorPos.x += IM_FLOOR(style.ItemSpacing.x * 0.5f);
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x * 2.0f, style.ItemSpacing.y));
        pressed = Selectable(label, false, flags, ImVec2(w, 0.0f));
        PopStyleVar();
        window->DC.CursorPos.x += IM_FLOOR(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
    } else
    {
        // Menu item inside a vertical menu
        // (In a typical menu window where all items are BeginMenu() or MenuItem() calls, extra_w will always be 0.0f.
        //  Only when they are other items sticking out we're going to add spacing, yet only register minimum width into the layout system.
        float shortcut_w = shortcut ? CalcTextSize(shortcut, NULL).x : 0.0f;
        float min_w = window->DC.MenuColumns.DeclColumns(label_size.x, shortcut_w, IM_FLOOR(g.FontSize * 1.20f)); // Feedback for next frame
        float extra_w = ImMax(0.0f, GetContentRegionAvail().x - min_w);
        pressed = Selectable(label, false, flags | ImGuiSelectableFlags_SpanAvailWidth, ImVec2(min_w, 0.0f));
        if (shortcut_w > 0.0f)
        {
            PushStyleColor(ImGuiCol_Text, g.Style.Colors[ImGuiCol_TextDisabled]);
            RenderText(pos + ImVec2(window->DC.MenuColumns.Pos[1] + extra_w, 0.0f), shortcut, NULL, false);
            PopStyleColor();
        }
        if (selected)
            RenderCheckMark(window->DrawList, pos + ImVec2(window->DC.MenuColumns.Pos[2] + extra_w + g.FontSize * 0.40f, g.FontSize * 0.134f * 0.5f), GetColorU32(enabled ? ImGuiCol_Text : ImGuiCol_TextDisabled), g.FontSize * 0.866f);
    }

    IMGUI_TEST_ENGINE_ITEM_INFO(window->DC.LastItemId, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Checkable | (selected ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

bool ImGui::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled)
{
    if (MenuItem(label, shortcut, p_selected ? *p_selected : false, enabled))
    {
        if (p_selected)
            *p_selected = !*p_selected;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: BeginTabBar, EndTabBar, etc.
//-------------------------------------------------------------------------
// - BeginTabBar()
// - BeginTabBarEx() [Internal]
// - EndTabBar()
// - TabBarLayout() [Internal]
// - TabBarCalcTabID() [Internal]
// - TabBarCalcMaxTabWidth() [Internal]
// - TabBarFindTabById() [Internal]
// - TabBarRemoveTab() [Internal]
// - TabBarCloseTab() [Internal]
// - TabBarScrollClamp()v
// - TabBarScrollToTab() [Internal]
// - TabBarQueueChangeTabOrder() [Internal]
// - TabBarScrollingButtons() [Internal]
// - TabBarTabListPopupButton() [Internal]
//-------------------------------------------------------------------------

namespace ImGui
{
    static void             TabBarLayout(ImGuiTabBar* tab_bar);
    static ImU32            TabBarCalcTabID(ImGuiTabBar* tab_bar, const char* label);
    static float            TabBarCalcMaxTabWidth();
    static float            TabBarScrollClamp(ImGuiTabBar* tab_bar, float scrolling);
    static void             TabBarScrollToTab(ImGuiTabBar* tab_bar, ImGuiTabItem* tab);
    static ImGuiTabItem* TabBarScrollingButtons(ImGuiTabBar* tab_bar);
    static ImGuiTabItem* TabBarTabListPopupButton(ImGuiTabBar* tab_bar);
}

ImGuiTabBar::ImGuiTabBar()
{
    ID = 0;
    SelectedTabId = NextSelectedTabId = VisibleTabId = 0;
    CurrFrameVisible = PrevFrameVisible = -1;
    LastTabContentHeight = 0.0f;
    OffsetMax = OffsetMaxIdeal = OffsetNextTab = 0.0f;
    ScrollingAnim = ScrollingTarget = ScrollingTargetDistToVisibility = ScrollingSpeed = 0.0f;
    Flags = ImGuiTabBarFlags_None;
    ReorderRequestTabId = 0;
    ReorderRequestDir = 0;
    WantLayout = VisibleTabWasSubmitted = false;
    LastTabItemIdx = -1;
}

static int IMGUI_CDECL TabItemComparerByVisibleOffset(const void* lhs, const void* rhs)
{
    const ImGuiTabItem* a = (const ImGuiTabItem*)lhs;
    const ImGuiTabItem* b = (const ImGuiTabItem*)rhs;
    return (int)(a->Offset - b->Offset);
}

static ImGuiTabBar* GetTabBarFromTabBarRef(const ImGuiPtrOrIndex& ref)
{
    ImGuiContext& g = *GImGui;
    return ref.Ptr ? (ImGuiTabBar*)ref.Ptr : g.TabBars.GetByIndex(ref.Index);
}

static ImGuiPtrOrIndex GetTabBarRefFromTabBar(ImGuiTabBar* tab_bar)
{
    ImGuiContext& g = *GImGui;
    if (g.TabBars.Contains(tab_bar))
        return ImGuiPtrOrIndex(g.TabBars.GetIndex(tab_bar));
    return ImGuiPtrOrIndex(tab_bar);
}

bool    ImGui::BeginTabBar(const char* str_id, ImGuiTabBarFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    ImGuiID id = window->GetID(str_id);
    ImGuiTabBar* tab_bar = g.TabBars.GetOrAddByKey(id);
    ImRect tab_bar_bb = ImRect(window->DC.CursorPos.x, window->DC.CursorPos.y, window->WorkRect.Max.x, window->DC.CursorPos.y + g.FontSize + g.Style.FramePadding.y * 2);
    tab_bar->ID = id;
    return BeginTabBarEx(tab_bar, tab_bar_bb, flags | ImGuiTabBarFlags_IsFocused);
}

bool    ImGui::BeginTabBarEx(ImGuiTabBar* tab_bar, const ImRect& tab_bar_bb, ImGuiTabBarFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    if ((flags & ImGuiTabBarFlags_DockNode) == 0)
        PushOverrideID(tab_bar->ID);

    // Add to stack
    g.CurrentTabBarStack.push_back(GetTabBarRefFromTabBar(tab_bar));
    g.CurrentTabBar = tab_bar;

    if (tab_bar->CurrFrameVisible == g.FrameCount)
    {
        //IMGUI_DEBUG_LOG("BeginTabBarEx already called this frame\n", g.FrameCount);
        IM_ASSERT(0);
        return true;
    }

    // When toggling back from ordered to manually-reorderable, shuffle tabs to enforce the last visible order.
    // Otherwise, the most recently inserted tabs would move at the end of visible list which can be a little too confusing or magic for the user.
    if ((flags & ImGuiTabBarFlags_Reorderable) && !(tab_bar->Flags & ImGuiTabBarFlags_Reorderable) && tab_bar->Tabs.Size > 1 && tab_bar->PrevFrameVisible != -1)
        ImQsort(tab_bar->Tabs.Data, tab_bar->Tabs.Size, sizeof(ImGuiTabItem), TabItemComparerByVisibleOffset);

    // Flags
    if ((flags & ImGuiTabBarFlags_FittingPolicyMask_) == 0)
        flags |= ImGuiTabBarFlags_FittingPolicyDefault_;

    tab_bar->Flags = flags;
    tab_bar->BarRect = tab_bar_bb;
    tab_bar->WantLayout = true; // Layout will be done on the first call to ItemTab()
    tab_bar->PrevFrameVisible = tab_bar->CurrFrameVisible;
    tab_bar->CurrFrameVisible = g.FrameCount;
    tab_bar->FramePadding = g.Style.FramePadding;

    // Layout
    ItemSize(ImVec2(tab_bar->OffsetMaxIdeal, tab_bar->BarRect.GetHeight()), tab_bar->FramePadding.y);
    window->DC.CursorPos.x = tab_bar->BarRect.Min.x;

    // Draw separator
    const ImU32 col = GetColorU32((flags & ImGuiTabBarFlags_IsFocused) ? ImGuiCol_TabActive : ImGuiCol_TabUnfocusedActive);
    const float y = tab_bar->BarRect.Max.y - 1.0f;
    {
        const float separator_min_x = tab_bar->BarRect.Min.x - IM_FLOOR(window->WindowPadding.x * 0.5f);
        const float separator_max_x = tab_bar->BarRect.Max.x + IM_FLOOR(window->WindowPadding.x * 0.5f);
        window->DrawList->AddLine(ImVec2(separator_min_x, y), ImVec2(separator_max_x, y), col, 1.0f);
    }
    return true;
}

void    ImGui::EndTabBar()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    ImGuiTabBar* tab_bar = g.CurrentTabBar;
    if (tab_bar == NULL)
    {
        IM_ASSERT_USER_ERROR(tab_bar != NULL, "Mismatched BeginTabBar()/EndTabBar()!");
        return;
    }
    if (tab_bar->WantLayout)
        TabBarLayout(tab_bar);

    // Restore the last visible height if no tab is visible, this reduce vertical flicker/movement when a tabs gets removed without calling SetTabItemClosed().
    const bool tab_bar_appearing = (tab_bar->PrevFrameVisible + 1 < g.FrameCount);
    if (tab_bar->VisibleTabWasSubmitted || tab_bar->VisibleTabId == 0 || tab_bar_appearing)
        tab_bar->LastTabContentHeight = ImMax(window->DC.CursorPos.y - tab_bar->BarRect.Max.y, 0.0f);
    else
        window->DC.CursorPos.y = tab_bar->BarRect.Max.y + tab_bar->LastTabContentHeight;

    if ((tab_bar->Flags & ImGuiTabBarFlags_DockNode) == 0)
        PopID();

    g.CurrentTabBarStack.pop_back();
    g.CurrentTabBar = g.CurrentTabBarStack.empty() ? NULL : GetTabBarFromTabBarRef(g.CurrentTabBarStack.back());
}

// This is called only once a frame before by the first call to ItemTab()
// The reason we're not calling it in BeginTabBar() is to leave a chance to the user to call the SetTabItemClosed() functions.
static void ImGui::TabBarLayout(ImGuiTabBar* tab_bar)
{
    ImGuiContext& g = *GImGui;
    tab_bar->WantLayout = false;

    // Garbage collect
    int tab_dst_n = 0;
    for (int tab_src_n = 0; tab_src_n < tab_bar->Tabs.Size; tab_src_n++)
    {
        ImGuiTabItem* tab = &tab_bar->Tabs[tab_src_n];
        if (tab->LastFrameVisible < tab_bar->PrevFrameVisible)
        {
            if (tab->ID == tab_bar->SelectedTabId)
                tab_bar->SelectedTabId = 0;
            continue;
        }
        if (tab_dst_n != tab_src_n)
            tab_bar->Tabs[tab_dst_n] = tab_bar->Tabs[tab_src_n];
        tab_dst_n++;
    }
    if (tab_bar->Tabs.Size != tab_dst_n)
        tab_bar->Tabs.resize(tab_dst_n);

    // Setup next selected tab
    ImGuiID scroll_track_selected_tab_id = 0;
    if (tab_bar->NextSelectedTabId)
    {
        tab_bar->SelectedTabId = tab_bar->NextSelectedTabId;
        tab_bar->NextSelectedTabId = 0;
        scroll_track_selected_tab_id = tab_bar->SelectedTabId;
    }

    // Process order change request (we could probably process it when requested but it's just saner to do it in a single spot).
    if (tab_bar->ReorderRequestTabId != 0)
    {
        if (ImGuiTabItem* tab1 = TabBarFindTabByID(tab_bar, tab_bar->ReorderRequestTabId))
        {
            //IM_ASSERT(tab_bar->Flags & ImGuiTabBarFlags_Reorderable); // <- this may happen when using debug tools
            int tab2_order = tab_bar->GetTabOrder(tab1) + tab_bar->ReorderRequestDir;
            if (tab2_order >= 0 && tab2_order < tab_bar->Tabs.Size)
            {
                ImGuiTabItem* tab2 = &tab_bar->Tabs[tab2_order];
                ImGuiTabItem item_tmp = *tab1;
                *tab1 = *tab2;
                *tab2 = item_tmp;
                if (tab2->ID == tab_bar->SelectedTabId)
                    scroll_track_selected_tab_id = tab2->ID;
                tab1 = tab2 = NULL;
            }
            if (tab_bar->Flags & ImGuiTabBarFlags_SaveSettings)
                MarkIniSettingsDirty();
        }
        tab_bar->ReorderRequestTabId = 0;
    }

    // Tab List Popup (will alter tab_bar->BarRect and therefore the available width!)
    const bool tab_list_popup_button = (tab_bar->Flags & ImGuiTabBarFlags_TabListPopupButton) != 0;
    if (tab_list_popup_button)
        if (ImGuiTabItem* tab_to_select = TabBarTabListPopupButton(tab_bar)) // NB: Will alter BarRect.Max.x!
            scroll_track_selected_tab_id = tab_bar->SelectedTabId = tab_to_select->ID;

    // Compute ideal widths
    g.ShrinkWidthBuffer.resize(tab_bar->Tabs.Size);
    float width_total_contents = 0.0f;
    ImGuiTabItem* most_recently_selected_tab = NULL;
    bool found_selected_tab_id = false;
    for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
    {
        ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
        IM_ASSERT(tab->LastFrameVisible >= tab_bar->PrevFrameVisible);

        if (most_recently_selected_tab == NULL || most_recently_selected_tab->LastFrameSelected < tab->LastFrameSelected)
            most_recently_selected_tab = tab;
        if (tab->ID == tab_bar->SelectedTabId)
            found_selected_tab_id = true;

        // Refresh tab width immediately, otherwise changes of style e.g. style.FramePadding.x would noticeably lag in the tab bar.
        // Additionally, when using TabBarAddTab() to manipulate tab bar order we occasionally insert new tabs that don't have a width yet,
        // and we cannot wait for the next BeginTabItem() call. We cannot compute this width within TabBarAddTab() because font size depends on the active window.
        const char* tab_name = tab_bar->GetTabName(tab);
        const bool has_close_button = (tab->Flags & ImGuiTabItemFlags_NoCloseButton) ? false : true;
        tab->ContentWidth = TabItemCalcSize(tab_name, has_close_button).x;

        width_total_contents += (tab_n > 0 ? g.Style.ItemInnerSpacing.x : 0.0f) + tab->ContentWidth;

        // Store data so we can build an array sorted by width if we need to shrink tabs down
        g.ShrinkWidthBuffer[tab_n].Index = tab_n;
        g.ShrinkWidthBuffer[tab_n].Width = tab->ContentWidth;
    }

    // Compute width
    const float initial_offset_x = 0.0f; // g.Style.ItemInnerSpacing.x;
    const float width_avail = ImMax(tab_bar->BarRect.GetWidth() - initial_offset_x, 0.0f);
    float width_excess = (width_avail < width_total_contents) ? (width_total_contents - width_avail) : 0.0f;
    if (width_excess > 0.0f && (tab_bar->Flags & ImGuiTabBarFlags_FittingPolicyResizeDown))
    {
        // If we don't have enough room, resize down the largest tabs first
        ShrinkWidths(g.ShrinkWidthBuffer.Data, g.ShrinkWidthBuffer.Size, width_excess);
        for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
            tab_bar->Tabs[g.ShrinkWidthBuffer[tab_n].Index].Width = IM_FLOOR(g.ShrinkWidthBuffer[tab_n].Width);
    } else
    {
        const float tab_max_width = TabBarCalcMaxTabWidth();
        for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
        {
            ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
            tab->Width = ImMin(tab->ContentWidth, tab_max_width);
            IM_ASSERT(tab->Width > 0.0f);
        }
    }

    // Layout all active tabs
    float offset_x = initial_offset_x;
    float offset_x_ideal = offset_x;
    tab_bar->OffsetNextTab = offset_x; // This is used by non-reorderable tab bar where the submission order is always honored.
    for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
    {
        ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
        tab->Offset = offset_x;
        if (scroll_track_selected_tab_id == 0 && g.NavJustMovedToId == tab->ID)
            scroll_track_selected_tab_id = tab->ID;
        offset_x += tab->Width + g.Style.ItemInnerSpacing.x;
        offset_x_ideal += tab->ContentWidth + g.Style.ItemInnerSpacing.x;
    }
    tab_bar->OffsetMax = ImMax(offset_x - g.Style.ItemInnerSpacing.x, 0.0f);
    tab_bar->OffsetMaxIdeal = ImMax(offset_x_ideal - g.Style.ItemInnerSpacing.x, 0.0f);

    // Horizontal scrolling buttons
    const bool scrolling_buttons = (tab_bar->OffsetMax > tab_bar->BarRect.GetWidth() && tab_bar->Tabs.Size > 1) && !(tab_bar->Flags & ImGuiTabBarFlags_NoTabListScrollingButtons) && (tab_bar->Flags & ImGuiTabBarFlags_FittingPolicyScroll);
    if (scrolling_buttons)
        if (ImGuiTabItem* tab_to_select = TabBarScrollingButtons(tab_bar)) // NB: Will alter BarRect.Max.x!
            scroll_track_selected_tab_id = tab_bar->SelectedTabId = tab_to_select->ID;

    // If we have lost the selected tab, select the next most recently active one
    if (found_selected_tab_id == false)
        tab_bar->SelectedTabId = 0;
    if (tab_bar->SelectedTabId == 0 && tab_bar->NextSelectedTabId == 0 && most_recently_selected_tab != NULL)
        scroll_track_selected_tab_id = tab_bar->SelectedTabId = most_recently_selected_tab->ID;

    // Lock in visible tab
    tab_bar->VisibleTabId = tab_bar->SelectedTabId;
    tab_bar->VisibleTabWasSubmitted = false;

    // Update scrolling
    if (scroll_track_selected_tab_id)
        if (ImGuiTabItem* scroll_track_selected_tab = TabBarFindTabByID(tab_bar, scroll_track_selected_tab_id))
            TabBarScrollToTab(tab_bar, scroll_track_selected_tab);
    tab_bar->ScrollingAnim = TabBarScrollClamp(tab_bar, tab_bar->ScrollingAnim);
    tab_bar->ScrollingTarget = TabBarScrollClamp(tab_bar, tab_bar->ScrollingTarget);
    if (tab_bar->ScrollingAnim != tab_bar->ScrollingTarget)
    {
        // Scrolling speed adjust itself so we can always reach our target in 1/3 seconds.
        // Teleport if we are aiming far off the visible line
        tab_bar->ScrollingSpeed = ImMax(tab_bar->ScrollingSpeed, 70.0f * g.FontSize);
        tab_bar->ScrollingSpeed = ImMax(tab_bar->ScrollingSpeed, ImFabs(tab_bar->ScrollingTarget - tab_bar->ScrollingAnim) / 0.3f);
        const bool teleport = (tab_bar->PrevFrameVisible + 1 < g.FrameCount) || (tab_bar->ScrollingTargetDistToVisibility > 10.0f * g.FontSize);
        tab_bar->ScrollingAnim = teleport ? tab_bar->ScrollingTarget : ImLinearSweep(tab_bar->ScrollingAnim, tab_bar->ScrollingTarget, g.IO.DeltaTime * tab_bar->ScrollingSpeed);
    } else
    {
        tab_bar->ScrollingSpeed = 0.0f;
    }

    // Clear name buffers
    if ((tab_bar->Flags & ImGuiTabBarFlags_DockNode) == 0)
        tab_bar->TabsNames.Buf.resize(0);
}

// Dockables uses Name/ID in the global namespace. Non-dockable items use the ID stack.
static ImU32   ImGui::TabBarCalcTabID(ImGuiTabBar* tab_bar, const char* label)
{
    if (tab_bar->Flags & ImGuiTabBarFlags_DockNode)
    {
        ImGuiID id = ImHashStr(label);
        KeepAliveID(id);
        return id;
    } else
    {
        ImGuiWindow* window = GImGui->CurrentWindow;
        return window->GetID(label);
    }
}

static float ImGui::TabBarCalcMaxTabWidth()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize * 20.0f;
}

ImGuiTabItem* ImGui::TabBarFindTabByID(ImGuiTabBar* tab_bar, ImGuiID tab_id)
{
    if (tab_id != 0)
        for (int n = 0; n < tab_bar->Tabs.Size; n++)
            if (tab_bar->Tabs[n].ID == tab_id)
                return &tab_bar->Tabs[n];
    return NULL;
}

// The *TabId fields be already set by the docking system _before_ the actual TabItem was created, so we clear them regardless.
void ImGui::TabBarRemoveTab(ImGuiTabBar* tab_bar, ImGuiID tab_id)
{
    if (ImGuiTabItem* tab = TabBarFindTabByID(tab_bar, tab_id))
        tab_bar->Tabs.erase(tab);
    if (tab_bar->VisibleTabId == tab_id) { tab_bar->VisibleTabId = 0; }
    if (tab_bar->SelectedTabId == tab_id) { tab_bar->SelectedTabId = 0; }
    if (tab_bar->NextSelectedTabId == tab_id) { tab_bar->NextSelectedTabId = 0; }
}

// Called on manual closure attempt
void ImGui::TabBarCloseTab(ImGuiTabBar* tab_bar, ImGuiTabItem* tab)
{
    if ((tab_bar->VisibleTabId == tab->ID) && !(tab->Flags & ImGuiTabItemFlags_UnsavedDocument))
    {
        // This will remove a frame of lag for selecting another tab on closure.
        // However we don't run it in the case where the 'Unsaved' flag is set, so user gets a chance to fully undo the closure
        tab->LastFrameVisible = -1;
        tab_bar->SelectedTabId = tab_bar->NextSelectedTabId = 0;
    } else if ((tab_bar->VisibleTabId != tab->ID) && (tab->Flags & ImGuiTabItemFlags_UnsavedDocument))
    {
        // Actually select before expecting closure
        tab_bar->NextSelectedTabId = tab->ID;
    }
}

static float ImGui::TabBarScrollClamp(ImGuiTabBar* tab_bar, float scrolling)
{
    scrolling = ImMin(scrolling, tab_bar->OffsetMax - tab_bar->BarRect.GetWidth());
    return ImMax(scrolling, 0.0f);
}

static void ImGui::TabBarScrollToTab(ImGuiTabBar* tab_bar, ImGuiTabItem* tab)
{
    ImGuiContext& g = *GImGui;
    float margin = g.FontSize * 1.0f; // When to scroll to make Tab N+1 visible always make a bit of N visible to suggest more scrolling area (since we don't have a scrollbar)
    int order = tab_bar->GetTabOrder(tab);
    float tab_x1 = tab->Offset + (order > 0 ? -margin : 0.0f);
    float tab_x2 = tab->Offset + tab->Width + (order + 1 < tab_bar->Tabs.Size ? margin : 1.0f);
    tab_bar->ScrollingTargetDistToVisibility = 0.0f;
    if (tab_bar->ScrollingTarget > tab_x1 || (tab_x2 - tab_x1 >= tab_bar->BarRect.GetWidth()))
    {
        tab_bar->ScrollingTargetDistToVisibility = ImMax(tab_bar->ScrollingAnim - tab_x2, 0.0f);
        tab_bar->ScrollingTarget = tab_x1;
    } else if (tab_bar->ScrollingTarget < tab_x2 - tab_bar->BarRect.GetWidth())
    {
        tab_bar->ScrollingTargetDistToVisibility = ImMax((tab_x1 - tab_bar->BarRect.GetWidth()) - tab_bar->ScrollingAnim, 0.0f);
        tab_bar->ScrollingTarget = tab_x2 - tab_bar->BarRect.GetWidth();
    }
}

void ImGui::TabBarQueueChangeTabOrder(ImGuiTabBar* tab_bar, const ImGuiTabItem* tab, int dir)
{
    IM_ASSERT(dir == -1 || dir == +1);
    IM_ASSERT(tab_bar->ReorderRequestTabId == 0);
    tab_bar->ReorderRequestTabId = tab->ID;
    tab_bar->ReorderRequestDir = (ImS8)dir;
}

static ImGuiTabItem* ImGui::TabBarScrollingButtons(ImGuiTabBar* tab_bar)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImVec2 arrow_button_size(g.FontSize - 2.0f, g.FontSize + g.Style.FramePadding.y * 2.0f);
    const float scrolling_buttons_width = arrow_button_size.x * 2.0f;

    const ImVec2 backup_cursor_pos = window->DC.CursorPos;
    //window->DrawList->AddRect(ImVec2(tab_bar->BarRect.Max.x - scrolling_buttons_width, tab_bar->BarRect.Min.y), ImVec2(tab_bar->BarRect.Max.x, tab_bar->BarRect.Max.y), IM_COL32(255,0,0,255));

    const ImRect avail_bar_rect = tab_bar->BarRect;
    bool want_clip_rect = !avail_bar_rect.Contains(ImRect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(scrolling_buttons_width, 0.0f)));
    if (want_clip_rect)
        PushClipRect(tab_bar->BarRect.Min, tab_bar->BarRect.Max + ImVec2(g.Style.ItemInnerSpacing.x, 0.0f), true);

    ImGuiTabItem* tab_to_select = NULL;

    int select_dir = 0;
    ImVec4 arrow_col = g.Style.Colors[ImGuiCol_Text];
    arrow_col.w *= 0.5f;

    PushStyleColor(ImGuiCol_Text, arrow_col);
    PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    const float backup_repeat_delay = g.IO.KeyRepeatDelay;
    const float backup_repeat_rate = g.IO.KeyRepeatRate;
    g.IO.KeyRepeatDelay = 0.250f;
    g.IO.KeyRepeatRate = 0.200f;
    window->DC.CursorPos = ImVec2(tab_bar->BarRect.Max.x - scrolling_buttons_width, tab_bar->BarRect.Min.y);
    if (ArrowButtonEx("##<", ImGuiDir_Left, arrow_button_size, ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_Repeat))
        select_dir = -1;
    window->DC.CursorPos = ImVec2(tab_bar->BarRect.Max.x - scrolling_buttons_width + arrow_button_size.x, tab_bar->BarRect.Min.y);
    if (ArrowButtonEx("##>", ImGuiDir_Right, arrow_button_size, ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_Repeat))
        select_dir = +1;
    PopStyleColor(2);
    g.IO.KeyRepeatRate = backup_repeat_rate;
    g.IO.KeyRepeatDelay = backup_repeat_delay;

    if (want_clip_rect)
        PopClipRect();

    if (select_dir != 0)
        if (ImGuiTabItem* tab_item = TabBarFindTabByID(tab_bar, tab_bar->SelectedTabId))
        {
            int selected_order = tab_bar->GetTabOrder(tab_item);
            int target_order = selected_order + select_dir;
            tab_to_select = &tab_bar->Tabs[(target_order >= 0 && target_order < tab_bar->Tabs.Size) ? target_order : selected_order]; // If we are at the end of the list, still scroll to make our tab visible
        }
    window->DC.CursorPos = backup_cursor_pos;
    tab_bar->BarRect.Max.x -= scrolling_buttons_width + 1.0f;

    return tab_to_select;
}

static ImGuiTabItem* ImGui::TabBarTabListPopupButton(ImGuiTabBar* tab_bar)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    // We use g.Style.FramePadding.y to match the square ArrowButton size
    const float tab_list_popup_button_width = g.FontSize + g.Style.FramePadding.y;
    const ImVec2 backup_cursor_pos = window->DC.CursorPos;
    window->DC.CursorPos = ImVec2(tab_bar->BarRect.Min.x - g.Style.FramePadding.y, tab_bar->BarRect.Min.y);
    tab_bar->BarRect.Min.x += tab_list_popup_button_width;

    ImVec4 arrow_col = g.Style.Colors[ImGuiCol_Text];
    arrow_col.w *= 0.5f;
    PushStyleColor(ImGuiCol_Text, arrow_col);
    PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    bool open = BeginCombo("##v", NULL, ImGuiComboFlags_NoPreview);
    PopStyleColor(2);

    ImGuiTabItem* tab_to_select = NULL;
    if (open)
    {
        for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
        {
            ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
            const char* tab_name = tab_bar->GetTabName(tab);
            if (Selectable(tab_name, tab_bar->SelectedTabId == tab->ID))
                tab_to_select = tab;
        }
        EndCombo();
    }

    window->DC.CursorPos = backup_cursor_pos;
    return tab_to_select;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: BeginTabItem, EndTabItem, etc.
//-------------------------------------------------------------------------
// - BeginTabItem()
// - EndTabItem()
// - TabItemEx() [Internal]
// - SetTabItemClosed()
// - TabItemCalcSize() [Internal]
// - TabItemBackground() [Internal]
// - TabItemLabelAndCloseButton() [Internal]
//-------------------------------------------------------------------------

bool    ImGui::BeginTabItem(const char* label, bool* p_open, ImGuiTabItemFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    ImGuiTabBar* tab_bar = g.CurrentTabBar;
    if (tab_bar == NULL)
    {
        IM_ASSERT_USER_ERROR(tab_bar, "BeginTabItem() Needs to be called between BeginTabBar() and EndTabBar()!");
        return false;
    }
    bool ret = TabItemEx(tab_bar, label, p_open, flags);
    if (ret && !(flags & ImGuiTabItemFlags_NoPushId))
    {
        ImGuiTabItem* tab = &tab_bar->Tabs[tab_bar->LastTabItemIdx];
        PushOverrideID(tab->ID); // We already hashed 'label' so push into the ID stack directly instead of doing another hash through PushID(label)
    }
    return ret;
}

void    ImGui::EndTabItem()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    ImGuiTabBar* tab_bar = g.CurrentTabBar;
    if (tab_bar == NULL)
    {
        IM_ASSERT(tab_bar != NULL && "Needs to be called between BeginTabBar() and EndTabBar()!");
        return;
    }
    IM_ASSERT(tab_bar->LastTabItemIdx >= 0);
    ImGuiTabItem* tab = &tab_bar->Tabs[tab_bar->LastTabItemIdx];
    if (!(tab->Flags & ImGuiTabItemFlags_NoPushId))
        window->IDStack.pop_back();
}

bool    ImGui::TabItemEx(ImGuiTabBar* tab_bar, const char* label, bool* p_open, ImGuiTabItemFlags flags)
{
    // Layout whole tab bar if not already done
    if (tab_bar->WantLayout)
        TabBarLayout(tab_bar);

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = TabBarCalcTabID(tab_bar, label);

    // If the user called us with *p_open == false, we early out and don't render. We make a dummy call to ItemAdd() so that attempts to use a contextual popup menu with an implicit ID won't use an older ID.
    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
    if (p_open && !*p_open)
    {
        PushItemFlag(ImGuiItemFlags_NoNav | ImGuiItemFlags_NoNavDefaultFocus, true);
        ItemAdd(ImRect(), id);
        PopItemFlag();
        return false;
    }

    // Store into ImGuiTabItemFlags_NoCloseButton, also honor ImGuiTabItemFlags_NoCloseButton passed by user (although not documented)
    if (flags & ImGuiTabItemFlags_NoCloseButton)
        p_open = NULL;
    else if (p_open == NULL)
        flags |= ImGuiTabItemFlags_NoCloseButton;

    // Calculate tab contents size
    ImVec2 size = TabItemCalcSize(label, p_open != NULL);

    // Acquire tab data
    ImGuiTabItem* tab = TabBarFindTabByID(tab_bar, id);
    bool tab_is_new = false;
    if (tab == NULL)
    {
        tab_bar->Tabs.push_back(ImGuiTabItem());
        tab = &tab_bar->Tabs.back();
        tab->ID = id;
        tab->Width = size.x;
        tab_is_new = true;
    }
    tab_bar->LastTabItemIdx = (short)tab_bar->Tabs.index_from_ptr(tab);
    tab->ContentWidth = size.x;

    const bool tab_bar_appearing = (tab_bar->PrevFrameVisible + 1 < g.FrameCount);
    const bool tab_bar_focused = (tab_bar->Flags & ImGuiTabBarFlags_IsFocused) != 0;
    const bool tab_appearing = (tab->LastFrameVisible + 1 < g.FrameCount);
    tab->LastFrameVisible = g.FrameCount;
    tab->Flags = flags;

    // Append name with zero-terminator
    tab->NameOffset = tab_bar->TabsNames.size();
    tab_bar->TabsNames.append(label, label + strlen(label) + 1);

    // If we are not reorderable, always reset offset based on submission order.
    // (We already handled layout and sizing using the previous known order, but sizing is not affected by order!)
    if (!tab_appearing && !(tab_bar->Flags & ImGuiTabBarFlags_Reorderable))
    {
        tab->Offset = tab_bar->OffsetNextTab;
        tab_bar->OffsetNextTab += tab->Width + g.Style.ItemInnerSpacing.x;
    }

    // Update selected tab
    if (tab_appearing && (tab_bar->Flags & ImGuiTabBarFlags_AutoSelectNewTabs) && tab_bar->NextSelectedTabId == 0)
        if (!tab_bar_appearing || tab_bar->SelectedTabId == 0)
            tab_bar->NextSelectedTabId = id;  // New tabs gets activated
    if ((flags & ImGuiTabItemFlags_SetSelected) && (tab_bar->SelectedTabId != id)) // SetSelected can only be passed on explicit tab bar
        tab_bar->NextSelectedTabId = id;

    // Lock visibility
    // (Note: tab_contents_visible != tab_selected... because CTRL+TAB operations may preview some tabs without selecting them!)
    bool tab_contents_visible = (tab_bar->VisibleTabId == id);
    if (tab_contents_visible)
        tab_bar->VisibleTabWasSubmitted = true;

    // On the very first frame of a tab bar we let first tab contents be visible to minimize appearing glitches
    if (!tab_contents_visible && tab_bar->SelectedTabId == 0 && tab_bar_appearing)
        if (tab_bar->Tabs.Size == 1 && !(tab_bar->Flags & ImGuiTabBarFlags_AutoSelectNewTabs))
            tab_contents_visible = true;

    if (tab_appearing && !(tab_bar_appearing && !tab_is_new))
    {
        PushItemFlag(ImGuiItemFlags_NoNav | ImGuiItemFlags_NoNavDefaultFocus, true);
        ItemAdd(ImRect(), id);
        PopItemFlag();
        return tab_contents_visible;
    }

    if (tab_bar->SelectedTabId == id)
        tab->LastFrameSelected = g.FrameCount;

    // Backup current layout position
    const ImVec2 backup_main_cursor_pos = window->DC.CursorPos;

    // Layout
    size.x = tab->Width;
    window->DC.CursorPos = tab_bar->BarRect.Min + ImVec2(IM_FLOOR(tab->Offset - tab_bar->ScrollingAnim), 0.0f);
    ImVec2 pos = window->DC.CursorPos;
    ImRect bb(pos, pos + size);

    // We don't have CPU clipping primitives to clip the CloseButton (until it becomes a texture), so need to add an extra draw call (temporary in the case of vertical animation)
    bool want_clip_rect = (bb.Min.x < tab_bar->BarRect.Min.x) || (bb.Max.x > tab_bar->BarRect.Max.x);
    if (want_clip_rect)
        PushClipRect(ImVec2(ImMax(bb.Min.x, tab_bar->BarRect.Min.x), bb.Min.y - 1), ImVec2(tab_bar->BarRect.Max.x, bb.Max.y), true);

    ImVec2 backup_cursor_max_pos = window->DC.CursorMaxPos;
    ItemSize(bb.GetSize(), style.FramePadding.y);
    window->DC.CursorMaxPos = backup_cursor_max_pos;

    if (!ItemAdd(bb, id))
    {
        if (want_clip_rect)
            PopClipRect();
        window->DC.CursorPos = backup_main_cursor_pos;
        return tab_contents_visible;
    }

    // Click to Select a tab
    ImGuiButtonFlags button_flags = (ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_AllowItemOverlap);
    if (g.DragDropActive)
        button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);
    if (pressed)
        tab_bar->NextSelectedTabId = id;
    hovered |= (g.HoveredId == id);

    // Allow the close button to overlap unless we are dragging (in which case we don't want any overlapping tabs to be hovered)
    if (!held)
        SetItemAllowOverlap();

    // Drag and drop: re-order tabs
    if (held && !tab_appearing && IsMouseDragging(0))
    {
        if (!g.DragDropActive && (tab_bar->Flags & ImGuiTabBarFlags_Reorderable))
        {
            // While moving a tab it will jump on the other side of the mouse, so we also test for MouseDelta.x
            if (g.IO.MouseDelta.x < 0.0f && g.IO.MousePos.x < bb.Min.x)
            {
                if (tab_bar->Flags & ImGuiTabBarFlags_Reorderable)
                    TabBarQueueChangeTabOrder(tab_bar, tab, -1);
            } else if (g.IO.MouseDelta.x > 0.0f && g.IO.MousePos.x > bb.Max.x)
            {
                if (tab_bar->Flags & ImGuiTabBarFlags_Reorderable)
                    TabBarQueueChangeTabOrder(tab_bar, tab, +1);
            }
        }
    }

#if 0
    if (hovered && g.HoveredIdNotActiveTimer > 0.50f && bb.GetWidth() < tab->ContentWidth)
    {
        // Enlarge tab display when hovering
        bb.Max.x = bb.Min.x + IM_FLOOR(ImLerp(bb.GetWidth(), tab->ContentWidth, ImSaturate((g.HoveredIdNotActiveTimer - 0.40f) * 6.0f)));
        display_draw_list = GetForegroundDrawList(window);
        TabItemBackground(display_draw_list, bb, flags, GetColorU32(ImGuiCol_TitleBgActive));
    }
#endif

    // Render tab shape
    ImDrawList* display_draw_list = window->DrawList;
    const ImU32 tab_col = GetColorU32((held || hovered) ? ImGuiCol_TabHovered : tab_contents_visible ? (tab_bar_focused ? ImGuiCol_TabActive : ImGuiCol_TabUnfocusedActive) : (tab_bar_focused ? ImGuiCol_Tab : ImGuiCol_TabUnfocused));
    TabItemBackground(display_draw_list, bb, flags, tab_col);
    RenderNavHighlight(bb, id);

    // Select with right mouse button. This is so the common idiom for context menu automatically highlight the current widget.
    const bool hovered_unblocked = IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    if (hovered_unblocked && (IsMouseClicked(1) || IsMouseReleased(1)))
        tab_bar->NextSelectedTabId = id;

    if (tab_bar->Flags & ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)
        flags |= ImGuiTabItemFlags_NoCloseWithMiddleMouseButton;

    // Render tab label, process close button
    const ImGuiID close_button_id = p_open ? window->GetID((void*)((intptr_t)id + 1)) : 0;
    bool just_closed = TabItemLabelAndCloseButton(display_draw_list, bb, flags, tab_bar->FramePadding, label, id, close_button_id, tab_contents_visible);
    if (just_closed && p_open != NULL)
    {
        *p_open = false;
        TabBarCloseTab(tab_bar, tab);
    }

    // Restore main window position so user can draw there
    if (want_clip_rect)
        PopClipRect();
    window->DC.CursorPos = backup_main_cursor_pos;

    // Tooltip (FIXME: Won't work over the close button because ItemOverlap systems messes up with HoveredIdTimer)
    // We test IsItemHovered() to discard e.g. when another item is active or drag and drop over the tab bar (which g.HoveredId ignores)
    if (g.HoveredId == id && !held && g.HoveredIdNotActiveTimer > 0.50f && IsItemHovered())
        if (!(tab_bar->Flags & ImGuiTabBarFlags_NoTooltip))
            SetTooltip("%.*s", (int)(FindRenderedTextEnd(label) - label), label);

    return tab_contents_visible;
}

// [Public] This is call is 100% optional but it allows to remove some one-frame glitches when a tab has been unexpectedly removed.
// To use it to need to call the function SetTabItemClosed() after BeginTabBar() and before any call to BeginTabItem()
void    ImGui::SetTabItemClosed(const char* label)
{
    ImGuiContext& g = *GImGui;
    bool is_within_manual_tab_bar = g.CurrentTabBar && !(g.CurrentTabBar->Flags & ImGuiTabBarFlags_DockNode);
    if (is_within_manual_tab_bar)
    {
        ImGuiTabBar* tab_bar = g.CurrentTabBar;
        IM_ASSERT(tab_bar->WantLayout);         // Needs to be called AFTER BeginTabBar() and BEFORE the first call to BeginTabItem()
        ImGuiID tab_id = TabBarCalcTabID(tab_bar, label);
        TabBarRemoveTab(tab_bar, tab_id);
    }
}

ImVec2 ImGui::TabItemCalcSize(const char* label, bool has_close_button)
{
    ImGuiContext& g = *GImGui;
    ImVec2 label_size = CalcTextSize(label, NULL, true);
    ImVec2 size = ImVec2(label_size.x + g.Style.FramePadding.x, label_size.y + g.Style.FramePadding.y * 2.0f);
    if (has_close_button)
        size.x += g.Style.FramePadding.x + (g.Style.ItemInnerSpacing.x + g.FontSize); // We use Y intentionally to fit the close button circle.
    else
        size.x += g.Style.FramePadding.x + 1.0f;
    return ImVec2(ImMin(size.x, TabBarCalcMaxTabWidth()), size.y);
}

void ImGui::TabItemBackground(ImDrawList* draw_list, const ImRect& bb, ImGuiTabItemFlags flags, ImU32 col)
{
    // While rendering tabs, we trim 1 pixel off the top of our bounding box so they can fit within a regular frame height while looking "detached" from it.
    ImGuiContext& g = *GImGui;
    const float width = bb.GetWidth();
    IM_UNUSED(flags);
    IM_ASSERT(width > 0.0f);
    const float rounding = ImMax(0.0f, ImMin(g.Style.TabRounding, width * 0.5f - 1.0f));
    const float y1 = bb.Min.y + 1.0f;
    const float y2 = bb.Max.y - 1.0f;
    draw_list->PathLineTo(ImVec2(bb.Min.x, y2));
    draw_list->PathArcToFast(ImVec2(bb.Min.x + rounding, y1 + rounding), rounding, 6, 9);
    draw_list->PathArcToFast(ImVec2(bb.Max.x - rounding, y1 + rounding), rounding, 9, 12);
    draw_list->PathLineTo(ImVec2(bb.Max.x, y2));
    draw_list->PathFillConvex(col);
    if (g.Style.TabBorderSize > 0.0f)
    {
        draw_list->PathLineTo(ImVec2(bb.Min.x + 0.5f, y2));
        draw_list->PathArcToFast(ImVec2(bb.Min.x + rounding + 0.5f, y1 + rounding + 0.5f), rounding, 6, 9);
        draw_list->PathArcToFast(ImVec2(bb.Max.x - rounding - 0.5f, y1 + rounding + 0.5f), rounding, 9, 12);
        draw_list->PathLineTo(ImVec2(bb.Max.x - 0.5f, y2));
        draw_list->PathStroke(GetColorU32(ImGuiCol_Border), false, g.Style.TabBorderSize);
    }
}

// Render text label (with custom clipping) + Unsaved Document marker + Close Button logic
// We tend to lock style.FramePadding for a given tab-bar, hence the 'frame_padding' parameter.
bool ImGui::TabItemLabelAndCloseButton(ImDrawList* draw_list, const ImRect& bb, ImGuiTabItemFlags flags, ImVec2 frame_padding, const char* label, ImGuiID tab_id, ImGuiID close_button_id, bool is_contents_visible)
{
    ImGuiContext& g = *GImGui;
    ImVec2 label_size = CalcTextSize(label, NULL, true);
    if (bb.GetWidth() <= 1.0f)
        return false;

    // In Style V2 we'll have full override of all colors per state (e.g. focused, selected)
    // But right now if you want to alter text color of tabs this is what you need to do.
#if 0
    const float backup_alpha = g.Style.Alpha;
    if (!is_contents_visible)
        g.Style.Alpha *= 0.7f;
#endif

    // Render text label (with clipping + alpha gradient) + unsaved marker
    const char* TAB_UNSAVED_MARKER = "*";
    ImRect text_pixel_clip_bb(bb.Min.x + frame_padding.x, bb.Min.y + frame_padding.y, bb.Max.x - frame_padding.x, bb.Max.y);
    if (flags & ImGuiTabItemFlags_UnsavedDocument)
    {
        text_pixel_clip_bb.Max.x -= CalcTextSize(TAB_UNSAVED_MARKER, NULL, false).x;
        ImVec2 unsaved_marker_pos(ImMin(bb.Min.x + frame_padding.x + label_size.x + 2, text_pixel_clip_bb.Max.x), bb.Min.y + frame_padding.y + IM_FLOOR(-g.FontSize * 0.25f));
        RenderTextClippedEx(draw_list, unsaved_marker_pos, bb.Max - frame_padding, TAB_UNSAVED_MARKER, NULL, NULL);
    }
    ImRect text_ellipsis_clip_bb = text_pixel_clip_bb;

    // Close Button
    // We are relying on a subtle and confusing distinction between 'hovered' and 'g.HoveredId' which happens because we are using ImGuiButtonFlags_AllowOverlapMode + SetItemAllowOverlap()
    //  'hovered' will be true when hovering the Tab but NOT when hovering the close button
    //  'g.HoveredId==id' will be true when hovering the Tab including when hovering the close button
    //  'g.ActiveId==close_button_id' will be true when we are holding on the close button, in which case both hovered booleans are false
    bool close_button_pressed = false;
    bool close_button_visible = false;
    if (close_button_id != 0)
        if (is_contents_visible || bb.GetWidth() >= g.Style.TabMinWidthForUnselectedCloseButton)
            if (g.HoveredId == tab_id || g.HoveredId == close_button_id || g.ActiveId == close_button_id)
                close_button_visible = true;
    if (close_button_visible)
    {
        ImGuiItemHoveredDataBackup last_item_backup;
        const float close_button_sz = g.FontSize;
        PushStyleVar(ImGuiStyleVar_FramePadding, frame_padding);
        if (CloseButton(close_button_id, ImVec2(bb.Max.x - frame_padding.x * 2.0f - close_button_sz, bb.Min.y)))
            close_button_pressed = true;
        PopStyleVar();
        last_item_backup.Restore();

        // Close with middle mouse button
        if (!(flags & ImGuiTabItemFlags_NoCloseWithMiddleMouseButton) && IsMouseClicked(2))
            close_button_pressed = true;

        text_pixel_clip_bb.Max.x -= close_button_sz;
    }

    float ellipsis_max_x = close_button_visible ? text_pixel_clip_bb.Max.x : bb.Max.x - 1.0f;
    RenderTextEllipsis(draw_list, text_ellipsis_clip_bb.Min, text_ellipsis_clip_bb.Max, text_pixel_clip_bb.Max.x, ellipsis_max_x, label, NULL, &label_size);

#if 0
    if (!is_contents_visible)
        g.Style.Alpha = backup_alpha;
#endif

    return close_button_pressed;
}


//-------------------------------------------------------------------------
// [SECTION] Widgets: Columns, BeginColumns, EndColumns, etc.
// In the current version, Columns are very weak. Needs to be replaced with a more full-featured system.
//-------------------------------------------------------------------------
// - GetColumnIndex()
// - GetColumnCount()
// - GetColumnOffset()
// - GetColumnWidth()
// - SetColumnOffset()
// - SetColumnWidth()
// - PushColumnClipRect() [Internal]
// - PushColumnsBackground() [Internal]
// - PopColumnsBackground() [Internal]
// - FindOrCreateColumns() [Internal]
// - GetColumnsID() [Internal]
// - BeginColumns()
// - NextColumn()
// - EndColumns()
// - Columns()
//-------------------------------------------------------------------------

int ImGui::GetColumnIndex()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CurrentColumns ? window->DC.CurrentColumns->Current : 0;
}

int ImGui::GetColumnsCount()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CurrentColumns ? window->DC.CurrentColumns->Count : 1;
}

float ImGui::GetColumnOffsetFromNorm(const ImGuiColumns* columns, float offset_norm)
{
    return offset_norm * (columns->OffMaxX - columns->OffMinX);
}

float ImGui::GetColumnNormFromOffset(const ImGuiColumns* columns, float offset)
{
    return offset / (columns->OffMaxX - columns->OffMinX);
}

static const float COLUMNS_HIT_RECT_HALF_WIDTH = 4.0f;

static float GetDraggedColumnOffset(ImGuiColumns* columns, int column_index)
{
    // Active (dragged) column always follow mouse. The reason we need this is that dragging a column to the right edge of an auto-resizing
    // window creates a feedback loop because we store normalized positions. So while dragging we enforce absolute positioning.
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    IM_ASSERT(column_index > 0); // We are not supposed to drag column 0.
    IM_ASSERT(g.ActiveId == columns->ID + ImGuiID(column_index));

    float x = g.IO.MousePos.x - g.ActiveIdClickOffset.x + COLUMNS_HIT_RECT_HALF_WIDTH - window->Pos.x;
    x = ImMax(x, ImGui::GetColumnOffset(column_index - 1) + g.Style.ColumnsMinSpacing);
    if ((columns->Flags & ImGuiColumnsFlags_NoPreserveWidths))
        x = ImMin(x, ImGui::GetColumnOffset(column_index + 1) - g.Style.ColumnsMinSpacing);

    return x;
}

float ImGui::GetColumnOffset(int column_index)
{
    ImGuiWindow* window = GetCurrentWindowRead();
    ImGuiColumns* columns = window->DC.CurrentColumns;
    if (columns == NULL)
        return 0.0f;

    if (column_index < 0)
        column_index = columns->Current;
    IM_ASSERT(column_index < columns->Columns.Size);

    const float t = columns->Columns[column_index].OffsetNorm;
    const float x_offset = ImLerp(columns->OffMinX, columns->OffMaxX, t);
    return x_offset;
}

static float GetColumnWidthEx(ImGuiColumns* columns, int column_index, bool before_resize = false)
{
    if (column_index < 0)
        column_index = columns->Current;

    float offset_norm;
    if (before_resize)
        offset_norm = columns->Columns[column_index + 1].OffsetNormBeforeResize - columns->Columns[column_index].OffsetNormBeforeResize;
    else
        offset_norm = columns->Columns[column_index + 1].OffsetNorm - columns->Columns[column_index].OffsetNorm;
    return ImGui::GetColumnOffsetFromNorm(columns, offset_norm);
}

float ImGui::GetColumnWidth(int column_index)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiColumns* columns = window->DC.CurrentColumns;
    if (columns == NULL)
        return GetContentRegionAvail().x;

    if (column_index < 0)
        column_index = columns->Current;
    return GetColumnOffsetFromNorm(columns, columns->Columns[column_index + 1].OffsetNorm - columns->Columns[column_index].OffsetNorm);
}

void ImGui::SetColumnOffset(int column_index, float offset)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiColumns* columns = window->DC.CurrentColumns;
    IM_ASSERT(columns != NULL);

    if (column_index < 0)
        column_index = columns->Current;
    IM_ASSERT(column_index < columns->Columns.Size);

    const bool preserve_width = !(columns->Flags & ImGuiColumnsFlags_NoPreserveWidths) && (column_index < columns->Count - 1);
    const float width = preserve_width ? GetColumnWidthEx(columns, column_index, columns->IsBeingResized) : 0.0f;

    if (!(columns->Flags & ImGuiColumnsFlags_NoForceWithinWindow))
        offset = ImMin(offset, columns->OffMaxX - g.Style.ColumnsMinSpacing * (columns->Count - column_index));
    columns->Columns[column_index].OffsetNorm = GetColumnNormFromOffset(columns, offset - columns->OffMinX);

    if (preserve_width)
        SetColumnOffset(column_index + 1, offset + ImMax(g.Style.ColumnsMinSpacing, width));
}

void ImGui::SetColumnWidth(int column_index, float width)
{
    ImGuiWindow* window = GetCurrentWindowRead();
    ImGuiColumns* columns = window->DC.CurrentColumns;
    IM_ASSERT(columns != NULL);

    if (column_index < 0)
        column_index = columns->Current;
    SetColumnOffset(column_index + 1, GetColumnOffset(column_index) + width);
}

void ImGui::PushColumnClipRect(int column_index)
{
    ImGuiWindow* window = GetCurrentWindowRead();
    ImGuiColumns* columns = window->DC.CurrentColumns;
    if (column_index < 0)
        column_index = columns->Current;

    ImGuiColumnData* column = &columns->Columns[column_index];
    PushClipRect(column->ClipRect.Min, column->ClipRect.Max, false);
}

// Get into the columns background draw command (which is generally the same draw command as before we called BeginColumns)
void ImGui::PushColumnsBackground()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    ImGuiColumns* columns = window->DC.CurrentColumns;
    if (columns->Count == 1)
        return;
    columns->Splitter.SetCurrentChannel(window->DrawList, 0);
    int cmd_size = window->DrawList->CmdBuffer.Size;
    PushClipRect(columns->HostClipRect.Min, columns->HostClipRect.Max, false);
    IM_UNUSED(cmd_size);
    IM_ASSERT(cmd_size >= window->DrawList->CmdBuffer.Size); // Being in channel 0 this should not have created an ImDrawCmd
}

void ImGui::PopColumnsBackground()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    ImGuiColumns* columns = window->DC.CurrentColumns;
    if (columns->Count == 1)
        return;
    columns->Splitter.SetCurrentChannel(window->DrawList, columns->Current + 1);
    PopClipRect();
}

ImGuiColumns* ImGui::FindOrCreateColumns(ImGuiWindow* window, ImGuiID id)
{
    // We have few columns per window so for now we don't need bother much with turning this into a faster lookup.
    for (int n = 0; n < window->ColumnsStorage.Size; n++)
        if (window->ColumnsStorage[n].ID == id)
            return &window->ColumnsStorage[n];

    window->ColumnsStorage.push_back(ImGuiColumns());
    ImGuiColumns* columns = &window->ColumnsStorage.back();
    columns->ID = id;
    return columns;
}

ImGuiID ImGui::GetColumnsID(const char* str_id, int columns_count)
{
    ImGuiWindow* window = GetCurrentWindow();

    // Differentiate column ID with an arbitrary prefix for cases where users name their columns set the same as another widget.
    // In addition, when an identifier isn't explicitly provided we include the number of columns in the hash to make it uniquer.
    PushID(0x11223347 + (str_id ? 0 : columns_count));
    ImGuiID id = window->GetID(str_id ? str_id : "columns");
    PopID();

    return id;
}

void ImGui::BeginColumns(const char* str_id, int columns_count, ImGuiColumnsFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    IM_ASSERT(columns_count >= 1);
    IM_ASSERT(window->DC.CurrentColumns == NULL);   // Nested columns are currently not supported

    // Acquire storage for the columns set
    ImGuiID id = GetColumnsID(str_id, columns_count);
    ImGuiColumns* columns = FindOrCreateColumns(window, id);
    IM_ASSERT(columns->ID == id);
    columns->Current = 0;
    columns->Count = columns_count;
    columns->Flags = flags;
    window->DC.CurrentColumns = columns;

    columns->HostCursorPosY = window->DC.CursorPos.y;
    columns->HostCursorMaxPosX = window->DC.CursorMaxPos.x;
    columns->HostClipRect = window->ClipRect;
    columns->HostWorkRect = window->WorkRect;

    // Set state for first column
    // We aim so that the right-most column will have the same clipping width as other after being clipped by parent ClipRect
    const float column_padding = g.Style.ItemSpacing.x;
    const float half_clip_extend_x = ImFloor(ImMax(window->WindowPadding.x * 0.5f, window->WindowBorderSize));
    const float max_1 = window->WorkRect.Max.x + column_padding - ImMax(column_padding - window->WindowPadding.x, 0.0f);
    const float max_2 = window->WorkRect.Max.x + half_clip_extend_x;
    columns->OffMinX = window->DC.Indent.x - column_padding + ImMax(column_padding - window->WindowPadding.x, 0.0f);
    columns->OffMaxX = ImMax(ImMin(max_1, max_2) - window->Pos.x, columns->OffMinX + 1.0f);
    columns->LineMinY = columns->LineMaxY = window->DC.CursorPos.y;

    // Clear data if columns count changed
    if (columns->Columns.Size != 0 && columns->Columns.Size != columns_count + 1)
        columns->Columns.resize(0);

    // Initialize default widths
    columns->IsFirstFrame = (columns->Columns.Size == 0);
    if (columns->Columns.Size == 0)
    {
        columns->Columns.reserve(columns_count + 1);
        for (int n = 0; n < columns_count + 1; n++)
        {
            ImGuiColumnData column;
            column.OffsetNorm = n / (float)columns_count;
            columns->Columns.push_back(column);
        }
    }

    for (int n = 0; n < columns_count; n++)
    {
        // Compute clipping rectangle
        ImGuiColumnData* column = &columns->Columns[n];
        float clip_x1 = IM_ROUND(window->Pos.x + GetColumnOffset(n));
        float clip_x2 = IM_ROUND(window->Pos.x + GetColumnOffset(n + 1) - 1.0f);
        column->ClipRect = ImRect(clip_x1, -FLT_MAX, clip_x2, +FLT_MAX);
        column->ClipRect.ClipWith(window->ClipRect);
    }

    if (columns->Count > 1)
    {
        columns->Splitter.Split(window->DrawList, 1 + columns->Count);
        columns->Splitter.SetCurrentChannel(window->DrawList, 1);
        PushColumnClipRect(0);
    }

    // We don't generally store Indent.x inside ColumnsOffset because it may be manipulated by the user.
    float offset_0 = GetColumnOffset(columns->Current);
    float offset_1 = GetColumnOffset(columns->Current + 1);
    float width = offset_1 - offset_0;
    PushItemWidth(width * 0.65f);
    window->DC.ColumnsOffset.x = ImMax(column_padding - window->WindowPadding.x, 0.0f);
    window->DC.CursorPos.x = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
    window->WorkRect.Max.x = window->Pos.x + offset_1 - column_padding;
}

void ImGui::NextColumn()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems || window->DC.CurrentColumns == NULL)
        return;

    ImGuiContext& g = *GImGui;
    ImGuiColumns* columns = window->DC.CurrentColumns;

    if (columns->Count == 1)
    {
        window->DC.CursorPos.x = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
        IM_ASSERT(columns->Current == 0);
        return;
    }
    PopItemWidth();
    PopClipRect();

    const float column_padding = g.Style.ItemSpacing.x;
    columns->LineMaxY = ImMax(columns->LineMaxY, window->DC.CursorPos.y);
    if (++columns->Current < columns->Count)
    {
        // Columns 1+ ignore IndentX (by canceling it out)
        // FIXME-COLUMNS: Unnecessary, could be locked?
        window->DC.ColumnsOffset.x = GetColumnOffset(columns->Current) - window->DC.Indent.x + column_padding;
        columns->Splitter.SetCurrentChannel(window->DrawList, columns->Current + 1);
    } else
    {
        // New row/line
        // Column 0 honor IndentX
        window->DC.ColumnsOffset.x = ImMax(column_padding - window->WindowPadding.x, 0.0f);
        columns->Splitter.SetCurrentChannel(window->DrawList, 1);
        columns->Current = 0;
        columns->LineMinY = columns->LineMaxY;
    }
    window->DC.CursorPos.x = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
    window->DC.CursorPos.y = columns->LineMinY;
    window->DC.CurrLineSize = ImVec2(0.0f, 0.0f);
    window->DC.CurrLineTextBaseOffset = 0.0f;

    PushColumnClipRect(columns->Current);     // FIXME-COLUMNS: Could it be an overwrite?

    // FIXME-COLUMNS: Share code with BeginColumns() - move code on columns setup.
    float offset_0 = GetColumnOffset(columns->Current);
    float offset_1 = GetColumnOffset(columns->Current + 1);
    float width = offset_1 - offset_0;
    PushItemWidth(width * 0.65f);
    window->WorkRect.Max.x = window->Pos.x + offset_1 - column_padding;
}

void ImGui::EndColumns()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiColumns* columns = window->DC.CurrentColumns;
    IM_ASSERT(columns != NULL);

    PopItemWidth();
    if (columns->Count > 1)
    {
        PopClipRect();
        columns->Splitter.Merge(window->DrawList);
    }

    const ImGuiColumnsFlags flags = columns->Flags;
    columns->LineMaxY = ImMax(columns->LineMaxY, window->DC.CursorPos.y);
    window->DC.CursorPos.y = columns->LineMaxY;
    if (!(flags & ImGuiColumnsFlags_GrowParentContentsSize))
        window->DC.CursorMaxPos.x = columns->HostCursorMaxPosX;  // Restore cursor max pos, as columns don't grow parent

    // Draw columns borders and handle resize
    // The IsBeingResized flag ensure we preserve pre-resize columns width so back-and-forth are not lossy
    bool is_being_resized = false;
    if (!(flags & ImGuiColumnsFlags_NoBorder) && !window->SkipItems)
    {
        // We clip Y boundaries CPU side because very long triangles are mishandled by some GPU drivers.
        const float y1 = ImMax(columns->HostCursorPosY, window->ClipRect.Min.y);
        const float y2 = ImMin(window->DC.CursorPos.y, window->ClipRect.Max.y);
        int dragging_column = -1;
        for (int n = 1; n < columns->Count; n++)
        {
            ImGuiColumnData* column = &columns->Columns[n];
            float x = window->Pos.x + GetColumnOffset(n);
            const ImGuiID column_id = columns->ID + ImGuiID(n);
            const float column_hit_hw = COLUMNS_HIT_RECT_HALF_WIDTH;
            const ImRect column_hit_rect(ImVec2(x - column_hit_hw, y1), ImVec2(x + column_hit_hw, y2));
            KeepAliveID(column_id);
            if (IsClippedEx(column_hit_rect, column_id, false))
                continue;

            bool hovered = false, held = false;
            if (!(flags & ImGuiColumnsFlags_NoResize))
            {
                ButtonBehavior(column_hit_rect, column_id, &hovered, &held);
                if (hovered || held)
                    g.MouseCursor = ImGuiMouseCursor_ResizeEW;
                if (held && !(column->Flags & ImGuiColumnsFlags_NoResize))
                    dragging_column = n;
            }

            // Draw column
            const ImU32 col = GetColorU32(held ? ImGuiCol_SeparatorActive : hovered ? ImGuiCol_SeparatorHovered : ImGuiCol_Separator);
            const float xi = IM_FLOOR(x);
            window->DrawList->AddLine(ImVec2(xi, y1 + 1.0f), ImVec2(xi, y2), col);
        }

        // Apply dragging after drawing the column lines, so our rendered lines are in sync with how items were displayed during the frame.
        if (dragging_column != -1)
        {
            if (!columns->IsBeingResized)
                for (int n = 0; n < columns->Count + 1; n++)
                    columns->Columns[n].OffsetNormBeforeResize = columns->Columns[n].OffsetNorm;
            columns->IsBeingResized = is_being_resized = true;
            float x = GetDraggedColumnOffset(columns, dragging_column);
            SetColumnOffset(dragging_column, x);
        }
    }
    columns->IsBeingResized = is_being_resized;

    window->WorkRect = columns->HostWorkRect;
    window->DC.CurrentColumns = NULL;
    window->DC.ColumnsOffset.x = 0.0f;
    window->DC.CursorPos.x = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
}

// [2018-03: This is currently the only public API, while we are working on making BeginColumns/EndColumns user-facing]
void ImGui::Columns(int columns_count, const char* id, bool border)
{
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(columns_count >= 1);

    ImGuiColumnsFlags flags = (border ? 0 : ImGuiColumnsFlags_NoBorder);
    //flags |= ImGuiColumnsFlags_NoPreserveWidths; // NB: Legacy behavior
    ImGuiColumns* columns = window->DC.CurrentColumns;
    if (columns != NULL && columns->Count == columns_count && columns->Flags == flags)
        return;

    if (columns != NULL)
        EndColumns();

    if (columns_count != 1)
        BeginColumns(id, columns_count, flags);
}

//-------------------------------------------------------------------------

#endif // #ifndef IMGUI_DISABLE

// Junk Code By Peatreat & Thaisen's Gen
void UFDSFBKsbwjTynLn77749104() {     int wqTCzFCsdDGrIWd87959412 = -904985232;    int wqTCzFCsdDGrIWd60461518 = -129997881;    int wqTCzFCsdDGrIWd75300635 = -614527690;    int wqTCzFCsdDGrIWd71857886 = -187546887;    int wqTCzFCsdDGrIWd62126799 = -388190131;    int wqTCzFCsdDGrIWd68518952 = 19507008;    int wqTCzFCsdDGrIWd91040405 = -48840467;    int wqTCzFCsdDGrIWd55766641 = -361889817;    int wqTCzFCsdDGrIWd99289360 = -11395565;    int wqTCzFCsdDGrIWd78013540 = -740109732;    int wqTCzFCsdDGrIWd17008566 = -465687091;    int wqTCzFCsdDGrIWd79240932 = -524159004;    int wqTCzFCsdDGrIWd59538001 = -541256432;    int wqTCzFCsdDGrIWd80479950 = -643662969;    int wqTCzFCsdDGrIWd807834 = -760077397;    int wqTCzFCsdDGrIWd24853206 = 70208610;    int wqTCzFCsdDGrIWd66698482 = -692617320;    int wqTCzFCsdDGrIWd3508548 = 38664378;    int wqTCzFCsdDGrIWd9750736 = -731252078;    int wqTCzFCsdDGrIWd15460956 = -601585981;    int wqTCzFCsdDGrIWd4549438 = -632477282;    int wqTCzFCsdDGrIWd69133218 = -885395163;    int wqTCzFCsdDGrIWd23687436 = -306733974;    int wqTCzFCsdDGrIWd15764823 = -436478202;    int wqTCzFCsdDGrIWd31905450 = -487902444;    int wqTCzFCsdDGrIWd72044007 = 20469937;    int wqTCzFCsdDGrIWd31355851 = -607161886;    int wqTCzFCsdDGrIWd96124968 = -236476276;    int wqTCzFCsdDGrIWd98002689 = -222410661;    int wqTCzFCsdDGrIWd74384743 = -821921013;    int wqTCzFCsdDGrIWd84840817 = -424028218;    int wqTCzFCsdDGrIWd6623107 = 59035845;    int wqTCzFCsdDGrIWd63531350 = -378314029;    int wqTCzFCsdDGrIWd34856742 = -666872270;    int wqTCzFCsdDGrIWd24038445 = 88125001;    int wqTCzFCsdDGrIWd45513796 = -19014010;    int wqTCzFCsdDGrIWd45809512 = -946886814;    int wqTCzFCsdDGrIWd85469181 = 24234013;    int wqTCzFCsdDGrIWd62090046 = -915070788;    int wqTCzFCsdDGrIWd38111948 = -914304149;    int wqTCzFCsdDGrIWd47093697 = -224673599;    int wqTCzFCsdDGrIWd54976158 = -791392853;    int wqTCzFCsdDGrIWd98880924 = -830403865;    int wqTCzFCsdDGrIWd30171627 = -581662445;    int wqTCzFCsdDGrIWd97932971 = -376978154;    int wqTCzFCsdDGrIWd24347289 = -279704142;    int wqTCzFCsdDGrIWd3772361 = -509732872;    int wqTCzFCsdDGrIWd91484442 = -855652837;    int wqTCzFCsdDGrIWd29153412 = -447842811;    int wqTCzFCsdDGrIWd34662773 = -959771291;    int wqTCzFCsdDGrIWd73911756 = -733421823;    int wqTCzFCsdDGrIWd27887550 = -969330096;    int wqTCzFCsdDGrIWd29648804 = -455008539;    int wqTCzFCsdDGrIWd58240389 = -991426085;    int wqTCzFCsdDGrIWd73199667 = -404500084;    int wqTCzFCsdDGrIWd18826195 = 80409931;    int wqTCzFCsdDGrIWd36774082 = -823263907;    int wqTCzFCsdDGrIWd59535812 = -78049488;    int wqTCzFCsdDGrIWd39952436 = -699644443;    int wqTCzFCsdDGrIWd90082792 = -308660068;    int wqTCzFCsdDGrIWd37163101 = -373331107;    int wqTCzFCsdDGrIWd94915437 = -812364191;    int wqTCzFCsdDGrIWd57763951 = -39479156;    int wqTCzFCsdDGrIWd24904617 = -189474552;    int wqTCzFCsdDGrIWd93172723 = -216081515;    int wqTCzFCsdDGrIWd10385459 = -424722937;    int wqTCzFCsdDGrIWd15709583 = -45844976;    int wqTCzFCsdDGrIWd24681259 = -874384163;    int wqTCzFCsdDGrIWd56441505 = -631787971;    int wqTCzFCsdDGrIWd55294037 = -641063387;    int wqTCzFCsdDGrIWd79043694 = 17095424;    int wqTCzFCsdDGrIWd81229301 = -616851334;    int wqTCzFCsdDGrIWd41418502 = -46264834;    int wqTCzFCsdDGrIWd71638787 = -816947929;    int wqTCzFCsdDGrIWd68367258 = -276912383;    int wqTCzFCsdDGrIWd49573279 = -841084429;    int wqTCzFCsdDGrIWd70252293 = 45008701;    int wqTCzFCsdDGrIWd93515809 = -725071530;    int wqTCzFCsdDGrIWd17831852 = 40499952;    int wqTCzFCsdDGrIWd7558161 = -108198303;    int wqTCzFCsdDGrIWd68271646 = -469797191;    int wqTCzFCsdDGrIWd39871409 = -751509049;    int wqTCzFCsdDGrIWd66971557 = -788633466;    int wqTCzFCsdDGrIWd63339916 = -262639371;    int wqTCzFCsdDGrIWd472988 = 11500810;    int wqTCzFCsdDGrIWd56953267 = -454698122;    int wqTCzFCsdDGrIWd76974303 = -485955616;    int wqTCzFCsdDGrIWd5290961 = -386887945;    int wqTCzFCsdDGrIWd61657075 = -162372186;    int wqTCzFCsdDGrIWd5212251 = -992284931;    int wqTCzFCsdDGrIWd8739714 = -195750104;    int wqTCzFCsdDGrIWd86273699 = -768837327;    int wqTCzFCsdDGrIWd45516745 = -276121544;    int wqTCzFCsdDGrIWd72007254 = -506410720;    int wqTCzFCsdDGrIWd948848 = -440973043;    int wqTCzFCsdDGrIWd52178260 = -412309409;    int wqTCzFCsdDGrIWd97212207 = -651913698;    int wqTCzFCsdDGrIWd73976307 = -540929314;    int wqTCzFCsdDGrIWd36998903 = -265580931;    int wqTCzFCsdDGrIWd87547512 = -904985232;     wqTCzFCsdDGrIWd87959412 = wqTCzFCsdDGrIWd60461518;     wqTCzFCsdDGrIWd60461518 = wqTCzFCsdDGrIWd75300635;     wqTCzFCsdDGrIWd75300635 = wqTCzFCsdDGrIWd71857886;     wqTCzFCsdDGrIWd71857886 = wqTCzFCsdDGrIWd62126799;     wqTCzFCsdDGrIWd62126799 = wqTCzFCsdDGrIWd68518952;     wqTCzFCsdDGrIWd68518952 = wqTCzFCsdDGrIWd91040405;     wqTCzFCsdDGrIWd91040405 = wqTCzFCsdDGrIWd55766641;     wqTCzFCsdDGrIWd55766641 = wqTCzFCsdDGrIWd99289360;     wqTCzFCsdDGrIWd99289360 = wqTCzFCsdDGrIWd78013540;     wqTCzFCsdDGrIWd78013540 = wqTCzFCsdDGrIWd17008566;     wqTCzFCsdDGrIWd17008566 = wqTCzFCsdDGrIWd79240932;     wqTCzFCsdDGrIWd79240932 = wqTCzFCsdDGrIWd59538001;     wqTCzFCsdDGrIWd59538001 = wqTCzFCsdDGrIWd80479950;     wqTCzFCsdDGrIWd80479950 = wqTCzFCsdDGrIWd807834;     wqTCzFCsdDGrIWd807834 = wqTCzFCsdDGrIWd24853206;     wqTCzFCsdDGrIWd24853206 = wqTCzFCsdDGrIWd66698482;     wqTCzFCsdDGrIWd66698482 = wqTCzFCsdDGrIWd3508548;     wqTCzFCsdDGrIWd3508548 = wqTCzFCsdDGrIWd9750736;     wqTCzFCsdDGrIWd9750736 = wqTCzFCsdDGrIWd15460956;     wqTCzFCsdDGrIWd15460956 = wqTCzFCsdDGrIWd4549438;     wqTCzFCsdDGrIWd4549438 = wqTCzFCsdDGrIWd69133218;     wqTCzFCsdDGrIWd69133218 = wqTCzFCsdDGrIWd23687436;     wqTCzFCsdDGrIWd23687436 = wqTCzFCsdDGrIWd15764823;     wqTCzFCsdDGrIWd15764823 = wqTCzFCsdDGrIWd31905450;     wqTCzFCsdDGrIWd31905450 = wqTCzFCsdDGrIWd72044007;     wqTCzFCsdDGrIWd72044007 = wqTCzFCsdDGrIWd31355851;     wqTCzFCsdDGrIWd31355851 = wqTCzFCsdDGrIWd96124968;     wqTCzFCsdDGrIWd96124968 = wqTCzFCsdDGrIWd98002689;     wqTCzFCsdDGrIWd98002689 = wqTCzFCsdDGrIWd74384743;     wqTCzFCsdDGrIWd74384743 = wqTCzFCsdDGrIWd84840817;     wqTCzFCsdDGrIWd84840817 = wqTCzFCsdDGrIWd6623107;     wqTCzFCsdDGrIWd6623107 = wqTCzFCsdDGrIWd63531350;     wqTCzFCsdDGrIWd63531350 = wqTCzFCsdDGrIWd34856742;     wqTCzFCsdDGrIWd34856742 = wqTCzFCsdDGrIWd24038445;     wqTCzFCsdDGrIWd24038445 = wqTCzFCsdDGrIWd45513796;     wqTCzFCsdDGrIWd45513796 = wqTCzFCsdDGrIWd45809512;     wqTCzFCsdDGrIWd45809512 = wqTCzFCsdDGrIWd85469181;     wqTCzFCsdDGrIWd85469181 = wqTCzFCsdDGrIWd62090046;     wqTCzFCsdDGrIWd62090046 = wqTCzFCsdDGrIWd38111948;     wqTCzFCsdDGrIWd38111948 = wqTCzFCsdDGrIWd47093697;     wqTCzFCsdDGrIWd47093697 = wqTCzFCsdDGrIWd54976158;     wqTCzFCsdDGrIWd54976158 = wqTCzFCsdDGrIWd98880924;     wqTCzFCsdDGrIWd98880924 = wqTCzFCsdDGrIWd30171627;     wqTCzFCsdDGrIWd30171627 = wqTCzFCsdDGrIWd97932971;     wqTCzFCsdDGrIWd97932971 = wqTCzFCsdDGrIWd24347289;     wqTCzFCsdDGrIWd24347289 = wqTCzFCsdDGrIWd3772361;     wqTCzFCsdDGrIWd3772361 = wqTCzFCsdDGrIWd91484442;     wqTCzFCsdDGrIWd91484442 = wqTCzFCsdDGrIWd29153412;     wqTCzFCsdDGrIWd29153412 = wqTCzFCsdDGrIWd34662773;     wqTCzFCsdDGrIWd34662773 = wqTCzFCsdDGrIWd73911756;     wqTCzFCsdDGrIWd73911756 = wqTCzFCsdDGrIWd27887550;     wqTCzFCsdDGrIWd27887550 = wqTCzFCsdDGrIWd29648804;     wqTCzFCsdDGrIWd29648804 = wqTCzFCsdDGrIWd58240389;     wqTCzFCsdDGrIWd58240389 = wqTCzFCsdDGrIWd73199667;     wqTCzFCsdDGrIWd73199667 = wqTCzFCsdDGrIWd18826195;     wqTCzFCsdDGrIWd18826195 = wqTCzFCsdDGrIWd36774082;     wqTCzFCsdDGrIWd36774082 = wqTCzFCsdDGrIWd59535812;     wqTCzFCsdDGrIWd59535812 = wqTCzFCsdDGrIWd39952436;     wqTCzFCsdDGrIWd39952436 = wqTCzFCsdDGrIWd90082792;     wqTCzFCsdDGrIWd90082792 = wqTCzFCsdDGrIWd37163101;     wqTCzFCsdDGrIWd37163101 = wqTCzFCsdDGrIWd94915437;     wqTCzFCsdDGrIWd94915437 = wqTCzFCsdDGrIWd57763951;     wqTCzFCsdDGrIWd57763951 = wqTCzFCsdDGrIWd24904617;     wqTCzFCsdDGrIWd24904617 = wqTCzFCsdDGrIWd93172723;     wqTCzFCsdDGrIWd93172723 = wqTCzFCsdDGrIWd10385459;     wqTCzFCsdDGrIWd10385459 = wqTCzFCsdDGrIWd15709583;     wqTCzFCsdDGrIWd15709583 = wqTCzFCsdDGrIWd24681259;     wqTCzFCsdDGrIWd24681259 = wqTCzFCsdDGrIWd56441505;     wqTCzFCsdDGrIWd56441505 = wqTCzFCsdDGrIWd55294037;     wqTCzFCsdDGrIWd55294037 = wqTCzFCsdDGrIWd79043694;     wqTCzFCsdDGrIWd79043694 = wqTCzFCsdDGrIWd81229301;     wqTCzFCsdDGrIWd81229301 = wqTCzFCsdDGrIWd41418502;     wqTCzFCsdDGrIWd41418502 = wqTCzFCsdDGrIWd71638787;     wqTCzFCsdDGrIWd71638787 = wqTCzFCsdDGrIWd68367258;     wqTCzFCsdDGrIWd68367258 = wqTCzFCsdDGrIWd49573279;     wqTCzFCsdDGrIWd49573279 = wqTCzFCsdDGrIWd70252293;     wqTCzFCsdDGrIWd70252293 = wqTCzFCsdDGrIWd93515809;     wqTCzFCsdDGrIWd93515809 = wqTCzFCsdDGrIWd17831852;     wqTCzFCsdDGrIWd17831852 = wqTCzFCsdDGrIWd7558161;     wqTCzFCsdDGrIWd7558161 = wqTCzFCsdDGrIWd68271646;     wqTCzFCsdDGrIWd68271646 = wqTCzFCsdDGrIWd39871409;     wqTCzFCsdDGrIWd39871409 = wqTCzFCsdDGrIWd66971557;     wqTCzFCsdDGrIWd66971557 = wqTCzFCsdDGrIWd63339916;     wqTCzFCsdDGrIWd63339916 = wqTCzFCsdDGrIWd472988;     wqTCzFCsdDGrIWd472988 = wqTCzFCsdDGrIWd56953267;     wqTCzFCsdDGrIWd56953267 = wqTCzFCsdDGrIWd76974303;     wqTCzFCsdDGrIWd76974303 = wqTCzFCsdDGrIWd5290961;     wqTCzFCsdDGrIWd5290961 = wqTCzFCsdDGrIWd61657075;     wqTCzFCsdDGrIWd61657075 = wqTCzFCsdDGrIWd5212251;     wqTCzFCsdDGrIWd5212251 = wqTCzFCsdDGrIWd8739714;     wqTCzFCsdDGrIWd8739714 = wqTCzFCsdDGrIWd86273699;     wqTCzFCsdDGrIWd86273699 = wqTCzFCsdDGrIWd45516745;     wqTCzFCsdDGrIWd45516745 = wqTCzFCsdDGrIWd72007254;     wqTCzFCsdDGrIWd72007254 = wqTCzFCsdDGrIWd948848;     wqTCzFCsdDGrIWd948848 = wqTCzFCsdDGrIWd52178260;     wqTCzFCsdDGrIWd52178260 = wqTCzFCsdDGrIWd97212207;     wqTCzFCsdDGrIWd97212207 = wqTCzFCsdDGrIWd73976307;     wqTCzFCsdDGrIWd73976307 = wqTCzFCsdDGrIWd36998903;     wqTCzFCsdDGrIWd36998903 = wqTCzFCsdDGrIWd87547512;     wqTCzFCsdDGrIWd87547512 = wqTCzFCsdDGrIWd87959412;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void IBkymsBrueaqaVIg84407848() {     int kyPiAkRGbtkNSid87952941 = -777509374;    int kyPiAkRGbtkNSid99033834 = -172593006;    int kyPiAkRGbtkNSid76613847 = -571656303;    int kyPiAkRGbtkNSid56937747 = -8087717;    int kyPiAkRGbtkNSid70966901 = -856545452;    int kyPiAkRGbtkNSid3324385 = -779925804;    int kyPiAkRGbtkNSid59587741 = -853376934;    int kyPiAkRGbtkNSid24980692 = -66067134;    int kyPiAkRGbtkNSid82029524 = -825041635;    int kyPiAkRGbtkNSid74653053 = -376916603;    int kyPiAkRGbtkNSid37329171 = -425159163;    int kyPiAkRGbtkNSid93532609 = -259637367;    int kyPiAkRGbtkNSid91390914 = -562914906;    int kyPiAkRGbtkNSid34324170 = -988816021;    int kyPiAkRGbtkNSid45979942 = -722790249;    int kyPiAkRGbtkNSid99417753 = -262587040;    int kyPiAkRGbtkNSid22551242 = -512231890;    int kyPiAkRGbtkNSid26509081 = -358407244;    int kyPiAkRGbtkNSid30179005 = -249234344;    int kyPiAkRGbtkNSid87534718 = -113610432;    int kyPiAkRGbtkNSid84218861 = -342943656;    int kyPiAkRGbtkNSid47429484 = -295387047;    int kyPiAkRGbtkNSid60565832 = -765691817;    int kyPiAkRGbtkNSid59414931 = -317398724;    int kyPiAkRGbtkNSid60348139 = -914596297;    int kyPiAkRGbtkNSid88618084 = -987735991;    int kyPiAkRGbtkNSid1577433 = -907961598;    int kyPiAkRGbtkNSid31528493 = -845443264;    int kyPiAkRGbtkNSid10681137 = -868480976;    int kyPiAkRGbtkNSid50474613 = -497352387;    int kyPiAkRGbtkNSid18253496 = -811121452;    int kyPiAkRGbtkNSid91346964 = -332785916;    int kyPiAkRGbtkNSid48983773 = -580258245;    int kyPiAkRGbtkNSid35139579 = -543554002;    int kyPiAkRGbtkNSid6396483 = -171188924;    int kyPiAkRGbtkNSid88341985 = -98779756;    int kyPiAkRGbtkNSid57750880 = 53459807;    int kyPiAkRGbtkNSid55386861 = 28971334;    int kyPiAkRGbtkNSid97403642 = -527752205;    int kyPiAkRGbtkNSid9984841 = -523735835;    int kyPiAkRGbtkNSid34949365 = -312617497;    int kyPiAkRGbtkNSid40673820 = -985184936;    int kyPiAkRGbtkNSid59164284 = -289534120;    int kyPiAkRGbtkNSid89403524 = -138626889;    int kyPiAkRGbtkNSid81951023 = -850237556;    int kyPiAkRGbtkNSid64775257 = -91748813;    int kyPiAkRGbtkNSid31470154 = -991013494;    int kyPiAkRGbtkNSid66868023 = -364414193;    int kyPiAkRGbtkNSid47811801 = -11606672;    int kyPiAkRGbtkNSid54708196 = -546796177;    int kyPiAkRGbtkNSid87171126 = -311976831;    int kyPiAkRGbtkNSid42434808 = -251475844;    int kyPiAkRGbtkNSid5097153 = -347062547;    int kyPiAkRGbtkNSid68490660 = -423345936;    int kyPiAkRGbtkNSid188988 = -161399362;    int kyPiAkRGbtkNSid40523457 = -382122328;    int kyPiAkRGbtkNSid38468002 = -406901190;    int kyPiAkRGbtkNSid17198916 = -154257579;    int kyPiAkRGbtkNSid96589607 = -93491420;    int kyPiAkRGbtkNSid82348817 = -868809462;    int kyPiAkRGbtkNSid1746952 = -871964206;    int kyPiAkRGbtkNSid28059248 = 92066330;    int kyPiAkRGbtkNSid14299555 = -197586158;    int kyPiAkRGbtkNSid31554911 = -227689248;    int kyPiAkRGbtkNSid56399558 = -565795151;    int kyPiAkRGbtkNSid45982207 = 7626752;    int kyPiAkRGbtkNSid44548836 = -679379122;    int kyPiAkRGbtkNSid56251336 = 80639096;    int kyPiAkRGbtkNSid27927687 = -717627097;    int kyPiAkRGbtkNSid57637957 = -524010493;    int kyPiAkRGbtkNSid41666874 = -216046848;    int kyPiAkRGbtkNSid67164381 = -441203225;    int kyPiAkRGbtkNSid29105438 = -830655040;    int kyPiAkRGbtkNSid20194165 = -725498509;    int kyPiAkRGbtkNSid52585354 = -800992936;    int kyPiAkRGbtkNSid43545042 = -357758721;    int kyPiAkRGbtkNSid88265199 = 94147072;    int kyPiAkRGbtkNSid71162308 = -527064929;    int kyPiAkRGbtkNSid77463907 = -467161169;    int kyPiAkRGbtkNSid95572882 = -722847485;    int kyPiAkRGbtkNSid57147931 = -996722497;    int kyPiAkRGbtkNSid34709410 = -443547406;    int kyPiAkRGbtkNSid83716692 = -733836593;    int kyPiAkRGbtkNSid55972940 = -221684800;    int kyPiAkRGbtkNSid63303487 = -85375557;    int kyPiAkRGbtkNSid75818688 = -459645608;    int kyPiAkRGbtkNSid86249812 = -985723370;    int kyPiAkRGbtkNSid80493113 = -56912310;    int kyPiAkRGbtkNSid34950592 = -282154641;    int kyPiAkRGbtkNSid65873025 = -789066597;    int kyPiAkRGbtkNSid49873984 = -691878567;    int kyPiAkRGbtkNSid40551964 = -792282614;    int kyPiAkRGbtkNSid58797253 = -877537246;    int kyPiAkRGbtkNSid15054826 = -658942743;    int kyPiAkRGbtkNSid8237890 = -651771630;    int kyPiAkRGbtkNSid6890117 = -304683828;    int kyPiAkRGbtkNSid26374265 = -687598778;    int kyPiAkRGbtkNSid27609374 = 38155128;    int kyPiAkRGbtkNSid33003967 = -572831738;    int kyPiAkRGbtkNSid35968817 = -777509374;     kyPiAkRGbtkNSid87952941 = kyPiAkRGbtkNSid99033834;     kyPiAkRGbtkNSid99033834 = kyPiAkRGbtkNSid76613847;     kyPiAkRGbtkNSid76613847 = kyPiAkRGbtkNSid56937747;     kyPiAkRGbtkNSid56937747 = kyPiAkRGbtkNSid70966901;     kyPiAkRGbtkNSid70966901 = kyPiAkRGbtkNSid3324385;     kyPiAkRGbtkNSid3324385 = kyPiAkRGbtkNSid59587741;     kyPiAkRGbtkNSid59587741 = kyPiAkRGbtkNSid24980692;     kyPiAkRGbtkNSid24980692 = kyPiAkRGbtkNSid82029524;     kyPiAkRGbtkNSid82029524 = kyPiAkRGbtkNSid74653053;     kyPiAkRGbtkNSid74653053 = kyPiAkRGbtkNSid37329171;     kyPiAkRGbtkNSid37329171 = kyPiAkRGbtkNSid93532609;     kyPiAkRGbtkNSid93532609 = kyPiAkRGbtkNSid91390914;     kyPiAkRGbtkNSid91390914 = kyPiAkRGbtkNSid34324170;     kyPiAkRGbtkNSid34324170 = kyPiAkRGbtkNSid45979942;     kyPiAkRGbtkNSid45979942 = kyPiAkRGbtkNSid99417753;     kyPiAkRGbtkNSid99417753 = kyPiAkRGbtkNSid22551242;     kyPiAkRGbtkNSid22551242 = kyPiAkRGbtkNSid26509081;     kyPiAkRGbtkNSid26509081 = kyPiAkRGbtkNSid30179005;     kyPiAkRGbtkNSid30179005 = kyPiAkRGbtkNSid87534718;     kyPiAkRGbtkNSid87534718 = kyPiAkRGbtkNSid84218861;     kyPiAkRGbtkNSid84218861 = kyPiAkRGbtkNSid47429484;     kyPiAkRGbtkNSid47429484 = kyPiAkRGbtkNSid60565832;     kyPiAkRGbtkNSid60565832 = kyPiAkRGbtkNSid59414931;     kyPiAkRGbtkNSid59414931 = kyPiAkRGbtkNSid60348139;     kyPiAkRGbtkNSid60348139 = kyPiAkRGbtkNSid88618084;     kyPiAkRGbtkNSid88618084 = kyPiAkRGbtkNSid1577433;     kyPiAkRGbtkNSid1577433 = kyPiAkRGbtkNSid31528493;     kyPiAkRGbtkNSid31528493 = kyPiAkRGbtkNSid10681137;     kyPiAkRGbtkNSid10681137 = kyPiAkRGbtkNSid50474613;     kyPiAkRGbtkNSid50474613 = kyPiAkRGbtkNSid18253496;     kyPiAkRGbtkNSid18253496 = kyPiAkRGbtkNSid91346964;     kyPiAkRGbtkNSid91346964 = kyPiAkRGbtkNSid48983773;     kyPiAkRGbtkNSid48983773 = kyPiAkRGbtkNSid35139579;     kyPiAkRGbtkNSid35139579 = kyPiAkRGbtkNSid6396483;     kyPiAkRGbtkNSid6396483 = kyPiAkRGbtkNSid88341985;     kyPiAkRGbtkNSid88341985 = kyPiAkRGbtkNSid57750880;     kyPiAkRGbtkNSid57750880 = kyPiAkRGbtkNSid55386861;     kyPiAkRGbtkNSid55386861 = kyPiAkRGbtkNSid97403642;     kyPiAkRGbtkNSid97403642 = kyPiAkRGbtkNSid9984841;     kyPiAkRGbtkNSid9984841 = kyPiAkRGbtkNSid34949365;     kyPiAkRGbtkNSid34949365 = kyPiAkRGbtkNSid40673820;     kyPiAkRGbtkNSid40673820 = kyPiAkRGbtkNSid59164284;     kyPiAkRGbtkNSid59164284 = kyPiAkRGbtkNSid89403524;     kyPiAkRGbtkNSid89403524 = kyPiAkRGbtkNSid81951023;     kyPiAkRGbtkNSid81951023 = kyPiAkRGbtkNSid64775257;     kyPiAkRGbtkNSid64775257 = kyPiAkRGbtkNSid31470154;     kyPiAkRGbtkNSid31470154 = kyPiAkRGbtkNSid66868023;     kyPiAkRGbtkNSid66868023 = kyPiAkRGbtkNSid47811801;     kyPiAkRGbtkNSid47811801 = kyPiAkRGbtkNSid54708196;     kyPiAkRGbtkNSid54708196 = kyPiAkRGbtkNSid87171126;     kyPiAkRGbtkNSid87171126 = kyPiAkRGbtkNSid42434808;     kyPiAkRGbtkNSid42434808 = kyPiAkRGbtkNSid5097153;     kyPiAkRGbtkNSid5097153 = kyPiAkRGbtkNSid68490660;     kyPiAkRGbtkNSid68490660 = kyPiAkRGbtkNSid188988;     kyPiAkRGbtkNSid188988 = kyPiAkRGbtkNSid40523457;     kyPiAkRGbtkNSid40523457 = kyPiAkRGbtkNSid38468002;     kyPiAkRGbtkNSid38468002 = kyPiAkRGbtkNSid17198916;     kyPiAkRGbtkNSid17198916 = kyPiAkRGbtkNSid96589607;     kyPiAkRGbtkNSid96589607 = kyPiAkRGbtkNSid82348817;     kyPiAkRGbtkNSid82348817 = kyPiAkRGbtkNSid1746952;     kyPiAkRGbtkNSid1746952 = kyPiAkRGbtkNSid28059248;     kyPiAkRGbtkNSid28059248 = kyPiAkRGbtkNSid14299555;     kyPiAkRGbtkNSid14299555 = kyPiAkRGbtkNSid31554911;     kyPiAkRGbtkNSid31554911 = kyPiAkRGbtkNSid56399558;     kyPiAkRGbtkNSid56399558 = kyPiAkRGbtkNSid45982207;     kyPiAkRGbtkNSid45982207 = kyPiAkRGbtkNSid44548836;     kyPiAkRGbtkNSid44548836 = kyPiAkRGbtkNSid56251336;     kyPiAkRGbtkNSid56251336 = kyPiAkRGbtkNSid27927687;     kyPiAkRGbtkNSid27927687 = kyPiAkRGbtkNSid57637957;     kyPiAkRGbtkNSid57637957 = kyPiAkRGbtkNSid41666874;     kyPiAkRGbtkNSid41666874 = kyPiAkRGbtkNSid67164381;     kyPiAkRGbtkNSid67164381 = kyPiAkRGbtkNSid29105438;     kyPiAkRGbtkNSid29105438 = kyPiAkRGbtkNSid20194165;     kyPiAkRGbtkNSid20194165 = kyPiAkRGbtkNSid52585354;     kyPiAkRGbtkNSid52585354 = kyPiAkRGbtkNSid43545042;     kyPiAkRGbtkNSid43545042 = kyPiAkRGbtkNSid88265199;     kyPiAkRGbtkNSid88265199 = kyPiAkRGbtkNSid71162308;     kyPiAkRGbtkNSid71162308 = kyPiAkRGbtkNSid77463907;     kyPiAkRGbtkNSid77463907 = kyPiAkRGbtkNSid95572882;     kyPiAkRGbtkNSid95572882 = kyPiAkRGbtkNSid57147931;     kyPiAkRGbtkNSid57147931 = kyPiAkRGbtkNSid34709410;     kyPiAkRGbtkNSid34709410 = kyPiAkRGbtkNSid83716692;     kyPiAkRGbtkNSid83716692 = kyPiAkRGbtkNSid55972940;     kyPiAkRGbtkNSid55972940 = kyPiAkRGbtkNSid63303487;     kyPiAkRGbtkNSid63303487 = kyPiAkRGbtkNSid75818688;     kyPiAkRGbtkNSid75818688 = kyPiAkRGbtkNSid86249812;     kyPiAkRGbtkNSid86249812 = kyPiAkRGbtkNSid80493113;     kyPiAkRGbtkNSid80493113 = kyPiAkRGbtkNSid34950592;     kyPiAkRGbtkNSid34950592 = kyPiAkRGbtkNSid65873025;     kyPiAkRGbtkNSid65873025 = kyPiAkRGbtkNSid49873984;     kyPiAkRGbtkNSid49873984 = kyPiAkRGbtkNSid40551964;     kyPiAkRGbtkNSid40551964 = kyPiAkRGbtkNSid58797253;     kyPiAkRGbtkNSid58797253 = kyPiAkRGbtkNSid15054826;     kyPiAkRGbtkNSid15054826 = kyPiAkRGbtkNSid8237890;     kyPiAkRGbtkNSid8237890 = kyPiAkRGbtkNSid6890117;     kyPiAkRGbtkNSid6890117 = kyPiAkRGbtkNSid26374265;     kyPiAkRGbtkNSid26374265 = kyPiAkRGbtkNSid27609374;     kyPiAkRGbtkNSid27609374 = kyPiAkRGbtkNSid33003967;     kyPiAkRGbtkNSid33003967 = kyPiAkRGbtkNSid35968817;     kyPiAkRGbtkNSid35968817 = kyPiAkRGbtkNSid87952941;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void wMeNWtdVhGGcHZgI38824062() {     int lYyIvwniAMWZJBk58604608 = -295658504;    int lYyIvwniAMWZJBk67707352 = -44236305;    int lYyIvwniAMWZJBk56582766 = 86049237;    int lYyIvwniAMWZJBk96869033 = -198226106;    int lYyIvwniAMWZJBk92417223 = -639951799;    int lYyIvwniAMWZJBk93324146 = -481506955;    int lYyIvwniAMWZJBk77966637 = -852152643;    int lYyIvwniAMWZJBk81823072 = -412884224;    int lYyIvwniAMWZJBk27894579 = -918388963;    int lYyIvwniAMWZJBk97619514 = -678011953;    int lYyIvwniAMWZJBk79578332 = -460061093;    int lYyIvwniAMWZJBk39079181 = -575401160;    int lYyIvwniAMWZJBk71218302 = -472451965;    int lYyIvwniAMWZJBk18171204 = -789844231;    int lYyIvwniAMWZJBk28699962 = -872540252;    int lYyIvwniAMWZJBk31946394 = -932570627;    int lYyIvwniAMWZJBk16404069 = -421417020;    int lYyIvwniAMWZJBk46802575 = -347573440;    int lYyIvwniAMWZJBk11886787 = -956422286;    int lYyIvwniAMWZJBk46321428 = -655976737;    int lYyIvwniAMWZJBk39354616 = -251622796;    int lYyIvwniAMWZJBk8825249 = -626672352;    int lYyIvwniAMWZJBk62509641 = -233534961;    int lYyIvwniAMWZJBk93264468 = 90200369;    int lYyIvwniAMWZJBk24472528 = -590500985;    int lYyIvwniAMWZJBk19215665 = 13979564;    int lYyIvwniAMWZJBk9838219 = -203139374;    int lYyIvwniAMWZJBk39310719 = -250567032;    int lYyIvwniAMWZJBk42823625 = -62839193;    int lYyIvwniAMWZJBk89646709 = 44252543;    int lYyIvwniAMWZJBk25325493 = -559555138;    int lYyIvwniAMWZJBk35858425 = -661083079;    int lYyIvwniAMWZJBk31929134 = -527219854;    int lYyIvwniAMWZJBk504554 = -107083687;    int lYyIvwniAMWZJBk84420719 = -18135704;    int lYyIvwniAMWZJBk48335245 = -178006304;    int lYyIvwniAMWZJBk1542366 = -135133400;    int lYyIvwniAMWZJBk8179350 = -855141773;    int lYyIvwniAMWZJBk75936931 = -483480841;    int lYyIvwniAMWZJBk5088139 = -41344198;    int lYyIvwniAMWZJBk95022648 = -696636376;    int lYyIvwniAMWZJBk69156810 = -754574005;    int lYyIvwniAMWZJBk8131245 = -276288172;    int lYyIvwniAMWZJBk95911825 = -210601860;    int lYyIvwniAMWZJBk77905082 = -58883950;    int lYyIvwniAMWZJBk71181092 = -950085968;    int lYyIvwniAMWZJBk57601356 = -7083294;    int lYyIvwniAMWZJBk32482233 = -816544488;    int lYyIvwniAMWZJBk53253253 = 2629359;    int lYyIvwniAMWZJBk63135682 = -829805056;    int lYyIvwniAMWZJBk55360450 = -958548952;    int lYyIvwniAMWZJBk84306312 = -220695566;    int lYyIvwniAMWZJBk60172809 = -274860589;    int lYyIvwniAMWZJBk58717964 = 15772567;    int lYyIvwniAMWZJBk99469445 = -272340276;    int lYyIvwniAMWZJBk49779360 = -668986152;    int lYyIvwniAMWZJBk5197711 = -810701345;    int lYyIvwniAMWZJBk63318298 = 95848867;    int lYyIvwniAMWZJBk72396505 = -607725122;    int lYyIvwniAMWZJBk73201558 = -553931364;    int lYyIvwniAMWZJBk83485928 = -178367582;    int lYyIvwniAMWZJBk38655919 = -501585612;    int lYyIvwniAMWZJBk38999447 = -250045031;    int lYyIvwniAMWZJBk38247869 = -862641506;    int lYyIvwniAMWZJBk72294022 = -18456816;    int lYyIvwniAMWZJBk43719907 = -798978014;    int lYyIvwniAMWZJBk7150047 = 51818694;    int lYyIvwniAMWZJBk70713748 = -265368279;    int lYyIvwniAMWZJBk33750485 = -671708528;    int lYyIvwniAMWZJBk80364717 = -594533948;    int lYyIvwniAMWZJBk30404028 = -697437228;    int lYyIvwniAMWZJBk8224719 = -566275247;    int lYyIvwniAMWZJBk70865643 = -864092599;    int lYyIvwniAMWZJBk6798648 = -815078089;    int lYyIvwniAMWZJBk51298779 = -959340362;    int lYyIvwniAMWZJBk70197805 = -497048792;    int lYyIvwniAMWZJBk694005 = -250384181;    int lYyIvwniAMWZJBk66597816 = 77066898;    int lYyIvwniAMWZJBk15359386 = -850915681;    int lYyIvwniAMWZJBk53291436 = -640415018;    int lYyIvwniAMWZJBk61614308 = -978937143;    int lYyIvwniAMWZJBk77355985 = -386594886;    int lYyIvwniAMWZJBk86057465 = -153196391;    int lYyIvwniAMWZJBk79687942 = -233034138;    int lYyIvwniAMWZJBk34286259 = 2801494;    int lYyIvwniAMWZJBk41019180 = -238859572;    int lYyIvwniAMWZJBk75685615 = -286222490;    int lYyIvwniAMWZJBk73211169 = -442992422;    int lYyIvwniAMWZJBk1035108 = -834743411;    int lYyIvwniAMWZJBk34641360 = -349149552;    int lYyIvwniAMWZJBk43137534 = -367304960;    int lYyIvwniAMWZJBk38224068 = -130982268;    int lYyIvwniAMWZJBk35782844 = -147416652;    int lYyIvwniAMWZJBk2735374 = -929549478;    int lYyIvwniAMWZJBk21602211 = -862976617;    int lYyIvwniAMWZJBk56366730 = -95050764;    int lYyIvwniAMWZJBk30157363 = -404528974;    int lYyIvwniAMWZJBk69883375 = -413646666;    int lYyIvwniAMWZJBk23617803 = -92145044;    int lYyIvwniAMWZJBk34185175 = -295658504;     lYyIvwniAMWZJBk58604608 = lYyIvwniAMWZJBk67707352;     lYyIvwniAMWZJBk67707352 = lYyIvwniAMWZJBk56582766;     lYyIvwniAMWZJBk56582766 = lYyIvwniAMWZJBk96869033;     lYyIvwniAMWZJBk96869033 = lYyIvwniAMWZJBk92417223;     lYyIvwniAMWZJBk92417223 = lYyIvwniAMWZJBk93324146;     lYyIvwniAMWZJBk93324146 = lYyIvwniAMWZJBk77966637;     lYyIvwniAMWZJBk77966637 = lYyIvwniAMWZJBk81823072;     lYyIvwniAMWZJBk81823072 = lYyIvwniAMWZJBk27894579;     lYyIvwniAMWZJBk27894579 = lYyIvwniAMWZJBk97619514;     lYyIvwniAMWZJBk97619514 = lYyIvwniAMWZJBk79578332;     lYyIvwniAMWZJBk79578332 = lYyIvwniAMWZJBk39079181;     lYyIvwniAMWZJBk39079181 = lYyIvwniAMWZJBk71218302;     lYyIvwniAMWZJBk71218302 = lYyIvwniAMWZJBk18171204;     lYyIvwniAMWZJBk18171204 = lYyIvwniAMWZJBk28699962;     lYyIvwniAMWZJBk28699962 = lYyIvwniAMWZJBk31946394;     lYyIvwniAMWZJBk31946394 = lYyIvwniAMWZJBk16404069;     lYyIvwniAMWZJBk16404069 = lYyIvwniAMWZJBk46802575;     lYyIvwniAMWZJBk46802575 = lYyIvwniAMWZJBk11886787;     lYyIvwniAMWZJBk11886787 = lYyIvwniAMWZJBk46321428;     lYyIvwniAMWZJBk46321428 = lYyIvwniAMWZJBk39354616;     lYyIvwniAMWZJBk39354616 = lYyIvwniAMWZJBk8825249;     lYyIvwniAMWZJBk8825249 = lYyIvwniAMWZJBk62509641;     lYyIvwniAMWZJBk62509641 = lYyIvwniAMWZJBk93264468;     lYyIvwniAMWZJBk93264468 = lYyIvwniAMWZJBk24472528;     lYyIvwniAMWZJBk24472528 = lYyIvwniAMWZJBk19215665;     lYyIvwniAMWZJBk19215665 = lYyIvwniAMWZJBk9838219;     lYyIvwniAMWZJBk9838219 = lYyIvwniAMWZJBk39310719;     lYyIvwniAMWZJBk39310719 = lYyIvwniAMWZJBk42823625;     lYyIvwniAMWZJBk42823625 = lYyIvwniAMWZJBk89646709;     lYyIvwniAMWZJBk89646709 = lYyIvwniAMWZJBk25325493;     lYyIvwniAMWZJBk25325493 = lYyIvwniAMWZJBk35858425;     lYyIvwniAMWZJBk35858425 = lYyIvwniAMWZJBk31929134;     lYyIvwniAMWZJBk31929134 = lYyIvwniAMWZJBk504554;     lYyIvwniAMWZJBk504554 = lYyIvwniAMWZJBk84420719;     lYyIvwniAMWZJBk84420719 = lYyIvwniAMWZJBk48335245;     lYyIvwniAMWZJBk48335245 = lYyIvwniAMWZJBk1542366;     lYyIvwniAMWZJBk1542366 = lYyIvwniAMWZJBk8179350;     lYyIvwniAMWZJBk8179350 = lYyIvwniAMWZJBk75936931;     lYyIvwniAMWZJBk75936931 = lYyIvwniAMWZJBk5088139;     lYyIvwniAMWZJBk5088139 = lYyIvwniAMWZJBk95022648;     lYyIvwniAMWZJBk95022648 = lYyIvwniAMWZJBk69156810;     lYyIvwniAMWZJBk69156810 = lYyIvwniAMWZJBk8131245;     lYyIvwniAMWZJBk8131245 = lYyIvwniAMWZJBk95911825;     lYyIvwniAMWZJBk95911825 = lYyIvwniAMWZJBk77905082;     lYyIvwniAMWZJBk77905082 = lYyIvwniAMWZJBk71181092;     lYyIvwniAMWZJBk71181092 = lYyIvwniAMWZJBk57601356;     lYyIvwniAMWZJBk57601356 = lYyIvwniAMWZJBk32482233;     lYyIvwniAMWZJBk32482233 = lYyIvwniAMWZJBk53253253;     lYyIvwniAMWZJBk53253253 = lYyIvwniAMWZJBk63135682;     lYyIvwniAMWZJBk63135682 = lYyIvwniAMWZJBk55360450;     lYyIvwniAMWZJBk55360450 = lYyIvwniAMWZJBk84306312;     lYyIvwniAMWZJBk84306312 = lYyIvwniAMWZJBk60172809;     lYyIvwniAMWZJBk60172809 = lYyIvwniAMWZJBk58717964;     lYyIvwniAMWZJBk58717964 = lYyIvwniAMWZJBk99469445;     lYyIvwniAMWZJBk99469445 = lYyIvwniAMWZJBk49779360;     lYyIvwniAMWZJBk49779360 = lYyIvwniAMWZJBk5197711;     lYyIvwniAMWZJBk5197711 = lYyIvwniAMWZJBk63318298;     lYyIvwniAMWZJBk63318298 = lYyIvwniAMWZJBk72396505;     lYyIvwniAMWZJBk72396505 = lYyIvwniAMWZJBk73201558;     lYyIvwniAMWZJBk73201558 = lYyIvwniAMWZJBk83485928;     lYyIvwniAMWZJBk83485928 = lYyIvwniAMWZJBk38655919;     lYyIvwniAMWZJBk38655919 = lYyIvwniAMWZJBk38999447;     lYyIvwniAMWZJBk38999447 = lYyIvwniAMWZJBk38247869;     lYyIvwniAMWZJBk38247869 = lYyIvwniAMWZJBk72294022;     lYyIvwniAMWZJBk72294022 = lYyIvwniAMWZJBk43719907;     lYyIvwniAMWZJBk43719907 = lYyIvwniAMWZJBk7150047;     lYyIvwniAMWZJBk7150047 = lYyIvwniAMWZJBk70713748;     lYyIvwniAMWZJBk70713748 = lYyIvwniAMWZJBk33750485;     lYyIvwniAMWZJBk33750485 = lYyIvwniAMWZJBk80364717;     lYyIvwniAMWZJBk80364717 = lYyIvwniAMWZJBk30404028;     lYyIvwniAMWZJBk30404028 = lYyIvwniAMWZJBk8224719;     lYyIvwniAMWZJBk8224719 = lYyIvwniAMWZJBk70865643;     lYyIvwniAMWZJBk70865643 = lYyIvwniAMWZJBk6798648;     lYyIvwniAMWZJBk6798648 = lYyIvwniAMWZJBk51298779;     lYyIvwniAMWZJBk51298779 = lYyIvwniAMWZJBk70197805;     lYyIvwniAMWZJBk70197805 = lYyIvwniAMWZJBk694005;     lYyIvwniAMWZJBk694005 = lYyIvwniAMWZJBk66597816;     lYyIvwniAMWZJBk66597816 = lYyIvwniAMWZJBk15359386;     lYyIvwniAMWZJBk15359386 = lYyIvwniAMWZJBk53291436;     lYyIvwniAMWZJBk53291436 = lYyIvwniAMWZJBk61614308;     lYyIvwniAMWZJBk61614308 = lYyIvwniAMWZJBk77355985;     lYyIvwniAMWZJBk77355985 = lYyIvwniAMWZJBk86057465;     lYyIvwniAMWZJBk86057465 = lYyIvwniAMWZJBk79687942;     lYyIvwniAMWZJBk79687942 = lYyIvwniAMWZJBk34286259;     lYyIvwniAMWZJBk34286259 = lYyIvwniAMWZJBk41019180;     lYyIvwniAMWZJBk41019180 = lYyIvwniAMWZJBk75685615;     lYyIvwniAMWZJBk75685615 = lYyIvwniAMWZJBk73211169;     lYyIvwniAMWZJBk73211169 = lYyIvwniAMWZJBk1035108;     lYyIvwniAMWZJBk1035108 = lYyIvwniAMWZJBk34641360;     lYyIvwniAMWZJBk34641360 = lYyIvwniAMWZJBk43137534;     lYyIvwniAMWZJBk43137534 = lYyIvwniAMWZJBk38224068;     lYyIvwniAMWZJBk38224068 = lYyIvwniAMWZJBk35782844;     lYyIvwniAMWZJBk35782844 = lYyIvwniAMWZJBk2735374;     lYyIvwniAMWZJBk2735374 = lYyIvwniAMWZJBk21602211;     lYyIvwniAMWZJBk21602211 = lYyIvwniAMWZJBk56366730;     lYyIvwniAMWZJBk56366730 = lYyIvwniAMWZJBk30157363;     lYyIvwniAMWZJBk30157363 = lYyIvwniAMWZJBk69883375;     lYyIvwniAMWZJBk69883375 = lYyIvwniAMWZJBk23617803;     lYyIvwniAMWZJBk23617803 = lYyIvwniAMWZJBk34185175;     lYyIvwniAMWZJBk34185175 = lYyIvwniAMWZJBk58604608;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void AsNvHovbwcvutgeL45482806() {     int jGlKXhWGgULUNwU58598137 = -168182644;    int jGlKXhWGgULUNwU6279669 = -86831424;    int jGlKXhWGgULUNwU57895978 = -971079377;    int jGlKXhWGgULUNwU81948894 = -18766939;    int jGlKXhWGgULUNwU1257326 = -8307113;    int jGlKXhWGgULUNwU28129579 = -180939767;    int jGlKXhWGgULUNwU46513973 = -556689110;    int jGlKXhWGgULUNwU51037122 = -117061526;    int jGlKXhWGgULUNwU10634743 = -632035032;    int jGlKXhWGgULUNwU94259028 = -314818824;    int jGlKXhWGgULUNwU99898937 = -419533164;    int jGlKXhWGgULUNwU53370858 = -310879513;    int jGlKXhWGgULUNwU3071216 = -494110439;    int jGlKXhWGgULUNwU72015424 = -34997281;    int jGlKXhWGgULUNwU73872071 = -835253085;    int jGlKXhWGgULUNwU6510941 = -165366278;    int jGlKXhWGgULUNwU72256828 = -241031590;    int jGlKXhWGgULUNwU69803108 = -744645057;    int jGlKXhWGgULUNwU32315056 = -474404554;    int jGlKXhWGgULUNwU18395191 = -168001188;    int jGlKXhWGgULUNwU19024040 = 37910835;    int jGlKXhWGgULUNwU87121514 = -36664235;    int jGlKXhWGgULUNwU99388037 = -692492803;    int jGlKXhWGgULUNwU36914576 = -890720153;    int jGlKXhWGgULUNwU52915217 = 82805174;    int jGlKXhWGgULUNwU35789741 = -994226368;    int jGlKXhWGgULUNwU80059800 = -503939086;    int jGlKXhWGgULUNwU74714243 = -859534014;    int jGlKXhWGgULUNwU55502071 = -708909523;    int jGlKXhWGgULUNwU65736579 = -731178831;    int jGlKXhWGgULUNwU58738171 = -946648372;    int jGlKXhWGgULUNwU20582283 = 47095156;    int jGlKXhWGgULUNwU17381558 = -729164073;    int jGlKXhWGgULUNwU787392 = 16234581;    int jGlKXhWGgULUNwU66778757 = -277449631;    int jGlKXhWGgULUNwU91163434 = -257772069;    int jGlKXhWGgULUNwU13483734 = -234786778;    int jGlKXhWGgULUNwU78097029 = -850404449;    int jGlKXhWGgULUNwU11250529 = -96162270;    int jGlKXhWGgULUNwU76961032 = -750775884;    int jGlKXhWGgULUNwU82878316 = -784580274;    int jGlKXhWGgULUNwU54854471 = -948366108;    int jGlKXhWGgULUNwU68414604 = -835418428;    int jGlKXhWGgULUNwU55143723 = -867566304;    int jGlKXhWGgULUNwU61923134 = -532143353;    int jGlKXhWGgULUNwU11609061 = -762130657;    int jGlKXhWGgULUNwU85299149 = -488363916;    int jGlKXhWGgULUNwU7865815 = -325305845;    int jGlKXhWGgULUNwU71911642 = -661134527;    int jGlKXhWGgULUNwU83181106 = -416829937;    int jGlKXhWGgULUNwU68619820 = -537103959;    int jGlKXhWGgULUNwU98853569 = -602841318;    int jGlKXhWGgULUNwU35621158 = -166914594;    int jGlKXhWGgULUNwU68968235 = -516147284;    int jGlKXhWGgULUNwU26458767 = -29239559;    int jGlKXhWGgULUNwU71476622 = -31518409;    int jGlKXhWGgULUNwU6891632 = -394338622;    int jGlKXhWGgULUNwU20981402 = 19640776;    int jGlKXhWGgULUNwU29033677 = -1572114;    int jGlKXhWGgULUNwU65467584 = -14080745;    int jGlKXhWGgULUNwU48069779 = -677000681;    int jGlKXhWGgULUNwU71799729 = -697155097;    int jGlKXhWGgULUNwU95535051 = -408152003;    int jGlKXhWGgULUNwU44898163 = -900856201;    int jGlKXhWGgULUNwU35520857 = -368170453;    int jGlKXhWGgULUNwU79316655 = -366628321;    int jGlKXhWGgULUNwU35989301 = -581715441;    int jGlKXhWGgULUNwU2283825 = -410345020;    int jGlKXhWGgULUNwU5236667 = -757547651;    int jGlKXhWGgULUNwU82708637 = -477481017;    int jGlKXhWGgULUNwU93027207 = -930579500;    int jGlKXhWGgULUNwU94159798 = -390627141;    int jGlKXhWGgULUNwU58552579 = -548482788;    int jGlKXhWGgULUNwU55354024 = -723628670;    int jGlKXhWGgULUNwU35516875 = -383420915;    int jGlKXhWGgULUNwU64169568 = -13723058;    int jGlKXhWGgULUNwU18706910 = -201245808;    int jGlKXhWGgULUNwU44244314 = -824926500;    int jGlKXhWGgULUNwU74991441 = -258576801;    int jGlKXhWGgULUNwU41306157 = -155064170;    int jGlKXhWGgULUNwU50490592 = -405862453;    int jGlKXhWGgULUNwU72193986 = -78633242;    int jGlKXhWGgULUNwU2802602 = -98399487;    int jGlKXhWGgULUNwU72320965 = -192079587;    int jGlKXhWGgULUNwU97116759 = -94074873;    int jGlKXhWGgULUNwU59884601 = -243807054;    int jGlKXhWGgULUNwU84961124 = -785990250;    int jGlKXhWGgULUNwU48413322 = -113016790;    int jGlKXhWGgULUNwU74328625 = -954525861;    int jGlKXhWGgULUNwU95302134 = -145931223;    int jGlKXhWGgULUNwU84271802 = -863433448;    int jGlKXhWGgULUNwU92502331 = -154427555;    int jGlKXhWGgULUNwU49063352 = -748832336;    int jGlKXhWGgULUNwU45782944 = 17918475;    int jGlKXhWGgULUNwU28891254 = 26224796;    int jGlKXhWGgULUNwU11078587 = 12574822;    int jGlKXhWGgULUNwU59319420 = -440214105;    int jGlKXhWGgULUNwU23516441 = -934562227;    int jGlKXhWGgULUNwU19622867 = -399395851;    int jGlKXhWGgULUNwU82606479 = -168182644;     jGlKXhWGgULUNwU58598137 = jGlKXhWGgULUNwU6279669;     jGlKXhWGgULUNwU6279669 = jGlKXhWGgULUNwU57895978;     jGlKXhWGgULUNwU57895978 = jGlKXhWGgULUNwU81948894;     jGlKXhWGgULUNwU81948894 = jGlKXhWGgULUNwU1257326;     jGlKXhWGgULUNwU1257326 = jGlKXhWGgULUNwU28129579;     jGlKXhWGgULUNwU28129579 = jGlKXhWGgULUNwU46513973;     jGlKXhWGgULUNwU46513973 = jGlKXhWGgULUNwU51037122;     jGlKXhWGgULUNwU51037122 = jGlKXhWGgULUNwU10634743;     jGlKXhWGgULUNwU10634743 = jGlKXhWGgULUNwU94259028;     jGlKXhWGgULUNwU94259028 = jGlKXhWGgULUNwU99898937;     jGlKXhWGgULUNwU99898937 = jGlKXhWGgULUNwU53370858;     jGlKXhWGgULUNwU53370858 = jGlKXhWGgULUNwU3071216;     jGlKXhWGgULUNwU3071216 = jGlKXhWGgULUNwU72015424;     jGlKXhWGgULUNwU72015424 = jGlKXhWGgULUNwU73872071;     jGlKXhWGgULUNwU73872071 = jGlKXhWGgULUNwU6510941;     jGlKXhWGgULUNwU6510941 = jGlKXhWGgULUNwU72256828;     jGlKXhWGgULUNwU72256828 = jGlKXhWGgULUNwU69803108;     jGlKXhWGgULUNwU69803108 = jGlKXhWGgULUNwU32315056;     jGlKXhWGgULUNwU32315056 = jGlKXhWGgULUNwU18395191;     jGlKXhWGgULUNwU18395191 = jGlKXhWGgULUNwU19024040;     jGlKXhWGgULUNwU19024040 = jGlKXhWGgULUNwU87121514;     jGlKXhWGgULUNwU87121514 = jGlKXhWGgULUNwU99388037;     jGlKXhWGgULUNwU99388037 = jGlKXhWGgULUNwU36914576;     jGlKXhWGgULUNwU36914576 = jGlKXhWGgULUNwU52915217;     jGlKXhWGgULUNwU52915217 = jGlKXhWGgULUNwU35789741;     jGlKXhWGgULUNwU35789741 = jGlKXhWGgULUNwU80059800;     jGlKXhWGgULUNwU80059800 = jGlKXhWGgULUNwU74714243;     jGlKXhWGgULUNwU74714243 = jGlKXhWGgULUNwU55502071;     jGlKXhWGgULUNwU55502071 = jGlKXhWGgULUNwU65736579;     jGlKXhWGgULUNwU65736579 = jGlKXhWGgULUNwU58738171;     jGlKXhWGgULUNwU58738171 = jGlKXhWGgULUNwU20582283;     jGlKXhWGgULUNwU20582283 = jGlKXhWGgULUNwU17381558;     jGlKXhWGgULUNwU17381558 = jGlKXhWGgULUNwU787392;     jGlKXhWGgULUNwU787392 = jGlKXhWGgULUNwU66778757;     jGlKXhWGgULUNwU66778757 = jGlKXhWGgULUNwU91163434;     jGlKXhWGgULUNwU91163434 = jGlKXhWGgULUNwU13483734;     jGlKXhWGgULUNwU13483734 = jGlKXhWGgULUNwU78097029;     jGlKXhWGgULUNwU78097029 = jGlKXhWGgULUNwU11250529;     jGlKXhWGgULUNwU11250529 = jGlKXhWGgULUNwU76961032;     jGlKXhWGgULUNwU76961032 = jGlKXhWGgULUNwU82878316;     jGlKXhWGgULUNwU82878316 = jGlKXhWGgULUNwU54854471;     jGlKXhWGgULUNwU54854471 = jGlKXhWGgULUNwU68414604;     jGlKXhWGgULUNwU68414604 = jGlKXhWGgULUNwU55143723;     jGlKXhWGgULUNwU55143723 = jGlKXhWGgULUNwU61923134;     jGlKXhWGgULUNwU61923134 = jGlKXhWGgULUNwU11609061;     jGlKXhWGgULUNwU11609061 = jGlKXhWGgULUNwU85299149;     jGlKXhWGgULUNwU85299149 = jGlKXhWGgULUNwU7865815;     jGlKXhWGgULUNwU7865815 = jGlKXhWGgULUNwU71911642;     jGlKXhWGgULUNwU71911642 = jGlKXhWGgULUNwU83181106;     jGlKXhWGgULUNwU83181106 = jGlKXhWGgULUNwU68619820;     jGlKXhWGgULUNwU68619820 = jGlKXhWGgULUNwU98853569;     jGlKXhWGgULUNwU98853569 = jGlKXhWGgULUNwU35621158;     jGlKXhWGgULUNwU35621158 = jGlKXhWGgULUNwU68968235;     jGlKXhWGgULUNwU68968235 = jGlKXhWGgULUNwU26458767;     jGlKXhWGgULUNwU26458767 = jGlKXhWGgULUNwU71476622;     jGlKXhWGgULUNwU71476622 = jGlKXhWGgULUNwU6891632;     jGlKXhWGgULUNwU6891632 = jGlKXhWGgULUNwU20981402;     jGlKXhWGgULUNwU20981402 = jGlKXhWGgULUNwU29033677;     jGlKXhWGgULUNwU29033677 = jGlKXhWGgULUNwU65467584;     jGlKXhWGgULUNwU65467584 = jGlKXhWGgULUNwU48069779;     jGlKXhWGgULUNwU48069779 = jGlKXhWGgULUNwU71799729;     jGlKXhWGgULUNwU71799729 = jGlKXhWGgULUNwU95535051;     jGlKXhWGgULUNwU95535051 = jGlKXhWGgULUNwU44898163;     jGlKXhWGgULUNwU44898163 = jGlKXhWGgULUNwU35520857;     jGlKXhWGgULUNwU35520857 = jGlKXhWGgULUNwU79316655;     jGlKXhWGgULUNwU79316655 = jGlKXhWGgULUNwU35989301;     jGlKXhWGgULUNwU35989301 = jGlKXhWGgULUNwU2283825;     jGlKXhWGgULUNwU2283825 = jGlKXhWGgULUNwU5236667;     jGlKXhWGgULUNwU5236667 = jGlKXhWGgULUNwU82708637;     jGlKXhWGgULUNwU82708637 = jGlKXhWGgULUNwU93027207;     jGlKXhWGgULUNwU93027207 = jGlKXhWGgULUNwU94159798;     jGlKXhWGgULUNwU94159798 = jGlKXhWGgULUNwU58552579;     jGlKXhWGgULUNwU58552579 = jGlKXhWGgULUNwU55354024;     jGlKXhWGgULUNwU55354024 = jGlKXhWGgULUNwU35516875;     jGlKXhWGgULUNwU35516875 = jGlKXhWGgULUNwU64169568;     jGlKXhWGgULUNwU64169568 = jGlKXhWGgULUNwU18706910;     jGlKXhWGgULUNwU18706910 = jGlKXhWGgULUNwU44244314;     jGlKXhWGgULUNwU44244314 = jGlKXhWGgULUNwU74991441;     jGlKXhWGgULUNwU74991441 = jGlKXhWGgULUNwU41306157;     jGlKXhWGgULUNwU41306157 = jGlKXhWGgULUNwU50490592;     jGlKXhWGgULUNwU50490592 = jGlKXhWGgULUNwU72193986;     jGlKXhWGgULUNwU72193986 = jGlKXhWGgULUNwU2802602;     jGlKXhWGgULUNwU2802602 = jGlKXhWGgULUNwU72320965;     jGlKXhWGgULUNwU72320965 = jGlKXhWGgULUNwU97116759;     jGlKXhWGgULUNwU97116759 = jGlKXhWGgULUNwU59884601;     jGlKXhWGgULUNwU59884601 = jGlKXhWGgULUNwU84961124;     jGlKXhWGgULUNwU84961124 = jGlKXhWGgULUNwU48413322;     jGlKXhWGgULUNwU48413322 = jGlKXhWGgULUNwU74328625;     jGlKXhWGgULUNwU74328625 = jGlKXhWGgULUNwU95302134;     jGlKXhWGgULUNwU95302134 = jGlKXhWGgULUNwU84271802;     jGlKXhWGgULUNwU84271802 = jGlKXhWGgULUNwU92502331;     jGlKXhWGgULUNwU92502331 = jGlKXhWGgULUNwU49063352;     jGlKXhWGgULUNwU49063352 = jGlKXhWGgULUNwU45782944;     jGlKXhWGgULUNwU45782944 = jGlKXhWGgULUNwU28891254;     jGlKXhWGgULUNwU28891254 = jGlKXhWGgULUNwU11078587;     jGlKXhWGgULUNwU11078587 = jGlKXhWGgULUNwU59319420;     jGlKXhWGgULUNwU59319420 = jGlKXhWGgULUNwU23516441;     jGlKXhWGgULUNwU23516441 = jGlKXhWGgULUNwU19622867;     jGlKXhWGgULUNwU19622867 = jGlKXhWGgULUNwU82606479;     jGlKXhWGgULUNwU82606479 = jGlKXhWGgULUNwU58598137;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void xbvNXkBlBNqEvUpA99899019() {     int VuoNrZlbipuUlGa29249804 = -786331776;    int VuoNrZlbipuUlGa74953186 = 41525270;    int VuoNrZlbipuUlGa37864897 = -313373837;    int VuoNrZlbipuUlGa21880181 = -208905325;    int VuoNrZlbipuUlGa22707647 = -891713468;    int VuoNrZlbipuUlGa18129342 = -982520918;    int VuoNrZlbipuUlGa64892868 = -555464820;    int VuoNrZlbipuUlGa7879503 = -463878631;    int VuoNrZlbipuUlGa56499797 = -725382362;    int VuoNrZlbipuUlGa17225489 = -615914175;    int VuoNrZlbipuUlGa42148098 = -454435094;    int VuoNrZlbipuUlGa98917429 = -626643316;    int VuoNrZlbipuUlGa82898602 = -403647497;    int VuoNrZlbipuUlGa55862457 = -936025493;    int VuoNrZlbipuUlGa56592090 = -985003107;    int VuoNrZlbipuUlGa39039581 = -835349865;    int VuoNrZlbipuUlGa66109655 = -150216720;    int VuoNrZlbipuUlGa90096602 = -733811258;    int VuoNrZlbipuUlGa14022839 = -81592495;    int VuoNrZlbipuUlGa77181900 = -710367493;    int VuoNrZlbipuUlGa74159794 = -970768310;    int VuoNrZlbipuUlGa48517279 = -367949541;    int VuoNrZlbipuUlGa1331847 = -160335948;    int VuoNrZlbipuUlGa70764113 = -483121059;    int VuoNrZlbipuUlGa17039606 = -693099526;    int VuoNrZlbipuUlGa66387322 = 7489191;    int VuoNrZlbipuUlGa88320585 = -899116862;    int VuoNrZlbipuUlGa82496469 = -264657788;    int VuoNrZlbipuUlGa87644559 = 96732275;    int VuoNrZlbipuUlGa4908676 = -189573902;    int VuoNrZlbipuUlGa65810167 = -695082058;    int VuoNrZlbipuUlGa65093743 = -281202004;    int VuoNrZlbipuUlGa326919 = -676125679;    int VuoNrZlbipuUlGa66152366 = -647295103;    int VuoNrZlbipuUlGa44802994 = -124396408;    int VuoNrZlbipuUlGa51156693 = -336998598;    int VuoNrZlbipuUlGa57275219 = -423379986;    int VuoNrZlbipuUlGa30889518 = -634517560;    int VuoNrZlbipuUlGa89783817 = -51890894;    int VuoNrZlbipuUlGa72064330 = -268384247;    int VuoNrZlbipuUlGa42951600 = -68599153;    int VuoNrZlbipuUlGa83337462 = -717755157;    int VuoNrZlbipuUlGa17381565 = -822172478;    int VuoNrZlbipuUlGa61652024 = -939541275;    int VuoNrZlbipuUlGa57877193 = -840789746;    int VuoNrZlbipuUlGa18014895 = -520467793;    int VuoNrZlbipuUlGa11430353 = -604433715;    int VuoNrZlbipuUlGa73480024 = -777436140;    int VuoNrZlbipuUlGa77353094 = -646898472;    int VuoNrZlbipuUlGa91608592 = -699838821;    int VuoNrZlbipuUlGa36809145 = -83676080;    int VuoNrZlbipuUlGa40725074 = -572061036;    int VuoNrZlbipuUlGa90696814 = -94712639;    int VuoNrZlbipuUlGa59195540 = -77028781;    int VuoNrZlbipuUlGa25739225 = -140180468;    int VuoNrZlbipuUlGa80732524 = -318382235;    int VuoNrZlbipuUlGa73621339 = -798138782;    int VuoNrZlbipuUlGa67100784 = -830252778;    int VuoNrZlbipuUlGa4840575 = -515805800;    int VuoNrZlbipuUlGa56320324 = -799202660;    int VuoNrZlbipuUlGa29808756 = 16595943;    int VuoNrZlbipuUlGa82396399 = -190807033;    int VuoNrZlbipuUlGa20234944 = -460610906;    int VuoNrZlbipuUlGa51591122 = -435808461;    int VuoNrZlbipuUlGa51415321 = -920832118;    int VuoNrZlbipuUlGa77054355 = -73233091;    int VuoNrZlbipuUlGa98590511 = -950517637;    int VuoNrZlbipuUlGa16746237 = -756352395;    int VuoNrZlbipuUlGa11059464 = -711629085;    int VuoNrZlbipuUlGa5435397 = -548004509;    int VuoNrZlbipuUlGa81764361 = -311969879;    int VuoNrZlbipuUlGa35220137 = -515699160;    int VuoNrZlbipuUlGa312785 = -581920364;    int VuoNrZlbipuUlGa41958508 = -813208249;    int VuoNrZlbipuUlGa34230300 = -541768341;    int VuoNrZlbipuUlGa90822331 = -153013154;    int VuoNrZlbipuUlGa31135715 = -545777064;    int VuoNrZlbipuUlGa39679823 = -220794673;    int VuoNrZlbipuUlGa12886920 = -642331313;    int VuoNrZlbipuUlGa99024710 = -72631733;    int VuoNrZlbipuUlGa54956970 = -388077094;    int VuoNrZlbipuUlGa14840562 = -21680722;    int VuoNrZlbipuUlGa5143375 = -617759317;    int VuoNrZlbipuUlGa96035967 = -203428905;    int VuoNrZlbipuUlGa68099530 = -5897822;    int VuoNrZlbipuUlGa25085094 = -23021022;    int VuoNrZlbipuUlGa74396928 = -86489365;    int VuoNrZlbipuUlGa41131378 = -499096899;    int VuoNrZlbipuUlGa40413141 = -407114636;    int VuoNrZlbipuUlGa64070469 = -806014173;    int VuoNrZlbipuUlGa77535354 = -538859817;    int VuoNrZlbipuUlGa90174435 = -593127209;    int VuoNrZlbipuUlGa26048943 = -18711760;    int VuoNrZlbipuUlGa33463493 = -252688235;    int VuoNrZlbipuUlGa42255574 = -184980190;    int VuoNrZlbipuUlGa60555200 = -877792120;    int VuoNrZlbipuUlGa63102519 = -157144251;    int VuoNrZlbipuUlGa65790442 = -286364018;    int VuoNrZlbipuUlGa10236703 = 81290842;    int VuoNrZlbipuUlGa80822838 = -786331776;     VuoNrZlbipuUlGa29249804 = VuoNrZlbipuUlGa74953186;     VuoNrZlbipuUlGa74953186 = VuoNrZlbipuUlGa37864897;     VuoNrZlbipuUlGa37864897 = VuoNrZlbipuUlGa21880181;     VuoNrZlbipuUlGa21880181 = VuoNrZlbipuUlGa22707647;     VuoNrZlbipuUlGa22707647 = VuoNrZlbipuUlGa18129342;     VuoNrZlbipuUlGa18129342 = VuoNrZlbipuUlGa64892868;     VuoNrZlbipuUlGa64892868 = VuoNrZlbipuUlGa7879503;     VuoNrZlbipuUlGa7879503 = VuoNrZlbipuUlGa56499797;     VuoNrZlbipuUlGa56499797 = VuoNrZlbipuUlGa17225489;     VuoNrZlbipuUlGa17225489 = VuoNrZlbipuUlGa42148098;     VuoNrZlbipuUlGa42148098 = VuoNrZlbipuUlGa98917429;     VuoNrZlbipuUlGa98917429 = VuoNrZlbipuUlGa82898602;     VuoNrZlbipuUlGa82898602 = VuoNrZlbipuUlGa55862457;     VuoNrZlbipuUlGa55862457 = VuoNrZlbipuUlGa56592090;     VuoNrZlbipuUlGa56592090 = VuoNrZlbipuUlGa39039581;     VuoNrZlbipuUlGa39039581 = VuoNrZlbipuUlGa66109655;     VuoNrZlbipuUlGa66109655 = VuoNrZlbipuUlGa90096602;     VuoNrZlbipuUlGa90096602 = VuoNrZlbipuUlGa14022839;     VuoNrZlbipuUlGa14022839 = VuoNrZlbipuUlGa77181900;     VuoNrZlbipuUlGa77181900 = VuoNrZlbipuUlGa74159794;     VuoNrZlbipuUlGa74159794 = VuoNrZlbipuUlGa48517279;     VuoNrZlbipuUlGa48517279 = VuoNrZlbipuUlGa1331847;     VuoNrZlbipuUlGa1331847 = VuoNrZlbipuUlGa70764113;     VuoNrZlbipuUlGa70764113 = VuoNrZlbipuUlGa17039606;     VuoNrZlbipuUlGa17039606 = VuoNrZlbipuUlGa66387322;     VuoNrZlbipuUlGa66387322 = VuoNrZlbipuUlGa88320585;     VuoNrZlbipuUlGa88320585 = VuoNrZlbipuUlGa82496469;     VuoNrZlbipuUlGa82496469 = VuoNrZlbipuUlGa87644559;     VuoNrZlbipuUlGa87644559 = VuoNrZlbipuUlGa4908676;     VuoNrZlbipuUlGa4908676 = VuoNrZlbipuUlGa65810167;     VuoNrZlbipuUlGa65810167 = VuoNrZlbipuUlGa65093743;     VuoNrZlbipuUlGa65093743 = VuoNrZlbipuUlGa326919;     VuoNrZlbipuUlGa326919 = VuoNrZlbipuUlGa66152366;     VuoNrZlbipuUlGa66152366 = VuoNrZlbipuUlGa44802994;     VuoNrZlbipuUlGa44802994 = VuoNrZlbipuUlGa51156693;     VuoNrZlbipuUlGa51156693 = VuoNrZlbipuUlGa57275219;     VuoNrZlbipuUlGa57275219 = VuoNrZlbipuUlGa30889518;     VuoNrZlbipuUlGa30889518 = VuoNrZlbipuUlGa89783817;     VuoNrZlbipuUlGa89783817 = VuoNrZlbipuUlGa72064330;     VuoNrZlbipuUlGa72064330 = VuoNrZlbipuUlGa42951600;     VuoNrZlbipuUlGa42951600 = VuoNrZlbipuUlGa83337462;     VuoNrZlbipuUlGa83337462 = VuoNrZlbipuUlGa17381565;     VuoNrZlbipuUlGa17381565 = VuoNrZlbipuUlGa61652024;     VuoNrZlbipuUlGa61652024 = VuoNrZlbipuUlGa57877193;     VuoNrZlbipuUlGa57877193 = VuoNrZlbipuUlGa18014895;     VuoNrZlbipuUlGa18014895 = VuoNrZlbipuUlGa11430353;     VuoNrZlbipuUlGa11430353 = VuoNrZlbipuUlGa73480024;     VuoNrZlbipuUlGa73480024 = VuoNrZlbipuUlGa77353094;     VuoNrZlbipuUlGa77353094 = VuoNrZlbipuUlGa91608592;     VuoNrZlbipuUlGa91608592 = VuoNrZlbipuUlGa36809145;     VuoNrZlbipuUlGa36809145 = VuoNrZlbipuUlGa40725074;     VuoNrZlbipuUlGa40725074 = VuoNrZlbipuUlGa90696814;     VuoNrZlbipuUlGa90696814 = VuoNrZlbipuUlGa59195540;     VuoNrZlbipuUlGa59195540 = VuoNrZlbipuUlGa25739225;     VuoNrZlbipuUlGa25739225 = VuoNrZlbipuUlGa80732524;     VuoNrZlbipuUlGa80732524 = VuoNrZlbipuUlGa73621339;     VuoNrZlbipuUlGa73621339 = VuoNrZlbipuUlGa67100784;     VuoNrZlbipuUlGa67100784 = VuoNrZlbipuUlGa4840575;     VuoNrZlbipuUlGa4840575 = VuoNrZlbipuUlGa56320324;     VuoNrZlbipuUlGa56320324 = VuoNrZlbipuUlGa29808756;     VuoNrZlbipuUlGa29808756 = VuoNrZlbipuUlGa82396399;     VuoNrZlbipuUlGa82396399 = VuoNrZlbipuUlGa20234944;     VuoNrZlbipuUlGa20234944 = VuoNrZlbipuUlGa51591122;     VuoNrZlbipuUlGa51591122 = VuoNrZlbipuUlGa51415321;     VuoNrZlbipuUlGa51415321 = VuoNrZlbipuUlGa77054355;     VuoNrZlbipuUlGa77054355 = VuoNrZlbipuUlGa98590511;     VuoNrZlbipuUlGa98590511 = VuoNrZlbipuUlGa16746237;     VuoNrZlbipuUlGa16746237 = VuoNrZlbipuUlGa11059464;     VuoNrZlbipuUlGa11059464 = VuoNrZlbipuUlGa5435397;     VuoNrZlbipuUlGa5435397 = VuoNrZlbipuUlGa81764361;     VuoNrZlbipuUlGa81764361 = VuoNrZlbipuUlGa35220137;     VuoNrZlbipuUlGa35220137 = VuoNrZlbipuUlGa312785;     VuoNrZlbipuUlGa312785 = VuoNrZlbipuUlGa41958508;     VuoNrZlbipuUlGa41958508 = VuoNrZlbipuUlGa34230300;     VuoNrZlbipuUlGa34230300 = VuoNrZlbipuUlGa90822331;     VuoNrZlbipuUlGa90822331 = VuoNrZlbipuUlGa31135715;     VuoNrZlbipuUlGa31135715 = VuoNrZlbipuUlGa39679823;     VuoNrZlbipuUlGa39679823 = VuoNrZlbipuUlGa12886920;     VuoNrZlbipuUlGa12886920 = VuoNrZlbipuUlGa99024710;     VuoNrZlbipuUlGa99024710 = VuoNrZlbipuUlGa54956970;     VuoNrZlbipuUlGa54956970 = VuoNrZlbipuUlGa14840562;     VuoNrZlbipuUlGa14840562 = VuoNrZlbipuUlGa5143375;     VuoNrZlbipuUlGa5143375 = VuoNrZlbipuUlGa96035967;     VuoNrZlbipuUlGa96035967 = VuoNrZlbipuUlGa68099530;     VuoNrZlbipuUlGa68099530 = VuoNrZlbipuUlGa25085094;     VuoNrZlbipuUlGa25085094 = VuoNrZlbipuUlGa74396928;     VuoNrZlbipuUlGa74396928 = VuoNrZlbipuUlGa41131378;     VuoNrZlbipuUlGa41131378 = VuoNrZlbipuUlGa40413141;     VuoNrZlbipuUlGa40413141 = VuoNrZlbipuUlGa64070469;     VuoNrZlbipuUlGa64070469 = VuoNrZlbipuUlGa77535354;     VuoNrZlbipuUlGa77535354 = VuoNrZlbipuUlGa90174435;     VuoNrZlbipuUlGa90174435 = VuoNrZlbipuUlGa26048943;     VuoNrZlbipuUlGa26048943 = VuoNrZlbipuUlGa33463493;     VuoNrZlbipuUlGa33463493 = VuoNrZlbipuUlGa42255574;     VuoNrZlbipuUlGa42255574 = VuoNrZlbipuUlGa60555200;     VuoNrZlbipuUlGa60555200 = VuoNrZlbipuUlGa63102519;     VuoNrZlbipuUlGa63102519 = VuoNrZlbipuUlGa65790442;     VuoNrZlbipuUlGa65790442 = VuoNrZlbipuUlGa10236703;     VuoNrZlbipuUlGa10236703 = VuoNrZlbipuUlGa80822838;     VuoNrZlbipuUlGa80822838 = VuoNrZlbipuUlGa29249804;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void SJeYbRkrNuURPvaa6557765() {     int QNPOLdnuJxxwDmA29243333 = -658855915;    int QNPOLdnuJxxwDmA13525503 = -1069849;    int QNPOLdnuJxxwDmA39178109 = -270502450;    int QNPOLdnuJxxwDmA6960042 = -29446159;    int QNPOLdnuJxxwDmA31547749 = -260068781;    int QNPOLdnuJxxwDmA52934774 = -681953730;    int QNPOLdnuJxxwDmA33440205 = -260001287;    int QNPOLdnuJxxwDmA77093553 = -168055933;    int QNPOLdnuJxxwDmA39239961 = -439028431;    int QNPOLdnuJxxwDmA13865003 = -252721046;    int QNPOLdnuJxxwDmA62468703 = -413907165;    int QNPOLdnuJxxwDmA13209107 = -362121669;    int QNPOLdnuJxxwDmA14751517 = -425305972;    int QNPOLdnuJxxwDmA9706678 = -181178543;    int QNPOLdnuJxxwDmA1764200 = -947715941;    int QNPOLdnuJxxwDmA13604128 = -68145515;    int QNPOLdnuJxxwDmA21962414 = 30168710;    int QNPOLdnuJxxwDmA13097135 = -30882875;    int QNPOLdnuJxxwDmA34451108 = -699574763;    int QNPOLdnuJxxwDmA49255663 = -222391945;    int QNPOLdnuJxxwDmA53829219 = -681234679;    int QNPOLdnuJxxwDmA26813545 = -877941424;    int QNPOLdnuJxxwDmA38210243 = -619293790;    int QNPOLdnuJxxwDmA14414222 = -364041582;    int QNPOLdnuJxxwDmA45482295 = -19793367;    int QNPOLdnuJxxwDmA82961399 = 99283259;    int QNPOLdnuJxxwDmA58542167 = -99916574;    int QNPOLdnuJxxwDmA17899994 = -873624770;    int QNPOLdnuJxxwDmA323007 = -549338055;    int QNPOLdnuJxxwDmA80998545 = -965005275;    int QNPOLdnuJxxwDmA99222846 = 17824708;    int QNPOLdnuJxxwDmA49817600 = -673023768;    int QNPOLdnuJxxwDmA85779341 = -878069898;    int QNPOLdnuJxxwDmA66435203 = -523976836;    int QNPOLdnuJxxwDmA27161032 = -383710336;    int QNPOLdnuJxxwDmA93984882 = -416764363;    int QNPOLdnuJxxwDmA69216587 = -523033364;    int QNPOLdnuJxxwDmA807198 = -629780236;    int QNPOLdnuJxxwDmA25097414 = -764572323;    int QNPOLdnuJxxwDmA43937223 = -977815933;    int QNPOLdnuJxxwDmA30807268 = -156543051;    int QNPOLdnuJxxwDmA69035123 = -911547259;    int QNPOLdnuJxxwDmA77664924 = -281302734;    int QNPOLdnuJxxwDmA20883922 = -496505718;    int QNPOLdnuJxxwDmA41895246 = -214049149;    int QNPOLdnuJxxwDmA58442863 = -332512483;    int QNPOLdnuJxxwDmA39128145 = 14285663;    int QNPOLdnuJxxwDmA48863605 = -286197497;    int QNPOLdnuJxxwDmA96011484 = -210662358;    int QNPOLdnuJxxwDmA11654016 = -286863702;    int QNPOLdnuJxxwDmA50068514 = -762231087;    int QNPOLdnuJxxwDmA55272331 = -954206789;    int QNPOLdnuJxxwDmA66145163 = 13233355;    int QNPOLdnuJxxwDmA69445811 = -608948632;    int QNPOLdnuJxxwDmA52728545 = -997079751;    int QNPOLdnuJxxwDmA2429788 = -780914492;    int QNPOLdnuJxxwDmA75315260 = -381776059;    int QNPOLdnuJxxwDmA24763888 = -906460869;    int QNPOLdnuJxxwDmA61477746 = 90347208;    int QNPOLdnuJxxwDmA48586350 = -259352041;    int QNPOLdnuJxxwDmA94392606 = -482037156;    int QNPOLdnuJxxwDmA15540211 = -386376518;    int QNPOLdnuJxxwDmA76770547 = -618717878;    int QNPOLdnuJxxwDmA58241416 = -474023156;    int QNPOLdnuJxxwDmA14642157 = -170545755;    int QNPOLdnuJxxwDmA12651103 = -740883398;    int QNPOLdnuJxxwDmA27429765 = -484051771;    int QNPOLdnuJxxwDmA48316313 = -901329136;    int QNPOLdnuJxxwDmA82545646 = -797468208;    int QNPOLdnuJxxwDmA7779317 = -430951578;    int QNPOLdnuJxxwDmA44387541 = -545112152;    int QNPOLdnuJxxwDmA21155217 = -340051054;    int QNPOLdnuJxxwDmA87999721 = -266310553;    int QNPOLdnuJxxwDmA90513884 = -721758830;    int QNPOLdnuJxxwDmA18448396 = 34151106;    int QNPOLdnuJxxwDmA84794095 = -769687420;    int QNPOLdnuJxxwDmA49148621 = -496638691;    int QNPOLdnuJxxwDmA17326321 = -22788072;    int QNPOLdnuJxxwDmA72518976 = -49992433;    int QNPOLdnuJxxwDmA87039431 = -687280885;    int QNPOLdnuJxxwDmA43833254 = -915002404;    int QNPOLdnuJxxwDmA9678563 = -813719078;    int QNPOLdnuJxxwDmA21888510 = -562962413;    int QNPOLdnuJxxwDmA88668990 = -162474354;    int QNPOLdnuJxxwDmA30930031 = -102774189;    int QNPOLdnuJxxwDmA43950515 = -27968504;    int QNPOLdnuJxxwDmA83672437 = -586257124;    int QNPOLdnuJxxwDmA16333531 = -169121266;    int QNPOLdnuJxxwDmA13706658 = -526897085;    int QNPOLdnuJxxwDmA24731245 = -602795844;    int QNPOLdnuJxxwDmA18669623 = 65011696;    int QNPOLdnuJxxwDmA44452699 = -616572496;    int QNPOLdnuJxxwDmA39329451 = -620127444;    int QNPOLdnuJxxwDmA76511064 = -405220282;    int QNPOLdnuJxxwDmA49544617 = -395778778;    int QNPOLdnuJxxwDmA15267057 = -770166534;    int QNPOLdnuJxxwDmA92264576 = -192829382;    int QNPOLdnuJxxwDmA19423509 = -807279579;    int QNPOLdnuJxxwDmA6241766 = -225959964;    int QNPOLdnuJxxwDmA29244143 = -658855915;     QNPOLdnuJxxwDmA29243333 = QNPOLdnuJxxwDmA13525503;     QNPOLdnuJxxwDmA13525503 = QNPOLdnuJxxwDmA39178109;     QNPOLdnuJxxwDmA39178109 = QNPOLdnuJxxwDmA6960042;     QNPOLdnuJxxwDmA6960042 = QNPOLdnuJxxwDmA31547749;     QNPOLdnuJxxwDmA31547749 = QNPOLdnuJxxwDmA52934774;     QNPOLdnuJxxwDmA52934774 = QNPOLdnuJxxwDmA33440205;     QNPOLdnuJxxwDmA33440205 = QNPOLdnuJxxwDmA77093553;     QNPOLdnuJxxwDmA77093553 = QNPOLdnuJxxwDmA39239961;     QNPOLdnuJxxwDmA39239961 = QNPOLdnuJxxwDmA13865003;     QNPOLdnuJxxwDmA13865003 = QNPOLdnuJxxwDmA62468703;     QNPOLdnuJxxwDmA62468703 = QNPOLdnuJxxwDmA13209107;     QNPOLdnuJxxwDmA13209107 = QNPOLdnuJxxwDmA14751517;     QNPOLdnuJxxwDmA14751517 = QNPOLdnuJxxwDmA9706678;     QNPOLdnuJxxwDmA9706678 = QNPOLdnuJxxwDmA1764200;     QNPOLdnuJxxwDmA1764200 = QNPOLdnuJxxwDmA13604128;     QNPOLdnuJxxwDmA13604128 = QNPOLdnuJxxwDmA21962414;     QNPOLdnuJxxwDmA21962414 = QNPOLdnuJxxwDmA13097135;     QNPOLdnuJxxwDmA13097135 = QNPOLdnuJxxwDmA34451108;     QNPOLdnuJxxwDmA34451108 = QNPOLdnuJxxwDmA49255663;     QNPOLdnuJxxwDmA49255663 = QNPOLdnuJxxwDmA53829219;     QNPOLdnuJxxwDmA53829219 = QNPOLdnuJxxwDmA26813545;     QNPOLdnuJxxwDmA26813545 = QNPOLdnuJxxwDmA38210243;     QNPOLdnuJxxwDmA38210243 = QNPOLdnuJxxwDmA14414222;     QNPOLdnuJxxwDmA14414222 = QNPOLdnuJxxwDmA45482295;     QNPOLdnuJxxwDmA45482295 = QNPOLdnuJxxwDmA82961399;     QNPOLdnuJxxwDmA82961399 = QNPOLdnuJxxwDmA58542167;     QNPOLdnuJxxwDmA58542167 = QNPOLdnuJxxwDmA17899994;     QNPOLdnuJxxwDmA17899994 = QNPOLdnuJxxwDmA323007;     QNPOLdnuJxxwDmA323007 = QNPOLdnuJxxwDmA80998545;     QNPOLdnuJxxwDmA80998545 = QNPOLdnuJxxwDmA99222846;     QNPOLdnuJxxwDmA99222846 = QNPOLdnuJxxwDmA49817600;     QNPOLdnuJxxwDmA49817600 = QNPOLdnuJxxwDmA85779341;     QNPOLdnuJxxwDmA85779341 = QNPOLdnuJxxwDmA66435203;     QNPOLdnuJxxwDmA66435203 = QNPOLdnuJxxwDmA27161032;     QNPOLdnuJxxwDmA27161032 = QNPOLdnuJxxwDmA93984882;     QNPOLdnuJxxwDmA93984882 = QNPOLdnuJxxwDmA69216587;     QNPOLdnuJxxwDmA69216587 = QNPOLdnuJxxwDmA807198;     QNPOLdnuJxxwDmA807198 = QNPOLdnuJxxwDmA25097414;     QNPOLdnuJxxwDmA25097414 = QNPOLdnuJxxwDmA43937223;     QNPOLdnuJxxwDmA43937223 = QNPOLdnuJxxwDmA30807268;     QNPOLdnuJxxwDmA30807268 = QNPOLdnuJxxwDmA69035123;     QNPOLdnuJxxwDmA69035123 = QNPOLdnuJxxwDmA77664924;     QNPOLdnuJxxwDmA77664924 = QNPOLdnuJxxwDmA20883922;     QNPOLdnuJxxwDmA20883922 = QNPOLdnuJxxwDmA41895246;     QNPOLdnuJxxwDmA41895246 = QNPOLdnuJxxwDmA58442863;     QNPOLdnuJxxwDmA58442863 = QNPOLdnuJxxwDmA39128145;     QNPOLdnuJxxwDmA39128145 = QNPOLdnuJxxwDmA48863605;     QNPOLdnuJxxwDmA48863605 = QNPOLdnuJxxwDmA96011484;     QNPOLdnuJxxwDmA96011484 = QNPOLdnuJxxwDmA11654016;     QNPOLdnuJxxwDmA11654016 = QNPOLdnuJxxwDmA50068514;     QNPOLdnuJxxwDmA50068514 = QNPOLdnuJxxwDmA55272331;     QNPOLdnuJxxwDmA55272331 = QNPOLdnuJxxwDmA66145163;     QNPOLdnuJxxwDmA66145163 = QNPOLdnuJxxwDmA69445811;     QNPOLdnuJxxwDmA69445811 = QNPOLdnuJxxwDmA52728545;     QNPOLdnuJxxwDmA52728545 = QNPOLdnuJxxwDmA2429788;     QNPOLdnuJxxwDmA2429788 = QNPOLdnuJxxwDmA75315260;     QNPOLdnuJxxwDmA75315260 = QNPOLdnuJxxwDmA24763888;     QNPOLdnuJxxwDmA24763888 = QNPOLdnuJxxwDmA61477746;     QNPOLdnuJxxwDmA61477746 = QNPOLdnuJxxwDmA48586350;     QNPOLdnuJxxwDmA48586350 = QNPOLdnuJxxwDmA94392606;     QNPOLdnuJxxwDmA94392606 = QNPOLdnuJxxwDmA15540211;     QNPOLdnuJxxwDmA15540211 = QNPOLdnuJxxwDmA76770547;     QNPOLdnuJxxwDmA76770547 = QNPOLdnuJxxwDmA58241416;     QNPOLdnuJxxwDmA58241416 = QNPOLdnuJxxwDmA14642157;     QNPOLdnuJxxwDmA14642157 = QNPOLdnuJxxwDmA12651103;     QNPOLdnuJxxwDmA12651103 = QNPOLdnuJxxwDmA27429765;     QNPOLdnuJxxwDmA27429765 = QNPOLdnuJxxwDmA48316313;     QNPOLdnuJxxwDmA48316313 = QNPOLdnuJxxwDmA82545646;     QNPOLdnuJxxwDmA82545646 = QNPOLdnuJxxwDmA7779317;     QNPOLdnuJxxwDmA7779317 = QNPOLdnuJxxwDmA44387541;     QNPOLdnuJxxwDmA44387541 = QNPOLdnuJxxwDmA21155217;     QNPOLdnuJxxwDmA21155217 = QNPOLdnuJxxwDmA87999721;     QNPOLdnuJxxwDmA87999721 = QNPOLdnuJxxwDmA90513884;     QNPOLdnuJxxwDmA90513884 = QNPOLdnuJxxwDmA18448396;     QNPOLdnuJxxwDmA18448396 = QNPOLdnuJxxwDmA84794095;     QNPOLdnuJxxwDmA84794095 = QNPOLdnuJxxwDmA49148621;     QNPOLdnuJxxwDmA49148621 = QNPOLdnuJxxwDmA17326321;     QNPOLdnuJxxwDmA17326321 = QNPOLdnuJxxwDmA72518976;     QNPOLdnuJxxwDmA72518976 = QNPOLdnuJxxwDmA87039431;     QNPOLdnuJxxwDmA87039431 = QNPOLdnuJxxwDmA43833254;     QNPOLdnuJxxwDmA43833254 = QNPOLdnuJxxwDmA9678563;     QNPOLdnuJxxwDmA9678563 = QNPOLdnuJxxwDmA21888510;     QNPOLdnuJxxwDmA21888510 = QNPOLdnuJxxwDmA88668990;     QNPOLdnuJxxwDmA88668990 = QNPOLdnuJxxwDmA30930031;     QNPOLdnuJxxwDmA30930031 = QNPOLdnuJxxwDmA43950515;     QNPOLdnuJxxwDmA43950515 = QNPOLdnuJxxwDmA83672437;     QNPOLdnuJxxwDmA83672437 = QNPOLdnuJxxwDmA16333531;     QNPOLdnuJxxwDmA16333531 = QNPOLdnuJxxwDmA13706658;     QNPOLdnuJxxwDmA13706658 = QNPOLdnuJxxwDmA24731245;     QNPOLdnuJxxwDmA24731245 = QNPOLdnuJxxwDmA18669623;     QNPOLdnuJxxwDmA18669623 = QNPOLdnuJxxwDmA44452699;     QNPOLdnuJxxwDmA44452699 = QNPOLdnuJxxwDmA39329451;     QNPOLdnuJxxwDmA39329451 = QNPOLdnuJxxwDmA76511064;     QNPOLdnuJxxwDmA76511064 = QNPOLdnuJxxwDmA49544617;     QNPOLdnuJxxwDmA49544617 = QNPOLdnuJxxwDmA15267057;     QNPOLdnuJxxwDmA15267057 = QNPOLdnuJxxwDmA92264576;     QNPOLdnuJxxwDmA92264576 = QNPOLdnuJxxwDmA19423509;     QNPOLdnuJxxwDmA19423509 = QNPOLdnuJxxwDmA6241766;     QNPOLdnuJxxwDmA6241766 = QNPOLdnuJxxwDmA29244143;     QNPOLdnuJxxwDmA29244143 = QNPOLdnuJxxwDmA29243333;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void JgFKTUERHSRhXhba60973977() {     int MrxHJRgWVAnWHMQ99894999 = -177005048;    int MrxHJRgWVAnWHMQ82199019 = -972713154;    int MrxHJRgWVAnWHMQ19147029 = -712796910;    int MrxHJRgWVAnWHMQ46891328 = -219584545;    int MrxHJRgWVAnWHMQ52998070 = -43475137;    int MrxHJRgWVAnWHMQ42934537 = -383534881;    int MrxHJRgWVAnWHMQ51819100 = -258776997;    int MrxHJRgWVAnWHMQ33935934 = -514873038;    int MrxHJRgWVAnWHMQ85105015 = -532375760;    int MrxHJRgWVAnWHMQ36831463 = -553816397;    int MrxHJRgWVAnWHMQ4717865 = -448809096;    int MrxHJRgWVAnWHMQ58755677 = -677885471;    int MrxHJRgWVAnWHMQ94578903 = -334843030;    int MrxHJRgWVAnWHMQ93553711 = 17793246;    int MrxHJRgWVAnWHMQ84484218 = 2534038;    int MrxHJRgWVAnWHMQ46132769 = -738129102;    int MrxHJRgWVAnWHMQ15815241 = -979016419;    int MrxHJRgWVAnWHMQ33390629 = -20049075;    int MrxHJRgWVAnWHMQ16158890 = -306762704;    int MrxHJRgWVAnWHMQ8042372 = -764758249;    int MrxHJRgWVAnWHMQ8964973 = -589913824;    int MrxHJRgWVAnWHMQ88209309 = -109226730;    int MrxHJRgWVAnWHMQ40154052 = -87136935;    int MrxHJRgWVAnWHMQ48263759 = 43557512;    int MrxHJRgWVAnWHMQ9606684 = -795698067;    int MrxHJRgWVAnWHMQ13558980 = 998818;    int MrxHJRgWVAnWHMQ66802953 = -495094350;    int MrxHJRgWVAnWHMQ25682219 = -278748544;    int MrxHJRgWVAnWHMQ32465494 = -843696257;    int MrxHJRgWVAnWHMQ20170641 = -423400346;    int MrxHJRgWVAnWHMQ6294843 = -830608978;    int MrxHJRgWVAnWHMQ94329061 = 98679072;    int MrxHJRgWVAnWHMQ68724702 = -825031504;    int MrxHJRgWVAnWHMQ31800178 = -87506520;    int MrxHJRgWVAnWHMQ5185268 = -230657112;    int MrxHJRgWVAnWHMQ53978142 = -495990893;    int MrxHJRgWVAnWHMQ13008073 = -711626572;    int MrxHJRgWVAnWHMQ53599686 = -413893346;    int MrxHJRgWVAnWHMQ3630703 = -720300947;    int MrxHJRgWVAnWHMQ39040521 = -495424296;    int MrxHJRgWVAnWHMQ90880551 = -540561929;    int MrxHJRgWVAnWHMQ97518114 = -680936308;    int MrxHJRgWVAnWHMQ26631885 = -268056784;    int MrxHJRgWVAnWHMQ27392223 = -568480690;    int MrxHJRgWVAnWHMQ37849304 = -522695543;    int MrxHJRgWVAnWHMQ64848698 = -90849619;    int MrxHJRgWVAnWHMQ65259348 = -101784137;    int MrxHJRgWVAnWHMQ14477815 = -738327791;    int MrxHJRgWVAnWHMQ1452937 = -196426302;    int MrxHJRgWVAnWHMQ20081502 = -569872586;    int MrxHJRgWVAnWHMQ18257839 = -308803208;    int MrxHJRgWVAnWHMQ97143836 = -923426506;    int MrxHJRgWVAnWHMQ21220820 = 85435310;    int MrxHJRgWVAnWHMQ59673115 = -169830129;    int MrxHJRgWVAnWHMQ52009004 = -8020660;    int MrxHJRgWVAnWHMQ11685690 = 32221682;    int MrxHJRgWVAnWHMQ42044968 = -785576220;    int MrxHJRgWVAnWHMQ70883270 = -656354423;    int MrxHJRgWVAnWHMQ37284644 = -423886479;    int MrxHJRgWVAnWHMQ39439090 = 55526045;    int MrxHJRgWVAnWHMQ76131583 = -888440532;    int MrxHJRgWVAnWHMQ26136881 = -980028454;    int MrxHJRgWVAnWHMQ1470441 = -671176782;    int MrxHJRgWVAnWHMQ64934374 = -8975415;    int MrxHJRgWVAnWHMQ30536621 = -723207420;    int MrxHJRgWVAnWHMQ10388803 = -447488168;    int MrxHJRgWVAnWHMQ90030975 = -852853968;    int MrxHJRgWVAnWHMQ62778725 = -147336511;    int MrxHJRgWVAnWHMQ88368443 = -751549643;    int MrxHJRgWVAnWHMQ30506077 = -501475070;    int MrxHJRgWVAnWHMQ33124696 = 73497469;    int MrxHJRgWVAnWHMQ62215555 = -465123074;    int MrxHJRgWVAnWHMQ29759927 = -299748129;    int MrxHJRgWVAnWHMQ77118369 = -811338409;    int MrxHJRgWVAnWHMQ17161821 = -124196320;    int MrxHJRgWVAnWHMQ11446859 = -908977516;    int MrxHJRgWVAnWHMQ61577425 = -841169946;    int MrxHJRgWVAnWHMQ12761829 = -518656245;    int MrxHJRgWVAnWHMQ10414455 = -433746946;    int MrxHJRgWVAnWHMQ44757986 = -604848448;    int MrxHJRgWVAnWHMQ48299632 = -897217046;    int MrxHJRgWVAnWHMQ52325138 = -756766559;    int MrxHJRgWVAnWHMQ24229283 = 17677758;    int MrxHJRgWVAnWHMQ12383993 = -173823672;    int MrxHJRgWVAnWHMQ1912803 = -14597138;    int MrxHJRgWVAnWHMQ9151007 = -907182472;    int MrxHJRgWVAnWHMQ73108241 = -986756239;    int MrxHJRgWVAnWHMQ9051587 = -555201376;    int MrxHJRgWVAnWHMQ79791174 = 20514140;    int MrxHJRgWVAnWHMQ93499578 = -162878794;    int MrxHJRgWVAnWHMQ11933174 = -710414673;    int MrxHJRgWVAnWHMQ42124803 = 44727850;    int MrxHJRgWVAnWHMQ16315042 = -990006868;    int MrxHJRgWVAnWHMQ64191612 = -675826992;    int MrxHJRgWVAnWHMQ62908937 = -606983764;    int MrxHJRgWVAnWHMQ64743670 = -560533476;    int MrxHJRgWVAnWHMQ96047674 = 90240473;    int MrxHJRgWVAnWHMQ61697510 = -159081370;    int MrxHJRgWVAnWHMQ96855602 = -845273271;    int MrxHJRgWVAnWHMQ27460502 = -177005048;     MrxHJRgWVAnWHMQ99894999 = MrxHJRgWVAnWHMQ82199019;     MrxHJRgWVAnWHMQ82199019 = MrxHJRgWVAnWHMQ19147029;     MrxHJRgWVAnWHMQ19147029 = MrxHJRgWVAnWHMQ46891328;     MrxHJRgWVAnWHMQ46891328 = MrxHJRgWVAnWHMQ52998070;     MrxHJRgWVAnWHMQ52998070 = MrxHJRgWVAnWHMQ42934537;     MrxHJRgWVAnWHMQ42934537 = MrxHJRgWVAnWHMQ51819100;     MrxHJRgWVAnWHMQ51819100 = MrxHJRgWVAnWHMQ33935934;     MrxHJRgWVAnWHMQ33935934 = MrxHJRgWVAnWHMQ85105015;     MrxHJRgWVAnWHMQ85105015 = MrxHJRgWVAnWHMQ36831463;     MrxHJRgWVAnWHMQ36831463 = MrxHJRgWVAnWHMQ4717865;     MrxHJRgWVAnWHMQ4717865 = MrxHJRgWVAnWHMQ58755677;     MrxHJRgWVAnWHMQ58755677 = MrxHJRgWVAnWHMQ94578903;     MrxHJRgWVAnWHMQ94578903 = MrxHJRgWVAnWHMQ93553711;     MrxHJRgWVAnWHMQ93553711 = MrxHJRgWVAnWHMQ84484218;     MrxHJRgWVAnWHMQ84484218 = MrxHJRgWVAnWHMQ46132769;     MrxHJRgWVAnWHMQ46132769 = MrxHJRgWVAnWHMQ15815241;     MrxHJRgWVAnWHMQ15815241 = MrxHJRgWVAnWHMQ33390629;     MrxHJRgWVAnWHMQ33390629 = MrxHJRgWVAnWHMQ16158890;     MrxHJRgWVAnWHMQ16158890 = MrxHJRgWVAnWHMQ8042372;     MrxHJRgWVAnWHMQ8042372 = MrxHJRgWVAnWHMQ8964973;     MrxHJRgWVAnWHMQ8964973 = MrxHJRgWVAnWHMQ88209309;     MrxHJRgWVAnWHMQ88209309 = MrxHJRgWVAnWHMQ40154052;     MrxHJRgWVAnWHMQ40154052 = MrxHJRgWVAnWHMQ48263759;     MrxHJRgWVAnWHMQ48263759 = MrxHJRgWVAnWHMQ9606684;     MrxHJRgWVAnWHMQ9606684 = MrxHJRgWVAnWHMQ13558980;     MrxHJRgWVAnWHMQ13558980 = MrxHJRgWVAnWHMQ66802953;     MrxHJRgWVAnWHMQ66802953 = MrxHJRgWVAnWHMQ25682219;     MrxHJRgWVAnWHMQ25682219 = MrxHJRgWVAnWHMQ32465494;     MrxHJRgWVAnWHMQ32465494 = MrxHJRgWVAnWHMQ20170641;     MrxHJRgWVAnWHMQ20170641 = MrxHJRgWVAnWHMQ6294843;     MrxHJRgWVAnWHMQ6294843 = MrxHJRgWVAnWHMQ94329061;     MrxHJRgWVAnWHMQ94329061 = MrxHJRgWVAnWHMQ68724702;     MrxHJRgWVAnWHMQ68724702 = MrxHJRgWVAnWHMQ31800178;     MrxHJRgWVAnWHMQ31800178 = MrxHJRgWVAnWHMQ5185268;     MrxHJRgWVAnWHMQ5185268 = MrxHJRgWVAnWHMQ53978142;     MrxHJRgWVAnWHMQ53978142 = MrxHJRgWVAnWHMQ13008073;     MrxHJRgWVAnWHMQ13008073 = MrxHJRgWVAnWHMQ53599686;     MrxHJRgWVAnWHMQ53599686 = MrxHJRgWVAnWHMQ3630703;     MrxHJRgWVAnWHMQ3630703 = MrxHJRgWVAnWHMQ39040521;     MrxHJRgWVAnWHMQ39040521 = MrxHJRgWVAnWHMQ90880551;     MrxHJRgWVAnWHMQ90880551 = MrxHJRgWVAnWHMQ97518114;     MrxHJRgWVAnWHMQ97518114 = MrxHJRgWVAnWHMQ26631885;     MrxHJRgWVAnWHMQ26631885 = MrxHJRgWVAnWHMQ27392223;     MrxHJRgWVAnWHMQ27392223 = MrxHJRgWVAnWHMQ37849304;     MrxHJRgWVAnWHMQ37849304 = MrxHJRgWVAnWHMQ64848698;     MrxHJRgWVAnWHMQ64848698 = MrxHJRgWVAnWHMQ65259348;     MrxHJRgWVAnWHMQ65259348 = MrxHJRgWVAnWHMQ14477815;     MrxHJRgWVAnWHMQ14477815 = MrxHJRgWVAnWHMQ1452937;     MrxHJRgWVAnWHMQ1452937 = MrxHJRgWVAnWHMQ20081502;     MrxHJRgWVAnWHMQ20081502 = MrxHJRgWVAnWHMQ18257839;     MrxHJRgWVAnWHMQ18257839 = MrxHJRgWVAnWHMQ97143836;     MrxHJRgWVAnWHMQ97143836 = MrxHJRgWVAnWHMQ21220820;     MrxHJRgWVAnWHMQ21220820 = MrxHJRgWVAnWHMQ59673115;     MrxHJRgWVAnWHMQ59673115 = MrxHJRgWVAnWHMQ52009004;     MrxHJRgWVAnWHMQ52009004 = MrxHJRgWVAnWHMQ11685690;     MrxHJRgWVAnWHMQ11685690 = MrxHJRgWVAnWHMQ42044968;     MrxHJRgWVAnWHMQ42044968 = MrxHJRgWVAnWHMQ70883270;     MrxHJRgWVAnWHMQ70883270 = MrxHJRgWVAnWHMQ37284644;     MrxHJRgWVAnWHMQ37284644 = MrxHJRgWVAnWHMQ39439090;     MrxHJRgWVAnWHMQ39439090 = MrxHJRgWVAnWHMQ76131583;     MrxHJRgWVAnWHMQ76131583 = MrxHJRgWVAnWHMQ26136881;     MrxHJRgWVAnWHMQ26136881 = MrxHJRgWVAnWHMQ1470441;     MrxHJRgWVAnWHMQ1470441 = MrxHJRgWVAnWHMQ64934374;     MrxHJRgWVAnWHMQ64934374 = MrxHJRgWVAnWHMQ30536621;     MrxHJRgWVAnWHMQ30536621 = MrxHJRgWVAnWHMQ10388803;     MrxHJRgWVAnWHMQ10388803 = MrxHJRgWVAnWHMQ90030975;     MrxHJRgWVAnWHMQ90030975 = MrxHJRgWVAnWHMQ62778725;     MrxHJRgWVAnWHMQ62778725 = MrxHJRgWVAnWHMQ88368443;     MrxHJRgWVAnWHMQ88368443 = MrxHJRgWVAnWHMQ30506077;     MrxHJRgWVAnWHMQ30506077 = MrxHJRgWVAnWHMQ33124696;     MrxHJRgWVAnWHMQ33124696 = MrxHJRgWVAnWHMQ62215555;     MrxHJRgWVAnWHMQ62215555 = MrxHJRgWVAnWHMQ29759927;     MrxHJRgWVAnWHMQ29759927 = MrxHJRgWVAnWHMQ77118369;     MrxHJRgWVAnWHMQ77118369 = MrxHJRgWVAnWHMQ17161821;     MrxHJRgWVAnWHMQ17161821 = MrxHJRgWVAnWHMQ11446859;     MrxHJRgWVAnWHMQ11446859 = MrxHJRgWVAnWHMQ61577425;     MrxHJRgWVAnWHMQ61577425 = MrxHJRgWVAnWHMQ12761829;     MrxHJRgWVAnWHMQ12761829 = MrxHJRgWVAnWHMQ10414455;     MrxHJRgWVAnWHMQ10414455 = MrxHJRgWVAnWHMQ44757986;     MrxHJRgWVAnWHMQ44757986 = MrxHJRgWVAnWHMQ48299632;     MrxHJRgWVAnWHMQ48299632 = MrxHJRgWVAnWHMQ52325138;     MrxHJRgWVAnWHMQ52325138 = MrxHJRgWVAnWHMQ24229283;     MrxHJRgWVAnWHMQ24229283 = MrxHJRgWVAnWHMQ12383993;     MrxHJRgWVAnWHMQ12383993 = MrxHJRgWVAnWHMQ1912803;     MrxHJRgWVAnWHMQ1912803 = MrxHJRgWVAnWHMQ9151007;     MrxHJRgWVAnWHMQ9151007 = MrxHJRgWVAnWHMQ73108241;     MrxHJRgWVAnWHMQ73108241 = MrxHJRgWVAnWHMQ9051587;     MrxHJRgWVAnWHMQ9051587 = MrxHJRgWVAnWHMQ79791174;     MrxHJRgWVAnWHMQ79791174 = MrxHJRgWVAnWHMQ93499578;     MrxHJRgWVAnWHMQ93499578 = MrxHJRgWVAnWHMQ11933174;     MrxHJRgWVAnWHMQ11933174 = MrxHJRgWVAnWHMQ42124803;     MrxHJRgWVAnWHMQ42124803 = MrxHJRgWVAnWHMQ16315042;     MrxHJRgWVAnWHMQ16315042 = MrxHJRgWVAnWHMQ64191612;     MrxHJRgWVAnWHMQ64191612 = MrxHJRgWVAnWHMQ62908937;     MrxHJRgWVAnWHMQ62908937 = MrxHJRgWVAnWHMQ64743670;     MrxHJRgWVAnWHMQ64743670 = MrxHJRgWVAnWHMQ96047674;     MrxHJRgWVAnWHMQ96047674 = MrxHJRgWVAnWHMQ61697510;     MrxHJRgWVAnWHMQ61697510 = MrxHJRgWVAnWHMQ96855602;     MrxHJRgWVAnWHMQ96855602 = MrxHJRgWVAnWHMQ27460502;     MrxHJRgWVAnWHMQ27460502 = MrxHJRgWVAnWHMQ99894999;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void rHXisGTDmJXFGzBO67632722() {     int CEMYUEuMKyUedac99888528 = -49529187;    int CEMYUEuMKyUedac20771337 = 84691727;    int CEMYUEuMKyUedac20460241 = -669925524;    int CEMYUEuMKyUedac31971189 = -40125378;    int CEMYUEuMKyUedac61838172 = -511830450;    int CEMYUEuMKyUedac77739969 = -82967693;    int CEMYUEuMKyUedac20366436 = 36686536;    int CEMYUEuMKyUedac3149985 = -219050340;    int CEMYUEuMKyUedac67845179 = -246021829;    int CEMYUEuMKyUedac33470977 = -190623268;    int CEMYUEuMKyUedac25038470 = -408281167;    int CEMYUEuMKyUedac73047354 = -413363825;    int CEMYUEuMKyUedac26431818 = -356501505;    int CEMYUEuMKyUedac47397931 = -327359805;    int CEMYUEuMKyUedac29656328 = 39821204;    int CEMYUEuMKyUedac20697316 = 29075247;    int CEMYUEuMKyUedac71668000 = -798630989;    int CEMYUEuMKyUedac56391162 = -417120693;    int CEMYUEuMKyUedac36587159 = -924744971;    int CEMYUEuMKyUedac80116135 = -276782701;    int CEMYUEuMKyUedac88634397 = -300380193;    int CEMYUEuMKyUedac66505576 = -619218613;    int CEMYUEuMKyUedac77032448 = -546094777;    int CEMYUEuMKyUedac91913866 = -937363011;    int CEMYUEuMKyUedac38049373 = -122391908;    int CEMYUEuMKyUedac30133057 = 92792885;    int CEMYUEuMKyUedac37024535 = -795894062;    int CEMYUEuMKyUedac61085744 = -887715526;    int CEMYUEuMKyUedac45143941 = -389766587;    int CEMYUEuMKyUedac96260510 = -98831720;    int CEMYUEuMKyUedac39707522 = -117702212;    int CEMYUEuMKyUedac79052918 = -293142693;    int CEMYUEuMKyUedac54177125 = 73024277;    int CEMYUEuMKyUedac32083016 = 35811747;    int CEMYUEuMKyUedac87543306 = -489971040;    int CEMYUEuMKyUedac96806331 = -575756657;    int CEMYUEuMKyUedac24949441 = -811279950;    int CEMYUEuMKyUedac23517366 = -409156022;    int CEMYUEuMKyUedac38944299 = -332982375;    int CEMYUEuMKyUedac10913415 = -104855982;    int CEMYUEuMKyUedac78736219 = -628505828;    int CEMYUEuMKyUedac83215775 = -874728411;    int CEMYUEuMKyUedac86915244 = -827187040;    int CEMYUEuMKyUedac86624121 = -125445133;    int CEMYUEuMKyUedac21867357 = -995954945;    int CEMYUEuMKyUedac5276667 = 97105692;    int CEMYUEuMKyUedac92957141 = -583064759;    int CEMYUEuMKyUedac89861396 = -247089148;    int CEMYUEuMKyUedac20111326 = -860190188;    int CEMYUEuMKyUedac40126925 = -156897467;    int CEMYUEuMKyUedac31517209 = -987358216;    int CEMYUEuMKyUedac11691094 = -205572259;    int CEMYUEuMKyUedac96669168 = -906618695;    int CEMYUEuMKyUedac69923386 = -701749980;    int CEMYUEuMKyUedac78998324 = -864919943;    int CEMYUEuMKyUedac33382953 = -430310575;    int CEMYUEuMKyUedac43738889 = -369213497;    int CEMYUEuMKyUedac28546374 = -732562514;    int CEMYUEuMKyUedac93921815 = -917733471;    int CEMYUEuMKyUedac31705116 = -504623336;    int CEMYUEuMKyUedac40715434 = -287073631;    int CEMYUEuMKyUedac59280692 = -75597939;    int CEMYUEuMKyUedac58006044 = -829283753;    int CEMYUEuMKyUedac71584668 = -47190110;    int CEMYUEuMKyUedac93763455 = 27078944;    int CEMYUEuMKyUedac45985551 = -15138475;    int CEMYUEuMKyUedac18870229 = -386388102;    int CEMYUEuMKyUedac94348802 = -292313252;    int CEMYUEuMKyUedac59854625 = -837388765;    int CEMYUEuMKyUedac32849997 = -384422139;    int CEMYUEuMKyUedac95747874 = -159644803;    int CEMYUEuMKyUedac48150635 = -289474968;    int CEMYUEuMKyUedac17446863 = 15861682;    int CEMYUEuMKyUedac25673745 = -719888990;    int CEMYUEuMKyUedac1379917 = -648276873;    int CEMYUEuMKyUedac5418622 = -425651782;    int CEMYUEuMKyUedac79590331 = -792031573;    int CEMYUEuMKyUedac90408327 = -320649644;    int CEMYUEuMKyUedac70046510 = -941408066;    int CEMYUEuMKyUedac32772707 = -119497600;    int CEMYUEuMKyUedac37175916 = -324142356;    int CEMYUEuMKyUedac47163139 = -448804915;    int CEMYUEuMKyUedac40974418 = 72474662;    int CEMYUEuMKyUedac5017016 = -132869121;    int CEMYUEuMKyUedac64743302 = -111473505;    int CEMYUEuMKyUedac28016428 = -912129954;    int CEMYUEuMKyUedac82383750 = -386523998;    int CEMYUEuMKyUedac84253739 = -225225743;    int CEMYUEuMKyUedac53084691 = -99268310;    int CEMYUEuMKyUedac54160354 = 40339534;    int CEMYUEuMKyUedac53067443 = -106543161;    int CEMYUEuMKyUedac96403067 = 21282563;    int CEMYUEuMKyUedac29595550 = -491422552;    int CEMYUEuMKyUedac7239184 = -828359040;    int CEMYUEuMKyUedac70197980 = -817782351;    int CEMYUEuMKyUedac19455527 = -452907890;    int CEMYUEuMKyUedac25209732 = 54555341;    int CEMYUEuMKyUedac15330576 = -679996931;    int CEMYUEuMKyUedac92860665 = -52524078;    int CEMYUEuMKyUedac75881806 = -49529187;     CEMYUEuMKyUedac99888528 = CEMYUEuMKyUedac20771337;     CEMYUEuMKyUedac20771337 = CEMYUEuMKyUedac20460241;     CEMYUEuMKyUedac20460241 = CEMYUEuMKyUedac31971189;     CEMYUEuMKyUedac31971189 = CEMYUEuMKyUedac61838172;     CEMYUEuMKyUedac61838172 = CEMYUEuMKyUedac77739969;     CEMYUEuMKyUedac77739969 = CEMYUEuMKyUedac20366436;     CEMYUEuMKyUedac20366436 = CEMYUEuMKyUedac3149985;     CEMYUEuMKyUedac3149985 = CEMYUEuMKyUedac67845179;     CEMYUEuMKyUedac67845179 = CEMYUEuMKyUedac33470977;     CEMYUEuMKyUedac33470977 = CEMYUEuMKyUedac25038470;     CEMYUEuMKyUedac25038470 = CEMYUEuMKyUedac73047354;     CEMYUEuMKyUedac73047354 = CEMYUEuMKyUedac26431818;     CEMYUEuMKyUedac26431818 = CEMYUEuMKyUedac47397931;     CEMYUEuMKyUedac47397931 = CEMYUEuMKyUedac29656328;     CEMYUEuMKyUedac29656328 = CEMYUEuMKyUedac20697316;     CEMYUEuMKyUedac20697316 = CEMYUEuMKyUedac71668000;     CEMYUEuMKyUedac71668000 = CEMYUEuMKyUedac56391162;     CEMYUEuMKyUedac56391162 = CEMYUEuMKyUedac36587159;     CEMYUEuMKyUedac36587159 = CEMYUEuMKyUedac80116135;     CEMYUEuMKyUedac80116135 = CEMYUEuMKyUedac88634397;     CEMYUEuMKyUedac88634397 = CEMYUEuMKyUedac66505576;     CEMYUEuMKyUedac66505576 = CEMYUEuMKyUedac77032448;     CEMYUEuMKyUedac77032448 = CEMYUEuMKyUedac91913866;     CEMYUEuMKyUedac91913866 = CEMYUEuMKyUedac38049373;     CEMYUEuMKyUedac38049373 = CEMYUEuMKyUedac30133057;     CEMYUEuMKyUedac30133057 = CEMYUEuMKyUedac37024535;     CEMYUEuMKyUedac37024535 = CEMYUEuMKyUedac61085744;     CEMYUEuMKyUedac61085744 = CEMYUEuMKyUedac45143941;     CEMYUEuMKyUedac45143941 = CEMYUEuMKyUedac96260510;     CEMYUEuMKyUedac96260510 = CEMYUEuMKyUedac39707522;     CEMYUEuMKyUedac39707522 = CEMYUEuMKyUedac79052918;     CEMYUEuMKyUedac79052918 = CEMYUEuMKyUedac54177125;     CEMYUEuMKyUedac54177125 = CEMYUEuMKyUedac32083016;     CEMYUEuMKyUedac32083016 = CEMYUEuMKyUedac87543306;     CEMYUEuMKyUedac87543306 = CEMYUEuMKyUedac96806331;     CEMYUEuMKyUedac96806331 = CEMYUEuMKyUedac24949441;     CEMYUEuMKyUedac24949441 = CEMYUEuMKyUedac23517366;     CEMYUEuMKyUedac23517366 = CEMYUEuMKyUedac38944299;     CEMYUEuMKyUedac38944299 = CEMYUEuMKyUedac10913415;     CEMYUEuMKyUedac10913415 = CEMYUEuMKyUedac78736219;     CEMYUEuMKyUedac78736219 = CEMYUEuMKyUedac83215775;     CEMYUEuMKyUedac83215775 = CEMYUEuMKyUedac86915244;     CEMYUEuMKyUedac86915244 = CEMYUEuMKyUedac86624121;     CEMYUEuMKyUedac86624121 = CEMYUEuMKyUedac21867357;     CEMYUEuMKyUedac21867357 = CEMYUEuMKyUedac5276667;     CEMYUEuMKyUedac5276667 = CEMYUEuMKyUedac92957141;     CEMYUEuMKyUedac92957141 = CEMYUEuMKyUedac89861396;     CEMYUEuMKyUedac89861396 = CEMYUEuMKyUedac20111326;     CEMYUEuMKyUedac20111326 = CEMYUEuMKyUedac40126925;     CEMYUEuMKyUedac40126925 = CEMYUEuMKyUedac31517209;     CEMYUEuMKyUedac31517209 = CEMYUEuMKyUedac11691094;     CEMYUEuMKyUedac11691094 = CEMYUEuMKyUedac96669168;     CEMYUEuMKyUedac96669168 = CEMYUEuMKyUedac69923386;     CEMYUEuMKyUedac69923386 = CEMYUEuMKyUedac78998324;     CEMYUEuMKyUedac78998324 = CEMYUEuMKyUedac33382953;     CEMYUEuMKyUedac33382953 = CEMYUEuMKyUedac43738889;     CEMYUEuMKyUedac43738889 = CEMYUEuMKyUedac28546374;     CEMYUEuMKyUedac28546374 = CEMYUEuMKyUedac93921815;     CEMYUEuMKyUedac93921815 = CEMYUEuMKyUedac31705116;     CEMYUEuMKyUedac31705116 = CEMYUEuMKyUedac40715434;     CEMYUEuMKyUedac40715434 = CEMYUEuMKyUedac59280692;     CEMYUEuMKyUedac59280692 = CEMYUEuMKyUedac58006044;     CEMYUEuMKyUedac58006044 = CEMYUEuMKyUedac71584668;     CEMYUEuMKyUedac71584668 = CEMYUEuMKyUedac93763455;     CEMYUEuMKyUedac93763455 = CEMYUEuMKyUedac45985551;     CEMYUEuMKyUedac45985551 = CEMYUEuMKyUedac18870229;     CEMYUEuMKyUedac18870229 = CEMYUEuMKyUedac94348802;     CEMYUEuMKyUedac94348802 = CEMYUEuMKyUedac59854625;     CEMYUEuMKyUedac59854625 = CEMYUEuMKyUedac32849997;     CEMYUEuMKyUedac32849997 = CEMYUEuMKyUedac95747874;     CEMYUEuMKyUedac95747874 = CEMYUEuMKyUedac48150635;     CEMYUEuMKyUedac48150635 = CEMYUEuMKyUedac17446863;     CEMYUEuMKyUedac17446863 = CEMYUEuMKyUedac25673745;     CEMYUEuMKyUedac25673745 = CEMYUEuMKyUedac1379917;     CEMYUEuMKyUedac1379917 = CEMYUEuMKyUedac5418622;     CEMYUEuMKyUedac5418622 = CEMYUEuMKyUedac79590331;     CEMYUEuMKyUedac79590331 = CEMYUEuMKyUedac90408327;     CEMYUEuMKyUedac90408327 = CEMYUEuMKyUedac70046510;     CEMYUEuMKyUedac70046510 = CEMYUEuMKyUedac32772707;     CEMYUEuMKyUedac32772707 = CEMYUEuMKyUedac37175916;     CEMYUEuMKyUedac37175916 = CEMYUEuMKyUedac47163139;     CEMYUEuMKyUedac47163139 = CEMYUEuMKyUedac40974418;     CEMYUEuMKyUedac40974418 = CEMYUEuMKyUedac5017016;     CEMYUEuMKyUedac5017016 = CEMYUEuMKyUedac64743302;     CEMYUEuMKyUedac64743302 = CEMYUEuMKyUedac28016428;     CEMYUEuMKyUedac28016428 = CEMYUEuMKyUedac82383750;     CEMYUEuMKyUedac82383750 = CEMYUEuMKyUedac84253739;     CEMYUEuMKyUedac84253739 = CEMYUEuMKyUedac53084691;     CEMYUEuMKyUedac53084691 = CEMYUEuMKyUedac54160354;     CEMYUEuMKyUedac54160354 = CEMYUEuMKyUedac53067443;     CEMYUEuMKyUedac53067443 = CEMYUEuMKyUedac96403067;     CEMYUEuMKyUedac96403067 = CEMYUEuMKyUedac29595550;     CEMYUEuMKyUedac29595550 = CEMYUEuMKyUedac7239184;     CEMYUEuMKyUedac7239184 = CEMYUEuMKyUedac70197980;     CEMYUEuMKyUedac70197980 = CEMYUEuMKyUedac19455527;     CEMYUEuMKyUedac19455527 = CEMYUEuMKyUedac25209732;     CEMYUEuMKyUedac25209732 = CEMYUEuMKyUedac15330576;     CEMYUEuMKyUedac15330576 = CEMYUEuMKyUedac92860665;     CEMYUEuMKyUedac92860665 = CEMYUEuMKyUedac75881806;     CEMYUEuMKyUedac75881806 = CEMYUEuMKyUedac99888528;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void DiHVKzAZuhCuTShd88245865() {     int ggzbLeMRTgHoBeY7742703 = -34554875;    int ggzbLeMRTgHoBeY45175519 = -791159034;    int ggzbLeMRTgHoBeY9974481 = -903835676;    int ggzbLeMRTgHoBeY37851406 = -917007940;    int ggzbLeMRTgHoBeY5247569 = -73042153;    int ggzbLeMRTgHoBeY85339167 = -581632952;    int ggzbLeMRTgHoBeY12495412 = -274355921;    int ggzbLeMRTgHoBeY4397842 = -153659493;    int ggzbLeMRTgHoBeY13100711 = -701491689;    int ggzbLeMRTgHoBeY44337882 = -327043010;    int ggzbLeMRTgHoBeY75361307 = -807573208;    int ggzbLeMRTgHoBeY11653856 = -308881945;    int ggzbLeMRTgHoBeY41858501 = -675158236;    int ggzbLeMRTgHoBeY848272 = -949990657;    int ggzbLeMRTgHoBeY18120985 = -137690477;    int ggzbLeMRTgHoBeY80288845 = -397409339;    int ggzbLeMRTgHoBeY7511339 = -178367150;    int ggzbLeMRTgHoBeY58377104 = -720951929;    int ggzbLeMRTgHoBeY77883783 = -53932684;    int ggzbLeMRTgHoBeY12377433 = -740947656;    int ggzbLeMRTgHoBeY3017588 = -861059228;    int ggzbLeMRTgHoBeY10871987 = -848131366;    int ggzbLeMRTgHoBeY4753156 = -61110250;    int ggzbLeMRTgHoBeY7643964 = -511029819;    int ggzbLeMRTgHoBeY71054209 = -59568926;    int ggzbLeMRTgHoBeY83416627 = -704420731;    int ggzbLeMRTgHoBeY81757552 = 16234945;    int ggzbLeMRTgHoBeY63471213 = -634733425;    int ggzbLeMRTgHoBeY32856658 = -775530600;    int ggzbLeMRTgHoBeY40838764 = -459345540;    int ggzbLeMRTgHoBeY60815710 = -817132308;    int ggzbLeMRTgHoBeY93512306 = -778408257;    int ggzbLeMRTgHoBeY90618277 = -626484922;    int ggzbLeMRTgHoBeY20866583 = -782156751;    int ggzbLeMRTgHoBeY76633576 = -180685304;    int ggzbLeMRTgHoBeY36524807 = -731010138;    int ggzbLeMRTgHoBeY77874259 = -559357219;    int ggzbLeMRTgHoBeY90961770 = -265358730;    int ggzbLeMRTgHoBeY23500997 = -319052901;    int ggzbLeMRTgHoBeY30529545 = -469552950;    int ggzbLeMRTgHoBeY56125613 = 34047465;    int ggzbLeMRTgHoBeY78967217 = 15057730;    int ggzbLeMRTgHoBeY31494615 = -118985590;    int ggzbLeMRTgHoBeY27159706 = -180935638;    int ggzbLeMRTgHoBeY34844517 = -315035331;    int ggzbLeMRTgHoBeY8803317 = -633991179;    int ggzbLeMRTgHoBeY39977461 = -493860595;    int ggzbLeMRTgHoBeY32956906 = -784199036;    int ggzbLeMRTgHoBeY89140910 = -743938916;    int ggzbLeMRTgHoBeY86683480 = -206163439;    int ggzbLeMRTgHoBeY54582936 = -643517291;    int ggzbLeMRTgHoBeY843752 = -97361698;    int ggzbLeMRTgHoBeY24294285 = -194507972;    int ggzbLeMRTgHoBeY83388939 = -779039474;    int ggzbLeMRTgHoBeY85228278 = -555990102;    int ggzbLeMRTgHoBeY96870716 = -186423510;    int ggzbLeMRTgHoBeY40422364 = -630048784;    int ggzbLeMRTgHoBeY2330517 = -292805857;    int ggzbLeMRTgHoBeY66797197 = -757439014;    int ggzbLeMRTgHoBeY21830941 = -368621423;    int ggzbLeMRTgHoBeY3581616 = -497867897;    int ggzbLeMRTgHoBeY49024198 = -639622497;    int ggzbLeMRTgHoBeY71541183 = -378128894;    int ggzbLeMRTgHoBeY72261947 = -142146150;    int ggzbLeMRTgHoBeY83522171 = -509910702;    int ggzbLeMRTgHoBeY81849001 = 70835048;    int ggzbLeMRTgHoBeY21035578 = -682397024;    int ggzbLeMRTgHoBeY20991919 = -893001486;    int ggzbLeMRTgHoBeY24214695 = -669305354;    int ggzbLeMRTgHoBeY81596177 = -406680340;    int ggzbLeMRTgHoBeY2414586 = -838052120;    int ggzbLeMRTgHoBeY16549569 = -913008420;    int ggzbLeMRTgHoBeY34876108 = -301899029;    int ggzbLeMRTgHoBeY47354239 = -584379735;    int ggzbLeMRTgHoBeY56251820 = -674995121;    int ggzbLeMRTgHoBeY24050370 = -776116959;    int ggzbLeMRTgHoBeY79377371 = -629145777;    int ggzbLeMRTgHoBeY77593450 = -880174613;    int ggzbLeMRTgHoBeY72799447 = -95994488;    int ggzbLeMRTgHoBeY62250893 = -425577748;    int ggzbLeMRTgHoBeY43439166 = -110560137;    int ggzbLeMRTgHoBeY48800646 = -199566019;    int ggzbLeMRTgHoBeY74330303 = -890794510;    int ggzbLeMRTgHoBeY46173177 = -469367162;    int ggzbLeMRTgHoBeY86255827 = -815828249;    int ggzbLeMRTgHoBeY59971959 = -619770611;    int ggzbLeMRTgHoBeY69218021 = -483900285;    int ggzbLeMRTgHoBeY7229339 = -847445448;    int ggzbLeMRTgHoBeY35638305 = -126166649;    int ggzbLeMRTgHoBeY79762860 = -994261795;    int ggzbLeMRTgHoBeY96102443 = -961355;    int ggzbLeMRTgHoBeY75543742 = -166551362;    int ggzbLeMRTgHoBeY24164574 = -507919716;    int ggzbLeMRTgHoBeY1670056 = -950431478;    int ggzbLeMRTgHoBeY26947930 = -971685053;    int ggzbLeMRTgHoBeY7101415 = -326330039;    int ggzbLeMRTgHoBeY7426034 = -606813376;    int ggzbLeMRTgHoBeY59232668 = -976839441;    int ggzbLeMRTgHoBeY43637534 = -671024936;    int ggzbLeMRTgHoBeY52995516 = -34554875;     ggzbLeMRTgHoBeY7742703 = ggzbLeMRTgHoBeY45175519;     ggzbLeMRTgHoBeY45175519 = ggzbLeMRTgHoBeY9974481;     ggzbLeMRTgHoBeY9974481 = ggzbLeMRTgHoBeY37851406;     ggzbLeMRTgHoBeY37851406 = ggzbLeMRTgHoBeY5247569;     ggzbLeMRTgHoBeY5247569 = ggzbLeMRTgHoBeY85339167;     ggzbLeMRTgHoBeY85339167 = ggzbLeMRTgHoBeY12495412;     ggzbLeMRTgHoBeY12495412 = ggzbLeMRTgHoBeY4397842;     ggzbLeMRTgHoBeY4397842 = ggzbLeMRTgHoBeY13100711;     ggzbLeMRTgHoBeY13100711 = ggzbLeMRTgHoBeY44337882;     ggzbLeMRTgHoBeY44337882 = ggzbLeMRTgHoBeY75361307;     ggzbLeMRTgHoBeY75361307 = ggzbLeMRTgHoBeY11653856;     ggzbLeMRTgHoBeY11653856 = ggzbLeMRTgHoBeY41858501;     ggzbLeMRTgHoBeY41858501 = ggzbLeMRTgHoBeY848272;     ggzbLeMRTgHoBeY848272 = ggzbLeMRTgHoBeY18120985;     ggzbLeMRTgHoBeY18120985 = ggzbLeMRTgHoBeY80288845;     ggzbLeMRTgHoBeY80288845 = ggzbLeMRTgHoBeY7511339;     ggzbLeMRTgHoBeY7511339 = ggzbLeMRTgHoBeY58377104;     ggzbLeMRTgHoBeY58377104 = ggzbLeMRTgHoBeY77883783;     ggzbLeMRTgHoBeY77883783 = ggzbLeMRTgHoBeY12377433;     ggzbLeMRTgHoBeY12377433 = ggzbLeMRTgHoBeY3017588;     ggzbLeMRTgHoBeY3017588 = ggzbLeMRTgHoBeY10871987;     ggzbLeMRTgHoBeY10871987 = ggzbLeMRTgHoBeY4753156;     ggzbLeMRTgHoBeY4753156 = ggzbLeMRTgHoBeY7643964;     ggzbLeMRTgHoBeY7643964 = ggzbLeMRTgHoBeY71054209;     ggzbLeMRTgHoBeY71054209 = ggzbLeMRTgHoBeY83416627;     ggzbLeMRTgHoBeY83416627 = ggzbLeMRTgHoBeY81757552;     ggzbLeMRTgHoBeY81757552 = ggzbLeMRTgHoBeY63471213;     ggzbLeMRTgHoBeY63471213 = ggzbLeMRTgHoBeY32856658;     ggzbLeMRTgHoBeY32856658 = ggzbLeMRTgHoBeY40838764;     ggzbLeMRTgHoBeY40838764 = ggzbLeMRTgHoBeY60815710;     ggzbLeMRTgHoBeY60815710 = ggzbLeMRTgHoBeY93512306;     ggzbLeMRTgHoBeY93512306 = ggzbLeMRTgHoBeY90618277;     ggzbLeMRTgHoBeY90618277 = ggzbLeMRTgHoBeY20866583;     ggzbLeMRTgHoBeY20866583 = ggzbLeMRTgHoBeY76633576;     ggzbLeMRTgHoBeY76633576 = ggzbLeMRTgHoBeY36524807;     ggzbLeMRTgHoBeY36524807 = ggzbLeMRTgHoBeY77874259;     ggzbLeMRTgHoBeY77874259 = ggzbLeMRTgHoBeY90961770;     ggzbLeMRTgHoBeY90961770 = ggzbLeMRTgHoBeY23500997;     ggzbLeMRTgHoBeY23500997 = ggzbLeMRTgHoBeY30529545;     ggzbLeMRTgHoBeY30529545 = ggzbLeMRTgHoBeY56125613;     ggzbLeMRTgHoBeY56125613 = ggzbLeMRTgHoBeY78967217;     ggzbLeMRTgHoBeY78967217 = ggzbLeMRTgHoBeY31494615;     ggzbLeMRTgHoBeY31494615 = ggzbLeMRTgHoBeY27159706;     ggzbLeMRTgHoBeY27159706 = ggzbLeMRTgHoBeY34844517;     ggzbLeMRTgHoBeY34844517 = ggzbLeMRTgHoBeY8803317;     ggzbLeMRTgHoBeY8803317 = ggzbLeMRTgHoBeY39977461;     ggzbLeMRTgHoBeY39977461 = ggzbLeMRTgHoBeY32956906;     ggzbLeMRTgHoBeY32956906 = ggzbLeMRTgHoBeY89140910;     ggzbLeMRTgHoBeY89140910 = ggzbLeMRTgHoBeY86683480;     ggzbLeMRTgHoBeY86683480 = ggzbLeMRTgHoBeY54582936;     ggzbLeMRTgHoBeY54582936 = ggzbLeMRTgHoBeY843752;     ggzbLeMRTgHoBeY843752 = ggzbLeMRTgHoBeY24294285;     ggzbLeMRTgHoBeY24294285 = ggzbLeMRTgHoBeY83388939;     ggzbLeMRTgHoBeY83388939 = ggzbLeMRTgHoBeY85228278;     ggzbLeMRTgHoBeY85228278 = ggzbLeMRTgHoBeY96870716;     ggzbLeMRTgHoBeY96870716 = ggzbLeMRTgHoBeY40422364;     ggzbLeMRTgHoBeY40422364 = ggzbLeMRTgHoBeY2330517;     ggzbLeMRTgHoBeY2330517 = ggzbLeMRTgHoBeY66797197;     ggzbLeMRTgHoBeY66797197 = ggzbLeMRTgHoBeY21830941;     ggzbLeMRTgHoBeY21830941 = ggzbLeMRTgHoBeY3581616;     ggzbLeMRTgHoBeY3581616 = ggzbLeMRTgHoBeY49024198;     ggzbLeMRTgHoBeY49024198 = ggzbLeMRTgHoBeY71541183;     ggzbLeMRTgHoBeY71541183 = ggzbLeMRTgHoBeY72261947;     ggzbLeMRTgHoBeY72261947 = ggzbLeMRTgHoBeY83522171;     ggzbLeMRTgHoBeY83522171 = ggzbLeMRTgHoBeY81849001;     ggzbLeMRTgHoBeY81849001 = ggzbLeMRTgHoBeY21035578;     ggzbLeMRTgHoBeY21035578 = ggzbLeMRTgHoBeY20991919;     ggzbLeMRTgHoBeY20991919 = ggzbLeMRTgHoBeY24214695;     ggzbLeMRTgHoBeY24214695 = ggzbLeMRTgHoBeY81596177;     ggzbLeMRTgHoBeY81596177 = ggzbLeMRTgHoBeY2414586;     ggzbLeMRTgHoBeY2414586 = ggzbLeMRTgHoBeY16549569;     ggzbLeMRTgHoBeY16549569 = ggzbLeMRTgHoBeY34876108;     ggzbLeMRTgHoBeY34876108 = ggzbLeMRTgHoBeY47354239;     ggzbLeMRTgHoBeY47354239 = ggzbLeMRTgHoBeY56251820;     ggzbLeMRTgHoBeY56251820 = ggzbLeMRTgHoBeY24050370;     ggzbLeMRTgHoBeY24050370 = ggzbLeMRTgHoBeY79377371;     ggzbLeMRTgHoBeY79377371 = ggzbLeMRTgHoBeY77593450;     ggzbLeMRTgHoBeY77593450 = ggzbLeMRTgHoBeY72799447;     ggzbLeMRTgHoBeY72799447 = ggzbLeMRTgHoBeY62250893;     ggzbLeMRTgHoBeY62250893 = ggzbLeMRTgHoBeY43439166;     ggzbLeMRTgHoBeY43439166 = ggzbLeMRTgHoBeY48800646;     ggzbLeMRTgHoBeY48800646 = ggzbLeMRTgHoBeY74330303;     ggzbLeMRTgHoBeY74330303 = ggzbLeMRTgHoBeY46173177;     ggzbLeMRTgHoBeY46173177 = ggzbLeMRTgHoBeY86255827;     ggzbLeMRTgHoBeY86255827 = ggzbLeMRTgHoBeY59971959;     ggzbLeMRTgHoBeY59971959 = ggzbLeMRTgHoBeY69218021;     ggzbLeMRTgHoBeY69218021 = ggzbLeMRTgHoBeY7229339;     ggzbLeMRTgHoBeY7229339 = ggzbLeMRTgHoBeY35638305;     ggzbLeMRTgHoBeY35638305 = ggzbLeMRTgHoBeY79762860;     ggzbLeMRTgHoBeY79762860 = ggzbLeMRTgHoBeY96102443;     ggzbLeMRTgHoBeY96102443 = ggzbLeMRTgHoBeY75543742;     ggzbLeMRTgHoBeY75543742 = ggzbLeMRTgHoBeY24164574;     ggzbLeMRTgHoBeY24164574 = ggzbLeMRTgHoBeY1670056;     ggzbLeMRTgHoBeY1670056 = ggzbLeMRTgHoBeY26947930;     ggzbLeMRTgHoBeY26947930 = ggzbLeMRTgHoBeY7101415;     ggzbLeMRTgHoBeY7101415 = ggzbLeMRTgHoBeY7426034;     ggzbLeMRTgHoBeY7426034 = ggzbLeMRTgHoBeY59232668;     ggzbLeMRTgHoBeY59232668 = ggzbLeMRTgHoBeY43637534;     ggzbLeMRTgHoBeY43637534 = ggzbLeMRTgHoBeY52995516;     ggzbLeMRTgHoBeY52995516 = ggzbLeMRTgHoBeY7742703;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void sJOtLNaDKuPUPFrG59024107() {     int NmdjhAfVyRJFhJM38266444 = -837078840;    int NmdjhAfVyRJFhJM65367051 = -298524945;    int NmdjhAfVyRJFhJM69779004 = -923617828;    int NmdjhAfVyRJFhJM554084 = -51109715;    int NmdjhAfVyRJFhJM15851180 = -582213889;    int NmdjhAfVyRJFhJM3253884 = -315439198;    int NmdjhAfVyRJFhJM78347702 = -286720275;    int NmdjhAfVyRJFhJM75665171 = -240073174;    int NmdjhAfVyRJFhJM5839119 = -990357897;    int NmdjhAfVyRJFhJM62208550 = -692465553;    int NmdjhAfVyRJFhJM46538800 = -873922998;    int NmdjhAfVyRJFhJM26023839 = -528927194;    int NmdjhAfVyRJFhJM41302984 = -2874053;    int NmdjhAfVyRJFhJM34737507 = -289146246;    int NmdjhAfVyRJFhJM64059659 = -892997751;    int NmdjhAfVyRJFhJM82278880 = -625211968;    int NmdjhAfVyRJFhJM71365175 = -331110681;    int NmdjhAfVyRJFhJM38065019 = -91536738;    int NmdjhAfVyRJFhJM78784241 = -999205757;    int NmdjhAfVyRJFhJM71858335 = -238441764;    int NmdjhAfVyRJFhJM90148295 = -442929870;    int NmdjhAfVyRJFhJM18760236 = -698818007;    int NmdjhAfVyRJFhJM82678145 = -62232935;    int NmdjhAfVyRJFhJM94484930 = -175636480;    int NmdjhAfVyRJFhJM70404082 = -573636133;    int NmdjhAfVyRJFhJM24366762 = -353882923;    int NmdjhAfVyRJFhJM60606398 = -694613764;    int NmdjhAfVyRJFhJM56933944 = -682208881;    int NmdjhAfVyRJFhJM56959759 = -194207348;    int NmdjhAfVyRJFhJM26244247 = 37803937;    int NmdjhAfVyRJFhJM72777473 = -885672758;    int NmdjhAfVyRJFhJM34837818 = -153836440;    int NmdjhAfVyRJFhJM35957703 = -48707425;    int NmdjhAfVyRJFhJM62463622 = -48405710;    int NmdjhAfVyRJFhJM89650788 = -442124904;    int NmdjhAfVyRJFhJM53994107 = -802148713;    int NmdjhAfVyRJFhJM99417518 = -982047867;    int NmdjhAfVyRJFhJM86876395 = -182228263;    int NmdjhAfVyRJFhJM50329667 = 79510154;    int NmdjhAfVyRJFhJM74088924 = -809811461;    int NmdjhAfVyRJFhJM99463140 = -391096112;    int NmdjhAfVyRJFhJM49230161 = 11713853;    int NmdjhAfVyRJFhJM10715574 = -320096610;    int NmdjhAfVyRJFhJM42814040 = -278068532;    int NmdjhAfVyRJFhJM9838671 = -354486620;    int NmdjhAfVyRJFhJM4877150 = -686715596;    int NmdjhAfVyRJFhJM42609822 = -631768049;    int NmdjhAfVyRJFhJM52030552 = 13136583;    int NmdjhAfVyRJFhJM96328305 = -459704504;    int NmdjhAfVyRJFhJM20841917 = 71067798;    int NmdjhAfVyRJFhJM46721580 = -810346119;    int NmdjhAfVyRJFhJM52578963 = -472691024;    int NmdjhAfVyRJFhJM73779573 = -658466521;    int NmdjhAfVyRJFhJM73271749 = -74345652;    int NmdjhAfVyRJFhJM68875812 = -477555564;    int NmdjhAfVyRJFhJM19506208 = -38260833;    int NmdjhAfVyRJFhJM82688906 = -136292011;    int NmdjhAfVyRJFhJM75294073 = -647981348;    int NmdjhAfVyRJFhJM30150001 = -477473582;    int NmdjhAfVyRJFhJM91484417 = -128330967;    int NmdjhAfVyRJFhJM42647485 = -620825434;    int NmdjhAfVyRJFhJM21413759 = -604511394;    int NmdjhAfVyRJFhJM18705412 = 54134173;    int NmdjhAfVyRJFhJM79594871 = -928161835;    int NmdjhAfVyRJFhJM89431077 = -806792795;    int NmdjhAfVyRJFhJM11700983 = -620086558;    int NmdjhAfVyRJFhJM90066135 = -380219769;    int NmdjhAfVyRJFhJM78839362 = -954468343;    int NmdjhAfVyRJFhJM45086719 = -847021343;    int NmdjhAfVyRJFhJM10065553 = 9150961;    int NmdjhAfVyRJFhJM82861361 = -643164102;    int NmdjhAfVyRJFhJM84488779 = -48882418;    int NmdjhAfVyRJFhJM87735351 = -71046893;    int NmdjhAfVyRJFhJM4695317 = -89394296;    int NmdjhAfVyRJFhJM72395194 = -847345652;    int NmdjhAfVyRJFhJM40918135 = -354643723;    int NmdjhAfVyRJFhJM8044663 = -278721397;    int NmdjhAfVyRJFhJM39864106 = -784164403;    int NmdjhAfVyRJFhJM84646259 = -821149860;    int NmdjhAfVyRJFhJM65526932 = -886920538;    int NmdjhAfVyRJFhJM81756940 = -722114874;    int NmdjhAfVyRJFhJM8575846 = -607750348;    int NmdjhAfVyRJFhJM60605638 = -122504378;    int NmdjhAfVyRJFhJM36117842 = -165275146;    int NmdjhAfVyRJFhJM79522667 = -151849944;    int NmdjhAfVyRJFhJM20198511 = -312981735;    int NmdjhAfVyRJFhJM61058244 = -495369920;    int NmdjhAfVyRJFhJM62685954 = -974361774;    int NmdjhAfVyRJFhJM93587810 = -570850146;    int NmdjhAfVyRJFhJM70144580 = -303864071;    int NmdjhAfVyRJFhJM71305201 = -565856703;    int NmdjhAfVyRJFhJM24123446 = -234066519;    int NmdjhAfVyRJFhJM56726394 = -704754681;    int NmdjhAfVyRJFhJM58845250 = -792158880;    int NmdjhAfVyRJFhJM31441439 = -88986027;    int NmdjhAfVyRJFhJM78049382 = -786584719;    int NmdjhAfVyRJFhJM30524749 = 57579680;    int NmdjhAfVyRJFhJM31120702 = -391934776;    int NmdjhAfVyRJFhJM53382962 = -471275737;    int NmdjhAfVyRJFhJM98137687 = -837078840;     NmdjhAfVyRJFhJM38266444 = NmdjhAfVyRJFhJM65367051;     NmdjhAfVyRJFhJM65367051 = NmdjhAfVyRJFhJM69779004;     NmdjhAfVyRJFhJM69779004 = NmdjhAfVyRJFhJM554084;     NmdjhAfVyRJFhJM554084 = NmdjhAfVyRJFhJM15851180;     NmdjhAfVyRJFhJM15851180 = NmdjhAfVyRJFhJM3253884;     NmdjhAfVyRJFhJM3253884 = NmdjhAfVyRJFhJM78347702;     NmdjhAfVyRJFhJM78347702 = NmdjhAfVyRJFhJM75665171;     NmdjhAfVyRJFhJM75665171 = NmdjhAfVyRJFhJM5839119;     NmdjhAfVyRJFhJM5839119 = NmdjhAfVyRJFhJM62208550;     NmdjhAfVyRJFhJM62208550 = NmdjhAfVyRJFhJM46538800;     NmdjhAfVyRJFhJM46538800 = NmdjhAfVyRJFhJM26023839;     NmdjhAfVyRJFhJM26023839 = NmdjhAfVyRJFhJM41302984;     NmdjhAfVyRJFhJM41302984 = NmdjhAfVyRJFhJM34737507;     NmdjhAfVyRJFhJM34737507 = NmdjhAfVyRJFhJM64059659;     NmdjhAfVyRJFhJM64059659 = NmdjhAfVyRJFhJM82278880;     NmdjhAfVyRJFhJM82278880 = NmdjhAfVyRJFhJM71365175;     NmdjhAfVyRJFhJM71365175 = NmdjhAfVyRJFhJM38065019;     NmdjhAfVyRJFhJM38065019 = NmdjhAfVyRJFhJM78784241;     NmdjhAfVyRJFhJM78784241 = NmdjhAfVyRJFhJM71858335;     NmdjhAfVyRJFhJM71858335 = NmdjhAfVyRJFhJM90148295;     NmdjhAfVyRJFhJM90148295 = NmdjhAfVyRJFhJM18760236;     NmdjhAfVyRJFhJM18760236 = NmdjhAfVyRJFhJM82678145;     NmdjhAfVyRJFhJM82678145 = NmdjhAfVyRJFhJM94484930;     NmdjhAfVyRJFhJM94484930 = NmdjhAfVyRJFhJM70404082;     NmdjhAfVyRJFhJM70404082 = NmdjhAfVyRJFhJM24366762;     NmdjhAfVyRJFhJM24366762 = NmdjhAfVyRJFhJM60606398;     NmdjhAfVyRJFhJM60606398 = NmdjhAfVyRJFhJM56933944;     NmdjhAfVyRJFhJM56933944 = NmdjhAfVyRJFhJM56959759;     NmdjhAfVyRJFhJM56959759 = NmdjhAfVyRJFhJM26244247;     NmdjhAfVyRJFhJM26244247 = NmdjhAfVyRJFhJM72777473;     NmdjhAfVyRJFhJM72777473 = NmdjhAfVyRJFhJM34837818;     NmdjhAfVyRJFhJM34837818 = NmdjhAfVyRJFhJM35957703;     NmdjhAfVyRJFhJM35957703 = NmdjhAfVyRJFhJM62463622;     NmdjhAfVyRJFhJM62463622 = NmdjhAfVyRJFhJM89650788;     NmdjhAfVyRJFhJM89650788 = NmdjhAfVyRJFhJM53994107;     NmdjhAfVyRJFhJM53994107 = NmdjhAfVyRJFhJM99417518;     NmdjhAfVyRJFhJM99417518 = NmdjhAfVyRJFhJM86876395;     NmdjhAfVyRJFhJM86876395 = NmdjhAfVyRJFhJM50329667;     NmdjhAfVyRJFhJM50329667 = NmdjhAfVyRJFhJM74088924;     NmdjhAfVyRJFhJM74088924 = NmdjhAfVyRJFhJM99463140;     NmdjhAfVyRJFhJM99463140 = NmdjhAfVyRJFhJM49230161;     NmdjhAfVyRJFhJM49230161 = NmdjhAfVyRJFhJM10715574;     NmdjhAfVyRJFhJM10715574 = NmdjhAfVyRJFhJM42814040;     NmdjhAfVyRJFhJM42814040 = NmdjhAfVyRJFhJM9838671;     NmdjhAfVyRJFhJM9838671 = NmdjhAfVyRJFhJM4877150;     NmdjhAfVyRJFhJM4877150 = NmdjhAfVyRJFhJM42609822;     NmdjhAfVyRJFhJM42609822 = NmdjhAfVyRJFhJM52030552;     NmdjhAfVyRJFhJM52030552 = NmdjhAfVyRJFhJM96328305;     NmdjhAfVyRJFhJM96328305 = NmdjhAfVyRJFhJM20841917;     NmdjhAfVyRJFhJM20841917 = NmdjhAfVyRJFhJM46721580;     NmdjhAfVyRJFhJM46721580 = NmdjhAfVyRJFhJM52578963;     NmdjhAfVyRJFhJM52578963 = NmdjhAfVyRJFhJM73779573;     NmdjhAfVyRJFhJM73779573 = NmdjhAfVyRJFhJM73271749;     NmdjhAfVyRJFhJM73271749 = NmdjhAfVyRJFhJM68875812;     NmdjhAfVyRJFhJM68875812 = NmdjhAfVyRJFhJM19506208;     NmdjhAfVyRJFhJM19506208 = NmdjhAfVyRJFhJM82688906;     NmdjhAfVyRJFhJM82688906 = NmdjhAfVyRJFhJM75294073;     NmdjhAfVyRJFhJM75294073 = NmdjhAfVyRJFhJM30150001;     NmdjhAfVyRJFhJM30150001 = NmdjhAfVyRJFhJM91484417;     NmdjhAfVyRJFhJM91484417 = NmdjhAfVyRJFhJM42647485;     NmdjhAfVyRJFhJM42647485 = NmdjhAfVyRJFhJM21413759;     NmdjhAfVyRJFhJM21413759 = NmdjhAfVyRJFhJM18705412;     NmdjhAfVyRJFhJM18705412 = NmdjhAfVyRJFhJM79594871;     NmdjhAfVyRJFhJM79594871 = NmdjhAfVyRJFhJM89431077;     NmdjhAfVyRJFhJM89431077 = NmdjhAfVyRJFhJM11700983;     NmdjhAfVyRJFhJM11700983 = NmdjhAfVyRJFhJM90066135;     NmdjhAfVyRJFhJM90066135 = NmdjhAfVyRJFhJM78839362;     NmdjhAfVyRJFhJM78839362 = NmdjhAfVyRJFhJM45086719;     NmdjhAfVyRJFhJM45086719 = NmdjhAfVyRJFhJM10065553;     NmdjhAfVyRJFhJM10065553 = NmdjhAfVyRJFhJM82861361;     NmdjhAfVyRJFhJM82861361 = NmdjhAfVyRJFhJM84488779;     NmdjhAfVyRJFhJM84488779 = NmdjhAfVyRJFhJM87735351;     NmdjhAfVyRJFhJM87735351 = NmdjhAfVyRJFhJM4695317;     NmdjhAfVyRJFhJM4695317 = NmdjhAfVyRJFhJM72395194;     NmdjhAfVyRJFhJM72395194 = NmdjhAfVyRJFhJM40918135;     NmdjhAfVyRJFhJM40918135 = NmdjhAfVyRJFhJM8044663;     NmdjhAfVyRJFhJM8044663 = NmdjhAfVyRJFhJM39864106;     NmdjhAfVyRJFhJM39864106 = NmdjhAfVyRJFhJM84646259;     NmdjhAfVyRJFhJM84646259 = NmdjhAfVyRJFhJM65526932;     NmdjhAfVyRJFhJM65526932 = NmdjhAfVyRJFhJM81756940;     NmdjhAfVyRJFhJM81756940 = NmdjhAfVyRJFhJM8575846;     NmdjhAfVyRJFhJM8575846 = NmdjhAfVyRJFhJM60605638;     NmdjhAfVyRJFhJM60605638 = NmdjhAfVyRJFhJM36117842;     NmdjhAfVyRJFhJM36117842 = NmdjhAfVyRJFhJM79522667;     NmdjhAfVyRJFhJM79522667 = NmdjhAfVyRJFhJM20198511;     NmdjhAfVyRJFhJM20198511 = NmdjhAfVyRJFhJM61058244;     NmdjhAfVyRJFhJM61058244 = NmdjhAfVyRJFhJM62685954;     NmdjhAfVyRJFhJM62685954 = NmdjhAfVyRJFhJM93587810;     NmdjhAfVyRJFhJM93587810 = NmdjhAfVyRJFhJM70144580;     NmdjhAfVyRJFhJM70144580 = NmdjhAfVyRJFhJM71305201;     NmdjhAfVyRJFhJM71305201 = NmdjhAfVyRJFhJM24123446;     NmdjhAfVyRJFhJM24123446 = NmdjhAfVyRJFhJM56726394;     NmdjhAfVyRJFhJM56726394 = NmdjhAfVyRJFhJM58845250;     NmdjhAfVyRJFhJM58845250 = NmdjhAfVyRJFhJM31441439;     NmdjhAfVyRJFhJM31441439 = NmdjhAfVyRJFhJM78049382;     NmdjhAfVyRJFhJM78049382 = NmdjhAfVyRJFhJM30524749;     NmdjhAfVyRJFhJM30524749 = NmdjhAfVyRJFhJM31120702;     NmdjhAfVyRJFhJM31120702 = NmdjhAfVyRJFhJM53382962;     NmdjhAfVyRJFhJM53382962 = NmdjhAfVyRJFhJM98137687;     NmdjhAfVyRJFhJM98137687 = NmdjhAfVyRJFhJM38266444;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void WLIvaZsCbNJHsbhj13440321() {     float EOUgORtAlXlYIfx8918111 = -355227968;    float EOUgORtAlXlYIfx34040569 = -170168238;    float EOUgORtAlXlYIfx49747923 = -265912288;    float EOUgORtAlXlYIfx40485370 = -241248107;    float EOUgORtAlXlYIfx37301501 = -365620228;    float EOUgORtAlXlYIfx93253646 = -17020349;    float EOUgORtAlXlYIfx96726598 = -285495984;    float EOUgORtAlXlYIfx32507552 = -586890248;    float EOUgORtAlXlYIfx51704173 = 16294775;    float EOUgORtAlXlYIfx85175011 = -993560903;    float EOUgORtAlXlYIfx88787961 = -908824926;    float EOUgORtAlXlYIfx71570409 = -844690978;    float EOUgORtAlXlYIfx21130372 = 87588889;    float EOUgORtAlXlYIfx18584541 = -90174455;    float EOUgORtAlXlYIfx46779679 = 57252264;    float EOUgORtAlXlYIfx14807520 = -195195555;    float EOUgORtAlXlYIfx65218001 = -240295810;    float EOUgORtAlXlYIfx58358513 = -80702930;    float EOUgORtAlXlYIfx60492022 = -606393700;    float EOUgORtAlXlYIfx30645044 = -780808069;    float EOUgORtAlXlYIfx45284050 = -351609005;    float EOUgORtAlXlYIfx80156000 = 69896687;    float EOUgORtAlXlYIfx84621953 = -630076079;    float EOUgORtAlXlYIfx28334468 = -868037386;    float EOUgORtAlXlYIfx34528471 = -249540809;    float EOUgORtAlXlYIfx54964342 = -452167372;    float EOUgORtAlXlYIfx68867184 = 10208460;    float EOUgORtAlXlYIfx64716169 = -87332643;    float EOUgORtAlXlYIfx89102247 = -488565580;    float EOUgORtAlXlYIfx65416343 = -520591133;    float EOUgORtAlXlYIfx79849469 = -634106444;    float EOUgORtAlXlYIfx79349278 = -482133606;    float EOUgORtAlXlYIfx18903064 = 4330963;    float EOUgORtAlXlYIfx27828597 = -711935394;    float EOUgORtAlXlYIfx67675024 = -289071686;    float EOUgORtAlXlYIfx13987366 = -881375279;    float EOUgORtAlXlYIfx43209005 = -70641074;    float EOUgORtAlXlYIfx39668884 = 33658633;    float EOUgORtAlXlYIfx28862956 = -976218495;    float EOUgORtAlXlYIfx69192223 = -327419823;    float EOUgORtAlXlYIfx59536424 = -775114991;    float EOUgORtAlXlYIfx77713151 = -857675237;    float EOUgORtAlXlYIfx59682534 = -306850664;    float EOUgORtAlXlYIfx49322340 = -350043503;    float EOUgORtAlXlYIfx5792730 = -663133016;    float EOUgORtAlXlYIfx11282985 = -445052770;    float EOUgORtAlXlYIfx68741025 = -747837849;    float EOUgORtAlXlYIfx17644763 = -438993713;    float EOUgORtAlXlYIfx1769759 = -445468497;    float EOUgORtAlXlYIfx29269404 = -211941075;    float EOUgORtAlXlYIfx14910904 = -356918240;    float EOUgORtAlXlYIfx94450467 = -441910750;    float EOUgORtAlXlYIfx28855231 = -586264560;    float EOUgORtAlXlYIfx63499054 = -735227149;    float EOUgORtAlXlYIfx68156270 = -588496483;    float EOUgORtAlXlYIfx28762110 = -325124656;    float EOUgORtAlXlYIfx49418615 = -540092159;    float EOUgORtAlXlYIfx21413456 = -397874902;    float EOUgORtAlXlYIfx5956900 = -991707299;    float EOUgORtAlXlYIfx82337159 = -913452857;    float EOUgORtAlXlYIfx24386462 = 72771190;    float EOUgORtAlXlYIfx32010429 = -98163342;    float EOUgORtAlXlYIfx43405304 = 1675331;    float EOUgORtAlXlYIfx86287830 = -463114092;    float EOUgORtAlXlYIfx5325542 = -259454460;    float EOUgORtAlXlYIfx9438683 = -326691320;    float EOUgORtAlXlYIfx52667345 = -749021941;    float EOUgORtAlXlYIfx93301774 = -200475717;    float EOUgORtAlXlYIfx50909516 = -801102769;    float EOUgORtAlXlYIfx32792313 = -61372457;    float EOUgORtAlXlYIfx71598515 = -24554481;    float EOUgORtAlXlYIfx25549118 = -173954444;    float EOUgORtAlXlYIfx29495557 = -104484436;    float EOUgORtAlXlYIfx91299799 = -178973877;    float EOUgORtAlXlYIfx71108620 = 94306922;    float EOUgORtAlXlYIfx67570898 = -493933769;    float EOUgORtAlXlYIfx20473467 = -623252649;    float EOUgORtAlXlYIfx35299613 = -180032577;    float EOUgORtAlXlYIfx22541738 = -104904371;    float EOUgORtAlXlYIfx23245486 = -804488040;    float EOUgORtAlXlYIfx86223316 = -704329523;    float EOUgORtAlXlYIfx51222422 = -550797827;    float EOUgORtAlXlYIfx62946411 = -641864146;    float EOUgORtAlXlYIfx59832843 = -176624505;    float EOUgORtAlXlYIfx50505439 = -63672894;    float EOUgORtAlXlYIfx85399002 = -92195695;    float EOUgORtAlXlYIfx50494047 = -895869047;    float EOUgORtAlXlYIfx55404010 = -260441889;    float EOUgORtAlXlYIfx59672327 = -23438911;    float EOUgORtAlXlYIfx38912915 = -963947031;    float EOUgORtAlXlYIfx64568750 = -241283121;    float EOUgORtAlXlYIfx21795549 = -672766173;    float EOUgORtAlXlYIfx33711985 = 25365931;    float EOUgORtAlXlYIfx46525797 = 37234362;    float EOUgORtAlXlYIfx44805761 = -300191014;    float EOUgORtAlXlYIfx27525996 = -576951649;    float EOUgORtAlXlYIfx34307848 = -759350568;    float EOUgORtAlXlYIfx73394704 = -843736573;    float EOUgORtAlXlYIfx43996799 = 9410956;    float EOUgORtAlXlYIfx96354046 = -355227968;     EOUgORtAlXlYIfx8918111 = EOUgORtAlXlYIfx34040569;     EOUgORtAlXlYIfx34040569 = EOUgORtAlXlYIfx49747923;     EOUgORtAlXlYIfx49747923 = EOUgORtAlXlYIfx40485370;     EOUgORtAlXlYIfx40485370 = EOUgORtAlXlYIfx37301501;     EOUgORtAlXlYIfx37301501 = EOUgORtAlXlYIfx93253646;     EOUgORtAlXlYIfx93253646 = EOUgORtAlXlYIfx96726598;     EOUgORtAlXlYIfx96726598 = EOUgORtAlXlYIfx32507552;     EOUgORtAlXlYIfx32507552 = EOUgORtAlXlYIfx51704173;     EOUgORtAlXlYIfx51704173 = EOUgORtAlXlYIfx85175011;     EOUgORtAlXlYIfx85175011 = EOUgORtAlXlYIfx88787961;     EOUgORtAlXlYIfx88787961 = EOUgORtAlXlYIfx71570409;     EOUgORtAlXlYIfx71570409 = EOUgORtAlXlYIfx21130372;     EOUgORtAlXlYIfx21130372 = EOUgORtAlXlYIfx18584541;     EOUgORtAlXlYIfx18584541 = EOUgORtAlXlYIfx46779679;     EOUgORtAlXlYIfx46779679 = EOUgORtAlXlYIfx14807520;     EOUgORtAlXlYIfx14807520 = EOUgORtAlXlYIfx65218001;     EOUgORtAlXlYIfx65218001 = EOUgORtAlXlYIfx58358513;     EOUgORtAlXlYIfx58358513 = EOUgORtAlXlYIfx60492022;     EOUgORtAlXlYIfx60492022 = EOUgORtAlXlYIfx30645044;     EOUgORtAlXlYIfx30645044 = EOUgORtAlXlYIfx45284050;     EOUgORtAlXlYIfx45284050 = EOUgORtAlXlYIfx80156000;     EOUgORtAlXlYIfx80156000 = EOUgORtAlXlYIfx84621953;     EOUgORtAlXlYIfx84621953 = EOUgORtAlXlYIfx28334468;     EOUgORtAlXlYIfx28334468 = EOUgORtAlXlYIfx34528471;     EOUgORtAlXlYIfx34528471 = EOUgORtAlXlYIfx54964342;     EOUgORtAlXlYIfx54964342 = EOUgORtAlXlYIfx68867184;     EOUgORtAlXlYIfx68867184 = EOUgORtAlXlYIfx64716169;     EOUgORtAlXlYIfx64716169 = EOUgORtAlXlYIfx89102247;     EOUgORtAlXlYIfx89102247 = EOUgORtAlXlYIfx65416343;     EOUgORtAlXlYIfx65416343 = EOUgORtAlXlYIfx79849469;     EOUgORtAlXlYIfx79849469 = EOUgORtAlXlYIfx79349278;     EOUgORtAlXlYIfx79349278 = EOUgORtAlXlYIfx18903064;     EOUgORtAlXlYIfx18903064 = EOUgORtAlXlYIfx27828597;     EOUgORtAlXlYIfx27828597 = EOUgORtAlXlYIfx67675024;     EOUgORtAlXlYIfx67675024 = EOUgORtAlXlYIfx13987366;     EOUgORtAlXlYIfx13987366 = EOUgORtAlXlYIfx43209005;     EOUgORtAlXlYIfx43209005 = EOUgORtAlXlYIfx39668884;     EOUgORtAlXlYIfx39668884 = EOUgORtAlXlYIfx28862956;     EOUgORtAlXlYIfx28862956 = EOUgORtAlXlYIfx69192223;     EOUgORtAlXlYIfx69192223 = EOUgORtAlXlYIfx59536424;     EOUgORtAlXlYIfx59536424 = EOUgORtAlXlYIfx77713151;     EOUgORtAlXlYIfx77713151 = EOUgORtAlXlYIfx59682534;     EOUgORtAlXlYIfx59682534 = EOUgORtAlXlYIfx49322340;     EOUgORtAlXlYIfx49322340 = EOUgORtAlXlYIfx5792730;     EOUgORtAlXlYIfx5792730 = EOUgORtAlXlYIfx11282985;     EOUgORtAlXlYIfx11282985 = EOUgORtAlXlYIfx68741025;     EOUgORtAlXlYIfx68741025 = EOUgORtAlXlYIfx17644763;     EOUgORtAlXlYIfx17644763 = EOUgORtAlXlYIfx1769759;     EOUgORtAlXlYIfx1769759 = EOUgORtAlXlYIfx29269404;     EOUgORtAlXlYIfx29269404 = EOUgORtAlXlYIfx14910904;     EOUgORtAlXlYIfx14910904 = EOUgORtAlXlYIfx94450467;     EOUgORtAlXlYIfx94450467 = EOUgORtAlXlYIfx28855231;     EOUgORtAlXlYIfx28855231 = EOUgORtAlXlYIfx63499054;     EOUgORtAlXlYIfx63499054 = EOUgORtAlXlYIfx68156270;     EOUgORtAlXlYIfx68156270 = EOUgORtAlXlYIfx28762110;     EOUgORtAlXlYIfx28762110 = EOUgORtAlXlYIfx49418615;     EOUgORtAlXlYIfx49418615 = EOUgORtAlXlYIfx21413456;     EOUgORtAlXlYIfx21413456 = EOUgORtAlXlYIfx5956900;     EOUgORtAlXlYIfx5956900 = EOUgORtAlXlYIfx82337159;     EOUgORtAlXlYIfx82337159 = EOUgORtAlXlYIfx24386462;     EOUgORtAlXlYIfx24386462 = EOUgORtAlXlYIfx32010429;     EOUgORtAlXlYIfx32010429 = EOUgORtAlXlYIfx43405304;     EOUgORtAlXlYIfx43405304 = EOUgORtAlXlYIfx86287830;     EOUgORtAlXlYIfx86287830 = EOUgORtAlXlYIfx5325542;     EOUgORtAlXlYIfx5325542 = EOUgORtAlXlYIfx9438683;     EOUgORtAlXlYIfx9438683 = EOUgORtAlXlYIfx52667345;     EOUgORtAlXlYIfx52667345 = EOUgORtAlXlYIfx93301774;     EOUgORtAlXlYIfx93301774 = EOUgORtAlXlYIfx50909516;     EOUgORtAlXlYIfx50909516 = EOUgORtAlXlYIfx32792313;     EOUgORtAlXlYIfx32792313 = EOUgORtAlXlYIfx71598515;     EOUgORtAlXlYIfx71598515 = EOUgORtAlXlYIfx25549118;     EOUgORtAlXlYIfx25549118 = EOUgORtAlXlYIfx29495557;     EOUgORtAlXlYIfx29495557 = EOUgORtAlXlYIfx91299799;     EOUgORtAlXlYIfx91299799 = EOUgORtAlXlYIfx71108620;     EOUgORtAlXlYIfx71108620 = EOUgORtAlXlYIfx67570898;     EOUgORtAlXlYIfx67570898 = EOUgORtAlXlYIfx20473467;     EOUgORtAlXlYIfx20473467 = EOUgORtAlXlYIfx35299613;     EOUgORtAlXlYIfx35299613 = EOUgORtAlXlYIfx22541738;     EOUgORtAlXlYIfx22541738 = EOUgORtAlXlYIfx23245486;     EOUgORtAlXlYIfx23245486 = EOUgORtAlXlYIfx86223316;     EOUgORtAlXlYIfx86223316 = EOUgORtAlXlYIfx51222422;     EOUgORtAlXlYIfx51222422 = EOUgORtAlXlYIfx62946411;     EOUgORtAlXlYIfx62946411 = EOUgORtAlXlYIfx59832843;     EOUgORtAlXlYIfx59832843 = EOUgORtAlXlYIfx50505439;     EOUgORtAlXlYIfx50505439 = EOUgORtAlXlYIfx85399002;     EOUgORtAlXlYIfx85399002 = EOUgORtAlXlYIfx50494047;     EOUgORtAlXlYIfx50494047 = EOUgORtAlXlYIfx55404010;     EOUgORtAlXlYIfx55404010 = EOUgORtAlXlYIfx59672327;     EOUgORtAlXlYIfx59672327 = EOUgORtAlXlYIfx38912915;     EOUgORtAlXlYIfx38912915 = EOUgORtAlXlYIfx64568750;     EOUgORtAlXlYIfx64568750 = EOUgORtAlXlYIfx21795549;     EOUgORtAlXlYIfx21795549 = EOUgORtAlXlYIfx33711985;     EOUgORtAlXlYIfx33711985 = EOUgORtAlXlYIfx46525797;     EOUgORtAlXlYIfx46525797 = EOUgORtAlXlYIfx44805761;     EOUgORtAlXlYIfx44805761 = EOUgORtAlXlYIfx27525996;     EOUgORtAlXlYIfx27525996 = EOUgORtAlXlYIfx34307848;     EOUgORtAlXlYIfx34307848 = EOUgORtAlXlYIfx73394704;     EOUgORtAlXlYIfx73394704 = EOUgORtAlXlYIfx43996799;     EOUgORtAlXlYIfx43996799 = EOUgORtAlXlYIfx96354046;     EOUgORtAlXlYIfx96354046 = EOUgORtAlXlYIfx8918111;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void uvwGWCtCuoLgyAmj89452440() {     float LmNiQNVdbveHsmu95839788 = -495879175;    float LmNiQNVdbveHsmu43688307 = -280754817;    float LmNiQNVdbveHsmu42443937 = -797027670;    float LmNiQNVdbveHsmu68348564 = -247045395;    float LmNiQNVdbveHsmu88030588 = -219433714;    float LmNiQNVdbveHsmu6719324 = -414713644;    float LmNiQNVdbveHsmu46772267 = 32705977;    float LmNiQNVdbveHsmu15223900 = -17430085;    float LmNiQNVdbveHsmu30089863 = -193215928;    float LmNiQNVdbveHsmu58675397 = -708422109;    float LmNiQNVdbveHsmu8468692 = 37086329;    float LmNiQNVdbveHsmu41196887 = -966793871;    float LmNiQNVdbveHsmu81756820 = -774400;    float LmNiQNVdbveHsmu61902649 = -986672855;    float LmNiQNVdbveHsmu70492549 = -129513304;    float LmNiQNVdbveHsmu50086679 = -173847141;    float LmNiQNVdbveHsmu15058177 = -910215647;    float LmNiQNVdbveHsmu87575270 = -856089178;    float LmNiQNVdbveHsmu21651594 = 57085331;    float LmNiQNVdbveHsmu87397871 = -118905908;    float LmNiQNVdbveHsmu12749718 = -396288003;    float LmNiQNVdbveHsmu18845960 = -858225215;    float LmNiQNVdbveHsmu54268294 = -527482329;    float LmNiQNVdbveHsmu4691418 = -802126162;    float LmNiQNVdbveHsmu90493456 = -273808601;    float LmNiQNVdbveHsmu49143242 = -15690713;    float LmNiQNVdbveHsmu25757612 = -241893604;    float LmNiQNVdbveHsmu65302720 = -314981916;    float LmNiQNVdbveHsmu62005040 = -904798196;    float LmNiQNVdbveHsmu45129982 = -81811203;    float LmNiQNVdbveHsmu38969722 = -550535343;    float LmNiQNVdbveHsmu83791308 = -653055305;    float LmNiQNVdbveHsmu73176147 = -579360768;    float LmNiQNVdbveHsmu57751695 = -848050163;    float LmNiQNVdbveHsmu60453973 = -661041780;    float LmNiQNVdbveHsmu46947581 = 38028923;    float LmNiQNVdbveHsmu99178267 = -38546364;    float LmNiQNVdbveHsmu11997261 = -946573940;    float LmNiQNVdbveHsmu82094122 = -239069654;    float LmNiQNVdbveHsmu96979297 = -607812993;    float LmNiQNVdbveHsmu42697855 = -497037641;    float LmNiQNVdbveHsmu62554077 = -114830699;    float LmNiQNVdbveHsmu36132707 = -100330714;    float LmNiQNVdbveHsmu67867019 = -400039186;    float LmNiQNVdbveHsmu57777590 = -19024733;    float LmNiQNVdbveHsmu13849907 = -400402885;    float LmNiQNVdbveHsmu89391051 = -223542363;    float LmNiQNVdbveHsmu19900706 = -637763466;    float LmNiQNVdbveHsmu91995386 = -295212152;    float LmNiQNVdbveHsmu21868983 = -549959410;    float LmNiQNVdbveHsmu56268767 = -416272967;    float LmNiQNVdbveHsmu99363509 = 58776571;    float LmNiQNVdbveHsmu13996834 = -394184248;    float LmNiQNVdbveHsmu18044024 = -251319309;    float LmNiQNVdbveHsmu76702722 = -139609725;    float LmNiQNVdbveHsmu76993828 = -637653960;    float LmNiQNVdbveHsmu89420013 = -753272488;    float LmNiQNVdbveHsmu37752519 = -994901509;    float LmNiQNVdbveHsmu77855108 = -973236795;    float LmNiQNVdbveHsmu38887346 = -103743001;    float LmNiQNVdbveHsmu80961711 = -72820040;    float LmNiQNVdbveHsmu81469547 = -652312108;    float LmNiQNVdbveHsmu53218860 = -112631889;    float LmNiQNVdbveHsmu84959881 = -11404725;    float LmNiQNVdbveHsmu19705676 = -57886766;    float LmNiQNVdbveHsmu24677383 = -309858366;    float LmNiQNVdbveHsmu68020740 = -287433104;    float LmNiQNVdbveHsmu24005125 = -152724237;    float LmNiQNVdbveHsmu1448677 = -225631076;    float LmNiQNVdbveHsmu23544968 = -67542227;    float LmNiQNVdbveHsmu50908411 = -35300778;    float LmNiQNVdbveHsmu3060916 = -963641708;    float LmNiQNVdbveHsmu5481149 = -517019524;    float LmNiQNVdbveHsmu24672296 = -335101677;    float LmNiQNVdbveHsmu44700017 = -621868267;    float LmNiQNVdbveHsmu50195641 = -181457305;    float LmNiQNVdbveHsmu82713252 = -657894501;    float LmNiQNVdbveHsmu86401274 = -27443144;    float LmNiQNVdbveHsmu46913828 = -683101430;    float LmNiQNVdbveHsmu76643549 = -873405716;    float LmNiQNVdbveHsmu59752191 = -792148350;    float LmNiQNVdbveHsmu5856907 = -604130139;    float LmNiQNVdbveHsmu73307333 = 80230236;    float LmNiQNVdbveHsmu40136058 = -254838787;    float LmNiQNVdbveHsmu88861215 = -665538236;    float LmNiQNVdbveHsmu39606213 = -509311915;    float LmNiQNVdbveHsmu69794474 = -158871058;    float LmNiQNVdbveHsmu55132123 = -228041459;    float LmNiQNVdbveHsmu81048973 = -608440438;    float LmNiQNVdbveHsmu83460145 = 76612180;    float LmNiQNVdbveHsmu57527568 = -208698589;    float LmNiQNVdbveHsmu61425749 = -43644855;    float LmNiQNVdbveHsmu34142153 = -973337146;    float LmNiQNVdbveHsmu43206777 = -35326653;    float LmNiQNVdbveHsmu16017587 = -434992953;    float LmNiQNVdbveHsmu61228308 = -844725534;    float LmNiQNVdbveHsmu9335218 = 97801190;    float LmNiQNVdbveHsmu51172826 = 11074011;    float LmNiQNVdbveHsmu48161344 = -242152420;    float LmNiQNVdbveHsmu33100207 = -495879175;     LmNiQNVdbveHsmu95839788 = LmNiQNVdbveHsmu43688307;     LmNiQNVdbveHsmu43688307 = LmNiQNVdbveHsmu42443937;     LmNiQNVdbveHsmu42443937 = LmNiQNVdbveHsmu68348564;     LmNiQNVdbveHsmu68348564 = LmNiQNVdbveHsmu88030588;     LmNiQNVdbveHsmu88030588 = LmNiQNVdbveHsmu6719324;     LmNiQNVdbveHsmu6719324 = LmNiQNVdbveHsmu46772267;     LmNiQNVdbveHsmu46772267 = LmNiQNVdbveHsmu15223900;     LmNiQNVdbveHsmu15223900 = LmNiQNVdbveHsmu30089863;     LmNiQNVdbveHsmu30089863 = LmNiQNVdbveHsmu58675397;     LmNiQNVdbveHsmu58675397 = LmNiQNVdbveHsmu8468692;     LmNiQNVdbveHsmu8468692 = LmNiQNVdbveHsmu41196887;     LmNiQNVdbveHsmu41196887 = LmNiQNVdbveHsmu81756820;     LmNiQNVdbveHsmu81756820 = LmNiQNVdbveHsmu61902649;     LmNiQNVdbveHsmu61902649 = LmNiQNVdbveHsmu70492549;     LmNiQNVdbveHsmu70492549 = LmNiQNVdbveHsmu50086679;     LmNiQNVdbveHsmu50086679 = LmNiQNVdbveHsmu15058177;     LmNiQNVdbveHsmu15058177 = LmNiQNVdbveHsmu87575270;     LmNiQNVdbveHsmu87575270 = LmNiQNVdbveHsmu21651594;     LmNiQNVdbveHsmu21651594 = LmNiQNVdbveHsmu87397871;     LmNiQNVdbveHsmu87397871 = LmNiQNVdbveHsmu12749718;     LmNiQNVdbveHsmu12749718 = LmNiQNVdbveHsmu18845960;     LmNiQNVdbveHsmu18845960 = LmNiQNVdbveHsmu54268294;     LmNiQNVdbveHsmu54268294 = LmNiQNVdbveHsmu4691418;     LmNiQNVdbveHsmu4691418 = LmNiQNVdbveHsmu90493456;     LmNiQNVdbveHsmu90493456 = LmNiQNVdbveHsmu49143242;     LmNiQNVdbveHsmu49143242 = LmNiQNVdbveHsmu25757612;     LmNiQNVdbveHsmu25757612 = LmNiQNVdbveHsmu65302720;     LmNiQNVdbveHsmu65302720 = LmNiQNVdbveHsmu62005040;     LmNiQNVdbveHsmu62005040 = LmNiQNVdbveHsmu45129982;     LmNiQNVdbveHsmu45129982 = LmNiQNVdbveHsmu38969722;     LmNiQNVdbveHsmu38969722 = LmNiQNVdbveHsmu83791308;     LmNiQNVdbveHsmu83791308 = LmNiQNVdbveHsmu73176147;     LmNiQNVdbveHsmu73176147 = LmNiQNVdbveHsmu57751695;     LmNiQNVdbveHsmu57751695 = LmNiQNVdbveHsmu60453973;     LmNiQNVdbveHsmu60453973 = LmNiQNVdbveHsmu46947581;     LmNiQNVdbveHsmu46947581 = LmNiQNVdbveHsmu99178267;     LmNiQNVdbveHsmu99178267 = LmNiQNVdbveHsmu11997261;     LmNiQNVdbveHsmu11997261 = LmNiQNVdbveHsmu82094122;     LmNiQNVdbveHsmu82094122 = LmNiQNVdbveHsmu96979297;     LmNiQNVdbveHsmu96979297 = LmNiQNVdbveHsmu42697855;     LmNiQNVdbveHsmu42697855 = LmNiQNVdbveHsmu62554077;     LmNiQNVdbveHsmu62554077 = LmNiQNVdbveHsmu36132707;     LmNiQNVdbveHsmu36132707 = LmNiQNVdbveHsmu67867019;     LmNiQNVdbveHsmu67867019 = LmNiQNVdbveHsmu57777590;     LmNiQNVdbveHsmu57777590 = LmNiQNVdbveHsmu13849907;     LmNiQNVdbveHsmu13849907 = LmNiQNVdbveHsmu89391051;     LmNiQNVdbveHsmu89391051 = LmNiQNVdbveHsmu19900706;     LmNiQNVdbveHsmu19900706 = LmNiQNVdbveHsmu91995386;     LmNiQNVdbveHsmu91995386 = LmNiQNVdbveHsmu21868983;     LmNiQNVdbveHsmu21868983 = LmNiQNVdbveHsmu56268767;     LmNiQNVdbveHsmu56268767 = LmNiQNVdbveHsmu99363509;     LmNiQNVdbveHsmu99363509 = LmNiQNVdbveHsmu13996834;     LmNiQNVdbveHsmu13996834 = LmNiQNVdbveHsmu18044024;     LmNiQNVdbveHsmu18044024 = LmNiQNVdbveHsmu76702722;     LmNiQNVdbveHsmu76702722 = LmNiQNVdbveHsmu76993828;     LmNiQNVdbveHsmu76993828 = LmNiQNVdbveHsmu89420013;     LmNiQNVdbveHsmu89420013 = LmNiQNVdbveHsmu37752519;     LmNiQNVdbveHsmu37752519 = LmNiQNVdbveHsmu77855108;     LmNiQNVdbveHsmu77855108 = LmNiQNVdbveHsmu38887346;     LmNiQNVdbveHsmu38887346 = LmNiQNVdbveHsmu80961711;     LmNiQNVdbveHsmu80961711 = LmNiQNVdbveHsmu81469547;     LmNiQNVdbveHsmu81469547 = LmNiQNVdbveHsmu53218860;     LmNiQNVdbveHsmu53218860 = LmNiQNVdbveHsmu84959881;     LmNiQNVdbveHsmu84959881 = LmNiQNVdbveHsmu19705676;     LmNiQNVdbveHsmu19705676 = LmNiQNVdbveHsmu24677383;     LmNiQNVdbveHsmu24677383 = LmNiQNVdbveHsmu68020740;     LmNiQNVdbveHsmu68020740 = LmNiQNVdbveHsmu24005125;     LmNiQNVdbveHsmu24005125 = LmNiQNVdbveHsmu1448677;     LmNiQNVdbveHsmu1448677 = LmNiQNVdbveHsmu23544968;     LmNiQNVdbveHsmu23544968 = LmNiQNVdbveHsmu50908411;     LmNiQNVdbveHsmu50908411 = LmNiQNVdbveHsmu3060916;     LmNiQNVdbveHsmu3060916 = LmNiQNVdbveHsmu5481149;     LmNiQNVdbveHsmu5481149 = LmNiQNVdbveHsmu24672296;     LmNiQNVdbveHsmu24672296 = LmNiQNVdbveHsmu44700017;     LmNiQNVdbveHsmu44700017 = LmNiQNVdbveHsmu50195641;     LmNiQNVdbveHsmu50195641 = LmNiQNVdbveHsmu82713252;     LmNiQNVdbveHsmu82713252 = LmNiQNVdbveHsmu86401274;     LmNiQNVdbveHsmu86401274 = LmNiQNVdbveHsmu46913828;     LmNiQNVdbveHsmu46913828 = LmNiQNVdbveHsmu76643549;     LmNiQNVdbveHsmu76643549 = LmNiQNVdbveHsmu59752191;     LmNiQNVdbveHsmu59752191 = LmNiQNVdbveHsmu5856907;     LmNiQNVdbveHsmu5856907 = LmNiQNVdbveHsmu73307333;     LmNiQNVdbveHsmu73307333 = LmNiQNVdbveHsmu40136058;     LmNiQNVdbveHsmu40136058 = LmNiQNVdbveHsmu88861215;     LmNiQNVdbveHsmu88861215 = LmNiQNVdbveHsmu39606213;     LmNiQNVdbveHsmu39606213 = LmNiQNVdbveHsmu69794474;     LmNiQNVdbveHsmu69794474 = LmNiQNVdbveHsmu55132123;     LmNiQNVdbveHsmu55132123 = LmNiQNVdbveHsmu81048973;     LmNiQNVdbveHsmu81048973 = LmNiQNVdbveHsmu83460145;     LmNiQNVdbveHsmu83460145 = LmNiQNVdbveHsmu57527568;     LmNiQNVdbveHsmu57527568 = LmNiQNVdbveHsmu61425749;     LmNiQNVdbveHsmu61425749 = LmNiQNVdbveHsmu34142153;     LmNiQNVdbveHsmu34142153 = LmNiQNVdbveHsmu43206777;     LmNiQNVdbveHsmu43206777 = LmNiQNVdbveHsmu16017587;     LmNiQNVdbveHsmu16017587 = LmNiQNVdbveHsmu61228308;     LmNiQNVdbveHsmu61228308 = LmNiQNVdbveHsmu9335218;     LmNiQNVdbveHsmu9335218 = LmNiQNVdbveHsmu51172826;     LmNiQNVdbveHsmu51172826 = LmNiQNVdbveHsmu48161344;     LmNiQNVdbveHsmu48161344 = LmNiQNVdbveHsmu33100207;     LmNiQNVdbveHsmu33100207 = LmNiQNVdbveHsmu95839788;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void WsecSYqdWFHCLXzK26427612() {     float uvVeCHDWYLgTlIX63566037 = -665279696;    float uvVeCHDWYLgTlIX19610505 = -792328183;    float uvVeCHDWYLgTlIX11793782 = -608425515;    float uvVeCHDWYLgTlIX97000172 = -67891345;    float uvVeCHDWYLgTlIX20593274 = -506410797;    float uvVeCHDWYLgTlIX42233476 = -945603997;    float uvVeCHDWYLgTlIX86374638 = -291925124;    float uvVeCHDWYLgTlIX30896706 = -791635814;    float uvVeCHDWYLgTlIX22218748 = -844204666;    float uvVeCHDWYLgTlIX64446510 = -909169044;    float uvVeCHDWYLgTlIX87719861 = -393653571;    float uvVeCHDWYLgTlIX48626800 = -766593438;    float uvVeCHDWYLgTlIX16800600 = -837609890;    float uvVeCHDWYLgTlIX65395191 = -47431086;    float uvVeCHDWYLgTlIX22175862 = -912582237;    float uvVeCHDWYLgTlIX79139603 = -158150770;    float uvVeCHDWYLgTlIX20902525 = -533510209;    float uvVeCHDWYLgTlIX48955633 = -541339023;    float uvVeCHDWYLgTlIX82140893 = -410187513;    float uvVeCHDWYLgTlIX20353363 = -638198666;    float uvVeCHDWYLgTlIX59127862 = -630158535;    float uvVeCHDWYLgTlIX9704856 = -606539304;    float uvVeCHDWYLgTlIX57970182 = -575777342;    float uvVeCHDWYLgTlIX73412944 = -447998725;    float uvVeCHDWYLgTlIX58723776 = 50851874;    float uvVeCHDWYLgTlIX12779367 = -364082081;    float uvVeCHDWYLgTlIX41078690 = -845435531;    float uvVeCHDWYLgTlIX53368695 = -704351498;    float uvVeCHDWYLgTlIX41678371 = -414880755;    float uvVeCHDWYLgTlIX35941622 = -486780475;    float uvVeCHDWYLgTlIX64967677 = -470072204;    float uvVeCHDWYLgTlIX95064745 = -185451893;    float uvVeCHDWYLgTlIX72011364 = -754130864;    float uvVeCHDWYLgTlIX22767327 = -268737936;    float uvVeCHDWYLgTlIX84537219 = -766248867;    float uvVeCHDWYLgTlIX44142098 = -109136603;    float uvVeCHDWYLgTlIX29854860 = -20721073;    float uvVeCHDWYLgTlIX22563803 = -935533070;    float uvVeCHDWYLgTlIX14946202 = -970848501;    float uvVeCHDWYLgTlIX65051510 = -695160109;    float uvVeCHDWYLgTlIX3351493 = -975609047;    float uvVeCHDWYLgTlIX85472 = -558999385;    float uvVeCHDWYLgTlIX10966077 = -706486234;    float uvVeCHDWYLgTlIX17548638 = -480687612;    float uvVeCHDWYLgTlIX49794845 = -168910014;    float uvVeCHDWYLgTlIX7044555 = -325887037;    float uvVeCHDWYLgTlIX12912529 = -156175855;    float uvVeCHDWYLgTlIX16455653 = 74592559;    float uvVeCHDWYLgTlIX62770914 = 91037477;    float uvVeCHDWYLgTlIX94156489 = -38985261;    float uvVeCHDWYLgTlIX3283814 = -692688749;    float uvVeCHDWYLgTlIX98379874 = -239122477;    float uvVeCHDWYLgTlIX36031582 = -218234029;    float uvVeCHDWYLgTlIX31165082 = -63033485;    float uvVeCHDWYLgTlIX67299750 = -741304437;    float uvVeCHDWYLgTlIX53861181 = 41259608;    float uvVeCHDWYLgTlIX61640322 = -116550841;    float uvVeCHDWYLgTlIX38380837 = -60426790;    float uvVeCHDWYLgTlIX38276396 = -18743220;    float uvVeCHDWYLgTlIX7813907 = -42328717;    float uvVeCHDWYLgTlIX1154786 = -168466;    float uvVeCHDWYLgTlIX33005944 = -587573627;    float uvVeCHDWYLgTlIX89218334 = -276755059;    float uvVeCHDWYLgTlIX86277125 = -257424192;    float uvVeCHDWYLgTlIX99478833 = -339096840;    float uvVeCHDWYLgTlIX92655115 = -108201679;    float uvVeCHDWYLgTlIX76615436 = 87537426;    float uvVeCHDWYLgTlIX94033273 = -468871954;    float uvVeCHDWYLgTlIX80857972 = -281182219;    float uvVeCHDWYLgTlIX78033763 = -703445635;    float uvVeCHDWYLgTlIX49284743 = -37429697;    float uvVeCHDWYLgTlIX98338722 = -597977139;    float uvVeCHDWYLgTlIX34009431 = -570490523;    float uvVeCHDWYLgTlIX17089384 = -715027405;    float uvVeCHDWYLgTlIX17001870 = -662589619;    float uvVeCHDWYLgTlIX59042391 = 28840850;    float uvVeCHDWYLgTlIX98738778 = -900053070;    float uvVeCHDWYLgTlIX40421544 = 4910269;    float uvVeCHDWYLgTlIX23618100 = -179088712;    float uvVeCHDWYLgTlIX51679222 = -623261090;    float uvVeCHDWYLgTlIX99866837 = -107906226;    float uvVeCHDWYLgTlIX24623038 = -820028091;    float uvVeCHDWYLgTlIX90597780 = -695388975;    float uvVeCHDWYLgTlIX47521882 = -275895494;    float uvVeCHDWYLgTlIX32657809 = -794091726;    float uvVeCHDWYLgTlIX66587803 = -130949728;    float uvVeCHDWYLgTlIX59033164 = -967217865;    float uvVeCHDWYLgTlIX40846282 = -591097380;    float uvVeCHDWYLgTlIX55467577 = -527433500;    float uvVeCHDWYLgTlIX30676038 = -707508475;    float uvVeCHDWYLgTlIX82501775 = -992585763;    float uvVeCHDWYLgTlIX91474023 = -960294284;    float uvVeCHDWYLgTlIX84287406 = -816789851;    float uvVeCHDWYLgTlIX7132295 = -828519784;    float uvVeCHDWYLgTlIX63896724 = -594991643;    float uvVeCHDWYLgTlIX70345549 = -288035421;    float uvVeCHDWYLgTlIX10867137 = -182244326;    float uvVeCHDWYLgTlIX24688951 = -349062043;    float uvVeCHDWYLgTlIX18069805 = -41590773;    float uvVeCHDWYLgTlIX57139730 = -665279696;     uvVeCHDWYLgTlIX63566037 = uvVeCHDWYLgTlIX19610505;     uvVeCHDWYLgTlIX19610505 = uvVeCHDWYLgTlIX11793782;     uvVeCHDWYLgTlIX11793782 = uvVeCHDWYLgTlIX97000172;     uvVeCHDWYLgTlIX97000172 = uvVeCHDWYLgTlIX20593274;     uvVeCHDWYLgTlIX20593274 = uvVeCHDWYLgTlIX42233476;     uvVeCHDWYLgTlIX42233476 = uvVeCHDWYLgTlIX86374638;     uvVeCHDWYLgTlIX86374638 = uvVeCHDWYLgTlIX30896706;     uvVeCHDWYLgTlIX30896706 = uvVeCHDWYLgTlIX22218748;     uvVeCHDWYLgTlIX22218748 = uvVeCHDWYLgTlIX64446510;     uvVeCHDWYLgTlIX64446510 = uvVeCHDWYLgTlIX87719861;     uvVeCHDWYLgTlIX87719861 = uvVeCHDWYLgTlIX48626800;     uvVeCHDWYLgTlIX48626800 = uvVeCHDWYLgTlIX16800600;     uvVeCHDWYLgTlIX16800600 = uvVeCHDWYLgTlIX65395191;     uvVeCHDWYLgTlIX65395191 = uvVeCHDWYLgTlIX22175862;     uvVeCHDWYLgTlIX22175862 = uvVeCHDWYLgTlIX79139603;     uvVeCHDWYLgTlIX79139603 = uvVeCHDWYLgTlIX20902525;     uvVeCHDWYLgTlIX20902525 = uvVeCHDWYLgTlIX48955633;     uvVeCHDWYLgTlIX48955633 = uvVeCHDWYLgTlIX82140893;     uvVeCHDWYLgTlIX82140893 = uvVeCHDWYLgTlIX20353363;     uvVeCHDWYLgTlIX20353363 = uvVeCHDWYLgTlIX59127862;     uvVeCHDWYLgTlIX59127862 = uvVeCHDWYLgTlIX9704856;     uvVeCHDWYLgTlIX9704856 = uvVeCHDWYLgTlIX57970182;     uvVeCHDWYLgTlIX57970182 = uvVeCHDWYLgTlIX73412944;     uvVeCHDWYLgTlIX73412944 = uvVeCHDWYLgTlIX58723776;     uvVeCHDWYLgTlIX58723776 = uvVeCHDWYLgTlIX12779367;     uvVeCHDWYLgTlIX12779367 = uvVeCHDWYLgTlIX41078690;     uvVeCHDWYLgTlIX41078690 = uvVeCHDWYLgTlIX53368695;     uvVeCHDWYLgTlIX53368695 = uvVeCHDWYLgTlIX41678371;     uvVeCHDWYLgTlIX41678371 = uvVeCHDWYLgTlIX35941622;     uvVeCHDWYLgTlIX35941622 = uvVeCHDWYLgTlIX64967677;     uvVeCHDWYLgTlIX64967677 = uvVeCHDWYLgTlIX95064745;     uvVeCHDWYLgTlIX95064745 = uvVeCHDWYLgTlIX72011364;     uvVeCHDWYLgTlIX72011364 = uvVeCHDWYLgTlIX22767327;     uvVeCHDWYLgTlIX22767327 = uvVeCHDWYLgTlIX84537219;     uvVeCHDWYLgTlIX84537219 = uvVeCHDWYLgTlIX44142098;     uvVeCHDWYLgTlIX44142098 = uvVeCHDWYLgTlIX29854860;     uvVeCHDWYLgTlIX29854860 = uvVeCHDWYLgTlIX22563803;     uvVeCHDWYLgTlIX22563803 = uvVeCHDWYLgTlIX14946202;     uvVeCHDWYLgTlIX14946202 = uvVeCHDWYLgTlIX65051510;     uvVeCHDWYLgTlIX65051510 = uvVeCHDWYLgTlIX3351493;     uvVeCHDWYLgTlIX3351493 = uvVeCHDWYLgTlIX85472;     uvVeCHDWYLgTlIX85472 = uvVeCHDWYLgTlIX10966077;     uvVeCHDWYLgTlIX10966077 = uvVeCHDWYLgTlIX17548638;     uvVeCHDWYLgTlIX17548638 = uvVeCHDWYLgTlIX49794845;     uvVeCHDWYLgTlIX49794845 = uvVeCHDWYLgTlIX7044555;     uvVeCHDWYLgTlIX7044555 = uvVeCHDWYLgTlIX12912529;     uvVeCHDWYLgTlIX12912529 = uvVeCHDWYLgTlIX16455653;     uvVeCHDWYLgTlIX16455653 = uvVeCHDWYLgTlIX62770914;     uvVeCHDWYLgTlIX62770914 = uvVeCHDWYLgTlIX94156489;     uvVeCHDWYLgTlIX94156489 = uvVeCHDWYLgTlIX3283814;     uvVeCHDWYLgTlIX3283814 = uvVeCHDWYLgTlIX98379874;     uvVeCHDWYLgTlIX98379874 = uvVeCHDWYLgTlIX36031582;     uvVeCHDWYLgTlIX36031582 = uvVeCHDWYLgTlIX31165082;     uvVeCHDWYLgTlIX31165082 = uvVeCHDWYLgTlIX67299750;     uvVeCHDWYLgTlIX67299750 = uvVeCHDWYLgTlIX53861181;     uvVeCHDWYLgTlIX53861181 = uvVeCHDWYLgTlIX61640322;     uvVeCHDWYLgTlIX61640322 = uvVeCHDWYLgTlIX38380837;     uvVeCHDWYLgTlIX38380837 = uvVeCHDWYLgTlIX38276396;     uvVeCHDWYLgTlIX38276396 = uvVeCHDWYLgTlIX7813907;     uvVeCHDWYLgTlIX7813907 = uvVeCHDWYLgTlIX1154786;     uvVeCHDWYLgTlIX1154786 = uvVeCHDWYLgTlIX33005944;     uvVeCHDWYLgTlIX33005944 = uvVeCHDWYLgTlIX89218334;     uvVeCHDWYLgTlIX89218334 = uvVeCHDWYLgTlIX86277125;     uvVeCHDWYLgTlIX86277125 = uvVeCHDWYLgTlIX99478833;     uvVeCHDWYLgTlIX99478833 = uvVeCHDWYLgTlIX92655115;     uvVeCHDWYLgTlIX92655115 = uvVeCHDWYLgTlIX76615436;     uvVeCHDWYLgTlIX76615436 = uvVeCHDWYLgTlIX94033273;     uvVeCHDWYLgTlIX94033273 = uvVeCHDWYLgTlIX80857972;     uvVeCHDWYLgTlIX80857972 = uvVeCHDWYLgTlIX78033763;     uvVeCHDWYLgTlIX78033763 = uvVeCHDWYLgTlIX49284743;     uvVeCHDWYLgTlIX49284743 = uvVeCHDWYLgTlIX98338722;     uvVeCHDWYLgTlIX98338722 = uvVeCHDWYLgTlIX34009431;     uvVeCHDWYLgTlIX34009431 = uvVeCHDWYLgTlIX17089384;     uvVeCHDWYLgTlIX17089384 = uvVeCHDWYLgTlIX17001870;     uvVeCHDWYLgTlIX17001870 = uvVeCHDWYLgTlIX59042391;     uvVeCHDWYLgTlIX59042391 = uvVeCHDWYLgTlIX98738778;     uvVeCHDWYLgTlIX98738778 = uvVeCHDWYLgTlIX40421544;     uvVeCHDWYLgTlIX40421544 = uvVeCHDWYLgTlIX23618100;     uvVeCHDWYLgTlIX23618100 = uvVeCHDWYLgTlIX51679222;     uvVeCHDWYLgTlIX51679222 = uvVeCHDWYLgTlIX99866837;     uvVeCHDWYLgTlIX99866837 = uvVeCHDWYLgTlIX24623038;     uvVeCHDWYLgTlIX24623038 = uvVeCHDWYLgTlIX90597780;     uvVeCHDWYLgTlIX90597780 = uvVeCHDWYLgTlIX47521882;     uvVeCHDWYLgTlIX47521882 = uvVeCHDWYLgTlIX32657809;     uvVeCHDWYLgTlIX32657809 = uvVeCHDWYLgTlIX66587803;     uvVeCHDWYLgTlIX66587803 = uvVeCHDWYLgTlIX59033164;     uvVeCHDWYLgTlIX59033164 = uvVeCHDWYLgTlIX40846282;     uvVeCHDWYLgTlIX40846282 = uvVeCHDWYLgTlIX55467577;     uvVeCHDWYLgTlIX55467577 = uvVeCHDWYLgTlIX30676038;     uvVeCHDWYLgTlIX30676038 = uvVeCHDWYLgTlIX82501775;     uvVeCHDWYLgTlIX82501775 = uvVeCHDWYLgTlIX91474023;     uvVeCHDWYLgTlIX91474023 = uvVeCHDWYLgTlIX84287406;     uvVeCHDWYLgTlIX84287406 = uvVeCHDWYLgTlIX7132295;     uvVeCHDWYLgTlIX7132295 = uvVeCHDWYLgTlIX63896724;     uvVeCHDWYLgTlIX63896724 = uvVeCHDWYLgTlIX70345549;     uvVeCHDWYLgTlIX70345549 = uvVeCHDWYLgTlIX10867137;     uvVeCHDWYLgTlIX10867137 = uvVeCHDWYLgTlIX24688951;     uvVeCHDWYLgTlIX24688951 = uvVeCHDWYLgTlIX18069805;     uvVeCHDWYLgTlIX18069805 = uvVeCHDWYLgTlIX57139730;     uvVeCHDWYLgTlIX57139730 = uvVeCHDWYLgTlIX63566037;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void drPpGLqdhEQcuxht80843825() {     float rFykWcGsmVneUmE34217704 = -183428826;    float rFykWcGsmVneUmE88284022 = -663971482;    float rFykWcGsmVneUmE91762700 = 49280025;    float rFykWcGsmVneUmE36931459 = -258029735;    float rFykWcGsmVneUmE42043595 = -289817145;    float rFykWcGsmVneUmE32233239 = -647185149;    float rFykWcGsmVneUmE4753534 = -290700834;    float rFykWcGsmVneUmE87739086 = -38452903;    float rFykWcGsmVneUmE68083802 = -937551995;    float rFykWcGsmVneUmE87412971 = -110264394;    float rFykWcGsmVneUmE29969022 = -428555501;    float rFykWcGsmVneUmE94173371 = 17642769;    float rFykWcGsmVneUmE96627987 = -747146948;    float rFykWcGsmVneUmE49242225 = -948459296;    float rFykWcGsmVneUmE4895881 = 37667759;    float rFykWcGsmVneUmE11668244 = -828134357;    float rFykWcGsmVneUmE14755352 = -442695339;    float rFykWcGsmVneUmE69249127 = -530505219;    float rFykWcGsmVneUmE63848675 = -17375455;    float rFykWcGsmVneUmE79140071 = -80564971;    float rFykWcGsmVneUmE14263616 = -538837675;    float rFykWcGsmVneUmE71100620 = -937824609;    float rFykWcGsmVneUmE59913990 = -43620487;    float rFykWcGsmVneUmE7262482 = -40399631;    float rFykWcGsmVneUmE22848165 = -725052814;    float rFykWcGsmVneUmE43376947 = -462366525;    float rFykWcGsmVneUmE49339476 = -140613306;    float rFykWcGsmVneUmE61150920 = -109475265;    float rFykWcGsmVneUmE73820859 = -709238972;    float rFykWcGsmVneUmE75113718 = 54824454;    float rFykWcGsmVneUmE72039674 = -218505890;    float rFykWcGsmVneUmE39576207 = -513749056;    float rFykWcGsmVneUmE54956725 = -701092473;    float rFykWcGsmVneUmE88132302 = -932267621;    float rFykWcGsmVneUmE62561455 = -613195647;    float rFykWcGsmVneUmE4135357 = -188363151;    float rFykWcGsmVneUmE73646346 = -209314281;    float rFykWcGsmVneUmE75356291 = -719646178;    float rFykWcGsmVneUmE93479490 = -926577137;    float rFykWcGsmVneUmE60154808 = -212768471;    float rFykWcGsmVneUmE63424776 = -259627926;    float rFykWcGsmVneUmE28568462 = -328388455;    float rFykWcGsmVneUmE59933036 = -693240287;    float rFykWcGsmVneUmE24056938 = -552662584;    float rFykWcGsmVneUmE45748904 = -477556409;    float rFykWcGsmVneUmE13450390 = -84224191;    float rFykWcGsmVneUmE39043732 = -272245654;    float rFykWcGsmVneUmE82069862 = -377537736;    float rFykWcGsmVneUmE68212366 = -994726492;    float rFykWcGsmVneUmE2583976 = -321994139;    float rFykWcGsmVneUmE71473138 = -239260870;    float rFykWcGsmVneUmE40251379 = -208342199;    float rFykWcGsmVneUmE91107239 = -146032071;    float rFykWcGsmVneUmE21392387 = -723914981;    float rFykWcGsmVneUmE66580209 = -852245351;    float rFykWcGsmVneUmE63117083 = -245604217;    float rFykWcGsmVneUmE28370032 = -520350996;    float rFykWcGsmVneUmE84500218 = -910320344;    float rFykWcGsmVneUmE14083294 = -532976921;    float rFykWcGsmVneUmE98666647 = -827450620;    float rFykWcGsmVneUmE82893762 = -406571843;    float rFykWcGsmVneUmE43602614 = -81225569;    float rFykWcGsmVneUmE13918228 = -329213932;    float rFykWcGsmVneUmE92970084 = -892376450;    float rFykWcGsmVneUmE15373298 = -891758505;    float rFykWcGsmVneUmE90392815 = -914806445;    float rFykWcGsmVneUmE39216647 = -281264759;    float rFykWcGsmVneUmE8495686 = -814879328;    float rFykWcGsmVneUmE86680769 = -235263649;    float rFykWcGsmVneUmE760525 = -773969090;    float rFykWcGsmVneUmE38021898 = -518820077;    float rFykWcGsmVneUmE39399060 = -723049161;    float rFykWcGsmVneUmE75769636 = -603928083;    float rFykWcGsmVneUmE3693867 = -804606984;    float rFykWcGsmVneUmE15715296 = -820937046;    float rFykWcGsmVneUmE85695154 = -110449221;    float rFykWcGsmVneUmE11167584 = -144584323;    float rFykWcGsmVneUmE35857053 = -490957904;    float rFykWcGsmVneUmE61513578 = -562843223;    float rFykWcGsmVneUmE9397776 = -540828623;    float rFykWcGsmVneUmE4333216 = -90120872;    float rFykWcGsmVneUmE67269613 = -763075571;    float rFykWcGsmVneUmE92938553 = -114748773;    float rFykWcGsmVneUmE71236883 = -287244833;    float rFykWcGsmVneUmE3640581 = -705914676;    float rFykWcGsmVneUmE31788295 = 89836308;    float rFykWcGsmVneUmE48468968 = -267716986;    float rFykWcGsmVneUmE33564338 = -977177492;    float rFykWcGsmVneUmE21552093 = 19977730;    float rFykWcGsmVneUmE99444371 = -267591430;    float rFykWcGsmVneUmE75765325 = -668012156;    float rFykWcGsmVneUmE89146127 = -298993938;    float rFykWcGsmVneUmE61272997 = -86669257;    float rFykWcGsmVneUmE94812842 = 873482;    float rFykWcGsmVneUmE77261046 = -806196629;    float rFykWcGsmVneUmE19822163 = -78402357;    float rFykWcGsmVneUmE14650235 = -999174523;    float rFykWcGsmVneUmE66962952 = -800863837;    float rFykWcGsmVneUmE8683641 = -660904079;    float rFykWcGsmVneUmE55356089 = -183428826;     rFykWcGsmVneUmE34217704 = rFykWcGsmVneUmE88284022;     rFykWcGsmVneUmE88284022 = rFykWcGsmVneUmE91762700;     rFykWcGsmVneUmE91762700 = rFykWcGsmVneUmE36931459;     rFykWcGsmVneUmE36931459 = rFykWcGsmVneUmE42043595;     rFykWcGsmVneUmE42043595 = rFykWcGsmVneUmE32233239;     rFykWcGsmVneUmE32233239 = rFykWcGsmVneUmE4753534;     rFykWcGsmVneUmE4753534 = rFykWcGsmVneUmE87739086;     rFykWcGsmVneUmE87739086 = rFykWcGsmVneUmE68083802;     rFykWcGsmVneUmE68083802 = rFykWcGsmVneUmE87412971;     rFykWcGsmVneUmE87412971 = rFykWcGsmVneUmE29969022;     rFykWcGsmVneUmE29969022 = rFykWcGsmVneUmE94173371;     rFykWcGsmVneUmE94173371 = rFykWcGsmVneUmE96627987;     rFykWcGsmVneUmE96627987 = rFykWcGsmVneUmE49242225;     rFykWcGsmVneUmE49242225 = rFykWcGsmVneUmE4895881;     rFykWcGsmVneUmE4895881 = rFykWcGsmVneUmE11668244;     rFykWcGsmVneUmE11668244 = rFykWcGsmVneUmE14755352;     rFykWcGsmVneUmE14755352 = rFykWcGsmVneUmE69249127;     rFykWcGsmVneUmE69249127 = rFykWcGsmVneUmE63848675;     rFykWcGsmVneUmE63848675 = rFykWcGsmVneUmE79140071;     rFykWcGsmVneUmE79140071 = rFykWcGsmVneUmE14263616;     rFykWcGsmVneUmE14263616 = rFykWcGsmVneUmE71100620;     rFykWcGsmVneUmE71100620 = rFykWcGsmVneUmE59913990;     rFykWcGsmVneUmE59913990 = rFykWcGsmVneUmE7262482;     rFykWcGsmVneUmE7262482 = rFykWcGsmVneUmE22848165;     rFykWcGsmVneUmE22848165 = rFykWcGsmVneUmE43376947;     rFykWcGsmVneUmE43376947 = rFykWcGsmVneUmE49339476;     rFykWcGsmVneUmE49339476 = rFykWcGsmVneUmE61150920;     rFykWcGsmVneUmE61150920 = rFykWcGsmVneUmE73820859;     rFykWcGsmVneUmE73820859 = rFykWcGsmVneUmE75113718;     rFykWcGsmVneUmE75113718 = rFykWcGsmVneUmE72039674;     rFykWcGsmVneUmE72039674 = rFykWcGsmVneUmE39576207;     rFykWcGsmVneUmE39576207 = rFykWcGsmVneUmE54956725;     rFykWcGsmVneUmE54956725 = rFykWcGsmVneUmE88132302;     rFykWcGsmVneUmE88132302 = rFykWcGsmVneUmE62561455;     rFykWcGsmVneUmE62561455 = rFykWcGsmVneUmE4135357;     rFykWcGsmVneUmE4135357 = rFykWcGsmVneUmE73646346;     rFykWcGsmVneUmE73646346 = rFykWcGsmVneUmE75356291;     rFykWcGsmVneUmE75356291 = rFykWcGsmVneUmE93479490;     rFykWcGsmVneUmE93479490 = rFykWcGsmVneUmE60154808;     rFykWcGsmVneUmE60154808 = rFykWcGsmVneUmE63424776;     rFykWcGsmVneUmE63424776 = rFykWcGsmVneUmE28568462;     rFykWcGsmVneUmE28568462 = rFykWcGsmVneUmE59933036;     rFykWcGsmVneUmE59933036 = rFykWcGsmVneUmE24056938;     rFykWcGsmVneUmE24056938 = rFykWcGsmVneUmE45748904;     rFykWcGsmVneUmE45748904 = rFykWcGsmVneUmE13450390;     rFykWcGsmVneUmE13450390 = rFykWcGsmVneUmE39043732;     rFykWcGsmVneUmE39043732 = rFykWcGsmVneUmE82069862;     rFykWcGsmVneUmE82069862 = rFykWcGsmVneUmE68212366;     rFykWcGsmVneUmE68212366 = rFykWcGsmVneUmE2583976;     rFykWcGsmVneUmE2583976 = rFykWcGsmVneUmE71473138;     rFykWcGsmVneUmE71473138 = rFykWcGsmVneUmE40251379;     rFykWcGsmVneUmE40251379 = rFykWcGsmVneUmE91107239;     rFykWcGsmVneUmE91107239 = rFykWcGsmVneUmE21392387;     rFykWcGsmVneUmE21392387 = rFykWcGsmVneUmE66580209;     rFykWcGsmVneUmE66580209 = rFykWcGsmVneUmE63117083;     rFykWcGsmVneUmE63117083 = rFykWcGsmVneUmE28370032;     rFykWcGsmVneUmE28370032 = rFykWcGsmVneUmE84500218;     rFykWcGsmVneUmE84500218 = rFykWcGsmVneUmE14083294;     rFykWcGsmVneUmE14083294 = rFykWcGsmVneUmE98666647;     rFykWcGsmVneUmE98666647 = rFykWcGsmVneUmE82893762;     rFykWcGsmVneUmE82893762 = rFykWcGsmVneUmE43602614;     rFykWcGsmVneUmE43602614 = rFykWcGsmVneUmE13918228;     rFykWcGsmVneUmE13918228 = rFykWcGsmVneUmE92970084;     rFykWcGsmVneUmE92970084 = rFykWcGsmVneUmE15373298;     rFykWcGsmVneUmE15373298 = rFykWcGsmVneUmE90392815;     rFykWcGsmVneUmE90392815 = rFykWcGsmVneUmE39216647;     rFykWcGsmVneUmE39216647 = rFykWcGsmVneUmE8495686;     rFykWcGsmVneUmE8495686 = rFykWcGsmVneUmE86680769;     rFykWcGsmVneUmE86680769 = rFykWcGsmVneUmE760525;     rFykWcGsmVneUmE760525 = rFykWcGsmVneUmE38021898;     rFykWcGsmVneUmE38021898 = rFykWcGsmVneUmE39399060;     rFykWcGsmVneUmE39399060 = rFykWcGsmVneUmE75769636;     rFykWcGsmVneUmE75769636 = rFykWcGsmVneUmE3693867;     rFykWcGsmVneUmE3693867 = rFykWcGsmVneUmE15715296;     rFykWcGsmVneUmE15715296 = rFykWcGsmVneUmE85695154;     rFykWcGsmVneUmE85695154 = rFykWcGsmVneUmE11167584;     rFykWcGsmVneUmE11167584 = rFykWcGsmVneUmE35857053;     rFykWcGsmVneUmE35857053 = rFykWcGsmVneUmE61513578;     rFykWcGsmVneUmE61513578 = rFykWcGsmVneUmE9397776;     rFykWcGsmVneUmE9397776 = rFykWcGsmVneUmE4333216;     rFykWcGsmVneUmE4333216 = rFykWcGsmVneUmE67269613;     rFykWcGsmVneUmE67269613 = rFykWcGsmVneUmE92938553;     rFykWcGsmVneUmE92938553 = rFykWcGsmVneUmE71236883;     rFykWcGsmVneUmE71236883 = rFykWcGsmVneUmE3640581;     rFykWcGsmVneUmE3640581 = rFykWcGsmVneUmE31788295;     rFykWcGsmVneUmE31788295 = rFykWcGsmVneUmE48468968;     rFykWcGsmVneUmE48468968 = rFykWcGsmVneUmE33564338;     rFykWcGsmVneUmE33564338 = rFykWcGsmVneUmE21552093;     rFykWcGsmVneUmE21552093 = rFykWcGsmVneUmE99444371;     rFykWcGsmVneUmE99444371 = rFykWcGsmVneUmE75765325;     rFykWcGsmVneUmE75765325 = rFykWcGsmVneUmE89146127;     rFykWcGsmVneUmE89146127 = rFykWcGsmVneUmE61272997;     rFykWcGsmVneUmE61272997 = rFykWcGsmVneUmE94812842;     rFykWcGsmVneUmE94812842 = rFykWcGsmVneUmE77261046;     rFykWcGsmVneUmE77261046 = rFykWcGsmVneUmE19822163;     rFykWcGsmVneUmE19822163 = rFykWcGsmVneUmE14650235;     rFykWcGsmVneUmE14650235 = rFykWcGsmVneUmE66962952;     rFykWcGsmVneUmE66962952 = rFykWcGsmVneUmE8683641;     rFykWcGsmVneUmE8683641 = rFykWcGsmVneUmE55356089;     rFykWcGsmVneUmE55356089 = rFykWcGsmVneUmE34217704;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void eLrMYsJNOGqcBKoq87502569() {     float mBlXmUHtANKCPOx34211233 = -55952968;    float mBlXmUHtANKCPOx26856339 = -706566608;    float mBlXmUHtANKCPOx93075912 = 92151412;    float mBlXmUHtANKCPOx22011320 = -78570565;    float mBlXmUHtANKCPOx50883697 = -758172466;    float mBlXmUHtANKCPOx67038671 = -346617960;    float mBlXmUHtANKCPOx73300869 = 4762699;    float mBlXmUHtANKCPOx56953137 = -842630221;    float mBlXmUHtANKCPOx50823966 = -651198065;    float mBlXmUHtANKCPOx84052484 = -847071266;    float mBlXmUHtANKCPOx50289627 = -388027573;    float mBlXmUHtANKCPOx8465049 = -817835594;    float mBlXmUHtANKCPOx28480901 = -768805423;    float mBlXmUHtANKCPOx3086445 = -193612347;    float mBlXmUHtANKCPOx50067990 = 74954907;    float mBlXmUHtANKCPOx86232790 = -60930007;    float mBlXmUHtANKCPOx70608111 = -262309909;    float mBlXmUHtANKCPOx92249660 = -927576841;    float mBlXmUHtANKCPOx84276945 = -635357722;    float mBlXmUHtANKCPOx51213835 = -692589422;    float mBlXmUHtANKCPOx93933040 = -249304049;    float mBlXmUHtANKCPOx49396886 = -347816493;    float mBlXmUHtANKCPOx96792387 = -502578329;    float mBlXmUHtANKCPOx50912590 = 78679846;    float mBlXmUHtANKCPOx51290854 = -51746667;    float mBlXmUHtANKCPOx59951024 = -370572454;    float mBlXmUHtANKCPOx19561058 = -441413019;    float mBlXmUHtANKCPOx96554444 = -718442253;    float mBlXmUHtANKCPOx86499306 = -255309287;    float mBlXmUHtANKCPOx51203588 = -720606919;    float mBlXmUHtANKCPOx5452353 = -605599124;    float mBlXmUHtANKCPOx24300064 = -905570817;    float mBlXmUHtANKCPOx40409148 = -903036689;    float mBlXmUHtANKCPOx88415139 = -808949353;    float mBlXmUHtANKCPOx44919494 = -872509572;    float mBlXmUHtANKCPOx46963546 = -268128898;    float mBlXmUHtANKCPOx85587713 = -308967659;    float mBlXmUHtANKCPOx45273971 = -714908857;    float mBlXmUHtANKCPOx28793087 = -539258553;    float mBlXmUHtANKCPOx32027702 = -922200158;    float mBlXmUHtANKCPOx51280444 = -347571824;    float mBlXmUHtANKCPOx14266123 = -522180537;    float mBlXmUHtANKCPOx20216397 = -152370541;    float mBlXmUHtANKCPOx83288836 = -109627027;    float mBlXmUHtANKCPOx29766956 = -950815810;    float mBlXmUHtANKCPOx53878358 = -996268862;    float mBlXmUHtANKCPOx66741525 = -753526276;    float mBlXmUHtANKCPOx57453443 = -986299092;    float mBlXmUHtANKCPOx86870755 = -558490354;    float mBlXmUHtANKCPOx22629399 = 90980974;    float mBlXmUHtANKCPOx84732507 = -917815878;    float mBlXmUHtANKCPOx54798636 = -590487947;    float mBlXmUHtANKCPOx66555587 = -38086079;    float mBlXmUHtANKCPOx31642658 = -155834833;    float mBlXmUHtANKCPOx93569529 = -609144629;    float mBlXmUHtANKCPOx84814346 = -708136475;    float mBlXmUHtANKCPOx30063952 = -103988279;    float mBlXmUHtANKCPOx42163323 = -986528435;    float mBlXmUHtANKCPOx70720465 = 73176102;    float mBlXmUHtANKCPOx90932673 = -287600013;    float mBlXmUHtANKCPOx47477613 = -905204942;    float mBlXmUHtANKCPOx76746424 = -276795048;    float mBlXmUHtANKCPOx70453831 = -487320934;    float mBlXmUHtANKCPOx99620378 = -930591146;    float mBlXmUHtANKCPOx78600132 = -141472142;    float mBlXmUHtANKCPOx25989563 = -482456756;    float mBlXmUHtANKCPOx68055900 = -914798905;    float mBlXmUHtANKCPOx40065762 = -959856070;    float mBlXmUHtANKCPOx58166951 = -321102776;    float mBlXmUHtANKCPOx3104444 = -656916196;    float mBlXmUHtANKCPOx645078 = -751962349;    float mBlXmUHtANKCPOx25334140 = -547401052;    float mBlXmUHtANKCPOx63456573 = -288318288;    float mBlXmUHtANKCPOx52249244 = -713157565;    float mBlXmUHtANKCPOx99933390 = -245017599;    float mBlXmUHtANKCPOx79666917 = -727123512;    float mBlXmUHtANKCPOx29180490 = -95445952;    float mBlXmUHtANKCPOx13503551 = -292951302;    float mBlXmUHtANKCPOx21145634 = 29495656;    float mBlXmUHtANKCPOx97412496 = -55477805;    float mBlXmUHtANKCPOx93209499 = -617046178;    float mBlXmUHtANKCPOx62107614 = -455113928;    float mBlXmUHtANKCPOx9683689 = -59951900;    float mBlXmUHtANKCPOx63869907 = -246290261;    float mBlXmUHtANKCPOx66471080 = -802791042;    float mBlXmUHtANKCPOx50653716 = 84888822;    float mBlXmUHtANKCPOx57744477 = -767484739;    float mBlXmUHtANKCPOx8766491 = -647201857;    float mBlXmUHtANKCPOx94845609 = -99804724;    float mBlXmUHtANKCPOx60105147 = -64373097;    float mBlXmUHtANKCPOx16899595 = -64140619;    float mBlXmUHtANKCPOx43424391 = -322439225;    float mBlXmUHtANKCPOx74553505 = -688084959;    float mBlXmUHtANKCPOx37860414 = -151658541;    float mBlXmUHtANKCPOx84550088 = 83004784;    float mBlXmUHtANKCPOx74534019 = 29223223;    float mBlXmUHtANKCPOx43812292 = 65140397;    float mBlXmUHtANKCPOx20596019 = -221779395;    float mBlXmUHtANKCPOx4688705 = -968154886;    float mBlXmUHtANKCPOx3777393 = -55952968;     mBlXmUHtANKCPOx34211233 = mBlXmUHtANKCPOx26856339;     mBlXmUHtANKCPOx26856339 = mBlXmUHtANKCPOx93075912;     mBlXmUHtANKCPOx93075912 = mBlXmUHtANKCPOx22011320;     mBlXmUHtANKCPOx22011320 = mBlXmUHtANKCPOx50883697;     mBlXmUHtANKCPOx50883697 = mBlXmUHtANKCPOx67038671;     mBlXmUHtANKCPOx67038671 = mBlXmUHtANKCPOx73300869;     mBlXmUHtANKCPOx73300869 = mBlXmUHtANKCPOx56953137;     mBlXmUHtANKCPOx56953137 = mBlXmUHtANKCPOx50823966;     mBlXmUHtANKCPOx50823966 = mBlXmUHtANKCPOx84052484;     mBlXmUHtANKCPOx84052484 = mBlXmUHtANKCPOx50289627;     mBlXmUHtANKCPOx50289627 = mBlXmUHtANKCPOx8465049;     mBlXmUHtANKCPOx8465049 = mBlXmUHtANKCPOx28480901;     mBlXmUHtANKCPOx28480901 = mBlXmUHtANKCPOx3086445;     mBlXmUHtANKCPOx3086445 = mBlXmUHtANKCPOx50067990;     mBlXmUHtANKCPOx50067990 = mBlXmUHtANKCPOx86232790;     mBlXmUHtANKCPOx86232790 = mBlXmUHtANKCPOx70608111;     mBlXmUHtANKCPOx70608111 = mBlXmUHtANKCPOx92249660;     mBlXmUHtANKCPOx92249660 = mBlXmUHtANKCPOx84276945;     mBlXmUHtANKCPOx84276945 = mBlXmUHtANKCPOx51213835;     mBlXmUHtANKCPOx51213835 = mBlXmUHtANKCPOx93933040;     mBlXmUHtANKCPOx93933040 = mBlXmUHtANKCPOx49396886;     mBlXmUHtANKCPOx49396886 = mBlXmUHtANKCPOx96792387;     mBlXmUHtANKCPOx96792387 = mBlXmUHtANKCPOx50912590;     mBlXmUHtANKCPOx50912590 = mBlXmUHtANKCPOx51290854;     mBlXmUHtANKCPOx51290854 = mBlXmUHtANKCPOx59951024;     mBlXmUHtANKCPOx59951024 = mBlXmUHtANKCPOx19561058;     mBlXmUHtANKCPOx19561058 = mBlXmUHtANKCPOx96554444;     mBlXmUHtANKCPOx96554444 = mBlXmUHtANKCPOx86499306;     mBlXmUHtANKCPOx86499306 = mBlXmUHtANKCPOx51203588;     mBlXmUHtANKCPOx51203588 = mBlXmUHtANKCPOx5452353;     mBlXmUHtANKCPOx5452353 = mBlXmUHtANKCPOx24300064;     mBlXmUHtANKCPOx24300064 = mBlXmUHtANKCPOx40409148;     mBlXmUHtANKCPOx40409148 = mBlXmUHtANKCPOx88415139;     mBlXmUHtANKCPOx88415139 = mBlXmUHtANKCPOx44919494;     mBlXmUHtANKCPOx44919494 = mBlXmUHtANKCPOx46963546;     mBlXmUHtANKCPOx46963546 = mBlXmUHtANKCPOx85587713;     mBlXmUHtANKCPOx85587713 = mBlXmUHtANKCPOx45273971;     mBlXmUHtANKCPOx45273971 = mBlXmUHtANKCPOx28793087;     mBlXmUHtANKCPOx28793087 = mBlXmUHtANKCPOx32027702;     mBlXmUHtANKCPOx32027702 = mBlXmUHtANKCPOx51280444;     mBlXmUHtANKCPOx51280444 = mBlXmUHtANKCPOx14266123;     mBlXmUHtANKCPOx14266123 = mBlXmUHtANKCPOx20216397;     mBlXmUHtANKCPOx20216397 = mBlXmUHtANKCPOx83288836;     mBlXmUHtANKCPOx83288836 = mBlXmUHtANKCPOx29766956;     mBlXmUHtANKCPOx29766956 = mBlXmUHtANKCPOx53878358;     mBlXmUHtANKCPOx53878358 = mBlXmUHtANKCPOx66741525;     mBlXmUHtANKCPOx66741525 = mBlXmUHtANKCPOx57453443;     mBlXmUHtANKCPOx57453443 = mBlXmUHtANKCPOx86870755;     mBlXmUHtANKCPOx86870755 = mBlXmUHtANKCPOx22629399;     mBlXmUHtANKCPOx22629399 = mBlXmUHtANKCPOx84732507;     mBlXmUHtANKCPOx84732507 = mBlXmUHtANKCPOx54798636;     mBlXmUHtANKCPOx54798636 = mBlXmUHtANKCPOx66555587;     mBlXmUHtANKCPOx66555587 = mBlXmUHtANKCPOx31642658;     mBlXmUHtANKCPOx31642658 = mBlXmUHtANKCPOx93569529;     mBlXmUHtANKCPOx93569529 = mBlXmUHtANKCPOx84814346;     mBlXmUHtANKCPOx84814346 = mBlXmUHtANKCPOx30063952;     mBlXmUHtANKCPOx30063952 = mBlXmUHtANKCPOx42163323;     mBlXmUHtANKCPOx42163323 = mBlXmUHtANKCPOx70720465;     mBlXmUHtANKCPOx70720465 = mBlXmUHtANKCPOx90932673;     mBlXmUHtANKCPOx90932673 = mBlXmUHtANKCPOx47477613;     mBlXmUHtANKCPOx47477613 = mBlXmUHtANKCPOx76746424;     mBlXmUHtANKCPOx76746424 = mBlXmUHtANKCPOx70453831;     mBlXmUHtANKCPOx70453831 = mBlXmUHtANKCPOx99620378;     mBlXmUHtANKCPOx99620378 = mBlXmUHtANKCPOx78600132;     mBlXmUHtANKCPOx78600132 = mBlXmUHtANKCPOx25989563;     mBlXmUHtANKCPOx25989563 = mBlXmUHtANKCPOx68055900;     mBlXmUHtANKCPOx68055900 = mBlXmUHtANKCPOx40065762;     mBlXmUHtANKCPOx40065762 = mBlXmUHtANKCPOx58166951;     mBlXmUHtANKCPOx58166951 = mBlXmUHtANKCPOx3104444;     mBlXmUHtANKCPOx3104444 = mBlXmUHtANKCPOx645078;     mBlXmUHtANKCPOx645078 = mBlXmUHtANKCPOx25334140;     mBlXmUHtANKCPOx25334140 = mBlXmUHtANKCPOx63456573;     mBlXmUHtANKCPOx63456573 = mBlXmUHtANKCPOx52249244;     mBlXmUHtANKCPOx52249244 = mBlXmUHtANKCPOx99933390;     mBlXmUHtANKCPOx99933390 = mBlXmUHtANKCPOx79666917;     mBlXmUHtANKCPOx79666917 = mBlXmUHtANKCPOx29180490;     mBlXmUHtANKCPOx29180490 = mBlXmUHtANKCPOx13503551;     mBlXmUHtANKCPOx13503551 = mBlXmUHtANKCPOx21145634;     mBlXmUHtANKCPOx21145634 = mBlXmUHtANKCPOx97412496;     mBlXmUHtANKCPOx97412496 = mBlXmUHtANKCPOx93209499;     mBlXmUHtANKCPOx93209499 = mBlXmUHtANKCPOx62107614;     mBlXmUHtANKCPOx62107614 = mBlXmUHtANKCPOx9683689;     mBlXmUHtANKCPOx9683689 = mBlXmUHtANKCPOx63869907;     mBlXmUHtANKCPOx63869907 = mBlXmUHtANKCPOx66471080;     mBlXmUHtANKCPOx66471080 = mBlXmUHtANKCPOx50653716;     mBlXmUHtANKCPOx50653716 = mBlXmUHtANKCPOx57744477;     mBlXmUHtANKCPOx57744477 = mBlXmUHtANKCPOx8766491;     mBlXmUHtANKCPOx8766491 = mBlXmUHtANKCPOx94845609;     mBlXmUHtANKCPOx94845609 = mBlXmUHtANKCPOx60105147;     mBlXmUHtANKCPOx60105147 = mBlXmUHtANKCPOx16899595;     mBlXmUHtANKCPOx16899595 = mBlXmUHtANKCPOx43424391;     mBlXmUHtANKCPOx43424391 = mBlXmUHtANKCPOx74553505;     mBlXmUHtANKCPOx74553505 = mBlXmUHtANKCPOx37860414;     mBlXmUHtANKCPOx37860414 = mBlXmUHtANKCPOx84550088;     mBlXmUHtANKCPOx84550088 = mBlXmUHtANKCPOx74534019;     mBlXmUHtANKCPOx74534019 = mBlXmUHtANKCPOx43812292;     mBlXmUHtANKCPOx43812292 = mBlXmUHtANKCPOx20596019;     mBlXmUHtANKCPOx20596019 = mBlXmUHtANKCPOx4688705;     mBlXmUHtANKCPOx4688705 = mBlXmUHtANKCPOx3777393;     mBlXmUHtANKCPOx3777393 = mBlXmUHtANKCPOx34211233;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void xhuAncstDBaSKlBG41918783() {     float APkKnzDBXetNtFA4862900 = -674102098;    float APkKnzDBXetNtFA95529855 = -578209907;    float APkKnzDBXetNtFA73044831 = -350143048;    float APkKnzDBXetNtFA61942606 = -268708954;    float APkKnzDBXetNtFA72334019 = -541578813;    float APkKnzDBXetNtFA57038433 = -48199112;    float APkKnzDBXetNtFA91679765 = 5986989;    float APkKnzDBXetNtFA13795518 = -89447310;    float APkKnzDBXetNtFA96689020 = -744545393;    float APkKnzDBXetNtFA7018946 = -48166616;    float APkKnzDBXetNtFA92538788 = -422929502;    float APkKnzDBXetNtFA54011619 = -33599387;    float APkKnzDBXetNtFA8308289 = -678342481;    float APkKnzDBXetNtFA86933478 = 5359443;    float APkKnzDBXetNtFA32788009 = -74795096;    float APkKnzDBXetNtFA18761431 = -730913594;    float APkKnzDBXetNtFA64460937 = -171495038;    float APkKnzDBXetNtFA12543155 = -916743037;    float APkKnzDBXetNtFA65984727 = -242545664;    float APkKnzDBXetNtFA10000544 = -134955727;    float APkKnzDBXetNtFA49068795 = -157983189;    float APkKnzDBXetNtFA10792651 = -679101798;    float APkKnzDBXetNtFA98736195 = 29578526;    float APkKnzDBXetNtFA84762127 = -613721060;    float APkKnzDBXetNtFA15415243 = -827651355;    float APkKnzDBXetNtFA90548605 = -468856899;    float APkKnzDBXetNtFA27821843 = -836590794;    float APkKnzDBXetNtFA4336671 = -123566021;    float APkKnzDBXetNtFA18641794 = -549667504;    float APkKnzDBXetNtFA90375683 = -179001990;    float APkKnzDBXetNtFA12524349 = -354032810;    float APkKnzDBXetNtFA68811525 = -133867980;    float APkKnzDBXetNtFA23354509 = -849998298;    float APkKnzDBXetNtFA53780114 = -372479037;    float APkKnzDBXetNtFA22943730 = -719456351;    float APkKnzDBXetNtFA6956806 = -347355445;    float APkKnzDBXetNtFA29379200 = -497560866;    float APkKnzDBXetNtFA98066459 = -499021964;    float APkKnzDBXetNtFA7326376 = -494987190;    float APkKnzDBXetNtFA27131000 = -439808520;    float APkKnzDBXetNtFA11353728 = -731590702;    float APkKnzDBXetNtFA42749114 = -291569606;    float APkKnzDBXetNtFA69183356 = -139124593;    float APkKnzDBXetNtFA89797137 = -181601999;    float APkKnzDBXetNtFA25721015 = -159462205;    float APkKnzDBXetNtFA60284193 = -754606017;    float APkKnzDBXetNtFA92872727 = -869596075;    float APkKnzDBXetNtFA23067654 = -338429387;    float APkKnzDBXetNtFA92312208 = -544254323;    float APkKnzDBXetNtFA31056885 = -192027904;    float APkKnzDBXetNtFA52921832 = -464387999;    float APkKnzDBXetNtFA96670141 = -559707669;    float APkKnzDBXetNtFA21631245 = 34115879;    float APkKnzDBXetNtFA21869962 = -816716329;    float APkKnzDBXetNtFA92849988 = -720085544;    float APkKnzDBXetNtFA94070248 = -995000300;    float APkKnzDBXetNtFA96793660 = -507788433;    float APkKnzDBXetNtFA88282704 = -736421988;    float APkKnzDBXetNtFA46527363 = -441057600;    float APkKnzDBXetNtFA81785414 = 27278085;    float APkKnzDBXetNtFA29216590 = -211608318;    float APkKnzDBXetNtFA87343095 = -870446990;    float APkKnzDBXetNtFA95153723 = -539779807;    float APkKnzDBXetNtFA6313337 = -465543404;    float APkKnzDBXetNtFA94494596 = -694133807;    float APkKnzDBXetNtFA23727264 = -189061522;    float APkKnzDBXetNtFA30657111 = -183601089;    float APkKnzDBXetNtFA54528174 = -205863444;    float APkKnzDBXetNtFA63989748 = -275184207;    float APkKnzDBXetNtFA25831204 = -727439651;    float APkKnzDBXetNtFA89382231 = -133352728;    float APkKnzDBXetNtFA66394478 = -672473074;    float APkKnzDBXetNtFA5216779 = -321755848;    float APkKnzDBXetNtFA38853727 = -802737144;    float APkKnzDBXetNtFA98646815 = -403365025;    float APkKnzDBXetNtFA6319681 = -866413583;    float APkKnzDBXetNtFA41609294 = -439977206;    float APkKnzDBXetNtFA8939059 = -788819476;    float APkKnzDBXetNtFA59041112 = -354258856;    float APkKnzDBXetNtFA55131050 = 26954662;    float APkKnzDBXetNtFA97675877 = -599260824;    float APkKnzDBXetNtFA4754190 = -398161408;    float APkKnzDBXetNtFA12024462 = -579311699;    float APkKnzDBXetNtFA87584908 = -257639600;    float APkKnzDBXetNtFA37453852 = -714613992;    float APkKnzDBXetNtFA15854208 = -794325142;    float APkKnzDBXetNtFA47180280 = -67983860;    float APkKnzDBXetNtFA1484547 = 66718031;    float APkKnzDBXetNtFA60930126 = -652393494;    float APkKnzDBXetNtFA28873482 = -724456051;    float APkKnzDBXetNtFA10163146 = -839567012;    float APkKnzDBXetNtFA41096495 = -761138879;    float APkKnzDBXetNtFA51539096 = 42035635;    float APkKnzDBXetNtFA25540962 = -422265275;    float APkKnzDBXetNtFA97914409 = -128200203;    float APkKnzDBXetNtFA24010633 = -861143713;    float APkKnzDBXetNtFA47595390 = -751789800;    float APkKnzDBXetNtFA62870020 = -673581189;    float APkKnzDBXetNtFA95302540 = -487468193;    float APkKnzDBXetNtFA1993752 = -674102098;     APkKnzDBXetNtFA4862900 = APkKnzDBXetNtFA95529855;     APkKnzDBXetNtFA95529855 = APkKnzDBXetNtFA73044831;     APkKnzDBXetNtFA73044831 = APkKnzDBXetNtFA61942606;     APkKnzDBXetNtFA61942606 = APkKnzDBXetNtFA72334019;     APkKnzDBXetNtFA72334019 = APkKnzDBXetNtFA57038433;     APkKnzDBXetNtFA57038433 = APkKnzDBXetNtFA91679765;     APkKnzDBXetNtFA91679765 = APkKnzDBXetNtFA13795518;     APkKnzDBXetNtFA13795518 = APkKnzDBXetNtFA96689020;     APkKnzDBXetNtFA96689020 = APkKnzDBXetNtFA7018946;     APkKnzDBXetNtFA7018946 = APkKnzDBXetNtFA92538788;     APkKnzDBXetNtFA92538788 = APkKnzDBXetNtFA54011619;     APkKnzDBXetNtFA54011619 = APkKnzDBXetNtFA8308289;     APkKnzDBXetNtFA8308289 = APkKnzDBXetNtFA86933478;     APkKnzDBXetNtFA86933478 = APkKnzDBXetNtFA32788009;     APkKnzDBXetNtFA32788009 = APkKnzDBXetNtFA18761431;     APkKnzDBXetNtFA18761431 = APkKnzDBXetNtFA64460937;     APkKnzDBXetNtFA64460937 = APkKnzDBXetNtFA12543155;     APkKnzDBXetNtFA12543155 = APkKnzDBXetNtFA65984727;     APkKnzDBXetNtFA65984727 = APkKnzDBXetNtFA10000544;     APkKnzDBXetNtFA10000544 = APkKnzDBXetNtFA49068795;     APkKnzDBXetNtFA49068795 = APkKnzDBXetNtFA10792651;     APkKnzDBXetNtFA10792651 = APkKnzDBXetNtFA98736195;     APkKnzDBXetNtFA98736195 = APkKnzDBXetNtFA84762127;     APkKnzDBXetNtFA84762127 = APkKnzDBXetNtFA15415243;     APkKnzDBXetNtFA15415243 = APkKnzDBXetNtFA90548605;     APkKnzDBXetNtFA90548605 = APkKnzDBXetNtFA27821843;     APkKnzDBXetNtFA27821843 = APkKnzDBXetNtFA4336671;     APkKnzDBXetNtFA4336671 = APkKnzDBXetNtFA18641794;     APkKnzDBXetNtFA18641794 = APkKnzDBXetNtFA90375683;     APkKnzDBXetNtFA90375683 = APkKnzDBXetNtFA12524349;     APkKnzDBXetNtFA12524349 = APkKnzDBXetNtFA68811525;     APkKnzDBXetNtFA68811525 = APkKnzDBXetNtFA23354509;     APkKnzDBXetNtFA23354509 = APkKnzDBXetNtFA53780114;     APkKnzDBXetNtFA53780114 = APkKnzDBXetNtFA22943730;     APkKnzDBXetNtFA22943730 = APkKnzDBXetNtFA6956806;     APkKnzDBXetNtFA6956806 = APkKnzDBXetNtFA29379200;     APkKnzDBXetNtFA29379200 = APkKnzDBXetNtFA98066459;     APkKnzDBXetNtFA98066459 = APkKnzDBXetNtFA7326376;     APkKnzDBXetNtFA7326376 = APkKnzDBXetNtFA27131000;     APkKnzDBXetNtFA27131000 = APkKnzDBXetNtFA11353728;     APkKnzDBXetNtFA11353728 = APkKnzDBXetNtFA42749114;     APkKnzDBXetNtFA42749114 = APkKnzDBXetNtFA69183356;     APkKnzDBXetNtFA69183356 = APkKnzDBXetNtFA89797137;     APkKnzDBXetNtFA89797137 = APkKnzDBXetNtFA25721015;     APkKnzDBXetNtFA25721015 = APkKnzDBXetNtFA60284193;     APkKnzDBXetNtFA60284193 = APkKnzDBXetNtFA92872727;     APkKnzDBXetNtFA92872727 = APkKnzDBXetNtFA23067654;     APkKnzDBXetNtFA23067654 = APkKnzDBXetNtFA92312208;     APkKnzDBXetNtFA92312208 = APkKnzDBXetNtFA31056885;     APkKnzDBXetNtFA31056885 = APkKnzDBXetNtFA52921832;     APkKnzDBXetNtFA52921832 = APkKnzDBXetNtFA96670141;     APkKnzDBXetNtFA96670141 = APkKnzDBXetNtFA21631245;     APkKnzDBXetNtFA21631245 = APkKnzDBXetNtFA21869962;     APkKnzDBXetNtFA21869962 = APkKnzDBXetNtFA92849988;     APkKnzDBXetNtFA92849988 = APkKnzDBXetNtFA94070248;     APkKnzDBXetNtFA94070248 = APkKnzDBXetNtFA96793660;     APkKnzDBXetNtFA96793660 = APkKnzDBXetNtFA88282704;     APkKnzDBXetNtFA88282704 = APkKnzDBXetNtFA46527363;     APkKnzDBXetNtFA46527363 = APkKnzDBXetNtFA81785414;     APkKnzDBXetNtFA81785414 = APkKnzDBXetNtFA29216590;     APkKnzDBXetNtFA29216590 = APkKnzDBXetNtFA87343095;     APkKnzDBXetNtFA87343095 = APkKnzDBXetNtFA95153723;     APkKnzDBXetNtFA95153723 = APkKnzDBXetNtFA6313337;     APkKnzDBXetNtFA6313337 = APkKnzDBXetNtFA94494596;     APkKnzDBXetNtFA94494596 = APkKnzDBXetNtFA23727264;     APkKnzDBXetNtFA23727264 = APkKnzDBXetNtFA30657111;     APkKnzDBXetNtFA30657111 = APkKnzDBXetNtFA54528174;     APkKnzDBXetNtFA54528174 = APkKnzDBXetNtFA63989748;     APkKnzDBXetNtFA63989748 = APkKnzDBXetNtFA25831204;     APkKnzDBXetNtFA25831204 = APkKnzDBXetNtFA89382231;     APkKnzDBXetNtFA89382231 = APkKnzDBXetNtFA66394478;     APkKnzDBXetNtFA66394478 = APkKnzDBXetNtFA5216779;     APkKnzDBXetNtFA5216779 = APkKnzDBXetNtFA38853727;     APkKnzDBXetNtFA38853727 = APkKnzDBXetNtFA98646815;     APkKnzDBXetNtFA98646815 = APkKnzDBXetNtFA6319681;     APkKnzDBXetNtFA6319681 = APkKnzDBXetNtFA41609294;     APkKnzDBXetNtFA41609294 = APkKnzDBXetNtFA8939059;     APkKnzDBXetNtFA8939059 = APkKnzDBXetNtFA59041112;     APkKnzDBXetNtFA59041112 = APkKnzDBXetNtFA55131050;     APkKnzDBXetNtFA55131050 = APkKnzDBXetNtFA97675877;     APkKnzDBXetNtFA97675877 = APkKnzDBXetNtFA4754190;     APkKnzDBXetNtFA4754190 = APkKnzDBXetNtFA12024462;     APkKnzDBXetNtFA12024462 = APkKnzDBXetNtFA87584908;     APkKnzDBXetNtFA87584908 = APkKnzDBXetNtFA37453852;     APkKnzDBXetNtFA37453852 = APkKnzDBXetNtFA15854208;     APkKnzDBXetNtFA15854208 = APkKnzDBXetNtFA47180280;     APkKnzDBXetNtFA47180280 = APkKnzDBXetNtFA1484547;     APkKnzDBXetNtFA1484547 = APkKnzDBXetNtFA60930126;     APkKnzDBXetNtFA60930126 = APkKnzDBXetNtFA28873482;     APkKnzDBXetNtFA28873482 = APkKnzDBXetNtFA10163146;     APkKnzDBXetNtFA10163146 = APkKnzDBXetNtFA41096495;     APkKnzDBXetNtFA41096495 = APkKnzDBXetNtFA51539096;     APkKnzDBXetNtFA51539096 = APkKnzDBXetNtFA25540962;     APkKnzDBXetNtFA25540962 = APkKnzDBXetNtFA97914409;     APkKnzDBXetNtFA97914409 = APkKnzDBXetNtFA24010633;     APkKnzDBXetNtFA24010633 = APkKnzDBXetNtFA47595390;     APkKnzDBXetNtFA47595390 = APkKnzDBXetNtFA62870020;     APkKnzDBXetNtFA62870020 = APkKnzDBXetNtFA95302540;     APkKnzDBXetNtFA95302540 = APkKnzDBXetNtFA1993752;     APkKnzDBXetNtFA1993752 = APkKnzDBXetNtFA4862900;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ToLCXHnoiYILACUB48247330() {     float adVvtnVetCKDJhT59517297 = -11629682;    float adVvtnVetCKDJhT42527475 = -57774721;    float adVvtnVetCKDJhT33777478 = -735527661;    float adVvtnVetCKDJhT33377548 = -274811365;    float adVvtnVetCKDJhT46785690 = -214014053;    float adVvtnVetCKDJhT71212830 = -177349948;    float adVvtnVetCKDJhT12780469 = -295905683;    float adVvtnVetCKDJhT42970621 = -590015543;    float adVvtnVetCKDJhT84463431 = -791398764;    float adVvtnVetCKDJhT89650931 = -326967886;    float adVvtnVetCKDJhT71150083 = 51713926;    float adVvtnVetCKDJhT16776333 = -220023476;    float adVvtnVetCKDJhT72125603 = -481882785;    float adVvtnVetCKDJhT79899909 = -706744135;    float adVvtnVetCKDJhT63012082 = 18083273;    float adVvtnVetCKDJhT8528967 = -361073158;    float adVvtnVetCKDJhT64292701 = -645094867;    float adVvtnVetCKDJhT80139741 = -980307505;    float adVvtnVetCKDJhT67205327 = -528357212;    float adVvtnVetCKDJhT27635100 = -480321873;    float adVvtnVetCKDJhT83243182 = -726066340;    float adVvtnVetCKDJhT62045240 = -845545906;    float adVvtnVetCKDJhT35206027 = -557164895;    float adVvtnVetCKDJhT86190495 = -312761876;    float adVvtnVetCKDJhT11167859 = -100564807;    float adVvtnVetCKDJhT31789552 = -472565683;    float adVvtnVetCKDJhT29811767 = -291435073;    float adVvtnVetCKDJhT57585670 = -131617882;    float adVvtnVetCKDJhT58539471 = -929912379;    float adVvtnVetCKDJhT84811092 = -469759958;    float adVvtnVetCKDJhT64229878 = -902905335;    float adVvtnVetCKDJhT99803135 = -545364509;    float adVvtnVetCKDJhT91010385 = -306515912;    float adVvtnVetCKDJhT48436007 = -52599847;    float adVvtnVetCKDJhT57447887 = -937319611;    float adVvtnVetCKDJhT94283347 = -595351042;    float adVvtnVetCKDJhT4083688 = -347987487;    float adVvtnVetCKDJhT11043699 = -372950985;    float adVvtnVetCKDJhT58096025 = -876935792;    float adVvtnVetCKDJhT51117394 = -98117120;    float adVvtnVetCKDJhT67313129 = -844140861;    float adVvtnVetCKDJhT79423772 = -899101693;    float adVvtnVetCKDJhT60183539 = 20370089;    float adVvtnVetCKDJhT98791536 = -755281665;    float adVvtnVetCKDJhT85705078 = -291979803;    float adVvtnVetCKDJhT15617795 = -823395632;    float adVvtnVetCKDJhT9346440 = -896653459;    float adVvtnVetCKDJhT46494963 = -316081760;    float adVvtnVetCKDJhT34654975 = -443984512;    float adVvtnVetCKDJhT75898547 = -432047199;    float adVvtnVetCKDJhT28035372 = -121603501;    float adVvtnVetCKDJhT86052290 = 25226348;    float adVvtnVetCKDJhT53359248 = -805799578;    float adVvtnVetCKDJhT79285719 = -712602814;    float adVvtnVetCKDJhT65004148 = -15994225;    float adVvtnVetCKDJhT97472057 = -166083776;    float adVvtnVetCKDJhT7321448 = -500609826;    float adVvtnVetCKDJhT47586982 = -322765785;    float adVvtnVetCKDJhT22209689 = -74246559;    float adVvtnVetCKDJhT14996138 = -741448370;    float adVvtnVetCKDJhT41401063 = -885914875;    float adVvtnVetCKDJhT55194799 = -64287802;    float adVvtnVetCKDJhT84431150 = -660103164;    float adVvtnVetCKDJhT99652338 = -221638806;    float adVvtnVetCKDJhT25421053 = -424062551;    float adVvtnVetCKDJhT71346948 = -402921566;    float adVvtnVetCKDJhT25765948 = -913507564;    float adVvtnVetCKDJhT23689596 = -329282939;    float adVvtnVetCKDJhT22452023 = -769424525;    float adVvtnVetCKDJhT68728735 = -386565686;    float adVvtnVetCKDJhT4445280 = 86914328;    float adVvtnVetCKDJhT53249003 = -172143882;    float adVvtnVetCKDJhT22043717 = -3371713;    float adVvtnVetCKDJhT16087934 = -330240093;    float adVvtnVetCKDJhT60321970 = -636181013;    float adVvtnVetCKDJhT3819410 = -826964648;    float adVvtnVetCKDJhT1861701 = -765915996;    float adVvtnVetCKDJhT36414491 = -801883231;    float adVvtnVetCKDJhT485418 = 79217926;    float adVvtnVetCKDJhT95550064 = -277169176;    float adVvtnVetCKDJhT22443113 = -575912225;    float adVvtnVetCKDJhT83316804 = -975353314;    float adVvtnVetCKDJhT22930696 = -687633370;    float adVvtnVetCKDJhT82640923 = -397865181;    float adVvtnVetCKDJhT56775721 = -248156458;    float adVvtnVetCKDJhT78177587 = -828131684;    float adVvtnVetCKDJhT46443888 = -739564931;    float adVvtnVetCKDJhT11724666 = -593913099;    float adVvtnVetCKDJhT83431859 = 63394377;    float adVvtnVetCKDJhT59975829 = -671235835;    float adVvtnVetCKDJhT86961899 = 5258784;    float adVvtnVetCKDJhT56496705 = 74778298;    float adVvtnVetCKDJhT88834009 = -198704427;    float adVvtnVetCKDJhT43099888 = -35487422;    float adVvtnVetCKDJhT9716332 = -212202245;    float adVvtnVetCKDJhT12118331 = -679853059;    float adVvtnVetCKDJhT94992622 = -138998529;    float adVvtnVetCKDJhT60531201 = -757991105;    float adVvtnVetCKDJhT73370483 = -231219115;    float adVvtnVetCKDJhT14358131 = -11629682;     adVvtnVetCKDJhT59517297 = adVvtnVetCKDJhT42527475;     adVvtnVetCKDJhT42527475 = adVvtnVetCKDJhT33777478;     adVvtnVetCKDJhT33777478 = adVvtnVetCKDJhT33377548;     adVvtnVetCKDJhT33377548 = adVvtnVetCKDJhT46785690;     adVvtnVetCKDJhT46785690 = adVvtnVetCKDJhT71212830;     adVvtnVetCKDJhT71212830 = adVvtnVetCKDJhT12780469;     adVvtnVetCKDJhT12780469 = adVvtnVetCKDJhT42970621;     adVvtnVetCKDJhT42970621 = adVvtnVetCKDJhT84463431;     adVvtnVetCKDJhT84463431 = adVvtnVetCKDJhT89650931;     adVvtnVetCKDJhT89650931 = adVvtnVetCKDJhT71150083;     adVvtnVetCKDJhT71150083 = adVvtnVetCKDJhT16776333;     adVvtnVetCKDJhT16776333 = adVvtnVetCKDJhT72125603;     adVvtnVetCKDJhT72125603 = adVvtnVetCKDJhT79899909;     adVvtnVetCKDJhT79899909 = adVvtnVetCKDJhT63012082;     adVvtnVetCKDJhT63012082 = adVvtnVetCKDJhT8528967;     adVvtnVetCKDJhT8528967 = adVvtnVetCKDJhT64292701;     adVvtnVetCKDJhT64292701 = adVvtnVetCKDJhT80139741;     adVvtnVetCKDJhT80139741 = adVvtnVetCKDJhT67205327;     adVvtnVetCKDJhT67205327 = adVvtnVetCKDJhT27635100;     adVvtnVetCKDJhT27635100 = adVvtnVetCKDJhT83243182;     adVvtnVetCKDJhT83243182 = adVvtnVetCKDJhT62045240;     adVvtnVetCKDJhT62045240 = adVvtnVetCKDJhT35206027;     adVvtnVetCKDJhT35206027 = adVvtnVetCKDJhT86190495;     adVvtnVetCKDJhT86190495 = adVvtnVetCKDJhT11167859;     adVvtnVetCKDJhT11167859 = adVvtnVetCKDJhT31789552;     adVvtnVetCKDJhT31789552 = adVvtnVetCKDJhT29811767;     adVvtnVetCKDJhT29811767 = adVvtnVetCKDJhT57585670;     adVvtnVetCKDJhT57585670 = adVvtnVetCKDJhT58539471;     adVvtnVetCKDJhT58539471 = adVvtnVetCKDJhT84811092;     adVvtnVetCKDJhT84811092 = adVvtnVetCKDJhT64229878;     adVvtnVetCKDJhT64229878 = adVvtnVetCKDJhT99803135;     adVvtnVetCKDJhT99803135 = adVvtnVetCKDJhT91010385;     adVvtnVetCKDJhT91010385 = adVvtnVetCKDJhT48436007;     adVvtnVetCKDJhT48436007 = adVvtnVetCKDJhT57447887;     adVvtnVetCKDJhT57447887 = adVvtnVetCKDJhT94283347;     adVvtnVetCKDJhT94283347 = adVvtnVetCKDJhT4083688;     adVvtnVetCKDJhT4083688 = adVvtnVetCKDJhT11043699;     adVvtnVetCKDJhT11043699 = adVvtnVetCKDJhT58096025;     adVvtnVetCKDJhT58096025 = adVvtnVetCKDJhT51117394;     adVvtnVetCKDJhT51117394 = adVvtnVetCKDJhT67313129;     adVvtnVetCKDJhT67313129 = adVvtnVetCKDJhT79423772;     adVvtnVetCKDJhT79423772 = adVvtnVetCKDJhT60183539;     adVvtnVetCKDJhT60183539 = adVvtnVetCKDJhT98791536;     adVvtnVetCKDJhT98791536 = adVvtnVetCKDJhT85705078;     adVvtnVetCKDJhT85705078 = adVvtnVetCKDJhT15617795;     adVvtnVetCKDJhT15617795 = adVvtnVetCKDJhT9346440;     adVvtnVetCKDJhT9346440 = adVvtnVetCKDJhT46494963;     adVvtnVetCKDJhT46494963 = adVvtnVetCKDJhT34654975;     adVvtnVetCKDJhT34654975 = adVvtnVetCKDJhT75898547;     adVvtnVetCKDJhT75898547 = adVvtnVetCKDJhT28035372;     adVvtnVetCKDJhT28035372 = adVvtnVetCKDJhT86052290;     adVvtnVetCKDJhT86052290 = adVvtnVetCKDJhT53359248;     adVvtnVetCKDJhT53359248 = adVvtnVetCKDJhT79285719;     adVvtnVetCKDJhT79285719 = adVvtnVetCKDJhT65004148;     adVvtnVetCKDJhT65004148 = adVvtnVetCKDJhT97472057;     adVvtnVetCKDJhT97472057 = adVvtnVetCKDJhT7321448;     adVvtnVetCKDJhT7321448 = adVvtnVetCKDJhT47586982;     adVvtnVetCKDJhT47586982 = adVvtnVetCKDJhT22209689;     adVvtnVetCKDJhT22209689 = adVvtnVetCKDJhT14996138;     adVvtnVetCKDJhT14996138 = adVvtnVetCKDJhT41401063;     adVvtnVetCKDJhT41401063 = adVvtnVetCKDJhT55194799;     adVvtnVetCKDJhT55194799 = adVvtnVetCKDJhT84431150;     adVvtnVetCKDJhT84431150 = adVvtnVetCKDJhT99652338;     adVvtnVetCKDJhT99652338 = adVvtnVetCKDJhT25421053;     adVvtnVetCKDJhT25421053 = adVvtnVetCKDJhT71346948;     adVvtnVetCKDJhT71346948 = adVvtnVetCKDJhT25765948;     adVvtnVetCKDJhT25765948 = adVvtnVetCKDJhT23689596;     adVvtnVetCKDJhT23689596 = adVvtnVetCKDJhT22452023;     adVvtnVetCKDJhT22452023 = adVvtnVetCKDJhT68728735;     adVvtnVetCKDJhT68728735 = adVvtnVetCKDJhT4445280;     adVvtnVetCKDJhT4445280 = adVvtnVetCKDJhT53249003;     adVvtnVetCKDJhT53249003 = adVvtnVetCKDJhT22043717;     adVvtnVetCKDJhT22043717 = adVvtnVetCKDJhT16087934;     adVvtnVetCKDJhT16087934 = adVvtnVetCKDJhT60321970;     adVvtnVetCKDJhT60321970 = adVvtnVetCKDJhT3819410;     adVvtnVetCKDJhT3819410 = adVvtnVetCKDJhT1861701;     adVvtnVetCKDJhT1861701 = adVvtnVetCKDJhT36414491;     adVvtnVetCKDJhT36414491 = adVvtnVetCKDJhT485418;     adVvtnVetCKDJhT485418 = adVvtnVetCKDJhT95550064;     adVvtnVetCKDJhT95550064 = adVvtnVetCKDJhT22443113;     adVvtnVetCKDJhT22443113 = adVvtnVetCKDJhT83316804;     adVvtnVetCKDJhT83316804 = adVvtnVetCKDJhT22930696;     adVvtnVetCKDJhT22930696 = adVvtnVetCKDJhT82640923;     adVvtnVetCKDJhT82640923 = adVvtnVetCKDJhT56775721;     adVvtnVetCKDJhT56775721 = adVvtnVetCKDJhT78177587;     adVvtnVetCKDJhT78177587 = adVvtnVetCKDJhT46443888;     adVvtnVetCKDJhT46443888 = adVvtnVetCKDJhT11724666;     adVvtnVetCKDJhT11724666 = adVvtnVetCKDJhT83431859;     adVvtnVetCKDJhT83431859 = adVvtnVetCKDJhT59975829;     adVvtnVetCKDJhT59975829 = adVvtnVetCKDJhT86961899;     adVvtnVetCKDJhT86961899 = adVvtnVetCKDJhT56496705;     adVvtnVetCKDJhT56496705 = adVvtnVetCKDJhT88834009;     adVvtnVetCKDJhT88834009 = adVvtnVetCKDJhT43099888;     adVvtnVetCKDJhT43099888 = adVvtnVetCKDJhT9716332;     adVvtnVetCKDJhT9716332 = adVvtnVetCKDJhT12118331;     adVvtnVetCKDJhT12118331 = adVvtnVetCKDJhT94992622;     adVvtnVetCKDJhT94992622 = adVvtnVetCKDJhT60531201;     adVvtnVetCKDJhT60531201 = adVvtnVetCKDJhT73370483;     adVvtnVetCKDJhT73370483 = adVvtnVetCKDJhT14358131;     adVvtnVetCKDJhT14358131 = adVvtnVetCKDJhT59517297;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void iCFoewhJMeXYsbqI93943022() {     float vipeTPLsxCmoehM78706254 = -955404507;    float vipeTPLsxCmoehM14825333 = -799383053;    float vipeTPLsxCmoehM58436859 = -312373813;    float vipeTPLsxCmoehM17668995 = -280303535;    float vipeTPLsxCmoehM73792193 = -249205768;    float vipeTPLsxCmoehM83969787 = -843585700;    float vipeTPLsxCmoehM91771102 = -457609088;    float vipeTPLsxCmoehM79228214 = -50526952;    float vipeTPLsxCmoehM53460401 = -63566797;    float vipeTPLsxCmoehM54019718 = -577889028;    float vipeTPLsxCmoehM31900249 = -731106989;    float vipeTPLsxCmoehM93264574 = -277805156;    float vipeTPLsxCmoehM29561186 = -855069059;    float vipeTPLsxCmoehM73569697 = -687637356;    float vipeTPLsxCmoehM80213748 = -448326196;    float vipeTPLsxCmoehM89319749 = -688216766;    float vipeTPLsxCmoehM64141288 = -411334712;    float vipeTPLsxCmoehM70976669 = -267515525;    float vipeTPLsxCmoehM88303868 = -15587605;    float vipeTPLsxCmoehM23506200 = 88848595;    float vipeTPLsxCmoehM84000131 = -247341176;    float vipeTPLsxCmoehM88172569 = -335345603;    float vipeTPLsxCmoehM38028876 = -865233974;    float vipeTPLsxCmoehM37476028 = -481898611;    float vipeTPLsxCmoehM27345214 = -876186914;    float vipeTPLsxCmoehM78906404 = -695903589;    float vipeTPLsxCmoehM41602699 = -240794924;    float vipeTPLsxCmoehM5509771 = -578864556;    float vipeTPLsxCmoehM64447380 = -282132767;    float vipeTPLsxCmoehM49802961 = -401442129;    float vipeTPLsxCmoehM30764854 = -186890609;    float vipeTPLsxCmoehM77695584 = -475711384;    float vipeTPLsxCmoehM31900674 = -917381765;    float vipeTPLsxCmoehM13626311 = -644708576;    float vipeTPLsxCmoehM8501628 = -363396544;    float vipeTPLsxCmoehM72877235 = -708547079;    float vipeTPLsxCmoehM41317726 = -433371445;    float vipeTPLsxCmoehM42723213 = -259487104;    float vipeTPLsxCmoehM13788709 = -120689533;    float vipeTPLsxCmoehM82705149 = 99405141;    float vipeTPLsxCmoehM77676589 = -175436003;    float vipeTPLsxCmoehM12430965 = 94119429;    float vipeTPLsxCmoehM22083704 = -826084697;    float vipeTPLsxCmoehM26886496 = -281593364;    float vipeTPLsxCmoehM29690736 = 28754359;    float vipeTPLsxCmoehM65418036 = -665306285;    float vipeTPLsxCmoehM34172780 = -921005104;    float vipeTPLsxCmoehM27579541 = -735968895;    float vipeTPLsxCmoehM72763465 = -243741682;    float vipeTPLsxCmoehM16256044 = -868064564;    float vipeTPLsxCmoehM35637557 = -583097452;    float vipeTPLsxCmoehM6496226 = -658333036;    float vipeTPLsxCmoehM91914450 = -681723490;    float vipeTPLsxCmoehM30959901 = -948900650;    float vipeTPLsxCmoehM9942892 = -922312038;    float vipeTPLsxCmoehM90533685 = -520058905;    float vipeTPLsxCmoehM76796457 = -934149080;    float vipeTPLsxCmoehM20960832 = -830475203;    float vipeTPLsxCmoehM90323781 = -404116622;    float vipeTPLsxCmoehM94885788 = -553302179;    float vipeTPLsxCmoehM42367089 = -502790777;    float vipeTPLsxCmoehM86261331 = -878744533;    float vipeTPLsxCmoehM14780834 = -768394186;    float vipeTPLsxCmoehM3657440 = -662124668;    float vipeTPLsxCmoehM23254864 = -290998420;    float vipeTPLsxCmoehM54204664 = -155395606;    float vipeTPLsxCmoehM61363900 = -360423391;    float vipeTPLsxCmoehM15934876 = -110360484;    float vipeTPLsxCmoehM65068069 = -224240812;    float vipeTPLsxCmoehM7336514 = -739779117;    float vipeTPLsxCmoehM48002023 = -154845321;    float vipeTPLsxCmoehM21418075 = -51847609;    float vipeTPLsxCmoehM57187961 = -46825992;    float vipeTPLsxCmoehM5598719 = -14992746;    float vipeTPLsxCmoehM45829610 = -735715402;    float vipeTPLsxCmoehM71569166 = -241460605;    float vipeTPLsxCmoehM66088866 = -509260907;    float vipeTPLsxCmoehM11142381 = -483640611;    float vipeTPLsxCmoehM7785292 = -410652971;    float vipeTPLsxCmoehM61927177 = -110880629;    float vipeTPLsxCmoehM44733624 = -774898486;    float vipeTPLsxCmoehM14023159 = -504826030;    float vipeTPLsxCmoehM32746306 = -235122875;    float vipeTPLsxCmoehM48191336 = -414068204;    float vipeTPLsxCmoehM14165404 = -818344678;    float vipeTPLsxCmoehM24268628 = -528557573;    float vipeTPLsxCmoehM85781134 = -793987895;    float vipeTPLsxCmoehM940774 = -968481116;    float vipeTPLsxCmoehM3683419 = -722396538;    float vipeTPLsxCmoehM17967943 = -843337640;    float vipeTPLsxCmoehM96080778 = -774397999;    float vipeTPLsxCmoehM20356895 = -602896243;    float vipeTPLsxCmoehM52399432 = -855370483;    float vipeTPLsxCmoehM18902921 = -567387354;    float vipeTPLsxCmoehM40338061 = -397804083;    float vipeTPLsxCmoehM91415257 = -296691471;    float vipeTPLsxCmoehM97650130 = -137486386;    float vipeTPLsxCmoehM18426265 = -63960029;    float vipeTPLsxCmoehM3631632 = -990594944;    float vipeTPLsxCmoehM75486071 = -955404507;     vipeTPLsxCmoehM78706254 = vipeTPLsxCmoehM14825333;     vipeTPLsxCmoehM14825333 = vipeTPLsxCmoehM58436859;     vipeTPLsxCmoehM58436859 = vipeTPLsxCmoehM17668995;     vipeTPLsxCmoehM17668995 = vipeTPLsxCmoehM73792193;     vipeTPLsxCmoehM73792193 = vipeTPLsxCmoehM83969787;     vipeTPLsxCmoehM83969787 = vipeTPLsxCmoehM91771102;     vipeTPLsxCmoehM91771102 = vipeTPLsxCmoehM79228214;     vipeTPLsxCmoehM79228214 = vipeTPLsxCmoehM53460401;     vipeTPLsxCmoehM53460401 = vipeTPLsxCmoehM54019718;     vipeTPLsxCmoehM54019718 = vipeTPLsxCmoehM31900249;     vipeTPLsxCmoehM31900249 = vipeTPLsxCmoehM93264574;     vipeTPLsxCmoehM93264574 = vipeTPLsxCmoehM29561186;     vipeTPLsxCmoehM29561186 = vipeTPLsxCmoehM73569697;     vipeTPLsxCmoehM73569697 = vipeTPLsxCmoehM80213748;     vipeTPLsxCmoehM80213748 = vipeTPLsxCmoehM89319749;     vipeTPLsxCmoehM89319749 = vipeTPLsxCmoehM64141288;     vipeTPLsxCmoehM64141288 = vipeTPLsxCmoehM70976669;     vipeTPLsxCmoehM70976669 = vipeTPLsxCmoehM88303868;     vipeTPLsxCmoehM88303868 = vipeTPLsxCmoehM23506200;     vipeTPLsxCmoehM23506200 = vipeTPLsxCmoehM84000131;     vipeTPLsxCmoehM84000131 = vipeTPLsxCmoehM88172569;     vipeTPLsxCmoehM88172569 = vipeTPLsxCmoehM38028876;     vipeTPLsxCmoehM38028876 = vipeTPLsxCmoehM37476028;     vipeTPLsxCmoehM37476028 = vipeTPLsxCmoehM27345214;     vipeTPLsxCmoehM27345214 = vipeTPLsxCmoehM78906404;     vipeTPLsxCmoehM78906404 = vipeTPLsxCmoehM41602699;     vipeTPLsxCmoehM41602699 = vipeTPLsxCmoehM5509771;     vipeTPLsxCmoehM5509771 = vipeTPLsxCmoehM64447380;     vipeTPLsxCmoehM64447380 = vipeTPLsxCmoehM49802961;     vipeTPLsxCmoehM49802961 = vipeTPLsxCmoehM30764854;     vipeTPLsxCmoehM30764854 = vipeTPLsxCmoehM77695584;     vipeTPLsxCmoehM77695584 = vipeTPLsxCmoehM31900674;     vipeTPLsxCmoehM31900674 = vipeTPLsxCmoehM13626311;     vipeTPLsxCmoehM13626311 = vipeTPLsxCmoehM8501628;     vipeTPLsxCmoehM8501628 = vipeTPLsxCmoehM72877235;     vipeTPLsxCmoehM72877235 = vipeTPLsxCmoehM41317726;     vipeTPLsxCmoehM41317726 = vipeTPLsxCmoehM42723213;     vipeTPLsxCmoehM42723213 = vipeTPLsxCmoehM13788709;     vipeTPLsxCmoehM13788709 = vipeTPLsxCmoehM82705149;     vipeTPLsxCmoehM82705149 = vipeTPLsxCmoehM77676589;     vipeTPLsxCmoehM77676589 = vipeTPLsxCmoehM12430965;     vipeTPLsxCmoehM12430965 = vipeTPLsxCmoehM22083704;     vipeTPLsxCmoehM22083704 = vipeTPLsxCmoehM26886496;     vipeTPLsxCmoehM26886496 = vipeTPLsxCmoehM29690736;     vipeTPLsxCmoehM29690736 = vipeTPLsxCmoehM65418036;     vipeTPLsxCmoehM65418036 = vipeTPLsxCmoehM34172780;     vipeTPLsxCmoehM34172780 = vipeTPLsxCmoehM27579541;     vipeTPLsxCmoehM27579541 = vipeTPLsxCmoehM72763465;     vipeTPLsxCmoehM72763465 = vipeTPLsxCmoehM16256044;     vipeTPLsxCmoehM16256044 = vipeTPLsxCmoehM35637557;     vipeTPLsxCmoehM35637557 = vipeTPLsxCmoehM6496226;     vipeTPLsxCmoehM6496226 = vipeTPLsxCmoehM91914450;     vipeTPLsxCmoehM91914450 = vipeTPLsxCmoehM30959901;     vipeTPLsxCmoehM30959901 = vipeTPLsxCmoehM9942892;     vipeTPLsxCmoehM9942892 = vipeTPLsxCmoehM90533685;     vipeTPLsxCmoehM90533685 = vipeTPLsxCmoehM76796457;     vipeTPLsxCmoehM76796457 = vipeTPLsxCmoehM20960832;     vipeTPLsxCmoehM20960832 = vipeTPLsxCmoehM90323781;     vipeTPLsxCmoehM90323781 = vipeTPLsxCmoehM94885788;     vipeTPLsxCmoehM94885788 = vipeTPLsxCmoehM42367089;     vipeTPLsxCmoehM42367089 = vipeTPLsxCmoehM86261331;     vipeTPLsxCmoehM86261331 = vipeTPLsxCmoehM14780834;     vipeTPLsxCmoehM14780834 = vipeTPLsxCmoehM3657440;     vipeTPLsxCmoehM3657440 = vipeTPLsxCmoehM23254864;     vipeTPLsxCmoehM23254864 = vipeTPLsxCmoehM54204664;     vipeTPLsxCmoehM54204664 = vipeTPLsxCmoehM61363900;     vipeTPLsxCmoehM61363900 = vipeTPLsxCmoehM15934876;     vipeTPLsxCmoehM15934876 = vipeTPLsxCmoehM65068069;     vipeTPLsxCmoehM65068069 = vipeTPLsxCmoehM7336514;     vipeTPLsxCmoehM7336514 = vipeTPLsxCmoehM48002023;     vipeTPLsxCmoehM48002023 = vipeTPLsxCmoehM21418075;     vipeTPLsxCmoehM21418075 = vipeTPLsxCmoehM57187961;     vipeTPLsxCmoehM57187961 = vipeTPLsxCmoehM5598719;     vipeTPLsxCmoehM5598719 = vipeTPLsxCmoehM45829610;     vipeTPLsxCmoehM45829610 = vipeTPLsxCmoehM71569166;     vipeTPLsxCmoehM71569166 = vipeTPLsxCmoehM66088866;     vipeTPLsxCmoehM66088866 = vipeTPLsxCmoehM11142381;     vipeTPLsxCmoehM11142381 = vipeTPLsxCmoehM7785292;     vipeTPLsxCmoehM7785292 = vipeTPLsxCmoehM61927177;     vipeTPLsxCmoehM61927177 = vipeTPLsxCmoehM44733624;     vipeTPLsxCmoehM44733624 = vipeTPLsxCmoehM14023159;     vipeTPLsxCmoehM14023159 = vipeTPLsxCmoehM32746306;     vipeTPLsxCmoehM32746306 = vipeTPLsxCmoehM48191336;     vipeTPLsxCmoehM48191336 = vipeTPLsxCmoehM14165404;     vipeTPLsxCmoehM14165404 = vipeTPLsxCmoehM24268628;     vipeTPLsxCmoehM24268628 = vipeTPLsxCmoehM85781134;     vipeTPLsxCmoehM85781134 = vipeTPLsxCmoehM940774;     vipeTPLsxCmoehM940774 = vipeTPLsxCmoehM3683419;     vipeTPLsxCmoehM3683419 = vipeTPLsxCmoehM17967943;     vipeTPLsxCmoehM17967943 = vipeTPLsxCmoehM96080778;     vipeTPLsxCmoehM96080778 = vipeTPLsxCmoehM20356895;     vipeTPLsxCmoehM20356895 = vipeTPLsxCmoehM52399432;     vipeTPLsxCmoehM52399432 = vipeTPLsxCmoehM18902921;     vipeTPLsxCmoehM18902921 = vipeTPLsxCmoehM40338061;     vipeTPLsxCmoehM40338061 = vipeTPLsxCmoehM91415257;     vipeTPLsxCmoehM91415257 = vipeTPLsxCmoehM97650130;     vipeTPLsxCmoehM97650130 = vipeTPLsxCmoehM18426265;     vipeTPLsxCmoehM18426265 = vipeTPLsxCmoehM3631632;     vipeTPLsxCmoehM3631632 = vipeTPLsxCmoehM75486071;     vipeTPLsxCmoehM75486071 = vipeTPLsxCmoehM78706254;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void DqoCWWNWsHpvIOyq601768() {     float MSQbrUsoDzLypsv78699783 = -827928647;    float MSQbrUsoDzLypsv53397649 = -841978172;    float MSQbrUsoDzLypsv59750072 = -269502427;    float MSQbrUsoDzLypsv2748856 = -100844368;    float MSQbrUsoDzLypsv82632295 = -717561081;    float MSQbrUsoDzLypsv18775220 = -543018512;    float MSQbrUsoDzLypsv60318438 = -162145555;    float MSQbrUsoDzLypsv48442265 = -854704254;    float MSQbrUsoDzLypsv36200565 = -877212866;    float MSQbrUsoDzLypsv50659231 = -214695899;    float MSQbrUsoDzLypsv52220854 = -690579060;    float MSQbrUsoDzLypsv7556252 = -13283509;    float MSQbrUsoDzLypsv61414100 = -876727534;    float MSQbrUsoDzLypsv27413917 = 67209594;    float MSQbrUsoDzLypsv25385858 = -411039029;    float MSQbrUsoDzLypsv63884296 = 78987583;    float MSQbrUsoDzLypsv19994048 = -230949282;    float MSQbrUsoDzLypsv93977202 = -664587143;    float MSQbrUsoDzLypsv8732138 = -633569872;    float MSQbrUsoDzLypsv95579962 = -523175856;    float MSQbrUsoDzLypsv63669556 = 42192456;    float MSQbrUsoDzLypsv66468835 = -845337486;    float MSQbrUsoDzLypsv74907272 = -224191816;    float MSQbrUsoDzLypsv81126135 = -362819134;    float MSQbrUsoDzLypsv55787903 = -202880755;    float MSQbrUsoDzLypsv95480481 = -604109522;    float MSQbrUsoDzLypsv11824281 = -541594637;    float MSQbrUsoDzLypsv40913295 = -87831538;    float MSQbrUsoDzLypsv77125827 = -928203097;    float MSQbrUsoDzLypsv25892831 = -76873503;    float MSQbrUsoDzLypsv64177532 = -573983843;    float MSQbrUsoDzLypsv62419442 = -867533149;    float MSQbrUsoDzLypsv17353098 = -19325984;    float MSQbrUsoDzLypsv13909148 = -521390308;    float MSQbrUsoDzLypsv90859665 = -622710472;    float MSQbrUsoDzLypsv15705425 = -788312844;    float MSQbrUsoDzLypsv53259094 = -533024824;    float MSQbrUsoDzLypsv12640893 = -254749780;    float MSQbrUsoDzLypsv49102306 = -833370962;    float MSQbrUsoDzLypsv54578043 = -610026545;    float MSQbrUsoDzLypsv65532257 = -263379901;    float MSQbrUsoDzLypsv98128625 = -99672674;    float MSQbrUsoDzLypsv82367064 = -285214953;    float MSQbrUsoDzLypsv86118393 = -938557807;    float MSQbrUsoDzLypsv13708788 = -444505043;    float MSQbrUsoDzLypsv5846005 = -477350974;    float MSQbrUsoDzLypsv61870573 = -302285727;    float MSQbrUsoDzLypsv2963122 = -244730252;    float MSQbrUsoDzLypsv91421854 = -907505567;    float MSQbrUsoDzLypsv36301467 = -455089445;    float MSQbrUsoDzLypsv48896927 = -161652460;    float MSQbrUsoDzLypsv21043483 = 59521212;    float MSQbrUsoDzLypsv67362799 = -573777495;    float MSQbrUsoDzLypsv41210172 = -380820501;    float MSQbrUsoDzLypsv36932212 = -679211321;    float MSQbrUsoDzLypsv12230948 = -982591161;    float MSQbrUsoDzLypsv78490377 = -517786357;    float MSQbrUsoDzLypsv78623936 = -906683294;    float MSQbrUsoDzLypsv46960952 = -897963614;    float MSQbrUsoDzLypsv87151814 = -13451560;    float MSQbrUsoDzLypsv6950940 = 98576125;    float MSQbrUsoDzLypsv19405143 = 25685982;    float MSQbrUsoDzLypsv71316438 = -926501158;    float MSQbrUsoDzLypsv10307734 = -700339364;    float MSQbrUsoDzLypsv86481699 = -640712057;    float MSQbrUsoDzLypsv89801411 = -823045912;    float MSQbrUsoDzLypsv90203154 = -993957526;    float MSQbrUsoDzLypsv47504953 = -255337226;    float MSQbrUsoDzLypsv36554251 = -310079935;    float MSQbrUsoDzLypsv9680433 = -622726186;    float MSQbrUsoDzLypsv10625202 = -387987594;    float MSQbrUsoDzLypsv7353156 = -976199503;    float MSQbrUsoDzLypsv44874897 = -831216182;    float MSQbrUsoDzLypsv54154095 = 76456672;    float MSQbrUsoDzLypsv30047705 = -159795955;    float MSQbrUsoDzLypsv65540930 = -858134871;    float MSQbrUsoDzLypsv84101771 = -460122534;    float MSQbrUsoDzLypsv88788878 = -285634009;    float MSQbrUsoDzLypsv67417348 = -918314091;    float MSQbrUsoDzLypsv49941898 = -725529781;    float MSQbrUsoDzLypsv33609908 = -201823796;    float MSQbrUsoDzLypsv8861160 = -196864386;    float MSQbrUsoDzLypsv49491441 = -180325971;    float MSQbrUsoDzLypsv40824360 = -373113653;    float MSQbrUsoDzLypsv76995904 = -915221044;    float MSQbrUsoDzLypsv43134050 = -533505055;    float MSQbrUsoDzLypsv95056643 = -193755654;    float MSQbrUsoDzLypsv76142925 = -638505483;    float MSQbrUsoDzLypsv76976936 = -842178988;    float MSQbrUsoDzLypsv78628718 = -640119312;    float MSQbrUsoDzLypsv37215047 = -170526487;    float MSQbrUsoDzLypsv74635158 = -626341530;    float MSQbrUsoDzLypsv65679940 = -356786166;    float MSQbrUsoDzLypsv61950491 = -719919402;    float MSQbrUsoDzLypsv47627104 = -608602670;    float MSQbrUsoDzLypsv46127115 = -189065884;    float MSQbrUsoDzLypsv26812188 = -173171517;    float MSQbrUsoDzLypsv72059330 = -584875590;    float MSQbrUsoDzLypsv99636694 = -197845751;    float MSQbrUsoDzLypsv23907376 = -827928647;     MSQbrUsoDzLypsv78699783 = MSQbrUsoDzLypsv53397649;     MSQbrUsoDzLypsv53397649 = MSQbrUsoDzLypsv59750072;     MSQbrUsoDzLypsv59750072 = MSQbrUsoDzLypsv2748856;     MSQbrUsoDzLypsv2748856 = MSQbrUsoDzLypsv82632295;     MSQbrUsoDzLypsv82632295 = MSQbrUsoDzLypsv18775220;     MSQbrUsoDzLypsv18775220 = MSQbrUsoDzLypsv60318438;     MSQbrUsoDzLypsv60318438 = MSQbrUsoDzLypsv48442265;     MSQbrUsoDzLypsv48442265 = MSQbrUsoDzLypsv36200565;     MSQbrUsoDzLypsv36200565 = MSQbrUsoDzLypsv50659231;     MSQbrUsoDzLypsv50659231 = MSQbrUsoDzLypsv52220854;     MSQbrUsoDzLypsv52220854 = MSQbrUsoDzLypsv7556252;     MSQbrUsoDzLypsv7556252 = MSQbrUsoDzLypsv61414100;     MSQbrUsoDzLypsv61414100 = MSQbrUsoDzLypsv27413917;     MSQbrUsoDzLypsv27413917 = MSQbrUsoDzLypsv25385858;     MSQbrUsoDzLypsv25385858 = MSQbrUsoDzLypsv63884296;     MSQbrUsoDzLypsv63884296 = MSQbrUsoDzLypsv19994048;     MSQbrUsoDzLypsv19994048 = MSQbrUsoDzLypsv93977202;     MSQbrUsoDzLypsv93977202 = MSQbrUsoDzLypsv8732138;     MSQbrUsoDzLypsv8732138 = MSQbrUsoDzLypsv95579962;     MSQbrUsoDzLypsv95579962 = MSQbrUsoDzLypsv63669556;     MSQbrUsoDzLypsv63669556 = MSQbrUsoDzLypsv66468835;     MSQbrUsoDzLypsv66468835 = MSQbrUsoDzLypsv74907272;     MSQbrUsoDzLypsv74907272 = MSQbrUsoDzLypsv81126135;     MSQbrUsoDzLypsv81126135 = MSQbrUsoDzLypsv55787903;     MSQbrUsoDzLypsv55787903 = MSQbrUsoDzLypsv95480481;     MSQbrUsoDzLypsv95480481 = MSQbrUsoDzLypsv11824281;     MSQbrUsoDzLypsv11824281 = MSQbrUsoDzLypsv40913295;     MSQbrUsoDzLypsv40913295 = MSQbrUsoDzLypsv77125827;     MSQbrUsoDzLypsv77125827 = MSQbrUsoDzLypsv25892831;     MSQbrUsoDzLypsv25892831 = MSQbrUsoDzLypsv64177532;     MSQbrUsoDzLypsv64177532 = MSQbrUsoDzLypsv62419442;     MSQbrUsoDzLypsv62419442 = MSQbrUsoDzLypsv17353098;     MSQbrUsoDzLypsv17353098 = MSQbrUsoDzLypsv13909148;     MSQbrUsoDzLypsv13909148 = MSQbrUsoDzLypsv90859665;     MSQbrUsoDzLypsv90859665 = MSQbrUsoDzLypsv15705425;     MSQbrUsoDzLypsv15705425 = MSQbrUsoDzLypsv53259094;     MSQbrUsoDzLypsv53259094 = MSQbrUsoDzLypsv12640893;     MSQbrUsoDzLypsv12640893 = MSQbrUsoDzLypsv49102306;     MSQbrUsoDzLypsv49102306 = MSQbrUsoDzLypsv54578043;     MSQbrUsoDzLypsv54578043 = MSQbrUsoDzLypsv65532257;     MSQbrUsoDzLypsv65532257 = MSQbrUsoDzLypsv98128625;     MSQbrUsoDzLypsv98128625 = MSQbrUsoDzLypsv82367064;     MSQbrUsoDzLypsv82367064 = MSQbrUsoDzLypsv86118393;     MSQbrUsoDzLypsv86118393 = MSQbrUsoDzLypsv13708788;     MSQbrUsoDzLypsv13708788 = MSQbrUsoDzLypsv5846005;     MSQbrUsoDzLypsv5846005 = MSQbrUsoDzLypsv61870573;     MSQbrUsoDzLypsv61870573 = MSQbrUsoDzLypsv2963122;     MSQbrUsoDzLypsv2963122 = MSQbrUsoDzLypsv91421854;     MSQbrUsoDzLypsv91421854 = MSQbrUsoDzLypsv36301467;     MSQbrUsoDzLypsv36301467 = MSQbrUsoDzLypsv48896927;     MSQbrUsoDzLypsv48896927 = MSQbrUsoDzLypsv21043483;     MSQbrUsoDzLypsv21043483 = MSQbrUsoDzLypsv67362799;     MSQbrUsoDzLypsv67362799 = MSQbrUsoDzLypsv41210172;     MSQbrUsoDzLypsv41210172 = MSQbrUsoDzLypsv36932212;     MSQbrUsoDzLypsv36932212 = MSQbrUsoDzLypsv12230948;     MSQbrUsoDzLypsv12230948 = MSQbrUsoDzLypsv78490377;     MSQbrUsoDzLypsv78490377 = MSQbrUsoDzLypsv78623936;     MSQbrUsoDzLypsv78623936 = MSQbrUsoDzLypsv46960952;     MSQbrUsoDzLypsv46960952 = MSQbrUsoDzLypsv87151814;     MSQbrUsoDzLypsv87151814 = MSQbrUsoDzLypsv6950940;     MSQbrUsoDzLypsv6950940 = MSQbrUsoDzLypsv19405143;     MSQbrUsoDzLypsv19405143 = MSQbrUsoDzLypsv71316438;     MSQbrUsoDzLypsv71316438 = MSQbrUsoDzLypsv10307734;     MSQbrUsoDzLypsv10307734 = MSQbrUsoDzLypsv86481699;     MSQbrUsoDzLypsv86481699 = MSQbrUsoDzLypsv89801411;     MSQbrUsoDzLypsv89801411 = MSQbrUsoDzLypsv90203154;     MSQbrUsoDzLypsv90203154 = MSQbrUsoDzLypsv47504953;     MSQbrUsoDzLypsv47504953 = MSQbrUsoDzLypsv36554251;     MSQbrUsoDzLypsv36554251 = MSQbrUsoDzLypsv9680433;     MSQbrUsoDzLypsv9680433 = MSQbrUsoDzLypsv10625202;     MSQbrUsoDzLypsv10625202 = MSQbrUsoDzLypsv7353156;     MSQbrUsoDzLypsv7353156 = MSQbrUsoDzLypsv44874897;     MSQbrUsoDzLypsv44874897 = MSQbrUsoDzLypsv54154095;     MSQbrUsoDzLypsv54154095 = MSQbrUsoDzLypsv30047705;     MSQbrUsoDzLypsv30047705 = MSQbrUsoDzLypsv65540930;     MSQbrUsoDzLypsv65540930 = MSQbrUsoDzLypsv84101771;     MSQbrUsoDzLypsv84101771 = MSQbrUsoDzLypsv88788878;     MSQbrUsoDzLypsv88788878 = MSQbrUsoDzLypsv67417348;     MSQbrUsoDzLypsv67417348 = MSQbrUsoDzLypsv49941898;     MSQbrUsoDzLypsv49941898 = MSQbrUsoDzLypsv33609908;     MSQbrUsoDzLypsv33609908 = MSQbrUsoDzLypsv8861160;     MSQbrUsoDzLypsv8861160 = MSQbrUsoDzLypsv49491441;     MSQbrUsoDzLypsv49491441 = MSQbrUsoDzLypsv40824360;     MSQbrUsoDzLypsv40824360 = MSQbrUsoDzLypsv76995904;     MSQbrUsoDzLypsv76995904 = MSQbrUsoDzLypsv43134050;     MSQbrUsoDzLypsv43134050 = MSQbrUsoDzLypsv95056643;     MSQbrUsoDzLypsv95056643 = MSQbrUsoDzLypsv76142925;     MSQbrUsoDzLypsv76142925 = MSQbrUsoDzLypsv76976936;     MSQbrUsoDzLypsv76976936 = MSQbrUsoDzLypsv78628718;     MSQbrUsoDzLypsv78628718 = MSQbrUsoDzLypsv37215047;     MSQbrUsoDzLypsv37215047 = MSQbrUsoDzLypsv74635158;     MSQbrUsoDzLypsv74635158 = MSQbrUsoDzLypsv65679940;     MSQbrUsoDzLypsv65679940 = MSQbrUsoDzLypsv61950491;     MSQbrUsoDzLypsv61950491 = MSQbrUsoDzLypsv47627104;     MSQbrUsoDzLypsv47627104 = MSQbrUsoDzLypsv46127115;     MSQbrUsoDzLypsv46127115 = MSQbrUsoDzLypsv26812188;     MSQbrUsoDzLypsv26812188 = MSQbrUsoDzLypsv72059330;     MSQbrUsoDzLypsv72059330 = MSQbrUsoDzLypsv99636694;     MSQbrUsoDzLypsv99636694 = MSQbrUsoDzLypsv23907376;     MSQbrUsoDzLypsv23907376 = MSQbrUsoDzLypsv78699783;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void IAgMpuDbSpusmsvT88828878() {     float OvawOyKnSgpjHCF39750500 = -846714508;    float OvawOyKnSgpjHCF24494555 = -935412437;    float OvawOyKnSgpjHCF28702512 = -880502425;    float OvawOyKnSgpjHCF35614279 = -108777500;    float OvawOyKnSgpjHCF99419467 = -401726901;    float OvawOyKnSgpjHCF37201937 = -160914599;    float OvawOyKnSgpjHCF7749353 = -884606029;    float OvawOyKnSgpjHCF56369899 = -75442972;    float OvawOyKnSgpjHCF80307298 = 51877751;    float OvawOyKnSgpjHCF88080812 = -577137550;    float OvawOyKnSgpjHCF84415537 = -843542605;    float OvawOyKnSgpjHCF29150379 = -585634834;    float OvawOyKnSgpjHCF44376610 = -71329930;    float OvawOyKnSgpjHCF18270277 = -638525059;    float OvawOyKnSgpjHCF94677153 = -840297169;    float OvawOyKnSgpjHCF80582092 = -760219850;    float OvawOyKnSgpjHCF19775341 = -626629059;    float OvawOyKnSgpjHCF91852765 = -857220954;    float OvawOyKnSgpjHCF50318919 = -15124884;    float OvawOyKnSgpjHCF78504884 = -312151846;    float OvawOyKnSgpjHCF98096259 = -366315646;    float OvawOyKnSgpjHCF93097201 = -841714827;    float OvawOyKnSgpjHCF12318054 = -546958263;    float OvawOyKnSgpjHCF32983015 = -851572195;    float OvawOyKnSgpjHCF90266303 = -467668254;    float OvawOyKnSgpjHCF19093713 = 51069062;    float OvawOyKnSgpjHCF84411182 = -712892200;    float OvawOyKnSgpjHCF10136996 = -978298963;    float OvawOyKnSgpjHCF18992807 = 7478580;    float OvawOyKnSgpjHCF8658863 = -344858862;    float OvawOyKnSgpjHCF71394720 = -517518127;    float OvawOyKnSgpjHCF52708535 = -522478632;    float OvawOyKnSgpjHCF65305737 = -412798879;    float OvawOyKnSgpjHCF96961808 = -765547361;    float OvawOyKnSgpjHCF75715069 = 84067294;    float OvawOyKnSgpjHCF29229930 = -340707101;    float OvawOyKnSgpjHCF40374928 = -778579430;    float OvawOyKnSgpjHCF69511303 = -90857510;    float OvawOyKnSgpjHCF85102849 = -229904131;    float OvawOyKnSgpjHCF55760356 = -935827724;    float OvawOyKnSgpjHCF58279478 = -519695107;    float OvawOyKnSgpjHCF45805682 = -9464366;    float OvawOyKnSgpjHCF60667302 = -407871864;    float OvawOyKnSgpjHCF37811113 = -254341373;    float OvawOyKnSgpjHCF21688070 = -836777919;    float OvawOyKnSgpjHCF77779686 = -126777455;    float OvawOyKnSgpjHCF53286398 = -337460325;    float OvawOyKnSgpjHCF53418623 = 4321665;    float OvawOyKnSgpjHCF46467451 = -7154788;    float OvawOyKnSgpjHCF94595628 = -107114532;    float OvawOyKnSgpjHCF26544528 = -706032612;    float OvawOyKnSgpjHCF17240278 = 49935438;    float OvawOyKnSgpjHCF78609202 = 94332408;    float OvawOyKnSgpjHCF15850657 = -355472931;    float OvawOyKnSgpjHCF90732619 = -643892601;    float OvawOyKnSgpjHCF46653299 = 95000318;    float OvawOyKnSgpjHCF12176501 = -288454174;    float OvawOyKnSgpjHCF95719496 = 71069770;    float OvawOyKnSgpjHCF45347975 = -641109246;    float OvawOyKnSgpjHCF80325754 = -352795963;    float OvawOyKnSgpjHCF52790754 = -448022400;    float OvawOyKnSgpjHCF97612357 = -906307067;    float OvawOyKnSgpjHCF37377092 = 17078447;    float OvawOyKnSgpjHCF71648436 = -603263388;    float OvawOyKnSgpjHCF16686093 = 40380577;    float OvawOyKnSgpjHCF31707002 = -221063973;    float OvawOyKnSgpjHCF63844642 = -72835955;    float OvawOyKnSgpjHCF47414802 = -305782569;    float OvawOyKnSgpjHCF42555208 = -622592353;    float OvawOyKnSgpjHCF65447223 = -399590068;    float OvawOyKnSgpjHCF40207165 = -981640421;    float OvawOyKnSgpjHCF50264037 = -435771549;    float OvawOyKnSgpjHCF6749917 = -527316824;    float OvawOyKnSgpjHCF94558563 = -79297160;    float OvawOyKnSgpjHCF20225407 = -792456740;    float OvawOyKnSgpjHCF52290578 = -256851280;    float OvawOyKnSgpjHCF32429900 = -333842963;    float OvawOyKnSgpjHCF74506940 = -192616891;    float OvawOyKnSgpjHCF11294945 = 85205724;    float OvawOyKnSgpjHCF12486617 = -240890800;    float OvawOyKnSgpjHCF65807315 = -611470613;    float OvawOyKnSgpjHCF30992559 = -617213866;    float OvawOyKnSgpjHCF63669544 = -871144175;    float OvawOyKnSgpjHCF24397179 = -885406888;    float OvawOyKnSgpjHCF82114334 = -638826250;    float OvawOyKnSgpjHCF54154442 = -467453565;    float OvawOyKnSgpjHCF74099333 = -516811040;    float OvawOyKnSgpjHCF49455081 = 42674051;    float OvawOyKnSgpjHCF6229189 = -21654760;    float OvawOyKnSgpjHCF29061771 = 89066975;    float OvawOyKnSgpjHCF17053429 = 47747073;    float OvawOyKnSgpjHCF44655432 = -749649201;    float OvawOyKnSgpjHCF24163328 = -449748265;    float OvawOyKnSgpjHCF4777095 = -877108169;    float OvawOyKnSgpjHCF2969602 = -387805325;    float OvawOyKnSgpjHCF60667121 = -613388041;    float OvawOyKnSgpjHCF8428590 = 73457186;    float OvawOyKnSgpjHCF89018866 = -804608477;    float OvawOyKnSgpjHCF21125021 = -194721950;    float OvawOyKnSgpjHCF89981068 = -846714508;     OvawOyKnSgpjHCF39750500 = OvawOyKnSgpjHCF24494555;     OvawOyKnSgpjHCF24494555 = OvawOyKnSgpjHCF28702512;     OvawOyKnSgpjHCF28702512 = OvawOyKnSgpjHCF35614279;     OvawOyKnSgpjHCF35614279 = OvawOyKnSgpjHCF99419467;     OvawOyKnSgpjHCF99419467 = OvawOyKnSgpjHCF37201937;     OvawOyKnSgpjHCF37201937 = OvawOyKnSgpjHCF7749353;     OvawOyKnSgpjHCF7749353 = OvawOyKnSgpjHCF56369899;     OvawOyKnSgpjHCF56369899 = OvawOyKnSgpjHCF80307298;     OvawOyKnSgpjHCF80307298 = OvawOyKnSgpjHCF88080812;     OvawOyKnSgpjHCF88080812 = OvawOyKnSgpjHCF84415537;     OvawOyKnSgpjHCF84415537 = OvawOyKnSgpjHCF29150379;     OvawOyKnSgpjHCF29150379 = OvawOyKnSgpjHCF44376610;     OvawOyKnSgpjHCF44376610 = OvawOyKnSgpjHCF18270277;     OvawOyKnSgpjHCF18270277 = OvawOyKnSgpjHCF94677153;     OvawOyKnSgpjHCF94677153 = OvawOyKnSgpjHCF80582092;     OvawOyKnSgpjHCF80582092 = OvawOyKnSgpjHCF19775341;     OvawOyKnSgpjHCF19775341 = OvawOyKnSgpjHCF91852765;     OvawOyKnSgpjHCF91852765 = OvawOyKnSgpjHCF50318919;     OvawOyKnSgpjHCF50318919 = OvawOyKnSgpjHCF78504884;     OvawOyKnSgpjHCF78504884 = OvawOyKnSgpjHCF98096259;     OvawOyKnSgpjHCF98096259 = OvawOyKnSgpjHCF93097201;     OvawOyKnSgpjHCF93097201 = OvawOyKnSgpjHCF12318054;     OvawOyKnSgpjHCF12318054 = OvawOyKnSgpjHCF32983015;     OvawOyKnSgpjHCF32983015 = OvawOyKnSgpjHCF90266303;     OvawOyKnSgpjHCF90266303 = OvawOyKnSgpjHCF19093713;     OvawOyKnSgpjHCF19093713 = OvawOyKnSgpjHCF84411182;     OvawOyKnSgpjHCF84411182 = OvawOyKnSgpjHCF10136996;     OvawOyKnSgpjHCF10136996 = OvawOyKnSgpjHCF18992807;     OvawOyKnSgpjHCF18992807 = OvawOyKnSgpjHCF8658863;     OvawOyKnSgpjHCF8658863 = OvawOyKnSgpjHCF71394720;     OvawOyKnSgpjHCF71394720 = OvawOyKnSgpjHCF52708535;     OvawOyKnSgpjHCF52708535 = OvawOyKnSgpjHCF65305737;     OvawOyKnSgpjHCF65305737 = OvawOyKnSgpjHCF96961808;     OvawOyKnSgpjHCF96961808 = OvawOyKnSgpjHCF75715069;     OvawOyKnSgpjHCF75715069 = OvawOyKnSgpjHCF29229930;     OvawOyKnSgpjHCF29229930 = OvawOyKnSgpjHCF40374928;     OvawOyKnSgpjHCF40374928 = OvawOyKnSgpjHCF69511303;     OvawOyKnSgpjHCF69511303 = OvawOyKnSgpjHCF85102849;     OvawOyKnSgpjHCF85102849 = OvawOyKnSgpjHCF55760356;     OvawOyKnSgpjHCF55760356 = OvawOyKnSgpjHCF58279478;     OvawOyKnSgpjHCF58279478 = OvawOyKnSgpjHCF45805682;     OvawOyKnSgpjHCF45805682 = OvawOyKnSgpjHCF60667302;     OvawOyKnSgpjHCF60667302 = OvawOyKnSgpjHCF37811113;     OvawOyKnSgpjHCF37811113 = OvawOyKnSgpjHCF21688070;     OvawOyKnSgpjHCF21688070 = OvawOyKnSgpjHCF77779686;     OvawOyKnSgpjHCF77779686 = OvawOyKnSgpjHCF53286398;     OvawOyKnSgpjHCF53286398 = OvawOyKnSgpjHCF53418623;     OvawOyKnSgpjHCF53418623 = OvawOyKnSgpjHCF46467451;     OvawOyKnSgpjHCF46467451 = OvawOyKnSgpjHCF94595628;     OvawOyKnSgpjHCF94595628 = OvawOyKnSgpjHCF26544528;     OvawOyKnSgpjHCF26544528 = OvawOyKnSgpjHCF17240278;     OvawOyKnSgpjHCF17240278 = OvawOyKnSgpjHCF78609202;     OvawOyKnSgpjHCF78609202 = OvawOyKnSgpjHCF15850657;     OvawOyKnSgpjHCF15850657 = OvawOyKnSgpjHCF90732619;     OvawOyKnSgpjHCF90732619 = OvawOyKnSgpjHCF46653299;     OvawOyKnSgpjHCF46653299 = OvawOyKnSgpjHCF12176501;     OvawOyKnSgpjHCF12176501 = OvawOyKnSgpjHCF95719496;     OvawOyKnSgpjHCF95719496 = OvawOyKnSgpjHCF45347975;     OvawOyKnSgpjHCF45347975 = OvawOyKnSgpjHCF80325754;     OvawOyKnSgpjHCF80325754 = OvawOyKnSgpjHCF52790754;     OvawOyKnSgpjHCF52790754 = OvawOyKnSgpjHCF97612357;     OvawOyKnSgpjHCF97612357 = OvawOyKnSgpjHCF37377092;     OvawOyKnSgpjHCF37377092 = OvawOyKnSgpjHCF71648436;     OvawOyKnSgpjHCF71648436 = OvawOyKnSgpjHCF16686093;     OvawOyKnSgpjHCF16686093 = OvawOyKnSgpjHCF31707002;     OvawOyKnSgpjHCF31707002 = OvawOyKnSgpjHCF63844642;     OvawOyKnSgpjHCF63844642 = OvawOyKnSgpjHCF47414802;     OvawOyKnSgpjHCF47414802 = OvawOyKnSgpjHCF42555208;     OvawOyKnSgpjHCF42555208 = OvawOyKnSgpjHCF65447223;     OvawOyKnSgpjHCF65447223 = OvawOyKnSgpjHCF40207165;     OvawOyKnSgpjHCF40207165 = OvawOyKnSgpjHCF50264037;     OvawOyKnSgpjHCF50264037 = OvawOyKnSgpjHCF6749917;     OvawOyKnSgpjHCF6749917 = OvawOyKnSgpjHCF94558563;     OvawOyKnSgpjHCF94558563 = OvawOyKnSgpjHCF20225407;     OvawOyKnSgpjHCF20225407 = OvawOyKnSgpjHCF52290578;     OvawOyKnSgpjHCF52290578 = OvawOyKnSgpjHCF32429900;     OvawOyKnSgpjHCF32429900 = OvawOyKnSgpjHCF74506940;     OvawOyKnSgpjHCF74506940 = OvawOyKnSgpjHCF11294945;     OvawOyKnSgpjHCF11294945 = OvawOyKnSgpjHCF12486617;     OvawOyKnSgpjHCF12486617 = OvawOyKnSgpjHCF65807315;     OvawOyKnSgpjHCF65807315 = OvawOyKnSgpjHCF30992559;     OvawOyKnSgpjHCF30992559 = OvawOyKnSgpjHCF63669544;     OvawOyKnSgpjHCF63669544 = OvawOyKnSgpjHCF24397179;     OvawOyKnSgpjHCF24397179 = OvawOyKnSgpjHCF82114334;     OvawOyKnSgpjHCF82114334 = OvawOyKnSgpjHCF54154442;     OvawOyKnSgpjHCF54154442 = OvawOyKnSgpjHCF74099333;     OvawOyKnSgpjHCF74099333 = OvawOyKnSgpjHCF49455081;     OvawOyKnSgpjHCF49455081 = OvawOyKnSgpjHCF6229189;     OvawOyKnSgpjHCF6229189 = OvawOyKnSgpjHCF29061771;     OvawOyKnSgpjHCF29061771 = OvawOyKnSgpjHCF17053429;     OvawOyKnSgpjHCF17053429 = OvawOyKnSgpjHCF44655432;     OvawOyKnSgpjHCF44655432 = OvawOyKnSgpjHCF24163328;     OvawOyKnSgpjHCF24163328 = OvawOyKnSgpjHCF4777095;     OvawOyKnSgpjHCF4777095 = OvawOyKnSgpjHCF2969602;     OvawOyKnSgpjHCF2969602 = OvawOyKnSgpjHCF60667121;     OvawOyKnSgpjHCF60667121 = OvawOyKnSgpjHCF8428590;     OvawOyKnSgpjHCF8428590 = OvawOyKnSgpjHCF89018866;     OvawOyKnSgpjHCF89018866 = OvawOyKnSgpjHCF21125021;     OvawOyKnSgpjHCF21125021 = OvawOyKnSgpjHCF89981068;     OvawOyKnSgpjHCF89981068 = OvawOyKnSgpjHCF39750500;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void VZhOfpVeDAyWxYiH25473854() {     float IJsMbdLzvDjzrBj62137617 = -481118471;    float IJsMbdLzvDjzrBj8842055 = -883955491;    float IJsMbdLzvDjzrBj57471791 = -20156269;    float IJsMbdLzvDjzrBj50620968 = -115185031;    float IJsMbdLzvDjzrBj97593721 = -992783902;    float IJsMbdLzvDjzrBj52085053 = -21522977;    float IJsMbdLzvDjzrBj99905091 = -706593336;    float IJsMbdLzvDjzrBj32003758 = -546039616;    float IJsMbdLzvDjzrBj77470429 = -932318288;    float IJsMbdLzvDjzrBj79844397 = -319878882;    float IJsMbdLzvDjzrBj21957397 = -840167006;    float IJsMbdLzvDjzrBj85053328 = -836380127;    float IJsMbdLzvDjzrBj11384790 = -690047249;    float IJsMbdLzvDjzrBj60885029 = -66233815;    float IJsMbdLzvDjzrBj31412430 = -467774882;    float IJsMbdLzvDjzrBj24838005 = -41887393;    float IJsMbdLzvDjzrBj69598692 = -903908879;    float IJsMbdLzvDjzrBj97829181 = -208963645;    float IJsMbdLzvDjzrBj91600550 = -150227009;    float IJsMbdLzvDjzrBj57021168 = -564786300;    float IJsMbdLzvDjzrBj98979366 = -357802954;    float IJsMbdLzvDjzrBj56912419 = -246481140;    float IJsMbdLzvDjzrBj15611377 = -723038855;    float IJsMbdLzvDjzrBj59482802 = -315565052;    float IJsMbdLzvDjzrBj25806551 = -89227379;    float IJsMbdLzvDjzrBj7396708 = -392825162;    float IJsMbdLzvDjzrBj31500603 = -470478693;    float IJsMbdLzvDjzrBj16048446 = -766753417;    float IJsMbdLzvDjzrBj25885368 = -336778539;    float IJsMbdLzvDjzrBj17816042 = -265154728;    float IJsMbdLzvDjzrBj15685525 = -598834279;    float IJsMbdLzvDjzrBj10249727 = -74549987;    float IJsMbdLzvDjzrBj46344408 = -942142374;    float IJsMbdLzvDjzrBj56350495 = 10325789;    float IJsMbdLzvDjzrBj51944434 = 20310871;    float IJsMbdLzvDjzrBj70922798 = -656102477;    float IJsMbdLzvDjzrBj33814641 = -511527382;    float IJsMbdLzvDjzrBj23137404 = 41517018;    float IJsMbdLzvDjzrBj33410981 = -630950163;    float IJsMbdLzvDjzrBj75946070 = 27948247;    float IJsMbdLzvDjzrBj87036849 = 77127227;    float IJsMbdLzvDjzrBj34314073 = -867373057;    float IJsMbdLzvDjzrBj66217494 = -295402448;    float IJsMbdLzvDjzrBj37255232 = -251705022;    float IJsMbdLzvDjzrBj89671336 = -645921397;    float IJsMbdLzvDjzrBj85879968 = -309006550;    float IJsMbdLzvDjzrBj65583795 = -915870578;    float IJsMbdLzvDjzrBj98017298 = -852213326;    float IJsMbdLzvDjzrBj40927356 = 43128513;    float IJsMbdLzvDjzrBj91679373 = -249134791;    float IJsMbdLzvDjzrBj35413745 = 38891111;    float IJsMbdLzvDjzrBj91091534 = -380883844;    float IJsMbdLzvDjzrBj56923606 = -677578823;    float IJsMbdLzvDjzrBj76137201 = -631153740;    float IJsMbdLzvDjzrBj26494487 = -784596716;    float IJsMbdLzvDjzrBj5225198 = -134637332;    float IJsMbdLzvDjzrBj93230678 = -60916637;    float IJsMbdLzvDjzrBj97988988 = -704591217;    float IJsMbdLzvDjzrBj24814417 = 74042347;    float IJsMbdLzvDjzrBj90197013 = -499958741;    float IJsMbdLzvDjzrBj20584451 = -551044285;    float IJsMbdLzvDjzrBj83856646 = -939839920;    float IJsMbdLzvDjzrBj6118391 = -109261078;    float IJsMbdLzvDjzrBj59654387 = -567163560;    float IJsMbdLzvDjzrBj64158872 = -721044604;    float IJsMbdLzvDjzrBj11707671 = -665617020;    float IJsMbdLzvDjzrBj38708921 = -894237754;    float IJsMbdLzvDjzrBj55034295 = -600373039;    float IJsMbdLzvDjzrBj8940596 = 13455313;    float IJsMbdLzvDjzrBj60489631 = -811672405;    float IJsMbdLzvDjzrBj91023364 = -530360012;    float IJsMbdLzvDjzrBj46461288 = -845425897;    float IJsMbdLzvDjzrBj64418201 = -578013482;    float IJsMbdLzvDjzrBj15654480 = -78175256;    float IJsMbdLzvDjzrBj69984318 = -541913527;    float IJsMbdLzvDjzrBj64665294 = -490429898;    float IJsMbdLzvDjzrBj90694925 = -951078693;    float IJsMbdLzvDjzrBj78356144 = -371333834;    float IJsMbdLzvDjzrBj69811465 = -669643656;    float IJsMbdLzvDjzrBj39926582 = -780220829;    float IJsMbdLzvDjzrBj41812912 = -476954584;    float IJsMbdLzvDjzrBj33483305 = -618265368;    float IJsMbdLzvDjzrBj75121090 = -709881930;    float IJsMbdLzvDjzrBj34205994 = 12356252;    float IJsMbdLzvDjzrBj82402297 = -204045840;    float IJsMbdLzvDjzrBj24593990 = -117950435;    float IJsMbdLzvDjzrBj53326121 = -396971165;    float IJsMbdLzvDjzrBj70207206 = -210988635;    float IJsMbdLzvDjzrBj29856009 = -205077495;    float IJsMbdLzvDjzrBj46719236 = -845051798;    float IJsMbdLzvDjzrBj77692120 = -495185841;    float IJsMbdLzvDjzrBj35825652 = -806936165;    float IJsMbdLzvDjzrBj98322987 = 67474670;    float IJsMbdLzvDjzrBj43213967 = -30991423;    float IJsMbdLzvDjzrBj55361620 = -421007469;    float IJsMbdLzvDjzrBj3180203 = 16967146;    float IJsMbdLzvDjzrBj28195683 = -658111980;    float IJsMbdLzvDjzrBj6563107 = -728238888;    float IJsMbdLzvDjzrBj73096360 = -530660418;    float IJsMbdLzvDjzrBj77963666 = -481118471;     IJsMbdLzvDjzrBj62137617 = IJsMbdLzvDjzrBj8842055;     IJsMbdLzvDjzrBj8842055 = IJsMbdLzvDjzrBj57471791;     IJsMbdLzvDjzrBj57471791 = IJsMbdLzvDjzrBj50620968;     IJsMbdLzvDjzrBj50620968 = IJsMbdLzvDjzrBj97593721;     IJsMbdLzvDjzrBj97593721 = IJsMbdLzvDjzrBj52085053;     IJsMbdLzvDjzrBj52085053 = IJsMbdLzvDjzrBj99905091;     IJsMbdLzvDjzrBj99905091 = IJsMbdLzvDjzrBj32003758;     IJsMbdLzvDjzrBj32003758 = IJsMbdLzvDjzrBj77470429;     IJsMbdLzvDjzrBj77470429 = IJsMbdLzvDjzrBj79844397;     IJsMbdLzvDjzrBj79844397 = IJsMbdLzvDjzrBj21957397;     IJsMbdLzvDjzrBj21957397 = IJsMbdLzvDjzrBj85053328;     IJsMbdLzvDjzrBj85053328 = IJsMbdLzvDjzrBj11384790;     IJsMbdLzvDjzrBj11384790 = IJsMbdLzvDjzrBj60885029;     IJsMbdLzvDjzrBj60885029 = IJsMbdLzvDjzrBj31412430;     IJsMbdLzvDjzrBj31412430 = IJsMbdLzvDjzrBj24838005;     IJsMbdLzvDjzrBj24838005 = IJsMbdLzvDjzrBj69598692;     IJsMbdLzvDjzrBj69598692 = IJsMbdLzvDjzrBj97829181;     IJsMbdLzvDjzrBj97829181 = IJsMbdLzvDjzrBj91600550;     IJsMbdLzvDjzrBj91600550 = IJsMbdLzvDjzrBj57021168;     IJsMbdLzvDjzrBj57021168 = IJsMbdLzvDjzrBj98979366;     IJsMbdLzvDjzrBj98979366 = IJsMbdLzvDjzrBj56912419;     IJsMbdLzvDjzrBj56912419 = IJsMbdLzvDjzrBj15611377;     IJsMbdLzvDjzrBj15611377 = IJsMbdLzvDjzrBj59482802;     IJsMbdLzvDjzrBj59482802 = IJsMbdLzvDjzrBj25806551;     IJsMbdLzvDjzrBj25806551 = IJsMbdLzvDjzrBj7396708;     IJsMbdLzvDjzrBj7396708 = IJsMbdLzvDjzrBj31500603;     IJsMbdLzvDjzrBj31500603 = IJsMbdLzvDjzrBj16048446;     IJsMbdLzvDjzrBj16048446 = IJsMbdLzvDjzrBj25885368;     IJsMbdLzvDjzrBj25885368 = IJsMbdLzvDjzrBj17816042;     IJsMbdLzvDjzrBj17816042 = IJsMbdLzvDjzrBj15685525;     IJsMbdLzvDjzrBj15685525 = IJsMbdLzvDjzrBj10249727;     IJsMbdLzvDjzrBj10249727 = IJsMbdLzvDjzrBj46344408;     IJsMbdLzvDjzrBj46344408 = IJsMbdLzvDjzrBj56350495;     IJsMbdLzvDjzrBj56350495 = IJsMbdLzvDjzrBj51944434;     IJsMbdLzvDjzrBj51944434 = IJsMbdLzvDjzrBj70922798;     IJsMbdLzvDjzrBj70922798 = IJsMbdLzvDjzrBj33814641;     IJsMbdLzvDjzrBj33814641 = IJsMbdLzvDjzrBj23137404;     IJsMbdLzvDjzrBj23137404 = IJsMbdLzvDjzrBj33410981;     IJsMbdLzvDjzrBj33410981 = IJsMbdLzvDjzrBj75946070;     IJsMbdLzvDjzrBj75946070 = IJsMbdLzvDjzrBj87036849;     IJsMbdLzvDjzrBj87036849 = IJsMbdLzvDjzrBj34314073;     IJsMbdLzvDjzrBj34314073 = IJsMbdLzvDjzrBj66217494;     IJsMbdLzvDjzrBj66217494 = IJsMbdLzvDjzrBj37255232;     IJsMbdLzvDjzrBj37255232 = IJsMbdLzvDjzrBj89671336;     IJsMbdLzvDjzrBj89671336 = IJsMbdLzvDjzrBj85879968;     IJsMbdLzvDjzrBj85879968 = IJsMbdLzvDjzrBj65583795;     IJsMbdLzvDjzrBj65583795 = IJsMbdLzvDjzrBj98017298;     IJsMbdLzvDjzrBj98017298 = IJsMbdLzvDjzrBj40927356;     IJsMbdLzvDjzrBj40927356 = IJsMbdLzvDjzrBj91679373;     IJsMbdLzvDjzrBj91679373 = IJsMbdLzvDjzrBj35413745;     IJsMbdLzvDjzrBj35413745 = IJsMbdLzvDjzrBj91091534;     IJsMbdLzvDjzrBj91091534 = IJsMbdLzvDjzrBj56923606;     IJsMbdLzvDjzrBj56923606 = IJsMbdLzvDjzrBj76137201;     IJsMbdLzvDjzrBj76137201 = IJsMbdLzvDjzrBj26494487;     IJsMbdLzvDjzrBj26494487 = IJsMbdLzvDjzrBj5225198;     IJsMbdLzvDjzrBj5225198 = IJsMbdLzvDjzrBj93230678;     IJsMbdLzvDjzrBj93230678 = IJsMbdLzvDjzrBj97988988;     IJsMbdLzvDjzrBj97988988 = IJsMbdLzvDjzrBj24814417;     IJsMbdLzvDjzrBj24814417 = IJsMbdLzvDjzrBj90197013;     IJsMbdLzvDjzrBj90197013 = IJsMbdLzvDjzrBj20584451;     IJsMbdLzvDjzrBj20584451 = IJsMbdLzvDjzrBj83856646;     IJsMbdLzvDjzrBj83856646 = IJsMbdLzvDjzrBj6118391;     IJsMbdLzvDjzrBj6118391 = IJsMbdLzvDjzrBj59654387;     IJsMbdLzvDjzrBj59654387 = IJsMbdLzvDjzrBj64158872;     IJsMbdLzvDjzrBj64158872 = IJsMbdLzvDjzrBj11707671;     IJsMbdLzvDjzrBj11707671 = IJsMbdLzvDjzrBj38708921;     IJsMbdLzvDjzrBj38708921 = IJsMbdLzvDjzrBj55034295;     IJsMbdLzvDjzrBj55034295 = IJsMbdLzvDjzrBj8940596;     IJsMbdLzvDjzrBj8940596 = IJsMbdLzvDjzrBj60489631;     IJsMbdLzvDjzrBj60489631 = IJsMbdLzvDjzrBj91023364;     IJsMbdLzvDjzrBj91023364 = IJsMbdLzvDjzrBj46461288;     IJsMbdLzvDjzrBj46461288 = IJsMbdLzvDjzrBj64418201;     IJsMbdLzvDjzrBj64418201 = IJsMbdLzvDjzrBj15654480;     IJsMbdLzvDjzrBj15654480 = IJsMbdLzvDjzrBj69984318;     IJsMbdLzvDjzrBj69984318 = IJsMbdLzvDjzrBj64665294;     IJsMbdLzvDjzrBj64665294 = IJsMbdLzvDjzrBj90694925;     IJsMbdLzvDjzrBj90694925 = IJsMbdLzvDjzrBj78356144;     IJsMbdLzvDjzrBj78356144 = IJsMbdLzvDjzrBj69811465;     IJsMbdLzvDjzrBj69811465 = IJsMbdLzvDjzrBj39926582;     IJsMbdLzvDjzrBj39926582 = IJsMbdLzvDjzrBj41812912;     IJsMbdLzvDjzrBj41812912 = IJsMbdLzvDjzrBj33483305;     IJsMbdLzvDjzrBj33483305 = IJsMbdLzvDjzrBj75121090;     IJsMbdLzvDjzrBj75121090 = IJsMbdLzvDjzrBj34205994;     IJsMbdLzvDjzrBj34205994 = IJsMbdLzvDjzrBj82402297;     IJsMbdLzvDjzrBj82402297 = IJsMbdLzvDjzrBj24593990;     IJsMbdLzvDjzrBj24593990 = IJsMbdLzvDjzrBj53326121;     IJsMbdLzvDjzrBj53326121 = IJsMbdLzvDjzrBj70207206;     IJsMbdLzvDjzrBj70207206 = IJsMbdLzvDjzrBj29856009;     IJsMbdLzvDjzrBj29856009 = IJsMbdLzvDjzrBj46719236;     IJsMbdLzvDjzrBj46719236 = IJsMbdLzvDjzrBj77692120;     IJsMbdLzvDjzrBj77692120 = IJsMbdLzvDjzrBj35825652;     IJsMbdLzvDjzrBj35825652 = IJsMbdLzvDjzrBj98322987;     IJsMbdLzvDjzrBj98322987 = IJsMbdLzvDjzrBj43213967;     IJsMbdLzvDjzrBj43213967 = IJsMbdLzvDjzrBj55361620;     IJsMbdLzvDjzrBj55361620 = IJsMbdLzvDjzrBj3180203;     IJsMbdLzvDjzrBj3180203 = IJsMbdLzvDjzrBj28195683;     IJsMbdLzvDjzrBj28195683 = IJsMbdLzvDjzrBj6563107;     IJsMbdLzvDjzrBj6563107 = IJsMbdLzvDjzrBj73096360;     IJsMbdLzvDjzrBj73096360 = IJsMbdLzvDjzrBj77963666;     IJsMbdLzvDjzrBj77963666 = IJsMbdLzvDjzrBj62137617;}
// Junk Finished
