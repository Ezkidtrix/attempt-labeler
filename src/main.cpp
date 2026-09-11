#include "ccTypes.h"
#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

struct Settings {
  bool enabled = true;
  ccColor3B color;

  int yoff = 30;
  int maxLabels = 10;
};
static Settings settings;

std::vector<std::string> phrases;

void getPhrases() {
  std::vector<std::string> phrasesSetting = Mod::get()->getSavedValue<std::vector<std::string>>("phrases");
  std::vector<std::string> defaultPhrases = {
    "You got this!",
    "Keep going!",
    "One more attempt.",
    "Stay focused.",
    "That was close!",
    "We are so back.",
    "Don't choke now.",
    "Skill issue detected.",
    "The level has spoken.",
    "Have you considered winning?",
    "You're cooking!",
    "Lock in!",
    "That never happened.",
    "Blame the hitbox.",
    "Absolutely intentional.",
    "We don't talk about that.",
    "Massive W incoming.",
    "The cube has betrayed you.",
    "Perhaps try not dying.",
    "New strategy: survive."
  };

  if (phrasesSetting.empty()) {
    phrases = defaultPhrases;
    Mod::get()->setSavedValue("phrases", defaultPhrases);
  } else {
    phrases = phrasesSetting;
  }
}

std::string randomLabel() {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  std::uniform_int_distribution<> dis(0, phrases.size() - 1);
  return phrases[dis(gen)];
}

class $modify(MyPlayLayer, PlayLayer) {
  struct Fields {
    std::vector<CCLabelBMFont*> m_labels;
  };

  bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
    if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
    if (!settings.enabled) return true;

    getPhrases();
    return true;
  }

  void destroyPlayer(PlayerObject* player, GameObject* object) {
    PlayLayer::destroyPlayer(player, object);
    if (!settings.enabled || !player->isVanillaPlayer() || !player->m_isDead || m_isEditor) return;

    if (m_fields->m_labels.size() >= settings.maxLabels) {
      auto label = m_fields->m_labels[0];

      m_objectLayer->removeChild(label);
      m_fields->m_labels.erase(m_fields->m_labels.begin());
    }
    
    std::string text = randomLabel();
    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    
    label->setScale(0.6);
    label->setColor(settings.color);

    label->setPosition(CCPoint{ player->getPositionX(), player->getPositionY() + settings.yoff });
    m_objectLayer->addChild(label, 1000);

    m_fields->m_labels.push_back(label);
  }

  void keyBackClicked() {
    PlayLayer::keyBackClicked();

    for (auto label : m_fields->m_labels) m_objectLayer->removeChild(label);
    m_fields->m_labels.clear();
  }
};

$on_mod(Loaded) {
  getPhrases();
  settings.enabled = Mod::get()->getSettingValue<bool>("enabled");
  settings.color = Mod::get()->getSettingValue<ccColor3B>("text-color");

  settings.yoff = Mod::get()->getSettingValue<int>("y-offset");
  settings.maxLabels = Mod::get()->getSettingValue<int>("max-labels");

  listenForSettingChanges<bool>("enabled", [](bool value) {
    settings.enabled = value;
  });
  listenForSettingChanges<ccColor3B>("text-color", [](ccColor3B value) {
    settings.color = value;
  });

  listenForSettingChanges<int>("y-offset", [](int value) {
    settings.yoff = value;
  });
  listenForSettingChanges<int>("max-labels", [](int value) {
    settings.maxLabels = value;
  });
};