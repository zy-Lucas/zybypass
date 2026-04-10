#pragma once

#include "compressedStream.hpp"
#include "nmethod.hpp"

namespace hotspot::code
{
class DebugInfoReadStream : public CompressedReadStream
{
  public:
    DebugInfoReadStream(nmethod code, uint32_t offset);

    oops::Method read_method() { return code.get_method(read_int()); }

    uint32_t read_bic() { return read_int() + *runtime::Jvm::get_invocation_entry_bic(); }

  private:
    nmethod code;
};
} // namespace hotspot::code