#pragma once

#include <Common/config.h>
#include <config_core.h>
#include <Poco/Util/AbstractConfiguration.h>

#if USE_AWS_S3
#include <Common/ConcurrentBoundedQueue.h>
#include <Common/ThreadPool.h>
#include <Common/logger_useful.h>

#include <string>
#endif

namespace DB
{

#if USE_AWS_S3
class KeeperSnapshotManagerS3
{
public:
    KeeperSnapshotManagerS3();

    void updateS3Configuration(const Poco::Util::AbstractConfiguration & config);
    void uploadSnapshot(const std::string & path);

    void startup(const Poco::Util::AbstractConfiguration & config);
    void shutdown();
private:
    using SnapshotS3Queue = ConcurrentBoundedQueue<std::string>;
    SnapshotS3Queue snapshots_s3_queue;

    /// Upload new snapshots to S3
    ThreadFromGlobalPool snapshot_s3_thread;

    struct S3Configuration;
    mutable std::mutex snapshot_s3_client_mutex;
    std::shared_ptr<S3Configuration> snapshot_s3_client;

    std::atomic<bool> shutdown_called{false};

    Poco::Logger * log;

    /// Thread upload snapshots to S3 in the background
    void snapshotS3Thread();
    std::shared_ptr<S3Configuration> getSnapshotS3Client() const;
};
#else
class KeeperSnapshotManagerS3
{
public:
    KeeperSnapshotManagerS3() = default;

    void updateS3Configuration(const Poco::Util::AbstractConfiguration &) {}
    void uploadSnapshot(const std::string &) {}

    void startup(const Poco::Util::AbstractConfiguration &) {}

    void shutdown() {}
};
#endif

}
