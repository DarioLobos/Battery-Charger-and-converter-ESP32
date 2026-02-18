package com.example.dc_acconverterandcontrolremote
import androidx.room.Database
import androidx.room.RoomDatabase
import android.content.Context
import androidx.compose.runtime.Composable
import androidx.room.Room


@Database(entities = arrayOf(Devices::class), version = 1, exportSchema = false)
abstract class DevicesDatabase : RoomDatabase() {
abstract fun daoDevices(): DaoDevices

    companion object {
        @Volatile
        private var devicesRepository: DevicesDatabase? = null

        fun DevicesDataBase (context: Context): DevicesDatabase {
            // if the Instance is not null, return it, otherwise create a new database instance.return devicesRepository ?: synchronized(this) {
            return devicesRepository ?:   Room.databaseBuilder(context , DevicesDatabase::class.java, "devices_database")
                    .build()
                    .also { devicesRepository = it }

            }
        }
    }
