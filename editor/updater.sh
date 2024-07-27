#!/bin/bash

# This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
# Copyright (C) 2024  Ryan Zmuda
# Licensed under the MIT license. See LICENSE file in the project root for details.

# let yoyoeditor quickly close
sleep 2

# Create and run the updater script
cat << 'EOF' > /tmp/yoyoeditor-updater.sh
#!/bin/bash
# Wait for the main script to terminate
sleep 1

# Run the uninstall script
/opt/zoogies/yoyoengine/uninstall.sh

# Reinstall from the GitHub repository
installer_download_url=$(curl -s https://api.github.com/repos/zoogies/Boneworks-Save-Manager/releases/latest | grep browser_download_url | head -n 1 | awk '{ print $2 }' | sed 's/,$//' | sed 's/"//g' );
echo "Running Newest Installer: $asset_download_url"
curl -s installer_download_url | bash

# Clean up the temporary script
rm -- "$0"
EOF

chmod +x /tmp/updater.sh
/tmp/updater.sh &

# Terminate the main script
exit 0