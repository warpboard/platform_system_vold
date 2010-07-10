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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define LOG_TAG "Vold"

#include <cutils/log.h>
#include <utils/List.h>

#include "ShareBackend.h"
#include "Ums.h"

ShareBackendCollection ShareBackend::sBackends;
bool ShareBackend::sInitialized = false;

void ShareBackend::initialize() {
    // Add new backends here as they are supported
    sBackends.push_back(new Ums());
}

ShareBackend *ShareBackend::Instance(const char *method) {
    ShareBackendCollection::iterator i;
    if (!sInitialized) {
        initialize();
        sInitialized = true;
    }

    for (i = sBackends.begin(); i != sBackends.end(); i++) {
        if (!strcmp((*i)->name(), method))
            return (*i);
    }

    return NULL;
}
