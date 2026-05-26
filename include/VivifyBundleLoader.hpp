#pragma once

#include "UnityEngine/AssetBundle.hpp"
#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace Vivify::BundleLoading {

  enum class BundleFormat {
    Android2021,
    Windows,
    WindowsStandalone,
    Universal,
    Unknown
  };

  // Cross-platform bundle loader
  class CrossPlatformBundleLoader {
  private:
    static std::vector<std::string> GetBundleSearchPatterns() {
      return {
        "bundleAndroid2021.vivify",
        "bundleandroid2021.vivify",
        "bundleWindows.vivify",
        "bundlewindows.vivify",
        "bundleWindows2022.vivify",
        "bundle.vivify",
        "bundle",
        "vivify.bundle"
      };
    }

  public:
    static std::optional<BundleFormat> DetectBundleFormat(const std::string& bundlePath) {
      if (!std::filesystem::exists(bundlePath)) {
        return std::nullopt;
      }

      std::string filename = std::filesystem::path(bundlePath).filename().string();
      std::string lower_filename = filename;
      std::transform(lower_filename.begin(), lower_filename.end(), lower_filename.begin(), ::tolower);

      if (lower_filename.find("android") != std::string::npos || lower_filename.find("2021") != std::string::npos) {
        return BundleFormat::Android2021;
      }
      if (lower_filename.find("windows") != std::string::npos) {
        if (lower_filename.find("2022") != std::string::npos) {
          return BundleFormat::WindowsStandalone;
        }
        return BundleFormat::Windows;
      }
      if (lower_filename.find("universal") != std::string::npos) {
        return BundleFormat::Universal;
      }

      return BundleFormat::Unknown;
    }

    static UnityEngine::AssetBundle* LoadBundleFromPath(const std::string& levelPath) {
      auto patterns = GetBundleSearchPatterns();
      
      for (const auto& pattern : patterns) {
        std::string fullPath = levelPath;
        if (!fullPath.empty() && fullPath.back() != '/' && fullPath.back() != '\\') {
          fullPath += '/';
        }
        fullPath += pattern;
        
        if (std::filesystem::exists(fullPath)) {
          auto format = DetectBundleFormat(fullPath);
          if (format.has_value()) {
            auto* bundle = UnityEngine::AssetBundle::LoadFromFile(StringW(fullPath));
            if (bundle != nullptr && UnityEngine::Object::op_Implicit_bool(bundle)) {
              return bundle;
            }
          }
        }
      }
      
      return nullptr;
    }

    static bool TryLoadBundleAndroid(const std::string& levelPath, UnityEngine::AssetBundle*& outBundle) {
      std::string bundlePath = levelPath;
      if (!bundlePath.empty() && bundlePath.back() != '/' && bundlePath.back() != '\\') {
        bundlePath += '/';
      }
      bundlePath += "bundleAndroid2021.vivify";
      
      if (std::filesystem::exists(bundlePath)) {
        outBundle = UnityEngine::AssetBundle::LoadFromFile(StringW(bundlePath));
        return outBundle != nullptr && UnityEngine::Object::op_Implicit_bool(outBundle);
      }
      
      return false;
    }

    static bool TryLoadBundleWindows(const std::string& levelPath, UnityEngine::AssetBundle*& outBundle) {
      std::vector<std::string> windowsPatterns = {
        "bundleWindows.vivify",
        "bundleWindows2022.vivify",
        "bundle.vivify"
      };
      
      for (const auto& pattern : windowsPatterns) {
        std::string bundlePath = levelPath;
        if (!bundlePath.empty() && bundlePath.back() != '/' && bundlePath.back() != '\\') {
          bundlePath += '/';
        }
        bundlePath += pattern;
        
        if (std::filesystem::exists(bundlePath)) {
          outBundle = UnityEngine::AssetBundle::LoadFromFile(StringW(bundlePath));
          if (outBundle != nullptr && UnityEngine::Object::op_Implicit_bool(outBundle)) {
            return true;
          }
        }
      }
      
      return false;
    }
  };

} // namespace Vivify::BundleLoading
