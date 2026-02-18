package com.example.dc_acconverterandcontrolremote

import androidx.compose.material3.TimePickerState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.res.stringResource
import androidx.lifecycle.ViewModel
import java.util.Calendar

class DeviceSchedulerViewModel: ViewModel() {

    var device_number by mutableStateOf(-1)

    var on_or_off by mutableStateOf("")

    var device by mutableStateOf("")

    var hourSet by mutableStateOf(12)
    var minuteSet by mutableStateOf(0)
    var calendar by mutableStateOf(Calendar.getInstance())

    var hourForPicker by mutableStateOf(hourSet ?: calendar[Calendar.HOUR_OF_DAY])

    var minuteForPicker by mutableStateOf(minuteSet ?: calendar[Calendar.MINUTE])

    var days_Selected by mutableStateOf(0)

    var selectedTime by mutableStateOf(0)

   fun setDeviceNumber(device_number:Int){
        this.device_number =device_number
    }

    fun setOnorOff(on_or_off: String){
        this.on_or_off =on_or_off
    }

    fun TimesToshow(device_number: Int, on_or_off: String, string_onoff: String): String {

        val device: List<Devices> = devicesDao.getItem(device_number) as List<Devices>
        var timeToShow: String = ""

        if (on_or_off == string_onoff) {
            hourSet = device.get(0).hour_on!!
            minuteSet = device.get(0).minutes_on!!
            timeToShow = "$hourSet : $minuteSet"
        } else {
            hourSet = device.get(0).hour_off!!
            minuteSet = device.get(0).minutes_off!!
            timeToShow = "$hourSet : $minuteSet"
        }
        return timeToShow
    }

    fun deviceName(device_number: Int): String{
        val device: List<Devices> = devicesDao.getItem(device_number) as List<Devices>

        return   device.get(0).device_name
    }

    fun setselectedTime(
        hourToSeT: Int,
        minuteToSet: Int,
        device_number: Int,
        on_or_off: String,
        string_onoff: String
        ): String {
            val device: List<Devices> = devicesDao.getItem(device_number) as List<Devices>

            if ((hourToSeT >= 0) and ((minuteToSet >= 0))) {

                if (on_or_off == string_onoff) {
                    device.get(0).hour_on = hourToSeT
                    device.get(0).minutes_on = minuteToSet
                } else {
                    device.get(0).hour_off = hourToSeT
                    device.get(0).minutes_off = minuteToSet
                }
            }
            return "$hourSet : $minuteSet"
        }


    fun selectedDays( option: Int, selectedOptions: Boolean) {
        when (option) {
             0 ->
                if (selectedOptions == true) {
                    days_Selected = days_Selected or 1
                } else {
                    days_Selected = (days_Selected and (1).inv())
                }

            1 ->
                if (selectedOptions == true) {
                    days_Selected = days_Selected or 2
                } else {
                    days_Selected = (days_Selected and (2).inv())
                }

            2 ->
                if (selectedOptions == true) {
                    days_Selected = days_Selected or 4
                } else {
                    days_Selected = (days_Selected and (4).inv())
                }

            3 ->
                if (selectedOptions == true) {
                    days_Selected = days_Selected or 8
                } else {
                    days_Selected = (days_Selected and (8).inv())
                }

            4 ->
                if (selectedOptions == true) {
                    days_Selected = days_Selected or 16
                } else {
                    days_Selected = (days_Selected and (16).inv())
                }

            5 ->
                if (selectedOptions == true) {
                    days_Selected = days_Selected or 32
                } else {
                    days_Selected = (days_Selected and (32).inv())
                }

            6 ->
                if (selectedOptions == true) {
                    days_Selected = days_Selected or 64
                } else {
                    days_Selected = (days_Selected and (64).inv())
                }

        }
    }
}