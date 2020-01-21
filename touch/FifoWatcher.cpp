/*
 * Copyright (C) 2020 The MoKee Open Source Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define LOG_TAG "FifoWatcher"

#include "FifoWatcher.h"

#include <android-base/logging.h>
#include <stdio.h>
#include <stdint.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>

#define EPOLLEVENTS 20

namespace vendor {
namespace mokee {
namespace touch {
namespace V1_0 {
namespace implementation {

static void *work(void *arg);

FifoWatcher::FifoWatcher(const std::string& file, const WatcherCallback& callback)
    : mFile(file)
    , mCallback(callback)
    , mExit(false)
    {
    if (pthread_create(&mPoll, NULL, work, this)) {
        LOG(ERROR) << "pthread creation failed: " << errno;
    }
}

void FifoWatcher::exit() {
    mExit = true;
    LOG(INFO) << "Exit";
}

static void *work(void *arg) {
    int epoll_fd, input_fd;
    struct epoll_event ev;
    int nevents = 0;
    int fd, len, value;
    char buf[10];

    LOG(INFO) << "Creating thread";

    FifoWatcher *thiz = (FifoWatcher *)arg;
    const char *file = thiz->mFile.c_str();

    unlink(file);

    if (mkfifo(file, 0660) < 0) {
        LOG(ERROR) << "Failed creating " << thiz->mFile << ": " << errno;
        return NULL;
    }

    input_fd = open(file, O_RDONLY);
    if (input_fd < 0) {
        LOG(ERROR) << "Failed opening " << thiz->mFile << ": " << errno;
        return NULL;
    }

    ev.events = EPOLLIN;
    ev.data.fd = input_fd;

    epoll_fd = epoll_create(EPOLLEVENTS);
    if (epoll_fd == -1) {
        LOG(ERROR) << "Failed epoll_create: " << errno;
        goto error;
    }

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, input_fd, &ev) == -1) {
        LOG(ERROR) << "Failed epoll_ctl: " << errno;
        goto error;
    }

    while (!thiz->mExit) {
        struct epoll_event events[EPOLLEVENTS];

        nevents = epoll_wait(epoll_fd, events, EPOLLEVENTS, -1);
        if (nevents == -1) {
            if (errno == EINTR) {
                continue;
            }
            LOG(ERROR) << "Failed epoll_wait: " << errno;
            break;
        }

        for (int i = 0; i < nevents; i++) {
            fd = events[i].data.fd;
            len = read(fd, buf, sizeof(buf));
            if (len < 0) {
                LOG(ERROR) << "Failed reading " << thiz->mFile << ": " << errno;
                goto error;
            } else if (len == 0) {
                usleep(10 * 1000);
                continue;
            }

            value = atoi(buf);

            thiz->mCallback(thiz->mFile, value);
        }
    }

    LOG(INFO) << "Exiting worker thread";

error:
    close(input_fd);

    if (epoll_fd >= 0)
        close(epoll_fd);

    return NULL;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace touch
}  // namespace mokee
}  // namespace vendor
