/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _SHAREBACKEND_H
#define _SHAREBACKEND_H

#include <utils/List.h>

#include "Volume.h"

class ShareBackend;
typedef android::List<ShareBackend *> ShareBackendCollection;

class ShareBackend {
public:
    virtual ~ShareBackend() {}
    static ShareBackend *Instance(const char *method);

    virtual const char *name() = 0;

    virtual int shareVolume(Volume *v) = 0;
    virtual int unshareVolume(Volume *v) = 0;

private:
    static void initialize();

    static ShareBackendCollection sBackends;
    static bool sInitialized;
};

#endif
