//===-- MCInstOrData.cpp ----------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCInstOrData.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"

MCInstOrData::MCInstOrData(const MCInstOrData &v) {
  type = v.type;

  switch (v.type) {
  case Tag::DATA:
    d = v.d;
    break;
  case Tag::INSTRUCTION:
    new (&i) MCInst(v.i);
  }
}

MCInstOrData::MCInstOrData(const MCInst &v) {
  type = Tag::INSTRUCTION;
  new (&i) MCInst(v); // placement new: explicitly construct MCInst
}

MCInstOrData::MCInstOrData(const uint32_t v) {
  type = Tag::DATA;
  d = v;
}

uint32_t MCInstOrData::get_data() const {
  assert(type == Tag::DATA);
  return d;
}

MCInst MCInstOrData::get_mcInst() const {
  assert(type == Tag::INSTRUCTION);
  return i;
}

// This is needed because of user-defined variant MCInst being part of MCInst
MCInstOrData &MCInstOrData::operator=(const MCInstOrData &e) {
  if (type == Tag::INSTRUCTION && e.type == Tag::INSTRUCTION) {
    i = e.i; // Usual MCInst assignment
    return *this;
  }
  if (type == Tag::INSTRUCTION)
    i.~MCInst(); // Explicit destroy

  switch (e.type) {
  case Tag::DATA:
    d = e.d;
    break;
  case Tag::INSTRUCTION:
    new (&i) MCInst(e.i);
    type = e.type;
  }
  return *this;
}

void MCInstOrData::dump() const {
  switch (type) {
  case Tag::DATA:
    outs() << "0x" << format("%04" PRIx16, d) << "\n";
    break;
  case Tag::INSTRUCTION:
    i.dump();
    break;
  }
}
MCInstOrData::~MCInstOrData() {
  if (type == Tag::INSTRUCTION)
    i.~MCInst(); // explicit destroy
}
