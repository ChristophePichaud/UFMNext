#pragma once

#include "ShapeFactory.h"

// Shape palette plugin API. A plugin is a shared library (built with the same compiler/runtime
// as UFMNext) exporting these two C symbols:
//
//   extern "C" void        ufmnext_register_shapes(ufmnext::core::ShapeFactory& factory);
//   extern "C" const char* ufmnext_palette_json();   // palette description (see Palette.h)
//
// ufmnext_register_shapes registers the plugin's Shape-derived classes by name;
// ufmnext_palette_json returns a static JSON string listing them.
#if defined(_WIN32)
#define UFMNEXT_PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
#define UFMNEXT_PLUGIN_EXPORT extern "C" __attribute__((visibility("default")))
#endif

#define UFMNEXT_PLUGIN_REGISTER_SYMBOL "ufmnext_register_shapes"
#define UFMNEXT_PLUGIN_PALETTE_SYMBOL "ufmnext_palette_json"

namespace ufmnext::core {
using PluginRegisterFn = void (*)(ShapeFactory&);
using PluginPaletteJsonFn = const char* (*)();
}
