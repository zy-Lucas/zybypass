#include "methodData.hpp"
#include "method.hpp"
#include <cassert>

namespace hotspot::oops
{
void MethodData::clean_extra_data_helper(DataLayout dp, int32_t shift, bool reset)
{
    if (!shift)
        return;

    uint64_t dp_address = dp.address() + dp.dp();

    if (!reset)
    {
        int64_t *start = (int64_t *)dp_address;
        int64_t *end = (int64_t *)(next_extra(dp).address() + next_extra(dp).dp());
        for (int64_t *ptr = start; ptr < end; ++ptr)
            *(ptr - shift) = *ptr;
        return;
    }
    int64_t *start = (int64_t *)dp_address - shift;
    int64_t *end = (int64_t *)dp_address;
    for (int64_t *ptr = start; ptr < end; ++ptr)
        *ptr = 0;
}

void MethodData::clean_extra_data()
{
    DataLayout dp{extra_data_base()};
    DataLayout end{args_data_limit()};

    for (int32_t shift = 0; dp.dp() < end.dp(); dp = next_extra(dp))
    {
        switch (dp.tag())
        {
        case DataLayout::speculative_trap_data_tag: {
            if (SpeculativeTrapData data{dp}; data.method().is_old())
            {
                shift += (next_extra(dp).dp() - dp.dp()) / sizeof(int64_t);
                break;
            }
            clean_extra_data_helper(dp, shift);
            break;
        }
        case DataLayout::bit_data_tag:
            clean_extra_data_helper(dp, shift);
            continue;
        case DataLayout::no_tag:
        case DataLayout::arg_info_data_tag:
            clean_extra_data_helper(dp, shift, true);
            return;
        default:
            assert(false && "unexpected tag in clean_extra_data");
            return;
        }
    }
}

int32_t MethodData::parameters_size_in_bytes() const noexcept
{
    ParametersTypeData param{parameters_type_data()};
    return !param.dp() ? 0 : param.size_in_bytes();
}

ParametersTypeData MethodData::parameters_type_data() const noexcept
{
    if (parameters_type_data_di() != no_parameters)
        return {{address(), parameters_type_data_di() + data_offset_}};
    return {{0, 0}};
}

DataLayout MethodData::next_extra(DataLayout dp) noexcept
{
    int32_t nb_cells = 0;
    switch (dp.tag())
    {
    case DataLayout::bit_data_tag:
    case DataLayout::no_tag:
        nb_cells = BitData::static_cell_count();
        break;
    case DataLayout::speculative_trap_data_tag:
        nb_cells = SpeculativeTrapData::static_cell_count();
        break;
    default:
        assert(false && "unexpected tag in next_extra");
        break;
    }
    return {dp.address(), dp.dp() + DataLayout::compute_size_in_bytes(nb_cells)};
}

void MethodData::initialize()
{
    utils::FieldResolver r{"MethodData"};

    r.field_offset("_method", method_offset_);
    r.field_offset("_size", size_offset_);
    r.field_offset("_data_size", data_size_offset_);
    r.field_offset("_parameters_type_data_di", parameters_type_data_di_offset_);
    r.field_offset("_data[0]", data_offset_);
}
} // namespace hotspot::oops