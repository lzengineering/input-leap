// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "inputleap/MonitorBridgeGeometry.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace inputleap {
// The low-level hook and server event dispatch run on the existing input
// thread, just like MSWindowsHook's mode/zone state. No wire state is added.
struct WindowsMonitorBridge {
    MonitorBridgeGeometry geometry;
    std::vector<BridgeRect> monitors;
    bool enabled = false;
    std::int32_t seamX = 0;
    std::uint32_t generation = 0;
    static WindowsMonitorBridge& instance() {
        static WindowsMonitorBridge state;
        return state;
    }
    static BOOL CALLBACK collect(HMONITOR monitor, HDC, LPRECT, LPARAM data) {
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        if (!GetMonitorInfoW(monitor, &info)) return FALSE;
        auto& list = *reinterpret_cast<std::vector<BridgeRect>*>(data);
        const auto& r = info.rcMonitor;
        list.push_back({r.left, r.top, r.right, r.bottom});
        return TRUE;
    }
    bool refresh() {
        std::vector<BridgeRect> current;
        if (!EnumDisplayMonitors(nullptr, nullptr, &collect, reinterpret_cast<LPARAM>(&current)))
            current.clear();
        std::sort(current.begin(), current.end(), [](const auto& a, const auto& b) {
            if (a.left != b.left) return a.left < b.left;
            if (a.top != b.top) return a.top < b.top;
            if (a.right != b.right) return a.right < b.right;
            return a.bottom < b.bottom;
        });
        const bool changed = current != monitors;
        if (changed) ++generation;
        monitors = current;
        geometry.update(current);
        return changed;
    }
};
} // namespace inputleap
