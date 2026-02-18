package com.example.dc_acconverterandcontrolremote
import kotlinx.coroutines.flow.Flow

interface DevicesRepository {

    public fun getAllDevicesStream(): Flow<List<Devices>>

    public fun getDeviceStream(deviceId: Int): Flow<Devices?>


    public suspend fun insertDevice(device: Devices)


    public suspend fun deleteDevice(device: Devices)


    public suspend fun updateDevice(device: Devices)
}

