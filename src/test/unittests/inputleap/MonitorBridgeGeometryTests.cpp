// SPDX-License-Identifier: GPL-2.0-or-later
#include "inputleap/MonitorBridgeGeometry.h"
#include <gtest/gtest.h>

using namespace inputleap;

TEST(MonitorBridgeGeometryTests, consumesBothInnerDirectionsIncludingFastJumps)
{
    MonitorBridgeGeometry g;
    g.update({{0, 0, 1920, 1080}, {1920, 0, 3840, 1200}});
    ASSERT_TRUE(g.valid);
    EXPECT_EQ(1, g.crossing(1800, 500, 2300, 500));
    EXPECT_EQ(-1, g.crossing(2200, 500, 1700, 500));
    EXPECT_EQ(0, g.crossing(100, 500, 200, 500));
    EXPECT_EQ(0, g.crossing(1800, 500, 1700, 500));
}

TEST(MonitorBridgeGeometryTests, threeMonitorsAndVerticalOrOverlappingLayoutsDisableBridge)
{
    MonitorBridgeGeometry g;
    g.update({{0, 0, 100, 100}, {100, 0, 200, 100}, {200, 0, 300, 100}});
    EXPECT_FALSE(g.valid);
    g.update({{0, 0, 100, 100}, {0, 100, 100, 200}});
    EXPECT_FALSE(g.valid);
    g.update({{0, 0, 100, 100}, {50, 0, 150, 100}});
    EXPECT_FALSE(g.valid);
}

TEST(MonitorBridgeGeometryTests, supportsNegativeCoordinatesGapsAndReorderedEnumeration)
{
    MonitorBridgeGeometry g;
    g.update({{100, -100, 2020, 1100}, {-1920, 0, 0, 1080}});
    ASSERT_TRUE(g.valid);
    EXPECT_EQ(1, g.crossing(-100, 500, 500, 500));
    EXPECT_EQ(-1, g.crossing(200, 500, -500, 500));
    EXPECT_EQ(0, g.crossing(200, -50, -500, -1000));
}

TEST(MonitorBridgeGeometryTests, mapYRemainsProportionalAndClampsOvershootForCompatCallers)
{
    // mapY is the retained legacy proportional helper (not used by the
    // enter/return path below, which rejects instead of clamping).
    EXPECT_EQ(0, MonitorBridgeGeometry::mapY(-10, 0, 1080, 0, 1200));
    EXPECT_EQ(1199, MonitorBridgeGeometry::mapY(2000, 0, 1080, 0, 1200));
    EXPECT_EQ(500, MonitorBridgeGeometry::mapY(500, 0, 1001, -100, 1201));
    EXPECT_EQ(-100, MonitorBridgeGeometry::mapY(500, 0, 1, -100, 1201));
}

TEST(MonitorBridgeGeometryTests, twoThreeTwoWithPortraitMonitor) {
 MonitorBridgeGeometry g;
 g.update({{0,0,1080,1920},{1080,420,3000,1500}});
 ASSERT_TRUE(g.valid);
 EXPECT_EQ(1,g.crossing(1000,800,1200,800));
 g.update({{0,0,1080,1920},{1080,0,3000,1080},{3000,0,4920,1080}});
 EXPECT_FALSE(g.valid);
 EXPECT_EQ(0,g.crossing(1000,800,1200,800));
 g.update({{0,0,1080,1920},{1080,420,3000,1500}});
 ASSERT_TRUE(g.valid);
 EXPECT_EQ(-1,g.crossing(1200,800,1000,800));
 // Remote middle maps the full source height, not only the overlap.
 EXPECT_EQ(1,g.crossing(1000,100,1200,100));
 EXPECT_EQ(420,MonitorBridgeGeometry::mapY(0,0,1920,420,1080));
 EXPECT_EQ(1499,MonitorBridgeGeometry::mapY(1919,0,1920,420,1080));
}

TEST(MonitorBridgeGeometryTests, mapEnterYAppliesPositiveOffset)
{
    std::int32_t outClientY = 0;
    // serverGlobalY=500, offset=+50, src=[0,1080), dst=[0,900):
    // clientY = 500 - 50 + 0 = 450.
    EXPECT_TRUE(MonitorBridgeGeometry::mapEnterY(500, 50, 0, 1080, 0, 900, outClientY));
    EXPECT_EQ(450, outClientY);
}

TEST(MonitorBridgeGeometryTests, mapEnterYAppliesNegativeOffset)
{
    std::int32_t outClientY = 0;
    // clientY = 500 - (-50) + 0 = 550.
    EXPECT_TRUE(MonitorBridgeGeometry::mapEnterY(500, -50, 0, 1080, 0, 900, outClientY));
    EXPECT_EQ(550, outClientY);
}

TEST(MonitorBridgeGeometryTests, mapEnterYHonoursNonzeroClientOrigin)
{
    std::int32_t outClientY = 0;
    // Portrait "remote" shape top at 420, like twoThreeTwoWithPortraitMonitor.
    // clientY = 100 - 0 + 420 = 520.
    EXPECT_TRUE(MonitorBridgeGeometry::mapEnterY(100, 0, 0, 1920, 420, 1080, outClientY));
    EXPECT_EQ(520, outClientY);
}

TEST(MonitorBridgeGeometryTests, mapEnterYRejectsExactlyAtEitherBoundary)
{
    std::int32_t outClientY = 0;
    // Destination span [0, 900): row 899 accepted, row 900 rejected.
    EXPECT_TRUE(MonitorBridgeGeometry::mapEnterY(899, 0, 0, 2000, 0, 900, outClientY));
    EXPECT_EQ(899, outClientY);
    EXPECT_FALSE(MonitorBridgeGeometry::mapEnterY(900, 0, 0, 2000, 0, 900, outClientY));
    // Source span [0, 2000): row 1999 accepted, row 2000 rejected outright.
    EXPECT_TRUE(MonitorBridgeGeometry::mapEnterY(1999, 1100, 0, 2000, 0, 900, outClientY));
    EXPECT_EQ(899, outClientY);
    EXPECT_FALSE(MonitorBridgeGeometry::mapEnterY(2000, 1100, 0, 2000, 0, 900, outClientY));
}

TEST(MonitorBridgeGeometryTests, mapReturnYIsExactInverseOfMapEnterY)
{
    std::int32_t client = 0, backToGlobal = 0;
    ASSERT_TRUE(MonitorBridgeGeometry::mapEnterY(700, 25, 0, 1200, 420, 1080, client));
    EXPECT_TRUE(MonitorBridgeGeometry::mapReturnY(client, 25, 420, 1080, 0, 1200, backToGlobal));
    EXPECT_EQ(700, backToGlobal);
}

TEST(MonitorBridgeGeometryTests, mapReturnYRejectsOutsideSourceOrDestinationSpanInsteadOfClamping)
{
    std::int32_t outGlobalY = 0;
    // Source (client being left) span is [420, 1500); clientY=1500 is outside
    // it and must be rejected even though the arithmetic would be in-range.
    EXPECT_FALSE(MonitorBridgeGeometry::mapReturnY(1500, 0, 420, 1080, 0, 2000, outGlobalY));
    EXPECT_TRUE(MonitorBridgeGeometry::mapReturnY(1499, 0, 420, 1080, 0, 2000, outGlobalY));
    EXPECT_EQ(1079, outGlobalY);
    // In-bounds source, but the resulting global row falls outside the
    // destination span: must reject, not clamp into the destination.
    EXPECT_FALSE(MonitorBridgeGeometry::mapReturnY(420, 0, 420, 1080, 500, 1000, outGlobalY));
}

TEST(MonitorBridgeGeometryTests, mapEnterYTreatsLeftAndRightOverlapIndependently)
{
    // A single server-global source span [0, 1000) bridged against two
    // differently offset/positioned client shapes: whether a given global
    // row overlaps one client must not depend on whether it overlaps the
    // other.
    std::int32_t outClientY = 0;
    // "left" client: offset=0, shape=[0,500) -> only global rows [0,500) land.
    EXPECT_TRUE(MonitorBridgeGeometry::mapEnterY(300, 0, 0, 1000, 0, 500, outClientY));
    EXPECT_EQ(300, outClientY);
    EXPECT_FALSE(MonitorBridgeGeometry::mapEnterY(700, 0, 0, 1000, 0, 500, outClientY));
    // "right" client: offset=+500, shape=[0,500) -> only global rows [500,1000) land.
    EXPECT_FALSE(MonitorBridgeGeometry::mapEnterY(300, 500, 0, 1000, 0, 500, outClientY));
    EXPECT_TRUE(MonitorBridgeGeometry::mapEnterY(700, 500, 0, 1000, 0, 500, outClientY));
    EXPECT_EQ(200, outClientY);
}

TEST(MonitorBridgeGeometryTests, crossingFastDiagonalSeamIntersectionFloorsNegativeRows)
{
    MonitorBridgeGeometry g;
    g.update({{-100, -10, 0, 50}, {0, -10, 100, 50}});
    ASSERT_TRUE(g.valid);
    std::int32_t outY = 0;
    // A fast diagonal move: endpoint Y (-2) is not the seam-crossing Y. The
    // interpolated crossing lands at cy=-1.49, which must floor to -2, not
    // truncate-toward-zero to -1.
    EXPECT_EQ(1, g.crossing(-50, -1, 50, -2, outY));
    EXPECT_EQ(-2, outY);
}

TEST(MonitorBridgeGeometryTests, crossingWithOutYSurvivesNearInt32ExtremeCoordinatesWithoutOverflow)
{
    MonitorBridgeGeometry g;
    // left/right deliberately span across the int32 midline so that a
    // naive int32 (x - ox) subtraction below would overflow (UB).
    g.update({{-1073741824, 0, -1073741724, 1080}, {-1073741724, 0, 1073741824, 1080}});
    ASSERT_TRUE(g.valid);
    std::int32_t outY = 0;
    EXPECT_EQ(1, g.crossing(-1073741824, 500, 1073741824, 500, outY));
    EXPECT_EQ(500, outY);
}
