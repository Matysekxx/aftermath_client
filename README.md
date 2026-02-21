<div align="center">

# ☢️ THE AFTERMATH CLIENT ☢️
### 🖥️ The Official TUI Client for Post-Apocalyptic Prague

![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![FTXUI](https://img.shields.io/badge/FTXUI-TUI_Library-green?style=for-the-badge)
![CMake](https://img.shields.io/badge/CMake-Build_System-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)

<br/>
The Aftermath Client is a lightweight, high-performance terminal-based interface for the Aftermath RPG. Built in C++, it connects to the authoritative server via WebSocket to render the grim reality of the Prague metro directly in your console.

[Report Bug](https://github.com/Matysekxx/aftermath-client/issues) · [Request Feature](https://github.com/Matysekxx/aftermath-client/issues)

</div>

---
## 🌟 Highlights

*   ⚡ **Native Performance:** Written in modern **C++17** for minimal resource usage and instant response times.
*   🖥️ **Rich TUI Experience:** Powered by **FTXUI**, offering a responsive layout, colors, and dynamic widgets in a text environment.
*   🌐 **Real-time Networking:** Uses **IXWebSocket** for low-latency communication with the game server.
*   🗺️ **Dynamic Rendering:** Supports UTF-8 map rendering, dynamic viewports, and visual indicators for entities and loot.
*   🎒 **Inventory Management:** Full GUI-like experience for managing loot, equipping gear, and trading with NPCs.
*   🛠️ **Cross-Platform Ready:** Built with **CMake**, designed primarily for Windows but adaptable for Linux/macOS.

---

## ℹ️ Overview

### 🛠️ Technological Pillars

*   **C++17:** The core language, ensuring speed and memory efficiency.
*   **FTXUI:** A modern C++ library for terminal user interfaces, allowing for DOM-like layout construction.
*   **IXWebSocket:** Robust WebSocket client library handling connection stability and message framing.
*   **nlohmann/json:** Industry-standard JSON parsing for processing complex game state updates.

### 🎮 Controls

Master the controls to survive in the metro:

| Key | Action | Description |
| :--- | :--- | :--- |
| **ARROWS** | **Move** | Navigate the character through the metro tunnels. |
| **SPACE** | **Attack** | Attack the nearest hostile entity. |
| **E** | **Interact** | Loot containers, talk to NPCs, or enter metro stations. |
| **I** | **Inventory** | Toggle the inventory/cargo hold panel. |
| **U** | **Use Item** | Use the selected consumable from your inventory. |
| **D** | **Drop Item** | Drop the selected item on the ground. |
| **L** | **Logs** | Toggle the system and network log panel. |
| **ESC** | **Menu/Back** | Open system menu or close current UI overlay. |

---

## 🚀 Quick Start Guide

### 📥 1. Installation

1.  **Download:** Get the latest `aftermath_client.exe` from the Releases page.
2.  **Prerequisites:** You only need a Windows PC. No additional runtime is required as the client is statically linked.

### 🎮 2. Connecting to the Server

1.  **Find a Server:** You need the IP address of a running Aftermath Server.
2.  **Launch:** Open your terminal (PowerShell or CMD) and run:
    
