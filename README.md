# Gallery Project - README

Welcome to the Gallery Project! This project is part of the Magshimim program, designed to manage and enhance photo albums efficiently. Below you will find detailed instructions on setting up and running the project, along with the necessary steps to download and integrate the required SQLite files.
The Gallery Project aims to provide users with a robust platform to manage photo albums. This version includes infrastructure improvements, bug fixes, and new features.

## Table of Contents

1. [Project Overview](#project-overview)
2. [Setup Instructions](#setup-instructions)
3. [Downloading SQLite Files](#downloading-sqlite-files)
4. [Compiling and Running the Project](#compiling-and-running-the-project-in-visual-studio)

## Project Overview

The project revolve around managing the user data inside an sqlite database and displaying it to the user.

## Setup Instructions

1. **Clone the Repository**
   ```sh
   git clone https://github.com/liorg2007/Gallery.git
   ```

2. **Navigate to the Project Directory**
   ```sh
   cd Gallery/Gallery/
   ```

## Downloading SQLite Files

The project relies on SQLite for database management. You need to download the necessary SQLite header and source files (`sqlite.h` and `sqlite.c`) from the official SQLite website.

1. **Download `sqlite.h` and `sqlite.c`**
   - Visit [SQLite Download Page](https://www.sqlite.org/download.html).
   - Download the zip under `Source Code`
   - Extract the files `sqlite.h` and `sqlite.c` into the project directory inside `Lib/` directory

2. **Add SQLite Files to the Project**
   - Place `sqlite.h` and `sqlite.c` in the project's root directory or a designated `Lib/` directory.

## Compiling and Running the Project in Visual Studio

Ensure you have Visual Studio installed on your machine.

1. **Open the Project in Visual Studio**
   - Open `Gallery.sln` file

2. **Build and Run the Project**
   - Press `F5` or select `Debug` > `Start Debugging` to build and run the project.
