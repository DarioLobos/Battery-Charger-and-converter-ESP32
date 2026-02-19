package com.example.dc_acconverterandcontrolremote

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.viewModelScope
import java.util.Calendar
import kotlinx.coroutines.flow.toList
import kotlinx.coroutines.launch

class DeviceSchedulerViewModel: ViewModel() {

    val device_number: MutableLiveData<Int> by lazy {
        MutableLiveData<Int>()
    }


    val on_or_off: MutableLiveData<String> by lazy {
        MutableLiveData<String>()
    }


    val device: MutableLiveData<String> by lazy {
        MutableLiveData<String>()
    }

    val hourSet: MutableLiveData<Int> by lazy {
        MutableLiveData<Int>()
    }

    val minuteSet: MutableLiveData<Int> by lazy {
        MutableLiveData<Int>()
    }
    var calendar by mutableStateOf(Calendar.getInstance())

    var hourForPicker by mutableStateOf(calendar[Calendar.HOUR_OF_DAY])

    var minuteForPicker by mutableStateOf(calendar[Calendar.MINUTE])

    val days_Selected: MutableLiveData<Int> by lazy {
        MutableLiveData<Int>()
    }

    val selectedTime: MutableLiveData<Int> by lazy {
        MutableLiveData<Int>()
    }


    fun deviceName(device_number: Int): String {
        var temp: String=""
        viewModelScope.launch {
            val device: List<Devices> = devicesDao.getItem(device_number).toList()

            temp = device.get(0).device_name

        }
        return temp
    }

    fun seton_or_off(on_or_off: String){
        this.on_or_off.setValue(on_or_off)
    }
    fun setdevice_number(device_number: Int){
        this.device_number.setValue(device_number)
    }

    fun setselectedTime(
        hourToSeT: Int,
        minuteToSet: Int,
        device_number: Int,
        on_or_off: String,
        string_onoff: String
    ): String {
        var device: List<Devices> = devicesDao.getItem(device_number) as List<Devices>

        viewModelScope.launch {
            device.toList()
        }

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

    fun TimesToshow(device_number: Int, on_or_off: String, string_onoff: String): String {

        var device: List<Devices> = devicesDao.getItem(device_number) as List<Devices>

        viewModelScope.launch {
            device.toList()
        }
        var timeToShow: String = ""

        if (on_or_off == string_onoff) {
            hourSet.setValue(device.get(0).hour_on!!)
            minuteSet.setValue(device.get(0).minutes_on!!)
            timeToShow = "$hourSet : $minuteSet"
        } else {
            hourSet.setValue(device.get(0).hour_off!!)
            minuteSet.setValue(device.get(0).minutes_off!!)
            timeToShow = "$hourSet : $minuteSet"
        }
        return timeToShow
    }


    fun selectedDays(device_number: Int, option: Int, selectedOptions:Boolean) {

        var device: List<Devices> = devicesDao.getItem(device_number) as List<Devices>

        viewModelScope.launch {
            device.toList()
        }

        var days_week: Int = device.get(0).days_week!!

        when (option) {
            0 ->
                if (selectedOptions == true) {
                    days_week = days_week or 1
                    device.get(0).days_week=days_week
                } else {
                    days_week = days_week and (1).inv()
                    device.get(0).days_week=days_week
                }

            1 ->
                if (selectedOptions == true) {
                    days_week = days_week or 2
                    device.get(0).days_week=days_week
                } else {
                    days_week = days_week and (2).inv()
                    device.get(0).days_week=days_week
                }

            2 ->
                if (selectedOptions == true) {
                    days_week = days_week or 4
                    device.get(0).days_week=days_week
                } else {
                    days_week = days_week and (4).inv()
                    device.get(0).days_week=days_week
                }

            3 ->
                if (selectedOptions == true) {
                    days_week = days_week or 8
                    device.get(0).days_week=days_week
                } else {
                    days_week = days_week and (8).inv()
                    device.get(0).days_week=days_week
                }

            4 ->
                if (selectedOptions == true) {
                    days_week = days_week or 16
                    device.get(0).days_week=days_week
                } else {
                    days_week = days_week and (16).inv()
                    device.get(0).days_week=days_week
                }

            5 ->
                if (selectedOptions == true) {
                    days_week = days_week or 32
                    device.get(0).days_week=days_week
                } else {
                    days_week = days_week and (32).inv()
                    device.get(0).days_week=days_week
                }

            6 ->
                if (selectedOptions == true) {
                    days_week = days_week or 64
                    device.get(0).days_week=days_week
                } else {
                    days_week = days_week and (64).inv()
                    device.get(0).days_week=days_week
                }

        }
    }
}