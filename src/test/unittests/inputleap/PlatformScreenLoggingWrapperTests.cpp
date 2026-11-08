// SPDX-License-Identifier: GPL-2.0-or-later
#include "inputleap/PlatformScreenLoggingWrapper.h"
#include "test/mock/inputleap/MockPlatformScreen.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
using namespace inputleap;
TEST(PlatformScreenLoggingWrapperTests, leaveDelegatesExactlyOnce) {
 auto s = std::make_unique<testing::StrictMock<MockPlatformScreen>>();
 EXPECT_CALL(*s, leave()).Times(1);
 PlatformScreenLoggingWrapper w(std::move(s)); w.leave();
}
TEST(PlatformScreenLoggingWrapperTests, enterDelegatesExactlyOnce) {
 auto s = std::make_unique<testing::StrictMock<MockPlatformScreen>>();
 EXPECT_CALL(*s, enter()).Times(1);
 PlatformScreenLoggingWrapper w(std::move(s)); w.enter();
}
TEST(PlatformScreenLoggingWrapperTests, canLeavePropagatesBothResults) {
 auto s = std::make_unique<testing::StrictMock<MockPlatformScreen>>();
 EXPECT_CALL(*s, canLeave()).WillOnce(testing::Return(true)).WillOnce(testing::Return(false));
 PlatformScreenLoggingWrapper w(std::move(s));
 EXPECT_TRUE(w.canLeave()); EXPECT_FALSE(w.canLeave());
}
