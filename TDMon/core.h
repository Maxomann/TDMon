/*********************************
 *
 * TD-Mon - Copyright 2023 (c) Kay Leon Gonschior
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the �Software�), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED �AS IS�,
 * WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *********************************/

#pragma once

#include <TDMon/application_state.h>
#include <TDMon/constants.h>
#include <TDMon/td_mon_cache.h>
#include <TDMon/td_mon_factory.h>

#include <SFML/Graphics.hpp>
#include <TGUI/Backends/SFML.hpp>
#include <TGUI/TGUI.hpp>
#include <concepts>
#include <iostream>
#include <memory>

namespace tdmon {
/**
 * @brief The core of the application. Handles the window, gui and application
 * states. Creates one instance each of: TdMonCacheType and TdMonFactoryType to
 * pass them to the appropriate application states where they are needed. Uses
 * the MainMenuType, SetupMenuType and ObserveMenuType to switch to different
 * application states respectively.
 *
 * @tparam TdMonFactoryType The td-mon factory to use. Must inherit from
 * TdMonFactory.
 * @tparam TdMonCacheType The td-mon cache type to use. Must inherit from
 * TdMonCache.
 * @tparam MainMenuType The main menu type to use. Must inherit from
 * ApplicationState.
 * @tparam SetupMenuType The setup menu type to use. Must inherit from
 * ApplicationState.
 * @tparam ObserveMenuType The observe menu type to use. Must inherit from
 * ApplicationState.
 */
template <class TdMonFactoryType, class TdMonCacheType, class MainMenuType,
          class SetupMenuType, class ObserveMenuType>
  requires std::constructible_from<SetupMenuType, TdMonFactoryType&> &&
           std::constructible_from<ObserveMenuType, TdMonCacheType&,
                                   TdMonFactoryType&> &&
           std::derived_from<TdMonFactoryType, TdMonFactory> &&
           std::derived_from<TdMonCacheType, TdMonCache> &&
           std::derived_from<MainMenuType, ApplicationState> &&
           std::derived_from<SetupMenuType, ApplicationState> &&
           std::derived_from<ObserveMenuType, ApplicationState>
class Core {
 public:
  /**
   * @brief The constructor. Creates instances of the dependencies passed via
   * template arguments (dependency injection to follow SOLID)
   */
  Core()
      : tdmon_factory_(std::make_unique<TdMonFactoryType>()),
        tdmon_cache_(std::make_unique<TdMonCacheType>()) {
    application_state_ = std::make_unique<MainMenuType>();
  };

  /**
   * @brief run the application
   */
  void run() {
    // try to load the cache from disk
    if (tdmon_cache_->existsOnDisk()) {
      tdmon_cache_->loadFromDisk();
    }

    // init window and gui
    window_.create(sf::VideoMode(600, 600), "Technical Debt Monsters!",
                   sf::Style::Close);
    gui_.setTarget(window_);

    application_state_->init(gui_);

    while (window_.isOpen()) {
      sf::Event event;
      while (window_.pollEvent(event)) {
        gui_.handleEvent(event);

        if (event.type == sf::Event::Closed) window_.close();
      }

      // update the application state
      // if false is returned, close the application
      if (!updateApplicationState()) {
        window_.close();
      }

      window_.clear(sf::Color(186, 186, 186));
      gui_.draw();
      window_.display();
    }

    // if the cache contains data, write it to disk when closing the application
    if (tdmon_cache_->hasCache()) {
      tdmon_cache_->storeOnDisk();
    }

    application_state_->cleanup(gui_);
  };

 private:
  /**
   * @brief The window.
   */
  sf::RenderWindow window_;
  /**
   * @brief The gui
   */
  tgui::GuiSFML gui_;

  /**
   * @brief The TdMonFactory to use in the application.
   */
  std::unique_ptr<TdMonFactoryType> tdmon_factory_ = nullptr;
  /**
   * @brief The TdMonCache to use in the application.
   */
  std::unique_ptr<TdMonCacheType> tdmon_cache_ = nullptr;

  /**
   * @brief The previous application state. This is cached to support the
   * kPrevious state change.
   */
  SupportedApplicationStateTypes previous_state_type_ =
      SupportedApplicationStateTypes::kNull;
  /**
   * @brief The current application state
   */
  std::unique_ptr<ApplicationState> application_state_;

  /**
   * @brief Update the current application state. May trigger a switch to a
   * different application state, if the current state requests it.
   * @return return true, if the application should continue to run. Return
   * false, if the application should be closed.
   */
  bool updateApplicationState() {
    // Update the current application state and handle possibly requested state
    // changes
    switch (SupportedApplicationStateChanges requested_state_change =
                application_state_->update();
            requested_state_change) {
      case tdmon::SupportedApplicationStateChanges::kNull:
        // do nothing, continue with current state
        break;
      case tdmon::SupportedApplicationStateChanges::kPrevious:
        if (previous_state_type_ != SupportedApplicationStateTypes::kNull) {
          switchToApplicationState(previous_state_type_);
        } else {
          throw std::exception("previous application state is kNull");
        }
        break;
      case tdmon::SupportedApplicationStateChanges::kMainMenu:
        switchToApplicationState(SupportedApplicationStateTypes::kMainMenu);
        break;
      case tdmon::SupportedApplicationStateChanges::kSetupMenu:
        switchToApplicationState(SupportedApplicationStateTypes::kSetupMenu);
        break;
      case tdmon::SupportedApplicationStateChanges::kObserveMenu:
        switchToApplicationState(SupportedApplicationStateTypes::kObserveMenu);
        break;
      case tdmon::SupportedApplicationStateChanges::kClose:
        return false;
        break;
      default:
        throw std::exception("requested state change not supported");
        break;
    }

    return true;
  };

  /**
   * @brief Switch to a different application state. Also, update
   * previous_state_type_.
   * @param state The new state type to switch to
   */
  void switchToApplicationState(SupportedApplicationStateTypes new_state_type) {
    std::unique_ptr<ApplicationState> new_application_state = nullptr;

    // Construct new state
    switch (new_state_type) {
      case tdmon::SupportedApplicationStateTypes::kNull:
        throw std::exception("cannot switch to application state type kNull");
        break;
      case tdmon::SupportedApplicationStateTypes::kMainMenu:
        new_application_state = std::make_unique<MainMenuType>();
        break;
      case tdmon::SupportedApplicationStateTypes::kSetupMenu:
        new_application_state =
            std::make_unique<SetupMenuType>(*tdmon_factory_);
        break;
      case tdmon::SupportedApplicationStateTypes::kObserveMenu:
        new_application_state =
            std::make_unique<ObserveMenuType>(*tdmon_cache_, *tdmon_factory_);
        break;
      default:
        throw std::exception("new_state_type not supported");
        break;
    }

    // store previous state type to enable switching back to the previous state
    // later
    previous_state_type_ = application_state_->getApplicationStateType();

    // change to the new state
    if (new_application_state != nullptr) {
      // run cleanup on current applications state
      application_state_->cleanup(gui_);
      // switch application state
      application_state_ = std::move(new_application_state);
      // run init on new application state
      application_state_->init(gui_);
    } else {
      throw std::exception("new_application_state is nullptr");
    }
  }
};
}  // namespace tdmon
