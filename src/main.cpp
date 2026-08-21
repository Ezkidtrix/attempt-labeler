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
  bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
    if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
    if (!settings.enabled) return true;

    getPhrases();
    return true;
  }
};

class $modify(MyPlayerObject, PlayerObject) {
  struct Fields {
    CCLabelBMFont* m_label1;
    CCLabelBMFont* m_label2;
  };

  void playDeathEffect() {
    PlayerObject::playDeathEffect();
    if (!settings.enabled || !PlayerObject::isVanillaPlayer() || !m_gameLayer || m_gameLayer->m_isEditor) return;
    
    std::string text = randomLabel();

    if (m_fields->m_label1) m_gameLayer->m_objectLayer->removeChild(m_fields->m_label1);
    if (m_fields->m_label2) m_gameLayer->m_objectLayer->removeChild(m_fields->m_label2);

    if (isPlayer1()) {
      m_fields->m_label1 = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
      m_fields->m_label1->setScale(0.6);
      
      m_fields->m_label1->setColor(settings.color);
      m_fields->m_label1->setPosition(CCPoint{ m_position.x, m_position.y + 30 });
      
      m_gameLayer->m_objectLayer->addChild(m_fields->m_label1, 1000);
    }
    
    if (isPlayer2()) {
      m_fields->m_label2 = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
      m_fields->m_label2->setScale(0.6);
      
      m_fields->m_label2->setColor(settings.color);
      m_fields->m_label2->setPosition(CCPoint{ m_position.x, m_position.y + 30 });
      
      m_gameLayer->m_objectLayer->addChild(m_fields->m_label2, 1000);
    }
  }
};

$on_mod(Loaded) {
  getPhrases();

  settings.enabled = Mod::get()->getSettingValue<bool>("enabled");
  settings.color = Mod::get()->getSettingValue<ccColor3B>("text-color");

  listenForSettingChanges<bool>("enabled", [](bool value) {
    settings.enabled = value;
  });

  listenForSettingChanges<ccColor3B>("text-color", [](ccColor3B value) {
    settings.color = value;
  });
};