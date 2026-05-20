#pragma once

#include "compressedStream.hpp"
#include "nmethod.hpp"

namespace hotspot::code
{
class DebugInfoReadStream : public CompressedReadStream
{
  public:
    DebugInfoReadStream(nmethod code, uint32_t offset);

    oops::Method read_method() noexcept { return code_.get_method(read_int()); }

    uint32_t read_bic() noexcept { return read_int() + runtime::Jvm::invocation_entry_bci(); }

  private:
    nmethod code_;
};
} // namespace hotspot::code