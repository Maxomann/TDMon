﻿# Technical debt monsters

## Table of Contents
- [Introduction]
- [Setting up the project (Getting started)]
- [Updating the dependencies]
- [Generating Documentation & Running Tests]
- [Important Data Structures & UML Class Diagram]
	- [Pure Virtual Classes (Interfaces)]
	- [Implementation Classes]
	- [Enum Classes]
- [Allowing other data sources (Extending the project)]
- [Changing which issues categories to parse from the "Technical Debt Dataset"]

## Introduction

This project is a proof of concept.
It introduces gamification in technical debt management by giving developers mascots
(technical debt monsters) with strengths based on metrics related to technical debt items
in the backlog.

The monster's appearance changes depending on its level.

The monster's values are based on:
- Attack: The amount of technical debt items closed by a developer
- Defense: The amount of technical debt items opened by a developer
- Speed: The cumulated amount of watchers on all technical debt items opened by a developer
- Level: The average (mean) value of the above three

## Setting up the project (Getting started)

Instructions are provided for Windows.

To set up the project, please follow the steps below:
1. Make sure the following are installed on your system:
	- Microsoft Visual Studio Community 2022 (64-bit) - Preview
Version 17.7.0 Preview 4.0 or newer. [Download](https://visualstudio.microsoft.com/de/vs/) (Please make sure to select the "Desktop development with C++" workload in the installer. If you want to run VisualStudio in English, please make sure to also select "English" in the "Language Packs" tab.)
	- [Git](https://git-scm.com/) & [Git LFS](https://git-lfs.com/)
	- Vcpkg. [Install instructions](https://vcpkg.io/en/getting-started.html) (You do not need to run any install commands for packages in the console. This is handled by the build scrips. But, please make sure to run `vcpkg integrate install` at least once before opening the Td-Mon project.)

2. Clone the Td-Mon [git repository](https://github.com/Maxomann/TDMon) to your pc.

3. Open the project using the "open folder" command in Visual Studio. (When opening the folder for the first time, VisualStudio generates the CMake cache and installs dependencies. This may take a while.) Please make sure to select the topmost folder containing a `CMakeLists.txt`. Subfolders may also contain `CMakeLists.txt` files, but opening those causes issues.

4. Compile and run.
	- In the shortcut bar at the top, it says: "Select Startup Item...". Please select `TdMon.exe` as the startup item.
	- Click the Button with the green arrow to build and run the application.

## Updating the dependencies

To update all dependencies at once, please follow the steps below:

1. Make sure your cloned vcpkg repository is up-to-date. If in doubt, run `git pull` in the folder where you cloned the vcpkg repository.
2. Go to the folder of the Td-Mon repository and open `vcpkg.json` in any text editor of your choice.
3. Replace the value for `builtin-baseline` with the hash value of the newest commit in the vcpkg repository (Get the hash from [here](https://github.com/microsoft/vcpkg/commits/master)).
4. Re-generate CMake cache in Visual Studio. (This usually happens automatically.)

To update individual dependencies, please see the vcpkg [documentation](https://learn.microsoft.com/en-us/vcpkg/users/versioning.concepts).

## Generating Documentation & Running Tests

For generating beautiful documentation, I recommend running [Doxygen](https://www.doxygen.nl/index.html) in the project folder.

Unit tests are available for all units, except for UI classes (ApplicationStates). The project includes GTest as testing framework. Mock objects are created using gMock.
To run unit tests, please follow the steps below:
1. Make sure the `Test Adapter for Google Test` is installed as part of your Visual Studio workload. If not, please add it to your installation, using the Visual Studio Installer.
2. Open the project in Visual Studio
3. Build the `TDMonTests.exe` target or build all `Build > Build All`
4. Open the test explorer under `View > Test Explorer`
5. Run the tests. (If, for whatever reason, the Test Explorer does not discover any tests, please run the TDMonTests target manually. This runs all tests in the console window.)

## Important Data Structures & UML Class Diagram

The following sections list all relevant pure virtual classes (interfaces) and all other classes & enumerations respectively.

Their relationship is shown in this UML Class Diagram:
![UML Class Diagram of TD-Mon](./tdmon.svg)

### Pure Virtual Classes (Interfaces)

| Class Name    | Description |
| -------- | ------- |
| TdMonFactory | Interface for TdMon factory implementations. It's purpose is to create instances of classes that inherit from the TdMon interface. The "Factory" pattern is used to create the TdMon, while supporting different data sources. On can implement a factory that creates TdMon instances from a Jira data source and another factory that create TdMon instances from an Azure data source for example. The concrete factory to use can be selected at compile time, as a template parameter in the Core class. |
| ConnectableToDataSources    | Interface for any class that supports connection to one or multiple data source(s) (sql database, Jira, etc...). Its purpose is to allow checking, if all required login information is available in the implementing class, connecting to data sources and checking the current status of the connection (connected or disconnected). |
| TechnicalDebtDatasetAccessInformationContainer | Interface class for any implementation which stores access information to the technical debt dataset. Information needed to access the technical debt dataset is: the path to the sqlite database on disk; the user-identifier to parse the data for (the dataset contains data for many different maintainers, but td-mon is intended to parse the data for one person.     |
| TdMonCache | Interface for storage container classes which allow storing of a td-mon, as well as, serialization/deserialization to a file on disk. Also stores the timestamp when it was last modified. The purpose of the TdMonCache is to allow viewing of a previously generated TdMon without regenerating it from a factory. This removes the need to enter login information (like Jira username and password) every time one starts the application to view their TdMon. |
| TdMon | The technical debt monster interface. The purpose of this interface is to represent a technical debt monster with its unique attack, defense and speed values. It also allows requesting the currently appropriate display texture for the TdMon, as well as, serialize it to json. |
| ApplicationState | Interface for all states of the application (main menu, setup menu, etc...). Implementations of this interface each represent a state and are responsible for setting up their UI and UI callbacks. "State" refers to the use of the "State" pattern. |

### Implementation Classes

| Class Name    | Description |
| -------- | ------- |
| Core  | The core of the application. Handles the window, gui and application states. Creates one instance each of: TdMonCacheType and TdMonFactoryType to pass them to the appropriate application states where they are needed. Uses the MainMenuType, SetupMenuType and ObserveMenuType to switch to different application states respectively. |
| TechnicalDebtDatasetConnectableDefaultTdMonFactory | The implementation for a td-mon factory which can be connected to the technical debt dataset     |
| DefaultTdMonCache | The default implementation of the TdMonCache. This implementation currently only supports serialization/deserialization of DefaultTdMon objects |
| DefaultTdMon | Implementation of the default TD-Mon. Has fixed paths to textures and level caps for different version of the textures. |
| MainMenu | The main menu ApplicationState. Responsible for allowing the user to select which Use-Case to access. |
| ObserveMenu | The observe menu application state. Responsible for displaying the td-mon from cache and updating it from the td-mon factory passed in the constructor, if requested by the click of a button. |
| TechnicalDebtDatasetSetupMenu | This setup menu can set up any type of td-mon factory that implements the required interfaces. |
| UiConstants | Global UI constants for the application. E.g. text strings or font size. |

### Enum Classes

| Class Name    | Description |
| -------- | ------- |
| SupportedApplicationStateTypes | The supported application states. This is required to allow dependency injection through templates in the core, while still being able to switch between different 'types' of states. |
| SupportedApplicationStateChanges | The supported application state changes |

## Allowing other data sources (Extending the project)

By default, TD-Mon supports connecting to a technical debt dataset database in sqlite format.
To enable other data sources (for example Jira), please follow the steps below:

1. Implement a custom TdMon factory. As an example, please look at `TechnicalDebtDatasetConnectableDefaultTdMonFactory`. It is recommended that your custom factory implements (inherits from) the following interfaces:
	- `TdMonFactory` (Necessary)
	- `ConnectableToDataSources` (Optional, but recommended)
	- A custom interface for a container that allows storing the information you need to access your specific data-source. Like the currently existing `TechnicalDebtDatasetAccessInformationContainer`, but modified to suit your needs. You might call your interface `JiraAccessInformationContainer`. (Optional, but recommended)
2. Implement a custom setup menu. As an example, please look at the currently existing `TechnicalDebtDatasetSetupMenu`. Your setup menu must inherit from `ApplicationState`. The setup menu handles the GUI needed to feed login data (or other information) to your factory. Your setup menu must take one constructor parameter: a reference to the `TdMonFactory` implementation you created in step 1. These constraints are checked at compile time. Compilation fails, if these constraints are not adhered to.
3. Inject your newly created classes as template parameters in the `Core` in the `main.cc` file. The project is based on dependency injection. You only need to modify the `main.cc` file.
4. Compile and run.

## Changing which issues categories to parse from the "Technical Debt Dataset"
The class `TechnicalDebtDatasetConnectableDefaultTdMonFactory` has a static member `static const std::string kCategoriesToParse`. This variable is inserted into the the SQL query when parsing the dataset. By default the value for this variable is `(type='Test' OR type='Documentation')` to parse issues of category `Test` or `Documentation`. To parse other categories, please change the value of the static member variable. For example, if you also want to include `Improvement` issues, the new value would be `(type='Test' OR type='Documentation' OR type='Improvement')`. Afterwards, please recompile the application.
