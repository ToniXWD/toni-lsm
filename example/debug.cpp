#include "../include/lsm/engine.h"
#include <iostream>
#include <string>

int main() {
  LSM lsm("test");
  // change the data manually
  lsm.put("k1", "v1");
  lsm.clear();
}