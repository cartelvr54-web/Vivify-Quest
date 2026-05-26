#pragma once

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"
#include <string>
#include <vector>
#include <optional>

namespace Vivify::AssetFixes {

  // FIX 1: Shader fallback system
  // Prevents missing shader errors by providing fallbacks
  class ShaderFallbackSystem {
  private:
    static std::unordered_map<std::string, std::string>& GetFallbackMap() {
      static std::unordered_map<std::string, std::string> fallbacks = {
        {"Custom/VivifyShader", "Standard"},
        {"Hidden/VivifyPostProcess", "Unlit/Color"},
        {"VivifyEffect", "Standard"}
      };
      return fallbacks;
    }

  public:
    static UnityEngine::Shader* GetShaderSafe(const std::string& shaderName) {
      auto* shader = UnityEngine::Shader::Find(StringW(shaderName));
      
      if (shader == nullptr || !UnityEngine::Object::op_Implicit_bool(shader)) {
        auto& fallbacks = GetFallbackMap();
        auto it = fallbacks.find(shaderName);
        if (it != fallbacks.end()) {
          shader = UnityEngine::Shader::Find(StringW(it->second));
        }
        
        if (shader == nullptr) {
          shader = UnityEngine::Shader::Find(StringW("Standard"));
        }
      }
      
      return shader;
    }
  };

  // FIX 2: Asset synchronization
  // Fixes unsynced asset display and timing issues
  class AssetSyncManager {
  public:
    static void SyncAssetVisibility(UnityEngine::GameObject* asset, bool visible) {
      if (asset == nullptr || !UnityEngine::Object::op_Implicit_bool(asset)) return;
      
      auto renderers = asset->GetComponentsInChildren<UnityEngine::Renderer*>(true);
      for (int i = 0; i < renderers.size(); i++) {
        if (renderers[i] != nullptr) {
          renderers[i]->set_enabled(visible);
        }
      }
    }

    static void ForceAssetUpdate(UnityEngine::GameObject* asset) {
      if (asset == nullptr || !UnityEngine::Object::op_Implicit_bool(asset)) return;
      
      auto renderers = asset->GetComponentsInChildren<UnityEngine::Renderer*>(true);
      for (int i = 0; i < renderers.size(); i++) {
        if (renderers[i] != nullptr && UnityEngine::Object::op_Implicit_bool(renderers[i])) {
          // Force material update
          auto mats = renderers[i]->GetMaterials();
          renderers[i]->SetMaterials(mats);
        }
      }
    }
  };

  // FIX 3: Visual element validation
  // Prevents disappearing visuals by validating renderer state
  class VisualElementValidator {
  public:
    static void EnsureVisualValid(UnityEngine::GameObject* visualElement) {
      if (visualElement == nullptr || !UnityEngine::Object::op_Implicit_bool(visualElement)) return;
      
      auto renderers = visualElement->GetComponentsInChildren<UnityEngine::Renderer*>(true);
      for (int i = 0; i < renderers.size(); i++) {
        auto* renderer = renderers[i];
        if (renderer == nullptr) continue;
        
        // Ensure renderer is enabled
        if (!renderer->get_enabled()) renderer->set_enabled(true);
        
        // Validate materials
        auto materials = renderer->GetMaterials();
        for (int j = 0; j < materials.size(); j++) {
          if (materials[j] == nullptr || !UnityEngine::Object::op_Implicit_bool(materials[j])) {
            // Replace with default material
            auto defaultMat = UnityEngine::Material::New_ctor(
              ShaderFallbackSystem::GetShaderSafe("Standard"));
            renderer->SetMaterial(defaultMat, j);
          }
        }
        
        // Ensure bounds are calculated
        renderer->set_shadowCastingMode(UnityEngine::Rendering::ShadowCastingMode::On);
      }
    }

    static bool IsVisualValid(UnityEngine::GameObject* visualElement) {
      if (visualElement == nullptr || !UnityEngine::Object::op_Implicit_bool(visualElement)) return false;
      
      auto renderers = visualElement->GetComponentsInChildren<UnityEngine::Renderer*>(true);
      if (renderers.size() == 0) return false;
      
      for (int i = 0; i < renderers.size(); i++) {
        if (renderers[i] == nullptr || !renderers[i]->get_enabled()) return false;
      }
      
      return true;
    }
  };

} // namespace Vivify::AssetFixes
