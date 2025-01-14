// Copyright 2024 Google LLC.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tensorflow/lite/experimental/litert/cc/litert_model.h"

#include "absl/types/span.h"
#include "tensorflow/lite/experimental/litert/c/litert_model.h"
#include "tensorflow/lite/experimental/litert/cc/litert_macros.h"
#include "tensorflow/lite/experimental/litert/core/graph_tools.h"

namespace litert {

bool Tensor::IsSubgraphOutput() const {
  return internal::MatchTensorNoUses(Get());
}

bool Tensor::IsSubgraphInput() const {
  return internal::MatchTensorNoDefiningOp(Get()) &&
         internal::MatchNoWeights(Get());
}

bool Tensor::IsConstant() const {
  return internal::MatchTensorNoDefiningOp(Get()) &&
         !internal::MatchNoWeights(Get());
}

Tensor::UsesT Tensor::Uses() const {
  LiteRtParamIndex num_uses;
  LiteRtOpArray users;
  LiteRtParamIndex* user_arg_inds;
  litert::internal::AssertGet(LiteRtGetTensorUses, Get(), &num_uses, &users,
                              &user_arg_inds);
  UsesT res;
  for (auto op = users; op < users + num_uses; ++op) {
    res.users.push_back(Op(*op));
  }
  res.user_arg_inds = absl::MakeSpan(user_arg_inds, num_uses);
  return res;
}

}  // namespace litert
