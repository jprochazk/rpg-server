// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_ID_H_
#define FLATBUFFERS_GENERATED_ID_H_

#include "flatbuffers/flatbuffers.h"

struct Id;
struct IdBuilder;

struct Id FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef IdBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_VALUE = 4
  };
  uint32_t value() const {
    return GetField<uint32_t>(VT_VALUE, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_VALUE) &&
           verifier.EndTable();
  }
};

struct IdBuilder {
  typedef Id Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_value(uint32_t value) {
    fbb_.AddElement<uint32_t>(Id::VT_VALUE, value, 0);
  }
  explicit IdBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Id> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Id>(end);
    return o;
  }
};

inline flatbuffers::Offset<Id> CreateId(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t value = 0) {
  IdBuilder builder_(_fbb);
  builder_.add_value(value);
  return builder_.Finish();
}

inline const Id *GetId(const void *buf) {
  return flatbuffers::GetRoot<Id>(buf);
}

inline const Id *GetSizePrefixedId(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<Id>(buf);
}

inline bool VerifyIdBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<Id>(nullptr);
}

inline bool VerifySizePrefixedIdBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<Id>(nullptr);
}

inline void FinishIdBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<Id> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedIdBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<Id> root) {
  fbb.FinishSizePrefixed(root);
}

#endif  // FLATBUFFERS_GENERATED_ID_H_
