#!/bin/bash

# Function to run when changes are found
run_my_task() {
    local folder=$1
    echo "🚀 Changes detected in '$folder'! Running task now..."
    cookiecutter $folder -o examples/ --no-input --overwrite-if-exists
}

# Core function to check a folder for changes
check_folder_changes() {
    local target_folder=$1

    # Check if git detects any staged, unstaged, or untracked changes
    if [ -n "$(git status --porcelain "$target_folder")" ]; then
        run_my_task "$target_folder"
    else
        echo "✅ No changes detected in '$target_folder'. Skipping."
    fi
}

# ========================================================
# CONFIGURATION: List all the folders you want to check
# ========================================================
FOLDERS_TO_CHECK=(
    "templates/Node"
   # "../another-folder/"
 #   "src/components/"
)

# Loop through each folder in the array
for folder in "${FOLDERS_TO_CHECK[@]}"; do
    check_folder_changes "$folder"
done
