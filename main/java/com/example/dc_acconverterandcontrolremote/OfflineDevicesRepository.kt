package com.example.dc_acconverterandcontrolremote

import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flowOf

class OfflineDevicesRepository(private val itemDao: DaoDevices) : DevicesRepository {

    override fun getAllDevicesStream(): Flow<List<Devices>> = flowOf( itemDao.getAll())

    override fun getDeviceStream(deviceId: Int): Flow<Devices?> = itemDao.getItem(deviceId)

    override suspend fun insertDevice(device: Devices) = itemDao.insert(device)


    override suspend fun deleteDevice(device: Devices) = itemDao.delete(device)

    override suspend fun updateDevice(device: Devices) = itemDao.update(device)
}
