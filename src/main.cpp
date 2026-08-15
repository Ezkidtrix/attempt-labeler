#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

struct Settings {
  bool enabled = true;
  int triggerChance = 500;
};
static Settings settings;

CCLabelBMFont* label = nullptr;
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
  void playDeathEffect() {
    PlayerObject::playDeathEffect();

    if (!settings.enabled || PlayLayer::get()->m_isEditor) return;
    auto objectLayer = PlayLayer::get()->m_objectLayer;

    if (label) objectLayer->removeChild(label);
    std::string text = randomLabel();

    label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setScale(0.6);

    CCPoint pos = this->getPosition();
    label->setPosition(CCPoint{ pos.x, pos.y + 30 });

    objectLayer->addChild(label, 1000);
  }
};

$on_mod(Loaded) {
  getPhrases();
  settings.enabled = Mod::get()->getSettingValue<bool>("enabled");

  listenForSettingChanges<bool>("enabled", [](bool value) {
    settings.enabled = value;
  });
};