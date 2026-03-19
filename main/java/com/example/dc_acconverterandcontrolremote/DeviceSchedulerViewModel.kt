package com.example.dc_acconverterandcontrolremote

import android.annotation.SuppressLint
import android.app.Application
import android.content.Context
import android.widget.Toast
import androidx.compose.ui.res.stringResource
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.flow.map
import kotlinx.coroutines.flow.stateIn
import kotlinx.coroutines.launch
import kotlin.text.forEach
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewmodel.initializer
import androidx.lifecycle.viewmodel.viewModelFactory
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.toList
import kotlinx.coroutines.runBlocking
import java.util.Calendar


data class DiscoverySettings(
    val matchFilter: String = "",
    val isReady: Boolean = false
)
// 1. Move this OUTSIDE and ABOVE the class
// It is private to this FILE, so only this ViewModel can see it.
private val Context.myDataStore by preferencesDataStore(name = "settings")

class DeviceSchedulerViewModel(private val devicesRepository: DevicesRepository,
                               private val application: DatabaseApplication
): AndroidViewModel(application) {


    init {
        println("VieModel Initilizing...")
    }

    override fun onCleared() {
        super.onCleared()
        println("Viewmodel on Cleaning...")
    }

    companion object {
        val Factory: ViewModelProvider.Factory = viewModelFactory {
            initializer {
                // 1. Grab the Application instance from the platform
                val app = (this[ViewModelProvider.AndroidViewModelFactory.APPLICATION_KEY] as DatabaseApplication)

                // 2. Grab the Repository from your AppContainer
                val repo = app.databaseContainer.devicesRepository

                // 3. Pass BOTH to your constructor exactly as you defined it
                DeviceSchedulerViewModel(
                    devicesRepository = repo,
                    application = app
                )
            }
        }
    }



    // Access the context safely using getApplication()
    @SuppressLint("StaticFieldLeak")
    private val context = getApplication<Application>().applicationContext


    private val _isInitialized = MutableStateFlow(false)
    // Public read-only state for the Composable

    val isInitialized: StateFlow<Boolean> = _isInitialized.asStateFlow()


    // this Mac address is the address of the phone and not remote link address
    val MAC_ADDRESS_LOCAL = stringPreferencesKey("mac_address_local")

    val macAddressLocal: StateFlow<String> = context.myDataStore.data
        .map { it[MAC_ADDRESS_LOCAL] ?: "" }
        .stateIn(viewModelScope, SharingStarted.WhileSubscribed(5000), "")

    val MAC_ADDRESS_REMOTE = stringPreferencesKey("mac_address_remote")

    val macAddressRemote: StateFlow<String> = context.myDataStore.data
        .map { it[MAC_ADDRESS_REMOTE] ?: "" }
        .stateIn(viewModelScope, SharingStarted.WhileSubscribed(5000), "")

    val IP_ADDRESS_LOCAL =stringPreferencesKey("ip_address_local")

    val IpAddressLocal: StateFlow<String> = context.myDataStore.data
        .map { it[IP_ADDRESS_LOCAL] ?: "" }
        .stateIn(viewModelScope, SharingStarted.WhileSubscribed(5000), "")

    // this IP address is from the renote link and not of the device
    val IP_ADDRESS_REMOTE = stringPreferencesKey("ip_address")

    val IpAddressRemote: StateFlow<String> = context.myDataStore.data
        .map { it[IP_ADDRESS_REMOTE] ?: "" }
        .stateIn(viewModelScope, SharingStarted.WhileSubscribed(5000), "")

    val PEER_PORT = intPreferencesKey("peer_port")
    val peerPort: StateFlow<Int> = context.myDataStore.data
        .map { preferences ->
            // Use an Int fallback (e.g., 8080 or 0), NOT a String ""
            preferences[PEER_PORT] ?: 8080
        }
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            // Use an Int initial value, NOT a String ""
            initialValue = 8080
        )

    val default_nbr_devices: Int = 8
    val NUMBER_DEVICES = intPreferencesKey("number_devices")

    val numberDevices: StateFlow<Int> = context.myDataStore.data
        .map { preferences ->
            preferences[NUMBER_DEVICES] ?:default_nbr_devices
        }
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            // Use an Int initial value, NOT a String ""
            initialValue = default_nbr_devices
        )

    val CHARGER_HOURON = intPreferencesKey("number_devices")

    val chargerHourOn: StateFlow<Int> = context.myDataStore.data
        .map { preferences ->
            preferences[NUMBER_DEVICES] ?:default_nbr_devices
        }
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            // Use an Int initial value, NOT a String ""
            initialValue = default_nbr_devices
        )

    val CHARGER_HOUROFF = intPreferencesKey("number_devices")

    val chargerHourOff: StateFlow<Int> = context.myDataStore.data
        .map { preferences ->
            preferences[NUMBER_DEVICES] ?:default_nbr_devices
        }
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            // Use an Int initial value, NOT a String ""
            initialValue = default_nbr_devices
        )

    val CHARGER_MINUTEON = intPreferencesKey("number_devices")

    val chargerMinuteOn: StateFlow<Int> = context.myDataStore.data
        .map { preferences ->
            preferences[NUMBER_DEVICES] ?:default_nbr_devices
        }
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            // Use an Int initial value, NOT a String ""
            initialValue = default_nbr_devices
        )

    val CHARGER_MINUTEOFF = intPreferencesKey("number_devices")

    val chargerMinuteOff: StateFlow<Int> = context.myDataStore.data
        .map { preferences ->
            preferences[NUMBER_DEVICES] ?:default_nbr_devices
        }
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            // Use an Int initial value, NOT a String ""
            initialValue = default_nbr_devices
        )

    val MATCH_FILTER = stringPreferencesKey("match_filter")
    val match_filter: StateFlow<String> = context.myDataStore.data
        .map { it[MATCH_FILTER] ?: "12456\n" }
        .stateIn(viewModelScope, SharingStarted.WhileSubscribed(5000), "12456\n")
    val serviceName:String ="ControlRemote"


    // This allows the UI to observe changes in the database
    val allDevices: StateFlow<List<Devices>> = devicesRepository.getAllDevicesStream()
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            initialValue = emptyList()
        )

    // 1. Combine DataStore filter and Database status
    val discoverySettings: StateFlow<DiscoverySettings> = kotlinx.coroutines.flow.combine(
        match_filter,
        allDevices
    ) { filter, devices ->
        DiscoverySettings(
            matchFilter = filter,
            // System is ready if filter is 7 digits AND database has loaded devices
            // (Adjust devices.isNotEmpty() based on your app's "ready" definition)
            isReady = filter.trim().length == 7 && devices.isNotEmpty()
        )
    }.stateIn(
        scope = viewModelScope,
        started = SharingStarted.WhileSubscribed(5000),
        initialValue = DiscoverySettings()
    )


    fun devicesList():List<Devices>{
         return runBlocking {allDevices.value}
    }

    // In your ViewModel
    suspend fun numberSetLaunch(numberDevicesText: String) {
        val temp: Int = numberDevicesText.toIntOrNull() ?: default_nbr_devices

        // edit() is a suspend function; it waits until the save is complete
        context.myDataStore.edit { settings ->
            settings[NUMBER_DEVICES] = temp
        }

        // This toast will only show after the disk write is finished
        Toast.makeText(context, R.string.toast_set_nbr_devices, Toast.LENGTH_SHORT).show()
    }

    fun setMatchFilterLaunch(matchFilter: String) {
        viewModelScope.launch {

            context.myDataStore.edit {

                it[MATCH_FILTER] = matchFilter
            }
        }
    }

    fun getMatchFilterLaunch(): ByteArray {

        val currentString = match_filter.value // INSTANT, no waiting

        return currentString.take(7).toByteArray(Charsets.UTF_8)
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

    fun getMacAddressLocalLaunch(): String? = macAddressLocal.value ?: null

            // this IP address is from the renote link and not of the device

    fun MacSetLaunchRemote(macAddress: String) {
        viewModelScope.launch {
            context.myDataStore.edit {
                it[MAC_ADDRESS_REMOTE] = macAddress
            }
            Toast.makeText(context, R.string.toast_set_MAC, Toast.LENGTH_SHORT).show()
        }
    }

    fun getMacAddressRemoteLaunch(): String? = macAddressRemote.value ?: null


                // this IP address is from the renote link and not of the device
    fun IpSetLaunchLocal(ipAddress: String, context: Context) {
        viewModelScope.launch {
            context.myDataStore.edit {
                it[IP_ADDRESS_LOCAL] = ipAddress
            }
        }
        Toast.makeText(context, R.string.toast_set_IP, Toast.LENGTH_SHORT).show()

    }

    fun getIpAddressLocalLaunch(): String? = IpAddressLocal.value ?: null

    // this IP address is from the renote link and not of the device
    fun IpSetLaunchRemote(ipAddress: String) {
        viewModelScope.launch {
            context.myDataStore.edit {
                it[IP_ADDRESS_REMOTE] = ipAddress
            }
        }
        Toast.makeText(context, R.string.toast_set_IP, Toast.LENGTH_SHORT).show()
    }

    fun getIpAddressRemoteLaunch(): String? = IpAddressRemote.value ?: null

    // this IP address is from the renote link and not of the device
    fun peerPortSetLaunch(port: Int) {
        viewModelScope.launch {
            context.myDataStore.edit {
                it[PEER_PORT] = port
            }
        }
        Toast.makeText(context,  R.string.PeerPortSet , Toast.LENGTH_SHORT).show()

                }


    fun getPeerPortLaunch(): Int? {

        val currentInt = peerPort.value // INSTANT, no waiting
        return currentInt ?: null
        }

    fun devicesInit() {
        viewModelScope.launch(Dispatchers.IO) {
            val currentDevices = devicesRepository.getAllDevicesStream().first()

            if (currentDevices.isEmpty()) {
                for (i in 1..default_nbr_devices) {
                    devicesRepository.insertDevice(
                        Devices(device_number = i, device_name = "Device $i")
                    )
                }
            }

            // Use .value for a fast, synchronous state update
            _isInitialized.value = true
        }
    }


    fun deviceListSizeUpdate(newQty: Int) {
        viewModelScope.launch(Dispatchers.IO) {
            // 1. Get current count from the StateFlow we already have
            val currentQty = numberDevices.value
            if (newQty < currentQty) {
                // Removing devices
                val currentList = devicesRepository.getAllDevicesStream().first()
                for (i in (currentQty - 1) downTo newQty) {
                    // Find device by ID and delete
                    currentList.find { it.device_number == i }?.let {
                        devicesRepository.deleteDevice(it)
                    }
                }
            } else if (newQty > currentQty) {
                // Adding devices
                for (i in currentQty until newQty) {
                    devicesRepository.insertDevice(Devices(i, "Device $i"))
                }
            }

            // 2. Update DataStore
            getApplication<Application>().applicationContext.myDataStore.edit { settings ->
                settings[NUMBER_DEVICES] = newQty
            }

            // 3. Show Toast on Main Thread
            viewModelScope.launch(Dispatchers.Main) {
                val diff =
                    if (newQty > currentQty) newQty - currentQty else currentQty - newQty
                val msg =
                    if (newQty > currentQty) "Added $diff devices" else "Removed $diff devices"
                Toast.makeText(context, msg, Toast.LENGTH_SHORT).show()
            }
        }
    }


    fun sendActionToWiFI(device_number: Int, on_or_off: Boolean, aware: WifiAware) {
        // data type same as IDF esp32
        //macro to identify data for run control remote
        // #define RECEIVED_C_REMOTE 2
        val control_remote: Int = 2
        // to make a definition (same as IDF ESP32 program):
        // the value for ON will be device_number x 2
        // the value for OFF will be devicd_number x 2 + 1
        // I set limit of devices in 100 and this will use 200 numbers

        val byteArray: ByteArray =
            if (on_or_off) byteArrayOf((device_number * 2).toByte()) else byteArrayOf((device_number * 2 + 1).toByte())

        viewModelScope.launch(Dispatchers.IO) {

            if(aware.sendData(control_remote,byteArray, )){
                Toast.makeText(context, R.string.remote, Toast.LENGTH_SHORT)
                    .show()
            }
        }
    }

    fun sendSchedulerToWiFI( aware: WifiAware) {
        val devices: List<Devices> = devicesList()
        val byteArray: ByteArray = ByteArray((numberDevices.value*5))

        for (i in  0 until numberDevices.value){

            byteArray[0+i*5]= devices[i].device_number!!.toByte()
            byteArray[1+i*5]= (devices[i].hour_on?:200).toByte()
            byteArray[2+i*5]= (devices[i].minutes_on?:200).toByte()
            byteArray[3+i*5]= (devices[i].hour_off?:200).toByte()
            byteArray[4+i*5]= (devices[i].minutes_off?:200).toByte()
        }


        viewModelScope.launch(Dispatchers.IO) {

            if(aware.sendData(RECEIVED_SCHEDULER,byteArray, )){
                Toast.makeText(context, R.string.schedulerSent, Toast.LENGTH_SHORT)
                    .show()
            }
        }
    }

    fun sendSchedulerChargerToWiFI( aware: WifiAware) {

        val byteArray: ByteArray = ByteArray(4)

            byteArray[0]= chargerHourOn.value.toByte()
            byteArray[1]= chargerMinuteOn.value.toByte()
            byteArray[2]= chargerHourOff.value.toByte()
            byteArray[3]= chargerMinuteOff.value.toByte()

        viewModelScope.launch(Dispatchers.IO) {
            if(aware.sendData(RECEIVED_CHARGER_TIME,byteArray, )) {
                Toast.makeText(context, R.string.chargerSent, Toast.LENGTH_SHORT)
                    .show()
            }
        }
    }

    fun sendTimeToWiFI( aware: WifiAware) {

        val byteArray: ByteArray = ByteArray(3)
        val calendar = Calendar.getInstance()

        byteArray[0]= calendar.get(Calendar.SECOND).toByte()
        byteArray[1]= calendar.get(Calendar.MINUTE).toByte()
        byteArray[2]= calendar.get(Calendar.HOUR_OF_DAY).toByte()

        viewModelScope.launch(Dispatchers.IO) {
            if(aware.sendData(RECEIVED_TIME,byteArray, )) {
                Toast.makeText(context, R.string.clockSent, Toast.LENGTH_SHORT)
                    .show()
            }
        }
    }

    fun deviceName(devicenbr: Int): String {

        // Search the list already in memory (StateFlow)
        val device = allDevices.value.find { it.device_number == devicenbr }
        return device?.device_name ?: "Device $devicenbr"
    }

    fun setselectedTime(
        hourToSet: Int,
        minuteToSet: Int,
        device_number: Int,
        on_or_off: Boolean
        ): String {

        // 1. Get the actual device object from your StateFlow (RAM)
        val deviceToUpdate = allDevices.value.find { it.device_number == device_number }

        // 2. Perform the update if the inputs are valid
        if (deviceToUpdate != null && hourToSet >= 0 && minuteToSet >= 0) {

            if (on_or_off) {
                deviceToUpdate.hour_on = hourToSet
                deviceToUpdate.minutes_on = minuteToSet
            } else {
                deviceToUpdate.hour_off = hourToSet
                deviceToUpdate.minutes_off = minuteToSet
            }

            // 3. Launch the update via Repository (Async)
            viewModelScope.launch(Dispatchers.IO) {
                devicesRepository.updateDevice(deviceToUpdate)
            }
        }

            // 4. Return the string for the UI immediately
        return "$hourToSet : $minuteToSet"
    }

    fun setSelectedTimeCharger(
        hourToSet: Int,
        minuteToSet: Int,
        on_or_off: Boolean
    ): String {


        // 2. Perform the update if the inputs are valid

            if (on_or_off) {
                viewModelScope.launch {
                    context.myDataStore.edit {
                        it[CHARGER_HOURON] = hourToSet
                        it[CHARGER_MINUTEON]=minuteToSet
                    }
                }

            }
            else {
                viewModelScope.launch {
                    context.myDataStore.edit {
                        it[CHARGER_HOUROFF] = hourToSet
                        it[CHARGER_MINUTEOFF]=minuteToSet
                    }
                }
            }
        return "$hourToSet : $minuteToSet"
    }

    fun TimesToshow(device_number: Int, on_or_off: Boolean): String {
        // 1. Get the actual device object from your StateFlow (RAM)
        val device = allDevices.value.find { it.device_number == device_number }
        // 2. Default value if device is not found
        if (device == null) return "00 : 00"
        // 3. Extract the correct hours and minutes
        val hour = if (on_or_off) device.hour_on else device.hour_off
        val minute = if (on_or_off) device.minutes_on else device.minutes_off
        // 4. Format to ensure 2 digits (e.g., 09:05)
        val h = hour.toString().padStart(2, '0')
        val m = minute.toString().padStart(2, '0')
            return "$h : $m"
    }

    fun TimesToshowCharger( on_or_off: Boolean): String {
        // 1. Get the actual device object from your StateFlow (RAM)
        val hour = if (on_or_off) chargerHourOn.value else chargerHourOff.value
        val minute = if (on_or_off) chargerMinuteOn.value else chargerMinuteOff.value
        // 4. Format to ensure 2 digits (e.g., 09:05)
        val h = hour.toString().padStart(2, '0')
        val m = minute.toString().padStart(2, '0')
        return "$h : $m"
    }


    fun selectedDays(device_number: Int, option: Int, isSelected: Boolean) {
        // 1. Find the device safely
        val device = allDevices.value.find { it.device_number == device_number } ?: return
        // 2. Calculate the bit mask (1, 2, 4, 8, 16, 32, 64)
        val bitMask = 1 shl option
        val currentDays = device.days_week ?: 0
        // 3. Update the value using Bitwise OR (to set) or AND NOT (to clear)
        val newDays = if (isSelected) {
            currentDays or bitMask
        } else {
            currentDays and bitMask.inv()
        }

        // 4. Update the object and database
        device.days_week = newDays
        viewModelScope.launch(Dispatchers.IO) {
                devicesRepository.updateDevice(device)
            }
        }


    fun Char.isHexDigit(): Boolean {
        return this in '0'..'9' || this in 'a'..'f' || this in 'A'..'F'
    }

    // this IP address is from the renote link and not of the device
    fun setIpAddressToString(message: ByteArray): String {
        // Standard IPv4: [192, 168, 1, 1] -> "192.168.1.1"
        return message.joinToString(".") { byte ->
            (byte.toInt() and 0xFF).toString()
        }
    }

    fun setIpStringToAddress(ipAddress: String): ByteArray {
        val parts = ipAddress.split(".")
        val temp = ByteArray(4)

        for (i in 0 until 4) {
            // Handle potential empty or malformed strings safely
            temp[i] = parts.getOrElse(i) { "0" }.toInt().toByte()
        }
        return temp
    }

    fun setMacAddressToString(message: ByteArray): String {
        // Standard Big Endian: [AA, BB, CC, DD, EE, FF] -> "AABBCCDDEEFF"
        return message.joinToString("") { byte ->
            "%02X".format(byte)
        }
    }

    fun setMacStringToAddress(macAddress: String): ByteArray {
        // Ensures we have exactly 6 bytes (12 hex characters)
        val cleanMac = macAddress.replace(":", "").uppercase()
        val temp = ByteArray(6)

        for (i in 0 until 6) {
            val hexPair = cleanMac.substring(i * 2, i * 2 + 2)
            // Big Endian: index 0 of string goes to index 0 of array
            temp[i] = hexPair.toInt(16).toByte()
        }
        return temp
    }

    fun setPortToUse(message: ByteArray): Int {

        val portToUse: Int = ((message[1].toInt().and(0xFF)).shl(8)) or (message[0].toInt().and(0xFF))
            //PENDING

            return portToUse
        }
    }
