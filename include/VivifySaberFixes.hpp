#pragma once

#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberModelController.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Renderer.hpp"
#include <optional>

namespace Vivify::SaberFixes {

  // FIX 1: Saber offset correction
  // Fixes custom sabers that are positioned incorrectly
  class SaberOffsetFixer {
  public:
    struct SaberOffsetData {
      UnityEngine::Vector3 positionOffset;
      UnityEngine::Vector3 rotationOffset;
      UnityEngine::Vector3 scaleMultiplier;
    };

    static void ApplySaberOffset(UnityEngine::Transform* saberTransform, const SaberOffsetData& offset) {
      if (saberTransform == nullptr) return;
      
      auto currentPos = saberTransform->get_localPosition();
      saberTransform->set_localPosition(
        UnityEngine::Vector3(
          currentPos.x + offset.positionOffset.x,
          currentPos.y + offset.positionOffset.y,
          currentPos.z + offset.positionOffset.z
        )
      );
      
      auto currentRot = saberTransform->get_localEulerAngles();
      saberTransform->set_localEulerAngles(
        UnityEngine::Vector3(
          currentRot.x + offset.rotationOffset.x,
          currentRot.y + offset.rotationOffset.y,
          currentRot.z + offset.rotationOffset.z
        )
      );
      
      auto currentScale = saberTransform->get_localScale();
      saberTransform->set_localScale(
        UnityEngine::Vector3(
          currentScale.x * offset.scaleMultiplier.x,
          currentScale.y * offset.scaleMultiplier.y,
          currentScale.z * offset.scaleMultiplier.z
        )
      );
    }

    static SaberOffsetData GetDefaultOffset() {
      return {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 1.0f}
      };
    }
  };

  // FIX 2: Saber visual validation
  // Ensures custom saber visuals are properly displayed
  class SaberVisualValidator {
  public:
    static void EnsureSaberVisible(UnityEngine::GameObject* saberModel) {
      if (saberModel == nullptr || !UnityEngine::Object::op_Implicit_bool(saberModel)) return;
      
      saberModel->SetActive(true);
      
      auto renderers = saberModel->GetComponentsInChildren<UnityEngine::Renderer*>(true);
      for (int i = 0; i < renderers.size(); i++) {
        if (renderers[i] != nullptr && UnityEngine::Object::op_Implicit_bool(renderers[i])) {
          renderers[i]->set_enabled(true);
        }
      }
    }

    static void ValidateSaberHierarchy(UnityEngine::GameObject* saberModel) {
      if (saberModel == nullptr) return;
      
      auto transform = saberModel->get_transform();
      if (transform == nullptr) return;
      
      // Verify transform is active
      if (!transform->get_gameObject()->activeSelf()) {
        transform->get_gameObject()->SetActive(true);
      }
      
      // Check all child renderers
      auto renderers = saberModel->GetComponentsInChildren<UnityEngine::Renderer*>(true);
      for (int i = 0; i < renderers.size(); i++) {
        auto* renderer = renderers[i];
        if (renderer == nullptr) continue;
        
        // Ensure renderer's game object is active
        if (!renderer->get_gameObject()->activeSelf()) {
          renderer->get_gameObject()->SetActive(true);
        }
        
        // Ensure renderer is enabled
        if (!renderer->get_enabled()) {
          renderer->set_enabled(true);
        }
      }
    }
  };

  // FIX 3: Arrow rendering fix
  // Ensures custom note arrows display correctly on custom sabers
  class NoteArrowRenderer {
  public:
    static void RefreshArrowVisuals(UnityEngine::GameObject* noteModel) {
      if (noteModel == nullptr || !UnityEngine::Object::op_Implicit_bool(noteModel)) return;
      
      // Re-enable all renderers to refresh arrow display
      auto renderers = noteModel->GetComponentsInChildren<UnityEngine::Renderer*>(true);
      for (int i = 0; i < renderers.size(); i++) {
        auto* renderer = renderers[i];
        if (renderer == nullptr) continue;
        
        renderer->set_enabled(false);
        renderer->set_enabled(true);
      }
    }
  };

} // namespace Vivify::SaberFixes
