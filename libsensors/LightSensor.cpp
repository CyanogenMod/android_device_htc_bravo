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

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>

#include <linux/lightsensor.h>

#include <cutils/log.h>

#include "LightSensor.h"

/*****************************************************************************/

LightSensor::LightSensor()
    : SensorBase(LS_DEVICE_NAME, "lightsensor-level"),
      mEnabled(0),
      mInputReader(4),
      mHasPendingEvent(false)
{
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = ID_L;
    mPendingEvent.type = SENSOR_TYPE_LIGHT;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

     open_device();

    int flags = 0;
    if (!ioctl(dev_fd, LIGHTSENSOR_IOCTL_GET_ENABLED, &flags)) {
        if (flags) {
            mEnabled = 1;
            setInitialState();
        }
    }

    if (!mEnabled) {
        close_device();
    }
}

LightSensor::~LightSensor() {
}

int LightSensor::setInitialState() {
    struct input_absinfo absinfo;
    if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_LIGHT), &absinfo)) {
        mPendingEvent.light = indexToValue(absinfo.value);
        mHasPendingEvent = true;
    }
    return 0;
}

int LightSensor::enable(int32_t, int en) {
    int flags = en ? 1 : 0;
    int err = 0;
    if (flags != mEnabled) {
        if (!mEnabled) {
            open_device();
        }
        err = ioctl(dev_fd, LIGHTSENSOR_IOCTL_ENABLE, &flags);
        err = err<0 ? -errno : 0;
        LOGE_IF(err, "LIGHTSENSOR_IOCTL_ENABLE failed (%s)", strerror(-err));
        if (!err) {
            mEnabled = en ? 1 : 0;
            if (en) {
                setInitialState();
            }
        }
        if (!mEnabled) {
            close_device();
        }
    }
    return err;
}

bool LightSensor::hasPendingEvents() const {
    return mHasPendingEvent;
}

int LightSensor::readEvents(sensors_event_t* data, int count)
{
    if (count < 1)
        return -EINVAL;

    if (mHasPendingEvent) {
        mHasPendingEvent = false;
        mPendingEvent.timestamp = getTimestamp();
        *data = mPendingEvent;
        return mEnabled ? 1 : 0;
    }

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0)
        return n;

    int numEventReceived = 0;
    input_event const* event;

    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            if (event->code == EVENT_TYPE_LIGHT) {
                if (event->value != -1) {
                    // FIXME: not sure why we're getting -1 sometimes
                    mPendingEvent.light = indexToValue(event->value);
                }
            }
        } else if (type == EV_SYN) {
            mPendingEvent.timestamp = timevalToNano(event->time);
            if (mEnabled) {
                *data++ = mPendingEvent;
                count--;
                numEventReceived++;
            }
        } else {
            LOGE("LightSensor: unknown event (type=%d, code=%d)",
                    type, event->code);
        }
        mInputReader.next();
    }

    return numEventReceived;
}

float LightSensor::indexToValue(size_t index) const
{
    static const float luxValues[10] = {
            10.0, 160.0, 225.0, 320.0, 640.0, 
            1280.0, 2600.0, 5800.0, 8000.0, 10240.0
    };

    const size_t maxIndex = sizeof(luxValues)/sizeof(*luxValues) - 1;
    if (index > maxIndex)
        index = maxIndex;
    return luxValues[index];
}
