/*
 * Copyright (c) 2022, Jakub Berkop <jakub.berkop@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/FileSystem/OpenFileDescription.h>
#include <Kernel/PerformanceManager.h>
#include <Kernel/Process.h>

namespace Kernel {

ErrorOr<NonnullRefPtr<OpenFileDescription>> open_readable_file_description(auto& fds, int fd);

static ErrorOr<size_t> get_path_index(auto& fds, int fd, PerformanceEventBuffer* event_buffer)
{
    auto description = TRY(open_readable_file_description(fds, fd));

    if (auto path = description->original_absolute_path(); !path.is_error()) {
        return TRY(event_buffer->register_string(move(path.value())));
    } else if (auto pseudo_path = description->pseudo_path(); !pseudo_path.is_error()) {
        return TRY(event_buffer->register_string(move(pseudo_path.value())));
    } else {
        auto invalid_path_string = TRY(KString::try_create("<INVALID_FILE_PATH>")); // TODO: Performance, unecessary allocations.
        return TRY(event_buffer->register_string(move(invalid_path_string)));
    }
}

ErrorOr<FlatPtr> Process::sys$open(Userspace<const Syscall::SC_open_params*> user_params)
{
    VERIFY_PROCESS_BIG_LOCK_ACQUIRED(this);
    const auto start_timestamp = TimeManagement::the().uptime_ms();
    const auto params = TRY(copy_typed_from_user(user_params));
    const auto result = open_impl(user_params);

    if (Thread::current()->is_profiling_suppressed())
        return result;

    auto* event_buffer = current_perf_events_buffer();
    if (event_buffer == nullptr)
        return result;

    FilesystemEvent data;
    data.type = FilesystemEventType::Open;
    data.start_timestamp = start_timestamp;

    if (result.is_error()) {
        data.result.is_error = true;
        data.result.value = result.error().code();
    } else {
        data.result.is_error = false;
        data.result.value = 0;
    }

    auto path = get_syscall_path_argument(params.path);
    if (!path.is_error()) {
        auto value = event_buffer->register_string(move(path.value()));
        data.data.open.filename_index = value.value();
    }

    data.data.open.dirfd = params.dirfd;
    data.data.open.options = params.options;
    data.data.open.mode = params.mode;

    (void)current_perf_events_buffer()->append(PERF_EVENT_FILESYSTEM, 0, 0, {}, Thread::current(), data);

    return result;
}

ErrorOr<FlatPtr> Process::sys$close(int fd)
{
    VERIFY_PROCESS_BIG_LOCK_ACQUIRED(this);
    const auto start_timestamp = TimeManagement::the().uptime_ms();
    const auto result = close_impl(fd);
    if (Thread::current()->is_profiling_suppressed())
        return result;

    auto* event_buffer = current_perf_events_buffer();
    if (event_buffer == nullptr)
        return result;

    FilesystemEvent data;
    data.type = FilesystemEventType::Close;
    data.start_timestamp = start_timestamp;
    data.data.close.fd = fd;

    if (result.is_error()) {
        data.result.is_error = true;
        data.result.value = result.error().code();
    } else {
        data.result.is_error = false;
        data.result.value = 0;
    }

    auto maybe_path_index = get_path_index(fds(), fd, event_buffer);
    if (maybe_path_index.is_error())
        return result;

    data.data.close.filename_index = maybe_path_index.value();

    (void)current_perf_events_buffer()->append(PERF_EVENT_FILESYSTEM, 0, 0, {}, Thread::current(), data);

    return result;
}

ErrorOr<FlatPtr> Process::sys$readv(int fd, Userspace<const struct iovec*> iov, int iov_count)
{
    VERIFY_PROCESS_BIG_LOCK_ACQUIRED(this);
    const auto start_timestamp = TimeManagement::the().uptime_ms();
    const auto result = readv_impl(fd, iov, iov_count);

    if (Thread::current()->is_profiling_suppressed())
        return result;

    auto* event_buffer = current_perf_events_buffer();
    if (event_buffer == nullptr)
        return result;

    FilesystemEvent data;
    data.type = FilesystemEventType::Readv;
    data.start_timestamp = start_timestamp;
    data.data.readv.fd = fd;

    if (result.is_error()) {
        data.result.is_error = true;
        data.result.value = result.error().code();
    } else {
        data.result.is_error = false;
        data.result.value = 0;
    }

    auto maybe_path_index = get_path_index(fds(), fd, event_buffer);
    if (maybe_path_index.is_error())
        return result;

    data.data.readv.filename_index = maybe_path_index.value();

    (void)current_perf_events_buffer()->append(PERF_EVENT_FILESYSTEM, 0, 0, {}, Thread::current(), data);

    return result;
}

ErrorOr<FlatPtr> Process::sys$read(int fd, Userspace<u8*> buffer, size_t size)
{
    VERIFY_PROCESS_BIG_LOCK_ACQUIRED(this);
    const auto start_timestamp = TimeManagement::the().uptime_ms();
    const auto result = read_impl(fd, buffer, size);

    if (Thread::current()->is_profiling_suppressed())
        return result;

    auto* event_buffer = current_perf_events_buffer();
    if (event_buffer == nullptr)
        return result;

    FilesystemEvent data;
    data.type = FilesystemEventType::Read;
    data.start_timestamp = start_timestamp;
    data.data.read.fd = fd;

    if (result.is_error()) {
        data.result.is_error = true;
        data.result.value = result.error().code();
    } else {
        data.result.is_error = false;
        data.result.value = 0;
    }

    auto maybe_path_index = get_path_index(fds(), fd, event_buffer);
    if (maybe_path_index.is_error())
        return result;

    data.data.read.filename_index = maybe_path_index.value();

    (void)current_perf_events_buffer()->append(PERF_EVENT_FILESYSTEM, 0, 0, {}, Thread::current(), data);

    return result;
}

ErrorOr<FlatPtr> Process::sys$pread(int fd, Userspace<u8*> buffer, size_t size, Userspace<off_t const*> userspace_offset)
{
    VERIFY_PROCESS_BIG_LOCK_ACQUIRED(this);
    const auto start_timestamp = TimeManagement::the().uptime_ms();
    const auto result = pread_impl(fd, buffer, size, userspace_offset);

    if (Thread::current()->is_profiling_suppressed())
        return result;

    auto* event_buffer = current_perf_events_buffer();
    if (event_buffer == nullptr)
        return result;

    FilesystemEvent data;
    data.type = FilesystemEventType::Pread;
    data.start_timestamp = start_timestamp;
    data.data.pread.fd = fd;
    data.data.pread.buffer_ptr = buffer.ptr();
    data.data.pread.size = size;
    data.data.pread.offset = MUST(copy_typed_from_user(userspace_offset));

    if (result.is_error()) {
        data.result.is_error = true;
        data.result.value = result.error().code();
    } else {
        data.result.is_error = false;
        data.result.value = 0;
    }

    auto maybe_path_index = get_path_index(fds(), fd, event_buffer);
    if (maybe_path_index.is_error())
        return result;

    data.data.pread.filename_index = maybe_path_index.value();

    (void)current_perf_events_buffer()->append(PERF_EVENT_FILESYSTEM, 0, 0, {}, Thread::current(), data);

    return result;
}

}