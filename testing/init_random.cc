/**
 * Copyright 2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <random>
#include "random.h"

//
// Generate float random numbers into a python buffer.
//
template <typename Generator, typename Distribution, typename... Args>
void GenerateFloatRandoms(std::uint64_t seed, float *buf, size_t buf_size, Args... args) {
  // Parallel generate randoms into buffer.
  random_udf::GenerateRandomsParallel<float, Generator, Distribution>(seed, buf, buf_size, args...);
}

void RandomUniform(std::uint64_t seed, float *buf, size_t buf_size, float a, float b) {
  using Generator = random_udf::Philox;
  using Distribution = random_udf::UniformDistribution<double>;
  GenerateFloatRandoms<Generator, Distribution>(seed, buf, buf_size, a, b);
}

void RandomNormal(std::uint64_t seed, float *buf, size_t buf_size, float mean, float sigma) {
  using Generator = random_udf::Philox;
  using Distribution = random_udf::NormalDistribution<double>;
  GenerateFloatRandoms<Generator, Distribution>(seed, buf, buf_size, mean, sigma);
}

void TruncatedNormal(std::uint64_t seed, float *buf, size_t buf_size, float a, float b, float mean, float sigma) {
  using Generator = random_udf::Philox;
  using Distribution = random_udf::TruncatedNormal<double>;
  GenerateFloatRandoms<Generator, Distribution>(seed, buf, buf_size, a, b, mean, sigma);
}