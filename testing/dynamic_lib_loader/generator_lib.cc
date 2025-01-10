#include "generator_lib.h"

static int kStaticData = 0;
int kData = 0;

__attribute__((visibility("default"))) int generate() {
  kStaticData++;
  kData++;
  return kStaticData + kData;
}