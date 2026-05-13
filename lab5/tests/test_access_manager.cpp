#include <gtest/gtest.h>
#include "../include/access_manager.h"

class AccessManagerTest : public ::testing::Test {
protected:
    AccessManager* access_manager;

    void SetUp() override {
        access_manager = new AccessManager();
    }

    void TearDown() override {
        delete access_manager;
    }
};

TEST_F(AccessManagerTest, AcquireReadLock) {
    EXPECT_TRUE(access_manager->can_acquire_read_lock(1, 100));
    EXPECT_TRUE(access_manager->acquire_read_lock(1, 100));
    EXPECT_TRUE(access_manager->has_lock(1, 100));
    EXPECT_EQ(access_manager->get_lock_type(1, 100), Access::LOCK_READ);
}

TEST_F(AccessManagerTest, AcquireWriteLock) {
    EXPECT_TRUE(access_manager->can_acquire_write_lock(1, 100));
    EXPECT_TRUE(access_manager->acquire_write_lock(1, 100));
    EXPECT_TRUE(access_manager->has_lock(1, 100));
    EXPECT_EQ(access_manager->get_lock_type(1, 100), Access::LOCK_WRITE);
}

TEST_F(AccessManagerTest, MultipleReadLocks) {
    EXPECT_TRUE(access_manager->acquire_read_lock(1, 100));
    EXPECT_TRUE(access_manager->can_acquire_read_lock(1, 101));
    EXPECT_TRUE(access_manager->acquire_read_lock(1, 101));

    EXPECT_TRUE(access_manager->has_lock(1, 100));
    EXPECT_TRUE(access_manager->has_lock(1, 101));
    EXPECT_EQ(access_manager->get_lock_type(1, 100), Access::LOCK_READ);
    EXPECT_EQ(access_manager->get_lock_type(1, 101), Access::LOCK_READ);
}

TEST_F(AccessManagerTest, ReadWriteConflict) {
    EXPECT_TRUE(access_manager->acquire_read_lock(1, 100));
    EXPECT_FALSE(access_manager->can_acquire_write_lock(1, 101));
    EXPECT_FALSE(access_manager->acquire_write_lock(1, 101));
}

TEST_F(AccessManagerTest, WriteReadConflict) {
    EXPECT_TRUE(access_manager->acquire_write_lock(1, 100));
    EXPECT_FALSE(access_manager->can_acquire_read_lock(1, 101));
    EXPECT_FALSE(access_manager->acquire_read_lock(1, 101));
}

TEST_F(AccessManagerTest, ReleaseLock) {
    EXPECT_TRUE(access_manager->acquire_read_lock(1, 100));
    EXPECT_TRUE(access_manager->has_lock(1, 100));

    access_manager->release_lock(1, 100);
    EXPECT_FALSE(access_manager->has_lock(1, 100));
}

TEST_F(AccessManagerTest, ReleasePartialReadLock) {
    EXPECT_TRUE(access_manager->acquire_read_lock(1, 100));
    EXPECT_TRUE(access_manager->acquire_read_lock(1, 101));

    access_manager->release_lock(1, 100);
    EXPECT_FALSE(access_manager->has_lock(1, 100));
    EXPECT_TRUE(access_manager->has_lock(1, 101));
    EXPECT_EQ(access_manager->get_lock_type(1, 101), Access::LOCK_READ);
}

TEST_F(AccessManagerTest, ReleaseAllLocks) {
    EXPECT_TRUE(access_manager->acquire_read_lock(1, 100));
    EXPECT_TRUE(access_manager->acquire_write_lock(2, 100));
    EXPECT_TRUE(access_manager->acquire_read_lock(3, 101));

    access_manager->release_all_locks(100);

    EXPECT_FALSE(access_manager->has_lock(1, 100));
    EXPECT_FALSE(access_manager->has_lock(2, 100));
    EXPECT_TRUE(access_manager->has_lock(3, 101));
}

TEST_F(AccessManagerTest, SameClientWriteAccess) {
    EXPECT_TRUE(access_manager->acquire_write_lock(1, 100));
    EXPECT_TRUE(access_manager->can_acquire_write_lock(1, 100));
    EXPECT_TRUE(access_manager->acquire_write_lock(1, 100));
}