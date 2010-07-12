/*
 * Copyright 2010 by Garmin Ltd. or its subsidiaries
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

#ifndef _UMS_H
#define _UMS_H

#include "Volume.h"
#include "ShareBackend.h"

typedef struct UmsLun {
    int lunNum;
    Volume *v;
} UmsLun;

typedef android::List<UmsLun *> UmsLunCollection;

class Ums : public ShareBackend {
public:
    const char *name() { return "ums"; }

    int shareVolume(Volume *v);
    int unshareVolume(Volume *v);

protected:
    UmsLunCollection mLuns;

    int allocateLun(Volume *v);
    int freeLun(Volume *v);
};

#endif
