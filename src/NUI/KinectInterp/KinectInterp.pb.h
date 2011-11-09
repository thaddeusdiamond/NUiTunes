// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: KinectInterp.proto

#ifndef PROTOBUF_KinectInterp_2eproto__INCLUDED
#define PROTOBUF_KinectInterp_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_KinectInterp_2eproto();
void protobuf_AssignDesc_KinectInterp_2eproto();
void protobuf_ShutdownFile_KinectInterp_2eproto();

class KinectInterpProto;

// ===================================================================

class KinectInterpProto : public ::google::protobuf::Message {
 public:
  KinectInterpProto();
  virtual ~KinectInterpProto();
  
  KinectInterpProto(const KinectInterpProto& from);
  
  inline KinectInterpProto& operator=(const KinectInterpProto& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const KinectInterpProto& default_instance();
  
  void Swap(KinectInterpProto* other);
  
  // implements Message ----------------------------------------------
  
  KinectInterpProto* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const KinectInterpProto& from);
  void MergeFrom(const KinectInterpProto& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // repeated bool active_skeletons = 1;
  inline int active_skeletons_size() const;
  inline void clear_active_skeletons();
  static const int kActiveSkeletonsFieldNumber = 1;
  inline bool active_skeletons(int index) const;
  inline void set_active_skeletons(int index, bool value);
  inline void add_active_skeletons(bool value);
  inline const ::google::protobuf::RepeatedField< bool >&
      active_skeletons() const;
  inline ::google::protobuf::RepeatedField< bool >*
      mutable_active_skeletons();
  
  // @@protoc_insertion_point(class_scope:KinectInterpProto)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedField< bool > active_skeletons_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_KinectInterp_2eproto();
  friend void protobuf_AssignDesc_KinectInterp_2eproto();
  friend void protobuf_ShutdownFile_KinectInterp_2eproto();
  
  void InitAsDefaultInstance();
  static KinectInterpProto* default_instance_;
};
// ===================================================================


// ===================================================================

// KinectInterpProto

// repeated bool active_skeletons = 1;
inline int KinectInterpProto::active_skeletons_size() const {
  return active_skeletons_.size();
}
inline void KinectInterpProto::clear_active_skeletons() {
  active_skeletons_.Clear();
}
inline bool KinectInterpProto::active_skeletons(int index) const {
  return active_skeletons_.Get(index);
}
inline void KinectInterpProto::set_active_skeletons(int index, bool value) {
  active_skeletons_.Set(index, value);
}
inline void KinectInterpProto::add_active_skeletons(bool value) {
  active_skeletons_.Add(value);
}
inline const ::google::protobuf::RepeatedField< bool >&
KinectInterpProto::active_skeletons() const {
  return active_skeletons_;
}
inline ::google::protobuf::RepeatedField< bool >*
KinectInterpProto::mutable_active_skeletons() {
  return &active_skeletons_;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_KinectInterp_2eproto__INCLUDED
