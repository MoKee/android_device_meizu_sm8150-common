/*
 * Copyright (C) 2020 The MoKee Open Source Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

package org.mokee.device.parts;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import org.mokee.internal.util.FileUtils;

public class AODService extends Service {

    private static final String TAG = "AODService";
    private static final boolean DEBUG = false;

    private SettingObserver mSettingObserver;
    private ScreenReceiver mScreenReceiver;

    @Override
    public void onCreate() {
        super.onCreate();
        if (DEBUG) Log.d(TAG, "Creating service");

        mSettingObserver = new SettingObserver(this);
        mScreenReceiver = new ScreenReceiver(this);

        mSettingObserver.enable();

        if (Utils.isAODEnabled(this)) {
            mScreenReceiver.enable();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (DEBUG) Log.d(TAG, "Destroying service");

        mSettingObserver.disable();
        mScreenReceiver.disable();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (DEBUG) Log.d(TAG, "Starting service");
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    void onSettingChange() {
        if (Utils.isAODEnabled(this)) {
            Log.d(TAG, "AOD enabled");
            mScreenReceiver.enable();
        } else {
            Log.d(TAG, "AOD disabled");
            mScreenReceiver.disable();
        }
    }

    void onDisplayOn() {
        Log.d(TAG, "Device interactive");
        FileUtils.writeLine(Constants.AOD_ENABLE, "0");
    }

    void onDisplayOff() {
        Log.d(TAG, "Device non-interactive");
        FileUtils.writeLine(Constants.AOD_ENABLE, "1");
    }

}
