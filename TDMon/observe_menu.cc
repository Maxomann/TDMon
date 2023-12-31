/*********************************
 *
 * TD-Mon - Copyright 2023 (c) Kay Leon Gonschior
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”,
 * WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *********************************/

#include <TDMon/constants.h>
#include <TDMon/observe_menu.h>

#include <format>
#include <iostream>

namespace tdmon {
ObserveMenu::ObserveMenu(TdMonCache& tdmon_cache, TdMonFactory& tdmon_factory)
    : tdmon_cache_(tdmon_cache), tdmon_factory_(tdmon_factory) {}

void ObserveMenu::init(tgui::GuiSFML& gui) {
  observe_menu_group_ = tgui::Group::create();

  back_button_ = tgui::Button::create(UiConstants::kBackButtonText);
  back_button_->setPosition(0, 0);
  back_button_->setSize(100, 50);
  back_button_->setTextSize(UiConstants::kButtonFontSize);
  back_button_->onPress.connect([&]() {
    next_application_state_change_ =
        SupportedApplicationStateChanges::kMainMenu;
  });
  observe_menu_group_->add(back_button_);

  refresh_button_ = tgui::Button::create(UiConstants::kRefreshButtonText);
  refresh_button_->setPosition("parent.width - width", 0);
  refresh_button_->setSize(100, 50);
  refresh_button_->setTextSize(UiConstants::kButtonFontSize);

  refresh_button_->onPress.connect([&]() {
    // update the td-mon and relevant UI
    try {
      refreshTdMon(false);
    } catch (std::exception e) {
      std::cout << "cannot update TdMon. Reason: " << e.what() << std::endl;
    }
  });
  observe_menu_group_->add(refresh_button_);

  tdmon_picture_ = tgui::Picture::create();
  tdmon_picture_->setPosition(100, 100);
  tdmon_picture_->setSize(400, 400);
  observe_menu_group_->add(tdmon_picture_);

  tdmon_data_label_ = tgui::Label::create("No data");
  tdmon_data_label_->setTextSize(UiConstants::kLabelFontSize);
  tdmon_data_label_->setPosition(0, 520);
  observe_menu_group_->add(tdmon_data_label_);

  gui.add(observe_menu_group_);

  // initialize the td-mon and relevant UI
  try {
    refreshTdMon(true);
  } catch (std::exception e) {
    std::cout
        << "cannot initialize TdMon. Reason: " << e.what()
        << "\nPlease make sure that you entered correct information in the setup."
        << std::endl;
  }

}

SupportedApplicationStateChanges ObserveMenu::update() {
  return next_application_state_change_;
}

void ObserveMenu::cleanup(tgui::GuiSFML& gui) {
  gui.remove(observe_menu_group_);
}

SupportedApplicationStateTypes ObserveMenu::getApplicationStateType() const {
  return SupportedApplicationStateTypes::kObserveMenu;
}

void ObserveMenu::refreshTdMon(bool prefer_cache) {
  // if no cache exists OR cache should not be preferred, create a new TdMon
  // from factory
  if (!prefer_cache || !tdmon_cache_.hasCache()) {
    try {
      std::unique_ptr<TdMon> td_mon = tdmon_factory_.create();
      tdmon_cache_.updateCache(std::move(td_mon));
    } catch (std::exception e) {
      std::cout << "error while updating TD-Mon: " << e.what() << std::endl;
    }
  }

  const TdMon* currentTdMon = tdmon_cache_.getCache();
  if (!currentTdMon) {
    throw std::exception("ObserveMenu::refreshTdMon currentTdMon is nullptr");
  }

  // convert stored timestamp to a time point, then to zoned_time (local
  // timezone)
  std::chrono::system_clock::time_point time_point{
      tdmon_cache_.getLastUpdatedTimestamp()};
  std::chrono::zoned_time zoned_time{std::chrono::current_zone(), time_point};

  // update UI & visual representation of the TdMon
  tdmon_data_label_->setText(
      "Level: " + std::to_string(currentTdMon->getLevel()) +
      "\nAttack: " + std::to_string(currentTdMon->getAttackValue()) +
      " || Defense: " + std::to_string(currentTdMon->getDefenseValue()) +
      " || Speed: " + std::to_string(currentTdMon->getSpeedValue()) +
      // use std::format to display the zoned_time in a
      "\nLast updated: " + std::format("{:%x %T}", zoned_time));

  // visual representation

  tdmon_visual_representation_.loadFromFile(currentTdMon->getTexturePath());
  tdmon_picture_->getRenderer()->setTexture(tdmon_visual_representation_);
}
}  // namespace tdmon
