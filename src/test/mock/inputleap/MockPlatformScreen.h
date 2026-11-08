// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "inputleap/IPlatformScreen.h"
#include <gmock/gmock.h>
namespace inputleap { class MockPlatformScreen : public IPlatformScreen { public:
MOCK_METHOD(void, enable, (), (override));
MOCK_METHOD(void, disable, (), (override));
MOCK_METHOD(void, enter, (), (override));
MOCK_METHOD(bool, canLeave, (), (override));
MOCK_METHOD(void, leave, (), (override));
MOCK_METHOD(bool, setClipboard, (ClipboardID id, const IClipboard* clipboard), (override));
MOCK_METHOD(void, checkClipboards, (), (override));
MOCK_METHOD(void, openScreensaver, (bool notify), (override));
MOCK_METHOD(void, closeScreensaver, (), (override));
MOCK_METHOD(void, screensaver, (bool activate), (override));
MOCK_METHOD(void, resetOptions, (), (override));
MOCK_METHOD(void, setOptions, (const OptionsList& options), (override));
MOCK_METHOD(void, setSequenceNumber, (std::uint32_t), (override));
MOCK_METHOD(void, setDraggingStarted, (bool started), (override));
MOCK_METHOD(bool, isPrimary, (), (const, override));
MOCK_METHOD(std::string&, getDraggingFilename, (), (override));
MOCK_METHOD(void, clearDraggingFilename, (), (override));
MOCK_METHOD(bool, isDraggingStarted, (), (override));
MOCK_METHOD(bool, isFakeDraggingStarted, (), (override));
MOCK_METHOD(void, fakeDraggingFiles, (DragFileList file_list), (override));
MOCK_METHOD(void, setDropTarget, (const std::string& target), (override));
MOCK_METHOD(bool, getClipboard, (ClipboardID id, IClipboard*), (const, override));
MOCK_METHOD(void, getShape, (std::int32_t& x, std::int32_t& y, std::int32_t& width,
                  std::int32_t& height), (const, override));
MOCK_METHOD(void, getCursorPos, (std::int32_t& x, std::int32_t& y), (const, override));
MOCK_METHOD(void, reconfigure, (std::uint32_t active_sides), (override));
MOCK_METHOD(void, warpCursor, (std::int32_t x, std::int32_t y), (override));
MOCK_METHOD(std::uint32_t, registerHotKey, (KeyID key, KeyModifierMask mask), (override));
MOCK_METHOD(void, unregisterHotKey, (std::uint32_t id), (override));
MOCK_METHOD(void, fakeInputBegin, (), (override));
MOCK_METHOD(void, fakeInputEnd, (), (override));
MOCK_METHOD(std::int32_t, getJumpZoneSize, (), (const, override));
MOCK_METHOD(bool, isAnyMouseButtonDown, (std::uint32_t& buttonID), (const, override));
MOCK_METHOD(void, getCursorCenter, (std::int32_t& x, std::int32_t& y), (const, override));
MOCK_METHOD(void, fakeMouseButton, (ButtonID id, bool press), (override));
MOCK_METHOD(void, fakeMouseMove, (std::int32_t x, std::int32_t y), (override));
MOCK_METHOD(void, fakeMouseRelativeMove, (std::int32_t dx, std::int32_t dy), (const, override));
MOCK_METHOD(void, fakeMouseWheel, (std::int32_t x_delta, std::int32_t y_delta), (const, override));
MOCK_METHOD(void, updateKeyMap, (), (override));
MOCK_METHOD(void, updateKeyState, (), (override));
MOCK_METHOD(void, setHalfDuplexMask, (KeyModifierMask), (override));
MOCK_METHOD(void, fakeKeyDown, (KeyID id, KeyModifierMask mask, KeyButton button), (override));
MOCK_METHOD(bool, fakeKeyRepeat, (KeyID id, KeyModifierMask mask, std::int32_t count,
                       KeyButton button), (override));
MOCK_METHOD(bool, fakeKeyUp, (KeyButton button), (override));
MOCK_METHOD(void, fakeAllKeysUp, (), (override));
MOCK_METHOD(bool, fakeCtrlAltDel, (), (override));
MOCK_METHOD(bool, fakeMediaKey, (KeyID id), (override));
MOCK_METHOD(bool, isKeyDown, (KeyButton), (const, override));
MOCK_METHOD(KeyModifierMask, getActiveModifiers, (), (const, override));
MOCK_METHOD(KeyModifierMask, pollActiveModifiers, (), (const, override));
MOCK_METHOD(std::int32_t, pollActiveGroup, (), (const, override));
MOCK_METHOD(void, pollPressedKeys, (KeyButtonSet& pressed_keys), (const, override));
MOCK_METHOD(void, handle_system_event, (const Event& event), (override));
MOCK_METHOD(const EventTarget*, get_event_target, (), (const, override));
MOCK_METHOD(const std::string&, getDropTarget, (), (const, override));
}; }
