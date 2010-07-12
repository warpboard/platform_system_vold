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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/kdev_t.h>

#define LOG_TAG "Vold"

#include <cutils/log.h>

#include "ShareBackend.h"
#include "Ums.h"

int Ums::shareVolume(Volume *v) {
    if (v->getState() != Volume::State_Idle) {
        // You need to unmount manually before sharing
        errno = EBUSY;
        return -1;
    }

    dev_t d = v->getPartDevice();
    if ((MAJOR(d) == 0) && (MINOR(d) == 0)) {
        // this device does not support raw disk access
        errno = EINVAL;
        return -1;
    }

    char nodepath[255];
    snprintf(nodepath,
             sizeof(nodepath), "/dev/block/vold/%d:%d",
             MAJOR(d), MINOR(d));

    int fd;
    int lunNum = allocateLun(v);
    char lunpath[255];
    snprintf(lunpath, sizeof(lunpath),
             "/sys/devices/platform/usb_mass_storage/lun%d/file", lunNum);
    if ((fd = open(lunpath, O_WRONLY)) < 0) {
        SLOGE("Unable to open ums lunfile (%s)", strerror(errno));
        goto err_open;
    }

    if (write(fd, nodepath, strlen(nodepath)) < 0) {
        SLOGE("Unable to write to ums lunfile (%s)", strerror(errno));
        goto err_write;
    }

    close(fd);
    return 0;

err_write:
    close(fd);
err_open:
    freeLun(v);
    return -1;
}

int Ums::unshareVolume(Volume *v) {
    int lunNum = freeLun(v);
    if (lunNum == -1) {
        SLOGE("No lun allocated for volume %s", v->getLabel());
        return -1;
    }

    int fd;
    char lunpath[64];
    snprintf(lunpath, sizeof(lunpath),
             "/sys/devices/platform/usb_mass_storage/lun%d/file", lunNum);
    if ((fd = open(lunpath, O_WRONLY)) < 0) {
        SLOGE("Unable to open ums lunfile (%s)", strerror(errno));
        return -1;
    }

    char ch = 0;
    if (write(fd, &ch, 1) < 0) {
        SLOGE("Unable to write to ums lunfile (%s)", strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int Ums::allocateLun(Volume *v) {
    int lunNum = 0;
    UmsLunCollection::iterator i;

    /*
     * mLuns must be kept sorted so that we can accurately determine
     * the lowest free lun number
     */
    for (i = mLuns.begin(); i != mLuns.end(); i++) {
        if (lunNum != (*i)->lunNum)
            break;
        lunNum++;
    }

    UmsLun *l = new UmsLun;
    l->lunNum = lunNum;
    l->v = v;
    mLuns.insert(i, l);

    return lunNum;
}

// returns the lun number that had been allocated to the volume, or -1 if none
int Ums::freeLun(Volume *v) {
    UmsLunCollection::iterator i;

    for (i = mLuns.begin(); i != mLuns.end(); i++) {
        if (v == (*i)->v) {
            UmsLun *l = (*i);
            int lunNum = l->lunNum;
            mLuns.erase(i);
            delete l;
            return lunNum;
        }
    }
    return -1;
}
