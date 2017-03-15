#!/bin/sh
#
# Bash functions to handle the settings for the Keetchi simulation
#
# source this file in your bash script if you want to use its functionality
#
# @author Jens Dede (jd@comnets.uni-bremen.de)
# @date: 01-November-2016
#

# File definitions

# User settings
settingsFilename="$HOME/.opsSettings"

# Default settings
defaultSettingsFilename="settings.default"

# Loads the file from the above defined files.
loadSettings() {
    if [ -f "$settingsFilename" ]; then
        echo "Loading settings from \"$settingsFilename\""
        . $settingsFilename
    else
        echo "No settings found. Using settings from \"$defaultSettingsFilename\""
        . $defaultSettingsFilename
    fi
}

# Print out path to user file containing the settings
userSettings() {
    echo $settingsFilename
}

# Print out path to default settings
defaultSettings() {
    echo $defaultSettingsFilename
}
