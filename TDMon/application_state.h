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

#include <TDMon/constants.h>

#include <TGUI/TGUI.hpp>

namespace tdmon {
/**
 * @brief Interface for all states of the application (main menu, setup menu,
 * etc...). Implementations of this interface each represent a state and are
 * responsible for setting up their UI and UI callbacks. "State" refers to the
 * use of the "State" pattern.
 */
class ApplicationState {
 public:
  /**
   * @brief Virtual default destructor to allow deletion of derived classes
   * from a pointer to this class
   */
  virtual ~ApplicationState() = default;

  /**
   * @brief Initialize this application state. Add gui elements to the gui
   * object.
   * @param gui The gui
   */
  virtual void init(tgui::GuiSFML& gui) = 0;

  /**
   * @brief Update this application state.
   * @return Indicate which (if any) application state change should take place.
   * If you want to stay in the same state, return kNull.
   */
  virtual SupportedApplicationStateChanges update() = 0;

  /**
   * @brief Remove gui elements which were added in init(). Also do other
   * cleanup, if applicable.
   * @param gui
   */
  virtual void cleanup(tgui::GuiSFML& gui) = 0;

  /**
   * @brief Get the type of application state for this object
   * @return The application state type for this object
   */
  virtual SupportedApplicationStateTypes getApplicationStateType() const = 0;
};
}  // namespace tdmon
