package com.example.dc_acconverterandcontrolremote
import androidx.room.Dao
import androidx.room.Delete
import androidx.room.Insert
import androidx.room.Update
import androidx.room.OnConflictStrategy
import androidx.room.Query
import kotlinx.coroutines.flow.*
@Dao
interface DaoDevices {

    @Query("SELECT * FROM devices")
 public fun getAll(): List<Devices>

    @Query("SELECT * FROM devices WHERE device_number = :deviceId")
 public fun getItem(deviceId: Int): Flow<Devices>


    @Query("SELECT * FROM devices WHERE device_number IN (:deviceIds)")
 public fun loadAllByIds(deviceIds: IntArray): List<Devices>

    @Update
 public  suspend fun update(device: Devices)

    @Insert(onConflict = OnConflictStrategy.IGNORE)
 public fun insert ( vararg device: Devices)

    @Delete
 public  fun delete(device: Devices)

}