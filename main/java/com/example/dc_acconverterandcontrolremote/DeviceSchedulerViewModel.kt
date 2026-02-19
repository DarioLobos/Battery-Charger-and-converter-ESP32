package com.example.dc_acconverterandcontrolremote

import android.content.Context
import android.content.res.Resources
import android.widget.Toast
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import androidx.lifecycle.ViewModel
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.map
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
    var devices: List <Devices>?= null



    private val Context.myDataStore by preferencesDataStore(name = "settings")

    val MAC_ADDRESS = intPreferencesKey("mac_address")
    val IP_ADDRESS = intPreferencesKey("ip_address")
    val NUMBER_DEVICES = intPreferencesKey("number_deices")

    val default_nbr_devices:Int = 8



    lateinit var devicesDao: DaoDevices



    suspend fun devicesSet(nbr_devices:Int, context: Context) {
        context.myDataStore.edit {
            it[NUMBER_DEVICES] = nbr_devices
        }
        Toast.makeText(context ,R.string.toast_set_nbr_devices, Toast.LENGTH_SHORT).show()
    }


    suspend fun macAddressSet(macaddress:Int, context: Context) {
        context.myDataStore.edit {
            it[MAC_ADDRESS]=macaddress
        }
        Toast.makeText(context,R.string.toast_set_MAC, Toast.LENGTH_SHORT).show()
    }


    suspend fun IPAddressSet(ipaddress:Int, context: Context) {
        context.myDataStore.edit {
            it[IP_ADDRESS]=ipaddress
        }
        Toast.makeText(context,R.string.toast_set_IP, Toast.LENGTH_SHORT).show()
    }

    suspend fun devicesInit(context: Context) {

        devicesSet(default_nbr_devices, context)
    }

    suspend fun deviceListInit() {
        for (i in 0..default_nbr_devices) {
            devicesDao.insert(Devices(i, "Devices $i"))
        }
    }

    fun deviceList():List<Devices>? {
        if (devicesDao != null) {
            return devicesDao.getAll()
        }
        else return null
    }



    suspend fun deviceListSizeUpdate(context: Context, qtydevices: Int) {
        val devices: Int = context.myDataStore.data.map {
            it[NUMBER_DEVICES] ?: 0
        }.toString().toInt()

        if (devices > qtydevices) {

            for (i in (devices - 1) downTo (qtydevices - 1)) {

                var device= devicesDao.getItem(i)

                viewModelScope.launch {
                    devicesDao.delete(device.toList().get(0))
                }

            }

            devicesSet(qtydevices, context)
            val deleted = devices - qtydevices
            val toast_message: String = "$deleted " + Resources.getSystem().getString((R.string.devices_added_toast))
            Toast.makeText(context, toast_message, Toast.LENGTH_SHORT).show()

        } else if (devices < qtydevices) {

            for (i in devices..(qtydevices - 1)) {
                devicesDao.insert(Devices(i, "Devices $i"))
            }
            val added = qtydevices - devices
            val toast_message: String = "$added " + Resources.getSystem().getString((R.string.devices_added_toast))
            Toast.makeText(context, toast_message, Toast.LENGTH_SHORT).show()


        }
    }

    fun sendActionToWiFI(device_number: Int, on_or_off: String){
        // pending
    }

    fun deviceName(devicenbr:Int ):String? {
        var device = devicesDao.getItem(devicenbr)
        var name: String=""

        viewModelScope.launch {
            name= device.toList().get(0).device_name
        }

        return name
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
        var device: MutableList<Devices>? = null
        devicesDao.getItem(device_number)

        viewModelScope.launch {
            devicesDao.getItem(device_number).toList(device!!)

        }

        viewModelScope.launch {
            device!!.toList()
        }

        if ((hourToSeT >= 0) and ((minuteToSet >= 0))) {

            if (on_or_off == string_onoff) {
                device!!.get(0).hour_on = hourToSeT
                device!!.get(0).minutes_on = minuteToSet
                viewModelScope.launch {
                    devicesDao.update(device!!.get(0))}
            } else {
                device!!.get(0).hour_off = hourToSeT
                device!!.get(0).minutes_off = minuteToSet
            viewModelScope.launch {
                devicesDao.update(device!!.get(0))}
            }
        }
        return "$hourSet : $minuteSet"
    }

    fun TimesToshow(device_number: Int, on_or_off: String, string_onoff: String): String {

        var device: MutableList<Devices>? = null

        viewModelScope.launch {
            devicesDao.getItem(device_number).toList(device!!)

        }

        viewModelScope.launch {
            device!!.toList()
        }
        var timeToShow: String = ""

        if (on_or_off == string_onoff) {
            hourSet.setValue(device!!.get(0).hour_on!!)
            minuteSet.setValue(device!!.get(0).minutes_on!!)
            timeToShow = "$hourSet : $minuteSet"

    } else {
            hourSet.setValue(device!!.get(0).hour_off!!)
            minuteSet.setValue(device!!.get(0).minutes_off!!)
            timeToShow = "$hourSet : $minuteSet"

    }
        return timeToShow
    }


    fun selectedDays(device_number: Int, option: Int, selectedOptions:Boolean) {

        var device: MutableList<Devices>? = null

        viewModelScope.launch {
            devicesDao.getItem(device_number).toList(device!!)

        }
        var days_week: Int = device!!.toList().get(0).days_week!!

        when (option) {
            0 ->
                if (selectedOptions == true) {
                    days_week = days_week or 1
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                    devicesDao.update(device!!.get(0))}
                } else {
                    days_week = days_week and (1).inv()
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                }

            1 ->
                if (selectedOptions == true) {
                    days_week = days_week or 2
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                } else {
                    days_week = days_week and (2).inv()
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}

                }

            2 ->
                if (selectedOptions == true) {
                    days_week = days_week or 4
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                } else {
                    days_week = days_week and (4).inv()
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                }

            3 ->
                if (selectedOptions == true) {
                    days_week = days_week or 8
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                } else {
                    days_week = days_week and (8).inv()
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                }

            4 ->
                if (selectedOptions == true) {
                    days_week = days_week or 16
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                } else {
                    days_week = days_week and (16).inv()
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                }

            5 ->
                if (selectedOptions == true) {
                    days_week = days_week or 32
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                } else {
                    days_week = days_week and (32).inv()
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                }

            6 ->
                if (selectedOptions == true) {
                    days_week = days_week or 64
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                } else {
                    days_week = days_week and (64).inv()
                    device.get(0).days_week=days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))}
                }

        }
    }
}