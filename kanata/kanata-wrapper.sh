#!/bin/bash
# Dummy wrapper for kanata-tray when kanata runs as a system LaunchDaemon.
# This keeps the tray process happy while it monitors the daemon on port 5829.
exec sleep 99999999
