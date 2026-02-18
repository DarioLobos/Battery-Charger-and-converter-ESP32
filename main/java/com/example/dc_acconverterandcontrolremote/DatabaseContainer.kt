package com.example.dc_acconverterandcontrolremote

import android.content.Context

interface DatabaseContainer {
    val devicesRepository : DevicesRepository

}

class AppDatabaseContainer(private val context: Context) : DatabaseContainer  {

    override val devicesRepository: DevicesRepository by lazy {
        OfflineDevicesRepository(DevicesDatabase.DevicesDataBase(context).daoDevices())
    }
}