// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/api/trackable_object.h"

#include "atom/browser/atom_browser_main_parts.h"
#include "base/bind.h"
#include "base/supports_user_data.h"

namespace mate {

namespace {

const char* kTrackedObjectKey = "TrackedObjectKey";

class IDUserData : public base::SupportsUserData::Data {
 public:
  explicit IDUserData(int32_t id) : id_(id) {}

  operator int32_t() const { return id_; }

 private:
  int32_t id_;

  DISALLOW_COPY_AND_ASSIGN(IDUserData);
};

}  // namespace

TrackableObjectBase::TrackableObjectBase()
    : weak_map_id_(0), weak_factory_(this) {
  atom::AtomBrowserMainParts::Get()->RegisterDestructionCallback(
      GetDestroyClosure());
}

TrackableObjectBase::~TrackableObjectBase() {}

base::OnceClosure TrackableObjectBase::GetDestroyClosure() {
  return base::BindOnce(&TrackableObjectBase::Destroy,
                        weak_factory_.GetWeakPtr());
}

void TrackableObjectBase::Destroy() {
  delete this;
}

void TrackableObjectBase::AttachAsUserData(base::SupportsUserData* wrapped) {
  wrapped->SetUserData(kTrackedObjectKey,
                       std::make_unique<IDUserData>(weak_map_id_));
}

// static
int32_t TrackableObjectBase::GetIDFromWrappedClass(
    base::SupportsUserData* wrapped) {
  if (wrapped) {
    auto* id = static_cast<IDUserData*>(
        wrapped->GetUserData(kTrackedObjectKey));
    if (id)
      return *id;
  }
  return 0;
}

}  // namespace mate
