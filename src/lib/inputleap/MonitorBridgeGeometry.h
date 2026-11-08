// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

namespace inputleap {
struct BridgeRect {
    std::int32_t left, top, right, bottom;
    bool operator==(const BridgeRect& r) const {
        return left == r.left && top == r.top && right == r.right && bottom == r.bottom;
    }
    bool contains(std::int32_t x, std::int32_t y) const {
        return x >= left && x < right && y >= top && y < bottom;
    }
};
// Only two horizontally ordered, non-overlapping monitors are supported.
// They need not share a vertical span with each other: overlap with the
// bridged remote client is checked independently per local monitor, using
// the configured Y offset (see mapEnterY/mapReturnY below). Three monitors
// deliberately disable the bridge.
struct MonitorBridgeGeometry {
    BridgeRect left{}, right{};
    bool valid = false;
    void update(std::vector<BridgeRect> monitors) {
        valid = false;
        if (monitors.size() != 2) return;
        std::sort(monitors.begin(), monitors.end(), [](const auto& a, const auto& b) {
            return a.left < b.left;
        });
        left = monitors[0]; right = monitors[1];
        valid = left.right <= right.left && left.right - left.left >= 16 &&
            right.right - right.left >= 16 && left.bottom > left.top &&
            right.bottom > right.top;
    }
    // Direction: +1 leaving the left monitor; -1 leaving the right monitor.
    // Test the entire segment, including jumps over a seam or physical gap.
    int crossing(std::int32_t ox, std::int32_t oy, std::int32_t x, std::int32_t y) const {
        std::int32_t unusedY = 0;
        return crossing(ox, oy, x, y, unusedY);
    }
    // Same as above but also reports the seam-intersection Y (outY), not the
    // segment's endpoint Y. A fast diagonal mouse move can land far below or
    // above the seam on the far monitor by the time the event is observed,
    // so the endpoint Y is the wrong row to enter the remote at; the row the
    // cursor actually crossed the seam on is the interpolated intersection.
    int crossing(std::int32_t ox, std::int32_t oy, std::int32_t x, std::int32_t y,
                 std::int32_t& outY) const {
        if (!valid || x == ox) return 0;
        const int dir = x > ox ? 1 : -1;
        const auto& source = dir > 0 ? left : right;
        if (!source.contains(ox, oy)) return 0;
        const auto edge = dir > 0 ? source.right - 1 : source.left;
        if ((dir > 0 && x < edge) || (dir < 0 && x > edge)) return 0;
        // Widen to int64 before subtracting: ox/x/edge can each sit near the
        // int32 extremes, and plain int32 subtraction here would be
        // signed-overflow UB for far-apart coordinates (e.g. a fast jump
        // from one far corner of a virtual desktop to the other).
        const double dxEdge = double(std::int64_t(edge) - std::int64_t(ox));
        const double dx = double(std::int64_t(x) - std::int64_t(ox));
        const double t = dxEdge / dx;
        if (t < 0.0 || t > 1.0) return 0;
        const double cy = double(oy) + t * double(std::int64_t(y) - std::int64_t(oy));
        if (cy < double(source.top) || cy >= double(source.bottom)) return 0;
        // floor, not truncate: a plain static_cast<int32_t> on a negative cy
        // rounds toward zero (-1.49 -> -1) instead of toward the row the
        // cursor actually occupies (-1.49 -> -2).
        outY = static_cast<std::int32_t>(std::floor(cy));
        return dir;
    }
    // Maps a Y crossing the seam from server-global space into the entered
    // client's own native Y: clientY = serverGlobalY - offset + clientShapeTop.
    // srcTop/srcHeight bound the global span being left; dstTop/dstHeight
    // bound the client being entered. Both ends are validated against their
    // own bounds; a non-overlapping row is rejected (returns false), never
    // squeezed to fit.
    static bool mapEnterY(std::int32_t serverGlobalY, std::int32_t offset,
                          std::int32_t srcTop, std::int32_t srcHeight,
                          std::int32_t dstTop, std::int32_t dstHeight,
                          std::int32_t& outClientY) {
        if (srcHeight <= 0 || dstHeight <= 0) return false;
        if (serverGlobalY < srcTop ||
            std::int64_t(serverGlobalY) >= std::int64_t(srcTop) + srcHeight) {
            return false;
        }
        const std::int64_t c = std::int64_t(serverGlobalY) - std::int64_t(offset) +
            std::int64_t(dstTop);
        if (c < (std::numeric_limits<std::int32_t>::min)() ||
            c > (std::numeric_limits<std::int32_t>::max)()) {
            return false;
        }
        if (c < std::int64_t(dstTop) || c >= std::int64_t(dstTop) + dstHeight) {
            return false;
        }
        outClientY = static_cast<std::int32_t>(c);
        return true;
    }
    // Reverse of mapEnterY: leaving a client back onto the server-global
    // bridge. globalY = clientY - clientShapeTop + offset. srcTop/srcHeight
    // describe the client being left (the row must actually be inside it);
    // dstTop/dstHeight describe the destination span in global space.
    // Either bound failing rejects the transition instead of clamping it
    // into range, so a non-overlapping row can never be forced to resolve
    // somewhere inside the destination.
    static bool mapReturnY(std::int32_t clientY, std::int32_t offset,
                           std::int32_t srcTop, std::int32_t srcHeight,
                           std::int32_t dstTop, std::int32_t dstHeight,
                           std::int32_t& outGlobalY) {
        if (srcHeight <= 0 || dstHeight <= 0) return false;
        if (clientY < srcTop ||
            std::int64_t(clientY) >= std::int64_t(srcTop) + srcHeight) {
            return false;
        }
        const std::int64_t g = std::int64_t(clientY) - std::int64_t(srcTop) +
            std::int64_t(offset);
        if (g < (std::numeric_limits<std::int32_t>::min)() ||
            g > (std::numeric_limits<std::int32_t>::max)()) {
            return false;
        }
        if (g < std::int64_t(dstTop) || g >= std::int64_t(dstTop) + dstHeight) {
            return false;
        }
        outGlobalY = static_cast<std::int32_t>(g);
        return true;
    }
    // While the monitor bridge is active, its target client is reachable only
    // through tagged seam events. An ordinary (untagged) GUI neighbor link to
    // that same client -- e.g. the outer edge of the server sharing a grid
    // side with the bridge client -- must not switch to it. Non-bridge
    // clients and an inactive bridge (including three-monitor layouts, where
    // valid == false) are never suppressed.
    template <typename TargetPtr, typename CandidatePtr>
    static bool ordinaryEdgeLinkAllowed(bool bridgeActive, TargetPtr bridgeTarget,
                                        CandidatePtr candidate) {
        return !bridgeActive || bridgeTarget == nullptr || candidate != bridgeTarget;
    }
    // Retained for callers/tests of the old proportional-scaling behaviour;
    // no longer used by the bridge's enter/return path (see mapEnterY and
    // mapReturnY above), but still a generally useful clamped linear remap.
    static std::int32_t mapY(std::int32_t y, std::int32_t srcTop, std::int32_t srcHeight,
                             std::int32_t dstTop, std::int32_t dstHeight) {
        if (srcHeight <= 1 || dstHeight <= 1) return dstTop;
        const auto offset = (std::max<std::int32_t>)(0, (std::min)(y - srcTop, srcHeight - 1));
        return dstTop + static_cast<std::int32_t>(std::int64_t(offset) * (dstHeight - 1) / (srcHeight - 1));
    }
};
} // namespace inputleap
