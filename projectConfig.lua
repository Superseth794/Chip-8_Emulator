workspace "Chip-8"
   configurations { "Debug", "Release" }

project "Chip-8"
   kind "ConsoleApp"

   language "C++"
   cppdialect("C++17")

   targetdir "Executable/%{cfg.buildcfg}"

   files {
         "Chip-8_Emulator/**.hpp",
         "Chip-8_Emulator/**.cpp",
         "Chip-8_Emulator/**.h"
      }

      includedirs {
      "SFML/include"
   }

   links {
      "sfml-graphics",
      "sfml-window",
      "sfml-system",
      "sfml-audio"
   }

   libdirs {
      "SFML/lib"
   }