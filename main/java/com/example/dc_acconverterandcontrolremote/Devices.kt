package com.example.dc_acconverterandcontrolremote
import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "devices")
data class Devices (
    @PrimaryKey(autoGenerate = true)
    val device_number: Int = 0,
    var device_name: String ="Device 0",
    var hour_on: Int? = null,
    var minutes_on: Int? = null,
    var hour_off: Int? = null,
    var minutes_off: Int? = null,
    var days_week: Int? = null,

    )