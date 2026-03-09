package com.example.dc_acconverterandcontrolremote

import android.content.Context
import android.content.res.Resources
import android.widget.Toast
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.res.stringResource
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import androidx.lifecycle.ViewModel
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.map
import java.util.Calendar
import kotlinx.coroutines.flow.toList
import kotlinx.coroutines.launch
import kotlin.text.forEach
import kotlin.text.toInt
import kotlin.math.pow

class DeviceSchedulerViewModel: ViewModel() {
    init {
        println("VieModel Initilizing...")
    }

    override fun onCleared() {
        super.onCleared()
        println("Viewmodel on Cleaning...")
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


    var devices: List<Devices>? = null


    private val Context.myDataStore by preferencesDataStore(name = "settings")

    // this Mac address is the address of the phone and not remote link address
    val MAC_ADDRESS_LOCAL = stringPreferencesKey("mac_address_local")

    val MAC_ADDRESS_REMOTE = stringPreferencesKey("mac_address_remote")

    val IP_ADDRESS_LOCAL =stringPreferencesKey("ip_address_local")

    // this IP address is from the renote link and not of the device
    val IP_ADDRESS_REMOTE = stringPreferencesKey("ip_address")

    val PEER_PORT = intPreferencesKey("ip_address")
    val NUMBER_DEVICES = intPreferencesKey("number_devices")

    val MATCH_FILTER = StringPreferencesKey("match_filter")

    val serviceName:String ="ControlRemote"

    val default_nbr_devices: Int = 8


    lateinit var devicesDao: DaoDevices

    suspend fun devicesSet(nbr_devices: Int, context: Context) {
        context.myDataStore.edit {
            it[NUMBER_DEVICES] = nbr_devices

        }
        Toast.makeText(context, R.string.toast_set_nbr_devices, Toast.LENGTH_SHORT).show()
    }
    fun numberSetLaunch(numberDevicesText: String, context: Context) {
        val temp: Int = numberDevicesText.toInt()
        viewModelScope.launch {
            devicesSet(temp, context)
        }
    }

    suspend fun setMatchFilter(byteArray: ByteArray){
        var temp: String=""
        for(i:Int in 0..byteArray.size-1){
            temp += byteArray[i].toString()
        }
        // temp a;ready is a String and have /n

        context.myDataStore.edit{

            it[MATCH_FILTER] = temp
        }
    }
    suspend fun setMatchFilter(filter: String){

        context.myDataStore.edit{

            it[MATCH_FILTER] = filter.toInt()
        }
    }

    fun setMatchFilterLaunch(filter: String){
        viewModelScope.launch {
            setMatchFilter(filter)
        }
    }

    fun setMatchFilterLaunch(byteArray: ByteArray){
        viewModelScope.launch {
            setMatchFilter(byteArray)
        }
    }
    suspend fun getMatchFilter(): ByteArray {
        var byteArray: ByteArray= ByteArray(7)
        \
        MATCH_FILTER.forEachIndexed { index:Int, char:Char ->

            byteArray[index]=char.toString().toByte()
        }
        return byteArray
    }
    fun getMatchFilterLaunch(): ByteArray {
        var byteArray: ByteArray= ByteArray(6)
        viewModelScope.launch {
            byteArray= getMatchFilter()
        }
    return byteArray
    }

    suspend fun macAddressSetLocal(macAddress: String, context: Context) {
        context.myDataStore.edit {
            it[MAC_ADDRESS_LOCAL] = macAddress
        }
        Toast.makeText(context, R.string.toast_set_MAC, Toast.LENGTH_SHORT).show()
    }
    fun MacSetLaunchLocal(macAddressText: String, context: Context) {
        viewModelScope.launch {
            macAddressSetLocal(macAddressText, context)
        }
    }

    suspend fun macAddressSetRemote(macAddress: String, context: Context) {
        context.myDataStore.edit {
            it[MAC_ADDRESS_REMOTE] = macAddress
        }
        Toast.makeText(context, R.string.toast_set_MAC, Toast.LENGTH_SHORT).show()
    }
    // this IP address is from the renote link and not of the device

    fun MacSetLaunchRemote(macAddressText: String, context: Context) {
        viewModelScope.launch {
            macAddressSetRemote(macAddressText, context)
        }
    }


    suspend fun IPAddressSetLocal(ipaddress: String, context: Context) {
        context.myDataStore.edit {
            it[IP_ADDRESS_LOCAL] = ipaddress
        }
        Toast.makeText(context, R.string.toast_set_IP, Toast.LENGTH_SHORT).show()

    }

    // this IP address is from the renote link and not of the device
    fun IpSetLaunchLocal(ipAddressText: String, context: Context) {
        viewModelScope.launch {
            IPAddressSetLocal(ipAddressText, context)
        }
    }

    suspend fun IPAddressSetRemote(ipaddress: String, context: Context) {
        context.myDataStore.edit {
            it[IP_ADDRESS_REMOTE] = ipaddress
        }
        Toast.makeText(context, R.string.toast_set_IP, Toast.LENGTH_SHORT).show()

    }

    // this IP address is from the renote link and not of the device
    fun IpSetLaunchRemote(ipAddressText: String, context: Context) {
        viewModelScope.launch {
            IPAddressSetRemote(ipAddressText, context)
        }
    }
    suspend fun peerPortSet(port: Int, context: Context) {
        context.myDataStore.edit {
            it[PEER_PORT] = port
        }
        Toast.makeText(context, R.string.toast_set_IP, Toast.LENGTH_SHORT).show()

    }

    // this IP address is from the renote link and not of the device
    fun peerPortSetLaunch(port: Int, context: Context) {
        viewModelScope.launch {
            peerPortSet(port, context)
        }
    }



    suspend fun devicesInit(context: Context) {

        devicesSet(default_nbr_devices, context)
    }

    suspend fun deviceListInit() {
        for (i in 0..default_nbr_devices) {
            devicesDao.insert(Devices(i, "Device $i"))

        }
    }

    fun deviceList(): List<Devices>? {

        var temp: List<Devices>?=null
        if (devicesDao != null) {
            viewModelScope.launch {
               temp = devicesDao.getAll()
            }
            return temp

        } else return null
    }


    suspend fun deviceListSizeUpdate(context: Context, qtydevices: Int) {
        val devices: Int = context.myDataStore.data.map {
            it[NUMBER_DEVICES] ?: 0
        }.toString().toInt()

        if (devices > qtydevices) {
            for (i in (devices - 1) downTo (qtydevices - 1)) {
                var device = devicesDao.getItem(i)
                //  viewModelScope.launch {
                devicesDao.delete(device.toList().get(0))
                // }

            }

            devicesSet(qtydevices, context)
            val deleted = devices - qtydevices
            val toast_message: String =
                "$deleted " + Resources.getSystem().getString((R.string.devices_added_toast))
            Toast.makeText(context, toast_message, Toast.LENGTH_SHORT).show()

        } else if (devices < qtydevices) {

            for (i in devices..(qtydevices - 1)) {
                devicesDao.insert(Devices(i, "Devices $i"))
            }
            val added = qtydevices - devices
            val toast_message: String =
                "$added " + Resources.getSystem().getString((R.string.devices_added_toast))
            Toast.makeText(context, toast_message, Toast.LENGTH_SHORT).show()


        }
    }

    fun sendActionToWiFI(device_number: Int, on_or_off: String) {
        // pending
    }

    fun deviceName(devicenbr: Int): String {

        var device = devicesDao.getItem(devicenbr)
        var name: String = ""

        viewModelScope.launch {
            name = device.toList().get(0).device_name!!
        }

        return name
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
                    devicesDao.update(device!!.get(0))
                }
            } else {
                device!!.get(0).hour_off = hourToSeT
                device!!.get(0).minutes_off = minuteToSet
                viewModelScope.launch {
                    devicesDao.update(device!!.get(0))
                }
            }
        }
        return "$hourSet : $minuteSet"
    }

    fun TimesToshow(device_number: Int, on_or_off: String, string_onoff: String): String? {

        var device: MutableList<Devices>? = null

        viewModelScope.launch {
            devicesDao.getItem(device_number).toList(device!!)

        }

        viewModelScope.launch {
            device!!.toList()
        }

        var timeToShow: String? = null

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


    fun selectedDays(device_number: Int, option: Int, selectedOptions: Boolean) {

        var device: MutableList<Devices>? = null

        viewModelScope.launch {
            devicesDao.getItem(device_number).toList(device!!)

        }
        var days_week: Int = device!!.toList().get(0).days_week!!

        when (option) {
            0 ->
                if (selectedOptions == true) {
                    days_week = days_week or 1
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                } else {
                    days_week = days_week and (1).inv()
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                }

            1 ->
                if (selectedOptions == true) {
                    days_week = days_week or 2
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                } else {
                    days_week = days_week and (2).inv()
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }

                }

            2 ->
                if (selectedOptions == true) {
                    days_week = days_week or 4
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                } else {
                    days_week = days_week and (4).inv()
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                }

            3 ->
                if (selectedOptions == true) {
                    days_week = days_week or 8
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                } else {
                    days_week = days_week and (8).inv()
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                }

            4 ->
                if (selectedOptions == true) {
                    days_week = days_week or 16
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                } else {
                    days_week = days_week and (16).inv()
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                }

            5 ->
                if (selectedOptions == true) {
                    days_week = days_week or 32
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                } else {
                    days_week = days_week and (32).inv()
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                }

            6 ->
                if (selectedOptions == true) {
                    days_week = days_week or 64
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                } else {
                    days_week = days_week and (64).inv()
                    device.get(0).days_week = days_week
                    viewModelScope.launch {
                        devicesDao.update(device!!.get(0))
                    }
                }

        }
    }

    fun Char.isHexDigit(): Boolean {
        return this in '0'..'9' || this in 'a'..'f' || this in 'A'..'F'
    }

// this IP address is from the renote link and not of the device
    fun setIpAddressToString(message: ByteArray):String{
        //big endian
        lateinit var temp: String

       for (i in  0..message.size ){
           temp += message[i].toString()
            }
        return temp
        }

    fun setIpStringToAddress(ipAddress:String): ByteArray{
        // big endian
        lateinit var temp: ByteArray

        ipAddress.forEach {char->
            var i: Int=0
            temp[i]=char.toString().toByte()
            i++
        }

        return temp
    }

    fun setMacAddressToString(message: ByteArray):String{
        //big endian
        lateinit var temp: String

        for (i in  0..message.size ){
            temp += message[i].toHexString()
        }
        return temp
    }

    fun setMacStringToAddress(macAddress:String): ByteArray{
        // little endian
        lateinit var temp: ByteArray
        lateinit var tempString: String
        var i: Int=0

        var  j: Int=5 // mac addres is 6 bytes
            macAddress.forEach { char ->
                tempString += char.toString()
                if (i == 1) {
                    tempString = "0x$tempString"
                    temp[j] = tempString.toByte()
                    tempString = ""
                    i = 0
                    j-- // should be from 5 to 0
                }
                i++
            }
            return temp
    }


    fun setPortToUse(message: ByteArray): Int{
        val portToUse : Int = ((message[1].toInt().and(0xFF)).shl(8)) or (message[0].toInt().and(0xFF))

        //PENDING

        return portToUse
    }
}
