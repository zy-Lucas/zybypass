#include "profileData.hpp"
#include "oops/method.hpp"

namespace hotspot::oops
{
TypeStackSlotEntries::TypeStackSlotEntries(ProfileData *pd, int32_t base_off, int32_t nb_entries) noexcept
    : TypeEntries(pd, base_off), number_of_entries_(nb_entries)
{
}

ParametersTypeData::ParametersTypeData(DataLayout layout) noexcept
    : ArrayData(layout), parameters_(this, 1, number_of_parameters())
{
}

Method SpeculativeTrapData::method() const noexcept { return cell_at(speculative_trap_method); }

void SpeculativeTrapData::set_method(Method m) noexcept { set_cell_at(speculative_trap_method, m.address()); }
} // namespace hotspot::oops