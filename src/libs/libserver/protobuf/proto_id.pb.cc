// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proto_id.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "proto_id.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace Proto {

namespace {

const ::google::protobuf::EnumDescriptor* MsgId_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_proto_5fid_2eproto() {
  protobuf_AddDesc_proto_5fid_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "proto_id.proto");
  GOOGLE_CHECK(file != NULL);
  MsgId_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_proto_5fid_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void protobuf_ShutdownFile_proto_5fid_2eproto() {
}

void protobuf_AddDesc_proto_5fid_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\016proto_id.proto\022\005Proto*)\n\005MsgId\022\010\n\004None"
    "\020\000\022\014\n\010SendData\020\001\022\010\n\004Over\020\002b\006proto3", 74);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "proto_id.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_proto_5fid_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_proto_5fid_2eproto {
  StaticDescriptorInitializer_proto_5fid_2eproto() {
    protobuf_AddDesc_proto_5fid_2eproto();
  }
} static_descriptor_initializer_proto_5fid_2eproto_;
const ::google::protobuf::EnumDescriptor* MsgId_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return MsgId_descriptor_;
}
bool MsgId_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Proto

// @@protoc_insertion_point(global_scope)
