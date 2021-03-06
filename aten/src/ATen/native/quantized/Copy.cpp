#include <ATen/native/quantized/Copy.h>

#include <ATen/ATen.h>
#include <ATen/quantized/Quantizer.h>

namespace at {
namespace native {
Tensor& quantized_copy_(Tensor& self, const Tensor& src) {
  TORCH_CHECK(
      src.scalar_type() == at::kFloat,
      "Quantized copy only works with kFloat as source Tensor");
  TORCH_CHECK(self.is_contiguous() && src.is_contiguous(),
      "Quantized copy only works with contiguous Tensors");
  TORCH_CHECK(self.sizes().equals(src.sizes()),
      "Quantized copy only works with Tensors with the same shape");
  float* src_data = src.data<float>();
  AT_DISPATCH_QINT_TYPES(self.scalar_type(), "Copy", [&]() {
    scalar_t* self_data = self.data<scalar_t>();
    for (int i = 0; i < self.numel(); ++i) {
      self_data[i] = quantize_val<scalar_t>(
          self.q_scale().to<float>(),
          self.q_zero_point().to<int32_t>(),
          src_data[i]);
    }
  });
  return self;
}
} // namespace native
} // namespace at
