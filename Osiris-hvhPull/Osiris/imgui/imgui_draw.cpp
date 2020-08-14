// dear imgui, v1.77 WIP
// (drawing and font code)

/*

Index of this file:

// [SECTION] STB libraries implementation
// [SECTION] Style functions
// [SECTION] ImDrawList
// [SECTION] ImDrawListSplitter
// [SECTION] ImDrawData
// [SECTION] Helpers ShadeVertsXXX functions
// [SECTION] ImFontConfig
// [SECTION] ImFontAtlas
// [SECTION] ImFontAtlas glyph ranges helpers
// [SECTION] ImFontGlyphRangesBuilder
// [SECTION] ImFont
// [SECTION] ImGui Internal Render Helpers
// [SECTION] Decompression code
// [SECTION] Default font data (ProggyClean.ttf)

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

#include <stdio.h>      // vsnprintf, sscanf, printf
#if !defined(alloca)
#if defined(__GLIBC__) || defined(__sun) || defined(__APPLE__) || defined(__NEWLIB__)
#include <alloca.h>     // alloca (glibc uses <alloca.h>. Note that Cygwin may have _WIN32 defined, so the order matters here)
#elif defined(_WIN32)
#include <malloc.h>     // alloca
#if !defined(alloca)
#define alloca _alloca  // for clang with MS Codegen
#endif
#else
#include <stdlib.h>     // alloca
#endif
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127) // condition expression is constant
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference is.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant              // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wcomma")
#pragma clang diagnostic ignored "-Wcomma"                  // warning : possible misuse of comma operator here             //
#endif
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"      // warning : macro name is a reserved identifier                //
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"       // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wstack-protector"          // warning: stack protector not protecting local variables: variable length buffer
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-------------------------------------------------------------------------
// [SECTION] STB libraries implementation
//-------------------------------------------------------------------------

// Compile time options:
//#define IMGUI_STB_NAMESPACE           ImStb
//#define IMGUI_STB_TRUETYPE_FILENAME   "my_folder/stb_truetype.h"
//#define IMGUI_STB_RECT_PACK_FILENAME  "my_folder/stb_rect_pack.h"
//#define IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
//#define IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION

#ifdef IMGUI_STB_NAMESPACE
namespace IMGUI_STB_NAMESPACE
{
#endif

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4456)                             // declaration of 'xx' hides previous local declaration
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wcast-qual"              // warning : cast from 'const xxxx *' to 'xxx *' drops const qualifier //
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"              // warning: comparison is always true due to limited range of data type [-Wtype-limits]
#pragma GCC diagnostic ignored "-Wcast-qual"                // warning: cast from type 'const xxxx *' to type 'xxxx *' casts away qualifiers
#endif

#ifndef STB_RECT_PACK_IMPLEMENTATION                        // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_ASSERT(x)     do { IM_ASSERT(x); } while (0)
#define STBRP_SORT          ImQsort
#define STB_RECT_PACK_IMPLEMENTATION
#endif
#ifdef IMGUI_STB_RECT_PACK_FILENAME
#include IMGUI_STB_RECT_PACK_FILENAME
#else
#include "imstb_rectpack.h"
#endif
#endif

#ifndef STB_TRUETYPE_IMPLEMENTATION                         // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(x,u)   ((void)(u), IM_ALLOC(x))
#define STBTT_free(x,u)     ((void)(u), IM_FREE(x))
#define STBTT_assert(x)     do { IM_ASSERT(x); } while(0)
#define STBTT_fmod(x,y)     ImFmod(x,y)
#define STBTT_sqrt(x)       ImSqrt(x)
#define STBTT_pow(x,y)      ImPow(x,y)
#define STBTT_fabs(x)       ImFabs(x)
#define STBTT_ifloor(x)     ((int)ImFloorStd(x))
#define STBTT_iceil(x)      ((int)ImCeil(x))
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#ifdef IMGUI_STB_TRUETYPE_FILENAME
#include IMGUI_STB_TRUETYPE_FILENAME
#else
#include "imstb_truetype.h"
#endif
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#if defined(_MSC_VER)
#pragma warning (pop)
#endif

#ifdef IMGUI_STB_NAMESPACE
} // namespace ImStb
using namespace IMGUI_STB_NAMESPACE;
#endif

//-----------------------------------------------------------------------------
// [SECTION] Style functions
//-----------------------------------------------------------------------------

void ImGui::StyleColorsDark(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ImGui::StyleColorsClassic(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_Button] = ImVec4(0.35f, 0.40f, 0.61f, 0.62f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.16f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
    colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

// Those light colors are better suited with a thicker font than the default one + FrameBorder
void ImGui::StyleColorsLight(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.90f);
    colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

//-----------------------------------------------------------------------------
// ImDrawList
//-----------------------------------------------------------------------------

ImDrawListSharedData::ImDrawListSharedData()
{
    Font = NULL;
    FontSize = 0.0f;
    CurveTessellationTol = 0.0f;
    CircleSegmentMaxError = 0.0f;
    ClipRectFullscreen = ImVec4(-8192.0f, -8192.0f, +8192.0f, +8192.0f);
    InitialFlags = ImDrawListFlags_None;

    // Lookup tables
    for (int i = 0; i < IM_ARRAYSIZE(ArcFastVtx); i++)
    {
        const float a = ((float)i * 2 * IM_PI) / (float)IM_ARRAYSIZE(ArcFastVtx);
        ArcFastVtx[i] = ImVec2(ImCos(a), ImSin(a));
    }
    memset(CircleSegmentCounts, 0, sizeof(CircleSegmentCounts)); // This will be set by SetCircleSegmentMaxError()
}

void ImDrawListSharedData::SetCircleSegmentMaxError(float max_error)
{
    if (CircleSegmentMaxError == max_error)
        return;
    CircleSegmentMaxError = max_error;
    for (int i = 0; i < IM_ARRAYSIZE(CircleSegmentCounts); i++)
    {
        const float radius = i + 1.0f;
        const int segment_count = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, CircleSegmentMaxError);
        CircleSegmentCounts[i] = (ImU8)ImMin(segment_count, 255);
    }
}

// Initialize before use in a new frame. We always have a command ready in the buffer.
void ImDrawList::_ResetForNewFrame()
{
    // Verify that the ImDrawCmd fields we want to memcmp() are contiguous in memory.
    // (those should be IM_STATIC_ASSERT() in theory but with our pre C++11 setup the whole check doesn't compile with GCC)
    IM_ASSERT(IM_OFFSETOF(ImDrawCmd, ClipRect) == 0);
    IM_ASSERT(IM_OFFSETOF(ImDrawCmd, TextureId) == sizeof(ImVec4));
    IM_ASSERT(IM_OFFSETOF(ImDrawCmd, VtxOffset) == sizeof(ImVec4) + sizeof(ImTextureID));

    CmdBuffer.resize(0);
    IdxBuffer.resize(0);
    VtxBuffer.resize(0);
    Flags = _Data->InitialFlags;
    memset(&_CmdHeader, 0, sizeof(_CmdHeader));
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.resize(0);
    _TextureIdStack.resize(0);
    _Path.resize(0);
    _Splitter.Clear();
    CmdBuffer.push_back(ImDrawCmd());
}

void ImDrawList::_ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    Flags = ImDrawListFlags_None;
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _Splitter.ClearFreeMemory();
}

ImDrawList* ImDrawList::CloneOutput() const
{
    ImDrawList* dst = IM_NEW(ImDrawList(_Data));
    dst->CmdBuffer = CmdBuffer;
    dst->IdxBuffer = IdxBuffer;
    dst->VtxBuffer = VtxBuffer;
    dst->Flags = Flags;
    return dst;
}

void ImDrawList::AddDrawCmd()
{
    ImDrawCmd draw_cmd;
    draw_cmd.ClipRect = _CmdHeader.ClipRect;    // Same as calling ImDrawCmd_HeaderCopy()
    draw_cmd.TextureId = _CmdHeader.TextureId;
    draw_cmd.VtxOffset = _CmdHeader.VtxOffset;
    draw_cmd.IdxOffset = IdxBuffer.Size;

    IM_ASSERT(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
    CmdBuffer.push_back(draw_cmd);
}

// Pop trailing draw command (used before merging or presenting to user)
// Note that this leaves the ImDrawList in a state unfit for further commands, as most code assume that CmdBuffer.Size > 0 && CmdBuffer.back().UserCallback == NULL
void ImDrawList::_PopUnusedDrawCmd()
{
    if (CmdBuffer.Size == 0)
        return;
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount == 0 && curr_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
}

void ImDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    IM_ASSERT(curr_cmd->UserCallback == NULL);
    if (curr_cmd->ElemCount != 0)
    {
        AddDrawCmd();
        curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    }
    curr_cmd->UserCallback = callback;
    curr_cmd->UserCallbackData = callback_data;

    AddDrawCmd(); // Force a new command after us (see comment below)
}

// Compare ClipRect, TextureId and VtxOffset with a single memcmp()
#define ImDrawCmd_HeaderSize                        (IM_OFFSETOF(ImDrawCmd, VtxOffset) + sizeof(unsigned int))
#define ImDrawCmd_HeaderCompare(CMD_LHS, CMD_RHS)   (memcmp(CMD_LHS, CMD_RHS, ImDrawCmd_HeaderSize))    // Compare ClipRect, TextureId, VtxOffset
#define ImDrawCmd_HeaderCopy(CMD_DST, CMD_SRC)      (memcpy(CMD_DST, CMD_SRC, ImDrawCmd_HeaderSize))    // Copy ClipRect, TextureId, VtxOffset

// Our scheme may appears a bit unusual, basically we want the most-common calls AddLine AddRect etc. to not have to perform any check so we always have a command ready in the stack.
// The cost of figuring out if a new command has to be added or if we can merge is paid in those Update** functions only.
void ImDrawList::_OnChangedClipRect()
{
    // If current command is used with different settings we need to add a new command
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount != 0 && memcmp(&curr_cmd->ClipRect, &_CmdHeader.ClipRect, sizeof(ImVec4)) != 0)
    {
        AddDrawCmd();
        return;
    }
    IM_ASSERT(curr_cmd->UserCallback == NULL);

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = curr_cmd - 1;
    if (curr_cmd->ElemCount == 0 && CmdBuffer.Size > 1 && ImDrawCmd_HeaderCompare(&_CmdHeader, prev_cmd) == 0 && prev_cmd->UserCallback == NULL)
    {
        CmdBuffer.pop_back();
        return;
    }

    curr_cmd->ClipRect = _CmdHeader.ClipRect;
}

void ImDrawList::_OnChangedTextureID()
{
    // If current command is used with different settings we need to add a new command
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount != 0 && curr_cmd->TextureId != _CmdHeader.TextureId)
    {
        AddDrawCmd();
        return;
    }
    IM_ASSERT(curr_cmd->UserCallback == NULL);

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = curr_cmd - 1;
    if (curr_cmd->ElemCount == 0 && CmdBuffer.Size > 1 && ImDrawCmd_HeaderCompare(&_CmdHeader, prev_cmd) == 0 && prev_cmd->UserCallback == NULL)
    {
        CmdBuffer.pop_back();
        return;
    }

    curr_cmd->TextureId = _CmdHeader.TextureId;
}

void ImDrawList::_OnChangedVtxOffset()
{
    // We don't need to compare curr_cmd->VtxOffset != _CmdHeader.VtxOffset because we know it'll be different at the time we call this.
    _VtxCurrentIdx = 0;
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    IM_ASSERT(curr_cmd->VtxOffset != _CmdHeader.VtxOffset);
    if (curr_cmd->ElemCount != 0)
    {
        AddDrawCmd();
        return;
    }
    IM_ASSERT(curr_cmd->UserCallback == NULL);
    curr_cmd->VtxOffset = _CmdHeader.VtxOffset;
}

// Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::PushClipRect() to affect logic (hit-testing and widget culling)
void ImDrawList::PushClipRect(ImVec2 cr_min, ImVec2 cr_max, bool intersect_with_current_clip_rect)
{
    ImVec4 cr(cr_min.x, cr_min.y, cr_max.x, cr_max.y);
    if (intersect_with_current_clip_rect)
    {
        ImVec4 current = _CmdHeader.ClipRect;
        if (cr.x < current.x) cr.x = current.x;
        if (cr.y < current.y) cr.y = current.y;
        if (cr.z > current.z) cr.z = current.z;
        if (cr.w > current.w) cr.w = current.w;
    }
    cr.z = ImMax(cr.x, cr.z);
    cr.w = ImMax(cr.y, cr.w);

    _ClipRectStack.push_back(cr);
    _CmdHeader.ClipRect = cr;
    _OnChangedClipRect();
}

void ImDrawList::PushClipRectFullScreen()
{
    PushClipRect(ImVec2(_Data->ClipRectFullscreen.x, _Data->ClipRectFullscreen.y), ImVec2(_Data->ClipRectFullscreen.z, _Data->ClipRectFullscreen.w));
}

void ImDrawList::PopClipRect()
{
    _ClipRectStack.pop_back();
    _CmdHeader.ClipRect = (_ClipRectStack.Size == 0) ? _Data->ClipRectFullscreen : _ClipRectStack.Data[_ClipRectStack.Size - 1];
    _OnChangedClipRect();
}

void ImDrawList::PushTextureID(ImTextureID texture_id)
{
    _TextureIdStack.push_back(texture_id);
    _CmdHeader.TextureId = texture_id;
    _OnChangedTextureID();
}

void ImDrawList::PopTextureID()
{
    _TextureIdStack.pop_back();
    _CmdHeader.TextureId = (_TextureIdStack.Size == 0) ? (ImTextureID)NULL : _TextureIdStack.Data[_TextureIdStack.Size - 1];
    _OnChangedTextureID();
}

// Reserve space for a number of vertices and indices.
// You must finish filling your reserved data before calling PrimReserve() again, as it may reallocate or
// submit the intermediate results. PrimUnreserve() can be used to release unused allocations.
void ImDrawList::PrimReserve(int idx_count, int vtx_count)
{
    // Large mesh support (when enabled)
    IM_ASSERT_PARANOID(idx_count >= 0 && vtx_count >= 0);
    if (sizeof(ImDrawIdx) == 2 && (_VtxCurrentIdx + vtx_count >= (1 << 16)) && (Flags & ImDrawListFlags_AllowVtxOffset))
    {
        _CmdHeader.VtxOffset = VtxBuffer.Size;
        _OnChangedVtxOffset();
    }

    ImDrawCmd* draw_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    draw_cmd->ElemCount += idx_count;

    int vtx_buffer_old_size = VtxBuffer.Size;
    VtxBuffer.resize(vtx_buffer_old_size + vtx_count);
    _VtxWritePtr = VtxBuffer.Data + vtx_buffer_old_size;

    int idx_buffer_old_size = IdxBuffer.Size;
    IdxBuffer.resize(idx_buffer_old_size + idx_count);
    _IdxWritePtr = IdxBuffer.Data + idx_buffer_old_size;
}

// Release the a number of reserved vertices/indices from the end of the last reservation made with PrimReserve().
void ImDrawList::PrimUnreserve(int idx_count, int vtx_count)
{
    IM_ASSERT_PARANOID(idx_count >= 0 && vtx_count >= 0);

    ImDrawCmd* draw_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    draw_cmd->ElemCount -= idx_count;
    VtxBuffer.shrink(VtxBuffer.Size - vtx_count);
    IdxBuffer.shrink(IdxBuffer.Size - idx_count);
}

// Fully unrolled with inline call to keep our debug builds decently fast.
void ImDrawList::PrimRect(const ImVec2& a, const ImVec2& c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv(_Data->TexUvWhitePixel);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
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
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col)
{
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

// On AddPolyline() and AddConvexPolyFilled() we intentionally avoid using ImVec2 and superfluous function calls to optimize debug/non-inlined builds.
// Those macros expects l-values.
#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     do { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = 1.0f / ImSqrt(d2); VX *= inv_len; VY *= inv_len; } } while (0)
#define IM_FIXNORMAL2F(VX,VY)               do { float d2 = VX*VX + VY*VY; if (d2 < 0.5f) d2 = 0.5f; float inv_lensq = 1.0f / d2; VX *= inv_lensq; VY *= inv_lensq; } while (0)

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
// We avoid using the ImVec2 math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddPolyline(const ImVec2* points, const int points_count, ImU32 col, bool closed, float thickness)
{
    if (points_count < 2)
        return;

    const ImVec2 opaque_uv = _Data->TexUvWhitePixel;
    int count = points_count;
    if (!closed)
        count = points_count - 1;

    const bool thick_line = (thickness > 1.0f);
    if (Flags & ImDrawListFlags_AntiAliasedLines)
    {
        // Anti-aliased stroke
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;

        const int idx_count = thick_line ? count * 18 : count * 12;
        const int vtx_count = thick_line ? points_count * 4 : points_count * 3;
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(ImVec2)); //-V630
        ImVec2* temp_points = temp_normals + points_count;

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            float dx = points[i2].x - points[i1].x;
            float dy = points[i2].y - points[i1].y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i1].x = dy;
            temp_normals[i1].y = -dx;
        }
        if (!closed)
            temp_normals[points_count - 1] = temp_normals[points_count - 2];

        if (!thick_line)
        {
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
                temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
                unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 3;

                // Average normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_FIXNORMAL2F(dm_x, dm_y);
                dm_x *= AA_SIZE;
                dm_y *= AA_SIZE;

                // Add temporary vertices
                ImVec2* out_vtx = &temp_points[i2 * 2];
                out_vtx[0].x = points[i2].x + dm_x;
                out_vtx[0].y = points[i2].y + dm_y;
                out_vtx[1].x = points[i2].x - dm_x;
                out_vtx[1].y = points[i2].y - dm_y;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 0);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8] = (ImDrawIdx)(idx1 + 0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr += 12;

                idx1 = idx2;
            }

            // Add vertices
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = points[i];          _VtxWritePtr[0].uv = opaque_uv; _VtxWritePtr[0].col = col;
                _VtxWritePtr[1].pos = temp_points[i * 2 + 0]; _VtxWritePtr[1].uv = opaque_uv; _VtxWritePtr[1].col = col_trans;
                _VtxWritePtr[2].pos = temp_points[i * 2 + 1]; _VtxWritePtr[2].uv = opaque_uv; _VtxWritePtr[2].col = col_trans;
                _VtxWritePtr += 3;
            }
        } else
        {
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
            if (!closed)
            {
                const int points_last = points_count - 1;
                temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
                temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
                temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[points_last * 4 + 0] = points[points_last] + temp_normals[points_last] * (half_inner_thickness + AA_SIZE);
                temp_points[points_last * 4 + 1] = points[points_last] + temp_normals[points_last] * (half_inner_thickness);
                temp_points[points_last * 4 + 2] = points[points_last] - temp_normals[points_last] * (half_inner_thickness);
                temp_points[points_last * 4 + 3] = points[points_last] - temp_normals[points_last] * (half_inner_thickness + AA_SIZE);
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == points_count ? 0 : (i1 + 1); // i2 is the second point of the line segment
                const unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : (idx1 + 4); // Vertex index for end of segment

                // Average normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_FIXNORMAL2F(dm_x, dm_y);
                float dm_out_x = dm_x * (half_inner_thickness + AA_SIZE);
                float dm_out_y = dm_y * (half_inner_thickness + AA_SIZE);
                float dm_in_x = dm_x * half_inner_thickness;
                float dm_in_y = dm_y * half_inner_thickness;

                // Add temporary vertices
                ImVec2* out_vtx = &temp_points[i2 * 4];
                out_vtx[0].x = points[i2].x + dm_out_x;
                out_vtx[0].y = points[i2].y + dm_out_y;
                out_vtx[1].x = points[i2].x + dm_in_x;
                out_vtx[1].y = points[i2].y + dm_in_y;
                out_vtx[2].x = points[i2].x - dm_in_x;
                out_vtx[2].y = points[i2].y - dm_in_y;
                out_vtx[3].x = points[i2].x - dm_out_x;
                out_vtx[3].y = points[i2].y - dm_out_y;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8] = (ImDrawIdx)(idx1 + 0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[12] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[13] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[14] = (ImDrawIdx)(idx1 + 3);
                _IdxWritePtr[15] = (ImDrawIdx)(idx1 + 3); _IdxWritePtr[16] = (ImDrawIdx)(idx2 + 3); _IdxWritePtr[17] = (ImDrawIdx)(idx2 + 2);
                _IdxWritePtr += 18;

                idx1 = idx2;
            }

            // Add vertices
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = temp_points[i * 4 + 0]; _VtxWritePtr[0].uv = opaque_uv; _VtxWritePtr[0].col = col_trans;
                _VtxWritePtr[1].pos = temp_points[i * 4 + 1]; _VtxWritePtr[1].uv = opaque_uv; _VtxWritePtr[1].col = col;
                _VtxWritePtr[2].pos = temp_points[i * 4 + 2]; _VtxWritePtr[2].uv = opaque_uv; _VtxWritePtr[2].col = col;
                _VtxWritePtr[3].pos = temp_points[i * 4 + 3]; _VtxWritePtr[3].uv = opaque_uv; _VtxWritePtr[3].col = col_trans;
                _VtxWritePtr += 4;
            }
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    } else
    {
        // Non Anti-aliased Stroke
        const int idx_count = count * 6;
        const int vtx_count = count * 4;      // FIXME-OPT: Not sharing edges
        PrimReserve(idx_count, vtx_count);

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            const ImVec2& p1 = points[i1];
            const ImVec2& p2 = points[i2];

            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            dx *= (thickness * 0.5f);
            dy *= (thickness * 0.5f);

            _VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = opaque_uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = opaque_uv; _VtxWritePtr[1].col = col;
            _VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = opaque_uv; _VtxWritePtr[2].col = col;
            _VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = opaque_uv; _VtxWritePtr[3].col = col;
            _VtxWritePtr += 4;

            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + 2);
            _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx + 2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx + 3);
            _IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}

// We intentionally avoid using ImVec2 and its math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddConvexPolyFilled(const ImVec2* points, const int points_count, ImU32 col)
{
    if (points_count < 3)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;

    if (Flags & ImDrawListFlags_AntiAliasedFill)
    {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;
        const int idx_count = (points_count - 2) * 3 + points_count * 6;
        const int vtx_count = (points_count * 2);
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
            _IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2)); //-V630
        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i0].x = dy;
            temp_normals[i0].y = -dx;
        }

        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            // Average normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
            float dm_x = (n0.x + n1.x) * 0.5f;
            float dm_y = (n0.y + n1.y) * 0.5f;
            IM_FIXNORMAL2F(dm_x, dm_y);
            dm_x *= AA_SIZE * 0.5f;
            dm_y *= AA_SIZE * 0.5f;

            // Add vertices
            _VtxWritePtr[0].pos.x = (points[i1].x - dm_x); _VtxWritePtr[0].pos.y = (points[i1].y - dm_y); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
            _VtxWritePtr[1].pos.x = (points[i1].x + dm_x); _VtxWritePtr[1].pos.y = (points[i1].y + dm_y); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
            _VtxWritePtr += 2;

            // Add indexes for fringes
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
            _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
            _IdxWritePtr += 6;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    } else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count - 2) * 3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            _VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + i - 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + i);
            _IdxWritePtr += 3;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}

void ImDrawList::PathArcToFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12)
{
    if (radius == 0.0f || a_min_of_12 > a_max_of_12)
    {
        _Path.push_back(center);
        return;
    }

    // For legacy reason the PathArcToFast() always takes angles where 2*PI is represented by 12,
    // but it is possible to set IM_DRAWLIST_ARCFAST_TESSELATION_MULTIPLIER to a higher value. This should compile to a no-op otherwise.
#if IM_DRAWLIST_ARCFAST_TESSELLATION_MULTIPLIER != 1
    a_min_of_12 *= IM_DRAWLIST_ARCFAST_TESSELLATION_MULTIPLIER;
    a_max_of_12 *= IM_DRAWLIST_ARCFAST_TESSELLATION_MULTIPLIER;
#endif

    _Path.reserve(_Path.Size + (a_max_of_12 - a_min_of_12 + 1));
    for (int a = a_min_of_12; a <= a_max_of_12; a++)
    {
        const ImVec2& c = _Data->ArcFastVtx[a % IM_ARRAYSIZE(_Data->ArcFastVtx)];
        _Path.push_back(ImVec2(center.x + c.x * radius, center.y + c.y * radius));
    }
}

void ImDrawList::PathArcTo(const ImVec2& center, float radius, float a_min, float a_max, int num_segments)
{
    if (radius == 0.0f)
    {
        _Path.push_back(center);
        return;
    }

    // Note that we are adding a point at both a_min and a_max.
    // If you are trying to draw a full closed circle you don't want the overlapping points!
    _Path.reserve(_Path.Size + (num_segments + 1));
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        _Path.push_back(ImVec2(center.x + ImCos(a) * radius, center.y + ImSin(a) * radius));
    }
}

ImVec2 ImBezierCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t)
{
    float u = 1.0f - t;
    float w1 = u * u * u;
    float w2 = 3 * u * u * t;
    float w3 = 3 * u * t * t;
    float w4 = t * t * t;
    return ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y);
}

// Closely mimics BezierClosestPointCasteljauStep() in imgui.cpp
static void PathBezierToCasteljau(ImVector<ImVec2>* path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
    float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy))
    {
        path->push_back(ImVec2(x4, y4));
    } else if (level < 10)
    {
        float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
        float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
        float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;
        PathBezierToCasteljau(path, x1, y1, x12, y12, x123, y123, x1234, y1234, tess_tol, level + 1);
        PathBezierToCasteljau(path, x1234, y1234, x234, y234, x34, y34, x4, y4, tess_tol, level + 1);
    }
}

void ImDrawList::PathBezierCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if (num_segments == 0)
    {
        PathBezierToCasteljau(&_Path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, _Data->CurveTessellationTol, 0); // Auto-tessellated
    } else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
            _Path.push_back(ImBezierCalc(p1, p2, p3, p4, t_step * i_step));
    }
}

void ImDrawList::PathRect(const ImVec2& a, const ImVec2& b, float rounding, ImDrawCornerFlags rounding_corners)
{
    rounding = ImMin(rounding, ImFabs(b.x - a.x) * (((rounding_corners & ImDrawCornerFlags_Top) == ImDrawCornerFlags_Top) || ((rounding_corners & ImDrawCornerFlags_Bot) == ImDrawCornerFlags_Bot) ? 0.5f : 1.0f) - 1.0f);
    rounding = ImMin(rounding, ImFabs(b.y - a.y) * (((rounding_corners & ImDrawCornerFlags_Left) == ImDrawCornerFlags_Left) || ((rounding_corners & ImDrawCornerFlags_Right) == ImDrawCornerFlags_Right) ? 0.5f : 1.0f) - 1.0f);

    if (rounding <= 0.0f || rounding_corners == 0)
    {
        PathLineTo(a);
        PathLineTo(ImVec2(b.x, a.y));
        PathLineTo(b);
        PathLineTo(ImVec2(a.x, b.y));
    } else
    {
        const float rounding_tl = (rounding_corners & ImDrawCornerFlags_TopLeft) ? rounding : 0.0f;
        const float rounding_tr = (rounding_corners & ImDrawCornerFlags_TopRight) ? rounding : 0.0f;
        const float rounding_br = (rounding_corners & ImDrawCornerFlags_BotRight) ? rounding : 0.0f;
        const float rounding_bl = (rounding_corners & ImDrawCornerFlags_BotLeft) ? rounding : 0.0f;
        PathArcToFast(ImVec2(a.x + rounding_tl, a.y + rounding_tl), rounding_tl, 6, 9);
        PathArcToFast(ImVec2(b.x - rounding_tr, a.y + rounding_tr), rounding_tr, 9, 12);
        PathArcToFast(ImVec2(b.x - rounding_br, b.y - rounding_br), rounding_br, 0, 3);
        PathArcToFast(ImVec2(a.x + rounding_bl, b.y - rounding_bl), rounding_bl, 3, 6);
    }
}

void ImDrawList::AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(p1 + ImVec2(0.5f, 0.5f));
    PathLineTo(p2 + ImVec2(0.5f, 0.5f));
    PathStroke(col, false, thickness);
}

// p_min = upper-left, p_max = lower-right
// Note we don't render 1 pixels sized rectangles properly.
void ImDrawList::AddRect(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawCornerFlags rounding_corners, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
        PathRect(p_min + ImVec2(0.50f, 0.50f), p_max - ImVec2(0.50f, 0.50f), rounding, rounding_corners);
    else
        PathRect(p_min + ImVec2(0.50f, 0.50f), p_max - ImVec2(0.49f, 0.49f), rounding, rounding_corners); // Better looking lower-right corner and rounded non-AA shapes.
    PathStroke(col, true, thickness);
}

void ImDrawList::AddRectFilled(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawCornerFlags rounding_corners)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (rounding > 0.0f)
    {
        PathRect(p_min, p_max, rounding, rounding_corners);
        PathFillConvex(col);
    } else
    {
        PrimReserve(6, 4);
        PrimRect(p_min, p_max, col);
    }
}

// p_min = upper-left, p_max = lower-right
void ImDrawList::AddRectFilledMultiColor(const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 1)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2));
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 3));
    PrimWriteVtx(p_min, uv, col_upr_left);
    PrimWriteVtx(ImVec2(p_max.x, p_min.y), uv, col_upr_right);
    PrimWriteVtx(p_max, uv, col_bot_right);
    PrimWriteVtx(ImVec2(p_min.x, p_max.y), uv, col_bot_left);
}

void ImDrawList::AddQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathLineTo(p4);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathLineTo(p4);
    PathFillConvex(col);
}

void ImDrawList::AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathFillConvex(col);
}

void ImDrawList::AddCircle(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || radius <= 0.0f)
        return;

    // Obtain segment count
    if (num_segments <= 0)
    {
        // Automatic segment count
        const int radius_idx = (int)radius - 1;
        if (radius_idx < IM_ARRAYSIZE(_Data->CircleSegmentCounts))
            num_segments = _Data->CircleSegmentCounts[radius_idx]; // Use cached value
        else
            num_segments = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, _Data->CircleSegmentMaxError);
    } else
    {
        // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
        num_segments = ImClamp(num_segments, 3, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);
    }

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    if (num_segments == 12)
        PathArcToFast(center, radius - 0.5f, 0, 12);
    else
        PathArcTo(center, radius - 0.5f, 0.0f, a_max, num_segments - 1);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || radius <= 0.0f)
        return;

    // Obtain segment count
    if (num_segments <= 0)
    {
        // Automatic segment count
        const int radius_idx = (int)radius - 1;
        if (radius_idx < IM_ARRAYSIZE(_Data->CircleSegmentCounts))
            num_segments = _Data->CircleSegmentCounts[radius_idx]; // Use cached value
        else
            num_segments = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, _Data->CircleSegmentMaxError);
    } else
    {
        // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
        num_segments = ImClamp(num_segments, 3, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);
    }

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    if (num_segments == 12)
        PathArcToFast(center, radius, 0, 12);
    else
        PathArcTo(center, radius, 0.0f, a_max, num_segments - 1);
    PathFillConvex(col);
}

// Guaranteed to honor 'num_segments'
void ImDrawList::AddNgon(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(center, radius - 0.5f, 0.0f, a_max, num_segments - 1);
    PathStroke(col, true, thickness);
}

// Guaranteed to honor 'num_segments'
void ImDrawList::AddNgonFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(center, radius, 0.0f, a_max, num_segments - 1);
    PathFillConvex(col);
}

// Cubic Bezier takes 4 controls points
void ImDrawList::AddBezierCurve(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathBezierCurveTo(p2, p3, p4, num_segments);
    PathStroke(col, false, thickness);
}

void ImDrawList::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (text_end == NULL)
        text_end = text_begin + strlen(text_begin);
    if (text_begin == text_end)
        return;

    // Pull default font/size from the shared ImDrawListSharedData instance
    if (font == NULL)
        font = _Data->Font;
    if (font_size == 0.0f)
        font_size = _Data->FontSize;

    IM_ASSERT(font->ContainerAtlas->TexID == _CmdHeader.TextureId);  // Use high-level ImGui::PushFont() or low-level ImDrawList::PushTextureId() to change font.

    ImVec4 clip_rect = _CmdHeader.ClipRect;
    if (cpu_fine_clip_rect)
    {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }
    font->RenderText(this, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip_rect != NULL);
}

void ImDrawList::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
    AddText(NULL, 0.0f, pos, col, text_begin, text_end);
}

void ImDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = user_texture_id != _CmdHeader.TextureId;
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimRectUV(p_min, p_max, uv_min, uv_max, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageQuad(ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1, const ImVec2& uv2, const ImVec2& uv3, const ImVec2& uv4, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = user_texture_id != _CmdHeader.TextureId;
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimQuadUV(p1, p2, p3, p4, uv1, uv2, uv3, uv4, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageRounded(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawCornerFlags rounding_corners)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (rounding <= 0.0f || (rounding_corners & ImDrawCornerFlags_All) == 0)
    {
        AddImage(user_texture_id, p_min, p_max, uv_min, uv_max, col);
        return;
    }

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    int vert_start_idx = VtxBuffer.Size;
    PathRect(p_min, p_max, rounding, rounding_corners);
    PathFillConvex(col);
    int vert_end_idx = VtxBuffer.Size;
    ImGui::ShadeVertsLinearUV(this, vert_start_idx, vert_end_idx, p_min, p_max, uv_min, uv_max, true);

    if (push_texture_id)
        PopTextureID();
}


//-----------------------------------------------------------------------------
// ImDrawListSplitter
//-----------------------------------------------------------------------------
// FIXME: This may be a little confusing, trying to be a little too low-level/optimal instead of just doing vector swap..
//-----------------------------------------------------------------------------

void ImDrawListSplitter::ClearFreeMemory()
{
    for (int i = 0; i < _Channels.Size; i++)
    {
        if (i == _Current)
            memset(&_Channels[i], 0, sizeof(_Channels[i]));  // Current channel is a copy of CmdBuffer/IdxBuffer, don't destruct again
        _Channels[i]._CmdBuffer.clear();
        _Channels[i]._IdxBuffer.clear();
    }
    _Current = 0;
    _Count = 1;
    _Channels.clear();
}

void ImDrawListSplitter::Split(ImDrawList* draw_list, int channels_count)
{
    IM_ASSERT(_Current == 0 && _Count <= 1 && "Nested channel splitting is not supported. Please use separate instances of ImDrawListSplitter.");
    int old_channels_count = _Channels.Size;
    if (old_channels_count < channels_count)
        _Channels.resize(channels_count);
    _Count = channels_count;

    // Channels[] (24/32 bytes each) hold storage that we'll swap with draw_list->_CmdBuffer/_IdxBuffer
    // The content of Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
    // When we switch to the next channel, we'll copy draw_list->_CmdBuffer/_IdxBuffer into Channels[0] and then Channels[1] into draw_list->CmdBuffer/_IdxBuffer
    memset(&_Channels[0], 0, sizeof(ImDrawChannel));
    for (int i = 1; i < channels_count; i++)
    {
        if (i >= old_channels_count)
        {
            IM_PLACEMENT_NEW(&_Channels[i]) ImDrawChannel();
        } else
        {
            _Channels[i]._CmdBuffer.resize(0);
            _Channels[i]._IdxBuffer.resize(0);
        }
        if (_Channels[i]._CmdBuffer.Size == 0)
        {
            ImDrawCmd draw_cmd;
            ImDrawCmd_HeaderCopy(&draw_cmd, &draw_list->_CmdHeader); // Copy ClipRect, TextureId, VtxOffset
            _Channels[i]._CmdBuffer.push_back(draw_cmd);
        }
    }
}

void ImDrawListSplitter::Merge(ImDrawList* draw_list)
{
    // Note that we never use or rely on _Channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    if (_Count <= 1)
        return;

    SetCurrentChannel(draw_list, 0);
    draw_list->_PopUnusedDrawCmd();

    // Calculate our final buffer sizes. Also fix the incorrect IdxOffset values in each command.
    int new_cmd_buffer_count = 0;
    int new_idx_buffer_count = 0;
    ImDrawCmd* last_cmd = (_Count > 0 && draw_list->CmdBuffer.Size > 0) ? &draw_list->CmdBuffer.back() : NULL;
    int idx_offset = last_cmd ? last_cmd->IdxOffset + last_cmd->ElemCount : 0;
    for (int i = 1; i < _Count; i++)
    {
        ImDrawChannel& ch = _Channels[i];

        // Equivalent of PopUnusedDrawCmd() for this channel's cmdbuffer and except we don't need to test for UserCallback.
        if (ch._CmdBuffer.Size > 0 && ch._CmdBuffer.back().ElemCount == 0)
            ch._CmdBuffer.pop_back();

        if (ch._CmdBuffer.Size > 0 && last_cmd != NULL)
        {
            ImDrawCmd* next_cmd = &ch._CmdBuffer[0];
            if (ImDrawCmd_HeaderCompare(last_cmd, next_cmd) == 0 && last_cmd->UserCallback == NULL && next_cmd->UserCallback == NULL)
            {
                // Merge previous channel last draw command with current channel first draw command if matching.
                last_cmd->ElemCount += next_cmd->ElemCount;
                idx_offset += next_cmd->ElemCount;
                ch._CmdBuffer.erase(ch._CmdBuffer.Data); // FIXME-OPT: Improve for multiple merges.
            }
        }
        if (ch._CmdBuffer.Size > 0)
            last_cmd = &ch._CmdBuffer.back();
        new_cmd_buffer_count += ch._CmdBuffer.Size;
        new_idx_buffer_count += ch._IdxBuffer.Size;
        for (int cmd_n = 0; cmd_n < ch._CmdBuffer.Size; cmd_n++)
        {
            ch._CmdBuffer.Data[cmd_n].IdxOffset = idx_offset;
            idx_offset += ch._CmdBuffer.Data[cmd_n].ElemCount;
        }
    }
    draw_list->CmdBuffer.resize(draw_list->CmdBuffer.Size + new_cmd_buffer_count);
    draw_list->IdxBuffer.resize(draw_list->IdxBuffer.Size + new_idx_buffer_count);

    // Write commands and indices in order (they are fairly small structures, we don't copy vertices only indices)
    ImDrawCmd* cmd_write = draw_list->CmdBuffer.Data + draw_list->CmdBuffer.Size - new_cmd_buffer_count;
    ImDrawIdx* idx_write = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size - new_idx_buffer_count;
    for (int i = 1; i < _Count; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (int sz = ch._CmdBuffer.Size) { memcpy(cmd_write, ch._CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if (int sz = ch._IdxBuffer.Size) { memcpy(idx_write, ch._IdxBuffer.Data, sz * sizeof(ImDrawIdx)); idx_write += sz; }
    }
    draw_list->_IdxWritePtr = idx_write;

    // Ensure there's always a non-callback draw command trailing the command-buffer
    if (draw_list->CmdBuffer.Size == 0 || draw_list->CmdBuffer.back().UserCallback != NULL)
        draw_list->AddDrawCmd();

    // If current command is used with different settings we need to add a new command
    ImDrawCmd* curr_cmd = &draw_list->CmdBuffer.Data[draw_list->CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount == 0)
        ImDrawCmd_HeaderCopy(curr_cmd, &draw_list->_CmdHeader); // Copy ClipRect, TextureId, VtxOffset
    else if (ImDrawCmd_HeaderCompare(curr_cmd, &draw_list->_CmdHeader) != 0)
        draw_list->AddDrawCmd();

    _Count = 1;
}

void ImDrawListSplitter::SetCurrentChannel(ImDrawList* draw_list, int idx)
{
    IM_ASSERT(idx >= 0 && idx < _Count);
    if (_Current == idx)
        return;

    // Overwrite ImVector (12/16 bytes), four times. This is merely a silly optimization instead of doing .swap()
    memcpy(&_Channels.Data[_Current]._CmdBuffer, &draw_list->CmdBuffer, sizeof(draw_list->CmdBuffer));
    memcpy(&_Channels.Data[_Current]._IdxBuffer, &draw_list->IdxBuffer, sizeof(draw_list->IdxBuffer));
    _Current = idx;
    memcpy(&draw_list->CmdBuffer, &_Channels.Data[idx]._CmdBuffer, sizeof(draw_list->CmdBuffer));
    memcpy(&draw_list->IdxBuffer, &_Channels.Data[idx]._IdxBuffer, sizeof(draw_list->IdxBuffer));
    draw_list->_IdxWritePtr = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size;

    // If current command is used with different settings we need to add a new command
    ImDrawCmd* curr_cmd = &draw_list->CmdBuffer.Data[draw_list->CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount == 0)
        ImDrawCmd_HeaderCopy(curr_cmd, &draw_list->_CmdHeader); // Copy ClipRect, TextureId, VtxOffset
    else if (ImDrawCmd_HeaderCompare(curr_cmd, &draw_list->_CmdHeader) != 0)
        draw_list->AddDrawCmd();
}

//-----------------------------------------------------------------------------
// [SECTION] ImDrawData
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

// Helper to scale the ClipRect field of each ImDrawCmd.
// Use if your final output buffer is at a different scale than draw_data->DisplaySize,
// or if there is a difference between your window resolution and framebuffer resolution.
void ImDrawData::ScaleClipRects(const ImVec2& fb_scale)
{
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            cmd->ClipRect = ImVec4(cmd->ClipRect.x * fb_scale.x, cmd->ClipRect.y * fb_scale.y, cmd->ClipRect.z * fb_scale.x, cmd->ClipRect.w * fb_scale.y);
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Helpers ShadeVertsXXX functions
//-----------------------------------------------------------------------------

// Generic linear color gradient, write to RGB fields, leave A untouched.
void ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
{
    ImVec2 gradient_extent = gradient_p1 - gradient_p0;
    float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
    {
        float d = ImDot(vert->pos - gradient_p0, gradient_extent);
        float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
        int r = ImLerp((int)(col0 >> IM_COL32_R_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_R_SHIFT) & 0xFF, t);
        int g = ImLerp((int)(col0 >> IM_COL32_G_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_G_SHIFT) & 0xFF, t);
        int b = ImLerp((int)(col0 >> IM_COL32_B_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_B_SHIFT) & 0xFF, t);
        vert->col = (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (vert->col & IM_COL32_A_MASK);
    }
}

// Distribute UV over (a, b) rectangle
void ImGui::ShadeVertsLinearUV(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, bool clamp)
{
    const ImVec2 size = b - a;
    const ImVec2 uv_size = uv_b - uv_a;
    const ImVec2 scale = ImVec2(
        size.x != 0.0f ? (uv_size.x / size.x) : 0.0f,
        size.y != 0.0f ? (uv_size.y / size.y) : 0.0f);

    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    if (clamp)
    {
        const ImVec2 min = ImMin(uv_a, uv_b);
        const ImVec2 max = ImMax(uv_a, uv_b);
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = ImClamp(uv_a + ImMul(ImVec2(vertex->pos.x, vertex->pos.y) - a, scale), min, max);
    } else
    {
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = uv_a + ImMul(ImVec2(vertex->pos.x, vertex->pos.y) - a, scale);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontConfig
//-----------------------------------------------------------------------------

ImFontConfig::ImFontConfig()
{
    FontData = NULL;
    FontDataSize = 0;
    FontDataOwnedByAtlas = true;
    FontNo = 0;
    SizePixels = 0.0f;
    OversampleH = 3; // FIXME: 2 may be a better default?
    OversampleV = 1;
    PixelSnapH = false;
    GlyphExtraSpacing = ImVec2(0.0f, 0.0f);
    GlyphOffset = ImVec2(0.0f, 0.0f);
    GlyphRanges = NULL;
    GlyphMinAdvanceX = 0.0f;
    GlyphMaxAdvanceX = FLT_MAX;
    MergeMode = false;
    RasterizerFlags = 0x00;
    RasterizerMultiply = 1.0f;
    EllipsisChar = (ImWchar)-1;
    memset(Name, 0, sizeof(Name));
    DstFont = NULL;
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas
//-----------------------------------------------------------------------------

// A work of art lies ahead! (. = white layer, X = black layer, others are blank)
// The white texels on the top left are the ones we'll use everywhere in Dear ImGui to render filled shapes.
const int FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF = 108;
const int FONT_ATLAS_DEFAULT_TEX_DATA_H = 27;
static const char FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * FONT_ATLAS_DEFAULT_TEX_DATA_H + 1] =
{
    "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX-     XX          "
    "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X-    X..X         "
    "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X-    X..X         "
    "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X-    X..X         "
    "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X-    X..X         "
    "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X-    X..XXX       "
    "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX-    X..X..XXX    "
    "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      -    X..X..X..XX  "
    "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       -    X..X..X..X.X "
    "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        -XXX X..X..X..X..X"
    "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         -X..XX........X..X"
    "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          -X...X...........X"
    "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           - X..............X"
    "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            -  X.............X"
    "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           -  X.............X"
    "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          -   X............X"
    "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          -   X...........X "
    "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       -------------------------------------    X..........X "
    "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           -    X..........X "
    "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           -     X........X  "
    "      X..X          -  X...X  -         X...X         -  X..X           X..X  -           -     X........X  "
    "       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           -     XXXXXXXXXX  "
    "------------        -    X    -           X           -X.....................X-           ------------------"
    "                    ----------------------------------- X...XXXXXXXXXXXXX...X -                             "
    "                                                      -  X..X           X..X  -                             "
    "                                                      -   X.X           X.X   -                             "
    "                                                      -    XX           XX    -                             "
};

static const ImVec2 FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[ImGuiMouseCursor_COUNT][3] =
{
    // Pos ........ Size ......... Offset ......
    { ImVec2(0,3), ImVec2(12,19), ImVec2(0, 0) }, // ImGuiMouseCursor_Arrow
    { ImVec2(13,0), ImVec2(7,16), ImVec2(1, 8) }, // ImGuiMouseCursor_TextInput
    { ImVec2(31,0), ImVec2(23,23), ImVec2(11,11) }, // ImGuiMouseCursor_ResizeAll
    { ImVec2(21,0), ImVec2(9,23), ImVec2(4,11) }, // ImGuiMouseCursor_ResizeNS
    { ImVec2(55,18),ImVec2(23, 9), ImVec2(11, 4) }, // ImGuiMouseCursor_ResizeEW
    { ImVec2(73,0), ImVec2(17,17), ImVec2(8, 8) }, // ImGuiMouseCursor_ResizeNESW
    { ImVec2(55,0), ImVec2(17,17), ImVec2(8, 8) }, // ImGuiMouseCursor_ResizeNWSE
    { ImVec2(91,0), ImVec2(17,22), ImVec2(5, 0) }, // ImGuiMouseCursor_Hand
};

ImFontAtlas::ImFontAtlas()
{
    Locked = false;
    Flags = ImFontAtlasFlags_None;
    TexID = (ImTextureID)NULL;
    TexDesiredWidth = 0;
    TexGlyphPadding = 1;

    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
    TexWidth = TexHeight = 0;
    TexUvScale = ImVec2(0.0f, 0.0f);
    TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    for (int n = 0; n < IM_ARRAYSIZE(CustomRectIds); n++)
        CustomRectIds[n] = -1;
}

ImFontAtlas::~ImFontAtlas()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    Clear();
}

void    ImFontAtlas::ClearInputData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < ConfigData.Size; i++)
        if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas)
        {
            IM_FREE(ConfigData[i].FontData);
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
    CustomRects.clear();
    for (int n = 0; n < IM_ARRAYSIZE(CustomRectIds); n++)
        CustomRectIds[n] = -1;
}

void    ImFontAtlas::ClearTexData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    if (TexPixelsAlpha8)
        IM_FREE(TexPixelsAlpha8);
    if (TexPixelsRGBA32)
        IM_FREE(TexPixelsRGBA32);
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
}

void    ImFontAtlas::ClearFonts()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < Fonts.Size; i++)
        IM_DELETE(Fonts[i]);
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
    // Build atlas on demand
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
    // Convert to RGBA32 format on demand
    // Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8 bpp
    if (!TexPixelsRGBA32)
    {
        unsigned char* pixels = NULL;
        GetTexDataAsAlpha8(&pixels, NULL, NULL);
        if (pixels)
        {
            TexPixelsRGBA32 = (unsigned int*)IM_ALLOC((size_t)TexWidth * (size_t)TexHeight * 4);
            const unsigned char* src = pixels;
            unsigned int* dst = TexPixelsRGBA32;
            for (int n = TexWidth * TexHeight; n > 0; n--)
                *dst++ = IM_COL32(255, 255, 255, (unsigned int)(*src++));
        }
    }

    *out_pixels = (unsigned char*)TexPixelsRGBA32;
    if (out_width) *out_width = TexWidth;
    if (out_height) *out_height = TexHeight;
    if (out_bytes_per_pixel) *out_bytes_per_pixel = 4;
}

ImFont* ImFontAtlas::AddFont(const ImFontConfig* font_cfg)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
    IM_ASSERT(font_cfg->SizePixels > 0.0f);

    // Create new font
    if (!font_cfg->MergeMode)
        Fonts.push_back(IM_NEW(ImFont));
    else
        IM_ASSERT(!Fonts.empty() && "Cannot use MergeMode for the first font"); // When using MergeMode make sure that a font has already been added before. You can use ImGui::GetIO().Fonts->AddFontDefault() to add the default imgui font.

    ConfigData.push_back(*font_cfg);
    ImFontConfig& new_font_cfg = ConfigData.back();
    if (new_font_cfg.DstFont == NULL)
        new_font_cfg.DstFont = Fonts.back();
    if (!new_font_cfg.FontDataOwnedByAtlas)
    {
        new_font_cfg.FontData = IM_ALLOC(new_font_cfg.FontDataSize);
        new_font_cfg.FontDataOwnedByAtlas = true;
        memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
    }

    if (new_font_cfg.DstFont->EllipsisChar == (ImWchar)-1)
        new_font_cfg.DstFont->EllipsisChar = font_cfg->EllipsisChar;

    // Invalidate texture
    ClearTexData();
    return new_font_cfg.DstFont;
}

// Default font TTF is compressed with stb_compress then base85 encoded (see misc/fonts/binary_to_compressed_c.cpp for encoder)
static unsigned int stb_decompress_length(const unsigned char* input);
static unsigned int stb_decompress(unsigned char* output, const unsigned char* input, unsigned int length);
static const char* GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c) { return c >= '\\' ? c - 36 : c - 35; }
static void         Decode85(const unsigned char* src, unsigned char* dst)
{
    while (*src)
    {
        unsigned int tmp = Decode85Byte(src[0]) + 85 * (Decode85Byte(src[1]) + 85 * (Decode85Byte(src[2]) + 85 * (Decode85Byte(src[3]) + 85 * Decode85Byte(src[4]))));
        dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianness.
        src += 5;
        dst += 4;
    }
}

// Load embedded ProggyClean.ttf at size 13, disable oversampling
ImFont* ImFontAtlas::AddFontDefault(const ImFontConfig* font_cfg_template)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (!font_cfg_template)
    {
        font_cfg.OversampleH = font_cfg.OversampleV = 1;
        font_cfg.PixelSnapH = true;
    }
    if (font_cfg.SizePixels <= 0.0f)
        font_cfg.SizePixels = 13.0f * 1.0f;
    if (font_cfg.Name[0] == '\0')
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "ProggyClean.ttf, %dpx", (int)font_cfg.SizePixels);
    font_cfg.EllipsisChar = (ImWchar)0x0085;

    const char* ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
    const ImWchar* glyph_ranges = font_cfg.GlyphRanges != NULL ? font_cfg.GlyphRanges : GetGlyphRangesDefault();
    ImFont* font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85, font_cfg.SizePixels, &font_cfg, glyph_ranges);
    font->DisplayOffset.y = 1.0f;
    return font;
}

ImFont* ImFontAtlas::AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    size_t data_size = 0;
    void* data = ImFileLoadToMemory(filename, "rb", &data_size, 0);
    if (!data)
    {
        IM_ASSERT_USER_ERROR(0, "Could not load font file!");
        return NULL;
    }
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (font_cfg.Name[0] == '\0')
    {
        // Store a short copy of filename into into the font name for convenience
        const char* p;
        for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {}
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s, %.0fpx", p, size_pixels);
    }
    return AddFontFromMemoryTTF(data, (int)data_size, size_pixels, &font_cfg, glyph_ranges);
}

// NB: Transfer ownership of 'ttf_data' to ImFontAtlas, unless font_cfg_template->FontDataOwnedByAtlas == false. Owned TTF buffer will be deleted after Build().
ImFont* ImFontAtlas::AddFontFromMemoryTTF(void* ttf_data, int ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
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
    const unsigned int buf_decompressed_size = stb_decompress_length((const unsigned char*)compressed_ttf_data);
    unsigned char* buf_decompressed_data = (unsigned char*)IM_ALLOC(buf_decompressed_size);
    stb_decompress(buf_decompressed_data, (const unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontDataOwnedByAtlas = true;
    return AddFontFromMemoryTTF(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, &font_cfg, glyph_ranges);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressed_ttf_data_base85, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges)
{
    int compressed_ttf_size = (((int)strlen(compressed_ttf_data_base85) + 4) / 5) * 4;
    void* compressed_ttf = IM_ALLOC((size_t)compressed_ttf_size);
    Decode85((const unsigned char*)compressed_ttf_data_base85, (unsigned char*)compressed_ttf);
    ImFont* font = AddFontFromMemoryCompressedTTF(compressed_ttf, compressed_ttf_size, size_pixels, font_cfg, glyph_ranges);
    IM_FREE(compressed_ttf);
    return font;
}

int ImFontAtlas::AddCustomRectRegular(int width, int height)
{
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    ImFontAtlasCustomRect r;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

int ImFontAtlas::AddCustomRectFontGlyph(ImFont* font, ImWchar id, int width, int height, float advance_x, const ImVec2& offset)
{
#ifdef IMGUI_USE_WCHAR32
    IM_ASSERT(id <= IM_UNICODE_CODEPOINT_MAX);
#endif
    IM_ASSERT(font != NULL);
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    ImFontAtlasCustomRect r;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    r.GlyphID = id;
    r.GlyphAdvanceX = advance_x;
    r.GlyphOffset = offset;
    r.Font = font;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

void ImFontAtlas::CalcCustomRectUV(const ImFontAtlasCustomRect* rect, ImVec2* out_uv_min, ImVec2* out_uv_max) const
{
    IM_ASSERT(TexWidth > 0 && TexHeight > 0);   // Font atlas needs to be built before we can calculate UV coordinates
    IM_ASSERT(rect->IsPacked());                // Make sure the rectangle has been packed
    *out_uv_min = ImVec2((float)rect->X * TexUvScale.x, (float)rect->Y * TexUvScale.y);
    *out_uv_max = ImVec2((float)(rect->X + rect->Width) * TexUvScale.x, (float)(rect->Y + rect->Height) * TexUvScale.y);
}

bool ImFontAtlas::GetMouseCursorTexData(ImGuiMouseCursor cursor_type, ImVec2* out_offset, ImVec2* out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2])
{
    if (cursor_type <= ImGuiMouseCursor_None || cursor_type >= ImGuiMouseCursor_COUNT)
        return false;
    if (Flags & ImFontAtlasFlags_NoMouseCursors)
        return false;

    IM_ASSERT(CustomRectIds[0] != -1);
    ImFontAtlasCustomRect& r = CustomRects[CustomRectIds[0]];
    ImVec2 pos = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][0] + ImVec2((float)r.X, (float)r.Y);
    ImVec2 size = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][1];
    *out_size = size;
    *out_offset = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][2];
    out_uv_border[0] = (pos)*TexUvScale;
    out_uv_border[1] = (pos + size) * TexUvScale;
    pos.x += FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
    out_uv_fill[0] = (pos)*TexUvScale;
    out_uv_fill[1] = (pos + size) * TexUvScale;
    return true;
}

bool    ImFontAtlas::Build()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    return ImFontAtlasBuildWithStbTruetype(this);
}

void    ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_brighten_factor)
{
    for (unsigned int i = 0; i < 256; i++)
    {
        unsigned int value = (unsigned int)(i * in_brighten_factor);
        out_table[i] = value > 255 ? 255 : (value & 0xFF);
    }
}

void    ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride)
{
    unsigned char* data = pixels + x + y * stride;
    for (int j = h; j > 0; j--, data += stride)
        for (int i = 0; i < w; i++)
            data[i] = table[data[i]];
}

// Temporary data for one source font (multiple source fonts can be merged into one destination ImFont)
// (C++03 doesn't allow instancing ImVector<> with function-local types so we declare the type here.)
struct ImFontBuildSrcData
{
    stbtt_fontinfo      FontInfo;
    stbtt_pack_range    PackRange;          // Hold the list of codepoints to pack (essentially points to Codepoints.Data)
    stbrp_rect* Rects;              // Rectangle to pack. We first fill in their size and the packer will give us their position.
    stbtt_packedchar* PackedChars;        // Output glyphs
    const ImWchar* SrcRanges;          // Ranges as requested by user (user is allowed to request too much, e.g. 0x0020..0xFFFF)
    int                 DstIndex;           // Index into atlas->Fonts[] and dst_tmp_array[]
    int                 GlyphsHighest;      // Highest requested codepoint
    int                 GlyphsCount;        // Glyph count (excluding missing glyphs and glyphs already set by an earlier source font)
    ImBitVector         GlyphsSet;          // Glyph bit map (random access, 1-bit per codepoint. This will be a maximum of 8KB)
    ImVector<int>       GlyphsList;         // Glyph codepoints list (flattened version of GlyphsMap)
};

// Temporary data for one destination ImFont* (multiple source fonts can be merged into one destination ImFont)
struct ImFontBuildDstData
{
    int                 SrcCount;           // Number of source fonts targeting this destination font.
    int                 GlyphsHighest;
    int                 GlyphsCount;
    ImBitVector         GlyphsSet;          // This is used to resolve collision when multiple sources are merged into a same destination font.
};

static void UnpackBitVectorToFlatIndexList(const ImBitVector* in, ImVector<int>* out)
{
    IM_ASSERT(sizeof(in->Storage.Data[0]) == sizeof(int));
    const ImU32* it_begin = in->Storage.begin();
    const ImU32* it_end = in->Storage.end();
    for (const ImU32* it = it_begin; it < it_end; it++)
        if (ImU32 entries_32 = *it)
            for (ImU32 bit_n = 0; bit_n < 32; bit_n++)
                if (entries_32 & ((ImU32)1 << bit_n))
                    out->push_back((int)(((it - it_begin) << 5) + bit_n));
}

bool    ImFontAtlasBuildWithStbTruetype(ImFontAtlas* atlas)
{
    IM_ASSERT(atlas->ConfigData.Size > 0);

    ImFontAtlasBuildInit(atlas);

    // Clear atlas
    atlas->TexID = (ImTextureID)NULL;
    atlas->TexWidth = atlas->TexHeight = 0;
    atlas->TexUvScale = ImVec2(0.0f, 0.0f);
    atlas->TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    atlas->ClearTexData();

    // Temporary storage for building
    ImVector<ImFontBuildSrcData> src_tmp_array;
    ImVector<ImFontBuildDstData> dst_tmp_array;
    src_tmp_array.resize(atlas->ConfigData.Size);
    dst_tmp_array.resize(atlas->Fonts.Size);
    memset(src_tmp_array.Data, 0, (size_t)src_tmp_array.size_in_bytes());
    memset(dst_tmp_array.Data, 0, (size_t)dst_tmp_array.size_in_bytes());

    // 1. Initialize font loading structure, check font data validity
    for (int src_i = 0; src_i < atlas->ConfigData.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == atlas));

        // Find index from cfg.DstFont (we allow the user to set cfg.DstFont. Also it makes casual debugging nicer than when storing indices)
        src_tmp.DstIndex = -1;
        for (int output_i = 0; output_i < atlas->Fonts.Size && src_tmp.DstIndex == -1; output_i++)
            if (cfg.DstFont == atlas->Fonts[output_i])
                src_tmp.DstIndex = output_i;
        IM_ASSERT(src_tmp.DstIndex != -1); // cfg.DstFont not pointing within atlas->Fonts[] array?
        if (src_tmp.DstIndex == -1)
            return false;

        // Initialize helper structure for font loading and verify that the TTF/OTF data is correct
        const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData, cfg.FontNo);
        IM_ASSERT(font_offset >= 0 && "FontData is incorrect, or FontNo cannot be found.");
        if (!stbtt_InitFont(&src_tmp.FontInfo, (unsigned char*)cfg.FontData, font_offset))
            return false;

        // Measure highest codepoints
        ImFontBuildDstData& dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.SrcRanges = cfg.GlyphRanges ? cfg.GlyphRanges : atlas->GetGlyphRangesDefault();
        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            src_tmp.GlyphsHighest = ImMax(src_tmp.GlyphsHighest, (int)src_range[1]);
        dst_tmp.SrcCount++;
        dst_tmp.GlyphsHighest = ImMax(dst_tmp.GlyphsHighest, src_tmp.GlyphsHighest);
    }

    // 2. For every requested codepoint, check for their presence in the font data, and handle redundancy or overlaps between source fonts to avoid unused glyphs.
    int total_glyphs_count = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontBuildDstData& dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.GlyphsSet.Create(src_tmp.GlyphsHighest + 1);
        if (dst_tmp.GlyphsSet.Storage.empty())
            dst_tmp.GlyphsSet.Create(dst_tmp.GlyphsHighest + 1);

        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            for (unsigned int codepoint = src_range[0]; codepoint <= src_range[1]; codepoint++)
            {
                if (dst_tmp.GlyphsSet.TestBit(codepoint))    // Don't overwrite existing glyphs. We could make this an option for MergeMode (e.g. MergeOverwrite==true)
                    continue;
                if (!stbtt_FindGlyphIndex(&src_tmp.FontInfo, codepoint))    // It is actually in the font?
                    continue;

                // Add to avail set/counters
                src_tmp.GlyphsCount++;
                dst_tmp.GlyphsCount++;
                src_tmp.GlyphsSet.SetBit(codepoint);
                dst_tmp.GlyphsSet.SetBit(codepoint);
                total_glyphs_count++;
            }
    }

    // 3. Unpack our bit map into a flat list (we now have all the Unicode points that we know are requested _and_ available _and_ not overlapping another)
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        src_tmp.GlyphsList.reserve(src_tmp.GlyphsCount);
        UnpackBitVectorToFlatIndexList(&src_tmp.GlyphsSet, &src_tmp.GlyphsList);
        src_tmp.GlyphsSet.Clear();
        IM_ASSERT(src_tmp.GlyphsList.Size == src_tmp.GlyphsCount);
    }
    for (int dst_i = 0; dst_i < dst_tmp_array.Size; dst_i++)
        dst_tmp_array[dst_i].GlyphsSet.Clear();
    dst_tmp_array.clear();

    // Allocate packing character data and flag packed characters buffer as non-packed (x0=y0=x1=y1=0)
    // (We technically don't need to zero-clear buf_rects, but let's do it for the sake of sanity)
    ImVector<stbrp_rect> buf_rects;
    ImVector<stbtt_packedchar> buf_packedchars;
    buf_rects.resize(total_glyphs_count);
    buf_packedchars.resize(total_glyphs_count);
    memset(buf_rects.Data, 0, (size_t)buf_rects.size_in_bytes());
    memset(buf_packedchars.Data, 0, (size_t)buf_packedchars.size_in_bytes());

    // 4. Gather glyphs sizes so we can pack them in our virtual canvas.
    int total_surface = 0;
    int buf_rects_out_n = 0;
    int buf_packedchars_out_n = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        src_tmp.Rects = &buf_rects[buf_rects_out_n];
        src_tmp.PackedChars = &buf_packedchars[buf_packedchars_out_n];
        buf_rects_out_n += src_tmp.GlyphsCount;
        buf_packedchars_out_n += src_tmp.GlyphsCount;

        // Convert our ranges in the format stb_truetype wants
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        src_tmp.PackRange.font_size = cfg.SizePixels;
        src_tmp.PackRange.first_unicode_codepoint_in_range = 0;
        src_tmp.PackRange.array_of_unicode_codepoints = src_tmp.GlyphsList.Data;
        src_tmp.PackRange.num_chars = src_tmp.GlyphsList.Size;
        src_tmp.PackRange.chardata_for_range = src_tmp.PackedChars;
        src_tmp.PackRange.h_oversample = (unsigned char)cfg.OversampleH;
        src_tmp.PackRange.v_oversample = (unsigned char)cfg.OversampleV;

        // Gather the sizes of all rectangles we will need to pack (this loop is based on stbtt_PackFontRangesGatherRects)
        const float scale = (cfg.SizePixels > 0) ? stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels) : stbtt_ScaleForMappingEmToPixels(&src_tmp.FontInfo, -cfg.SizePixels);
        const int padding = atlas->TexGlyphPadding;
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsList.Size; glyph_i++)
        {
            int x0, y0, x1, y1;
            const int glyph_index_in_font = stbtt_FindGlyphIndex(&src_tmp.FontInfo, src_tmp.GlyphsList[glyph_i]);
            IM_ASSERT(glyph_index_in_font != 0);
            stbtt_GetGlyphBitmapBoxSubpixel(&src_tmp.FontInfo, glyph_index_in_font, scale * cfg.OversampleH, scale * cfg.OversampleV, 0, 0, &x0, &y0, &x1, &y1);
            src_tmp.Rects[glyph_i].w = (stbrp_coord)(x1 - x0 + padding + cfg.OversampleH - 1);
            src_tmp.Rects[glyph_i].h = (stbrp_coord)(y1 - y0 + padding + cfg.OversampleV - 1);
            total_surface += src_tmp.Rects[glyph_i].w * src_tmp.Rects[glyph_i].h;
        }
    }

    // We need a width for the skyline algorithm, any width!
    // The exact width doesn't really matter much, but some API/GPU have texture size limitations and increasing width can decrease height.
    // User can override TexDesiredWidth and TexGlyphPadding if they wish, otherwise we use a simple heuristic to select the width based on expected surface.
    const int surface_sqrt = (int)ImSqrt((float)total_surface) + 1;
    atlas->TexHeight = 0;
    if (atlas->TexDesiredWidth > 0)
        atlas->TexWidth = atlas->TexDesiredWidth;
    else
        atlas->TexWidth = (surface_sqrt >= 4096 * 0.7f) ? 4096 : (surface_sqrt >= 2048 * 0.7f) ? 2048 : (surface_sqrt >= 1024 * 0.7f) ? 1024 : 512;

    // 5. Start packing
    // Pack our extra data rectangles first, so it will be on the upper-left corner of our texture (UV will have small values).
    const int TEX_HEIGHT_MAX = 1024 * 32;
    stbtt_pack_context spc = {};
    stbtt_PackBegin(&spc, NULL, atlas->TexWidth, TEX_HEIGHT_MAX, 0, atlas->TexGlyphPadding, NULL);
    ImFontAtlasBuildPackCustomRects(atlas, spc.pack_info);

    // 6. Pack each source font. No rendering yet, we are working with rectangles in an infinitely tall texture at this point.
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbrp_pack_rects((stbrp_context*)spc.pack_info, src_tmp.Rects, src_tmp.GlyphsCount);

        // Extend texture height and mark missing glyphs as non-packed so we won't render them.
        // FIXME: We are not handling packing failure here (would happen if we got off TEX_HEIGHT_MAX or if a single if larger than TexWidth?)
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
            if (src_tmp.Rects[glyph_i].was_packed)
                atlas->TexHeight = ImMax(atlas->TexHeight, src_tmp.Rects[glyph_i].y + src_tmp.Rects[glyph_i].h);
    }

    // 7. Allocate texture
    atlas->TexHeight = (atlas->Flags & ImFontAtlasFlags_NoPowerOfTwoHeight) ? (atlas->TexHeight + 1) : ImUpperPowerOfTwo(atlas->TexHeight);
    atlas->TexUvScale = ImVec2(1.0f / atlas->TexWidth, 1.0f / atlas->TexHeight);
    atlas->TexPixelsAlpha8 = (unsigned char*)IM_ALLOC(atlas->TexWidth * atlas->TexHeight);
    memset(atlas->TexPixelsAlpha8, 0, atlas->TexWidth * atlas->TexHeight);
    spc.pixels = atlas->TexPixelsAlpha8;
    spc.height = atlas->TexHeight;

    // 8. Render/rasterize font characters into the texture
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbtt_PackFontRangesRenderIntoRects(&spc, &src_tmp.FontInfo, &src_tmp.PackRange, 1, src_tmp.Rects);

        // Apply multiply operator
        if (cfg.RasterizerMultiply != 1.0f)
        {
            unsigned char multiply_table[256];
            ImFontAtlasBuildMultiplyCalcLookupTable(multiply_table, cfg.RasterizerMultiply);
            stbrp_rect* r = &src_tmp.Rects[0];
            for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++, r++)
                if (r->was_packed)
                    ImFontAtlasBuildMultiplyRectAlpha8(multiply_table, atlas->TexPixelsAlpha8, r->x, r->y, r->w, r->h, atlas->TexWidth * 1);
        }
        src_tmp.Rects = NULL;
    }

    // End packing
    stbtt_PackEnd(&spc);
    buf_rects.clear();

    // 9. Setup ImFont and glyphs for runtime
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        ImFontConfig& cfg = atlas->ConfigData[src_i];
        ImFont* dst_font = cfg.DstFont; // We can have multiple input fonts writing into a same destination font (when using MergeMode=true)

        const float font_scale = stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels);
        int unscaled_ascent, unscaled_descent, unscaled_line_gap;
        stbtt_GetFontVMetrics(&src_tmp.FontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

        const float ascent = ImFloor(unscaled_ascent * font_scale + ((unscaled_ascent > 0.0f) ? +1 : -1));
        const float descent = ImFloor(unscaled_descent * font_scale + ((unscaled_descent > 0.0f) ? +1 : -1));
        ImFontAtlasBuildSetupFont(atlas, dst_font, &cfg, ascent, descent);
        const float font_off_x = cfg.GlyphOffset.x;
        const float font_off_y = cfg.GlyphOffset.y + IM_ROUND(dst_font->Ascent);

        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
        {
            const int codepoint = src_tmp.GlyphsList[glyph_i];
            const stbtt_packedchar& pc = src_tmp.PackedChars[glyph_i];

            const float char_advance_x_org = pc.xadvance;
            const float char_advance_x_mod = ImClamp(char_advance_x_org, cfg.GlyphMinAdvanceX, cfg.GlyphMaxAdvanceX);
            float char_off_x = font_off_x;
            if (char_advance_x_org != char_advance_x_mod)
                char_off_x += cfg.PixelSnapH ? ImFloor((char_advance_x_mod - char_advance_x_org) * 0.5f) : (char_advance_x_mod - char_advance_x_org) * 0.5f;

            // Register glyph
            stbtt_aligned_quad q;
            float dummy_x = 0.0f, dummy_y = 0.0f;
            stbtt_GetPackedQuad(src_tmp.PackedChars, atlas->TexWidth, atlas->TexHeight, glyph_i, &dummy_x, &dummy_y, &q, 0);
            dst_font->AddGlyph((ImWchar)codepoint, q.x0 + char_off_x, q.y0 + font_off_y, q.x1 + char_off_x, q.y1 + font_off_y, q.s0, q.t0, q.s1, q.t1, char_advance_x_mod);
        }
    }

    // Cleanup temporary (ImVector doesn't honor destructor)
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
        src_tmp_array[src_i].~ImFontBuildSrcData();

    ImFontAtlasBuildFinish(atlas);
    return true;
}

// Register default custom rectangles (this is called/shared by both the stb_truetype and the FreeType builder)
void ImFontAtlasBuildInit(ImFontAtlas* atlas)
{
    if (atlas->CustomRectIds[0] >= 0)
        return;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
        atlas->CustomRectIds[0] = atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * 2 + 1, FONT_ATLAS_DEFAULT_TEX_DATA_H);
    else
        atlas->CustomRectIds[0] = atlas->AddCustomRectRegular(2, 2);
}

void ImFontAtlasBuildSetupFont(ImFontAtlas* atlas, ImFont* font, ImFontConfig* font_config, float ascent, float descent)
{
    if (!font_config->MergeMode)
    {
        font->ClearOutputData();
        font->FontSize = font_config->SizePixels;
        font->ConfigData = font_config;
        font->ContainerAtlas = atlas;
        font->Ascent = ascent;
        font->Descent = descent;
    }
    font->ConfigDataCount++;
}

void ImFontAtlasBuildPackCustomRects(ImFontAtlas* atlas, void* stbrp_context_opaque)
{
    stbrp_context* pack_context = (stbrp_context*)stbrp_context_opaque;
    IM_ASSERT(pack_context != NULL);

    ImVector<ImFontAtlasCustomRect>& user_rects = atlas->CustomRects;
    IM_ASSERT(user_rects.Size >= 1); // We expect at least the default custom rects to be registered, else something went wrong.

    ImVector<stbrp_rect> pack_rects;
    pack_rects.resize(user_rects.Size);
    memset(pack_rects.Data, 0, (size_t)pack_rects.size_in_bytes());
    for (int i = 0; i < user_rects.Size; i++)
    {
        pack_rects[i].w = user_rects[i].Width;
        pack_rects[i].h = user_rects[i].Height;
    }
    stbrp_pack_rects(pack_context, &pack_rects[0], pack_rects.Size);
    for (int i = 0; i < pack_rects.Size; i++)
        if (pack_rects[i].was_packed)
        {
            user_rects[i].X = pack_rects[i].x;
            user_rects[i].Y = pack_rects[i].y;
            IM_ASSERT(pack_rects[i].w == user_rects[i].Width && pack_rects[i].h == user_rects[i].Height);
            atlas->TexHeight = ImMax(atlas->TexHeight, pack_rects[i].y + pack_rects[i].h);
        }
}

static void ImFontAtlasBuildRenderDefaultTexData(ImFontAtlas* atlas)
{
    IM_ASSERT(atlas->CustomRectIds[0] >= 0);
    IM_ASSERT(atlas->TexPixelsAlpha8 != NULL);
    ImFontAtlasCustomRect& r = atlas->CustomRects[atlas->CustomRectIds[0]];
    IM_ASSERT(r.IsPacked());

    const int w = atlas->TexWidth;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
    {
        // Render/copy pixels
        IM_ASSERT(r.Width == FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * 2 + 1 && r.Height == FONT_ATLAS_DEFAULT_TEX_DATA_H);
        for (int y = 0, n = 0; y < FONT_ATLAS_DEFAULT_TEX_DATA_H; y++)
            for (int x = 0; x < FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF; x++, n++)
            {
                const int offset0 = (int)(r.X + x) + (int)(r.Y + y) * w;
                const int offset1 = offset0 + FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
                atlas->TexPixelsAlpha8[offset0] = FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[n] == '.' ? 0xFF : 0x00;
                atlas->TexPixelsAlpha8[offset1] = FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[n] == 'X' ? 0xFF : 0x00;
            }
    } else
    {
        IM_ASSERT(r.Width == 2 && r.Height == 2);
        const int offset = (int)(r.X) + (int)(r.Y) * w;
        atlas->TexPixelsAlpha8[offset] = atlas->TexPixelsAlpha8[offset + 1] = atlas->TexPixelsAlpha8[offset + w] = atlas->TexPixelsAlpha8[offset + w + 1] = 0xFF;
    }
    atlas->TexUvWhitePixel = ImVec2((r.X + 0.5f) * atlas->TexUvScale.x, (r.Y + 0.5f) * atlas->TexUvScale.y);
}

void ImFontAtlasBuildFinish(ImFontAtlas* atlas)
{
    // Render into our custom data block
    ImFontAtlasBuildRenderDefaultTexData(atlas);

    // Register custom rectangle glyphs
    for (int i = 0; i < atlas->CustomRects.Size; i++)
    {
        const ImFontAtlasCustomRect& r = atlas->CustomRects[i];
        if (r.Font == NULL || r.GlyphID == 0)
            continue;

        IM_ASSERT(r.Font->ContainerAtlas == atlas);
        ImVec2 uv0, uv1;
        atlas->CalcCustomRectUV(&r, &uv0, &uv1);
        r.Font->AddGlyph((ImWchar)r.GlyphID, r.GlyphOffset.x, r.GlyphOffset.y, r.GlyphOffset.x + r.Width, r.GlyphOffset.y + r.Height, uv0.x, uv0.y, uv1.x, uv1.y, r.GlyphAdvanceX);
    }

    // Build all fonts lookup tables
    for (int i = 0; i < atlas->Fonts.Size; i++)
        if (atlas->Fonts[i]->DirtyLookupTables)
            atlas->Fonts[i]->BuildLookupTable();

    // Ellipsis character is required for rendering elided text. We prefer using U+2026 (horizontal ellipsis).
    // However some old fonts may contain ellipsis at U+0085. Here we auto-detect most suitable ellipsis character.
    // FIXME: Also note that 0x2026 is currently seldom included in our font ranges. Because of this we are more likely to use three individual dots.
    for (int i = 0; i < atlas->Fonts.size(); i++)
    {
        ImFont* font = atlas->Fonts[i];
        if (font->EllipsisChar != (ImWchar)-1)
            continue;
        const ImWchar ellipsis_variants[] = { (ImWchar)0x2026, (ImWchar)0x0085 };
        for (int j = 0; j < IM_ARRAYSIZE(ellipsis_variants); j++)
            if (font->FindGlyphNoFallback(ellipsis_variants[j]) != NULL) // Verify glyph exists
            {
                font->EllipsisChar = ellipsis_variants[j];
                break;
            }
    }
}

// Retrieve list of range (2 int per range, values are inclusive)
const ImWchar* ImFontAtlas::GetGlyphRangesDefault()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesKorean()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3131, 0x3163, // Korean alphabets
        0xAC00, 0xD7A3, // Korean characters
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesChineseFull()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0x4e00, 0x9FAF, // CJK Ideograms
        0,
    };
    return &ranges[0];
}

static void UnpackAccumulativeOffsetsIntoRanges(int base_codepoint, const short* accumulative_offsets, int accumulative_offsets_count, ImWchar* out_ranges)
{
    for (int n = 0; n < accumulative_offsets_count; n++, out_ranges += 2)
    {
        out_ranges[0] = out_ranges[1] = (ImWchar)(base_codepoint + accumulative_offsets[n]);
        base_codepoint += accumulative_offsets[n];
    }
    out_ranges[0] = 0;
}

//-------------------------------------------------------------------------
// [SECTION] ImFontAtlas glyph ranges helpers
//-------------------------------------------------------------------------

const ImWchar* ImFontAtlas::GetGlyphRangesChineseSimplifiedCommon()
{
    // Store 2500 regularly used characters for Simplified Chinese.
    // Sourced from https://zh.wiktionary.org/wiki/%E9%99%84%E5%BD%95:%E7%8E%B0%E4%BB%A3%E6%B1%89%E8%AF%AD%E5%B8%B8%E7%94%A8%E5%AD%97%E8%A1%A8
    // This table covers 97.97% of all characters used during the month in July, 1987.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,3,2,1,2,2,1,1,1,1,1,5,2,1,2,3,3,3,2,2,4,1,1,1,2,1,5,2,3,1,2,1,2,1,1,2,1,1,2,2,1,4,1,1,1,1,5,10,1,2,19,2,1,2,1,2,1,2,1,2,
        1,5,1,6,3,2,1,2,2,1,1,1,4,8,5,1,1,4,1,1,3,1,2,1,5,1,2,1,1,1,10,1,1,5,2,4,6,1,4,2,2,2,12,2,1,1,6,1,1,1,4,1,1,4,6,5,1,4,2,2,4,10,7,1,1,4,2,4,
        2,1,4,3,6,10,12,5,7,2,14,2,9,1,1,6,7,10,4,7,13,1,5,4,8,4,1,1,2,28,5,6,1,1,5,2,5,20,2,2,9,8,11,2,9,17,1,8,6,8,27,4,6,9,20,11,27,6,68,2,2,1,1,
        1,2,1,2,2,7,6,11,3,3,1,1,3,1,2,1,1,1,1,1,3,1,1,8,3,4,1,5,7,2,1,4,4,8,4,2,1,2,1,1,4,5,6,3,6,2,12,3,1,3,9,2,4,3,4,1,5,3,3,1,3,7,1,5,1,1,1,1,2,
        3,4,5,2,3,2,6,1,1,2,1,7,1,7,3,4,5,15,2,2,1,5,3,22,19,2,1,1,1,1,2,5,1,1,1,6,1,1,12,8,2,9,18,22,4,1,1,5,1,16,1,2,7,10,15,1,1,6,2,4,1,2,4,1,6,
        1,1,3,2,4,1,6,4,5,1,2,1,1,2,1,10,3,1,3,2,1,9,3,2,5,7,2,19,4,3,6,1,1,1,1,1,4,3,2,1,1,1,2,5,3,1,1,1,2,2,1,1,2,1,1,2,1,3,1,1,1,3,7,1,4,1,1,2,1,
        1,2,1,2,4,4,3,8,1,1,1,2,1,3,5,1,3,1,3,4,6,2,2,14,4,6,6,11,9,1,15,3,1,28,5,2,5,5,3,1,3,4,5,4,6,14,3,2,3,5,21,2,7,20,10,1,2,19,2,4,28,28,2,3,
        2,1,14,4,1,26,28,42,12,40,3,52,79,5,14,17,3,2,2,11,3,4,6,3,1,8,2,23,4,5,8,10,4,2,7,3,5,1,1,6,3,1,2,2,2,5,28,1,1,7,7,20,5,3,29,3,17,26,1,8,4,
        27,3,6,11,23,5,3,4,6,13,24,16,6,5,10,25,35,7,3,2,3,3,14,3,6,2,6,1,4,2,3,8,2,1,1,3,3,3,4,1,1,13,2,2,4,5,2,1,14,14,1,2,2,1,4,5,2,3,1,14,3,12,
        3,17,2,16,5,1,2,1,8,9,3,19,4,2,2,4,17,25,21,20,28,75,1,10,29,103,4,1,2,1,1,4,2,4,1,2,3,24,2,2,2,1,1,2,1,3,8,1,1,1,2,1,1,3,1,1,1,6,1,5,3,1,1,
        1,3,4,1,1,5,2,1,5,6,13,9,16,1,1,1,1,3,2,3,2,4,5,2,5,2,2,3,7,13,7,2,2,1,1,1,1,2,3,3,2,1,6,4,9,2,1,14,2,14,2,1,18,3,4,14,4,11,41,15,23,15,23,
        176,1,3,4,1,1,1,1,5,3,1,2,3,7,3,1,1,2,1,2,4,4,6,2,4,1,9,7,1,10,5,8,16,29,1,1,2,2,3,1,3,5,2,4,5,4,1,1,2,2,3,3,7,1,6,10,1,17,1,44,4,6,2,1,1,6,
        5,4,2,10,1,6,9,2,8,1,24,1,2,13,7,8,8,2,1,4,1,3,1,3,3,5,2,5,10,9,4,9,12,2,1,6,1,10,1,1,7,7,4,10,8,3,1,13,4,3,1,6,1,3,5,2,1,2,17,16,5,2,16,6,
        1,4,2,1,3,3,6,8,5,11,11,1,3,3,2,4,6,10,9,5,7,4,7,4,7,1,1,4,2,1,3,6,8,7,1,6,11,5,5,3,24,9,4,2,7,13,5,1,8,82,16,61,1,1,1,4,2,2,16,10,3,8,1,1,
        6,4,2,1,3,1,1,1,4,3,8,4,2,2,1,1,1,1,1,6,3,5,1,1,4,6,9,2,1,1,1,2,1,7,2,1,6,1,5,4,4,3,1,8,1,3,3,1,3,2,2,2,2,3,1,6,1,2,1,2,1,3,7,1,8,2,1,2,1,5,
        2,5,3,5,10,1,2,1,1,3,2,5,11,3,9,3,5,1,1,5,9,1,2,1,5,7,9,9,8,1,3,3,3,6,8,2,3,2,1,1,32,6,1,2,15,9,3,7,13,1,3,10,13,2,14,1,13,10,2,1,3,10,4,15,
        2,15,15,10,1,3,9,6,9,32,25,26,47,7,3,2,3,1,6,3,4,3,2,8,5,4,1,9,4,2,2,19,10,6,2,3,8,1,2,2,4,2,1,9,4,4,4,6,4,8,9,2,3,1,1,1,1,3,5,5,1,3,8,4,6,
        2,1,4,12,1,5,3,7,13,2,5,8,1,6,1,2,5,14,6,1,5,2,4,8,15,5,1,23,6,62,2,10,1,1,8,1,2,2,10,4,2,2,9,2,1,1,3,2,3,1,5,3,3,2,1,3,8,1,1,1,11,3,1,1,4,
        3,7,1,14,1,2,3,12,5,2,5,1,6,7,5,7,14,11,1,3,1,8,9,12,2,1,11,8,4,4,2,6,10,9,13,1,1,3,1,5,1,3,2,4,4,1,18,2,3,14,11,4,29,4,2,7,1,3,13,9,2,2,5,
        3,5,20,7,16,8,5,72,34,6,4,22,12,12,28,45,36,9,7,39,9,191,1,1,1,4,11,8,4,9,2,3,22,1,1,1,1,4,17,1,7,7,1,11,31,10,2,4,8,2,3,2,1,4,2,16,4,32,2,
        3,19,13,4,9,1,5,2,14,8,1,1,3,6,19,6,5,1,16,6,2,10,8,5,1,2,3,1,5,5,1,11,6,6,1,3,3,2,6,3,8,1,1,4,10,7,5,7,7,5,8,9,2,1,3,4,1,1,3,1,3,3,2,6,16,
        1,4,6,3,1,10,6,1,3,15,2,9,2,10,25,13,9,16,6,2,2,10,11,4,3,9,1,2,6,6,5,4,30,40,1,10,7,12,14,33,6,3,6,7,3,1,3,1,11,14,4,9,5,12,11,49,18,51,31,
        140,31,2,2,1,5,1,8,1,10,1,4,4,3,24,1,10,1,3,6,6,16,3,4,5,2,1,4,2,57,10,6,22,2,22,3,7,22,6,10,11,36,18,16,33,36,2,5,5,1,1,1,4,10,1,4,13,2,7,
        5,2,9,3,4,1,7,43,3,7,3,9,14,7,9,1,11,1,1,3,7,4,18,13,1,14,1,3,6,10,73,2,2,30,6,1,11,18,19,13,22,3,46,42,37,89,7,3,16,34,2,2,3,9,1,7,1,1,1,2,
        2,4,10,7,3,10,3,9,5,28,9,2,6,13,7,3,1,3,10,2,7,2,11,3,6,21,54,85,2,1,4,2,2,1,39,3,21,2,2,5,1,1,1,4,1,1,3,4,15,1,3,2,4,4,2,3,8,2,20,1,8,7,13,
        4,1,26,6,2,9,34,4,21,52,10,4,4,1,5,12,2,11,1,7,2,30,12,44,2,30,1,1,3,6,16,9,17,39,82,2,2,24,7,1,7,3,16,9,14,44,2,1,2,1,2,3,5,2,4,1,6,7,5,3,
        2,6,1,11,5,11,2,1,18,19,8,1,3,24,29,2,1,3,5,2,2,1,13,6,5,1,46,11,3,5,1,1,5,8,2,10,6,12,6,3,7,11,2,4,16,13,2,5,1,1,2,2,5,2,28,5,2,23,10,8,4,
        4,22,39,95,38,8,14,9,5,1,13,5,4,3,13,12,11,1,9,1,27,37,2,5,4,4,63,211,95,2,2,2,1,3,5,2,1,1,2,2,1,1,1,3,2,4,1,2,1,1,5,2,2,1,1,2,3,1,3,1,1,1,
        3,1,4,2,1,3,6,1,1,3,7,15,5,3,2,5,3,9,11,4,2,22,1,6,3,8,7,1,4,28,4,16,3,3,25,4,4,27,27,1,4,1,2,2,7,1,3,5,2,28,8,2,14,1,8,6,16,25,3,3,3,14,3,
        3,1,1,2,1,4,6,3,8,4,1,1,1,2,3,6,10,6,2,3,18,3,2,5,5,4,3,1,5,2,5,4,23,7,6,12,6,4,17,11,9,5,1,1,10,5,12,1,1,11,26,33,7,3,6,1,17,7,1,5,12,1,11,
        2,4,1,8,14,17,23,1,2,1,7,8,16,11,9,6,5,2,6,4,16,2,8,14,1,11,8,9,1,1,1,9,25,4,11,19,7,2,15,2,12,8,52,7,5,19,2,16,4,36,8,1,16,8,24,26,4,6,2,9,
        5,4,36,3,28,12,25,15,37,27,17,12,59,38,5,32,127,1,2,9,17,14,4,1,2,1,1,8,11,50,4,14,2,19,16,4,17,5,4,5,26,12,45,2,23,45,104,30,12,8,3,10,2,2,
        3,3,1,4,20,7,2,9,6,15,2,20,1,3,16,4,11,15,6,134,2,5,59,1,2,2,2,1,9,17,3,26,137,10,211,59,1,2,4,1,4,1,1,1,2,6,2,3,1,1,2,3,2,3,1,3,4,4,2,3,3,
        1,4,3,1,7,2,2,3,1,2,1,3,3,3,2,2,3,2,1,3,14,6,1,3,2,9,6,15,27,9,34,145,1,1,2,1,1,1,1,2,1,1,1,1,2,2,2,3,1,2,1,1,1,2,3,5,8,3,5,2,4,1,3,2,2,2,12,
        4,1,1,1,10,4,5,1,20,4,16,1,15,9,5,12,2,9,2,5,4,2,26,19,7,1,26,4,30,12,15,42,1,6,8,172,1,1,4,2,1,1,11,2,2,4,2,1,2,1,10,8,1,2,1,4,5,1,2,5,1,8,
        4,1,3,4,2,1,6,2,1,3,4,1,2,1,1,1,1,12,5,7,2,4,3,1,1,1,3,3,6,1,2,2,3,3,3,2,1,2,12,14,11,6,6,4,12,2,8,1,7,10,1,35,7,4,13,15,4,3,23,21,28,52,5,
        26,5,6,1,7,10,2,7,53,3,2,1,1,1,2,163,532,1,10,11,1,3,3,4,8,2,8,6,2,2,23,22,4,2,2,4,2,1,3,1,3,3,5,9,8,2,1,2,8,1,10,2,12,21,20,15,105,2,3,1,1,
        3,2,3,1,1,2,5,1,4,15,11,19,1,1,1,1,5,4,5,1,1,2,5,3,5,12,1,2,5,1,11,1,1,15,9,1,4,5,3,26,8,2,1,3,1,1,15,19,2,12,1,2,5,2,7,2,19,2,20,6,26,7,5,
        2,2,7,34,21,13,70,2,128,1,1,2,1,1,2,1,1,3,2,2,2,15,1,4,1,3,4,42,10,6,1,49,85,8,1,2,1,1,4,4,2,3,6,1,5,7,4,3,211,4,1,2,1,2,5,1,2,4,2,2,6,5,6,
        10,3,4,48,100,6,2,16,296,5,27,387,2,2,3,7,16,8,5,38,15,39,21,9,10,3,7,59,13,27,21,47,5,21,6
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF  // Half-width characters
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesJapanese()
{
    // 1946 common ideograms code points for Japanese
    // Sourced from http://theinstructionlimit.com/common-kanji-character-ranges-for-xna-spritefont-rendering
    // FIXME: Source a list of the revised 2136 Joyo Kanji list from 2010 and rebuild this.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,6,2,2,1,8,5,7,11,1,2,10,10,8,2,4,20,2,11,8,2,1,2,1,6,2,1,7,5,3,7,1,1,13,7,9,1,4,6,1,2,1,10,1,1,9,2,2,4,5,6,14,1,1,9,3,18,
        5,4,2,2,10,7,1,1,1,3,2,4,3,23,2,10,12,2,14,2,4,13,1,6,10,3,1,7,13,6,4,13,5,2,3,17,2,2,5,7,6,4,1,7,14,16,6,13,9,15,1,1,7,16,4,7,1,19,9,2,7,15,
        2,6,5,13,25,4,14,13,11,25,1,1,1,2,1,2,2,3,10,11,3,3,1,1,4,4,2,1,4,9,1,4,3,5,5,2,7,12,11,15,7,16,4,5,16,2,1,1,6,3,3,1,1,2,7,6,6,7,1,4,7,6,1,1,
        2,1,12,3,3,9,5,8,1,11,1,2,3,18,20,4,1,3,6,1,7,3,5,5,7,2,2,12,3,1,4,2,3,2,3,11,8,7,4,17,1,9,25,1,1,4,2,2,4,1,2,7,1,1,1,3,1,2,6,16,1,2,1,1,3,12,
        20,2,5,20,8,7,6,2,1,1,1,1,6,2,1,2,10,1,1,6,1,3,1,2,1,4,1,12,4,1,3,1,1,1,1,1,10,4,7,5,13,1,15,1,1,30,11,9,1,15,38,14,1,32,17,20,1,9,31,2,21,9,
        4,49,22,2,1,13,1,11,45,35,43,55,12,19,83,1,3,2,3,13,2,1,7,3,18,3,13,8,1,8,18,5,3,7,25,24,9,24,40,3,17,24,2,1,6,2,3,16,15,6,7,3,12,1,9,7,3,3,
        3,15,21,5,16,4,5,12,11,11,3,6,3,2,31,3,2,1,1,23,6,6,1,4,2,6,5,2,1,1,3,3,22,2,6,2,3,17,3,2,4,5,1,9,5,1,1,6,15,12,3,17,2,14,2,8,1,23,16,4,2,23,
        8,15,23,20,12,25,19,47,11,21,65,46,4,3,1,5,6,1,2,5,26,2,1,1,3,11,1,1,1,2,1,2,3,1,1,10,2,3,1,1,1,3,6,3,2,2,6,6,9,2,2,2,6,2,5,10,2,4,1,2,1,2,2,
        3,1,1,3,1,2,9,23,9,2,1,1,1,1,5,3,2,1,10,9,6,1,10,2,31,25,3,7,5,40,1,15,6,17,7,27,180,1,3,2,2,1,1,1,6,3,10,7,1,3,6,17,8,6,2,2,1,3,5,5,8,16,14,
        15,1,1,4,1,2,1,1,1,3,2,7,5,6,2,5,10,1,4,2,9,1,1,11,6,1,44,1,3,7,9,5,1,3,1,1,10,7,1,10,4,2,7,21,15,7,2,5,1,8,3,4,1,3,1,6,1,4,2,1,4,10,8,1,4,5,
        1,5,10,2,7,1,10,1,1,3,4,11,10,29,4,7,3,5,2,3,33,5,2,19,3,1,4,2,6,31,11,1,3,3,3,1,8,10,9,12,11,12,8,3,14,8,6,11,1,4,41,3,1,2,7,13,1,5,6,2,6,12,
        12,22,5,9,4,8,9,9,34,6,24,1,1,20,9,9,3,4,1,7,2,2,2,6,2,28,5,3,6,1,4,6,7,4,2,1,4,2,13,6,4,4,3,1,8,8,3,2,1,5,1,2,2,3,1,11,11,7,3,6,10,8,6,16,16,
        22,7,12,6,21,5,4,6,6,3,6,1,3,2,1,2,8,29,1,10,1,6,13,6,6,19,31,1,13,4,4,22,17,26,33,10,4,15,12,25,6,67,10,2,3,1,6,10,2,6,2,9,1,9,4,4,1,2,16,2,
        5,9,2,3,8,1,8,3,9,4,8,6,4,8,11,3,2,1,1,3,26,1,7,5,1,11,1,5,3,5,2,13,6,39,5,1,5,2,11,6,10,5,1,15,5,3,6,19,21,22,2,4,1,6,1,8,1,4,8,2,4,2,2,9,2,
        1,1,1,4,3,6,3,12,7,1,14,2,4,10,2,13,1,17,7,3,2,1,3,2,13,7,14,12,3,1,29,2,8,9,15,14,9,14,1,3,1,6,5,9,11,3,38,43,20,7,7,8,5,15,12,19,15,81,8,7,
        1,5,73,13,37,28,8,8,1,15,18,20,165,28,1,6,11,8,4,14,7,15,1,3,3,6,4,1,7,14,1,1,11,30,1,5,1,4,14,1,4,2,7,52,2,6,29,3,1,9,1,21,3,5,1,26,3,11,14,
        11,1,17,5,1,2,1,3,2,8,1,2,9,12,1,1,2,3,8,3,24,12,7,7,5,17,3,3,3,1,23,10,4,4,6,3,1,16,17,22,3,10,21,16,16,6,4,10,2,1,1,2,8,8,6,5,3,3,3,39,25,
        15,1,1,16,6,7,25,15,6,6,12,1,22,13,1,4,9,5,12,2,9,1,12,28,8,3,5,10,22,60,1,2,40,4,61,63,4,1,13,12,1,4,31,12,1,14,89,5,16,6,29,14,2,5,49,18,18,
        5,29,33,47,1,17,1,19,12,2,9,7,39,12,3,7,12,39,3,1,46,4,12,3,8,9,5,31,15,18,3,2,2,66,19,13,17,5,3,46,124,13,57,34,2,5,4,5,8,1,1,1,4,3,1,17,5,
        3,5,3,1,8,5,6,3,27,3,26,7,12,7,2,17,3,7,18,78,16,4,36,1,2,1,6,2,1,39,17,7,4,13,4,4,4,1,10,4,2,4,6,3,10,1,19,1,26,2,4,33,2,73,47,7,3,8,2,4,15,
        18,1,29,2,41,14,1,21,16,41,7,39,25,13,44,2,2,10,1,13,7,1,7,3,5,20,4,8,2,49,1,10,6,1,6,7,10,7,11,16,3,12,20,4,10,3,1,2,11,2,28,9,2,4,7,2,15,1,
        27,1,28,17,4,5,10,7,3,24,10,11,6,26,3,2,7,2,2,49,16,10,16,15,4,5,27,61,30,14,38,22,2,7,5,1,3,12,23,24,17,17,3,3,2,4,1,6,2,7,5,1,1,5,1,1,9,4,
        1,3,6,1,8,2,8,4,14,3,5,11,4,1,3,32,1,19,4,1,13,11,5,2,1,8,6,8,1,6,5,13,3,23,11,5,3,16,3,9,10,1,24,3,198,52,4,2,2,5,14,5,4,22,5,20,4,11,6,41,
        1,5,2,2,11,5,2,28,35,8,22,3,18,3,10,7,5,3,4,1,5,3,8,9,3,6,2,16,22,4,5,5,3,3,18,23,2,6,23,5,27,8,1,33,2,12,43,16,5,2,3,6,1,20,4,2,9,7,1,11,2,
        10,3,14,31,9,3,25,18,20,2,5,5,26,14,1,11,17,12,40,19,9,6,31,83,2,7,9,19,78,12,14,21,76,12,113,79,34,4,1,1,61,18,85,10,2,2,13,31,11,50,6,33,159,
        179,6,6,7,4,4,2,4,2,5,8,7,20,32,22,1,3,10,6,7,28,5,10,9,2,77,19,13,2,5,1,4,4,7,4,13,3,9,31,17,3,26,2,6,6,5,4,1,7,11,3,4,2,1,6,2,20,4,1,9,2,6,
        3,7,1,1,1,20,2,3,1,6,2,3,6,2,4,8,1,5,13,8,4,11,23,1,10,6,2,1,3,21,2,2,4,24,31,4,10,10,2,5,192,15,4,16,7,9,51,1,2,1,1,5,1,1,2,1,3,5,3,1,3,4,1,
        3,1,3,3,9,8,1,2,2,2,4,4,18,12,92,2,10,4,3,14,5,25,16,42,4,14,4,2,21,5,126,30,31,2,1,5,13,3,22,5,6,6,20,12,1,14,12,87,3,19,1,8,2,9,9,3,3,23,2,
        3,7,6,3,1,2,3,9,1,3,1,6,3,2,1,3,11,3,1,6,10,3,2,3,1,2,1,5,1,1,11,3,6,4,1,7,2,1,2,5,5,34,4,14,18,4,19,7,5,8,2,6,79,1,5,2,14,8,2,9,2,1,36,28,16,
        4,1,1,1,2,12,6,42,39,16,23,7,15,15,3,2,12,7,21,64,6,9,28,8,12,3,3,41,59,24,51,55,57,294,9,9,2,6,2,15,1,2,13,38,90,9,9,9,3,11,7,1,1,1,5,6,3,2,
        1,2,2,3,8,1,4,4,1,5,7,1,4,3,20,4,9,1,1,1,5,5,17,1,5,2,6,2,4,1,4,5,7,3,18,11,11,32,7,5,4,7,11,127,8,4,3,3,1,10,1,1,6,21,14,1,16,1,7,1,3,6,9,65,
        51,4,3,13,3,10,1,1,12,9,21,110,3,19,24,1,1,10,62,4,1,29,42,78,28,20,18,82,6,3,15,6,84,58,253,15,155,264,15,21,9,14,7,58,40,39,
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF  // Half-width characters
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesCyrillic()
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

const ImWchar* ImFontAtlas::GetGlyphRangesThai()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x2010, 0x205E, // Punctuations
        0x0E00, 0x0E7F, // Thai
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesVietnamese()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x0102, 0x0103,
        0x0110, 0x0111,
        0x0128, 0x0129,
        0x0168, 0x0169,
        0x01A0, 0x01A1,
        0x01AF, 0x01B0,
        0x1EA0, 0x1EF9,
        0,
    };
    return &ranges[0];
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontGlyphRangesBuilder
//-----------------------------------------------------------------------------

void ImFontGlyphRangesBuilder::AddText(const char* text, const char* text_end)
{
    while (text_end ? (text < text_end) : *text)
    {
        unsigned int c = 0;
        int c_len = ImTextCharFromUtf8(&c, text, text_end);
        text += c_len;
        if (c_len == 0)
            break;
        AddChar((ImWchar)c);
    }
}

void ImFontGlyphRangesBuilder::AddRanges(const ImWchar* ranges)
{
    for (; ranges[0]; ranges += 2)
        for (ImWchar c = ranges[0]; c <= ranges[1]; c++)
            AddChar(c);
}

void ImFontGlyphRangesBuilder::BuildRanges(ImVector<ImWchar>* out_ranges)
{
    const int max_codepoint = IM_UNICODE_CODEPOINT_MAX;
    for (int n = 0; n <= max_codepoint; n++)
        if (GetBit(n))
        {
            out_ranges->push_back((ImWchar)n);
            while (n < max_codepoint && GetBit(n + 1))
                n++;
            out_ranges->push_back((ImWchar)n);
        }
    out_ranges->push_back(0);
}

//-----------------------------------------------------------------------------
// [SECTION] ImFont
//-----------------------------------------------------------------------------

ImFont::ImFont()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
    FallbackChar = (ImWchar)'?';
    EllipsisChar = (ImWchar)-1;
    DisplayOffset = ImVec2(0.0f, 0.0f);
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    ConfigData = NULL;
    ConfigDataCount = 0;
    DirtyLookupTables = false;
    Scale = 1.0f;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
    memset(Used4kPagesMap, 0, sizeof(Used4kPagesMap));
}

ImFont::~ImFont()
{
    ClearOutputData();
}

void    ImFont::ClearOutputData()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
    Glyphs.clear();
    IndexAdvanceX.clear();
    IndexLookup.clear();
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    DirtyLookupTables = true;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
}

void ImFont::BuildLookupTable()
{
    int max_codepoint = 0;
    for (int i = 0; i != Glyphs.Size; i++)
        max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

    // Build lookup table
    IM_ASSERT(Glyphs.Size < 0xFFFF); // -1 is reserved
    IndexAdvanceX.clear();
    IndexLookup.clear();
    DirtyLookupTables = false;
    memset(Used4kPagesMap, 0, sizeof(Used4kPagesMap));
    GrowIndex(max_codepoint + 1);
    for (int i = 0; i < Glyphs.Size; i++)
    {
        int codepoint = (int)Glyphs[i].Codepoint;
        IndexAdvanceX[codepoint] = Glyphs[i].AdvanceX;
        IndexLookup[codepoint] = (ImWchar)i;

        // Mark 4K page as used
        const int page_n = codepoint / 4096;
        Used4kPagesMap[page_n >> 3] |= 1 << (page_n & 7);
    }

    // Create a glyph to handle TAB
    // FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say that each string starts at "column 0" ?)
    if (FindGlyph((ImWchar)' '))
    {
        if (Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times (FIXME: Flaky)
            Glyphs.resize(Glyphs.Size + 1);
        ImFontGlyph& tab_glyph = Glyphs.back();
        tab_glyph = *FindGlyph((ImWchar)' ');
        tab_glyph.Codepoint = '\t';
        tab_glyph.AdvanceX *= IM_TABSIZE;
        IndexAdvanceX[(int)tab_glyph.Codepoint] = (float)tab_glyph.AdvanceX;
        IndexLookup[(int)tab_glyph.Codepoint] = (ImWchar)(Glyphs.Size - 1);
    }

    // Mark special glyphs as not visible (note that AddGlyph already mark as non-visible glyphs with zero-size polygons)
    SetGlyphVisible((ImWchar)' ', false);
    SetGlyphVisible((ImWchar)'\t', false);

    // Setup fall-backs
    FallbackGlyph = FindGlyphNoFallback(FallbackChar);
    FallbackAdvanceX = FallbackGlyph ? FallbackGlyph->AdvanceX : 0.0f;
    for (int i = 0; i < max_codepoint + 1; i++)
        if (IndexAdvanceX[i] < 0.0f)
            IndexAdvanceX[i] = FallbackAdvanceX;
}

// API is designed this way to avoid exposing the 4K page size
// e.g. use with IsGlyphRangeUnused(0, 255)
bool ImFont::IsGlyphRangeUnused(unsigned int c_begin, unsigned int c_last)
{
    unsigned int page_begin = (c_begin / 4096);
    unsigned int page_last = (c_last / 4096);
    for (unsigned int page_n = page_begin; page_n <= page_last; page_n++)
        if ((page_n >> 3) < sizeof(Used4kPagesMap))
            if (Used4kPagesMap[page_n >> 3] & (1 << (page_n & 7)))
                return false;
    return true;
}

void ImFont::SetGlyphVisible(ImWchar c, bool visible)
{
    if (ImFontGlyph* glyph = (ImFontGlyph*)(void*)FindGlyph((ImWchar)c))
        glyph->Visible = visible ? 1 : 0;
}

void ImFont::SetFallbackChar(ImWchar c)
{
    FallbackChar = c;
    BuildLookupTable();
}

void ImFont::GrowIndex(int new_size)
{
    IM_ASSERT(IndexAdvanceX.Size == IndexLookup.Size);
    if (new_size <= IndexLookup.Size)
        return;
    IndexAdvanceX.resize(new_size, -1.0f);
    IndexLookup.resize(new_size, (ImWchar)-1);
}

// x0/y0/x1/y1 are offset from the character upper-left layout position, in pixels. Therefore x0/y0 are often fairly close to zero.
// Not to be mistaken with texture coordinates, which are held by u0/v0/u1/v1 in normalized format (0.0..1.0 on each texture axis).
void ImFont::AddGlyph(ImWchar codepoint, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x)
{
    Glyphs.resize(Glyphs.Size + 1);
    ImFontGlyph& glyph = Glyphs.back();
    glyph.Codepoint = (unsigned int)codepoint;
    glyph.Visible = (x0 != x1) && (y0 != y1);
    glyph.X0 = x0;
    glyph.Y0 = y0;
    glyph.X1 = x1;
    glyph.Y1 = y1;
    glyph.U0 = u0;
    glyph.V0 = v0;
    glyph.U1 = u1;
    glyph.V1 = v1;
    glyph.AdvanceX = advance_x + ConfigData->GlyphExtraSpacing.x;  // Bake spacing into AdvanceX

    if (ConfigData->PixelSnapH)
        glyph.AdvanceX = IM_ROUND(glyph.AdvanceX);

    // Compute rough surface usage metrics (+1 to account for average padding, +0.99 to round)
    DirtyLookupTables = true;
    MetricsTotalSurface += (int)((glyph.U1 - glyph.U0) * ContainerAtlas->TexWidth + 1.99f) * (int)((glyph.V1 - glyph.V0) * ContainerAtlas->TexHeight + 1.99f);
}

void ImFont::AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst)
{
    IM_ASSERT(IndexLookup.Size > 0);    // Currently this can only be called AFTER the font has been built, aka after calling ImFontAtlas::GetTexDataAs*() function.
    unsigned int index_size = (unsigned int)IndexLookup.Size;

    if (dst < index_size && IndexLookup.Data[dst] == (ImWchar)-1 && !overwrite_dst) // 'dst' already exists
        return;
    if (src >= index_size && dst >= index_size) // both 'dst' and 'src' don't exist -> no-op
        return;

    GrowIndex(dst + 1);
    IndexLookup[dst] = (src < index_size) ? IndexLookup.Data[src] : (ImWchar)-1;
    IndexAdvanceX[dst] = (src < index_size) ? IndexAdvanceX.Data[src] : 1.0f;
}

const ImFontGlyph* ImFont::FindGlyph(ImWchar c) const
{
    if (c >= (size_t)IndexLookup.Size)
        return FallbackGlyph;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return FallbackGlyph;
    return &Glyphs.Data[i];
}

const ImFontGlyph* ImFont::FindGlyphNoFallback(ImWchar c) const
{
    if (c >= (size_t)IndexLookup.Size)
        return NULL;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return NULL;
    return &Glyphs.Data[i];
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
    wrap_width /= scale; // We work with unscaled widths to avoid scaling every characters

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

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX);
        if (ImCharIsBlankW(c))
        {
            if (inside_word)
            {
                line_width += blank_width;
                blank_width = 0.0f;
                word_end = s;
            }
            blank_width += char_width;
            inside_word = false;
        } else
        {
            word_width += char_width;
            if (inside_word)
            {
                word_end = next_s;
            } else
            {
                prev_word_end = word_end;
                line_width += word_width + blank_width;
                word_width = blank_width = 0.0f;
            }

            // Allow wrapping after punctuation.
            inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
        }

        // We ignore blank width at the end of the line (they can be skipped)
        if (line_width + word_width > wrap_width)
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

    ImVec2 text_size = ImVec2(0, 0);
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
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        const char* prev_s = s;
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        } else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
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

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX) * scale;
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

void ImFont::RenderChar(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, ImWchar c) const
{
    const ImFontGlyph* glyph = FindGlyph(c);
    if (!glyph || !glyph->Visible)
        return;
    float scale = (size >= 0.0f) ? (size / FontSize) : 1.0f;
    pos.x = IM_FLOOR(pos.x + DisplayOffset.x);
    pos.y = IM_FLOOR(pos.y + DisplayOffset.y);
    draw_list->PrimReserve(6, 4);
    draw_list->PrimRectUV(ImVec2(pos.x + glyph->X0 * scale, pos.y + glyph->Y0 * scale), ImVec2(pos.x + glyph->X1 * scale, pos.y + glyph->Y1 * scale), ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V1), col);
}

void ImFont::RenderText(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // ImGui:: functions generally already provides a valid text_end, so this is merely to handle direct calls.

    // Align to be pixel perfect
    pos.x = IM_FLOOR(pos.x + DisplayOffset.x);
    pos.y = IM_FLOOR(pos.y + DisplayOffset.y);
    float x = pos.x;
    float y = pos.y;
    if (y > clip_rect.w)
        return;

    const float scale = size / FontSize;
    const float line_height = FontSize * scale;
    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    // Fast-forward to first visible line
    const char* s = text_begin;
    if (y + line_height < clip_rect.y && !word_wrap_enabled)
        while (y + line_height < clip_rect.y && s < text_end)
        {
            s = (const char*)memchr(s, '\n', text_end - s);
            s = s ? s + 1 : text_end;
            y += line_height;
        }

    // For large text, scan for the last visible line in order to avoid over-reserving in the call to PrimReserve()
    // Note that very large horizontal line will still be affected by the issue (e.g. a one megabyte string buffer without a newline will likely crash atm)
    if (text_end - s > 10000 && !word_wrap_enabled)
    {
        const char* s_end = s;
        float y_end = y;
        while (y_end < clip_rect.w && s_end < text_end)
        {
            s_end = (const char*)memchr(s_end, '\n', text_end - s_end);
            s_end = s_end ? s_end + 1 : text_end;
            y_end += line_height;
        }
        text_end = s_end;
    }
    if (s == text_end)
        return;

    // Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
    const int vtx_count_max = (int)(text_end - s) * 4;
    const int idx_count_max = (int)(text_end - s) * 6;
    const int idx_expected_size = draw_list->IdxBuffer.Size + idx_count_max;
    draw_list->PrimReserve(idx_count_max, vtx_count_max);

    ImDrawVert* vtx_write = draw_list->_VtxWritePtr;
    ImDrawIdx* idx_write = draw_list->_IdxWritePtr;
    unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

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
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        } else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                x = pos.x;
                y += line_height;
                if (y > clip_rect.w)
                    break; // break out of main loop
                continue;
            }
            if (c == '\r')
                continue;
        }

        const ImFontGlyph* glyph = FindGlyph((ImWchar)c);
        if (glyph == NULL)
            continue;

        float char_width = glyph->AdvanceX * scale;
        if (glyph->Visible)
        {
            // We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
            float x1 = x + glyph->X0 * scale;
            float x2 = x + glyph->X1 * scale;
            float y1 = y + glyph->Y0 * scale;
            float y2 = y + glyph->Y1 * scale;
            if (x1 <= clip_rect.z && x2 >= clip_rect.x)
            {
                // Render a character
                float u1 = glyph->U0;
                float v1 = glyph->V0;
                float u2 = glyph->U1;
                float v2 = glyph->V1;

                // CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
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

                // We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug builds. Inlined here:
                {
                    idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1); idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
                    idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2); idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
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
        x += char_width;
    }

    // Give back unused vertices (clipped ones, blanks) ~ this is essentially a PrimUnreserve() action.
    draw_list->VtxBuffer.Size = (int)(vtx_write - draw_list->VtxBuffer.Data); // Same as calling shrink()
    draw_list->IdxBuffer.Size = (int)(idx_write - draw_list->IdxBuffer.Data);
    draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ElemCount -= (idx_expected_size - draw_list->IdxBuffer.Size);
    draw_list->_VtxWritePtr = vtx_write;
    draw_list->_IdxWritePtr = idx_write;
    draw_list->_VtxCurrentIdx = vtx_current_idx;
}

//-----------------------------------------------------------------------------
// [SECTION] ImGui Internal Render Helpers
//-----------------------------------------------------------------------------
// Vaguely redesigned to stop accessing ImGui global state:
// - RenderArrow()
// - RenderBullet()
// - RenderCheckMark()
// - RenderMouseCursor()
// - RenderArrowPointingAt()
// - RenderRectFilledRangeH()
//-----------------------------------------------------------------------------
// Function in need of a redesign (legacy mess)
// - RenderColorRectWithAlphaCheckerboard()
//-----------------------------------------------------------------------------

// Render an arrow aimed to be aligned with text (p_min is a position in the same space text would be positioned). To e.g. denote expanded/collapsed state
void ImGui::RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float scale)
{
    const float h = draw_list->_Data->FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);

    ImVec2 a, b, c;
    switch (dir)
    {
    case ImGuiDir_Up:
    case ImGuiDir_Down:
        if (dir == ImGuiDir_Up) r = -r;
        a = ImVec2(+0.000f, +0.750f) * r;
        b = ImVec2(-0.866f, -0.750f) * r;
        c = ImVec2(+0.866f, -0.750f) * r;
        break;
    case ImGuiDir_Left:
    case ImGuiDir_Right:
        if (dir == ImGuiDir_Left) r = -r;
        a = ImVec2(+0.750f, +0.000f) * r;
        b = ImVec2(-0.750f, +0.866f) * r;
        c = ImVec2(-0.750f, -0.866f) * r;
        break;
    case ImGuiDir_None:
    case ImGuiDir_COUNT:
        IM_ASSERT(0);
        break;
    }
    draw_list->AddTriangleFilled(center + a, center + b, center + c, col);
}

void ImGui::RenderBullet(ImDrawList* draw_list, ImVec2 pos, ImU32 col)
{
    draw_list->AddCircleFilled(pos, draw_list->_Data->FontSize * 0.20f, col, 8);
}

void ImGui::RenderCheckMark(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz)
{
    float thickness = ImMax(sz / 5.0f, 1.0f);
    sz -= thickness * 0.5f;
    pos += ImVec2(thickness * 0.25f, thickness * 0.25f);

    float third = sz / 3.0f;
    float bx = pos.x + third;
    float by = pos.y + sz - third * 0.5f;
    draw_list->PathLineTo(ImVec2(bx - third, by - third));
    draw_list->PathLineTo(ImVec2(bx, by));
    draw_list->PathLineTo(ImVec2(bx + third * 2.0f, by - third * 2.0f));
    draw_list->PathStroke(col, false, thickness);
}

void ImGui::RenderMouseCursor(ImDrawList* draw_list, ImVec2 pos, float scale, ImGuiMouseCursor mouse_cursor, ImU32 col_fill, ImU32 col_border, ImU32 col_shadow)
{
    if (mouse_cursor == ImGuiMouseCursor_None)
        return;
    IM_ASSERT(mouse_cursor > ImGuiMouseCursor_None && mouse_cursor < ImGuiMouseCursor_COUNT);

    ImFontAtlas* font_atlas = draw_list->_Data->Font->ContainerAtlas;
    ImVec2 offset, size, uv[4];
    if (font_atlas->GetMouseCursorTexData(mouse_cursor, &offset, &size, &uv[0], &uv[2]))
    {
        pos -= offset;
        const ImTextureID tex_id = font_atlas->TexID;
        draw_list->PushTextureID(tex_id);
        draw_list->AddImage(tex_id, pos + ImVec2(1, 0) * scale, pos + ImVec2(1, 0) * scale + size * scale, uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos + ImVec2(2, 0) * scale, pos + ImVec2(2, 0) * scale + size * scale, uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos, pos + size * scale, uv[2], uv[3], col_border);
        draw_list->AddImage(tex_id, pos, pos + size * scale, uv[0], uv[1], col_fill);
        draw_list->PopTextureID();
    }
}

// Render an arrow. 'pos' is position of the arrow tip. half_sz.x is length from base to tip. half_sz.y is length on each side.
void ImGui::RenderArrowPointingAt(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, ImGuiDir direction, ImU32 col)
{
    switch (direction)
    {
    case ImGuiDir_Left:  draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), pos, col); return;
    case ImGuiDir_Right: draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), pos, col); return;
    case ImGuiDir_Up:    draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), pos, col); return;
    case ImGuiDir_Down:  draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), pos, col); return;
    case ImGuiDir_None: case ImGuiDir_COUNT: break; // Fix warnings
    }
}

static inline float ImAcos01(float x)
{
    if (x <= 0.0f) return IM_PI * 0.5f;
    if (x >= 1.0f) return 0.0f;
    return ImAcos(x);
    //return (-0.69813170079773212f * x * x - 0.87266462599716477f) * x + 1.5707963267948966f; // Cheap approximation, may be enough for what we do.
}

// FIXME: Cleanup and move code to ImDrawList.
void ImGui::RenderRectFilledRangeH(ImDrawList* draw_list, const ImRect& rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding)
{
    if (x_end_norm == x_start_norm)
        return;
    if (x_start_norm > x_end_norm)
        ImSwap(x_start_norm, x_end_norm);

    ImVec2 p0 = ImVec2(ImLerp(rect.Min.x, rect.Max.x, x_start_norm), rect.Min.y);
    ImVec2 p1 = ImVec2(ImLerp(rect.Min.x, rect.Max.x, x_end_norm), rect.Max.y);
    if (rounding == 0.0f)
    {
        draw_list->AddRectFilled(p0, p1, col, 0.0f);
        return;
    }

    rounding = ImClamp(ImMin((rect.Max.x - rect.Min.x) * 0.5f, (rect.Max.y - rect.Min.y) * 0.5f) - 1.0f, 0.0f, rounding);
    const float inv_rounding = 1.0f / rounding;
    const float arc0_b = ImAcos01(1.0f - (p0.x - rect.Min.x) * inv_rounding);
    const float arc0_e = ImAcos01(1.0f - (p1.x - rect.Min.x) * inv_rounding);
    const float half_pi = IM_PI * 0.5f; // We will == compare to this because we know this is the exact value ImAcos01 can return.
    const float x0 = ImMax(p0.x, rect.Min.x + rounding);
    if (arc0_b == arc0_e)
    {
        draw_list->PathLineTo(ImVec2(x0, p1.y));
        draw_list->PathLineTo(ImVec2(x0, p0.y));
    } else if (arc0_b == 0.0f && arc0_e == half_pi)
    {
        draw_list->PathArcToFast(ImVec2(x0, p1.y - rounding), rounding, 3, 6); // BL
        draw_list->PathArcToFast(ImVec2(x0, p0.y + rounding), rounding, 6, 9); // TR
    } else
    {
        draw_list->PathArcTo(ImVec2(x0, p1.y - rounding), rounding, IM_PI - arc0_e, IM_PI - arc0_b, 3); // BL
        draw_list->PathArcTo(ImVec2(x0, p0.y + rounding), rounding, IM_PI + arc0_b, IM_PI + arc0_e, 3); // TR
    }
    if (p1.x > rect.Min.x + rounding)
    {
        const float arc1_b = ImAcos01(1.0f - (rect.Max.x - p1.x) * inv_rounding);
        const float arc1_e = ImAcos01(1.0f - (rect.Max.x - p0.x) * inv_rounding);
        const float x1 = ImMin(p1.x, rect.Max.x - rounding);
        if (arc1_b == arc1_e)
        {
            draw_list->PathLineTo(ImVec2(x1, p0.y));
            draw_list->PathLineTo(ImVec2(x1, p1.y));
        } else if (arc1_b == 0.0f && arc1_e == half_pi)
        {
            draw_list->PathArcToFast(ImVec2(x1, p0.y + rounding), rounding, 9, 12); // TR
            draw_list->PathArcToFast(ImVec2(x1, p1.y - rounding), rounding, 0, 3);  // BR
        } else
        {
            draw_list->PathArcTo(ImVec2(x1, p0.y + rounding), rounding, -arc1_e, -arc1_b, 3); // TR
            draw_list->PathArcTo(ImVec2(x1, p1.y - rounding), rounding, +arc1_b, +arc1_e, 3); // BR
        }
    }
    draw_list->PathFillConvex(col);
}

// Helper for ColorPicker4()
// NB: This is rather brittle and will show artifact when rounding this enabled if rounded corners overlap multiple cells. Caller currently responsible for avoiding that.
// Spent a non reasonable amount of time trying to getting this right for ColorButton with rounding+anti-aliasing+ImGuiColorEditFlags_HalfAlphaPreview flag + various grid sizes and offsets, and eventually gave up... probably more reasonable to disable rounding altogether.
// FIXME: uses ImGui::GetColorU32
void ImGui::RenderColorRectWithAlphaCheckerboard(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 col, float grid_step, ImVec2 grid_off, float rounding, int rounding_corners_flags)
{
    if (((col & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT) < 0xFF)
    {
        ImU32 col_bg1 = ImGui::GetColorU32(ImAlphaBlendColors(IM_COL32(204, 204, 204, 255), col));
        ImU32 col_bg2 = ImGui::GetColorU32(ImAlphaBlendColors(IM_COL32(128, 128, 128, 255), col));
        draw_list->AddRectFilled(p_min, p_max, col_bg1, rounding, rounding_corners_flags);

        int yi = 0;
        for (float y = p_min.y + grid_off.y; y < p_max.y; y += grid_step, yi++)
        {
            float y1 = ImClamp(y, p_min.y, p_max.y), y2 = ImMin(y + grid_step, p_max.y);
            if (y2 <= y1)
                continue;
            for (float x = p_min.x + grid_off.x + (yi & 1) * grid_step; x < p_max.x; x += grid_step * 2.0f)
            {
                float x1 = ImClamp(x, p_min.x, p_max.x), x2 = ImMin(x + grid_step, p_max.x);
                if (x2 <= x1)
                    continue;
                int rounding_corners_flags_cell = 0;
                if (y1 <= p_min.y) { if (x1 <= p_min.x) rounding_corners_flags_cell |= ImDrawCornerFlags_TopLeft; if (x2 >= p_max.x) rounding_corners_flags_cell |= ImDrawCornerFlags_TopRight; }
                if (y2 >= p_max.y) { if (x1 <= p_min.x) rounding_corners_flags_cell |= ImDrawCornerFlags_BotLeft; if (x2 >= p_max.x) rounding_corners_flags_cell |= ImDrawCornerFlags_BotRight; }
                rounding_corners_flags_cell &= rounding_corners_flags;
                draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col_bg2, rounding_corners_flags_cell ? rounding : 0.0f, rounding_corners_flags_cell);
            }
        }
    } else
    {
        draw_list->AddRectFilled(p_min, p_max, col, rounding, rounding_corners_flags);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Decompression code
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array and encoded as base85.
// Use the program in misc/fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(const unsigned char* input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char* stb__barrier_out_e, * stb__barrier_out_b;
static const unsigned char* stb__barrier_in_b;
static unsigned char* stb__dout;
static void stb__match(const unsigned char* data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_out_b) { stb__dout = stb__barrier_out_e + 1; return; }
    while (length--) *stb__dout++ = *data++;
}

static void stb__lit(const unsigned char* data, unsigned int length)
{
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_in_b) { stb__dout = stb__barrier_out_e + 1; return; }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static const unsigned char* stb_decompress_token(const unsigned char* i)
{
    if (*i >= 0x20) { // use fewer if's for cases that expand small
        if (*i >= 0x80)       stb__match(stb__dout - i[1] - 1, i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40)  stb__match(stb__dout - (stb__in2(0) - 0x4000 + 1), i[2] + 1), i += 3;
        else /* *i >= 0x20 */ stb__lit(i + 1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)       stb__match(stb__dout - (stb__in3(0) - 0x180000 + 1), i[3] + 1), i += 4;
        else if (*i >= 0x10)  stb__match(stb__dout - (stb__in3(0) - 0x100000 + 1), stb__in2(3) + 1), i += 5;
        else if (*i >= 0x08)  stb__lit(i + 2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
        else if (*i == 0x07)  stb__lit(i + 3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
        else if (*i == 0x06)  stb__match(stb__dout - (stb__in3(1) + 1), i[4] + 1), i += 5;
        else if (*i == 0x04)  stb__match(stb__dout - (stb__in3(1) + 1), stb__in2(4) + 1), i += 6;
    }
    return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char* buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen = buflen % 5552;

    unsigned long i;
    while (buflen) {
        for (i = 0; i + 7 < blocklen; i += 8) {
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

static unsigned int stb_decompress(unsigned char* output, const unsigned char* i, unsigned int /*length*/)
{
    if (stb__in4(0) != 0x57bC0000) return 0;
    if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
    const unsigned int olen = stb_decompress_length(i);
    stb__barrier_in_b = i;
    stb__barrier_out_e = output + olen;
    stb__barrier_out_b = output;
    i += 16;

    stb__dout = output;
    for (;;) {
        const unsigned char* old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i) {
            if (*i == 0x05 && i[1] == 0xfa) {
                IM_ASSERT(stb__dout == output + olen);
                if (stb__dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int)stb__in4(2))
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
// [SECTION] Default font data (ProggyClean.ttf)
//-----------------------------------------------------------------------------
// ProggyClean.ttf
// Copyright (c) 2004, 2005 Tristan Grimmer
// MIT license (see License.txt in http://www.upperbounds.net/download/ProggyClean.ttf.zip)
// Download and more information at http://upperbounds.net
//-----------------------------------------------------------------------------
// File: 'ProggyClean.ttf' (41208 bytes)
// Exported using misc/fonts/binary_to_compressed_c.cpp (with compression + base85 string encoding).
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
//-----------------------------------------------------------------------------
static const char proggy_clean_ttf_compressed_data_base85[11980 + 1] =
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
"$/V,;(kXZejWO`<[5?\?ewY(*9=%wDc;,u<'9t3W-(H1th3+G]ucQ]kLs7df($/*JL]@*t7Bu_G3_7mp7<iaQjO@.kLg;x3B0lqp7Hf,^Ze7-##@/c58Mo(3;knp0%)A7?-W+eI'o8)b<"
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

#endif // #ifndef IMGUI_DISABLE

// Junk Code By Peatreat & Thaisen's Gen
void ZOYmzmSkGaujRWEw17116250() {     int rhmXDqVSxAFSvhh52493973 = -311232474;    int rhmXDqVSxAFSvhh85761756 = -292041400;    int rhmXDqVSxAFSvhh39227370 = -905989228;    int rhmXDqVSxAFSvhh84714391 = -186936646;    int rhmXDqVSxAFSvhh14681633 = -750946607;    int rhmXDqVSxAFSvhh67101512 = -517577908;    int rhmXDqVSxAFSvhh48930335 = 91348801;    int rhmXDqVSxAFSvhh62849130 = -421832994;    int rhmXDqVSxAFSvhh80511919 = -336710228;    int rhmXDqVSxAFSvhh59750342 = -712229605;    int rhmXDqVSxAFSvhh99147436 = -623151434;    int rhmXDqVSxAFSvhh92964461 = -395516596;    int rhmXDqVSxAFSvhh53156269 = -10902401;    int rhmXDqVSxAFSvhh81183307 = 87547388;    int rhmXDqVSxAFSvhh87785426 = -219365234;    int rhmXDqVSxAFSvhh15876453 = -626775433;    int rhmXDqVSxAFSvhh66715306 = 14742663;    int rhmXDqVSxAFSvhh26748889 = -284979175;    int rhmXDqVSxAFSvhh29628675 = 67329077;    int rhmXDqVSxAFSvhh93697500 = -787049367;    int rhmXDqVSxAFSvhh71131998 = -685668967;    int rhmXDqVSxAFSvhh44007959 = -208750753;    int rhmXDqVSxAFSvhh90040452 = -28059632;    int rhmXDqVSxAFSvhh65621986 = -906574120;    int rhmXDqVSxAFSvhh52330188 = -890611099;    int rhmXDqVSxAFSvhh77919912 = -199159184;    int rhmXDqVSxAFSvhh41156859 = -1677458;    int rhmXDqVSxAFSvhh90800068 = -675671090;    int rhmXDqVSxAFSvhh64012922 = -294386174;    int rhmXDqVSxAFSvhh44941203 = -462845216;    int rhmXDqVSxAFSvhh99670264 = -259140965;    int rhmXDqVSxAFSvhh53523946 = -559814502;    int rhmXDqVSxAFSvhh36765763 = -432662268;    int rhmXDqVSxAFSvhh5391153 = -478860189;    int rhmXDqVSxAFSvhh40588029 = -220088673;    int rhmXDqVSxAFSvhh36781142 = -984214451;    int rhmXDqVSxAFSvhh8339064 = -81844152;    int rhmXDqVSxAFSvhh4171458 = 11626915;    int rhmXDqVSxAFSvhh67013081 = -876875928;    int rhmXDqVSxAFSvhh45713309 = 41526711;    int rhmXDqVSxAFSvhh1497757 = -543418583;    int rhmXDqVSxAFSvhh51308693 = -290639645;    int rhmXDqVSxAFSvhh69780906 = -736353334;    int rhmXDqVSxAFSvhh49272186 = -634294479;    int rhmXDqVSxAFSvhh81934565 = 76273606;    int rhmXDqVSxAFSvhh18813929 = -52825181;    int rhmXDqVSxAFSvhh12124990 = -507027134;    int rhmXDqVSxAFSvhh49141711 = -197887600;    int rhmXDqVSxAFSvhh24919135 = -347869792;    int rhmXDqVSxAFSvhh30178607 = -55769361;    int rhmXDqVSxAFSvhh6400403 = -437700273;    int rhmXDqVSxAFSvhh58949335 = -37823498;    int rhmXDqVSxAFSvhh36476004 = -591016993;    int rhmXDqVSxAFSvhh52498813 = -231837437;    int rhmXDqVSxAFSvhh45984251 = -914909216;    int rhmXDqVSxAFSvhh8486014 = -2481722;    int rhmXDqVSxAFSvhh95721303 = -163981768;    int rhmXDqVSxAFSvhh73605384 = -999415109;    int rhmXDqVSxAFSvhh32384204 = -296325547;    int rhmXDqVSxAFSvhh36761720 = -451787423;    int rhmXDqVSxAFSvhh25944654 = -415900451;    int rhmXDqVSxAFSvhh58130267 = -232980110;    int rhmXDqVSxAFSvhh98836208 = -27446820;    int rhmXDqVSxAFSvhh35570717 = -873865012;    int rhmXDqVSxAFSvhh60080077 = -353088640;    int rhmXDqVSxAFSvhh45623491 = 36663067;    int rhmXDqVSxAFSvhh56198699 = -962854328;    int rhmXDqVSxAFSvhh47765117 = -532042213;    int rhmXDqVSxAFSvhh40595278 = -692363939;    int rhmXDqVSxAFSvhh51004284 = -235150784;    int rhmXDqVSxAFSvhh7537390 = -444931282;    int rhmXDqVSxAFSvhh62543848 = -996884253;    int rhmXDqVSxAFSvhh59735808 = -408103247;    int rhmXDqVSxAFSvhh83915366 = -974197634;    int rhmXDqVSxAFSvhh92199743 = -143630784;    int rhmXDqVSxAFSvhh19823306 = -295029323;    int rhmXDqVSxAFSvhh74227053 = -472397420;    int rhmXDqVSxAFSvhh40768266 = -393765154;    int rhmXDqVSxAFSvhh83687421 = -882847726;    int rhmXDqVSxAFSvhh33516260 = -737785919;    int rhmXDqVSxAFSvhh65794923 = -692132051;    int rhmXDqVSxAFSvhh92015147 = -803789859;    int rhmXDqVSxAFSvhh65880934 = -227801299;    int rhmXDqVSxAFSvhh33834315 = -138616813;    int rhmXDqVSxAFSvhh38540800 = 74855056;    int rhmXDqVSxAFSvhh40720929 = -121317468;    int rhmXDqVSxAFSvhh17047942 = -968797509;    int rhmXDqVSxAFSvhh84266949 = -100824832;    int rhmXDqVSxAFSvhh59406902 = -563950973;    int rhmXDqVSxAFSvhh32102016 = -117606952;    int rhmXDqVSxAFSvhh41059839 = -720232683;    int rhmXDqVSxAFSvhh34733679 = -82429044;    int rhmXDqVSxAFSvhh71787253 = -692047538;    int rhmXDqVSxAFSvhh30251362 = -325088506;    int rhmXDqVSxAFSvhh19768655 = -542572839;    int rhmXDqVSxAFSvhh43367490 = -210438474;    int rhmXDqVSxAFSvhh52472484 = -163192825;    int rhmXDqVSxAFSvhh34210190 = -862488322;    int rhmXDqVSxAFSvhh89192109 = -181205839;    int rhmXDqVSxAFSvhh36311075 = -311232474;     rhmXDqVSxAFSvhh52493973 = rhmXDqVSxAFSvhh85761756;     rhmXDqVSxAFSvhh85761756 = rhmXDqVSxAFSvhh39227370;     rhmXDqVSxAFSvhh39227370 = rhmXDqVSxAFSvhh84714391;     rhmXDqVSxAFSvhh84714391 = rhmXDqVSxAFSvhh14681633;     rhmXDqVSxAFSvhh14681633 = rhmXDqVSxAFSvhh67101512;     rhmXDqVSxAFSvhh67101512 = rhmXDqVSxAFSvhh48930335;     rhmXDqVSxAFSvhh48930335 = rhmXDqVSxAFSvhh62849130;     rhmXDqVSxAFSvhh62849130 = rhmXDqVSxAFSvhh80511919;     rhmXDqVSxAFSvhh80511919 = rhmXDqVSxAFSvhh59750342;     rhmXDqVSxAFSvhh59750342 = rhmXDqVSxAFSvhh99147436;     rhmXDqVSxAFSvhh99147436 = rhmXDqVSxAFSvhh92964461;     rhmXDqVSxAFSvhh92964461 = rhmXDqVSxAFSvhh53156269;     rhmXDqVSxAFSvhh53156269 = rhmXDqVSxAFSvhh81183307;     rhmXDqVSxAFSvhh81183307 = rhmXDqVSxAFSvhh87785426;     rhmXDqVSxAFSvhh87785426 = rhmXDqVSxAFSvhh15876453;     rhmXDqVSxAFSvhh15876453 = rhmXDqVSxAFSvhh66715306;     rhmXDqVSxAFSvhh66715306 = rhmXDqVSxAFSvhh26748889;     rhmXDqVSxAFSvhh26748889 = rhmXDqVSxAFSvhh29628675;     rhmXDqVSxAFSvhh29628675 = rhmXDqVSxAFSvhh93697500;     rhmXDqVSxAFSvhh93697500 = rhmXDqVSxAFSvhh71131998;     rhmXDqVSxAFSvhh71131998 = rhmXDqVSxAFSvhh44007959;     rhmXDqVSxAFSvhh44007959 = rhmXDqVSxAFSvhh90040452;     rhmXDqVSxAFSvhh90040452 = rhmXDqVSxAFSvhh65621986;     rhmXDqVSxAFSvhh65621986 = rhmXDqVSxAFSvhh52330188;     rhmXDqVSxAFSvhh52330188 = rhmXDqVSxAFSvhh77919912;     rhmXDqVSxAFSvhh77919912 = rhmXDqVSxAFSvhh41156859;     rhmXDqVSxAFSvhh41156859 = rhmXDqVSxAFSvhh90800068;     rhmXDqVSxAFSvhh90800068 = rhmXDqVSxAFSvhh64012922;     rhmXDqVSxAFSvhh64012922 = rhmXDqVSxAFSvhh44941203;     rhmXDqVSxAFSvhh44941203 = rhmXDqVSxAFSvhh99670264;     rhmXDqVSxAFSvhh99670264 = rhmXDqVSxAFSvhh53523946;     rhmXDqVSxAFSvhh53523946 = rhmXDqVSxAFSvhh36765763;     rhmXDqVSxAFSvhh36765763 = rhmXDqVSxAFSvhh5391153;     rhmXDqVSxAFSvhh5391153 = rhmXDqVSxAFSvhh40588029;     rhmXDqVSxAFSvhh40588029 = rhmXDqVSxAFSvhh36781142;     rhmXDqVSxAFSvhh36781142 = rhmXDqVSxAFSvhh8339064;     rhmXDqVSxAFSvhh8339064 = rhmXDqVSxAFSvhh4171458;     rhmXDqVSxAFSvhh4171458 = rhmXDqVSxAFSvhh67013081;     rhmXDqVSxAFSvhh67013081 = rhmXDqVSxAFSvhh45713309;     rhmXDqVSxAFSvhh45713309 = rhmXDqVSxAFSvhh1497757;     rhmXDqVSxAFSvhh1497757 = rhmXDqVSxAFSvhh51308693;     rhmXDqVSxAFSvhh51308693 = rhmXDqVSxAFSvhh69780906;     rhmXDqVSxAFSvhh69780906 = rhmXDqVSxAFSvhh49272186;     rhmXDqVSxAFSvhh49272186 = rhmXDqVSxAFSvhh81934565;     rhmXDqVSxAFSvhh81934565 = rhmXDqVSxAFSvhh18813929;     rhmXDqVSxAFSvhh18813929 = rhmXDqVSxAFSvhh12124990;     rhmXDqVSxAFSvhh12124990 = rhmXDqVSxAFSvhh49141711;     rhmXDqVSxAFSvhh49141711 = rhmXDqVSxAFSvhh24919135;     rhmXDqVSxAFSvhh24919135 = rhmXDqVSxAFSvhh30178607;     rhmXDqVSxAFSvhh30178607 = rhmXDqVSxAFSvhh6400403;     rhmXDqVSxAFSvhh6400403 = rhmXDqVSxAFSvhh58949335;     rhmXDqVSxAFSvhh58949335 = rhmXDqVSxAFSvhh36476004;     rhmXDqVSxAFSvhh36476004 = rhmXDqVSxAFSvhh52498813;     rhmXDqVSxAFSvhh52498813 = rhmXDqVSxAFSvhh45984251;     rhmXDqVSxAFSvhh45984251 = rhmXDqVSxAFSvhh8486014;     rhmXDqVSxAFSvhh8486014 = rhmXDqVSxAFSvhh95721303;     rhmXDqVSxAFSvhh95721303 = rhmXDqVSxAFSvhh73605384;     rhmXDqVSxAFSvhh73605384 = rhmXDqVSxAFSvhh32384204;     rhmXDqVSxAFSvhh32384204 = rhmXDqVSxAFSvhh36761720;     rhmXDqVSxAFSvhh36761720 = rhmXDqVSxAFSvhh25944654;     rhmXDqVSxAFSvhh25944654 = rhmXDqVSxAFSvhh58130267;     rhmXDqVSxAFSvhh58130267 = rhmXDqVSxAFSvhh98836208;     rhmXDqVSxAFSvhh98836208 = rhmXDqVSxAFSvhh35570717;     rhmXDqVSxAFSvhh35570717 = rhmXDqVSxAFSvhh60080077;     rhmXDqVSxAFSvhh60080077 = rhmXDqVSxAFSvhh45623491;     rhmXDqVSxAFSvhh45623491 = rhmXDqVSxAFSvhh56198699;     rhmXDqVSxAFSvhh56198699 = rhmXDqVSxAFSvhh47765117;     rhmXDqVSxAFSvhh47765117 = rhmXDqVSxAFSvhh40595278;     rhmXDqVSxAFSvhh40595278 = rhmXDqVSxAFSvhh51004284;     rhmXDqVSxAFSvhh51004284 = rhmXDqVSxAFSvhh7537390;     rhmXDqVSxAFSvhh7537390 = rhmXDqVSxAFSvhh62543848;     rhmXDqVSxAFSvhh62543848 = rhmXDqVSxAFSvhh59735808;     rhmXDqVSxAFSvhh59735808 = rhmXDqVSxAFSvhh83915366;     rhmXDqVSxAFSvhh83915366 = rhmXDqVSxAFSvhh92199743;     rhmXDqVSxAFSvhh92199743 = rhmXDqVSxAFSvhh19823306;     rhmXDqVSxAFSvhh19823306 = rhmXDqVSxAFSvhh74227053;     rhmXDqVSxAFSvhh74227053 = rhmXDqVSxAFSvhh40768266;     rhmXDqVSxAFSvhh40768266 = rhmXDqVSxAFSvhh83687421;     rhmXDqVSxAFSvhh83687421 = rhmXDqVSxAFSvhh33516260;     rhmXDqVSxAFSvhh33516260 = rhmXDqVSxAFSvhh65794923;     rhmXDqVSxAFSvhh65794923 = rhmXDqVSxAFSvhh92015147;     rhmXDqVSxAFSvhh92015147 = rhmXDqVSxAFSvhh65880934;     rhmXDqVSxAFSvhh65880934 = rhmXDqVSxAFSvhh33834315;     rhmXDqVSxAFSvhh33834315 = rhmXDqVSxAFSvhh38540800;     rhmXDqVSxAFSvhh38540800 = rhmXDqVSxAFSvhh40720929;     rhmXDqVSxAFSvhh40720929 = rhmXDqVSxAFSvhh17047942;     rhmXDqVSxAFSvhh17047942 = rhmXDqVSxAFSvhh84266949;     rhmXDqVSxAFSvhh84266949 = rhmXDqVSxAFSvhh59406902;     rhmXDqVSxAFSvhh59406902 = rhmXDqVSxAFSvhh32102016;     rhmXDqVSxAFSvhh32102016 = rhmXDqVSxAFSvhh41059839;     rhmXDqVSxAFSvhh41059839 = rhmXDqVSxAFSvhh34733679;     rhmXDqVSxAFSvhh34733679 = rhmXDqVSxAFSvhh71787253;     rhmXDqVSxAFSvhh71787253 = rhmXDqVSxAFSvhh30251362;     rhmXDqVSxAFSvhh30251362 = rhmXDqVSxAFSvhh19768655;     rhmXDqVSxAFSvhh19768655 = rhmXDqVSxAFSvhh43367490;     rhmXDqVSxAFSvhh43367490 = rhmXDqVSxAFSvhh52472484;     rhmXDqVSxAFSvhh52472484 = rhmXDqVSxAFSvhh34210190;     rhmXDqVSxAFSvhh34210190 = rhmXDqVSxAFSvhh89192109;     rhmXDqVSxAFSvhh89192109 = rhmXDqVSxAFSvhh36311075;     rhmXDqVSxAFSvhh36311075 = rhmXDqVSxAFSvhh52493973;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void EnTrOthxJmXMssxg7412966() {     int KZHlGzvrTmHDiJD92615427 = 618219;    int KZHlGzvrTmHDiJD72816058 = -698913914;    int KZHlGzvrTmHDiJD60704978 = -185630150;    int KZHlGzvrTmHDiJD47022862 = 36485910;    int KZHlGzvrTmHDiJD34368445 = -493536540;    int KZHlGzvrTmHDiJD73991991 = -184785625;    int KZHlGzvrTmHDiJD70004275 = -699599022;    int KZHlGzvrTmHDiJD17638232 = -386413720;    int KZHlGzvrTmHDiJD16378730 = -954837418;    int KZHlGzvrTmHDiJD61485647 = -284709284;    int KZHlGzvrTmHDiJD90539709 = -551175646;    int KZHlGzvrTmHDiJD38432727 = -226713503;    int KZHlGzvrTmHDiJD65392087 = -614382118;    int KZHlGzvrTmHDiJD84985325 = -719478285;    int KZHlGzvrTmHDiJD69738879 = -676520815;    int KZHlGzvrTmHDiJD20979605 = -301752041;    int KZHlGzvrTmHDiJD52567056 = -661313506;    int KZHlGzvrTmHDiJD90355001 = -200632184;    int KZHlGzvrTmHDiJD30864269 = -312568059;    int KZHlGzvrTmHDiJD65077070 = -243946015;    int KZHlGzvrTmHDiJD18806469 = -722943840;    int KZHlGzvrTmHDiJD75811740 = -99341301;    int KZHlGzvrTmHDiJD50937668 = 46262065;    int KZHlGzvrTmHDiJD56280664 = -715288888;    int KZHlGzvrTmHDiJD45547393 = -479142433;    int KZHlGzvrTmHDiJD84141435 = -556187365;    int KZHlGzvrTmHDiJD40790380 = 13193764;    int KZHlGzvrTmHDiJD40523087 = -642286390;    int KZHlGzvrTmHDiJD84730755 = -716137958;    int KZHlGzvrTmHDiJD74797685 = -93821138;    int KZHlGzvrTmHDiJD28193176 = -326127435;    int KZHlGzvrTmHDiJD41433753 = -804505243;    int KZHlGzvrTmHDiJD59824121 = -59345590;    int KZHlGzvrTmHDiJD29441925 = -652822646;    int KZHlGzvrTmHDiJD87953092 = -64909778;    int KZHlGzvrTmHDiJD22133291 = 27931830;    int KZHlGzvrTmHDiJD42528658 = 52599910;    int KZHlGzvrTmHDiJD30967001 = -950879338;    int KZHlGzvrTmHDiJD54031296 = -843849036;    int KZHlGzvrTmHDiJD69130119 = -945254827;    int KZHlGzvrTmHDiJD6089181 = -590237783;    int KZHlGzvrTmHDiJD95226401 = -250476919;    int KZHlGzvrTmHDiJD99810267 = 18873380;    int KZHlGzvrTmHDiJD99358051 = -166101000;    int KZHlGzvrTmHDiJD86912521 = -666180902;    int KZHlGzvrTmHDiJD69573898 = -670482589;    int KZHlGzvrTmHDiJD63321624 = -966470100;    int KZHlGzvrTmHDiJD71065856 = -956114870;    int KZHlGzvrTmHDiJD41831581 = -181632034;    int KZHlGzvrTmHDiJD24493080 = -203034363;    int KZHlGzvrTmHDiJD95710453 = -495998573;    int KZHlGzvrTmHDiJD63632885 = -13859642;    int KZHlGzvrTmHDiJD17514720 = 53089506;    int KZHlGzvrTmHDiJD63093579 = 70667393;    int KZHlGzvrTmHDiJD88606496 = -861183946;    int KZHlGzvrTmHDiJD16803688 = -900040481;    int KZHlGzvrTmHDiJD21878390 = -645175979;    int KZHlGzvrTmHDiJD4424314 = -470341262;    int KZHlGzvrTmHDiJD1475469 = -484371658;    int KZHlGzvrTmHDiJD50227009 = -937349175;    int KZHlGzvrTmHDiJD33201611 = -97979389;    int KZHlGzvrTmHDiJD29481188 = 42687367;    int KZHlGzvrTmHDiJD32907477 = -670275763;    int KZHlGzvrTmHDiJD41581045 = -761016280;    int KZHlGzvrTmHDiJD33292471 = -958581849;    int KZHlGzvrTmHDiJD49105957 = -746670404;    int KZHlGzvrTmHDiJD78608605 = -67367914;    int KZHlGzvrTmHDiJD35950162 = -961559472;    int KZHlGzvrTmHDiJD97032233 = -554568507;    int KZHlGzvrTmHDiJD47605589 = -604452646;    int KZHlGzvrTmHDiJD78450947 = -254351952;    int KZHlGzvrTmHDiJD21600056 = -710434169;    int KZHlGzvrTmHDiJD36323706 = -356783148;    int KZHlGzvrTmHDiJD61734149 = -367313232;    int KZHlGzvrTmHDiJD58987889 = -653708233;    int KZHlGzvrTmHDiJD23580068 = -372466921;    int KZHlGzvrTmHDiJD76001473 = -18214681;    int KZHlGzvrTmHDiJD51579617 = -787636936;    int KZHlGzvrTmHDiJD69368143 = 50892013;    int KZHlGzvrTmHDiJD75973494 = -808659844;    int KZHlGzvrTmHDiJD20819811 = -589717265;    int KZHlGzvrTmHDiJD69724524 = -30691367;    int KZHlGzvrTmHDiJD98691506 = -360654356;    int KZHlGzvrTmHDiJD60237675 = -413103595;    int KZHlGzvrTmHDiJD79087231 = -597822565;    int KZHlGzvrTmHDiJD64560290 = -212267793;    int KZHlGzvrTmHDiJD23919033 = -757594749;    int KZHlGzvrTmHDiJD96730542 = -30012983;    int KZHlGzvrTmHDiJD40835429 = -791638701;    int KZHlGzvrTmHDiJD71149405 = -164869297;    int KZHlGzvrTmHDiJD254901 = -326892192;    int KZHlGzvrTmHDiJD38104344 = -477058828;    int KZHlGzvrTmHDiJD29491532 = -366507680;    int KZHlGzvrTmHDiJD3804287 = -906499862;    int KZHlGzvrTmHDiJD35928509 = -747275438;    int KZHlGzvrTmHDiJD76607993 = -532925150;    int KZHlGzvrTmHDiJD62318925 = -580201157;    int KZHlGzvrTmHDiJD58229222 = -220110340;    int KZHlGzvrTmHDiJD66065580 = -207519151;    int KZHlGzvrTmHDiJD37806565 = 618219;     KZHlGzvrTmHDiJD92615427 = KZHlGzvrTmHDiJD72816058;     KZHlGzvrTmHDiJD72816058 = KZHlGzvrTmHDiJD60704978;     KZHlGzvrTmHDiJD60704978 = KZHlGzvrTmHDiJD47022862;     KZHlGzvrTmHDiJD47022862 = KZHlGzvrTmHDiJD34368445;     KZHlGzvrTmHDiJD34368445 = KZHlGzvrTmHDiJD73991991;     KZHlGzvrTmHDiJD73991991 = KZHlGzvrTmHDiJD70004275;     KZHlGzvrTmHDiJD70004275 = KZHlGzvrTmHDiJD17638232;     KZHlGzvrTmHDiJD17638232 = KZHlGzvrTmHDiJD16378730;     KZHlGzvrTmHDiJD16378730 = KZHlGzvrTmHDiJD61485647;     KZHlGzvrTmHDiJD61485647 = KZHlGzvrTmHDiJD90539709;     KZHlGzvrTmHDiJD90539709 = KZHlGzvrTmHDiJD38432727;     KZHlGzvrTmHDiJD38432727 = KZHlGzvrTmHDiJD65392087;     KZHlGzvrTmHDiJD65392087 = KZHlGzvrTmHDiJD84985325;     KZHlGzvrTmHDiJD84985325 = KZHlGzvrTmHDiJD69738879;     KZHlGzvrTmHDiJD69738879 = KZHlGzvrTmHDiJD20979605;     KZHlGzvrTmHDiJD20979605 = KZHlGzvrTmHDiJD52567056;     KZHlGzvrTmHDiJD52567056 = KZHlGzvrTmHDiJD90355001;     KZHlGzvrTmHDiJD90355001 = KZHlGzvrTmHDiJD30864269;     KZHlGzvrTmHDiJD30864269 = KZHlGzvrTmHDiJD65077070;     KZHlGzvrTmHDiJD65077070 = KZHlGzvrTmHDiJD18806469;     KZHlGzvrTmHDiJD18806469 = KZHlGzvrTmHDiJD75811740;     KZHlGzvrTmHDiJD75811740 = KZHlGzvrTmHDiJD50937668;     KZHlGzvrTmHDiJD50937668 = KZHlGzvrTmHDiJD56280664;     KZHlGzvrTmHDiJD56280664 = KZHlGzvrTmHDiJD45547393;     KZHlGzvrTmHDiJD45547393 = KZHlGzvrTmHDiJD84141435;     KZHlGzvrTmHDiJD84141435 = KZHlGzvrTmHDiJD40790380;     KZHlGzvrTmHDiJD40790380 = KZHlGzvrTmHDiJD40523087;     KZHlGzvrTmHDiJD40523087 = KZHlGzvrTmHDiJD84730755;     KZHlGzvrTmHDiJD84730755 = KZHlGzvrTmHDiJD74797685;     KZHlGzvrTmHDiJD74797685 = KZHlGzvrTmHDiJD28193176;     KZHlGzvrTmHDiJD28193176 = KZHlGzvrTmHDiJD41433753;     KZHlGzvrTmHDiJD41433753 = KZHlGzvrTmHDiJD59824121;     KZHlGzvrTmHDiJD59824121 = KZHlGzvrTmHDiJD29441925;     KZHlGzvrTmHDiJD29441925 = KZHlGzvrTmHDiJD87953092;     KZHlGzvrTmHDiJD87953092 = KZHlGzvrTmHDiJD22133291;     KZHlGzvrTmHDiJD22133291 = KZHlGzvrTmHDiJD42528658;     KZHlGzvrTmHDiJD42528658 = KZHlGzvrTmHDiJD30967001;     KZHlGzvrTmHDiJD30967001 = KZHlGzvrTmHDiJD54031296;     KZHlGzvrTmHDiJD54031296 = KZHlGzvrTmHDiJD69130119;     KZHlGzvrTmHDiJD69130119 = KZHlGzvrTmHDiJD6089181;     KZHlGzvrTmHDiJD6089181 = KZHlGzvrTmHDiJD95226401;     KZHlGzvrTmHDiJD95226401 = KZHlGzvrTmHDiJD99810267;     KZHlGzvrTmHDiJD99810267 = KZHlGzvrTmHDiJD99358051;     KZHlGzvrTmHDiJD99358051 = KZHlGzvrTmHDiJD86912521;     KZHlGzvrTmHDiJD86912521 = KZHlGzvrTmHDiJD69573898;     KZHlGzvrTmHDiJD69573898 = KZHlGzvrTmHDiJD63321624;     KZHlGzvrTmHDiJD63321624 = KZHlGzvrTmHDiJD71065856;     KZHlGzvrTmHDiJD71065856 = KZHlGzvrTmHDiJD41831581;     KZHlGzvrTmHDiJD41831581 = KZHlGzvrTmHDiJD24493080;     KZHlGzvrTmHDiJD24493080 = KZHlGzvrTmHDiJD95710453;     KZHlGzvrTmHDiJD95710453 = KZHlGzvrTmHDiJD63632885;     KZHlGzvrTmHDiJD63632885 = KZHlGzvrTmHDiJD17514720;     KZHlGzvrTmHDiJD17514720 = KZHlGzvrTmHDiJD63093579;     KZHlGzvrTmHDiJD63093579 = KZHlGzvrTmHDiJD88606496;     KZHlGzvrTmHDiJD88606496 = KZHlGzvrTmHDiJD16803688;     KZHlGzvrTmHDiJD16803688 = KZHlGzvrTmHDiJD21878390;     KZHlGzvrTmHDiJD21878390 = KZHlGzvrTmHDiJD4424314;     KZHlGzvrTmHDiJD4424314 = KZHlGzvrTmHDiJD1475469;     KZHlGzvrTmHDiJD1475469 = KZHlGzvrTmHDiJD50227009;     KZHlGzvrTmHDiJD50227009 = KZHlGzvrTmHDiJD33201611;     KZHlGzvrTmHDiJD33201611 = KZHlGzvrTmHDiJD29481188;     KZHlGzvrTmHDiJD29481188 = KZHlGzvrTmHDiJD32907477;     KZHlGzvrTmHDiJD32907477 = KZHlGzvrTmHDiJD41581045;     KZHlGzvrTmHDiJD41581045 = KZHlGzvrTmHDiJD33292471;     KZHlGzvrTmHDiJD33292471 = KZHlGzvrTmHDiJD49105957;     KZHlGzvrTmHDiJD49105957 = KZHlGzvrTmHDiJD78608605;     KZHlGzvrTmHDiJD78608605 = KZHlGzvrTmHDiJD35950162;     KZHlGzvrTmHDiJD35950162 = KZHlGzvrTmHDiJD97032233;     KZHlGzvrTmHDiJD97032233 = KZHlGzvrTmHDiJD47605589;     KZHlGzvrTmHDiJD47605589 = KZHlGzvrTmHDiJD78450947;     KZHlGzvrTmHDiJD78450947 = KZHlGzvrTmHDiJD21600056;     KZHlGzvrTmHDiJD21600056 = KZHlGzvrTmHDiJD36323706;     KZHlGzvrTmHDiJD36323706 = KZHlGzvrTmHDiJD61734149;     KZHlGzvrTmHDiJD61734149 = KZHlGzvrTmHDiJD58987889;     KZHlGzvrTmHDiJD58987889 = KZHlGzvrTmHDiJD23580068;     KZHlGzvrTmHDiJD23580068 = KZHlGzvrTmHDiJD76001473;     KZHlGzvrTmHDiJD76001473 = KZHlGzvrTmHDiJD51579617;     KZHlGzvrTmHDiJD51579617 = KZHlGzvrTmHDiJD69368143;     KZHlGzvrTmHDiJD69368143 = KZHlGzvrTmHDiJD75973494;     KZHlGzvrTmHDiJD75973494 = KZHlGzvrTmHDiJD20819811;     KZHlGzvrTmHDiJD20819811 = KZHlGzvrTmHDiJD69724524;     KZHlGzvrTmHDiJD69724524 = KZHlGzvrTmHDiJD98691506;     KZHlGzvrTmHDiJD98691506 = KZHlGzvrTmHDiJD60237675;     KZHlGzvrTmHDiJD60237675 = KZHlGzvrTmHDiJD79087231;     KZHlGzvrTmHDiJD79087231 = KZHlGzvrTmHDiJD64560290;     KZHlGzvrTmHDiJD64560290 = KZHlGzvrTmHDiJD23919033;     KZHlGzvrTmHDiJD23919033 = KZHlGzvrTmHDiJD96730542;     KZHlGzvrTmHDiJD96730542 = KZHlGzvrTmHDiJD40835429;     KZHlGzvrTmHDiJD40835429 = KZHlGzvrTmHDiJD71149405;     KZHlGzvrTmHDiJD71149405 = KZHlGzvrTmHDiJD254901;     KZHlGzvrTmHDiJD254901 = KZHlGzvrTmHDiJD38104344;     KZHlGzvrTmHDiJD38104344 = KZHlGzvrTmHDiJD29491532;     KZHlGzvrTmHDiJD29491532 = KZHlGzvrTmHDiJD3804287;     KZHlGzvrTmHDiJD3804287 = KZHlGzvrTmHDiJD35928509;     KZHlGzvrTmHDiJD35928509 = KZHlGzvrTmHDiJD76607993;     KZHlGzvrTmHDiJD76607993 = KZHlGzvrTmHDiJD62318925;     KZHlGzvrTmHDiJD62318925 = KZHlGzvrTmHDiJD58229222;     KZHlGzvrTmHDiJD58229222 = KZHlGzvrTmHDiJD66065580;     KZHlGzvrTmHDiJD66065580 = KZHlGzvrTmHDiJD37806565;     KZHlGzvrTmHDiJD37806565 = KZHlGzvrTmHDiJD92615427;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void RBXhGLCFsrTEKHEo78191207() {     int GUdNCZnIWsfmOFy23139169 = -801905746;    int GUdNCZnIWsfmOFy93007589 = -206279824;    int GUdNCZnIWsfmOFy20509502 = -205412302;    int GUdNCZnIWsfmOFy9725540 = -197615865;    int GUdNCZnIWsfmOFy44972056 = 97291725;    int GUdNCZnIWsfmOFy91906707 = 81408129;    int GUdNCZnIWsfmOFy35856567 = -711963376;    int GUdNCZnIWsfmOFy88905561 = -472827401;    int GUdNCZnIWsfmOFy9117138 = -143703626;    int GUdNCZnIWsfmOFy79356316 = -650131826;    int GUdNCZnIWsfmOFy61717202 = -617525436;    int GUdNCZnIWsfmOFy52802710 = -446758751;    int GUdNCZnIWsfmOFy64836570 = 57902066;    int GUdNCZnIWsfmOFy18874561 = -58633873;    int GUdNCZnIWsfmOFy15677555 = -331828089;    int GUdNCZnIWsfmOFy22969640 = -529554671;    int GUdNCZnIWsfmOFy16420892 = -814057037;    int GUdNCZnIWsfmOFy70042916 = -671216993;    int GUdNCZnIWsfmOFy31764727 = -157841132;    int GUdNCZnIWsfmOFy24557972 = -841440123;    int GUdNCZnIWsfmOFy5937178 = -304814481;    int GUdNCZnIWsfmOFy83699989 = 49972059;    int GUdNCZnIWsfmOFy28862658 = 45139381;    int GUdNCZnIWsfmOFy43121631 = -379895549;    int GUdNCZnIWsfmOFy44897266 = -993209640;    int GUdNCZnIWsfmOFy25091570 = -205649557;    int GUdNCZnIWsfmOFy19639226 = -697654946;    int GUdNCZnIWsfmOFy33985819 = -689761846;    int GUdNCZnIWsfmOFy8833857 = -134814706;    int GUdNCZnIWsfmOFy60203168 = -696671661;    int GUdNCZnIWsfmOFy40154939 = -394667885;    int GUdNCZnIWsfmOFy82759264 = -179933426;    int GUdNCZnIWsfmOFy5163547 = -581568093;    int GUdNCZnIWsfmOFy71038964 = 80928394;    int GUdNCZnIWsfmOFy970304 = -326349378;    int GUdNCZnIWsfmOFy39602591 = -43206745;    int GUdNCZnIWsfmOFy64071917 = -370090738;    int GUdNCZnIWsfmOFy26881626 = -867748871;    int GUdNCZnIWsfmOFy80859966 = -445285981;    int GUdNCZnIWsfmOFy12689500 = -185513338;    int GUdNCZnIWsfmOFy49426708 = 84618640;    int GUdNCZnIWsfmOFy65489345 = -253820796;    int GUdNCZnIWsfmOFy79031226 = -182237640;    int GUdNCZnIWsfmOFy15012385 = -263233894;    int GUdNCZnIWsfmOFy61906676 = -705632191;    int GUdNCZnIWsfmOFy65647732 = -723207006;    int GUdNCZnIWsfmOFy65953985 = -4377555;    int GUdNCZnIWsfmOFy90139502 = -158779251;    int GUdNCZnIWsfmOFy49018976 = -997397622;    int GUdNCZnIWsfmOFy58651516 = 74196874;    int GUdNCZnIWsfmOFy87849096 = -662827401;    int GUdNCZnIWsfmOFy15368097 = -389188968;    int GUdNCZnIWsfmOFy67000009 = -410869043;    int GUdNCZnIWsfmOFy52976389 = -324638784;    int GUdNCZnIWsfmOFy72254030 = -782749408;    int GUdNCZnIWsfmOFy39439179 = -751877805;    int GUdNCZnIWsfmOFy64144932 = -151419205;    int GUdNCZnIWsfmOFy77387870 = -825516753;    int GUdNCZnIWsfmOFy64828273 = -204406226;    int GUdNCZnIWsfmOFy19880486 = -697058719;    int GUdNCZnIWsfmOFy72267481 = -220936926;    int GUdNCZnIWsfmOFy1870749 = 77798469;    int GUdNCZnIWsfmOFy80071704 = -238012696;    int GUdNCZnIWsfmOFy48913969 = -447031966;    int GUdNCZnIWsfmOFy39201377 = -155463942;    int GUdNCZnIWsfmOFy78957938 = -337592010;    int GUdNCZnIWsfmOFy47639163 = -865190659;    int GUdNCZnIWsfmOFy93797605 = 76973671;    int GUdNCZnIWsfmOFy17904257 = -732284496;    int GUdNCZnIWsfmOFy76074964 = -188621345;    int GUdNCZnIWsfmOFy58897723 = -59463933;    int GUdNCZnIWsfmOFy89539266 = -946308166;    int GUdNCZnIWsfmOFy89182949 = -125931012;    int GUdNCZnIWsfmOFy19075227 = -972327794;    int GUdNCZnIWsfmOFy75131264 = -826058763;    int GUdNCZnIWsfmOFy40447832 = 49006315;    int GUdNCZnIWsfmOFy4668764 = -767790302;    int GUdNCZnIWsfmOFy13850273 = -691626726;    int GUdNCZnIWsfmOFy81214955 = -674263359;    int GUdNCZnIWsfmOFy79249534 = -170002634;    int GUdNCZnIWsfmOFy59137585 = -101272002;    int GUdNCZnIWsfmOFy29499724 = -438875695;    int GUdNCZnIWsfmOFy84966842 = -692364224;    int GUdNCZnIWsfmOFy50182340 = -109011580;    int GUdNCZnIWsfmOFy72354072 = 66155740;    int GUdNCZnIWsfmOFy24786843 = 94521082;    int GUdNCZnIWsfmOFy15759255 = -769064383;    int GUdNCZnIWsfmOFy52187158 = -156929309;    int GUdNCZnIWsfmOFy98784934 = -136322198;    int GUdNCZnIWsfmOFy61531125 = -574471573;    int GUdNCZnIWsfmOFy75457658 = -891787540;    int GUdNCZnIWsfmOFy86684046 = -544573985;    int GUdNCZnIWsfmOFy62053352 = -563342646;    int GUdNCZnIWsfmOFy60979481 = -748227263;    int GUdNCZnIWsfmOFy40422019 = -964576413;    int GUdNCZnIWsfmOFy47555960 = -993179830;    int GUdNCZnIWsfmOFy85417640 = 84191899;    int GUdNCZnIWsfmOFy30117257 = -735205674;    int GUdNCZnIWsfmOFy75811008 = -7769952;    int GUdNCZnIWsfmOFy82948737 = -801905746;     GUdNCZnIWsfmOFy23139169 = GUdNCZnIWsfmOFy93007589;     GUdNCZnIWsfmOFy93007589 = GUdNCZnIWsfmOFy20509502;     GUdNCZnIWsfmOFy20509502 = GUdNCZnIWsfmOFy9725540;     GUdNCZnIWsfmOFy9725540 = GUdNCZnIWsfmOFy44972056;     GUdNCZnIWsfmOFy44972056 = GUdNCZnIWsfmOFy91906707;     GUdNCZnIWsfmOFy91906707 = GUdNCZnIWsfmOFy35856567;     GUdNCZnIWsfmOFy35856567 = GUdNCZnIWsfmOFy88905561;     GUdNCZnIWsfmOFy88905561 = GUdNCZnIWsfmOFy9117138;     GUdNCZnIWsfmOFy9117138 = GUdNCZnIWsfmOFy79356316;     GUdNCZnIWsfmOFy79356316 = GUdNCZnIWsfmOFy61717202;     GUdNCZnIWsfmOFy61717202 = GUdNCZnIWsfmOFy52802710;     GUdNCZnIWsfmOFy52802710 = GUdNCZnIWsfmOFy64836570;     GUdNCZnIWsfmOFy64836570 = GUdNCZnIWsfmOFy18874561;     GUdNCZnIWsfmOFy18874561 = GUdNCZnIWsfmOFy15677555;     GUdNCZnIWsfmOFy15677555 = GUdNCZnIWsfmOFy22969640;     GUdNCZnIWsfmOFy22969640 = GUdNCZnIWsfmOFy16420892;     GUdNCZnIWsfmOFy16420892 = GUdNCZnIWsfmOFy70042916;     GUdNCZnIWsfmOFy70042916 = GUdNCZnIWsfmOFy31764727;     GUdNCZnIWsfmOFy31764727 = GUdNCZnIWsfmOFy24557972;     GUdNCZnIWsfmOFy24557972 = GUdNCZnIWsfmOFy5937178;     GUdNCZnIWsfmOFy5937178 = GUdNCZnIWsfmOFy83699989;     GUdNCZnIWsfmOFy83699989 = GUdNCZnIWsfmOFy28862658;     GUdNCZnIWsfmOFy28862658 = GUdNCZnIWsfmOFy43121631;     GUdNCZnIWsfmOFy43121631 = GUdNCZnIWsfmOFy44897266;     GUdNCZnIWsfmOFy44897266 = GUdNCZnIWsfmOFy25091570;     GUdNCZnIWsfmOFy25091570 = GUdNCZnIWsfmOFy19639226;     GUdNCZnIWsfmOFy19639226 = GUdNCZnIWsfmOFy33985819;     GUdNCZnIWsfmOFy33985819 = GUdNCZnIWsfmOFy8833857;     GUdNCZnIWsfmOFy8833857 = GUdNCZnIWsfmOFy60203168;     GUdNCZnIWsfmOFy60203168 = GUdNCZnIWsfmOFy40154939;     GUdNCZnIWsfmOFy40154939 = GUdNCZnIWsfmOFy82759264;     GUdNCZnIWsfmOFy82759264 = GUdNCZnIWsfmOFy5163547;     GUdNCZnIWsfmOFy5163547 = GUdNCZnIWsfmOFy71038964;     GUdNCZnIWsfmOFy71038964 = GUdNCZnIWsfmOFy970304;     GUdNCZnIWsfmOFy970304 = GUdNCZnIWsfmOFy39602591;     GUdNCZnIWsfmOFy39602591 = GUdNCZnIWsfmOFy64071917;     GUdNCZnIWsfmOFy64071917 = GUdNCZnIWsfmOFy26881626;     GUdNCZnIWsfmOFy26881626 = GUdNCZnIWsfmOFy80859966;     GUdNCZnIWsfmOFy80859966 = GUdNCZnIWsfmOFy12689500;     GUdNCZnIWsfmOFy12689500 = GUdNCZnIWsfmOFy49426708;     GUdNCZnIWsfmOFy49426708 = GUdNCZnIWsfmOFy65489345;     GUdNCZnIWsfmOFy65489345 = GUdNCZnIWsfmOFy79031226;     GUdNCZnIWsfmOFy79031226 = GUdNCZnIWsfmOFy15012385;     GUdNCZnIWsfmOFy15012385 = GUdNCZnIWsfmOFy61906676;     GUdNCZnIWsfmOFy61906676 = GUdNCZnIWsfmOFy65647732;     GUdNCZnIWsfmOFy65647732 = GUdNCZnIWsfmOFy65953985;     GUdNCZnIWsfmOFy65953985 = GUdNCZnIWsfmOFy90139502;     GUdNCZnIWsfmOFy90139502 = GUdNCZnIWsfmOFy49018976;     GUdNCZnIWsfmOFy49018976 = GUdNCZnIWsfmOFy58651516;     GUdNCZnIWsfmOFy58651516 = GUdNCZnIWsfmOFy87849096;     GUdNCZnIWsfmOFy87849096 = GUdNCZnIWsfmOFy15368097;     GUdNCZnIWsfmOFy15368097 = GUdNCZnIWsfmOFy67000009;     GUdNCZnIWsfmOFy67000009 = GUdNCZnIWsfmOFy52976389;     GUdNCZnIWsfmOFy52976389 = GUdNCZnIWsfmOFy72254030;     GUdNCZnIWsfmOFy72254030 = GUdNCZnIWsfmOFy39439179;     GUdNCZnIWsfmOFy39439179 = GUdNCZnIWsfmOFy64144932;     GUdNCZnIWsfmOFy64144932 = GUdNCZnIWsfmOFy77387870;     GUdNCZnIWsfmOFy77387870 = GUdNCZnIWsfmOFy64828273;     GUdNCZnIWsfmOFy64828273 = GUdNCZnIWsfmOFy19880486;     GUdNCZnIWsfmOFy19880486 = GUdNCZnIWsfmOFy72267481;     GUdNCZnIWsfmOFy72267481 = GUdNCZnIWsfmOFy1870749;     GUdNCZnIWsfmOFy1870749 = GUdNCZnIWsfmOFy80071704;     GUdNCZnIWsfmOFy80071704 = GUdNCZnIWsfmOFy48913969;     GUdNCZnIWsfmOFy48913969 = GUdNCZnIWsfmOFy39201377;     GUdNCZnIWsfmOFy39201377 = GUdNCZnIWsfmOFy78957938;     GUdNCZnIWsfmOFy78957938 = GUdNCZnIWsfmOFy47639163;     GUdNCZnIWsfmOFy47639163 = GUdNCZnIWsfmOFy93797605;     GUdNCZnIWsfmOFy93797605 = GUdNCZnIWsfmOFy17904257;     GUdNCZnIWsfmOFy17904257 = GUdNCZnIWsfmOFy76074964;     GUdNCZnIWsfmOFy76074964 = GUdNCZnIWsfmOFy58897723;     GUdNCZnIWsfmOFy58897723 = GUdNCZnIWsfmOFy89539266;     GUdNCZnIWsfmOFy89539266 = GUdNCZnIWsfmOFy89182949;     GUdNCZnIWsfmOFy89182949 = GUdNCZnIWsfmOFy19075227;     GUdNCZnIWsfmOFy19075227 = GUdNCZnIWsfmOFy75131264;     GUdNCZnIWsfmOFy75131264 = GUdNCZnIWsfmOFy40447832;     GUdNCZnIWsfmOFy40447832 = GUdNCZnIWsfmOFy4668764;     GUdNCZnIWsfmOFy4668764 = GUdNCZnIWsfmOFy13850273;     GUdNCZnIWsfmOFy13850273 = GUdNCZnIWsfmOFy81214955;     GUdNCZnIWsfmOFy81214955 = GUdNCZnIWsfmOFy79249534;     GUdNCZnIWsfmOFy79249534 = GUdNCZnIWsfmOFy59137585;     GUdNCZnIWsfmOFy59137585 = GUdNCZnIWsfmOFy29499724;     GUdNCZnIWsfmOFy29499724 = GUdNCZnIWsfmOFy84966842;     GUdNCZnIWsfmOFy84966842 = GUdNCZnIWsfmOFy50182340;     GUdNCZnIWsfmOFy50182340 = GUdNCZnIWsfmOFy72354072;     GUdNCZnIWsfmOFy72354072 = GUdNCZnIWsfmOFy24786843;     GUdNCZnIWsfmOFy24786843 = GUdNCZnIWsfmOFy15759255;     GUdNCZnIWsfmOFy15759255 = GUdNCZnIWsfmOFy52187158;     GUdNCZnIWsfmOFy52187158 = GUdNCZnIWsfmOFy98784934;     GUdNCZnIWsfmOFy98784934 = GUdNCZnIWsfmOFy61531125;     GUdNCZnIWsfmOFy61531125 = GUdNCZnIWsfmOFy75457658;     GUdNCZnIWsfmOFy75457658 = GUdNCZnIWsfmOFy86684046;     GUdNCZnIWsfmOFy86684046 = GUdNCZnIWsfmOFy62053352;     GUdNCZnIWsfmOFy62053352 = GUdNCZnIWsfmOFy60979481;     GUdNCZnIWsfmOFy60979481 = GUdNCZnIWsfmOFy40422019;     GUdNCZnIWsfmOFy40422019 = GUdNCZnIWsfmOFy47555960;     GUdNCZnIWsfmOFy47555960 = GUdNCZnIWsfmOFy85417640;     GUdNCZnIWsfmOFy85417640 = GUdNCZnIWsfmOFy30117257;     GUdNCZnIWsfmOFy30117257 = GUdNCZnIWsfmOFy75811008;     GUdNCZnIWsfmOFy75811008 = GUdNCZnIWsfmOFy82948737;     GUdNCZnIWsfmOFy82948737 = GUdNCZnIWsfmOFy23139169;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void CGczmsJDNyHVzAsI84849951() {     int oYdZmPGizNNnVRR23132697 = -674429887;    int oYdZmPGizNNnVRR31579907 = -248874949;    int oYdZmPGizNNnVRR21822714 = -162540915;    int oYdZmPGizNNnVRR94805399 = -18156695;    int oYdZmPGizNNnVRR53812158 = -371063597;    int oYdZmPGizNNnVRR26712140 = -718024683;    int oYdZmPGizNNnVRR4403903 = -416499843;    int oYdZmPGizNNnVRR58119612 = -177004718;    int oYdZmPGizNNnVRR91857301 = -957349696;    int oYdZmPGizNNnVRR75995829 = -286938698;    int oYdZmPGizNNnVRR82037807 = -576997508;    int oYdZmPGizNNnVRR67094387 = -182237114;    int oYdZmPGizNNnVRR96689484 = 36243591;    int oYdZmPGizNNnVRR72718781 = -403786925;    int oYdZmPGizNNnVRR60849663 = -294540941;    int oYdZmPGizNNnVRR97534187 = -862350321;    int oYdZmPGizNNnVRR72273651 = -633671607;    int oYdZmPGizNNnVRR93043449 = 31711385;    int oYdZmPGizNNnVRR52192997 = -775823398;    int oYdZmPGizNNnVRR96631735 = -353464574;    int oYdZmPGizNNnVRR85606601 = -15280855;    int oYdZmPGizNNnVRR61996255 = -460019825;    int oYdZmPGizNNnVRR65741054 = -413818461;    int oYdZmPGizNNnVRR86771739 = -260816072;    int oYdZmPGizNNnVRR73339955 = -319903493;    int oYdZmPGizNNnVRR41665647 = -113855486;    int oYdZmPGizNNnVRR89860807 = -998454659;    int oYdZmPGizNNnVRR69389343 = -198728834;    int oYdZmPGizNNnVRR21512304 = -780885021;    int oYdZmPGizNNnVRR36293039 = -372103034;    int oYdZmPGizNNnVRR73567618 = -781761119;    int oYdZmPGizNNnVRR67483121 = -571755188;    int oYdZmPGizNNnVRR90615969 = -783512309;    int oYdZmPGizNNnVRR71321802 = -895753338;    int oYdZmPGizNNnVRR83328341 = -585663302;    int oYdZmPGizNNnVRR82430780 = -122972491;    int oYdZmPGizNNnVRR76013284 = -469744116;    int oYdZmPGizNNnVRR96799305 = -863011550;    int oYdZmPGizNNnVRR16173564 = -57967397;    int oYdZmPGizNNnVRR84562392 = -894945024;    int oYdZmPGizNNnVRR37282376 = -3325258;    int oYdZmPGizNNnVRR51187006 = -447612879;    int oYdZmPGizNNnVRR39314586 = -741367894;    int oYdZmPGizNNnVRR74244283 = -920198337;    int oYdZmPGizNNnVRR45924728 = -78891592;    int oYdZmPGizNNnVRR6075700 = -535251677;    int oYdZmPGizNNnVRR93651777 = -485658178;    int oYdZmPGizNNnVRR65523083 = -767540607;    int oYdZmPGizNNnVRR67677366 = -561161484;    int oYdZmPGizNNnVRR78696939 = -612828013;    int oYdZmPGizNNnVRR1108467 = -241382409;    int oYdZmPGizNNnVRR29915355 = -771334716;    int oYdZmPGizNNnVRR42448357 = -302923052;    int oYdZmPGizNNnVRR63226659 = -856558636;    int oYdZmPGizNNnVRR99243350 = -539648686;    int oYdZmPGizNNnVRR61136441 = -114410063;    int oYdZmPGizNNnVRR65838852 = -835056488;    int oYdZmPGizNNnVRR35050974 = -901724844;    int oYdZmPGizNNnVRR21465445 = -698253203;    int oYdZmPGizNNnVRR12146511 = -157208112;    int oYdZmPGizNNnVRR36851332 = -719570025;    int oYdZmPGizNNnVRR35014559 = -117771009;    int oYdZmPGizNNnVRR36607309 = -396119698;    int oYdZmPGizNNnVRR55564263 = -485246662;    int oYdZmPGizNNnVRR2428212 = -505177579;    int oYdZmPGizNNnVRR14554687 = 94757680;    int oYdZmPGizNNnVRR76478417 = -398724806;    int oYdZmPGizNNnVRR25367683 = -68003071;    int oYdZmPGizNNnVRR89390439 = -818123623;    int oYdZmPGizNNnVRR78418883 = -71568450;    int oYdZmPGizNNnVRR21520903 = -292606206;    int oYdZmPGizNNnVRR75474346 = -770660057;    int oYdZmPGizNNnVRR76869885 = -910321218;    int oYdZmPGizNNnVRR67630604 = -880878374;    int oYdZmPGizNNnVRR59349359 = -250139316;    int oYdZmPGizNNnVRR34419596 = -567667977;    int oYdZmPGizNNnVRR22681670 = -718651931;    int oYdZmPGizNNnVRR91496771 = -493620125;    int oYdZmPGizNNnVRR40847011 = -81924480;    int oYdZmPGizNNnVRR67264255 = -784651817;    int oYdZmPGizNNnVRR48013869 = -628197309;    int oYdZmPGizNNnVRR24337725 = -130914052;    int oYdZmPGizNNnVRR1711978 = -637567351;    int oYdZmPGizNNnVRR42815364 = -68057009;    int oYdZmPGizNNnVRR35184572 = -30720626;    int oYdZmPGizNNnVRR43652264 = 89573596;    int oYdZmPGizNNnVRR25034764 = -168832137;    int oYdZmPGizNNnVRR27389310 = -926953674;    int oYdZmPGizNNnVRR72078451 = -256104653;    int oYdZmPGizNNnVRR22191900 = -371253240;    int oYdZmPGizNNnVRR16591929 = -287916003;    int oYdZmPGizNNnVRR40962311 = -568019272;    int oYdZmPGizNNnVRR75333861 = -64758348;    int oYdZmPGizNNnVRR4027053 = -900759286;    int oYdZmPGizNNnVRR47711061 = -75375000;    int oYdZmPGizNNnVRR2267817 = -885554250;    int oYdZmPGizNNnVRR14579698 = 48506819;    int oYdZmPGizNNnVRR83750322 = -156121232;    int oYdZmPGizNNnVRR71816072 = -315020759;    int oYdZmPGizNNnVRR31370042 = -674429887;     oYdZmPGizNNnVRR23132697 = oYdZmPGizNNnVRR31579907;     oYdZmPGizNNnVRR31579907 = oYdZmPGizNNnVRR21822714;     oYdZmPGizNNnVRR21822714 = oYdZmPGizNNnVRR94805399;     oYdZmPGizNNnVRR94805399 = oYdZmPGizNNnVRR53812158;     oYdZmPGizNNnVRR53812158 = oYdZmPGizNNnVRR26712140;     oYdZmPGizNNnVRR26712140 = oYdZmPGizNNnVRR4403903;     oYdZmPGizNNnVRR4403903 = oYdZmPGizNNnVRR58119612;     oYdZmPGizNNnVRR58119612 = oYdZmPGizNNnVRR91857301;     oYdZmPGizNNnVRR91857301 = oYdZmPGizNNnVRR75995829;     oYdZmPGizNNnVRR75995829 = oYdZmPGizNNnVRR82037807;     oYdZmPGizNNnVRR82037807 = oYdZmPGizNNnVRR67094387;     oYdZmPGizNNnVRR67094387 = oYdZmPGizNNnVRR96689484;     oYdZmPGizNNnVRR96689484 = oYdZmPGizNNnVRR72718781;     oYdZmPGizNNnVRR72718781 = oYdZmPGizNNnVRR60849663;     oYdZmPGizNNnVRR60849663 = oYdZmPGizNNnVRR97534187;     oYdZmPGizNNnVRR97534187 = oYdZmPGizNNnVRR72273651;     oYdZmPGizNNnVRR72273651 = oYdZmPGizNNnVRR93043449;     oYdZmPGizNNnVRR93043449 = oYdZmPGizNNnVRR52192997;     oYdZmPGizNNnVRR52192997 = oYdZmPGizNNnVRR96631735;     oYdZmPGizNNnVRR96631735 = oYdZmPGizNNnVRR85606601;     oYdZmPGizNNnVRR85606601 = oYdZmPGizNNnVRR61996255;     oYdZmPGizNNnVRR61996255 = oYdZmPGizNNnVRR65741054;     oYdZmPGizNNnVRR65741054 = oYdZmPGizNNnVRR86771739;     oYdZmPGizNNnVRR86771739 = oYdZmPGizNNnVRR73339955;     oYdZmPGizNNnVRR73339955 = oYdZmPGizNNnVRR41665647;     oYdZmPGizNNnVRR41665647 = oYdZmPGizNNnVRR89860807;     oYdZmPGizNNnVRR89860807 = oYdZmPGizNNnVRR69389343;     oYdZmPGizNNnVRR69389343 = oYdZmPGizNNnVRR21512304;     oYdZmPGizNNnVRR21512304 = oYdZmPGizNNnVRR36293039;     oYdZmPGizNNnVRR36293039 = oYdZmPGizNNnVRR73567618;     oYdZmPGizNNnVRR73567618 = oYdZmPGizNNnVRR67483121;     oYdZmPGizNNnVRR67483121 = oYdZmPGizNNnVRR90615969;     oYdZmPGizNNnVRR90615969 = oYdZmPGizNNnVRR71321802;     oYdZmPGizNNnVRR71321802 = oYdZmPGizNNnVRR83328341;     oYdZmPGizNNnVRR83328341 = oYdZmPGizNNnVRR82430780;     oYdZmPGizNNnVRR82430780 = oYdZmPGizNNnVRR76013284;     oYdZmPGizNNnVRR76013284 = oYdZmPGizNNnVRR96799305;     oYdZmPGizNNnVRR96799305 = oYdZmPGizNNnVRR16173564;     oYdZmPGizNNnVRR16173564 = oYdZmPGizNNnVRR84562392;     oYdZmPGizNNnVRR84562392 = oYdZmPGizNNnVRR37282376;     oYdZmPGizNNnVRR37282376 = oYdZmPGizNNnVRR51187006;     oYdZmPGizNNnVRR51187006 = oYdZmPGizNNnVRR39314586;     oYdZmPGizNNnVRR39314586 = oYdZmPGizNNnVRR74244283;     oYdZmPGizNNnVRR74244283 = oYdZmPGizNNnVRR45924728;     oYdZmPGizNNnVRR45924728 = oYdZmPGizNNnVRR6075700;     oYdZmPGizNNnVRR6075700 = oYdZmPGizNNnVRR93651777;     oYdZmPGizNNnVRR93651777 = oYdZmPGizNNnVRR65523083;     oYdZmPGizNNnVRR65523083 = oYdZmPGizNNnVRR67677366;     oYdZmPGizNNnVRR67677366 = oYdZmPGizNNnVRR78696939;     oYdZmPGizNNnVRR78696939 = oYdZmPGizNNnVRR1108467;     oYdZmPGizNNnVRR1108467 = oYdZmPGizNNnVRR29915355;     oYdZmPGizNNnVRR29915355 = oYdZmPGizNNnVRR42448357;     oYdZmPGizNNnVRR42448357 = oYdZmPGizNNnVRR63226659;     oYdZmPGizNNnVRR63226659 = oYdZmPGizNNnVRR99243350;     oYdZmPGizNNnVRR99243350 = oYdZmPGizNNnVRR61136441;     oYdZmPGizNNnVRR61136441 = oYdZmPGizNNnVRR65838852;     oYdZmPGizNNnVRR65838852 = oYdZmPGizNNnVRR35050974;     oYdZmPGizNNnVRR35050974 = oYdZmPGizNNnVRR21465445;     oYdZmPGizNNnVRR21465445 = oYdZmPGizNNnVRR12146511;     oYdZmPGizNNnVRR12146511 = oYdZmPGizNNnVRR36851332;     oYdZmPGizNNnVRR36851332 = oYdZmPGizNNnVRR35014559;     oYdZmPGizNNnVRR35014559 = oYdZmPGizNNnVRR36607309;     oYdZmPGizNNnVRR36607309 = oYdZmPGizNNnVRR55564263;     oYdZmPGizNNnVRR55564263 = oYdZmPGizNNnVRR2428212;     oYdZmPGizNNnVRR2428212 = oYdZmPGizNNnVRR14554687;     oYdZmPGizNNnVRR14554687 = oYdZmPGizNNnVRR76478417;     oYdZmPGizNNnVRR76478417 = oYdZmPGizNNnVRR25367683;     oYdZmPGizNNnVRR25367683 = oYdZmPGizNNnVRR89390439;     oYdZmPGizNNnVRR89390439 = oYdZmPGizNNnVRR78418883;     oYdZmPGizNNnVRR78418883 = oYdZmPGizNNnVRR21520903;     oYdZmPGizNNnVRR21520903 = oYdZmPGizNNnVRR75474346;     oYdZmPGizNNnVRR75474346 = oYdZmPGizNNnVRR76869885;     oYdZmPGizNNnVRR76869885 = oYdZmPGizNNnVRR67630604;     oYdZmPGizNNnVRR67630604 = oYdZmPGizNNnVRR59349359;     oYdZmPGizNNnVRR59349359 = oYdZmPGizNNnVRR34419596;     oYdZmPGizNNnVRR34419596 = oYdZmPGizNNnVRR22681670;     oYdZmPGizNNnVRR22681670 = oYdZmPGizNNnVRR91496771;     oYdZmPGizNNnVRR91496771 = oYdZmPGizNNnVRR40847011;     oYdZmPGizNNnVRR40847011 = oYdZmPGizNNnVRR67264255;     oYdZmPGizNNnVRR67264255 = oYdZmPGizNNnVRR48013869;     oYdZmPGizNNnVRR48013869 = oYdZmPGizNNnVRR24337725;     oYdZmPGizNNnVRR24337725 = oYdZmPGizNNnVRR1711978;     oYdZmPGizNNnVRR1711978 = oYdZmPGizNNnVRR42815364;     oYdZmPGizNNnVRR42815364 = oYdZmPGizNNnVRR35184572;     oYdZmPGizNNnVRR35184572 = oYdZmPGizNNnVRR43652264;     oYdZmPGizNNnVRR43652264 = oYdZmPGizNNnVRR25034764;     oYdZmPGizNNnVRR25034764 = oYdZmPGizNNnVRR27389310;     oYdZmPGizNNnVRR27389310 = oYdZmPGizNNnVRR72078451;     oYdZmPGizNNnVRR72078451 = oYdZmPGizNNnVRR22191900;     oYdZmPGizNNnVRR22191900 = oYdZmPGizNNnVRR16591929;     oYdZmPGizNNnVRR16591929 = oYdZmPGizNNnVRR40962311;     oYdZmPGizNNnVRR40962311 = oYdZmPGizNNnVRR75333861;     oYdZmPGizNNnVRR75333861 = oYdZmPGizNNnVRR4027053;     oYdZmPGizNNnVRR4027053 = oYdZmPGizNNnVRR47711061;     oYdZmPGizNNnVRR47711061 = oYdZmPGizNNnVRR2267817;     oYdZmPGizNNnVRR2267817 = oYdZmPGizNNnVRR14579698;     oYdZmPGizNNnVRR14579698 = oYdZmPGizNNnVRR83750322;     oYdZmPGizNNnVRR83750322 = oYdZmPGizNNnVRR71816072;     oYdZmPGizNNnVRR71816072 = oYdZmPGizNNnVRR31370042;     oYdZmPGizNNnVRR31370042 = oYdZmPGizNNnVRR23132697;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void BNfnbndrwjYAmFXV39266165() {     int SfstUYVpTuiPJeO93784364 = -192579018;    int SfstUYVpTuiPJeO253424 = -120518248;    int SfstUYVpTuiPJeO1791633 = -604835375;    int SfstUYVpTuiPJeO34736687 = -208295084;    int SfstUYVpTuiPJeO75262479 = -154469944;    int SfstUYVpTuiPJeO16711902 = -419605834;    int SfstUYVpTuiPJeO22782798 = -415275553;    int SfstUYVpTuiPJeO14961993 = -523821808;    int SfstUYVpTuiPJeO37722356 = 49302975;    int SfstUYVpTuiPJeO98962290 = -588034048;    int SfstUYVpTuiPJeO24286969 = -611899437;    int SfstUYVpTuiPJeO12640958 = -498000907;    int SfstUYVpTuiPJeO76516871 = -973293467;    int SfstUYVpTuiPJeO56565814 = -204815135;    int SfstUYVpTuiPJeO43569683 = -444290944;    int SfstUYVpTuiPJeO30062828 = -432333908;    int SfstUYVpTuiPJeO66126478 = -542856737;    int SfstUYVpTuiPJeO13336944 = 42545189;    int SfstUYVpTuiPJeO33900778 = -383011340;    int SfstUYVpTuiPJeO55418444 = -895830879;    int SfstUYVpTuiPJeO40742356 = 76040005;    int SfstUYVpTuiPJeO23392021 = -791305130;    int SfstUYVpTuiPJeO67684863 = -981661606;    int SfstUYVpTuiPJeO20621277 = -953216978;    int SfstUYVpTuiPJeO37464344 = 4191819;    int SfstUYVpTuiPJeO72263227 = -212139930;    int SfstUYVpTuiPJeO98121593 = -293632434;    int SfstUYVpTuiPJeO77171569 = -703852602;    int SfstUYVpTuiPJeO53654791 = 24756762;    int SfstUYVpTuiPJeO75465134 = -930498105;    int SfstUYVpTuiPJeO80639614 = -530194805;    int SfstUYVpTuiPJeO11994582 = -900052351;    int SfstUYVpTuiPJeO73561330 = -730473918;    int SfstUYVpTuiPJeO36686777 = -459283022;    int SfstUYVpTuiPJeO61352578 = -432610082;    int SfstUYVpTuiPJeO42424039 = -202199039;    int SfstUYVpTuiPJeO19804771 = -658337324;    int SfstUYVpTuiPJeO49591794 = -647124658;    int SfstUYVpTuiPJeO94706852 = -13696034;    int SfstUYVpTuiPJeO79665690 = -412553387;    int SfstUYVpTuiPJeO97355659 = -387344137;    int SfstUYVpTuiPJeO79669997 = -217001948;    int SfstUYVpTuiPJeO88281546 = -728121946;    int SfstUYVpTuiPJeO80752584 = -992173308;    int SfstUYVpTuiPJeO41878787 = -387537987;    int SfstUYVpTuiPJeO12481535 = -293588832;    int SfstUYVpTuiPJeO19782981 = -601727977;    int SfstUYVpTuiPJeO31137293 = -119670902;    int SfstUYVpTuiPJeO73118818 = -546925453;    int SfstUYVpTuiPJeO87124425 = -895836891;    int SfstUYVpTuiPJeO69297790 = -887954530;    int SfstUYVpTuiPJeO71786859 = -740554438;    int SfstUYVpTuiPJeO97524014 = -230721094;    int SfstUYVpTuiPJeO53453964 = -417440132;    int SfstUYVpTuiPJeO98523809 = -650589600;    int SfstUYVpTuiPJeO70392343 = -401273888;    int SfstUYVpTuiPJeO32568561 = -138856643;    int SfstUYVpTuiPJeO81170356 = -651618398;    int SfstUYVpTuiPJeO97272342 = -112486904;    int SfstUYVpTuiPJeO2999252 = -942330014;    int SfstUYVpTuiPJeO18590309 = -25973401;    int SfstUYVpTuiPJeO45611229 = -711422952;    int SfstUYVpTuiPJeO61307201 = -448578571;    int SfstUYVpTuiPJeO62257222 = -20198920;    int SfstUYVpTuiPJeO18322676 = 42160756;    int SfstUYVpTuiPJeO12292387 = -711847087;    int SfstUYVpTuiPJeO39079627 = -767526990;    int SfstUYVpTuiPJeO39830095 = -414010445;    int SfstUYVpTuiPJeO95213236 = -772205053;    int SfstUYVpTuiPJeO1145644 = -142091906;    int SfstUYVpTuiPJeO10258057 = -773996585;    int SfstUYVpTuiPJeO16534685 = -895732080;    int SfstUYVpTuiPJeO18630091 = -943758777;    int SfstUYVpTuiPJeO54235088 = -970457954;    int SfstUYVpTuiPJeO58062784 = -408486742;    int SfstUYVpTuiPJeO61072359 = -706958048;    int SfstUYVpTuiPJeO35110474 = 36816815;    int SfstUYVpTuiPJeO86932279 = -989488298;    int SfstUYVpTuiPJeO78742489 = -465678992;    int SfstUYVpTuiPJeO24982809 = -702219349;    int SfstUYVpTuiPJeO52480247 = -610411954;    int SfstUYVpTuiPJeO66984300 = -73961532;    int SfstUYVpTuiPJeO4052751 = -56927149;    int SfstUYVpTuiPJeO66530366 = -79406347;    int SfstUYVpTuiPJeO6167344 = 57456424;    int SfstUYVpTuiPJeO8852756 = -789640368;    int SfstUYVpTuiPJeO14470568 = -569331257;    int SfstUYVpTuiPJeO20107367 = -213033786;    int SfstUYVpTuiPJeO38162968 = -808693423;    int SfstUYVpTuiPJeO90960234 = 68663806;    int SfstUYVpTuiPJeO9855479 = 36657604;    int SfstUYVpTuiPJeO38634414 = 93281074;    int SfstUYVpTuiPJeO52319451 = -434637754;    int SfstUYVpTuiPJeO91707600 = -71366020;    int SfstUYVpTuiPJeO61075382 = -286579986;    int SfstUYVpTuiPJeO51744430 = -675921186;    int SfstUYVpTuiPJeO18362796 = -768423378;    int SfstUYVpTuiPJeO26024325 = -607923026;    int SfstUYVpTuiPJeO62429908 = -934334065;    int SfstUYVpTuiPJeO29586401 = -192579018;     SfstUYVpTuiPJeO93784364 = SfstUYVpTuiPJeO253424;     SfstUYVpTuiPJeO253424 = SfstUYVpTuiPJeO1791633;     SfstUYVpTuiPJeO1791633 = SfstUYVpTuiPJeO34736687;     SfstUYVpTuiPJeO34736687 = SfstUYVpTuiPJeO75262479;     SfstUYVpTuiPJeO75262479 = SfstUYVpTuiPJeO16711902;     SfstUYVpTuiPJeO16711902 = SfstUYVpTuiPJeO22782798;     SfstUYVpTuiPJeO22782798 = SfstUYVpTuiPJeO14961993;     SfstUYVpTuiPJeO14961993 = SfstUYVpTuiPJeO37722356;     SfstUYVpTuiPJeO37722356 = SfstUYVpTuiPJeO98962290;     SfstUYVpTuiPJeO98962290 = SfstUYVpTuiPJeO24286969;     SfstUYVpTuiPJeO24286969 = SfstUYVpTuiPJeO12640958;     SfstUYVpTuiPJeO12640958 = SfstUYVpTuiPJeO76516871;     SfstUYVpTuiPJeO76516871 = SfstUYVpTuiPJeO56565814;     SfstUYVpTuiPJeO56565814 = SfstUYVpTuiPJeO43569683;     SfstUYVpTuiPJeO43569683 = SfstUYVpTuiPJeO30062828;     SfstUYVpTuiPJeO30062828 = SfstUYVpTuiPJeO66126478;     SfstUYVpTuiPJeO66126478 = SfstUYVpTuiPJeO13336944;     SfstUYVpTuiPJeO13336944 = SfstUYVpTuiPJeO33900778;     SfstUYVpTuiPJeO33900778 = SfstUYVpTuiPJeO55418444;     SfstUYVpTuiPJeO55418444 = SfstUYVpTuiPJeO40742356;     SfstUYVpTuiPJeO40742356 = SfstUYVpTuiPJeO23392021;     SfstUYVpTuiPJeO23392021 = SfstUYVpTuiPJeO67684863;     SfstUYVpTuiPJeO67684863 = SfstUYVpTuiPJeO20621277;     SfstUYVpTuiPJeO20621277 = SfstUYVpTuiPJeO37464344;     SfstUYVpTuiPJeO37464344 = SfstUYVpTuiPJeO72263227;     SfstUYVpTuiPJeO72263227 = SfstUYVpTuiPJeO98121593;     SfstUYVpTuiPJeO98121593 = SfstUYVpTuiPJeO77171569;     SfstUYVpTuiPJeO77171569 = SfstUYVpTuiPJeO53654791;     SfstUYVpTuiPJeO53654791 = SfstUYVpTuiPJeO75465134;     SfstUYVpTuiPJeO75465134 = SfstUYVpTuiPJeO80639614;     SfstUYVpTuiPJeO80639614 = SfstUYVpTuiPJeO11994582;     SfstUYVpTuiPJeO11994582 = SfstUYVpTuiPJeO73561330;     SfstUYVpTuiPJeO73561330 = SfstUYVpTuiPJeO36686777;     SfstUYVpTuiPJeO36686777 = SfstUYVpTuiPJeO61352578;     SfstUYVpTuiPJeO61352578 = SfstUYVpTuiPJeO42424039;     SfstUYVpTuiPJeO42424039 = SfstUYVpTuiPJeO19804771;     SfstUYVpTuiPJeO19804771 = SfstUYVpTuiPJeO49591794;     SfstUYVpTuiPJeO49591794 = SfstUYVpTuiPJeO94706852;     SfstUYVpTuiPJeO94706852 = SfstUYVpTuiPJeO79665690;     SfstUYVpTuiPJeO79665690 = SfstUYVpTuiPJeO97355659;     SfstUYVpTuiPJeO97355659 = SfstUYVpTuiPJeO79669997;     SfstUYVpTuiPJeO79669997 = SfstUYVpTuiPJeO88281546;     SfstUYVpTuiPJeO88281546 = SfstUYVpTuiPJeO80752584;     SfstUYVpTuiPJeO80752584 = SfstUYVpTuiPJeO41878787;     SfstUYVpTuiPJeO41878787 = SfstUYVpTuiPJeO12481535;     SfstUYVpTuiPJeO12481535 = SfstUYVpTuiPJeO19782981;     SfstUYVpTuiPJeO19782981 = SfstUYVpTuiPJeO31137293;     SfstUYVpTuiPJeO31137293 = SfstUYVpTuiPJeO73118818;     SfstUYVpTuiPJeO73118818 = SfstUYVpTuiPJeO87124425;     SfstUYVpTuiPJeO87124425 = SfstUYVpTuiPJeO69297790;     SfstUYVpTuiPJeO69297790 = SfstUYVpTuiPJeO71786859;     SfstUYVpTuiPJeO71786859 = SfstUYVpTuiPJeO97524014;     SfstUYVpTuiPJeO97524014 = SfstUYVpTuiPJeO53453964;     SfstUYVpTuiPJeO53453964 = SfstUYVpTuiPJeO98523809;     SfstUYVpTuiPJeO98523809 = SfstUYVpTuiPJeO70392343;     SfstUYVpTuiPJeO70392343 = SfstUYVpTuiPJeO32568561;     SfstUYVpTuiPJeO32568561 = SfstUYVpTuiPJeO81170356;     SfstUYVpTuiPJeO81170356 = SfstUYVpTuiPJeO97272342;     SfstUYVpTuiPJeO97272342 = SfstUYVpTuiPJeO2999252;     SfstUYVpTuiPJeO2999252 = SfstUYVpTuiPJeO18590309;     SfstUYVpTuiPJeO18590309 = SfstUYVpTuiPJeO45611229;     SfstUYVpTuiPJeO45611229 = SfstUYVpTuiPJeO61307201;     SfstUYVpTuiPJeO61307201 = SfstUYVpTuiPJeO62257222;     SfstUYVpTuiPJeO62257222 = SfstUYVpTuiPJeO18322676;     SfstUYVpTuiPJeO18322676 = SfstUYVpTuiPJeO12292387;     SfstUYVpTuiPJeO12292387 = SfstUYVpTuiPJeO39079627;     SfstUYVpTuiPJeO39079627 = SfstUYVpTuiPJeO39830095;     SfstUYVpTuiPJeO39830095 = SfstUYVpTuiPJeO95213236;     SfstUYVpTuiPJeO95213236 = SfstUYVpTuiPJeO1145644;     SfstUYVpTuiPJeO1145644 = SfstUYVpTuiPJeO10258057;     SfstUYVpTuiPJeO10258057 = SfstUYVpTuiPJeO16534685;     SfstUYVpTuiPJeO16534685 = SfstUYVpTuiPJeO18630091;     SfstUYVpTuiPJeO18630091 = SfstUYVpTuiPJeO54235088;     SfstUYVpTuiPJeO54235088 = SfstUYVpTuiPJeO58062784;     SfstUYVpTuiPJeO58062784 = SfstUYVpTuiPJeO61072359;     SfstUYVpTuiPJeO61072359 = SfstUYVpTuiPJeO35110474;     SfstUYVpTuiPJeO35110474 = SfstUYVpTuiPJeO86932279;     SfstUYVpTuiPJeO86932279 = SfstUYVpTuiPJeO78742489;     SfstUYVpTuiPJeO78742489 = SfstUYVpTuiPJeO24982809;     SfstUYVpTuiPJeO24982809 = SfstUYVpTuiPJeO52480247;     SfstUYVpTuiPJeO52480247 = SfstUYVpTuiPJeO66984300;     SfstUYVpTuiPJeO66984300 = SfstUYVpTuiPJeO4052751;     SfstUYVpTuiPJeO4052751 = SfstUYVpTuiPJeO66530366;     SfstUYVpTuiPJeO66530366 = SfstUYVpTuiPJeO6167344;     SfstUYVpTuiPJeO6167344 = SfstUYVpTuiPJeO8852756;     SfstUYVpTuiPJeO8852756 = SfstUYVpTuiPJeO14470568;     SfstUYVpTuiPJeO14470568 = SfstUYVpTuiPJeO20107367;     SfstUYVpTuiPJeO20107367 = SfstUYVpTuiPJeO38162968;     SfstUYVpTuiPJeO38162968 = SfstUYVpTuiPJeO90960234;     SfstUYVpTuiPJeO90960234 = SfstUYVpTuiPJeO9855479;     SfstUYVpTuiPJeO9855479 = SfstUYVpTuiPJeO38634414;     SfstUYVpTuiPJeO38634414 = SfstUYVpTuiPJeO52319451;     SfstUYVpTuiPJeO52319451 = SfstUYVpTuiPJeO91707600;     SfstUYVpTuiPJeO91707600 = SfstUYVpTuiPJeO61075382;     SfstUYVpTuiPJeO61075382 = SfstUYVpTuiPJeO51744430;     SfstUYVpTuiPJeO51744430 = SfstUYVpTuiPJeO18362796;     SfstUYVpTuiPJeO18362796 = SfstUYVpTuiPJeO26024325;     SfstUYVpTuiPJeO26024325 = SfstUYVpTuiPJeO62429908;     SfstUYVpTuiPJeO62429908 = SfstUYVpTuiPJeO29586401;     SfstUYVpTuiPJeO29586401 = SfstUYVpTuiPJeO93784364;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void IFUjufptowWvJBkd45924909() {     int dQgyGDrKIiziLuV93777892 = -65103157;    int dQgyGDrKIiziLuV38825741 = -163113367;    int dQgyGDrKIiziLuV3104845 = -561963989;    int dQgyGDrKIiziLuV19816548 = -28835918;    int dQgyGDrKIiziLuV84102581 = -622825257;    int dQgyGDrKIiziLuV51517334 = -119038646;    int dQgyGDrKIiziLuV91330134 = -119812020;    int dQgyGDrKIiziLuV84176043 = -227999110;    int dQgyGDrKIiziLuV20462520 = -764343094;    int dQgyGDrKIiziLuV95601804 = -224840919;    int dQgyGDrKIiziLuV44607574 = -571371508;    int dQgyGDrKIiziLuV26932635 = -233479260;    int dQgyGDrKIiziLuV8369786 = -994951941;    int dQgyGDrKIiziLuV10410035 = -549968185;    int dQgyGDrKIiziLuV88741791 = -407003777;    int dQgyGDrKIiziLuV4627375 = -765129559;    int dQgyGDrKIiziLuV21979238 = -362471307;    int dQgyGDrKIiziLuV36337476 = -354526428;    int dQgyGDrKIiziLuV54329048 = 99006392;    int dQgyGDrKIiziLuV27492207 = -407855330;    int dQgyGDrKIiziLuV20411780 = -734426364;    int dQgyGDrKIiziLuV1688287 = -201297013;    int dQgyGDrKIiziLuV4563260 = -340619448;    int dQgyGDrKIiziLuV64271385 = -834137500;    int dQgyGDrKIiziLuV65907033 = -422502022;    int dQgyGDrKIiziLuV88837304 = -120345863;    int dQgyGDrKIiziLuV68343175 = -594432147;    int dQgyGDrKIiziLuV12575094 = -212819584;    int dQgyGDrKIiziLuV66333238 = -621313568;    int dQgyGDrKIiziLuV51555004 = -605929479;    int dQgyGDrKIiziLuV14052294 = -917288040;    int dQgyGDrKIiziLuV96718439 = -191874115;    int dQgyGDrKIiziLuV59013754 = -932418137;    int dQgyGDrKIiziLuV36969614 = -335964755;    int dQgyGDrKIiziLuV43710616 = -691924010;    int dQgyGDrKIiziLuV85252228 = -281964803;    int dQgyGDrKIiziLuV31746138 = -757990702;    int dQgyGDrKIiziLuV19509474 = -642387334;    int dQgyGDrKIiziLuV30020449 = -726377462;    int dQgyGDrKIiziLuV51538584 = -21985073;    int dQgyGDrKIiziLuV85211327 = -475288035;    int dQgyGDrKIiziLuV65367658 = -410794051;    int dQgyGDrKIiziLuV48564906 = -187252202;    int dQgyGDrKIiziLuV39984482 = -549137752;    int dQgyGDrKIiziLuV25896839 = -860797389;    int dQgyGDrKIiziLuV52909503 = -105633521;    int dQgyGDrKIiziLuV47480774 = 16991401;    int dQgyGDrKIiziLuV6520875 = -728432260;    int dQgyGDrKIiziLuV91777207 = -110689339;    int dQgyGDrKIiziLuV7169850 = -482861772;    int dQgyGDrKIiziLuV82557160 = -466509537;    int dQgyGDrKIiziLuV86334116 = -22700190;    int dQgyGDrKIiziLuV72972363 = -122775099;    int dQgyGDrKIiziLuV63704235 = -949359984;    int dQgyGDrKIiziLuV25513130 = -407488883;    int dQgyGDrKIiziLuV92089606 = -863806144;    int dQgyGDrKIiziLuV34262481 = -822493920;    int dQgyGDrKIiziLuV38833460 = -727826489;    int dQgyGDrKIiziLuV53909514 = -606333897;    int dQgyGDrKIiziLuV95265277 = -402479395;    int dQgyGDrKIiziLuV83174159 = -524606500;    int dQgyGDrKIiziLuV78755040 = -906992436;    int dQgyGDrKIiziLuV17842805 = -606685542;    int dQgyGDrKIiziLuV68907516 = -58413615;    int dQgyGDrKIiziLuV81549510 = -307552880;    int dQgyGDrKIiziLuV47889134 = -279497393;    int dQgyGDrKIiziLuV67918881 = -301061124;    int dQgyGDrKIiziLuV71400171 = -558987187;    int dQgyGDrKIiziLuV66699418 = -858044176;    int dQgyGDrKIiziLuV3489564 = -25038975;    int dQgyGDrKIiziLuV72881236 = 92861143;    int dQgyGDrKIiziLuV2469765 = -720083974;    int dQgyGDrKIiziLuV6317028 = -628148967;    int dQgyGDrKIiziLuV2790464 = -879008535;    int dQgyGDrKIiziLuV42280880 = -932567295;    int dQgyGDrKIiziLuV55044122 = -223632313;    int dQgyGDrKIiziLuV53123380 = 85955188;    int dQgyGDrKIiziLuV64578777 = -791481697;    int dQgyGDrKIiziLuV38374546 = -973340111;    int dQgyGDrKIiziLuV12997531 = -216868501;    int dQgyGDrKIiziLuV41356530 = -37337264;    int dQgyGDrKIiziLuV61822301 = -865999887;    int dQgyGDrKIiziLuV20797887 = -2130246;    int dQgyGDrKIiziLuV59163389 = -38451796;    int dQgyGDrKIiziLuV68997844 = -39419942;    int dQgyGDrKIiziLuV27718177 = -794587850;    int dQgyGDrKIiziLuV23746077 = 30900983;    int dQgyGDrKIiziLuV95309518 = -983058153;    int dQgyGDrKIiziLuV11456485 = -928475873;    int dQgyGDrKIiziLuV51621009 = -828117866;    int dQgyGDrKIiziLuV50989747 = -459470884;    int dQgyGDrKIiziLuV92912678 = 69835786;    int dQgyGDrKIiziLuV65599959 = 63946562;    int dQgyGDrKIiziLuV34755171 = -223898068;    int dQgyGDrKIiziLuV68364424 = -497378573;    int dQgyGDrKIiziLuV6456287 = -568295599;    int dQgyGDrKIiziLuV47524853 = -804108509;    int dQgyGDrKIiziLuV79657390 = -28838587;    int dQgyGDrKIiziLuV58434972 = -141584872;    int dQgyGDrKIiziLuV78007705 = -65103157;     dQgyGDrKIiziLuV93777892 = dQgyGDrKIiziLuV38825741;     dQgyGDrKIiziLuV38825741 = dQgyGDrKIiziLuV3104845;     dQgyGDrKIiziLuV3104845 = dQgyGDrKIiziLuV19816548;     dQgyGDrKIiziLuV19816548 = dQgyGDrKIiziLuV84102581;     dQgyGDrKIiziLuV84102581 = dQgyGDrKIiziLuV51517334;     dQgyGDrKIiziLuV51517334 = dQgyGDrKIiziLuV91330134;     dQgyGDrKIiziLuV91330134 = dQgyGDrKIiziLuV84176043;     dQgyGDrKIiziLuV84176043 = dQgyGDrKIiziLuV20462520;     dQgyGDrKIiziLuV20462520 = dQgyGDrKIiziLuV95601804;     dQgyGDrKIiziLuV95601804 = dQgyGDrKIiziLuV44607574;     dQgyGDrKIiziLuV44607574 = dQgyGDrKIiziLuV26932635;     dQgyGDrKIiziLuV26932635 = dQgyGDrKIiziLuV8369786;     dQgyGDrKIiziLuV8369786 = dQgyGDrKIiziLuV10410035;     dQgyGDrKIiziLuV10410035 = dQgyGDrKIiziLuV88741791;     dQgyGDrKIiziLuV88741791 = dQgyGDrKIiziLuV4627375;     dQgyGDrKIiziLuV4627375 = dQgyGDrKIiziLuV21979238;     dQgyGDrKIiziLuV21979238 = dQgyGDrKIiziLuV36337476;     dQgyGDrKIiziLuV36337476 = dQgyGDrKIiziLuV54329048;     dQgyGDrKIiziLuV54329048 = dQgyGDrKIiziLuV27492207;     dQgyGDrKIiziLuV27492207 = dQgyGDrKIiziLuV20411780;     dQgyGDrKIiziLuV20411780 = dQgyGDrKIiziLuV1688287;     dQgyGDrKIiziLuV1688287 = dQgyGDrKIiziLuV4563260;     dQgyGDrKIiziLuV4563260 = dQgyGDrKIiziLuV64271385;     dQgyGDrKIiziLuV64271385 = dQgyGDrKIiziLuV65907033;     dQgyGDrKIiziLuV65907033 = dQgyGDrKIiziLuV88837304;     dQgyGDrKIiziLuV88837304 = dQgyGDrKIiziLuV68343175;     dQgyGDrKIiziLuV68343175 = dQgyGDrKIiziLuV12575094;     dQgyGDrKIiziLuV12575094 = dQgyGDrKIiziLuV66333238;     dQgyGDrKIiziLuV66333238 = dQgyGDrKIiziLuV51555004;     dQgyGDrKIiziLuV51555004 = dQgyGDrKIiziLuV14052294;     dQgyGDrKIiziLuV14052294 = dQgyGDrKIiziLuV96718439;     dQgyGDrKIiziLuV96718439 = dQgyGDrKIiziLuV59013754;     dQgyGDrKIiziLuV59013754 = dQgyGDrKIiziLuV36969614;     dQgyGDrKIiziLuV36969614 = dQgyGDrKIiziLuV43710616;     dQgyGDrKIiziLuV43710616 = dQgyGDrKIiziLuV85252228;     dQgyGDrKIiziLuV85252228 = dQgyGDrKIiziLuV31746138;     dQgyGDrKIiziLuV31746138 = dQgyGDrKIiziLuV19509474;     dQgyGDrKIiziLuV19509474 = dQgyGDrKIiziLuV30020449;     dQgyGDrKIiziLuV30020449 = dQgyGDrKIiziLuV51538584;     dQgyGDrKIiziLuV51538584 = dQgyGDrKIiziLuV85211327;     dQgyGDrKIiziLuV85211327 = dQgyGDrKIiziLuV65367658;     dQgyGDrKIiziLuV65367658 = dQgyGDrKIiziLuV48564906;     dQgyGDrKIiziLuV48564906 = dQgyGDrKIiziLuV39984482;     dQgyGDrKIiziLuV39984482 = dQgyGDrKIiziLuV25896839;     dQgyGDrKIiziLuV25896839 = dQgyGDrKIiziLuV52909503;     dQgyGDrKIiziLuV52909503 = dQgyGDrKIiziLuV47480774;     dQgyGDrKIiziLuV47480774 = dQgyGDrKIiziLuV6520875;     dQgyGDrKIiziLuV6520875 = dQgyGDrKIiziLuV91777207;     dQgyGDrKIiziLuV91777207 = dQgyGDrKIiziLuV7169850;     dQgyGDrKIiziLuV7169850 = dQgyGDrKIiziLuV82557160;     dQgyGDrKIiziLuV82557160 = dQgyGDrKIiziLuV86334116;     dQgyGDrKIiziLuV86334116 = dQgyGDrKIiziLuV72972363;     dQgyGDrKIiziLuV72972363 = dQgyGDrKIiziLuV63704235;     dQgyGDrKIiziLuV63704235 = dQgyGDrKIiziLuV25513130;     dQgyGDrKIiziLuV25513130 = dQgyGDrKIiziLuV92089606;     dQgyGDrKIiziLuV92089606 = dQgyGDrKIiziLuV34262481;     dQgyGDrKIiziLuV34262481 = dQgyGDrKIiziLuV38833460;     dQgyGDrKIiziLuV38833460 = dQgyGDrKIiziLuV53909514;     dQgyGDrKIiziLuV53909514 = dQgyGDrKIiziLuV95265277;     dQgyGDrKIiziLuV95265277 = dQgyGDrKIiziLuV83174159;     dQgyGDrKIiziLuV83174159 = dQgyGDrKIiziLuV78755040;     dQgyGDrKIiziLuV78755040 = dQgyGDrKIiziLuV17842805;     dQgyGDrKIiziLuV17842805 = dQgyGDrKIiziLuV68907516;     dQgyGDrKIiziLuV68907516 = dQgyGDrKIiziLuV81549510;     dQgyGDrKIiziLuV81549510 = dQgyGDrKIiziLuV47889134;     dQgyGDrKIiziLuV47889134 = dQgyGDrKIiziLuV67918881;     dQgyGDrKIiziLuV67918881 = dQgyGDrKIiziLuV71400171;     dQgyGDrKIiziLuV71400171 = dQgyGDrKIiziLuV66699418;     dQgyGDrKIiziLuV66699418 = dQgyGDrKIiziLuV3489564;     dQgyGDrKIiziLuV3489564 = dQgyGDrKIiziLuV72881236;     dQgyGDrKIiziLuV72881236 = dQgyGDrKIiziLuV2469765;     dQgyGDrKIiziLuV2469765 = dQgyGDrKIiziLuV6317028;     dQgyGDrKIiziLuV6317028 = dQgyGDrKIiziLuV2790464;     dQgyGDrKIiziLuV2790464 = dQgyGDrKIiziLuV42280880;     dQgyGDrKIiziLuV42280880 = dQgyGDrKIiziLuV55044122;     dQgyGDrKIiziLuV55044122 = dQgyGDrKIiziLuV53123380;     dQgyGDrKIiziLuV53123380 = dQgyGDrKIiziLuV64578777;     dQgyGDrKIiziLuV64578777 = dQgyGDrKIiziLuV38374546;     dQgyGDrKIiziLuV38374546 = dQgyGDrKIiziLuV12997531;     dQgyGDrKIiziLuV12997531 = dQgyGDrKIiziLuV41356530;     dQgyGDrKIiziLuV41356530 = dQgyGDrKIiziLuV61822301;     dQgyGDrKIiziLuV61822301 = dQgyGDrKIiziLuV20797887;     dQgyGDrKIiziLuV20797887 = dQgyGDrKIiziLuV59163389;     dQgyGDrKIiziLuV59163389 = dQgyGDrKIiziLuV68997844;     dQgyGDrKIiziLuV68997844 = dQgyGDrKIiziLuV27718177;     dQgyGDrKIiziLuV27718177 = dQgyGDrKIiziLuV23746077;     dQgyGDrKIiziLuV23746077 = dQgyGDrKIiziLuV95309518;     dQgyGDrKIiziLuV95309518 = dQgyGDrKIiziLuV11456485;     dQgyGDrKIiziLuV11456485 = dQgyGDrKIiziLuV51621009;     dQgyGDrKIiziLuV51621009 = dQgyGDrKIiziLuV50989747;     dQgyGDrKIiziLuV50989747 = dQgyGDrKIiziLuV92912678;     dQgyGDrKIiziLuV92912678 = dQgyGDrKIiziLuV65599959;     dQgyGDrKIiziLuV65599959 = dQgyGDrKIiziLuV34755171;     dQgyGDrKIiziLuV34755171 = dQgyGDrKIiziLuV68364424;     dQgyGDrKIiziLuV68364424 = dQgyGDrKIiziLuV6456287;     dQgyGDrKIiziLuV6456287 = dQgyGDrKIiziLuV47524853;     dQgyGDrKIiziLuV47524853 = dQgyGDrKIiziLuV79657390;     dQgyGDrKIiziLuV79657390 = dQgyGDrKIiziLuV58434972;     dQgyGDrKIiziLuV58434972 = dQgyGDrKIiziLuV78007705;     dQgyGDrKIiziLuV78007705 = dQgyGDrKIiziLuV93777892;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void yXHwmzpIvnWCryxU341123() {     int bYaNunzJmxyWhEu64429559 = -683252289;    int bYaNunzJmxyWhEu7499258 = -34756673;    int bYaNunzJmxyWhEu83073763 = 95741551;    int bYaNunzJmxyWhEu59747834 = -218974304;    int bYaNunzJmxyWhEu5552904 = -406231613;    int bYaNunzJmxyWhEu41517097 = -920619798;    int bYaNunzJmxyWhEu9709030 = -118587730;    int bYaNunzJmxyWhEu41018424 = -574816215;    int bYaNunzJmxyWhEu66327574 = -857690423;    int bYaNunzJmxyWhEu18568265 = -525936270;    int bYaNunzJmxyWhEu86856734 = -606273438;    int bYaNunzJmxyWhEu72479206 = -549243062;    int bYaNunzJmxyWhEu88197172 = -904489000;    int bYaNunzJmxyWhEu94257068 = -350996396;    int bYaNunzJmxyWhEu71461811 = -556753799;    int bYaNunzJmxyWhEu37156015 = -335113146;    int bYaNunzJmxyWhEu15832065 = -271656437;    int bYaNunzJmxyWhEu56630970 = -343692629;    int bYaNunzJmxyWhEu36036830 = -608181549;    int bYaNunzJmxyWhEu86278916 = -950221635;    int bYaNunzJmxyWhEu75547534 = -643105509;    int bYaNunzJmxyWhEu63084051 = -532582319;    int bYaNunzJmxyWhEu6507069 = -908462593;    int bYaNunzJmxyWhEu98120921 = -426538406;    int bYaNunzJmxyWhEu30031422 = -98406722;    int bYaNunzJmxyWhEu19434886 = -218630304;    int bYaNunzJmxyWhEu76603960 = -989609922;    int bYaNunzJmxyWhEu20357319 = -717943358;    int bYaNunzJmxyWhEu98475726 = -915671769;    int bYaNunzJmxyWhEu90727100 = -64324549;    int bYaNunzJmxyWhEu21124290 = -665721725;    int bYaNunzJmxyWhEu41229900 = -520171275;    int bYaNunzJmxyWhEu41959115 = -879379742;    int bYaNunzJmxyWhEu2334589 = -999494439;    int bYaNunzJmxyWhEu21734853 = -538870786;    int bYaNunzJmxyWhEu45245488 = -361191333;    int bYaNunzJmxyWhEu75537624 = -946583910;    int bYaNunzJmxyWhEu72301962 = -426500444;    int bYaNunzJmxyWhEu8553738 = -682106087;    int bYaNunzJmxyWhEu46641882 = -639593436;    int bYaNunzJmxyWhEu45284611 = -859306914;    int bYaNunzJmxyWhEu93850648 = -180183100;    int bYaNunzJmxyWhEu97531866 = -174006252;    int bYaNunzJmxyWhEu46492783 = -621112723;    int bYaNunzJmxyWhEu21850898 = -69443783;    int bYaNunzJmxyWhEu59315338 = -963970658;    int bYaNunzJmxyWhEu73611976 = -99078398;    int bYaNunzJmxyWhEu72135084 = -80562554;    int bYaNunzJmxyWhEu97218659 = -96453283;    int bYaNunzJmxyWhEu15597336 = -765870656;    int bYaNunzJmxyWhEu50746485 = -13081658;    int bYaNunzJmxyWhEu28205621 = 8080092;    int bYaNunzJmxyWhEu28048020 = -50573144;    int bYaNunzJmxyWhEu53931539 = -510241480;    int bYaNunzJmxyWhEu24793588 = -518429792;    int bYaNunzJmxyWhEu1345509 = -50669971;    int bYaNunzJmxyWhEu992190 = -126294081;    int bYaNunzJmxyWhEu84952842 = -477720043;    int bYaNunzJmxyWhEu29716412 = -20567583;    int bYaNunzJmxyWhEu86118018 = -87601310;    int bYaNunzJmxyWhEu64913136 = -931009876;    int bYaNunzJmxyWhEu89351710 = -400644373;    int bYaNunzJmxyWhEu42542698 = -659144446;    int bYaNunzJmxyWhEu75600474 = -693365874;    int bYaNunzJmxyWhEu97443974 = -860214545;    int bYaNunzJmxyWhEu45626834 = 13897836;    int bYaNunzJmxyWhEu30520092 = -669863320;    int bYaNunzJmxyWhEu85862583 = -904994561;    int bYaNunzJmxyWhEu72522216 = -812125611;    int bYaNunzJmxyWhEu26216324 = -95562467;    int bYaNunzJmxyWhEu61618391 = -388529236;    int bYaNunzJmxyWhEu43530103 = -845155993;    int bYaNunzJmxyWhEu48077233 = -661586542;    int bYaNunzJmxyWhEu89394948 = -968588114;    int bYaNunzJmxyWhEu40994305 = 9085278;    int bYaNunzJmxyWhEu81696885 = -362922410;    int bYaNunzJmxyWhEu65552185 = -258576067;    int bYaNunzJmxyWhEu60014286 = -187349870;    int bYaNunzJmxyWhEu76270023 = -257094624;    int bYaNunzJmxyWhEu70716084 = -134436065;    int bYaNunzJmxyWhEu45822909 = -19551906;    int bYaNunzJmxyWhEu4468877 = -809047368;    int bYaNunzJmxyWhEu23138660 = -521490075;    int bYaNunzJmxyWhEu82878391 = -49801114;    int bYaNunzJmxyWhEu39980615 = 48757108;    int bYaNunzJmxyWhEu92918668 = -573801818;    int bYaNunzJmxyWhEu13181881 = -369598132;    int bYaNunzJmxyWhEu88027575 = -269138263;    int bYaNunzJmxyWhEu77541000 = -381064648;    int bYaNunzJmxyWhEu20389344 = -388200816;    int bYaNunzJmxyWhEu44253298 = -134897253;    int bYaNunzJmxyWhEu90584782 = -368863867;    int bYaNunzJmxyWhEu42585550 = -305932862;    int bYaNunzJmxyWhEu22435720 = -494504777;    int bYaNunzJmxyWhEu81728745 = -708583560;    int bYaNunzJmxyWhEu55932901 = -358662541;    int bYaNunzJmxyWhEu51307951 = -521038654;    int bYaNunzJmxyWhEu21931392 = -480640378;    int bYaNunzJmxyWhEu49048808 = -760898179;    int bYaNunzJmxyWhEu76224063 = -683252289;     bYaNunzJmxyWhEu64429559 = bYaNunzJmxyWhEu7499258;     bYaNunzJmxyWhEu7499258 = bYaNunzJmxyWhEu83073763;     bYaNunzJmxyWhEu83073763 = bYaNunzJmxyWhEu59747834;     bYaNunzJmxyWhEu59747834 = bYaNunzJmxyWhEu5552904;     bYaNunzJmxyWhEu5552904 = bYaNunzJmxyWhEu41517097;     bYaNunzJmxyWhEu41517097 = bYaNunzJmxyWhEu9709030;     bYaNunzJmxyWhEu9709030 = bYaNunzJmxyWhEu41018424;     bYaNunzJmxyWhEu41018424 = bYaNunzJmxyWhEu66327574;     bYaNunzJmxyWhEu66327574 = bYaNunzJmxyWhEu18568265;     bYaNunzJmxyWhEu18568265 = bYaNunzJmxyWhEu86856734;     bYaNunzJmxyWhEu86856734 = bYaNunzJmxyWhEu72479206;     bYaNunzJmxyWhEu72479206 = bYaNunzJmxyWhEu88197172;     bYaNunzJmxyWhEu88197172 = bYaNunzJmxyWhEu94257068;     bYaNunzJmxyWhEu94257068 = bYaNunzJmxyWhEu71461811;     bYaNunzJmxyWhEu71461811 = bYaNunzJmxyWhEu37156015;     bYaNunzJmxyWhEu37156015 = bYaNunzJmxyWhEu15832065;     bYaNunzJmxyWhEu15832065 = bYaNunzJmxyWhEu56630970;     bYaNunzJmxyWhEu56630970 = bYaNunzJmxyWhEu36036830;     bYaNunzJmxyWhEu36036830 = bYaNunzJmxyWhEu86278916;     bYaNunzJmxyWhEu86278916 = bYaNunzJmxyWhEu75547534;     bYaNunzJmxyWhEu75547534 = bYaNunzJmxyWhEu63084051;     bYaNunzJmxyWhEu63084051 = bYaNunzJmxyWhEu6507069;     bYaNunzJmxyWhEu6507069 = bYaNunzJmxyWhEu98120921;     bYaNunzJmxyWhEu98120921 = bYaNunzJmxyWhEu30031422;     bYaNunzJmxyWhEu30031422 = bYaNunzJmxyWhEu19434886;     bYaNunzJmxyWhEu19434886 = bYaNunzJmxyWhEu76603960;     bYaNunzJmxyWhEu76603960 = bYaNunzJmxyWhEu20357319;     bYaNunzJmxyWhEu20357319 = bYaNunzJmxyWhEu98475726;     bYaNunzJmxyWhEu98475726 = bYaNunzJmxyWhEu90727100;     bYaNunzJmxyWhEu90727100 = bYaNunzJmxyWhEu21124290;     bYaNunzJmxyWhEu21124290 = bYaNunzJmxyWhEu41229900;     bYaNunzJmxyWhEu41229900 = bYaNunzJmxyWhEu41959115;     bYaNunzJmxyWhEu41959115 = bYaNunzJmxyWhEu2334589;     bYaNunzJmxyWhEu2334589 = bYaNunzJmxyWhEu21734853;     bYaNunzJmxyWhEu21734853 = bYaNunzJmxyWhEu45245488;     bYaNunzJmxyWhEu45245488 = bYaNunzJmxyWhEu75537624;     bYaNunzJmxyWhEu75537624 = bYaNunzJmxyWhEu72301962;     bYaNunzJmxyWhEu72301962 = bYaNunzJmxyWhEu8553738;     bYaNunzJmxyWhEu8553738 = bYaNunzJmxyWhEu46641882;     bYaNunzJmxyWhEu46641882 = bYaNunzJmxyWhEu45284611;     bYaNunzJmxyWhEu45284611 = bYaNunzJmxyWhEu93850648;     bYaNunzJmxyWhEu93850648 = bYaNunzJmxyWhEu97531866;     bYaNunzJmxyWhEu97531866 = bYaNunzJmxyWhEu46492783;     bYaNunzJmxyWhEu46492783 = bYaNunzJmxyWhEu21850898;     bYaNunzJmxyWhEu21850898 = bYaNunzJmxyWhEu59315338;     bYaNunzJmxyWhEu59315338 = bYaNunzJmxyWhEu73611976;     bYaNunzJmxyWhEu73611976 = bYaNunzJmxyWhEu72135084;     bYaNunzJmxyWhEu72135084 = bYaNunzJmxyWhEu97218659;     bYaNunzJmxyWhEu97218659 = bYaNunzJmxyWhEu15597336;     bYaNunzJmxyWhEu15597336 = bYaNunzJmxyWhEu50746485;     bYaNunzJmxyWhEu50746485 = bYaNunzJmxyWhEu28205621;     bYaNunzJmxyWhEu28205621 = bYaNunzJmxyWhEu28048020;     bYaNunzJmxyWhEu28048020 = bYaNunzJmxyWhEu53931539;     bYaNunzJmxyWhEu53931539 = bYaNunzJmxyWhEu24793588;     bYaNunzJmxyWhEu24793588 = bYaNunzJmxyWhEu1345509;     bYaNunzJmxyWhEu1345509 = bYaNunzJmxyWhEu992190;     bYaNunzJmxyWhEu992190 = bYaNunzJmxyWhEu84952842;     bYaNunzJmxyWhEu84952842 = bYaNunzJmxyWhEu29716412;     bYaNunzJmxyWhEu29716412 = bYaNunzJmxyWhEu86118018;     bYaNunzJmxyWhEu86118018 = bYaNunzJmxyWhEu64913136;     bYaNunzJmxyWhEu64913136 = bYaNunzJmxyWhEu89351710;     bYaNunzJmxyWhEu89351710 = bYaNunzJmxyWhEu42542698;     bYaNunzJmxyWhEu42542698 = bYaNunzJmxyWhEu75600474;     bYaNunzJmxyWhEu75600474 = bYaNunzJmxyWhEu97443974;     bYaNunzJmxyWhEu97443974 = bYaNunzJmxyWhEu45626834;     bYaNunzJmxyWhEu45626834 = bYaNunzJmxyWhEu30520092;     bYaNunzJmxyWhEu30520092 = bYaNunzJmxyWhEu85862583;     bYaNunzJmxyWhEu85862583 = bYaNunzJmxyWhEu72522216;     bYaNunzJmxyWhEu72522216 = bYaNunzJmxyWhEu26216324;     bYaNunzJmxyWhEu26216324 = bYaNunzJmxyWhEu61618391;     bYaNunzJmxyWhEu61618391 = bYaNunzJmxyWhEu43530103;     bYaNunzJmxyWhEu43530103 = bYaNunzJmxyWhEu48077233;     bYaNunzJmxyWhEu48077233 = bYaNunzJmxyWhEu89394948;     bYaNunzJmxyWhEu89394948 = bYaNunzJmxyWhEu40994305;     bYaNunzJmxyWhEu40994305 = bYaNunzJmxyWhEu81696885;     bYaNunzJmxyWhEu81696885 = bYaNunzJmxyWhEu65552185;     bYaNunzJmxyWhEu65552185 = bYaNunzJmxyWhEu60014286;     bYaNunzJmxyWhEu60014286 = bYaNunzJmxyWhEu76270023;     bYaNunzJmxyWhEu76270023 = bYaNunzJmxyWhEu70716084;     bYaNunzJmxyWhEu70716084 = bYaNunzJmxyWhEu45822909;     bYaNunzJmxyWhEu45822909 = bYaNunzJmxyWhEu4468877;     bYaNunzJmxyWhEu4468877 = bYaNunzJmxyWhEu23138660;     bYaNunzJmxyWhEu23138660 = bYaNunzJmxyWhEu82878391;     bYaNunzJmxyWhEu82878391 = bYaNunzJmxyWhEu39980615;     bYaNunzJmxyWhEu39980615 = bYaNunzJmxyWhEu92918668;     bYaNunzJmxyWhEu92918668 = bYaNunzJmxyWhEu13181881;     bYaNunzJmxyWhEu13181881 = bYaNunzJmxyWhEu88027575;     bYaNunzJmxyWhEu88027575 = bYaNunzJmxyWhEu77541000;     bYaNunzJmxyWhEu77541000 = bYaNunzJmxyWhEu20389344;     bYaNunzJmxyWhEu20389344 = bYaNunzJmxyWhEu44253298;     bYaNunzJmxyWhEu44253298 = bYaNunzJmxyWhEu90584782;     bYaNunzJmxyWhEu90584782 = bYaNunzJmxyWhEu42585550;     bYaNunzJmxyWhEu42585550 = bYaNunzJmxyWhEu22435720;     bYaNunzJmxyWhEu22435720 = bYaNunzJmxyWhEu81728745;     bYaNunzJmxyWhEu81728745 = bYaNunzJmxyWhEu55932901;     bYaNunzJmxyWhEu55932901 = bYaNunzJmxyWhEu51307951;     bYaNunzJmxyWhEu51307951 = bYaNunzJmxyWhEu21931392;     bYaNunzJmxyWhEu21931392 = bYaNunzJmxyWhEu49048808;     bYaNunzJmxyWhEu49048808 = bYaNunzJmxyWhEu76224063;     bYaNunzJmxyWhEu76224063 = bYaNunzJmxyWhEu64429559;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void nrJoIGRGjnGelZnQ6999868() {     int kKCJsfjtSdrAbDF64423088 = -555776429;    int kKCJsfjtSdrAbDF46071575 = -77351792;    int kKCJsfjtSdrAbDF84386975 = -961387062;    int kKCJsfjtSdrAbDF44827695 = -39515137;    int kKCJsfjtSdrAbDF14393006 = -874586926;    int kKCJsfjtSdrAbDF76322529 = -620052609;    int kKCJsfjtSdrAbDF78256365 = -923124197;    int kKCJsfjtSdrAbDF10232475 = -278993517;    int kKCJsfjtSdrAbDF49067738 = -571336492;    int kKCJsfjtSdrAbDF15207779 = -162743141;    int kKCJsfjtSdrAbDF7177340 = -565745509;    int kKCJsfjtSdrAbDF86770883 = -284721416;    int kKCJsfjtSdrAbDF20050086 = -926147474;    int kKCJsfjtSdrAbDF48101288 = -696149447;    int kKCJsfjtSdrAbDF16633921 = -519466633;    int kKCJsfjtSdrAbDF11720562 = -667908796;    int kKCJsfjtSdrAbDF71684824 = -91271006;    int kKCJsfjtSdrAbDF79631503 = -740764246;    int kKCJsfjtSdrAbDF56465099 = -126163817;    int kKCJsfjtSdrAbDF58352679 = -462246086;    int kKCJsfjtSdrAbDF55216958 = -353571878;    int kKCJsfjtSdrAbDF41380317 = 57425798;    int kKCJsfjtSdrAbDF43385465 = -267420435;    int kKCJsfjtSdrAbDF41771030 = -307458929;    int kKCJsfjtSdrAbDF58474112 = -525100562;    int kKCJsfjtSdrAbDF36008962 = -126836236;    int kKCJsfjtSdrAbDF46825542 = -190409635;    int kKCJsfjtSdrAbDF55760844 = -226910340;    int kKCJsfjtSdrAbDF11154174 = -461742100;    int kKCJsfjtSdrAbDF66816970 = -839755923;    int kKCJsfjtSdrAbDF54536969 = 47185040;    int kKCJsfjtSdrAbDF25953758 = -911993040;    int kKCJsfjtSdrAbDF27411538 = 18676039;    int kKCJsfjtSdrAbDF2617427 = -876176172;    int kKCJsfjtSdrAbDF4092891 = -798184714;    int kKCJsfjtSdrAbDF88073677 = -440957097;    int kKCJsfjtSdrAbDF87478991 = 53762712;    int kKCJsfjtSdrAbDF42219641 = -421763120;    int kKCJsfjtSdrAbDF43867334 = -294787515;    int kKCJsfjtSdrAbDF18514775 = -249025122;    int kKCJsfjtSdrAbDF33140279 = -947250812;    int kKCJsfjtSdrAbDF79548310 = -373975203;    int kKCJsfjtSdrAbDF57815226 = -733136509;    int kKCJsfjtSdrAbDF5724681 = -178077167;    int kKCJsfjtSdrAbDF5868951 = -542703185;    int kKCJsfjtSdrAbDF99743306 = -776015347;    int kKCJsfjtSdrAbDF1309770 = -580359020;    int kKCJsfjtSdrAbDF47518665 = -689323911;    int kKCJsfjtSdrAbDF15877049 = -760217169;    int kKCJsfjtSdrAbDF35642759 = -352895537;    int kKCJsfjtSdrAbDF64005854 = -691636665;    int kKCJsfjtSdrAbDF42752879 = -374065660;    int kKCJsfjtSdrAbDF3496369 = 57372851;    int kKCJsfjtSdrAbDF64181810 = 57838668;    int kKCJsfjtSdrAbDF51782909 = -275329075;    int kKCJsfjtSdrAbDF23042772 = -513202227;    int kKCJsfjtSdrAbDF2686110 = -809931358;    int kKCJsfjtSdrAbDF42615946 = -553928134;    int kKCJsfjtSdrAbDF86353583 = -514414575;    int kKCJsfjtSdrAbDF78384043 = -647750691;    int kKCJsfjtSdrAbDF29496987 = -329642975;    int kKCJsfjtSdrAbDF22495522 = -596213857;    int kKCJsfjtSdrAbDF99078301 = -817251417;    int kKCJsfjtSdrAbDF82250768 = -731580570;    int kKCJsfjtSdrAbDF60670810 = -109928182;    int kKCJsfjtSdrAbDF81223582 = -653752470;    int kKCJsfjtSdrAbDF59359345 = -203397455;    int kKCJsfjtSdrAbDF17432660 = 50028697;    int kKCJsfjtSdrAbDF44008398 = -897964734;    int kKCJsfjtSdrAbDF28560244 = 21490464;    int kKCJsfjtSdrAbDF24241570 = -621671509;    int kKCJsfjtSdrAbDF29465183 = -669507887;    int kKCJsfjtSdrAbDF35764169 = -345976732;    int kKCJsfjtSdrAbDF37950324 = -877138695;    int kKCJsfjtSdrAbDF25212401 = -514995274;    int kKCJsfjtSdrAbDF75668648 = -979596676;    int kKCJsfjtSdrAbDF83565090 = -209437694;    int kKCJsfjtSdrAbDF37660784 = 10656732;    int kKCJsfjtSdrAbDF35902080 = -764755744;    int kKCJsfjtSdrAbDF58730805 = -749085216;    int kKCJsfjtSdrAbDF34699192 = -546477216;    int kKCJsfjtSdrAbDF99306877 = -501085724;    int kKCJsfjtSdrAbDF39883795 = -466693171;    int kKCJsfjtSdrAbDF75511414 = -8846563;    int kKCJsfjtSdrAbDF2811116 = -48119258;    int kKCJsfjtSdrAbDF11784091 = -578749300;    int kKCJsfjtSdrAbDF22457390 = -869365891;    int kKCJsfjtSdrAbDF63229727 = 60837370;    int kKCJsfjtSdrAbDF50834518 = -500847097;    int kKCJsfjtSdrAbDF81050118 = -184982487;    int kKCJsfjtSdrAbDF85387567 = -631025740;    int kKCJsfjtSdrAbDF44863046 = -392309155;    int kKCJsfjtSdrAbDF55866058 = -907348546;    int kKCJsfjtSdrAbDF65483291 = -647036825;    int kKCJsfjtSdrAbDF89017788 = -919382147;    int kKCJsfjtSdrAbDF10644758 = -251036955;    int kKCJsfjtSdrAbDF80470008 = -556723786;    int kKCJsfjtSdrAbDF75564457 = 98444061;    int kKCJsfjtSdrAbDF45053871 = 31851014;    int kKCJsfjtSdrAbDF24645368 = -555776429;     kKCJsfjtSdrAbDF64423088 = kKCJsfjtSdrAbDF46071575;     kKCJsfjtSdrAbDF46071575 = kKCJsfjtSdrAbDF84386975;     kKCJsfjtSdrAbDF84386975 = kKCJsfjtSdrAbDF44827695;     kKCJsfjtSdrAbDF44827695 = kKCJsfjtSdrAbDF14393006;     kKCJsfjtSdrAbDF14393006 = kKCJsfjtSdrAbDF76322529;     kKCJsfjtSdrAbDF76322529 = kKCJsfjtSdrAbDF78256365;     kKCJsfjtSdrAbDF78256365 = kKCJsfjtSdrAbDF10232475;     kKCJsfjtSdrAbDF10232475 = kKCJsfjtSdrAbDF49067738;     kKCJsfjtSdrAbDF49067738 = kKCJsfjtSdrAbDF15207779;     kKCJsfjtSdrAbDF15207779 = kKCJsfjtSdrAbDF7177340;     kKCJsfjtSdrAbDF7177340 = kKCJsfjtSdrAbDF86770883;     kKCJsfjtSdrAbDF86770883 = kKCJsfjtSdrAbDF20050086;     kKCJsfjtSdrAbDF20050086 = kKCJsfjtSdrAbDF48101288;     kKCJsfjtSdrAbDF48101288 = kKCJsfjtSdrAbDF16633921;     kKCJsfjtSdrAbDF16633921 = kKCJsfjtSdrAbDF11720562;     kKCJsfjtSdrAbDF11720562 = kKCJsfjtSdrAbDF71684824;     kKCJsfjtSdrAbDF71684824 = kKCJsfjtSdrAbDF79631503;     kKCJsfjtSdrAbDF79631503 = kKCJsfjtSdrAbDF56465099;     kKCJsfjtSdrAbDF56465099 = kKCJsfjtSdrAbDF58352679;     kKCJsfjtSdrAbDF58352679 = kKCJsfjtSdrAbDF55216958;     kKCJsfjtSdrAbDF55216958 = kKCJsfjtSdrAbDF41380317;     kKCJsfjtSdrAbDF41380317 = kKCJsfjtSdrAbDF43385465;     kKCJsfjtSdrAbDF43385465 = kKCJsfjtSdrAbDF41771030;     kKCJsfjtSdrAbDF41771030 = kKCJsfjtSdrAbDF58474112;     kKCJsfjtSdrAbDF58474112 = kKCJsfjtSdrAbDF36008962;     kKCJsfjtSdrAbDF36008962 = kKCJsfjtSdrAbDF46825542;     kKCJsfjtSdrAbDF46825542 = kKCJsfjtSdrAbDF55760844;     kKCJsfjtSdrAbDF55760844 = kKCJsfjtSdrAbDF11154174;     kKCJsfjtSdrAbDF11154174 = kKCJsfjtSdrAbDF66816970;     kKCJsfjtSdrAbDF66816970 = kKCJsfjtSdrAbDF54536969;     kKCJsfjtSdrAbDF54536969 = kKCJsfjtSdrAbDF25953758;     kKCJsfjtSdrAbDF25953758 = kKCJsfjtSdrAbDF27411538;     kKCJsfjtSdrAbDF27411538 = kKCJsfjtSdrAbDF2617427;     kKCJsfjtSdrAbDF2617427 = kKCJsfjtSdrAbDF4092891;     kKCJsfjtSdrAbDF4092891 = kKCJsfjtSdrAbDF88073677;     kKCJsfjtSdrAbDF88073677 = kKCJsfjtSdrAbDF87478991;     kKCJsfjtSdrAbDF87478991 = kKCJsfjtSdrAbDF42219641;     kKCJsfjtSdrAbDF42219641 = kKCJsfjtSdrAbDF43867334;     kKCJsfjtSdrAbDF43867334 = kKCJsfjtSdrAbDF18514775;     kKCJsfjtSdrAbDF18514775 = kKCJsfjtSdrAbDF33140279;     kKCJsfjtSdrAbDF33140279 = kKCJsfjtSdrAbDF79548310;     kKCJsfjtSdrAbDF79548310 = kKCJsfjtSdrAbDF57815226;     kKCJsfjtSdrAbDF57815226 = kKCJsfjtSdrAbDF5724681;     kKCJsfjtSdrAbDF5724681 = kKCJsfjtSdrAbDF5868951;     kKCJsfjtSdrAbDF5868951 = kKCJsfjtSdrAbDF99743306;     kKCJsfjtSdrAbDF99743306 = kKCJsfjtSdrAbDF1309770;     kKCJsfjtSdrAbDF1309770 = kKCJsfjtSdrAbDF47518665;     kKCJsfjtSdrAbDF47518665 = kKCJsfjtSdrAbDF15877049;     kKCJsfjtSdrAbDF15877049 = kKCJsfjtSdrAbDF35642759;     kKCJsfjtSdrAbDF35642759 = kKCJsfjtSdrAbDF64005854;     kKCJsfjtSdrAbDF64005854 = kKCJsfjtSdrAbDF42752879;     kKCJsfjtSdrAbDF42752879 = kKCJsfjtSdrAbDF3496369;     kKCJsfjtSdrAbDF3496369 = kKCJsfjtSdrAbDF64181810;     kKCJsfjtSdrAbDF64181810 = kKCJsfjtSdrAbDF51782909;     kKCJsfjtSdrAbDF51782909 = kKCJsfjtSdrAbDF23042772;     kKCJsfjtSdrAbDF23042772 = kKCJsfjtSdrAbDF2686110;     kKCJsfjtSdrAbDF2686110 = kKCJsfjtSdrAbDF42615946;     kKCJsfjtSdrAbDF42615946 = kKCJsfjtSdrAbDF86353583;     kKCJsfjtSdrAbDF86353583 = kKCJsfjtSdrAbDF78384043;     kKCJsfjtSdrAbDF78384043 = kKCJsfjtSdrAbDF29496987;     kKCJsfjtSdrAbDF29496987 = kKCJsfjtSdrAbDF22495522;     kKCJsfjtSdrAbDF22495522 = kKCJsfjtSdrAbDF99078301;     kKCJsfjtSdrAbDF99078301 = kKCJsfjtSdrAbDF82250768;     kKCJsfjtSdrAbDF82250768 = kKCJsfjtSdrAbDF60670810;     kKCJsfjtSdrAbDF60670810 = kKCJsfjtSdrAbDF81223582;     kKCJsfjtSdrAbDF81223582 = kKCJsfjtSdrAbDF59359345;     kKCJsfjtSdrAbDF59359345 = kKCJsfjtSdrAbDF17432660;     kKCJsfjtSdrAbDF17432660 = kKCJsfjtSdrAbDF44008398;     kKCJsfjtSdrAbDF44008398 = kKCJsfjtSdrAbDF28560244;     kKCJsfjtSdrAbDF28560244 = kKCJsfjtSdrAbDF24241570;     kKCJsfjtSdrAbDF24241570 = kKCJsfjtSdrAbDF29465183;     kKCJsfjtSdrAbDF29465183 = kKCJsfjtSdrAbDF35764169;     kKCJsfjtSdrAbDF35764169 = kKCJsfjtSdrAbDF37950324;     kKCJsfjtSdrAbDF37950324 = kKCJsfjtSdrAbDF25212401;     kKCJsfjtSdrAbDF25212401 = kKCJsfjtSdrAbDF75668648;     kKCJsfjtSdrAbDF75668648 = kKCJsfjtSdrAbDF83565090;     kKCJsfjtSdrAbDF83565090 = kKCJsfjtSdrAbDF37660784;     kKCJsfjtSdrAbDF37660784 = kKCJsfjtSdrAbDF35902080;     kKCJsfjtSdrAbDF35902080 = kKCJsfjtSdrAbDF58730805;     kKCJsfjtSdrAbDF58730805 = kKCJsfjtSdrAbDF34699192;     kKCJsfjtSdrAbDF34699192 = kKCJsfjtSdrAbDF99306877;     kKCJsfjtSdrAbDF99306877 = kKCJsfjtSdrAbDF39883795;     kKCJsfjtSdrAbDF39883795 = kKCJsfjtSdrAbDF75511414;     kKCJsfjtSdrAbDF75511414 = kKCJsfjtSdrAbDF2811116;     kKCJsfjtSdrAbDF2811116 = kKCJsfjtSdrAbDF11784091;     kKCJsfjtSdrAbDF11784091 = kKCJsfjtSdrAbDF22457390;     kKCJsfjtSdrAbDF22457390 = kKCJsfjtSdrAbDF63229727;     kKCJsfjtSdrAbDF63229727 = kKCJsfjtSdrAbDF50834518;     kKCJsfjtSdrAbDF50834518 = kKCJsfjtSdrAbDF81050118;     kKCJsfjtSdrAbDF81050118 = kKCJsfjtSdrAbDF85387567;     kKCJsfjtSdrAbDF85387567 = kKCJsfjtSdrAbDF44863046;     kKCJsfjtSdrAbDF44863046 = kKCJsfjtSdrAbDF55866058;     kKCJsfjtSdrAbDF55866058 = kKCJsfjtSdrAbDF65483291;     kKCJsfjtSdrAbDF65483291 = kKCJsfjtSdrAbDF89017788;     kKCJsfjtSdrAbDF89017788 = kKCJsfjtSdrAbDF10644758;     kKCJsfjtSdrAbDF10644758 = kKCJsfjtSdrAbDF80470008;     kKCJsfjtSdrAbDF80470008 = kKCJsfjtSdrAbDF75564457;     kKCJsfjtSdrAbDF75564457 = kKCJsfjtSdrAbDF45053871;     kKCJsfjtSdrAbDF45053871 = kKCJsfjtSdrAbDF24645368;     kKCJsfjtSdrAbDF24645368 = kKCJsfjtSdrAbDF64423088;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ltfvBXgrqrZMlASE61416080() {     int OIRpwXAqWVsWmFZ35074755 = -73925561;    int OIRpwXAqWVsWmFZ14745092 = 51004903;    int OIRpwXAqWVsWmFZ64355895 = -303681522;    int OIRpwXAqWVsWmFZ84758981 = -229653523;    int OIRpwXAqWVsWmFZ35843327 = -657993282;    int OIRpwXAqWVsWmFZ66322292 = -321633761;    int OIRpwXAqWVsWmFZ96635261 = -921899907;    int OIRpwXAqWVsWmFZ67074855 = -625810622;    int OIRpwXAqWVsWmFZ94932792 = -664683822;    int OIRpwXAqWVsWmFZ38174239 = -463838491;    int OIRpwXAqWVsWmFZ49426501 = -600647440;    int OIRpwXAqWVsWmFZ32317455 = -600485218;    int OIRpwXAqWVsWmFZ99877473 = -835684533;    int OIRpwXAqWVsWmFZ31948322 = -497177658;    int OIRpwXAqWVsWmFZ99353939 = -669216654;    int OIRpwXAqWVsWmFZ44249203 = -237892383;    int OIRpwXAqWVsWmFZ65537651 = -456136;    int OIRpwXAqWVsWmFZ99924997 = -729930446;    int OIRpwXAqWVsWmFZ38172881 = -833351758;    int OIRpwXAqWVsWmFZ17139389 = 95387609;    int OIRpwXAqWVsWmFZ10352713 = -262251023;    int OIRpwXAqWVsWmFZ2776082 = -273859508;    int OIRpwXAqWVsWmFZ45329274 = -835263579;    int OIRpwXAqWVsWmFZ75620567 = -999859835;    int OIRpwXAqWVsWmFZ22598501 = -201005263;    int OIRpwXAqWVsWmFZ66606543 = -225120677;    int OIRpwXAqWVsWmFZ55086328 = -585587410;    int OIRpwXAqWVsWmFZ63543069 = -732034114;    int OIRpwXAqWVsWmFZ43296661 = -756100301;    int OIRpwXAqWVsWmFZ5989066 = -298150994;    int OIRpwXAqWVsWmFZ61608965 = -801248645;    int OIRpwXAqWVsWmFZ70465218 = -140290200;    int OIRpwXAqWVsWmFZ10356899 = 71714433;    int OIRpwXAqWVsWmFZ67982401 = -439705856;    int OIRpwXAqWVsWmFZ82117126 = -645131490;    int OIRpwXAqWVsWmFZ48066936 = -520183627;    int OIRpwXAqWVsWmFZ31270478 = -134830496;    int OIRpwXAqWVsWmFZ95012130 = -205876231;    int OIRpwXAqWVsWmFZ22400624 = -250516140;    int OIRpwXAqWVsWmFZ13618073 = -866633485;    int OIRpwXAqWVsWmFZ93213562 = -231269690;    int OIRpwXAqWVsWmFZ8031301 = -143364251;    int OIRpwXAqWVsWmFZ6782187 = -719890559;    int OIRpwXAqWVsWmFZ12232982 = -250052138;    int OIRpwXAqWVsWmFZ1823009 = -851349579;    int OIRpwXAqWVsWmFZ6149142 = -534352483;    int OIRpwXAqWVsWmFZ27440973 = -696428820;    int OIRpwXAqWVsWmFZ13132875 = -41454205;    int OIRpwXAqWVsWmFZ21318502 = -745981114;    int OIRpwXAqWVsWmFZ44070245 = -635904421;    int OIRpwXAqWVsWmFZ32195179 = -238208786;    int OIRpwXAqWVsWmFZ84624383 = -343285378;    int OIRpwXAqWVsWmFZ58572025 = -970425194;    int OIRpwXAqWVsWmFZ54409115 = -603042828;    int OIRpwXAqWVsWmFZ51063367 = -386269984;    int OIRpwXAqWVsWmFZ32298674 = -800066054;    int OIRpwXAqWVsWmFZ69415818 = -113731518;    int OIRpwXAqWVsWmFZ88735327 = -303821688;    int OIRpwXAqWVsWmFZ62160481 = 71351739;    int OIRpwXAqWVsWmFZ69236784 = -332872606;    int OIRpwXAqWVsWmFZ11235964 = -736046352;    int OIRpwXAqWVsWmFZ33092192 = -89865794;    int OIRpwXAqWVsWmFZ23778194 = -869710321;    int OIRpwXAqWVsWmFZ88943727 = -266532829;    int OIRpwXAqWVsWmFZ76565274 = -662589847;    int OIRpwXAqWVsWmFZ78961282 = -360357241;    int OIRpwXAqWVsWmFZ21960556 = -572199651;    int OIRpwXAqWVsWmFZ31895072 = -295978677;    int OIRpwXAqWVsWmFZ49831195 = -852046168;    int OIRpwXAqWVsWmFZ51287003 = -49033028;    int OIRpwXAqWVsWmFZ12978725 = -3061888;    int OIRpwXAqWVsWmFZ70525521 = -794579906;    int OIRpwXAqWVsWmFZ77524374 = -379414307;    int OIRpwXAqWVsWmFZ24554809 = -966718274;    int OIRpwXAqWVsWmFZ23925826 = -673342701;    int OIRpwXAqWVsWmFZ2321412 = -18886772;    int OIRpwXAqWVsWmFZ95993895 = -553968950;    int OIRpwXAqWVsWmFZ33096292 = -485211442;    int OIRpwXAqWVsWmFZ73797558 = -48510257;    int OIRpwXAqWVsWmFZ16449359 = -666652780;    int OIRpwXAqWVsWmFZ39165571 = -528691858;    int OIRpwXAqWVsWmFZ41953453 = -444133205;    int OIRpwXAqWVsWmFZ42224568 = -986053000;    int OIRpwXAqWVsWmFZ99226416 = -20195881;    int OIRpwXAqWVsWmFZ73793887 = 40057792;    int OIRpwXAqWVsWmFZ76984582 = -357963267;    int OIRpwXAqWVsWmFZ11893194 = -169865006;    int OIRpwXAqWVsWmFZ55947784 = -325242739;    int OIRpwXAqWVsWmFZ16919034 = 46564128;    int OIRpwXAqWVsWmFZ49818453 = -845065437;    int OIRpwXAqWVsWmFZ78651118 = -306452109;    int OIRpwXAqWVsWmFZ42535150 = -831008809;    int OIRpwXAqWVsWmFZ32851649 = -177227970;    int OIRpwXAqWVsWmFZ53163839 = -917643535;    int OIRpwXAqWVsWmFZ2382109 = -30587134;    int OIRpwXAqWVsWmFZ60121371 = -41403897;    int OIRpwXAqWVsWmFZ84253107 = -273653931;    int OIRpwXAqWVsWmFZ17838459 = -353357730;    int OIRpwXAqWVsWmFZ35667708 = -587462292;    int OIRpwXAqWVsWmFZ22861727 = -73925561;     OIRpwXAqWVsWmFZ35074755 = OIRpwXAqWVsWmFZ14745092;     OIRpwXAqWVsWmFZ14745092 = OIRpwXAqWVsWmFZ64355895;     OIRpwXAqWVsWmFZ64355895 = OIRpwXAqWVsWmFZ84758981;     OIRpwXAqWVsWmFZ84758981 = OIRpwXAqWVsWmFZ35843327;     OIRpwXAqWVsWmFZ35843327 = OIRpwXAqWVsWmFZ66322292;     OIRpwXAqWVsWmFZ66322292 = OIRpwXAqWVsWmFZ96635261;     OIRpwXAqWVsWmFZ96635261 = OIRpwXAqWVsWmFZ67074855;     OIRpwXAqWVsWmFZ67074855 = OIRpwXAqWVsWmFZ94932792;     OIRpwXAqWVsWmFZ94932792 = OIRpwXAqWVsWmFZ38174239;     OIRpwXAqWVsWmFZ38174239 = OIRpwXAqWVsWmFZ49426501;     OIRpwXAqWVsWmFZ49426501 = OIRpwXAqWVsWmFZ32317455;     OIRpwXAqWVsWmFZ32317455 = OIRpwXAqWVsWmFZ99877473;     OIRpwXAqWVsWmFZ99877473 = OIRpwXAqWVsWmFZ31948322;     OIRpwXAqWVsWmFZ31948322 = OIRpwXAqWVsWmFZ99353939;     OIRpwXAqWVsWmFZ99353939 = OIRpwXAqWVsWmFZ44249203;     OIRpwXAqWVsWmFZ44249203 = OIRpwXAqWVsWmFZ65537651;     OIRpwXAqWVsWmFZ65537651 = OIRpwXAqWVsWmFZ99924997;     OIRpwXAqWVsWmFZ99924997 = OIRpwXAqWVsWmFZ38172881;     OIRpwXAqWVsWmFZ38172881 = OIRpwXAqWVsWmFZ17139389;     OIRpwXAqWVsWmFZ17139389 = OIRpwXAqWVsWmFZ10352713;     OIRpwXAqWVsWmFZ10352713 = OIRpwXAqWVsWmFZ2776082;     OIRpwXAqWVsWmFZ2776082 = OIRpwXAqWVsWmFZ45329274;     OIRpwXAqWVsWmFZ45329274 = OIRpwXAqWVsWmFZ75620567;     OIRpwXAqWVsWmFZ75620567 = OIRpwXAqWVsWmFZ22598501;     OIRpwXAqWVsWmFZ22598501 = OIRpwXAqWVsWmFZ66606543;     OIRpwXAqWVsWmFZ66606543 = OIRpwXAqWVsWmFZ55086328;     OIRpwXAqWVsWmFZ55086328 = OIRpwXAqWVsWmFZ63543069;     OIRpwXAqWVsWmFZ63543069 = OIRpwXAqWVsWmFZ43296661;     OIRpwXAqWVsWmFZ43296661 = OIRpwXAqWVsWmFZ5989066;     OIRpwXAqWVsWmFZ5989066 = OIRpwXAqWVsWmFZ61608965;     OIRpwXAqWVsWmFZ61608965 = OIRpwXAqWVsWmFZ70465218;     OIRpwXAqWVsWmFZ70465218 = OIRpwXAqWVsWmFZ10356899;     OIRpwXAqWVsWmFZ10356899 = OIRpwXAqWVsWmFZ67982401;     OIRpwXAqWVsWmFZ67982401 = OIRpwXAqWVsWmFZ82117126;     OIRpwXAqWVsWmFZ82117126 = OIRpwXAqWVsWmFZ48066936;     OIRpwXAqWVsWmFZ48066936 = OIRpwXAqWVsWmFZ31270478;     OIRpwXAqWVsWmFZ31270478 = OIRpwXAqWVsWmFZ95012130;     OIRpwXAqWVsWmFZ95012130 = OIRpwXAqWVsWmFZ22400624;     OIRpwXAqWVsWmFZ22400624 = OIRpwXAqWVsWmFZ13618073;     OIRpwXAqWVsWmFZ13618073 = OIRpwXAqWVsWmFZ93213562;     OIRpwXAqWVsWmFZ93213562 = OIRpwXAqWVsWmFZ8031301;     OIRpwXAqWVsWmFZ8031301 = OIRpwXAqWVsWmFZ6782187;     OIRpwXAqWVsWmFZ6782187 = OIRpwXAqWVsWmFZ12232982;     OIRpwXAqWVsWmFZ12232982 = OIRpwXAqWVsWmFZ1823009;     OIRpwXAqWVsWmFZ1823009 = OIRpwXAqWVsWmFZ6149142;     OIRpwXAqWVsWmFZ6149142 = OIRpwXAqWVsWmFZ27440973;     OIRpwXAqWVsWmFZ27440973 = OIRpwXAqWVsWmFZ13132875;     OIRpwXAqWVsWmFZ13132875 = OIRpwXAqWVsWmFZ21318502;     OIRpwXAqWVsWmFZ21318502 = OIRpwXAqWVsWmFZ44070245;     OIRpwXAqWVsWmFZ44070245 = OIRpwXAqWVsWmFZ32195179;     OIRpwXAqWVsWmFZ32195179 = OIRpwXAqWVsWmFZ84624383;     OIRpwXAqWVsWmFZ84624383 = OIRpwXAqWVsWmFZ58572025;     OIRpwXAqWVsWmFZ58572025 = OIRpwXAqWVsWmFZ54409115;     OIRpwXAqWVsWmFZ54409115 = OIRpwXAqWVsWmFZ51063367;     OIRpwXAqWVsWmFZ51063367 = OIRpwXAqWVsWmFZ32298674;     OIRpwXAqWVsWmFZ32298674 = OIRpwXAqWVsWmFZ69415818;     OIRpwXAqWVsWmFZ69415818 = OIRpwXAqWVsWmFZ88735327;     OIRpwXAqWVsWmFZ88735327 = OIRpwXAqWVsWmFZ62160481;     OIRpwXAqWVsWmFZ62160481 = OIRpwXAqWVsWmFZ69236784;     OIRpwXAqWVsWmFZ69236784 = OIRpwXAqWVsWmFZ11235964;     OIRpwXAqWVsWmFZ11235964 = OIRpwXAqWVsWmFZ33092192;     OIRpwXAqWVsWmFZ33092192 = OIRpwXAqWVsWmFZ23778194;     OIRpwXAqWVsWmFZ23778194 = OIRpwXAqWVsWmFZ88943727;     OIRpwXAqWVsWmFZ88943727 = OIRpwXAqWVsWmFZ76565274;     OIRpwXAqWVsWmFZ76565274 = OIRpwXAqWVsWmFZ78961282;     OIRpwXAqWVsWmFZ78961282 = OIRpwXAqWVsWmFZ21960556;     OIRpwXAqWVsWmFZ21960556 = OIRpwXAqWVsWmFZ31895072;     OIRpwXAqWVsWmFZ31895072 = OIRpwXAqWVsWmFZ49831195;     OIRpwXAqWVsWmFZ49831195 = OIRpwXAqWVsWmFZ51287003;     OIRpwXAqWVsWmFZ51287003 = OIRpwXAqWVsWmFZ12978725;     OIRpwXAqWVsWmFZ12978725 = OIRpwXAqWVsWmFZ70525521;     OIRpwXAqWVsWmFZ70525521 = OIRpwXAqWVsWmFZ77524374;     OIRpwXAqWVsWmFZ77524374 = OIRpwXAqWVsWmFZ24554809;     OIRpwXAqWVsWmFZ24554809 = OIRpwXAqWVsWmFZ23925826;     OIRpwXAqWVsWmFZ23925826 = OIRpwXAqWVsWmFZ2321412;     OIRpwXAqWVsWmFZ2321412 = OIRpwXAqWVsWmFZ95993895;     OIRpwXAqWVsWmFZ95993895 = OIRpwXAqWVsWmFZ33096292;     OIRpwXAqWVsWmFZ33096292 = OIRpwXAqWVsWmFZ73797558;     OIRpwXAqWVsWmFZ73797558 = OIRpwXAqWVsWmFZ16449359;     OIRpwXAqWVsWmFZ16449359 = OIRpwXAqWVsWmFZ39165571;     OIRpwXAqWVsWmFZ39165571 = OIRpwXAqWVsWmFZ41953453;     OIRpwXAqWVsWmFZ41953453 = OIRpwXAqWVsWmFZ42224568;     OIRpwXAqWVsWmFZ42224568 = OIRpwXAqWVsWmFZ99226416;     OIRpwXAqWVsWmFZ99226416 = OIRpwXAqWVsWmFZ73793887;     OIRpwXAqWVsWmFZ73793887 = OIRpwXAqWVsWmFZ76984582;     OIRpwXAqWVsWmFZ76984582 = OIRpwXAqWVsWmFZ11893194;     OIRpwXAqWVsWmFZ11893194 = OIRpwXAqWVsWmFZ55947784;     OIRpwXAqWVsWmFZ55947784 = OIRpwXAqWVsWmFZ16919034;     OIRpwXAqWVsWmFZ16919034 = OIRpwXAqWVsWmFZ49818453;     OIRpwXAqWVsWmFZ49818453 = OIRpwXAqWVsWmFZ78651118;     OIRpwXAqWVsWmFZ78651118 = OIRpwXAqWVsWmFZ42535150;     OIRpwXAqWVsWmFZ42535150 = OIRpwXAqWVsWmFZ32851649;     OIRpwXAqWVsWmFZ32851649 = OIRpwXAqWVsWmFZ53163839;     OIRpwXAqWVsWmFZ53163839 = OIRpwXAqWVsWmFZ2382109;     OIRpwXAqWVsWmFZ2382109 = OIRpwXAqWVsWmFZ60121371;     OIRpwXAqWVsWmFZ60121371 = OIRpwXAqWVsWmFZ84253107;     OIRpwXAqWVsWmFZ84253107 = OIRpwXAqWVsWmFZ17838459;     OIRpwXAqWVsWmFZ17838459 = OIRpwXAqWVsWmFZ35667708;     OIRpwXAqWVsWmFZ35667708 = OIRpwXAqWVsWmFZ22861727;     OIRpwXAqWVsWmFZ22861727 = OIRpwXAqWVsWmFZ35074755;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void wkQMcVzJILIVKqls98391252() {     int zPuSChzolVkaCFT2801004 = -243326080;    int zPuSChzolVkaCFT90667289 = -460568457;    int zPuSChzolVkaCFT33705739 = -115079367;    int zPuSChzolVkaCFT13410589 = -50499477;    int zPuSChzolVkaCFT68406012 = -944970357;    int zPuSChzolVkaCFT1836445 = -852524114;    int zPuSChzolVkaCFT36237632 = -146531007;    int zPuSChzolVkaCFT82747660 = -300016335;    int zPuSChzolVkaCFT87061677 = -215672559;    int zPuSChzolVkaCFT43945352 = -664585426;    int zPuSChzolVkaCFT28677671 = 68612661;    int zPuSChzolVkaCFT39747367 = -400284776;    int zPuSChzolVkaCFT34921253 = -572520022;    int zPuSChzolVkaCFT35440864 = -657935887;    int zPuSChzolVkaCFT51037252 = -352285569;    int zPuSChzolVkaCFT73302126 = -222196012;    int zPuSChzolVkaCFT71381998 = -723750698;    int zPuSChzolVkaCFT61305360 = -415180287;    int zPuSChzolVkaCFT98662180 = -200624603;    int zPuSChzolVkaCFT50094879 = -423905149;    int zPuSChzolVkaCFT56730856 = -496121549;    int zPuSChzolVkaCFT93634976 = -22173596;    int zPuSChzolVkaCFT49031162 = -883558593;    int zPuSChzolVkaCFT44342094 = -645732398;    int zPuSChzolVkaCFT90828820 = -976344776;    int zPuSChzolVkaCFT30242667 = -573512048;    int zPuSChzolVkaCFT70407406 = -89129337;    int zPuSChzolVkaCFT51609044 = -21403689;    int zPuSChzolVkaCFT22969992 = -266182875;    int zPuSChzolVkaCFT96800706 = -703120266;    int zPuSChzolVkaCFT87606920 = -720785506;    int zPuSChzolVkaCFT81738656 = -772686791;    int zPuSChzolVkaCFT9192116 = -103055667;    int zPuSChzolVkaCFT32998033 = -960393629;    int zPuSChzolVkaCFT6200373 = -750338581;    int zPuSChzolVkaCFT45261453 = -667349171;    int zPuSChzolVkaCFT61947070 = -117005205;    int zPuSChzolVkaCFT5578672 = -194835358;    int zPuSChzolVkaCFT55252702 = -982294998;    int zPuSChzolVkaCFT81690285 = -953980601;    int zPuSChzolVkaCFT53867200 = -709841097;    int zPuSChzolVkaCFT45562695 = -587532959;    int zPuSChzolVkaCFT81615555 = -226046081;    int zPuSChzolVkaCFT61914600 = -330700565;    int zPuSChzolVkaCFT93840264 = 98765139;    int zPuSChzolVkaCFT99343789 = -459836653;    int zPuSChzolVkaCFT50962451 = -629062311;    int zPuSChzolVkaCFT9687822 = -429098181;    int zPuSChzolVkaCFT92094029 = -359731509;    int zPuSChzolVkaCFT16357752 = -124930267;    int zPuSChzolVkaCFT79210225 = -514624569;    int zPuSChzolVkaCFT83640748 = -641184430;    int zPuSChzolVkaCFT80606774 = -794474972;    int zPuSChzolVkaCFT67530174 = -414757004;    int zPuSChzolVkaCFT41660396 = -987964701;    int zPuSChzolVkaCFT9166027 = -121152484;    int zPuSChzolVkaCFT41636128 = -577009865;    int zPuSChzolVkaCFT89363645 = -469346969;    int zPuSChzolVkaCFT22581769 = -74154701;    int zPuSChzolVkaCFT38163346 = -271458309;    int zPuSChzolVkaCFT31429038 = -663394778;    int zPuSChzolVkaCFT84628588 = -25127319;    int zPuSChzolVkaCFT59777669 = 66166540;    int zPuSChzolVkaCFT90260971 = -512552294;    int zPuSChzolVkaCFT56338432 = -943799920;    int zPuSChzolVkaCFT46939014 = -158700549;    int zPuSChzolVkaCFT30555252 = -197229109;    int zPuSChzolVkaCFT1923220 = -612126394;    int zPuSChzolVkaCFT29240491 = -907597307;    int zPuSChzolVkaCFT5775800 = -684936398;    int zPuSChzolVkaCFT11355057 = -5190808;    int zPuSChzolVkaCFT65803327 = -428915340;    int zPuSChzolVkaCFT6052658 = -432885290;    int zPuSChzolVkaCFT16971896 = -246644003;    int zPuSChzolVkaCFT96227679 = -714064053;    int zPuSChzolVkaCFT11168162 = -908588591;    int zPuSChzolVkaCFT12019422 = -796127516;    int zPuSChzolVkaCFT87116562 = -452858028;    int zPuSChzolVkaCFT50501829 = -644497537;    int zPuSChzolVkaCFT91485031 = -416508123;    int zPuSChzolVkaCFT79280215 = -944449738;    int zPuSChzolVkaCFT60719584 = -660031156;    int zPuSChzolVkaCFT59515015 = -661672180;    int zPuSChzolVkaCFT6612241 = -41252609;    int zPuSChzolVkaCFT17590481 = -88495697;    int zPuSChzolVkaCFT3966173 = 20398923;    int zPuSChzolVkaCFT1131883 = -978211819;    int zPuSChzolVkaCFT41661942 = -688298664;    int zPuSChzolVkaCFT91337637 = -972428929;    int zPuSChzolVkaCFT97034345 = -529186098;    int zPuSChzolVkaCFT3625325 = 9660693;    int zPuSChzolVkaCFT72583424 = -647658237;    int zPuSChzolVkaCFT82996903 = -20680657;    int zPuSChzolVkaCFT17089357 = -610836690;    int zPuSChzolVkaCFT50261247 = -190585823;    int zPuSChzolVkaCFT69238612 = -584713778;    int zPuSChzolVkaCFT85785025 = -553699499;    int zPuSChzolVkaCFT91354584 = -713493787;    int zPuSChzolVkaCFT5576168 = -386900645;    int zPuSChzolVkaCFT46901250 = -243326080;     zPuSChzolVkaCFT2801004 = zPuSChzolVkaCFT90667289;     zPuSChzolVkaCFT90667289 = zPuSChzolVkaCFT33705739;     zPuSChzolVkaCFT33705739 = zPuSChzolVkaCFT13410589;     zPuSChzolVkaCFT13410589 = zPuSChzolVkaCFT68406012;     zPuSChzolVkaCFT68406012 = zPuSChzolVkaCFT1836445;     zPuSChzolVkaCFT1836445 = zPuSChzolVkaCFT36237632;     zPuSChzolVkaCFT36237632 = zPuSChzolVkaCFT82747660;     zPuSChzolVkaCFT82747660 = zPuSChzolVkaCFT87061677;     zPuSChzolVkaCFT87061677 = zPuSChzolVkaCFT43945352;     zPuSChzolVkaCFT43945352 = zPuSChzolVkaCFT28677671;     zPuSChzolVkaCFT28677671 = zPuSChzolVkaCFT39747367;     zPuSChzolVkaCFT39747367 = zPuSChzolVkaCFT34921253;     zPuSChzolVkaCFT34921253 = zPuSChzolVkaCFT35440864;     zPuSChzolVkaCFT35440864 = zPuSChzolVkaCFT51037252;     zPuSChzolVkaCFT51037252 = zPuSChzolVkaCFT73302126;     zPuSChzolVkaCFT73302126 = zPuSChzolVkaCFT71381998;     zPuSChzolVkaCFT71381998 = zPuSChzolVkaCFT61305360;     zPuSChzolVkaCFT61305360 = zPuSChzolVkaCFT98662180;     zPuSChzolVkaCFT98662180 = zPuSChzolVkaCFT50094879;     zPuSChzolVkaCFT50094879 = zPuSChzolVkaCFT56730856;     zPuSChzolVkaCFT56730856 = zPuSChzolVkaCFT93634976;     zPuSChzolVkaCFT93634976 = zPuSChzolVkaCFT49031162;     zPuSChzolVkaCFT49031162 = zPuSChzolVkaCFT44342094;     zPuSChzolVkaCFT44342094 = zPuSChzolVkaCFT90828820;     zPuSChzolVkaCFT90828820 = zPuSChzolVkaCFT30242667;     zPuSChzolVkaCFT30242667 = zPuSChzolVkaCFT70407406;     zPuSChzolVkaCFT70407406 = zPuSChzolVkaCFT51609044;     zPuSChzolVkaCFT51609044 = zPuSChzolVkaCFT22969992;     zPuSChzolVkaCFT22969992 = zPuSChzolVkaCFT96800706;     zPuSChzolVkaCFT96800706 = zPuSChzolVkaCFT87606920;     zPuSChzolVkaCFT87606920 = zPuSChzolVkaCFT81738656;     zPuSChzolVkaCFT81738656 = zPuSChzolVkaCFT9192116;     zPuSChzolVkaCFT9192116 = zPuSChzolVkaCFT32998033;     zPuSChzolVkaCFT32998033 = zPuSChzolVkaCFT6200373;     zPuSChzolVkaCFT6200373 = zPuSChzolVkaCFT45261453;     zPuSChzolVkaCFT45261453 = zPuSChzolVkaCFT61947070;     zPuSChzolVkaCFT61947070 = zPuSChzolVkaCFT5578672;     zPuSChzolVkaCFT5578672 = zPuSChzolVkaCFT55252702;     zPuSChzolVkaCFT55252702 = zPuSChzolVkaCFT81690285;     zPuSChzolVkaCFT81690285 = zPuSChzolVkaCFT53867200;     zPuSChzolVkaCFT53867200 = zPuSChzolVkaCFT45562695;     zPuSChzolVkaCFT45562695 = zPuSChzolVkaCFT81615555;     zPuSChzolVkaCFT81615555 = zPuSChzolVkaCFT61914600;     zPuSChzolVkaCFT61914600 = zPuSChzolVkaCFT93840264;     zPuSChzolVkaCFT93840264 = zPuSChzolVkaCFT99343789;     zPuSChzolVkaCFT99343789 = zPuSChzolVkaCFT50962451;     zPuSChzolVkaCFT50962451 = zPuSChzolVkaCFT9687822;     zPuSChzolVkaCFT9687822 = zPuSChzolVkaCFT92094029;     zPuSChzolVkaCFT92094029 = zPuSChzolVkaCFT16357752;     zPuSChzolVkaCFT16357752 = zPuSChzolVkaCFT79210225;     zPuSChzolVkaCFT79210225 = zPuSChzolVkaCFT83640748;     zPuSChzolVkaCFT83640748 = zPuSChzolVkaCFT80606774;     zPuSChzolVkaCFT80606774 = zPuSChzolVkaCFT67530174;     zPuSChzolVkaCFT67530174 = zPuSChzolVkaCFT41660396;     zPuSChzolVkaCFT41660396 = zPuSChzolVkaCFT9166027;     zPuSChzolVkaCFT9166027 = zPuSChzolVkaCFT41636128;     zPuSChzolVkaCFT41636128 = zPuSChzolVkaCFT89363645;     zPuSChzolVkaCFT89363645 = zPuSChzolVkaCFT22581769;     zPuSChzolVkaCFT22581769 = zPuSChzolVkaCFT38163346;     zPuSChzolVkaCFT38163346 = zPuSChzolVkaCFT31429038;     zPuSChzolVkaCFT31429038 = zPuSChzolVkaCFT84628588;     zPuSChzolVkaCFT84628588 = zPuSChzolVkaCFT59777669;     zPuSChzolVkaCFT59777669 = zPuSChzolVkaCFT90260971;     zPuSChzolVkaCFT90260971 = zPuSChzolVkaCFT56338432;     zPuSChzolVkaCFT56338432 = zPuSChzolVkaCFT46939014;     zPuSChzolVkaCFT46939014 = zPuSChzolVkaCFT30555252;     zPuSChzolVkaCFT30555252 = zPuSChzolVkaCFT1923220;     zPuSChzolVkaCFT1923220 = zPuSChzolVkaCFT29240491;     zPuSChzolVkaCFT29240491 = zPuSChzolVkaCFT5775800;     zPuSChzolVkaCFT5775800 = zPuSChzolVkaCFT11355057;     zPuSChzolVkaCFT11355057 = zPuSChzolVkaCFT65803327;     zPuSChzolVkaCFT65803327 = zPuSChzolVkaCFT6052658;     zPuSChzolVkaCFT6052658 = zPuSChzolVkaCFT16971896;     zPuSChzolVkaCFT16971896 = zPuSChzolVkaCFT96227679;     zPuSChzolVkaCFT96227679 = zPuSChzolVkaCFT11168162;     zPuSChzolVkaCFT11168162 = zPuSChzolVkaCFT12019422;     zPuSChzolVkaCFT12019422 = zPuSChzolVkaCFT87116562;     zPuSChzolVkaCFT87116562 = zPuSChzolVkaCFT50501829;     zPuSChzolVkaCFT50501829 = zPuSChzolVkaCFT91485031;     zPuSChzolVkaCFT91485031 = zPuSChzolVkaCFT79280215;     zPuSChzolVkaCFT79280215 = zPuSChzolVkaCFT60719584;     zPuSChzolVkaCFT60719584 = zPuSChzolVkaCFT59515015;     zPuSChzolVkaCFT59515015 = zPuSChzolVkaCFT6612241;     zPuSChzolVkaCFT6612241 = zPuSChzolVkaCFT17590481;     zPuSChzolVkaCFT17590481 = zPuSChzolVkaCFT3966173;     zPuSChzolVkaCFT3966173 = zPuSChzolVkaCFT1131883;     zPuSChzolVkaCFT1131883 = zPuSChzolVkaCFT41661942;     zPuSChzolVkaCFT41661942 = zPuSChzolVkaCFT91337637;     zPuSChzolVkaCFT91337637 = zPuSChzolVkaCFT97034345;     zPuSChzolVkaCFT97034345 = zPuSChzolVkaCFT3625325;     zPuSChzolVkaCFT3625325 = zPuSChzolVkaCFT72583424;     zPuSChzolVkaCFT72583424 = zPuSChzolVkaCFT82996903;     zPuSChzolVkaCFT82996903 = zPuSChzolVkaCFT17089357;     zPuSChzolVkaCFT17089357 = zPuSChzolVkaCFT50261247;     zPuSChzolVkaCFT50261247 = zPuSChzolVkaCFT69238612;     zPuSChzolVkaCFT69238612 = zPuSChzolVkaCFT85785025;     zPuSChzolVkaCFT85785025 = zPuSChzolVkaCFT91354584;     zPuSChzolVkaCFT91354584 = zPuSChzolVkaCFT5576168;     zPuSChzolVkaCFT5576168 = zPuSChzolVkaCFT46901250;     zPuSChzolVkaCFT46901250 = zPuSChzolVkaCFT2801004;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void wVTfXuJdWkTajRcO52807465() {     float pOqSzeASrnpyhgt73452671 = -861475210;    float pOqSzeASrnpyhgt59340807 = -332211756;    float pOqSzeASrnpyhgt13674659 = -557373826;    float pOqSzeASrnpyhgt53341876 = -240637866;    float pOqSzeASrnpyhgt89856334 = -728376704;    float pOqSzeASrnpyhgt91836206 = -554105266;    float pOqSzeASrnpyhgt54616528 = -145306717;    float pOqSzeASrnpyhgt39590041 = -646833425;    float pOqSzeASrnpyhgt32926732 = -309019888;    float pOqSzeASrnpyhgt66911813 = -965680776;    float pOqSzeASrnpyhgt70926831 = 33710731;    float pOqSzeASrnpyhgt85293938 = -716048569;    float pOqSzeASrnpyhgt14748640 = -482057081;    float pOqSzeASrnpyhgt19287897 = -458964097;    float pOqSzeASrnpyhgt33757272 = -502035573;    float pOqSzeASrnpyhgt5830767 = -892179599;    float pOqSzeASrnpyhgt65234825 = -632935828;    float pOqSzeASrnpyhgt81598854 = -404346483;    float pOqSzeASrnpyhgt80369962 = -907812545;    float pOqSzeASrnpyhgt8881589 = -966271454;    float pOqSzeASrnpyhgt11866611 = -404800690;    float pOqSzeASrnpyhgt55030742 = -353458902;    float pOqSzeASrnpyhgt50974970 = -351401737;    float pOqSzeASrnpyhgt78191630 = -238133305;    float pOqSzeASrnpyhgt54953209 = -652249464;    float pOqSzeASrnpyhgt60840247 = -671796493;    float pOqSzeASrnpyhgt78668191 = -484307112;    float pOqSzeASrnpyhgt59391269 = -526527456;    float pOqSzeASrnpyhgt55112480 = -560541092;    float pOqSzeASrnpyhgt35972802 = -161515336;    float pOqSzeASrnpyhgt94678916 = -469219191;    float pOqSzeASrnpyhgt26250117 = -983953;    float pOqSzeASrnpyhgt92137476 = -50017276;    float pOqSzeASrnpyhgt98363007 = -523923313;    float pOqSzeASrnpyhgt84224608 = -597285360;    float pOqSzeASrnpyhgt5254712 = -746575719;    float pOqSzeASrnpyhgt5738556 = -305598412;    float pOqSzeASrnpyhgt58371160 = 21051535;    float pOqSzeASrnpyhgt33785991 = -938023635;    float pOqSzeASrnpyhgt76793583 = -471588963;    float pOqSzeASrnpyhgt13940485 = 6140025;    float pOqSzeASrnpyhgt74045686 = -356922028;    float pOqSzeASrnpyhgt30582516 = -212800133;    float pOqSzeASrnpyhgt68422900 = -402675537;    float pOqSzeASrnpyhgt89794323 = -209881256;    float pOqSzeASrnpyhgt5749625 = -218173808;    float pOqSzeASrnpyhgt77093654 = -745132110;    float pOqSzeASrnpyhgt75302031 = -881228476;    float pOqSzeASrnpyhgt97535481 = -345495479;    float pOqSzeASrnpyhgt24785238 = -407939146;    float pOqSzeASrnpyhgt47399550 = -61196690;    float pOqSzeASrnpyhgt25512253 = -610404151;    float pOqSzeASrnpyhgt35682431 = -722273014;    float pOqSzeASrnpyhgt57757478 = 24361500;    float pOqSzeASrnpyhgt40940855 = 1094385;    float pOqSzeASrnpyhgt18421929 = -408016309;    float pOqSzeASrnpyhgt8365837 = -980810020;    float pOqSzeASrnpyhgt35483028 = -219240522;    float pOqSzeASrnpyhgt98388666 = -588388403;    float pOqSzeASrnpyhgt29016087 = 43419788;    float pOqSzeASrnpyhgt13168015 = 30201845;    float pOqSzeASrnpyhgt95225258 = -618779261;    float pOqSzeASrnpyhgt84477561 = 13707667;    float pOqSzeASrnpyhgt96953929 = -47504552;    float pOqSzeASrnpyhgt72232896 = -396461586;    float pOqSzeASrnpyhgt44676714 = -965305316;    float pOqSzeASrnpyhgt93156461 = -566031293;    float pOqSzeASrnpyhgt16385632 = -958133768;    float pOqSzeASrnpyhgt35063289 = -861678738;    float pOqSzeASrnpyhgt28502560 = -755459854;    float pOqSzeASrnpyhgt92211 = -486581187;    float pOqSzeASrnpyhgt6863666 = -553987363;    float pOqSzeASrnpyhgt47812863 = -466322849;    float pOqSzeASrnpyhgt3576379 = -336223582;    float pOqSzeASrnpyhgt94941104 = -872411480;    float pOqSzeASrnpyhgt37820925 = 52121338;    float pOqSzeASrnpyhgt24448227 = -40658770;    float pOqSzeASrnpyhgt82552070 = -948726201;    float pOqSzeASrnpyhgt88397307 = 71747951;    float pOqSzeASrnpyhgt49203585 = -334075656;    float pOqSzeASrnpyhgt83746593 = -926664383;    float pOqSzeASrnpyhgt3366161 = -603078636;    float pOqSzeASrnpyhgt61855788 = -81031978;    float pOqSzeASrnpyhgt30327242 = -52601947;    float pOqSzeASrnpyhgt88573252 = -318647;    float pOqSzeASrnpyhgt69166664 = -858815041;    float pOqSzeASrnpyhgt90567686 = -278710940;    float pOqSzeASrnpyhgt34379998 = 25621224;    float pOqSzeASrnpyhgt57422153 = -425017698;    float pOqSzeASrnpyhgt65802680 = -89269052;    float pOqSzeASrnpyhgt96888874 = -765765700;    float pOqSzeASrnpyhgt70255528 = 13642109;    float pOqSzeASrnpyhgt59982493 = -390560063;    float pOqSzeASrnpyhgt4769905 = -881443424;    float pOqSzeASrnpyhgt63625569 = -401790809;    float pOqSzeASrnpyhgt18715226 = -375080714;    float pOqSzeASrnpyhgt89568124 = -270629695;    float pOqSzeASrnpyhgt33628586 = -65295581;    float pOqSzeASrnpyhgt96190004 = 93786048;    float pOqSzeASrnpyhgt45117609 = -861475210;     pOqSzeASrnpyhgt73452671 = pOqSzeASrnpyhgt59340807;     pOqSzeASrnpyhgt59340807 = pOqSzeASrnpyhgt13674659;     pOqSzeASrnpyhgt13674659 = pOqSzeASrnpyhgt53341876;     pOqSzeASrnpyhgt53341876 = pOqSzeASrnpyhgt89856334;     pOqSzeASrnpyhgt89856334 = pOqSzeASrnpyhgt91836206;     pOqSzeASrnpyhgt91836206 = pOqSzeASrnpyhgt54616528;     pOqSzeASrnpyhgt54616528 = pOqSzeASrnpyhgt39590041;     pOqSzeASrnpyhgt39590041 = pOqSzeASrnpyhgt32926732;     pOqSzeASrnpyhgt32926732 = pOqSzeASrnpyhgt66911813;     pOqSzeASrnpyhgt66911813 = pOqSzeASrnpyhgt70926831;     pOqSzeASrnpyhgt70926831 = pOqSzeASrnpyhgt85293938;     pOqSzeASrnpyhgt85293938 = pOqSzeASrnpyhgt14748640;     pOqSzeASrnpyhgt14748640 = pOqSzeASrnpyhgt19287897;     pOqSzeASrnpyhgt19287897 = pOqSzeASrnpyhgt33757272;     pOqSzeASrnpyhgt33757272 = pOqSzeASrnpyhgt5830767;     pOqSzeASrnpyhgt5830767 = pOqSzeASrnpyhgt65234825;     pOqSzeASrnpyhgt65234825 = pOqSzeASrnpyhgt81598854;     pOqSzeASrnpyhgt81598854 = pOqSzeASrnpyhgt80369962;     pOqSzeASrnpyhgt80369962 = pOqSzeASrnpyhgt8881589;     pOqSzeASrnpyhgt8881589 = pOqSzeASrnpyhgt11866611;     pOqSzeASrnpyhgt11866611 = pOqSzeASrnpyhgt55030742;     pOqSzeASrnpyhgt55030742 = pOqSzeASrnpyhgt50974970;     pOqSzeASrnpyhgt50974970 = pOqSzeASrnpyhgt78191630;     pOqSzeASrnpyhgt78191630 = pOqSzeASrnpyhgt54953209;     pOqSzeASrnpyhgt54953209 = pOqSzeASrnpyhgt60840247;     pOqSzeASrnpyhgt60840247 = pOqSzeASrnpyhgt78668191;     pOqSzeASrnpyhgt78668191 = pOqSzeASrnpyhgt59391269;     pOqSzeASrnpyhgt59391269 = pOqSzeASrnpyhgt55112480;     pOqSzeASrnpyhgt55112480 = pOqSzeASrnpyhgt35972802;     pOqSzeASrnpyhgt35972802 = pOqSzeASrnpyhgt94678916;     pOqSzeASrnpyhgt94678916 = pOqSzeASrnpyhgt26250117;     pOqSzeASrnpyhgt26250117 = pOqSzeASrnpyhgt92137476;     pOqSzeASrnpyhgt92137476 = pOqSzeASrnpyhgt98363007;     pOqSzeASrnpyhgt98363007 = pOqSzeASrnpyhgt84224608;     pOqSzeASrnpyhgt84224608 = pOqSzeASrnpyhgt5254712;     pOqSzeASrnpyhgt5254712 = pOqSzeASrnpyhgt5738556;     pOqSzeASrnpyhgt5738556 = pOqSzeASrnpyhgt58371160;     pOqSzeASrnpyhgt58371160 = pOqSzeASrnpyhgt33785991;     pOqSzeASrnpyhgt33785991 = pOqSzeASrnpyhgt76793583;     pOqSzeASrnpyhgt76793583 = pOqSzeASrnpyhgt13940485;     pOqSzeASrnpyhgt13940485 = pOqSzeASrnpyhgt74045686;     pOqSzeASrnpyhgt74045686 = pOqSzeASrnpyhgt30582516;     pOqSzeASrnpyhgt30582516 = pOqSzeASrnpyhgt68422900;     pOqSzeASrnpyhgt68422900 = pOqSzeASrnpyhgt89794323;     pOqSzeASrnpyhgt89794323 = pOqSzeASrnpyhgt5749625;     pOqSzeASrnpyhgt5749625 = pOqSzeASrnpyhgt77093654;     pOqSzeASrnpyhgt77093654 = pOqSzeASrnpyhgt75302031;     pOqSzeASrnpyhgt75302031 = pOqSzeASrnpyhgt97535481;     pOqSzeASrnpyhgt97535481 = pOqSzeASrnpyhgt24785238;     pOqSzeASrnpyhgt24785238 = pOqSzeASrnpyhgt47399550;     pOqSzeASrnpyhgt47399550 = pOqSzeASrnpyhgt25512253;     pOqSzeASrnpyhgt25512253 = pOqSzeASrnpyhgt35682431;     pOqSzeASrnpyhgt35682431 = pOqSzeASrnpyhgt57757478;     pOqSzeASrnpyhgt57757478 = pOqSzeASrnpyhgt40940855;     pOqSzeASrnpyhgt40940855 = pOqSzeASrnpyhgt18421929;     pOqSzeASrnpyhgt18421929 = pOqSzeASrnpyhgt8365837;     pOqSzeASrnpyhgt8365837 = pOqSzeASrnpyhgt35483028;     pOqSzeASrnpyhgt35483028 = pOqSzeASrnpyhgt98388666;     pOqSzeASrnpyhgt98388666 = pOqSzeASrnpyhgt29016087;     pOqSzeASrnpyhgt29016087 = pOqSzeASrnpyhgt13168015;     pOqSzeASrnpyhgt13168015 = pOqSzeASrnpyhgt95225258;     pOqSzeASrnpyhgt95225258 = pOqSzeASrnpyhgt84477561;     pOqSzeASrnpyhgt84477561 = pOqSzeASrnpyhgt96953929;     pOqSzeASrnpyhgt96953929 = pOqSzeASrnpyhgt72232896;     pOqSzeASrnpyhgt72232896 = pOqSzeASrnpyhgt44676714;     pOqSzeASrnpyhgt44676714 = pOqSzeASrnpyhgt93156461;     pOqSzeASrnpyhgt93156461 = pOqSzeASrnpyhgt16385632;     pOqSzeASrnpyhgt16385632 = pOqSzeASrnpyhgt35063289;     pOqSzeASrnpyhgt35063289 = pOqSzeASrnpyhgt28502560;     pOqSzeASrnpyhgt28502560 = pOqSzeASrnpyhgt92211;     pOqSzeASrnpyhgt92211 = pOqSzeASrnpyhgt6863666;     pOqSzeASrnpyhgt6863666 = pOqSzeASrnpyhgt47812863;     pOqSzeASrnpyhgt47812863 = pOqSzeASrnpyhgt3576379;     pOqSzeASrnpyhgt3576379 = pOqSzeASrnpyhgt94941104;     pOqSzeASrnpyhgt94941104 = pOqSzeASrnpyhgt37820925;     pOqSzeASrnpyhgt37820925 = pOqSzeASrnpyhgt24448227;     pOqSzeASrnpyhgt24448227 = pOqSzeASrnpyhgt82552070;     pOqSzeASrnpyhgt82552070 = pOqSzeASrnpyhgt88397307;     pOqSzeASrnpyhgt88397307 = pOqSzeASrnpyhgt49203585;     pOqSzeASrnpyhgt49203585 = pOqSzeASrnpyhgt83746593;     pOqSzeASrnpyhgt83746593 = pOqSzeASrnpyhgt3366161;     pOqSzeASrnpyhgt3366161 = pOqSzeASrnpyhgt61855788;     pOqSzeASrnpyhgt61855788 = pOqSzeASrnpyhgt30327242;     pOqSzeASrnpyhgt30327242 = pOqSzeASrnpyhgt88573252;     pOqSzeASrnpyhgt88573252 = pOqSzeASrnpyhgt69166664;     pOqSzeASrnpyhgt69166664 = pOqSzeASrnpyhgt90567686;     pOqSzeASrnpyhgt90567686 = pOqSzeASrnpyhgt34379998;     pOqSzeASrnpyhgt34379998 = pOqSzeASrnpyhgt57422153;     pOqSzeASrnpyhgt57422153 = pOqSzeASrnpyhgt65802680;     pOqSzeASrnpyhgt65802680 = pOqSzeASrnpyhgt96888874;     pOqSzeASrnpyhgt96888874 = pOqSzeASrnpyhgt70255528;     pOqSzeASrnpyhgt70255528 = pOqSzeASrnpyhgt59982493;     pOqSzeASrnpyhgt59982493 = pOqSzeASrnpyhgt4769905;     pOqSzeASrnpyhgt4769905 = pOqSzeASrnpyhgt63625569;     pOqSzeASrnpyhgt63625569 = pOqSzeASrnpyhgt18715226;     pOqSzeASrnpyhgt18715226 = pOqSzeASrnpyhgt89568124;     pOqSzeASrnpyhgt89568124 = pOqSzeASrnpyhgt33628586;     pOqSzeASrnpyhgt33628586 = pOqSzeASrnpyhgt96190004;     pOqSzeASrnpyhgt96190004 = pOqSzeASrnpyhgt45117609;     pOqSzeASrnpyhgt45117609 = pOqSzeASrnpyhgt73452671;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void jeALdzOafwkzwyPK28819586() {     float zoosTsJdkkMhfjK60374348 = 97873585;    float zoosTsJdkkMhfjK68988545 = -442798330;    float zoosTsJdkkMhfjK6370673 = 11510791;    float zoosTsJdkkMhfjK81205070 = -246435157;    float zoosTsJdkkMhfjK40585421 = -582190182;    float zoosTsJdkkMhfjK5301884 = -951798560;    float zoosTsJdkkMhfjK4662197 = -927104756;    float zoosTsJdkkMhfjK22306390 = -77373246;    float zoosTsJdkkMhfjK11312422 = -518530590;    float zoosTsJdkkMhfjK40412199 = -680541982;    float zoosTsJdkkMhfjK90607561 = -120378012;    float zoosTsJdkkMhfjK54920416 = -838151453;    float zoosTsJdkkMhfjK75375089 = -570420370;    float zoosTsJdkkMhfjK62606006 = -255462496;    float zoosTsJdkkMhfjK57470141 = -688801122;    float zoosTsJdkkMhfjK41109925 = -870831185;    float zoosTsJdkkMhfjK15075001 = -202855665;    float zoosTsJdkkMhfjK10815612 = -79732727;    float zoosTsJdkkMhfjK41529533 = -244333515;    float zoosTsJdkkMhfjK65634416 = -304369293;    float zoosTsJdkkMhfjK79332279 = -449479683;    float zoosTsJdkkMhfjK93720701 = -181580804;    float zoosTsJdkkMhfjK20621311 = -248807987;    float zoosTsJdkkMhfjK54548581 = -172222080;    float zoosTsJdkkMhfjK10918195 = -676517243;    float zoosTsJdkkMhfjK55019147 = -235319839;    float zoosTsJdkkMhfjK35558620 = -736409177;    float zoosTsJdkkMhfjK59977820 = -754176724;    float zoosTsJdkkMhfjK28015273 = -976773724;    float zoosTsJdkkMhfjK15686441 = -822735406;    float zoosTsJdkkMhfjK53799169 = -385648091;    float zoosTsJdkkMhfjK30692147 = -171905655;    float zoosTsJdkkMhfjK46410559 = -633709009;    float zoosTsJdkkMhfjK28286106 = -660038082;    float zoosTsJdkkMhfjK77003557 = -969255457;    float zoosTsJdkkMhfjK38214927 = -927171536;    float zoosTsJdkkMhfjK61707819 = -273503702;    float zoosTsJdkkMhfjK30699537 = -959181035;    float zoosTsJdkkMhfjK87017157 = -200874806;    float zoosTsJdkkMhfjK4580659 = -751982133;    float zoosTsJdkkMhfjK97101915 = -815782625;    float zoosTsJdkkMhfjK58886611 = -714077510;    float zoosTsJdkkMhfjK7032690 = -6280185;    float zoosTsJdkkMhfjK86967579 = -452671219;    float zoosTsJdkkMhfjK41779183 = -665772974;    float zoosTsJdkkMhfjK8316547 = -173523942;    float zoosTsJdkkMhfjK97743680 = -220836625;    float zoosTsJdkkMhfjK77557975 = 20001770;    float zoosTsJdkkMhfjK87761109 = -195239158;    float zoosTsJdkkMhfjK17384817 = -745957475;    float zoosTsJdkkMhfjK88757412 = -120551417;    float zoosTsJdkkMhfjK30425295 = -109716835;    float zoosTsJdkkMhfjK20824034 = -530192699;    float zoosTsJdkkMhfjK12302448 = -591730661;    float zoosTsJdkkMhfjK49487306 = -650018862;    float zoosTsJdkkMhfjK66653647 = -720545611;    float zoosTsJdkkMhfjK48367235 = -93990343;    float zoosTsJdkkMhfjK51822091 = -816267129;    float zoosTsJdkkMhfjK70286876 = -569917914;    float zoosTsJdkkMhfjK85566274 = -246870344;    float zoosTsJdkkMhfjK69743264 = -115389384;    float zoosTsJdkkMhfjK44684377 = -72928033;    float zoosTsJdkkMhfjK94291116 = -100599523;    float zoosTsJdkkMhfjK95625981 = -695795184;    float zoosTsJdkkMhfjK86613029 = -194893892;    float zoosTsJdkkMhfjK59915415 = -948472358;    float zoosTsJdkkMhfjK8509857 = -104442444;    float zoosTsJdkkMhfjK47088983 = -910382288;    float zoosTsJdkkMhfjK85602448 = -286207040;    float zoosTsJdkkMhfjK19255215 = -761629587;    float zoosTsJdkkMhfjK79402106 = -497327483;    float zoosTsJdkkMhfjK84375463 = -243674630;    float zoosTsJdkkMhfjK23798455 = -878857922;    float zoosTsJdkkMhfjK36948874 = -492351383;    float zoosTsJdkkMhfjK68532501 = -488586668;    float zoosTsJdkkMhfjK20445668 = -735402173;    float zoosTsJdkkMhfjK86688012 = -75300620;    float zoosTsJdkkMhfjK33653731 = -796136769;    float zoosTsJdkkMhfjK12769398 = -506449107;    float zoosTsJdkkMhfjK2601649 = -402993302;    float zoosTsJdkkMhfjK57275467 = 85516786;    float zoosTsJdkkMhfjK58000644 = -656410947;    float zoosTsJdkkMhfjK72216710 = -458937567;    float zoosTsJdkkMhfjK10630456 = -130816249;    float zoosTsJdkkMhfjK26929028 = -602183990;    float zoosTsJdkkMhfjK23373875 = -175931256;    float zoosTsJdkkMhfjK9868114 = -641712957;    float zoosTsJdkkMhfjK34108112 = 58021651;    float zoosTsJdkkMhfjK78798800 = 89980780;    float zoosTsJdkkMhfjK10349911 = -148709847;    float zoosTsJdkkMhfjK89847691 = -733181194;    float zoosTsJdkkMhfjK9885728 = -457236573;    float zoosTsJdkkMhfjK60412661 = -289263122;    float zoosTsJdkkMhfjK1450884 = -954004463;    float zoosTsJdkkMhfjK34837395 = -536592749;    float zoosTsJdkkMhfjK52417538 = -642854593;    float zoosTsJdkkMhfjK64595494 = -513477988;    float zoosTsJdkkMhfjK11406708 = -310485001;    float zoosTsJdkkMhfjK354550 = -157777328;    float zoosTsJdkkMhfjK81863768 = 97873585;     zoosTsJdkkMhfjK60374348 = zoosTsJdkkMhfjK68988545;     zoosTsJdkkMhfjK68988545 = zoosTsJdkkMhfjK6370673;     zoosTsJdkkMhfjK6370673 = zoosTsJdkkMhfjK81205070;     zoosTsJdkkMhfjK81205070 = zoosTsJdkkMhfjK40585421;     zoosTsJdkkMhfjK40585421 = zoosTsJdkkMhfjK5301884;     zoosTsJdkkMhfjK5301884 = zoosTsJdkkMhfjK4662197;     zoosTsJdkkMhfjK4662197 = zoosTsJdkkMhfjK22306390;     zoosTsJdkkMhfjK22306390 = zoosTsJdkkMhfjK11312422;     zoosTsJdkkMhfjK11312422 = zoosTsJdkkMhfjK40412199;     zoosTsJdkkMhfjK40412199 = zoosTsJdkkMhfjK90607561;     zoosTsJdkkMhfjK90607561 = zoosTsJdkkMhfjK54920416;     zoosTsJdkkMhfjK54920416 = zoosTsJdkkMhfjK75375089;     zoosTsJdkkMhfjK75375089 = zoosTsJdkkMhfjK62606006;     zoosTsJdkkMhfjK62606006 = zoosTsJdkkMhfjK57470141;     zoosTsJdkkMhfjK57470141 = zoosTsJdkkMhfjK41109925;     zoosTsJdkkMhfjK41109925 = zoosTsJdkkMhfjK15075001;     zoosTsJdkkMhfjK15075001 = zoosTsJdkkMhfjK10815612;     zoosTsJdkkMhfjK10815612 = zoosTsJdkkMhfjK41529533;     zoosTsJdkkMhfjK41529533 = zoosTsJdkkMhfjK65634416;     zoosTsJdkkMhfjK65634416 = zoosTsJdkkMhfjK79332279;     zoosTsJdkkMhfjK79332279 = zoosTsJdkkMhfjK93720701;     zoosTsJdkkMhfjK93720701 = zoosTsJdkkMhfjK20621311;     zoosTsJdkkMhfjK20621311 = zoosTsJdkkMhfjK54548581;     zoosTsJdkkMhfjK54548581 = zoosTsJdkkMhfjK10918195;     zoosTsJdkkMhfjK10918195 = zoosTsJdkkMhfjK55019147;     zoosTsJdkkMhfjK55019147 = zoosTsJdkkMhfjK35558620;     zoosTsJdkkMhfjK35558620 = zoosTsJdkkMhfjK59977820;     zoosTsJdkkMhfjK59977820 = zoosTsJdkkMhfjK28015273;     zoosTsJdkkMhfjK28015273 = zoosTsJdkkMhfjK15686441;     zoosTsJdkkMhfjK15686441 = zoosTsJdkkMhfjK53799169;     zoosTsJdkkMhfjK53799169 = zoosTsJdkkMhfjK30692147;     zoosTsJdkkMhfjK30692147 = zoosTsJdkkMhfjK46410559;     zoosTsJdkkMhfjK46410559 = zoosTsJdkkMhfjK28286106;     zoosTsJdkkMhfjK28286106 = zoosTsJdkkMhfjK77003557;     zoosTsJdkkMhfjK77003557 = zoosTsJdkkMhfjK38214927;     zoosTsJdkkMhfjK38214927 = zoosTsJdkkMhfjK61707819;     zoosTsJdkkMhfjK61707819 = zoosTsJdkkMhfjK30699537;     zoosTsJdkkMhfjK30699537 = zoosTsJdkkMhfjK87017157;     zoosTsJdkkMhfjK87017157 = zoosTsJdkkMhfjK4580659;     zoosTsJdkkMhfjK4580659 = zoosTsJdkkMhfjK97101915;     zoosTsJdkkMhfjK97101915 = zoosTsJdkkMhfjK58886611;     zoosTsJdkkMhfjK58886611 = zoosTsJdkkMhfjK7032690;     zoosTsJdkkMhfjK7032690 = zoosTsJdkkMhfjK86967579;     zoosTsJdkkMhfjK86967579 = zoosTsJdkkMhfjK41779183;     zoosTsJdkkMhfjK41779183 = zoosTsJdkkMhfjK8316547;     zoosTsJdkkMhfjK8316547 = zoosTsJdkkMhfjK97743680;     zoosTsJdkkMhfjK97743680 = zoosTsJdkkMhfjK77557975;     zoosTsJdkkMhfjK77557975 = zoosTsJdkkMhfjK87761109;     zoosTsJdkkMhfjK87761109 = zoosTsJdkkMhfjK17384817;     zoosTsJdkkMhfjK17384817 = zoosTsJdkkMhfjK88757412;     zoosTsJdkkMhfjK88757412 = zoosTsJdkkMhfjK30425295;     zoosTsJdkkMhfjK30425295 = zoosTsJdkkMhfjK20824034;     zoosTsJdkkMhfjK20824034 = zoosTsJdkkMhfjK12302448;     zoosTsJdkkMhfjK12302448 = zoosTsJdkkMhfjK49487306;     zoosTsJdkkMhfjK49487306 = zoosTsJdkkMhfjK66653647;     zoosTsJdkkMhfjK66653647 = zoosTsJdkkMhfjK48367235;     zoosTsJdkkMhfjK48367235 = zoosTsJdkkMhfjK51822091;     zoosTsJdkkMhfjK51822091 = zoosTsJdkkMhfjK70286876;     zoosTsJdkkMhfjK70286876 = zoosTsJdkkMhfjK85566274;     zoosTsJdkkMhfjK85566274 = zoosTsJdkkMhfjK69743264;     zoosTsJdkkMhfjK69743264 = zoosTsJdkkMhfjK44684377;     zoosTsJdkkMhfjK44684377 = zoosTsJdkkMhfjK94291116;     zoosTsJdkkMhfjK94291116 = zoosTsJdkkMhfjK95625981;     zoosTsJdkkMhfjK95625981 = zoosTsJdkkMhfjK86613029;     zoosTsJdkkMhfjK86613029 = zoosTsJdkkMhfjK59915415;     zoosTsJdkkMhfjK59915415 = zoosTsJdkkMhfjK8509857;     zoosTsJdkkMhfjK8509857 = zoosTsJdkkMhfjK47088983;     zoosTsJdkkMhfjK47088983 = zoosTsJdkkMhfjK85602448;     zoosTsJdkkMhfjK85602448 = zoosTsJdkkMhfjK19255215;     zoosTsJdkkMhfjK19255215 = zoosTsJdkkMhfjK79402106;     zoosTsJdkkMhfjK79402106 = zoosTsJdkkMhfjK84375463;     zoosTsJdkkMhfjK84375463 = zoosTsJdkkMhfjK23798455;     zoosTsJdkkMhfjK23798455 = zoosTsJdkkMhfjK36948874;     zoosTsJdkkMhfjK36948874 = zoosTsJdkkMhfjK68532501;     zoosTsJdkkMhfjK68532501 = zoosTsJdkkMhfjK20445668;     zoosTsJdkkMhfjK20445668 = zoosTsJdkkMhfjK86688012;     zoosTsJdkkMhfjK86688012 = zoosTsJdkkMhfjK33653731;     zoosTsJdkkMhfjK33653731 = zoosTsJdkkMhfjK12769398;     zoosTsJdkkMhfjK12769398 = zoosTsJdkkMhfjK2601649;     zoosTsJdkkMhfjK2601649 = zoosTsJdkkMhfjK57275467;     zoosTsJdkkMhfjK57275467 = zoosTsJdkkMhfjK58000644;     zoosTsJdkkMhfjK58000644 = zoosTsJdkkMhfjK72216710;     zoosTsJdkkMhfjK72216710 = zoosTsJdkkMhfjK10630456;     zoosTsJdkkMhfjK10630456 = zoosTsJdkkMhfjK26929028;     zoosTsJdkkMhfjK26929028 = zoosTsJdkkMhfjK23373875;     zoosTsJdkkMhfjK23373875 = zoosTsJdkkMhfjK9868114;     zoosTsJdkkMhfjK9868114 = zoosTsJdkkMhfjK34108112;     zoosTsJdkkMhfjK34108112 = zoosTsJdkkMhfjK78798800;     zoosTsJdkkMhfjK78798800 = zoosTsJdkkMhfjK10349911;     zoosTsJdkkMhfjK10349911 = zoosTsJdkkMhfjK89847691;     zoosTsJdkkMhfjK89847691 = zoosTsJdkkMhfjK9885728;     zoosTsJdkkMhfjK9885728 = zoosTsJdkkMhfjK60412661;     zoosTsJdkkMhfjK60412661 = zoosTsJdkkMhfjK1450884;     zoosTsJdkkMhfjK1450884 = zoosTsJdkkMhfjK34837395;     zoosTsJdkkMhfjK34837395 = zoosTsJdkkMhfjK52417538;     zoosTsJdkkMhfjK52417538 = zoosTsJdkkMhfjK64595494;     zoosTsJdkkMhfjK64595494 = zoosTsJdkkMhfjK11406708;     zoosTsJdkkMhfjK11406708 = zoosTsJdkkMhfjK354550;     zoosTsJdkkMhfjK354550 = zoosTsJdkkMhfjK81863768;     zoosTsJdkkMhfjK81863768 = zoosTsJdkkMhfjK60374348;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void sgaacABXLJGCYYtC35478330() {     float EbCLAqYnHCvJzRc60367877 = -874650558;    float EbCLAqYnHCvJzRc7560862 = -485393461;    float EbCLAqYnHCvJzRc7683885 = 54382177;    float EbCLAqYnHCvJzRc66284931 = -66975984;    float EbCLAqYnHCvJzRc49425523 = 49454489;    float EbCLAqYnHCvJzRc40107316 = -651231372;    float EbCLAqYnHCvJzRc73209532 = -631641223;    float EbCLAqYnHCvJzRc91520440 = -881550579;    float EbCLAqYnHCvJzRc94052586 = -232176661;    float EbCLAqYnHCvJzRc37051713 = -317348853;    float EbCLAqYnHCvJzRc10928167 = -79850085;    float EbCLAqYnHCvJzRc69212093 = -573629825;    float EbCLAqYnHCvJzRc7228003 = -592078844;    float EbCLAqYnHCvJzRc16450227 = -600615549;    float EbCLAqYnHCvJzRc2642251 = -651513993;    float EbCLAqYnHCvJzRc15674473 = -103626835;    float EbCLAqYnHCvJzRc70927760 = -22470234;    float EbCLAqYnHCvJzRc33816145 = -476804353;    float EbCLAqYnHCvJzRc61957803 = -862315781;    float EbCLAqYnHCvJzRc37708179 = -916393744;    float EbCLAqYnHCvJzRc59001703 = -159946062;    float EbCLAqYnHCvJzRc72016967 = -691572688;    float EbCLAqYnHCvJzRc57499707 = -707765829;    float EbCLAqYnHCvJzRc98198689 = -53142603;    float EbCLAqYnHCvJzRc39360884 = -3211109;    float EbCLAqYnHCvJzRc71593224 = -143525763;    float EbCLAqYnHCvJzRc5780202 = 62791111;    float EbCLAqYnHCvJzRc95381344 = -263143718;    float EbCLAqYnHCvJzRc40693720 = -522844023;    float EbCLAqYnHCvJzRc91776310 = -498166780;    float EbCLAqYnHCvJzRc87211848 = -772741325;    float EbCLAqYnHCvJzRc15416005 = -563727414;    float EbCLAqYnHCvJzRc31862983 = -835653222;    float EbCLAqYnHCvJzRc28568943 = -536719815;    float EbCLAqYnHCvJzRc59361596 = -128569379;    float EbCLAqYnHCvJzRc81043116 = 93062736;    float EbCLAqYnHCvJzRc73649187 = -373157080;    float EbCLAqYnHCvJzRc617217 = -954443717;    float EbCLAqYnHCvJzRc22330755 = -913556210;    float EbCLAqYnHCvJzRc76453551 = -361413819;    float EbCLAqYnHCvJzRc84957582 = -903726524;    float EbCLAqYnHCvJzRc44584272 = -907869572;    float EbCLAqYnHCvJzRc67316049 = -565410437;    float EbCLAqYnHCvJzRc46199478 = -9635663;    float EbCLAqYnHCvJzRc25797236 = -39032374;    float EbCLAqYnHCvJzRc48744514 = 14431406;    float EbCLAqYnHCvJzRc25441473 = -702117247;    float EbCLAqYnHCvJzRc52941556 = -588759585;    float EbCLAqYnHCvJzRc6419500 = -859002995;    float EbCLAqYnHCvJzRc37430240 = -332982367;    float EbCLAqYnHCvJzRc2016783 = -799106424;    float EbCLAqYnHCvJzRc44972552 = -491862579;    float EbCLAqYnHCvJzRc96272381 = -422246710;    float EbCLAqYnHCvJzRc22552719 = -23650512;    float EbCLAqYnHCvJzRc76476626 = -406918135;    float EbCLAqYnHCvJzRc88350910 = -83077871;    float EbCLAqYnHCvJzRc50061154 = -777627632;    float EbCLAqYnHCvJzRc9485195 = -892475220;    float EbCLAqYnHCvJzRc26924047 = 36235124;    float EbCLAqYnHCvJzRc77832298 = -807019749;    float EbCLAqYnHCvJzRc34327115 = -614022483;    float EbCLAqYnHCvJzRc77828188 = -268497505;    float EbCLAqYnHCvJzRc50826721 = -258706556;    float EbCLAqYnHCvJzRc2276276 = -734009881;    float EbCLAqYnHCvJzRc49839865 = -544607529;    float EbCLAqYnHCvJzRc95512162 = -516122672;    float EbCLAqYnHCvJzRc37349111 = -737976603;    float EbCLAqYnHCvJzRc78659059 = 44640970;    float EbCLAqYnHCvJzRc57088631 = -372046171;    float EbCLAqYnHCvJzRc21599134 = -644576729;    float EbCLAqYnHCvJzRc42025286 = -730469756;    float EbCLAqYnHCvJzRc70310543 = -68026518;    float EbCLAqYnHCvJzRc11485391 = -563248144;    float EbCLAqYnHCvJzRc85504252 = -400901963;    float EbCLAqYnHCvJzRc52750597 = 87332779;    float EbCLAqYnHCvJzRc14417432 = -252076490;    float EbCLAqYnHCvJzRc4700918 = -26162251;    float EbCLAqYnHCvJzRc11300230 = -598130167;    float EbCLAqYnHCvJzRc72401453 = 85889771;    float EbCLAqYnHCvJzRc90616369 = 82357485;    float EbCLAqYnHCvJzRc46151752 = -441408516;    float EbCLAqYnHCvJzRc52838645 = -348449305;    float EbCLAqYnHCvJzRc88961845 = -404140724;    float EbCLAqYnHCvJzRc3263480 = -89861657;    float EbCLAqYnHCvJzRc89759528 = -699060356;    float EbCLAqYnHCvJzRc42239296 = -180878747;    float EbCLAqYnHCvJzRc19143623 = -41480704;    float EbCLAqYnHCvJzRc9310264 = -712002711;    float EbCLAqYnHCvJzRc52092317 = -29801680;    float EbCLAqYnHCvJzRc71010685 = 54508492;    float EbCLAqYnHCvJzRc30981962 = -129309632;    float EbCLAqYnHCvJzRc64163992 = -480681860;    float EbCLAqYnHCvJzRc73693169 = -890678842;    float EbCLAqYnHCvJzRc44498456 = -6536462;    float EbCLAqYnHCvJzRc42126436 = -747391337;    float EbCLAqYnHCvJzRc7129395 = -535229019;    float EbCLAqYnHCvJzRc93757551 = -549163017;    float EbCLAqYnHCvJzRc65039773 = -831400556;    float EbCLAqYnHCvJzRc96359613 = -465028134;    float EbCLAqYnHCvJzRc30285073 = -874650558;     EbCLAqYnHCvJzRc60367877 = EbCLAqYnHCvJzRc7560862;     EbCLAqYnHCvJzRc7560862 = EbCLAqYnHCvJzRc7683885;     EbCLAqYnHCvJzRc7683885 = EbCLAqYnHCvJzRc66284931;     EbCLAqYnHCvJzRc66284931 = EbCLAqYnHCvJzRc49425523;     EbCLAqYnHCvJzRc49425523 = EbCLAqYnHCvJzRc40107316;     EbCLAqYnHCvJzRc40107316 = EbCLAqYnHCvJzRc73209532;     EbCLAqYnHCvJzRc73209532 = EbCLAqYnHCvJzRc91520440;     EbCLAqYnHCvJzRc91520440 = EbCLAqYnHCvJzRc94052586;     EbCLAqYnHCvJzRc94052586 = EbCLAqYnHCvJzRc37051713;     EbCLAqYnHCvJzRc37051713 = EbCLAqYnHCvJzRc10928167;     EbCLAqYnHCvJzRc10928167 = EbCLAqYnHCvJzRc69212093;     EbCLAqYnHCvJzRc69212093 = EbCLAqYnHCvJzRc7228003;     EbCLAqYnHCvJzRc7228003 = EbCLAqYnHCvJzRc16450227;     EbCLAqYnHCvJzRc16450227 = EbCLAqYnHCvJzRc2642251;     EbCLAqYnHCvJzRc2642251 = EbCLAqYnHCvJzRc15674473;     EbCLAqYnHCvJzRc15674473 = EbCLAqYnHCvJzRc70927760;     EbCLAqYnHCvJzRc70927760 = EbCLAqYnHCvJzRc33816145;     EbCLAqYnHCvJzRc33816145 = EbCLAqYnHCvJzRc61957803;     EbCLAqYnHCvJzRc61957803 = EbCLAqYnHCvJzRc37708179;     EbCLAqYnHCvJzRc37708179 = EbCLAqYnHCvJzRc59001703;     EbCLAqYnHCvJzRc59001703 = EbCLAqYnHCvJzRc72016967;     EbCLAqYnHCvJzRc72016967 = EbCLAqYnHCvJzRc57499707;     EbCLAqYnHCvJzRc57499707 = EbCLAqYnHCvJzRc98198689;     EbCLAqYnHCvJzRc98198689 = EbCLAqYnHCvJzRc39360884;     EbCLAqYnHCvJzRc39360884 = EbCLAqYnHCvJzRc71593224;     EbCLAqYnHCvJzRc71593224 = EbCLAqYnHCvJzRc5780202;     EbCLAqYnHCvJzRc5780202 = EbCLAqYnHCvJzRc95381344;     EbCLAqYnHCvJzRc95381344 = EbCLAqYnHCvJzRc40693720;     EbCLAqYnHCvJzRc40693720 = EbCLAqYnHCvJzRc91776310;     EbCLAqYnHCvJzRc91776310 = EbCLAqYnHCvJzRc87211848;     EbCLAqYnHCvJzRc87211848 = EbCLAqYnHCvJzRc15416005;     EbCLAqYnHCvJzRc15416005 = EbCLAqYnHCvJzRc31862983;     EbCLAqYnHCvJzRc31862983 = EbCLAqYnHCvJzRc28568943;     EbCLAqYnHCvJzRc28568943 = EbCLAqYnHCvJzRc59361596;     EbCLAqYnHCvJzRc59361596 = EbCLAqYnHCvJzRc81043116;     EbCLAqYnHCvJzRc81043116 = EbCLAqYnHCvJzRc73649187;     EbCLAqYnHCvJzRc73649187 = EbCLAqYnHCvJzRc617217;     EbCLAqYnHCvJzRc617217 = EbCLAqYnHCvJzRc22330755;     EbCLAqYnHCvJzRc22330755 = EbCLAqYnHCvJzRc76453551;     EbCLAqYnHCvJzRc76453551 = EbCLAqYnHCvJzRc84957582;     EbCLAqYnHCvJzRc84957582 = EbCLAqYnHCvJzRc44584272;     EbCLAqYnHCvJzRc44584272 = EbCLAqYnHCvJzRc67316049;     EbCLAqYnHCvJzRc67316049 = EbCLAqYnHCvJzRc46199478;     EbCLAqYnHCvJzRc46199478 = EbCLAqYnHCvJzRc25797236;     EbCLAqYnHCvJzRc25797236 = EbCLAqYnHCvJzRc48744514;     EbCLAqYnHCvJzRc48744514 = EbCLAqYnHCvJzRc25441473;     EbCLAqYnHCvJzRc25441473 = EbCLAqYnHCvJzRc52941556;     EbCLAqYnHCvJzRc52941556 = EbCLAqYnHCvJzRc6419500;     EbCLAqYnHCvJzRc6419500 = EbCLAqYnHCvJzRc37430240;     EbCLAqYnHCvJzRc37430240 = EbCLAqYnHCvJzRc2016783;     EbCLAqYnHCvJzRc2016783 = EbCLAqYnHCvJzRc44972552;     EbCLAqYnHCvJzRc44972552 = EbCLAqYnHCvJzRc96272381;     EbCLAqYnHCvJzRc96272381 = EbCLAqYnHCvJzRc22552719;     EbCLAqYnHCvJzRc22552719 = EbCLAqYnHCvJzRc76476626;     EbCLAqYnHCvJzRc76476626 = EbCLAqYnHCvJzRc88350910;     EbCLAqYnHCvJzRc88350910 = EbCLAqYnHCvJzRc50061154;     EbCLAqYnHCvJzRc50061154 = EbCLAqYnHCvJzRc9485195;     EbCLAqYnHCvJzRc9485195 = EbCLAqYnHCvJzRc26924047;     EbCLAqYnHCvJzRc26924047 = EbCLAqYnHCvJzRc77832298;     EbCLAqYnHCvJzRc77832298 = EbCLAqYnHCvJzRc34327115;     EbCLAqYnHCvJzRc34327115 = EbCLAqYnHCvJzRc77828188;     EbCLAqYnHCvJzRc77828188 = EbCLAqYnHCvJzRc50826721;     EbCLAqYnHCvJzRc50826721 = EbCLAqYnHCvJzRc2276276;     EbCLAqYnHCvJzRc2276276 = EbCLAqYnHCvJzRc49839865;     EbCLAqYnHCvJzRc49839865 = EbCLAqYnHCvJzRc95512162;     EbCLAqYnHCvJzRc95512162 = EbCLAqYnHCvJzRc37349111;     EbCLAqYnHCvJzRc37349111 = EbCLAqYnHCvJzRc78659059;     EbCLAqYnHCvJzRc78659059 = EbCLAqYnHCvJzRc57088631;     EbCLAqYnHCvJzRc57088631 = EbCLAqYnHCvJzRc21599134;     EbCLAqYnHCvJzRc21599134 = EbCLAqYnHCvJzRc42025286;     EbCLAqYnHCvJzRc42025286 = EbCLAqYnHCvJzRc70310543;     EbCLAqYnHCvJzRc70310543 = EbCLAqYnHCvJzRc11485391;     EbCLAqYnHCvJzRc11485391 = EbCLAqYnHCvJzRc85504252;     EbCLAqYnHCvJzRc85504252 = EbCLAqYnHCvJzRc52750597;     EbCLAqYnHCvJzRc52750597 = EbCLAqYnHCvJzRc14417432;     EbCLAqYnHCvJzRc14417432 = EbCLAqYnHCvJzRc4700918;     EbCLAqYnHCvJzRc4700918 = EbCLAqYnHCvJzRc11300230;     EbCLAqYnHCvJzRc11300230 = EbCLAqYnHCvJzRc72401453;     EbCLAqYnHCvJzRc72401453 = EbCLAqYnHCvJzRc90616369;     EbCLAqYnHCvJzRc90616369 = EbCLAqYnHCvJzRc46151752;     EbCLAqYnHCvJzRc46151752 = EbCLAqYnHCvJzRc52838645;     EbCLAqYnHCvJzRc52838645 = EbCLAqYnHCvJzRc88961845;     EbCLAqYnHCvJzRc88961845 = EbCLAqYnHCvJzRc3263480;     EbCLAqYnHCvJzRc3263480 = EbCLAqYnHCvJzRc89759528;     EbCLAqYnHCvJzRc89759528 = EbCLAqYnHCvJzRc42239296;     EbCLAqYnHCvJzRc42239296 = EbCLAqYnHCvJzRc19143623;     EbCLAqYnHCvJzRc19143623 = EbCLAqYnHCvJzRc9310264;     EbCLAqYnHCvJzRc9310264 = EbCLAqYnHCvJzRc52092317;     EbCLAqYnHCvJzRc52092317 = EbCLAqYnHCvJzRc71010685;     EbCLAqYnHCvJzRc71010685 = EbCLAqYnHCvJzRc30981962;     EbCLAqYnHCvJzRc30981962 = EbCLAqYnHCvJzRc64163992;     EbCLAqYnHCvJzRc64163992 = EbCLAqYnHCvJzRc73693169;     EbCLAqYnHCvJzRc73693169 = EbCLAqYnHCvJzRc44498456;     EbCLAqYnHCvJzRc44498456 = EbCLAqYnHCvJzRc42126436;     EbCLAqYnHCvJzRc42126436 = EbCLAqYnHCvJzRc7129395;     EbCLAqYnHCvJzRc7129395 = EbCLAqYnHCvJzRc93757551;     EbCLAqYnHCvJzRc93757551 = EbCLAqYnHCvJzRc65039773;     EbCLAqYnHCvJzRc65039773 = EbCLAqYnHCvJzRc96359613;     EbCLAqYnHCvJzRc96359613 = EbCLAqYnHCvJzRc30285073;     EbCLAqYnHCvJzRc30285073 = EbCLAqYnHCvJzRc60367877;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ZSTkOoDguuImdwrX20210971() {     float CBZjHMTqyYSNzCh98752263 = -689676068;    float CBZjHMTqyYSNzCh13584260 = -826015001;    float CBZjHMTqyYSNzCh55689436 = -242181513;    float CBZjHMTqyYSNzCh49787965 = -257419494;    float CBZjHMTqyYSNzCh94598428 = -652573621;    float CBZjHMTqyYSNzCh30815799 = -84270065;    float CBZjHMTqyYSNzCh62643463 = -150511567;    float CBZjHMTqyYSNzCh94821575 = -98396080;    float CBZjHMTqyYSNzCh49306361 = -162866658;    float CBZjHMTqyYSNzCh69149772 = -82384268;    float CBZjHMTqyYSNzCh12107893 = -586019843;    float CBZjHMTqyYSNzCh7896900 = -953714822;    float CBZjHMTqyYSNzCh90246255 = -216792918;    float CBZjHMTqyYSNzCh49945582 = -217248938;    float CBZjHMTqyYSNzCh91873473 = -521620078;    float CBZjHMTqyYSNzCh2691490 = -425118400;    float CBZjHMTqyYSNzCh14772175 = -835335356;    float CBZjHMTqyYSNzCh92489468 = -854148773;    float CBZjHMTqyYSNzCh83726615 = -318794300;    float CBZjHMTqyYSNzCh57376616 = -266028356;    float CBZjHMTqyYSNzCh80846177 = -592029360;    float CBZjHMTqyYSNzCh45975361 = -261180199;    float CBZjHMTqyYSNzCh26267008 = -864946145;    float CBZjHMTqyYSNzCh57119645 = -510495550;    float CBZjHMTqyYSNzCh43272903 = -27761469;    float CBZjHMTqyYSNzCh49252853 = -681995647;    float CBZjHMTqyYSNzCh59140483 = -635128879;    float CBZjHMTqyYSNzCh55826020 = -548670079;    float CBZjHMTqyYSNzCh39831091 = -781214484;    float CBZjHMTqyYSNzCh45670177 = -686099749;    float CBZjHMTqyYSNzCh86869121 = -53618637;    float CBZjHMTqyYSNzCh86477045 = -32599403;    float CBZjHMTqyYSNzCh28191137 = -755440712;    float CBZjHMTqyYSNzCh58666713 = -744255540;    float CBZjHMTqyYSNzCh79111040 = -921409321;    float CBZjHMTqyYSNzCh95402702 = -53563592;    float CBZjHMTqyYSNzCh36175897 = -444271619;    float CBZjHMTqyYSNzCh94058567 = -732253276;    float CBZjHMTqyYSNzCh98402525 = -888382277;    float CBZjHMTqyYSNzCh67756169 = -356937611;    float CBZjHMTqyYSNzCh17828837 = -578372910;    float CBZjHMTqyYSNzCh24900997 = -927635246;    float CBZjHMTqyYSNzCh30833018 = -599189755;    float CBZjHMTqyYSNzCh43157498 = -605294617;    float CBZjHMTqyYSNzCh29750498 = -24304649;    float CBZjHMTqyYSNzCh7917030 = -957345230;    float CBZjHMTqyYSNzCh47396361 = -269539916;    float CBZjHMTqyYSNzCh39727132 = -819772499;    float CBZjHMTqyYSNzCh63978090 = -894753473;    float CBZjHMTqyYSNzCh98099809 = -517992210;    float CBZjHMTqyYSNzCh3961784 = 56460680;    float CBZjHMTqyYSNzCh71313164 = -376835600;    float CBZjHMTqyYSNzCh97934438 = -282040525;    float CBZjHMTqyYSNzCh15650811 = 35673667;    float CBZjHMTqyYSNzCh39364793 = -262654483;    float CBZjHMTqyYSNzCh52776903 = -328495870;    float CBZjHMTqyYSNzCh87317252 = -961068856;    float CBZjHMTqyYSNzCh98569791 = -731685964;    float CBZjHMTqyYSNzCh6515062 = -129658025;    float CBZjHMTqyYSNzCh45345576 = -970577974;    float CBZjHMTqyYSNzCh71675315 = -449141187;    float CBZjHMTqyYSNzCh6817444 = -601841488;    float CBZjHMTqyYSNzCh54990485 = -317181596;    float CBZjHMTqyYSNzCh3636185 = -476766909;    float CBZjHMTqyYSNzCh82280651 = 71234369;    float CBZjHMTqyYSNzCh25630847 = -453420441;    float CBZjHMTqyYSNzCh79705762 = -98274111;    float CBZjHMTqyYSNzCh31579543 = -472537379;    float CBZjHMTqyYSNzCh70834542 = -295839618;    float CBZjHMTqyYSNzCh96470770 = -368056486;    float CBZjHMTqyYSNzCh66515593 = -980846782;    float CBZjHMTqyYSNzCh20713608 = -3082081;    float CBZjHMTqyYSNzCh94086942 = -965766496;    float CBZjHMTqyYSNzCh15970446 = -961856689;    float CBZjHMTqyYSNzCh39547780 = -687655447;    float CBZjHMTqyYSNzCh55945181 = -664394115;    float CBZjHMTqyYSNzCh15142343 = -661990444;    float CBZjHMTqyYSNzCh83109509 = -159651528;    float CBZjHMTqyYSNzCh27369147 = -386190901;    float CBZjHMTqyYSNzCh35355874 = -70416239;    float CBZjHMTqyYSNzCh1856492 = -312455732;    float CBZjHMTqyYSNzCh19413352 = -815356380;    float CBZjHMTqyYSNzCh91847930 = -653916606;    float CBZjHMTqyYSNzCh41731282 = -163222275;    float CBZjHMTqyYSNzCh41708393 = -642560429;    float CBZjHMTqyYSNzCh15555957 = -676783037;    float CBZjHMTqyYSNzCh88542606 = -750558879;    float CBZjHMTqyYSNzCh12540327 = -691114379;    float CBZjHMTqyYSNzCh19301920 = -381601057;    float CBZjHMTqyYSNzCh26334137 = -492913452;    float CBZjHMTqyYSNzCh8085450 = -92494736;    float CBZjHMTqyYSNzCh37606106 = -712585655;    float CBZjHMTqyYSNzCh87543505 = -502595251;    float CBZjHMTqyYSNzCh53056950 = -917804303;    float CBZjHMTqyYSNzCh96080853 = -907796425;    float CBZjHMTqyYSNzCh11011394 = -976531422;    float CBZjHMTqyYSNzCh69910511 = -510453650;    float CBZjHMTqyYSNzCh27196834 = -22422846;    float CBZjHMTqyYSNzCh60876846 = -576528987;    float CBZjHMTqyYSNzCh4119651 = -689676068;     CBZjHMTqyYSNzCh98752263 = CBZjHMTqyYSNzCh13584260;     CBZjHMTqyYSNzCh13584260 = CBZjHMTqyYSNzCh55689436;     CBZjHMTqyYSNzCh55689436 = CBZjHMTqyYSNzCh49787965;     CBZjHMTqyYSNzCh49787965 = CBZjHMTqyYSNzCh94598428;     CBZjHMTqyYSNzCh94598428 = CBZjHMTqyYSNzCh30815799;     CBZjHMTqyYSNzCh30815799 = CBZjHMTqyYSNzCh62643463;     CBZjHMTqyYSNzCh62643463 = CBZjHMTqyYSNzCh94821575;     CBZjHMTqyYSNzCh94821575 = CBZjHMTqyYSNzCh49306361;     CBZjHMTqyYSNzCh49306361 = CBZjHMTqyYSNzCh69149772;     CBZjHMTqyYSNzCh69149772 = CBZjHMTqyYSNzCh12107893;     CBZjHMTqyYSNzCh12107893 = CBZjHMTqyYSNzCh7896900;     CBZjHMTqyYSNzCh7896900 = CBZjHMTqyYSNzCh90246255;     CBZjHMTqyYSNzCh90246255 = CBZjHMTqyYSNzCh49945582;     CBZjHMTqyYSNzCh49945582 = CBZjHMTqyYSNzCh91873473;     CBZjHMTqyYSNzCh91873473 = CBZjHMTqyYSNzCh2691490;     CBZjHMTqyYSNzCh2691490 = CBZjHMTqyYSNzCh14772175;     CBZjHMTqyYSNzCh14772175 = CBZjHMTqyYSNzCh92489468;     CBZjHMTqyYSNzCh92489468 = CBZjHMTqyYSNzCh83726615;     CBZjHMTqyYSNzCh83726615 = CBZjHMTqyYSNzCh57376616;     CBZjHMTqyYSNzCh57376616 = CBZjHMTqyYSNzCh80846177;     CBZjHMTqyYSNzCh80846177 = CBZjHMTqyYSNzCh45975361;     CBZjHMTqyYSNzCh45975361 = CBZjHMTqyYSNzCh26267008;     CBZjHMTqyYSNzCh26267008 = CBZjHMTqyYSNzCh57119645;     CBZjHMTqyYSNzCh57119645 = CBZjHMTqyYSNzCh43272903;     CBZjHMTqyYSNzCh43272903 = CBZjHMTqyYSNzCh49252853;     CBZjHMTqyYSNzCh49252853 = CBZjHMTqyYSNzCh59140483;     CBZjHMTqyYSNzCh59140483 = CBZjHMTqyYSNzCh55826020;     CBZjHMTqyYSNzCh55826020 = CBZjHMTqyYSNzCh39831091;     CBZjHMTqyYSNzCh39831091 = CBZjHMTqyYSNzCh45670177;     CBZjHMTqyYSNzCh45670177 = CBZjHMTqyYSNzCh86869121;     CBZjHMTqyYSNzCh86869121 = CBZjHMTqyYSNzCh86477045;     CBZjHMTqyYSNzCh86477045 = CBZjHMTqyYSNzCh28191137;     CBZjHMTqyYSNzCh28191137 = CBZjHMTqyYSNzCh58666713;     CBZjHMTqyYSNzCh58666713 = CBZjHMTqyYSNzCh79111040;     CBZjHMTqyYSNzCh79111040 = CBZjHMTqyYSNzCh95402702;     CBZjHMTqyYSNzCh95402702 = CBZjHMTqyYSNzCh36175897;     CBZjHMTqyYSNzCh36175897 = CBZjHMTqyYSNzCh94058567;     CBZjHMTqyYSNzCh94058567 = CBZjHMTqyYSNzCh98402525;     CBZjHMTqyYSNzCh98402525 = CBZjHMTqyYSNzCh67756169;     CBZjHMTqyYSNzCh67756169 = CBZjHMTqyYSNzCh17828837;     CBZjHMTqyYSNzCh17828837 = CBZjHMTqyYSNzCh24900997;     CBZjHMTqyYSNzCh24900997 = CBZjHMTqyYSNzCh30833018;     CBZjHMTqyYSNzCh30833018 = CBZjHMTqyYSNzCh43157498;     CBZjHMTqyYSNzCh43157498 = CBZjHMTqyYSNzCh29750498;     CBZjHMTqyYSNzCh29750498 = CBZjHMTqyYSNzCh7917030;     CBZjHMTqyYSNzCh7917030 = CBZjHMTqyYSNzCh47396361;     CBZjHMTqyYSNzCh47396361 = CBZjHMTqyYSNzCh39727132;     CBZjHMTqyYSNzCh39727132 = CBZjHMTqyYSNzCh63978090;     CBZjHMTqyYSNzCh63978090 = CBZjHMTqyYSNzCh98099809;     CBZjHMTqyYSNzCh98099809 = CBZjHMTqyYSNzCh3961784;     CBZjHMTqyYSNzCh3961784 = CBZjHMTqyYSNzCh71313164;     CBZjHMTqyYSNzCh71313164 = CBZjHMTqyYSNzCh97934438;     CBZjHMTqyYSNzCh97934438 = CBZjHMTqyYSNzCh15650811;     CBZjHMTqyYSNzCh15650811 = CBZjHMTqyYSNzCh39364793;     CBZjHMTqyYSNzCh39364793 = CBZjHMTqyYSNzCh52776903;     CBZjHMTqyYSNzCh52776903 = CBZjHMTqyYSNzCh87317252;     CBZjHMTqyYSNzCh87317252 = CBZjHMTqyYSNzCh98569791;     CBZjHMTqyYSNzCh98569791 = CBZjHMTqyYSNzCh6515062;     CBZjHMTqyYSNzCh6515062 = CBZjHMTqyYSNzCh45345576;     CBZjHMTqyYSNzCh45345576 = CBZjHMTqyYSNzCh71675315;     CBZjHMTqyYSNzCh71675315 = CBZjHMTqyYSNzCh6817444;     CBZjHMTqyYSNzCh6817444 = CBZjHMTqyYSNzCh54990485;     CBZjHMTqyYSNzCh54990485 = CBZjHMTqyYSNzCh3636185;     CBZjHMTqyYSNzCh3636185 = CBZjHMTqyYSNzCh82280651;     CBZjHMTqyYSNzCh82280651 = CBZjHMTqyYSNzCh25630847;     CBZjHMTqyYSNzCh25630847 = CBZjHMTqyYSNzCh79705762;     CBZjHMTqyYSNzCh79705762 = CBZjHMTqyYSNzCh31579543;     CBZjHMTqyYSNzCh31579543 = CBZjHMTqyYSNzCh70834542;     CBZjHMTqyYSNzCh70834542 = CBZjHMTqyYSNzCh96470770;     CBZjHMTqyYSNzCh96470770 = CBZjHMTqyYSNzCh66515593;     CBZjHMTqyYSNzCh66515593 = CBZjHMTqyYSNzCh20713608;     CBZjHMTqyYSNzCh20713608 = CBZjHMTqyYSNzCh94086942;     CBZjHMTqyYSNzCh94086942 = CBZjHMTqyYSNzCh15970446;     CBZjHMTqyYSNzCh15970446 = CBZjHMTqyYSNzCh39547780;     CBZjHMTqyYSNzCh39547780 = CBZjHMTqyYSNzCh55945181;     CBZjHMTqyYSNzCh55945181 = CBZjHMTqyYSNzCh15142343;     CBZjHMTqyYSNzCh15142343 = CBZjHMTqyYSNzCh83109509;     CBZjHMTqyYSNzCh83109509 = CBZjHMTqyYSNzCh27369147;     CBZjHMTqyYSNzCh27369147 = CBZjHMTqyYSNzCh35355874;     CBZjHMTqyYSNzCh35355874 = CBZjHMTqyYSNzCh1856492;     CBZjHMTqyYSNzCh1856492 = CBZjHMTqyYSNzCh19413352;     CBZjHMTqyYSNzCh19413352 = CBZjHMTqyYSNzCh91847930;     CBZjHMTqyYSNzCh91847930 = CBZjHMTqyYSNzCh41731282;     CBZjHMTqyYSNzCh41731282 = CBZjHMTqyYSNzCh41708393;     CBZjHMTqyYSNzCh41708393 = CBZjHMTqyYSNzCh15555957;     CBZjHMTqyYSNzCh15555957 = CBZjHMTqyYSNzCh88542606;     CBZjHMTqyYSNzCh88542606 = CBZjHMTqyYSNzCh12540327;     CBZjHMTqyYSNzCh12540327 = CBZjHMTqyYSNzCh19301920;     CBZjHMTqyYSNzCh19301920 = CBZjHMTqyYSNzCh26334137;     CBZjHMTqyYSNzCh26334137 = CBZjHMTqyYSNzCh8085450;     CBZjHMTqyYSNzCh8085450 = CBZjHMTqyYSNzCh37606106;     CBZjHMTqyYSNzCh37606106 = CBZjHMTqyYSNzCh87543505;     CBZjHMTqyYSNzCh87543505 = CBZjHMTqyYSNzCh53056950;     CBZjHMTqyYSNzCh53056950 = CBZjHMTqyYSNzCh96080853;     CBZjHMTqyYSNzCh96080853 = CBZjHMTqyYSNzCh11011394;     CBZjHMTqyYSNzCh11011394 = CBZjHMTqyYSNzCh69910511;     CBZjHMTqyYSNzCh69910511 = CBZjHMTqyYSNzCh27196834;     CBZjHMTqyYSNzCh27196834 = CBZjHMTqyYSNzCh60876846;     CBZjHMTqyYSNzCh60876846 = CBZjHMTqyYSNzCh4119651;     CBZjHMTqyYSNzCh4119651 = CBZjHMTqyYSNzCh98752263;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void YUFzOlyEVJYCKqtt26869715() {     float TbXCZTJvcPaajjr98745792 = -562200209;    float TbXCZTJvcPaajjr52156576 = -868610126;    float TbXCZTJvcPaajjr57002648 = -199310127;    float TbXCZTJvcPaajjr34867825 = -77960324;    float TbXCZTJvcPaajjr3438531 = -20928942;    float TbXCZTJvcPaajjr65621231 = -883702877;    float TbXCZTJvcPaajjr31190799 = -955048033;    float TbXCZTJvcPaajjr64035626 = -902573397;    float TbXCZTJvcPaajjr32046525 = -976512728;    float TbXCZTJvcPaajjr65789286 = -819191139;    float TbXCZTJvcPaajjr32428498 = -545491915;    float TbXCZTJvcPaajjr22188577 = -689193185;    float TbXCZTJvcPaajjr22099170 = -238451392;    float TbXCZTJvcPaajjr3789802 = -562401989;    float TbXCZTJvcPaajjr37045583 = -484332929;    float TbXCZTJvcPaajjr77256037 = -757914051;    float TbXCZTJvcPaajjr70624934 = -654949926;    float TbXCZTJvcPaajjr15490002 = -151220394;    float TbXCZTJvcPaajjr4154886 = -936776567;    float TbXCZTJvcPaajjr29450379 = -878052808;    float TbXCZTJvcPaajjr60515601 = -302495734;    float TbXCZTJvcPaajjr24271627 = -771172082;    float TbXCZTJvcPaajjr63145404 = -223903987;    float TbXCZTJvcPaajjr769754 = -391416072;    float TbXCZTJvcPaajjr71715593 = -454455322;    float TbXCZTJvcPaajjr65826930 = -590201575;    float TbXCZTJvcPaajjr29362065 = -935928591;    float TbXCZTJvcPaajjr91229544 = -57637067;    float TbXCZTJvcPaajjr52509538 = -327284799;    float TbXCZTJvcPaajjr21760047 = -361531123;    float TbXCZTJvcPaajjr20281800 = -440711872;    float TbXCZTJvcPaajjr71200903 = -424421165;    float TbXCZTJvcPaajjr13643561 = -957384928;    float TbXCZTJvcPaajjr58949550 = -620937272;    float TbXCZTJvcPaajjr61469078 = -80723246;    float TbXCZTJvcPaajjr38230892 = -133329338;    float TbXCZTJvcPaajjr48117265 = -543924997;    float TbXCZTJvcPaajjr63976247 = -727515955;    float TbXCZTJvcPaajjr33716122 = -501063693;    float TbXCZTJvcPaajjr39629062 = 33630702;    float TbXCZTJvcPaajjr5684505 = -666316808;    float TbXCZTJvcPaajjr10598658 = -21427328;    float TbXCZTJvcPaajjr91116378 = -58320009;    float TbXCZTJvcPaajjr2389397 = -162259061;    float TbXCZTJvcPaajjr13768550 = -497564050;    float TbXCZTJvcPaajjr48344998 = -769389901;    float TbXCZTJvcPaajjr75094153 = -750820538;    float TbXCZTJvcPaajjr15110713 = -328533855;    float TbXCZTJvcPaajjr82636479 = -458517335;    float TbXCZTJvcPaajjr18145233 = -105017096;    float TbXCZTJvcPaajjr17221154 = -622094327;    float TbXCZTJvcPaajjr85860421 = -758981348;    float TbXCZTJvcPaajjr73382787 = -174094533;    float TbXCZTJvcPaajjr25901082 = -496246184;    float TbXCZTJvcPaajjr66354113 = -19553761;    float TbXCZTJvcPaajjr74474165 = -791028128;    float TbXCZTJvcPaajjr89011172 = -544706140;    float TbXCZTJvcPaajjr56232895 = -807894055;    float TbXCZTJvcPaajjr63152232 = -623505002;    float TbXCZTJvcPaajjr37611601 = -430727367;    float TbXCZTJvcPaajjr36259166 = -947774286;    float TbXCZTJvcPaajjr39961254 = -797410967;    float TbXCZTJvcPaajjr11526089 = -475288599;    float TbXCZTJvcPaajjr10286479 = -514981606;    float TbXCZTJvcPaajjr45507487 = -278479267;    float TbXCZTJvcPaajjr61227595 = -21070751;    float TbXCZTJvcPaajjr8545017 = -731808258;    float TbXCZTJvcPaajjr63149619 = -617514120;    float TbXCZTJvcPaajjr42320724 = -381678744;    float TbXCZTJvcPaajjr98814690 = -251003592;    float TbXCZTJvcPaajjr29138773 = -113989055;    float TbXCZTJvcPaajjr6648688 = -927433971;    float TbXCZTJvcPaajjr81773879 = -650156702;    float TbXCZTJvcPaajjr64525823 = -870407270;    float TbXCZTJvcPaajjr23765875 = -111736000;    float TbXCZTJvcPaajjr49916944 = -181068406;    float TbXCZTJvcPaajjr33155249 = -612852073;    float TbXCZTJvcPaajjr60756007 = 38355073;    float TbXCZTJvcPaajjr87001203 = -893852022;    float TbXCZTJvcPaajjr23370595 = -685065422;    float TbXCZTJvcPaajjr90732776 = -839381038;    float TbXCZTJvcPaajjr14251353 = -507394737;    float TbXCZTJvcPaajjr8593066 = -599119733;    float TbXCZTJvcPaajjr34364306 = -122267703;    float TbXCZTJvcPaajjr4538894 = -739436796;    float TbXCZTJvcPaajjr34421378 = -681730524;    float TbXCZTJvcPaajjr97818115 = -150326632;    float TbXCZTJvcPaajjr87742478 = -361138744;    float TbXCZTJvcPaajjr92595436 = -501383512;    float TbXCZTJvcPaajjr86994912 = -289695118;    float TbXCZTJvcPaajjr49219720 = -588623199;    float TbXCZTJvcPaajjr91884370 = -736030942;    float TbXCZTJvcPaajjr824015 = -4010953;    float TbXCZTJvcPaajjr96104521 = 29663674;    float TbXCZTJvcPaajjr3369896 = -18595012;    float TbXCZTJvcPaajjr65723250 = -868905842;    float TbXCZTJvcPaajjr99072568 = -546138730;    float TbXCZTJvcPaajjr80829900 = -543338404;    float TbXCZTJvcPaajjr56881910 = -883779794;    float TbXCZTJvcPaajjr52540955 = -562200209;     TbXCZTJvcPaajjr98745792 = TbXCZTJvcPaajjr52156576;     TbXCZTJvcPaajjr52156576 = TbXCZTJvcPaajjr57002648;     TbXCZTJvcPaajjr57002648 = TbXCZTJvcPaajjr34867825;     TbXCZTJvcPaajjr34867825 = TbXCZTJvcPaajjr3438531;     TbXCZTJvcPaajjr3438531 = TbXCZTJvcPaajjr65621231;     TbXCZTJvcPaajjr65621231 = TbXCZTJvcPaajjr31190799;     TbXCZTJvcPaajjr31190799 = TbXCZTJvcPaajjr64035626;     TbXCZTJvcPaajjr64035626 = TbXCZTJvcPaajjr32046525;     TbXCZTJvcPaajjr32046525 = TbXCZTJvcPaajjr65789286;     TbXCZTJvcPaajjr65789286 = TbXCZTJvcPaajjr32428498;     TbXCZTJvcPaajjr32428498 = TbXCZTJvcPaajjr22188577;     TbXCZTJvcPaajjr22188577 = TbXCZTJvcPaajjr22099170;     TbXCZTJvcPaajjr22099170 = TbXCZTJvcPaajjr3789802;     TbXCZTJvcPaajjr3789802 = TbXCZTJvcPaajjr37045583;     TbXCZTJvcPaajjr37045583 = TbXCZTJvcPaajjr77256037;     TbXCZTJvcPaajjr77256037 = TbXCZTJvcPaajjr70624934;     TbXCZTJvcPaajjr70624934 = TbXCZTJvcPaajjr15490002;     TbXCZTJvcPaajjr15490002 = TbXCZTJvcPaajjr4154886;     TbXCZTJvcPaajjr4154886 = TbXCZTJvcPaajjr29450379;     TbXCZTJvcPaajjr29450379 = TbXCZTJvcPaajjr60515601;     TbXCZTJvcPaajjr60515601 = TbXCZTJvcPaajjr24271627;     TbXCZTJvcPaajjr24271627 = TbXCZTJvcPaajjr63145404;     TbXCZTJvcPaajjr63145404 = TbXCZTJvcPaajjr769754;     TbXCZTJvcPaajjr769754 = TbXCZTJvcPaajjr71715593;     TbXCZTJvcPaajjr71715593 = TbXCZTJvcPaajjr65826930;     TbXCZTJvcPaajjr65826930 = TbXCZTJvcPaajjr29362065;     TbXCZTJvcPaajjr29362065 = TbXCZTJvcPaajjr91229544;     TbXCZTJvcPaajjr91229544 = TbXCZTJvcPaajjr52509538;     TbXCZTJvcPaajjr52509538 = TbXCZTJvcPaajjr21760047;     TbXCZTJvcPaajjr21760047 = TbXCZTJvcPaajjr20281800;     TbXCZTJvcPaajjr20281800 = TbXCZTJvcPaajjr71200903;     TbXCZTJvcPaajjr71200903 = TbXCZTJvcPaajjr13643561;     TbXCZTJvcPaajjr13643561 = TbXCZTJvcPaajjr58949550;     TbXCZTJvcPaajjr58949550 = TbXCZTJvcPaajjr61469078;     TbXCZTJvcPaajjr61469078 = TbXCZTJvcPaajjr38230892;     TbXCZTJvcPaajjr38230892 = TbXCZTJvcPaajjr48117265;     TbXCZTJvcPaajjr48117265 = TbXCZTJvcPaajjr63976247;     TbXCZTJvcPaajjr63976247 = TbXCZTJvcPaajjr33716122;     TbXCZTJvcPaajjr33716122 = TbXCZTJvcPaajjr39629062;     TbXCZTJvcPaajjr39629062 = TbXCZTJvcPaajjr5684505;     TbXCZTJvcPaajjr5684505 = TbXCZTJvcPaajjr10598658;     TbXCZTJvcPaajjr10598658 = TbXCZTJvcPaajjr91116378;     TbXCZTJvcPaajjr91116378 = TbXCZTJvcPaajjr2389397;     TbXCZTJvcPaajjr2389397 = TbXCZTJvcPaajjr13768550;     TbXCZTJvcPaajjr13768550 = TbXCZTJvcPaajjr48344998;     TbXCZTJvcPaajjr48344998 = TbXCZTJvcPaajjr75094153;     TbXCZTJvcPaajjr75094153 = TbXCZTJvcPaajjr15110713;     TbXCZTJvcPaajjr15110713 = TbXCZTJvcPaajjr82636479;     TbXCZTJvcPaajjr82636479 = TbXCZTJvcPaajjr18145233;     TbXCZTJvcPaajjr18145233 = TbXCZTJvcPaajjr17221154;     TbXCZTJvcPaajjr17221154 = TbXCZTJvcPaajjr85860421;     TbXCZTJvcPaajjr85860421 = TbXCZTJvcPaajjr73382787;     TbXCZTJvcPaajjr73382787 = TbXCZTJvcPaajjr25901082;     TbXCZTJvcPaajjr25901082 = TbXCZTJvcPaajjr66354113;     TbXCZTJvcPaajjr66354113 = TbXCZTJvcPaajjr74474165;     TbXCZTJvcPaajjr74474165 = TbXCZTJvcPaajjr89011172;     TbXCZTJvcPaajjr89011172 = TbXCZTJvcPaajjr56232895;     TbXCZTJvcPaajjr56232895 = TbXCZTJvcPaajjr63152232;     TbXCZTJvcPaajjr63152232 = TbXCZTJvcPaajjr37611601;     TbXCZTJvcPaajjr37611601 = TbXCZTJvcPaajjr36259166;     TbXCZTJvcPaajjr36259166 = TbXCZTJvcPaajjr39961254;     TbXCZTJvcPaajjr39961254 = TbXCZTJvcPaajjr11526089;     TbXCZTJvcPaajjr11526089 = TbXCZTJvcPaajjr10286479;     TbXCZTJvcPaajjr10286479 = TbXCZTJvcPaajjr45507487;     TbXCZTJvcPaajjr45507487 = TbXCZTJvcPaajjr61227595;     TbXCZTJvcPaajjr61227595 = TbXCZTJvcPaajjr8545017;     TbXCZTJvcPaajjr8545017 = TbXCZTJvcPaajjr63149619;     TbXCZTJvcPaajjr63149619 = TbXCZTJvcPaajjr42320724;     TbXCZTJvcPaajjr42320724 = TbXCZTJvcPaajjr98814690;     TbXCZTJvcPaajjr98814690 = TbXCZTJvcPaajjr29138773;     TbXCZTJvcPaajjr29138773 = TbXCZTJvcPaajjr6648688;     TbXCZTJvcPaajjr6648688 = TbXCZTJvcPaajjr81773879;     TbXCZTJvcPaajjr81773879 = TbXCZTJvcPaajjr64525823;     TbXCZTJvcPaajjr64525823 = TbXCZTJvcPaajjr23765875;     TbXCZTJvcPaajjr23765875 = TbXCZTJvcPaajjr49916944;     TbXCZTJvcPaajjr49916944 = TbXCZTJvcPaajjr33155249;     TbXCZTJvcPaajjr33155249 = TbXCZTJvcPaajjr60756007;     TbXCZTJvcPaajjr60756007 = TbXCZTJvcPaajjr87001203;     TbXCZTJvcPaajjr87001203 = TbXCZTJvcPaajjr23370595;     TbXCZTJvcPaajjr23370595 = TbXCZTJvcPaajjr90732776;     TbXCZTJvcPaajjr90732776 = TbXCZTJvcPaajjr14251353;     TbXCZTJvcPaajjr14251353 = TbXCZTJvcPaajjr8593066;     TbXCZTJvcPaajjr8593066 = TbXCZTJvcPaajjr34364306;     TbXCZTJvcPaajjr34364306 = TbXCZTJvcPaajjr4538894;     TbXCZTJvcPaajjr4538894 = TbXCZTJvcPaajjr34421378;     TbXCZTJvcPaajjr34421378 = TbXCZTJvcPaajjr97818115;     TbXCZTJvcPaajjr97818115 = TbXCZTJvcPaajjr87742478;     TbXCZTJvcPaajjr87742478 = TbXCZTJvcPaajjr92595436;     TbXCZTJvcPaajjr92595436 = TbXCZTJvcPaajjr86994912;     TbXCZTJvcPaajjr86994912 = TbXCZTJvcPaajjr49219720;     TbXCZTJvcPaajjr49219720 = TbXCZTJvcPaajjr91884370;     TbXCZTJvcPaajjr91884370 = TbXCZTJvcPaajjr824015;     TbXCZTJvcPaajjr824015 = TbXCZTJvcPaajjr96104521;     TbXCZTJvcPaajjr96104521 = TbXCZTJvcPaajjr3369896;     TbXCZTJvcPaajjr3369896 = TbXCZTJvcPaajjr65723250;     TbXCZTJvcPaajjr65723250 = TbXCZTJvcPaajjr99072568;     TbXCZTJvcPaajjr99072568 = TbXCZTJvcPaajjr80829900;     TbXCZTJvcPaajjr80829900 = TbXCZTJvcPaajjr56881910;     TbXCZTJvcPaajjr56881910 = TbXCZTJvcPaajjr52540955;     TbXCZTJvcPaajjr52540955 = TbXCZTJvcPaajjr98745792;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void qqVndVJElMfXqYQE81285928() {     float ujCQQTzOJOknVlP69397459 = -80349340;    float ujCQQTzOJOknVlP20830094 = -740253425;    float ujCQQTzOJOknVlP36971567 = -641604587;    float ujCQQTzOJOknVlP74799112 = -268098713;    float ujCQQTzOJOknVlP24888852 = -904335290;    float ujCQQTzOJOknVlP55620994 = -585284028;    float ujCQQTzOJOknVlP49569695 = -953823743;    float ujCQQTzOJOknVlP20878007 = -149390487;    float ujCQQTzOJOknVlP77911579 = 30139944;    float ujCQQTzOJOknVlP88755747 = -20286489;    float ujCQQTzOJOknVlP74677658 = -580393845;    float ujCQQTzOJOknVlP67735148 = 95043022;    float ujCQQTzOJOknVlP1926557 = -147988451;    float ujCQQTzOJOknVlP87636835 = -363430200;    float ujCQQTzOJOknVlP19765602 = -634082933;    float ujCQQTzOJOknVlP9784678 = -327897638;    float ujCQQTzOJOknVlP64477761 = -564135056;    float ujCQQTzOJOknVlP35783496 = -140386591;    float ujCQQTzOJOknVlP85862666 = -543964509;    float ujCQQTzOJOknVlP88237088 = -320419112;    float ujCQQTzOJOknVlP15651356 = -211174874;    float ujCQQTzOJOknVlP85667392 = -2457387;    float ujCQQTzOJOknVlP65089212 = -791747132;    float ujCQQTzOJOknVlP34619290 = 16183022;    float ujCQQTzOJOknVlP35839981 = -130360010;    float ujCQQTzOJOknVlP96424510 = -688486020;    float ujCQQTzOJOknVlP37622851 = -231106367;    float ujCQQTzOJOknVlP99011770 = -562760835;    float ujCQQTzOJOknVlP84652026 = -621643016;    float ujCQQTzOJOknVlP60932143 = -919926193;    float ujCQQTzOJOknVlP27353796 = -189145557;    float ujCQQTzOJOknVlP15712364 = -752718327;    float ujCQQTzOJOknVlP96588921 = -904346537;    float ujCQQTzOJOknVlP24314525 = -184466956;    float ujCQQTzOJOknVlP39493314 = 72329975;    float ujCQQTzOJOknVlP98224151 = -212555886;    float ujCQQTzOJOknVlP91908750 = -732518204;    float ujCQQTzOJOknVlP16768736 = -511629062;    float ujCQQTzOJOknVlP12249411 = -456792330;    float ujCQQTzOJOknVlP34732360 = -583977660;    float ujCQQTzOJOknVlP65757788 = 49664313;    float ujCQQTzOJOknVlP39081648 = -890816398;    float ujCQQTzOJOknVlP40083338 = -45074061;    float ujCQQTzOJOknVlP8897697 = -234234032;    float ujCQQTzOJOknVlP9722609 = -806210445;    float ujCQQTzOJOknVlP54750832 = -527727056;    float ujCQQTzOJOknVlP1225357 = -866890337;    float ujCQQTzOJOknVlP80724922 = -780664150;    float ujCQQTzOJOknVlP88077931 = -444281304;    float ujCQQTzOJOknVlP26572719 = -388025975;    float ujCQQTzOJOknVlP85410478 = -168666448;    float ujCQQTzOJOknVlP27731926 = -728201070;    float ujCQQTzOJOknVlP28458444 = -101892575;    float ujCQQTzOJOknVlP16128387 = -57127681;    float ujCQQTzOJOknVlP65634572 = -130494675;    float ujCQQTzOJOknVlP83730067 = 22108047;    float ujCQQTzOJOknVlP55740881 = -948506294;    float ujCQQTzOJOknVlP2352277 = -557787609;    float ujCQQTzOJOknVlP38959131 = -37738704;    float ujCQQTzOJOknVlP28464342 = -115849270;    float ujCQQTzOJOknVlP17998143 = -254177662;    float ujCQQTzOJOknVlP50557925 = -291062909;    float ujCQQTzOJOknVlP36225981 = -527747472;    float ujCQQTzOJOknVlP16979437 = -49933864;    float ujCQQTzOJOknVlP61401951 = -831140932;    float ujCQQTzOJOknVlP58965295 = -827675518;    float ujCQQTzOJOknVlP71146227 = -610442;    float ujCQQTzOJOknVlP77612031 = -963521495;    float ujCQQTzOJOknVlP48143521 = -335760175;    float ujCQQTzOJOknVlP21541451 = -321527047;    float ujCQQTzOJOknVlP17875927 = -595379434;    float ujCQQTzOJOknVlP47709026 = 47494006;    float ujCQQTzOJOknVlP23534085 = -683594261;    float ujCQQTzOJOknVlP51130307 = -959986849;    float ujCQQTzOJOknVlP22479301 = -270083426;    float ujCQQTzOJOknVlP76569707 = -320358477;    float ujCQQTzOJOknVlP45584054 = -957383327;    float ujCQQTzOJOknVlP56191516 = -457513100;    float ujCQQTzOJOknVlP24896682 = -177606534;    float ujCQQTzOJOknVlP81089148 = -602632955;    float ujCQQTzOJOknVlP95199153 = -821595684;    float ujCQQTzOJOknVlP56897928 = -450442217;    float ujCQQTzOJOknVlP10933839 = -18479532;    float ujCQQTzOJOknVlP58079307 = -133617042;    float ujCQQTzOJOknVlP75521665 = -651259745;    float ujCQQTzOJOknVlP99621870 = -460944487;    float ujCQQTzOJOknVlP87253919 = -550825753;    float ujCQQTzOJOknVlP80460535 = -747218856;    float ujCQQTzOJOknVlP58679953 = 46027718;    float ujCQQTzOJOknVlP55763246 = -949778073;    float ujCQQTzOJOknVlP42483270 = -264049592;    float ujCQQTzOJOknVlP89556474 = -74730596;    float ujCQQTzOJOknVlP77809604 = -373890359;    float ujCQQTzOJOknVlP83785069 = -240943060;    float ujCQQTzOJOknVlP16734217 = -229799999;    float ujCQQTzOJOknVlP15199864 = -659272778;    float ujCQQTzOJOknVlP2855668 = -263068927;    float ujCQQTzOJOknVlP23103902 = -995140198;    float ujCQQTzOJOknVlP47495746 = -403093100;    float ujCQQTzOJOknVlP50757314 = -80349340;     ujCQQTzOJOknVlP69397459 = ujCQQTzOJOknVlP20830094;     ujCQQTzOJOknVlP20830094 = ujCQQTzOJOknVlP36971567;     ujCQQTzOJOknVlP36971567 = ujCQQTzOJOknVlP74799112;     ujCQQTzOJOknVlP74799112 = ujCQQTzOJOknVlP24888852;     ujCQQTzOJOknVlP24888852 = ujCQQTzOJOknVlP55620994;     ujCQQTzOJOknVlP55620994 = ujCQQTzOJOknVlP49569695;     ujCQQTzOJOknVlP49569695 = ujCQQTzOJOknVlP20878007;     ujCQQTzOJOknVlP20878007 = ujCQQTzOJOknVlP77911579;     ujCQQTzOJOknVlP77911579 = ujCQQTzOJOknVlP88755747;     ujCQQTzOJOknVlP88755747 = ujCQQTzOJOknVlP74677658;     ujCQQTzOJOknVlP74677658 = ujCQQTzOJOknVlP67735148;     ujCQQTzOJOknVlP67735148 = ujCQQTzOJOknVlP1926557;     ujCQQTzOJOknVlP1926557 = ujCQQTzOJOknVlP87636835;     ujCQQTzOJOknVlP87636835 = ujCQQTzOJOknVlP19765602;     ujCQQTzOJOknVlP19765602 = ujCQQTzOJOknVlP9784678;     ujCQQTzOJOknVlP9784678 = ujCQQTzOJOknVlP64477761;     ujCQQTzOJOknVlP64477761 = ujCQQTzOJOknVlP35783496;     ujCQQTzOJOknVlP35783496 = ujCQQTzOJOknVlP85862666;     ujCQQTzOJOknVlP85862666 = ujCQQTzOJOknVlP88237088;     ujCQQTzOJOknVlP88237088 = ujCQQTzOJOknVlP15651356;     ujCQQTzOJOknVlP15651356 = ujCQQTzOJOknVlP85667392;     ujCQQTzOJOknVlP85667392 = ujCQQTzOJOknVlP65089212;     ujCQQTzOJOknVlP65089212 = ujCQQTzOJOknVlP34619290;     ujCQQTzOJOknVlP34619290 = ujCQQTzOJOknVlP35839981;     ujCQQTzOJOknVlP35839981 = ujCQQTzOJOknVlP96424510;     ujCQQTzOJOknVlP96424510 = ujCQQTzOJOknVlP37622851;     ujCQQTzOJOknVlP37622851 = ujCQQTzOJOknVlP99011770;     ujCQQTzOJOknVlP99011770 = ujCQQTzOJOknVlP84652026;     ujCQQTzOJOknVlP84652026 = ujCQQTzOJOknVlP60932143;     ujCQQTzOJOknVlP60932143 = ujCQQTzOJOknVlP27353796;     ujCQQTzOJOknVlP27353796 = ujCQQTzOJOknVlP15712364;     ujCQQTzOJOknVlP15712364 = ujCQQTzOJOknVlP96588921;     ujCQQTzOJOknVlP96588921 = ujCQQTzOJOknVlP24314525;     ujCQQTzOJOknVlP24314525 = ujCQQTzOJOknVlP39493314;     ujCQQTzOJOknVlP39493314 = ujCQQTzOJOknVlP98224151;     ujCQQTzOJOknVlP98224151 = ujCQQTzOJOknVlP91908750;     ujCQQTzOJOknVlP91908750 = ujCQQTzOJOknVlP16768736;     ujCQQTzOJOknVlP16768736 = ujCQQTzOJOknVlP12249411;     ujCQQTzOJOknVlP12249411 = ujCQQTzOJOknVlP34732360;     ujCQQTzOJOknVlP34732360 = ujCQQTzOJOknVlP65757788;     ujCQQTzOJOknVlP65757788 = ujCQQTzOJOknVlP39081648;     ujCQQTzOJOknVlP39081648 = ujCQQTzOJOknVlP40083338;     ujCQQTzOJOknVlP40083338 = ujCQQTzOJOknVlP8897697;     ujCQQTzOJOknVlP8897697 = ujCQQTzOJOknVlP9722609;     ujCQQTzOJOknVlP9722609 = ujCQQTzOJOknVlP54750832;     ujCQQTzOJOknVlP54750832 = ujCQQTzOJOknVlP1225357;     ujCQQTzOJOknVlP1225357 = ujCQQTzOJOknVlP80724922;     ujCQQTzOJOknVlP80724922 = ujCQQTzOJOknVlP88077931;     ujCQQTzOJOknVlP88077931 = ujCQQTzOJOknVlP26572719;     ujCQQTzOJOknVlP26572719 = ujCQQTzOJOknVlP85410478;     ujCQQTzOJOknVlP85410478 = ujCQQTzOJOknVlP27731926;     ujCQQTzOJOknVlP27731926 = ujCQQTzOJOknVlP28458444;     ujCQQTzOJOknVlP28458444 = ujCQQTzOJOknVlP16128387;     ujCQQTzOJOknVlP16128387 = ujCQQTzOJOknVlP65634572;     ujCQQTzOJOknVlP65634572 = ujCQQTzOJOknVlP83730067;     ujCQQTzOJOknVlP83730067 = ujCQQTzOJOknVlP55740881;     ujCQQTzOJOknVlP55740881 = ujCQQTzOJOknVlP2352277;     ujCQQTzOJOknVlP2352277 = ujCQQTzOJOknVlP38959131;     ujCQQTzOJOknVlP38959131 = ujCQQTzOJOknVlP28464342;     ujCQQTzOJOknVlP28464342 = ujCQQTzOJOknVlP17998143;     ujCQQTzOJOknVlP17998143 = ujCQQTzOJOknVlP50557925;     ujCQQTzOJOknVlP50557925 = ujCQQTzOJOknVlP36225981;     ujCQQTzOJOknVlP36225981 = ujCQQTzOJOknVlP16979437;     ujCQQTzOJOknVlP16979437 = ujCQQTzOJOknVlP61401951;     ujCQQTzOJOknVlP61401951 = ujCQQTzOJOknVlP58965295;     ujCQQTzOJOknVlP58965295 = ujCQQTzOJOknVlP71146227;     ujCQQTzOJOknVlP71146227 = ujCQQTzOJOknVlP77612031;     ujCQQTzOJOknVlP77612031 = ujCQQTzOJOknVlP48143521;     ujCQQTzOJOknVlP48143521 = ujCQQTzOJOknVlP21541451;     ujCQQTzOJOknVlP21541451 = ujCQQTzOJOknVlP17875927;     ujCQQTzOJOknVlP17875927 = ujCQQTzOJOknVlP47709026;     ujCQQTzOJOknVlP47709026 = ujCQQTzOJOknVlP23534085;     ujCQQTzOJOknVlP23534085 = ujCQQTzOJOknVlP51130307;     ujCQQTzOJOknVlP51130307 = ujCQQTzOJOknVlP22479301;     ujCQQTzOJOknVlP22479301 = ujCQQTzOJOknVlP76569707;     ujCQQTzOJOknVlP76569707 = ujCQQTzOJOknVlP45584054;     ujCQQTzOJOknVlP45584054 = ujCQQTzOJOknVlP56191516;     ujCQQTzOJOknVlP56191516 = ujCQQTzOJOknVlP24896682;     ujCQQTzOJOknVlP24896682 = ujCQQTzOJOknVlP81089148;     ujCQQTzOJOknVlP81089148 = ujCQQTzOJOknVlP95199153;     ujCQQTzOJOknVlP95199153 = ujCQQTzOJOknVlP56897928;     ujCQQTzOJOknVlP56897928 = ujCQQTzOJOknVlP10933839;     ujCQQTzOJOknVlP10933839 = ujCQQTzOJOknVlP58079307;     ujCQQTzOJOknVlP58079307 = ujCQQTzOJOknVlP75521665;     ujCQQTzOJOknVlP75521665 = ujCQQTzOJOknVlP99621870;     ujCQQTzOJOknVlP99621870 = ujCQQTzOJOknVlP87253919;     ujCQQTzOJOknVlP87253919 = ujCQQTzOJOknVlP80460535;     ujCQQTzOJOknVlP80460535 = ujCQQTzOJOknVlP58679953;     ujCQQTzOJOknVlP58679953 = ujCQQTzOJOknVlP55763246;     ujCQQTzOJOknVlP55763246 = ujCQQTzOJOknVlP42483270;     ujCQQTzOJOknVlP42483270 = ujCQQTzOJOknVlP89556474;     ujCQQTzOJOknVlP89556474 = ujCQQTzOJOknVlP77809604;     ujCQQTzOJOknVlP77809604 = ujCQQTzOJOknVlP83785069;     ujCQQTzOJOknVlP83785069 = ujCQQTzOJOknVlP16734217;     ujCQQTzOJOknVlP16734217 = ujCQQTzOJOknVlP15199864;     ujCQQTzOJOknVlP15199864 = ujCQQTzOJOknVlP2855668;     ujCQQTzOJOknVlP2855668 = ujCQQTzOJOknVlP23103902;     ujCQQTzOJOknVlP23103902 = ujCQQTzOJOknVlP47495746;     ujCQQTzOJOknVlP47495746 = ujCQQTzOJOknVlP50757314;     ujCQQTzOJOknVlP50757314 = ujCQQTzOJOknVlP69397459;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void fbQosDgqBqXAowWy87614475() {     float tpHyUfzFShDNXsJ24051857 = -517876923;    float tpHyUfzFShDNXsJ67827713 = -219818239;    float tpHyUfzFShDNXsJ97704213 = 73010800;    float tpHyUfzFShDNXsJ46234053 = -274201124;    float tpHyUfzFShDNXsJ99340522 = -576770529;    float tpHyUfzFShDNXsJ69795391 = -714434864;    float tpHyUfzFShDNXsJ70670398 = -155716416;    float tpHyUfzFShDNXsJ50053111 = -649958720;    float tpHyUfzFShDNXsJ65685990 = -16713427;    float tpHyUfzFShDNXsJ71387732 = -299087759;    float tpHyUfzFShDNXsJ53288954 = -105750417;    float tpHyUfzFShDNXsJ30499862 = -91381067;    float tpHyUfzFShDNXsJ65743871 = 48471245;    float tpHyUfzFShDNXsJ80603266 = 24466222;    float tpHyUfzFShDNXsJ49989675 = -541204564;    float tpHyUfzFShDNXsJ99552213 = 41942798;    float tpHyUfzFShDNXsJ64309525 = 62265116;    float tpHyUfzFShDNXsJ3380083 = -203951058;    float tpHyUfzFShDNXsJ87083267 = -829776057;    float tpHyUfzFShDNXsJ5871644 = -665785259;    float tpHyUfzFShDNXsJ49825744 = -779258025;    float tpHyUfzFShDNXsJ36919981 = -168901495;    float tpHyUfzFShDNXsJ1559045 = -278490553;    float tpHyUfzFShDNXsJ36047659 = -782857795;    float tpHyUfzFShDNXsJ31592597 = -503273462;    float tpHyUfzFShDNXsJ37665458 = -692194805;    float tpHyUfzFShDNXsJ39612775 = -785950645;    float tpHyUfzFShDNXsJ52260770 = -570812695;    float tpHyUfzFShDNXsJ24549703 = 98112109;    float tpHyUfzFShDNXsJ55367552 = -110684161;    float tpHyUfzFShDNXsJ79059325 = -738018083;    float tpHyUfzFShDNXsJ46703974 = -64214856;    float tpHyUfzFShDNXsJ64244798 = -360864151;    float tpHyUfzFShDNXsJ18970418 = -964587766;    float tpHyUfzFShDNXsJ73997471 = -145533285;    float tpHyUfzFShDNXsJ85550693 = -460551482;    float tpHyUfzFShDNXsJ66613238 = -582944825;    float tpHyUfzFShDNXsJ29745974 = -385558083;    float tpHyUfzFShDNXsJ63019060 = -838740932;    float tpHyUfzFShDNXsJ58718755 = -242286260;    float tpHyUfzFShDNXsJ21717189 = -62885845;    float tpHyUfzFShDNXsJ75756306 = -398348484;    float tpHyUfzFShDNXsJ31083521 = -985579379;    float tpHyUfzFShDNXsJ17892097 = -807913698;    float tpHyUfzFShDNXsJ69706672 = -938728043;    float tpHyUfzFShDNXsJ10084435 = -596516670;    float tpHyUfzFShDNXsJ17699069 = -893947721;    float tpHyUfzFShDNXsJ4152232 = -758316522;    float tpHyUfzFShDNXsJ30420698 = -344011493;    float tpHyUfzFShDNXsJ71414381 = -628045269;    float tpHyUfzFShDNXsJ60524017 = -925881950;    float tpHyUfzFShDNXsJ17114076 = -143267053;    float tpHyUfzFShDNXsJ60186447 = -941808032;    float tpHyUfzFShDNXsJ73544143 = 46985835;    float tpHyUfzFShDNXsJ37788732 = -526403357;    float tpHyUfzFShDNXsJ87131876 = -248975429;    float tpHyUfzFShDNXsJ66268669 = -941327687;    float tpHyUfzFShDNXsJ61656554 = -144131406;    float tpHyUfzFShDNXsJ14641456 = -770927663;    float tpHyUfzFShDNXsJ61675065 = -884575725;    float tpHyUfzFShDNXsJ30182616 = -928484219;    float tpHyUfzFShDNXsJ18409629 = -584903721;    float tpHyUfzFShDNXsJ25503408 = -648070829;    float tpHyUfzFShDNXsJ10318439 = -906029266;    float tpHyUfzFShDNXsJ92328407 = -561069676;    float tpHyUfzFShDNXsJ6584980 = 58464438;    float tpHyUfzFShDNXsJ66255064 = -730516917;    float tpHyUfzFShDNXsJ46773454 = 13059011;    float tpHyUfzFShDNXsJ6605796 = -830000493;    float tpHyUfzFShDNXsJ64438982 = 19346918;    float tpHyUfzFShDNXsJ32938975 = -375112378;    float tpHyUfzFShDNXsJ34563551 = -552176801;    float tpHyUfzFShDNXsJ40361023 = -365210127;    float tpHyUfzFShDNXsJ28364513 = -487489798;    float tpHyUfzFShDNXsJ84154455 = -502899414;    float tpHyUfzFShDNXsJ74069437 = -280909541;    float tpHyUfzFShDNXsJ5836460 = -183322117;    float tpHyUfzFShDNXsJ83666948 = -470576855;    float tpHyUfzFShDNXsJ66340987 = -844129752;    float tpHyUfzFShDNXsJ21508163 = -906756792;    float tpHyUfzFShDNXsJ19966389 = -798247085;    float tpHyUfzFShDNXsJ35460543 = 72365876;    float tpHyUfzFShDNXsJ21840072 = -126801203;    float tpHyUfzFShDNXsJ53135322 = -273842623;    float tpHyUfzFShDNXsJ94843534 = -184802211;    float tpHyUfzFShDNXsJ61945249 = -494751030;    float tpHyUfzFShDNXsJ86517527 = -122406824;    float tpHyUfzFShDNXsJ90700654 = -307849986;    float tpHyUfzFShDNXsJ81181686 = -338184410;    float tpHyUfzFShDNXsJ86865594 = -896557856;    float tpHyUfzFShDNXsJ19282024 = -519223796;    float tpHyUfzFShDNXsJ4956685 = -338813420;    float tpHyUfzFShDNXsJ15104519 = -614630421;    float tpHyUfzFShDNXsJ1343995 = -954165207;    float tpHyUfzFShDNXsJ28536139 = -313802041;    float tpHyUfzFShDNXsJ3307561 = -477982124;    float tpHyUfzFShDNXsJ50252899 = -750277656;    float tpHyUfzFShDNXsJ20765083 = 20449887;    float tpHyUfzFShDNXsJ25563689 = -146844022;    float tpHyUfzFShDNXsJ63121693 = -517876923;     tpHyUfzFShDNXsJ24051857 = tpHyUfzFShDNXsJ67827713;     tpHyUfzFShDNXsJ67827713 = tpHyUfzFShDNXsJ97704213;     tpHyUfzFShDNXsJ97704213 = tpHyUfzFShDNXsJ46234053;     tpHyUfzFShDNXsJ46234053 = tpHyUfzFShDNXsJ99340522;     tpHyUfzFShDNXsJ99340522 = tpHyUfzFShDNXsJ69795391;     tpHyUfzFShDNXsJ69795391 = tpHyUfzFShDNXsJ70670398;     tpHyUfzFShDNXsJ70670398 = tpHyUfzFShDNXsJ50053111;     tpHyUfzFShDNXsJ50053111 = tpHyUfzFShDNXsJ65685990;     tpHyUfzFShDNXsJ65685990 = tpHyUfzFShDNXsJ71387732;     tpHyUfzFShDNXsJ71387732 = tpHyUfzFShDNXsJ53288954;     tpHyUfzFShDNXsJ53288954 = tpHyUfzFShDNXsJ30499862;     tpHyUfzFShDNXsJ30499862 = tpHyUfzFShDNXsJ65743871;     tpHyUfzFShDNXsJ65743871 = tpHyUfzFShDNXsJ80603266;     tpHyUfzFShDNXsJ80603266 = tpHyUfzFShDNXsJ49989675;     tpHyUfzFShDNXsJ49989675 = tpHyUfzFShDNXsJ99552213;     tpHyUfzFShDNXsJ99552213 = tpHyUfzFShDNXsJ64309525;     tpHyUfzFShDNXsJ64309525 = tpHyUfzFShDNXsJ3380083;     tpHyUfzFShDNXsJ3380083 = tpHyUfzFShDNXsJ87083267;     tpHyUfzFShDNXsJ87083267 = tpHyUfzFShDNXsJ5871644;     tpHyUfzFShDNXsJ5871644 = tpHyUfzFShDNXsJ49825744;     tpHyUfzFShDNXsJ49825744 = tpHyUfzFShDNXsJ36919981;     tpHyUfzFShDNXsJ36919981 = tpHyUfzFShDNXsJ1559045;     tpHyUfzFShDNXsJ1559045 = tpHyUfzFShDNXsJ36047659;     tpHyUfzFShDNXsJ36047659 = tpHyUfzFShDNXsJ31592597;     tpHyUfzFShDNXsJ31592597 = tpHyUfzFShDNXsJ37665458;     tpHyUfzFShDNXsJ37665458 = tpHyUfzFShDNXsJ39612775;     tpHyUfzFShDNXsJ39612775 = tpHyUfzFShDNXsJ52260770;     tpHyUfzFShDNXsJ52260770 = tpHyUfzFShDNXsJ24549703;     tpHyUfzFShDNXsJ24549703 = tpHyUfzFShDNXsJ55367552;     tpHyUfzFShDNXsJ55367552 = tpHyUfzFShDNXsJ79059325;     tpHyUfzFShDNXsJ79059325 = tpHyUfzFShDNXsJ46703974;     tpHyUfzFShDNXsJ46703974 = tpHyUfzFShDNXsJ64244798;     tpHyUfzFShDNXsJ64244798 = tpHyUfzFShDNXsJ18970418;     tpHyUfzFShDNXsJ18970418 = tpHyUfzFShDNXsJ73997471;     tpHyUfzFShDNXsJ73997471 = tpHyUfzFShDNXsJ85550693;     tpHyUfzFShDNXsJ85550693 = tpHyUfzFShDNXsJ66613238;     tpHyUfzFShDNXsJ66613238 = tpHyUfzFShDNXsJ29745974;     tpHyUfzFShDNXsJ29745974 = tpHyUfzFShDNXsJ63019060;     tpHyUfzFShDNXsJ63019060 = tpHyUfzFShDNXsJ58718755;     tpHyUfzFShDNXsJ58718755 = tpHyUfzFShDNXsJ21717189;     tpHyUfzFShDNXsJ21717189 = tpHyUfzFShDNXsJ75756306;     tpHyUfzFShDNXsJ75756306 = tpHyUfzFShDNXsJ31083521;     tpHyUfzFShDNXsJ31083521 = tpHyUfzFShDNXsJ17892097;     tpHyUfzFShDNXsJ17892097 = tpHyUfzFShDNXsJ69706672;     tpHyUfzFShDNXsJ69706672 = tpHyUfzFShDNXsJ10084435;     tpHyUfzFShDNXsJ10084435 = tpHyUfzFShDNXsJ17699069;     tpHyUfzFShDNXsJ17699069 = tpHyUfzFShDNXsJ4152232;     tpHyUfzFShDNXsJ4152232 = tpHyUfzFShDNXsJ30420698;     tpHyUfzFShDNXsJ30420698 = tpHyUfzFShDNXsJ71414381;     tpHyUfzFShDNXsJ71414381 = tpHyUfzFShDNXsJ60524017;     tpHyUfzFShDNXsJ60524017 = tpHyUfzFShDNXsJ17114076;     tpHyUfzFShDNXsJ17114076 = tpHyUfzFShDNXsJ60186447;     tpHyUfzFShDNXsJ60186447 = tpHyUfzFShDNXsJ73544143;     tpHyUfzFShDNXsJ73544143 = tpHyUfzFShDNXsJ37788732;     tpHyUfzFShDNXsJ37788732 = tpHyUfzFShDNXsJ87131876;     tpHyUfzFShDNXsJ87131876 = tpHyUfzFShDNXsJ66268669;     tpHyUfzFShDNXsJ66268669 = tpHyUfzFShDNXsJ61656554;     tpHyUfzFShDNXsJ61656554 = tpHyUfzFShDNXsJ14641456;     tpHyUfzFShDNXsJ14641456 = tpHyUfzFShDNXsJ61675065;     tpHyUfzFShDNXsJ61675065 = tpHyUfzFShDNXsJ30182616;     tpHyUfzFShDNXsJ30182616 = tpHyUfzFShDNXsJ18409629;     tpHyUfzFShDNXsJ18409629 = tpHyUfzFShDNXsJ25503408;     tpHyUfzFShDNXsJ25503408 = tpHyUfzFShDNXsJ10318439;     tpHyUfzFShDNXsJ10318439 = tpHyUfzFShDNXsJ92328407;     tpHyUfzFShDNXsJ92328407 = tpHyUfzFShDNXsJ6584980;     tpHyUfzFShDNXsJ6584980 = tpHyUfzFShDNXsJ66255064;     tpHyUfzFShDNXsJ66255064 = tpHyUfzFShDNXsJ46773454;     tpHyUfzFShDNXsJ46773454 = tpHyUfzFShDNXsJ6605796;     tpHyUfzFShDNXsJ6605796 = tpHyUfzFShDNXsJ64438982;     tpHyUfzFShDNXsJ64438982 = tpHyUfzFShDNXsJ32938975;     tpHyUfzFShDNXsJ32938975 = tpHyUfzFShDNXsJ34563551;     tpHyUfzFShDNXsJ34563551 = tpHyUfzFShDNXsJ40361023;     tpHyUfzFShDNXsJ40361023 = tpHyUfzFShDNXsJ28364513;     tpHyUfzFShDNXsJ28364513 = tpHyUfzFShDNXsJ84154455;     tpHyUfzFShDNXsJ84154455 = tpHyUfzFShDNXsJ74069437;     tpHyUfzFShDNXsJ74069437 = tpHyUfzFShDNXsJ5836460;     tpHyUfzFShDNXsJ5836460 = tpHyUfzFShDNXsJ83666948;     tpHyUfzFShDNXsJ83666948 = tpHyUfzFShDNXsJ66340987;     tpHyUfzFShDNXsJ66340987 = tpHyUfzFShDNXsJ21508163;     tpHyUfzFShDNXsJ21508163 = tpHyUfzFShDNXsJ19966389;     tpHyUfzFShDNXsJ19966389 = tpHyUfzFShDNXsJ35460543;     tpHyUfzFShDNXsJ35460543 = tpHyUfzFShDNXsJ21840072;     tpHyUfzFShDNXsJ21840072 = tpHyUfzFShDNXsJ53135322;     tpHyUfzFShDNXsJ53135322 = tpHyUfzFShDNXsJ94843534;     tpHyUfzFShDNXsJ94843534 = tpHyUfzFShDNXsJ61945249;     tpHyUfzFShDNXsJ61945249 = tpHyUfzFShDNXsJ86517527;     tpHyUfzFShDNXsJ86517527 = tpHyUfzFShDNXsJ90700654;     tpHyUfzFShDNXsJ90700654 = tpHyUfzFShDNXsJ81181686;     tpHyUfzFShDNXsJ81181686 = tpHyUfzFShDNXsJ86865594;     tpHyUfzFShDNXsJ86865594 = tpHyUfzFShDNXsJ19282024;     tpHyUfzFShDNXsJ19282024 = tpHyUfzFShDNXsJ4956685;     tpHyUfzFShDNXsJ4956685 = tpHyUfzFShDNXsJ15104519;     tpHyUfzFShDNXsJ15104519 = tpHyUfzFShDNXsJ1343995;     tpHyUfzFShDNXsJ1343995 = tpHyUfzFShDNXsJ28536139;     tpHyUfzFShDNXsJ28536139 = tpHyUfzFShDNXsJ3307561;     tpHyUfzFShDNXsJ3307561 = tpHyUfzFShDNXsJ50252899;     tpHyUfzFShDNXsJ50252899 = tpHyUfzFShDNXsJ20765083;     tpHyUfzFShDNXsJ20765083 = tpHyUfzFShDNXsJ25563689;     tpHyUfzFShDNXsJ25563689 = tpHyUfzFShDNXsJ63121693;     tpHyUfzFShDNXsJ63121693 = tpHyUfzFShDNXsJ24051857;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void rUMqbhUbvouRabiV33310168() {     float cOmpecZqrpeNLjV43240815 = -361651749;    float cOmpecZqrpeNLjV40125571 = -961426572;    float cOmpecZqrpeNLjV22363595 = -603835352;    float cOmpecZqrpeNLjV30525501 = -279693294;    float cOmpecZqrpeNLjV26347026 = -611962244;    float cOmpecZqrpeNLjV82552348 = -280670617;    float cOmpecZqrpeNLjV49661032 = -317419821;    float cOmpecZqrpeNLjV86310703 = -110470129;    float cOmpecZqrpeNLjV34682960 = -388881460;    float cOmpecZqrpeNLjV35756519 = -550008901;    float cOmpecZqrpeNLjV14039119 = -888571332;    float cOmpecZqrpeNLjV6988104 = -149162747;    float cOmpecZqrpeNLjV23179455 = -324715029;    float cOmpecZqrpeNLjV74273054 = 43573002;    float cOmpecZqrpeNLjV67191341 = 92385967;    float cOmpecZqrpeNLjV80342995 = -285200810;    float cOmpecZqrpeNLjV64158112 = -803974730;    float cOmpecZqrpeNLjV94217010 = -591159078;    float cOmpecZqrpeNLjV8181809 = -317006450;    float cOmpecZqrpeNLjV1742744 = -96614790;    float cOmpecZqrpeNLjV50582693 = -300532861;    float cOmpecZqrpeNLjV63047311 = -758701192;    float cOmpecZqrpeNLjV4381893 = -586559632;    float cOmpecZqrpeNLjV87333190 = -951994530;    float cOmpecZqrpeNLjV47769952 = -178895568;    float cOmpecZqrpeNLjV84782310 = -915532711;    float cOmpecZqrpeNLjV51403707 = -735310496;    float cOmpecZqrpeNLjV184871 = 81940630;    float cOmpecZqrpeNLjV30457613 = -354108279;    float cOmpecZqrpeNLjV20359420 = -42366333;    float cOmpecZqrpeNLjV45594301 = -22003356;    float cOmpecZqrpeNLjV24596424 = 5438269;    float cOmpecZqrpeNLjV5135087 = -971730003;    float cOmpecZqrpeNLjV84160721 = -456696495;    float cOmpecZqrpeNLjV25051212 = -671610218;    float cOmpecZqrpeNLjV64144581 = -573747519;    float cOmpecZqrpeNLjV3847278 = -668328783;    float cOmpecZqrpeNLjV61425489 = -272094202;    float cOmpecZqrpeNLjV18711744 = -82494673;    float cOmpecZqrpeNLjV90306510 = -44763999;    float cOmpecZqrpeNLjV32080650 = -494180987;    float cOmpecZqrpeNLjV8763500 = -505127362;    float cOmpecZqrpeNLjV92983685 = -732034165;    float cOmpecZqrpeNLjV45987056 = -334225397;    float cOmpecZqrpeNLjV13692329 = -617993881;    float cOmpecZqrpeNLjV59884676 = -438427324;    float cOmpecZqrpeNLjV42525409 = -918299366;    float cOmpecZqrpeNLjV85236809 = -78203657;    float cOmpecZqrpeNLjV68529188 = -143768663;    float cOmpecZqrpeNLjV11771878 = 35937366;    float cOmpecZqrpeNLjV68126203 = -287375902;    float cOmpecZqrpeNLjV37558011 = -826826438;    float cOmpecZqrpeNLjV98741649 = -817731944;    float cOmpecZqrpeNLjV25218325 = -189312001;    float cOmpecZqrpeNLjV82727475 = -332721170;    float cOmpecZqrpeNLjV80193504 = -602950557;    float cOmpecZqrpeNLjV35743678 = -274866941;    float cOmpecZqrpeNLjV35030405 = -651840823;    float cOmpecZqrpeNLjV82755548 = -797726;    float cOmpecZqrpeNLjV41564716 = -696429534;    float cOmpecZqrpeNLjV31148642 = -545360121;    float cOmpecZqrpeNLjV49476161 = -299360452;    float cOmpecZqrpeNLjV55853091 = -756361850;    float cOmpecZqrpeNLjV14323540 = -246515128;    float cOmpecZqrpeNLjV90162218 = -428005546;    float cOmpecZqrpeNLjV89442695 = -794009601;    float cOmpecZqrpeNLjV1853017 = -177432744;    float cOmpecZqrpeNLjV39018734 = -868018535;    float cOmpecZqrpeNLjV49221842 = -284816780;    float cOmpecZqrpeNLjV3046761 = -333866514;    float cOmpecZqrpeNLjV76495718 = -616872027;    float cOmpecZqrpeNLjV2732623 = -431880528;    float cOmpecZqrpeNLjV75505267 = -408664406;    float cOmpecZqrpeNLjV17875298 = -172242451;    float cOmpecZqrpeNLjV69662094 = -602433804;    float cOmpecZqrpeNLjV41819194 = -795405499;    float cOmpecZqrpeNLjV70063625 = 73332972;    float cOmpecZqrpeNLjV58394837 = -152334235;    float cOmpecZqrpeNLjV73640861 = -234000649;    float cOmpecZqrpeNLjV87885275 = -740468245;    float cOmpecZqrpeNLjV42256901 = -997233345;    float cOmpecZqrpeNLjV66166897 = -557106840;    float cOmpecZqrpeNLjV31655682 = -774290708;    float cOmpecZqrpeNLjV18685735 = -290045646;    float cOmpecZqrpeNLjV52233217 = -754990431;    float cOmpecZqrpeNLjV8036291 = -195176919;    float cOmpecZqrpeNLjV25854774 = -176829788;    float cOmpecZqrpeNLjV79916761 = -682418003;    float cOmpecZqrpeNLjV1433246 = -23975326;    float cOmpecZqrpeNLjV44857708 = 31340338;    float cOmpecZqrpeNLjV28400903 = -198880579;    float cOmpecZqrpeNLjV68816873 = 83512039;    float cOmpecZqrpeNLjV78669940 = -171296477;    float cOmpecZqrpeNLjV77147028 = -386065140;    float cOmpecZqrpeNLjV59157869 = -499403879;    float cOmpecZqrpeNLjV82604488 = -94820536;    float cOmpecZqrpeNLjV52910408 = -748765513;    float cOmpecZqrpeNLjV78660146 = -385519037;    float cOmpecZqrpeNLjV55824837 = -906219852;    float cOmpecZqrpeNLjV24249634 = -361651749;     cOmpecZqrpeNLjV43240815 = cOmpecZqrpeNLjV40125571;     cOmpecZqrpeNLjV40125571 = cOmpecZqrpeNLjV22363595;     cOmpecZqrpeNLjV22363595 = cOmpecZqrpeNLjV30525501;     cOmpecZqrpeNLjV30525501 = cOmpecZqrpeNLjV26347026;     cOmpecZqrpeNLjV26347026 = cOmpecZqrpeNLjV82552348;     cOmpecZqrpeNLjV82552348 = cOmpecZqrpeNLjV49661032;     cOmpecZqrpeNLjV49661032 = cOmpecZqrpeNLjV86310703;     cOmpecZqrpeNLjV86310703 = cOmpecZqrpeNLjV34682960;     cOmpecZqrpeNLjV34682960 = cOmpecZqrpeNLjV35756519;     cOmpecZqrpeNLjV35756519 = cOmpecZqrpeNLjV14039119;     cOmpecZqrpeNLjV14039119 = cOmpecZqrpeNLjV6988104;     cOmpecZqrpeNLjV6988104 = cOmpecZqrpeNLjV23179455;     cOmpecZqrpeNLjV23179455 = cOmpecZqrpeNLjV74273054;     cOmpecZqrpeNLjV74273054 = cOmpecZqrpeNLjV67191341;     cOmpecZqrpeNLjV67191341 = cOmpecZqrpeNLjV80342995;     cOmpecZqrpeNLjV80342995 = cOmpecZqrpeNLjV64158112;     cOmpecZqrpeNLjV64158112 = cOmpecZqrpeNLjV94217010;     cOmpecZqrpeNLjV94217010 = cOmpecZqrpeNLjV8181809;     cOmpecZqrpeNLjV8181809 = cOmpecZqrpeNLjV1742744;     cOmpecZqrpeNLjV1742744 = cOmpecZqrpeNLjV50582693;     cOmpecZqrpeNLjV50582693 = cOmpecZqrpeNLjV63047311;     cOmpecZqrpeNLjV63047311 = cOmpecZqrpeNLjV4381893;     cOmpecZqrpeNLjV4381893 = cOmpecZqrpeNLjV87333190;     cOmpecZqrpeNLjV87333190 = cOmpecZqrpeNLjV47769952;     cOmpecZqrpeNLjV47769952 = cOmpecZqrpeNLjV84782310;     cOmpecZqrpeNLjV84782310 = cOmpecZqrpeNLjV51403707;     cOmpecZqrpeNLjV51403707 = cOmpecZqrpeNLjV184871;     cOmpecZqrpeNLjV184871 = cOmpecZqrpeNLjV30457613;     cOmpecZqrpeNLjV30457613 = cOmpecZqrpeNLjV20359420;     cOmpecZqrpeNLjV20359420 = cOmpecZqrpeNLjV45594301;     cOmpecZqrpeNLjV45594301 = cOmpecZqrpeNLjV24596424;     cOmpecZqrpeNLjV24596424 = cOmpecZqrpeNLjV5135087;     cOmpecZqrpeNLjV5135087 = cOmpecZqrpeNLjV84160721;     cOmpecZqrpeNLjV84160721 = cOmpecZqrpeNLjV25051212;     cOmpecZqrpeNLjV25051212 = cOmpecZqrpeNLjV64144581;     cOmpecZqrpeNLjV64144581 = cOmpecZqrpeNLjV3847278;     cOmpecZqrpeNLjV3847278 = cOmpecZqrpeNLjV61425489;     cOmpecZqrpeNLjV61425489 = cOmpecZqrpeNLjV18711744;     cOmpecZqrpeNLjV18711744 = cOmpecZqrpeNLjV90306510;     cOmpecZqrpeNLjV90306510 = cOmpecZqrpeNLjV32080650;     cOmpecZqrpeNLjV32080650 = cOmpecZqrpeNLjV8763500;     cOmpecZqrpeNLjV8763500 = cOmpecZqrpeNLjV92983685;     cOmpecZqrpeNLjV92983685 = cOmpecZqrpeNLjV45987056;     cOmpecZqrpeNLjV45987056 = cOmpecZqrpeNLjV13692329;     cOmpecZqrpeNLjV13692329 = cOmpecZqrpeNLjV59884676;     cOmpecZqrpeNLjV59884676 = cOmpecZqrpeNLjV42525409;     cOmpecZqrpeNLjV42525409 = cOmpecZqrpeNLjV85236809;     cOmpecZqrpeNLjV85236809 = cOmpecZqrpeNLjV68529188;     cOmpecZqrpeNLjV68529188 = cOmpecZqrpeNLjV11771878;     cOmpecZqrpeNLjV11771878 = cOmpecZqrpeNLjV68126203;     cOmpecZqrpeNLjV68126203 = cOmpecZqrpeNLjV37558011;     cOmpecZqrpeNLjV37558011 = cOmpecZqrpeNLjV98741649;     cOmpecZqrpeNLjV98741649 = cOmpecZqrpeNLjV25218325;     cOmpecZqrpeNLjV25218325 = cOmpecZqrpeNLjV82727475;     cOmpecZqrpeNLjV82727475 = cOmpecZqrpeNLjV80193504;     cOmpecZqrpeNLjV80193504 = cOmpecZqrpeNLjV35743678;     cOmpecZqrpeNLjV35743678 = cOmpecZqrpeNLjV35030405;     cOmpecZqrpeNLjV35030405 = cOmpecZqrpeNLjV82755548;     cOmpecZqrpeNLjV82755548 = cOmpecZqrpeNLjV41564716;     cOmpecZqrpeNLjV41564716 = cOmpecZqrpeNLjV31148642;     cOmpecZqrpeNLjV31148642 = cOmpecZqrpeNLjV49476161;     cOmpecZqrpeNLjV49476161 = cOmpecZqrpeNLjV55853091;     cOmpecZqrpeNLjV55853091 = cOmpecZqrpeNLjV14323540;     cOmpecZqrpeNLjV14323540 = cOmpecZqrpeNLjV90162218;     cOmpecZqrpeNLjV90162218 = cOmpecZqrpeNLjV89442695;     cOmpecZqrpeNLjV89442695 = cOmpecZqrpeNLjV1853017;     cOmpecZqrpeNLjV1853017 = cOmpecZqrpeNLjV39018734;     cOmpecZqrpeNLjV39018734 = cOmpecZqrpeNLjV49221842;     cOmpecZqrpeNLjV49221842 = cOmpecZqrpeNLjV3046761;     cOmpecZqrpeNLjV3046761 = cOmpecZqrpeNLjV76495718;     cOmpecZqrpeNLjV76495718 = cOmpecZqrpeNLjV2732623;     cOmpecZqrpeNLjV2732623 = cOmpecZqrpeNLjV75505267;     cOmpecZqrpeNLjV75505267 = cOmpecZqrpeNLjV17875298;     cOmpecZqrpeNLjV17875298 = cOmpecZqrpeNLjV69662094;     cOmpecZqrpeNLjV69662094 = cOmpecZqrpeNLjV41819194;     cOmpecZqrpeNLjV41819194 = cOmpecZqrpeNLjV70063625;     cOmpecZqrpeNLjV70063625 = cOmpecZqrpeNLjV58394837;     cOmpecZqrpeNLjV58394837 = cOmpecZqrpeNLjV73640861;     cOmpecZqrpeNLjV73640861 = cOmpecZqrpeNLjV87885275;     cOmpecZqrpeNLjV87885275 = cOmpecZqrpeNLjV42256901;     cOmpecZqrpeNLjV42256901 = cOmpecZqrpeNLjV66166897;     cOmpecZqrpeNLjV66166897 = cOmpecZqrpeNLjV31655682;     cOmpecZqrpeNLjV31655682 = cOmpecZqrpeNLjV18685735;     cOmpecZqrpeNLjV18685735 = cOmpecZqrpeNLjV52233217;     cOmpecZqrpeNLjV52233217 = cOmpecZqrpeNLjV8036291;     cOmpecZqrpeNLjV8036291 = cOmpecZqrpeNLjV25854774;     cOmpecZqrpeNLjV25854774 = cOmpecZqrpeNLjV79916761;     cOmpecZqrpeNLjV79916761 = cOmpecZqrpeNLjV1433246;     cOmpecZqrpeNLjV1433246 = cOmpecZqrpeNLjV44857708;     cOmpecZqrpeNLjV44857708 = cOmpecZqrpeNLjV28400903;     cOmpecZqrpeNLjV28400903 = cOmpecZqrpeNLjV68816873;     cOmpecZqrpeNLjV68816873 = cOmpecZqrpeNLjV78669940;     cOmpecZqrpeNLjV78669940 = cOmpecZqrpeNLjV77147028;     cOmpecZqrpeNLjV77147028 = cOmpecZqrpeNLjV59157869;     cOmpecZqrpeNLjV59157869 = cOmpecZqrpeNLjV82604488;     cOmpecZqrpeNLjV82604488 = cOmpecZqrpeNLjV52910408;     cOmpecZqrpeNLjV52910408 = cOmpecZqrpeNLjV78660146;     cOmpecZqrpeNLjV78660146 = cOmpecZqrpeNLjV55824837;     cOmpecZqrpeNLjV55824837 = cOmpecZqrpeNLjV24249634;     cOmpecZqrpeNLjV24249634 = cOmpecZqrpeNLjV43240815;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void XBIksfoUNxMkQIvN39968913() {     float TuthSvpTujXvijw43234344 = -234175890;    float TuthSvpTujXvijw78697887 = 95978303;    float TuthSvpTujXvijw23676807 = -560963966;    float TuthSvpTujXvijw15605362 = -100234124;    float TuthSvpTujXvijw35187128 = 19682434;    float TuthSvpTujXvijw17357781 = 19896571;    float TuthSvpTujXvijw18208368 = -21956288;    float TuthSvpTujXvijw55524754 = -914647446;    float TuthSvpTujXvijw17423124 = -102527530;    float TuthSvpTujXvijw32396033 = -186815772;    float TuthSvpTujXvijw34359724 = -848043404;    float TuthSvpTujXvijw21279781 = -984641109;    float TuthSvpTujXvijw55032369 = -346373503;    float TuthSvpTujXvijw28117274 = -301580049;    float TuthSvpTujXvijw12363451 = -970326884;    float TuthSvpTujXvijw54907543 = -617996460;    float TuthSvpTujXvijw20010872 = -623589299;    float TuthSvpTujXvijw17217544 = -988230700;    float TuthSvpTujXvijw28610078 = -934988717;    float TuthSvpTujXvijw73816507 = -708639242;    float TuthSvpTujXvijw30252117 = -10999234;    float TuthSvpTujXvijw41343577 = -168693076;    float TuthSvpTujXvijw41260289 = 54482526;    float TuthSvpTujXvijw30983299 = -832915052;    float TuthSvpTujXvijw76212641 = -605589422;    float TuthSvpTujXvijw1356387 = -823738639;    float TuthSvpTujXvijw21625289 = 63889791;    float TuthSvpTujXvijw35588395 = -527026358;    float TuthSvpTujXvijw43136059 = 99821406;    float TuthSvpTujXvijw96449289 = -817797706;    float TuthSvpTujXvijw79006979 = -409096591;    float TuthSvpTujXvijw9320281 = -386383493;    float TuthSvpTujXvijw90587509 = -73674219;    float TuthSvpTujXvijw84443558 = -333378227;    float TuthSvpTujXvijw7409251 = -930924143;    float TuthSvpTujXvijw6972771 = -653513266;    float TuthSvpTujXvijw15788646 = -767982162;    float TuthSvpTujXvijw31343169 = -267356881;    float TuthSvpTujXvijw54025341 = -795176089;    float TuthSvpTujXvijw62179403 = -754195685;    float TuthSvpTujXvijw19936317 = -582124886;    float TuthSvpTujXvijw94461160 = -698919445;    float TuthSvpTujXvijw53267046 = -191164419;    float TuthSvpTujXvijw5218954 = -991189841;    float TuthSvpTujXvijw97710381 = 8746718;    float TuthSvpTujXvijw312645 = -250471994;    float TuthSvpTujXvijw70223201 = -299579988;    float TuthSvpTujXvijw60620391 = -686965014;    float TuthSvpTujXvijw87187577 = -807532524;    float TuthSvpTujXvijw31817301 = -651087520;    float TuthSvpTujXvijw81385573 = -965930909;    float TuthSvpTujXvijw52105268 = -108972186;    float TuthSvpTujXvijw74189998 = -709785952;    float TuthSvpTujXvijw35468596 = -721231853;    float TuthSvpTujXvijw9716796 = -89620447;    float TuthSvpTujXvijw1890767 = 34517185;    float TuthSvpTujXvijw37437598 = -958504224;    float TuthSvpTujXvijw92693508 = -728048914;    float TuthSvpTujXvijw39392720 = -494644703;    float TuthSvpTujXvijw33830741 = -156578927;    float TuthSvpTujXvijw95732492 = 56006780;    float TuthSvpTujXvijw82619972 = -494929930;    float TuthSvpTujXvijw12388696 = -914468853;    float TuthSvpTujXvijw20973834 = -284729824;    float TuthSvpTujXvijw53389053 = -777719182;    float TuthSvpTujXvijw25039444 = -361659912;    float TuthSvpTujXvijw30692271 = -810966890;    float TuthSvpTujXvijw70588810 = 87004723;    float TuthSvpTujXvijw20708024 = -370655907;    float TuthSvpTujXvijw5390680 = -216813619;    float TuthSvpTujXvijw39118897 = -850014299;    float TuthSvpTujXvijw88667702 = -256232419;    float TuthSvpTujXvijw63192203 = -93054612;    float TuthSvpTujXvijw66430675 = -80793032;    float TuthSvpTujXvijw53880190 = -26514356;    float TuthSvpTujXvijw35790957 = -312079790;    float TuthSvpTujXvijw88076531 = -977528657;    float TuthSvpTujXvijw36041335 = 45672366;    float TuthSvpTujXvijw33272918 = -741661770;    float TuthSvpTujXvijw75899997 = -255117428;    float TuthSvpTujXvijw31133185 = -424158652;    float TuthSvpTujXvijw61004897 = -249145196;    float TuthSvpTujXvijw48400818 = -719493835;    float TuthSvpTujXvijw11318759 = -249091074;    float TuthSvpTujXvijw15063717 = -851866798;    float TuthSvpTujXvijw26901712 = -200124405;    float TuthSvpTujXvijw35130283 = -676597541;    float TuthSvpTujXvijw55118914 = -352442367;    float TuthSvpTujXvijw74726762 = -143757780;    float TuthSvpTujXvijw5518484 = -865441328;    float TuthSvpTujXvijw69535172 = -695009042;    float TuthSvpTujXvijw23095137 = 60066752;    float TuthSvpTujXvijw91950449 = -772712179;    float TuthSvpTujXvijw20194600 = -538597163;    float TuthSvpTujXvijw66446911 = -710202466;    float TuthSvpTujXvijw37316345 = 12805044;    float TuthSvpTujXvijw82072465 = -784450593;    float TuthSvpTujXvijw32293212 = -906434595;    float TuthSvpTujXvijw51829901 = -113470659;    float TuthSvpTujXvijw72670938 = -234175890;     TuthSvpTujXvijw43234344 = TuthSvpTujXvijw78697887;     TuthSvpTujXvijw78697887 = TuthSvpTujXvijw23676807;     TuthSvpTujXvijw23676807 = TuthSvpTujXvijw15605362;     TuthSvpTujXvijw15605362 = TuthSvpTujXvijw35187128;     TuthSvpTujXvijw35187128 = TuthSvpTujXvijw17357781;     TuthSvpTujXvijw17357781 = TuthSvpTujXvijw18208368;     TuthSvpTujXvijw18208368 = TuthSvpTujXvijw55524754;     TuthSvpTujXvijw55524754 = TuthSvpTujXvijw17423124;     TuthSvpTujXvijw17423124 = TuthSvpTujXvijw32396033;     TuthSvpTujXvijw32396033 = TuthSvpTujXvijw34359724;     TuthSvpTujXvijw34359724 = TuthSvpTujXvijw21279781;     TuthSvpTujXvijw21279781 = TuthSvpTujXvijw55032369;     TuthSvpTujXvijw55032369 = TuthSvpTujXvijw28117274;     TuthSvpTujXvijw28117274 = TuthSvpTujXvijw12363451;     TuthSvpTujXvijw12363451 = TuthSvpTujXvijw54907543;     TuthSvpTujXvijw54907543 = TuthSvpTujXvijw20010872;     TuthSvpTujXvijw20010872 = TuthSvpTujXvijw17217544;     TuthSvpTujXvijw17217544 = TuthSvpTujXvijw28610078;     TuthSvpTujXvijw28610078 = TuthSvpTujXvijw73816507;     TuthSvpTujXvijw73816507 = TuthSvpTujXvijw30252117;     TuthSvpTujXvijw30252117 = TuthSvpTujXvijw41343577;     TuthSvpTujXvijw41343577 = TuthSvpTujXvijw41260289;     TuthSvpTujXvijw41260289 = TuthSvpTujXvijw30983299;     TuthSvpTujXvijw30983299 = TuthSvpTujXvijw76212641;     TuthSvpTujXvijw76212641 = TuthSvpTujXvijw1356387;     TuthSvpTujXvijw1356387 = TuthSvpTujXvijw21625289;     TuthSvpTujXvijw21625289 = TuthSvpTujXvijw35588395;     TuthSvpTujXvijw35588395 = TuthSvpTujXvijw43136059;     TuthSvpTujXvijw43136059 = TuthSvpTujXvijw96449289;     TuthSvpTujXvijw96449289 = TuthSvpTujXvijw79006979;     TuthSvpTujXvijw79006979 = TuthSvpTujXvijw9320281;     TuthSvpTujXvijw9320281 = TuthSvpTujXvijw90587509;     TuthSvpTujXvijw90587509 = TuthSvpTujXvijw84443558;     TuthSvpTujXvijw84443558 = TuthSvpTujXvijw7409251;     TuthSvpTujXvijw7409251 = TuthSvpTujXvijw6972771;     TuthSvpTujXvijw6972771 = TuthSvpTujXvijw15788646;     TuthSvpTujXvijw15788646 = TuthSvpTujXvijw31343169;     TuthSvpTujXvijw31343169 = TuthSvpTujXvijw54025341;     TuthSvpTujXvijw54025341 = TuthSvpTujXvijw62179403;     TuthSvpTujXvijw62179403 = TuthSvpTujXvijw19936317;     TuthSvpTujXvijw19936317 = TuthSvpTujXvijw94461160;     TuthSvpTujXvijw94461160 = TuthSvpTujXvijw53267046;     TuthSvpTujXvijw53267046 = TuthSvpTujXvijw5218954;     TuthSvpTujXvijw5218954 = TuthSvpTujXvijw97710381;     TuthSvpTujXvijw97710381 = TuthSvpTujXvijw312645;     TuthSvpTujXvijw312645 = TuthSvpTujXvijw70223201;     TuthSvpTujXvijw70223201 = TuthSvpTujXvijw60620391;     TuthSvpTujXvijw60620391 = TuthSvpTujXvijw87187577;     TuthSvpTujXvijw87187577 = TuthSvpTujXvijw31817301;     TuthSvpTujXvijw31817301 = TuthSvpTujXvijw81385573;     TuthSvpTujXvijw81385573 = TuthSvpTujXvijw52105268;     TuthSvpTujXvijw52105268 = TuthSvpTujXvijw74189998;     TuthSvpTujXvijw74189998 = TuthSvpTujXvijw35468596;     TuthSvpTujXvijw35468596 = TuthSvpTujXvijw9716796;     TuthSvpTujXvijw9716796 = TuthSvpTujXvijw1890767;     TuthSvpTujXvijw1890767 = TuthSvpTujXvijw37437598;     TuthSvpTujXvijw37437598 = TuthSvpTujXvijw92693508;     TuthSvpTujXvijw92693508 = TuthSvpTujXvijw39392720;     TuthSvpTujXvijw39392720 = TuthSvpTujXvijw33830741;     TuthSvpTujXvijw33830741 = TuthSvpTujXvijw95732492;     TuthSvpTujXvijw95732492 = TuthSvpTujXvijw82619972;     TuthSvpTujXvijw82619972 = TuthSvpTujXvijw12388696;     TuthSvpTujXvijw12388696 = TuthSvpTujXvijw20973834;     TuthSvpTujXvijw20973834 = TuthSvpTujXvijw53389053;     TuthSvpTujXvijw53389053 = TuthSvpTujXvijw25039444;     TuthSvpTujXvijw25039444 = TuthSvpTujXvijw30692271;     TuthSvpTujXvijw30692271 = TuthSvpTujXvijw70588810;     TuthSvpTujXvijw70588810 = TuthSvpTujXvijw20708024;     TuthSvpTujXvijw20708024 = TuthSvpTujXvijw5390680;     TuthSvpTujXvijw5390680 = TuthSvpTujXvijw39118897;     TuthSvpTujXvijw39118897 = TuthSvpTujXvijw88667702;     TuthSvpTujXvijw88667702 = TuthSvpTujXvijw63192203;     TuthSvpTujXvijw63192203 = TuthSvpTujXvijw66430675;     TuthSvpTujXvijw66430675 = TuthSvpTujXvijw53880190;     TuthSvpTujXvijw53880190 = TuthSvpTujXvijw35790957;     TuthSvpTujXvijw35790957 = TuthSvpTujXvijw88076531;     TuthSvpTujXvijw88076531 = TuthSvpTujXvijw36041335;     TuthSvpTujXvijw36041335 = TuthSvpTujXvijw33272918;     TuthSvpTujXvijw33272918 = TuthSvpTujXvijw75899997;     TuthSvpTujXvijw75899997 = TuthSvpTujXvijw31133185;     TuthSvpTujXvijw31133185 = TuthSvpTujXvijw61004897;     TuthSvpTujXvijw61004897 = TuthSvpTujXvijw48400818;     TuthSvpTujXvijw48400818 = TuthSvpTujXvijw11318759;     TuthSvpTujXvijw11318759 = TuthSvpTujXvijw15063717;     TuthSvpTujXvijw15063717 = TuthSvpTujXvijw26901712;     TuthSvpTujXvijw26901712 = TuthSvpTujXvijw35130283;     TuthSvpTujXvijw35130283 = TuthSvpTujXvijw55118914;     TuthSvpTujXvijw55118914 = TuthSvpTujXvijw74726762;     TuthSvpTujXvijw74726762 = TuthSvpTujXvijw5518484;     TuthSvpTujXvijw5518484 = TuthSvpTujXvijw69535172;     TuthSvpTujXvijw69535172 = TuthSvpTujXvijw23095137;     TuthSvpTujXvijw23095137 = TuthSvpTujXvijw91950449;     TuthSvpTujXvijw91950449 = TuthSvpTujXvijw20194600;     TuthSvpTujXvijw20194600 = TuthSvpTujXvijw66446911;     TuthSvpTujXvijw66446911 = TuthSvpTujXvijw37316345;     TuthSvpTujXvijw37316345 = TuthSvpTujXvijw82072465;     TuthSvpTujXvijw82072465 = TuthSvpTujXvijw32293212;     TuthSvpTujXvijw32293212 = TuthSvpTujXvijw51829901;     TuthSvpTujXvijw51829901 = TuthSvpTujXvijw72670938;     TuthSvpTujXvijw72670938 = TuthSvpTujXvijw43234344;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void NkzrWOLevNcHEUlV94385125() {     float vjnNFNNjAJLpCxT13886011 = -852325021;    float vjnNFNNjAJLpCxT47371405 = -875664996;    float vjnNFNNjAJLpCxT3645727 = 96741574;    float vjnNFNNjAJLpCxT55536648 = -290372513;    float vjnNFNNjAJLpCxT56637450 = -863723913;    float vjnNFNNjAJLpCxT7357543 = -781684580;    float vjnNFNNjAJLpCxT36587263 = -20731998;    float vjnNFNNjAJLpCxT12367135 = -161464536;    float vjnNFNNjAJLpCxT63288178 = -195874859;    float vjnNFNNjAJLpCxT55362494 = -487911123;    float vjnNFNNjAJLpCxT76608885 = -882945333;    float vjnNFNNjAJLpCxT66826351 = -200404902;    float vjnNFNNjAJLpCxT34859756 = -255910562;    float vjnNFNNjAJLpCxT11964308 = -102608259;    float vjnNFNNjAJLpCxT95083469 = -20076888;    float vjnNFNNjAJLpCxT87436183 = -187980047;    float vjnNFNNjAJLpCxT13863699 = -532774429;    float vjnNFNNjAJLpCxT37511038 = -977396896;    float vjnNFNNjAJLpCxT10317860 = -542176659;    float vjnNFNNjAJLpCxT32603216 = -151005546;    float vjnNFNNjAJLpCxT85387871 = 80321625;    float vjnNFNNjAJLpCxT2739342 = -499978381;    float vjnNFNNjAJLpCxT43204098 = -513360618;    float vjnNFNNjAJLpCxT64832836 = -425315958;    float vjnNFNNjAJLpCxT40337030 = -281494109;    float vjnNFNNjAJLpCxT31953968 = -922023084;    float vjnNFNNjAJLpCxT29886074 = -331287984;    float vjnNFNNjAJLpCxT43370621 = 67849874;    float vjnNFNNjAJLpCxT75278547 = -194536811;    float vjnNFNNjAJLpCxT35621386 = -276192777;    float vjnNFNNjAJLpCxT86078976 = -157530276;    float vjnNFNNjAJLpCxT53831742 = -714680656;    float vjnNFNNjAJLpCxT73532870 = -20635828;    float vjnNFNNjAJLpCxT49808533 = -996907912;    float vjnNFNNjAJLpCxT85433486 = -777870922;    float vjnNFNNjAJLpCxT66966029 = -732739813;    float vjnNFNNjAJLpCxT59580131 = -956575369;    float vjnNFNNjAJLpCxT84135657 = -51469989;    float vjnNFNNjAJLpCxT32558630 = -750904726;    float vjnNFNNjAJLpCxT57282701 = -271804048;    float vjnNFNNjAJLpCxT80009601 = -966143764;    float vjnNFNNjAJLpCxT22944151 = -468308514;    float vjnNFNNjAJLpCxT2234006 = -177918471;    float vjnNFNNjAJLpCxT11727255 = 36835188;    float vjnNFNNjAJLpCxT93664440 = -299899677;    float vjnNFNNjAJLpCxT6718480 = -8809149;    float vjnNFNNjAJLpCxT96354404 = -415649787;    float vjnNFNNjAJLpCxT26234601 = -39095309;    float vjnNFNNjAJLpCxT92629029 = -793296493;    float vjnNFNNjAJLpCxT40244787 = -934096399;    float vjnNFNNjAJLpCxT49574897 = -512503030;    float vjnNFNNjAJLpCxT93976772 = -78191908;    float vjnNFNNjAJLpCxT29265656 = -637583994;    float vjnNFNNjAJLpCxT25695901 = -282113349;    float vjnNFNNjAJLpCxT8997255 = -200561362;    float vjnNFNNjAJLpCxT11146669 = -252346640;    float vjnNFNNjAJLpCxT4167307 = -262304379;    float vjnNFNNjAJLpCxT38812890 = -477942468;    float vjnNFNNjAJLpCxT15199618 = 91121596;    float vjnNFNNjAJLpCxT24683482 = -941700829;    float vjnNFNNjAJLpCxT77471469 = -350396596;    float vjnNFNNjAJLpCxT93216642 = 11418127;    float vjnNFNNjAJLpCxT37088588 = -966927725;    float vjnNFNNjAJLpCxT27666793 = -919682082;    float vjnNFNNjAJLpCxT69283517 = -230380847;    float vjnNFNNjAJLpCxT22777144 = -68264678;    float vjnNFNNjAJLpCxT93293481 = -79769075;    float vjnNFNNjAJLpCxT85051222 = -259002651;    float vjnNFNNjAJLpCxT26530821 = -324737338;    float vjnNFNNjAJLpCxT28117440 = -287337075;    float vjnNFNNjAJLpCxT27856052 = -231404679;    float vjnNFNNjAJLpCxT29728041 = -381304441;    float vjnNFNNjAJLpCxT4952409 = -126492171;    float vjnNFNNjAJLpCxT53035159 = -170372611;    float vjnNFNNjAJLpCxT52593615 = -184861783;    float vjnNFNNjAJLpCxT62443720 = -451369861;    float vjnNFNNjAJLpCxT505336 = -222059911;    float vjnNFNNjAJLpCxT31476844 = -450195807;    float vjnNFNNjAJLpCxT71168396 = -25416282;    float vjnNFNNjAJLpCxT33618551 = -172684961;    float vjnNFNNjAJLpCxT35599563 = -406373297;    float vjnNFNNjAJLpCxT3651474 = -192192676;    float vjnNFNNjAJLpCxT50741591 = -138853633;    float vjnNFNNjAJLpCxT35033760 = -260440413;    float vjnNFNNjAJLpCxT86046488 = -763689747;    float vjnNFNNjAJLpCxT92102203 = 20661631;    float vjnNFNNjAJLpCxT24566087 = 22903338;    float vjnNFNNjAJLpCxT47836970 = -738522479;    float vjnNFNNjAJLpCxT40811279 = -696346550;    float vjnNFNNjAJLpCxT74286817 = -425524283;    float vjnNFNNjAJLpCxT62798723 = -370435435;    float vjnNFNNjAJLpCxT20767241 = -378632902;    float vjnNFNNjAJLpCxT68936039 = -42591585;    float vjnNFNNjAJLpCxT7875148 = -809203897;    float vjnNFNNjAJLpCxT79811232 = -921407452;    float vjnNFNNjAJLpCxT86792958 = -877561892;    float vjnNFNNjAJLpCxT85855563 = -501380789;    float vjnNFNNjAJLpCxT74567213 = -258236389;    float vjnNFNNjAJLpCxT42443737 = -732783965;    float vjnNFNNjAJLpCxT70887297 = -852325021;     vjnNFNNjAJLpCxT13886011 = vjnNFNNjAJLpCxT47371405;     vjnNFNNjAJLpCxT47371405 = vjnNFNNjAJLpCxT3645727;     vjnNFNNjAJLpCxT3645727 = vjnNFNNjAJLpCxT55536648;     vjnNFNNjAJLpCxT55536648 = vjnNFNNjAJLpCxT56637450;     vjnNFNNjAJLpCxT56637450 = vjnNFNNjAJLpCxT7357543;     vjnNFNNjAJLpCxT7357543 = vjnNFNNjAJLpCxT36587263;     vjnNFNNjAJLpCxT36587263 = vjnNFNNjAJLpCxT12367135;     vjnNFNNjAJLpCxT12367135 = vjnNFNNjAJLpCxT63288178;     vjnNFNNjAJLpCxT63288178 = vjnNFNNjAJLpCxT55362494;     vjnNFNNjAJLpCxT55362494 = vjnNFNNjAJLpCxT76608885;     vjnNFNNjAJLpCxT76608885 = vjnNFNNjAJLpCxT66826351;     vjnNFNNjAJLpCxT66826351 = vjnNFNNjAJLpCxT34859756;     vjnNFNNjAJLpCxT34859756 = vjnNFNNjAJLpCxT11964308;     vjnNFNNjAJLpCxT11964308 = vjnNFNNjAJLpCxT95083469;     vjnNFNNjAJLpCxT95083469 = vjnNFNNjAJLpCxT87436183;     vjnNFNNjAJLpCxT87436183 = vjnNFNNjAJLpCxT13863699;     vjnNFNNjAJLpCxT13863699 = vjnNFNNjAJLpCxT37511038;     vjnNFNNjAJLpCxT37511038 = vjnNFNNjAJLpCxT10317860;     vjnNFNNjAJLpCxT10317860 = vjnNFNNjAJLpCxT32603216;     vjnNFNNjAJLpCxT32603216 = vjnNFNNjAJLpCxT85387871;     vjnNFNNjAJLpCxT85387871 = vjnNFNNjAJLpCxT2739342;     vjnNFNNjAJLpCxT2739342 = vjnNFNNjAJLpCxT43204098;     vjnNFNNjAJLpCxT43204098 = vjnNFNNjAJLpCxT64832836;     vjnNFNNjAJLpCxT64832836 = vjnNFNNjAJLpCxT40337030;     vjnNFNNjAJLpCxT40337030 = vjnNFNNjAJLpCxT31953968;     vjnNFNNjAJLpCxT31953968 = vjnNFNNjAJLpCxT29886074;     vjnNFNNjAJLpCxT29886074 = vjnNFNNjAJLpCxT43370621;     vjnNFNNjAJLpCxT43370621 = vjnNFNNjAJLpCxT75278547;     vjnNFNNjAJLpCxT75278547 = vjnNFNNjAJLpCxT35621386;     vjnNFNNjAJLpCxT35621386 = vjnNFNNjAJLpCxT86078976;     vjnNFNNjAJLpCxT86078976 = vjnNFNNjAJLpCxT53831742;     vjnNFNNjAJLpCxT53831742 = vjnNFNNjAJLpCxT73532870;     vjnNFNNjAJLpCxT73532870 = vjnNFNNjAJLpCxT49808533;     vjnNFNNjAJLpCxT49808533 = vjnNFNNjAJLpCxT85433486;     vjnNFNNjAJLpCxT85433486 = vjnNFNNjAJLpCxT66966029;     vjnNFNNjAJLpCxT66966029 = vjnNFNNjAJLpCxT59580131;     vjnNFNNjAJLpCxT59580131 = vjnNFNNjAJLpCxT84135657;     vjnNFNNjAJLpCxT84135657 = vjnNFNNjAJLpCxT32558630;     vjnNFNNjAJLpCxT32558630 = vjnNFNNjAJLpCxT57282701;     vjnNFNNjAJLpCxT57282701 = vjnNFNNjAJLpCxT80009601;     vjnNFNNjAJLpCxT80009601 = vjnNFNNjAJLpCxT22944151;     vjnNFNNjAJLpCxT22944151 = vjnNFNNjAJLpCxT2234006;     vjnNFNNjAJLpCxT2234006 = vjnNFNNjAJLpCxT11727255;     vjnNFNNjAJLpCxT11727255 = vjnNFNNjAJLpCxT93664440;     vjnNFNNjAJLpCxT93664440 = vjnNFNNjAJLpCxT6718480;     vjnNFNNjAJLpCxT6718480 = vjnNFNNjAJLpCxT96354404;     vjnNFNNjAJLpCxT96354404 = vjnNFNNjAJLpCxT26234601;     vjnNFNNjAJLpCxT26234601 = vjnNFNNjAJLpCxT92629029;     vjnNFNNjAJLpCxT92629029 = vjnNFNNjAJLpCxT40244787;     vjnNFNNjAJLpCxT40244787 = vjnNFNNjAJLpCxT49574897;     vjnNFNNjAJLpCxT49574897 = vjnNFNNjAJLpCxT93976772;     vjnNFNNjAJLpCxT93976772 = vjnNFNNjAJLpCxT29265656;     vjnNFNNjAJLpCxT29265656 = vjnNFNNjAJLpCxT25695901;     vjnNFNNjAJLpCxT25695901 = vjnNFNNjAJLpCxT8997255;     vjnNFNNjAJLpCxT8997255 = vjnNFNNjAJLpCxT11146669;     vjnNFNNjAJLpCxT11146669 = vjnNFNNjAJLpCxT4167307;     vjnNFNNjAJLpCxT4167307 = vjnNFNNjAJLpCxT38812890;     vjnNFNNjAJLpCxT38812890 = vjnNFNNjAJLpCxT15199618;     vjnNFNNjAJLpCxT15199618 = vjnNFNNjAJLpCxT24683482;     vjnNFNNjAJLpCxT24683482 = vjnNFNNjAJLpCxT77471469;     vjnNFNNjAJLpCxT77471469 = vjnNFNNjAJLpCxT93216642;     vjnNFNNjAJLpCxT93216642 = vjnNFNNjAJLpCxT37088588;     vjnNFNNjAJLpCxT37088588 = vjnNFNNjAJLpCxT27666793;     vjnNFNNjAJLpCxT27666793 = vjnNFNNjAJLpCxT69283517;     vjnNFNNjAJLpCxT69283517 = vjnNFNNjAJLpCxT22777144;     vjnNFNNjAJLpCxT22777144 = vjnNFNNjAJLpCxT93293481;     vjnNFNNjAJLpCxT93293481 = vjnNFNNjAJLpCxT85051222;     vjnNFNNjAJLpCxT85051222 = vjnNFNNjAJLpCxT26530821;     vjnNFNNjAJLpCxT26530821 = vjnNFNNjAJLpCxT28117440;     vjnNFNNjAJLpCxT28117440 = vjnNFNNjAJLpCxT27856052;     vjnNFNNjAJLpCxT27856052 = vjnNFNNjAJLpCxT29728041;     vjnNFNNjAJLpCxT29728041 = vjnNFNNjAJLpCxT4952409;     vjnNFNNjAJLpCxT4952409 = vjnNFNNjAJLpCxT53035159;     vjnNFNNjAJLpCxT53035159 = vjnNFNNjAJLpCxT52593615;     vjnNFNNjAJLpCxT52593615 = vjnNFNNjAJLpCxT62443720;     vjnNFNNjAJLpCxT62443720 = vjnNFNNjAJLpCxT505336;     vjnNFNNjAJLpCxT505336 = vjnNFNNjAJLpCxT31476844;     vjnNFNNjAJLpCxT31476844 = vjnNFNNjAJLpCxT71168396;     vjnNFNNjAJLpCxT71168396 = vjnNFNNjAJLpCxT33618551;     vjnNFNNjAJLpCxT33618551 = vjnNFNNjAJLpCxT35599563;     vjnNFNNjAJLpCxT35599563 = vjnNFNNjAJLpCxT3651474;     vjnNFNNjAJLpCxT3651474 = vjnNFNNjAJLpCxT50741591;     vjnNFNNjAJLpCxT50741591 = vjnNFNNjAJLpCxT35033760;     vjnNFNNjAJLpCxT35033760 = vjnNFNNjAJLpCxT86046488;     vjnNFNNjAJLpCxT86046488 = vjnNFNNjAJLpCxT92102203;     vjnNFNNjAJLpCxT92102203 = vjnNFNNjAJLpCxT24566087;     vjnNFNNjAJLpCxT24566087 = vjnNFNNjAJLpCxT47836970;     vjnNFNNjAJLpCxT47836970 = vjnNFNNjAJLpCxT40811279;     vjnNFNNjAJLpCxT40811279 = vjnNFNNjAJLpCxT74286817;     vjnNFNNjAJLpCxT74286817 = vjnNFNNjAJLpCxT62798723;     vjnNFNNjAJLpCxT62798723 = vjnNFNNjAJLpCxT20767241;     vjnNFNNjAJLpCxT20767241 = vjnNFNNjAJLpCxT68936039;     vjnNFNNjAJLpCxT68936039 = vjnNFNNjAJLpCxT7875148;     vjnNFNNjAJLpCxT7875148 = vjnNFNNjAJLpCxT79811232;     vjnNFNNjAJLpCxT79811232 = vjnNFNNjAJLpCxT86792958;     vjnNFNNjAJLpCxT86792958 = vjnNFNNjAJLpCxT85855563;     vjnNFNNjAJLpCxT85855563 = vjnNFNNjAJLpCxT74567213;     vjnNFNNjAJLpCxT74567213 = vjnNFNNjAJLpCxT42443737;     vjnNFNNjAJLpCxT42443737 = vjnNFNNjAJLpCxT70887297;     vjnNFNNjAJLpCxT70887297 = vjnNFNNjAJLpCxT13886011;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void NldbMxMRVojXNFdR64840999() {     float PJVyMqDwTRQOigg26672178 = -987365713;    float PJVyMqDwTRQOigg34142293 = 54000990;    float PJVyMqDwTRQOigg21398526 = -311617807;    float PJVyMqDwTRQOigg63477473 = -114574790;    float PJVyMqDwTRQOigg50148554 = -255540378;    float PJVyMqDwTRQOigg50667614 = -558607893;    float PJVyMqDwTRQOigg57795021 = -566404068;    float PJVyMqDwTRQOigg39086248 = -605982793;    float PJVyMqDwTRQOigg58692988 = -157632951;    float PJVyMqDwTRQOigg61581198 = -291998756;    float PJVyMqDwTRQOigg4096268 = -997631349;    float PJVyMqDwTRQOigg98776856 = -707737718;    float PJVyMqDwTRQOigg5003059 = -159693219;    float PJVyMqDwTRQOigg61588386 = -435023458;    float PJVyMqDwTRQOigg18390023 = 72937281;    float PJVyMqDwTRQOigg15861252 = -738871436;    float PJVyMqDwTRQOigg69615516 = -196548896;    float PJVyMqDwTRQOigg21069523 = -532607198;    float PJVyMqDwTRQOigg11478491 = -451645854;    float PJVyMqDwTRQOigg35257712 = -750249685;    float PJVyMqDwTRQOigg65561928 = -410994639;    float PJVyMqDwTRQOigg31787161 = -669836729;    float PJVyMqDwTRQOigg81964393 = -444364513;    float PJVyMqDwTRQOigg9339966 = -785660971;    float PJVyMqDwTRQOigg46231289 = -491936034;    float PJVyMqDwTRQOigg13272613 = -612454283;    float PJVyMqDwTRQOigg41301610 = -964994265;    float PJVyMqDwTRQOigg10723546 = -105948231;    float PJVyMqDwTRQOigg91895600 = -408754051;    float PJVyMqDwTRQOigg88372501 = 93921068;    float PJVyMqDwTRQOigg30514972 = -433947026;    float PJVyMqDwTRQOigg57150565 = -693400334;    float PJVyMqDwTRQOigg19578820 = -996490613;    float PJVyMqDwTRQOigg26884906 = -901662130;    float PJVyMqDwTRQOigg68494018 = -287902803;    float PJVyMqDwTRQOigg62190144 = -521302918;    float PJVyMqDwTRQOigg96344191 = -746484720;    float PJVyMqDwTRQOigg41839680 = 28909920;    float PJVyMqDwTRQOigg38334016 = -592755303;    float PJVyMqDwTRQOigg83547431 = -116220893;    float PJVyMqDwTRQOigg41440909 = -241617757;    float PJVyMqDwTRQOigg30646607 = -366619848;    float PJVyMqDwTRQOigg37117476 = -201351916;    float PJVyMqDwTRQOigg56355792 = -304337055;    float PJVyMqDwTRQOigg73672930 = -192669637;    float PJVyMqDwTRQOigg80346608 = -82127589;    float PJVyMqDwTRQOigg73936424 = -913164840;    float PJVyMqDwTRQOigg55674567 = -194448088;    float PJVyMqDwTRQOigg36693079 = -956898468;    float PJVyMqDwTRQOigg87195207 = -445132862;    float PJVyMqDwTRQOigg67902391 = -765387339;    float PJVyMqDwTRQOigg22153320 = -549377246;    float PJVyMqDwTRQOigg63750805 = -813587277;    float PJVyMqDwTRQOigg70395626 = -971565091;    float PJVyMqDwTRQOigg99279070 = -195005848;    float PJVyMqDwTRQOigg94885017 = -217528984;    float PJVyMqDwTRQOigg52177899 = -501634497;    float PJVyMqDwTRQOigg12058561 = -525956837;    float PJVyMqDwTRQOigg17246185 = -622638757;    float PJVyMqDwTRQOigg36875942 = -643086095;    float PJVyMqDwTRQOigg9366004 = -593613629;    float PJVyMqDwTRQOigg47071476 = -360455838;    float PJVyMqDwTRQOigg47190647 = -97228742;    float PJVyMqDwTRQOigg70320487 = -151554020;    float PJVyMqDwTRQOigg31066227 = -858051730;    float PJVyMqDwTRQOigg46945702 = -204231015;    float PJVyMqDwTRQOigg79198037 = -711247106;    float PJVyMqDwTRQOigg78118152 = -258031090;    float PJVyMqDwTRQOigg93094367 = -47120655;    float PJVyMqDwTRQOigg56199878 = -405759801;    float PJVyMqDwTRQOigg19517060 = -992386717;    float PJVyMqDwTRQOigg27775836 = -125458817;    float PJVyMqDwTRQOigg82735507 = -939851896;    float PJVyMqDwTRQOigg27931059 = -235424961;    float PJVyMqDwTRQOigg93816803 = -408631929;    float PJVyMqDwTRQOigg34915321 = 55625209;    float PJVyMqDwTRQOigg94669685 = -368484814;    float PJVyMqDwTRQOigg25608601 = -40027459;    float PJVyMqDwTRQOigg35667035 = -492991334;    float PJVyMqDwTRQOigg65884680 = -309808445;    float PJVyMqDwTRQOigg39336189 = -699289444;    float PJVyMqDwTRQOigg85627043 = -670546177;    float PJVyMqDwTRQOigg74030466 = -149049763;    float PJVyMqDwTRQOigg4700393 = -963621190;    float PJVyMqDwTRQOigg20470111 = -140691593;    float PJVyMqDwTRQOigg8361653 = -884569781;    float PJVyMqDwTRQOigg93399760 = -879813058;    float PJVyMqDwTRQOigg49183194 = 75074478;    float PJVyMqDwTRQOigg27605836 = -606656282;    float PJVyMqDwTRQOigg73609001 = 29626181;    float PJVyMqDwTRQOigg10012245 = 80331579;    float PJVyMqDwTRQOigg84285631 = -120527883;    float PJVyMqDwTRQOigg24593496 = -348451324;    float PJVyMqDwTRQOigg1458075 = -949669208;    float PJVyMqDwTRQOigg74181427 = -522607265;    float PJVyMqDwTRQOigg94369433 = -881161919;    float PJVyMqDwTRQOigg83455959 = -169391107;    float PJVyMqDwTRQOigg66796988 = 50202103;    float PJVyMqDwTRQOigg25289566 = -446285325;    float PJVyMqDwTRQOigg26727229 = -987365713;     PJVyMqDwTRQOigg26672178 = PJVyMqDwTRQOigg34142293;     PJVyMqDwTRQOigg34142293 = PJVyMqDwTRQOigg21398526;     PJVyMqDwTRQOigg21398526 = PJVyMqDwTRQOigg63477473;     PJVyMqDwTRQOigg63477473 = PJVyMqDwTRQOigg50148554;     PJVyMqDwTRQOigg50148554 = PJVyMqDwTRQOigg50667614;     PJVyMqDwTRQOigg50667614 = PJVyMqDwTRQOigg57795021;     PJVyMqDwTRQOigg57795021 = PJVyMqDwTRQOigg39086248;     PJVyMqDwTRQOigg39086248 = PJVyMqDwTRQOigg58692988;     PJVyMqDwTRQOigg58692988 = PJVyMqDwTRQOigg61581198;     PJVyMqDwTRQOigg61581198 = PJVyMqDwTRQOigg4096268;     PJVyMqDwTRQOigg4096268 = PJVyMqDwTRQOigg98776856;     PJVyMqDwTRQOigg98776856 = PJVyMqDwTRQOigg5003059;     PJVyMqDwTRQOigg5003059 = PJVyMqDwTRQOigg61588386;     PJVyMqDwTRQOigg61588386 = PJVyMqDwTRQOigg18390023;     PJVyMqDwTRQOigg18390023 = PJVyMqDwTRQOigg15861252;     PJVyMqDwTRQOigg15861252 = PJVyMqDwTRQOigg69615516;     PJVyMqDwTRQOigg69615516 = PJVyMqDwTRQOigg21069523;     PJVyMqDwTRQOigg21069523 = PJVyMqDwTRQOigg11478491;     PJVyMqDwTRQOigg11478491 = PJVyMqDwTRQOigg35257712;     PJVyMqDwTRQOigg35257712 = PJVyMqDwTRQOigg65561928;     PJVyMqDwTRQOigg65561928 = PJVyMqDwTRQOigg31787161;     PJVyMqDwTRQOigg31787161 = PJVyMqDwTRQOigg81964393;     PJVyMqDwTRQOigg81964393 = PJVyMqDwTRQOigg9339966;     PJVyMqDwTRQOigg9339966 = PJVyMqDwTRQOigg46231289;     PJVyMqDwTRQOigg46231289 = PJVyMqDwTRQOigg13272613;     PJVyMqDwTRQOigg13272613 = PJVyMqDwTRQOigg41301610;     PJVyMqDwTRQOigg41301610 = PJVyMqDwTRQOigg10723546;     PJVyMqDwTRQOigg10723546 = PJVyMqDwTRQOigg91895600;     PJVyMqDwTRQOigg91895600 = PJVyMqDwTRQOigg88372501;     PJVyMqDwTRQOigg88372501 = PJVyMqDwTRQOigg30514972;     PJVyMqDwTRQOigg30514972 = PJVyMqDwTRQOigg57150565;     PJVyMqDwTRQOigg57150565 = PJVyMqDwTRQOigg19578820;     PJVyMqDwTRQOigg19578820 = PJVyMqDwTRQOigg26884906;     PJVyMqDwTRQOigg26884906 = PJVyMqDwTRQOigg68494018;     PJVyMqDwTRQOigg68494018 = PJVyMqDwTRQOigg62190144;     PJVyMqDwTRQOigg62190144 = PJVyMqDwTRQOigg96344191;     PJVyMqDwTRQOigg96344191 = PJVyMqDwTRQOigg41839680;     PJVyMqDwTRQOigg41839680 = PJVyMqDwTRQOigg38334016;     PJVyMqDwTRQOigg38334016 = PJVyMqDwTRQOigg83547431;     PJVyMqDwTRQOigg83547431 = PJVyMqDwTRQOigg41440909;     PJVyMqDwTRQOigg41440909 = PJVyMqDwTRQOigg30646607;     PJVyMqDwTRQOigg30646607 = PJVyMqDwTRQOigg37117476;     PJVyMqDwTRQOigg37117476 = PJVyMqDwTRQOigg56355792;     PJVyMqDwTRQOigg56355792 = PJVyMqDwTRQOigg73672930;     PJVyMqDwTRQOigg73672930 = PJVyMqDwTRQOigg80346608;     PJVyMqDwTRQOigg80346608 = PJVyMqDwTRQOigg73936424;     PJVyMqDwTRQOigg73936424 = PJVyMqDwTRQOigg55674567;     PJVyMqDwTRQOigg55674567 = PJVyMqDwTRQOigg36693079;     PJVyMqDwTRQOigg36693079 = PJVyMqDwTRQOigg87195207;     PJVyMqDwTRQOigg87195207 = PJVyMqDwTRQOigg67902391;     PJVyMqDwTRQOigg67902391 = PJVyMqDwTRQOigg22153320;     PJVyMqDwTRQOigg22153320 = PJVyMqDwTRQOigg63750805;     PJVyMqDwTRQOigg63750805 = PJVyMqDwTRQOigg70395626;     PJVyMqDwTRQOigg70395626 = PJVyMqDwTRQOigg99279070;     PJVyMqDwTRQOigg99279070 = PJVyMqDwTRQOigg94885017;     PJVyMqDwTRQOigg94885017 = PJVyMqDwTRQOigg52177899;     PJVyMqDwTRQOigg52177899 = PJVyMqDwTRQOigg12058561;     PJVyMqDwTRQOigg12058561 = PJVyMqDwTRQOigg17246185;     PJVyMqDwTRQOigg17246185 = PJVyMqDwTRQOigg36875942;     PJVyMqDwTRQOigg36875942 = PJVyMqDwTRQOigg9366004;     PJVyMqDwTRQOigg9366004 = PJVyMqDwTRQOigg47071476;     PJVyMqDwTRQOigg47071476 = PJVyMqDwTRQOigg47190647;     PJVyMqDwTRQOigg47190647 = PJVyMqDwTRQOigg70320487;     PJVyMqDwTRQOigg70320487 = PJVyMqDwTRQOigg31066227;     PJVyMqDwTRQOigg31066227 = PJVyMqDwTRQOigg46945702;     PJVyMqDwTRQOigg46945702 = PJVyMqDwTRQOigg79198037;     PJVyMqDwTRQOigg79198037 = PJVyMqDwTRQOigg78118152;     PJVyMqDwTRQOigg78118152 = PJVyMqDwTRQOigg93094367;     PJVyMqDwTRQOigg93094367 = PJVyMqDwTRQOigg56199878;     PJVyMqDwTRQOigg56199878 = PJVyMqDwTRQOigg19517060;     PJVyMqDwTRQOigg19517060 = PJVyMqDwTRQOigg27775836;     PJVyMqDwTRQOigg27775836 = PJVyMqDwTRQOigg82735507;     PJVyMqDwTRQOigg82735507 = PJVyMqDwTRQOigg27931059;     PJVyMqDwTRQOigg27931059 = PJVyMqDwTRQOigg93816803;     PJVyMqDwTRQOigg93816803 = PJVyMqDwTRQOigg34915321;     PJVyMqDwTRQOigg34915321 = PJVyMqDwTRQOigg94669685;     PJVyMqDwTRQOigg94669685 = PJVyMqDwTRQOigg25608601;     PJVyMqDwTRQOigg25608601 = PJVyMqDwTRQOigg35667035;     PJVyMqDwTRQOigg35667035 = PJVyMqDwTRQOigg65884680;     PJVyMqDwTRQOigg65884680 = PJVyMqDwTRQOigg39336189;     PJVyMqDwTRQOigg39336189 = PJVyMqDwTRQOigg85627043;     PJVyMqDwTRQOigg85627043 = PJVyMqDwTRQOigg74030466;     PJVyMqDwTRQOigg74030466 = PJVyMqDwTRQOigg4700393;     PJVyMqDwTRQOigg4700393 = PJVyMqDwTRQOigg20470111;     PJVyMqDwTRQOigg20470111 = PJVyMqDwTRQOigg8361653;     PJVyMqDwTRQOigg8361653 = PJVyMqDwTRQOigg93399760;     PJVyMqDwTRQOigg93399760 = PJVyMqDwTRQOigg49183194;     PJVyMqDwTRQOigg49183194 = PJVyMqDwTRQOigg27605836;     PJVyMqDwTRQOigg27605836 = PJVyMqDwTRQOigg73609001;     PJVyMqDwTRQOigg73609001 = PJVyMqDwTRQOigg10012245;     PJVyMqDwTRQOigg10012245 = PJVyMqDwTRQOigg84285631;     PJVyMqDwTRQOigg84285631 = PJVyMqDwTRQOigg24593496;     PJVyMqDwTRQOigg24593496 = PJVyMqDwTRQOigg1458075;     PJVyMqDwTRQOigg1458075 = PJVyMqDwTRQOigg74181427;     PJVyMqDwTRQOigg74181427 = PJVyMqDwTRQOigg94369433;     PJVyMqDwTRQOigg94369433 = PJVyMqDwTRQOigg83455959;     PJVyMqDwTRQOigg83455959 = PJVyMqDwTRQOigg66796988;     PJVyMqDwTRQOigg66796988 = PJVyMqDwTRQOigg25289566;     PJVyMqDwTRQOigg25289566 = PJVyMqDwTRQOigg26727229;     PJVyMqDwTRQOigg26727229 = PJVyMqDwTRQOigg26672178;}
// Junk Finished
