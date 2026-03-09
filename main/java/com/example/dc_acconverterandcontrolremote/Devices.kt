package com.example.dc_acconverterandcontrolremote
import androidx.room.ColumnInfo
import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "devices")
data class Devices (
    @PrimaryKey(autoGenerate = false)
    @ColumnInfo(name="device_number") val device_number: Int? = null,
    @ColumnInfo(name="device_name")var device_name: String? =null,
    @ColumnInfo(name="hour_on") var hour_on: Int? = null,
    @ColumnInfo(name="minutes_on") var minutes_on: Int? = null,
    @ColumnInfo(name="hour_off") var hour_off: Int? = null,
    @ColumnInfo(name="minutes_off") var minutes_off: Int? = null,
    @ColumnInfo(name="days_week") var days_week: Int? = null,

    )