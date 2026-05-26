#pragma once

#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Camera.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <algorithm>

namespace Vivify::RenderingFixes {

  // FIX 1: Blit validation and error handling
  // Prevents null pointer crashes in screen effect rendering
  class BlitValidator {
  public:
    static bool ValidateBlitMaterial(UnityEngine::Material* material) {
      if (material == nullptr) return false;
      if (!UnityEngine::Object::op_Implicit_bool(material)) return false;
      return true;
    }

    static bool ValidateRenderTexture(UnityEngine::RenderTexture* rt) {
      if (rt == nullptr) return false;
      if (!UnityEngine::Object::op_Implicit_bool(rt)) return false;
      if (!rt->IsCreated()) return false;
      return true;
    }

    static bool CanBlitSafely(UnityEngine::Texture* src, UnityEngine::RenderTexture* dest) {
      return src != nullptr && ValidateRenderTexture(dest);
    }
  };

  // FIX 2: Improved render texture creation with fallback
  // Handles devices with limited render texture support (Quest devices)
  class RenderTextureFactory {
  public:
    static UnityEngine::RenderTexture* CreateOptimizedRT(
        int width, int height, int depth, UnityEngine::RenderTextureFormat format) {
      
      // Clamp to reasonable values for Quest devices
      width = std::clamp(width, 64, 2048);
      height = std::clamp(height, 64, 2048);
      
      auto* rt = UnityEngine::RenderTexture::New_ctor(width, height, depth, format);
      if (rt != nullptr) {
        rt->set_filterMode(UnityEngine::FilterMode::Bilinear);
        if (!rt->Create()) {
          // Fallback: smaller RT if creation fails
          UnityEngine::Object::Destroy(rt);
          rt = UnityEngine::RenderTexture::New_ctor(
              std::max(64, width / 2), 
              std::max(64, height / 2), 
              depth, 
              UnityEngine::RenderTextureFormat::ARGB32);
          if (rt != nullptr) {
            rt->set_filterMode(UnityEngine::FilterMode::Point);
            rt->Create();
          }
        }
      }
      return rt;
    }
  };

  // FIX 3: Camera property safety
  // Prevents invalid camera configurations that cause black screens
  class CameraPropertyValidator {
  public:
    static void EnsureCameraValid(UnityEngine::Camera* camera) {
      if (camera == nullptr) return;
      
      // Verify camera is enabled
      if (!camera->get_enabled()) camera->set_enabled(true);
      
      // Ensure nearClip is reasonable
      if (camera->get_nearClipPlane() <= 0.0f) 
        camera->set_nearClipPlane(0.1f);
      
      // Ensure farClip > nearClip
      if (camera->get_farClipPlane() <= camera->get_nearClipPlane()) 
        camera->set_farClipPlane(camera->get_nearClipPlane() + 1000.0f);
      
      // Verify FOV is valid
      if (camera->get_fieldOfView() <= 0.0f || camera->get_fieldOfView() > 180.0f)
        camera->set_fieldOfView(60.0f);
    }
  };

  // FIX 4: Screen texture resolution optimization
  // Prevents black screens caused by zero-size textures
  class ScreenTextureOptimizer {
  public:
    static std::pair<int, int> GetOptimalResolution(float xRatio, float yRatio, int targetWidth, int targetHeight) {
      // Ensure ratios are valid and clamped
      float safeXRatio = std::max(0.1f, std::min(2.0f, xRatio));
      float safeYRatio = std::max(0.1f, std::min(2.0f, yRatio));
      
      int w = static_cast<int>(targetWidth / safeXRatio);
      int h = static_cast<int>(targetHeight / safeYRatio);
      
      // Clamp to reasonable values
      w = std::clamp(w, 64, 2048);
      h = std::clamp(h, 64, 2048);
      
      return {w, h};
    }
  };

} // namespace Vivify::RenderingFixes
