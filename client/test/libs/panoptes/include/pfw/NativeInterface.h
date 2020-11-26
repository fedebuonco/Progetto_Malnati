#ifndef PFW_NATIVE_INTERFACE_H
#define PFW_NATIVE_INTERFACE_H

#if defined(_WIN32)
#include "pfw/win32/Controller.h"
using NativeImplementation = pfw::Controller;
#elif defined(__APPLE__)
#include "pfw/osx/FSEventsService.h"
using NativeImplementation = pfw::FSEventsService;
#elif defined(__linux__)
#include "pfw/linux/InotifyService.h"
using NativeImplementation = pfw::InotifyService;
#endif

#include "pfw/Filter.h"
#include <vector>

namespace pfw {
namespace fs = std::filesystem;

class NativeInterface
{
  public:
    NativeInterface(const fs::path &                path,
                    const std::chrono::milliseconds latency,
                    CallBackSignatur                callback);
    ~NativeInterface();

    bool isWatching();

  private:
    std::shared_ptr<Filter>               _filter;
    std::unique_ptr<NativeImplementation> _nativeInterface;
};
}  // namespace pfw

#endif /* PFW_NATIVE_INTERFACE_H */
