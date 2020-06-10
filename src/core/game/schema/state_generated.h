// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_STATE_H_
#define FLATBUFFERS_GENERATED_STATE_H_

#include "flatbuffers/flatbuffers.h"

struct Position;
struct PositionBuilder;

struct Components;
struct ComponentsBuilder;

struct Entity;
struct EntityBuilder;

struct State;
struct StateBuilder;

enum class Action : uint8_t {
  Create = 0,
  Update = 1,
  Delete = 2,
  MIN = Create,
  MAX = Delete
};

inline const Action (&EnumValuesAction())[3] {
  static const Action values[] = {
    Action::Create,
    Action::Update,
    Action::Delete
  };
  return values;
}

inline const char * const *EnumNamesAction() {
  static const char * const names[4] = {
    "Create",
    "Update",
    "Delete",
    nullptr
  };
  return names;
}

inline const char *EnumNameAction(Action e) {
  if (flatbuffers::IsOutRange(e, Action::Create, Action::Delete)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesAction()[index];
}

struct Position FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef PositionBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6
  };
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X) &&
           VerifyField<float>(verifier, VT_Y) &&
           verifier.EndTable();
  }
};

struct PositionBuilder {
  typedef Position Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(Position::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(Position::VT_Y, y, 0.0f);
  }
  explicit PositionBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Position> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Position>(end);
    return o;
  }
};

inline flatbuffers::Offset<Position> CreatePosition(
    flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f) {
  PositionBuilder builder_(_fbb);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

struct Components FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ComponentsBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_POSITION = 4
  };
  const Position *position() const {
    return GetPointer<const Position *>(VT_POSITION);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_POSITION) &&
           verifier.VerifyTable(position()) &&
           verifier.EndTable();
  }
};

struct ComponentsBuilder {
  typedef Components Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_position(flatbuffers::Offset<Position> position) {
    fbb_.AddOffset(Components::VT_POSITION, position);
  }
  explicit ComponentsBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Components> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Components>(end);
    return o;
  }
};

inline flatbuffers::Offset<Components> CreateComponents(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<Position> position = 0) {
  ComponentsBuilder builder_(_fbb);
  builder_.add_position(position);
  return builder_.Finish();
}

struct Entity FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef EntityBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_ACTION = 6,
    VT_COMPONENTS = 8
  };
  uint32_t id() const {
    return GetField<uint32_t>(VT_ID, 0);
  }
  Action action() const {
    return static_cast<Action>(GetField<uint8_t>(VT_ACTION, 0));
  }
  const Components *components() const {
    return GetPointer<const Components *>(VT_COMPONENTS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_ID) &&
           VerifyField<uint8_t>(verifier, VT_ACTION) &&
           VerifyOffset(verifier, VT_COMPONENTS) &&
           verifier.VerifyTable(components()) &&
           verifier.EndTable();
  }
};

struct EntityBuilder {
  typedef Entity Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint32_t id) {
    fbb_.AddElement<uint32_t>(Entity::VT_ID, id, 0);
  }
  void add_action(Action action) {
    fbb_.AddElement<uint8_t>(Entity::VT_ACTION, static_cast<uint8_t>(action), 0);
  }
  void add_components(flatbuffers::Offset<Components> components) {
    fbb_.AddOffset(Entity::VT_COMPONENTS, components);
  }
  explicit EntityBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Entity> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Entity>(end);
    return o;
  }
};

inline flatbuffers::Offset<Entity> CreateEntity(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t id = 0,
    Action action = Action::Create,
    flatbuffers::Offset<Components> components = 0) {
  EntityBuilder builder_(_fbb);
  builder_.add_components(components);
  builder_.add_id(id);
  builder_.add_action(action);
  return builder_.Finish();
}

struct State FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef StateBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ENTITIES = 4
  };
  const flatbuffers::Vector<flatbuffers::Offset<Entity>> *entities() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Entity>> *>(VT_ENTITIES);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_ENTITIES) &&
           verifier.VerifyVector(entities()) &&
           verifier.VerifyVectorOfTables(entities()) &&
           verifier.EndTable();
  }
};

struct StateBuilder {
  typedef State Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_entities(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Entity>>> entities) {
    fbb_.AddOffset(State::VT_ENTITIES, entities);
  }
  explicit StateBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<State> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<State>(end);
    return o;
  }
};

inline flatbuffers::Offset<State> CreateState(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Entity>>> entities = 0) {
  StateBuilder builder_(_fbb);
  builder_.add_entities(entities);
  return builder_.Finish();
}

inline flatbuffers::Offset<State> CreateStateDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<Entity>> *entities = nullptr) {
  auto entities__ = entities ? _fbb.CreateVector<flatbuffers::Offset<Entity>>(*entities) : 0;
  return CreateState(
      _fbb,
      entities__);
}

inline const State *GetState(const void *buf) {
  return flatbuffers::GetRoot<State>(buf);
}

inline const State *GetSizePrefixedState(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<State>(buf);
}

inline bool VerifyStateBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<State>(nullptr);
}

inline bool VerifySizePrefixedStateBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<State>(nullptr);
}

inline void FinishStateBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<State> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedStateBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<State> root) {
  fbb.FinishSizePrefixed(root);
}

#endif  // FLATBUFFERS_GENERATED_STATE_H_